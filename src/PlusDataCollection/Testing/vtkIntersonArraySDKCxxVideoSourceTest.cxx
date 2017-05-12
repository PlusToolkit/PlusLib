/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

/*!
  \file vtkIntersonArraySDKCxxVideoSourceTest.cxx
  \brief Test basic connection to the Interson Array SDKCxx USB ultrasound probe

  If the --rendering-off switch is defined then the connection is established, images are 
  transferred for a few seconds, then the connection is closed (useful for automatic testing).
  If the --rendering-off switch is not defined then the live ultrasound image is displayed
  in a window (useful for quick interactive testing of the image transfer).
  \ingroup PlusLibDataCollection
*/

#include "PlusConfigure.h"
#include "vtkAxis.h"
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
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkSmartPointer.h"
#include "vtkPlusIntersonArraySDKCxxVideoSource.h"
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

    if (inputImage->GetScalarType() != VTK_SHORT)
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
void TestLinePlot(vtkPlusIntersonArraySDKCxxVideoSource *intersonDevice)
{
  // Set up a 2D scene, add an XY chart to it
  vtkSmartPointer<vtkContextView> view = vtkSmartPointer<vtkContextView>::New();
  view->GetRenderer()->SetBackground(1.0, 1.0, 1.0);
  view->GetRenderWindow()->SetSize(1200, 400);
  vtkSmartPointer<vtkChartXY> chart = vtkSmartPointer<vtkChartXY>::New();
  view->GetScene()->AddItem(chart);

  vtkSmartPointer<vtkExtractImageRow> imageToTableAdaptor=vtkSmartPointer<vtkExtractImageRow>::New();
  imageToTableAdaptor->SetInputConnection(intersonDevice->GetOutputPort());
  imageToTableAdaptor->Update();

  // Add multiple line plots, setting the colors etc
  vtkPlot *line = chart->AddPlot(vtkChart::LINE);
  line->SetInputData(imageToTableAdaptor->GetOutput(), 0, 1);
  line->SetColor(0, 255, 0, 255);
  line->SetWidth(1.0);

  vtkAxis * yAxis = line->GetYAxis();
  yAxis->SetBehavior(vtkAxis::FIXED);
  yAxis->SetMinimumLimit( -32768 );
  yAxis->SetMaximumLimit( 32767 );
  yAxis->SetMinimum( -32768 );
  yAxis->SetMaximum( 32767 );

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
  std::string inputConfigFileName;
  double depthCm = -1;
  double dynRangeDb = -1;
  double frequencyMhz = -1;

  std::string acqMode("B");

  vtksys::CommandLineArguments args;
  args.Initialize(argc, argv);

  int verboseLevel = vtkPlusLogger::LOG_LEVEL_UNDEFINED;

  args.AddArgument("--help", vtksys::CommandLineArguments::NO_ARGUMENT, &printHelp, "Print this help.");
  args.AddArgument("--config-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputConfigFileName, "Config file containing the device configuration.");
  args.AddArgument("--acq-mode", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &acqMode, "Acquisition mode: B or RF (Default: B).");
  args.AddArgument("--depth", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &depthCm, "Depth in cm.");
  args.AddArgument("--frequencyMhz", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &frequencyMhz, "Frequency in MHz");
  args.AddArgument("--dynRangeDb", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &dynRangeDb, "BMode Dynamic Range. 1 corresponds to the maximum dynamic range.");
  args.AddArgument("--rendering-off", vtksys::CommandLineArguments::NO_ARGUMENT, &renderingOff, "Run test without rendering.");
  args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level 1=error only, 2=warning, 3=info, 4=debug, 5=trace)");

  if ( !args.Parse() )
  {
    std::cerr << "Problem parsing arguments" << std::endl;
    std::cout << "\n\nvtkPlusIntersonArraySDKCxxVideoSourceTest help:" << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }

  vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);

  if ( printHelp )
  {
    std::cout << "\n\nvtkPlusIntersonArraySDKCxxVideoSourceTest help:" << args.GetHelp() << std::endl;
    exit(EXIT_SUCCESS);
  }

  vtkSmartPointer<vtkPlusIntersonArraySDKCxxVideoSource> intersonDevice =
    vtkSmartPointer<vtkPlusIntersonArraySDKCxxVideoSource>::New();
  intersonDevice->SetDeviceId("VideoDevice");

  // Read config file
  if( STRCASECMP(inputConfigFileName.c_str(), "") != 0 )
  {
    LOG_DEBUG("Reading config file...");
    vtkSmartPointer<vtkXMLDataElement> configRootElement = vtkSmartPointer<vtkXMLDataElement>::New();
    if (PlusXmlUtils::ReadDeviceSetConfigurationFromFile(configRootElement, inputConfigFileName.c_str())==PLUS_FAIL)
    {
      LOG_ERROR("Unable to read configuration from file " << inputConfigFileName.c_str()); 
      return EXIT_FAILURE;
    }
    intersonDevice->ReadConfiguration(configRootElement);
  }

  DisplayMode displayMode = SHOW_IMAGE;

  if (STRCASECMP(acqMode.c_str(), "B")==0)
  {
    LOG_DEBUG("Acquisition mode: B");
    displayMode = SHOW_IMAGE;
  }
  else if (STRCASECMP(acqMode.c_str(), "RF")==0)
  {
    LOG_DEBUG("Acquisition mode: RF");
    displayMode=SHOW_PLOT;
  }
  else
  {
    LOG_ERROR("Unsupported Acquisition mode requested: " << acqMode);
    exit(EXIT_FAILURE);
  }

  if ( intersonDevice->Connect() != PLUS_SUCCESS )
  {
    LOG_ERROR( "Unable to connect to Interson Probe" );
    exit(EXIT_FAILURE);
  }

  intersonDevice->StartRecording(); // start recording frames for the video

  if( renderingOff )
    {
    // just run the recording for  a few seconds then exit
    LOG_DEBUG("Rendering disabled. Wait for just a few seconds to acquire data before exiting");
    Sleep(3000); // no need to use accurate timer, it's just an approximate delay
    }
  else
    {
    if( displayMode == SHOW_PLOT )
      {
      TestLinePlot(intersonDevice);
      }
    else
      {
      // Show the live ultrasound image in a VTK renderer window
      vtkSmartPointer<vtkImageViewer> viewer = vtkSmartPointer<vtkImageViewer>::New();
      viewer->SetInputConnection(intersonDevice->GetOutputPort());   //set image to the render and window
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

  intersonDevice->StopRecording();
  intersonDevice->Disconnect();
  return EXIT_SUCCESS;
}

