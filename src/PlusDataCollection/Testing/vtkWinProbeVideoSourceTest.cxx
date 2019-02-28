/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

/*!
  \file vtkPlusWinProbeVideoSourceTest.cxx
  \brief Test basic connection to the WinProbe ultrasound system
  and write some frames to output file(s)
  \ingroup PlusLibDataCollection
*/

#include "PlusConfigure.h"
#include "vtkImageData.h"
#include "vtkSmartPointer.h"
#include "vtkPlusWinProbeVideoSource.h"
#include "vtkXMLUtilities.h"
#include "vtksys/CommandLineArguments.hxx"
#include "vtkImageViewer.h"
#include "vtkActor2D.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkPlusDataSource.h"

class vtkMyCallback : public vtkCommand
{
public:
  static vtkMyCallback* New() { return new vtkMyCallback; }

  virtual void Execute(vtkObject* caller, unsigned long, void*)
  {
    m_Viewer->Render();

    //update the timer so it will trigger again
    m_Interactor->CreateTimer(VTKI_TIMER_UPDATE);
  }

  vtkRenderWindowInteractor* m_Interactor;
  vtkImageViewer* m_Viewer;

private:

  vtkMyCallback()
  {
    m_Interactor = nullptr;
    m_Viewer = nullptr;
  }
};

int main(int argc, char* argv[])
{
  bool printHelp(false);
  bool renderingOff(false);
  std::string inputConfigFileName;
  std::string outputFileName("WinProbeOutputSeq.nrrd");
  int verboseLevel = vtkPlusLogger::LOG_LEVEL_DEBUG;

  vtksys::CommandLineArguments args;
  args.Initialize(argc, argv);
  args.AddArgument("--help", vtksys::CommandLineArguments::NO_ARGUMENT, &printHelp, "Print this help.");
  args.AddArgument("--config-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputConfigFileName, "Config file containing the device configuration.");
  args.AddArgument("--rendering-off", vtksys::CommandLineArguments::NO_ARGUMENT, &renderingOff, "Run test without rendering.");
  args.AddArgument("--output-seq-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputFileName, "Filename of the output video buffer sequence metafile (Default: VideoBufferMetafile)");
  args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level 1=error only, 2=warning, 3=info, 4=debug, 5=trace)");

  if(!args.Parse())
  {
    std::cerr << "Problem parsing arguments" << std::endl;
    std::cout << "\nvtkPlusWinProbeVideoSourceTest help:" << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }

  vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);
  if(printHelp)
  {
    std::cout << "\n\nvtkPlusWinProbeVideoSourceTest help:" << args.GetHelp() << std::endl;
    exit(EXIT_SUCCESS);
  }


  vtkSmartPointer< vtkPlusWinProbeVideoSource > WinProbeDevice = vtkSmartPointer< vtkPlusWinProbeVideoSource >::New();
  WinProbeDevice->SetDeviceId("VideoDeviceWP");

  vtkSmartPointer<vtkXMLDataElement> configRootElement = vtkSmartPointer<vtkXMLDataElement>::New();
  if(STRCASECMP(inputConfigFileName.c_str(), "") != 0)
  {
    LOG_DEBUG("Reading config file...");

    if(PlusXmlUtils::ReadDeviceSetConfigurationFromFile(configRootElement, inputConfigFileName.c_str()) == PLUS_FAIL)
    {
      LOG_ERROR("Unable to read configuration from file " << inputConfigFileName.c_str());
      return EXIT_FAILURE;
    }

    WinProbeDevice->ReadConfiguration(configRootElement);
  }

  std::cout << "\n" << *WinProbeDevice; //invokes PrintSelf()

  if(WinProbeDevice->Connect() != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to connect to WinProbe Probe");
    exit(EXIT_FAILURE);
  }

  //test starting and stopping (pausing recording)
  WinProbeDevice->StartRecording(); //applies the setting read from config file
  std::cout << "\n" << *WinProbeDevice; //invokes PrintSelf()
  WinProbeDevice->StopRecording();

  //test TGCs
  double tgc = WinProbeDevice->GetTimeGainCompensation(7);
  tgc = WinProbeDevice->GetTimeGainCompensation(3);
  WinProbeDevice->SetTimeGainCompensation(3, 0.2);
  tgc = WinProbeDevice->GetTimeGainCompensation(3);

  //test intensity compression
  uint16_t uVal = WinProbeDevice->GetLogLinearKnee();
  WinProbeDevice->SetLogLinearKnee(123);
  uVal = WinProbeDevice->GetLogLinearKnee();

  WinProbeDevice->StartRecording();

  if(renderingOff)
  {
    Sleep(2500); //allow some time to buffer frames

    vtkPlusChannel* bChannel(nullptr);
    if(WinProbeDevice->GetOutputChannelByName(bChannel, "VideoStream") != PLUS_SUCCESS)
    {
      LOG_ERROR("Unable to locate the channel with Id=\"VideoStream\". Check config file.");
      exit(EXIT_FAILURE);
    }

    vtkPlusChannel* rfChannel(nullptr);
    if(WinProbeDevice->GetOutputChannelByName(rfChannel, "RfStream") != PLUS_SUCCESS)
    {
      LOG_WARNING("Unable to locate the channel with Id=\"RFStream\". RF mode will not be used.");
    }

    WinProbeDevice->FreezeDevice(true);

    vtkPlusDataSource* bSource(nullptr);
    bChannel->GetVideoSource(bSource);
    bSource->WriteToSequenceFile(outputFileName.c_str());

    if(rfChannel)
    {
      vtkPlusDataSource* rfSource(nullptr);
      rfChannel->GetVideoSource(rfSource);
      rfSource->WriteToSequenceFile((outputFileName + "_RF.mha").c_str());
    }

    //update and write configuration
    WinProbeDevice->WriteConfiguration(configRootElement);
    bool success = vtkXMLUtilities::WriteElementToFile(configRootElement, (outputFileName + ".xml").c_str());
    if(!success)
    {
      LOG_ERROR("Unable to write configuration to: " << outputFileName + ".xml");
    }
    else
    {
      LOG_INFO("Configuration file written to: " << outputFileName + ".xml");
    }

    WinProbeDevice->FreezeDevice(false);
  }
  else
  {
    vtkSmartPointer<vtkImageViewer> viewer = vtkSmartPointer<vtkImageViewer>::New();
    viewer->SetInputConnection(WinProbeDevice->GetOutputPort(0)); //set image to the render and window
    viewer->SetColorWindow(255);
    viewer->SetColorLevel(127.5);
    viewer->SetZSlice(0);
    viewer->SetSize(256, 640);

    //Create the interactor that handles the event loop
    vtkSmartPointer<vtkRenderWindowInteractor> iren = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    iren->SetRenderWindow(viewer->GetRenderWindow());
    viewer->SetupInteractor(iren);

    viewer->Render(); //must be called after iren and viewer are linked or there will be problems

    // Establish timer event and create timer to update the live image
    vtkSmartPointer<vtkMyCallback> call = vtkSmartPointer<vtkMyCallback>::New();
    call->m_Interactor = iren;
    call->m_Viewer = viewer;
    iren->AddObserver(vtkCommand::TimerEvent, call);
    iren->CreateTimer(VTKI_TIMER_FIRST);

    //iren must be initialized so that it can handle events
    iren->Initialize();
    iren->Start();
  }

  WinProbeDevice->StopRecording();
  WinProbeDevice->Disconnect();

  return EXIT_SUCCESS;
}
