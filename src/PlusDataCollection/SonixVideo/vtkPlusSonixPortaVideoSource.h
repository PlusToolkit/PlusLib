/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

/*=========================================================================
The following copyright notice is applicable to parts of this file:
Copyright (c) Elvis Chen, 2008
All rights reserved.
See Copyright.txt or http://www.kitware.com/Copyright.htm for details.
Authors include: Elvis Chen (Queen's University)
=========================================================================*/

#ifndef __vtkPlusSONIXPORTAVIDEOSOURCE_H__
#define __vtkPlusSONIXPORTAVIDEOSOURCE_H__

#include "PlusConfigure.h"
#include "vtkPlusDataCollectionExport.h"

#include "vtkPlusUsDevice.h"

class vtkPlusDataCollectionExport vtkPlusSonixPortaVideoSource;
class porta;

#if (PLUS_ULTRASONIX_SDK_MAJOR_VERSION < 6)
  class probeInfo;
#else
  struct probeInfo;
#endif


/*!
\class vtkPlusSonixPortaVideoSourceCleanup
\brief Class that cleans up (deletes singleton instance of) vtkPlusSonixPortaVideoSource when destroyed
\ingroup PlusLibDataCollection
*/
class vtkPlusDataCollectionExport vtkPlusSonixPortaVideoSourceCleanup
{
public:
  vtkPlusSonixPortaVideoSourceCleanup();
  ~vtkPlusSonixPortaVideoSourceCleanup();

private:
  vtkPlusSonixPortaVideoSourceCleanup(const vtkPlusSonixPortaVideoSourceCleanup& other);
  vtkPlusSonixPortaVideoSourceCleanup& operator=(const vtkPlusSonixPortaVideoSourceCleanup& rhs);
};

/*!
  \class vtkPlusSonixPortaVideoSource
  \brief Class providing VTK video input interface for SonixRP

  vtkPlusSonixPortaVideoSource is a subclass of vtkVideoSource that
  implementes Porta API for importing video directly from the Cine
  buffer of a Sonix RP Ultrasound machine.

  You must call the ReleaseSystemResources() method before the application
  exits.  Otherwise the application might hang while trying to exit.

  the 3D/4D probe should always be connected to port-0

  In order to reduce the number of missing frames the motor speed can be
  decreased by changing the b frame rate id in the imaging.set.xml file of the
  Porta SDK.

  Usage:
  sonixGrabber = vtkPlusSonixPortaVideoSource::New();
  sonixGrabber->SetPortaSettingPath( "../../dat/" );
  sonixGrabber->SetPortaFirmwarePath( "../../fw/" );
  sonixGrabber->SetPortaLUTPath( "C:/LUTs/" );
  sonixGrabber->SetPortaCineSize( 256*1024*1024 );
  // not needed, defaults 256MB
  sonixGrabber->SetBLineDensity( 128 );
  // line density effect the frame rate
  sonixGrabber->Record();
  imageviewer->SetInputData( sonixGrabber->GetOutput() );

  \ingroup PlusLibDataCollection
*/
class vtkPlusDataCollectionExport vtkPlusSonixPortaVideoSource : public vtkPlusUsDevice
{
  vtkTypeMacro(vtkPlusSonixPortaVideoSource, vtkPlusDevice);
  void PrintSelf(ostream& os, vtkIndent indent);

  /*! This is a singleton pattern New.  There will only be ONE
   reference to a vtkOutputWindow object per process.  Clients that
   call this must call Delete on the object so that the reference
   counting will work.  The single instance will be unreferenced
   when the program exits. */
  static vtkPlusSonixPortaVideoSource* New();

  /*! return the singleton instance with no reference counting */
  static vtkPlusSonixPortaVideoSource* GetInstance();

  /*! Supply a user defined output window.  Call ->Delete() on the supplied instance after setting it. */
  static void SetInstance(vtkPlusSonixPortaVideoSource* instance);

  //BTX
  /*! use this as a way of memory management when the
   program exits the SmartPointer will be deleted which
   will delete the Instance singleton */
  static vtkPlusSonixPortaVideoSourceCleanup Cleanup;
  //ETX

  /*! Hardware device SDK version. */
  virtual std::string GetSdkVersion();
  /*! Read main configuration from xml data */  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* config);
  /*! Write main configuration to xml data */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* config);

  /* List of parameters which can be set or read in B-mode, other mode parameters not currently implemented*/

  /*! Get/Set the probe name */
  vtkSetStringMacro(PortaProbeName);
  vtkGetStringMacro(PortaProbeName);

  /*! Get/Set the probe name */
  vtkSetStringMacro(PortaSettingPath);
  vtkGetStringMacro(PortaSettingPath);

  /*! Get/Set the firmware path name */
  vtkSetStringMacro(PortaFirmwarePath);
  vtkGetStringMacro(PortaFirmwarePath);

  /*! Get/Set the look-up table path name */
  vtkSetStringMacro(PortaLUTPath);
  vtkGetStringMacro(PortaLUTPath);

  /*! Set USM initialization parameter */
  vtkSetMacro(Usm, int);
  /*! Get USM initialization parameter */
  vtkGetMacro(Usm, int);

  /*! Set PCI initialization parameter */
  vtkSetMacro(Pci, int);
  /*! Get PCI initialization parameter */
  vtkGetMacro(Pci, int);

  /*! Get/Set the look-up table path name */
  vtkSetStringMacro(PortaLicensePath);
  vtkGetStringMacro(PortaLicensePath);

  /*! Set ultrasound transmitter frequency (MHz) */
  PlusStatus SetFrequency(int aFrequency);
  /*! Get ultrasound transmitter frequency (MHz) */
  PlusStatus GetFrequency(int& aFrequency);

  /*! Set the depth (mm) of B-mode ultrasound */
  PlusStatus SetDepth(int aDepth);
  /*! Get the depth (mm) of B-mode ultrasound */
  PlusStatus GetDepth(int& aDepth);

  /*! Set the Gain (%) of B-mode ultrasound; valid range: 0-100 */
  PlusStatus SetGain(int aGain);
  /*! Get the Gain (%) of B-mode ultrasound; valid range: 0-100 */
  PlusStatus GetGain(int& aGain);

  /*! Set the Zoom (%) of B-mode ultrasound; valid range: 0-100 */
  PlusStatus SetZoom(int aZoom);
  /*! Get the Zoom (%) of B-mode ultrasound; valid range: 0-100 */
  PlusStatus GetZoom(int& aZoom);

  /*! Set the number of frames per volume */
  PlusStatus SetFramePerVolume(int aFramePerVolume);
  /*! Get the number of frames per volume */
  PlusStatus GetFramePerVolume(int& aFramePerVolume);

  /*! Set the number of frames per volume */
  PlusStatus SetStepPerFrame(int aStepPerFrame);
  /*! Get the number of frames per volume */
  PlusStatus GetStepPerFrame(int& aStepPerFrame);

  /*! Set field of view by adjusting the motor range (in degrees). If <=0 then the motor is turned off. */
  PlusStatus SetMotorRotationRangeDeg(double fovDeg);
  /*! Get field of view by adjusting the motor range (in degrees) */
  PlusStatus GetMotorRotationRangeDeg(double& fovDeg);

  vtkSetMacro(ImagingMode, int);
  vtkGetMacro(ImagingMode, int);

  vtkSetMacro(PortaBModeWidth, int);
  vtkGetMacro(PortaBModeWidth, int);

  vtkSetMacro(PortaBModeHeight, int);
  vtkGetMacro(PortaBModeHeight, int);

  /*! set the CineSize, defaults to 256MB, size in bytes */
  void SetPortaCineSize(int size);

  /*! Return current frame rate as reported by Porta */
  int GetPortaFrameRate();

  /*! Set clip rectangle origin and size according to the ROI provided by the ultrasound system */
  vtkSetMacro(AutoClipEnabled, bool);
  vtkGetMacro(AutoClipEnabled, bool);

  /*! Verify the device is correctly configured */
  virtual PlusStatus NotifyConfigured();

protected:
  vtkPlusSonixPortaVideoSource();
  ~vtkPlusSonixPortaVideoSource();

  /*! Connect to device */
  virtual PlusStatus InternalConnect();

  /*! Disconnect from device */
  virtual PlusStatus InternalDisconnect();

  /*! Record incoming video at the specified FrameRate.  The recording */
  // continues indefinitely until StopRecording() is called.
  virtual PlusStatus InternalStartRecording();

  /*! Stop recording or playing. */
  virtual PlusStatus InternalStopRecording();

  /*! Get the last error string returned by Porta */
  std::string GetLastPortaError();

  /*! Pointer to the hardware. Only required for SDK versions prior to 6.0 */
  porta* Porta;
  bool PortaConnected;

  /*! Imaging parameters */
  int Depth;
  int Frequency;
  int Gain;
  int Zoom;

  int ImagingMode;

  /*! Porta initialization parameters */
  int Usm;
  int Pci;

  /*!
    Porta can provide output on multiple virtual displays.
    This index identifies the one that this device uses.
  */
  int DisplayIndex;

  /*! Indicates that current depth, spacing, transducer origin has to be queried */
  bool ImageGeometryChanged;

  double CurrentDepthMm;
  double CurrentPixelSpacingMm[2];
  int CurrentTransducerOriginPixels[2];

  bool AutoClipEnabled;

  /*! for internal use only */
  PlusStatus AddFrameToBuffer(void* param, int id, bool motorRotationCcw, int motorStepCount);

  /*! For internal use only */
  PlusStatus SetParamValue(char* paramId, int paramValue, int& validatedParamValue);
  /*! For internal use only */
  PlusStatus GetParamValue(char* paramId, int& paramValue, int& validatedParamValue);

  virtual bool IsTracker() const { return false; }

private:
  // data members
  static vtkPlusSonixPortaVideoSource* Instance;

  /*! call back whenever a new frame is available */
#if (PLUS_ULTRASONIX_SDK_MAJOR_VERSION < 5) || (PLUS_ULTRASONIX_SDK_MAJOR_VERSION == 5 && PLUS_ULTRASONIX_SDK_MINOR_VERSION < 7)
  //  SDK version < 5.7.x
  static bool vtkPlusSonixPortaVideoSourceNewFrameCallback(void* param, int id);
#elif (PLUS_ULTRASONIX_SDK_MAJOR_VERSION < 6)
  //  5.7.x <= SDK version < 6.x
  static bool vtkPlusSonixPortaVideoSourceNewFrameCallback(void* param, int id, int header);
#else
  static int vtkPlusSonixPortaVideoSourceNewFrameCallback(void* param, int id, int header);
#endif

  // vtkPlusSonixPortaVideoSource(const vtkPlusSonixPortaVideoSource&);  // Not implemented.
  // void operator=(const vtkPlusSonixPortaVideoSource&);  // Not implemented.

  /*! Update porta parameters */
  PlusStatus vtkPlusSonixPortaVideoSource::UpdateSonixPortaParams();

  /*! Get probe head to transducer center transform */
  std::string GetMotorToMotorRotatedTransform(double MotorAngle);

  // Porta parameters
  /*! The B-mode image width */
  int PortaBModeWidth;

  /*! The B-mode image height */
  int PortaBModeHeight;

  /*! name of the probe */
  char* PortaProbeName;

  /*! Path to the Porta Setting directory */
  char* PortaSettingPath;

  /*! Path to the Porta Firmware directory */
  char* PortaFirmwarePath;

  /*! Path to the Porta LUT directory */
  char* PortaLUTPath;

  /*! Path to the licenses text file */
  char* PortaLicensePath;

  /*! Size of the Cine buffer */
  int PortaCineSize;

  /*! The number of frames that will acquired in one sweep of the motor. Controls the volume's field of view on SDK-5.x. */
  int FramePerVolume;

  /*! Controls the volume field of view on SDK-6.x. */
  double MotorRotationRangeDeg;

  /*!  The number of steps the motor will move while acuqiring a single frame. Controls the speed. */
  int StepPerFrame;

  /*! Probe information */
  probeInfo* ProbeInformation;

  /*! Motor rotation per step (in degrees). Computed from probe descriptor. */
  double MotorRotationPerStepDeg;

  /*! Motor rotation start angle (in degrees). Computed from probe descriptor and number of frames per volume. */
  double MotorRotationStartAngleDeg;

  /*! Set to false after first call to AddFramToBuffer */
  bool FirstCallToAddFrameToBuffer;

  /*! Index keeping track of which frame belongs to which volume */
  int VolumeIndex;

  /*! Index keeping track of number of frames in the current volume. */
  int FrameIndexInVolume;

  /*!  Last motor rotation direction. Stored to detect when the motor turns back and starts a new volume. */
  bool LastRotationCcw;

  unsigned char* ImageBuffer;
};

#endif // of __vtkPlusSONIXPORTAVIDEOSOURCE_H__
