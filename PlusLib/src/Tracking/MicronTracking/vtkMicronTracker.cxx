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

#include "vtkMicronTracker.h"
#include <fstream>
#include <iostream>

#include "vtkImageData.h"

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
  this->Version = NULL;
  this->SendMatrix = vtkMatrix4x4::New();
  this->IsMicronTracking = 0;
  MT = new MicronTrackerInterface();
  
  // for accurate timing
  this->Timer = vtkFrameToTimeConverter::New();
  this->Timer->SetNominalFrequency(20.0);
  
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
  
  if (MT == NULL)
    {
      MT = new MicronTrackerInterface();
    }
}

//----------------------------------------------------------------------------
void vtkMicronTracker::ReadToolsFile()
{
  string toolsFileAddress = MT->mtGetCurrDir();
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
    std::cout << "Couldn't open tools file" << std::endl;
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
  if ( MT != NULL )
    {
      MT->mtEnd();
    }
  if (this->Version)
    {
      delete [] this->Version;
    }
  if (this->Timer)
    {
      this->Timer->Delete();
    }
}

//----------------------------------------------------------------------------
void vtkMicronTracker::UpdateINI()
{
  MT->mtUpdateINI();
}

//----------------------------------------------------------------------------
int vtkMicronTracker::Probe()
{  
  // This function should not be called while the device is tracking (see the documentation of vtkTracker)
  if (this->IsMicronTracking)
    {
      return 1;
    }
  int callResult = 0;
  callResult = MT->mtInit();
  
  if (1 == callResult)
    {
      // Try to attach the cameras till find the cameras
      callResult = MT->mtSetupCameras();
      if (callResult == 1)
  {
    this->numOfCameras = MT->mtGetNumOfCameras();
    const char* camString = (numOfCameras = 1) ? " camera is " : " cameras are ";
    vtkstd::cout << endl << numOfCameras << camString << "attached!" << endl;
    for (int i=0; i<numOfCameras; i++)
      {
        serialNums->InsertValue(i, MT->mtGetSerialNum(i));
        xResolutions->InsertValue(i, MT->mtGetXResolution(i));
        yResolutions->InsertValue(i, MT->mtGetYResolution(i));
        sensorNums->InsertValue(i, MT->mtGetNumOfSensors(i));
      }
    std::cout << "Serial number of the current camera: " << serialNums->GetValue(0) << std::endl;
    std::cout << "Resolution of the current camera: " << xResolutions->GetValue(0) << " x " << yResolutions->GetValue(0) << std::endl;
      this->IsMicronTracking = 1;
      
  }
      else
  {
    MT->mtEnd();
    MT = NULL;
    callResult = 0;
    std::cout << "Failed to find any cameras! \nCheck the camera connections..." << std::endl;//vtkErrorMacro( << "No camera found!");
  }
    }
  else
    {
      MT->mtEnd();
      MT = NULL;
      std::cout << "Error in initializing Micron Tracker!" << std::endl; //vtkErrorMacro( << "Error in initializing COM components! ");
    }
  return callResult;
} 

//----------------------------------------------------------------------------
int vtkMicronTracker::InternalStartTracking()
{
  if (this->IsMicronTracking)
    {
      this->RefreshMarkerTemplates();
      // for accurate timing
      this->Timer->Initialize();
    }
  return this->IsMicronTracking;
}

//----------------------------------------------------------------------------
int vtkMicronTracker::InternalStopTracking()
{
  if (MT != NULL )
    {
      //    this->UpdateMutex->Lock();
      // Ends the COM services and calls the destructor of MicronTracker class
      MT->mtEnd();
      MT = NULL;
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
  return 1;
}

//----------------------------------------------------------------------------
void vtkMicronTracker::InternalUpdate()
{
  int callResult = 0;
  
  if (this->IsMicronTracking)
    {
      // If grabing a frame was not successful prevent it from calling the mtGrabFrame
      // method of MT, until the problem is solved.
      callResult = MT->mtGrabFrame();
      
      if (-1 == callResult)
  {
    this->InternalStopTracking();
    std::cout << "Error in grabing a frame!\n" << MT->mtGetErrorString() << std::endl;
    return;
  }
      else
  {
    callResult = MT->mtProcessFrame();
  }
      if (-1 == callResult)
  {
    this->InternalStopTracking();
    std::cout << "Error in processing a frame!\n" << MT->mtGetErrorString() << std::endl;
    return;
  }
      MT->mtFindIdentifiedMarkers();
      MT->mtFindUnidentifiedMarkers();
      // Collecting new samples if creating a new template by the user
      if (this->isCollectingNewSamples == 1)
  {
    callResult = MT->mtCollectNewSamples(this->isAdditionalFacetAdding);
    if (callResult == -1)
      {
        std::cout << "Less than two vectors are detected." << std::endl;
      }
    else if (callResult == 1)
      {
        std::cout << "More than two vectors are detected." << std::endl;
      }
    else if (callResult == 99)
      {
        std::cout << "No known facet detected." << std::endl;
      }
    else
      {
        this->newSampleFramesCollected++;
        std::cout << "Samples collected so far: " << newSampleFramesCollected << std::endl;
      }
  }
      // Setting the timestamp
      static unsigned long framenum = 0;
      framenum = Timer->GetLastFrame() + 1;
      this->Timer->SetLastFrame(framenum);
      
      static double timestamp;
      timestamp = this->Timer->GetTimeStampForFrame(framenum);
      
      int* identifiedMarkerIndexAssignedToTool = new int(0);
      this->numOfIdentifiedMarkers = MT->mtGetIdentifiedMarkersCount();
     //  if(MT->mtGetIdentifiedMarkersCount() == 0)
//   {  
//     cout << "No Markers found\n";
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
        if (MT->mtGetMarkerStatus(this->markerIndexAssingedToTools[i], identifiedMarkerIndexAssignedToTool) != MTI_MARKER_CAPTURED)
    //if (MT->mtGetStatus() != mti_utils::MTI_MARKER_CAPTURED)
    {  
      statusFlags[i] = 0;
    }
        else
    {
      statusFlags[i] = TR_OUT_OF_VIEW;//TR_MISSING;  
    }
      }
    SendMatrix->DeepCopy(this->GetTransformMatrix(*identifiedMarkerIndexAssignedToTool, i));
    this->ToolUpdate(i,this->SendMatrix,statusFlags[i],timestamp);
  }
      delete identifiedMarkerIndexAssignedToTool;
      identifiedMarkerIndexAssignedToTool = 0;
    }
}

//----------------------------------------------
void vtkMicronTracker::SetMarkerIndexAssignedToTool(int toolIndex, int markerIndex)
{
  this->markerIndexAssingedToTools[toolIndex] = markerIndex;
}

//----------------------------------------------
void vtkMicronTracker::print_matrix(vtkMatrix4x4* m)
{
  for (int i=0; i<4; i++)
    {
      for (int j=0; j<4; j++)
  {
    std::cout << std::setw(10) <<  std::fixed << std::right << std::setprecision(5) << m->GetElement(i,j);
    if (j==3)
      {
        std::cout << "\n";
      }
  }
    }
  std::cout << "\n";
}

//----------------------------------------------
void vtkMicronTracker::RefreshMarkerTemplates()
{
  vector<string> vTemplatesName;
  vector<string> vTemplatesError;
  vector<string> vTemplatesWarn;
  
  int i = 0;
  int callResult = MT->mtRefreshTemplates(vTemplatesName, vTemplatesError);
  std::cout << "\nLoading the marker templates... " << std::endl;
  for (i=0; i<vTemplatesName.size(); i++)
    {
      std::cout << "\tLoaded " << vTemplatesName[i] << std::endl;
    }
  if ( callResult == -1)
    {
    for (i=0; i<vTemplatesError.size(); i++)
      {
  std::cout << "\t" << vTemplatesError[i] << std::endl;
      }
    }
}

//----------------------------------------------
int vtkMicronTracker::GetNumOfLoadedMarkers()
{
  return MT->mtGetLoadedTemplatesNum();
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
      MT->mtGetRotations( vRotMat, markerIndex );
      vector<double> vPos;
      MT->mtGetTranslations(vPos, markerIndex);
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
  MT->mtGetLeftRightImageArray(leftImageArray, rightImageArray, 0);
  this->numOfIdentifiedMarkers = MT->mtGetIdentifiedMarkersCount();
  this->numOfUnidentifiedMarkers = MT->mtGetUnidentifiedMarkersCount();
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
  //imageWriter = vtkJPEGWriter()
  this->UpdateLeftRightImage();
  vtkJPEGWriter* imageWriter = vtkJPEGWriter::New();
  vtkImageFlip* flip = vtkImageFlip::New();
  flip->SetFilteredAxes(1);
  
  //static leftSnapShotCounter = 0;
  //static rightSnapShotCounter = 0;
  
  // For left image
  flip->SetInput(this->GetLeftImage()->GetOutput());
  string fileName = MT->mtGetCurrDir();
#if (WIN32)
  fileName += "\\SnapShots\\";
#else
  fileName += "/SnapShots/";
#endif
  fileName += testNum;
  fileName += "_LeftSnapShot_";
  fileName += identifier;
  fileName += ".JPEG";
  
  imageWriter->SetFilePattern(fileName.c_str());
  imageWriter->SetInput(flip->GetOutput());
  imageWriter->Write();
  
  // For right image
  flip->SetInput(this->GetRightImage()->GetOutput());
  fileName = MT->mtGetCurrDir();
#if (WIN32)
  fileName += "\\SnapShots\\";
#else
  fileName += "/SnapShots/";
#endif
  fileName += testNum;
  fileName += "_RightSnapShot_";
  fileName += identifier;
  fileName += ".JPEG"; 
  
  imageWriter->SetFilePattern(fileName.c_str());
  imageWriter->SetInput(flip->GetOutput());
  imageWriter->Write();
  
  imageWriter->Delete();
  flip->Delete();
}

//----------------------------------------------------------------------------
int vtkMicronTracker::StopSampling(char* name, double jitter)
{
  return MT->mtStopSampling(name, jitter);
}

//----------------------------------------------------------------------------
void vtkMicronTracker::ResetNewSampleFramesCollected()
{
  this->newSampleFramesCollected = 0;
  MT->mtResetSamples();
}

//----------------------------------------------------------------------------
int vtkMicronTracker::SaveTemplate(char* markerName)
{
  int callResult = MT->mtSaveMarkerTemplate(markerName);
  this->RefreshMarkerTemplates();
  return callResult;
}

//----------------------------------------------------------------------------
int vtkMicronTracker::GetNumberOfFacetsInMarker(int markerIdx)
{
  //  this->UpdateMutex->Lock();
  return MT->mtGetNumOfFacetsInMarker(markerIdx);
  //  this->UpdateMutex->Unlock();
}

//----------------------------------------------------------------------------
int vtkMicronTracker::GetNumberOfTotalFacetsInMarker(int markerIdx)
{
  //  this->UpdateMutex->Lock();
  return MT->mtGetNumOfTotalFacetsInMarker(markerIdx);
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
  MT->mtGetIdentifiedMarkersXPoints(xpointsTemp, markerIdx);
  int arraySize = MT->mtGetNumOfFacetsInMarker(markerIdx);
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
  MT->mtGetUnidentifiedMarkersEnds(vectorEndsTemp, vectorIdx);
  this->UpdateMutex->Unlock();
  // Each facet contains 8 piece of information
  vectorEnds->SetArray(vectorEndsTemp,8,1);
  return vectorEnds;
}

//----------------------------------------------------------------------------
void vtkMicronTracker::SelectCamera(int n)
{
  this->UpdateMutex->Lock();
  MT->mtSelectCamera(n);
  this->UpdateMutex->Unlock();
}

//----------------------------------------------------------------------------
int vtkMicronTracker::GetCurrCamIndex()
{
  this->currCamIndex = MT->mtGetCurrCamIndex();
  return this->currCamIndex;
}

//----------------------------------------------------------------------------
void vtkMicronTracker::SetTemplatesMatchingTolerance(double mTolerance)
{
  MT->mtSetTemplMatchTolerance(mTolerance);
}

//----------------------------------------------------------------------------
double vtkMicronTracker::GetTemplatesMatchingTolerance()
{
  return MT->mtGetTemplMatchTolerance();
}

//----------------------------------------------------------------------------
double vtkMicronTracker::GetTemplatesMatchingToleranceDefault()
{
  return MT->mtGetTemplMatchToleranceDefault();
}

//----------------------------------------------------------------------------
void vtkMicronTracker::SetPredictiveFrameInterleave(int predInterleave)
{
  MT->mtSetPredictiveFramesInterleave(predInterleave);
}

//----------------------------------------------------------------------------
int vtkMicronTracker::GetPredictiveFrameInterleave()
{
  return MT->mtGetPredictiveFramesInterleave();
}

//-----------------------------------------------------------------------------
void vtkMicronTracker::SetAdjustCamAfterEveryProcess(short autoCamExposure)
{
  MT->mtSetAdjustCamAfterEveryProcess(autoCamExposure);
}

//----------------------------------------------------------------------------
short vtkMicronTracker::GetAdjustCamAfterEveryProcess()
{
  return MT->mtGetAdjustCamAfterEveryProcess();
}

//----------------------------------------------------------------------------
void vtkMicronTracker::SetPredictiveTracking(short predictiveTracking)
{
  MT->mtSetPredictiveTracking(predictiveTracking);
}

//----------------------------------------------------------------------------
short vtkMicronTracker::GetPredictiveTracking()
{
  return MT->mtGetPredictiveTracking();
}

//----------------------------------------------------------------------------
int vtkMicronTracker::GetShutterPref()
{
  return MT->mtGetShutterPreference();
}

//----------------------------------------------------------------------------
void vtkMicronTracker::SetShutterPref(int shutterPref)
{
  MT->mtSetShutterPreference(shutterPref);
}

//----------------------------------------------------------------------------
double vtkMicronTracker::GetShutterTime(int cam)
{
  return  MT->mtGetShutterTime(cam);
}

//----------------------------------------------------------------------------
void vtkMicronTracker::SetShutterTime(double shutterTime, int cam)
{
  MT->mtSetShutterTime(shutterTime, cam);
}

//----------------------------------------------------------------------------
double vtkMicronTracker::GetMinShutterTime(int cam)
{
  return MT->mtGetMinShutterTime(cam);
}

//----------------------------------------------------------------------------
double vtkMicronTracker::GetMaxShutterTime(int cam)
{
  return MT->mtGetMaxShutterTime(cam);
}

//----------------------------------------------------------------------------
double vtkMicronTracker::GetGain(int cam)
{
  return MT->mtGetGain(cam);
}

//----------------------------------------------------------------------------
void vtkMicronTracker::SetGain(double gain, int cam)
{
  MT->mtSetGain(gain, cam);
}

//----------------------------------------------------------------------------
double vtkMicronTracker::GetMinGain(int cam)
{
  return MT->mtGetMinGain(cam);
}

//----------------------------------------------------------------------------
double vtkMicronTracker::GetMaxGain(int cam)
{
  return MT->mtGetMaxGain(cam);
}

//-----------------------------------------------------------------------------
double vtkMicronTracker::GetDBGain(int cam)
{
  return MT->mtGetDBGain(cam);
}

//----------------------------------------------------------------------------
double vtkMicronTracker::GetExposure(int cam)
{
  return MT->mtGetExposure(cam);
}

//----------------------------------------------------------------------------
void vtkMicronTracker::SetExposure(double exposure, int cam)
{
  MT->mtSetExposure(exposure, cam);
}

//----------------------------------------------------------------------------
double vtkMicronTracker::GetMinExposure(int cam)
{
  return MT->mtGetMinExposure(cam);
}

//----------------------------------------------------------------------------
double vtkMicronTracker::GetMaxExposure(int cam)
{
  return MT->mtGetMaxExposure(cam);
}

//----------------------------------------------------------------------------
int vtkMicronTracker::GetCamAutoExposure(int cam)
{
  return MT->mtGetCamAutoExposure(cam);
}

//----------------------------------------------------------------------------
void vtkMicronTracker::SetCamAutoExposure(int autoExposure, int cam)
{
  MT->mtSetCamAutoExposure(autoExposure, cam);
}

//----------------------------------------------------------------------------
double vtkMicronTracker::GetLatestFrameTime(int cam)
{
  return MT->mtGetLatestFrameTime(cam);
}

//-----------------------------------------------------------------------------
char* vtkMicronTracker::GetTemplateName(int markerIdx)
{  
  return MT->mtGetTemplateName(markerIdx);
}

//-----------------------------------------------------------------------------
char* vtkMicronTracker::GetIdentifiedTemplateName(int markerIdx)
{
  return (char*)(MT->mtGetIdentifiedTemplateName(markerIdx));
}

//-----------------------------------------------------------------------------
void vtkMicronTracker::SetTemplateName(int markerIdx, char* templateName)
{
  int callResult =  MT->mtSetTemplateName(markerIdx, templateName);
  if (callResult != 0)
    {
      std::cout << MT->mtGetErrorString() << std::endl;
    }
  this->RefreshMarkerTemplates();
}

//
//-----------------------------------------------------------------------------
void vtkMicronTracker::DeleteTemplate(int markerIdx)
{
  int callResult = MT->mtDeleteTemplate(markerIdx);
  if (callResult != 0 )
    {
      std::cout << MT->mtGetErrorString() << std::endl;
    }
  this->RefreshMarkerTemplates();
}

//-----------------------------------------------------------------------------
vtkLongArray* vtkMicronTracker::GetLatestFramePixHistogram(int cam, int ssr)
{
  long* paPixHist;
  MT->mtGetLatestFramePixHistogram(paPixHist, ssr, cam);
  
  vtkLongArray* pTemp = vtkLongArray::New();
  pTemp->SetArray(paPixHist, 256, 1);
  
  vtkLongArray* pPixHist = vtkLongArray::New();
  pPixHist->DeepCopy(pTemp);
  return pPixHist;
}

//----------------------------------------------------------------------------
int vtkMicronTracker::GetNumOfFramesGrabbed(int cam)
{
  return MT->mtGetNumOfFramesGrabbed(cam);
}

//----------------------------------------------------------------------------
int vtkMicronTracker::GetBitsPerPixel(int cam)
{
  return MT->mtGetBitsPerPixel(cam);
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
