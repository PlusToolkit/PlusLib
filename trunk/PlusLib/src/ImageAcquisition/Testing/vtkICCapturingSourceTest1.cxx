/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtksys/CommandLineArguments.hxx"
#include "vtkICCapturingSource.h"
#include "vtkImageData.h"
#include "vtkImageViewer2.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkSmartPointer.h"
#include "vtkCommand.h"
#include "vtkCallbackCommand.h"


vtkImageViewer2 *viewer = NULL;
vtkRenderWindowInteractor *iren = NULL;

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
};

int main(int argc, char **argv)
{

	bool printHelp(false); 
	bool renderingOff(false);
	std::string licenseKey("ISB3200016679");
	std::string deviceName("DFG/USB2-lt");
	std::string videoNorm("NTSC_M");
	std::string videoFormat("Y800 (640x480)");
	std::string inputChannel("01 Video: SVideo");

	vtksys::CommandLineArguments args;
	args.Initialize(argc, argv);

	int verboseLevel = vtkPlusLogger::LOG_LEVEL_ERROR;

	args.AddArgument("--help", vtksys::CommandLineArguments::NO_ARGUMENT, &printHelp, "Print this help.");	
	args.AddArgument("--rendering-off", vtksys::CommandLineArguments::NO_ARGUMENT, &renderingOff, "Run test without rendering.");	
	args.AddArgument("--license-key", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &licenseKey, "IC Capturing device license key." );
	args.AddArgument("--device-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &deviceName, "IC Capturing device name (Default: DFG/USB2-lt)." );
	args.AddArgument("--video-norm", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &videoNorm, "IC Capturing device video norm (Default: NTSC_M)." );
	args.AddArgument("--video-format", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &videoFormat, "IC Capturing device video format (Default: Y800 (640x480))." );
	args.AddArgument("--input-channel", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputChannel, "IC Capturing device input channel (Default: 01 Video: SVideo)." );
	args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (Default: 1; 1=error only, 2=warning, 3=info, 4=debug)");	

	if ( !args.Parse() )
	{
		std::cerr << "Problem parsing arguments" << std::endl;
		std::cout << "\n\nvtkICCapturingSourceTest1 help:" << args.GetHelp() << std::endl;
		exit(EXIT_FAILURE);
	}

	if ( printHelp ) 
	{
		std::cout << "\n\nvtkICCapturingSourceTest1 help:" << args.GetHelp() << std::endl;
		exit(EXIT_SUCCESS); 

	}

	vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);

	vtkICCapturingSource* frameGrabber = vtkICCapturingSource::New();

	frameGrabber->SetLicenceKey(licenseKey.c_str()); 
	frameGrabber->SetDeviceName(deviceName.c_str()); 
	frameGrabber->SetVideoNorm(videoNorm.c_str()); 
	frameGrabber->SetVideoFormat(videoFormat.c_str()); 
	frameGrabber->SetInputChannel(inputChannel.c_str()); 

	// Add an observer to warning and error events for redirecting it to the stdout 
	vtkCallbackCommand* callbackCommand = vtkCallbackCommand::New();
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
		if ( frameGrabber != NULL ) 
		{
			frameGrabber->Delete();
		}

		if ( viewer != NULL ) 
		{
			viewer->Delete();
		}

		if ( iren != NULL ) 
		{
			iren->Delete();
		}

		if ( callbackCommand != NULL ) 
		{
			callbackCommand->Delete(); 
		}

		LOG_ERROR( "Unable to connect to IC capture device: " << deviceName ); 
		exit(EXIT_FAILURE); 
	}


	if (renderingOff)
	{
		LOG_INFO("No need for rendering, stop the device..."); 
		frameGrabber->StopRecording(); 
		frameGrabber->Disconnect();

		if ( frameGrabber != NULL ) 
		{
			frameGrabber->Delete();
		}

		if ( viewer != NULL ) 
		{
			viewer->Delete();
		}

		if ( iren != NULL ) 
		{
			iren->Delete();
		}

		if ( callbackCommand != NULL ) 
		{
			callbackCommand->Delete(); 
		}

		LOG_INFO("Exit successfully"); 
		exit(EXIT_SUCCESS); 
	}

	viewer = vtkImageViewer2::New();

	viewer->SetColorWindow(255);
	viewer->SetColorLevel(100.5);
	viewer->SetSize(640,480); 

	viewer->SetInput(frameGrabber->GetOutput()); 

	//Create the interactor that handles the event loop
	iren = vtkRenderWindowInteractor::New();
	iren->SetRenderWindow(viewer->GetRenderWindow());
	viewer->SetupInteractor(iren);
	
	viewer->Render(); 

	//establish timer event and create timer
	vtkSmartPointer<vtkMyCallback> call = vtkSmartPointer<vtkMyCallback>::New();
	iren->AddObserver(vtkCommand::TimerEvent, call);
	iren->CreateTimer(VTKI_TIMER_FIRST);		//VTKI_TIMER_FIRST = 0

	//iren must be initialized so that it can handle events
	iren->Initialize();
	iren->Start();

	frameGrabber->Delete(); 
	viewer->Delete();
	iren->Delete();
	callbackCommand->Delete(); 

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