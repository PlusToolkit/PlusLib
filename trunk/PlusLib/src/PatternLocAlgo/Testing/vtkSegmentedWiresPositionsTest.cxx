/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"

#include "vtkSmartPointer.h"
#include "vtkCommand.h"
#include "vtksys/CommandLineArguments.hxx" 
#include "vtkMatrix4x4.h"
#include "vtkTransform.h"
#include "vtkMath.h"
#include "vtkProbeCalibrationAlgo.h"
#include "TrackedFrame.h"
#include "vtkTrackedFrameList.h"

#include "FidPatternRecognition.h"

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include <stdlib.h>
#include <iostream>

typedef unsigned char PixelType;
typedef itk::Image< PixelType, 2 > ImageType;
typedef itk::Image< PixelType, 3 > ImageSequenceType;
typedef itk::ImageFileReader< ImageSequenceType > ImageSequenceReaderType;

///////////////////////////////////////////////////////////////////

int main (int argc, char* argv[])
{ 
	std::string inputSequenceMetafile;
  std::string inputTransformName; 
	std::string outputWirePositionFile("./SegmentedWirePositions.txt");
	int inputImageType(1); 

	std::string inputBaselineFileName;
	double inputTranslationErrorThreshold(0); 
	double inputRotationErrorThreshold(0); 

	int verboseLevel=vtkPlusLogger::LOG_LEVEL_UNDEFINED;

	vtksys::CommandLineArguments cmdargs;
	cmdargs.Initialize(argc, argv);

	cmdargs.AddArgument("--image-seq-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputSequenceMetafile, "Image sequence metafile");
	cmdargs.AddArgument("--image-position-transform", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputTransformName, "Transform name used for image position display");
	cmdargs.AddArgument("--image-type", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputImageType, "Image type (1=SonixVideo, 2=FrameGrabber - Default: SonixVideo");	
	cmdargs.AddArgument("--output-wire-position-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputWirePositionFile, "Result wire position file name (Default: ./SegmentedWirePositions.txt)");

	cmdargs.AddArgument("--baseline", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputBaselineFileName, "Name of file storing baseline calibration results");
	cmdargs.AddArgument("--translation-error-threshold", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputTranslationErrorThreshold, "Translation error threshold in mm.");	
	cmdargs.AddArgument("--rotation-error-threshold", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputRotationErrorThreshold, "Rotation error threshold in degrees.");	
	cmdargs.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug, 5=trace)");	

	if ( !cmdargs.Parse() )
	{
		std::cerr << "Problem parsing arguments" << std::endl;
		std::cout << "Help: " << cmdargs.GetHelp() << std::endl;
		exit(EXIT_FAILURE);
	}
  
  vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);

	if ( inputSequenceMetafile.empty() ) 
	{
		std::cerr << "image-seq-file argument required" << std::endl << std::endl;
		std::cout << "Help: " << cmdargs.GetHelp() << std::endl;
		exit(EXIT_FAILURE);

	}	

	int SearchRegionXMin(0), SearchRegionXSize(0), SearchRegionYMin(0), SearchRegionYSize(0); 

	switch ( inputImageType )
	{
	case 1: // SonixVideo frames
		SearchRegionXMin = 30; 
		SearchRegionXSize = 565; 
		SearchRegionYMin = 50; 
		SearchRegionYSize = 370;

		break; 
	case 2: // FrameGrabber frames
		SearchRegionXMin = 100; 
		SearchRegionXSize = 360; 
		SearchRegionYMin = 65; 
		SearchRegionYSize = 280;    
		break; 
	}

	LOG_INFO( "Reading sequence meta file");  
  vtkSmartPointer<vtkTrackedFrameList> trackedFrameList = vtkSmartPointer<vtkTrackedFrameList>::New(); 
  if ( trackedFrameList->ReadFromSequenceMetafile(inputSequenceMetafile.c_str()) != PLUS_SUCCESS )
  {
      LOG_ERROR("Failed to read sequence metafile: " << inputSequenceMetafile); 
      return EXIT_FAILURE;
  }

	std::ofstream positionInfo;
	positionInfo.open (outputWirePositionFile.c_str(), ios::out );

	LOG_INFO( "Segmenting frames..."); 

	for ( int imgNumber = 0; imgNumber < trackedFrameList->GetNumberOfTrackedFrames(); imgNumber++ )
	{
		vtkPlusLogger::PrintProgressbar( (100.0 * imgNumber) / trackedFrameList->GetNumberOfTrackedFrames() ); 

    FidPatternRecognition patternRecognition;
    PatternRecognitionResult segResults;
    PatternRecognitionError error;

		try
		{
			// Send the image to the Segmentation component to segment
      if (trackedFrameList->GetTrackedFrame(imgNumber)->GetImageData()->GetITKScalarPixelType()!=itk::ImageIOBase::UCHAR)
      {
        LOG_ERROR("patternRecognition.RecognizePattern only works on unsigned char images");
      }
      else
      {
        patternRecognition.RecognizePattern( trackedFrameList->GetTrackedFrame(imgNumber), segResults, error, imgNumber );
      }
  	}
		catch(...)
		{
			LOG_ERROR("SegmentImage: The segmentation has failed for due to UNKNOWN exception thrown, the image was ignored!!!"); 
			continue; 
		}

    PlusTransformName transformName; 
    if ( transformName.SetTransformName(inputTransformName.c_str()) != PLUS_SUCCESS )
    {
      LOG_ERROR("Invalid transform name: " << inputTransformName ); 
      return EXIT_FAILURE; 
    }
   
    if ( segResults.GetDotsFound() )
		{
      double defaultTransform[16]={0}; 
      if ( trackedFrameList->GetTrackedFrame(imgNumber)->GetCustomFrameTransform(transformName, defaultTransform) != PLUS_SUCCESS )
      {
        LOG_ERROR("Failed to get default frame transform from tracked frame #" << imgNumber); 
        continue; 
      }

      vtkSmartPointer<vtkTransform> frameTransform = vtkSmartPointer<vtkTransform>::New(); 
      frameTransform->SetMatrix(defaultTransform); 

			double posZ = frameTransform->GetPosition()[2]; 
			double rotZ = frameTransform->GetOrientation()[2]; 

			int dataType = -1; 
			positionInfo << dataType << "\t\t" << posZ << "\t" << rotZ << "\t\t"; 

			for (int i=0; i < segResults.GetFoundDotsCoordinateValue().size(); i++)
			{
				positionInfo << segResults.GetFoundDotsCoordinateValue()[i][0] << "\t" << segResults.GetFoundDotsCoordinateValue()[i][1] << "\t\t"; 
			}

			positionInfo << std::endl; 

		}
	}
	
	positionInfo.close(); 
	vtkPlusLogger::PrintProgressbar(100); 
	std::cout << std::endl; 

	std::cout << "Exit success!!!" << std::endl; 
	return EXIT_SUCCESS; 
}
