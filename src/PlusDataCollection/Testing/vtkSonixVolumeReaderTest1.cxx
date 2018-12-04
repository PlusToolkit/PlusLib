/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "igsioVideoFrame.h"
#include "igsioTrackedFrame.h"
#include "vtkImageData.h"
#include "vtkImageDifference.h"
#include "vtkImageExtractComponents.h"
#include "vtkIGSIOSequenceIO.h"
#include "vtkSmartPointer.h"
#include "vtkPlusSonixVolumeReader.h"
#include "vtkIGSIOTrackedFrameList.h"
#include "vtksys/CommandLineArguments.hxx"
#include "vtksys/SystemTools.hxx"
#include <iostream>
#include <stdlib.h>

int main ( int argc, char* argv[] )
{
  bool printHelp( false );
  std::string inputFileName;
  std::string inputBaselineName;
  int inputFrameNumber( -1 );
  std::string outputFileName;

  vtksys::CommandLineArguments args;
  args.Initialize( argc, argv );

  int verboseLevel = vtkIGSIOLogger::LOG_LEVEL_ERROR;

  args.AddArgument( "--help", vtksys::CommandLineArguments::NO_ARGUMENT, &printHelp, "Print this help." );
  args.AddArgument( "--volume-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputFileName, "The file name of the Sonix volume." );
  args.AddArgument( "--output-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputFileName, "Sequence meta file name to save (save only if defined)" );
  args.AddArgument( "--frame-number", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputFrameNumber, "The frame number to compare with baseline." );
  args.AddArgument( "--baseline", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputBaselineName, "The file name of the baseline image." );
  args.AddArgument( "--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (Default: 1; 1=error only, 2=warning, 3=info, 4=debug)" );

  if ( !args.Parse() )
  {
    std::cerr << "Problem parsing arguments" << std::endl;
    std::cout << "\n\nvtkPlusICCapturingSourceTest1 help:" << args.GetHelp() << std::endl;
    exit( EXIT_FAILURE );
  }

  vtkIGSIOLogger::Instance()->SetLogLevel( verboseLevel );

  if ( printHelp )
  {
    std::cout << "\n\nvtkPlusICCapturingSourceTest1 help:" << args.GetHelp() << std::endl;
    exit( EXIT_SUCCESS );

  }

  if ( inputFileName.empty() )
  {
    LOG_ERROR( "The input-file-name  parameter is required!" );
    exit( EXIT_FAILURE );
  }


  vtkSmartPointer<vtkIGSIOTrackedFrameList> sonixVolumeData = vtkSmartPointer<vtkIGSIOTrackedFrameList>::New();

  if ( vtkPlusSonixVolumeReader::GenerateTrackedFrameFromSonixVolume( inputFileName.c_str(), sonixVolumeData ) != PLUS_SUCCESS )
  {
    LOG_ERROR( "Failed to generate tracked frame from sonix volume: " << inputFileName );
    exit( EXIT_FAILURE );
  }

  if ( !outputFileName.empty() )
  {
    std::string path = vtkPlusConfig::GetInstance()->GetOutputPath( outputFileName );
    LOG_INFO( "Save tracked frames to " << path );
    if( vtkIGSIOSequenceIO::Write( path, sonixVolumeData, sonixVolumeData->GetImageOrientation(), false ) != PLUS_SUCCESS )
    {
      LOG_ERROR( "Failed to save sonix volume to " << path );
    }
  }

  vtkSmartPointer<vtkImageDifference> imgDiff = vtkSmartPointer<vtkImageDifference>::New();

  igsioVideoFrame baselineVideoFrame;
  if ( igsioVideoFrame::ReadImageFromFile( baselineVideoFrame, inputBaselineName.c_str() ) != PLUS_SUCCESS )
  {
    LOG_ERROR( "Failed to read baseline image from file: " << inputBaselineName );
    exit( EXIT_FAILURE );
  }

  vtkSmartPointer<vtkImageExtractComponents> imageExtractorBase =  vtkSmartPointer<vtkImageExtractComponents>::New();
  imageExtractorBase->SetInputData( baselineVideoFrame.GetImage() );
  imageExtractorBase->SetComponents( 0, 0, 0 ); // we are using only the 0th component
  imageExtractorBase->Update();
  vtkImageData* baselineRGB = imageExtractorBase->GetOutput();

  if ( inputFrameNumber < 0 )
  {
    inputFrameNumber = 0;
  }
  unsigned int inputFrameNumberUint = static_cast<unsigned int>( inputFrameNumber );
  if ( sonixVolumeData->GetNumberOfTrackedFrames() < inputFrameNumberUint )
  {
    LOG_ERROR( "Unable to get tracked frame from list, frame number (" << inputFrameNumberUint
               << ") is larger than tracked frame list size (" << sonixVolumeData->GetNumberOfTrackedFrames() << ")!" );
    exit( EXIT_FAILURE );
  }

  igsioVideoFrame* videoFrame = sonixVolumeData->GetTrackedFrame( inputFrameNumberUint )->GetImageData();

  if ( !videoFrame->IsImageValid() )
  {
    LOG_ERROR( "Video frame is not valid!" );
    exit( EXIT_FAILURE );
  }

  vtkSmartPointer<vtkImageExtractComponents> imageExtractorInput =  vtkSmartPointer<vtkImageExtractComponents>::New();
  imageExtractorInput->SetInputData( videoFrame->GetImage() );
  imageExtractorInput->SetComponents( 0, 0, 0 ); // we are using only the 0th component

  imgDiff->SetImageData( baselineRGB );

  imgDiff->SetInputConnection( imageExtractorInput->GetOutputPort() );
  imgDiff->Update();

  double error = imgDiff->GetError();

  if ( error > 0 )
  {
    std::cout << "Error = " << error << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}