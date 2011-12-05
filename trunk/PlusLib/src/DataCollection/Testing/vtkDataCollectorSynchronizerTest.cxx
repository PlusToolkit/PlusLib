/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

/*!
  \file This program tests the temporal calibration algorithm and compares to a baseline.
*/ 

#include "PlusConfigure.h"
#include "vtksys/CommandLineArguments.hxx"
#include <iomanip>

#include "vtkSmartPointer.h"
#include "vtkMatrix4x4.h"
#include "vtkTrackerBuffer.h"
#include "vtkVideoBuffer.h"
#include "vtkDataCollectorSynchronizer.h"
#include "vtkHTMLGenerator.h"
#include "vtkGnuplotExecuter.h"
#include "vtksys/SystemTools.hxx"
#include "vtkTrackedFrameList.h"


#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

int main(int argc, char **argv)
{
  int numberOfErrors(0); 
  std::string inputVideoBufferSequenceFileName;
  std::string inputTrackerBufferSequenceFileName;
  std::string inputBaselineReportFilePath;
  double inputSyncStartTimestamp(0); 
  double synchronizationTimeLength(20); 
  int minNumOfSyncSteps(5); 
  int numberOfAveragedFrames(15); 
  int numberOfAveragedTransforms(20); 
  double minTransformThreshold(3.0); 
  double minFrameThreshold(10.0); 
  double maxFrameDifference(5.0); 
  double maxTransformDifference(2.0); 
  double thresholdMultiplier(5); 
  bool generateReport(false); 
  std::string inputTransformName; 

  int verboseLevel=vtkPlusLogger::LOG_LEVEL_DEFAULT;

  vtksys::CommandLineArguments args;
  args.Initialize(argc, argv);

  args.AddArgument("--input-transform-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputTransformName, "Transform name used for the test");
  args.AddArgument("--input-video-buffer-seq-file-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputVideoBufferSequenceFileName, "Filename of the input video buffer sequence metafile.");
  args.AddArgument("--input-tracker-buffer-seq-file-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputTrackerBufferSequenceFileName, "Filename of the input tracker bufffer sequence metafile.");
  args.AddArgument("--input-number-of-averaged-frames", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &numberOfAveragedFrames, "Number of averaged frames (Default: 15)");
  args.AddArgument("--input-number-of-averaged-transforms", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &numberOfAveragedTransforms, "Number of averaged transforms (Default: 20)");
  args.AddArgument("--input-threshold-multiplier", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &thresholdMultiplier, "Threshold multiplier (Default: 5)");
  args.AddArgument("--input-min-transform-threshold", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &minTransformThreshold, "Minimum transform threshold (Default: 3.0)");
  args.AddArgument("--input-min-frame-threshold", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &minFrameThreshold, "Minimum frame threshold (Default: 10.0)");
  args.AddArgument("--input-max-frame-difference", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &maxFrameDifference, "Maximum frame difference (Default: 5.0)");
  args.AddArgument("--input-max-transform-difference", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &maxTransformDifference, "Maximum transform difference (Default: 5.0)");
  args.AddArgument("--input-baseline-report-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputBaselineReportFilePath, "Baseline report file path");
  args.AddArgument("--generate-report", vtksys::CommandLineArguments::NO_ARGUMENT, &generateReport, "Generate HTML report (it assumes ../gnuplot/gnuplot.exe and ../scripts/ are valid)");

	args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug, 5=trace)");	

  if ( !args.Parse() )
  {
    std::cerr << "Problem parsing arguments" << std::endl;
    std::cout << "Help: " << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }

  if (inputVideoBufferSequenceFileName.empty() || inputTrackerBufferSequenceFileName.empty() )
  {
    std::cerr << "input-video-buffer-seq-file-name and input-tracker-buffer-seq-file-name arguments are required!" << std::endl;
    std::cout << "\nHelp: " << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }

  vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);

  PlusTransformName transformName; 
  if ( transformName.SetTransformName(inputTransformName.c_str())!= PLUS_SUCCESS )
  {
    LOG_ERROR("Invalid transform name: " << inputTransformName ); 
    return EXIT_FAILURE; 
  }

  // Read tracker buffer 
  LOG_INFO("Reading tracker buffer meta file..."); 
  vtkSmartPointer<vtkTrackedFrameList> trackerFrameList = vtkSmartPointer<vtkTrackedFrameList>::New(); 
  trackerFrameList->ReadFromSequenceMetafile(inputTrackerBufferSequenceFileName.c_str()); 

  LOG_INFO("Copy buffer to tracker buffer..."); 
  vtkSmartPointer<vtkTrackerBuffer> trackerBuffer = vtkSmartPointer<vtkTrackerBuffer>::New(); 
  if (trackerBuffer->CopyTransformFromTrackedFrameList(trackerFrameList, vtkTrackerBuffer::READ_FILTERED_AND_UNFILTERED_TIMESTAMPS,transformName)!=PLUS_SUCCESS)
  {
    LOG_ERROR("CopyDefaultTrackerDataToBuffer failed");
    return EXIT_FAILURE;
  }

  // Read video buffer 
  LOG_INFO("Reading video buffer meta file..."); 
  vtkSmartPointer<vtkTrackedFrameList> videoFrameList = vtkSmartPointer<vtkTrackedFrameList>::New(); 
  videoFrameList->ReadFromSequenceMetafile(inputVideoBufferSequenceFileName.c_str()); 

  vtkSmartPointer<vtkVideoBuffer> videoBuffer = vtkSmartPointer<vtkVideoBuffer>::New(); 
  if (videoBuffer->CopyImagesFromTrackedFrameList(videoFrameList, vtkVideoBuffer::READ_FILTERED_AND_UNFILTERED_TIMESTAMPS)!=PLUS_SUCCESS)
  {
    LOG_ERROR("CopyImagesFromTrackedFrameList failed");
    return EXIT_FAILURE;
  }

  LOG_INFO("Initialize synchronizer..."); 
  vtkSmartPointer<vtkDataCollectorSynchronizer> synchronizer = vtkSmartPointer<vtkDataCollectorSynchronizer>::New(); 
  synchronizer->SetSyncStartTime(inputSyncStartTimestamp); 
  synchronizer->SetSynchronizationTimeLength(synchronizationTimeLength); 
  synchronizer->SetMinNumOfSyncSteps(minNumOfSyncSteps);
  synchronizer->SetNumberOfAveragedFrames(numberOfAveragedFrames); 
  synchronizer->SetNumberOfAveragedTransforms(numberOfAveragedTransforms); 
  synchronizer->SetThresholdMultiplier(thresholdMultiplier); 
  synchronizer->SetMinFrameThreshold(minFrameThreshold); 
  synchronizer->SetMinTransformThreshold(minTransformThreshold); 
  synchronizer->SetMaxFrameDifference(maxFrameDifference); 
  synchronizer->SetMaxTransformDifference(maxTransformDifference); 
  synchronizer->SetTrackerBuffer(trackerBuffer); 
  synchronizer->SetVideoBuffer(videoBuffer); 
  synchronizer->SetProgressBarUpdateCallbackFunction(vtkPlusLogger::PrintProgressbar); 

  LOG_INFO("Synchronization Start Timestamp: " << std::fixed << inputSyncStartTimestamp); 
  LOG_INFO("Synchronization Time Length: " << synchronizationTimeLength << "s"); 
  LOG_INFO("Minimum Number Of Synchronization Steps: " << minNumOfSyncSteps); 
  LOG_INFO("Number Of Averaged Frames: " << numberOfAveragedFrames); 
  LOG_INFO("Minimum Frame Threshold: " << minFrameThreshold); 
  LOG_INFO("Maximum Frame Difference: " << maxFrameDifference); 
  LOG_INFO("Number Of Averaged Transforms: " << numberOfAveragedTransforms ); 
  LOG_INFO("Minimum Transform Threshold: " << minTransformThreshold);
  LOG_INFO("Maximum Transform Difference: " << maxTransformDifference); 
  LOG_INFO("Threshold Multiplier: " << thresholdMultiplier ); 
  LOG_INFO("Tracker Buffer Size: " << trackerBuffer->GetNumberOfItems() ); 
  LOG_INFO("Tracker Frame Rate: " << trackerBuffer->GetFrameRate() ); 
  LOG_INFO("Video Buffer Size: " << videoBuffer->GetNumberOfItems() ); 
  LOG_INFO("Video Frame Rate: " << videoBuffer->GetFrameRate() ); 

  if ( synchronizer->Synchronize() != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to synchronize buffers!"); 
    numberOfErrors++; 
  }

  vtkSmartPointer<vtkTable> syncReportTable = vtkSmartPointer<vtkTable>::New(); 
  if ( synchronizer->GetSyncReportTable(syncReportTable) != PLUS_SUCCESS )
  { 
    LOG_ERROR("Failed to get sync report table!"); 
    numberOfErrors++; 
  }

  std::string reportFile = vtksys::SystemTools::GetCurrentWorkingDirectory() + std::string("/DataCollectorSyncReport.txt"); 

  if ( vtkGnuplotExecuter::DumpTableToFileInGnuplotFormat( syncReportTable, reportFile.c_str() ) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to write table to file in gnuplot format!"); 
    return PLUS_FAIL; 
  }

  if (!inputBaselineReportFilePath.empty())
  {
	  if ( vtksys::SystemTools::FilesDiffer(reportFile.c_str(), inputBaselineReportFilePath.c_str() ) )
	  {
		LOG_ERROR("Sync report differ from baseline!"); 
		numberOfErrors++; 
	  }
  }
  else
  {
	  LOG_INFO("Comparison to baseline data is skipped, no baseline file was specified");
  }

  //************************************************************************************
  // Generate html report
  if ( generateReport )
  {
    std::string programPath("./");
    std::string errorMsg; 
    if ( !vtksys::SystemTools::FindProgramPath(argv[0], programPath, errorMsg) )
    {
      LOG_ERROR(errorMsg); 
    }
    programPath = vtksys::SystemTools::GetParentDirectory(programPath.c_str()); 

    vtkSmartPointer<vtkHTMLGenerator> htmlReport = vtkSmartPointer<vtkHTMLGenerator>::New(); 
    htmlReport->SetTitle("iCAL Temporal Calibration Report"); 

    vtkSmartPointer<vtkGnuplotExecuter> plotter = vtkSmartPointer<vtkGnuplotExecuter>::New(); 
    plotter->SetHideWindow(true); 

    // Synchronizer Analysis
    synchronizer->GenerateSynchronizationReport(htmlReport, plotter); 

    htmlReport->SaveHtmlPage("iCALTemporalCalibrationReport.html"); 
  }
  //************************************************************************************

  if ( numberOfErrors != 0 )
  {
    return EXIT_FAILURE; 
  }

  std::cout << "Test completed successfully!" << std::endl;
  return EXIT_SUCCESS; 

}
