/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusWinProbeVideoSource_h
#define __vtkPlusWinProbeVideoSource_h

#include <thread>

#include "vtkPlusDataCollectionExport.h"
#include "vtkPlusUsDevice.h"
#include "vtkPlusUsImagingParameters.h"

/*!
 \class vtkPlusWinProbeVideoSource
 \brief Class for acquiring ultrasound images from WinProbe ultrasound systems.

 Requires PLUS_USE_WINPROBE_VIDEO option in CMake.
 Requires the WinProbeSDK.

 \ingroup PlusLibDataCollection.
*/
class vtkPlusDataCollectionExport vtkPlusWinProbeVideoSource : public vtkPlusUsDevice
{
public:
  /*! Constructor for a smart pointer of this class*/
  static vtkPlusWinProbeVideoSource* New();
  vtkTypeMacro(vtkPlusWinProbeVideoSource, vtkPlusDevice);
  virtual void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

  /*! Specify the device connected to this class */
  virtual bool IsTracker() const { return false; }

  /*! Read configuration from xml data */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* config);

  /*! Write configuration to xml data */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* config);

  /* Set the transmit frequency of US probe (MHz) */
  PlusStatus SetTransmitFrequencyMHz(float frequency);

  /* Get the transmit frequency of US probe (MHz) */
  float GetTransmitFrequencyMHz();

  /* Set the voltage of US probe (percent?) */
  PlusStatus SetVoltage(uint8_t voltage);

  /* Get the voltage of US probe (percent?) */
  uint8_t GetVoltage();

  /* Set the scan depth of US probe (mm) */
  PlusStatus SetScanDepthMm(float depth);

  /* Get the scan depth of US probe (mm) */
  float GetScanDepthMm();

   /* Set the scan depth of US probe (mm) */
  PlusStatus SetSSDecimation(uint8_t value);

  /* Get the scan depth of US probe (mm) */
  uint8_t GetSSDecimation();

  /* Get the width of current transducer (mm) */
  float GetTransducerWidthMm();

  /* Get the pixel spacing for all 3 axes (mm) */
  const double* GetCurrentPixelSpacingMm();

  /* Get the TGC value, index 0 to 7, value 0.0 to 40.0 */
  double GetTimeGainCompensation(int index);

  /* Set the TGC value, index 0 to 7, value 0.0 to 40.0 */
  PlusStatus SetTimeGainCompensation(int index, double value);

  /* Get the TGC First Gain Value near transducer face */
  double GetFirstGainValue();

  /* Set the TGC First Gain Value near transducer face */
  PlusStatus SetFirstGainValue(double value);

  /* Get the focal depth, index 0 to 4 */
  float GetFocalPointDepth(int index);

  /* Set the focal depth, index 0 to 4 */
  PlusStatus SetFocalPointDepth(int index, float depth);

  /* Whether or not to use device's built-in frame reconstruction */
  void SetUseDeviceFrameReconstruction(bool value) { m_UseDeviceFrameReconstruction = value; }

  /* Whether or not to use device's built-in frame reconstruction */
  bool GetUseDeviceFrameReconstruction() { return m_UseDeviceFrameReconstruction; }

  /*! Set ON/OFF of collecting US data. */
  PlusStatus FreezeDevice(bool freeze);

  /*! Checks whether the device is frozen or live. */
  bool IsFrozen();

  /*! Sets GUID of the probe type to be used. */
  PlusStatus SetTransducerID(std::string guid);

  /*! Gets GUID of the probe type to be used. */
  std::string GetTransducerID();

  /*! Sets the noise floor for intensity range compression. */
  void SetMinValue(const uint16_t minValue) { m_MinValue = minValue; }

  /*! Gets the noise floor for intensity range compression. */
  uint16_t GetMinValue() const { return m_MinValue; }

  /*! Sets the typical high value for intensity range compression. */
  void SetMaxValue(const uint16_t maxValue) { m_MaxValue = maxValue; }

  /*! Gets the typical high value for intensity range compression. */
  uint16_t GetMaxValue() const { return m_MaxValue; }

  /*! Sets the threshold value for switching from log to linear mapping for intensity range compression. */
  void SetLogLinearKnee(const uint16_t threshold) { m_Knee = threshold; }

  /*! Gets the threshold value for switching from log to linear mapping for intensity range compression. */
  uint16_t GetLogLinearKnee() const { return m_Knee; }

  /*! Sets the maximum output value for log mapping of intensity range. */
  void SetLogMax(const uint8_t threshold) { m_OutputKnee = threshold; }

  /*! Gets the maximum output value for log mapping of intensity range. */
  uint8_t GetLogMax() const { return m_OutputKnee; }

  void SetSpatialCompoundEnabled(bool value);
  bool GetSpatialCompoundEnabled();

  void SetSpatialCompoundAngle(float value);
  float GetSpatialCompoundAngle();

  void SetSpatialCompoundCount(int32_t value);
  int32_t GetSpatialCompoundCount();

  void SetBHarmonicEnabled(bool value);
  bool GetBHarmonicEnabled();

  void SetBRFEnabled(bool value);
  bool GetBRFEnabled();

  void SetMModeEnabled(bool value);
  bool GetMModeEnabled();

  void SetMRevolvingEnabled(bool value);
  bool GetMRevolvingEnabled();

  void SetMPRFrequency(int32_t value);
  int32_t GetMPRFrequency();

  void SetMLineIndex(int32_t value);
  int32_t GetMLineIndex();

  void SetMWidth(int value);
  int GetMWidth();

  void SetMWidthLines(int32_t value);
  int32_t GetMWidthLines();

  void SetMAcousticLineCount(int32_t value);
  int32_t GetMAcousticLineCount();

  void SetMDepth(int32_t value);
  int32_t GetMDepth();

  void SetBFrameRateLimit(int32_t value);
  int32_t GetBFrameRateLimit();

  int GetTransducerInternalID();

  enum class Mode
  {
    B = 0, // only B mode
    BRF, // RF mode with reference B mode
    RF, // RF mode only
    M, // M mode
    PW, // Pulsed Wave Doppler
    CFD // Color-Flow Doppler
  };

  /*! Sets the ultrasound imaging mode. */
  void SetMode(Mode mode)
  {
    m_Mode = mode;
  }

  /*! Gets the ultrasound imaging mode. */
  Mode GetMode()
  {
    return m_Mode;
  }

  Mode StringToMode(std::string modeString);
  std::string ModeToString(Mode mode);

  int32_t MWidthFromSeconds(int value);
  int MSecondsFromWidth(int32_t value);

  std::vector<double> GetPrimarySourceSpacing();
  std::vector<double> GetExtraSourceSpacing();

protected:
  /*! Constructor */
  vtkPlusWinProbeVideoSource();

  /*! Destructor */
  ~vtkPlusWinProbeVideoSource();

  /*! Device-specific connect */
  virtual PlusStatus InternalConnect() VTK_OVERRIDE;

  /*! Device-specific disconnect */
  virtual PlusStatus InternalDisconnect() VTK_OVERRIDE;

  /*! Device-specific recording start */
  virtual PlusStatus InternalStartRecording() VTK_OVERRIDE;

  /*! Device-specific recording stop */
  virtual PlusStatus InternalStopRecording() VTK_OVERRIDE;

  /*! Updates internal spacing based on current depth */
  void AdjustSpacing(bool value);

  /*! Updates buffer size based on current depth */
  void AdjustBufferSizes();

  friend int __stdcall frameCallback(int length, char* data, char* hHeader, char* hGeometry, char* hModeFrameHeader);
  void ReconstructFrame(char* data, std::vector<uint8_t>& buffer, const FrameSizeType& frameSize);
  void FlipTexture(char* data, const FrameSizeType& frameSize, int rowPitch);
  void FrameCallback(int length, char* data, char* hHeader, char* hGeometry);

  float m_ScanDepth = 26.0; //mm
  float m_TransducerWidth = 38.1; //mm
  float m_Frequency = 10.9; //MHz
  uint8_t m_Voltage = 40;
  std::string m_TransducerID; //GUID
  double m_ADCfrequency = 60.0e6; //MHz
  double m_TimestampOffset = 0; //difference between program start time and latest internal timer restart
  double first_timestamp = 0;
  double m_LastTimestamp = 1000; //used to determine timer restarts and to update timestamp offset
  unsigned m_LineCount = 128;
  unsigned m_SamplesPerLine = 0;
  std::vector<uint8_t> m_PrimaryBuffer;
  std::vector<uint8_t> m_ExtraBuffer;
  bool m_UseDeviceFrameReconstruction = true;
  igsioFieldMapType m_CustomFields;
  double m_TimeGainCompensation[8];
  float m_FocalPointDepth[4];
  uint16_t m_MinValue = 16; //noise floor
  uint16_t m_MaxValue = 16384; //maximum typical value
  uint16_t m_Knee = 4096; // threshold value for switching from log to linear
  uint8_t m_OutputKnee = 64; // log-linear knee in output range
  bool m_SpatialCompoundEnabled = false;
  float m_SpatialCompoundAngle = 10.0f;
  int32_t m_SpatialCompoundCount = 0;
  bool m_MRevolvingEnabled = false;
  int32_t m_MPRF = 100;
  int32_t m_MLineIndex = 60;
  int32_t m_MWidth = 256;
  int32_t m_MAcousticLineCount = 0;
  int32_t m_MDepth = 0;
  uint8_t m_SSDecimation = 2;
  double m_FirstGainValue = 15;
  int32_t m_BFrameRateLimit = 0;
  bool m_BHarmonicEnabled = false;
  std::vector<vtkPlusDataSource*> m_PrimarySources;
  std::vector<vtkPlusDataSource*> m_ExtraSources;

  Mode m_Mode = Mode::B;

public:
  vtkPlusWinProbeVideoSource(const vtkPlusWinProbeVideoSource&) = delete;
  void operator=(const vtkPlusWinProbeVideoSource&) = delete;
};

#endif
