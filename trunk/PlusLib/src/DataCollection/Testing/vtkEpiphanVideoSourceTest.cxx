/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkCallbackCommand.h"
#include "vtkCommand.h"
#include "vtkEpiphanVideoSource.h"
#include "vtkImageData.h"
#include "vtkImageViewer2.h"
#include "vtkPlusChannel.h"
#include "vtkPlusDataSource.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkSmartPointer.h"
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
  std::string grabberLocation;
  double frameRate=30;
  std::vector<int> clipRectOrigin;
  std::vector<int> clipRectSize;
	bool printHelp(false); 
	bool renderingOff(false);  

	vtksys::CommandLineArguments args;
	args.Initialize(argc, argv);

	int verboseLevel = vtkPlusLogger::LOG_LEVEL_UNDEFINED;

  args.AddArgument("--grabber-location", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &grabberLocation, "Location of the of the Epiphan device to connect (use the format sn:SERIAL for the specification of serial number)");
  args.AddArgument("--frame-rate", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &frameRate, "Requested acquisition frame rate (in FPS, default = 30)");
  args.AddArgument("--clip-rect-origin", vtksys::CommandLineArguments::MULTI_ARGUMENT, &clipRectOrigin, "Origin of the clipping rectangle");
  args.AddArgument("--clip-rect-size", vtksys::CommandLineArguments::MULTI_ARGUMENT, &clipRectSize, "Size of the clipping rectangle");
	args.AddArgument("--help", vtksys::CommandLineArguments::NO_ARGUMENT, &printHelp, "Print this help.");	
	args.AddArgument("--rendering-off", vtksys::CommandLineArguments::NO_ARGUMENT, &renderingOff, "Run test without rendering.");	
	args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug, 5=trace)");	  

	if ( !args.Parse() )
	{
		std::cerr << "Problem parsing arguments" << std::endl;
		std::cout << "\n\nvtkEpiphanVideoSourceTest help:" << args.GetHelp() << std::endl;
		exit(EXIT_FAILURE);
	}

	vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);

	if ( printHelp ) 
	{
		std::cout << "\n\nvtkEpiphanVideoSourceTest help:" << args.GetHelp() << std::endl;
		exit(EXIT_SUCCESS); 

	}

	vtkSmartPointer<vtkEpiphanVideoSource> frameGrabber = vtkSmartPointer<vtkEpiphanVideoSource>::New();

  if (!grabberLocation.empty())
  {
    frameGrabber->SetGrabberLocation(grabberLocation.c_str());
  }
  
  frameGrabber->SetAcquisitionRate(frameRate);

  if (clipRectOrigin.size()>0 || clipRectSize.size()>0)
  {
    if (clipRectOrigin.size()!=2 || clipRectSize.size()!=2)
    {
      LOG_ERROR("Invalid clip rectangle origin and/or size");
      exit(EXIT_FAILURE);
    }
    frameGrabber->SetClipRectangleOrigin(clipRectOrigin[0],clipRectOrigin[1]);
    frameGrabber->SetClipRectangleSize(clipRectSize[0],clipRectSize[1]);
  }
 
  vtkPlusChannel* aChannel(NULL);
  vtkPlusDataSource* aSource(NULL);
  if( frameGrabber->GetOutputChannelByName(aChannel, "VideoStream") != PLUS_SUCCESS || aChannel->GetVideoSource(aSource) != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to retrieve the video source.");
    return NULL;
  }
  aSource->SetPortImageOrientation( US_IMG_ORIENT_MN );

	// Add an observer to warning and error events for redirecting it to the stdout 
	vtkSmartPointer<vtkCallbackCommand> callbackCommand = vtkSmartPointer<vtkCallbackCommand>::New();
	callbackCommand->SetCallback(PrintLogsCallback);
	frameGrabber->AddObserver("WarningEvent", callbackCommand); 
	frameGrabber->AddObserver("ErrorEvent", callbackCommand); 

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
	viewer->SetSize(1024,768); 

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
