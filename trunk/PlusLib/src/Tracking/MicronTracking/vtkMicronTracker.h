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

#ifndef __vtkMicronTracker_h
#define __vtkMicronTracker_h

#include <limits.h>
#include <float.h>
#include <math.h>
#include <ctype.h>
#include <iomanip>

#include "vtkUnsignedCharArray.h"
#include "vtkImageFlip.h"

#include "vtkMath.h"
#include "vtkTimerLog.h"
#include "vtkMatrix4x4.h"
#include "vtkTransform.h"
#include "vtkMicronTracker.h"
#include "vtkTrackerTool.h"
#include "vtkObjectFactory.h"
#include "vtkImageImport.h"
#include "vtkTracker.h"
#include "vtkIntArray.h"
#include "vtkLongArray.h"
#include "vtkDoubleArray.h"
#include "time.h"

#define MAX_TOOL_NUM 10

class MicronTrackerInterface;

#define CAM_FRAME_WIDTH 1024
#define CAM_FRAME_HEIGHT 768

/*!
  \class vtkMicronTracker
  \brief Interface class to Claron MicronTracker optical trackers
  \ingroup PlusLibTracking
*/
class VTK_EXPORT vtkMicronTracker : public vtkTracker
{
public:

  /*!
    \struct CameraInfo
    \brief Description of a MicronTracker camera
    \ingroup PlusLibTracking
  */
  struct CameraInfo
  {
    int serialNum;
    int xResolution;
    int yResolution;
    int numOfSensors;
  };

  static vtkMicronTracker *New();
  vtkTypeMacro(vtkMicronTracker,vtkTracker);

  /*! Hardware device SDK version. */
  virtual std::string GetSdkVersion(); 
 
  /*!
    Probe to see if the tracking system is present.
  */
  PlusStatus Probe();

  /*!
    Get an update from the tracking system and push the new transforms
    to the tools.  This should only be used within vtkTracker.cxx.
  */
  PlusStatus InternalUpdate();

  /*! Save the changes in the INI (persistence) file */
  void SaveSettingsToINI();

  /*! Get the number of attached cameras. */
  int GetNumOfCameras();

  /*!
    Get the camera(s) serial number and x and y resolution. 
    Each of these properties are stored in an array. The first element of the array
    belongs to the first camera, the second to the second cameras and so on.
  */
  const CameraInfo& GetCameraInfo(int cameraIndex) { return this->CameraInfoList[cameraIndex]; };

  /*! Selects the camera with the index number of n */
  void SelectCamera(int n);

  /*! Returns the index of the current camera */
  int GetCurrCamIndex();
 
  /*!
    Set and get the templates matching tolerance. This is the maximum allowed distance (in mm)
    between the ends of vectors in each facet template and its matched measure vectors during identification.
  */
  void SetTemplatesMatchingTolerance(double mTolerance);

  /* Get the templatesMatchingTolerance in mm */
  double GetTemplatesMatchingTolerance();

  /*! Get the default templatesMatchingTolerance in mm */
  double GetTemplatesMatchingToleranceDefault();

  /*! Get general information about the identified markers */
  int GetNumOfIdentifiedMarkers();
  /*! Get general information about the identified markers */
  int GetNumberOfFacetsInMarker(int markerIdx);
  /*! Get general information about the identified markers */
  int GetNumberOfTotalFacetsInMarker(int markerIdx);

  /*! Get general information about the unidentified markers */
  int GetNumOfUnidentifiedMarkers();

  /*!
    Set and get the predictive frames interleave. This is the numbe of predictive-only frames 
    between each pair of comprehensive frames.
  */
  void SetPredictiveFrameInterleave(int predInterleave);

  /*! Get the predictive frame interleave */
  int GetPredictiveFrameInterleave();

  /*! 
    Set and get some properties of the loaded markers in the markers folder
    \param predictiveTracking 1 (enabled) or 0 (disabled)
  */
  void SetPredictiveTracking(short predictiveTracking);

  /*! Get the predictive tracking of the markers */
  short GetPredictiveTracking();

  /*!
    Set the AutoAdjustCameraExposure. If set to true (-1) the camera settings will be updated
    after every call to process frame. If set to false(0) this does not happen.
  */
  void SetAdjustCamAfterEveryProcess(short autoCamExposure);

  /*! Get the AutoAdjustCameraExposure. See the description of SetAutoAdjustCamExposure above */
  short GetAdjustCamAfterEveryProcess();

  /*! Set the name of the marker markerIdx in the list */
  void SetTemplateName(int markerIdx, char* templateName);
  /*! Get the name of the marker markerIdx in the list */
  char* GetTemplateName(int markerIdx);
  /*! Get the name of the marker markerIdx in the list of identified markers */
  char* GetIdentifiedTemplateName(int markerIdx);
  /*! Deletes the marker with the index markerIdx from the list of the loaded templates */
  void DeleteTemplate(int markerIdx);
  /*! Saves the marker with the name markerName. Returns 0 if successful, -1 if not */
  int SaveTemplate(char* markerName);
  /*! Refresh the loaded markers. If successful returns 0, if not -1 */
  PlusStatus RefreshMarkerTemplates();
  /*! Stops the process of collecting frames for the new template */
  int StopSampling(char* name, double jitter);  
  /*! Get the number of loaded markers */
  int GetNumOfLoadedMarkers();

  /*! Get the preferred setting for shutter period. This is relative (no units). */
  int GetShutterPref();
  /*! Set the preferred shutter setting */
  void SetShutterPref(int shutterPref);
  /*! 
    Set and get the shutter opening time. When setting 
    the value of it, the AutoExposure will be set to false automatically.
  */
  double GetShutterTime(int cam);
  /*! Set the shutter opening time */
  void SetShutterTime(double shutterTime, int cam);
  /*! Get the minimum shutter opening time */
  double GetMinShutterTime(int cam);
  /*! Get maximum the shutter opening time */
  double GetMaxShutterTime(int cam);
 
  /*! Set and get the gain of the camera */
  double GetGain(int cam);
  /*! Set the gain of the camera */
  void SetGain(double gain, int cam);
  /*! Get the minimum gain of the camera */
  double GetMinGain(int cam);
  /*! Get the maximum gain of the camera */
  double GetMaxGain(int cam);
  /*! Get the DB gain of the camera */
  double GetDBGain(int cam);

  /*! Get the exposure of the camera */
  double GetExposure(int cam);
  /*! Set the exposure of the camera */
  void SetExposure(double exposure, int cam);
  /*! Get the minimum exposure of the camera */
  double GetMinExposure(int cam);
  /*! Get the maximum exposure of the camera */
  double GetMaxExposure(int cam);

  /*! Get the colour temperature setting of the camera */
  double GetLightCoolness(int cam = -1);

  /*!
    Set the AutoExposure property of the camera. When set to true the exposure is
    automatically adjusted to maintain a good distribution of grey levels in the image.
    \param autoExposure -1 is true, 0 if false.
    \param cam index of the current camera
  */
  void SetCamAutoExposure(int autoExposure, int cam);
  
  /*!
    Set the AutoExposure property of the camera
    \return -1 is true, 0 if false.
  */
  int GetCamAutoExposure(int cam);

  /*! Get the latest frame time relative to the start time of the MicronTracker */
  double GetLatestFrameTime(int cam);

  /*! Get the BitsPerPixel value of the frames */
  int GetBitsPerPixel(int cam);

  /*!
    Gets the pixel value histogram of the latest frame. ssr is the subSamplingRate. 
    If ssr set to values > 1 the speed of computation increases.
  */
  vtkLongArray* GetLatestFramePixHistogram(int cam, int ssr);

  /*! Returns the number of frame grabbed by the camera. Increments by one each time a frame is grabbed. */
  int GetNumOfFramesGrabbed(int cam);

  /*!
    Query current camera's readiness; if more detail is desired this is available
    in the value returned by mtGetLatestFrameHazard()
  */
  int GetLatestFrameHazard();

  /*!
    Set the flag that indicates whether new samples are to be collected 
    for new templates.
  */
  vtkSetMacro(IsCollectingNewSamples, int);

  /*!
    Set the flag that indicates an additional facet is being added to 
    an existing marker.
  */
  vtkSetMacro(IsAdditionalFacetAdding, int);

  /*! Resets the counter of the frames collected for the new marker to 0 */
  void ResetNewSampleFramesCollected();
  /*! Get the number of the frames collected for the new marker */
  vtkGetMacro(NewSampleFramesCollected, int);

  /*!
    Get the image arrays or constructed image.
    Note: The client should call the UpdateLeftRightImage() first and subsequently
    retrieve the information on the image pixel array via GetLeftImage() and GetRightImage.
    These two functions returns the image data in the form of a vtkImageImport which can
    be manipulated or displayed in the desired form by the client.
  */
  vtkImageImport* GetLeftImage();
  /*!
    Get the image arrays or constructed image.
    Note: The client should call the UpdateLeftRightImage() first and subsequently
    retrieve the information on the image pixel array via GetLeftImage() and GetRightImage.
    These two functions returns the image data in the form of a vtkImageImport which can
    be manipulated or displayed in the desired form by the client.
  */
  vtkImageImport* GetRightImage();
  /*!
    Prepare the image arrays or constructed image for getting them.
    Note: The client should call the UpdateLeftRightImage() first and subsequently
    retrieve the information on the image pixel array via GetLeftImage() and GetRightImage.
    These two functions returns the image data in the form of a vtkImageImport which can
    be manipulated or displayed in the desired form by the client.
  */
  void UpdateLeftRightImage();
  /*! Saves the current camera images into JPG files */
  void GetSnapShot(const std::string &leftJpgFilePath, const std::string &rightJpgFilePath);

  /*! Get identified markers xpoints */
  vtkDoubleArray* vtkGetIdentifiedMarkersXPoints(int markerIdx);

  /*! Get unidentified markers xpoints */
  vtkDoubleArray* vtkGetUnidentifiedMarkersEnds(int vectorIdx);

  /*!
    Get number of loaded tools 
    The name and class name of the tools to be loaded is read from a text file (ToolNames.txt).
    The number of laoded tools is kept in the numOfLoadedTools variable. The name of the individual tools and 
    their corresponding class name is stored in containers and they can be accessed via the accessor methods below.
  */
  vtkGetMacro(numOfLoadedTools, int);

  /*! Get a tool name corresponding to a tool index */
  char* GetToolName(int toolIndex);

  /*! Get a tool class name corresponding to a tool index */
  char* GetToolClassName(int toolIndex);
  
  /*!
    Sets the toolIndex_th member of the markerIndexAssignedToTool array to markerIndex.
    In fact this array holds the index of the loaded markers that are assigned to each tool.
    If this index is 99 it means that the specified tool is not assigned to any specific marker and the 
    marker will be assigned to tools on the "first come, first served" basis, i.e. the first detected marker
    will be assigend to the first loaded tool, the second marker to the second tool and so on..
  */
  void SetMarkerIndexAssignedToTool(int toolIndex, int markerIndex);

  /*! Get the status of the MicronTracker (Tracking or not) */
  vtkGetMacro(IsMicronTrackingInitialized, int);
 
  /*! Returns the transformation matrix of the index_th marker */
  void GetTransformMatrix(int markerIndex, vtkMatrix4x4* transformMatrix);
  /*! Returns the reference transformation matrix of the index_th marker. Deprecated. */
  void GetReferenceTransformMatrix(int markerIndex, vtkMatrix4x4* ReferencetransformMatrix);

  /*! Pointer to the MicronTrackerInterface class instance */
  MicronTrackerInterface* MT;

  /*! Read MicronTracker configuration and update the tracker settings accordingly */
  PlusStatus ReadConfiguration( vtkXMLDataElement* config );

  /*! Write current MicronTracker configuration settings to XML */
  PlusStatus WriteConfiguration(vtkXMLDataElement* rootConfigElement);

  /*! Connect to the tracker hardware */
  PlusStatus Connect();
  /*! Disconnect from the tracker hardware */
  PlusStatus Disconnect();

protected:
  vtkMicronTracker();
  ~vtkMicronTracker();

  /*!
    Start the tracking system.  The tracking system is brought from
    its ground state into full tracking mode.  The POLARIS will
    only be reset if communication cannot be established without
    a reset.
  */
  PlusStatus InternalStartTracking();

  /*! Stop the tracking system and bring it back to its initial state. */
  PlusStatus InternalStopTracking();

  /*! Non-zero if the tracker has been initialized */
  int IsMicronTrackingInitialized;

  /*! Index of the last frame number. This is used for providing a frame number when the tracker doesn't return any transform */
  double LastFrameNumber;

  // Camera information
  int CurrCamIndex;  // index of the current camera, if -1 then all cameras selected
  std::vector<CameraInfo> CameraInfoList;

  // Marker information
  int NumOfIdentifiedMarkers;
  int NumOfUnidentifiedMarkers;
  int IsCollectingNewSamples;
  int IsAdditionalFacetAdding;
  int NewSampleFramesCollected;
  
  // Camera image data 
  vtkImageImport* LeftImage;
  vtkImageImport* RightImage;
  unsigned char** LeftImageArray;
  unsigned char** RightImageArray;

  vtkDoubleArray* Xpoints;
  vtkDoubleArray* VectorEnds;

  int numOfLoadedTools;

  std::string ToolNames[MAX_TOOL_NUM];
  std::string ToolFileLines[12];
  std::string ToolClassNames[MAX_TOOL_NUM];
  int MarkerIndexAssingedToTools[MAX_TOOL_NUM];

  unsigned int FrameNumber;
  std::string TemplateDirectory;
  std::string IniFile;

private:
  vtkMicronTracker(const vtkMicronTracker&);
  void operator=(const vtkMicronTracker&);  
  void PrintMatrix(FILE *file, float a[4][4]);
  void PrintMatrix(float a[4][4]);
  void PrintMatrix(vtkMatrix4x4*);
};

#endif
