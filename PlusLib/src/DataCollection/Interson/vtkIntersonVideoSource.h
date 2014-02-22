/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkIntersonVideoSource_h
#define __vtkIntersonVideoSource_h

#include "vtkPlusDevice.h"

/*!
  \class vtkIntersonVideoSource 
  \brief Class for acquiring ultrasound images from Interson USB ultrasound systems

  Requires the PLUS_USE_INTERSON option in CMake.
  Requires Interson iSDK2012 (SDK provided by Interson).

  \ingroup PlusLibDataCollection
*/
class VTK_EXPORT vtkIntersonVideoSource : public vtkPlusDevice
{
public:
  static vtkIntersonVideoSource *New();
  vtkTypeRevisionMacro(vtkIntersonVideoSource,vtkPlusDevice);
  void PrintSelf(ostream& os, vtkIndent indent);   

  virtual bool IsTracker() const { return false; }

  /*! Read configuration from xml data */  
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* config); 
  /*! Write configuration to xml data */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* config);    

  /*! Verify the device is correctly configured */
  virtual PlusStatus NotifyConfigured();

  virtual std::string GetSdkVersion();

protected:
  /*! Constructor */
  vtkIntersonVideoSource();
  /*! Destructor */
  ~vtkIntersonVideoSource();

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

  PlusStatus GetFullIniFilePath(std::string &fullPath);

  PlusStatus DecodePngImage(unsigned char* pngBuffer, unsigned int pngBufferSize, vtkImageData* decodedImage);

  PlusStatus Freeze(bool freeze);

  PlusStatus WaitForFrame();

  // For internal storage of additional variables (to minimize the number of included headers)
  class vtkInternal;
  vtkInternal* Internal;

  bool Interpolate;
  bool BidirectionalScan;
  bool Frozen;

private:
  vtkIntersonVideoSource(const vtkIntersonVideoSource&);  // Not implemented.
  void operator=(const vtkIntersonVideoSource&);  // Not implemented.
};

#endif
