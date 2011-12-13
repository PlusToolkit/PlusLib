/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

/*!
  \file vtkDataCollectorTest1.cxx 
  \brief This program acquires tracked ultrasound data and displays it on the screen (in a 2D viewer).
  It can also broadcast the acquired data through OpenIGTLink.
*/ 

#include "PlusConfigure.h"
#include "vtksys/CommandLineArguments.hxx"
#include "vtkSmartPointer.h"
#include "vtkImageViewer.h"
#include "vtkCallbackCommand.h"
#include "vtkCommand.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkDataCollectorHardwareDevice.h"
#include "vtkTextProperty.h"
#include "vtkTextActor.h"
#include "vtkPlusVideoSource.h"
#include "vtkSavedDataTracker.h"
#include "vtkSavedDataVideoSource.h"
#include "vtkOpenIGTLinkBroadcaster.h"
#include "vtkXMLUtilities.h"
#include "vtkImageData.h" 
#include "vtkTrackerTool.h"
#include "TrackedFrame.h" 

PlusStatus InitBroadcaster(vtkSmartPointer<vtkOpenIGTLinkBroadcaster> &broadcaster, vtkDataCollector* dataCollector); 
PlusStatus InvokeBroadcasterMessage(vtkOpenIGTLinkBroadcaster* broadcaster); 

class vtkMyCallback : public vtkCommand
{
public:
  static vtkMyCallback *New()
  {return new vtkMyCallback;}
  virtual void Execute(vtkObject *caller, unsigned long, void*)
  {
    vtkSmartPointer<vtkMatrix4x4> tFrame2Tracker = vtkSmartPointer<vtkMatrix4x4>::New(); 

    TrackedFrame trackedFrame; 
    if ( this->DataCollector->GetTrackedFrame(&trackedFrame) != PLUS_SUCCESS )
    {
      LOG_WARNING("Unable to get tracked frame!"); 
      return; 
    }

    if ( trackedFrame.GetImageData()->IsImageValid() )
    {
      // Display image if it's valid
      this->Viewer->SetInput(trackedFrame.GetImageData()->GetVtkImage()); 
      this->Viewer->Modified(); 
    }

    std::ostringstream ss;
    ss.precision( 2 ); 
    if ( this->DataCollector->GetTracker()->IsTracking())
    {
      TrackedFrameFieldStatus status; 
      if (trackedFrame.GetCustomFrameTransformStatus(TransformName, status) == PLUS_SUCCESS 
        && status == FIELD_OK )
      {
        ss	<< std::fixed 
          << tFrame2Tracker->GetElement(0,0) << "   " << tFrame2Tracker->GetElement(0,1) << "   " << tFrame2Tracker->GetElement(0,2) << "   " << tFrame2Tracker->GetElement(0,3) << "\n"
          << tFrame2Tracker->GetElement(1,0) << "   " << tFrame2Tracker->GetElement(1,1) << "   " << tFrame2Tracker->GetElement(1,2) << "   " << tFrame2Tracker->GetElement(1,3) << "\n"
          << tFrame2Tracker->GetElement(2,0) << "   " << tFrame2Tracker->GetElement(2,1) << "   " << tFrame2Tracker->GetElement(2,2) << "   " << tFrame2Tracker->GetElement(2,3) << "\n"
          << tFrame2Tracker->GetElement(3,0) << "   " << tFrame2Tracker->GetElement(3,1) << "   " << tFrame2Tracker->GetElement(3,2) << "   " << tFrame2Tracker->GetElement(3,3) << "\n"; 
      }
      else
      {
        std::string strTransformName; 
        TransformName.GetTransformName(strTransformName); 
        ss	<< "Transform '" << strTransformName << "' is invalid ..."; 
      }
    }
    else
    {
      ss << "No tracker connected...";		
    }

    this->StepperTextActor->SetInput(ss.str().c_str());
    this->StepperTextActor->Modified(); 

    this->Viewer->Render();

    InvokeBroadcasterMessage(this->Broadcaster); 

    //update the timer so it will trigger again
    this->Iren->CreateTimer(VTKI_TIMER_UPDATE);
  }

  vtkDataCollectorHardwareDevice* DataCollector; 
  vtkImageViewer *Viewer;
  vtkRenderWindowInteractor *Iren;
  vtkTextActor *StepperTextActor; 
  vtkOpenIGTLinkBroadcaster* Broadcaster;
  PlusTransformName TransformName; 
};

int main(int argc, char **argv)
{

  std::string inputConfigFileName;
  bool renderingOff(false);
  std::string inputVideoBufferMetafile;
  std::string inputTrackerBufferMetafile;
  std::string inputTransformName; 
  bool inputReplay(false); 
  bool inputEnableBroadcasting(false); 

  int verboseLevel=vtkPlusLogger::LOG_LEVEL_DEFAULT;

  vtksys::CommandLineArguments args;
  args.Initialize(argc, argv);

  args.AddArgument("--input-config-file-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputConfigFileName, "Name of the input configuration file.");
  args.AddArgument("--input-video-buffer-metafile", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputVideoBufferMetafile, "Video buffer sequence metafile.");
  args.AddArgument("--input-tracker-buffer-metafile", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputTrackerBufferMetafile, "Tracker buffer sequence metafile.");
  args.AddArgument("--input-transform-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputTransformName, "Name of the transform displayed.");
  
  args.AddArgument("--rendering-off", vtksys::CommandLineArguments::NO_ARGUMENT, &renderingOff, "Run test without rendering.");	
  args.AddArgument("--replay", vtksys::CommandLineArguments::NO_ARGUMENT, &inputReplay, "Replay tracked frames after reached the latest one." );
  args.AddArgument("--enable-broadcasting", vtksys::CommandLineArguments::NO_ARGUMENT, &inputEnableBroadcasting, "Enable OpenIGTLink broadcasting." );
  args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug, 5=trace)");	

  if ( !args.Parse() )
  {
    std::cerr << "Problem parsing arguments" << std::endl;
    std::cout << "Help: " << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }

  if (inputConfigFileName.empty())
  {
    std::cerr << "input-config-file-name is required" << std::endl;
    exit(EXIT_FAILURE);
  }

  vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);

  ///////////////

  vtkSmartPointer<vtkXMLDataElement> configRootElement = vtkSmartPointer<vtkXMLDataElement>::Take(
    vtkXMLUtilities::ReadElementFromFile(inputConfigFileName.c_str()));
  if (configRootElement == NULL)
  {	
    std::cerr << "Unable to read configuration from file " << inputConfigFileName.c_str() << std::endl;
    exit( EXIT_FAILURE );
  }

  vtkPlusConfig::GetInstance()->SetDeviceSetConfigurationData(configRootElement);

  vtkSmartPointer<vtkDataCollector> dataCollector = vtkSmartPointer<vtkDataCollector>::New(); 

  vtkDataCollectorHardwareDevice* dataCollectorHardwareDevice = dynamic_cast<vtkDataCollectorHardwareDevice*>(dataCollector.GetPointer());
  if ( dataCollectorHardwareDevice == NULL )
  {
    LOG_ERROR("Failed to create the propertype of data collector!");
    exit( EXIT_FAILURE );
  }

  dataCollectorHardwareDevice->ReadConfiguration( configRootElement );

  if ( ! inputVideoBufferMetafile.empty()
    && dataCollectorHardwareDevice->GetAcquisitionType() == SYNCHRO_VIDEO_SAVEDDATASET )
  {
    vtkSavedDataVideoSource* videoSource = dynamic_cast<vtkSavedDataVideoSource*>(dataCollectorHardwareDevice->GetVideoSource()); 
    if ( videoSource == NULL )
    {
      LOG_ERROR( "Unable to cast video source to vtkSavedDataVideoSource." );
      exit( EXIT_FAILURE );
    }
    videoSource->SetSequenceMetafile(inputVideoBufferMetafile.c_str()); 
    videoSource->SetReplayEnabled(inputReplay); 
  }

  if ( ! inputTrackerBufferMetafile.empty()
    && dataCollectorHardwareDevice->GetTrackerType() == TRACKER_SAVEDDATASET )
  {
    vtkSavedDataTracker* tracker = dynamic_cast<vtkSavedDataTracker*>(dataCollectorHardwareDevice->GetTracker()); 
    if ( tracker == NULL )
    {
      LOG_ERROR( "Unable to cast tracker to vtkSavedDataTracker." );
      exit( EXIT_FAILURE );
    }
    tracker->SetSequenceMetafile(inputTrackerBufferMetafile.c_str()); 
    tracker->SetReplayEnabled(inputReplay); 
  }

  dataCollectorHardwareDevice->Connect(); 

  vtkSmartPointer<vtkOpenIGTLinkBroadcaster> broadcaster;
  if ( inputEnableBroadcasting && InitBroadcaster(broadcaster, dataCollector) != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to initialize OpenIGTLink bradcaster!"); 
    if ( broadcaster != NULL )
    {
      broadcaster->Delete(); 
      broadcaster = NULL; 
    }
  }

  dataCollectorHardwareDevice->Start();

  if (renderingOff)
  {
    LOG_DEBUG("Rendering is disabled");
  }
  else
  {

    vtkSmartPointer<vtkImageViewer> viewer = vtkSmartPointer<vtkImageViewer>::New();
    viewer->SetColorWindow(255);
    viewer->SetColorLevel(127.5);
    viewer->SetZSlice(0);

    // Create a text actor for tracking information
    vtkSmartPointer<vtkTextActor> stepperTextActor = vtkSmartPointer<vtkTextActor>::New(); 
    vtkSmartPointer<vtkTextProperty> textprop = stepperTextActor->GetTextProperty();
    textprop->SetColor(1,0,0);
    textprop->SetFontFamilyToArial();
    textprop->SetFontSize(15);
    textprop->SetJustificationToLeft();
    textprop->SetVerticalJustificationToTop();
    stepperTextActor->VisibilityOn(); 
    stepperTextActor->SetDisplayPosition(20,65); 
    viewer->GetRenderer()->AddActor(stepperTextActor); 

    //Create the interactor that handles the event loop
    vtkSmartPointer<vtkRenderWindowInteractor> iren = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    iren->SetRenderWindow(viewer->GetRenderWindow());
    viewer->SetupInteractor(iren);

    viewer->Render();	//must be called after iren and viewer are linked
    //or there will be problems

    //establish timer event and create timer
    vtkSmartPointer<vtkMyCallback> call = vtkSmartPointer<vtkMyCallback>::New();
    call->DataCollector=dataCollectorHardwareDevice; 
    call->Viewer=viewer;
    call->Iren=iren;
    call->StepperTextActor=stepperTextActor; 
    call->Broadcaster=broadcaster;
    
    if ( !inputTransformName.empty() )
    {
      if (call->TransformName.SetTransformName( inputTransformName.c_str() ) != PLUS_SUCCESS )
      {
        LOG_ERROR("Transform name '"<<inputTransformName<<"' is invalid!"); 
        return EXIT_FAILURE; 
      }
    }

    iren->AddObserver(vtkCommand::TimerEvent, call);
    iren->CreateTimer(VTKI_TIMER_FIRST);		//VTKI_TIMER_FIRST = 0

    //iren must be initialized so that it can handle events
    iren->Initialize();
    iren->Start();
  }

  dataCollectorHardwareDevice->Disconnect();

  std::cout << "vtkDataCollectorTest1 completed successfully!" << std::endl;
  return EXIT_SUCCESS; 

}

PlusStatus InitBroadcaster(vtkSmartPointer<vtkOpenIGTLinkBroadcaster> &broadcaster, vtkDataCollector* dataCollector)
{
  broadcaster = vtkSmartPointer<vtkOpenIGTLinkBroadcaster>::New();
  broadcaster->SetDataCollector( dataCollector );

  if (broadcaster->Initialize() != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to initialize OpenIGTLink broadcaster!");
    return PLUS_FAIL; 
  }

  return PLUS_SUCCESS; 
}

PlusStatus InvokeBroadcasterMessage(vtkOpenIGTLinkBroadcaster* broadcaster)
{
  if ( broadcaster == NULL )
  {
    LOG_DEBUG("Unable invoke broadcaster message - broadcaster is NULL!"); 
    return PLUS_FAIL; 
  }

  std::string errorMessage;

  if (broadcaster->SendMessages() != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to send message!");
    return PLUS_FAIL; 
  }

  return PLUS_SUCCESS; 
}
