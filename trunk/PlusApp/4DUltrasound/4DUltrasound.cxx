/*=========================================================================
Module:  $RCSfile: SynchroGrab.cxx,v $
Author:  Jonathan Boisvert, Queens School Of Computing
Author:  Jan Gumprecht, Nobuhiko Hata, Harvard Medical School
Authors: Danielle Pace,
         Robarts Research Institute and The University of Western Ontario

Copyright (c) 2008, Queen's University, Kingston, Ontario, Canada
All rights reserved.

Copyright (c) 2008, Brigham and Women's Hospital, Boston, MA

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

 * Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.

 * Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in
   the documentation and/or other materials provided with the
   distribution.

 * Neither the name of Queen's University nor the names of any
   contributors may be used to endorse or promote products derived
   from this software without specific prior written permission.

 * Neither the name of Harvard Medical School nor the names of any
   contributors may be used to endorse or promote products derived
   from this software without specific prior written permission

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.

=========================================================================*/

//
// This command line application lets you reconstruct 3D and 4D ultrasound
// volumes in real-time based on the images collected with a tracked 2D
// probe.  The application first connects to an ultrasound machine using
// the frame grabber of the user's choice.  Then it also connects to a
// tracking system of the user's choice, as well as to an ECG-gating
// system if 4D ultrasound data is to be acquired.  A calibration file
// specifies the parameters of the reconstruction.  The reconstruction
// occurs in real-time while image messages containing the incremental
// results are sent using the OpenIGTLink protocol to an external
// visualization system, such as 3D Slicer.

#include "vtkFileOutputWindow.h"
#include "vtkImageData.h"
#include "vtkTimerLog.h"
#include "vtkDataProcessor.h"
#include "vtkDataSender.h"
#include "vtkInstrumentTracker.h"
#include "vtkFreehandUltrasound2Dynamic.h"
#include "vtkDataSetWriter.h"
#include "vtkImageExtractComponents.h"

#include "vtkTracker.h"
#include "vtkTrackerTool.h"
#include "vtkTrackerBuffer.h"
#include "vtkFakeTracker.h"
#ifdef PLUS_USE_POLARIS
#include "vtkPOLARISTracker.h"
#include "vtkNDITracker.h"
#endif
#ifdef PLUS_USE_CERTUS
#include "vtkNDICertusTracker.h"
#endif
#ifdef PLUS_USE_FLOCK
#include "vtkFlockTracker.h"
#endif
#ifdef PLUS_USE_MICRONTRACKER
#include "vtkMicronTracker.h"
#endif
#ifdef PLUS_USE_AMS_TRACKER
#include "vtkAMSTracker.h"
#endif

#include "vtkVideoSource2.h"
#ifdef PLUS_USE_MATROX_IMAGING
#include "vtkMILVideoSource2.h"
#endif
#ifdef WIN32
#ifdef VTK_VFW_SUPPORTS_CAPTURE
#include "vtkWin32VideoSource2.h"
#endif
/*#else
#ifdef USE_LINUX_VIDEO
#include "vtkV4L2VideoSource2.h"
#endif*/
#endif
#ifdef PLUS_USE_SONIX_VIDEO
#include "vtkSonixVideoSource2.h"
#endif

#include "vtkSignalBox.h"
#ifdef PLUS_USE_HEARTSIGNALBOX
#include "vtkHeartSignalBox.h"
#endif
#ifdef PLUS_USE_USBECGBOX
#include "vtkUSBECGBox.h"
#endif

#define SYNCHRO_VIDEO_NOISE 1
#define SYNCHRO_VIDEO_MIL 2
#define SYNCHRO_VIDEO_WIN32 3
#define SYNCHRO_VIDEO_LINUX 4
#define SYNCHRO_VIDEO_SONIX 5

#define SYNCHRO_TRACKER_FAKE 1
#define SYNCHRO_TRACKER_FLOCK 2
#define SYNCHRO_TRACKER_MICRON 3
#define SYNCHRO_TRACKER_CERTUS 4
#define SYNCHRO_TRACKER_POLARIS 5
#define SYNCHRO_TRACKER_AURORA 6
#define SYNCHRO_TRACKER_AMS 7

#define SYNCHRO_SIGNAL_NOGATING 1
#define SYNCHRO_SIGNAL_FAKE 2
#define SYNCHRO_SIGNAL_HEART 3
#define SYNCHRO_SIGNAL_USB 4

#define SYNCHRO_VIDEO_NTSC 1
#define SYNCHRO_VIDEO_PAL 2

struct ReconstructionParameters {
  char* CalibrationFileName;
  char* OIGTLServer;
  int OIGTLPortForImages;
  int OIGTLPortForTransforms;
  int VideoSourceType;
  char* VideoDeviceName; // for Linux
  int VideoChannel; // for Linux
  int VideoMode; // for Linux
  int TrackerType;
  int TrackerPort;
  int SignalBoxType;
  bool SendImagesLater;
  double ImageSendPeriod;
  bool SendTransforms;
  double TransformSendRate;
  char *SaveDirectory;
  bool SaveFrames;
  bool SaveTimestamps;
  bool SaveBuffer;
  bool Verbose;
  };

using namespace std;

void printSplashScreen();
void printUsage(struct ReconstructionParameters *defaultParams);
bool parseCommandLineArguments(int argc, char** argv,
                               struct ReconstructionParameters *reconParams);
void goodByeScreen();
void goodByeInput();
static inline void vtkSleep(double duration);
vtkVideoSource2* MakeVideoSource(int type, char* videoDeviceName, int videoChannel, int VideoMode);
vtkTracker* MakeTracker(int type);
vtkSignalBox* MakeSignalBox(int type);

int GetVideoSourceType(string videoSourceString);
int GetTrackerType(string trackerString);
int GetSignalBoxType(string signalBoxString);
int GetVideoMode(string videoModeString);

void printDefaultVideo(struct ReconstructionParameters *defaultParams);
void printDefaultVideoMode (struct ReconstructionParameters *defaultParams);
void printDefaultTracker (struct ReconstructionParameters *defaultParams);
void printDefaultSignalBox(struct ReconstructionParameters *defaultParams);

/******************************************************************************
 *
 * MAIN
 *
 ******************************************************************************/
int main(int argc, char **argv)
{
  int terminate = 0;
  ofstream logStream;
  char* logFile = "./logFile.txt";

  // setup default parameters
  struct ReconstructionParameters defaultParams;
  defaultParams.CalibrationFileName = "";
  defaultParams.OIGTLServer = "localhost";
  defaultParams.OIGTLPortForImages = 18944;
  defaultParams.OIGTLPortForTransforms = 18945;
  defaultParams.VideoSourceType = SYNCHRO_VIDEO_SONIX;
  defaultParams.VideoDeviceName = "/dev/video"; // default for Linux
  defaultParams.VideoChannel = 3; // default for Linux - S-Video at Hauppauge Impact VCB Modell 558
  defaultParams.VideoMode = SYNCHRO_VIDEO_NTSC; // default for Linux - NTSC
  defaultParams.TrackerType = SYNCHRO_TRACKER_AMS;
  defaultParams.TrackerPort = 0;
  defaultParams.SignalBoxType = SYNCHRO_SIGNAL_NOGATING;
  defaultParams.SendImagesLater = true;
  defaultParams.ImageSendPeriod = 5.0;
  defaultParams.SendTransforms = false;
  defaultParams.TransformSendRate = 5.0;
#ifdef _WIN32
  defaultParams.SaveDirectory = "..\\..\\..\\SynchroGrab4DOutput";
#else
  defaultParams.SaveDirectory = "../../../SynchroGrab4DOutput";
#endif
  defaultParams.SaveFrames = true;
  defaultParams.SaveTimestamps = true;
  defaultParams.SaveBuffer = true;
  defaultParams.Verbose = true;

  // setup actual parameters
  struct ReconstructionParameters reconParams;
  reconParams.CalibrationFileName = defaultParams.CalibrationFileName;
  reconParams.OIGTLServer = defaultParams.OIGTLServer;
  reconParams.OIGTLPortForImages = defaultParams.OIGTLPortForImages;
  reconParams.OIGTLPortForTransforms = defaultParams.OIGTLPortForTransforms;
  reconParams.VideoSourceType = defaultParams.VideoSourceType;
  reconParams.VideoDeviceName = defaultParams.VideoDeviceName;
  reconParams.VideoChannel = defaultParams.VideoChannel;
  reconParams.VideoMode = defaultParams.VideoMode;
  reconParams.TrackerType = defaultParams.TrackerType;
  reconParams.TrackerPort = defaultParams.TrackerPort;
  reconParams.SignalBoxType = defaultParams.SignalBoxType;
  reconParams.SendImagesLater = defaultParams.SendImagesLater;
  reconParams.ImageSendPeriod = defaultParams.ImageSendPeriod;
  reconParams.SendTransforms = defaultParams.SendTransforms;
  reconParams.TransformSendRate = defaultParams.TransformSendRate;
  reconParams.SaveDirectory = defaultParams.SaveDirectory;
  reconParams.SaveFrames = defaultParams.SaveFrames;
  reconParams.SaveTimestamps = defaultParams.SaveTimestamps;
  reconParams.SaveBuffer = defaultParams.SaveBuffer;
  reconParams.Verbose = defaultParams.Verbose;

  vtkDataProcessor *processor = vtkDataProcessor::New();
  vtkDataSender *sender = vtkDataSender::New();
  vtkInstrumentTracker *instrumentTracker = vtkInstrumentTracker::New();

  //Fix StartUp time
  double startTime = vtkTimerLog::GetUniversalTime();
  processor->SetStartUpTime(startTime);
  sender->SetStartUpTime(startTime);
  instrumentTracker->SetStartUpTime(startTime);

  printSplashScreen();

  //Read command line arguments
  bool successParsingCommandLine = parseCommandLineArguments(argc, argv, &reconParams);
  // now the reconParams struct has all of the information we need

  if(!successParsingCommandLine)
    {
    cerr << "ERROR: Could not parse commandline arguments" << endl << endl;
    printUsage(&defaultParams);
    //goodByeScreen();
    //goodByeInput();
    }
  else
    {
    cout << "using the following calibration file:" << endl;
    cout << reconParams.CalibrationFileName << endl << endl;

    vtkFreehandUltrasound2Dynamic *reconstructor = vtkFreehandUltrasound2Dynamic::New();
    char* calibrationFileName = NULL;

    //Log Stream Preparation
    logStream.precision(0);
    logStream.setf(ios::fixed,ios::floatfield);
    logStream.open(logFile, ios::out);
    processor->SetLogStream(logStream);
    sender->SetLogStream(logStream);
    instrumentTracker->SetLogStream(logStream);

    cout << "--- Started ---" << endl << endl;

    //redirect vtk errors to a file
    vtkFileOutputWindow *errOut = vtkFileOutputWindow::New();
    errOut->SetFileName("vtkError.txt");
    vtkOutputWindow::SetInstance(errOut);

    // fix up the video source, tracker and signal box according to the command line arguments
    vtkVideoSource2* videoSource = MakeVideoSource(reconParams.VideoSourceType, reconParams.VideoDeviceName, reconParams.VideoChannel, reconParams.VideoMode);
    vtkTracker* tracker = MakeTracker(reconParams.TrackerType);
    // TODO buggy when changing tracker buffer size, so we'll leave it to what the user has for now
    //tracker->GetTool(reconParams.TrackerPort)->GetBuffer()->SetBufferSize(1000);
    // TODO potential conflict between calibration file and what the user says
    vtkSignalBox* signalBox = MakeSignalBox(reconParams.SignalBoxType);

    // other user options
    if (reconParams.SaveFrames)
      {
      reconstructor->SetSaveInsertedDirectory(reconParams.SaveDirectory);
      reconstructor->SaveInsertedSlicesOn();
      reconstructor->SaveInsertedTimestampsOn();
      }
    else if (reconParams.SaveTimestamps)
      {
      reconstructor->SetSaveInsertedDirectory(reconParams.SaveDirectory);
      reconstructor->SaveInsertedTimestampsOn();
      reconstructor->SaveInsertedSlicesOff();
      }
    else
      {
      reconstructor->SaveInsertedSlicesOff();
      reconstructor->SaveInsertedSlicesOff();
      }
    sender->SetOIGTLServer(reconParams.OIGTLServer);
    instrumentTracker->SetOIGTLServer(reconParams.OIGTLServer);
    sender->SetServerPort(reconParams.OIGTLPortForImages);
    instrumentTracker->SetServerPort(reconParams.OIGTLPortForTransforms);
    sender->SetVerbose(reconParams.Verbose);
    processor->SetVerbose(reconParams.Verbose);
    instrumentTracker->SetVerbose(reconParams.Verbose);

    // setup the object that will perform the reconstruction
    reconstructor->SetVideoSource(videoSource);
    reconstructor->SetSlice(videoSource->GetOutput());
    reconstructor->SetTrackerTool(tracker->GetTool(reconParams.TrackerPort));
    if (reconParams.SignalBoxType != SYNCHRO_SIGNAL_NOGATING)
      {
      reconstructor->SetSignalBox(signalBox);
      }

    if (!reconstructor->ReadSummaryFile(reconParams.CalibrationFileName))
      {
       cout << "ERROR could not read calibration file " << reconParams.CalibrationFileName << endl;
       #ifdef ERRORPROCESSOR
       processor->GetLogStream() << processor->GetUpTime() << "ERROR: Can not read calibration file => Volume reconstruction not possible" << endl;
       #endif
       return -1;
      }

    // If the user specification for the signal box conflicts with the calibration file
    // calibration file says to trigger, but user says not to
    if (reconstructor->GetTriggering() && reconParams.SignalBoxType == SYNCHRO_SIGNAL_NOGATING)
      {
      cout << "You specified NOGATING, but the calibration file contains triggering" << endl << "information" << endl;
      cout << "Turning off gating..." << endl << endl;
      reconstructor->TriggeringOff();
      reconstructor->SetSignalBox(NULL);
      }
    // user says to trigger, but the calibration file has no triggering informatoin
    else if (!reconstructor->GetTriggering() && reconParams.SignalBoxType != SYNCHRO_SIGNAL_NOGATING)
      {
      cout << "ERROR You specified triggering but the calibration file specifies not to" << endl;
      cout << "trigger.  Please change the calibration file or choose NOGATING" << endl;
      terminate = 1;
      }
    
    // Setup the processor
    processor->SetReconstructor(reconstructor);

    // Start tracking
    if (terminate == 0)
      {
      cout << "Starting tracker - using port " << reconParams.TrackerPort << endl;
      if(tracker->Probe() != 1)
        {
        cerr << "ERROR Tracking system not found" << endl;
        terminate = 2;
        }
      else 
        {
        tracker->StartTracking();
        }
      }

    // Start video source
    if (terminate == 0)
      {
      cout << "Starting video" << endl;
      videoSource->StartRecording();

      if (!videoSource->GetRecording())
        {
        cerr << "ERROR Could not start video source" << endl;
        terminate = 3;
        }
      }

    // Start signal box
    if (terminate == 0 && reconParams.SignalBoxType != SYNCHRO_SIGNAL_NOGATING)
      {
      cout << "Starting signal box" << endl;
      signalBox->Initialize();
      signalBox->Start();
      
      if (!signalBox->GetIsStarted())
        {
        cerr << "ERROR Could not start signal box" << endl;
        terminate = 4;
        }
      }

    sender->SetSendPeriod(reconParams.ImageSendPeriod);
    processor->SetProcessPeriod(reconParams.ImageSendPeriod);

    // connect sender to OpenIGTLink
    if (terminate == 0)
      {
      cout << "Connecting to server" << endl;
      if(sender->ConnectToServer() != 0)
        {
        terminate = 5;
        }
      }

    // start send thread
    if(terminate == 0)
      {
      cout << "Starting sending" << endl;
      if(sender->StartSending() != 0)
        {
        terminate = 6;
        }
      }

    // start reconstruction
    if (terminate == 0)
      {
      cout << "Starting real-time reconstruction" << endl;
      reconstructor->StartRealTimeReconstruction();
      }

    if (!reconParams.SendImagesLater)
      {

      // start processing thread
      if(terminate == 0)
        {
        cout << "Starting processing" << endl;
        if(processor->StartProcessing(sender) != 0)
          {
          terminate = 7;
          }
        }
      }

    if (terminate == 0)
      {
      goodByeScreen();
      }

    // start sending transforms if the user wants to
    if (terminate == 0 && reconParams.SendTransforms)
      {
      if(-1 != instrumentTracker->ConnectToServer())
        {
        instrumentTracker->SetTrackingRate(reconParams.TransformSendRate);
        if(-1 == instrumentTracker->StartTracking(tracker, reconParams.TrackerPort))
          {
          terminate = 8;
          }
        }
      else
        {
        terminate = 9;
        }
      }

    // wait until the user says to stop
    if (terminate == 0)
      {
      goodByeInput();
      }

//TODO: remove DEBUG info
//videoSource->WriteFramesAsPNG("I_TRUS_Images.txt", "IMG_"); 

    // stop reconstruction
    if (terminate == 0)
      {
      cout << "Stopping real-time reconstruction" << endl;
      reconstructor->StopRealTimeReconstruction();
      }

    //Stop processing, sending, server connection and instrument tracker
    if (processor->GetProcessing())
      {
      cout << "Stopping real-time processing" << endl;
      processor->StopProcessing();
      }
      
    if (sender->GetSending())
      {
      cout << "Stopping real-time sending" << endl << endl;
      sender->StopSending();
      }

    // remove everything from the sender
    sender->DeleteAllData();

    if (instrumentTracker->GetTracking())
      {
      cout << "Stopping instrument tracker" << endl;
      instrumentTracker->StopTracking();
      cout << "Closing instrument tracker connection" << endl;
      instrumentTracker->CloseServerConnection();
      }

    // send final version of reconstruction
    // this is where the images get sent over if the user elects to send images
    // over only at the end
    // plus it gives the final version to Slicer for when visualizing
    // the whole time
    if (terminate == 0)
      {
      // fill holes
      cout << "Filling holes in output volume(s)..." << endl;
      reconstructor->FillHolesInOutput();
      cout << "Finished filling holes" << endl << endl;
      }

    // start send thread
    if(terminate == 0)
      {
      cout << "Starting sending the final volumes" << endl;
      if(sender->StartSending() != 0)
        {
        terminate = 10;
        }
      }

    // start processing thread
    if(terminate == 0)
      {
      cout << "Starting processing the final volumes" << endl;
      if(processor->StartProcessing(sender) != 0)
        {
        terminate = 11;
        }
      }

    // wait for all of the final volumes to be sent
    if (terminate == 0)
      {
      int numSent = 0;
      int prevNumSent = 0;
      int numNeeded = reconstructor->GetNumberOfOutputVolumes();
      cout << "Waiting until all " << numNeeded << " volumes are sent" << endl;
      do
        {
        numSent = sender->GetNumberSent();
        if (numSent != prevNumSent)
          {
          cout << "\tSent volume " << (numSent-1) << endl;
          prevNumSent = numSent;
          }
        vtkSleep(1.0); // sleep 1 second
        }
        while (numSent < numNeeded);
      cout << "Successfully sent all final volumes" << endl;
      }

    if (processor->GetProcessing())
      {
      cout << "Stopping final processing" << endl;
      processor->StopProcessing();
      }
    if (sender->GetSending())
      {
      cout << "Stopping final sending" << endl;
      sender->StopSending();
      }
    if (sender->GetConnected())
      {
      cout << "Closing image server connection " << endl << endl;
      sender->CloseServerConnection();
      }

    // save output
    if (terminate == 0)
      {
      // save calibration file
      // Side effect of creating the output directory for the output volumes, if needed
      cout << "Saving calibration file" << endl;
      reconstructor->SaveSummaryFile(reconParams.SaveDirectory);

      // save output volumes
      cout << "Saving " << reconstructor->GetNumberOfOutputVolumes() << " output volumes():" << endl;
      vtkImageExtractComponents *extract = vtkImageExtractComponents::New();
      vtkDataSetWriter *writer3D = vtkDataSetWriter::New();
      char filename[512];
      int phase;
      for (int port = 0; port < reconstructor->GetNumberOfOutputVolumes(); port++)
        {
        // set filename
        phase = reconstructor->CalculatePhaseFromPort(port);
  #ifdef _WIN32
        sprintf(filename, "%s\\ultra_phase%d.vtk", reconParams.SaveDirectory, phase);
  #else
        sprintf(filename, "%s/ultra_phase%d.vtk", reconParams.SaveDirectory, phase);
  #endif
        cout << "Saving output volume to " << filename << endl;

        // keep only 0th component
        extract->SetComponents(0);
        extract->SetInput(reconstructor->GetOutputFromPort(port));

        // write out to file
        writer3D->SetFileTypeToBinary();
        writer3D->SetInput(extract->GetOutput());
        writer3D->SetFileName(filename);
        writer3D->Write();
        }
      extract->Delete();
      writer3D->Delete();

      // save tracker buffer
      if (reconParams.SaveBuffer)
        {
        // set filename
  #ifdef _WIN32
        sprintf(filename, "%s\\trackedInstrument.txt", reconParams.SaveDirectory);
  #else
        sprintf(filename, "%s/trackedInstrument.txt", reconParams.SaveDirectory);
  #endif
        cout << "Saving tracker buffer to " << filename << endl;
        tracker->GetTool(reconParams.TrackerPort)->GetBuffer()->WriteToFile(filename);
        }
      }

    // stop tracker, video source and signal box
    if (tracker->IsTracking())
      {
      tracker->StopTracking();
      }
    if (videoSource->GetRecording())
      {
      videoSource->ReleaseSystemResources();
      videoSource->StopRecording();
      }
    if (reconParams.SignalBoxType != SYNCHRO_SIGNAL_NOGATING)
      {
      if (signalBox->GetIsStarted())
        {
        signalBox->Stop();
        }
      }

    if(terminate == 0)
      {
      cout << endl << "--- SynchroGrab4D finished ---" << endl << endl;
      }
    else
      {
      cout << endl << "--- SynchroGrab4D finished with ERRORS ---" << endl
           << "---           ERRORCODE: "<< terminate <<"             ---" << endl << endl;
      }
    logStream.close();
    errOut->Delete();

    if (tracker)
      {
      tracker->Delete();
      }
    if (videoSource)
      {
      videoSource->Delete();
      }
    if (reconParams.SignalBoxType != SYNCHRO_SIGNAL_NOGATING)
      {
      if (signalBox)
        {
        signalBox->Delete();
        }
      }
    if (reconstructor)
      {
      reconstructor->Delete();
      }
    }

  if (processor)
    {
    processor->Delete();
    }
  if (sender)
    {
    sender->Delete();
    }
  if (instrumentTracker)
    {
    instrumentTracker->Delete();
    }
}

//------------------------------------------------------------------------------
void printSplashScreen()
{

  cout << endl
       <<"*******************************************************************" << endl
       <<"*                                                                 *" << endl
       <<"* SynchroGrab4D - Real-Time Tracked 4D Ultrasound Imaging         *" << endl
       <<"*                                                                 *" << endl
       <<"* Author:     Danielle Pace                                       *" << endl
       <<"* (Supervisor: Dr. Terry Peters)                                  *" << endl
       <<"*             Robarts Research Institute and                      *" << endl
       <<"*             The University of Western Ontario                   *" << endl
       <<"* Date:       July 2009                                           *" << endl
       <<"*                                                                 *" << endl
       <<"* Thanks to:  Dr. David Gobbi,                                    *" << endl
       <<"*             Atamai Inc.                                         *" << endl
       <<"*                                                                 *" << endl
       <<"*             Chris Wedlake,                                      *" << endl
       <<"*             Robarts Research Institute                          *" << endl
       <<"*                                                                 *" << endl
       <<"*             Jan Gumprecht,                                      *" << endl
       <<"*             Dr. Junichi Tokuda and                              *" << endl
       <<"*             Dr. Nobuhiko Hata                                   *" << endl
       <<"*             Harvard Medical School                              *" << endl
       <<"*                                                                 *" << endl
       <<"*             Dr. Jonathan Boisvert,                              *" << endl
       <<"*             National Research Council Canada                    *" << endl
       <<"*                                                                 *" << endl
       <<"*******************************************************************" << endl << endl;

}

//------------------------------------------------------------------------------
void printDefaultVideo(struct ReconstructionParameters *defaultParams)
  {
  if (defaultParams->VideoSourceType == SYNCHRO_VIDEO_NOISE)
    {
    cout << "                                        (default: NOISE)" << endl;
    }
  else if (defaultParams->VideoSourceType == SYNCHRO_VIDEO_MIL)
    {
    cout << "                                        (default: MIL)" << endl;
    }
  else if (defaultParams->VideoSourceType == SYNCHRO_VIDEO_WIN32)
    {
    cout << "                                        (default: WIN32)" << endl;
    }
  else if (defaultParams->VideoSourceType == SYNCHRO_VIDEO_LINUX)
    {
    cout << "                                        (default: LINUX)" << endl;
    }
  else if (defaultParams->VideoSourceType == SYNCHRO_VIDEO_SONIX)
    {
    cout << "                                        (default: SONIX)" << endl;
    }
  }

//------------------------------------------------------------------------------
void printDefaultVideoMode(struct ReconstructionParameters *defaultParams)
  {
  if (defaultParams->VideoMode == SYNCHRO_VIDEO_NTSC)
    {
    cout << "                                        (default: NTSC)"<<endl;
    }
  else if (defaultParams->VideoMode == SYNCHRO_VIDEO_PAL)
    {
    cout << "                                        (default: PAL)"<<endl;
    }
  }

//------------------------------------------------------------------------------
void printDefaultTracker(struct ReconstructionParameters *defaultParams)
  {
  if (defaultParams->TrackerType == SYNCHRO_TRACKER_FAKE)
    {
    cout << "                                        (default: FAKE)" << endl;
    }
  else if (defaultParams->TrackerType == SYNCHRO_TRACKER_FLOCK)
    {
    cout << "                                        (default: FLOCK)" << endl;
    }
  else if (defaultParams->TrackerType == SYNCHRO_TRACKER_MICRON)
    {
    cout << "                                        (default: MICRON)" << endl;
    }
  else if (defaultParams->TrackerType == SYNCHRO_TRACKER_CERTUS)
    {
    cout << "                                        (default: CERTUS)" << endl;
    }
  else if (defaultParams->TrackerType == SYNCHRO_TRACKER_POLARIS)
    {
    cout << "                                        (default: POLARIS)" << endl;
    }
  else if (defaultParams->TrackerType == SYNCHRO_TRACKER_AURORA)
    {
    cout << "                                        (default: AURORA)" << endl;
    }
    else if (defaultParams->TrackerType == SYNCHRO_TRACKER_AMS)
    {
    cout << "                                        (default: AMS)" << endl;
    }
  }

//------------------------------------------------------------------------------
void printDefaultSignalBox(struct ReconstructionParameters *defaultParams)
  {
  if (defaultParams->SignalBoxType == SYNCHRO_SIGNAL_NOGATING)
    {
    cout << "                                        (default: NOGATING)" << endl;
    }
  else if (defaultParams->SignalBoxType == SYNCHRO_SIGNAL_FAKE)
    {
    cout << "                                        (default: FAKE)" << endl;
    }
  else if (defaultParams->SignalBoxType == SYNCHRO_SIGNAL_HEART)
    {
    cout << "                                        (default: HEART)" << endl;
    }
  else if (defaultParams->SignalBoxType == SYNCHRO_SIGNAL_USB)
    {
    cout << "                                        (default: USB)" << endl;
    }
  }

//------------------------------------------------------------------------------
void printUsage(struct ReconstructionParameters *defaultParams)
{
    cout << "--------------------------------------------------------------------------------"
         << endl;

    // print instructions on how to use this program.
    cout << "DESCRIPTION " << endl
         << "-----------"<<endl;
    cout << "This software does reconstructed 3D and 4D ultrasound imaging from a tracked" << endl
         << "2D probe.  It captures frames from an ultrasound device, transformation" << endl
         << "matricies from a tracking system, and gating information from an ECG source"<<endl
         << "The images are combined into a 3D dataset or a 4D dataset (i.e. a time series" <<endl
         << "of 3D volumes).  The reconstructed volume(s) and/or the transformation matrices" <<endl
         << "are sent to an OpenIGTLink server of choice, ex. 3D Slicer." << endl
         << "--------------------------------------------------------------------------------"
         << endl;
    cout << "OPTIONS " << endl
         << "-------"<<endl
         << "--calibration-file xxx or -c xxx:       Specify the calibration file" << endl
         << "                                        (MANDATORY)"<< endl
         << "--oigtl-server xxx or -os xxx:          Specify OpenIGTLink server"<< endl
         << "                                        (default: " << defaultParams->OIGTLServer << ")"<<endl
         << "--oigtl-port-images xxx or -opi xxx:    Specify OpenIGTLink port for images"<< endl
         << "                                        default: " << defaultParams->OIGTLPortForImages << ")"<<endl
         << "--oigtl-port-transforms xxx or -opt xxx:Specify OpenIGTLink port for transforms"<< endl
         << "                                        default: " << defaultParams->OIGTLPortForTransforms << ")"<<endl
         << "--video-source-type xxx or -vst xxx:    Video source type" << endl
         << "                                        (options: NOISE, MIL, WIN32)" << endl; // TODO LINUX, SONIX
    printDefaultVideo(defaultParams);
    cout << "--video-device-name xxx or -vdn xxx:    Video device name, for Linux systems" <<endl
         << "                                        (Linux not currently supported)"<<endl
         << "                                        (default: " << defaultParams->VideoDeviceName << ")"<<endl
         << "--video-channel xxx or -vc xxx:         Video channel, for Linux systems" <<endl
         << "                                        (Linux not currently supported)"<<endl
         << "                                        (default: " << defaultParams->VideoChannel << ")"<<endl
         << "--video-mode xxx or -vm -xxx:           Video mode, for Linux systems" <<endl
         << "                                        (Linux not currently supported)"<<endl
         << "                                        (options: NTSC, PAL" << endl;
    printDefaultVideoMode(defaultParams);
    cout << "--tracker-type xxx or -tt xxx:          Tracking system type" << endl
         << "                                        Options: FAKE, FLOCK, MICRON, CERTUS," << endl
         << "                                        POLARIS, AURORA"<<endl;
    printDefaultTracker(defaultParams);
    cout << "--tracker-port xxx or -tp xxx:          Port of tracked US tool" << endl
         << "                                        (default: "<<defaultParams->TrackerPort <<")"<<endl
         << "--signal-box-type xxx or -sbt xxx:      Signal box type" << endl
         << "                                        (options: NOGATING, FAKE, HEART, USB)" << endl;
    printDefaultSignalBox(defaultParams);
    cout << "--send-images-later or -sil:            Wait to send images until after" << endl
         << "                                        reconstruction finishes" << endl
         << "                                        (default: " << defaultParams->SendImagesLater <<")" << endl
         << "--image-send-period xxx or -isp xxx:    Time period (s) between image transfer" <<endl
         << "                                        (default: " << defaultParams->ImageSendPeriod << ")"<<endl
         << "--send-transform-matricies or -stm      Send transformation matrices" << endl
         << "                                        (default: " << defaultParams->SendTransforms << ")"<<endl
         << "--transform-send-rate xxx or -tsr xxx:  Rate at which tranforms are transfered" << endl
         << "                                        (default: " << defaultParams->TransformSendRate << ")"<<endl
         << "--save-directory xxx or -sd xxx:        Directory for output data" << endl
         << "                                        (default: " << defaultParams->SaveDirectory << endl
         << "--save-frames or -sf                    Save inserted 2D frames as .bmp" << endl
         << "                                        (default: " << defaultParams->SaveFrames << ")"<<endl
         << "--save-timestamps or -st                Save timestamps of inserted 2D frames" << endl
         << "                                        (default: " << defaultParams->SaveTimestamps << ")"<<endl
         << "--save-buffer or -sb                    Save transform buffer at end" << endl
         << "                                        (default: " << defaultParams->SaveBuffer << ")" << endl
         << "--verbose or -v                         Print more information" << endl
         << "                                        (default: " << defaultParams->Verbose << ")" << endl
         << endl
         << "--------------------------------------------------------------------------------"
         << endl << endl;
}

//------------------------------------------------------------------------------
//
// Parse the command line options.
//
bool parseCommandLineArguments(int argc, char **argv,
                               struct ReconstructionParameters *reconParams)
  {
  bool calibrationFileSpecified = false;
  bool successful = true;
  bool goodArg = false;
  int temp;
  double temp_double;
  string next;

  int i = 1;

  while(i < argc)
    {

    string currArg(argv[i]);
    goodArg = false;

    // args with parameters
    if (i < argc - 1)
      {
      // straight up char parameters
      if (currArg == "-c" || currArg == "--calibration-file")
        {
        reconParams->CalibrationFileName = argv[++i];
        calibrationFileSpecified = true;
        goodArg = true;
        }
      else if (currArg == "-os" || currArg == "--oigtl-server")
        {
        reconParams->OIGTLServer = argv[++i];
        goodArg = true;
        }
      else if (currArg == "-vdn" || currArg == "--video-device-name")
        {
        reconParams->VideoDeviceName = argv[++i];
        goodArg = true;
        }
      else if (currArg == "-sd" || currArg == "--save-directory")
        {
        reconParams->SaveDirectory = argv[++i];
        goodArg = true;
        }

      // char args that need to get converted to ints
      else if (currArg == "-vst" || currArg == "--video-source-type")
        {
        temp = GetVideoSourceType(argv[++i]);
        goodArg = true;
        if (temp != -1)
          {
          reconParams->VideoSourceType = temp;
          }
        else
          {
          cerr << "ERROR could not parse command line - unknown video source type " << argv[i] << endl;
          cerr << "\tPerhaps you didn't build this option?" << endl;
          successful = false;
          }
        }
      else if (currArg == "-vm" || currArg == "--video-mode")
        {
        temp = GetVideoMode(argv[++i]);
        goodArg = true;
        if (temp != -1)
          {
          reconParams->VideoMode = temp;
          }
        else
          {
          cerr << "ERROR could not parse command line - unknown video mode " << argv[i] << endl;
          successful = false;
          }
        }
      else if (currArg == "-tt" || currArg == "--tracker-type")
        {
        temp = GetTrackerType(argv[++i]);
        goodArg = true;
        if (temp != -1)
          {
          reconParams->TrackerType = temp;
          }
        else
          {
          cerr << "ERROR could not parse command line - unknown tracker type " << argv[i] << endl;
          cerr << "\tPerhaps you didn't build this option?" << endl;
          successful = false;
          }
        }
      else if (currArg == "-sbt" || currArg == "--signal-box-type")
        {
        temp = GetSignalBoxType(argv[++i]);
        goodArg = true;
        if (temp != -1)
          {
          reconParams->SignalBoxType = temp;
          }
        else
          {
          cerr << "ERROR could not parse command line - unknown signal box type " << argv[i] << endl;
          cerr << "\tPerhaps you didn't build this option?" << endl;
          successful = false;
          }
        }

      // int args
      else if (currArg == "-opi" || currArg == "--oigtl-port-images")
        {
        temp = atoi(argv[++i]);
        goodArg = true;
        next = argv[i];
        if (temp == 0 && next != "0")
          {
          cout << "ERROR the value you specified for " << currArg << " (" << argv[i] << ") doesn't convert to an int" << endl;
          successful = false;
          }
        else
          {
          reconParams->OIGTLPortForImages = temp;
          }
        }
      else if (currArg == "-opt" || currArg == "--oigtl-port-transforms")
        {
        temp = atoi(argv[++i]);
        goodArg = true;
        next = argv[i];
        if (temp == 0 && next != "0")
          {
          cout << "ERROR the value you specified for " << currArg << " (" << argv[i] << ") doesn't convert to an int" << endl;
          successful = false;
          }
        else
          {
          reconParams->OIGTLPortForTransforms = temp;
          }
        }
      else if (currArg == "-vc" || currArg == "--video-channel")
        {
        temp = atoi(argv[++i]);
        goodArg = true;
        next = argv[i];
        if (temp == 0 && next != "0")
          {
          cout << "ERROR the value you specified for " << currArg << " (" << argv[i] << ") doesn't convert to an int" << endl;
          successful = false;
          }
        else
          {
          reconParams->VideoChannel = temp;
          }
        }
      else if (currArg == "-tp" || currArg == "--tracker-port")
        {
        temp = atoi(argv[++i]);
        goodArg = true;
        next = argv[i];
        if (temp == 0 && next != "0")
          {
          cout << "ERROR the value you specified for " << currArg << " (" << argv[i] << ") doesn't convert to an int" << endl;
          successful = false;
          }
        else
          {
          reconParams->TrackerPort = temp;
          }
        }

      // double args
      else if (currArg == "-isp" || currArg == "--image-send-period")
        {
        temp_double = atof(argv[++i]);
        goodArg = true;
        next = argv[i];
        if (temp_double == 0)
          {
          cout << "ERROR the value you specified for " << currArg << " (" << argv[i] << ") doesn't convert to a double" << endl;
          successful = false;
          }
        else
          {
          reconParams->ImageSendPeriod = temp_double;
          }
        }
      else if (currArg == "-tsr" || currArg == "--transform-send-rate")
        {
        temp_double = atof(argv[++i]);
        goodArg = true;
        next = argv[i];
        if (temp_double == 0)
          {
          cout << "ERROR the value you specified for " << currArg << " (" << argv[i] << ") doesn't convert to a double" << endl;
          successful = false;
          }
        else
          {
          reconParams->TransformSendRate = temp_double;
          }
        }

      }
    else if (currArg == "-c" || currArg == "--calibration-file"
      || currArg == "-os" || currArg == "--oigtl-server"
      || currArg == "-op" || currArg == "--oigtl-port"
      || currArg == "-vst" || currArg == "--video-source-type"
      || currArg == "-vdn" || currArg == "--video-device-name"
      || currArg == "-vc" || currArg == "--video-channel"
      || currArg == "-vm" || currArg == "--video-mode"
      || currArg == "-tt" || currArg == "--tracker-type"
      || currArg == "-tp" || currArg == "--tracker-port"
      || currArg == "-sbt" || currArg == "--signal-box-type"
      || currArg == "-isp" || currArg == "--image-send-period"
      || currArg == "-tsr" || currArg == "--transform-send-rate"
      || currArg == "-sd" || currArg == "--save-directory")
      {
      cerr << "ERROR could not parse command line for option " << currArg << " - ran out of arguments" << endl;
      goodArg = true;
      successful = false;
      }

    // boolean args
    if (currArg == "-sil" || currArg == "--send-images-later")
      {
      reconParams->SendImagesLater = true;
      goodArg = true;
      }
    else if (currArg == "-stm" || currArg == "--send-transform-matrices")
      {
      reconParams->SendTransforms = true;
      goodArg = true;
      }
    else if (currArg == "-sf" || currArg == "--save-frames")
      {
      reconParams->SaveFrames = true;
      goodArg = true;
      }
    else if (currArg == "-st" || currArg == "--save-timestamps")
      {
      reconParams->SaveTimestamps = true;
      goodArg = true;
      }
    else if (currArg == "-sb" || currArg == "--save-buffer")
      {
      reconParams->SaveBuffer = true;
      goodArg = true;
      }
    else if (currArg == "-v" || currArg == "--verbose")
      {
      reconParams->Verbose = true;
      goodArg = true;
      }

    if (!goodArg)
      {
      cerr << "ERROR unable to parse command line - unknown argument: " << currArg << endl;
      successful = false;
      }

    i++;
    }

  if (!calibrationFileSpecified)
    {
    cerr << "ERROR could not parse command line - calibration file not found" << endl;
    successful = false;
    }

  return successful;

  }

//------------------------------------------------------------------------------
void goodByeScreen()
{

 cout << endl;
 cout << "Press 't' and hit 'ENTER' to terminate SynchroGrab4D"<<endl;
}

//------------------------------------------------------------------------------
void goodByeInput()
{
 string input;
 while(cin >> input)
   {
   if(input == "t")
     {
       break;
     }
   }

  /*cout << endl;
  cout << "Terminate SynchroGrab4D" << endl
       << endl
       << "Bye Bye..." << endl <<endl << endl;*/

  }

//------------------------------------------------------------------------------
int GetVideoSourceType(string videoSourceString)
  {

  if (videoSourceString == "NOISE")
    {
    return SYNCHRO_VIDEO_NOISE;
    }
#ifdef PLUS_USE_MATROX_IMAGING
  else if (videoSourceString == "MIL")
    {
    return SYNCHRO_VIDEO_MIL;
    }
#endif
#ifdef WIN32
#ifdef VTK_VFW_SUPPORTS_CAPTURE
  else if (videoSourceString == "WIN32")
    {
    return SYNCHRO_VIDEO_WIN32;
    }
#endif
/*#else
#ifdef USE_VIDEO_LINUX
  else if (videoSourceString == "LINUX")
    {
    return SYNCHRO_VIDEO_LINUX;
    }
#ifdef USE_VIDEO_LINUX*/
#endif
#ifdef PLUS_USE_SONIX_VIDEO
  else if (videoSourceString == "SONIX")
    {
    return SYNCHRO_VIDEO_SONIX;
    }
#endif

  return -1;
  }


//------------------------------------------------------------------------------
vtkVideoSource2* MakeVideoSource(int type, char *videoDeviceName, int videoChannel, int VideoMode)
{

  vtkVideoSource2* videoSource;

  if (type == SYNCHRO_VIDEO_NOISE)
  {
    videoSource = vtkVideoSource2::New();
  }
#ifdef PLUS_USE_MATROX_IMAGING
  else if (type == SYNCHRO_VIDEO_MIL)
  {
    videoSource = vtkMILVideoSource2::New();
  }
#endif
#ifdef WIN32
#ifdef VTK_VFW_SUPPORTS_CAPTURE
  else if (type == SYNCHRO_VIDEO_WIN32)
  {
    videoSource = vtkWin32VideoSource2::New();
  }
#endif
/*#else
#ifdef USE_VIDEO_LINUX
  else if (type == SYNCHRO_VIDEO_LINUX)
  {
  vtkV4L2LinuxSource2* videoSourceLinux = vtkV4L2LinuxSource2::New(); // TODO
  videoSourceLinux->SetVideoDevice(videoDeviceName);
  videoSourceLinux->SetVideoChannel(videoChannel);
  videoSourceLinux->SetVideoMode(videoMode);
  videoSource = videoSourceLinux;
  }
#endif*/
#endif
#ifdef PLUS_USE_SONIX_VIDEO
  else if (type == SYNCHRO_VIDEO_SONIX)
  {
    videoSource = vtkSonixVideoSource2::New();
  }
#endif

  return videoSource;
}

//------------------------------------------------------------------------------
int GetTrackerType(string trackerString)
  {

  if (trackerString == "FAKE")
    {
    return SYNCHRO_TRACKER_FAKE;
    }
#ifdef PLUS_USE_FLOCK
  else if (trackerString == "FLOCK")
    {
    return SYNCHRO_TRACKER_FLOCK;
    }
#endif
#ifdef PLUS_USE_MICRONTRACKER
  else if (trackerString == "MICRON")
    {
    return SYNCHRO_TRACKER_MICRON;
    }
#endif
#ifdef PLUS_USE_CERTUS
  else if (trackerString == "CERTUS")
    {
    return SYNCHRO_TRACKER_CERTUS;
    }
#endif
#ifdef PLUS_USE_POLARIS
  else if (trackerString == "POLARIS")
    {
    return SYNCHRO_TRACKER_POLARIS;
    }
  else if (trackerString == "AURORA")
    {
    return SYNCHRO_TRACKER_AURORA;
    }
#endif
#ifdef PLUS_USE_AMS_TRACKER
  else if (trackerString == "AMS")
    {
    return SYNCHRO_TRACKER_AMS;
    }
#endif

  return -1;


  }

//------------------------------------------------------------------------------
vtkTracker* MakeTracker(int type)
  {

  vtkTracker* tracker;

  if (type == SYNCHRO_TRACKER_FAKE)
    {
    tracker = vtkFakeTracker::New();
    }
#ifdef PLUS_USE_FLOCK
  else if (type == SYNCHRO_TRACKER_FLOCK)
    {
    tracker = vtkFlockTracker::New();
    }
#endif
#ifdef PLUS_USE_MICRONTRACKER
  else if (type == SYNCHRO_TRACKER_MICRON)
    {
    tracker = vtkMicronTracker::New();
    }
#endif
#ifdef PLUS_USE_CERTUS
  else if (type == SYNCHRO_TRACKER_CERTUS)
    {
    tracker = vtkNDICertusTracker::New();
    }
#endif
#ifdef PLUS_USE_POLARIS
  else if (type == SYNCHRO_TRACKER_POLARIS)
    {
    tracker = vtkPOLARISTracker::New();
    }
  else if (type == SYNCHRO_TRACKER_AURORA)
    {
    tracker = vtkNDITracker::New();
    }
#endif
#ifdef PLUS_USE_AMS_TRACKER
	else if (type == SYNCHRO_TRACKER_AMS)
    {
    tracker = vtkAMSTracker::New();
    }
#endif
  return tracker;

}

//------------------------------------------------------------------------------
int GetSignalBoxType(string signalBoxString)
  {
  if (signalBoxString == "NOGATING")
    {
    return SYNCHRO_SIGNAL_NOGATING;
    }
  else if (signalBoxString == "FAKE")
    {
    return SYNCHRO_SIGNAL_FAKE;
    }
#ifdef PLUS_USE_HEARTSIGNALBOX
  else if (signalBoxString == "HEART")
    {
    return SYNCHRO_SIGNAL_HEART;
    }
#endif
#ifdef PLUS_USE_USBECGBOX
  else if (signalBoxString == "USB")
    {
    return SYNCHRO_SIGNAL_USB;
    }
#endif

  return -1;
  }

//------------------------------------------------------------------------------
vtkSignalBox* MakeSignalBox(int type)
  {
  
  vtkSignalBox* signalBox;

  if (type == SYNCHRO_SIGNAL_NOGATING)
    {
    signalBox = NULL;
    }
  else if (type == SYNCHRO_SIGNAL_FAKE)
    {
    signalBox = vtkSignalBox::New();
    }
#ifdef PLUS_USE_HEARTSIGNALBOX
  else if (type == SYNCHRO_SIGNAL_HEART)
    {
    signalBox = vtkHeartSignalBox::New();
    }
#endif
#ifdef PLUS_USE_USBECGBOX
  else if (type == SYNCHRO_SIGNAL_USB)
    {
    signalBox = vtkUSBECGBox::New();
    }
#endif

  return signalBox;
  }

//------------------------------------------------------------------------------
int GetVideoMode(string videoModeString)
  {
  if (videoModeString == "NTSC")
    {
    return SYNCHRO_VIDEO_NTSC;
    }
  else if (videoModeString == "PAL")
    {
    return SYNCHRO_VIDEO_PAL;
    }

  return -1;
  }


/******************************************************************************
 *  static inline void vtkSleep(double duration)
 *
 *  Platform-independent sleep function
 *  Set the current thread to sleep for a certain amount of time
 *
 *  @Param: double duration - Time to sleep in ms
 *
 * ****************************************************************************/
static inline void vtkSleep(double duration)
{
  duration = duration; // avoid warnings
  // sleep according to OS preference
#ifdef _WIN32
  Sleep((int)(1000*duration));
#elif defined(__FreeBSD__) || defined(__linux__) || defined(sgi) || defined(__APPLE__)
  struct timespec sleep_time, dummy;
  sleep_time.tv_sec = (int)duration;
  sleep_time.tv_nsec = (int)(1000000000*(duration-sleep_time.tv_sec));
  nanosleep(&sleep_time,&dummy);
#endif
}
