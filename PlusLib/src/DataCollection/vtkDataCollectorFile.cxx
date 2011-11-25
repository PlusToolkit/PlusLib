/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#include "vtkDataCollectorFile.h"

#include "vtkXMLUtilities.h"
#include "vtkTrackedFrameList.h"
#include "vtkObjectFactory.h"
#include "vtkInformationVector.h"
#include "vtkInformation.h"

//----------------------------------------------------------------------------

vtkCxxRevisionMacro(vtkDataCollectorFile, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkDataCollectorFile);

vtkCxxSetObjectMacro(vtkDataCollectorFile, TrackedFrameList, vtkTrackedFrameList);

//----------------------------------------------------------------------------
vtkDataCollectorFile::vtkDataCollectorFile()
  : vtkDataCollector()
{	
  this->TrackedFrameList = NULL;
}

//----------------------------------------------------------------------------
vtkDataCollectorFile::~vtkDataCollectorFile()
{
  this->SetTrackedFrameList(NULL); 
}

//----------------------------------------------------------------------------
void vtkDataCollectorFile::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  if ( this->GetTrackedFrameList() != NULL )
  {
    os << indent << "TrackedFrameList: " << std::endl; 
    this->TrackedFrameList->PrintSelf(os, indent); 
  }
}

//----------------------------------------------------------------------------
PlusStatus vtkDataCollectorFile::Connect()
{
  LOG_TRACE("vtkDataCollectorFile::Connect"); 

  LOG_ERROR("Not implemented!");

  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkDataCollectorFile::Disconnect()
{
  LOG_TRACE("vtkDataCollectorFile::Disconnect"); 

  if ( !this->GetConnected() )
  {
    // Devices already disconnected 
    return PLUS_SUCCESS; 
  }

  //TODO
  LOG_ERROR("Not implemented!");

  this->ConnectedOff(); 

  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkDataCollectorFile::Start()
{
  LOG_TRACE("vtkDataCollectorFile::Start"); 

  if (this->TrackedFrameList == NULL)
  {
    LOG_ERROR("Data collection cannot be started because tracked frame list have not been initialized properly!");
    return PLUS_FAIL;
  }

  LOG_ERROR("Not implemented!");

  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkDataCollectorFile::Stop()
{
  LOG_TRACE("vtkDataCollectorFile::Stop"); 

  LOG_ERROR("Not implemented!");

  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkDataCollectorFile::Synchronize( const char* bufferOutputFolder /*= NULL*/ , bool acquireDataOnly /*= false*/ )
{
  LOG_TRACE("vtkDataCollectorFile::Synchronize"); 

  LOG_ERROR("Synchronization of simulated data collection is not possible!");

  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
void vtkDataCollectorFile::SetLocalTimeOffset(double videoOffset, double trackerOffset)
{
  LOG_TRACE("vtkDataCollectorFile::SetLocalTimeOffset");

  LOG_ERROR("Not implemented!");
}

//----------------------------------------------------------------------------
PlusStatus vtkDataCollectorFile::GetOldestTimestamp(double &ts)
{
  LOG_TRACE("vtkDataCollectorFile::GetOldestTimestamp"); 

  LOG_ERROR("Not implemented!");

  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkDataCollectorFile::GetMostRecentTimestamp(double &ts)
{
  LOG_TRACE("vtkDataCollectorFile::GetMostRecentTimestamp"); 

  LOG_ERROR("Not implemented!");

  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkDataCollectorFile::GetTransformWithTimestamp(vtkMatrix4x4* toolTransMatrix, double& transformTimestamp, TrackerStatus& status, const char* aToolName, bool calibratedTransform /*= false*/ )
{
  LOG_TRACE("vtkDataCollectorFile::GetTransformWithTimestamp"); 

  LOG_ERROR("Not implemented!");

  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkDataCollectorFile::GetTrackedFrameList(double& frameTimestamp, vtkTrackedFrameList* trackedFrameList, int maxNumberOfFramesToAdd/*=-1*/)
{
  LOG_TRACE("vtkDataCollectorFile::GetTrackedFrameList(" << frameTimestamp << ", " << maxNumberOfFramesToAdd << ")"); 

  LOG_ERROR("Not implemented!");

  return PLUS_FAIL;
}

//----------------------------------------------------------------------------

PlusStatus vtkDataCollectorFile::GetTrackedFrameListSampled(double& frameTimestamp, vtkTrackedFrameList* trackedFrameList, double samplingRateSec)
{
  LOG_TRACE("vtkDataCollectorFile::GetTrackedFrameListSampled(" << frameTimestamp << ", " << samplingRateSec << ")"); 

  LOG_ERROR("Not implemented!");

  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkDataCollectorFile::GetTrackedFrame(TrackedFrame* trackedFrame, bool calibratedTransform /*= false*/)
{
  LOG_TRACE("vtkDataCollectorFile::GetTrackedFrame"); 

  LOG_ERROR("Not implemented!");

  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
int vtkDataCollectorFile::RequestData( vtkInformation* vtkNotUsed( request ), vtkInformationVector**  inputVector, vtkInformationVector* outputVector )
{
  LOG_TRACE("vtkDataCollectorFile::RequestData");

  LOG_ERROR("Not implemented!");

  return 1;
}

//------------------------------------------------------------------------------
PlusStatus vtkDataCollectorFile::ReadConfiguration(vtkXMLDataElement* aConfigurationData)
{
  LOG_TRACE("vtkDataCollectorFile::ReadConfiguration");

  LOG_ERROR("Not implemented!");

  return PLUS_FAIL;
}

//------------------------------------------------------------------------------
void vtkDataCollectorFile::SetTrackingOnly(bool trackingOnly)
{
  LOG_TRACE("vtkDataCollectorFile::SetTrackingOnly");

  LOG_ERROR("Tracking cannot be explicitly turned on or off in case of simulation mode!");
}

//------------------------------------------------------------------------------
void vtkDataCollectorFile::SetVideoOnly(bool videoOnly)
{
  LOG_TRACE("vtkDataCollectorFile::SetVideoOnly");

  LOG_ERROR("Video cannot be explicitly turned on or off in case of simulation mode!");
}

//------------------------------------------------------------------------------
vtkPlusVideoSource* vtkDataCollectorFile::GetVideoSource()
{
  LOG_TRACE("vtkDataCollectorFile::GetVideoSource");

  LOG_ERROR("There is no separate video source in simulation mode!");

  return NULL;
}

//------------------------------------------------------------------------------
vtkTracker* vtkDataCollectorFile::GetTracker()
{
  LOG_TRACE("vtkDataCollectorFile::GetTracker");

  LOG_ERROR("There is no separate tracker in simulation mode!");

  return NULL;
}

//------------------------------------------------------------------------------
void vtkDataCollectorFile::SetProgressBarUpdateCallbackFunction(ProgressBarUpdatePtr cb)
{
  LOG_TRACE("vtkDataCollectorFile::SetProgressBarUpdateCallbackFunction");

  LOG_ERROR("There is no update callback function in simulation mode!");
}
