/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

/*=========================================================================
The following copyright notice is applicable to parts of this file:

Copyright (c) 2000-2005 Atamai, Inc.

Use, modification and redistribution of the software, in source or
binary forms, are permitted provided that the following terms and
conditions are met:

1) Redistribution of the source code, in verbatim or modified
form, must retain the above copyright notice, this license,
the following disclaimer, and any notices that refer to this
license and/or the following disclaimer.  

2) Redistribution in binary form must include the above copyright
notice, a copy of this license and the following disclaimer
in the documentation or with other materials provided with the
distribution.

3) Modified copies of the source code must be clearly marked as such,
and must not be misrepresented as verbatim copies of the source code.

THE COPYRIGHT HOLDERS AND/OR OTHER PARTIES PROVIDE THE SOFTWARE "AS IS"
WITHOUT EXPRESSED OR IMPLIED WARRANTY INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE.  IN NO EVENT SHALL ANY COPYRIGHT HOLDER OR OTHER PARTY WHO MAY
MODIFY AND/OR REDISTRIBUTE THE SOFTWARE UNDER THE TERMS OF THIS LICENSE
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, LOSS OF DATA OR DATA BECOMING INACCURATE
OR LOSS OF PROFIT OR BUSINESS INTERRUPTION) ARISING IN ANY WAY OUT OF
THE USE OR INABILITY TO USE THE SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGES.

=========================================================================*/

#include "PlusConfigure.h"

#include "vtkMicronTracker.h"
#include <fstream>
#include <iostream>
#include <set>

#include "vtkImageData.h"
#include "PlusVideoFrame.h"

#include "MicronTrackerInterface.h"

/****************************************************************************/


//----------------------------------------------------------------------------
vtkMicronTracker* vtkMicronTracker::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMicronTracker");
  if(ret)
  {
    return (vtkMicronTracker*)ret;
  }
  // If the factory was unable to create the object, then create it here.
  return new vtkMicronTracker;
}

//----------------------------------------------------------------------------
vtkMicronTracker::vtkMicronTracker()
{
  this->IsMicronTrackingInitialized = 0;
  this->MT = new MicronTrackerInterface();

  // for accurate timing
  this->LastFrameNumber=0;

  this->IsAdditionalFacetAdding = 0;
  this->IsCollectingNewSamples = 0;
  this->NewSampleFramesCollected = 0;
  this->FrameNumber = 0;
  this->LeftImage = NULL;
  this->RightImage = NULL;
  this->Xpoints = NULL;
  this->VectorEnds = NULL;
  //  this->LeftImageArray = new unsigned char();
  //  this->RightImageArray = new unsigned char();

}

//----------------------------------------------------------------------------
vtkMicronTracker::~vtkMicronTracker() 
{
  if ( this->MT != NULL )
  {
    this->MT->mtEnd();
    delete this->MT;
    this->MT = NULL;
  }
}

//----------------------------------------------------------------------------
void vtkMicronTracker::UpdateINI()
{
  this->MT->mtUpdateINI();
}

//----------------------------------------------------------------------------
PlusStatus vtkMicronTracker::Probe()
{  
  if (this->IsMicronTrackingInitialized)
  {
    LOG_ERROR("vtkMicronTracker::Probe should not be called while the device is already initialized");
    return PLUS_FAIL;
  }

  if (this->MT->mtInit()!=1)
  {
    LOG_ERROR("Error in initializing Micron Tracker");
    return PLUS_FAIL;
  }

  // Try to attach the cameras till find the cameras
  if (this->MT->mtSetupCameras()!=1)
  {
    LOG_ERROR("Error in initializing Micron Tracker: setup cameras failed. Check the camera connections.");
    return PLUS_FAIL;
  }

  int numOfCameras = this->MT->mtGetNumOfCameras();
  if (numOfCameras==0)
  {
    LOG_ERROR("Error in initializing Micron Tracker: no cameras attached. Check the camera connections.");
    return PLUS_FAIL;
  }

  LOG_DEBUG("Number of attached cameras: " << numOfCameras );

  this->MT->mtEnd();

  return PLUS_SUCCESS;
} 

//----------------------------------------------------------------------------
PlusStatus vtkMicronTracker::InternalStartTracking()
{
  if (!this->IsMicronTrackingInitialized)
  {
    LOG_ERROR("InternalStartTracking failed: MicronTracker has not been initialized");
    return PLUS_FAIL;
  }

  this->RefreshMarkerTemplates();

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkMicronTracker::InternalStopTracking()
{
  if (!this->IsMicronTrackingInitialized)
  {    
    return PLUS_SUCCESS;
  }

  // :TODO: we could stop the MicronTracker

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkMicronTracker::InternalUpdate()
{
  int callResult = 0;

  if (!this->IsMicronTrackingInitialized)
  {
    LOG_ERROR("InternalUpdate failed: MicronTracker has not been initialized");
    return PLUS_FAIL;
  }
  // TODO: Frame number is fake here!
  // FrameNumber will be used in ToolTimeStampedUpdate
  ++ this->FrameNumber;
  // If grabing a frame was not successful prevent it from calling the mtGrabFrame
  // method of this->MT, until the problem is solved.
  if (this->MT->mtGrabFrame() == -1)
  {
    LOG_ERROR("Error in grabing a frame! (" << this->MT->mtGetErrorString() <<")");
    return PLUS_FAIL;
  }
  if (this->MT->mtProcessFrame() == -1)
  {
    LOG_ERROR("Error in processing a frame! (" << this->MT->mtGetErrorString() <<")");
    return PLUS_FAIL;
  }

  this->MT->mtFindIdentifiedMarkers();
  // this->MT->mtFindUnidentifiedMarkers();

  // Collecting new samples if creating a new template by the user
  if (this->IsCollectingNewSamples == 1)
  {

    int collectNewSamplesResult=this->MT->mtCollectNewSamples(this->IsAdditionalFacetAdding);
    if ( collectNewSamplesResult == -1)
    {
      LOG_ERROR("Less than two vectors are detected.");
      return PLUS_FAIL;
    }
    else if (collectNewSamplesResult == 1)
    {
      LOG_ERROR("More than two vectors are detected.");
      return PLUS_FAIL;
    }
    else if (collectNewSamplesResult == 99)
    {
      LOG_ERROR("No known facet detected.");
      return PLUS_FAIL;
    }
    this->NewSampleFramesCollected++;
    LOG_TRACE("Samples collected so far: " << this->NewSampleFramesCollected);

  }

  // Setting the timestamp
  this->LastFrameNumber++;
  const double unfilteredTimestamp = vtkAccurateTimer::GetSystemTime();

  int numOfIdentifiedMarkers = this->MT->mtGetIdentifiedMarkersCount();
  LOG_DEBUG("Number of identified markers: " << numOfIdentifiedMarkers);

  // Set status and transform for tools with detected markers
  vtkSmartPointer<vtkMatrix4x4> transformMatrix=vtkSmartPointer<vtkMatrix4x4>::New();
  std::set<std::string> identifiedToolNames;
  vtkSmartPointer< vtkMatrix4x4 > mTrackerToReference = vtkSmartPointer< vtkMatrix4x4 >::New();
  mTrackerToReference->Identity();
  vtkSmartPointer< vtkMatrix4x4 > mToolToReference = vtkSmartPointer< vtkMatrix4x4 >::New();
  mToolToReference->Identity();
  vtkSmartPointer< vtkMatrix4x4 > mToolToTracker = vtkSmartPointer< vtkMatrix4x4 >::New();
  mToolToTracker->Identity();
  for (int identifedMarkerIndex=0; identifedMarkerIndex<this->MT->mtGetIdentifiedMarkersCount(); identifedMarkerIndex++)
  {
    char* identifiedTemplateName=this->MT->mtGetIdentifiedTemplateName(identifedMarkerIndex);
    vtkTrackerTool* tool = NULL; 
    if ( this->GetToolByPortName(identifiedTemplateName, tool) != PLUS_SUCCESS )
    {
      LOG_DEBUG("Marker " << identifiedTemplateName << " has no associated tool"); 
      continue;
    }


    if(strcmp(identifiedTemplateName,"patientMarker")==0)
    {
      GetReferenceTransformMatrix(identifedMarkerIndex, mTrackerToReference);
      mTrackerToReference->Invert();
      this->ToolTimeStampedUpdate( tool->GetToolName(), mTrackerToReference, TR_OK, this->LastFrameNumber, unfilteredTimestamp);
    }
    else
    {
      GetTransformMatrix(identifedMarkerIndex, mToolToTracker);
      vtkMatrix4x4::Multiply4x4( mTrackerToReference, mToolToTracker , mToolToReference );
      this->ToolTimeStampedUpdate( tool->GetToolName(), mToolToReference, TR_OK, this->LastFrameNumber, unfilteredTimestamp);
    }
    identifiedToolNames.insert(tool->GetPortName());
  }

  // Set status for tools with non-detected markers
  transformMatrix->Identity();
  for ( ToolIteratorType it = this->GetToolIteratorBegin(); it != this->GetToolIteratorEnd(); ++it)
  {
    if (identifiedToolNames.find(it->second->GetToolName())!=identifiedToolNames.end())
    {
      // this tool has been found and update has been already called with the correct transform
      continue;
    }
    ToolTimeStampedUpdate(it->second->GetToolName(), transformMatrix, TR_OUT_OF_VIEW, this->LastFrameNumber, unfilteredTimestamp);   
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------
void vtkMicronTracker::PrintMatrix(vtkMatrix4x4* m)
{
  std::ostringstream dumpStr; 
  for (int i=0; i<4; i++)
  {
    for (int j=0; j<4; j++)
    {
      dumpStr << std::setw(10) <<  std::fixed << std::right << std::setprecision(5) << m->GetElement(i,j);
      if (j==3)
      {
        dumpStr << "   ";
      }
    }
  }
  LOG_INFO(dumpStr.str());
  LOG_DEBUG(dumpStr.str());
}

//----------------------------------------------
void vtkMicronTracker::RefreshMarkerTemplates()
{
  std::vector<std::string> vTemplatesName;
  std::vector<std::string> vTemplatesError;
  std::vector<std::string> vTemplatesWarn;

  int i = 0;
  int callResult = this->MT->mtRefreshTemplates(vTemplatesName, vTemplatesError);
  LOG_DEBUG("Loading the marker templates... ");
  for (i=0; i<vTemplatesName.size(); i++)
  {
    LOG_DEBUG("Loaded " << vTemplatesName[i]);
  }
  if ( callResult == -1)
  {
    for (i=0; i<vTemplatesError.size(); i++)
    {
      LOG_ERROR("MicronTracker error: " << vTemplatesError[i]);
    }
  }
}

//----------------------------------------------
int vtkMicronTracker::GetNumOfLoadedMarkers()
{
  return this->MT->mtGetLoadedTemplatesNum();
}
//----------------------------------------------
void vtkMicronTracker::GetReferenceTransformMatrix(int markerIndex, vtkMatrix4x4* Referencetransform)
{  
  std::vector<double> vRotMat;
  this->MT->mtGetRotations( vRotMat, markerIndex );
  std::vector<double> vPos;
  this->MT->mtGetTranslations(vPos, markerIndex);

  Referencetransform->Identity();
  int rotIndex =0;
  for(int col=0; col < 3; col++)
  {
    for (int row=0; row < 3; row++)
    {
      Referencetransform->SetElement(row, col, vRotMat[rotIndex++]);
    }
  }
  // Add the offset to the last column of the transformation matrix
  Referencetransform->SetElement(0,3,vPos[0]);
  Referencetransform->SetElement(1,3,vPos[1]);
  Referencetransform->SetElement(2,3,vPos[2]);
}
//----------------------------------------------
void vtkMicronTracker::GetTransformMatrix(int markerIndex, vtkMatrix4x4* transformMatrix)
{  
  std::vector<double> vRotMat;
  this->MT->mtGetRotations( vRotMat, markerIndex );
  std::vector<double> vPos;
  this->MT->mtGetTranslations(vPos, markerIndex);

  transformMatrix->Identity();
  int rotIndex =0;
  for(int col=0; col < 3; col++)
  {
    for (int row=0; row < 3; row++)
    {
      transformMatrix->SetElement(row, col, vRotMat[rotIndex++]);
    }
  }
  // Add the offset to the last column of the transformation matrix
  transformMatrix->SetElement(0,3,vPos[0]);
  transformMatrix->SetElement(1,3,vPos[1]);
  transformMatrix->SetElement(2,3,vPos[2]);
}

//----------------------------------------------------------------------------
void vtkMicronTracker::UpdateLeftRightImage()
{
  this->UpdateMutex->Lock();
  this->MT->mtGetLeftRightImageArray(this->LeftImageArray, this->RightImageArray, 0);
  this->UpdateMutex->Unlock();

  if (this->LeftImage != NULL)
  {
    this->LeftImage->Delete();
  }
  if (this->RightImage != NULL)
  {
    this->RightImage->Delete();
  }

  this->LeftImage = vtkImageImport::New();
  this->RightImage = vtkImageImport::New();

  this->LeftImage->SetDataScalarTypeToUnsignedChar();
  this->LeftImage->SetImportVoidPointer((unsigned char*)this->LeftImageArray);
  this->LeftImage->SetDataScalarTypeToUnsignedChar();
  this->LeftImage->SetDataExtent(0,CAM_FRAME_WIDTH-1, 0,CAM_FRAME_HEIGHT-1, 0,0);

  this->RightImage->SetDataScalarTypeToUnsignedChar();
  this->RightImage->SetImportVoidPointer((unsigned char*)this->RightImageArray);
  this->RightImage->SetDataScalarTypeToUnsignedChar();
  this->RightImage->SetDataExtent(0,CAM_FRAME_WIDTH-1, 0,CAM_FRAME_HEIGHT-1, 0,0);
}

//----------------------------------------------------------------------------
vtkImageImport* vtkMicronTracker::GetLeftImage()
{
  this->LeftImage->GlobalWarningDisplayOff();
  return this->LeftImage;
}

//----------------------------------------------------------------------------
vtkImageImport* vtkMicronTracker::GetRightImage()
{
  this->RightImage->GlobalWarningDisplayOff();
  return this->RightImage;
}

//----------------------------------------------------------------------------
void vtkMicronTracker::GetSnapShot(char* testNum, char* identifier)
{
  this->UpdateLeftRightImage();

  //static leftSnapShotCounter = 0;
  //static rightSnapShotCounter = 0;

  // For left image
  std::string fileName = this->MT->mtGetCurrDir();
#if (WIN32)
  fileName += "\\SnapShots\\";
#else
  fileName += "/SnapShots/";
#endif
  fileName += testNum;
  fileName += "_LeftSnapShot_";
  fileName += identifier;
  fileName += ".JPEG";

  PlusVideoFrame::SaveImageToFile(this->GetLeftImage()->GetOutput(), fileName.c_str()); 

  // For right image
  fileName = this->MT->mtGetCurrDir();
#if (WIN32)
  fileName += "\\SnapShots\\";
#else
  fileName += "/SnapShots/";
#endif
  fileName += testNum;
  fileName += "_RightSnapShot_";
  fileName += identifier;
  fileName += ".JPEG"; 

  PlusVideoFrame::SaveImageToFile(this->GetRightImage()->GetOutput(), fileName.c_str()); 
}

//----------------------------------------------------------------------------
int vtkMicronTracker::StopSampling(char* name, double jitter)
{
  return this->MT->mtStopSampling(name, jitter);
}

//----------------------------------------------------------------------------
void vtkMicronTracker::ResetNewSampleFramesCollected()
{
  this->NewSampleFramesCollected = 0;
  this->MT->mtResetSamples();
}

//----------------------------------------------------------------------------
int vtkMicronTracker::SaveTemplate(char* markerName)
{
  int callResult = this->MT->mtSaveMarkerTemplate(markerName);
  this->RefreshMarkerTemplates();
  return callResult;
}

//----------------------------------------------------------------------------
int vtkMicronTracker::GetNumberOfFacetsInMarker(int markerIdx)
{
  //  this->UpdateMutex->Lock();
  return this->MT->mtGetNumOfFacetsInMarker(markerIdx);
  //  this->UpdateMutex->Unlock();
}

//----------------------------------------------------------------------------
int vtkMicronTracker::GetNumberOfTotalFacetsInMarker(int markerIdx)
{
  //  this->UpdateMutex->Lock();
  return this->MT->mtGetNumOfTotalFacetsInMarker(markerIdx);
  //  this->UpdateMutex->Unlock();
}

//----------------------------------------------------------------------------
vtkDoubleArray* vtkMicronTracker::vtkGetIdentifiedMarkersXPoints(int markerIdx)
{
  if (this->Xpoints != NULL)
  {
    this->Xpoints->Delete();
  }
  this->Xpoints = vtkDoubleArray::New();
  double* xpointsTemp;
  // Note: Removing lock/unlock here causes crash
  this->UpdateMutex->Lock();
  this->MT->mtGetIdentifiedMarkersXPoints(xpointsTemp, markerIdx);
  int arraySize = this->MT->mtGetNumOfFacetsInMarker(markerIdx);
  this->UpdateMutex->Unlock();
  // Each facet contains 16*numberOfFacets piece of information 
  this->Xpoints->SetArray(xpointsTemp,arraySize*16,1);
  return this->Xpoints;
}

//----------------------------------------------------------------------------
vtkDoubleArray* vtkMicronTracker::vtkGetUnidentifiedMarkersEnds(int vectorIdx)
{  
  if (this->VectorEnds != NULL)
  {
    this->VectorEnds->Delete();
  }
  this->VectorEnds = vtkDoubleArray::New();
  double* vectorEndsTemp;
  // Note: Removing lock/unlock here causes crash
  this->UpdateMutex->Lock();
  this->MT->mtGetUnidentifiedMarkersEnds(vectorEndsTemp, vectorIdx);
  this->UpdateMutex->Unlock();
  // Each facet contains 8 piece of information
  this->VectorEnds->SetArray(vectorEndsTemp,8,1);
  return this->VectorEnds;
}

//----------------------------------------------------------------------------
void vtkMicronTracker::SelectCamera(int n)
{
  this->UpdateMutex->Lock();
  this->MT->mtSelectCamera(n);
  this->UpdateMutex->Unlock();
}

//----------------------------------------------------------------------------
int vtkMicronTracker::GetCurrCamIndex()
{
  int currCamIndex = this->MT->mtGetCurrCamIndex();
  return currCamIndex;
}

//----------------------------------------------------------------------------
void vtkMicronTracker::SetTemplatesMatchingTolerance(double mTolerance)
{
  this->MT->mtSetTemplMatchTolerance(mTolerance);
}

//----------------------------------------------------------------------------
double vtkMicronTracker::GetTemplatesMatchingTolerance()
{
  return this->MT->mtGetTemplMatchTolerance();
}

//----------------------------------------------------------------------------
double vtkMicronTracker::GetTemplatesMatchingToleranceDefault()
{
  return this->MT->mtGetTemplMatchToleranceDefault();
}

//----------------------------------------------------------------------------
void vtkMicronTracker::SetPredictiveFrameInterleave(int predInterleave)
{
  this->MT->mtSetPredictiveFramesInterleave(predInterleave);
}

//----------------------------------------------------------------------------
int vtkMicronTracker::GetPredictiveFrameInterleave()
{
  return this->MT->mtGetPredictiveFramesInterleave();
}

//-----------------------------------------------------------------------------
void vtkMicronTracker::SetAdjustCamAfterEveryProcess(short autoCamExposure)
{
  this->MT->mtSetAdjustCamAfterEveryProcess(autoCamExposure);
}

//----------------------------------------------------------------------------
short vtkMicronTracker::GetAdjustCamAfterEveryProcess()
{
  return this->MT->mtGetAdjustCamAfterEveryProcess();
}

//----------------------------------------------------------------------------
void vtkMicronTracker::SetPredictiveTracking(short predictiveTracking)
{
  this->MT->mtSetPredictiveTracking(predictiveTracking);
}

//----------------------------------------------------------------------------
short vtkMicronTracker::GetPredictiveTracking()
{
  return this->MT->mtGetPredictiveTracking();
}

//----------------------------------------------------------------------------
int vtkMicronTracker::GetShutterPref()
{
  return this->MT->mtGetShutterPreference();
}

//----------------------------------------------------------------------------
void vtkMicronTracker::SetShutterPref(int shutterPref)
{
  this->MT->mtSetShutterPreference(shutterPref);
}

//----------------------------------------------------------------------------
double vtkMicronTracker::GetShutterTime(int cam)
{
  return  this->MT->mtGetShutterTime(cam);
}

//----------------------------------------------------------------------------
void vtkMicronTracker::SetShutterTime(double shutterTime, int cam)
{
  this->MT->mtSetShutterTime(shutterTime, cam);
}

//----------------------------------------------------------------------------
double vtkMicronTracker::GetMinShutterTime(int cam)
{
  return this->MT->mtGetMinShutterTime(cam);
}

//----------------------------------------------------------------------------
double vtkMicronTracker::GetMaxShutterTime(int cam)
{
  return this->MT->mtGetMaxShutterTime(cam);
}

//----------------------------------------------------------------------------
double vtkMicronTracker::GetGain(int cam)
{
  return this->MT->mtGetGain(cam);
}

//----------------------------------------------------------------------------
void vtkMicronTracker::SetGain(double gain, int cam)
{
  this->MT->mtSetGain(gain, cam);
}

//----------------------------------------------------------------------------
double vtkMicronTracker::GetMinGain(int cam)
{
  return this->MT->mtGetMinGain(cam);
}

//----------------------------------------------------------------------------
double vtkMicronTracker::GetMaxGain(int cam)
{
  return this->MT->mtGetMaxGain(cam);
}

//-----------------------------------------------------------------------------
double vtkMicronTracker::GetDBGain(int cam)
{
  return this->MT->mtGetDBGain(cam);
}

//----------------------------------------------------------------------------
double vtkMicronTracker::GetExposure(int cam)
{
  return this->MT->mtGetExposure(cam);
}

//----------------------------------------------------------------------------
void vtkMicronTracker::SetExposure(double exposure, int cam)
{
  this->MT->mtSetExposure(exposure, cam);
}

//----------------------------------------------------------------------------
double vtkMicronTracker::GetMinExposure(int cam)
{
  return this->MT->mtGetMinExposure(cam);
}

//----------------------------------------------------------------------------
double vtkMicronTracker::GetMaxExposure(int cam)
{
  return this->MT->mtGetMaxExposure(cam);
}

//----------------------------------------------------------------------------
int vtkMicronTracker::GetCamAutoExposure(int cam)
{
  return this->MT->mtGetCamAutoExposure(cam);
}

//----------------------------------------------------------------------------
void vtkMicronTracker::SetCamAutoExposure(int autoExposure, int cam)
{
  this->MT->mtSetCamAutoExposure(autoExposure, cam);
}

//----------------------------------------------------------------------------
double vtkMicronTracker::GetLatestFrameTime(int cam)
{
  return this->MT->mtGetLatestFrameTime(cam);
}

//-----------------------------------------------------------------------------
char* vtkMicronTracker::GetTemplateName(int markerIdx)
{  
  return this->MT->mtGetTemplateName(markerIdx);
}

//-----------------------------------------------------------------------------
char* vtkMicronTracker::GetIdentifiedTemplateName(int markerIdx)
{
  return (char*)(this->MT->mtGetIdentifiedTemplateName(markerIdx));
}

//-----------------------------------------------------------------------------
void vtkMicronTracker::SetTemplateName(int markerIdx, char* templateName)
{
  int callResult =  this->MT->mtSetTemplateName(markerIdx, templateName);
  if (callResult != 0)
  {
    LOG_ERROR(this->MT->mtGetErrorString());
  }
  this->RefreshMarkerTemplates();
}

//
//-----------------------------------------------------------------------------
void vtkMicronTracker::DeleteTemplate(int markerIdx)
{
  int callResult = this->MT->mtDeleteTemplate(markerIdx);
  if (callResult != 0 )
  {
    LOG_ERROR(this->MT->mtGetErrorString());
  }
  this->RefreshMarkerTemplates();
}

//-----------------------------------------------------------------------------
vtkLongArray* vtkMicronTracker::GetLatestFramePixHistogram(int cam, int ssr)
{
  long* paPixHist;
  this->MT->mtGetLatestFramePixHistogram(paPixHist, ssr, cam);

  vtkLongArray* pTemp = vtkLongArray::New();
  pTemp->SetArray(paPixHist, 256, 1);

  vtkLongArray* pPixHist = vtkLongArray::New();
  pPixHist->DeepCopy(pTemp);
  return pPixHist;
}

//----------------------------------------------------------------------------
int vtkMicronTracker::GetNumOfFramesGrabbed(int cam)
{
  return this->MT->mtGetNumOfFramesGrabbed(cam);
}

//----------------------------------------------------------------------------
int vtkMicronTracker::GetBitsPerPixel(int cam)
{
  return this->MT->mtGetBitsPerPixel(cam);
}

//----------------------------------------------------------------------------
// Protected Methods

// helper method to strip whitespace
static char *vtkStripWhitespace(char *text)
{
  int n = strlen(text);
  // strip from right
  while (--n >= 0) 
  {
    if (isspace(text[n])) 
    {
      text[n] = '\0';
    }
    else 
    {
      break;
    }
  }
  // strip from left
  while (isspace(*text)) 
  {
    text++;
  }
  return text;
}

PlusStatus vtkMicronTracker::ReadConfiguration( vtkXMLDataElement* config )
{
  // Read superclass configuration first
  Superclass::ReadConfiguration(config); 

  LOG_TRACE( "vtkMicronTrackerTracker::ReadConfiguration" ); 
  if ( config == NULL ) 
  {
    LOG_ERROR("Unable to find vtkMicronTrackerTracker XML data element");
    return PLUS_FAIL; 
  }

  vtkSmartPointer<vtkXMLDataElement> dataCollectionConfig = config->FindNestedElementWithName("USDataCollection");
  if (dataCollectionConfig == NULL)
  {
    LOG_ERROR("Cannot find USDataCollection element in XML tree!");
    return PLUS_FAIL;
  }

  vtkSmartPointer<vtkXMLDataElement> trackerConfig = dataCollectionConfig->FindNestedElementWithName("Tracker"); 
  if (trackerConfig == NULL) 
  {
    LOG_ERROR("Cannot find Tracker element in XML tree!");
    return PLUS_FAIL;
  }	

  return PLUS_SUCCESS;
}

PlusStatus vtkMicronTracker::Connect()
{ 
  if (this->IsMicronTrackingInitialized)
  {
    LOG_DEBUG("Already connected to MicronTracker");
    return PLUS_SUCCESS;
  }

  if (this->MT->mtInit()!=1)
  {
    LOG_ERROR("Error in initializing Micron Tracker");
    return PLUS_FAIL;
  }

  // Try to attach the cameras till find the cameras
  if (this->MT->mtSetupCameras()!=1)
  {
    LOG_ERROR("Error in initializing Micron Tracker: setup cameras failed. Check the camera connections.");
    this->MT->mtEnd();
    return PLUS_FAIL;
  }

  int numOfCameras = this->MT->mtGetNumOfCameras();
  if (numOfCameras==0)
  {
    LOG_ERROR("Error in initializing Micron Tracker: no cameras attached. Check the camera connections.");
    this->MT->mtEnd();
    return PLUS_FAIL;
  }

  RefreshMarkerTemplates();

  this->IsMicronTrackingInitialized=1;

  LOG_DEBUG("Number of attached cameras: " << numOfCameras );
  this->CameraInfoList.clear();
  for (int i=0; i<numOfCameras; i++)
  {
    CameraInfo info;
    info.serialNum=this->MT->mtGetSerialNum(i);
    info.xResolution=this->MT->mtGetXResolution(i);
    info.yResolution=this->MT->mtGetYResolution(i);
    info.numOfSensors=this->MT->mtGetNumOfSensors(i);
    this->CameraInfoList.push_back(info);
  }
  if (numOfCameras>0)
  {
    LOG_DEBUG("Serial number of the current camera: " << this->CameraInfoList[0].serialNum);
    LOG_DEBUG("Resolution of the current camera: " << this->CameraInfoList[0].xResolution << " x " << this->CameraInfoList[0].yResolution );
  }

  return PLUS_SUCCESS;
}

PlusStatus vtkMicronTracker::Disconnect()
{ 
  this->MT->mtEnd();  
  return PLUS_SUCCESS;
}

int vtkMicronTracker::GetNumOfCameras()
{
  return this->MT->mtGetNumOfCameras();
}

int vtkMicronTracker::GetNumOfIdentifiedMarkers()
{
  return this->MT->mtGetIdentifiedMarkersCount();
}

int vtkMicronTracker::GetNumOfUnidentifiedMarkers()
{
  return this->MT->mtGetUnidentifiedMarkersCount();
}

double vtkMicronTracker::GetLightCoolness(int cam /* = -1 */) 
{
  return MT->mtGetLightCoolness(cam);
}

int vtkMicronTracker::GetLatestFrameHazard() 
{
  return MT->mtGetLatestFrameHazard();
}
