/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

/*!
  \file vtk3DDataCollectorTest1.cxx 
  \brief This program acquires tracked ultrasound data and displays it on the screen (in a 2D viewer).
*/ 

#include "PlusConfigure.h"
#include "TrackedFrame.h"
#include "itkMath.h"
#include "vtkDataCollector.h"
#include "vtkImageData.h" 
#include "vtkImageHistogramStatistics.h"
#include "vtkMatrix4x4.h"
#include "vtkPlusChannel.h"
#include "vtkPlusDataSource.h"
#include "vtkPlusDevice.h"
#include "vtkSavedDataSource.h"
#include "vtkTrackedFrameList.h"
#include "vtkVirtualMixer.h"
#include "vtkXMLUtilities.h"
#include "vtksys/CommandLineArguments.hxx"

int main(int argc, char **argv)
{
  std::string inputConfigFileName;
  double minExpected(-1.0), maxExpected(-1.0), meanExpected(-1.0), stdDevExpected(-1.0), medianExpected(-1.0);
  double xDimension(-1.0), yDimension(-1.0), zDimension(-1.0);
  int verboseLevel=vtkPlusLogger::LOG_LEVEL_UNDEFINED;

  vtksys::CommandLineArguments args;
  args.Initialize(argc, argv);

  args.AddArgument("--config-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputConfigFileName, "Name of the input configuration file.");
  args.AddArgument("--minimum", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &minExpected, "Minimum pixel value expected.");
  args.AddArgument("--maximum", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &maxExpected, "Maximum pixel value expected.");
  args.AddArgument("--mean", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &meanExpected, "Mean pixel value expected.");
  args.AddArgument("--standard-deviation", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &stdDevExpected, "Standard deviation from the mean expected.");
  args.AddArgument("--median", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &medianExpected, "Median pixel value expected.");
  args.AddArgument("--xDimension", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &xDimension, "Expected size of the data in the X dimension.");
  args.AddArgument("--yDimension", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &yDimension, "Expected size of the data in the Y dimension.");
  args.AddArgument("--zDimension", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &zDimension, "Expected size of the data in the Z dimension.");
  args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug, 5=trace)");  

  if ( !args.Parse() )
  {
    std::cerr << "Problem parsing arguments" << std::endl;
    std::cout << "Help: " << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }
  
  vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);

  if (inputConfigFileName.empty())
  {
    LOG_ERROR("input-config-file-name is required");
    exit(EXIT_FAILURE);
  }

  if( minExpected == -1.0 )
  {
    LOG_ERROR("minimum is required");
    exit(EXIT_FAILURE);
  }
  if( maxExpected == -1.0 )
  {
    LOG_ERROR("maximum is required");
    exit(EXIT_FAILURE);
  }
  if( meanExpected == -1.0 )
  {
    LOG_ERROR("mean is required");
    exit(EXIT_FAILURE);
  }
  if( stdDevExpected == -1.0 )
  {
    LOG_ERROR("standard-deviation is required");
    exit(EXIT_FAILURE);
  }
  if( medianExpected == -1.0 )
  {
    LOG_ERROR("median is required");
    exit(EXIT_FAILURE);
  }
  if( xDimension == -1.0 )
  {
    LOG_ERROR("xDimension is required");
    exit(EXIT_FAILURE);
  }
  if( yDimension == -1.0 )
  {
    LOG_ERROR("yDimension is required");
    exit(EXIT_FAILURE);
  }
  if( zDimension == -1.0 )
  {
    LOG_ERROR("zDimension is required");
    exit(EXIT_FAILURE);
  }

  ///////////////

  vtkSmartPointer<vtkXMLDataElement> configRootElement = vtkSmartPointer<vtkXMLDataElement>::New();
  if (PlusXmlUtils::ReadDeviceSetConfigurationFromFile(configRootElement, inputConfigFileName.c_str())==PLUS_FAIL)
  {  
    LOG_ERROR("Unable to read configuration from file " << inputConfigFileName.c_str()); 
    return EXIT_FAILURE;
  }
  
  vtkPlusConfig::GetInstance()->SetDeviceSetConfigurationData(configRootElement);

  vtkSmartPointer<vtkDataCollector> dataCollector = vtkSmartPointer<vtkDataCollector>::New(); 

  if( dataCollector->ReadConfiguration( configRootElement ) != PLUS_SUCCESS )
  {
    LOG_ERROR("Configuration incorrect for vtkDataCollectorTest1.");
    exit( EXIT_FAILURE );
  }
  vtkPlusDevice* videoDevice(NULL);
  vtkPlusDevice* trackerDevice(NULL);
  vtkPlusDevice* trackedVideoDevice(NULL);

  if( dataCollector->GetDevice(videoDevice, "VideoDevice") != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to locate the device with Id=\"VideoDevice\". Check config file.");
    exit(EXIT_FAILURE);
  }
  vtkSavedDataSource* videoSource = dynamic_cast<vtkSavedDataSource*>(videoDevice); 
  if ( videoSource == NULL )
  {
    LOG_ERROR( "Unable to cast video source to vtkSavedDataSource." );
    exit( EXIT_FAILURE );
  }

  if( dataCollector->GetDevice(trackerDevice, "TrackerDevice") != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to locate the device with Id=\"TrackerDevice\". Check config file.");
    exit(EXIT_FAILURE);
  }
  vtkSavedDataSource* tracker = dynamic_cast<vtkSavedDataSource*>(trackerDevice); 
  if ( tracker == NULL )
  {
    LOG_ERROR( "Unable to cast tracker to vtkSavedDataSource" );
    exit( EXIT_FAILURE );
  }

  if( dataCollector->GetDevice(trackedVideoDevice, "TrackedVideoDevice") != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to locate the device with Id=\"TrackedVideoDevice\". Check config file.");
    exit(EXIT_FAILURE);
  }
  vtkVirtualMixer* mixer = dynamic_cast<vtkVirtualMixer*>(trackedVideoDevice);
  if( mixer == NULL )
  {
    LOG_ERROR( "Unable to cast tracked video device to vtkVirtualMixer" );
    exit( EXIT_FAILURE );
  }

  if ( dataCollector->Connect() != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to connect to devices!" ); 
    exit( EXIT_FAILURE );
  }

  if ( dataCollector->Start() != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to start data collection!" ); 
    exit( EXIT_FAILURE );
  }

  // Wait an amount of time to enable some data collection to happen
#ifdef _WIN32
  Sleep(1*1000);
#else
  usleep(1 * 1000000);
#endif

  vtkSmartPointer<vtkTrackedFrameList> frameList = vtkSmartPointer<vtkTrackedFrameList>::New();
  double timestamp(0.0);
  mixer->GetChannel()->GetOldestTimestamp(timestamp);
  if( mixer->GetChannel()->GetTrackedFrameList(timestamp, frameList, 20) != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to retrieve frames from virtual mixer.");
    exit( EXIT_FAILURE );
  }

  vtkImageData* data = frameList->GetTrackedFrame(9)->GetImageData()->GetImage(); // Get the 10th frame's image data

  int dimensions[3];
  data->GetDimensions(dimensions);

  if( xDimension != dimensions[0] || yDimension != dimensions[1] || zDimension != dimensions[2] )
  {
    LOG_ERROR("Dimensions don't match. Got [" << dimensions[0] << "," << dimensions[1] << "," << dimensions[2] << "]. Expected [" << xDimension << "," << yDimension << "," << zDimension << "]");
    exit(EXIT_FAILURE);
  }


  vtkSmartPointer<vtkImageHistogramStatistics> stats = vtkSmartPointer<vtkImageHistogramStatistics>::New();
  stats->SetInputData_vtk5compatible(data);
  stats->GenerateHistogramImageOff();
  stats->Update();

  double minVal = stats->GetMinimum();
  double maxVal = stats->GetMaximum();
  double meanVal = stats->GetMean();
  double median = stats->GetMedian();
  double stdDev = stats->GetStandardDeviation();

  int numErrors(0);
  if( !itk::Math::FloatAlmostEqual<double>(minVal, minExpected) )
  {
    LOG_ERROR("Min values don't match. Got: " << minVal << ". Expected: " << minExpected);
    numErrors++;
  }

  if( !itk::Math::FloatAlmostEqual<double>(maxVal, maxExpected) )
  {
    LOG_ERROR("Max values don't match. Got: " << maxVal << ". Expected: " << maxExpected);
    numErrors++;
  }

  if( !itk::Math::FloatAlmostEqual<double>(meanVal, meanExpected) )
  {
    LOG_ERROR("Mean values don't match. Got: " << meanVal << ". Expected: " << meanExpected);
    numErrors++;
  }

  if( !itk::Math::FloatAlmostEqual<double>(median, medianExpected) )
  {
    LOG_ERROR("Median values don't match. Got: " << median << ". Expected: " << medianExpected);
    numErrors++;
  }

  if( !itk::Math::FloatAlmostEqual<double>(stdDev, stdDevExpected) )
  {
    LOG_ERROR("Standard deviation values don't match. Got: " << stdDev << ". Expected: " << stdDevExpected);
    numErrors++;
  }

  dataCollector->Disconnect();

  if( numErrors > 0 )
  {
    return EXIT_FAILURE;
  }

  LOG_INFO("vtk3DDataCollectorTest1 completed successfully!");
  return EXIT_SUCCESS; 

}
