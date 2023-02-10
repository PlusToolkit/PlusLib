/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.

  Developed by ULL & IACTEC group  
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkCallbackCommand.h"
#include "vtkCommand.h"
#include "vtkImageData.h"
#include "vtkImageViewer2.h"
#include "vtkPlusDataSource.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkSmartPointer.h"
#include "vtkPlusUltravioletPCOUVCam.h"
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

  vtksys::CommandLineArguments args;
  args.Initialize(argc, argv);

  int verboseLevel = vtkPlusLogger::LOG_LEVEL_UNDEFINED;

  args.AddArgument("--help", vtksys::CommandLineArguments::NO_ARGUMENT, &printHelp, "Print this help.");  
  args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug, 5=trace)");  

  if ( !args.Parse() )
  {
    std::cerr << "Problem parsing arguments" << std::endl;
    std::cout << "\n\nvtkUltravioletPCOUVCamTest help:" << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }

  vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);
  
  if ( printHelp ) 
  {
    std::cout << "\n\nvtkUltravioletPCOUVCamTest help:" << args.GetHelp() << std::endl;
    exit(EXIT_SUCCESS); 
  }

  vtkSmartPointer<vtkPlusUltravioletPCOUVCam> UltravioletPCOUVCam = vtkSmartPointer<vtkPlusUltravioletPCOUVCam>::New();

  UltravioletPCOUVCam->CreateDefaultOutputChannel();

// Add an observer to warning and error events for redirecting it to the stdout 
  vtkSmartPointer<vtkCallbackCommand> callbackCommand = vtkSmartPointer<vtkCallbackCommand>::New();
  callbackCommand->SetCallback(PrintLogsCallback);
  UltravioletPCOUVCam->AddObserver("WarningEvent", callbackCommand); 
  UltravioletPCOUVCam->AddObserver("ErrorEvent", callbackCommand); 
  
  LOG_INFO("Initialize..."); 
  UltravioletPCOUVCam->Connect();

  if ( UltravioletPCOUVCam->GetConnected() )
  {
    LOG_INFO("Start recording..."); 
    UltravioletPCOUVCam->StartRecording(); 
  }
  else
  {
    UltravioletPCOUVCam->Disconnect();
    LOG_ERROR( "Unable to connect to PCO Ultraviolet device"); 
    exit(EXIT_FAILURE); 
  }

  LOG_INFO("Stop recording...");
  UltravioletPCOUVCam->StopRecording(); 
  UltravioletPCOUVCam->Disconnect();
  LOG_INFO("Exit successfully"); 
  exit(EXIT_SUCCESS); 
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
