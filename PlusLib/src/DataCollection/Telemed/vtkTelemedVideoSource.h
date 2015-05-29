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

/*********** PARAMETERS *************/

  /*! Set the Depth value, in millimeters */
  void SetDepthMm(long DepthMm);
  /*! Set the Gain value, in percent */
  void SetGainPercent(int GainPerCent);
  /*! Set the Power value, in percent */
  void SetPowerPercent(int PowerPerCent);
  /*! Set the Dynamic Range value, in dB. Supported values are : 38, 44, 50, 56, 62, 68, 74, 80 */
  void SetDynRangeDb(int DynRangeValue);
  /*! Set the frequency in megahertz */
  void SetFrequencyMhz(int FrequencyMhz);

  /*! Verify the device is correctly configured */
  virtual PlusStatus NotifyConfigured();

  virtual std::string GetSdkVersion();

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
  int FrequencyMhz;
  int DynRangeValue;
  int PowerPerCent;
  int GainPerCent;
  long DepthMm;

  PlusVideoFrame UncompressedVideoFrame;

private:

  vtkImageImport* importer;


};

#endif // __vtkTelemedVideoSource_h
