/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

/*!
\file vtkPhantomRegistrationAutoDetectLandmarkingTest.cxx 
\brief This test runs a phantom registration on a recorded data set and 
compares the results to a baseline
*/ 

#include "PlusConfigure.h"
#include "PlusMath.h"
#include "TrackedFrame.h"
#include "vtkDataCollector.h"
#include "vtkSavedDataSource.h"
#include "vtkMath.h"
#include "vtkMatrix4x4.h"
#include "vtkPhantomLandmarkRegistrationAlgo.h"
#include "vtkPlusChannel.h"
#include "vtkPlusConfig.h"
#include "vtkSmartPointer.h"
#include "vtkTrackedFrameList.h"
#include "vtkTransform.h"
#include "vtkTransformRepository.h"
#include "vtkXMLDataElement.h"
#include "vtkXMLUtilities.h"
#include "vtksys/CommandLineArguments.hxx" 
#include "vtksys/SystemTools.hxx"
#include "vtkDirectory.h"

#include <iostream>
#include <stdlib.h>

#include "vtkAxis.h"
#include "vtkChartXY.h"
#include "vtkContextScene.h"
#include "vtkContextView.h"
#include "vtkDoubleArray.h"
#include "vtkWindowToImageFilter.h"
#include "vtkRenderer.h"
#include "vtkPNGWriter.h"
#include "vtkPlot.h"
#include "vtkRenderWindow.h"
#include "vtkTable.h"

#include "vtkReadTrackedSignals.h"
#include "vtkLandmarkDetectionAlgo.h"

///////////////////////////////////////////////////////////////////
const double ERROR_THRESHOLD_MM = 0.1; // error threshold
const double ERROR_THRESHOLD_DEG = 0.2; // error threshold

PlusStatus CompareRegistrationResultsWithBaseline(const char* baselineFileName, const char* currentResultFileName, const char* phantomCoordinateFrame, const char* referenceCoordinateFrame);

//-----------------------------------------------------------------------------
PlusStatus ConstructTableSignal(std::deque<double> &x, std::deque<double> &y, vtkTable* table)
{
  // Clear table
  while (table->GetNumberOfColumns() > 0)
  {
    table->RemoveColumn(0);
  }

  //  Create array corresponding to the time values of the tracker plot
  vtkSmartPointer<vtkDoubleArray> arrX = vtkSmartPointer<vtkDoubleArray>::New();
  table->AddColumn(arrX);

  //  Create array corresponding to the metric values of the tracker plot
  vtkSmartPointer<vtkDoubleArray> arrY = vtkSmartPointer<vtkDoubleArray>::New();
  table->AddColumn(arrY);

  // Set the tracker data
  table->SetNumberOfRows(x.size());
  for (int i = 0; i < x.size(); ++i)
  {
    table->SetValue(i, 0, x.at(i));
    table->SetValue(i, 1, y.at(i));
  }

  return PLUS_SUCCESS;
}
//----------------------------------------------------------------------------
void SaveMetricPlot(const char* filename, vtkTable* stylusRef, vtkTable* stylusTipRef, vtkTable* stylusTipSpeed, std::string &xAxisLabel,
                    std::string &yAxisLabel)
{
  // Set up the view
  vtkSmartPointer<vtkContextView> view = vtkSmartPointer<vtkContextView>::New();
  view->GetRenderer()->SetBackground(1.0, 1.0, 1.0);
  vtkSmartPointer<vtkChartXY> chart =  vtkSmartPointer<vtkChartXY>::New();
  view->GetScene()->AddItem(chart);

  // Add the two line plots    
  vtkPlot *StylusRefLine = chart->AddPlot(vtkChart::POINTS);
  StylusRefLine->SetInputData_vtk5compatible(stylusRef, 0, 1);
  StylusRefLine->SetColor(0,0,1);
  StylusRefLine->SetWidth(0.3);

  vtkPlot *StylusTipRefLine = chart->AddPlot(vtkChart::POINTS);
  StylusTipRefLine->SetInputData_vtk5compatible(stylusTipRef, 0, 1);
  StylusTipRefLine->SetColor(0,1,0);
  StylusTipRefLine->SetWidth(0.3);

  vtkPlot *StylusTipFromLandmarkLine = chart->AddPlot(vtkChart::LINE);
  StylusTipFromLandmarkLine->SetInputData_vtk5compatible(stylusTipSpeed, 0, 1);
  StylusTipFromLandmarkLine->SetColor(1,0,0);
  StylusTipFromLandmarkLine->SetWidth(1.0);

  chart->SetShowLegend(true);
  chart->GetAxis(vtkAxis::LEFT)->SetTitle(yAxisLabel.c_str());
  chart->GetAxis(vtkAxis::BOTTOM)->SetTitle(xAxisLabel.c_str());

  // Render plot and save it to file
  vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
  renderWindow->AddRenderer(view->GetRenderer());
  renderWindow->SetSize(1600,1200);
  renderWindow->OffScreenRenderingOn(); 

  vtkSmartPointer<vtkWindowToImageFilter> windowToImageFilter = vtkSmartPointer<vtkWindowToImageFilter>::New();
  windowToImageFilter->SetInput(renderWindow);
  windowToImageFilter->Update();

  vtkSmartPointer<vtkPNGWriter> writer = vtkSmartPointer<vtkPNGWriter>::New();
  writer->SetFileName(filename);
  writer->SetInputData_vtk5compatible(windowToImageFilter->GetOutput());
  writer->Write();
}

PlusStatus ConstructSignalPlot(vtkTrackedFrameList* trackedStylusTipFrames, std::string intermediateFileOutputDirectory, vtkXMLDataElement* aConfig)
{
  double signalTimeRangeMin = trackedStylusTipFrames->GetTrackedFrame(0)->GetTimestamp();
  double signalTimeRangeMax = trackedStylusTipFrames->GetTrackedFrame(trackedStylusTipFrames->GetNumberOfTrackedFrames()-1)->GetTimestamp();
  std::deque<double> signalTimestamps;
  std::deque<double> signalValues;

  //////this is for removing US image data and leave only the tracking pose data
  //std::string filenameTracked=intermediateFileOutputDirectory + "\\EightLandmarksPointsTracked.mha";
  //trackedStylusTipFrames->SaveTrackerDataOnlyToSequenceMetafile(filenameTracked.c_str(),false);

  LOG_INFO("Range ["<< signalTimeRangeMin<<"-"<<signalTimeRangeMax<<"] "<<(signalTimeRangeMax-signalTimeRangeMin) << "[s]");
  double frequency = 1/(trackedStylusTipFrames->GetTrackedFrame(1)->GetTimestamp()-trackedStylusTipFrames->GetTrackedFrame(0)->GetTimestamp());
  LOG_INFO("Frequency first frames = "<< frequency << " Frequency average frame = "<<trackedStylusTipFrames->GetNumberOfTrackedFrames()/(signalTimeRangeMax-signalTimeRangeMin));

  vtkSmartPointer<vtkReadTrackedSignals> trackerDataMetricExtractor = vtkSmartPointer<vtkReadTrackedSignals>::New();

  trackerDataMetricExtractor->SetTrackerFrames(trackedStylusTipFrames);
  trackerDataMetricExtractor->SetSignalTimeRange(signalTimeRangeMin, signalTimeRangeMax);
  trackerDataMetricExtractor->ReadConfiguration(aConfig);

  if (trackerDataMetricExtractor->Update() != PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to get line positions from video frames");
    return PLUS_FAIL;
  }
  trackerDataMetricExtractor->GetTimestamps(signalTimestamps);
  trackerDataMetricExtractor->GetSignalStylusTipSpeed(signalValues);
  vtkSmartPointer<vtkTable> stylusTipSpeedTable=vtkSmartPointer<vtkTable>::New();
  ConstructTableSignal(signalTimestamps, signalValues, stylusTipSpeedTable); 
  stylusTipSpeedTable->GetColumn(0)->SetName("Time [s]");
  stylusTipSpeedTable->GetColumn(1)->SetName("stylusTipSpeed");

  trackerDataMetricExtractor->GetSignalStylusRef(signalValues);
  vtkSmartPointer<vtkTable> stylusRefTable=vtkSmartPointer<vtkTable>::New();
  ConstructTableSignal(signalTimestamps, signalValues,stylusRefTable); 
  stylusRefTable->GetColumn(0)->SetName("Time [s]");
  stylusRefTable->GetColumn(1)->SetName("stylusRef");

  trackerDataMetricExtractor->GetSignalStylusTipRef(signalValues);
  vtkSmartPointer<vtkTable> stylusTipRefTable=vtkSmartPointer<vtkTable>::New();
  ConstructTableSignal(signalTimestamps, signalValues,stylusTipRefTable); 
  stylusTipRefTable->GetColumn(0)->SetName("Time [s]");
  stylusTipRefTable->GetColumn(1)->SetName("stylusTipRef");

  if(stylusTipSpeedTable->GetNumberOfColumns() != 2)
  {
    LOG_ERROR("Error in constructing the vtk tables that are to hold fixed signal. Table has " << 
      stylusTipSpeedTable->GetNumberOfColumns() << " columns, but should have two columns");
    return PLUS_FAIL;
  }

  std::string filename=intermediateFileOutputDirectory /*+ "\\StylusTracked.png"*/;
  std::string xLabel = "Time [s]";
  std::string yLabel = "Position Metric";
  SaveMetricPlot(filename.c_str(), stylusRefTable, stylusTipRefTable,  stylusTipSpeedTable, xLabel, yLabel);
  return PLUS_SUCCESS;
}

int main (int argc, char* argv[])
{
  std::string inputConfigFileName;
  std::string inputBaselineFileName;
  int verboseLevel=vtkPlusLogger::LOG_LEVEL_UNDEFINED;
  std::string inputTrackedStylusTipSequence;
  std::string intermediateFileOutputDirectory;
  bool plotSignal(false);
  vtksys::CommandLineArguments cmdargs;
  double accumulatedError=0.0;
  int succesfulDatasets=0;

  cmdargs.Initialize(argc, argv);
  cmdargs.AddArgument("--config-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputConfigFileName, "Configuration file name");
  cmdargs.AddArgument("--baseline-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputBaselineFileName, "Name of file storing baseline calibration results");
  cmdargs.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug, 5=trace)");
  cmdargs.AddArgument("--seq-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputTrackedStylusTipSequence, "Input tracker sequence metafile name (or directory) with path");
  cmdargs.AddArgument("--intermediate-file-output-dir", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &intermediateFileOutputDirectory, "Directory into which the intermediate files are written");
  cmdargs.AddArgument("--plot-signal", vtksys::CommandLineArguments::NO_ARGUMENT, &plotSignal, "Run test without plotting the signal.");
  if ( !cmdargs.Parse() )
  {
    std::cerr << "Problem parsing arguments" << std::endl;
    std::cout << "Help: " << cmdargs.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }
  vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);
  LOG_INFO("Initialize"); 

  // Read LandmarkDetection configuration
  vtkSmartPointer<vtkXMLDataElement> configLandmarkDetection = vtkSmartPointer<vtkXMLDataElement>::Take(vtkXMLUtilities::ReadElementFromFile(inputConfigFileName.c_str()));
  if (configLandmarkDetection == NULL)
  {  
    LOG_ERROR("Unable to read LandmarkDetection configuration from file " << inputConfigFileName.c_str()); 
    exit(EXIT_FAILURE);
  }
  vtkPlusConfig::GetInstance()->SetDeviceSetConfigurationData(configLandmarkDetection); 
  if ( intermediateFileOutputDirectory.empty() )
  {
    intermediateFileOutputDirectory = vtkPlusConfig::GetInstance()->GetOutputDirectory();
  }

  //--------------------------------------------------------------------------------------------------------------------------------------
  // Initialize data collection
  vtkSmartPointer<vtkDataCollector> dataCollector = vtkSmartPointer<vtkDataCollector>::New(); 
  if (dataCollector->ReadConfiguration(configLandmarkDetection) != PLUS_SUCCESS) {
    LOG_ERROR("Unable to parse configuration from file " << inputConfigFileName.c_str()); 
    exit(EXIT_FAILURE);
  }
  if (dataCollector->Connect() != PLUS_SUCCESS)
  {
    LOG_ERROR("Data collector was unable to connect to devices!");
    exit(EXIT_FAILURE);
  }
  if (dataCollector->Start() != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to start data collection!");
    exit(EXIT_FAILURE);
  }
  vtkPlusChannel* aChannel(NULL);
  vtkPlusDevice* aDevice(NULL);
  if( dataCollector->GetDevice(aDevice, std::string("TrackerDevice")) != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to locate device by ID: \'TrackerDevice\'");
    exit(EXIT_FAILURE);
  }
  if( aDevice->GetOutputChannelByName(aChannel, "TrackerStream") != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to locate channel by ID: \'TrackerStream\'");
    exit(EXIT_FAILURE);
  }
  if ( aChannel->GetTrackingDataAvailable() == false ) {
    LOG_ERROR("Channel \'" << aChannel->GetChannelId() << "\' is not tracking!");
    exit(EXIT_FAILURE);
  }
  if (aChannel->GetTrackingDataAvailable() == false)
  {
    LOG_ERROR("Data collector is not tracking!");
    exit(EXIT_FAILURE);
  }
  // Read coordinate definitions
  vtkSmartPointer<vtkTransformRepository> transformRepository = vtkSmartPointer<vtkTransformRepository>::New();
  if ( transformRepository->ReadConfiguration(configLandmarkDetection) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to read CoordinateDefinitions!"); 
    exit(EXIT_FAILURE);
  }
  if ( transformRepository->ReadConfiguration(configLandmarkDetection) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to read CoordinateDefinitions!"); 
    exit(EXIT_FAILURE);
  }

  // Initialize fake tracker
  vtkSavedDataSource *trackerDevice = dynamic_cast<vtkSavedDataSource*>(aDevice);
  if (trackerDevice== NULL)
  {
    LOG_ERROR("Invalid tracker object!");
    exit(EXIT_FAILURE);
  }

  std::string extension = vtksys::SystemTools::GetFilenameExtension(inputTrackedStylusTipSequence);
  int numberFiles = 0;
  vtkSmartPointer<vtkDirectory> myDir = vtkSmartPointer<vtkDirectory>::New();

  if(extension==".mha")
  {
    LOG_INFO("Only one sequence"<<extension);
    numberFiles=1;
  }
  else
  {
    myDir->Open (inputTrackedStylusTipSequence.c_str());
    numberFiles = myDir->GetNumberOfFiles();
  }
  // Get each file name in the directory
  for (int i = 0; i < numberFiles; i++)
  {
    std::string fileString;
    if(numberFiles==1)
    {
      fileString = inputTrackedStylusTipSequence;
    }
    else
    {   
      fileString =inputTrackedStylusTipSequence;
      fileString += "\\";
      fileString += myDir->GetFile(i);
    }
    extension = vtksys::SystemTools::GetFilenameExtension(fileString);
    if(extension==".mha")
    {
      vtkSmartPointer<vtkTrackedFrameList> trackedStylusTipFrames = vtkSmartPointer<vtkTrackedFrameList>::New();
      if( !fileString.empty() )
      {
        trackedStylusTipFrames->SetValidationRequirements(REQUIRE_UNIQUE_TIMESTAMP | REQUIRE_TRACKING_OK);
        LOG_INFO("Read stylus tracker data from " << fileString);
        if ( trackedStylusTipFrames->ReadFromSequenceMetafile(fileString.c_str()) != PLUS_SUCCESS )
        {
          LOG_ERROR("Failed to read stylus data from sequence metafile: " << fileString << ". Exiting...");
          exit(EXIT_FAILURE);
        }
        trackedStylusTipFrames->Register(NULL);
      }
      else
      {
        LOG_ERROR("Empty file name to read sequence metafile: " );
      }

      //This is to construct a plot of the tracked stylus and stylus tip position norms.
      fileString =intermediateFileOutputDirectory;
      fileString+="\\";
      if(numberFiles==1)
      {      
        fileString+=vtksys::SystemTools::GetFilenameWithoutLastExtension(inputTrackedStylusTipSequence);
        fileString+=".png";
      }
      else
      {
        fileString+=vtksys::SystemTools::GetFilenameWithoutLastExtension(myDir->GetFile(i));
        fileString+=".png";
      }
      if(plotSignal)
      {    
        ConstructSignalPlot( trackedStylusTipFrames, fileString, configLandmarkDetection);
      }

      //----------------------------------------------------------------------------------------------
      // Initialize phantom registration
      vtkSmartPointer<vtkPhantomLandmarkRegistrationAlgo> phantomRegistration = vtkSmartPointer<vtkPhantomLandmarkRegistrationAlgo>::New();
      if (phantomRegistration == NULL)
      {
        LOG_ERROR("Unable to instantiate phantom registration algorithm class!");
        exit(EXIT_FAILURE);
      }
      if (phantomRegistration->ReadConfiguration(configLandmarkDetection) != PLUS_SUCCESS)
      {
        LOG_ERROR("Unable to read phantom definition!");
        exit(EXIT_FAILURE);
      }
      int numberOfExpectedLandmarks = phantomRegistration->GetDefinedLandmarks_Phantom()->GetNumberOfPoints();
      if (numberOfExpectedLandmarks != 8)
      {
        LOG_ERROR("Number of defined landmarks should be 8 instead of " << numberOfExpectedLandmarks << "!");
        exit(EXIT_FAILURE);
      }
      // Initialize Landmark detection
      vtkSmartPointer<vtkLandmarkDetectionAlgo> landmarkDetection = vtkSmartPointer<vtkLandmarkDetectionAlgo>::New();
      if (landmarkDetection == NULL)
      {
        LOG_ERROR("Unable to instantiate landmark detection algorithm class!");
        exit(EXIT_FAILURE);
      }
      landmarkDetection->SetMinimunDistanceBetweenLandmarksMm(phantomRegistration->GetMinimunDistanceBetweenTwoLandmarksMm());
      landmarkDetection->SetAcquisitionRate(1/(trackedStylusTipFrames->GetTrackedFrame(1)->GetTimestamp()-trackedStylusTipFrames->GetTrackedFrame(0)->GetTimestamp()));
      if (landmarkDetection->ReadConfiguration(configLandmarkDetection) != PLUS_SUCCESS)
      {
        LOG_ERROR("Unable to read Landmark detection configuration!");
        exit(EXIT_FAILURE);
      }

      // Check stylus tool
      PlusTransformName stylusTipToReferenceTransformName(phantomRegistration->GetStylusTipCoordinateFrame(), phantomRegistration->GetReferenceCoordinateFrame());
      PlusTransformName stylusToReferenceTransformName("Stylus", phantomRegistration->GetReferenceCoordinateFrame());
      PlusTransformName stylusTipToStylusTransformName(phantomRegistration->GetStylusTipCoordinateFrame(), "Stylus");

      vtkSmartPointer<vtkMatrix4x4> stylusTipToStylusTransform = vtkSmartPointer<vtkMatrix4x4>::New();
      bool valid = false;
      transformRepository->GetTransform(stylusTipToStylusTransformName, stylusTipToStylusTransform, &valid);
      double offsetPhantom[3] = {0,0,0};
      double landmarkFound[3] = {0,0,0};

      if (valid)
      {
        // Acquire positions for landmark detection
        TrackedFrame trackedFrame;
        vtkSmartPointer<vtkMatrix4x4> stylusToReferenceMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
        for (int j=0; j < trackedStylusTipFrames->GetNumberOfTrackedFrames(); ++j)
        {
          //trackerDevice->SetCounter(j);
          //aChannel->GetTrackedFrame((trackedStylusTipFrames->GetTrackedFrame(j)));

          if ( transformRepository->SetTransforms(*(trackedStylusTipFrames->GetTrackedFrame(j))) != PLUS_SUCCESS )
          {
            LOG_ERROR("Failed to update transforms in repository with tracked frame!");
            exit(EXIT_FAILURE);
          }

          valid = false;
          if ( (transformRepository->GetTransform(stylusToReferenceTransformName, stylusToReferenceMatrix, &valid) != PLUS_SUCCESS) || (!valid) )
          {
            //LOG_ERROR("No valid transform found between stylus to reference!");
            //exit(EXIT_FAILURE);
            // There is no available transform for this frame; skip that frame
            LOG_INFO("There is no available transform for this frame; skip frame "<<trackedStylusTipFrames->GetTrackedFrame(j)->GetTimestamp()<<" [s]")
              continue;
          }

          vtkSmartPointer<vtkMatrix4x4> stylusTipToReferenceTransformMatrix = vtkMatrix4x4::New();
          vtkMatrix4x4::Multiply4x4(stylusToReferenceMatrix,stylusTipToStylusTransform,stylusTipToReferenceTransformMatrix);
          int newLandmarkDetected=-1;
          landmarkDetection->InsertNextStylusTipToReferenceTransform(stylusTipToReferenceTransformMatrix, newLandmarkDetected);
          if(newLandmarkDetected>0)
          {
            landmarkDetection->GetDetectedLandmarkPoints_Reference()->GetPoint(landmarkDetection->GetDetectedLandmarkPoints_Reference()->GetNumberOfPoints()-1, landmarkFound);
            // Add recorded point to algorithm
            phantomRegistration->GetRecordedLandmarks_Reference()->InsertPoint(landmarkDetection->GetDetectedLandmarkPoints_Reference()->GetNumberOfPoints()-1, landmarkFound);
            phantomRegistration->GetRecordedLandmarks_Reference()->Modified();
            LOG_INFO("\nLandmark found (" << landmarkFound[0]<<", " << landmarkFound[1]<<", " << landmarkFound[2]<<") at "<<trackedStylusTipFrames->GetTrackedFrame(j)->GetTimestamp()<<"[ms]"<< "\nNumber of landmarks in phantonReg "<<phantomRegistration->GetRecordedLandmarks_Reference()->GetNumberOfPoints());
            vtkPlusLogger::PrintProgressbar((100.0 *  newLandmarkDetected-1) / numberOfExpectedLandmarks);

            if(newLandmarkDetected==numberOfExpectedLandmarks)
            {
              succesfulDatasets++;
              //LOG_INFO("\nREgistration completed" );
              break;
            }
          }
 
        }
      }
      else
      {
        LOG_ERROR("No valid transform found between stylus to stylus tip!");
      }

      LOG_INFO(landmarkDetection->GetDetectedLandmarksString());

      if (phantomRegistration->Register(transformRepository) != PLUS_SUCCESS)
      {
        LOG_ERROR("Phantom registration failed!");
        exit(EXIT_FAILURE);
      }

      phantomRegistration->PrintRecordedLandmarks_Phantom();

      LOG_INFO("Registration error = " << phantomRegistration->GetRegistrationErrorMm());
      accumulatedError+=phantomRegistration->GetRegistrationErrorMm();
      vtkPlusLogger::PrintProgressbar(100); 

      if(numberFiles==1)
      {
        // Save result
        if (transformRepository->WriteConfiguration(configLandmarkDetection) != PLUS_SUCCESS )
        {
          LOG_ERROR("Failed to write phantom registration result to configuration element!");
          exit(EXIT_FAILURE);
        }

        std::string registrationResultFileName = "PhantomRegistrationAutoDetectLandmarkTest.xml";
        vtksys::SystemTools::RemoveFile(registrationResultFileName.c_str());
        PlusCommon::PrintXML(registrationResultFileName.c_str(), configLandmarkDetection); 

        if ( CompareRegistrationResultsWithBaseline( inputBaselineFileName.c_str(), registrationResultFileName.c_str(), phantomRegistration->GetPhantomCoordinateFrame(), phantomRegistration->GetReferenceCoordinateFrame() ) != PLUS_SUCCESS )
        {
          LOG_ERROR("Comparison of calibration data to baseline failed");
          std::cout << "Exit failure!!!" << std::endl; 
          return EXIT_FAILURE;
        }
      }
    }
  }
  LOG_INFO("AccumulatedError = " << accumulatedError<< " Number of succesful registrations/datasets = " << succesfulDatasets<<"/"<<numberFiles-2);
  LOG_INFO( "Exit success!!!" ); 
  return EXIT_SUCCESS; 
}

//-----------------------------------------------------------------------------

// return the number of differences
PlusStatus CompareRegistrationResultsWithBaseline(const char* baselineFileName, const char* currentResultFileName, const char* phantomCoordinateFrame, const char* referenceCoordinateFrame)
{
  int numberOfFailures=0;

  if ( baselineFileName == NULL )
  {
    LOG_ERROR("Unable to read the baseline configuration file - filename is NULL"); 
    return PLUS_FAIL;
  }

  if ( currentResultFileName == NULL )
  {
    LOG_ERROR("Unable to read the current configuration file - filename is NULL"); 
    return PLUS_FAIL;
  }

  PlusTransformName tnPhantomToPhantomReference(phantomCoordinateFrame, referenceCoordinateFrame); 

  // Load current phantom registration
  vtkSmartPointer<vtkXMLDataElement> currentRootElem = vtkSmartPointer<vtkXMLDataElement>::Take(
    vtkXMLUtilities::ReadElementFromFile(currentResultFileName));
  if (currentRootElem == NULL) 
  {  
    LOG_ERROR("Unable to read the current configuration file: " << currentResultFileName); 
    return PLUS_FAIL;
  }

  vtkSmartPointer<vtkTransformRepository> currentTransformRepository = vtkSmartPointer<vtkTransformRepository>::New(); 
  if ( currentTransformRepository->ReadConfiguration(currentRootElem) != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to read the current CoordinateDefinitions from configuration file: " << currentResultFileName); 
    return PLUS_FAIL;
  }

  vtkSmartPointer<vtkMatrix4x4> currentMatrix = vtkSmartPointer<vtkMatrix4x4>::New(); 
  if ( currentTransformRepository->GetTransform(tnPhantomToPhantomReference, currentMatrix) != PLUS_SUCCESS )
  {
    std::string strTransformName; 
    tnPhantomToPhantomReference.GetTransformName(strTransformName);
    LOG_ERROR("Unable to get '" << strTransformName << "' coordinate definition from configuration file: " << currentResultFileName); 
    return PLUS_FAIL;
  }

  // Load baseline phantom registration
  vtkSmartPointer<vtkXMLDataElement> baselineRootElem = vtkSmartPointer<vtkXMLDataElement>::Take(
    vtkXMLUtilities::ReadElementFromFile(baselineFileName));
  if (baselineFileName == NULL) 
  {  
    LOG_ERROR("Unable to read the baseline configuration file: " << baselineFileName); 
    return PLUS_FAIL;
  }

  vtkSmartPointer<vtkTransformRepository> baselineTransformRepository = vtkSmartPointer<vtkTransformRepository>::New(); 
  if ( baselineTransformRepository->ReadConfiguration(baselineRootElem) != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to read the baseline CoordinateDefinitions from configuration file: " << baselineFileName); 
    return PLUS_FAIL;
  }

  vtkSmartPointer<vtkMatrix4x4> baselineMatrix = vtkSmartPointer<vtkMatrix4x4>::New(); 
  if ( baselineTransformRepository->GetTransform(tnPhantomToPhantomReference, baselineMatrix) != PLUS_SUCCESS )
  {
    std::string strTransformName; 
    tnPhantomToPhantomReference.GetTransformName(strTransformName);
    LOG_ERROR("Unable to get '" << strTransformName << "' coordinate definition from configuration file: " << baselineFileName); 
    return PLUS_FAIL;
  }

  // Compare the transforms
  double posDiff=PlusMath::GetPositionDifference(currentMatrix, baselineMatrix); 
  double orientDiff=PlusMath::GetOrientationDifference(currentMatrix, baselineMatrix); 

  if ( fabs(posDiff) > ERROR_THRESHOLD_MM || fabs(orientDiff) > ERROR_THRESHOLD_DEG )
  {
    LOG_ERROR("Transform mismatch (position difference: " << posDiff << "  orientation difference: " << orientDiff);
    return PLUS_FAIL; 
  }
  LOG_INFO("Transform difference is acceptable (position difference: " << posDiff << "  orientation difference: " << orientDiff);

  return PLUS_SUCCESS; 
}
