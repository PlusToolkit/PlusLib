/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtksys/CommandLineArguments.hxx"
#include "vtkPlusDeviceFactory.h"
#include "vtkPlusDevice.h"
#include "vtkSavedDataVideoSource.h"

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

  vtkPlusDevice* videoSource = NULL; 
  vtkSmartPointer<vtkPlusDeviceFactory> videoSourceFactory = vtkSmartPointer<vtkPlusDeviceFactory>::New(); 

  std::ostringstream printself; 
  videoSourceFactory->PrintSelf(printself, vtkIndent()); 
  LOG_INFO("Video source factory printself: " << printself.str()); 

  ////////////////////////////////////////////////////////
  // Video source type: NULL 

  LOG_INFO("Testing video source factory create instance function with NULL video source type..."); 
  if ( videoSourceFactory->CreateInstance(NULL, videoSource, "someDevice") == PLUS_SUCCESS )
  {
    LOG_ERROR("Expected failure in creating video source instance, but method call returned with success"); 
    return EXIT_FAILURE; 
  }
  if ( videoSource != NULL )
  {
    LOG_ERROR("NULL video source expected!"); 
    return EXIT_FAILURE; 
  }

  ////////////////////////////////////////////////////////
  // Video source type: not existing device type

  LOG_INFO("Testing video source factory create instance function with not existing video source type..."); 
  if ( videoSourceFactory->CreateInstance("NotExistingAbc", videoSource, "anotherDevice") == PLUS_SUCCESS )
  {
    LOG_ERROR("Expected failure in creating not existing video source instance, but the operation was successful"); 
    return EXIT_FAILURE; 
  }
  if ( videoSource != NULL )
  {
    LOG_ERROR("NULL video source expected!"); 
    return EXIT_FAILURE; 
  }
  
  ////////////////////////////////////////////////////////
  // Video source type: SavedDataset

  LOG_INFO("Testing video source factory create instance function with SavedDataset video source type..."); 
  if ( videoSourceFactory->CreateInstance("SavedDataset", videoSource, "vidDevice") != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to create SavedDataset video source instance!"); 
    return EXIT_FAILURE; 
  }
  if ( dynamic_cast<vtkSavedDataVideoSource*>(videoSource) == NULL )
  {
    LOG_ERROR("Failed to create SavedDataset video source instance!"); 
    return EXIT_FAILURE; 
  }
  videoSource->Delete();
  videoSource=NULL;

  LOG_INFO("Test finished successfully!"); 
  return EXIT_SUCCESS;
} 