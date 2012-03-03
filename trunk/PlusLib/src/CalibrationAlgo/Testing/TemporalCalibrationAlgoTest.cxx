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

#include "TemporalCalibrationAlgo.h"

int main(int argc, char **argv)
{
  bool printHelp(false);
  int verboseLevel = vtkPlusLogger::LOG_LEVEL_DEFAULT;
  std::string inputTrackerSequenceMetafile; // Raw-buffer tracker file
  std::string inputVideoSequenceMetafile; // Corresponding raw-buffer video file
  double samplingResolutionSec = 0.001; //  Resolution used for re-sampling [s]

  vtksys::CommandLineArguments args;
  args.Initialize(argc, argv);

  args.AddArgument("--help",vtksys::CommandLineArguments::NO_ARGUMENT, &printHelp, "Print this help.");
  args.AddArgument("--input-video-sequence-metafile", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputVideoSequenceMetafile, "Input US image sequence metafile name with path");
  args.AddArgument("--input-tracker-sequence-metafile", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputTrackerSequenceMetafile, "Input tracker sequence metafile name with path");
  args.AddArgument("--verbose",vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug, 5=trace)");
  args.AddArgument("--sampling-resolution-sec",vtksys::CommandLineArguments::EQUAL_ARGUMENT, &samplingResolutionSec, "Sampling resolution (in seconds, default is 0.001)");    

  
  if ( !args.Parse() )
  {
    std::cerr << "Problem parsing arguments" << std::endl;
    std::cout << "Help: " << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }

  if ( printHelp )
  {
    std::cout << "Help: " << args.GetHelp() << std::endl;
    exit(EXIT_SUCCESS);
  }

  vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);

  if ( inputTrackerSequenceMetafile.empty() )
  {
    std::cerr << "input-tracker-sequence-metafile required argument!" << std::endl;
    std::cout << "Help: " << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }

  if ( inputVideoSequenceMetafile.empty() )
  {
    std::cerr << "input-video-sequence-metafile required argument!" << std::endl;
    std::cout << "Help: " << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }

  vtkSmartPointer<vtkTrackedFrameList> trackerFrames = vtkSmartPointer<vtkTrackedFrameList>::New();
  vtkSmartPointer<vtkTrackedFrameList> videoFrames = vtkSmartPointer<vtkTrackedFrameList>::New(); 
  
  //  Read tracker frames
  LOG_DEBUG("Read tracker data from "<<inputTrackerSequenceMetafile);
  if ( trackerFrames->ReadFromSequenceMetafile(inputTrackerSequenceMetafile.c_str()) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to read tracked pose sequence metafile: " << inputTrackerSequenceMetafile << ". Exiting...");
    exit(EXIT_FAILURE);
  }

  //  Read US video frames
  LOG_DEBUG("Read video data from "<<inputVideoSequenceMetafile);
  if ( videoFrames->ReadFromSequenceMetafile(inputVideoSequenceMetafile.c_str()) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to read US image sequence metafile: " << inputVideoSequenceMetafile << ". Exiting...");
    exit(EXIT_FAILURE);
  }
  
  //  Create temporal calibration object; Set pertinent parameters
  TemporalCalibration testTemporalCalibrationObject;
  testTemporalCalibrationObject.SetTrackerFrames(trackerFrames);
  testTemporalCalibrationObject.SetVideoFrames(videoFrames);
  testTemporalCalibrationObject.setSamplingResolutionSec(0.001);

  //  Calculate the time-offset
  if (testTemporalCalibrationObject.Update()!=PLUS_SUCCESS)
  {
    LOG_ERROR("Cannot determine tracker lag, temporal calibration failed");
    exit(EXIT_FAILURE);
  }

  double trackerLagSec=0;
  if (testTemporalCalibrationObject.GetTrackerLagSec(trackerLagSec)!=PLUS_SUCCESS)
  {
    LOG_ERROR("Cannot determine tracker lag, temporal calibration failed");
    exit(EXIT_FAILURE);
  }

  LOG_INFO("Tracker lag: " << trackerLagSec << " sec (>0 if the tracker data lags)");

  /* Plot the temporally uncalibrated tracker and video streams */
  vtkSmartPointer<vtkTable> videoPositionMetric = testTemporalCalibrationObject.GetVideoPositionSignal();
  vtkSmartPointer<vtkTable> trackerPositionMetric = testTemporalCalibrationObject.GetUncalibratedTrackerPositionSignal();

  // Set up the view
  vtkSmartPointer<vtkContextView> view = vtkSmartPointer<vtkContextView>::New();
  view->GetRenderer()->SetBackground(1.0, 1.0, 1.0);
 
  // Add the two line plots
  vtkSmartPointer<vtkChartXY> chart =  vtkSmartPointer<vtkChartXY>::New();
  view->GetScene()->AddItem(chart);
  vtkPlot *line = chart->AddPlot(vtkChart::LINE);

  #if VTK_MAJOR_VERSION <= 5
    line->SetInput(videoPositionMetric, 0, 1);
  #else
    line->SetInputData(table, 0, 1);
  #endif

  line->SetColor(1,0,0);
  line->SetWidth(1.0);
  line = chart->AddPlot(vtkChart::LINE);

  vtkPlot *line2 = chart->AddPlot(vtkChart::LINE);
  #if VTK_MAJOR_VERSION <= 5
    line2->SetInput(trackerPositionMetric, 0, 1);
  #else
    line2->SetInputData(table, 0, 1);
  #endif

  line2->SetColor(0,1,0);
  line2->SetWidth(1.0);
  line2 = chart->AddPlot(vtkChart::LINE);

  // Start interactor
  view->GetInteractor()->Initialize();
  view->GetInteractor()->Start();

  /* Plot the temporally calibrated tracker and video streams */
  vtkSmartPointer<vtkTable> calibratedTrackerPositionMetric = testTemporalCalibrationObject.GetCalibratedTrackerPositionSignal();

  // Set up the view
  vtkSmartPointer<vtkContextView> calibratedView = vtkSmartPointer<vtkContextView>::New();
  calibratedView->GetRenderer()->SetBackground(1.0, 1.0, 1.0);
 
  // Add the two line plots
  vtkSmartPointer<vtkChartXY> chartCalibrated =  vtkSmartPointer<vtkChartXY>::New();
  calibratedView->GetScene()->AddItem(chartCalibrated);
  vtkPlot *lineCalibrated = chartCalibrated ->AddPlot(vtkChart::LINE);

  #if VTK_MAJOR_VERSION <= 5
    lineCalibrated->SetInput(videoPositionMetric, 0, 1);
  #else
    line->SetInputData(table, 0, 1);
  #endif

  lineCalibrated->SetColor(0,0,1);
  lineCalibrated->SetWidth(1.0);
  lineCalibrated = chartCalibrated->AddPlot(vtkChart::LINE);

  vtkPlot *lineCalibrated2 = chartCalibrated->AddPlot(vtkChart::LINE);
  #if VTK_MAJOR_VERSION <= 5
    lineCalibrated2->SetInput(calibratedTrackerPositionMetric, 0, 1);
  #else
    line2->SetInputData(table, 0, 1);
  #endif

  lineCalibrated2->SetColor(1,0,0);
  lineCalibrated2->SetWidth(1.0);
  lineCalibrated2 = chartCalibrated->AddPlot(vtkChart::LINE);

  // Start interactor
  calibratedView->GetInteractor()->Initialize();
  calibratedView->GetInteractor()->Start();

  return EXIT_SUCCESS;
}
