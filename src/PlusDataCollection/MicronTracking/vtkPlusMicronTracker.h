/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusMicronTracker_h
#define __vtkPlusMicronTracker_h

#include "vtkPlusDataCollectionExport.h"

#include "vtkPlusDevice.h"

class MicronTrackerInterface;
class vtkMatrix4x4;

/*!
  \class vtkPlusMicronTracker
  \brief Interface class to Claron MicronTracker optical trackers
  \ingroup PlusLibDataCollection
*/
class vtkPlusDataCollectionExport vtkPlusMicronTracker : public vtkPlusDevice
{
public:
  static vtkPlusMicronTracker* New();
  vtkTypeMacro(vtkPlusMicronTracker, vtkPlusDevice);

  /*! Hardware device SDK version. */
  virtual std::string GetSdkVersion();

  virtual bool IsTracker() const { return true; }

  /*! Probe to see if the tracking system is present. */
  PlusStatus Probe();
  PlusStatus InternalUpdate();

  /*! Get image from the camera into VTK images. If an input arguments is NULL then that image is not retrieved. */
  PlusStatus GetImage(vtkImageData* leftImage, vtkImageData* rightImage);

  /*! Get the status of the MicronTracker (Tracking or not) */
  vtkGetMacro(IsMicronTrackingInitialized, int);

  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* config);
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* rootConfigElement);

  MicronTrackerInterface* GetMicronTrackerInterface() { return this->MicronTracker; };

  static void LogMessageCallback(int level, const char* message, void* userdata);

  vtkSetMacro(TemplateDirectory, std::string);
  vtkGetMacro(TemplateDirectory, std::string);

  vtkSetMacro(IniFile, std::string);
  vtkGetMacro(IniFile, std::string);

protected:
  vtkPlusMicronTracker();
  ~vtkPlusMicronTracker();

  PlusStatus InternalStartRecording();

  virtual PlusStatus InternalConnect();
  virtual PlusStatus InternalDisconnect();
  virtual PlusStatus NotifyConfigured();

  /*! Refresh the loaded markers by loading them from the Markers directory */
  PlusStatus RefreshMarkerTemplates();

  /*! Returns the transformation matrix of the index_th marker */
  void GetTransformMatrix(int markerIndex, vtkMatrix4x4* transformMatrix);

protected:
  MicronTrackerInterface*   MicronTracker;
  bool                      IsMicronTrackingInitialized;
  std::string               TemplateDirectory;
  std::string               IniFile;

#ifdef USE_MicronTracker_TIMESTAMPS
  double                    TrackerTimeToSystemTimeSec; // time_System = time_Tracker + TrackerTimeToSystemTimeSec
  bool                      TrackerTimeToSystemTimeComputed; // the time offset is always computed when the first frame is received after start tracking
#endif

  vtkSmartPointer<vtkImageData> FrameLeft;
  vtkSmartPointer<vtkImageData> FrameRight;
  FrameSizeType                 FrameSize;

private:
  vtkPlusMicronTracker(const vtkPlusMicronTracker&);
  void operator=(const vtkPlusMicronTracker&);
};

#endif
