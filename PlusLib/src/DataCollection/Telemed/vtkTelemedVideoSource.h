/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkTelemedVideoSource_h
#define __vtkTelemedVideoSource_h

#include "vtkDataCollectionExport.h"

#include "TelemedUltrasound.h"
#include "vtkPlusDevice.h"

class vtkImageImport;
class vtkUsImagingParameters;

/*!
  \class vtkTelemedVideoSource
  \brief Class for interfacing the device manager class,
    and the PLUS library. The devices are Telemed ultrasound systems

  Requires the PLUS_USE_TELEMED option in CMake.
  Requires Telemed Usgfw2 SDK (SDK provided by Telemed).

  \ingroup PlusLibDataCollection
*/

class vtkDataCollectionExport vtkTelemedVideoSource : public vtkPlusDevice
{
public:

  static vtkTelemedVideoSource *New();
  vtkTypeMacro(vtkTelemedVideoSource,vtkPlusDevice);
  void PrintSelf(ostream& os, vtkIndent indent);


  /*! Read configuration from xml data */
  PlusStatus ReadConfiguration(vtkXMLDataElement* config);
  /*! Write configuration to xml data */
  PlusStatus WriteConfiguration(vtkXMLDataElement* config);

  /*! Manage device frozen state */
  PlusStatus FreezeDevice(bool freeze);

  /*! Is this device a tracker */
  bool IsTracker() const {return false;}

  /*! Get an update from the tracking system and push the new transforms to the tools. This function is called by the tracker thread.*/
  PlusStatus InternalUpdate();

  /*! Set ultrasound transmitter frequency (MHz) */
  PlusStatus SetFrequencyMhz(double aFrequencyMhz);
  /*! Get ultrasound transmitter frequency (MHz) */
  PlusStatus GetFrequencyMhz(double& aFrequencyMhz);

  /*! Set the imaging depth of B-mode ultrasound (mm) */
  PlusStatus SetDepthMm(double aDepthMm);
  /*! Get the imaging depth of B-mode ultrasound (mm) */
  PlusStatus GetDepthMm(double& aDepthMm);

  /*! Set the gain percentage of B-mode ultrasound (valid range: 0-100) */
  PlusStatus SetGainPercent(double aGainPercent);
  /*! Get the gain percentage of B-mode ultrasound (valid range: 0-100) */
  PlusStatus GetGainPercent(double& aGainPercent);

  /*! Set the dynamic range of B-mode ultrasound (dB)*/
  PlusStatus SetDynRangeDb(double aDynamicRange);
  /*! Get the dynamic range of B-mode ultrasound (dB)*/
  PlusStatus GetDynRangeDb(double& aDynamicRange);

  /*! Set the imaging power of B-mode ultrasound as percentage of maximum power (valid range: 0-100) */
  PlusStatus SetPowerPercent(double aGainPercent);
  /*! Get the imaging power of B-mode ultrasound as percentage of maximum power (valid range: 0-100) */
  PlusStatus GetPowerPercent(double& aGainPercent);

  /*! Verify the device is correctly configured */
  virtual PlusStatus NotifyConfigured();

  virtual std::string GetSdkVersion();

    /*! Request a particular frame size */
  virtual PlusStatus SetFrameSize(int frameSize[2]);

protected:

  /*! Constructor */
  vtkTelemedVideoSource();
  /*! Destructor */
  ~vtkTelemedVideoSource();


  /*! Device-specific connect */
  PlusStatus InternalConnect();
  /*! Device-specific disconnect */
  PlusStatus InternalDisconnect();

  TelemedUltrasound *Device;
  bool ConnectedToDevice;

  PlusVideoFrame UncompressedVideoFrame;

  int FrameSize[3];

  double FrequencyMhz;
  double DepthMm;
  double GainPercent;
  double DynRangeDb;
  double PowerPercent;

private:

  vtkImageImport* importer;


};

#endif // __vtkTelemedVideoSource_h
