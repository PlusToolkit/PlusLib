/*=Plus=header=begin======================================================
    Program: Plus
    Copyright (c) UBC Biomedical Signal and Image Computing Laboratory. All rights reserved.
=========================================================Plus=header=end*/

#ifndef _VTKPLUSCLARIUS_H
#define _VTKPLUSCLARIUS_H

// Local Includes
#include "vtkPlusConfig.h"
#include "vtkPlusDataCollectionExport.h"
#include "vtkPlusDevice.h"
#include "vtkPlusDataSource.h"
#include "vtkPlusUsDevice.h"

/*!
\class vtkPlusClarius
\brief Interface to the Clarius ultrasound scans
This class talks with a Clarius Scanner over the Clarius API.
Requires PLUS_USE_CLARIUS option in CMake.
 \ingroup PlusLibDataCollection
*/
class vtkPlusDataCollectionExport vtkPlusClarius : public vtkPlusUsDevice
  /*vtkPlusCLARIUS is a subclass of vtkPlusDevice*/
{
public:
  vtkTypeMacro(vtkPlusClarius, vtkPlusDevice);
  /*! This is a singleton pattern New. There will only be ONE
  reference to a vtkPlusClarius object per process. Clients that
  call this must call Delete on the object so that the reference
  counting will work. The single instance will be unreferenced
  when the program exits. */
  static vtkPlusClarius* New();

  /*! return the singleton instance with no reference counting */
  static vtkPlusClarius* GetInstance();

  void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

  /*!
  Probe to see to see if the device is connected to the
  computer. This method should be overridden in subclasses.
  */
  virtual PlusStatus Probe();

  /*! Hardware device SDK version. This method should be overridden in subclasses. */
  virtual std::string GetSdkVersion();

  /*! Read configuration from xml data */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* config);

  /*! Write configuration to xml data */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* config);

  /*! Perform any completion tasks once configured
   a multi-purpose function which is called after all devices have been configured,
   all inputs and outputs have been connected between devices,
   but before devices begin collecting data.
   This is the last chance for your device to raise an error about improper or insufficient configuration.
  */
  virtual PlusStatus NotifyConfigured();

  /*!
  Request raw ultrasound data in the last N seconds
  */
  PlusStatus RequestLastNSecondsRawData(double lastNSeconds);

  /*!
  Request raw ultrasound data between two timestamps
  If both timestamps are zero, all available data will be requested
  */
  PlusStatus RequestRawData(long long startTimestampNanoSeconds, long long endTimestampNanoSeconds);

  /*! The IMU streaming is supported and raw IMU data is written to csv file, however interpreting imu data as tracking data is not supported*/
  bool IsTracker() const { return false; }

  vtkSetMacro(FrameHeight, unsigned int);
  vtkGetMacro(FrameHeight, unsigned int);

  vtkSetMacro(FrameWidth, unsigned int);
  vtkGetMacro(FrameWidth, unsigned int);

  vtkSetMacro(IpAddress, std::string);
  vtkGetMacro(IpAddress, std::string);

  vtkSetMacro(TcpPort, unsigned int);
  vtkGetMacro(TcpPort, unsigned int);

  vtkSetMacro(ImuEnabled, bool);
  vtkGetMacro(ImuEnabled, bool);

  vtkSetMacro(WriteImagesToDisk, bool);
  vtkGetMacro(WriteImagesToDisk, bool);

  vtkSetMacro(ImuOutputFileName, std::string);
  vtkGetMacro(ImuOutputFileName, std::string);

  /*!
  Compress raw data using gzip if enabled
  */
  vtkGetMacro(CompressRawData, bool);
  vtkSetMacro(CompressRawData, bool);
  vtkBooleanMacro(CompressRawData, bool);

  /*!
  Output filename of the raw Clarius data
  If empty, data will be written to the Plus output directory
  */
  vtkSetStdStringMacro(RawDataOutputFilename);
  vtkGetStdStringMacro(RawDataOutputFilename);

  vtkSetVector2Macro(AhrsAlgorithmGain, double);
  vtkGetVector2Macro(AhrsAlgorithmGain, double);
  vtkSetVector2Macro(FilteredTiltSensorAhrsAlgorithmGain, double);
  vtkGetVector2Macro(FilteredTiltSensorAhrsAlgorithmGain, double);

  enum AHRS_METHOD
  {
    AHRS_MADGWICK,
    AHRS_MAHONY
  };

protected:
  vtkPlusClarius();
  ~vtkPlusClarius();

  virtual PlusStatus InternalConnect();
  virtual PlusStatus InternalDisconnect();

protected:
  unsigned int TcpPort;
  std::string IpAddress;

  std::string ImuOutputFileName;
  int FrameWidth;
  int FrameHeight;
  bool ImuEnabled;
  bool WriteImagesToDisk;
  bool CompressRawData;

  std::string RawDataOutputFilename;

  static vtkPlusClarius* Instance;

  /*!
    Gain values used by the AHRS algorithm (Mahony: first parameter is proportional, second is integral gain; Madgwick: only the first parameter is used)
    Higher gain gives higher reliability to accelerometer&magnetometer data.
  */
  double AhrsAlgorithmGain[2];
  double FilteredTiltSensorAhrsAlgorithmGain[2];

  /*!
    In tilt sensor mode we don't use the magnetometer, so we have to provide a direction reference.
    The orientation is specified by specifying an axis that will always point to the "West" direction.
    Recommended values:
    If sensor axis 0 points down (the sensor plane is about vertical) => TiltSensorDownAxisIndex = 2.
    If sensor axis 1 points down (the sensor plane is about vertical) => TiltSensorDownAxisIndex = 0.
    If sensor axis 2 points down (the sensor plane is about horizontal) => TiltSensorDownAxisIndex = 1.
  */
  int TiltSensorWestAxisIndex;
  int FilteredTiltSensorWestAxisIndex;

  class vtkInternal;
  vtkInternal* Internal;
};

#endif //_VTKPLUSCLARIUS_H
