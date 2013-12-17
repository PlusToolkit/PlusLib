/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkCommand.h"
#include "vtkImageData.h"
#include "vtkImageViewer2.h"
#include "vtkMmfVideoSource.h"
#include "vtkPlusBuffer.h"
#include "vtkPlusChannel.h"
#include "vtkPlusDataSource.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkSmartPointer.h"
#include "vtkMmfVideoSource.h"
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
  int width(640);
  int height(480);

  vtksys::CommandLineArguments args;
  args.Initialize(argc, argv);

  int verboseLevel = vtkPlusLogger::LOG_LEVEL_UNDEFINED;

  args.AddArgument("--help", vtksys::CommandLineArguments::NO_ARGUMENT, &printHelp, "Print this help.");  
  args.AddArgument("--rendering-off", vtksys::CommandLineArguments::NO_ARGUMENT, &renderingOff, "Run test without rendering.");  
  args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug, 5=trace)");  
  args.AddArgument("--width", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &width, "X resolution for the capture device.");
  args.AddArgument("--height", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &height, "Y resolution for the capture device.");

  if ( !args.Parse() )
  {
    std::cerr << "Problem parsing arguments" << std::endl;
    std::cout << "\n\nvtkMmfVideoSourceTest help:" << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }

  vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);

  if ( printHelp ) 
  {
    std::cout << "\n\nvtkMmfVideoSourceTest help:" << args.GetHelp() << std::endl;
    exit(EXIT_SUCCESS); 

  }

  vtkSmartPointer<vtkMmfVideoSource> frameGrabber = vtkSmartPointer<vtkMmfVideoSource>::New();
  frameGrabber->TestCreateDefaultVideoSource();
  vtkPlusChannel* aChannel(NULL);
  vtkPlusDataSource* aSource(NULL);
  if( frameGrabber->GetOutputChannelByName(aChannel, "DefaultChannel") != PLUS_SUCCESS || aChannel->GetVideoSource(aSource) != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to retrieve the video source.");
    return NULL;
  }

  LOG_INFO("Initialize..."); 
  frameGrabber->Connect();

  aSource->SetPortImageOrientation( US_IMG_ORIENT_MN );

  if ( frameGrabber->GetConnected() )
  {
    LOG_INFO("Start recording..."); 
    frameGrabber->StartRecording(); 
  }
  else
  {
    frameGrabber->Disconnect();
    LOG_ERROR( "Unable to connect to media foundation capture device."); 
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
  viewer->SetSize(aSource->GetBuffer()->GetFrameSize()[0], aSource->GetBuffer()->GetFrameSize()[1]); 

  vtkImageData* output = vtkImageData::SafeDownCast(frameGrabber->GetOutput());
  viewer->SetInput(vtkImageData::SafeDownCast(frameGrabber->GetOutput())); 

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
  iren->CreateTimer(VTKI_TIMER_FIRST);    //VTKI_TIMER_FIRST = 0

  //iren must be initialized so that it can handle events
  iren->Initialize();
  iren->Start();

  LOG_INFO("Exit successfully"); 
  return EXIT_SUCCESS; 

}