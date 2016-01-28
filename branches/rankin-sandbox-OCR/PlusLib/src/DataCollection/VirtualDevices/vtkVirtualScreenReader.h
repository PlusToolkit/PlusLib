/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkVirtualScreenReader_h
#define __vtkVirtualScreenReader_h

#include "vtkDataCollectionExport.h"
#include "vtkPlusDevice.h"

namespace tesseract
{
  class TessBaseAPI;
}

/*!
\class vtkVirtualScreenReader
\brief 

\ingroup PlusLibDataCollection
*/
class vtkDataCollectionExport vtkVirtualScreenReader : public vtkPlusDevice
{
public:
  static vtkVirtualScreenReader *New();
  vtkTypeMacro(vtkVirtualScreenReader, vtkPlusDevice);
  void PrintSelf(ostream& os, vtkIndent indent);

  /*! Prepare the device for recognition */
  virtual PlusStatus InternalConnect();

  /*! Prepare the device for recognition */
  virtual PlusStatus InternalDisconnect();

  /*! Read main configuration from xml data */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement*);

  /*! Write main configuration from xml data */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement*);

  /*! Callback after configuration of all devices is complete */
  virtual PlusStatus NotifyConfigured();

  virtual bool IsTracker() const { return false; }
  virtual bool IsVirtual() const { return true; }

  vtkSetStringMacro(Language);
  vtkGetStringMacro(Language);

protected:
  virtual PlusStatus InternalUpdate();

  vtkVirtualScreenReader();
  virtual ~vtkVirtualScreenReader();

  /// Language used for detection
  char* Language;

  /// Main entry point for the tesseract API
  tesseract::TessBaseAPI* TesseractAPI;

private:
  vtkVirtualScreenReader(const vtkVirtualScreenReader&);
  void operator=(const vtkVirtualScreenReader&);
};

#endif //__vtkVirtualScreenReader_h