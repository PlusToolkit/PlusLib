/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusIntersonSDKCxxVideoSource_h
#define __vtkPlusIntersonSDKCxxVideoSource_h

#include "vtkPlusDataCollectionExport.h"

#include "vtkPlusDevice.h"
#include "vtkPlusUSImagingParameters.h"

class vtkPlusDataCollectionExport vtkPlusIntersonSDKCxxVideoSource;

/*!
  \class vtkPlusIntersonSDKCxxVideoSource 
  \brief Class for acquiring ultrasound images from Interson USB ultrasound systems
  with C++ Wrapped SDK.

  Requires the PLUS_USE_INTERSONSDKCXX_VIDEO option in CMake.
  Requires Interson SDK 1.X and the C++ Wrappers (SDK provided by Interson,
  Wrappers provided by Kitware).

  Both RF and B-Mode output is supported. If there is a single B-Mode output
  channel configured, then the native B-Mode generated on the transducer is
  output utilizing Plus scan conversion.  If an RF channel is configured, then
  RF is output on the RF channel, and, if a B-Mode channel is also configured,
  the B-Mode derived from the RF using Plus envelope detection, amplitude
  compression, and scan conversion derived directly from the RF will be output
  on the B-Mode channel.

  \ingroup PlusLibDataCollection
*/
class vtkPlusDataCollectionExport vtkPlusIntersonSDKCxxVideoSource : public vtkPlusDevice
{
public:
  static vtkPlusIntersonSDKCxxVideoSource *New();
  vtkTypeMacro(vtkPlusIntersonSDKCxxVideoSource,vtkPlusDevice);
  virtual void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

  virtual bool IsTracker() const { return false; }

  /*! Read configuration from xml data */  
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* config); 
  /*! Write configuration to xml data */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* config);    

  /*! Verify the device is correctly configured */
  virtual PlusStatus NotifyConfigured();

  virtual std::string GetSdkVersion();

  /* Set the desired probe frequency in MHz. */
  PlusStatus SetProbeFrequencyMhz(double aFreq);

  /* Set the gain in percent. Used for on board B-mode generation. */
  PlusStatus SetDynRangeDb(double dynRangeDb);

  /* Set the excitation pulse voltage in volts. */
  PlusStatus SetPulseVoltage(unsigned char voltage);
  vtkGetMacro(PulseVoltage, unsigned char);

  /* Set whether the RF decimator is used.  If enabled, the sampling rate is
   * halved, but the imaged depth is doubled. */
  PlusStatus SetRfDecimation(int enableDecimation);

  typedef unsigned char  BmodePixelType;
  typedef short          RfPixelType;

protected:
  /*! Constructor */
  vtkPlusIntersonSDKCxxVideoSource();
  /*! Destructor */
  ~vtkPlusIntersonSDKCxxVideoSource();

  /*! Device-specific connect */
  virtual PlusStatus InternalConnect();

  /*! Device-specific disconnect */
  virtual PlusStatus InternalDisconnect();

  /*! Device-specific recording start */
  virtual PlusStatus InternalStartRecording();

  /*! Device-specific recording stop */
  virtual PlusStatus InternalStopRecording();

  /*! The internal function which actually does the grab.  */
  virtual PlusStatus InternalUpdate();

  // For internal storage of additional variables (to minimize the number of included headers)
  class vtkInternal;
  vtkInternal* Internal;

  vtkPlusUsImagingParameters* ImagingParameters;

  unsigned char PulseVoltage;

  int RfDecimation;

private:
  vtkPlusIntersonSDKCxxVideoSource(const vtkPlusIntersonSDKCxxVideoSource&);  // Not implemented.
  void operator=(const vtkPlusIntersonSDKCxxVideoSource&);  // Not implemented.

  static void __stdcall vtkPlusIntersonSDKCxxVideoSource::NewBmodeImageCallback( BmodePixelType * buffer, void * clientData );
  static void __stdcall vtkPlusIntersonSDKCxxVideoSource::NewRfImageCallback( RfPixelType * buffer, void * clientData );
  PlusStatus AddBmodeFrameToBuffer( BmodePixelType * buffer, void * clientData );
  PlusStatus AddRfFrameToBuffer( RfPixelType * buffer, void * clientData );
};

#endif
