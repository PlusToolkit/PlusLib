/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "PlusRevision.h"
#include "vtkTrackerFactory.h"
#include "vtkPlusVideoSourceFactory.h"

int main(int argc, char **argv)
{

  LOG_INFO("Plus SVN revision: " << PLUSLIB_REVISION); 

  vtkSmartPointer<vtkTrackerFactory> trackerFactory = vtkSmartPointer<vtkTrackerFactory>::New(); 
  std::ostringstream supportedTrackers; 
  trackerFactory->PrintAvailableTrackers(supportedTrackers, vtkIndent()); 
  LOG_INFO(supportedTrackers.str()); 

  vtkSmartPointer<vtkPlusVideoSourceFactory> videoSourceFactory = vtkSmartPointer<vtkPlusVideoSourceFactory>::New(); 
  std::ostringstream supportedVideoSources; 
  videoSourceFactory->PrintAvailableVideoSources(supportedVideoSources, vtkIndent()); 
  LOG_INFO(supportedVideoSources.str());
  
  return EXIT_SUCCESS; 
}