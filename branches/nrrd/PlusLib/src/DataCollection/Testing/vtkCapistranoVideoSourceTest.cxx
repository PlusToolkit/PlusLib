/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

/*!
  \file vtkCapistranoVideoSourceTest.cxx
  \brief Test basic connection to the Capistrano USB ultrasound probe

  If the --rendering-off switch is defined then the connection is established, images are 
  transferred for a few seconds, then the connection is closed (useful for automatic testing).
  If the --rendering-off switch is not defined then the live ultrasound image is displayed
  in a window (useful for quick interactive testing of the image transfer).
  \ingroup PlusLibDataCollection
*/

#include "PlusConfigure.h"
#include "vtkCallbackCommand.h"
#include "vtkCommand.h"
#include "vtkImageData.h"
#include "vtkImageViewer.h"
#include "vtkInformationVector.h"
#include "vtkPlot.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkSmartPointer.h"
#include "vtkCapistranoVideoSource.h"
#include "vtkXMLUtilities.h"
#include "vtksys/CommandLineArguments.hxx"
#include <stdlib.h>


//---------------------------------------------------------------------------------

class vtkMyCallback : public vtkCommand
{
public:
  static vtkMyCallback *New()	{ return new vtkMyCallback; }

  virtual void Execute(vtkObject *caller, unsigned long, void*)
  {
    m_Viewer->Render();

    //update the timer so it will trigger again
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

//-------------------------------------------------------------------------------------------

int main(int argc, char* argv[])
{
  bool printHelp(false); 
  bool renderingOff(false);
  bool printParams(false);
  std::string inputConfigFileName;
  double frequencyMhz = -1;

  vtksys::CommandLineArguments args;
  args.Initialize(argc, argv);

  int verboseLevel = vtkPlusLogger::LOG_LEVEL_UNDEFINED;

  args.AddArgument("--help", vtksys::CommandLineArguments::NO_ARGUMENT, &printHelp, "Print this help.");	
  args.AddArgument("--config-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputConfigFileName, "Config file containing the device configuration.");
  args.AddArgument("--frequencyMhz", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &frequencyMhz, "Frequency in MHz");	
  args.AddArgument("--rendering-off", vtksys::CommandLineArguments::NO_ARGUMENT, &renderingOff, "Run test without rendering.");	
  args.AddArgument("--print-params", vtksys::CommandLineArguments::NO_ARGUMENT, &printParams, "Print all the supported imaging parameters (for diagnostic purposes only).");	
  args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level 1=error only, 2=warning, 3=info, 4=debug, 5=trace)");	

  if ( !args.Parse() )
  {
    std::cerr << "Problem parsing arguments" << std::endl;
    std::cout << "\n\nvtkCapistranoVideoSourceTest1 help:" << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }
  
  vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);

  if ( printHelp ) 
  {
    std::cout << "\n\nvtkCapistranoVideoSourceTest help:" << args.GetHelp() << std::endl;
    exit(EXIT_SUCCESS); 
  }

  vtkSmartPointer< vtkCapistranoVideoSource > capistranoDevice =
    vtkSmartPointer< vtkCapistranoVideoSource >::New();
  capistranoDevice->SetDeviceId("VideoDevice");

  // Read config file
  if (STRCASECMP(inputConfigFileName.c_str(), "")!=0)
  {
    LOG_DEBUG("Reading config file...");
    vtkSmartPointer<vtkXMLDataElement> configRootElement = vtkSmartPointer<vtkXMLDataElement>::New();
    if (PlusXmlUtils::ReadDeviceSetConfigurationFromFile(configRootElement, inputConfigFileName.c_str())==PLUS_FAIL)
    {  
      LOG_ERROR("Unable to read configuration from file " << inputConfigFileName.c_str()); 
      return EXIT_FAILURE;
    }
    capistranoDevice->ReadConfiguration(configRootElement);
  }
  
  LOG_INFO("SDK version: " << capistranoDevice->GetSdkVersion());

  if ( capistranoDevice->Connect() != PLUS_SUCCESS ) 
  {
    LOG_ERROR( "Unable to connect to Capistrano Labs probe" ); 
    exit(EXIT_FAILURE); 
  }

/*
  if (printParams)
  {
    LOG_INFO("List of supported imaging parameters:");
    //capistranoDevice->PrintListOfImagingParameters();
  }

  capistranoDevice->StartRecording();				//start recording frame from the video

  if (renderingOff)
  {
    // just run the recording for  a few seconds then exit
    LOG_DEBUG("Rendering disabled. Wait for just a few seconds to acquire data before exiting");
    Sleep(5000); // no need to use accurate timer, it's just an approximate delay
    capistranoDevice->StopRecording(); 
    capistranoDevice->Disconnect();
  }
  else
  {
    if (displayMode==SHOW_PLOT)
    {
      TestLinePlot(capistranoDevice);
    }
    else
    {
      // Show the live ultrasound image in a VTK renderer window

      vtkSmartPointer<vtkImageViewer> viewer = vtkSmartPointer<vtkImageViewer>::New();
      viewer->SetInputConnection(capistranoDevice->GetOutputPort());   //set image to the render and window
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
      iren->CreateTimer(VTKI_TIMER_FIRST);

      //iren must be initialized so that it can handle events
      iren->Initialize();
      iren->Start();
    }
  }

  capistranoDevice->Disconnect();
  */
  return EXIT_SUCCESS;
}

