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
  vtkTypeRevisionMacro(vtkTelemedVideoSource,vtkPlusDevice);
  void PrintSelf(ostream& os, vtkIndent indent);


  /*! Read configuration from xml data */
  PlusStatus ReadConfiguration(vtkXMLDataElement* config);
  /*! Write configuration to xml data */
  PlusStatus WriteConfiguration(vtkXMLDataElement* config);

  /*! Connect to device. Connection is needed for recording or single frame acquisition */
  PlusStatus Connect();

  /*!  Disconnect from device.
  This method must be called before application exit, or else the
  application might hang during exit.  */
  PlusStatus Disconnect();

  /*! Manage device frozen state */
  PlusStatus FreezeDevice(bool freeze);

  /*! Is this device a tracker */
  bool IsTracker() const {return false;}

  /*! Get an update from the tracking system and push the new transforms to the tools. This function is called by the tracker thread.*/
  PlusStatus InternalUpdate();

/*********** PARAMETERS *************/

  /*! Set the Gain value, in percent*/
  void SetGainValue(int GainPerCent);
  /*! Set the Power value, in percent */
  void SetPowerValue(int PowerPerCent);
  /*! Set the Dynamic Range value, in dB. Supported values are : 38, 44, 50, 56, 62, 68, 74, 80*/
  void SetDynRangeValue(int DynRangeValue);

protected:

  /*! Constructor */
  vtkTelemedVideoSource();
  /*! Destructor */
  ~vtkTelemedVideoSource();


  /*! Device-specific connect */
  PlusStatus InternalConnect();
  /*! Device-specific disconnect */
  PlusStatus InternalDisconnect();

  TelemedUltrasound *device;

private:

  vtkImageImport* importer;


};

#endif // __vtkTelemedVideoSource_h
