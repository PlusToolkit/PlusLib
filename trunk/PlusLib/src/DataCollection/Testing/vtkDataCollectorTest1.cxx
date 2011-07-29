#include "PlusConfigure.h"
#include "vtksys/CommandLineArguments.hxx"
#include "vtkSmartPointer.h"
#include "vtkImageViewer.h"
#include "vtkCallbackCommand.h"
#include "vtkCommand.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkDataCollector.h"
#include "vtkTextProperty.h"
#include "vtkTextActor.h"
#include "vtkVideoSource2.h"
#include "vtkSavedDataTracker.h"
#include "vtkSavedDataVideoSource.h"
#include "vtkOpenIGTLinkBroadcaster.h"

vtkDataCollector* dataCollector = NULL; 
vtkImageViewer *viewer = NULL;
vtkRenderWindowInteractor *iren = NULL;
vtkTextActor *stepperTextActor = NULL; 
vtkOpenIGTLinkBroadcaster* broadcaster = NULL; 
vtkImageData* realtimeImage; 

PlusStatus InitBroadcater(); 
PlusStatus InvokeBroadcasterMessage(); 

class vtkMyCallback : public vtkCommand
{
public:
  static vtkMyCallback *New()
  {return new vtkMyCallback;}
  virtual void Execute(vtkObject *caller, unsigned long, void*)
  {
    TrackerStatus status = TR_OK; 
    double synchronizedTime(0); 
    vtkSmartPointer<vtkMatrix4x4> tFrame2Tracker = vtkSmartPointer<vtkMatrix4x4>::New(); 

	int probeToolNumber = dataCollector->GetTracker()->GetFirstPortNumberByType(TRACKER_TOOL_PROBE);
	if ( probeToolNumber < 0 )
	{
		LOG_ERROR("Unable to find probe!");
		return;
	}
    if ( dataCollector->GetTrackedFrame(realtimeImage, tFrame2Tracker, status, synchronizedTime, probeToolNumber) == PLUS_SUCCESS )
    {
      viewer->SetInput(realtimeImage); 
      viewer->Modified(); 
    }
    else
    {
      LOG_WARNING("Unable to get tracked frame!"); 
      return; 
    }

    std::ostringstream ss;
    ss.precision( 2 ); 
    if ( dataCollector->GetTracker()->IsTracking())
    {
      if (status == TR_MISSING || status == TR_OUT_OF_VIEW ) 
      {
        ss	<< "Tracker out of view..."; 
      }
      else if ( status == TR_REQ_TIMEOUT ) 
      {
        ss	<< "Tracker request timeout..."; 
      }
      else
      {
        ss	<< std::fixed 
          << tFrame2Tracker->GetElement(0,0) << "   " << tFrame2Tracker->GetElement(0,1) << "   " << tFrame2Tracker->GetElement(0,2) << "   " << tFrame2Tracker->GetElement(0,3) << "\n"
          << tFrame2Tracker->GetElement(1,0) << "   " << tFrame2Tracker->GetElement(1,1) << "   " << tFrame2Tracker->GetElement(1,2) << "   " << tFrame2Tracker->GetElement(1,3) << "\n"
          << tFrame2Tracker->GetElement(2,0) << "   " << tFrame2Tracker->GetElement(2,1) << "   " << tFrame2Tracker->GetElement(2,2) << "   " << tFrame2Tracker->GetElement(2,3) << "\n"
          << tFrame2Tracker->GetElement(3,0) << "   " << tFrame2Tracker->GetElement(3,1) << "   " << tFrame2Tracker->GetElement(3,2) << "   " << tFrame2Tracker->GetElement(3,3) << "\n"; 
      }
    }
    else
    {
      ss << "Unable to connect to tracker...";		
    }

    stepperTextActor->SetInput(ss.str().c_str());
    stepperTextActor->Modified(); 

    viewer->Render();

    InvokeBroadcasterMessage(); 

    //update the timer so it will trigger again
    iren->CreateTimer(VTKI_TIMER_UPDATE);
  }
};

int main(int argc, char **argv)
{

  std::string inputConfigFileName;
  bool renderingOff(false);
  std::string inputVideoBufferMetafile;
  std::string inputTrackerBufferMetafile;
  bool inputReplay(false); 
  bool inputEnableBroadcasting(false); 

  int verboseLevel=PlusLogger::LOG_LEVEL_INFO;

  vtksys::CommandLineArguments args;
  args.Initialize(argc, argv);

  args.AddArgument("--input-config-file-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputConfigFileName, "Name of the input configuration file.");
  args.AddArgument("--input-video-buffer-metafile", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputVideoBufferMetafile, "Video buffer sequence metafile.");
  args.AddArgument("--input-tracker-buffer-metafile", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputTrackerBufferMetafile, "Tracker buffer sequence metafile.");
  args.AddArgument("--rendering-off", vtksys::CommandLineArguments::NO_ARGUMENT, &renderingOff, "Run test without rendering.");	
  args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug)");	
  args.AddArgument( "--replay", vtksys::CommandLineArguments::NO_ARGUMENT, &inputReplay, "Replay tracked frames after reached the latest one." );
  args.AddArgument( "--enable-broadcasting", vtksys::CommandLineArguments::NO_ARGUMENT, &inputEnableBroadcasting, "Enable OpenIGTLink broadcasting." );

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

  PlusLogger::Instance()->SetLogLevel(verboseLevel);
  PlusLogger::Instance()->SetDisplayLogLevel(verboseLevel);

  ///////////////

  VTK_LOG_TO_CONSOLE_ON; 

  dataCollector = vtkDataCollector::New(); 
  dataCollector->ReadConfigurationFromFile(inputConfigFileName.c_str());

  if ( ! inputVideoBufferMetafile.empty()
    && dataCollector->GetAcquisitionType() == SYNCHRO_VIDEO_SAVEDDATASET )
  {
    vtkSavedDataVideoSource* videoSource = dynamic_cast<vtkSavedDataVideoSource*>(dataCollector->GetVideoSource()); 
    if ( videoSource == NULL )
    {
      LOG_ERROR( "Unable to cast video source to vtkSavedDataVideoSource." );
      exit( EXIT_FAILURE );
    }
    videoSource->SetSequenceMetafile(inputVideoBufferMetafile.c_str()); 
    videoSource->SetReplayEnabled(inputReplay); 
  }

  if ( ! inputTrackerBufferMetafile.empty()
    && dataCollector->GetTrackerType() == TRACKER_SAVEDDATASET )
  {
    vtkSavedDataTracker* tracker = dynamic_cast<vtkSavedDataTracker*>(dataCollector->GetTracker()); 
    if ( tracker == NULL )
    {
      LOG_ERROR( "Unable to cast tracker to vtkSavedDataTracker." );
      exit( EXIT_FAILURE );
    }
    tracker->SetSequenceMetafile(inputTrackerBufferMetafile.c_str()); 
    tracker->SetReplayEnabled(inputReplay); 
  }

  dataCollector->Initialize(); 

  if ( inputEnableBroadcasting && InitBroadcater() != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to initialize OpenIGTLink bradcaster!"); 
    if ( broadcaster != NULL )
    {
      broadcaster->Delete(); 
      broadcaster = NULL; 
    }
  }

  dataCollector->Start();

  if (renderingOff)
  {
    if ( dataCollector != NULL ) 
    {
      dataCollector->Delete();
    }

    exit(EXIT_SUCCESS); 
  }

  int * frameSize = dataCollector->GetVideoSource()->GetFrameSize(); 
  realtimeImage = vtkImageData::New(); 
  realtimeImage->SetExtent( 0, frameSize[0] - 1, 0, frameSize[1] - 1, 0, 0); 
  realtimeImage->SetNumberOfScalarComponents(1); 
  realtimeImage->SetScalarTypeToUnsignedChar(); 
  realtimeImage->AllocateScalars();

  viewer = vtkImageViewer::New();
  viewer->SetInput(realtimeImage);   //set image to the render and window
  viewer->SetColorWindow(255);
  viewer->SetColorLevel(127.5);
  viewer->SetZSlice(0);

  // Create a text actor for tracking information
  stepperTextActor = vtkTextActor::New(); 
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
  iren = vtkRenderWindowInteractor::New();
  iren->SetRenderWindow(viewer->GetRenderWindow());
  viewer->SetupInteractor(iren);

  viewer->Render();	//must be called after iren and viewer are linked
  //or there will be problems

  //establish timer event and create timer
  vtkMyCallback* call = vtkMyCallback::New();
  iren->AddObserver(vtkCommand::TimerEvent, call);
  iren->CreateTimer(VTKI_TIMER_FIRST);		//VTKI_TIMER_FIRST = 0

  //iren must be initialized so that it can handle events
  iren->Initialize();
  iren->Start();

  if ( dataCollector != NULL )
  {
    dataCollector->Delete();
    dataCollector = NULL; 
  }

  if ( broadcaster != NULL )
  {
    broadcaster->Delete(); 
    broadcaster = NULL; 
  }

  call->Delete(); 
  viewer->Delete();
  iren->Delete();
  stepperTextActor->Delete(); 
  realtimeImage->Delete(); 

  VTK_LOG_TO_CONSOLE_OFF; 

  std::cout << "vtkDataCollectorTest1 completed successfully!" << std::endl;
  return EXIT_SUCCESS; 

}

PlusStatus InitBroadcater()
{
  vtkOpenIGTLinkBroadcaster::Status broadcasterStatus = vtkOpenIGTLinkBroadcaster::STATUS_NOT_INITIALIZED;
  broadcaster = vtkOpenIGTLinkBroadcaster::New();
  broadcaster->SetDataCollector( dataCollector );

  std::string errorMessage;
  broadcasterStatus = broadcaster->Initialize( errorMessage );
  switch ( broadcasterStatus )
  {
  case vtkOpenIGTLinkBroadcaster::STATUS_OK:
    // no error, continue
    break;
  case vtkOpenIGTLinkBroadcaster::STATUS_NOT_INITIALIZED:
    LOG_ERROR("Couldn't initialize OpenIGTLink broadcaster.");
    return PLUS_FAIL; 
  case vtkOpenIGTLinkBroadcaster::STATUS_HOST_NOT_FOUND:
    LOG_ERROR("Could not connect to host: " << errorMessage);
    return PLUS_FAIL; 
  case vtkOpenIGTLinkBroadcaster::STATUS_MISSING_DEFAULT_TOOL:
    LOG_ERROR("Default tool not defined. ");
    return PLUS_FAIL; 
  default:
    LOG_ERROR("Unknown error while trying to intialize the broadcaster. ");
    return PLUS_FAIL; 
  }

  return PLUS_SUCCESS; 
}

PlusStatus InvokeBroadcasterMessage()
{
  if ( broadcaster == NULL )
  {
    LOG_DEBUG("Unable invoke broadcaster message - broadcaster is NULL!"); 
    return PLUS_FAIL; 
  }

  vtkOpenIGTLinkBroadcaster::Status broadcasterStatus = vtkOpenIGTLinkBroadcaster::STATUS_NOT_INITIALIZED;
  std::string errorMessage;

  broadcasterStatus = broadcaster->SendMessages( errorMessage );

  // Display messages depending on the status of broadcaster.
  switch (broadcasterStatus)
  {
  case vtkOpenIGTLinkBroadcaster::STATUS_OK:
    // no error, no message
    break;
  case vtkOpenIGTLinkBroadcaster::STATUS_HOST_NOT_FOUND:
    LOG_WARNING("Host not found: " << errorMessage);
    return PLUS_FAIL; 
  case vtkOpenIGTLinkBroadcaster::STATUS_NOT_INITIALIZED:
    LOG_WARNING("OpenIGTLink broadcaster not initialized.");
    return PLUS_FAIL; 
  case vtkOpenIGTLinkBroadcaster::STATUS_NOT_TRACKING:
    LOG_WARNING("Tracking error detected.");
    return PLUS_FAIL; 
  case vtkOpenIGTLinkBroadcaster::STATUS_SEND_ERROR:
    LOG_WARNING("Could not send OpenIGTLink message.");
    return PLUS_FAIL; 
  default:
    LOG_WARNING("Unknown status while trying to send OpenIGTLink message.");
    return PLUS_FAIL; 
  }

  return PLUS_SUCCESS; 
}
