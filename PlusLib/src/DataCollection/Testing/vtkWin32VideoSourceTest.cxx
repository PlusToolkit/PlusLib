/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkCallbackCommand.h"
#include "vtkCommand.h"
#include "vtkImageData.h"
#include "vtkImageViewer2.h"
#include "vtkPlusChannel.h"
#include "vtkPlusDataSource.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkSmartPointer.h"
#include "vtkWin32VideoSource2.h"
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

	vtksys::CommandLineArguments args;
	args.Initialize(argc, argv);

	int verboseLevel = vtkPlusLogger::LOG_LEVEL_UNDEFINED;

	args.AddArgument("--help", vtksys::CommandLineArguments::NO_ARGUMENT, &printHelp, "Print this help.");	
	args.AddArgument("--rendering-off", vtksys::CommandLineArguments::NO_ARGUMENT, &renderingOff, "Run test without rendering.");	
  args.AddArgument("--show-dialogs", vtksys::CommandLineArguments::NO_ARGUMENT, &showDialogs, "Show video source and format dialogs");	
	args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug, 5=trace)");	

	if ( !args.Parse() )
	{
		std::cerr << "Problem parsing arguments" << std::endl;
		std::cout << "\n\nvtkWin32VideoSourceTest help:" << args.GetHelp() << std::endl;
		exit(EXIT_FAILURE);
	}

  vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);
  
	if ( printHelp ) 
	{
		std::cout << "\n\nvtkWin32VideoSourceTest help:" << args.GetHelp() << std::endl;
		exit(EXIT_SUCCESS); 

	}

	vtkSmartPointer<vtkWin32VideoSource2> frameGrabber = vtkSmartPointer<vtkWin32VideoSource2>::New();
  frameGrabber->TestCreateDefaultVideoSource();
  vtkPlusChannel* aChannel(NULL);
  vtkPlusDataSource* aSource(NULL);
  if( frameGrabber->GetOutputChannelByName(aChannel, "DefaultChannel") != PLUS_SUCCESS || aChannel->GetVideoSource(aSource) != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to retrieve the video source.");
    return NULL;
  }

	// Add an observer to warning and error events for redirecting it to the stdout 
	vtkSmartPointer<vtkCallbackCommand> callbackCommand = vtkSmartPointer<vtkCallbackCommand>::New();
	callbackCommand->SetCallback(PrintLogsCallback);
	frameGrabber->AddObserver("WarningEvent", callbackCommand); 
	frameGrabber->AddObserver("ErrorEvent", callbackCommand); 

	LOG_INFO("Initialize..."); 
	frameGrabber->Connect();

  aSource->SetPortImageOrientation( US_IMG_ORIENT_MN );

  if (showDialogs)
  {
    frameGrabber->VideoFormatDialog();
    frameGrabber->VideoSourceDialog();
  }

	if ( frameGrabber->GetConnected() )
	{
		LOG_INFO("Start recording..."); 
		frameGrabber->StartRecording(); 
	}
	else
	{
		frameGrabber->Disconnect();
		LOG_ERROR( "Unable to connect to IC capture device"); 
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
	viewer->SetSize(640,480); 

  viewer->SetInput(vtkImageData::SafeDownCast(frameGrabber->GetOutputDataObject(0))); 

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
	iren->CreateTimer(VTKI_TIMER_FIRST);		//VTKI_TIMER_FIRST = 0

	//iren must be initialized so that it can handle events
	iren->Initialize();
	iren->Start();

	LOG_INFO("Exit successfully"); 
	return EXIT_SUCCESS; 

 }

 // Callback function for error and warning redirects
void PrintLogsCallback(vtkObject* obj, unsigned long eid, void* clientdata, void* calldata)
{
	if ( eid == vtkCommand::GetEventIdFromString("WarningEvent") )
	{
		LOG_WARNING((const char*)calldata);
	}
	else if ( eid == vtkCommand::GetEventIdFromString("ErrorEvent") )
	{
		LOG_ERROR((const char*)calldata);
	}
}
