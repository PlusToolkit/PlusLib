/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

/*!
\file vtkPhantomRegistrationAutoDetectPivotingTest.cxx 
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
#include "vtkWindowToImageFilter.h"
#include "vtkRenderer.h"
#include "vtkPNGWriter.h"
#include "vtkPlot.h"
#include "vtkRenderWindow.h"
#include "vtkReadTrackedSignals.h"

#include "vtkPivotDetectionAlgo.h"

///////////////////////////////////////////////////////////////////
const double ERROR_THRESHOLD_MM = 0.1; // error threshold
const double ERROR_THRESHOLD_DEG = 0.1; // error threshold

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
void SaveMetricPlot(const char* filename, vtkTable* stylusRef, vtkTable* stylusTipRef, vtkTable* stylusTipSpeed,
  std::string &xAxisLabel, std::string &yAxisLabel)
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

  vtkPlot *StylusTipFromPivotLine = chart->AddPlot(vtkChart::LINE);
  StylusTipFromPivotLine->SetInputData_vtk5compatible(stylusTipSpeed, 0, 1);
  StylusTipFromPivotLine->SetColor(1,0,0);
  StylusTipFromPivotLine->SetWidth(1.0);

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

PlusStatus ConstructSignalPlot(vtkTrackedFrameList* trackedFrameList, std::string intermediateFileOutputDirectory, vtkXMLDataElement* aConfig)
{
  double signalTimeRangeMin = trackedFrameList->GetTrackedFrame(0)->GetTimestamp();
  double signalTimeRangeMax = trackedFrameList->GetTrackedFrame(trackedFrameList->GetNumberOfTrackedFrames()-1)->GetTimestamp();
  std::deque<double> signalTimestamps;
  std::deque<double> signalValues;

  //////this is for removing US image data and leave only the tracking pose data
  //std::string filenameTracked=intermediateFileOutputDirectory + "\\EightPivotingPointsTracked.mha";
  //trackedFrameList->SaveTrackerDataOnlyToSequenceMetafile(filenameTracked.c_str(),false);

  LOG_INFO("Range ["<< signalTimeRangeMin<<"-"<<signalTimeRangeMax<<"] "<<(signalTimeRangeMax-signalTimeRangeMin) << "[s]");
  double frequency = 1/(trackedFrameList->GetTrackedFrame(1)->GetTimestamp()-trackedFrameList->GetTrackedFrame(0)->GetTimestamp());
  LOG_INFO("Frequency first frames = "<< frequency << " Frequency average frame = "<<trackedFrameList->GetNumberOfTrackedFrames()/(signalTimeRangeMax-signalTimeRangeMin));

  vtkSmartPointer<vtkReadTrackedSignals> trackerDataMetricExtractor = vtkSmartPointer<vtkReadTrackedSignals>::New();

  trackerDataMetricExtractor->SetTrackerFrames(trackedFrameList);
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
  std::string inputSequenceMetafile;
  std::string inputConfigFileName;
  std::string inputBaselineFileName;
  int verboseLevel=vtkPlusLogger::LOG_LEVEL_UNDEFINED;
  std::string intermediateFileOutputDirectory;
  bool plotSignal(false);
  vtksys::CommandLineArguments cmdargs;
  int succesfulDatasets=0;

  cmdargs.Initialize(argc, argv);
  cmdargs.AddArgument("--seq-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputSequenceMetafile, "Input sequence metafile name with path");    
  cmdargs.AddArgument("--config-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputConfigFileName, "Configuration file name");
  cmdargs.AddArgument("--baseline-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputBaselineFileName, "Name of file storing baseline calibration results");
  cmdargs.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug, 5=trace)");
  cmdargs.AddArgument("--intermediate-file-output-dir", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &intermediateFileOutputDirectory, "Directory into which the intermediate files are written");
  cmdargs.AddArgument("--plot-signal", vtksys::CommandLineArguments::NO_ARGUMENT, &plotSignal, "Run test without plotting the signal.");
  if ( !cmdargs.Parse() )
  {
    std::cerr << "Problem parsing arguments" << std::endl;
    std::cout << "Help: " << cmdargs.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }
  vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);

  if ( inputSequenceMetafile.empty() )
  {
    std::cerr << "--seq-file argument is required" << std::endl;
    std::cout << "Help: " << cmdargs.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }  

  LOG_DEBUG("Read input sequence");
  vtkSmartPointer<vtkTrackedFrameList> trackedFrameList = vtkSmartPointer<vtkTrackedFrameList>::New(); 
  if ( trackedFrameList->ReadFromSequenceMetafile(inputSequenceMetafile.c_str()) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to read sequence metafile: " << inputSequenceMetafile); 
    return EXIT_FAILURE;
  }

  // Read pivotDetectionAlgo configuration
  vtkSmartPointer<vtkXMLDataElement> configPivotDetection = vtkSmartPointer<vtkXMLDataElement>::Take(vtkXMLUtilities::ReadElementFromFile(inputConfigFileName.c_str()));
  if (configPivotDetection == NULL)
  {  
    LOG_ERROR("Unable to read pivotDetectionAlgo configuration from file " << inputConfigFileName.c_str()); 
    exit(EXIT_FAILURE);
  }
  vtkPlusConfig::GetInstance()->SetDeviceSetConfigurationData(configPivotDetection); 
  if ( intermediateFileOutputDirectory.empty() )
  {
    intermediateFileOutputDirectory = vtkPlusConfig::GetInstance()->GetOutputDirectory();
  }

  LOG_INFO("Initialize"); 

  // Read coordinate definitions
  vtkSmartPointer<vtkTransformRepository> transformRepository = vtkSmartPointer<vtkTransformRepository>::New();
  if ( transformRepository->ReadConfiguration(configPivotDetection) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to read CoordinateDefinitions!"); 
    exit(EXIT_FAILURE);
  }
  if ( transformRepository->ReadConfiguration(configPivotDetection) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to read CoordinateDefinitions!"); 
    exit(EXIT_FAILURE);
  }

  //This is to construct a plot of the tracked stylus and stylus tip position norms.
  if(plotSignal)
  {    
    std::string plotFilename = intermediateFileOutputDirectory + "/" +
      vtksys::SystemTools::GetFilenameWithoutLastExtension(inputSequenceMetafile) + ".png";
    ConstructSignalPlot( trackedFrameList, inputSequenceMetafile, configPivotDetection);
  }

  //----------------------------------------------------------------------------------------------
  // Initialize phantom registration
  vtkSmartPointer<vtkPhantomLandmarkRegistrationAlgo> phantomRegistrationAlgo = vtkSmartPointer<vtkPhantomLandmarkRegistrationAlgo>::New();
  if (phantomRegistrationAlgo->ReadConfiguration(configPivotDetection) != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to read phantom definition!");
    exit(EXIT_FAILURE);
  }
  int numberOfLandmarks = phantomRegistrationAlgo->GetDefinedLandmarks()->GetNumberOfPoints();
  if (numberOfLandmarks <= 0)
  {
    LOG_ERROR("Number of defined landmarks should be >0 but it is" << numberOfLandmarks);
    exit(EXIT_FAILURE);
  }
  // Initialize pivot detection
  vtkSmartPointer<vtkPivotDetectionAlgo> pivotDetectionAlgo = vtkSmartPointer<vtkPivotDetectionAlgo>::New();
  if (pivotDetectionAlgo->ReadConfiguration(configPivotDetection) != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to read pivot calibration configuration!");
    exit(EXIT_FAILURE);
  }
  pivotDetectionAlgo->SetExpectedPivotsNumber(numberOfLandmarks);
  pivotDetectionAlgo->SetMinimunDistanceBetweenLandmarksMm(phantomRegistrationAlgo->GetMinimunDistanceBetweenTwoLandmarks());
  if (trackedFrameList->GetNumberOfTrackedFrames()<2)
  {
    LOG_ERROR("There are only "<<trackedFrameList->GetNumberOfTrackedFrames()<<" frames in the sequence file");
    exit(EXIT_FAILURE);
  }
  // Estimate frame rate from the total number of frames and total timespan
  TrackedFrame *firstTrackedFrame = trackedFrameList->GetTrackedFrame(0);
  TrackedFrame *lastTrackedFrame = trackedFrameList->GetTrackedFrame(trackedFrameList->GetNumberOfTrackedFrames()-1);
  double frameTimestampDifferenceSec = lastTrackedFrame->GetTimestamp() - firstTrackedFrame->GetTimestamp();
  double estimatedFrameRate = double(trackedFrameList->GetNumberOfTrackedFrames()-1)/frameTimestampDifferenceSec;
  LOG_INFO("Estimated acquisition rate: "<<estimatedFrameRate);
  pivotDetectionAlgo->SetAcquisitionRate(estimatedFrameRate);

  // Check stylus tool
  PlusTransformName stylusTipToReferenceTransformName(phantomRegistrationAlgo->GetStylusTipCoordinateFrame(), phantomRegistrationAlgo->GetReferenceCoordinateFrame());
  PlusTransformName stylusToReferenceTransformName("Stylus", pivotDetectionAlgo->GetReferenceCoordinateFrame());

  // Acquire positions for pivot detection
  TrackedFrame trackedFrame;
  vtkSmartPointer<vtkMatrix4x4> stylusToReferenceMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
  int numberOfLandmarksDetected = 0;
  bool isCompleted = false;
  for (int frameIndex=0; frameIndex < trackedFrameList->GetNumberOfTrackedFrames(); ++frameIndex)
  {
    if ( transformRepository->SetTransforms(*(trackedFrameList->GetTrackedFrame(frameIndex))) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to update transforms in repository with tracked frame!");
      exit(EXIT_FAILURE);
    }

    vtkSmartPointer<vtkMatrix4x4> stylusTipToReferenceTransformMatrix = vtkMatrix4x4::New();
    bool valid = false;
    if ( (transformRepository->GetTransform(stylusTipToReferenceTransformName, stylusTipToReferenceTransformMatrix, &valid) != PLUS_SUCCESS) || (!valid))
    {
      // There is no available transform for this frame; skip that frame
      LOG_DEBUG("There is no available transform for this frame; skip frame "<<trackedFrameList->GetTrackedFrame(frameIndex)->GetTimestamp()<<" [s]");
      continue;
    }

    pivotDetectionAlgo->InsertNextStylusTipToReferenceTransform(stylusTipToReferenceTransformMatrix);
    if (pivotDetectionAlgo->GetPivotPointsReference()->GetNumberOfPoints()>numberOfLandmarksDetected)
    {
      // new landmark point detected
      double pivotFound[3] = {0,0,0};
      pivotDetectionAlgo->GetPivotPointsReference()->GetPoint(numberOfLandmarksDetected, pivotFound);
      LOG_INFO("Pivot #"<<numberOfLandmarksDetected+1<<" found: (" << pivotFound[0]<<", " << pivotFound[1]<<", " << pivotFound[2]<<") at timestamp = "<<trackedFrameList->GetTrackedFrame(frameIndex)->GetTimestamp());
      // Add recorded point to algorithm
      phantomRegistrationAlgo->GetRecordedLandmarks()->InsertPoint(numberOfLandmarksDetected, pivotFound);
      phantomRegistrationAlgo->GetRecordedLandmarks()->Modified();
      numberOfLandmarksDetected++;
    }
    pivotDetectionAlgo->IsPivotDetectionCompleted(isCompleted);
    if(isCompleted)
    {
      LOG_INFO("Registration completed.");
      break;
    }
  }

  LOG_INFO("Pivot detection algorithm string result:\n" << pivotDetectionAlgo->GetDetectedPivotsString());

  if (phantomRegistrationAlgo->Register(transformRepository) != PLUS_SUCCESS)
  {
    LOG_ERROR("Phantom registration failed!");
    exit(EXIT_FAILURE);
  }

  LOG_INFO("Phantom registration algorithm string result:");
  phantomRegistrationAlgo->PrintRecordedLandmarks_Phantom();

  LOG_INFO("Registration error = " << phantomRegistrationAlgo->GetRegistrationError());

  // Save result
  if (transformRepository->WriteConfiguration(configPivotDetection) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to write phantom registration result to configuration element!");
    exit(EXIT_FAILURE);
  }

  std::string registrationResultFileName = "PhantomRegistrationAutoDetectPivotingTest.xml";
  vtksys::SystemTools::RemoveFile(registrationResultFileName.c_str());
  PlusCommon::PrintXML(registrationResultFileName.c_str(), configPivotDetection); 

  if ( CompareRegistrationResultsWithBaseline( inputBaselineFileName.c_str(), registrationResultFileName.c_str(), phantomRegistrationAlgo->GetPhantomCoordinateFrame(), phantomRegistrationAlgo->GetReferenceCoordinateFrame() ) != PLUS_SUCCESS )
  {
    LOG_ERROR("Comparison of calibration data to baseline failed");
    return EXIT_FAILURE;
  }

  LOG_INFO("Test completed successfully"); 
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
    LOG_ERROR("Transform mismatch (position difference: " << posDiff << "  orientation difference: " << orientDiff << ")");
    return PLUS_FAIL; 
  }
  LOG_INFO("Transform difference is acceptable (position difference: " << posDiff << "  orientation difference: " << orientDiff << ")");

  return PLUS_SUCCESS; 
}
