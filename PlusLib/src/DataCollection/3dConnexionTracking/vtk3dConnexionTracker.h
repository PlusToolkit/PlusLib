/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtk3dConnexionTracker_h
#define __vtk3dConnexionTracker_h

#include "vtkPlusDevice.h"

class vtkPlusBuffer;
class vtkPlusDataSource;
class vtkMatrix4x4;

/*!
\class vtk3dConnexionTracker 
\brief Interface for 3D Connexion 3D mouse devices

This class reads transforms from 3D mouse devices.

\par Configuration settings
- \c Mode Operation mode of the device
  - \c MOUSE If the mouse is released then the transform does not change
  - \c JOYSTICK If the mouse is released then the transforms reverts to identity
- \c TranslationScales Sensitivity of the 3 translation axes. Default: <tt>"0.001 0.001 0.001"</tt>. Higher values mean higher sensitivity. Negative values can be used for inverting axes.
- \c RotationScales Sensitivity of the 3 rotation axes. Default: <tt>"0.001 0.001 0.001"</tt>. Higher values mean higher sensitivity. Negative values can be used for inverting axes.

\ingroup PlusLibDataCollection
*/
class VTK_EXPORT vtk3dConnexionTracker : public vtkPlusDevice
{
public:

  static vtk3dConnexionTracker *New();
  vtkTypeMacro( vtk3dConnexionTracker,vtkPlusDevice );

  /*! Connect to device */
  PlusStatus InternalConnect();

  /*! Disconnect from device */
  virtual PlusStatus InternalDisconnect();

  /*! 
  Probe to see if the tracking system is present on the specified serial port.  
  */
  PlusStatus Probe();

  /*!
  Get an update from the tracking system and push the new transforms
  to the tools.  This should only be used within vtkTracker.cxx.
  This method is called by the tracker thread.
  */
  PlusStatus InternalUpdate();

  /*! Read configuration from xml data */
  PlusStatus ReadConfiguration(vtkXMLDataElement* config); 

  /*! Write configuration to xml data */
  PlusStatus WriteConfiguration(vtkXMLDataElement* config);  

  /*! Process input event received from the device. For internal use only. Public to allow calling from static function. */
  void ProcessDeviceInputEvent( LPARAM lParam );

  /*! Callback function called when capture window is destroyed. For internal use only. Public to allow calling from static function. */
  void OnCaptureWindowDestroy(); 

  enum OperatingModeType
  {
    MOUSE_MODE,
    JOYSTICK_MODE
  };

  virtual bool IsTracker() const { return true; }
  
protected:

  vtk3dConnexionTracker();
  ~vtk3dConnexionTracker();

  /*! Start processing data received from the device */
  PlusStatus InternalStartRecording();

  /*! Stop processing data received from the device */
  PlusStatus InternalStopRecording();

  /*! Register callback to get notifications from the device */
  PlusStatus RegisterDevice();

  /*! Unregister callback function */
  void UnregisterDevice();

  /*! Create an invisible window that will be used to receive input messages from the device  */
  PlusStatus CreateCaptureWindow();

  /*! Delete the capture window */
  void DestroyCaptureWindow();  

private:  // Functions.

  vtk3dConnexionTracker( const vtk3dConnexionTracker& );
  void operator=( const vtk3dConnexionTracker& );  

private:  // Variables.

  vtkPlusDataSource* SpaceNavigatorTool;
  vtkMatrix4x4* LatestMouseTransform;
  vtkMatrix4x4* DeviceToTrackerTransform;
  double TranslationScales[3];
  double RotationScales[3];
  
  OperatingModeType OperatingMode;

  /*! Mutex instance simultaneous access of mouse pose transform (LatestMouseTransform) from the tracker and the main thread */ 
  vtkSmartPointer<vtkRecursiveCriticalSection> Mutex;

  std::string CaptureWindowClassName;
  HWND CaptureWindowHandle;

  /*! Pointer to the first element of an array of raw input devices */
  PRAWINPUTDEVICE     RegisteredRawInputDevices;
  /*! Number of raw input devices in the RawInputDevices array */
  int NumberOfRegisteredRawInputDevices;
  
};

#endif
