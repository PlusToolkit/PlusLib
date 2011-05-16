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

vtkDataCollector* dataCollector = NULL; 
vtkImageViewer *viewer = NULL;
vtkRenderWindowInteractor *iren = NULL;
vtkTextActor *stepperTextActor = NULL; 

class vtkMyCallback : public vtkCommand
{
public:
	static vtkMyCallback *New()
	{return new vtkMyCallback;}
	virtual void Execute(vtkObject *caller, unsigned long, void*)
	{
		viewer->Render();

		std::ostringstream ss;
		ss.precision( 2 ); 
		vtkSmartPointer<vtkMatrix4x4> tFrame2Tracker = vtkSmartPointer<vtkMatrix4x4>::New(); 
		if ( dataCollector->GetTracker()->IsTracking())
		{
			double timestamp(0); 
			long flags(0); 
			dataCollector->GetTransformWithTimestamp(tFrame2Tracker, timestamp, flags, dataCollector->GetDefaultToolPortNumber()); 

			if (flags & (TR_MISSING | TR_OUT_OF_VIEW )) 
			{
				ss	<< "Tracker out of view..."; 
			}
			else if ( flags & (TR_REQ_TIMEOUT) ) 
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

	int verboseLevel=PlusLogger::LOG_LEVEL_INFO;

	vtksys::CommandLineArguments args;
	args.Initialize(argc, argv);

	args.AddArgument("--input-config-file-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputConfigFileName, "Name of the input configuration file.");
	args.AddArgument("--input-video-buffer-metafile", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputVideoBufferMetafile, "Video buffer sequence metafile.");
	args.AddArgument("--input-tracker-buffer-metafile", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputTrackerBufferMetafile, "Tracker buffer sequence metafile.");
	args.AddArgument("--rendering-off", vtksys::CommandLineArguments::NO_ARGUMENT, &renderingOff, "Run test without rendering.");	
	args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug)");	



	PlusLogger::Instance()->SetLogLevel(verboseLevel);

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

	///////////////

	VTK_LOG_TO_CONSOLE_ON; 

	dataCollector = vtkDataCollector::New(); 
	dataCollector->ReadConfiguration(inputConfigFileName.c_str());

	if ( dataCollector->GetAcquisitionType() == SYNCHRO_VIDEO_SAVEDDATASET )
	{
		vtkSavedDataVideoSource* videoSource = static_cast<vtkSavedDataVideoSource*>(dataCollector->GetVideoSource()); 
		videoSource->SetSequenceMetafile(inputVideoBufferMetafile.c_str()); 
	}

	if ( dataCollector->GetTrackerType() == TRACKER_SAVEDDATASET )
	{
		vtkSavedDataTracker* tracker = static_cast<vtkSavedDataTracker*>(dataCollector->GetTracker()); 
		tracker->SetSequenceMetafile(inputTrackerBufferMetafile.c_str()); 
	}

	dataCollector->Initialize(); 
	dataCollector->Start();

	if (renderingOff)
	{
		if ( dataCollector != NULL ) 
		{
			dataCollector->Delete();
		}

		exit(EXIT_SUCCESS); 
	}

	viewer = vtkImageViewer::New();
	viewer->SetInput(dataCollector->GetOutput());   //set image to the render and window
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
	
	dataCollector->Delete();

	call->Delete(); 
	viewer->Delete();
	iren->Delete();
	stepperTextActor->Delete(); 

	VTK_LOG_TO_CONSOLE_OFF; 

	std::cout << "vtkDataCollectorTest1 completed successfully!" << std::endl;
	return EXIT_SUCCESS; 

}
