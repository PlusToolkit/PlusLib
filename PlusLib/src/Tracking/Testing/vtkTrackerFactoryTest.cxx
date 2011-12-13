/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtksys/CommandLineArguments.hxx"
#include "vtkTrackerFactory.h"
#include "vtkTracker.h"
#include "vtkSavedDataTracker.h"
#include "vtkFakeTracker.h"


int main (int argc, char* argv[])
{
  bool printHelp(false); 
  int verboseLevel = vtkPlusLogger::LOG_LEVEL_DEFAULT;

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

  if ( printHelp ) 
  {
    std::cout << "\nHelp:" << args.GetHelp() << std::endl;
    exit(EXIT_SUCCESS); 

  }

  vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);

  vtkTracker* tracker = NULL; 
  vtkSmartPointer<vtkTrackerFactory> trackerFactory = vtkSmartPointer<vtkTrackerFactory>::New(); 

  std::ostringstream printself; 
  trackerFactory->PrintSelf(printself, vtkIndent()); 
  LOG_INFO("Tracker factory printself: " << printself.str()); 

  ////////////////////////////////////////////////////////
  // Tracker type: NULL 

  LOG_INFO("Testing tracker factory create instance function with NULL tracker type..."); 
  if ( trackerFactory->CreateInstance(NULL, tracker) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to create None tracker instance!"); 
    return EXIT_FAILURE; 
  }

  if ( tracker != NULL )
  {
    LOG_ERROR("NULL tracker expected!"); 
    return EXIT_FAILURE; 
  }

  ////////////////////////////////////////////////////////
  // Tracker type: None

  LOG_INFO("Testing tracker factory create instance function with None tracker type..."); 
  if ( trackerFactory->CreateInstance("None", tracker) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to create None tracker instance!"); 
    return EXIT_FAILURE; 
  }

  if ( tracker != NULL )
  {
    LOG_ERROR("NULL tracker expected!"); 
    return EXIT_FAILURE; 
  }
  
  ////////////////////////////////////////////////////////
  // Tracker type: SavedDataset

  LOG_INFO("Testing tracker factory create instance function with SavedDataset tracker type..."); 
  if ( trackerFactory->CreateInstance("SavedDataset", tracker) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to create SavedDataset tracker instance!"); 
    return EXIT_FAILURE; 
  }

  if ( dynamic_cast<vtkSavedDataTracker*>(tracker) == NULL )
  {
    LOG_ERROR("Failed to create SavedDataset tracker instance!"); 
    return EXIT_FAILURE; 
  }

   ////////////////////////////////////////////////////////
  // Tracker type: FakeTracker

  LOG_INFO("Testing tracker factory create instance function with FakeTracker tracker type..."); 
  if ( trackerFactory->CreateInstance("FakeTracker", tracker) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to create FakeTracker tracker instance!"); 
    return EXIT_FAILURE; 
  }

  if ( dynamic_cast<vtkFakeTracker*>(tracker) == NULL )
  {
    LOG_ERROR("Failed to create FakeTracker tracker instance!"); 
    return EXIT_FAILURE; 
  }
  
  LOG_INFO("Test finished successfully!"); 
  return EXIT_SUCCESS;
} 