/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkBkProFocusCameraLinkVideoSource.h"
#include "vtkCallbackCommand.h"
#include "vtkCommand.h"
#include "vtkImageData.h"
#include "vtkImageViewer2.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkSmartPointer.h"
#include "vtksys/CommandLineArguments.hxx"
#include "vtkPlusDataSource.h"

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
  std::string iniFile="BkSettings/IniFile.ini";
  bool showBmode(false); 
  bool showSapera(false); 
  bool printHelp(false); 
  bool renderingOff(false);  

  vtksys::CommandLineArguments args;
  args.Initialize(argc, argv);

  vtkPlusConfig::GetInstance(); // set default log level
  int verboseLevel = vtkPlusLogger::LOG_LEVEL_UNDEFINED;

  args.AddArgument("--ini-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &iniFile, "Path to the BK ini file, relative to the configuration file directory. Default: BkSettings/IniFile.ini");
  args.AddArgument("--show-bmode", vtksys::CommandLineArguments::NO_ARGUMENT, &showBmode, "Show B-mode image debug window");	
  args.AddArgument("--show-sapera", vtksys::CommandLineArguments::NO_ARGUMENT, &showSapera, "Show Sapera framegrabber debug window");
  args.AddArgument("--help", vtksys::CommandLineArguments::NO_ARGUMENT, &printHelp, "Print this help.");	
  args.AddArgument("--rendering-off", vtksys::CommandLineArguments::NO_ARGUMENT, &renderingOff, "Run test without rendering the video source output to the screen.");	
  args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug, 5=trace)");	  

  if ( !args.Parse() )
  {
    std::cerr << "Problem parsing arguments" << std::endl;
    std::cout << "\n\nvtkBkProFocusCameraLinkVideoSourceTest help:" << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }

  vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);

  if ( printHelp ) 
  {
    std::cout << "\n\nvtkBkProFocusCameraLinkVideoSourceTest help:" << args.GetHelp() << std::endl;
    exit(EXIT_SUCCESS); 

  }

  vtkSmartPointer<vtkBkProFocusCameraLinkVideoSource> frameGrabber = vtkSmartPointer<vtkBkProFocusCameraLinkVideoSource>::New();

  frameGrabber->SetIniFileName(iniFile.c_str());

  frameGrabber->SetShowBModeWindow(showBmode);
  frameGrabber->SetShowSaperaWindow(showSapera);
  frameGrabber->SetImagingMode(vtkBkProFocusCameraLinkVideoSource::RfMode);

  frameGrabber->CreateDefaultOutputChannel();
  vtkPlusDataSource* videoSource=NULL;
  if (frameGrabber->GetFirstActiveOutputVideoSource(videoSource) != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to retrieve the video source.");
    exit(EXIT_FAILURE);
  }
  videoSource->SetPortImageOrientation(US_IMG_ORIENT_FM);

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
    LOG_ERROR( "Unable to connect to framegrabber"); 
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

	frameGrabber->StopRecording(); 
  frameGrabber->Disconnect();

  LOG_INFO("Exit successfully"); 
  return EXIT_SUCCESS; 
}
