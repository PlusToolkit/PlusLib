/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#include "PlusConfigure.h"
#include "TrackedFrame.h"
#include "vtkImageData.h" 
#include "vtkMetaImageSequenceIO.h"
#include "vtkRfProcessor.h"
#include "vtkSmartPointer.h"
#include "vtkTrackedFrameList.h"
#include "vtkTransform.h"
#include "vtkXMLUtilities.h"
#include "vtksys/CommandLineArguments.hxx"
#include "vtksys/SystemTools.hxx"
#include <iomanip>
#include <iostream>


//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
  std::string inputRfFile;
  std::string inputConfigFile;
  std::string outputImgFile;
  std::string operation="BRIGHTNESS_SCAN_CONVERT";

  int verboseLevel=vtkPlusLogger::LOG_LEVEL_UNDEFINED;

  vtksys::CommandLineArguments args;
  args.Initialize(argc, argv);

  args.AddArgument("--rf-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputRfFile, "File name of input RF image data");
  args.AddArgument("--config-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputConfigFile, "Config file containing processing parameters");
  args.AddArgument("--output-img-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &outputImgFile, "File name of the generated output brightness image");
  args.AddArgument("--operation", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &operation, "Processing operation to be applied on the input file (BRIGHTNESS_CONVERT, BRIGHTNESS_SCAN_CONVERT, default: BRIGHTNESS_SCAN_CONVERT");
  args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug, 5=trace)");


  // Input arguments error checking
  if ( !args.Parse() )
  {
    std::cerr << "Problem parsing arguments" << std::endl;
    std::cout << "Help: " << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }

  vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);

  if (inputConfigFile.empty())
  {
    std::cerr << "--config-file required " << std::endl;
    exit(EXIT_FAILURE);
  }
  if (inputRfFile.empty())
  {
    std::cerr << "--rf-file required" << std::endl;
    exit(EXIT_FAILURE);
  }
  if (outputImgFile.empty())
  {
    std::cerr << "Missing --output-img-file parameter. Specification of the output image file name is required." << std::endl;
    exit(EXIT_FAILURE);
  }

  // Read transformations data 
  LOG_DEBUG("Reading input meta file..."); 
  // frameList it will contain initially the RF data and the image data will be replaced by the processed output
  vtkSmartPointer< vtkTrackedFrameList > frameList = vtkSmartPointer< vtkTrackedFrameList >::New();
  frameList->ReadFromSequenceMetafile( inputRfFile.c_str() );
  LOG_DEBUG("Reading input RF file completed"); 

  // Read config file
  LOG_DEBUG("Reading config file...")
  vtkSmartPointer<vtkXMLDataElement> configRead = vtkSmartPointer<vtkXMLDataElement>::Take(::vtkXMLUtilities::ReadElementFromFile(inputConfigFile.c_str())); 
  LOG_DEBUG("Reading config file finished.");

  vtkXMLDataElement* dataCollectionConfig = configRead->FindNestedElementWithName("DataCollection");
  if (dataCollectionConfig == NULL)
  {
    LOG_ERROR("Cannot find DataCollection element in XML tree!");
    return PLUS_FAIL;
  }
  vtkXMLDataElement* deviceConfig = dataCollectionConfig->FindNestedElementWithName("Device");
  if (deviceConfig == NULL)
  {
    LOG_ERROR("Cannot find DataCollection/ImageAcquisition element in XML tree!");
    return PLUS_FAIL;
  }
  vtkXMLDataElement* outputChannelsElement = deviceConfig->FindNestedElementWithName("OutputChannels");
  if (outputChannelsElement == NULL)
  {
    LOG_ERROR("Cannot find OutputChannels element in device tag tree!");
    return PLUS_FAIL;
  }

  for ( int i = 0; i < outputChannelsElement->GetNumberOfNestedElements(); ++i )
  {
    vtkXMLDataElement* outputChannelElement = outputChannelsElement->GetNestedElement(i); 

    if (STRCASECMP(outputChannelElement->GetName(), "OutputChannel") != 0 )
    {
      continue;
    }

    // Create converter
    vtkSmartPointer<vtkRfProcessor> rfProcessor = vtkSmartPointer<vtkRfProcessor>::New(); 
    if ( rfProcessor->ReadConfiguration(outputChannelElement) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to read conversion parameters from the configuration file"); 
      exit(EXIT_FAILURE); 
    }

    // Process the frames
    for (unsigned int j = 0; j < frameList->GetNumberOfTrackedFrames(); j++)
    {
      TrackedFrame* rfFrame = frameList->GetTrackedFrame(j);

      // Do the conversion
      rfProcessor->SetRfFrame(rfFrame->GetImageData()->GetVtkImage(), rfFrame->GetImageData()->GetImageType());

      if (STRCASECMP(operation.c_str(),"BRIGHTNESS_CONVERT")==0)
      {
        // do brightness conversion only
        vtkImageData* brightnessImage = rfProcessor->GetBrightessConvertedImage();
        // Update the pixel data in the frame
        rfFrame->GetImageData()->DeepCopyFrom(brightnessImage);  
        rfFrame->GetImageData()->SetImageType(US_IMG_BRIGHTNESS);
      }
      else if (STRCASECMP(operation.c_str(),"BRIGHTNESS_SCAN_CONVERT")==0)
      {
        // do brightness and scan conversion
        vtkImageData* brightnessImage = rfProcessor->GetBrightessScanConvertedImage();
        // Update the pixel data in the frame
        rfFrame->GetImageData()->DeepCopyFrom(brightnessImage);    
        rfFrame->GetImageData()->SetImageOrientation(US_IMG_ORIENT_MF); 
        rfFrame->GetImageData()->SetImageType(US_IMG_BRIGHTNESS);
      }
      else
      {
        LOG_ERROR("Unknown operation: "<<operation);
        exit(EXIT_FAILURE);
      }
    }

    vtkSmartPointer<vtkMetaImageSequenceIO> outputImgSeqFileWriter = vtkSmartPointer<vtkMetaImageSequenceIO>::New();
    std::stringstream ss;
    std::string path = vtksys::SystemTools::GetFilenamePath(outputImgFile);
    if( !path.empty() )
    {
      ss << path << "/";
    }
    if( outputChannelElement->GetAttribute("Id") != NULL )
    {
      ss << vtksys::SystemTools::GetFilenameWithoutExtension(outputImgFile) << "_OutputChannel_" << outputChannelElement->GetAttribute("Id") << vtksys::SystemTools::GetFilenameExtension(outputImgFile);
    }
    else
    {
      ss << vtksys::SystemTools::GetFilenameWithoutExtension(outputImgFile) << "_OutputChannel_" << i << vtksys::SystemTools::GetFilenameExtension(outputImgFile);
    }
    outputImgSeqFileWriter->SetFileName(ss.str().c_str()); 
    outputImgSeqFileWriter->SetTrackedFrameList(frameList); 
    outputImgSeqFileWriter->SetImageOrientationInFile(frameList->GetImageOrientation());
    outputImgSeqFileWriter->Write(); 
  }

	return EXIT_SUCCESS; 
}
