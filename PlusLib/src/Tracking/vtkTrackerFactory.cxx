/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#include "vtkTrackerFactory.h"


//----------------------------------------------------------------------------
// Tracker devices
#include "vtkTracker.h"
#include "vtkTrackerTool.h"
#include "vtkTrackerBuffer.h"
#ifdef PLUS_USE_OpenIGTLink
#include "vtkOpenIGTLinkTracker.h" 
#endif
#ifdef PLUS_USE_POLARIS
#include "vtkNDITracker.h"
#endif
#ifdef PLUS_USE_CERTUS
#include "vtkNDICertusTracker.h"
#endif
#ifdef PLUS_USE_MICRONTRACKER
#include "vtkMicronTracker.h"
#endif
#ifdef PLUS_USE_BRACHY_TRACKER
#include "vtkBrachyTracker.h"
#endif
#ifdef PLUS_USE_Ascension3DG
#include "vtkAscension3DGTracker.h"
#endif
#ifdef PLUS_USE_PHIDGET_SPATIAL_TRACKER
#include "vtkPhidgetSpatialTracker.h"
#endif
#include "vtkFakeTracker.h"
#include "vtkSavedDataTracker.h"


//----------------------------------------------------------------------------

vtkCxxRevisionMacro(vtkTrackerFactory, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkTrackerFactory);

//----------------------------------------------------------------------------
vtkTrackerFactory::vtkTrackerFactory()
{	
  TrackerTypes["None"]=NULL; 
  TrackerTypes["SavedDataset"]=(PointerToTracker)&vtkSavedDataTracker::New; 
  TrackerTypes["FakeTracker"]=(PointerToTracker)&vtkFakeTracker::New; 
#ifdef PLUS_USE_OpenIGTLink
  TrackerTypes["OpenIGTLinkTracker"]=(PointerToTracker)&vtkOpenIGTLinkTracker::New; 
#endif
#ifdef PLUS_USE_BRACHY_TRACKER
  TrackerTypes["BrachyTracker"]=(PointerToTracker)&vtkBrachyTracker::New; 
#endif 
#ifdef PLUS_USE_CERTUS
  TrackerTypes["CertusTracker"]=(PointerToTracker)&vtkNDICertusTracker::New; 
#endif
#ifdef PLUS_USE_POLARIS
  TrackerTypes["PolarisTracker"]=(PointerToTracker)&vtkNDITracker::New; 
#endif
#ifdef PLUS_USE_POLARIS
  TrackerTypes["AuroraTracker"]=(PointerToTracker)&vtkNDITracker::New; 
#endif
#ifdef PLUS_USE_MICRONTRACKER  
  TrackerTypes["MicronTracker"]=(PointerToTracker)&vtkMicronTracker::New; 
#endif
#ifdef PLUS_USE_Ascension3DG  
  TrackerTypes["Ascension3DG"]=(PointerToTracker)&vtkAscension3DGTracker::New; 
#endif
#ifdef PLUS_USE_PHIDGET_SPATIAL_TRACKER  
  TrackerTypes["PhidgetSpatial"]=(PointerToTracker)&vtkPhidgetSpatialTracker::New; 
#endif

}

//----------------------------------------------------------------------------
vtkTrackerFactory::~vtkTrackerFactory()
{
}

//----------------------------------------------------------------------------
void vtkTrackerFactory::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  this->PrintAvailableTrackers(os, indent); 

}

//----------------------------------------------------------------------------
void vtkTrackerFactory::PrintAvailableTrackers(ostream& os, vtkIndent indent)
{
  os << indent << "Supported tracker devices: " << std::endl; 
  std::map<std::string,PointerToTracker>::iterator it; 
  for ( it = TrackerTypes.begin(); it != TrackerTypes.end(); ++it)
  {
    if ( it->second != NULL )
    {
      vtkTracker* tracker = (*it->second)(); 
      os << indent.GetNextIndent() << "- " << it->first << " (ver: " << tracker->GetSdkVersion() << ")" << std::endl; 
      tracker->Delete();
      tracker=NULL;
    }
  }
}

//----------------------------------------------------------------------------
PlusStatus vtkTrackerFactory::CreateInstance(const char* aTrackerType, vtkTracker* &aTracker)
{
  if ( aTracker != NULL )
  {
    aTracker->Delete(); 
    aTracker = NULL; 
  }
  
  std::string trackerType; 
  if ( aTrackerType == NULL ) 
  {
    LOG_WARNING("Tracker type is NULL, set to default: None"); 
    trackerType = "None"; 
  }
  else
  {
    trackerType = aTrackerType; 
  }

  if ( TrackerTypes.find(trackerType) != TrackerTypes.end() )
  {
    if ( TrackerTypes[trackerType] != NULL )
    { // Call tracker New() function if tracker not NULL
      aTracker = (*TrackerTypes[trackerType])(); 
    }
  }
  else
  {
    LOG_ERROR("Unknown tracker type: " << trackerType);
    return PLUS_FAIL; 
  }

  return PLUS_SUCCESS; 
}
