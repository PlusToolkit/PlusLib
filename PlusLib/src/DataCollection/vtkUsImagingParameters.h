/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkUsImagingParameters_h
#define __vtkUsImagingParameters_h

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

class VTK_EXPORT vtkUsImagingParameters : public vtkObject
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
  PlusStatus SetGainPercent(double aGainPercent);
  /*! Get the Gain (%) of B-mode ultrasound; valid range: 0-100 */
  PlusStatus GetGainPercent(double& aGainPercent);

  /*! Set the DynRange (dB) of B-mode ultrasound */
  PlusStatus SetDynRangedB(double aDynRangeDb);
  /*! Get the DynRange (dB) of B-mode ultrasound */
  PlusStatus GetDynRangedB(double& aDynRangeDb);

  /*! Set the Zoom (%) of B-mode ultrasound; valid range: 0-100 */
  PlusStatus SetZoomFactor(double aZoomFactor);
  /*! Get the Zoom (%) of B-mode ultrasound; valid range: 0-100 */
  PlusStatus GetZoomFactor(double& aZoomFactor);

  /*! Set the Sector (%) of B-mode ultrasound; valid range: 0-100 */
  PlusStatus SetSectorPercent(double aSectorPercent);
  /*! Get the Sector (%) of B-mode ultrasound; valid range: 0-100 */
  PlusStatus GetSectorPercent(double& aSectorPercent);

  /*! Get the displayed frame rate. */
  PlusStatus GetDisplayedFrameRate(double &aFrameRate);

  /*! Print the list of supported parameters. For diagnostic purposes only. */
  PlusStatus PrintListOfImagingParameters();

protected:

  vtkPlusDevice *ImagingDevice; 
  double FrequencyMhz;
  double DepthMm;
  double SectorPercent; 
  double GainPercent; 
  double DynRangedB; 
  double ZoomFactor; 
  enum ImagingMode;
  enum OutputFormat;
  double SoundVelocity;

};

#endif