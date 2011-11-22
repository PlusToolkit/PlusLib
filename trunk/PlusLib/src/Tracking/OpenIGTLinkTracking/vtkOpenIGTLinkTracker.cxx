/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"

#include "vtkOpenIGTLinkTracker.h"

#include <sstream>

#include "vtkMatrix4x4.h"
#include "vtkObjectFactory.h"
#include "vtksys/SystemTools.hxx"
#include "vtkTransform.h"
#include "vtkXMLDataElement.h"

#include "PlusConfigure.h"
#include "vtkTracker.h"
#include "vtkTrackerTool.h"
#include "vtkTrackerBuffer.h"

vtkOpenIGTLinkTracker* vtkOpenIGTLinkTracker::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkOpenIGTLinkTracker");
  if( ret )
  {
    return ( vtkOpenIGTLinkTracker* )ret;
  }
  // If the factory was unable to create the object, then create it here.
  return new vtkOpenIGTLinkTracker;
}

vtkOpenIGTLinkTracker ::vtkOpenIGTLinkTracker()
{
  this->FrameNumber = 0;
  this->NumberOfSensors = 0; 
}

vtkOpenIGTLinkTracker::~vtkOpenIGTLinkTracker() 
{
  if ( this->Tracking )
  {
    this->StopTracking();
  }
}

void vtkOpenIGTLinkTracker::PrintSelf( ostream& os, vtkIndent indent )
{
  vtkTracker::PrintSelf( os, indent );
}


/**
 * Starts up a data receiver thread.
 * Sends the PlusServer a StartTracking command.
 * After that, PlusServer sends tracking data to this client as long as the connection is alive.
 */
PlusStatus vtkOpenIGTLinkTracker::Connect()
{
  LOG_TRACE( "vtkOpenIGTLinkTracker::Connect" ); 
  
  
  return PLUS_SUCCESS; 
}

PlusStatus vtkOpenIGTLinkTracker::Disconnect()
{
  LOG_TRACE( "vtkOpenIGTLinkTracker::Disconnect" ); 
  return this->StopTracking(); 
}

PlusStatus vtkOpenIGTLinkTracker::Probe()
{
  LOG_TRACE( "vtkOpenIGTLinkTracker::Probe" ); 

  return PLUS_SUCCESS; 
} 

/**
* @returns 1 on success, 0 on failure.
*/
PlusStatus vtkOpenIGTLinkTracker::InternalStartTracking()
{
  LOG_TRACE( "vtkOpenIGTLinkTracker::InternalStartTracking" ); 
  if ( this->Tracking )
  {
    return PLUS_SUCCESS;
  }

  if ( ! this->InitOpenIGTLinkTracker() )
  {
    LOG_ERROR( "Couldn't initialize vtkOpenIGTLinkTracker" );
    return PLUS_FAIL;
  } 
  
  return PLUS_SUCCESS;
}

PlusStatus vtkOpenIGTLinkTracker::InternalStopTracking()
{
  LOG_TRACE( "vtkOpenIGTLinkTracker::InternalStopTracking" ); 
  
  return PLUS_SUCCESS;
}

/**
* This function is called by the tracker thread.
*/
PlusStatus vtkOpenIGTLinkTracker::InternalUpdate()
{
  LOG_TRACE( "vtkOpenIGTLinkTracker::InternalUpdate" ); 

  if ( ! this->Tracking )
  {
    LOG_ERROR("called Update() when not tracking" );
    return PLUS_FAIL;
  }

  // TODO: Frame number is fake here!
  ++ this->FrameNumber;
  
  return PLUS_SUCCESS;
}

PlusStatus vtkOpenIGTLinkTracker::InitOpenIGTLinkTracker()
{
  LOG_TRACE( "vtkOpenIGTLinkTracker::InitOpenIGTLinkTracker" ); 
  return this->Connect(); 
}

PlusStatus vtkOpenIGTLinkTracker::ReadConfiguration( vtkXMLDataElement* config )
{
  // Read superclass configuration first
  Superclass::ReadConfiguration( config ); 

  LOG_TRACE( "vtkOpenIGTLinkTracker::ReadConfiguration" ); 
  if ( config == NULL ) 
  {
    LOG_ERROR("Unable to find OpenIGTLinkTracker XML data element");
    return PLUS_FAIL; 
  }

	vtkXMLDataElement* dataCollectionConfig = config->FindNestedElementWithName("USDataCollection");
	if (dataCollectionConfig == NULL)
  {
    LOG_ERROR("Cannot find USDataCollection element in XML tree!");
		return PLUS_FAIL;
	}

  vtkXMLDataElement* trackerConfig = dataCollectionConfig->FindNestedElementWithName("Tracker"); 
  if (trackerConfig == NULL) 
  {
    LOG_ERROR("Cannot find Tracker element in XML tree!");
		return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

PlusStatus vtkOpenIGTLinkTracker::WriteConfiguration( vtkXMLDataElement* config )
{
  LOG_TRACE( "vtkOpenIGTLinkTracker::WriteConfiguration" ); 
  if ( config == NULL )
  {
    LOG_ERROR("Invalid write configuration pointer");
    return PLUS_FAIL;
  }

  //config->SetName("Ascension3DGTracker");

  LOG_ERROR("Not implemented");
  return PLUS_FAIL;
}

