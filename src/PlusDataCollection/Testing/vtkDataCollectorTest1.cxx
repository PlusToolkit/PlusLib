/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

/*!
  \file vtkDataCollectorTest2.cxx
  \brief This program acquires tracked ultrasound data and displays it on the screen (in a 2D viewer).
*/

// Local includes
#include "PlusConfigure.h"
//#include "igsioTrackedFrame.h"
#include "vtkPlusChannel.h"
#include "vtkPlusDataCollector.h"
#include "vtkPlusDataSource.h"
#include "vtkPlusDevice.h"
#include "vtkPlusRfProcessor.h"
#include "vtkPlusSavedDataSource.h"
#ifdef PLUS_USE_ULTRASONIX_VIDEO
  #include "vtkPlusSonixVideoSource.h"
#endif

// VTK includes
#include <vtkCallbackCommand.h>
#include <vtkCommand.h>
#include <vtkImageData.h>
#include <vtkImageViewer.h>
#include <vtkMatrix4x4.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <vtkXMLUtilities.h>
#include <vtksys/CommandLineArguments.hxx>

class vtkMyCallback : public vtkCommand
{
public:
  static vtkMyCallback* New()
  {
    return new vtkMyCallback;
  }

  virtual void Execute(vtkObject* caller, unsigned long, void*)
  {
    vtkSmartPointer<vtkMatrix4x4> tFrame2Tracker = vtkSmartPointer<vtkMatrix4x4>::New();

    igsioTrackedFrame trackedFrame;
    if (this->BroadcastChannel->GetTrackedFrame(trackedFrame) != PLUS_SUCCESS)
    {
      LOG_WARNING("Unable to get tracked frame!");
      return;
    }

    if (trackedFrame.GetImageData()->IsImageValid())
    {
      // Display image if it's valid
      if (trackedFrame.GetImageData()->GetImageType() == US_IMG_BRIGHTNESS || trackedFrame.GetImageData()->GetImageType() == US_IMG_RGB_COLOR)
      {
        // B mode
        this->ImageData->DeepCopy(trackedFrame.GetImageData()->GetImage());
      }
      else
      {
        // RF mode
        RfProcessor->SetRfFrame(trackedFrame.GetImageData()->GetImage(), trackedFrame.GetImageData()->GetImageType());
        this->ImageData->ShallowCopy(RfProcessor->GetBrightnessScanConvertedImage());
      }
      this->Viewer->SetInputData(this->ImageData);
      this->Viewer->Modified();
    }

    if (TransformName.IsValid())
    {
      std::ostringstream ss;
      ss.precision(2);
      ToolStatus status(TOOL_INVALID);
      if (trackedFrame.GetFrameTransformStatus(TransformName, status) == PLUS_SUCCESS && status == TOOL_OK)
      {
        trackedFrame.GetFrameTransform(TransformName, tFrame2Tracker);
        ss  << std::fixed
            << tFrame2Tracker->GetElement(0, 0) << "   " << tFrame2Tracker->GetElement(0, 1) << "   " << tFrame2Tracker->GetElement(0, 2) << "   " << tFrame2Tracker->GetElement(0, 3) << "\n"
            << tFrame2Tracker->GetElement(1, 0) << "   " << tFrame2Tracker->GetElement(1, 1) << "   " << tFrame2Tracker->GetElement(1, 2) << "   " << tFrame2Tracker->GetElement(1, 3) << "\n"
            << tFrame2Tracker->GetElement(2, 0) << "   " << tFrame2Tracker->GetElement(2, 1) << "   " << tFrame2Tracker->GetElement(2, 2) << "   " << tFrame2Tracker->GetElement(2, 3) << "\n"
            << tFrame2Tracker->GetElement(3, 0) << "   " << tFrame2Tracker->GetElement(3, 1) << "   " << tFrame2Tracker->GetElement(3, 2) << "   " << tFrame2Tracker->GetElement(3, 3) << "\n";
      }
      else
      {
        std::string strTransformName;
        TransformName.GetTransformName(strTransformName);
        ss  << "Transform '" << strTransformName << "' is invalid ...";
      }
      this->StepperTextActor->SetInput(ss.str().c_str());
      this->StepperTextActor->Modified();
    }

    this->Viewer->Render();

    //update the timer so it will trigger again
    this->RenderWindowInteractor->CreateTimer(VTKI_TIMER_UPDATE);
  }

  vtkPlusDataCollector* DataCollector;
  vtkPlusChannel* BroadcastChannel;
  vtkImageViewer* Viewer;
  vtkRenderWindowInteractor* RenderWindowInteractor;
  vtkTextActor* StepperTextActor;
  igsioTransformName TransformName;
  vtkImageData* ImageData;
  vtkPlusRfProcessor* RfProcessor;
};

int main(int argc, char** argv)
{
  std::string inputConfigFileName;
  bool renderingOff(false);
  std::string inputVideoBufferMetafile;
  std::string inputTrackerBufferMetafile;
  std::string inputTransformName;
  bool inputRepeat(false);
  std::string inputSonixIp;

  int verboseLevel = vtkIGSIOLogger::LOG_LEVEL_UNDEFINED;

  vtksys::CommandLineArguments args;
  args.Initialize(argc, argv);

  args.AddArgument("--config-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputConfigFileName, "Name of the input configuration file.");
#ifdef PLUS_USE_ULTRASONIX_VIDEO
  args.AddArgument("--sonix-ip", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputSonixIp, "IP address of the Ultrasonix scanner (overrides the IP address parameter defined in the config file; only applicable if VideoDevice is SonixVideo).");
#endif
  args.AddArgument("--video-buffer-seq-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputVideoBufferMetafile, "Video buffer sequence metafile.");
  args.AddArgument("--tracker-buffer-seq-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputTrackerBufferMetafile, "Tracker buffer sequence metafile.");
  args.AddArgument("--transform", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputTransformName, "Name of the transform displayed.");

  args.AddArgument("--rendering-off", vtksys::CommandLineArguments::NO_ARGUMENT, &renderingOff, "Run test without rendering.");
  args.AddArgument("--repeat", vtksys::CommandLineArguments::NO_ARGUMENT, &inputRepeat, "Repeat tracked frames after reached the latest one.");
  args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug, 5=trace)");

  if (!args.Parse())
  {
    std::cerr << "Problem parsing arguments" << std::endl;
    std::cout << "Help: " << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }

  vtkIGSIOLogger::Instance()->SetLogLevel(verboseLevel);

  if (inputConfigFileName.empty())
  {
    std::cerr << "input-config-file-name is required" << std::endl;
    exit(EXIT_FAILURE);
  }

  ///////////////

  vtkSmartPointer<vtkXMLDataElement> configRootElement = vtkSmartPointer<vtkXMLDataElement>::New();
  if (PlusXmlUtils::ReadDeviceSetConfigurationFromFile(configRootElement, inputConfigFileName.c_str()) == PLUS_FAIL)
  {
    LOG_ERROR("Unable to read configuration from file " << inputConfigFileName.c_str());
    return EXIT_FAILURE;
  }

  vtkPlusConfig::GetInstance()->SetDeviceSetConfigurationData(configRootElement);

  vtkSmartPointer<vtkPlusDataCollector> dataCollector = vtkSmartPointer<vtkPlusDataCollector>::New();

  if (dataCollector->ReadConfiguration(configRootElement) != PLUS_SUCCESS)
  {
    LOG_ERROR("Configuration incorrect for vtkPlusDataCollectorTest1.");
    exit(EXIT_FAILURE);
  }
  vtkPlusDevice* videoDevice = NULL;
  vtkPlusDevice* trackerDevice = NULL;

  if (! inputVideoBufferMetafile.empty())
  {
    if (dataCollector->GetDevice(videoDevice, "VideoDevice") != PLUS_SUCCESS)
    {
      LOG_ERROR("Unable to locate the device with Id=\"VideoDevice\". Check config file.");
      exit(EXIT_FAILURE);
    }
    vtkPlusSavedDataSource* videoSource = dynamic_cast<vtkPlusSavedDataSource*>(videoDevice);
    if (videoSource == NULL)
    {
      LOG_ERROR("Unable to cast video source to vtkPlusSavedDataSource.");
      exit(EXIT_FAILURE);
    }
    videoSource->SetSequenceFile(inputVideoBufferMetafile.c_str());
    videoSource->SetRepeatEnabled(inputRepeat);
  }
#ifdef PLUS_USE_ULTRASONIX_VIDEO
  else if (!inputSonixIp.empty())
  {
    if (dataCollector->GetDevice(videoDevice, "VideoDevice") != PLUS_SUCCESS)
    {
      LOG_ERROR("Unable to locate the device with Id=\"VideoDevice\". Check config file.");
      exit(EXIT_FAILURE);
    }
    vtkPlusSonixVideoSource* videoSource = dynamic_cast<vtkPlusSonixVideoSource*>(videoDevice);
    if (videoSource == NULL)
    {
      LOG_ERROR("Video source is not SonixVideo. Cannot set IP address.");
      exit(EXIT_FAILURE);
    }
    videoSource->SetSonixIP(inputSonixIp.c_str());
  }
#endif

  if (! inputTrackerBufferMetafile.empty())
  {
    if (dataCollector->GetDevice(trackerDevice, "TrackerDevice") != PLUS_SUCCESS)
    {
      LOG_ERROR("Unable to locate the device with Id=\"TrackerDevice\". Check config file.");
      exit(EXIT_FAILURE);
    }
    vtkPlusSavedDataSource* tracker = dynamic_cast<vtkPlusSavedDataSource*>(trackerDevice);
    if (tracker == NULL)
    {
      LOG_ERROR("Unable to cast tracker to vtkPlusSavedDataSource");
      exit(EXIT_FAILURE);
    }
    tracker->SetSequenceFile(inputTrackerBufferMetafile.c_str());
    tracker->SetRepeatEnabled(inputRepeat);
  }

  if (dataCollector->Connect() != PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to connect to devices!");
    exit(EXIT_FAILURE);
  }

  if (dataCollector->Start() != PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to start data collection!");
    exit(EXIT_FAILURE);
  }

  if (renderingOff)
  {
    LOG_DEBUG("Rendering is disabled");
  }
  else
  {
    if (dataCollector->GetDevice(videoDevice, "TrackedVideoDevice") != PLUS_SUCCESS)
    {
      LOG_ERROR("Unable to locate the device with Id=\"TrackedVideoDevice\". Check config file.");
      exit(EXIT_FAILURE);
    }
    vtkPlusChannel* aChannel(NULL);
    if (videoDevice->GetOutputChannelByName(aChannel, "TrackedVideoStream") != PLUS_SUCCESS)
    {
      LOG_ERROR("Unable to locate the channel with Id=\"TrackedVideoStream\". Check config file.");
      exit(EXIT_FAILURE);
    }

    vtkSmartPointer<vtkImageViewer> viewer = vtkSmartPointer<vtkImageViewer>::New();
    viewer->SetColorWindow(255);
    viewer->SetColorLevel(127.5);
    viewer->SetZSlice(0);
    viewer->SetSize(800, 600);

    // Create a text actor for tracking information
    vtkSmartPointer<vtkTextActor> stepperTextActor = vtkSmartPointer<vtkTextActor>::New();
    vtkSmartPointer<vtkTextProperty> textprop = stepperTextActor->GetTextProperty();
    textprop->SetColor(1, 0, 0);
    textprop->SetFontFamilyToArial();
    textprop->SetFontSize(15);
    textprop->SetJustificationToLeft();
    textprop->SetVerticalJustificationToTop();
    stepperTextActor->VisibilityOn();
    stepperTextActor->SetDisplayPosition(20, 65);
    viewer->GetRenderer()->AddActor(stepperTextActor);

    //Create the interactor that handles the event loop
    vtkSmartPointer<vtkRenderWindowInteractor> iren = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    iren->SetRenderWindow(viewer->GetRenderWindow());
    viewer->SetupInteractor(iren);

    viewer->Render();  //must be called after iren and viewer are linked
    //or there will be problems

    vtkSmartPointer<vtkImageData> imageData = vtkSmartPointer<vtkImageData>::New();

    vtkSmartPointer<vtkPlusRfProcessor> rfProc = vtkSmartPointer<vtkPlusRfProcessor>::New();
    rfProc->ReadConfiguration(configRootElement);

    //establish timer event and create timer
    vtkSmartPointer<vtkMyCallback> call = vtkSmartPointer<vtkMyCallback>::New();
    call->DataCollector = dataCollector;
    call->BroadcastChannel = aChannel;
    call->Viewer = viewer;
    call->RenderWindowInteractor = iren;
    call->StepperTextActor = stepperTextActor;
    call->ImageData = imageData;
    call->RfProcessor = rfProc;

    if (!inputTransformName.empty())
    {
      if (call->TransformName.SetTransformName(inputTransformName.c_str()) != PLUS_SUCCESS)
      {
        LOG_ERROR("Transform name '" << inputTransformName << "' is invalid!");
        return EXIT_FAILURE;
      }
    }

    iren->AddObserver(vtkCommand::TimerEvent, call);
    iren->CreateTimer(VTKI_TIMER_FIRST);    //VTKI_TIMER_FIRST = 0

    //iren must be initialized so that it can handle events
    iren->Initialize();
    iren->Start();
  }

  dataCollector->Disconnect();

  std::cout << "vtkPlusDataCollectorTest1 completed successfully!" << std::endl;
  return EXIT_SUCCESS;

}
