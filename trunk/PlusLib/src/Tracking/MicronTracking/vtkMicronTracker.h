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


// .NAME vtkPOLARISTracker - VTK interface for Northern Digital's POLARIS
// .SECTION Description
// The vtkPOLARISTracker class provides an  interface to the POLARIS
// (Northern Digital Inc., Waterloo, Canada) optical tracking system.
// It also works with the AURORA magnetic tracking system, using the
// POLARIS API.
// .SECTION Caveats
// This class refers to ports 1,2,3,A,B,C as ports 0,1,2,3,4,5
// .SECTION see also
// vtkTrackerTool vtkFlockTracker


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
#include "vtkCriticalSection.h"
#include "vtkMicronTracker.h"
#include "vtkTrackerTool.h"
#include "vtkFrameToTimeConverter.h"
#include "vtkObjectFactory.h"
#include "vtkImageImport.h"
#include "vtkTracker.h"
#include "vtkIntArray.h"
#include "vtkLongArray.h"
#include "vtkDoubleArray.h"
#include "MicronTrackerInterface.h"
//#include "tracking.h"
#include "time.h"

#define MAX_TOOL_NUM 10

class vtkFrameToTimeConverter;
class vtkTrackerBuffer;

// the number of tools the polaris can handle
//#define VTK_POLARIS_NTOOLS 12
//#define VTK_POLARIS_REPLY_LEN 512

#define CAM_FRAME_WIDTH 1024
#define CAM_FRAME_HEIGHT 768

class VTK_EXPORT vtkMicronTracker : public vtkTracker
{
public:

  static vtkMicronTracker *New();
  vtkTypeMacro(vtkMicronTracker,vtkTracker);
 
  // Description:
  // Probe to see if the tracking system is present on the
  // specified serial port.  If the SerialPort is set to -1,
  // then all serial ports will be checked.
  int Probe();

  // Description:
  // Get an update from the tracking system and push the new transforms
  // to the tools.  This should only be used within vtkTracker.cxx.
  void InternalUpdate();

  // Description:
  // Send a command to the POLARIS in the format INIT: or VER:0 (the
  // command should include a colon).  Commands can only be done after
  // either Probe() or StartTracking() has been called.
  // The text reply from the POLARIS is returned, without the CRC or
  // final carriage return.
  //  char *Command(const char *command);

  // Description:
  // Get the a string (perhaps a long one) describing the type and version
  // of the device.
//  vtkGetStringMacro(Version);

  // Description:
  // Set which serial port to use, 1 through 4.  Default: 1.
  // user calls SetSerialPort(int portNumber)
  // Python: lap.SetSerialPort(int)
  // C++: void SetSerialPort (int );
  vtkSetMacro(SerialPort, int); // you call SetSerialPort(int PortNumber)
 
  // Description:
  // Get the set serial port.
  // Python: lap.GetSerialPort() -> int
  // C++: int GetSerialPort ();
  vtkGetMacro(SerialPort, int); // you call GetSerialPort()

  // Description:
  // Set the desired baud rate.  Default: 9600.
  // user calls SetBaudRate(int baudRate)
  // 9600; 14400; 19200; 38400;  57600; 115200;
  // The baud parameter is the desired baud rate in bits per
  // second. The actual baud rate used will be the
  // closest one available on the host system.
  vtkSetMacro(BaudRate, int); // you call SetBaudRate(int bps)

  // Description:
  // Get the set baud rate.
  vtkGetMacro(BaudRate, int); // you call GetBaudRate()

  /*********************************/
  /*
  /* Save the changes in the INI (persistence) file.
  /*
  /*********************************/
  // Description:
  // Save the changes in the INI file
  void UpdateINI();

  /*********************************/
  /*
  /* Get the camera info (number of cameras, serial number, resolution).
  /*
  /*********************************/
  // Description:
  // Set / Get the number of attached cameras.
  vtkSetMacro(numOfCameras, int);
  vtkGetMacro(numOfCameras, int);

  // Description:
  // Set / Get the camera(s) serial number and x and y resolution. 
  // Each of these properties are stored in an array. The first element of the array
  // belongs to the first camera, the second to the second cameras and so on.
  vtkGetObjectMacro(serialNums, vtkIntArray);
  vtkGetObjectMacro(xResolutions, vtkIntArray);
  vtkGetObjectMacro(yResolutions, vtkIntArray);
  vtkGetObjectMacro(sensorNums, vtkIntArray);


  /*********************************/
  /*
  /* Select a camera; Get the index of the current camera
  /*
  /*********************************/
  // Description:
  // Selects the camera with the index number of n.
  void SelectCamera(int n);

  // Description:
  // Returns the index of the current camera
  int GetCurrCamIndex();
 
  /*********************************/
  /*
  /* Set and get the templates matching tolerance. This is the maximum allowed distance (in mm)
  /* between the ends of vectors in each facet template and its matched measure vectors during identification.
  /*
  /*********************************/
  // Description:
  // Set the templatesMatchingTolerance to the passed argument. 
  void SetTemplatesMatchingTolerance(double mTolerance);

  // Description:
  // Get the templatesMatchingTolerance in mm.
  double GetTemplatesMatchingTolerance();

  // Description:
  // Get the default templatesMatchingTolerance in mm.
  double GetTemplatesMatchingToleranceDefault();

  /*********************************/
  /*
  /* Get the general information about the identified markers
  /*
  /*********************************/
  vtkGetMacro(numOfIdentifiedMarkers, int);

  int GetNumberOfFacetsInMarker(int markerIdx);
  int GetNumberOfTotalFacetsInMarker(int markerIdx);

  /*********************************/
  /*
  /* Get the general information about the unidentified markers
  /*
  /*********************************/
  vtkGetMacro(numOfUnidentifiedMarkers, int);

  /*********************************/
  /*
  /* Set and get the predictive frames interleave. This is the numbe of predictive-only frames 
  /* between each pair of comprehensive frames.
  /*
  /*********************************/
  // Description:
  // Set the predictive frame interleave.
  void SetPredictiveFrameInterleave(int predInterleave);

  // Description:
  // Get the predictive frame interleave.
  int GetPredictiveFrameInterleave();

  /*********************************/
  /*
  /* Set and get some properties of the loaded markers in the markers folder.
  /*
  /*********************************/
  // Description:
  // Set the predictive tracking of the markers to 1(true) or 0(false).
  void SetPredictiveTracking(short predictiveTracking);

  // Description:
  // Get the predictive tracking of the markers.
  short GetPredictiveTracking();

  // Description:
  // Set the AutoAdjustCameraExposure. If set to true (-1) the camera settings will be updated
  // after every call to process frame. If set to false(0) this does not happen.
  void SetAdjustCamAfterEveryProcess(short autoCamExposure);

  // Description:
  // Get the AutoAdjustCameraExposure. See the description of SetAutoAdjustCamExposure above.
  short GetAdjustCamAfterEveryProcess();

  // Description:
  // Set the name of the marker markerIdx in the list.
  void SetTemplateName(int markerIdx, char* templateName);

  // Description:
  // Get the name of the marker markerIdx in the list.
  char* GetTemplateName(int markerIdx);

  // Description:
  // Get the name of the marker markerIdx in the list of identified markers
  char* GetIdentifiedTemplateName(int markerIdx);
 
  // Description:
  // Stops the process of collecting frames for the new template.
  int StopSampling(char* name, double jitter);

  // Description:
  // Deletes the marker with the index markerIdx from the list of the loaded templates.
  void DeleteTemplate(int markerIdx);

  // Description:
  // Saves the marker with the name markerName. Returns 0 if successful, -1 if not.
  int SaveTemplate(char* markerName);
  
  // Description:
  // Refresh the loaded markers. If successful returns 0, if not -1.
  void RefreshMarkerTemplates();
  
  // Description:
  // Get the number of loaded markers.
  int GetNumOfLoadedMarkers();

  /*********************************/
  /*
  /* Set and get the preferred setting for shutter period. This is relative (no units)
  /*
  /*********************************/
  // Description:
  // Get the shutter setting.
  int GetShutterPref();

  // Description:
  // Set the shutter setting.
  void SetShutterPref(int shutterPref);


  /*********************************/
  /*
  /* Set and get the shutter opening time. When setting 
  /* the value of it, the AutoExposure will be set to false automatically.
  /*
  /*********************************/
  // Description:
  // Get the shutter opening time.
  double GetShutterTime(int cam);

  // Description:
  // Set the shutter opening time.
  void SetShutterTime(double shutterTime, int cam);
  
  // Description:
  // Get the shutter opening time.
  double GetMinShutterTime(int cam);

  // Description:
  // Get the shutter opening time.
  double GetMaxShutterTime(int cam);
 
  /*********************************/
  /*
  /* Set and get the Gain and Exposure of the camera.
  /*
  /*********************************/
  // Description:
  // Get the gain of the camera.
  double GetGain(int cam);

  // Description:
  // Set the gain of the camera.
  void SetGain(double gain, int cam);

  // Description:
  // Get the minimum gain of the camera.
  double GetMinGain(int cam);

  // Description:
  // Get the maximum gain of the camera.
  double GetMaxGain(int cam);

  // Description:
  // Get the DB gain of the camera.
  double GetDBGain(int cam);

  // Description:
  // Get the exposure of the camera.
  double GetExposure(int cam);

  // Description:
  // Set the exposure of the camera.
  void SetExposure(double exposure, int cam);

  // Description:
  // Get the minimum exposure of the camera.
  double GetMinExposure(int cam);

  // Description:
  // Get the maximum exposure of the camera.
  double GetMaxExposure(int cam);

  // Description:
  // Get the colour temperature setting of the camera.
  double GetLightCoolness(int cam = -1) {return MT->mtGetLightCoolness(cam);}

  /*********************************/
  /*
  /* Set and get the AutoExposure property of the camera. When set to true the exposure is
  /* automatically adjusted to maintain a good distribution of grey levels in the image.
  /*
  /*********************************/
  // Description:
  // Get the AutoExposure property of the camera. -1 is true, 0 if false.
  int GetCamAutoExposure(int cam);

  // Description:
  // Set the AutoExposure property of the camera
  void SetCamAutoExposure(int autoExposure, int cam);

  /*********************************/
  /*
  /* Set and get the latest frame (or general frame) info.
  /*
  /*********************************/
  // Description:
  // Get the latest frame time relative to the start time of the MicronTracker.
  double GetLatestFrameTime(int cam);

  // Description:
  // Get the BitsPerPixel value of the frames.
  int GetBitsPerPixel(int cam);

  // Description:
  // Gets the pixel value histogram of the latest frame. ssr is the subSamplingRate. 
  // If ssr set to values > 1 the speed of computation increases.
  vtkLongArray* GetLatestFramePixHistogram(int cam, int ssr);

  // Description:
  // Returns the number of frame grabbed by the camera. Increments by one each
  // time a frame is grabbed.
  int GetNumOfFramesGrabbed(int cam);

  // Description:
  // Query current camera's readiness; if more detail is desired this is available
  // in the value returned by mtGetLatestFrameHazard()
  int GetLatestFrameHazard() {return MT->mtGetLatestFrameHazard();}

  // Description:
  // Set the flag that indicates whether new samples are to be collected 
  // for new templates.
  vtkSetMacro(isCollectingNewSamples, int);

  // Description:
  // Set the flag that indiates an additional facet is being added to 
  // an existing marker.
  vtkSetMacro(isAdditionalFacetAdding, int);

  // Description:
  // Resets the counter of the frames collected for the new marker to 0.
  void ResetNewSampleFramesCollected();
  vtkGetMacro(newSampleFramesCollected, int);

  /*********************************/
  /*
  /* Calibration methods.
  /*
  /*********************************/
  // Description: Returns the frameLimit which is the number of frames to be grabed 
  // for finding the rotation matrix of the calibration matrix.
//  vtkGetObjectMacro(finalCalibrationMatrix, vtkMatrix4x4);

  /*********************************/
  /*
  /* Get the image arrays or constructed image.
  /* Note: The client should call the UpdateLeftRightImage() first and subsequently
  /*       retrieve the information on the image pixel array via GetLeftImage() and GetRightImage.
  /*       These two functions returns the image data in the form of a vtkImageImport which can
  /*       be manipulated or displayed in the desired form by the client.
  /*
  /*********************************/
  vtkImageImport* GetLeftImage();
  vtkImageImport* GetRightImage();
  void UpdateLeftRightImage();
  void GetSnapShot(char* testNum, char* identifier);

  /*********************************/
  /*
  /* Get identified markers xpoints
  /*
  /*********************************/
  vtkDoubleArray* vtkGetIdentifiedMarkersXPoints(int markerIdx);

  /*********************************/
  /*
  /* Get unidentified markers xpoints
  /*
  /*********************************/
  vtkDoubleArray* vtkGetUnidentifiedMarkersEnds(int vectorIdx);


  /*********************************/
  /*
  /* Get number of loaded tools 
  /*
  /*********************************/
  // Description: The name and class name of the tools to be loaded is read from a text file (ToolNames.txt).
  // The number of laoded tools is kept in the numOfLoadedTools variable. The name of the individual tools and 
  // their corresponding class name is stored in containers and they can be accessed via the accessor methods below.
  int numOfLoadedTools;
  vtkGetMacro(numOfLoadedTools, int);
  char* GetToolName(int toolIndex);
  char* GetToolClassName(int toolIndex);
  // Description: Sets the toolIndex_th member of the markerIndexAssignedToTool array to markerIndex.
  // In fact this array holds the index of the loaded markers that are assigned to each tool.
  // If this index is 99 it means that the specified tool is not assigned to any specific marker and the 
  // marker will be assigned to tools on the "first come, first served" basis, i.e. the first detected marker
  // will be assigend to the first loaded tool, the second marker to the second tool and so on..
  void SetMarkerIndexAssignedToTool(int toolIndex, int markerIndex);

  /*********************************/
  /*
  /* Get the status of the MicronTracker (Tracking or not)
  /*
  /*********************************/
  int IsMicronTracking;
  vtkGetMacro(IsMicronTracking, int);

  // Description:
  // Returns the transformation matrix of the index_th marker for the toolIndex_th tool. The default
  // value of the toolIndex is 0. For internal use only (? made public Aug 5).
  vtkMatrix4x4* GetTransformMatrix(int index, int toolIndex = 0);
  vtkMatrix4x4* previousTransformMatrix[MAX_TOOL_NUM];


  // An instance of the MicronTrackerInterface class.
  MicronTrackerInterface* MT;


protected:
  vtkMicronTracker();
  ~vtkMicronTracker();

  // Description:
  // Start the tracking system.  The tracking system is brought from
  // its ground state into full tracking mode.  The POLARIS will
  // only be reset if communication cannot be established without
  // a reset.
  int InternalStartTracking();

  // Description:
  // Stop the tracking system and bring it back to its ground state:
  // Initialized, not tracking, at 9600 Baud.
  int InternalStopTracking();

  // Description:
  // Class for updating the virtual clock that accurately times the
  // arrival of each transform, more accurately than is possible with
  // the system clock alone because the virtual clock averages out the
  // jitter.
  vtkFrameToTimeConverter *Timer;

  char *Version;
  int SerialPort; 
  vtkMatrix4x4 *SendMatrix;
  int BaudRate;

  //SI
  int pos[3];
  int numOfCameras;
  int currCamIndex;  // index of the current camera, if -1 then all cameras selected
  int numOfIdentifiedMarkers;
  int isCollectingNewSamples;
  int isAdditionalFacetAdding;
  int newSampleFramesCollected;
  int numOfUnidentifiedMarkers;
 
  // Three vtkIntArrays to keep the serial number, xResolution and yResulotions of the cameras.
  vtkIntArray* serialNums;
  vtkIntArray* xResolutions;
  vtkIntArray* yResolutions;
  vtkIntArray* sensorNums;

  // Camera image data 
  vtkImageImport* leftImage;
  vtkImageImport* rightImage;
  unsigned char** leftImageArray;
  unsigned char** rightImageArray;

  vtkDoubleArray* xpoints;
  vtkDoubleArray* vectorEnds;

  string toolNames[MAX_TOOL_NUM];
  string toolFileLines[12];
  string toolClassNames[MAX_TOOL_NUM];
  int markerIndexAssingedToTools[MAX_TOOL_NUM];
  void ReadToolsFile();

private:
  vtkMicronTracker(const vtkMicronTracker&);
  void operator=(const vtkMicronTracker&);  
  long statusFlags[MAX_TOOL_NUM];
  void print_matrix(FILE *file, float a[4][4]);
  void print_matrix(float a[4][4]);
  void print_matrix(vtkMatrix4x4*);
  float matrices [50][4][4];
//  vtkMatrix4x4* finalCalibrationMatrix;
  vtkMatrix4x4 *rm[MAX_TOOL_NUM];

};

#endif
