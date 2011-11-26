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
#include "vtkImageData.h"

//----------------------------------------------------------------------------

vtkCxxRevisionMacro(vtkDataCollectorFile, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkDataCollectorFile);

vtkCxxSetObjectMacro(vtkDataCollectorFile, TrackedFrameList, vtkTrackedFrameList);

//----------------------------------------------------------------------------
vtkDataCollectorFile::vtkDataCollectorFile()
  : vtkDataCollector()
{	
  this->TrackedFrameList = NULL;
  this->SequenceMetafileName = NULL;
  //this->OutputImageData = NULL;
  this->StartTime = 0.0;
  this->ReplayEnabled = false; 
  this->FirstTimestamp = 0.0;
  this->LastTimestamp = 0.0;
  this->LastAccessedFrameIndex = -1;

  //vtkSmartPointer<vtkImageData> outputImageData = vtkSmartPointer<vtkImageData>::New();
  //this->SetOutputImageData(outputImageData);
}

//----------------------------------------------------------------------------
vtkDataCollectorFile::~vtkDataCollectorFile()
{
  this->SetTrackedFrameList(NULL);
  //this->SetOutputImageData(NULL);
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

  vtkSmartPointer<vtkTrackedFrameList> trackedFrameList = vtkSmartPointer<vtkTrackedFrameList>::New(); 

  // Read metafile
  if ( trackedFrameList->ReadFromSequenceMetafile(this->SequenceMetafileName) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to read sequence metafile!"); 
    return PLUS_FAIL; 
  }

  if ( trackedFrameList->GetNumberOfTrackedFrames() < 1 ) 
  {
    LOG_ERROR("Failed to connect - there is no frame in the sequence metafile!"); 
    return PLUS_FAIL; 
  }

  // Set tracked frame list
  this->SetTrackedFrameList(trackedFrameList);

  // Set oldest and most recent timestamps
  this->FirstTimestamp = this->TrackedFrameList->GetTrackedFrame(0)->GetTimestamp();
  this->LastTimestamp = this->TrackedFrameList->GetTrackedFrame(this->TrackedFrameList->GetNumberOfTrackedFrames() - 1)->GetTimestamp();

  if (this->FirstTimestamp >= this->LastTimestamp)
  {
    LOG_ERROR("Invalid tracked frame list - timestamps must be ascending!");
    this->ConnectedOn();
    this->Disconnect();
    return PLUS_FAIL;
  }

  // Set output connection to output image data
  //this->SetOutput(this->OutputImageData);

  this->ConnectedOn();

  return PLUS_SUCCESS;
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

  this->SetTrackedFrameList(NULL); 
  this->FirstTimestamp = 0.0;
  this->LastTimestamp = 0.0;

  this->ConnectedOff(); 

  return PLUS_SUCCESS;
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

  this->StartTime = vtkAccurateTimer::GetSystemTime();

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkDataCollectorFile::Stop()
{
  LOG_TRACE("vtkDataCollectorFile::Stop"); 

  this->StartTime = 0.0;
  this->LastAccessedFrameIndex = -1;

  return PLUS_SUCCESS;
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

  LOG_ERROR("Not implemented yet!");
}

//----------------------------------------------------------------------------
PlusStatus vtkDataCollectorFile::GetOldestTimestamp(double &ts)
{
  LOG_TRACE("vtkDataCollectorFile::GetOldestTimestamp"); 

  if (! this->Connected)
  {
    LOG_ERROR("Unable to get oldest timestamp while disconnected!");
    return PLUS_FAIL;
  }

  ts = this->FirstTimestamp;

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkDataCollectorFile::GetMostRecentTimestamp(double &ts)
{
  LOG_TRACE("vtkDataCollectorFile::GetMostRecentTimestamp"); 

  if (! this->Connected)
  {
    LOG_ERROR("Unable to get most recent timestamp while disconnected!");
    return PLUS_FAIL;
  }

  ts = this->LastTimestamp;

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkDataCollectorFile::GetTransformWithTimestamp(vtkMatrix4x4* toolTransMatrix, double& transformTimestamp, TrackerStatus& status, const char* aToolName, bool calibratedTransform /*= false*/ )
{
  LOG_TRACE("vtkDataCollectorFile::GetTransformWithTimestamp"); 

  LOG_ERROR("Not implemented!");

  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkDataCollectorFile::GetTrackedFrameList(double& aTimestamp, vtkTrackedFrameList* aTrackedFrameList, int aMaxNumberOfFramesToAdd/*=-1*/)
{
  LOG_TRACE("vtkDataCollectorFile::GetTrackedFrameList(" << aTimestamp << ", " << aMaxNumberOfFramesToAdd << ")"); 

  if ( aTrackedFrameList == NULL )
  {
    LOG_ERROR("Unable to get tracked frame list - output tracked frmae list is NULL!"); 
    return PLUS_FAIL; 
  }

  // Get first and last frame index to add
  int indexOfFrameBeforeTheFirstToReturn = 0;
  if (GetTrackedFrameIndexForTimestamp(aTimestamp, indexOfFrameBeforeTheFirstToReturn) != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to get tracked frame index for timestamp " << aTimestamp);
    return PLUS_FAIL;
  }

  aTimestamp = GetCurrentFrameTimestamp();

  int indexOfLastFrameToReturn = 0;
  if (GetTrackedFrameIndexForTimestamp(aTimestamp, indexOfLastFrameToReturn) != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to get tracked frame index for timestamp " << aTimestamp);
    return PLUS_FAIL;
  }

  std::deque<int> frameIndicesToAdd;

  for (int i = indexOfLastFrameToReturn; i != indexOfFrameBeforeTheFirstToReturn; --i)
  {
    // Break if maximum number of frames added
    if (aMaxNumberOfFramesToAdd == 0)
    {
      break;
    }

    // Jump to last if begin reached
    if (i < 0)
    {
      i = this->TrackedFrameList->GetNumberOfTrackedFrames() - 1;
    }

    // Add index to list
    frameIndicesToAdd.push_front(i);

    aMaxNumberOfFramesToAdd--;
  }

  // Add tracked frames to the list 
  for (std::deque<int>::iterator it = frameIndicesToAdd.begin(); it != frameIndicesToAdd.end(); ++it)
  {
    if ( aTrackedFrameList->AddTrackedFrame(this->TrackedFrameList->GetTrackedFrame(*it), vtkTrackedFrameList::SKIP_INVALID_FRAME) != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to add tracked frame to the list!" ); 
      return PLUS_FAIL; 
    }
  }

  int numberOfFramesSinceTimestamp = (indexOfFrameBeforeTheFirstToReturn < indexOfLastFrameToReturn)
                                    ? (indexOfLastFrameToReturn - indexOfFrameBeforeTheFirstToReturn)
                                    : (this->TrackedFrameList->GetNumberOfTrackedFrames() - indexOfLastFrameToReturn + indexOfFrameBeforeTheFirstToReturn);
  LOG_DEBUG("Number of added frames: " << frameIndicesToAdd.size() << " out of " << numberOfFramesSinceTimestamp);

  this->LastAccessedFrameIndex = indexOfLastFrameToReturn;

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------

PlusStatus vtkDataCollectorFile::GetTrackedFrameListSampled(double& aTimestamp, vtkTrackedFrameList* aTrackedFrameList, double aSamplingRateSec)
{
  LOG_TRACE("vtkDataCollectorFile::GetTrackedFrameListSampled(" << aTimestamp << ", " << aSamplingRateSec << ")"); 

  if ( aTrackedFrameList == NULL )
  {
    LOG_ERROR("Unable to get tracked frame list - output tracked frmae list is NULL!"); 
    return PLUS_FAIL; 
  }

  double currentFrameTimestamp = GetCurrentFrameTimestamp();

  if (aTimestamp > currentFrameTimestamp)
  {
    double loopTime = this->LastTimestamp - this->FirstTimestamp;
    currentFrameTimestamp += loopTime;
  }

  while (aTimestamp + aSamplingRateSec <= currentFrameTimestamp)
  {
    // Get tracked frame from buffer
    TrackedFrame trackedFrame; 

    if ( this->GetTrackedFrameByTime(aTimestamp, &trackedFrame) != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to get tracked frame by time: " << std::fixed << aTimestamp ); 
      return PLUS_FAIL;
    }

    // Add tracked frame to the list 
    if ( aTrackedFrameList->AddTrackedFrame(&trackedFrame, vtkTrackedFrameList::SKIP_INVALID_FRAME) != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to add tracked frame to the list!" ); 
      return PLUS_FAIL; 
    }

    // Set timestamp to the next sampled one
    aTimestamp += aSamplingRateSec;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkDataCollectorFile::GetTrackedFrame(TrackedFrame* trackedFrame, bool calibratedTransform /*= false*/)
{
  //LOG_TRACE("vtkDataCollectorFile::GetTrackedFrame"); 

  // Get tracked frame by computed timestamp
  TrackedFrame outTrackedFrame;
  if (GetTrackedFrameByTime(GetCurrentFrameTimestamp(), &outTrackedFrame) != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to get tracked frame by timestamp: " << GetCurrentFrameTimestamp());
    return PLUS_FAIL;
  }

  trackedFrame = &outTrackedFrame;

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkDataCollectorFile::GetTrackedFrameIndexForTimestamp(double aTimestamp, int &aIndex)
{
  //LOG_TRACE("vtkDataCollectorFile::GetTrackedFrameIndexForTimestamp(" << aTimestamp << ")"); 

  if (! this->ReplayEnabled)
  {
    // If timestamp is smaller the timestamp of the first frame then return the first frame index
    if (aTimestamp <= this->FirstTimestamp)
    {
      aIndex = 0;
      return PLUS_SUCCESS;
    }

    // If timestamp is bigger than the timestamp of the last frame then return with the last frame index
    if (aTimestamp > this->LastTimestamp)
    {
      aIndex = this->TrackedFrameList->GetNumberOfTrackedFrames() - 1;
      return PLUS_SUCCESS;
    }
  }
  else // Compute the corresponding valid timestamp if replay enabled
  {
    double loopTime = this->LastTimestamp - this->FirstTimestamp;
    aTimestamp = this->FirstTimestamp + fmod(aTimestamp, loopTime); 
  }

  // Start searching from last accessed tracked frame
  aIndex = this->LastAccessedFrameIndex;

  // If requested timestamp is before the timestamp of the last accessed tracked frame then start from the beginning
  if (aTimestamp < this->TrackedFrameList->GetTrackedFrame(aIndex)->GetTimestamp())
  {
    aIndex = -1;
  }

  while (aTimestamp > this->TrackedFrameList->GetTrackedFrame(aIndex + 1)->GetTimestamp())
  {
    aIndex++;

    if (aIndex >= this->TrackedFrameList->GetNumberOfTrackedFrames())
    {
      LOG_ERROR("Error occurred when searching tracked frame index for timestamp!");
      return PLUS_FAIL;
    }
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkDataCollectorFile::GetTrackedFrameByTime(double aTimestamp, TrackedFrame* aTrackedFrame, bool calibratedTransform /*= false*/)
{
  //LOG_TRACE("vtkDataCollectorFile::GetTrackedFrameByTime");

  int index = -1;
  if (GetTrackedFrameIndexForTimestamp(aTimestamp, index) != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to get tracked frame index for timestamp " << aTimestamp);
    return PLUS_FAIL;
  }

  this->LastAccessedFrameIndex = index;

  aTrackedFrame = this->TrackedFrameList->GetTrackedFrame(this->LastAccessedFrameIndex);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
int vtkDataCollectorFile::RequestData( vtkInformation* vtkNotUsed( request ), vtkInformationVector**  inputVector, vtkInformationVector* outputVector )
{
  //LOG_TRACE("vtkDataCollectorFile::RequestData");

  vtkInformation *outInfo = outputVector->GetInformationObject(0);
  vtkImageData *outData = vtkImageData::SafeDownCast( outInfo->Get(vtkDataObject::DATA_OBJECT()) );

  if (this->TrackedFrameList == NULL || this->TrackedFrameList->GetNumberOfTrackedFrames() < 1)
  {
    int size[2] = {640, 480};
    outData->SetExtent( 0, size[0] -1, 0, size[1] - 1, 0, 0);
    outData->SetScalarTypeToUnsignedChar();
    outData->SetNumberOfScalarComponents(1); 
    outData->AllocateScalars(); 
    unsigned long memorysize = size[0]*size[1]*outData->GetScalarSize(); 
    memset(outData->GetScalarPointer(), 0, memorysize); 

    LOG_ERROR("Cannot request vide data connection was not estabilished (sequence metafile not loaded successfully)!"); 
    return 1;
  }

  // Get tracked frame by computed timestamp
  TrackedFrame outTrackedFrame;
  if (GetTrackedFrameByTime(GetCurrentFrameTimestamp(), &outTrackedFrame) != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to get tracked frame by timestamp: " << GetCurrentFrameTimestamp());
    return 1;
  }

  outData->DeepCopy(outTrackedFrame.GetImageData()->GetVtkImage());

  return 1;
}

//------------------------------------------------------------------------------
double vtkDataCollectorFile::GetCurrentFrameTimestamp()
{
  //LOG_TRACE("vtkDataCollectorFile::GetCurrentFrameTimestamp");

  double elapsedTime = vtkAccurateTimer::GetSystemTime() - this->StartTime;

  double nextFrameTimestamp = this->FirstTimestamp + elapsedTime; 
  if ( nextFrameTimestamp > this->LastTimestamp )
  {
    if ( this->ReplayEnabled )
    {
      double loopTime = this->LastTimestamp - this->FirstTimestamp;
      nextFrameTimestamp = this->FirstTimestamp + fmod(elapsedTime, loopTime); 
    }
    else
    {
      nextFrameTimestamp = this->LastTimestamp; 
    }
  }

  return nextFrameTimestamp;
}

//------------------------------------------------------------------------------
PlusStatus vtkDataCollectorFile::ReadConfiguration(vtkXMLDataElement* aConfigurationData)
{
  LOG_TRACE("vtkDataCollectorFile::ReadConfiguration");

  if ( aConfigurationData == NULL )
  {
    LOG_ERROR("Unable to configure data collector! (XML data element is NULL)"); 
    return PLUS_FAIL; 
  }

	vtkXMLDataElement* dataCollectionConfig = aConfigurationData->FindNestedElementWithName("DataCollection");
	if (dataCollectionConfig == NULL)
  {
    LOG_ERROR("Cannot find DataCollection element in XML tree!");
		return PLUS_FAIL;
	}

  vtkXMLDataElement* fileConfig = dataCollectionConfig->FindNestedElementWithName("File");
  if (fileConfig == NULL)
  {
    LOG_ERROR("Cannot find File element in XML tree!");
		return PLUS_FAIL;
  }

  // Read sequence metafile name
  const char* sequenceMetafileName = fileConfig->GetAttribute("SequenceMetafile"); 
  if ( sequenceMetafileName != NULL ) 
  {
    this->SetSequenceMetafileName(vtkPlusConfig::GetAbsoluteImagePath(sequenceMetafileName).c_str());
  }

  // Read replay enabled flag
  const char* replayEnabled = fileConfig->GetAttribute("ReplayEnabled"); 
  if ( replayEnabled != NULL ) 
  {
    if ( STRCASECMP("TRUE", replayEnabled ) == 0 )
    {
      this->ReplayEnabled = true; 
    }
    else if ( STRCASECMP("FALSE", replayEnabled ) == 0 )
    {
      this->ReplayEnabled = false; 
    }
    else
    {
      LOG_WARNING("Unable to recognize ReplayEnabled attribute: " << replayEnabled << " - changed to false by default!"); 
      this->ReplayEnabled = false; 
    }
  }

  return PLUS_SUCCESS;
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

  LOG_ERROR("No video source object in simulation mode!");

  return NULL;
}

//------------------------------------------------------------------------------
vtkTracker* vtkDataCollectorFile::GetTracker()
{
  LOG_TRACE("vtkDataCollectorFile::GetTracker");

  LOG_ERROR("No tracker object in simulation mode!");

  return NULL;
}

//------------------------------------------------------------------------------
void vtkDataCollectorFile::SetProgressBarUpdateCallbackFunction(ProgressBarUpdatePtr cb)
{
  LOG_TRACE("vtkDataCollectorFile::SetProgressBarUpdateCallbackFunction");

  LOG_ERROR("There is no update callback function in simulation mode!");
}

//------------------------------------------------------------------------------
void vtkDataCollectorFile::GetFrameSize(int aDim[2])
{
  LOG_TRACE("vtkDataCollectorFile::GetFrameSize");

  if (this->Connected == false)
  {
    LOG_ERROR("Data collector is not connected!");
    return;
  }

  this->TrackedFrameList->GetTrackedFrame(0)->GetFrameSize(aDim);
}

//------------------------------------------------------------------------------
PlusStatus vtkDataCollectorFile::GetFrameRate(double &aFrameRate)
{
  LOG_TRACE("vtkDataCollectorFile::GetFrameRate");

  if (this->Connected == false)
  {
    LOG_ERROR("Data collector is not connected!");
    return PLUS_FAIL;
  }

  double loopTime = this->LastTimestamp - this->FirstTimestamp;

  aFrameRate = (double)this->TrackedFrameList->GetNumberOfTrackedFrames() / loopTime;

  return PLUS_SUCCESS;
}
