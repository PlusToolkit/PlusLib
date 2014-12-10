/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkIntersonVideoSource_h
#define __vtkIntersonVideoSource_h

#include "vtkDataCollectionExport.h"

#include "vtkPlusDevice.h"
#include "vtkUSImagingParameters.h"

/*!
  \class vtkIntersonVideoSource 
  \brief Class for acquiring ultrasound images from Interson USB ultrasound systems

  Requires the PLUS_USE_INTERSON option in CMake.
  Requires Interson iSDK2012 (SDK provided by Interson).

  \ingroup PlusLibDataCollection
*/
class vtkDataCollectionExport vtkIntersonVideoSource : public vtkPlusDevice
{
  static const int MAXIMUM_TGC_DB;
public:
  static vtkIntersonVideoSource *New();
  vtkTypeMacro(vtkIntersonVideoSource,vtkPlusDevice);
  void PrintSelf(ostream& os, vtkIndent indent);   

  virtual bool IsTracker() const { return false; }

  /*! Read configuration from xml data */  
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* config); 
  /*! Write configuration to xml data */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* config);    

  /*! Verify the device is correctly configured */
  virtual PlusStatus NotifyConfigured();

  virtual std::string GetSdkVersion();

  /*! Set the view sector in the parameters */
  PlusStatus SetSectorPercent(double value);
  /*! Set the intensity in the parameters */
  PlusStatus SetIntensity(int value);
  /*! Set the contrast in the parameters */
  PlusStatus SetContrast(int value);
  /*! Set the dynamic range in the parameters */
  PlusStatus SetDynRangeDb(double value);
  /*! Set the speed of sound in the parameters */
  PlusStatus SetSoundVelocity(double aVel);

  vtkGetMacro(EnableProbeButtonMonitoring, bool);
  vtkSetMacro(EnableProbeButtonMonitoring, bool);

protected:
  /*! Constructor */
  vtkIntersonVideoSource();
  /*! Destructor */
  ~vtkIntersonVideoSource();

  /*! Device-specific connect */
  virtual PlusStatus InternalConnect();


  /*! Device-specific disconnect */
  virtual PlusStatus InternalDisconnect();

  /*! Device-specific recording start */
  virtual PlusStatus InternalStartRecording();

  /*! Device-specific recording stop */
  virtual PlusStatus InternalStopRecording();

  /*! The internal function which actually does the grab.  */
  PlusStatus InternalUpdate();

  PlusStatus GetFullIniFilePath(std::string &fullPath);

  PlusStatus FreezeDevice(bool freeze);

  PlusStatus WaitForFrame();

  PlusStatus GetSampleFrequencyDevice(double& aFreq);

  PlusStatus GetProbeVelocityDevice(double& aVel);
  
  /*! Set the speed of sound for the device */
  PlusStatus SetSoundVelocityDevice(double aVel);

  /* Represents the depth, in pixels, the display window will be. This defaults to 512 pixels for newly initialized probes.*/
  PlusStatus SetWindowDepthDevice(int height);

  /* Set the probe depth in mm */
  PlusStatus SetDepthMm(double depthMm);

  /* Set the probe depth in mm in the device */
  PlusStatus SetDepthMmDevice(double depthMm);

  /* Set the image size */
  PlusStatus SetImageSize(int imageSize[2]);

  /* Set the frquency in Mhz */
  PlusStatus SetFrequencyMhz(double freq);

  /* Set the desired probe frequency in Hz. The resulting probe speed will be approximately the value specified */
  PlusStatus SetProbeFrequencyDevice(double aFreq);

  /* Set the gain in percent */
  PlusStatus SetGainPercent(double gainPercent[3]);
  /* Set the gain in percent in the device */
  PlusStatus SetGainPercentDevice(double gainPercent[3]);

  /* Set the zoom factor. */
  PlusStatus SetZoomFactor(double gainPercent);
  /* Set the zoom factor in the device. */
  PlusStatus SetDisplayZoomDevice(double zoom);

  /* Each probe has a defined set of allowed modes. 
  These modes are combinations of pulse frequency and sample rate that yield acceptable results
  with that particular probe. While there is no enforcement to use only these modes, one should 
  understand the implications on image quality if one of the allowed modes is not selected.*/
  PlusStatus GetProbeAllowedModes(std::vector< std::pair<double,double> > &allowedModes);

  /*! Get probe name from the device */
  PlusStatus GetProbeNameDevice(std::string& probeName);

  /*! Receive new imaging parameters and apply them to this device
  \param newImagingParameters the new parameters to apply to the device
  */
  virtual PlusStatus ApplyNewImagingParameters(const vtkUsImagingParameters& newImagingParameters);

  /*! Create a new lookup table */
  PlusStatus CreateLUT();

  // For internal storage of additional variables (to minimize the number of included headers)
  class vtkInternal;
  vtkInternal* Internal;

  vtkUsImagingParameters* ImagingParameters;

  bool Interpolate;
  bool BidirectionalScan;
  bool Frozen;

  int ClockDivider;
  double ClockFrequencyMHz;
  int PulseFrequencyDivider;

  double LutCenter;
  double LutWindow;
  int ImageSize[2];
  double PulseVoltage;

  // ProbeButtonPressCount is incremented each time the button on the probe is pressed
  // The value is available in the output channel in the translation component of the ProbeButtonToDummyTransform
  int ProbeButtonPressCount;

  bool EnableProbeButtonMonitoring;

private:
  vtkIntersonVideoSource(const vtkIntersonVideoSource&);  // Not implemented.
  void operator=(const vtkIntersonVideoSource&);  // Not implemented.
};

#endif
