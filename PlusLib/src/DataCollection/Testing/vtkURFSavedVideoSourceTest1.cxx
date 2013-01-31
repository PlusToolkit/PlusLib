/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkCallbackCommand.h"
#include "vtkCommand.h"
#include "vtkImageData.h"
#include "vtkImageViewer.h"
#include "vtkPlusChannel.h"
#include "vtkPlusStreamBuffer.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkSmartPointer.h"
#include "vtkSonixVideoSource.h"
#include "vtkXMLUtilities.h"
#include "vtksys/CommandLineArguments.hxx"
#include <stdlib.h>

vtkSmartPointer<vtkSonixVideoSource> sonixGrabber;
vtkImageViewer *viewer = NULL;
vtkRenderWindowInteractor *iren = NULL;

class vtkMyCallback : public vtkCommand
{
public:
  static vtkMyCallback *New()
  {return new vtkMyCallback;}
  virtual void Execute(vtkObject *caller, unsigned long, void*)
  {
    viewer->Render();

    //update the timer so it will trigger again
    //VTKI_TIMER_UPDATE = 1
    iren->CreateTimer(VTKI_TIMER_UPDATE);
  }
};

int main(int argc, char* argv[])
{

  bool printHelp(false); 
  bool renderingOff(false);
  std::string inputConfigFile;
  std::string inputSonixIP("137.82.56.185");

  vtksys::CommandLineArguments args;
  args.Initialize(argc, argv);

  int verboseLevel = vtkPlusLogger::LOG_LEVEL_INFO;

  args.AddArgument("--help", vtksys::CommandLineArguments::NO_ARGUMENT, &printHelp, "Print this help.");	
  args.AddArgument("--sonix-ip", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputSonixIP, "SonixRP ip address (Default: 137.82.56.185)" );
  args.AddArgument("--config-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputConfigFile, "Config file containing the device configuration.");
  args.AddArgument("--rendering-off", vtksys::CommandLineArguments::NO_ARGUMENT, &renderingOff, "Run test without rendering.");	
  args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (Default: 1; 1=error only, 2=warning, 3=info, 4=debug)");	

  if ( !args.Parse() )
  {
    std::cerr << "Problem parsing arguments" << std::endl;
    std::cout << "\n\nvtkSonixVideoSourceTest1 help:" << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }

  if ( printHelp ) 
  {
    std::cout << "\n\nvtkSonixVideoSourceTest1 help:" << args.GetHelp() << std::endl;
    exit(EXIT_SUCCESS); 

  }

  vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);

  LOG_DEBUG("Reading config file...");
  vtkSmartPointer<vtkXMLDataElement> configRead = vtkSmartPointer<vtkXMLDataElement>::Take(::vtkXMLUtilities::ReadElementFromFile(inputConfigFile.c_str())); 
  LOG_DEBUG("Reading config file finished.");

  //Add the video source here
  sonixGrabber = vtkSmartPointer<vtkSonixVideoSource>::New();
  sonixGrabber->SetSonixIP(inputSonixIP.c_str());
  sonixGrabber->SetImagingMode(0);
  sonixGrabber->SetAcquisitionDataType(0x00000005);
  sonixGrabber->ReadConfiguration(configRead);

  if ( sonixGrabber->GetBuffer()->SetBufferSize(30) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to set video buffer size!"); 
    exit(EXIT_FAILURE);
  }

  sonixGrabber->Connect(); 

  if ( sonixGrabber->GetConnected() ) 
  {
    sonixGrabber->StartRecording();				//start recording frame from the video
  } 
  else 
  {
    sonixGrabber->Disconnect();
    if ( sonixGrabber != NULL ) 
    {
      sonixGrabber->Delete();
    }

    if ( viewer != NULL ) 
    {
      viewer->Delete();
    }

    if ( iren != NULL ) 
    {
      iren->Delete();
    }

    LOG_ERROR( "Unable to connect to Sonix RP machine at: " << inputSonixIP ); 
    exit(EXIT_FAILURE); 
  }

  if (renderingOff)
  {
    sonixGrabber->StopRecording(); 
    sonixGrabber->Disconnect();

    if ( sonixGrabber != NULL ) 
    {
      sonixGrabber->Delete();
    }

    if ( viewer != NULL ) 
    {
      viewer->Delete();
    }

    if ( iren != NULL ) 
    {
      iren->Delete();
    }

    exit(EXIT_SUCCESS); 
  }

  viewer = vtkImageViewer::New();
  viewer->SetInput(sonixGrabber->GetOutput());
  viewer->SetColorWindow(255);
  viewer->SetColorLevel(127.5);
  viewer->SetZSlice(0);

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

  //delete all instances and release the hold the win32videosource
  //has on the pci card	
  sonixGrabber->Disconnect();
  sonixGrabber->Delete();

  call->Delete(); 
  viewer->Delete();
  iren->Delete();

  return 0;
}


