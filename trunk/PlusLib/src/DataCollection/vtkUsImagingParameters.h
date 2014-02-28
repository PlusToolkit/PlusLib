/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkUsImagingParameters_h
#define __vtkUsImagingParameters_h

#include "vtkPlusDevice.h"


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
  PlusStatus SetFrequency(int aFrequency);
  /*! Get ultrasound transmitter frequency (MHz) */
  PlusStatus GetFrequency(int& aFrequency);

  /*! Set the depth (mm) of B-mode ultrasound */
  PlusStatus SetDepth(int aDepth);
  /*! Get the depth (mm) of B-mode ultrasound */
  PlusStatus GetDepth(int& aDepth);

  /*! Set the Gain (%) of B-mode ultrasound; valid range: 0-100 */
  PlusStatus SetGain(int aGain);
  /*! Get the Gain (%) of B-mode ultrasound; valid range: 0-100 */
  PlusStatus GetGain(int& aGain);

  /*! Set the DynRange (dB) of B-mode ultrasound */
  PlusStatus SetDynRange(int aDynRange);
  /*! Get the DynRange (dB) of B-mode ultrasound */
  PlusStatus GetDynRange(int& aDynRange);

  /*! Set the Zoom (%) of B-mode ultrasound; valid range: 0-100 */
  PlusStatus SetZoom(int aZoom);
  /*! Get the Zoom (%) of B-mode ultrasound; valid range: 0-100 */
  PlusStatus GetZoom(int& aZoom);

  /*! Set the Sector (%) of B-mode ultrasound; valid range: 0-100 */
  PlusStatus SetSector(int aSector);
  /*! Get the Sector (%) of B-mode ultrasound; valid range: 0-100 */
  PlusStatus GetSector(int& aSector);

  /*! Get the displayed frame rate. */
  PlusStatus GetDisplayedFrameRate(int &aFrameRate);

  /*! Print the list of supported parameters. For diagnostic purposes only. */
  PlusStatus PrintListOfImagingParameters();

protected:

  vtkPlusDevice *ImagingDevice; 
  int Frequency;
  int Depth;
  int Sector; 
  int Gain; 
  int DynRange; 
  int Zoom; 
  int ImagingMode;
  int OutputFormat;
  int SoundVelocity;

};

#endif