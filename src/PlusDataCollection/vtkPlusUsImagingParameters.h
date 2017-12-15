/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusUsImagingParameters_h
#define __vtkPlusUsImagingParameters_h

#include "vtkPlusDataCollectionExport.h"

#include <string>
#include <map>

/*!
\class vtkPlusUsImagingParameters
\brief This class is used to store a configuration of the imaging parameters of an ultrasound video device.
Ultrasound video devices should contain a member variable of this class that is used to set/query the depth, gain, etc.
This class exists mainly for two reasons:
* Provide a standard interface for accessing ultrasound parameters
* Enable standardized API for operating on ultrasound parameters
\ingroup PlusLibDataCollection

Currently contains the following items
* FrequencyMhz
* DepthMm
* SectorPercent
* GainPercent
* TGC [initialgain, midgain, fargain]
* Intensity
* Contrast
* DynRangeDb
* ZoomFactor
* Voltage
* ImageSize [x, y, z]
* SoundVelocity
*/

class vtkPlusDataCollectionExport vtkPlusUsImagingParameters : public vtkObject
{
protected:
  class ParameterInfo
  {
  public:
    ParameterInfo() : Value(""), Set(false), Changed(false) {};
    ParameterInfo(std::string defaultValue) : Value(defaultValue), Set(false), Changed(false) {};

    /// Serialized parameter value
    std::string Value;
    /// Flag indicating whether the invalid default has been changed to meaningful value
    /// (Note: sound velocity has a meaningful default)
    bool Set;
    /// Flag indicating whether the parameter is changed but has not been set to device
    bool Changed;
  };
  typedef std::map<std::string, ParameterInfo> ParameterMap;
  typedef ParameterMap::iterator ParameterMapIterator;
  typedef ParameterMap::const_iterator ParameterMapConstIterator;

  /*! Return an iterator to the beginning of the parameter space */
  ParameterMapConstIterator begin() const;
  /*! Return an iterator to the end of the parameter space */
  ParameterMapConstIterator end() const;

public:
  static const char* XML_ELEMENT_TAG;
  static const char* KEY_FREQUENCY;
  static const char* KEY_DEPTH;
  static const char* KEY_DYNRANGE;
  static const char* KEY_GAIN;
  static const char* KEY_TGC;
  static const char* KEY_INTENSITY;
  static const char* KEY_CONTRAST;
  static const char* KEY_SECTOR;
  static const char* KEY_ZOOM;
  static const char* KEY_SOUNDVELOCITY;
  static const char* KEY_VOLTAGE;
  static const char* KEY_IMAGESIZE;

public:
  static vtkPlusUsImagingParameters* New();
  vtkTypeMacro(vtkPlusUsImagingParameters,vtkObject);

  /*!
  Read main configuration from/to XML data
  Assumes that the data element passed is the device element, not the root!
  \param deviceConfig the XML element of the device
  */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* deviceConfig);

  /*!
  Write main configuration from/to XML data
  Assumes that the data element passed is the device element, not the root!
  \param deviceConfig the XML element of the device
  */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* deviceConfig);

  /*!
  Copy the values from another imaging parameters
  */
  virtual PlusStatus DeepCopy(const vtkPlusUsImagingParameters& otherParameters);

  /*!
  Request a stored value by key name
  \param paramName the key value to retrieve
  \param outputValue the output variable to write to
  */
  template<typename T> PlusStatus GetValue(const std::string& paramName, T& outputValue) const;
  /*!
  Set a stored value by key name
  Defined in the header to make it available externally
  \param paramName the key value to retrieve
  \param aValue the value to write
  */
  template<typename T> PlusStatus SetValue(const std::string& paramName, T aValue)
  {
    std::stringstream ss;
    ss << aValue;
    this->Parameters[paramName] = ParameterInfo(ss.str());
    this->Parameters[paramName].Set = true;
    return PLUS_SUCCESS;
  };
  /*!
  Request the set status of a member (whether it is not the default value)
  \param paramName the key value to retrieve
  */
  bool IsSet(const std::string& paramName) const;
  /*!
  Request the changed status of a member
  \param paramName the key value to retrieve
  */
  bool IsChanged(const std::string& paramName) const;

  /*! Set ultrasound transmitter frequency (MHz) */
  PlusStatus SetFrequencyMhz(double aFrequencyMhz);
  /*! Get ultrasound transmitter frequency (MHz) */
  PlusStatus GetFrequencyMhz(double& aFrequencyMhz) const;
  double GetFrequencyMhz() const;

  /*! Set the depth (mm) of B-mode ultrasound */
  PlusStatus SetDepthMm(double aDepthMm);
  /*! Get the depth (mm) of B-mode ultrasound */
  PlusStatus GetDepthMm(double& aDepthMm) const;
  double GetDepthMm() const;

  /*! Set the Gain (%) of B-mode ultrasound; valid range: 0-100 */
  PlusStatus SetGainPercent(double aGainPercent);
  /*! Get the Gain (%) of B-mode ultrasound; valid range: 0-100 */
  PlusStatus GetGainPercent(double aGainPercent) const;
  double GetGainPercent() const;

  /*! Set the TGC [initialgain, midgain, fargain] of B-mode ultrasound; valid range: 0-100 */
  PlusStatus SetTimeGainCompensation(const std::vector<double>& tgc);
  PlusStatus SetTimeGainCompensation(double* tgc, int length);
  /*! Get the TGC [initialgain, midgain, fargain] of B-mode ultrasound; valid range: 0-100 */
  PlusStatus GetTimeGainCompensation(std::vector<double>& tgc) const;
  std::vector<double> GetTimeGainCompensation() const;

  /*! Set the intensity of B-mode ultrasound */
  PlusStatus SetIntensity(double aIntensity);
  /*! Get the Intensity of B-mode ultrasound */
  PlusStatus GetIntensity(double& aIntensity) const;
  double GetIntensity() const;

  /*! Set the contrast of B-mode ultrasound */
  PlusStatus SetContrast(double aContrast);
  /*! Get the contrast of B-mode ultrasound */
  PlusStatus GetContrast(double& aContrast) const;
  double GetContrast() const;

  /*! Set the DynRange (dB) of B-mode ultrasound */
  PlusStatus SetDynRangeDb(double aDynRangeDb);
  /*! Get the DynRange (dB) of B-mode ultrasound */
  PlusStatus GetDynRangeDb(double& aDynRangeDb) const;
  double GetDynRangeDb() const;

  /*! Set the Zoom (%) of B-mode ultrasound; valid range: 0-100 */
  PlusStatus SetZoomFactor(double aZoomFactor);
  /*! Get the Zoom (%) of B-mode ultrasound; valid range: 0-100 */
  PlusStatus GetZoomFactor(double& aZoomFactor) const;
  double GetZoomFactor() const;

  /*! Set the Sector (%) of B-mode ultrasound; valid range: 0-100 */
  PlusStatus SetSectorPercent(double aSectorPercent);
  /*! Get the Sector (%) of B-mode ultrasound; valid range: 0-100 */
  PlusStatus GetSectorPercent(double& aSectorPercent) const;
  double GetSectorPercent() const;

  /*! Set the Sector (%) of B-mode ultrasound; valid range: 0-100 */
  PlusStatus SetSoundVelocity(float aSoundVelocity);
  /*! Get the Sector (%) of B-mode ultrasound; valid range: 0-100 */
  PlusStatus GetSoundVelocity(float& aSoundVelocity) const;
  float GetSoundVelocity() const;

  /*! Set the voltage of ultrasound probe (depends on an ultrasound system)*/
  PlusStatus SetProbeVoltage(float aVoltage);
  /*! Get the voltage of ultrasound probe (depends on an ultrasound system)*/
  PlusStatus GetProbeVoltage(float& aVoltage) const;
  float GetProbeVoltage() const;

  /*! Set the image size [width, height, depth(elevational dimension)] of the B-mode ultrasound */
  PlusStatus SetImageSize(const std::vector<int>& imageSize);
  PlusStatus SetImageSize(int* imageSize, int length);
  PlusStatus SetImageSize(int x, int y, int z);
  /*! Get the image size of B-mode ultrasound */
  PlusStatus GetImageSize(std::vector<int>& imageSize) const;
  std::vector<int> GetImageSize() const;

  /*! Print the list of supported parameters. For diagnostic purposes only. */
  virtual void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

  enum ImagingMode
  {
    BMode                 = 0,
    MMode                 = 1,
    ColourMode            = 2,
    PwMode                = 3,
    TriplexMode           = 4,
    PanoMode              = 5,
    DualMode              = 6,
    QuadMode              = 7,
    CompoundMode          = 8,
    DualColourMode        = 9,
    DualCompoundMode      = 10,
    CwMode                = 11,
    RfMode                = 12,
    ColorSplitMode        = 13,
    F4DMode               = 14,
    TriplexCwMode         = 15,
    ColourMMode           = 16,
    ElastoMode            = 17,
    SDUVMode              = 18,
    AnatomicalMMode       = 19,
    ElastoComparativeMode = 20,
    FusionMode            = 21,
    VecDopMode            = 22,
    BiplaneMode           = 23,
    ClinicalRfMode        = 24,
    RfCompoundMode        = 25,
    SHINEMode             = 26,
    ColourRfMode          = 27
  };

  enum DataType
  {
    DataTypeScreen                = 0x00000001,
    DataTypeBPre                  = 0x00000002,
    DataTypeBPost                 = 0x00000004,
    DataTypeBPost32               = 0x00000008,
    DataTypeRF                    = 0x00000010,
    DataTypeMPre                  = 0x00000020,
    DataTypeMPost                 = 0x00000040,
    DataTypePWRF                  = 0x00000080,
    DataTypePWSpectrum            = 0x00000100,
    DataTypeColorRF               = 0x00000200,
    DataTypeColorCombined         = 0x00000400,
    DataTypeColorVelocityVariance = 0x00000800,
    DataTypeContrast              = 0x00001000,
    DataTypeElastoCombined        = 0x00002000,
    DataTypeElastoOverlay         = 0x00004000,
    DataTypeElastoPre             = 0x00008000,
    DataTypeECG                   = 0x00010000,
    DataTypeGPS1                  = 0x00020000,
    DataTypeGPS2                  = 0x00040000,
    DataTypeTimeStamp             = 0x00080000,
    DataTypeColorSpectrumRF       = 0x00100000,
    DataTypeMRF                   = 0x00200000,
    DataTypeDAQRF                 = 0x00400000,
    DataType3DPre                 = 0x00800000,
    DataType3DPost                = 0x01000000,
    DataTypePNG                   = 0x10000000
  };

protected:
  vtkPlusUsImagingParameters();
  virtual ~vtkPlusUsImagingParameters();

  ParameterMap Parameters;
};

#endif