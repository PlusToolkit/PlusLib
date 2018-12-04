/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusCapistranoVideoSource_h
#define __vtkPlusCapistranoVideoSource_h

#include "vtkPlusDataCollectionExport.h"
#include "vtkPlusUsDevice.h"
#include "vtkPlusUsImagingParameters.h"

/*!
 \class vtkPlusCapistranoVideoSource
 \brief Class for acquiring ultrasound images from Capistrano Labs USB ultrasound systems.

 Requires PLUS_USE_CAPISTRANO_VIDEO option in CMake.
 Requires the Capistrano cSDK2018, cSDK2016, or cSDK2013 (SDK provided by Capistrano Labs).

 \ingroup PlusLibDataCollection.
*/
class vtkPlusDataCollectionExport vtkPlusCapistranoVideoSource: public vtkPlusUsDevice
{
public:
  /*! Constructor for a smart pointer of this class*/
  static vtkPlusCapistranoVideoSource* New();
  vtkTypeMacro(vtkPlusCapistranoVideoSource, vtkPlusDevice);
  virtual void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

  /*! Specify the device connected to this class */
  virtual bool IsTracker() const { return false; }

  /*! Read configuration from xml data */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* config);

  /*! Write configuration to xml data */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* config);

  /*! Verify the device is correctly configured */
  virtual PlusStatus NotifyConfigured();

  /*! Get the version of SDK */
  virtual std::string GetSdkVersion();

#ifdef CAPISTRANO_SDK2018
  /*! Get the hardware version. */
  virtual int GetHardwareVersion();

  /*! Get the high pass filter. */
  virtual int GetHighPassFilter();

  /*! Get the low pass filter. */
  virtual int GetLowPassFilter();
#endif

  /* Update Speed of Sound */
  PlusStatus GetProbeVelocityDevice(float& aVel);

  /* Set the speed of sound of US probe */
  PlusStatus SetSoundVelocity(float ss);

  /* Set the scan directional mode of US probe */
  PlusStatus SetBidirectionalMode(bool mode);

  /* Get the scan directional mode of US probe */
  bool GetBidirectionalMode();

  /* Set the size of cinebuffer of US probe */
  PlusStatus SetCineBuffers(int cinebuffer);

  /* Set the sample frequency of US probe */
  PlusStatus SetSampleFrequency(float sf);

  /* Set the pulser frequency of US probe */
  PlusStatus SetPulseFrequency(float pf);

  /* Set the desired probe speed in Hz.
  *  The resulting probe speed will be approximately the value specified. */
  PlusStatus SetWobbleRate(unsigned char wobbleRate);

  /* Probe speed in Hz */
  unsigned char GetWobbleRate();

  /* Set the jitter compensation value for the probe servo */
  PlusStatus SetJitterCompensation(unsigned char jitterComp);

  /* Jitter compensation value for the probe servo */
  unsigned char GetJitterCompensation();

  /* Set the position scale value for the probe servo */
  PlusStatus SetPositionScale(unsigned char scale);

  /* Position scale value for the probe servo */
  unsigned char GetPositionScale();

  /* Set the desired probe scan angle (in radians) */
  PlusStatus SetSweepAngle(float sweepAngle);

  /* Probe scan angle (in radians) */
  float GetSweepAngle();

  /* Set the gain value for the probe servo */
  PlusStatus SetServoGain(unsigned char servoGain);

  /* Gain value for the probe servo */
  unsigned char GetServoGain();

  /* State is the desired overscan multiplier.
  *  Overscan is done in 6.25% steps from 6.25% to 50% (three-bit number).
  *  As such, the valid values are 0…7 which correspond to 6.25%, 12.5%, …, 43.75%, 50%. */
  PlusStatus SetOverscan(int state);

  /* Overscan multiplier. Overscan is done in 6.25% steps from 6.25% to 50% (three-bit number).
  *  As such, the valid values are 0…7 which correspond to 6.25%, 12.5%, …, 43.75%, 50%. */
  int GetOverscan();

  /* Sets the desired probe servo derivative compensation */
  PlusStatus SetDerivativeCompensation(unsigned char derivativeCompensation);

  /* Probe servo derivative compensation */
  unsigned char GetDerivativeCompensation();

  /* Set the pulser voltage of US probe */
  PlusStatus SetPulseVoltage(float pv);

  /* Set the scan depth of US probe */
  PlusStatus SetScanDepth(float sd);

  /* Set the interpolation of B-Mode image */
  PlusStatus SetInterpolate(bool interpolate);

  /* Set the average mode of US B-Mode image */
  PlusStatus SetAverageMode(bool averagemode);

  /* Set the view option of US B-Mode image */
  PlusStatus SetBModeViewOption(unsigned int bmodeviewoption);

  /* Set the size of US B-mode image */
  PlusStatus SetImageSize(const FrameSizeType& imageSize);

  /* Set the Intensity (Brightness) of US B-mode image */
  PlusStatus SetIntensity(double value);

  /* Set the Contrast of US B-mode image */
  PlusStatus SetContrast(double value);

  /* Set the zoom factor. */
  PlusStatus SetZoomFactor(double zoomfactor);

  /* Set the zoom factor on the US Device. */
  PlusStatus SetDisplayZoomDevice(double zoom);

  /* Set the LUT Center of US B-mode image */
  PlusStatus SetLutCenter(double lutcenter);

  /* Set the LUT Window of US B-mode image */
  PlusStatus SetLutWindow(double lutwindow);

  /* Set the gain in percent */
  PlusStatus SetGainPercent(double gainPercent[3]);

  /* Set the gain in percent in the device */
  PlusStatus SetGainPercentDevice(double gainPercent[3]);

  /* Set the probe depth on US Device in mm */
  PlusStatus SetDepthMmDevice(float depthMm);

  /* Set the probe depth in mm */
  PlusStatus SetDepthMm(float depthMm);

  /* Apply a completely new set of imaging parameters to the device */
  PlusStatus SetNewImagingParametersDevice(const vtkPlusUsImagingParameters& newImagingParameters);

  /*! Set Current Pixel Spacing values Of US Image (mm) */
  vtkSetVector3Macro(CurrentPixelSpacingMm, double);

  /*! Get Current Pixel Spacing values Of US Image (mm) */
  vtkGetVector3Macro(CurrentPixelSpacingMm, double);

  /*! Set ON/OFF of collecting US data. */
  PlusStatus FreezeDevice(bool freeze);

  /*! Checks whether the device is frozen or live. */
  bool IsFrozen();

protected:
  /*! Constructor */
  vtkPlusCapistranoVideoSource();

  /*! Destructor */
  ~vtkPlusCapistranoVideoSource();

  /*! Device-specific connect */
  virtual PlusStatus InternalConnect();

  /*! Device-specific disconnect */
  virtual PlusStatus InternalDisconnect();

  /*! Device-specific recording start */
  virtual PlusStatus InternalStartRecording();

  /*! Device-specific recording stop */
  virtual PlusStatus InternalStopRecording();

  /*! Initialize vtkPlusCapistranoVideoSource */
  PlusStatus InitializeCapistranoVideoSource(bool probeConnected = false);

  /*! The internal function which actually does the grab. */
  PlusStatus InternalUpdate();

  /*! Initialize a Capistrano Probe */
  PlusStatus InitializeCapistranoProbe();

  /* Set up US Probe with ID */
  PlusStatus SetupProbe(int probeID = 0);

  /*! Initialize a ImageWindow and vtkPlusDataSource */
  PlusStatus InitializeImageWindow();

  /*! Initialize an LUT for US B-Mode image */
  PlusStatus InitializeLUT();

  /*! Initialize an TGC for US B-Mode image */
  PlusStatus InitializeTGC();

  /*! Wait US Data from US device  */
  PlusStatus WaitForFrame();

  /* Set the boolean value to use US parameters from XML file */
  PlusStatus SetUpdateParameters(bool b);

  /* Update US parameters (US probe/B-Mode parameters) */
  PlusStatus UpdateUSParameters();

  /* Update US probe parameters  */
  PlusStatus UpdateUSProbeParameters();

  /* Update US B-Mode parameters */
  PlusStatus UpdateUSBModeParameters();

  /* Calculate US Image Display */
  PlusStatus CalculateDisplay();

  /* Calculate US Image Display with a given B-Mode view option */
  PlusStatus CalculateDisplay(unsigned int option);

  /* Update US Scan depth */
  PlusStatus UpdateDepthMode();

  /* Update US Scan depth with a given clockdivider */
  PlusStatus UpdateDepthMode(int clockdivider);

  /* Update US Sample frequency */
  PlusStatus GetSampleFrequencyDevice(float& aFreq);

  /*! Get probe name from the device */
  PlusStatus GetProbeNameDevice(std::string& probeName);

  /*! For internal storage of additional variables
      (to minimize the number of included headers) */
  class        vtkInternal;
  vtkInternal* Internal;

  bool                           Frozen;
  bool                           UpdateParameters;
  bool                           BidirectionalMode;
  int                            ProbeID;
  int                            ClockDivider;
  int                            CineBuffers;
  float                          SampleFrequency;
  float                          PulseFrequency;
  unsigned char                  PositionScale;
  bool                           Interpolate;
  bool                           AverageMode;
  unsigned int                   CurrentBModeViewOption;
  double                         LutCenter;
  double                         LutWindow;

  igsioTrackedFrame::FieldMapType CustomFields;

private:
  vtkPlusCapistranoVideoSource(const vtkPlusCapistranoVideoSource&);  // Not implemented
  void operator=(const vtkPlusCapistranoVideoSource&);  // Not implemented
};

#endif