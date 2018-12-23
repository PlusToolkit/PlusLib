/*=Plus=header=begin======================================================
  Progra  : Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusSpinnakerVideoSource_h
#define __vtkPlusSpinnakerVideoSource_h

#include "vtkPlusDataCollectionExport.h"
#include "vtkPlusDevice.h"
#include <string>

/*!
  \class vtkPlusSpinnakerVideoSource
  \brief Interface class to Spinnaker API compatible Point Grey Cameras
  \ingroup PlusLibDataCollection
*/
class vtkPlusDataCollectionExport vtkPlusSpinnakerVideoSource : public vtkPlusDevice
{
public:

  static vtkPlusSpinnakerVideoSource *New();
  
  vtkTypeMacro(vtkPlusSpinnakerVideoSource, vtkPlusDevice);
  void PrintConfiguration(ostream& os, vtkIndent indent);
  virtual void PrintSelf(ostream& os, vtkIndent indent);

  /*! Read configuration from xml data */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* config);
  /*! Write configuration to xml data */
  PlusStatus WriteConfiguration(vtkXMLDataElement* config);

  /*!
  Record incoming data at the specified acquisition rate.  The recording
  continues indefinitely until StopRecording() is called.
  */
  PlusStatus InternalStartRecording();

  /*! Stop recording */
  PlusStatus InternalStopRecording();

  /*! Is this device a tracker */
  bool IsTracker() const { return false; }
  bool IsVirtual() const { return false; }
  
  /*! Get an update from the tracking system and push the new transforms to the tools. This function is called by the tracker thread.*/
  virtual PlusStatus InternalUpdate();

  virtual PlusStatus InternalConnect();
  virtual PlusStatus InternalDisconnect();

  /*! Verify the device is correctly configured */
  virtual PlusStatus NotifyConfigured();

  // enums for camera controls
  enum EXPOSURE_MODE
  {
    EXPOSURE_TIMED = 0,
    EXPOSURE_AUTO_ONCE,
    EXPOSURE_AUTO_CONTINUOUS
  };

  enum GAIN_MODE
  {
    GAIN_MANUAL = 0,
    GAIN_AUTO_ONCE,
    GAIN_AUTO_CONTINUOUS
  };

  enum WHITE_BALANCE_MODE
  {
    WB_MANUAL = 0,
    WB_AUTO_ONCE,
    WB_AUTO_CONTINUOUS
  };

  enum SHARPENING_MODE
  {
    SHARPENING_MANUAL = 0,
    SHARPENING_AUTO
  };

  // methods to set & get camera parameters
  vtkGetMacro(CameraNumber, unsigned int);
  vtkSetMacro(CameraNumber, unsigned int);
  vtkGetMacro(VideoFormat, std::string);
  vtkSetMacro(VideoFormat, std::string);
  PlusStatus SetFrameRate(int FrameRate);
  EXPOSURE_MODE GetExposureMode() { return this->ExposureMode; }
  PlusStatus SetExposureMode(EXPOSURE_MODE expMode);
  vtkGetMacro(ExposureMicroSec, float);
  PlusStatus SetExposureMicroSec(int exposureMicroSec);
  GAIN_MODE GetGainMode() { return this->GainMode; }
  PlusStatus SetGainMode(GAIN_MODE gainMode);
  vtkGetMacro(GainDB, float);
  PlusStatus SetGainDB(int gainDb);
  WHITE_BALANCE_MODE GetWhiteBalanceMode() { return this->WhiteBalanceMode; }
  PlusStatus SetWhiteBalanceMode(WHITE_BALANCE_MODE wbMode);
  vtkGetMacro(WhiteBalance, float);
  vtkSetMacro(WhiteBalance, float);
  SHARPENING_MODE GetSharpeningMode() { return this->SharpeningMode; }
  PlusStatus SetSharpeningMode(SHARPENING_MODE sharpMode);
  vtkGetMacro(Sharpening, float);
  vtkSetMacro(Sharpening, float);

protected:
  vtkPlusSpinnakerVideoSource();
  ~vtkPlusSpinnakerVideoSource();
  
  // camera configuration parameters
  int CameraNumber;
  std::string VideoFormat;
  FrameSizeType FrameSize;
  int FrameRate;
  EXPOSURE_MODE ExposureMode;
  float ExposureMicroSec;
  GAIN_MODE GainMode;
  float GainDB;
  WHITE_BALANCE_MODE WhiteBalanceMode;
  float WhiteBalance;
  SHARPENING_MODE SharpeningMode;
  float Sharpening;

private:
  vtkPlusSpinnakerVideoSource(const vtkPlusSpinnakerVideoSource&);
  void operator=(const vtkPlusSpinnakerVideoSource&);

  class vtkInternal;
  vtkInternal* Internal;

  unsigned long FrameNumber;
};

#endif