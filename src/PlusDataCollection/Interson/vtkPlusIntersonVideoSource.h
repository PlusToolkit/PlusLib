/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusIntersonVideoSource_h
#define __vtkPlusIntersonVideoSource_h

#include "vtkPlusDataCollectionExport.h"
#include "vtkPlusUsDevice.h"

/*!
  \class vtkPlusIntersonVideoSource
  \brief Class for acquiring ultrasound images from Interson USB ultrasound systems

  Requires the PLUS_USE_INTERSON option in CMake.
  Requires Interson iSDK2012 (SDK provided by Interson).

  \ingroup PlusLibDataCollection
*/
class vtkPlusDataCollectionExport vtkPlusIntersonVideoSource : public vtkPlusUsDevice
{
public:
  static vtkPlusIntersonVideoSource* New();
  vtkTypeMacro(vtkPlusIntersonVideoSource, vtkPlusUsDevice);
  virtual void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

  virtual bool IsTracker() const { return false; }

  /*! Read configuration from xml data */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* config);
  /*! Write configuration to xml data */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* config);

  /*! Verify the device is correctly configured */
  virtual PlusStatus NotifyConfigured();

  virtual std::string GetSdkVersion();

  /*! Get the probe sound velocity */
  PlusStatus GetSoundVelocity(double& aVel);

  /*! Set the probe depth in mm */
  PlusStatus SetDepthMm(double depthMm);
  /*! Set the frequency in Mhz */
  PlusStatus SetFrequencyMhz(float freq);
  /*! Set the sector percent */
  PlusStatus SetSectorPercent(double value);
  /*! Set the intensity */
  PlusStatus SetIntensity(int value);
  /*! Set the contrast */
  PlusStatus SetContrast(int value);
  /*! Set the dynamic range */
  PlusStatus SetDynRangeDb(double value);
  /*! Set the sound velocity */
  PlusStatus SetSoundVelocity(double value);
  /*! Set the gain in percent */
  PlusStatus SetTimeGainCompensationPercent(double gainPercent[3]);
  /*! Set the zoom factor. */
  PlusStatus SetZoomFactor(float zoomFactor);
  /*! Set the image size */
  PlusStatus SetImageSize(int imageSize[2]);

  /*! Set the parameters in bulk */
  virtual PlusStatus SetNewImagingParameters(const vtkPlusUsImagingParameters& newImagingParameters);

  virtual PlusStatus InternalApplyImagingParameterChange();

  bool GetEnableProbeButtonMonitoring() const;
  void SetEnableProbeButtonMonitoring(bool _arg);

protected:
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

  PlusStatus GetFullIniFilePath(std::string& fullPath);

  PlusStatus FreezeDevice(bool freeze);

  PlusStatus WaitForFrame();

  PlusStatus GetSampleFrequencyDevice(double& aFreq);
  PlusStatus GetSoundVelocityDevice(float& aVel);

  /*! Represents the depth, in pixels, the display window will be. This defaults to 512 pixels for newly initialized probes.*/
  PlusStatus SetWindowDepthDevice(int height);
  /*! Set the probe depth in mm */
  PlusStatus SetDepthMmDevice(double depthMm);
  /*! Set the desired probe frequency in Hz. The resulting probe speed will be approximately the value specified */
  PlusStatus SetFrequencyMhzDevice(float aFreq);
  /*! Set the sound velocity in the device */
  PlusStatus SetSoundVelocityDevice(double velocity);
  /*! Set the gain in percent in the device */
  PlusStatus SetTimeGainCompensationPercentDevice(double gainPercent[3]);
  /*! Set the zoom factor in the device. */
  PlusStatus SetZoomFactorDevice(float zoomFactor);
  /*! Set the lookup table from intensity and contrast */
  PlusStatus SetLookupTableDevice(double intensity, double contrast);

  /*! Each probe has a defined set of allowed modes.
  These modes are combinations of pulse frequency and sample rate that yield acceptable results
  with that particular probe. While there is no enforcement to use only these modes, one should
  understand the implications on image quality if one of the allowed modes is not selected.*/
  PlusStatus GetProbeAllowedModes(std::vector< std::pair<double, double> >& allowedModes);

  /*! Get probe name from the device */
  PlusStatus GetProbeNameDevice(std::string& probeName);

protected:
  /*! Constructor */
  vtkPlusIntersonVideoSource();
  /*! Destructor */
  ~vtkPlusIntersonVideoSource();

  class vtkInternal;
  vtkInternal* Internal;
private:
  vtkPlusIntersonVideoSource(const vtkPlusIntersonVideoSource&);  // Not implemented.
  void operator=(const vtkPlusIntersonVideoSource&);  // Not implemented.
};

#endif