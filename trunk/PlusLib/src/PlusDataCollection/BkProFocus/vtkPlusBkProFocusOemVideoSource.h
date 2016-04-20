/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusBkProFocusOemVideoSource_h
#define __vtkPlusBkProFocusOemVideoSource_h

#include "vtkPlusDataCollectionExport.h"

#include "vtkPlusDevice.h"

/*!
  \class vtkPlusBkProFocusOemVideoSource 
  \brief Class for acquiring ultrasound images from BK ultrasound systems through the OEM interface

  Requires the PLUS_USE_BKPROFOCUS_VIDEO option in CMake.
  Requires GrabbieLib (SDK provided by BK).

  \ingroup PlusLibDataCollection
*/
class vtkPlusDataCollectionExport vtkPlusBkProFocusOemVideoSource : public vtkPlusDevice
{
public:
  static vtkPlusBkProFocusOemVideoSource *New();
  vtkTypeMacro(vtkPlusBkProFocusOemVideoSource,vtkPlusDevice);
  void PrintSelf(ostream& os, vtkIndent indent);   

  virtual bool IsTracker() const { return false; }

  /*! Read configuration from xml data */  
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* config); 
  /*! Write configuration to xml data */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* config);    

  /*! Verify the device is correctly configured */
  virtual PlusStatus NotifyConfigured();

  /*! Set the name of the BK ini file that stores connection and acquisition settings */
  vtkSetStringMacro(IniFileName);

protected:
  /*! Constructor */
  vtkPlusBkProFocusOemVideoSource();
  /*! Destructor */
  ~vtkPlusBkProFocusOemVideoSource();

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

  /*! BK ini file storing the connection and acquisition settings */
  char* IniFileName;

  // For internal storage of additional variables (to minimize the number of included headers)
  class vtkInternal;
  vtkInternal* Internal;  

private:
  vtkPlusBkProFocusOemVideoSource(const vtkPlusBkProFocusOemVideoSource&);  // Not implemented.
  void operator=(const vtkPlusBkProFocusOemVideoSource&);  // Not implemented.
};

#endif
