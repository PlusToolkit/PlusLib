/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusAndorVideoSource_h
#define __vtkPlusAndorVideoSource_h

#include "vtkPlusDataCollectionExport.h"
#include "vtkPlusDevice.h"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"

/*!
 \class vtkPlusAndorVideoSource
 \brief Class for acquiring images from Andor cameras

 Requires PLUS_USE_ANDOR_CAMERA option in CMake.
 Requires the Andor SDK (SDK provided by Andor).

 \ingroup PlusLibDataCollection.
*/
class vtkPlusDataCollectionExport vtkPlusAndorVideoSource: public vtkPlusDevice
{
public:
  /*! Constructor for a smart pointer of this class*/
  static vtkPlusAndorVideoSource* New();
  vtkTypeMacro(vtkPlusAndorVideoSource, vtkPlusDevice);
  virtual void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

  /*! Specify the device connected to this class */
  virtual bool IsTracker() const
  {
    return false;
  }

  /*! Read configuration from xml data */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* config);

  /*! Write configuration to xml data */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* config);

  /*! Verify the device is correctly configured */
  virtual PlusStatus NotifyConfigured();

  /*! Get the version of SDK */
  virtual std::string GetSdkVersion();

  /*! Shutter mode:
   * For an external shutter: Output TTL high signal to open shutter.
   */
  enum ShutterMode : int
  {
    FullyAuto = 0,
    PermanentlyOpen = 1,
    PermanentlyClosed = 2,
    OpenForFVBSeries = 4,
    OpenForAnySeries = 5
  };
  PlusStatus SetShutter(ShutterMode shutter);
  ShutterMode GetShutter();

  /*! Frame exposure time, seconds. Sets to the nearest valid value not less than the given value. */
  PlusStatus SetExposureTime(float exposureTime);
  float GetExposureTime();

  /*! Horizontal and vertical binning. Allowed values: 1, 2, 4, 8. */
  PlusStatus SetHorizontalBins(int bins);
  PlusStatus SetVerticalBins(int bins);

  /*! Horizontal and vertical shift speed. */
  PlusStatus SetHSSpeed(int type, int index);
  PlusStatus SetVSSpeed(int index);

  /*! Index of the pre-amp gain, not the actual value. */
  PlusStatus SetPreAmpGain(int preAmptGain);
  int GetPreAmpGain();

  /*! Acquisition mode. Valid values:
   * 1 Single Scan
   * 2 Accumulate
   * 3 Kinetics
   * 4 Fast Kinetics
   * 5 Run till abort
   */
  enum AcquisitionMode : int
  {
    SingleScan = 1,
    Accumulate = 2,
    Kinetics = 3,
    FastKinetics = 4,
    RunTillAbort = 5
  };
  PlusStatus SetAcquisitionMode(AcquisitionMode acquisitionMode);
  AcquisitionMode GetAcquisitionMode();

  /*! Readout mode. */
  enum ReadMode : int
  {
    FullVerticalBinning = 0,
    MultiTrack = 1,
    RandomTrack = 2,
    SingleTrack = 3,
    Image = 4
  };
  PlusStatus SetReadMode(ReadMode setReadMode);
  ReadMode GetReadMode();

  /*! Trigger mode. Valid values:
   * 0. Internal
   * 1. External
   * 6. External Start
   * 7. External Exposure (Bulb)
   * 9. External FVB EM (only valid for EM Newton models in FVB mode)
   * 10. Software Trigger
   * 12. External Charge Shifting
   */
  enum TriggerMode : int
  {
    Internal = 0,
    External = 1,
    ExternalStart = 6,
    ExternalExposure = 7,
    ExternalFVBEM = 9,
    SoftwareTrigger = 10,
    ExternalChargeShifting = 12
  };
  PlusStatus SetTriggerMode(TriggerMode triggerMode);
  TriggerMode GetTriggerMode();

  std::vector<double> GetSpacing(int horizontalBins, int verticalBins);

  /*! Normal operating temperature (degrees celsius). */
  PlusStatus SetCoolTemperature(int coolTemp);
  int GetCoolTemperature();

  /*! Lowest temperature at which it is safe to shut down the camera. */
  PlusStatus SetSafeTemperature(int safeTemp);
  int GetSafeTemperature();

  /*! Get the current temperature of the camera in degrees celsius. */
  float GetCurrentTemperature();

  /*! Paths to correction images for dead pixels, additive and multiplicative bias. */
  PlusStatus SetBadPixelCorrectionImage(const std::string badPixelFilePath);
  std::string GetBadPixelCorrectionImage()
  {
    return badPixelCorrection;
  }
  PlusStatus SetBiasDarkCorrectionImage(const std::string biasDarkFilePath);
  std::string GetBiasDarkCorrectionImage()
  {
    return biasDarkCorrection;
  }
  PlusStatus SetFlatCorrectionImage(const std::string flatFilePath);
  std::string GetFlatCorrectionImage()
  {
    return flatCorrection;
  }

  /*! -1 uses currently active settings. */
  PlusStatus AcquireBLIFrame(int binning, int vsSpeed, int hsSpeed, float exposureTime);

  /*! -1 uses currently active settings. */
  PlusStatus AcquireGrayscaleFrame(int binning, int vsSpeed, int hsSpeed, float exposureTime);

  /*! Convenience function to save a bias frame for a certain binning/speed configuration. */
  PlusStatus AcquireCorrectionFrame(std::string correctionFilePath, ShutterMode shutter, int binning, int vsSpeed, int hsSpeed, float exposureTime);

  /*! Cooler Mode control. When CoolerMode is set on, the cooler
      will be kept on when the camera is shutdown. This is helpful to
      reduce the number of cooling cycles the camera undergoes. Power loss to the camera
      will result in the camera returning to ambient temperature.
  */
  PlusStatus SetCoolerMode(int mode);
  int GetCoolerMode();

  /*! Turn the cooler on/off. */
  PlusStatus SetCoolerState(bool coolerState);
  bool IsCoolerOn();

  /*! Wait for the camera to reach operating temperature (e.g. -70°C). */
  void WaitForCooldown();

  /*! Wait for the camera to reach safe temperature for poweroff (e.g. -20°C).
      From Andor Employee:
      Only Classic, ICCD and cameras with a fibre attached must have their cooling and warming up controlled at a particular rate.
      For everything else you can just call ShutDown and the camera will safely return to room temperature.
      Classic systems are cameras that use our original PCI controller cards eg CCI-010 or CCI-001.
  */
  void WaitForWarmup();

  /*! Check the return status of Andor SDK functions. */
  unsigned int checkStatus(unsigned int returnStatus, std::string functionName);

  vtkPlusAndorVideoSource(const vtkPlusAndorVideoSource&) = delete;
  void operator=(const vtkPlusAndorVideoSource&) = delete;

protected:
  /*! Constructor */
  vtkPlusAndorVideoSource();

  /*! Destructor */
  ~vtkPlusAndorVideoSource();

  /*! Device-specific connect */
  virtual PlusStatus InternalConnect();

  /*! Device-specific disconnect */
  virtual PlusStatus InternalDisconnect();

  /*! Device-specific recording start */
  PlusStatus InternalStartRecording() override;

  /*! Device-specific recording stop */
  PlusStatus InternalStopRecording() override;

  /*! Initialize vtkPlusAndorVideoSource */
  PlusStatus InitializeAndorCamera();

  using DataSourceArray = std::vector<vtkPlusDataSource*>;

  /*! Initialize all data sources of the provided port */
  void InitializePort(DataSourceArray& port);

  void AdjustBuffers(int horizontalBins, int verticalBins);
  void AdjustSpacing(int horizontalBins, int verticalBins);

  /*! Acquire a single frame using current parameters. Data is put in the frameBuffer ivar. */
  PlusStatus AcquireFrame(float exposure, ShutterMode shutterMode, int binning, int vsSpeed, int hsSpeed);

  /*! Data from the frameBuffer ivar is added to the provided data source. */
  void AddFrameToDataSource(DataSourceArray& ds);

  /*! Calculates which cells need bad-pixel correction for the given binning level. */
  void FindBadCells(int binning);

  /*! Applies correction for bad pixels. */
  void CorrectBadPixels(int binning, cv::Mat& cvIMG);

  /*! Applies bias correction for dark current, flat correction and lens distortion. */
  void ApplyFrameCorrections(int binning);

  /*! Flag whether to call ApplyFrameCorrections on the raw acquired frame on acquisition
      or to skip frame corrections.
   */
  PlusStatus SetUseFrameCorrections(bool UseFrameCorrections);
  bool GetUseFrameCorrections();

  /*! This will be triggered regularly if this->StartThreadForInternalUpdates is true.
   * Framerate is controlled by this->AcquisitionRate. This is meant for debugging.
   */
  PlusStatus InternalUpdate() override
  {
    AcquireGrayscaleFrame(-1, -1, -1, -1);
    return PLUS_SUCCESS;
  }

  /*! Setting to true means cooler turns on at startup.
      Setting to false means cooler state doesn't change on start.
      Therefore use false if your previous session was set to maintain temperature on ShutDown.
  */
  PlusStatus SetInitializeCoolerState(bool InitializeCoolerState);
  bool InitializeCoolerState = true;

  /*! Dev flag whether to require the Camera to be at the Cool Temperature to acquire frames.
      It can be set to false to acquire frames even though not at the cool temperature.
  */
  PlusStatus SetRequireCoolTemp(bool RequireCoolTemp);
  bool RequireCoolTemp = true;

  PlusStatus TurnCoolerON();
  PlusStatus TurnCoolerOFF();

  vtkPlusAndorVideoSource::ShutterMode Shutter = ShutterMode::FullyAuto;
  float ExposureTime = 1.0; // seconds
  int HorizontalBins = 1;
  int VerticalBins = 1;
  int HSSpeed[2] = { 0, 1 };  // type, index
  int VSSpeed = 0;  // index
  int PreAmpGain = 0;
  bool UseFrameCorrections = true;

  // TODO: Need to handle differet cases for read/acquisiton modes?

  /*! From AndorSDK:=> 1: Single Scan   2: Accumulate   3: Kinetics   4: Fast Kinetics   5: Run till abort  */
  AcquisitionMode m_AcquisitionMode = AcquisitionMode::SingleScan;

  /*! From AndorSDK:=> 0: Full Vertical Binning   1: Multi-Track   2: Random-Track   3: Single-Track   4: Image */
  ReadMode m_ReadMode = ReadMode::Image;

  /*! From AndorSDK:=> 0. Internal   1. External  6. External Start  7. External Exposure(Bulb)  9. External FVB EM(only valid for EM Newton models in FVB mode) 10. Software Trigger  12. External Charge Shifting */
  TriggerMode m_TriggerMode = TriggerMode::Internal;

  /*! Temperatures are in °C (degrees Celsius) */
  int CoolerMode = 0;  // whether to return to ambient temperature on ShutDown
  int CoolTemperature = -50;
  int SafeTemperature = 5;
  float CurrentTemperature = 0.123456789; // easy to spot as uninitialized

  FrameSizeType frameSize = {1024, 1024, 1};
  std::vector<uint16_t> rawFrame;
  double currentTime = UNDEFINED_TIMESTAMP;

  // {f_x}{0}{c_x}
  // {0}{f_y}{c_y}
  // {0}{0}{1}
  double cameraIntrinsics[9] = { 0 };
  double distanceCoefficients[4] = { 0 }; // k_1, k_2, p_1, p_2
  std::string badPixelCorrection; //filepath to bad pixel image
  std::string flatCorrection; // filepath to master flat image
  std::string biasDarkCorrection; // filepath to master bias+dark image

  DataSourceArray BLIRaw;
  DataSourceArray BLICorrected;
  DataSourceArray GrayRaw;
  DataSourceArray GrayCorrected;

  double OutputSpacing[3] = { 0 };

  igsioFieldMapType CustomFields;
};

#endif