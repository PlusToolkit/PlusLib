/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkBkProFocusOemVideoSource_h
#define __vtkBkProFocusOemVideoSource_h

#include "vtkPlusDevice.h"

/*!
  \class vtkBkProFocusOemVideoSource 
  \brief Class for acquiring ultrasound images from BK ultrasound systems through the OEM interface
  \ingroup PlusLibDataCollection
*/
class VTK_EXPORT vtkBkProFocusOemVideoSource : public vtkPlusDevice
{
public:
  static vtkBkProFocusOemVideoSource *New();
  vtkTypeRevisionMacro(vtkBkProFocusOemVideoSource,vtkPlusDevice);
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
  vtkBkProFocusOemVideoSource();
  /*! Destructor */
  ~vtkBkProFocusOemVideoSource();

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
  vtkBkProFocusOemVideoSource(const vtkBkProFocusOemVideoSource&);  // Not implemented.
  void operator=(const vtkBkProFocusOemVideoSource&);  // Not implemented.
};

#endif
