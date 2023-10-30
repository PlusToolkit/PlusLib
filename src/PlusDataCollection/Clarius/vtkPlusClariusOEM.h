/*=Plus=header=begin======================================================
    Program: Plus
    Copyright (c) Verdure Imaging Inc, Stockton, California. All rights reserved.
    See License.txt for details.

    We would like to acknowledge Verdure Imaging Inc for generously open-sourcing
    this support for the Clarius OEM interface to the PLUS & Slicer communities.
=========================================================Plus=header=end*/

#ifndef _VTKPLUSCLARIUSOEM_H
#define _VTKPLUSCLARIUSOEM_H

// Local includes
#include "vtkPlusDataCollectionExport.h"
#include "vtkPlusUsDevice.h"

// STL includes
#include <vector>

/*!
\class vtkPlusClariusOEM
\brief Interface to Clarius Ultrasound Devices
This class talks with a Clarius US Scanner over the Clarius OEM API.
Requires the PLUS_USE_CLARIUS_OEM option in CMake.
 \ingroup PlusLibDataCollection
*/
class vtkPlusDataCollectionExport vtkPlusClariusOEM : public vtkPlusUsDevice
{
public:
  vtkTypeMacro(vtkPlusClariusOEM, vtkPlusUsDevice);
  static vtkPlusClariusOEM* New();
  void PrintSelf(ostream& os, vtkIndent indent) override;

  /*! Probe to see to see if the device is connected to the
  computer. This method should be overridden in subclasses. */
  PlusStatus Probe() override;

  /*! PLUS device info methods */
  std::string GetSdkVersion() override;
  bool IsTracker() const override { return true; };
  bool IsVirtual() const override { return false; };

  static const std::string OVERLAY_PORT_NAME;

public:

  /*! Read configuration from xml data */
  PlusStatus ReadConfiguration(vtkXMLDataElement* config) override;

  /*! Write configuration to xml data */
  PlusStatus WriteConfiguration(vtkXMLDataElement* config) override;

  /*! Perform any completion tasks once configured
   a multi-purpose function which is called after all devices have been configured,
   all inputs and outputs have been connected between devices,
   but before devices begin collecting data.
   This is the last chance for your device to raise an error about improper or insufficient configuration.
  */
  PlusStatus NotifyConfigured() override;

  /*! return the singleton instance with no reference counting */
  static vtkPlusClariusOEM* GetInstance();

  PlusStatus InternalUpdate() override;

  PlusStatus UpdateProbeStatus();

protected:
  vtkPlusClariusOEM();
  ~vtkPlusClariusOEM();

protected:

  /*! Connect to the Clarius bluetooth low energy network */
  PlusStatus InitializeBLE();

  /*! Power on the probe, and wait for the wifi info to be available */
  PlusStatus InitializeProbe();

  /*! Connect to the Clarius probe's access point Wi-Fi network */
  PlusStatus InitializeWifi();

  /*! Setup the Clarius OEM library */
  PlusStatus InitializeOEM();

  /*! Send certificate file required for probe use */
  PlusStatus SetClariusCert();

  /*! Connect and Configure the Clarius probe application */
  PlusStatus ConfigureProbeApplication();

  /*! Initialize Clarius US parameters */
  PlusStatus SetInitialUsParams();

  /*! Overridden PLUS method to manage probe power on, setup, and connection */
  PlusStatus InternalConnect() override;

  /*! Disconnect from Clarius OEM library */
  void DeInitializeOEM();

  /*! Disconnect from the Clarius probe's Wi-Fi network */
  void DeInitializeWifi();

  /*! Power off the Clarius probe */
  void DeInitializeProbe();

  /*! Disconnect from the Clarius probe's BLE network */
  void DeInitializeBLE();

  /*! Overriden PLUS method to manage probe dis-connection, tear down, and power off */
  PlusStatus InternalDisconnect() override;

  /*! Start acquistion of Clarius US frames */
  PlusStatus InternalStartRecording() override;

  /*! Stop acquisition of Clarius US frames */
  PlusStatus InternalStopRecording() override;

  /*! Manage changes to the Clarius probe US parameters */
  PlusStatus InternalApplyImagingParameterChange() override;

public:

  // US parameters API

  /*! Get the imaging depth of B-mode ultrasound (mm) */
  PlusStatus GetDepthMm(double& aDepthMm);
  /*! Set the imaging depth of B-mode ultrasound (mm) */
  PlusStatus SetDepthMm(double aDepthMm);

  /*! Get the gain percentage of B-mode ultrasound (%) */
  PlusStatus GetGainPercent(double& aGainPercent);
  /*! Set the gain percentage of B-mode ultrasound (%) */
  PlusStatus SetGainPercent(double aGainPercent);

  /*! Get the dynamic range of B-mode ultrasound (%)*/
  PlusStatus GetDynRangePercent(double& aDynamicRangePercent);
  /*! Set the dynamic range of B-mode ultrasound (%)*/
  PlusStatus SetDynRangePercent(double aDynamicRangePercent);

  /*! Get the time gain compensation in (%) */
  PlusStatus GetTimeGainCompensationDb(std::vector<double>& aTGC);
  /*! Set the time gain compensation (%) */
  PlusStatus SetTimeGainCompensationDb(const std::vector<double>& aTGC);

  /*! Set the auto focus enabled */
  PlusStatus SetEnableAutoFocus(bool aEnableAutoFocus);
  /*! Get if the auto focus is enabled */
  PlusStatus GetEnableAutoFocus(bool& aEnableAutoFocus);

  /*! Set the auto gain enabled */
  PlusStatus SetEnableAutoGain(bool aEnableAutoGain);
  /*! Get if the auto gain is enabled */
  PlusStatus GetEnableAutoGain(bool& aEnableAutoGain);

  /*! Set the penetration mode enabled */
  PlusStatus SetEnablePenetrationMode(bool aEnablePenetrationMode);
  /*! Get if the penetration mode is enabled */
  PlusStatus GetEnablePenetrationMode(bool& aEnablePenetrationMode);

  /*! Get the focus depth of B-mode ultrasound (%) */
  PlusStatus GetFocusDepthPercent(double& aFocusDepthPercent);
  /*! Set the focus depth of B-mode ultrasound (%) */
  PlusStatus SetFocusDepthPercent(double aFocusDepthPercent);

  /*! Convert the depth in cm to a percent of the current depth */
  double ConvertDepthCmToPercent(double aFocusDepthMm);
  /*! Convert the depth in % of the current depth to cm */
  double ConvertDepthPercentToCm(double aFocusDepthPercent);

  PlusStatus UpdateFrameSize();

private:

  static vtkPlusClariusOEM* instance;

  class vtkInternal;
  vtkInternal* Internal;
};

#endif
