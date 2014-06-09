/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkIntersonSDKCxxVideoSource_h
#define __vtkIntersonSDKCxxVideoSource_h

#include "vtkPlusDevice.h"
#include "vtkUSImagingParameters.h"

class VTK_EXPORT vtkIntersonSDKCxxVideoSource;

/*!
  \class vtkIntersonSDKCxxVideoSource 
  \brief Class for acquiring ultrasound images from Interson USB ultrasound systems
  with C++ Wrapped SDK.

  Requires the PLUS_USE_INTERSONSDKCXX option in CMake.
  Requires Interson SDK 1.X and the C++ Wrappers (SDK provided by Interson,
  Wrappers provided by Kitware).

  \ingroup PlusLibDataCollection
*/
class VTK_EXPORT vtkIntersonSDKCxxVideoSource : public vtkPlusDevice
{
public:
  static vtkIntersonSDKCxxVideoSource *New();
  vtkTypeRevisionMacro(vtkIntersonSDKCxxVideoSource,vtkPlusDevice);
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

  /* Set the probe depth in mm */
  PlusStatus SetDepthMm(double depthMm);

  /* Set the gain in percent */
  PlusStatus SetDynRangeDb(double dynRangeDb);

  typedef unsigned char  BmodePixelType;

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

  static void __stdcall vtkIntersonSDKCxxVideoSource::NewBmodeImageCallback( BmodePixelType * buffer, void * clientData );
  PlusStatus AddBmodeFrameToBuffer( BmodePixelType * buffer, void * clientData );
};

#endif
