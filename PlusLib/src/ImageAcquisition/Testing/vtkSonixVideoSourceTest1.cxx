// Test basic connection to the Ultrasonix image acquisition
// If the --rendering-off switch is defined then the connection is established, images are 
// transferred for a few seconds, then the connection is closed (useful for automatic testing).
// If the --rendering-off switch is not defined then the live ultrasound image is displayed
// in a window (useful for quick interactive testing of the image transfer).

#include "PlusConfigure.h"
#include "vtksys/CommandLineArguments.hxx"
#include <stdlib.h>
#include "vtkRenderWindowInteractor.h"
#include "vtkSonixVideoSource.h"
#include "vtkVideoBuffer.h"
#include "vtkImageViewer.h"
#include "vtkCallbackCommand.h"
#include "vtkCommand.h"
#include "vtkSmartPointer.h"

class vtkMyCallback : public vtkCommand
{
public:
  static vtkMyCallback *New()	{ return new vtkMyCallback; }

  virtual void Execute(vtkObject *caller, unsigned long, void*)
  {
    m_Viewer->Render();

    //update the timer so it will trigger again
    //VTKI_TIMER_UPDATE = 1
    m_Interactor->CreateTimer(VTKI_TIMER_UPDATE);
  }

  vtkRenderWindowInteractor *m_Interactor;
  vtkImageViewer *m_Viewer;

private:

  vtkMyCallback()
  { 
    m_Interactor=NULL;
    m_Viewer=NULL;
  }
};

int main(int argc, char* argv[])
{

  bool printHelp(false); 
  bool renderingOff(false);
  std::string inputSonixIP("130.15.7.212");

  vtksys::CommandLineArguments args;
  args.Initialize(argc, argv);

  int verboseLevel = PlusLogger::LOG_LEVEL_INFO;

  args.AddArgument("--help", vtksys::CommandLineArguments::NO_ARGUMENT, &printHelp, "Print this help.");	
  args.AddArgument("--sonix-ip", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputSonixIP, "SonixRP ip address (Default: 130.15.7.212)" );
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

  PlusLogger::Instance()->SetLogLevel(verboseLevel);

  vtkSmartPointer<vtkSonixVideoSource> sonixGrabber = vtkSmartPointer<vtkSonixVideoSource>::New();

  sonixGrabber->SetSonixIP(inputSonixIP.c_str());
  sonixGrabber->SetUsImageOrientation( US_IMG_ORIENT_UF ); // just randomly set an orientation (otherwise we would get an error that the orientation is undefined)
  sonixGrabber->SetImagingMode(0);
  sonixGrabber->SetAcquisitionDataType(udtBPost);
  if ( sonixGrabber->GetBuffer()->SetBufferSize(30) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to set video buffer size!"); 
    exit(EXIT_FAILURE);
  }

  if ( sonixGrabber->Initialize()!=PLUS_SUCCESS ) 
  {
    sonixGrabber->ReleaseSystemResources();
    LOG_ERROR( "Unable to connect to Sonix RP machine at: " << inputSonixIP ); 
    exit(EXIT_FAILURE); 
  }

  sonixGrabber->StartRecording();				//start recording frame from the video

  if (renderingOff)
  {
    // just run the recording for  a few seconds then exit
    Sleep(5000); // no need to use accurate timer, it's just an approximate delay
    sonixGrabber->StopRecording(); 
    sonixGrabber->ReleaseSystemResources();
    exit(EXIT_SUCCESS);
  }

  // Show the live ultrasound image in a VTK renderer window

  vtkSmartPointer<vtkImageViewer> viewer = vtkSmartPointer<vtkImageViewer>::New();
  viewer->SetInput(sonixGrabber->GetOutput());   //set image to the render and window
  viewer->SetColorWindow(255);
  viewer->SetColorLevel(127.5);
  viewer->SetZSlice(0);

  //Create the interactor that handles the event loop
  vtkSmartPointer<vtkRenderWindowInteractor> iren = vtkSmartPointer<vtkRenderWindowInteractor>::New();
  iren->SetRenderWindow(viewer->GetRenderWindow());
  viewer->SetupInteractor(iren);

  viewer->Render();	//must be called after iren and viewer are linked or there will be problems

  // Establish timer event and create timer to update the live image
  vtkSmartPointer<vtkMyCallback> call = vtkSmartPointer<vtkMyCallback>::New();
  call->m_Interactor=iren;
  call->m_Viewer=viewer;
  iren->AddObserver(vtkCommand::TimerEvent, call);
  iren->CreateTimer(VTKI_TIMER_FIRST);		//VTKI_TIMER_FIRST = 0

  //iren must be initialized so that it can handle events
  iren->Initialize();
  iren->Start();

  sonixGrabber->ReleaseSystemResources();

  return EXIT_SUCCESS;
}


