/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

/*!
  \file vtkThorLabsVideoSourceTest.cxx
  \brief Test basic connection to ThorLabs spectrometer

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
#include "vtkPlusChannel.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkSmartPointer.h"
#include "vtkThorLabsVideoSource.h"
#include "vtkTable.h"
#include "vtkTableAlgorithm.h"
#include "vtkUsImagingParameters.h"
#include "vtkXMLUtilities.h"
#include "vtksys/CommandLineArguments.hxx"
#include <stdlib.h>

//----------------------------------------------------------------------------

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
    if (outputTable->GetColumnByName("wavelength")==NULL)
    {
      vtkSmartPointer<vtkFloatArray> arrXnew = vtkSmartPointer<vtkFloatArray>::New();
      arrXnew->SetName("wavelength");
      outputTable->AddColumn(arrXnew);
    }
    if (outputTable->GetColumnByName("Intensity")==NULL)
    {
      vtkSmartPointer<vtkFloatArray> arrRfValNew = vtkSmartPointer<vtkFloatArray>::New();
      arrRfValNew->SetName("Intensity");
      outputTable->AddColumn(arrRfValNew);
    }
    
    int rowCount=inputImage->GetDimensions()[1];
    if (rowCount!=2)
    {
      LOG_ERROR("Expected two rows in the input image, found "<<rowCount);
      return 0;
    }
    int numPoints=inputImage->GetDimensions()[0];
    int wavelengthRow=0;
    int intensityRow=1;

    outputTable->SetNumberOfRows(numPoints);
    if (inputImage->GetScalarType()==VTK_FLOAT)
    {
      float* wavelengthBuffer=reinterpret_cast<float*>(inputImage->GetScalarPointer())+wavelengthRow*numPoints;
      float* intensityBuffer=reinterpret_cast<float*>(inputImage->GetScalarPointer())+intensityRow*numPoints;
      for (int i = 0; i < numPoints; ++i)
      {        
        outputTable->SetValue(i, 0, *(wavelengthBuffer++));
        outputTable->SetValue(i, 1, *(intensityBuffer++));
      }
    }
    else if (inputImage->GetScalarType()==VTK_DOUBLE)
    {
      double* wavelengthBuffer=reinterpret_cast<double*>(inputImage->GetScalarPointer())+wavelengthRow*numPoints;
      double* intensityBuffer=reinterpret_cast<double*>(inputImage->GetScalarPointer())+intensityRow*numPoints;
      for (int i = 0; i < numPoints; ++i)
      {        
        outputTable->SetValue(i, 0, *(wavelengthBuffer++));
        outputTable->SetValue(i, 1, *(intensityBuffer++));
      }
    }
    else
    {
      LOG_ERROR("Plotting is only supported for float and double data");
      return 0;
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
void TestLinePlot(vtkThorLabsVideoSource *spectrometer)
{
  // Set up a 2D scene, add an XY chart to it
  vtkSmartPointer<vtkContextView> view = vtkSmartPointer<vtkContextView>::New();
  view->GetRenderer()->SetBackground(1.0, 1.0, 1.0);
  view->GetRenderWindow()->SetSize(400, 300);
  vtkSmartPointer<vtkChartXY> chart = vtkSmartPointer<vtkChartXY>::New();
  view->GetScene()->AddItem(chart);

  vtkSmartPointer<vtkExtractImageRow> imageToTableAdaptor=vtkSmartPointer<vtkExtractImageRow>::New();
  imageToTableAdaptor->SetInputConnection(spectrometer->GetOutputPort());
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
  std::string inputInstrumentName;

  vtksys::CommandLineArguments args;
  args.Initialize(argc, argv);

  int verboseLevel = vtkPlusLogger::LOG_LEVEL_UNDEFINED;

  args.AddArgument("--help", vtksys::CommandLineArguments::NO_ARGUMENT, &printHelp, "Print this help.");	
  args.AddArgument("--instrument-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputInstrumentName, "Name of the instrument to collect data from");
  args.AddArgument("--rendering-off", vtksys::CommandLineArguments::NO_ARGUMENT, &renderingOff, "Run test without rendering.");	
  args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level 1=error only, 2=warning, 3=info, 4=debug, 5=trace)");	

  if ( !args.Parse() )
  {
    std::cerr << "Problem parsing arguments" << std::endl;
    std::cout << "\n\nvtkThorLabsVideoSourceTest help:" << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }
  
  vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);

  if ( printHelp ) 
  {
    std::cout << "\n\nvtkThorLabsVideoSourceTest help:" << args.GetHelp() << std::endl;
    exit(EXIT_SUCCESS); 
  }

  vtkSmartPointer<vtkThorLabsVideoSource> spectrometer = vtkSmartPointer<vtkThorLabsVideoSource>::New();
  spectrometer->SetDeviceId("VideoDevice");  

  if (!inputInstrumentName.empty())
  {
    spectrometer->SetInstrumentName(inputInstrumentName.c_str());
  }

  spectrometer->CreateDefaultOutputChannel();

  if ( spectrometer->Connect()!=PLUS_SUCCESS ) 
  {
    LOG_ERROR( "Unable to connect to spectrometer" ); 
    exit(EXIT_FAILURE); 
  }

  spectrometer->StartRecording();				//start recording frame from the video

  if (renderingOff)
  {
    // just run the recording for  a few seconds then exit
    LOG_DEBUG("Rendering disabled. Wait for just a few seconds to acquire data before exiting");
    Sleep(5000); // no need to use accurate timer, it's just an approximate delay
    spectrometer->StopRecording(); 
    spectrometer->Disconnect();
  }
  else
  {
    // Allow some time to acquire the first frames (having a first valid frame is important for auto-scale of the display)
    Sleep(500);

    // Print a warning message if there are multiple output channels and rendering is enabled,
    // because only the first output channel is rendered
    std::string firstChannelName;
    std::string allChannelNames;
    int numberOfVideoOutputChannels=0;
    for( ChannelContainerIterator it = spectrometer->GetOutputChannelsStart(); it != spectrometer->GetOutputChannelsEnd(); ++it)
    {
      if ((*it)->HasVideoSource())
      {
        if (numberOfVideoOutputChannels==0)
        {
          // first channel containing video output
          firstChannelName = (*it)->GetChannelId();
          allChannelNames = (*it)->GetChannelId();
        }
        else
        {
          allChannelNames += std::string(", ") + (*it)->GetChannelId();
        }
        numberOfVideoOutputChannels++;
      }
    }
    if (numberOfVideoOutputChannels>1)
    {
      LOG_WARNING("Multiple output channels contain video data: "<<allChannelNames<<". Only the first one ("<<firstChannelName<<") will be displayed");
    }

    TestLinePlot(spectrometer);
  }

  spectrometer->Disconnect();
  return EXIT_SUCCESS;
}

