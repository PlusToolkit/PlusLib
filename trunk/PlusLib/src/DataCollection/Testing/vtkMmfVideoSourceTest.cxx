/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkCommand.h"
#include "vtkImageData.h"
#include "vtkImageViewer2.h"
#include "vtkPlusBuffer.h"
#include "vtkMmfVideoSource.h"
#include "vtkPlusDataSource.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkSmartPointer.h"
#include "vtkMmfVideoSource.h"
#include "vtksys/CommandLineArguments.hxx"

void PrintLogsCallback(vtkObject* obj, unsigned long eid, void* clientdata, void* calldata); 

class vtkMyCallback : public vtkCommand
{
public:
  static vtkMyCallback *New()
  {return new vtkMyCallback;}
  virtual void Execute(vtkObject *caller, unsigned long, void*)
  {
    viewer->Render();

    //update the timer so it will trigger again
    iren->CreateTimer(VTKI_TIMER_UPDATE);
  }
  vtkImageViewer2 *viewer;
  vtkRenderWindowInteractor *iren;
protected:
  vtkMyCallback()
  {
    viewer = NULL;
    iren = NULL;
  }
};

int main(int argc, char **argv)
{
  bool printHelp(false); 
  bool renderingOff(false);
  bool showDialogs(false);
  std::vector<int> frameSize;
  int deviceId=0;
  std::string pixelFormatName;
  bool listDevices=false;
  bool listVideoFormats=false;

  vtksys::CommandLineArguments args;
  args.Initialize(argc, argv);

  int verboseLevel = vtkPlusLogger::LOG_LEVEL_UNDEFINED;

  args.AddArgument("--list-devices", vtksys::CommandLineArguments::NO_ARGUMENT, &listDevices, "Show the list of available devices and exit");
  args.AddArgument("--list-video-formats", vtksys::CommandLineArguments::NO_ARGUMENT, &listVideoFormats, "Show the list of supported video formats for the selected device and exit");  
  args.AddArgument("--device-id", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &deviceId, "Capture device ID (default: 0)");
  args.AddArgument("--frame-size", vtksys::CommandLineArguments::MULTI_ARGUMENT, &frameSize, "Requested frame size from the capture device");
  args.AddArgument("--video-format", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &pixelFormatName, "Requested video format (YUY2, ...)");
  args.AddArgument("--help", vtksys::CommandLineArguments::NO_ARGUMENT, &printHelp, "Print this help.");  
  args.AddArgument("--rendering-off", vtksys::CommandLineArguments::NO_ARGUMENT, &renderingOff, "Run test without rendering.");  
  args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug, 5=trace)");  

  if ( !args.Parse() )
  {
    std::cerr << "Problem parsing arguments" << std::endl;
    std::cout << "\n\nvtkMmfVideoSourceTest help:" << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }

  vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);

  if ( printHelp ) 
  {
    std::cout << "\n\nvtkMmfVideoSourceTest help:" << args.GetHelp() << std::endl;
    exit(EXIT_SUCCESS);
  }

  vtkSmartPointer<vtkMmfVideoSource> frameGrabber = vtkSmartPointer<vtkMmfVideoSource>::New();  

  if (listDevices)
  {
    frameGrabber->LogListOfCaptureDevices();
    exit(EXIT_SUCCESS);
  }

  frameGrabber->SetRequestedDeviceId(deviceId);

  if (listVideoFormats)
  {
    frameGrabber->LogListOfCaptureVideoFormats(deviceId);
    exit(EXIT_SUCCESS);
  }

  if (!pixelFormatName.empty())
  {
    frameGrabber->SetRequestedVideoFormat(pixelFormatName);
  }

  if (!frameSize.empty())
  {
    if (frameSize.size()!=2)
    {
      LOG_ERROR("Frame size shall contain two numbers, separated by a space");
      return EXIT_FAILURE;
    }
    frameGrabber->SetRequestedFrameSize(&(frameSize[0]));   
  }
 
  frameGrabber->CreateDefaultOutputChannel();
  LOG_INFO("Initialize..."); 
  frameGrabber->Connect();

  if ( frameGrabber->GetConnected() )
  {
    LOG_INFO("Start recording..."); 
    frameGrabber->StartRecording(); 
  }
  else
  {
    frameGrabber->Disconnect();
    LOG_ERROR( "Unable to connect to media foundation capture device."); 
    exit(EXIT_FAILURE); 
  }


  if (renderingOff)
  {
    LOG_INFO("No need for rendering, stop the device..."); 
    frameGrabber->StopRecording(); 
    frameGrabber->Disconnect();
    LOG_INFO("Exit successfully"); 
    exit(EXIT_SUCCESS); 
  }

  vtkSmartPointer<vtkImageViewer2> viewer = vtkSmartPointer<vtkImageViewer2>::New();

  viewer->SetColorWindow(255);
  viewer->SetColorLevel(100.5);
  vtkPlusDataSource* videoSource=NULL;
  if (frameGrabber->GetFirstActiveOutputVideoSource(videoSource) != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to retrieve the video source.");
    exit(EXIT_FAILURE);
  }
  viewer->SetSize(videoSource->GetBuffer()->GetFrameSize()[0], videoSource->GetBuffer()->GetFrameSize()[1]); 

  vtkImageData* output = vtkImageData::SafeDownCast(frameGrabber->GetOutput());
  viewer->SetInput(vtkImageData::SafeDownCast(frameGrabber->GetOutput())); 

  //Create the interactor that handles the event loop
  vtkSmartPointer<vtkRenderWindowInteractor> iren = vtkSmartPointer<vtkRenderWindowInteractor>::New();
  iren->SetRenderWindow(viewer->GetRenderWindow());
  viewer->SetupInteractor(iren);

  viewer->Render(); 

  //establish timer event and create timer
  vtkSmartPointer<vtkMyCallback> call = vtkSmartPointer<vtkMyCallback>::New();
  call->iren=iren;
  call->viewer=viewer;
  iren->AddObserver(vtkCommand::TimerEvent, call);
  iren->CreateTimer(VTKI_TIMER_FIRST);    //VTKI_TIMER_FIRST = 0

  //iren must be initialized so that it can handle events
  iren->Initialize();
  iren->Start();

  LOG_INFO("Exit successfully"); 
  return EXIT_SUCCESS; 

}