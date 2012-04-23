/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

/*!
  \file vtkDataCollectorFileTest.cxx
  \brief This program tests if a recorded tracked ultrasound buffer can be read and replayed from file using vtkDataCollectorFile
*/ 

#include "vtkSmartPointer.h"
#include "vtksys/CommandLineArguments.hxx"
#include "vtkXMLUtilities.h"
#include "vtkMatrix4x4.h"

#include "PlusConfigure.h"
#include "vtkDataCollectorFile.h"
#include "vtkTransformRepository.h"
#include "TrackedFrame.h"

static const int COMPARE_TRANSFORM_TOLERANCE=0.001;

PlusStatus CompareTransform(PlusTransformName &transformName, vtkTransformRepository* transformRepository, double xExpected, double yExpected, double zExpected)
{
  vtkSmartPointer<vtkMatrix4x4> transformMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
  bool valid=false;
  if ( (transformRepository->GetTransform(transformName, transformMatrix , &valid) != PLUS_SUCCESS) || (!valid) )
  {
    std::string transformNameStr;
    transformName.GetTransformName(transformNameStr);
    LOG_ERROR("Unable to get transform " << transformNameStr);
    return PLUS_FAIL;
  }
  PlusStatus status=PLUS_SUCCESS;
  std::string transformNameStr;
  transformName.GetTransformName(transformNameStr);
  double actualValue=transformMatrix ->GetElement(0,3);
  if ( fabs(actualValue - xExpected) > COMPARE_TRANSFORM_TOLERANCE )
  {    
    LOG_ERROR("Transform " << transformNameStr << " x translation does not match (actual="<<actualValue<<", expected="<<xExpected<<")");
    status=PLUS_FAIL;
  }
  actualValue=transformMatrix ->GetElement(1,3);
  if ( fabs(actualValue - yExpected) > COMPARE_TRANSFORM_TOLERANCE )
  {    
    LOG_ERROR("Transform " << transformNameStr << " y translation does not match (actual="<<actualValue<<", expected="<<yExpected<<")");
    status=PLUS_FAIL;
  }
  actualValue=transformMatrix ->GetElement(2,3);
  if ( fabs(actualValue - zExpected) > COMPARE_TRANSFORM_TOLERANCE )
  {    
    LOG_ERROR("Transform " << transformNameStr << " z translation does not match (actual="<<actualValue<<", expected="<<zExpected<<")");
    status=PLUS_FAIL;
  }
  return status;
}

int main( int argc, char** argv )
{

  // Check command line arguments.
  std::string  inputConfigFileName;
  int          verboseLevel = vtkPlusLogger::LOG_LEVEL_DEFAULT;

  vtksys::CommandLineArguments args;
  args.Initialize( argc, argv );

  args.AddArgument( "--input-config-file-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputConfigFileName, "Name of the input configuration file." );
  args.AddArgument( "--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug 5=trace)" );  

  if ( ! args.Parse() )
  {
    std::cerr << "Problem parsing arguments." << std::endl;
    std::cout << "Help: " << args.GetHelp() << std::endl;
    return 1;
  }

  vtkPlusLogger::Instance()->SetLogLevel( verboseLevel );

  // Prepare and start data collection
  vtkSmartPointer<vtkXMLDataElement> configRootElement = vtkSmartPointer<vtkXMLDataElement>::Take(vtkXMLUtilities::ReadElementFromFile(inputConfigFileName.c_str()));
  if (configRootElement == NULL)
  {	
    LOG_ERROR("Unable to read configuration from file " << inputConfigFileName.c_str()); 
    return 1;
  }

  vtkPlusConfig::GetInstance()->SetDeviceSetConfigurationData(configRootElement);

  vtkSmartPointer<vtkDataCollector> dataCollector = vtkSmartPointer<vtkDataCollector>::New();

  vtkDataCollectorFile* dataCollectorFile = dynamic_cast<vtkDataCollectorFile*>(dataCollector.GetPointer());
  if ( dataCollectorFile == NULL )
  {
    LOG_ERROR("Failed to create the propertype of proper data collector type!");
    return 1;
  }

  dataCollector->ReadConfiguration( configRootElement );

  LOG_DEBUG( "Initializing data collector... " );
  dataCollector->Connect();
  dataCollector->Start();

  if (! dataCollector->GetConnected())
  {
    LOG_ERROR("Unable to start data collection!"); 
    return 1;
  }

  // Create the used objects
  TrackedFrame trackedFrame;

  PlusTransformName referenceToTrackerTransformName("Reference", "Tracker");
  PlusTransformName probeToTrackerTransformName("Probe", "Tracker");
  PlusTransformName stylusToTrackerTransformName("Stylus", "Tracker");

  vtkSmartPointer<vtkTransformRepository> transformRepository = vtkSmartPointer<vtkTransformRepository>::New();

  PlusStatus compareStatus=PLUS_SUCCESS;

  // Check some transforms to ensure that the correct data is returned by the data collector
  // THIS TEST ONLY WORKS WITH THIS SEQUENCE METAFILE: PlusLib\data\TestImages\fCal_Test_Calibration.mha

  // Frame 0001
  dataCollector->GetTrackedFrameByTime(218.8, &trackedFrame);
  transformRepository->SetTransforms(trackedFrame);
  
  if (CompareTransform(referenceToTrackerTransformName, transformRepository, -292.088, 60.4261, -1762.41)!=PLUS_SUCCESS)
  {
    LOG_ERROR("Test failed on frame 1");
    compareStatus=PLUS_FAIL;
  }
  if (CompareTransform(probeToTrackerTransformName, transformRepository, -275.514, 82.2319, -1701.99)!=PLUS_SUCCESS)
  {
    LOG_ERROR("Test failed on frame 1");
    compareStatus=PLUS_FAIL;
  }  

  vtkSmartPointer<vtkMatrix4x4> stylusToTrackerTransformMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
  bool valid=false;
  if ( transformRepository->GetTransform(stylusToTrackerTransformName, stylusToTrackerTransformMatrix, &valid) != PLUS_SUCCESS )
  {
    std::string transformNameStr;
    stylusToTrackerTransformName.GetTransformName(transformNameStr);
    LOG_ERROR("Test failed on frame 1: unable to get transform " << transformNameStr);
  }
  if ( valid )
  {
    std::string transformNameStr;
    stylusToTrackerTransformName.GetTransformName(transformNameStr);
    LOG_ERROR("Test failed on frame 1: Valid transform received, while non-valid transform was expected for " << transformNameStr);
  }

  // Frame 0013
  dataCollector->GetTrackedFrameByTime(222.3, &trackedFrame);
  transformRepository->SetTransforms(trackedFrame);

  if (CompareTransform(referenceToTrackerTransformName, transformRepository, -292.055, 60.647, -1762.63)!=PLUS_SUCCESS)
  {
    LOG_ERROR("Test failed on frame 13");
    compareStatus=PLUS_FAIL;
  }
  if (CompareTransform(probeToTrackerTransformName, transformRepository, -276.036, 82.8124, -1707.32)!=PLUS_SUCCESS)
  {
    LOG_ERROR("Test failed on frame 13");
    compareStatus=PLUS_FAIL;
  }
  valid=false;
  if ( transformRepository->GetTransform(stylusToTrackerTransformName, stylusToTrackerTransformMatrix, &valid) != PLUS_SUCCESS )
  {
    std::string transformNameStr;
    stylusToTrackerTransformName.GetTransformName(transformNameStr);
    LOG_ERROR("Test failed on frame 13: unable to get transform " << transformNameStr);
  }
  if ( valid )
  {
    std::string transformNameStr;
    stylusToTrackerTransformName.GetTransformName(transformNameStr);
    LOG_ERROR("Test failed on frame 13: Valid transform received, while non-valid transform was expected for " << transformNameStr);
  }

  dataCollector->Stop();
  dataCollector->Disconnect();

  return 0;
} 
