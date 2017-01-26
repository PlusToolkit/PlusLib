/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

// GRAB_FRAME API was partly contributed by by Xin Kang at SZI, Children's National Medical Center

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
  virtual void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

  virtual bool IsTracker() const { return false; }

  /*! Read configuration from xml data */  
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* config); 
  /*! Write configuration to xml data */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* config);    

  /*! Verify the device is correctly configured */
  virtual PlusStatus NotifyConfigured();

  /*! Set the name of the BK ini file that stores connection and acquisition settings */
  vtkSetStringMacro(IniFileName);

  /*!
    Enable/disable continuous streaming. Continuous streaming (GRAB_FRAME command) requires extra license
    from BK but it allows faster image acquisition.
  */
  vtkSetMacro(ContinuousStreamingEnabled, bool);

  /*!
  Enable/disable continuous streaming. Continuous streaming (GRAB_FRAME command) requires extra license
  from BK but it allows faster image acquisition.
  */
  vtkGetMacro(ContinuousStreamingEnabled, bool);

  /*!
  Enable/disable continuous streaming. Continuous streaming (GRAB_FRAME command) requires extra license
  from BK but it allows faster image acquisition.
  */
  vtkBooleanMacro(ContinuousStreamingEnabled, bool);

protected:
	
  // Size of the ultrasound image. Only used if ContinuousStreamingEnabled is true.
  int UltrasoundWindowSize[2];

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

  /*! The internal function which ...  */
	PlusStatus QueryImageSize();

  PlusStatus GetFullIniFilePath(std::string &fullPath);

  PlusStatus DecodePngImage(unsigned char* pngBuffer, unsigned int pngBufferSize, vtkImageData* decodedImage);

  /*! BK ini file storing the connection and acquisition settings */
  char* IniFileName;

  bool ContinuousStreamingEnabled;

  // For internal storage of additional variables (to minimize the number of included headers)
  class vtkInternal;
  vtkInternal* Internal;

private:
  vtkPlusBkProFocusOemVideoSource(const vtkPlusBkProFocusOemVideoSource&);  // Not implemented.
  void operator=(const vtkPlusBkProFocusOemVideoSource&);  // Not implemented.
};

#endif
