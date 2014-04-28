/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

/*!
  \file vtkSonixVideoSourceTest1.cxx
  \brief Test basic connection to the Ultrasonix image acquisition

  If the --rendering-off switch is defined then the connection is established, images are 
  transferred for a few seconds, then the connection is closed (useful for automatic testing).
  If the --rendering-off switch is not defined then the live ultrasound image is displayed
  in a window (useful for quick interactive testing of the image transfer).
  \todo This is a test todo
  \ingroup PlusLibDataCollection
*/

#include "PlusConfigure.h"
#include "vtkCallbackCommand.h"
#include "vtkChartXY.h"
#include "vtkCommand.h"
#include "vtkContextScene.h"
#include "vtkContextView.h"
#include "vtkFloatArray.h"
#include "vtkImageData.h"
#include "vtkImageViewer.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkPlot.h"
#include "vtkPlusBuffer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkSmartPointer.h"
#include "vtkSonixVideoSource.h"
#include "vtkTable.h"
#include "vtkTableAlgorithm.h"
#include "vtkXMLUtilities.h"
#include "vtksys/CommandLineArguments.hxx"
#include <stdlib.h>

//----------------------------------------------------------------------------

enum DisplayMode
{
  SHOW_IMAGE,
  SHOW_PLOT
};

//----------------------------------------------------------------------------

class vtkExtractImageRow : public vtkTableAlgorithm
{
public:
  static vtkExtractImageRow* New();
  vtkTypeMacro(vtkExtractImageRow,vtkTableAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent)
  {
    this->Superclass::PrintSelf(os, indent);
  }

  // Description:
  // Specify the first vtkGraph input and the second vtkSelection input.
  int FillInputPortInformation(int port, vtkInformation* info)
  {
    if (port == 0)
    {
      info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkImageData");
      return 1;
    }
    return 0;
  }

protected:
  vtkExtractImageRow()
  {
    this->SetNumberOfInputPorts(1);
  }
  virtual ~vtkExtractImageRow()
  {
  }

  int RequestData(vtkInformation* vtkNotUsed(request), vtkInformationVector** inputVector, vtkInformationVector* outputVector)
  {
    vtkImageData* inputImage = vtkImageData::GetData(inputVector[0]);
    vtkInformation* outInfo = outputVector->GetInformationObject(0);
    vtkTable* outputTable = vtkTable::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

    if(!inputImage)
    {
      LOG_ERROR("No input image is available");
      return 0;
    }

    // Create the tables if they haven't been created yet
    if (outputTable->GetColumnByName("time")==NULL)
    {
      vtkSmartPointer<vtkFloatArray> arrXnew = vtkSmartPointer<vtkFloatArray>::New();
      arrXnew->SetName("time");
      outputTable->AddColumn(arrXnew);
    }
    if (outputTable->GetColumnByName("RF value")==NULL)
    {
      vtkSmartPointer<vtkFloatArray> arrRfValNew = vtkSmartPointer<vtkFloatArray>::New();
      arrRfValNew->SetName("RF value");
      outputTable->AddColumn(arrRfValNew);
    }

    if (inputImage->GetScalarType()!=VTK_SHORT)
    {
      LOG_ERROR("Plotting is only supported for signed short data");
      return 0;
    }
    int rowCount=inputImage->GetDimensions()[1]; // number of transducer crystals
    int numPoints=inputImage->GetDimensions()[0]; // number of data points (RF data values) recorded for one crystal
    int selectedRow=rowCount/2; // plot the center column of the image
    short* pixelBuffer=reinterpret_cast<short*>(inputImage->GetScalarPointer())+selectedRow*numPoints;

    outputTable->SetNumberOfRows(numPoints);
    int timeIndex=numPoints-1; // the RF data set starts with the latest time
    for (int i = 0; i < numPoints; ++i)
    {
      outputTable->SetValue(i, 0, timeIndex);
      short value=*pixelBuffer;
      outputTable->SetValue(i, 1, value);
      pixelBuffer++;
      timeIndex--;
    }

    return 1;
  } 

private:
  vtkExtractImageRow(const vtkExtractImageRow&); // Not implemented
  void operator=(const vtkExtractImageRow&);   // Not implemented
};

vtkStandardNewMacro(vtkExtractImageRow);

//---------------------------------------------------------------------------------

class vtkMyPlotCallback : public vtkCommand
{
public:
  static vtkMyPlotCallback *New()	{ return new vtkMyPlotCallback; }

  virtual void Execute(vtkObject *caller, unsigned long eventId, void* callData)
  {   
    if (eventId==vtkCommand::KeyPressEvent)
    {
      if (m_Interactor->GetKeyCode()=='q')
      {
        m_Interactor->ExitCallback();
      }
      return;
    }

    m_ImageToTableAdaptor->Update();
    m_Viewer->Render();
    //update the timer so it will trigger again
    m_Interactor->CreateTimer(VTKI_TIMER_UPDATE);
  }

  vtkRenderWindowInteractor *m_Interactor;
  vtkContextView *m_Viewer;
  vtkExtractImageRow *m_ImageToTableAdaptor;

private:

  vtkMyPlotCallback()
  { 
    m_Interactor=NULL;
    m_Viewer=NULL;
    m_ImageToTableAdaptor=NULL;
  }
};

//----------------------------------------------------------------------------
void TestLinePlot(vtkSonixVideoSource *sonixGrabber)
{
  // Set up a 2D scene, add an XY chart to it
  vtkSmartPointer<vtkContextView> view = vtkSmartPointer<vtkContextView>::New();
  view->GetRenderer()->SetBackground(1.0, 1.0, 1.0);
  view->GetRenderWindow()->SetSize(400, 300);
  vtkSmartPointer<vtkChartXY> chart = vtkSmartPointer<vtkChartXY>::New();
  view->GetScene()->AddItem(chart);

  vtkSmartPointer<vtkExtractImageRow> imageToTableAdaptor=vtkSmartPointer<vtkExtractImageRow>::New();
  imageToTableAdaptor->SetInputConnection(sonixGrabber->GetOutputPort());
  imageToTableAdaptor->Update();

  // Add multiple line plots, setting the colors etc
  vtkPlot *line = chart->AddPlot(vtkChart::LINE);
  line->SetInputData_vtk5compatible(imageToTableAdaptor->GetOutput(), 0, 1);
  line->SetColor(0, 255, 0, 255);
  line->SetWidth(1.0);

  vtkSmartPointer<vtkMyPlotCallback> call = vtkSmartPointer<vtkMyPlotCallback>::New();
  call->m_Interactor=view->GetInteractor();
  call->m_Viewer=view;
  call->m_ImageToTableAdaptor=imageToTableAdaptor;

  view->GetInteractor()->Initialize();

  view->GetInteractor()->AddObserver(vtkCommand::TimerEvent, call);
  view->GetInteractor()->CreateTimer(VTKI_TIMER_FIRST);

  view->GetInteractor()->AddObserver(vtkCommand::KeyPressEvent, call);

  view->GetInteractor()->Start();

} 

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
  std::string inputConfigFile;
  std::string inputSonixIp;

  std::string acqMode("B");

  vtksys::CommandLineArguments args;
  args.Initialize(argc, argv);

  int verboseLevel = vtkPlusLogger::LOG_LEVEL_UNDEFINED;

  args.AddArgument("--help", vtksys::CommandLineArguments::NO_ARGUMENT, &printHelp, "Print this help.");	
  args.AddArgument("--config-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputConfigFile, "Config file containing the device configuration.");
  args.AddArgument("--acq-mode", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &acqMode, "Acquisition mode: B or RF (Default: B).");	
  args.AddArgument("--sonix-ip", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputSonixIp, "IP address of the Ultrasonix scanner (overrides the IP address parameter defined in the config file).");
  args.AddArgument("--rendering-off", vtksys::CommandLineArguments::NO_ARGUMENT, &renderingOff, "Run test without rendering.");	
  args.AddArgument("--print-params", vtksys::CommandLineArguments::NO_ARGUMENT, &printParams, "Print all the supported imaging parameters (for diagnostic purposes only).");	
  args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level 1=error only, 2=warning, 3=info, 4=debug, 5=trace)");	

  if ( !args.Parse() )
  {
    std::cerr << "Problem parsing arguments" << std::endl;
    std::cout << "\n\nvtkSonixVideoSourceTest1 help:" << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }
  
  vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);

  if ( printHelp ) 
  {
    std::cout << "\n\nvtkSonixVideoSourceTest1 help:" << args.GetHelp() << std::endl;
    exit(EXIT_SUCCESS); 
  }

  // Read config file
  LOG_DEBUG("Reading config file...");
  vtkSmartPointer<vtkXMLDataElement> configRead = vtkSmartPointer<vtkXMLDataElement>::Take(::vtkXMLUtilities::ReadElementFromFile(inputConfigFile.c_str())); 
  LOG_DEBUG("Reading config file finished.");

  vtkSmartPointer<vtkSonixVideoSource> sonixGrabber = vtkSmartPointer<vtkSonixVideoSource>::New();
  sonixGrabber->SetDeviceId("VideoDevice");
  sonixGrabber->ReadConfiguration(configRead);

  DisplayMode displayMode=SHOW_IMAGE;
  
  if (STRCASECMP(acqMode.c_str(), "B")==0)
  {
    LOG_DEBUG("Acquisition mode: B");
    sonixGrabber->SetImagingMode(BMode);
    sonixGrabber->SetAcquisitionDataType(udtBPost);
    displayMode=SHOW_IMAGE;
  }
  else if (STRCASECMP(acqMode.c_str(), "RF")==0)
  {
    LOG_DEBUG("Acquisition mode: RF");
#if (PLUS_ULTRASONIX_SDK_MAJOR_VERSION < 6) 
    sonixGrabber->SetImagingMode(RfMode);
    sonixGrabber->SetAcquisitionDataType(udtRF);
    displayMode=SHOW_PLOT;
#else
    LOG_ERROR("RF acquisition mode is not supported on Ultrasonix SDK 6.x and above"); // see https://www.assembla.com/spaces/plus/tickets/489-add-rf-image-acquisition-support-on-ulterius-6-x
#endif
  }
  else
  {
    LOG_ERROR("Unsupported AcquisitionDataType requested: "<<acqMode);
    exit(EXIT_FAILURE);
  }

  if (!inputSonixIp.empty())
  {
    sonixGrabber->SetSonixIP(inputSonixIp.c_str());
  }

  if ( sonixGrabber->Connect()!=PLUS_SUCCESS ) 
  {
    LOG_ERROR( "Unable to connect to Sonix RP machine at: " << sonixGrabber->GetSonixIP() ); 
    exit(EXIT_FAILURE); 
  }

  if (printParams)
  {
    LOG_INFO("List of supported imaging parameters:");
    sonixGrabber->PrintListOfImagingParameters();
  }

  sonixGrabber->StartRecording();				//start recording frame from the video

  if (renderingOff)
  {
    // just run the recording for  a few seconds then exit
    LOG_DEBUG("Rendering disabled. Wait for just a few seconds to acquire data before exiting");
    Sleep(5000); // no need to use accurate timer, it's just an approximate delay
    sonixGrabber->StopRecording(); 
    sonixGrabber->Disconnect();
  }
  else
  {
    if (displayMode==SHOW_PLOT)
    {
      TestLinePlot(sonixGrabber);
    }
    else
    {
      // Show the live ultrasound image in a VTK renderer window

      vtkSmartPointer<vtkImageViewer> viewer = vtkSmartPointer<vtkImageViewer>::New();
      viewer->SetInputConnection(sonixGrabber->GetOutputPort());   //set image to the render and window
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

  sonixGrabber->Disconnect();
  return EXIT_SUCCESS;
}

