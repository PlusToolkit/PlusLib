/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

// Local includes
#include "PlusConfigure.h"
#include "vtkPlusDataSource.h"
#include "vtkPlusMmfVideoSource.h"

// VTK includes
#include <vtkCommand.h>
#include <vtkImageData.h>
#include <vtkImageViewer2.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtksys/CommandLineArguments.hxx>

// MF Library
#include <MediaFoundationVideoCaptureApi.h>

void PrintLogsCallback(vtkObject* obj, unsigned long eid, void* clientdata, void* calldata);

class vtkMyCallback : public vtkCommand
{
public:
  static vtkMyCallback* New()
  {return new vtkMyCallback;}
  virtual void Execute(vtkObject* caller, unsigned long, void*)
  {
    viewer->Render();

    //update the timer so it will trigger again
    iren->CreateTimer(VTKI_TIMER_UPDATE);
  }
  vtkImageViewer2* viewer;
  vtkRenderWindowInteractor* iren;
protected:
  vtkMyCallback()
  {
    viewer = NULL;
    iren = NULL;
  }
};

int main(int argc, char** argv)
{
  bool printHelp(false);
  bool renderingOff(false);
  bool showDialogs(false);
  FrameSizeType frameSize;
  int deviceId = 0;
  int streamIndex = 0;
  std::string pixelFormatName;
  bool listDevices = false;
  bool listVideoFormats = false;

  vtksys::CommandLineArguments args;
  args.Initialize(argc, argv);

  int verboseLevel = vtkIGSIOLogger::LOG_LEVEL_UNDEFINED;

  std::vector<int> frameSizeInt;

  args.AddArgument("--list-devices", vtksys::CommandLineArguments::NO_ARGUMENT, &listDevices, "Show the list of available devices and exit");
  args.AddArgument("--list-video-formats", vtksys::CommandLineArguments::NO_ARGUMENT, &listVideoFormats, "Show the list of supported video formats for the selected device and exit");
  args.AddArgument("--device-id", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &deviceId, "Capture device ID (default: 0)");
  args.AddArgument("--stream-index", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &streamIndex, "Stream index (default=0; see --list-devices for available streams");
  args.AddArgument("--frame-size", vtksys::CommandLineArguments::MULTI_ARGUMENT, &frameSizeInt, "Requested frame size from the capture device");
  args.AddArgument("--video-format", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &pixelFormatName, "Requested video format (YUY2, ...)");
  args.AddArgument("--help", vtksys::CommandLineArguments::NO_ARGUMENT, &printHelp, "Print this help.");
  args.AddArgument("--rendering-off", vtksys::CommandLineArguments::NO_ARGUMENT, &renderingOff, "Run test without rendering.");
  args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug, 5=trace)");

  if (!args.Parse())
  {
    std::cerr << "Problem parsing arguments" << std::endl;
    std::cout << "\n\nvtkPlusMmfVideoSourceTest help:" << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }

  vtkIGSIOLogger::Instance()->SetLogLevel(verboseLevel);

  if (printHelp)
  {
    std::cout << "\n\nvtkPlusMmfVideoSourceTest help:" << args.GetHelp() << std::endl;
    exit(EXIT_SUCCESS);
  }

  if (frameSize.size() < 2 || frameSize[0] < 0 || frameSize[1] < 0)
  {
    LOG_ERROR("Invalid frame size inputted. At least two non-negative entries required.");
    exit(EXIT_SUCCESS);
  }
  vtkSmartPointer<vtkPlusMmfVideoSource> frameGrabber = vtkSmartPointer<vtkPlusMmfVideoSource>::New();

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

  if (pixelFormatName.empty())
  {
    auto& api = MfVideoCapture::MediaFoundationVideoCaptureApi::GetInstance();
    auto mediaType = api.GetFormat(deviceId, streamIndex, 0);
    pixelFormatName = std::string(begin(mediaType.MF_MT_SUBTYPEName), end(mediaType.MF_MT_SUBTYPEName));
    pixelFormatName = pixelFormatName.substr(pixelFormatName.find('_') + 1);
  }

  frameGrabber->SetRequestedVideoFormat(std::wstring(pixelFormatName.begin(), pixelFormatName.end()));
  frameGrabber->SetRequestedStreamIndex(streamIndex);

  if (!frameSize.empty())
  {
    if (frameSize.size() != 2)
    {
      LOG_ERROR("Frame size shall contain two numbers, separated by a space");
      return EXIT_FAILURE;
    }
    FrameSizeType size;
    for (unsigned int i = 0; i < frameSize.size(); ++i)
    {
      size[i] = frameSize[i];
    }
    frameGrabber->SetRequestedFrameSize(frameSize);
  }

  frameGrabber->CreateDefaultOutputChannel();
  LOG_INFO("Initialize...");
  frameGrabber->Connect();

  if (frameGrabber->GetConnected())
  {
    LOG_INFO("Start recording...");
    frameGrabber->StartRecording();
  }
  else
  {
    frameGrabber->Disconnect();
    LOG_ERROR("Unable to disconnect from media foundation capture device.");
    exit(EXIT_FAILURE);
  }

  if (renderingOff)
  {
    LOG_INFO("No need for rendering, stop the device...");
    frameGrabber->Disconnect();
    LOG_INFO("Exit successfully");
    exit(EXIT_SUCCESS);
  }

  vtkSmartPointer<vtkImageViewer2> viewer = vtkSmartPointer<vtkImageViewer2>::New();

  viewer->SetColorWindow(255);
  viewer->SetColorLevel(100.5);
  vtkPlusDataSource* videoSource = NULL;
  if (frameGrabber->GetFirstActiveOutputVideoSource(videoSource) != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to retrieve the video source.");
    exit(EXIT_FAILURE);
  }
  viewer->SetSize(videoSource->GetOutputFrameSize()[0], videoSource->GetOutputFrameSize()[1]);

  viewer->SetInputConnection(frameGrabber->GetOutputPort());

  //Create the interactor that handles the event loop
  vtkSmartPointer<vtkRenderWindowInteractor> iren = vtkSmartPointer<vtkRenderWindowInteractor>::New();
  iren->SetRenderWindow(viewer->GetRenderWindow());
  viewer->SetupInteractor(iren);

  viewer->Render();

  //establish timer event and create timer
  vtkSmartPointer<vtkMyCallback> call = vtkSmartPointer<vtkMyCallback>::New();
  call->iren = iren;
  call->viewer = viewer;
  iren->AddObserver(vtkCommand::TimerEvent, call);
  iren->CreateTimer(VTKI_TIMER_FIRST);    //VTKI_TIMER_FIRST = 0

  //iren must be initialized so that it can handle events
  iren->Initialize();
  iren->Start();

  LOG_INFO("Exit successfully");
  return EXIT_SUCCESS;
}