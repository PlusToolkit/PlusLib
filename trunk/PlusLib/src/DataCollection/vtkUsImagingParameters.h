/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkUsImagingParameters_h
#define __vtkUsImagingParameters_h

#include "vtkDataCollectionExport.h"

#include "vtkPlusDevice.h"

/*!
\class vtkUSImagingParameters 
\brief This class is used to configure the imaging parameters of the video devices.
Ultrasound video devices have an a attribute of this class that is used to set/query 
the depth, gain, etc.
This class exists mainly for two reasons:
	* Be sure that the Us video devices have some methods.
	* Avoid repeting code.
\ingroup PlusLibDataCollection
*/

class vtkDataCollectionExport vtkUsImagingParameters : public vtkObject
{
public:
  vtkUsImagingParameters(vtkPlusDevice*);
  virtual ~vtkUsImagingParameters();

    /*! Read main configuration from/to xml data */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* config); 

  /*! Write main configuration from/to xml data */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* config);

  /*! Set ultrasound transmitter frequency (MHz) */
  PlusStatus SetFrequencyMhz(double aFrequencyMhz);
  /*! Get ultrasound transmitter frequency (MHz) */
  PlusStatus GetFrequencyMhz(double& aFrequencyMhz);

  /*! Set the depth (mm) of B-mode ultrasound */
  PlusStatus SetDepthMm(double aDepthMm);
  /*! Get the depth (mm) of B-mode ultrasound */
  PlusStatus GetDepthMm(double& aDepthMm);

  /*! Set the Gain (%) of B-mode ultrasound; valid range: 0-100 */
  PlusStatus SetGainPercent(double aGainPercent[3]);
  /*! Get the Gain (%) of B-mode ultrasound; valid range: 0-100 */
  PlusStatus GetGainPercent(double aGainPercent[3]);

  /*! Set the intensity of B-mode ultrasound */
  PlusStatus SetIntensity(double aIntensity);
  /*! Get the Intensity of B-mode ultrasound */
  PlusStatus GetIntensity(double& aIntensity);

  /*! Set the contrast of B-mode ultrasound */
  PlusStatus SetContrast(double aContrast);
  /*! Get the contrast of B-mode ultrasound */
  PlusStatus GetContrast(double& aContrast);

  /*! Set the DynRange (dB) of B-mode ultrasound */
  PlusStatus SetDynRangeDb(double aDynRangeDb);
  /*! Get the DynRange (dB) of B-mode ultrasound */
  PlusStatus GetDynRangeDb(double& aDynRangeDb);

  /*! Set the Zoom (%) of B-mode ultrasound; valid range: 0-100 */
  PlusStatus SetZoomFactor(double aZoomFactor);
  /*! Get the Zoom (%) of B-mode ultrasound; valid range: 0-100 */
  PlusStatus GetZoomFactor(double& aZoomFactor);

  /*! Set the Sector (%) of B-mode ultrasound; valid range: 0-100 */
  PlusStatus SetSectorPercent(double aSectorPercent);
  /*! Get the Sector (%) of B-mode ultrasound; valid range: 0-100 */
  PlusStatus GetSectorPercent(double& aSectorPercent);

  /*! Set the Sector (%) of B-mode ultrasound; valid range: 0-100 */
  PlusStatus SetSoundVelocity(double aSoundVelocity);
  /*! Get the Sector (%) of B-mode ultrasound; valid range: 0-100 */
  PlusStatus GetSoundVelocity(double& aSoundVelocity);

  /*! Get the displayed frame rate. */
  PlusStatus GetDisplayedFrameRate(double &aFrameRate);

  /*! Print the list of supported parameters. For diagnostic purposes only. */
  PlusStatus PrintListOfImagingParameters();

    /*!
    Request a particular mode of imaging
    Usable values are described in ImagingModes.h (default: B-mode)  
    BMode = 0,
    MMode = 1,
    ColourMode = 2,
    PwMode = 3,
    TriplexMode = 4,
    PanoMode = 5,
    DualMode = 6,
    QuadMode = 7,
    CompoundMode = 8,
    DualColourMode = 9,
    DualCompoundMode = 10,
    CwMode = 11,
    RfMode = 12,
    ColorSplitMode = 13,
    F4DMode = 14,
    TriplexCwMode = 15,
    ColourMMode = 16,
    ElastoMode = 17,
    SDUVMode = 18,
    AnatomicalMMode = 19,
    ElastoComparativeMode = 20,
    FusionMode = 21,
    VecDopMode = 22,
    BiplaneMode = 23,
    ClinicalRfMode = 24,
    RfCompoundMode = 25,
    SHINEMode = 26,
    ColourRfMode = 27,
  */
  enum ImagingMode
  { 
	BMode = 0,
    MMode = 1,
    ColourMode = 2,
    PwMode = 3,
    TriplexMode = 4,
    PanoMode = 5,
    DualMode = 6,
    QuadMode = 7,
    CompoundMode = 8,
    DualColourMode = 9,
    DualCompoundMode = 10,
    CwMode = 11,
    RfMode = 12,
    ColorSplitMode = 13,
    F4DMode = 14,
    TriplexCwMode = 15,
    ColourMMode = 16,
    ElastoMode = 17,
    SDUVMode = 18,
    AnatomicalMMode = 19,
    ElastoComparativeMode = 20,
    FusionMode = 21,
    VecDopMode = 22,
    BiplaneMode = 23,
    ClinicalRfMode = 24,
    RfCompoundMode = 25,
    SHINEMode = 26,
    ColourRfMode = 27
  };

  enum DataType
  {
    DataTypeScreen = 0x00000001,
    DataTypeBPre = 0x00000002,
    DataTypeBPost = 0x00000004,
    DataTypeBPost32 = 0x00000008,
    DataTypeRF = 0x00000010,
    DataTypeMPre = 0x00000020,
    DataTypeMPost = 0x00000040,
    DataTypePWRF = 0x00000080,
    DataTypePWSpectrum = 0x00000100,
    DataTypeColorRF = 0x00000200,
    DataTypeColorCombined = 0x00000400,
    DataTypeColorVelocityVariance = 0x00000800,
    DataTypeContrast = 0x00001000,
    DataTypeElastoCombined = 0x00002000,
    DataTypeElastoOverlay = 0x00004000,
    DataTypeElastoPre = 0x00008000,
    DataTypeECG = 0x00010000,
    DataTypeGPS1 = 0x00020000,
    DataTypeGPS2 = 0x00040000,
    DataTypeTimeStamp = 0x00080000,
    DataTypeColorSpectrumRF = 0x00100000,
    DataTypeMRF = 0x00200000,
    DataTypeDAQRF = 0x00400000,
    DataType3DPre = 0x00800000,
    DataType3DPost = 0x01000000,
    DataTypePNG = 0x10000000
  };
protected:

  vtkPlusDevice *ImagingDevice; 
  double FrequencyMhz;
  double DepthMm;
  double SectorPercent; 
  double GainPercent[3]; 
  int Intensity;
  int Contrast;
  double DynRangeDb; 
  double ZoomFactor; 
  double SoundVelocity;

};

#endif
