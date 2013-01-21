/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkFakeTracker.h"
#include "vtkPlusDevice.h"
#include "vtkPlusDeviceFactory.h"
#include "vtkSavedDataVideoSource.h"
#include "vtksys/CommandLineArguments.hxx"

int main (int argc, char* argv[])
{
  bool printHelp(false); 
  int verboseLevel = vtkPlusLogger::LOG_LEVEL_UNDEFINED;

  vtksys::CommandLineArguments args;
  args.Initialize(argc, argv);

  args.AddArgument("--help", vtksys::CommandLineArguments::NO_ARGUMENT, &printHelp, "Print this help.");	
  args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug, 5=trace)");	

  if ( !args.Parse() )
  {
    std::cerr << "Problem parsing arguments" << std::endl;
    std::cout << "\nHelp:" << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }

  vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);

  if ( printHelp ) 
  {
    std::cout << "\nHelp:" << args.GetHelp() << std::endl;
    exit(EXIT_SUCCESS); 

  }

  vtkPlusDevice* device = NULL; 
  vtkSmartPointer<vtkPlusDeviceFactory> deviceFactory = vtkSmartPointer<vtkPlusDeviceFactory>::New(); 

  std::ostringstream printself; 
  deviceFactory->PrintSelf(printself, vtkIndent()); 
  LOG_INFO("device factory printself: " << printself.str()); 

  ////////////////////////////////////////////////////////
  // device type: NULL 

  LOG_INFO("Testing device factory create instance function with NULL device type..."); 
  if ( deviceFactory->CreateInstance(NULL, device, "aDevice") == PLUS_SUCCESS )
  {
    LOG_ERROR("Expected failure in creating device instance, but method call returned with success"); 
    return EXIT_FAILURE; 
  }
  if ( device != NULL )
  {
    LOG_ERROR("NULL device expected!"); 
    return EXIT_FAILURE; 
  }

  ////////////////////////////////////////////////////////
  // device type: not existing device type

  LOG_INFO("Testing device factory create instance function with a not existing device type..."); 
  if ( deviceFactory->CreateInstance("NotExistingAbc", device, "aDevice") == PLUS_SUCCESS )
  {
    LOG_ERROR("Expected failure in creating not existing device instance, but the operation was successful"); 
    return EXIT_FAILURE; 
  }
  if ( device != NULL )
  {
    LOG_ERROR("NULL device expected!"); 
    return EXIT_FAILURE; 
  }

  ////////////////////////////////////////////////////////
  // device type: Fakedevice

  LOG_INFO("Testing device factory create instance function with FakeTracker device type..."); 
  if ( deviceFactory->CreateInstance("FakeTracker", device, "FakeTracker") != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to create Fakedevice device instance!"); 
    return EXIT_FAILURE; 
  }
  if ( dynamic_cast<vtkFakeTracker*>(device) == NULL )
  {
    LOG_ERROR("Failed to create FakeTracker device instance!"); 
    return EXIT_FAILURE; 
  }

  ////////////////////////////////////////////////////////
  // Video source type: NULL 

  LOG_INFO("Testing video source factory create instance function with NULL video source type..."); 
  if ( deviceFactory->CreateInstance(NULL, device, "someDevice") == PLUS_SUCCESS )
  {
    LOG_ERROR("Expected failure in creating video source instance, but method call returned with success"); 
    return EXIT_FAILURE; 
  }
  if ( device != NULL )
  {
    LOG_ERROR("NULL video source expected!"); 
    return EXIT_FAILURE; 
  }

  ////////////////////////////////////////////////////////
  // Video source type: not existing device type

  LOG_INFO("Testing video source factory create instance function with not existing video source type..."); 
  if ( deviceFactory->CreateInstance("NotExistingAbc", device, "anotherDevice") == PLUS_SUCCESS )
  {
    LOG_ERROR("Expected failure in creating not existing video source instance, but the operation was successful"); 
    return EXIT_FAILURE; 
  }
  if ( device != NULL )
  {
    LOG_ERROR("NULL video source expected!"); 
    return EXIT_FAILURE; 
  }

  ////////////////////////////////////////////////////////
  // Video source type: SavedDataset

  LOG_INFO("Testing video source factory create instance function with SavedDataset video source type..."); 
  if ( deviceFactory->CreateInstance("SavedVideoDataset", device, "vidDevice") != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to create SavedDataset video source instance!"); 
    return EXIT_FAILURE; 
  }
  if ( dynamic_cast<vtkSavedDataVideoSource*>(device) == NULL )
  {
    LOG_ERROR("Failed to create SavedDataset video source instance!"); 
    return EXIT_FAILURE; 
  }
  device->Delete();
  device=NULL;

  LOG_INFO("Test finished successfully!"); 
  return EXIT_SUCCESS;
} 