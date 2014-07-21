/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

/*!
  \file TemporalCalibrationAlgoTest.cxx
  \brief This program tests the temporal calibration algorithm by computing the time by which the inputted tracker data lags the inputted US video data.
  
  The inputted data--video and tracker--is assumed to be collected by a US probe imaging a planar object; furthermore,
  it is assumed that the probe is undergoing uni-dirctional periodic motion in the direction perpendicular to the
  plane's face (E.g. moving the probe in a repeating up-and-down fashion while imaging the bottom of a water bath).
  The inputted data is assumed to contain at least ?five? full periods (although the algorithm may work for fewer periods
  it has not been tested under these conditions.

  \ingroup PlusLibCalibrationAlgorithm
*/

#include "PlusConfigure.h"
#include "TrackedFrame.h"
#include "vtkAxis.h"
#include "vtkChartXY.h"
#include "vtkContextScene.h"
#include "vtkContextView.h"
#include "vtkPNGWriter.h"
#include "vtkPlot.h"
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"
#include "vtkTemporalCalibrationAlgo.h"
#include "vtkTrackedFrameList.h"
#include "vtkWindowToImageFilter.h"
#include "vtkXMLDataElement.h"
#include "vtkXMLUtilities.h"
#include "vtksys/CommandLineArguments.hxx"

// define tolerance used for comparing double numbers
namespace
{
  const double MAX_ALLOWED_TIME_LAG_DIFF_SEC = 0.005;
  const double MAX_ALLOWED_ERROR_DIFF = 1.0;

  struct TemporalCalibrationResult
  {
    double trackerLagSec;
    double calibrationError;
    double maxCalibrationError;

    TemporalCalibrationResult()
      : trackerLagSec(0)
      , calibrationError(0)
      , maxCalibrationError(0)
    {}
  };
}

//----------------------------------------------------------------------------
void SaveMetricPlot(const char* filename, vtkTable* videoPositionMetric, vtkTable* trackerPositionMetric, std::string &xAxisLabel,
                    std::string &yAxisLabel)
{
  // Set up the view
  vtkSmartPointer<vtkContextView> view = vtkSmartPointer<vtkContextView>::New();
  view->GetRenderer()->SetBackground(1.0, 1.0, 1.0);
  vtkSmartPointer<vtkChartXY> chart =  vtkSmartPointer<vtkChartXY>::New();
  view->GetScene()->AddItem(chart);

  // Add the two line plots    
  vtkPlot *videoPositionMetricLine = chart->AddPlot(vtkChart::LINE);
  videoPositionMetricLine->SetInputData_vtk5compatible(videoPositionMetric, 0, 1);
  videoPositionMetricLine->SetColor(0,0,1);
  videoPositionMetricLine->SetWidth(1.0);

  vtkPlot *trackerMetricLine = chart->AddPlot(vtkChart::LINE);
  trackerMetricLine->SetInputData_vtk5compatible(trackerPositionMetric, 0, 1);
  trackerMetricLine->SetColor(0,1,0);
  trackerMetricLine->SetWidth(1.0);
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

//----------------------------------------------------------------------------
void WriteCalibrationResultToFile(const std::string &outputFileName, const TemporalCalibrationResult &calibResult)
{
  ofstream myfile;
  myfile.open (outputFileName.c_str());
  myfile << "<TemporalCalibrationResults TrackerLagSec=\"" << calibResult.trackerLagSec 
    << "\" CalibrationError=\"" << calibResult.calibrationError 
    << "\" MaxCalibrationError=\"" << calibResult.maxCalibrationError<< "\" />";
  myfile.close();
}

//----------------------------------------------------------------------------
PlusStatus ReadCalibrationResultsFromFile(const std::string& resultSaveFilename, TemporalCalibrationResult &calibResult)
{  
  calibResult.calibrationError=0.0;
  calibResult.maxCalibrationError=0.0;
  calibResult.trackerLagSec=0.0;

  if (resultSaveFilename.empty())
  {
    LOG_ERROR("Cannot read line calibration results, filename is empty");
    return PLUS_FAIL;
  }
  vtkSmartPointer<vtkXMLDataElement> resultsElem = vtkSmartPointer<vtkXMLDataElement>::Take(vtkXMLUtilities::ReadElementFromFile(resultSaveFilename.c_str()));      
  if ( resultsElem == NULL )
  {
    LOG_ERROR("Failed to read baseline file: "<< resultSaveFilename);
    return PLUS_FAIL;
  }
  if (resultsElem->GetName()==NULL || STRCASECMP( resultsElem->GetName(), "TemporalCalibrationResults" ) != 0)
  {
    LOG_ERROR("Unable to find TemporalCalibrationResults XML data element in baseline: " << resultSaveFilename); 
    return PLUS_FAIL;
  }
  PlusStatus status=PLUS_SUCCESS;
  if ( !resultsElem->GetScalarAttribute( "CalibrationError", calibResult.calibrationError) )
  {
    LOG_ERROR("Unable to find CalibrationError attribute in TemporalCalibrationResults element");
    status=PLUS_FAIL;
  }
  if ( !resultsElem->GetScalarAttribute( "MaxCalibrationError", calibResult.maxCalibrationError) )
  {
    LOG_ERROR("Unable to find MaxCalibrationError attribute in TemporalCalibrationResults element");
    status=PLUS_FAIL;
  }
  if ( !resultsElem->GetScalarAttribute( "TrackerLagSec", calibResult.trackerLagSec) )
  {
    LOG_ERROR("Unable to find TrackerLagSec attribute in TemporalCalibrationResults element");
    status=PLUS_FAIL;
  }
  return status;
}

//----------------------------------------------------------------------------
int CompareCalibrationResults(const TemporalCalibrationResult &calibResult, const TemporalCalibrationResult &baselineCalibResult)
{
  int numberOfFailures=0;

  if (fabs(calibResult.trackerLagSec-baselineCalibResult.trackerLagSec)>MAX_ALLOWED_TIME_LAG_DIFF_SEC)
  {
    LOG_ERROR("TrackerLagSec comparison error: current=" << calibResult.trackerLagSec << ", baseline=" << baselineCalibResult.trackerLagSec);
    ++numberOfFailures;
  }
  if (fabs(calibResult.calibrationError-baselineCalibResult.calibrationError)>MAX_ALLOWED_ERROR_DIFF)
  {
    LOG_ERROR("CalibrationError comparison error: current=" << calibResult.calibrationError << ", baseline=" << baselineCalibResult.calibrationError);
    ++numberOfFailures;
  }
  if (fabs(calibResult.maxCalibrationError-baselineCalibResult.maxCalibrationError)>MAX_ALLOWED_ERROR_DIFF)
  {
    LOG_ERROR("MaxCalibrationError comparison error: current=" << calibResult.maxCalibrationError << ", baseline=" << baselineCalibResult.maxCalibrationError);
    ++numberOfFailures;
  }
    
  return numberOfFailures;
}

//----------------------------------------------------------------------------
int main(int argc, char **argv)
{
  bool printHelp(false);
  bool plotResults(false);
  bool saveIntermediateImages(false);
  int verboseLevel = vtkPlusLogger::LOG_LEVEL_UNDEFINED;
  std::string inputMovingSequenceMetafile("");
  std::string inputFixedSequenceMetafile("");
  std::string intermediateFileOutputDirectory; // Directory into which the intermediate files are written
  double samplingResolutionSec = 0.001; //  Resolution used for re-sampling [s]
  std::string fixedProbeToReferenceTransformNameStr("");
  std::string movingProbeToReferenceTransformNameStr("");
  double maxTimeOffsetSec = 0.5;
  std::vector<int> clipRectOrigin;
  std::vector<int> clipRectSize;
  std::string inputBaselineFileName;

  vtksys::CommandLineArguments args;
  args.Initialize(argc, argv);

  args.AddArgument("--help", vtksys::CommandLineArguments::NO_ARGUMENT, &printHelp, "Print this help.");
  args.AddArgument("--fixed-seq-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputFixedSequenceMetafile, "Input US image sequence metafile name with path");
  args.AddArgument("--moving-seq-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputMovingSequenceMetafile, "Input tracker sequence metafile name with path");
  args.AddArgument("--fixed-probe-to-reference-transform", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &fixedProbeToReferenceTransformNameStr, "Transform name that describes the probe pose relative to a static reference (default: ProbeToReference)");  
  args.AddArgument("--moving-probe-to-reference-transform", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &movingProbeToReferenceTransformNameStr, "Transform name that describes the probe pose relative to a static reference (default: ProbeToReference)");  
  args.AddArgument("--max-time-offset-sec", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &maxTimeOffsetSec, "Maximum time offset between the signals, in seconds (default: 2 seconds)");  
  args.AddArgument("--plot-results", vtksys::CommandLineArguments::NO_ARGUMENT, &plotResults, "Plot results (display position vs. time plots without and with temporal calibration)");
  args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug, 5=trace)");
  args.AddArgument("--sampling-resolution-sec", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &samplingResolutionSec, "Sampling resolution (in seconds, default is 0.001)");    
  args.AddArgument("--save-intermediate-images", vtksys::CommandLineArguments::NO_ARGUMENT, &saveIntermediateImages, "Save images of intermediate steps (scanlines used, and detected lines)");
  args.AddArgument("--intermediate-file-output-dir", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &intermediateFileOutputDirectory, "Directory into which the intermediate files are written");
  args.AddArgument("--clip-rect-origin", vtksys::CommandLineArguments::MULTI_ARGUMENT, &clipRectOrigin, "Origin of the clipping rectangle");
  args.AddArgument("--clip-rect-size", vtksys::CommandLineArguments::MULTI_ARGUMENT, &clipRectSize, "Size of the clipping rectangle");
  args.AddArgument("--baseline-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputBaselineFileName, "Input xml baseline file name with path");    

  if ( !args.Parse() )
  {
    std::cerr << "Problem parsing arguments" << std::endl;
    std::cout << "Help: " << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }
  
  vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);

  if ( printHelp )
  {
    std::cout << "Help: " << args.GetHelp() << std::endl;
    exit(EXIT_SUCCESS);
  }

  if ( inputMovingSequenceMetafile.empty() )
  {
    std::cerr << "input-tracker-sequence-metafile required argument!" << std::endl;
    std::cout << "Help: " << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }

  if ( inputFixedSequenceMetafile.empty() )
  {
    std::cerr << "input-video-sequence-metafile required argument!" << std::endl;
    std::cout << "Help: " << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }

  if ( intermediateFileOutputDirectory.empty() )
  {
    intermediateFileOutputDirectory = vtkPlusConfig::GetInstance()->GetOutputDirectory();
  }

  vtkTemporalCalibrationAlgo* testTemporalCalibrationObject = vtkTemporalCalibrationAlgo::New();
  vtkTemporalCalibrationAlgo::FRAME_TYPE movingType(vtkTemporalCalibrationAlgo::FRAME_TYPE_NONE);
  vtkTemporalCalibrationAlgo::FRAME_TYPE fixedType(vtkTemporalCalibrationAlgo::FRAME_TYPE_NONE);
  vtkSmartPointer<vtkTrackedFrameList> movingFrames = vtkSmartPointer<vtkTrackedFrameList>::New();
  vtkSmartPointer<vtkTrackedFrameList> fixedFrames = vtkSmartPointer<vtkTrackedFrameList>::New();
  if( !fixedProbeToReferenceTransformNameStr.empty() )
  {
    fixedFrames->SetValidationRequirements(REQUIRE_UNIQUE_TIMESTAMP | REQUIRE_TRACKING_OK);
    testTemporalCalibrationObject->SetFixedProbeToReferenceTransformName(fixedProbeToReferenceTransformNameStr);
    fixedType = vtkTemporalCalibrationAlgo::FRAME_TYPE_TRACKER;
  }
  else
  {
    fixedFrames->SetValidationRequirements(REQUIRE_UNIQUE_TIMESTAMP);
    fixedType = vtkTemporalCalibrationAlgo::FRAME_TYPE_VIDEO;
  }

  if( !movingProbeToReferenceTransformNameStr.empty() )
  {
    movingFrames->SetValidationRequirements(REQUIRE_UNIQUE_TIMESTAMP | REQUIRE_TRACKING_OK);
    testTemporalCalibrationObject->SetMovingProbeToReferenceTransformName(movingProbeToReferenceTransformNameStr);
    movingType = vtkTemporalCalibrationAlgo::FRAME_TYPE_TRACKER;
  }
  else
  {
    movingFrames->SetValidationRequirements(REQUIRE_UNIQUE_TIMESTAMP);
    movingType = vtkTemporalCalibrationAlgo::FRAME_TYPE_VIDEO;
  }

  //  Read moving frames
  LOG_DEBUG("Read tracker data from " << inputMovingSequenceMetafile);
  if ( movingFrames->ReadFromSequenceMetafile(inputMovingSequenceMetafile.c_str()) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to read moving data from sequence metafile: " << inputMovingSequenceMetafile << ". Exiting...");
    exit(EXIT_FAILURE);
  }
  testTemporalCalibrationObject->SetMovingFrames(movingFrames, movingType);

  //  Read fixed frames
  LOG_DEBUG("Read video data from " << inputFixedSequenceMetafile);
  if ( fixedFrames->ReadFromSequenceMetafile(inputFixedSequenceMetafile.c_str()) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to read fixed data from sequence metafile: " << inputFixedSequenceMetafile << ". Exiting...");
    exit(EXIT_FAILURE);
  }
  testTemporalCalibrationObject->SetFixedFrames(fixedFrames, fixedType);
  
  //  Create temporal calibration object; Set pertinent parameters
  testTemporalCalibrationObject->SetSamplingResolutionSec(samplingResolutionSec);
  testTemporalCalibrationObject->SetSaveIntermediateImages(saveIntermediateImages);
  testTemporalCalibrationObject->SetIntermediateFilesOutputDirectory(intermediateFileOutputDirectory);
  testTemporalCalibrationObject->SetMaximumMovingLagSec(maxTimeOffsetSec);

  if (clipRectOrigin.size() > 0 || clipRectSize.size() > 0)
  {
    if (clipRectOrigin.size() != 2 || clipRectSize.size() != 2)
    {
      LOG_ERROR("Invalid clip rectangle origin and/or size");
      exit(EXIT_FAILURE);
    }
    int clipRectOriginIntVec[2] = { clipRectOrigin[0], clipRectOrigin[1] };
    int clipRectSizeIntVec[2] = { clipRectSize[0], clipRectSize[1] };
    testTemporalCalibrationObject->SetVideoClipRectangle(clipRectOriginIntVec, clipRectSizeIntVec);
  }

  vtkTemporalCalibrationAlgo::TEMPORAL_CALIBRATION_ERROR error(vtkTemporalCalibrationAlgo::TEMPORAL_CALIBRATION_ERROR_NONE);

  //  Calculate the time-offset
  if (testTemporalCalibrationObject->Update(error) != PLUS_SUCCESS)
  {
    LOG_ERROR("Cannot determine tracker lag, temporal calibration failed");
    exit(EXIT_FAILURE);
  }

  // Display results
  TemporalCalibrationResult calibResult;
  if (testTemporalCalibrationObject->GetMovingLagSec(calibResult.trackerLagSec) != PLUS_SUCCESS)
  {
    LOG_ERROR("Cannot determine tracker lag, temporal calibration failed");
    exit(EXIT_FAILURE);
  }
  LOG_INFO("Tracker lag: " << calibResult.trackerLagSec << " sec (>0 if the tracker data lags)");
  if (testTemporalCalibrationObject->GetCalibrationError(calibResult.calibrationError)!=PLUS_SUCCESS)
  {
    LOG_ERROR("Cannot determine calibration error, temporal calibration failed");
    exit(EXIT_FAILURE);
  }
  LOG_INFO("Calibration error: " << calibResult.calibrationError);
  if (testTemporalCalibrationObject->GetMaxCalibrationError(calibResult.maxCalibrationError)!=PLUS_SUCCESS)
  {
    LOG_ERROR("Cannot determine max calibration error, temporal calibration failed");
    exit(EXIT_FAILURE);
  }
  LOG_INFO("Max calibration error: " << calibResult.maxCalibrationError);

  // Write results to file
  std::ostrstream trackerLagOutputFilename;
  trackerLagOutputFilename << intermediateFileOutputDirectory << "/TemporalCalibrationResults.xml" << std::ends;
  WriteCalibrationResultToFile(trackerLagOutputFilename.str(), calibResult);
  
  if (plotResults)
  {
    vtkSmartPointer<vtkTable> videoPositionMetric=vtkSmartPointer<vtkTable>::New();
    testTemporalCalibrationObject->GetFixedPositionSignal(videoPositionMetric);

    // Uncalibrated
    vtkSmartPointer<vtkTable> uncalibratedTrackerPositionMetric=vtkSmartPointer<vtkTable>::New();
    testTemporalCalibrationObject->GetUncalibratedMovingPositionSignal(uncalibratedTrackerPositionMetric);
    std::string filename=intermediateFileOutputDirectory + "/MetricPlotUncalibrated.png";

    std::string xLabel = "Time [s]";
    std::string yLabel = "Position Metric";
    SaveMetricPlot(filename.c_str(), videoPositionMetric, uncalibratedTrackerPositionMetric, xLabel, yLabel);
  
    // Calibrated
    vtkSmartPointer<vtkTable> calibratedTrackerPositionMetric=vtkSmartPointer<vtkTable>::New();
    testTemporalCalibrationObject->GetCalibratedMovingPositionSignal(calibratedTrackerPositionMetric);
    filename=intermediateFileOutputDirectory + "/MetricPlotCalibrated.png";
    SaveMetricPlot(filename.c_str(), videoPositionMetric, calibratedTrackerPositionMetric,  xLabel, yLabel);

    // Correlation Signal
    vtkSmartPointer<vtkTable> correlationSignal = vtkSmartPointer<vtkTable>::New();
    testTemporalCalibrationObject->GetCorrelationSignal(correlationSignal);
    vtkSmartPointer<vtkTable> correlationSignalFine = vtkSmartPointer<vtkTable>::New();
    testTemporalCalibrationObject->GetCorrelationSignalFine(correlationSignalFine);
    filename = intermediateFileOutputDirectory + "/CorrelationSignal.png";
    xLabel = "Tracker Offset [s]"; 
    yLabel = "Correlation Value";
    SaveMetricPlot(filename.c_str(), correlationSignal, correlationSignalFine, xLabel, yLabel);
  }

    // Compare result to baseline
  if (!inputBaselineFileName.empty())
  {
    LOG_INFO("Comparing result with baseline..."); 
    TemporalCalibrationResult baselineCalibResult;
    if (ReadCalibrationResultsFromFile(inputBaselineFileName, baselineCalibResult)!=PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to read baseline data file");
      exit(EXIT_FAILURE);
    }
    int numberOfFailures=CompareCalibrationResults(calibResult, baselineCalibResult);
    if (numberOfFailures>0)
    {
      LOG_ERROR("Number of differences compared to baseline: "<<numberOfFailures<<". Test failed!");
      exit(EXIT_FAILURE);
    }
    LOG_INFO("Baseline comparison completed successfully");
  }

  testTemporalCalibrationObject->Delete();

  return EXIT_SUCCESS;
}
