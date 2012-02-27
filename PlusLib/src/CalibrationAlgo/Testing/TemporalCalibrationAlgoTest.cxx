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
  //TemporalCalibration testTemporalCalibrationObject;
  //testTemporalCalibrationObject.SetTrackerFrames(trackerFrames);
  //testTemporalCalibrationObject.SetVideoFrames(videoFrames);
  //testTemporalCalibrationObject.setSamplingResolutionSec(0.001);

  //  Calculate the time-offset
  //if (testTemporalCalibrationObject.Update()!=PLUS_SUCCESS)
  //{
  //  LOG_ERROR("Cannot determine tracker lag, temporal calibration failed");
  //  exit(EXIT_FAILURE);
  //}

  //double trackerLagSec=0;
  //if (testTemporalCalibrationObject.GetTrackerLagSec(trackerLagSec)!=PLUS_SUCCESS)
  //{
  //  LOG_ERROR("Cannot determine tracker lag, temporal calibration failed");
  //  exit(EXIT_FAILURE);
  //}

  //LOG_DEBUG("Tracker lag: " << trackerLagSec << " sec (>0 if the tracker data lags)");

  return EXIT_SUCCESS;
}
