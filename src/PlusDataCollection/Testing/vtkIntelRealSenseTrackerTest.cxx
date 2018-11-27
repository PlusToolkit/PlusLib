/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtksys/CommandLineArguments.hxx"
#include "vtkPlusIntelRealSense.h"
#include "vtkImageData.h"
#include "vtkImageViewer2.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkSmartPointer.h"
#include "vtkCommand.h"
#include "vtkCallbackCommand.h"

//void PrintLogsCallback(vtkObject* obj, unsigned long eid, void* clientdata, void* calldata); 
//
//class vtkMyCallback : public vtkCommand
//{
//public:
//  static vtkMyCallback *New()
//  {return new vtkMyCallback;}
//  virtual void Execute(vtkObject *caller, unsigned long, void*)
//  {
//    frameGrabber->GetImage(leftImage,NULL);
//    viewer->SetInputData(leftImage);
//    viewer->Render();
//
//    update the timer so it will trigger again
//    iren->CreateTimer(VTKI_TIMER_UPDATE);
//  }
//  vtkImageViewer2 *viewer;
//  vtkRenderWindowInteractor *iren;
//  vtkPlusIntelRealSenseTracker *frameGrabber;
//  vtkImageData *leftImage;
//protected:
//  vtkMyCallback()
//  {
//    viewer = NULL;
//    iren = NULL;
//  }
//};

int main(int argc, char **argv)
{
  //std::string serialNumber;
  //double frameRate=20;
  //bool printHelp(false); 
  //bool renderingOff(false);  

  //vtksys::CommandLineArguments args;
  //args.Initialize(argc, argv);

  //int verboseLevel = vtkIGSIOLogger::LOG_LEVEL_UNDEFINED;

  ////args.AddArgument("--serial-number", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &serialNumber, "Serial number of the Epiphan device to connect");
  //args.AddArgument("--frame-rate", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &frameRate, "Requested acquisition frame rate (in FPS, default = 30)");
  //args.AddArgument("--help", vtksys::CommandLineArguments::NO_ARGUMENT, &printHelp, "Print this help.");  
  //args.AddArgument("--rendering-off", vtksys::CommandLineArguments::NO_ARGUMENT, &renderingOff, "Run test without rendering.");  
  //args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug, 5=trace)");    

  //if ( !args.Parse() )
  //{
  //  std::cerr << "Problem parsing arguments" << std::endl;
  //  std::cout << "\n\nvtkPlusEpiphanVideoSourceTest help:" << args.GetHelp() << std::endl;
  //  exit(EXIT_FAILURE);
  //}

  //vtkIGSIOLogger::Instance()->SetLogLevel(verboseLevel);

  //if ( printHelp ) 
  //{
  //  std::cout << "\n\nvtkPlusIntelRealSenseTrackerTest help:" << args.GetHelp() << std::endl;
  //  exit(EXIT_SUCCESS);
  //}

  //vtkSmartPointer<vtkPlusIntelRealSenseTracker> frameGrabber = vtkSmartPointer<vtkPlusIntelRealSenseTracker>::New();

  //frameGrabber->SetAcquisitionRate(frameRate);

  //LOG_INFO("Initialize..."); 

  //if ( frameGrabber->Connect()!=PLUS_SUCCESS )
  //{
  //  LOG_ERROR( "Unable to connect to device"); 
  //  exit(EXIT_FAILURE); 
  //}

  //LOG_INFO("Start recording..."); 
  //if ( frameGrabber->StartRecording()!=PLUS_SUCCESS )
  //{
  //  LOG_ERROR( "Unable to connect to device"); 
  //  exit(EXIT_FAILURE); 
  //}

  //if (renderingOff)
  //{
  //  LOG_INFO("No need for rendering, stop the device..."); 
  //  frameGrabber->StopRecording(); 
  //  frameGrabber->Disconnect();
  //  LOG_INFO("Exit successfully"); 
  //  exit(EXIT_SUCCESS); 
  //}

  //vtkSmartPointer<vtkImageViewer2> viewer = vtkSmartPointer<vtkImageViewer2>::New();

  //viewer->SetColorWindow(255);
  //viewer->SetColorLevel(100.5);
  //viewer->SetSize(1024,768); 

  //vtkSmartPointer<vtkImageData> leftImage=vtkSmartPointer<vtkImageData>::New();
  //frameGrabber->GetImage(leftImage,NULL);
  //viewer->SetInputData(leftImage); 

  ////Create the interactor that handles the event loop
  //vtkSmartPointer<vtkRenderWindowInteractor> iren = vtkSmartPointer<vtkRenderWindowInteractor>::New();
  //iren->SetRenderWindow(viewer->GetRenderWindow());
  //viewer->SetupInteractor(iren);

  //viewer->Render(); 

  ////establish timer event and create timer
  //vtkSmartPointer<vtkMyCallback> call = vtkSmartPointer<vtkMyCallback>::New();
  //call->iren=iren;
  //call->viewer=viewer;
  //call->frameGrabber=frameGrabber;
  //call->leftImage=leftImage;
  //iren->AddObserver(vtkCommand::TimerEvent, call);
  //iren->CreateTimer(VTKI_TIMER_FIRST);    //VTKI_TIMER_FIRST = 0

  ////iren must be initialized so that it can handle events
  //iren->Initialize();
  //iren->Start();

  //LOG_INFO("Exit successfully"); 
  return EXIT_SUCCESS; 

}
