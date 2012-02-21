/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

/*!
  \file TemporalCalibrationAlgoTest.cxx
  \brief This program tests the temporal calibraiton algo by computing the time offset between a video and a tracking data. The input files shall be acquired while imaging a plane and moving the transducer periodically. 
*/ 

#include "TemporalCalibrationAlgo.h"

int main(int argc, char **argv)
{
  bool printHelp(false);
  int verboseLevel = vtkPlusLogger::LOG_LEVEL_DEFAULT;
  std::string inputTrackerSequenceMetafile;
  std::string inputVideoSequenceMetafile;
  std::string outputFilepath;
  double samplingResolutionSec = 0.001; //  Resolution used for re-sampling [seconds]

  vtksys::CommandLineArguments args;
  args.Initialize(argc, argv);

  args.AddArgument("--help",vtksys::CommandLineArguments::NO_ARGUMENT, &printHelp, "Print this help.");
  args.AddArgument("--input-video-sequence-metafile", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputVideoSequenceMetafile, "Input US image sequence metafile name with path");
  args.AddArgument("--input-tracker-sequence-metafile", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputTrackerSequenceMetafile, "Input tracker sequence metafile name with path");
  args.AddArgument("--verbose",vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug, 5=trace)");
  args.AddArgument("--sampling-resolution-sec",vtksys::CommandLineArguments::EQUAL_ARGUMENT, &samplingResolutionSec, "Sampling resolution (in seconds, default is 0.001)");    
  args.AddArgument("--output-filepath", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputFilepath, "Filepath where the output files will be written");

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
  vtkSmartPointer<vtkTrackedFrameList> USVideoFrames = vtkSmartPointer<vtkTrackedFrameList>::New(); 

  //  read tracker frames
  if ( trackerFrames->ReadFromSequenceMetafile(inputTrackerSequenceMetafile.c_str()) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to read tracked pose sequence metafile: " << inputTrackerSequenceMetafile);
    exit(EXIT_FAILURE);
  }

  //  read US video frames
  if ( USVideoFrames->ReadFromSequenceMetafile(inputVideoSequenceMetafile.c_str()) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to read US image sequence metafile: " << inputVideoSequenceMetafile);
    exit(EXIT_FAILURE);
  }


  vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);
  TemporalCalibration testTemporalCalibrationObject;

  testTemporalCalibrationObject.SetTrackerFrames(trackerFrames);
  testTemporalCalibrationObject.SetUSVideoFrames(USVideoFrames);
  testTemporalCalibrationObject.setSamplingResolutionSec(0.001);

  
  //  Calculate the time-offset
  testTemporalCalibrationObject.CalculateTrackerLagSec();

  LOG_DEBUG("Time offset: " << testTemporalCalibrationObject.GetTrackerLagSec() << " sec (>0 if the tracker data lags)");


  return 0;
}
