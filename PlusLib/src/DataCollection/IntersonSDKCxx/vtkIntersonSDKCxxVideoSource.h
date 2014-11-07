/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkIntersonSDKCxxVideoSource_h
#define __vtkIntersonSDKCxxVideoSource_h

#include "vtkDataCollectionExport.h"

#include "vtkPlusDevice.h"
#include "vtkUSImagingParameters.h"

class vtkDataCollectionExport vtkIntersonSDKCxxVideoSource;

/*!
  \class vtkIntersonSDKCxxVideoSource 
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
class vtkDataCollectionExport vtkIntersonSDKCxxVideoSource : public vtkPlusDevice
{
public:
  static vtkIntersonSDKCxxVideoSource *New();
  vtkTypeMacro(vtkIntersonSDKCxxVideoSource,vtkPlusDevice);
  void PrintSelf(ostream& os, vtkIndent indent);   

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
  vtkIntersonSDKCxxVideoSource();
  /*! Destructor */
  ~vtkIntersonSDKCxxVideoSource();

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

  vtkUsImagingParameters* ImagingParameters;

private:
  vtkIntersonSDKCxxVideoSource(const vtkIntersonSDKCxxVideoSource&);  // Not implemented.
  void operator=(const vtkIntersonSDKCxxVideoSource&);  // Not implemented.

  unsigned char PulseVoltage;

  int RfDecimation;

  static void __stdcall vtkIntersonSDKCxxVideoSource::NewBmodeImageCallback( BmodePixelType * buffer, void * clientData );
  static void __stdcall vtkIntersonSDKCxxVideoSource::NewRfImageCallback( RfPixelType * buffer, void * clientData );
  PlusStatus AddBmodeFrameToBuffer( BmodePixelType * buffer, void * clientData );
  PlusStatus AddRfFrameToBuffer( RfPixelType * buffer, void * clientData );
};

#endif
