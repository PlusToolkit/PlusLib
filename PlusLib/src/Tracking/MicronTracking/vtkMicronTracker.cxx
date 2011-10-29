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

#include "vtkImageData.h"
#include "PlusVideoFrame.h"

//#include "Cameras.h"
//#include "MTC.h"
//#include "MCamera.h"

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
  this->SendMatrix = vtkMatrix4x4::New();
  this->IsMicronTracking = 0;
  this->MT = new MicronTrackerInterface();

  // for accurate timing
  this->UpdateNominalFrequency=20.0;
  this->LastFrameNumber=0;
  this->ReferenceTool=0;

  // Initialize 
  this->serialNums = vtkIntArray::New();
  this->xResolutions = vtkIntArray::New();
  this->yResolutions = vtkIntArray::New();
  this->sensorNums = vtkIntArray::New();

  for (int i=0; i< MAX_TOOL_NUM; i++)
  {
    this->previousTransformMatrix[i] = vtkMatrix4x4::New();
    this->previousTransformMatrix[i]->Identity();
    rm[i] = vtkMatrix4x4::New();
    rm[i] = this->previousTransformMatrix[i];
  }

  this->isAdditionalFacetAdding = 0;
  this->isCollectingNewSamples = 0;
  this->newSampleFramesCollected = 0;
  this->numOfLoadedTools = 0;

  this->ReadToolsFile();
  this->SetNumberOfTools(this->numOfLoadedTools);
  //  finalCalibrationMatrix = vtkMatrix4x4::New();

  leftImage = NULL;
  rightImage = NULL;
  xpoints = NULL;
  vectorEnds = NULL;
  //  leftImageArray = new unsigned char();
  //  rightImageArray = new unsigned char();

}

//----------------------------------------------------------------------------
void vtkMicronTracker::ReadToolsFile()
{
  string toolsFileAddress = this->MT->mtGetCurrDir();
#if(WIN32)
  toolsFileAddress += "\\Tools\\ToolNames.txt";
#else
  toolsFileAddress += "/Tools/ToolNames.txt";
#endif

  std::ifstream fs;
  string toolName;
  string toolClassName;
  fs.open(toolsFileAddress.c_str());
  if (fs.fail())
  {
    LOG_ERROR("Couldn't open tools file");
  }
  int counter = 0;
  while (true)
  {
    std::getline(fs, toolFileLines[counter]);
    if (fs.fail()) 
    {
      break; // no more lines to read
    }
    // Find the tool's name in the string. (The tool name is separated from the tool class name by a comma.
    int lineSize = toolFileLines[counter].size();
    int commaPos = toolFileLines[counter].find(",");
    toolName = toolFileLines[counter].substr(0,commaPos);
    // Add the tool name to the storage container
    this->toolNames[this->numOfLoadedTools] = toolName;

    // Find the tool's class name in the string.
    toolClassName = toolFileLines[counter].substr(commaPos+1, lineSize);
    // Add the tool's class name to the storage container
    this->toolClassNames[this->numOfLoadedTools] = toolClassName;
    this->numOfLoadedTools++;
    counter++;
  }
  fs.close();   
}

//----------------------------------------------------------------------------
char* vtkMicronTracker::GetToolName(int toolIndex)
{
  return (char*)this->toolNames[toolIndex].c_str();
}

//----------------------------------------------------------------------------
char* vtkMicronTracker::GetToolClassName(int toolIndex)
{
  return (char*)this->toolClassNames[toolIndex].c_str();
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
  // This function should not be called while the device is tracking (see the documentation of vtkTracker)
  if (this->IsMicronTracking)
  {
    return PLUS_FAIL;
  }
  int callResult = 0;
  callResult = this->MT->mtInit();

  if (1 == callResult)
  {
    // Try to attach the cameras till find the cameras
    callResult = this->MT->mtSetupCameras();
    if (callResult == 1)
    {
      this->numOfCameras = this->MT->mtGetNumOfCameras();
      const char* camString = (numOfCameras = 1) ? " camera is " : " cameras are ";
      LOG_DEBUG(numOfCameras << camString << "attached!");
      for (int i=0; i<numOfCameras; i++)
      {
        serialNums->InsertValue(i, this->MT->mtGetSerialNum(i));
        xResolutions->InsertValue(i, this->MT->mtGetXResolution(i));
        yResolutions->InsertValue(i, this->MT->mtGetYResolution(i));
        sensorNums->InsertValue(i, this->MT->mtGetNumOfSensors(i));
      }
      LOG_DEBUG("Serial number of the current camera: " << serialNums->GetValue(0));
      LOG_DEBUG("Resolution of the current camera: " << xResolutions->GetValue(0) << " x " << yResolutions->GetValue(0));
      this->IsMicronTracking = 1;

    }
    else
    {
      this->MT->mtEnd();
      this->MT = NULL;
      callResult = 0;
      LOG_ERROR("Failed to find any cameras! \nCheck the camera connections..."); //vtkErrorMacro( << "No camera found!");
    }
  }
  else
  {
    this->MT->mtEnd();
    this->MT = NULL;
    LOG_ERROR("Error in initializing Micron Tracker!"); //vtkErrorMacro( << "Error in initializing COM components! ");
  }
  return (callResult==0)?PLUS_FAIL:PLUS_SUCCESS;
} 

//----------------------------------------------------------------------------
PlusStatus vtkMicronTracker::InternalStartTracking()
{
  if (this->IsMicronTracking)
  {
    this->RefreshMarkerTemplates();
  }
  return (this->IsMicronTracking)?PLUS_SUCCESS:PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkMicronTracker::InternalStopTracking()
{
  if (this->MT != NULL )
  {
    //    this->UpdateMutex->Lock();
    // Ends the COM services and calls the destructor of MicronTracker class
    this->MT->mtEnd();
    this->MT = NULL;
    this->IsMicronTracking = 0;
    for (int i=0; i<MAX_TOOL_NUM; i++)
    {
      //if ( this->previousTransformMatrix[i] != NULL )
      //  this->previousTransformMatrix[i]->Delete();
      if (this->rm[i] != NULL)
      {
        this->rm[i]->Delete();
      }
    }
    if ( this->serialNums != NULL )
    {
      this->serialNums->Delete();
    }
    if ( this->yResolutions != NULL )
    {
      this->yResolutions->Delete();
      // this->UpdateMutex->Unlock();
    }
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkMicronTracker::InternalUpdate()
{
  int callResult = 0;

  if (this->IsMicronTracking)
  {
    // If grabing a frame was not successful prevent it from calling the mtGrabFrame
    // method of this->MT, until the problem is solved.
    callResult = this->MT->mtGrabFrame();

    if (-1 == callResult)
    {
      this->InternalStopTracking();
      LOG_ERROR("Error in grabing a frame!\n" << this->MT->mtGetErrorString());
      return PLUS_FAIL;
    }
    else
    {
      callResult = this->MT->mtProcessFrame();
    }
    if (-1 == callResult)
    {
      this->InternalStopTracking();
      LOG_ERROR("Error in processing a frame!\n" << this->MT->mtGetErrorString());
      return PLUS_FAIL;
    }
    this->MT->mtFindIdentifiedMarkers();
    this->MT->mtFindUnidentifiedMarkers();
    // Collecting new samples if creating a new template by the user
    if (this->isCollectingNewSamples == 1)
    {
      callResult = this->MT->mtCollectNewSamples(this->isAdditionalFacetAdding);
      if (callResult == -1)
      {
        LOG_ERROR("Less than two vectors are detected.");
      }
      else if (callResult == 1)
      {
        LOG_ERROR("More than two vectors are detected.");
      }
      else if (callResult == 99)
      {
        LOG_ERROR("No known facet detected.");
      }
      else
      {
        this->newSampleFramesCollected++;
        LOG_TRACE("Samples collected so far: " << newSampleFramesCollected);
      }
    }
    // Setting the timestamp
    this->LastFrameNumber++;
    const double unfilteredTimestamp = vtkAccurateTimer::GetSystemTime();

    int identifiedMarkerIndexAssignedToTool=0;
    this->numOfIdentifiedMarkers = this->MT->mtGetIdentifiedMarkersCount();
    //  if(this->MT->mtGetIdentifiedMarkersCount() == 0)
    //   {  
    //     LOG_ERROR("No Markers found");
    //   }
    for (int i=0; i< this->NumberOfTools; i++)
    {
      // If no marker is assigned to the tool, grab the first one available and so on till 
      // the list of the identified markers is exhausted (in which case the statusFlag would
      // be set to TR_OUT_OF_VIEW.
      if (this->markerIndexAssingedToTools[i] == 99)
      {
        //if (i<this->numOfIdentifiedMarkers)
        //  statusFlags[i] = 0;
        //else
        statusFlags[i] = TR_OUT_OF_VIEW;
      }
      else
      {  
        if (this->MT->mtGetMarkerStatus(this->markerIndexAssingedToTools[i], &identifiedMarkerIndexAssignedToTool) != MTI_MARKER_CAPTURED)
          //if (this->MT->mtGetStatus() != mti_utils::MTI_MARKER_CAPTURED)
        {  
          statusFlags[i] = 0;
        }
        else
        {
          statusFlags[i] = TR_OUT_OF_VIEW;//TR_MISSING;  
        }
      }
      SendMatrix->DeepCopy(this->GetTransformMatrix(identifiedMarkerIndexAssignedToTool, i));
      this->ToolTimeStampedUpdate(i, this->SendMatrix, (TrackerStatus)statusFlags[i], this->LastFrameNumber, unfilteredTimestamp);   
    }
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------
void vtkMicronTracker::SetMarkerIndexAssignedToTool(int toolIndex, int markerIndex)
{
  this->markerIndexAssingedToTools[toolIndex] = markerIndex;
}

//----------------------------------------------
void vtkMicronTracker::print_matrix(vtkMatrix4x4* m)
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
}

//----------------------------------------------
void vtkMicronTracker::RefreshMarkerTemplates()
{
  vector<string> vTemplatesName;
  vector<string> vTemplatesError;
  vector<string> vTemplatesWarn;

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
vtkMatrix4x4* vtkMicronTracker::GetTransformMatrix(int markerIndex, int toolIndex)
{  
  // Safety check
  if ( statusFlags[toolIndex] == 0 && markerIndex < this->numOfIdentifiedMarkers)
  {  
    //this->UpdateMutex->Lock();
    rm[markerIndex]->Identity();
    vector<double> vRotMat;
    this->MT->mtGetRotations( vRotMat, markerIndex );
    vector<double> vPos;
    this->MT->mtGetTranslations(vPos, markerIndex);
    //this->UpdateMutex->Unlock();
    int rotIndex =0;

    for(int col=0; col < 3; col++)
    {
      for (int row=0; row < 3; row++)
      {
        rm[markerIndex]->SetElement(row, col, vRotMat[rotIndex++]);
      }
    }
    // Add the offset to the last column of the transformation matrix
    rm[markerIndex]->SetElement(0,3,vPos[0]);
    rm[markerIndex]->SetElement(1,3,vPos[1]);
    rm[markerIndex]->SetElement(2,3,vPos[2]);

    this->previousTransformMatrix[markerIndex] = rm[markerIndex];
    return rm[markerIndex];
  }
  else
  {
    return this->previousTransformMatrix[markerIndex];
  }
}

//----------------------------------------------------------------------------
void vtkMicronTracker::UpdateLeftRightImage()
{
  this->UpdateMutex->Lock();
  this->MT->mtGetLeftRightImageArray(leftImageArray, rightImageArray, 0);
  this->numOfIdentifiedMarkers = this->MT->mtGetIdentifiedMarkersCount();
  this->numOfUnidentifiedMarkers = this->MT->mtGetUnidentifiedMarkersCount();
  this->UpdateMutex->Unlock();

  if (leftImage != NULL)
  {
    leftImage->Delete();
  }
  if (rightImage != NULL)
  {
    rightImage->Delete();
  }

  leftImage = vtkImageImport::New();
  rightImage = vtkImageImport::New();

  leftImage->SetDataScalarTypeToUnsignedChar();
  leftImage->SetImportVoidPointer((unsigned char*)leftImageArray);
  leftImage->SetDataScalarTypeToUnsignedChar();
  leftImage->SetDataExtent(0,CAM_FRAME_WIDTH-1, 0,CAM_FRAME_HEIGHT-1, 0,0);

  rightImage->SetDataScalarTypeToUnsignedChar();
  rightImage->SetImportVoidPointer((unsigned char*)rightImageArray);
  rightImage->SetDataScalarTypeToUnsignedChar();
  rightImage->SetDataExtent(0,CAM_FRAME_WIDTH-1, 0,CAM_FRAME_HEIGHT-1, 0,0);
}

//----------------------------------------------------------------------------
vtkImageImport* vtkMicronTracker::GetLeftImage()
{
  leftImage->GlobalWarningDisplayOff();
  return leftImage;
}

//----------------------------------------------------------------------------
vtkImageImport* vtkMicronTracker::GetRightImage()
{
  rightImage->GlobalWarningDisplayOff();
  return rightImage;
}

//----------------------------------------------------------------------------
void vtkMicronTracker::GetSnapShot(char* testNum, char* identifier)
{
  this->UpdateLeftRightImage();

  //static leftSnapShotCounter = 0;
  //static rightSnapShotCounter = 0;

  // For left image
  string fileName = this->MT->mtGetCurrDir();
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
  this->newSampleFramesCollected = 0;
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
  if (xpoints != NULL)
  {
    xpoints->Delete();
  }
  xpoints = vtkDoubleArray::New();
  double* xpointsTemp;
  // Note: Removing lock/unlock here causes crash
  this->UpdateMutex->Lock();
  this->MT->mtGetIdentifiedMarkersXPoints(xpointsTemp, markerIdx);
  int arraySize = this->MT->mtGetNumOfFacetsInMarker(markerIdx);
  this->UpdateMutex->Unlock();
  // Each facet contains 16*numberOfFacets piece of information 
  xpoints->SetArray(xpointsTemp,arraySize*16,1);
  return xpoints;
}

//----------------------------------------------------------------------------
vtkDoubleArray* vtkMicronTracker::vtkGetUnidentifiedMarkersEnds(int vectorIdx)
{  
  if (vectorEnds != NULL)
  {
    vectorEnds->Delete();
  }
  vectorEnds = vtkDoubleArray::New();
  double* vectorEndsTemp;
  // Note: Removing lock/unlock here causes crash
  this->UpdateMutex->Lock();
  this->MT->mtGetUnidentifiedMarkersEnds(vectorEndsTemp, vectorIdx);
  this->UpdateMutex->Unlock();
  // Each facet contains 8 piece of information
  vectorEnds->SetArray(vectorEndsTemp,8,1);
  return vectorEnds;
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
  this->currCamIndex = this->MT->mtGetCurrCamIndex();
  return this->currCamIndex;
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
/*
PlusStatus vtkMicronTracker::ReadConfiguration( vtkXMLDataElement* config )
{
  // Read superclass configuration first
  Superclass::ReadConfiguration(config); 

  LOG_TRACE( "vtkAscension3DGTracker::ReadConfiguration" ); 
  if ( config == NULL ) 
  {
    LOG_ERROR("Unable to find Ascension3DGTracker XML data element");
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


	this->pCameras = new Cameras();
	int result = this->pCameras->AttachAvailableCameras();

	if (result == 0 &&  this->pCameras->getCount() >= 1 ) {
		this->pCurrCam = this->pCameras->m_vCameras[0];
		if (this->pCurrCam->getXRes() > 1200 ) this->isShowingHalfSize = true;
		cout << " Camera attached successfully " << endl;
	} else {
		LOG_ERROR(" No camera available or missing calibration file. Please also check that MTHome system environment variable is set ");
		return PLUS_FAIL;
	}



  return PLUS_SUCCESS;
}
*/