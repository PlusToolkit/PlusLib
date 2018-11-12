/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusSequenceIO_h
#define __vtkPlusSequenceIO_h

#include "PlusCommon.h"
#include "PlusVideoFrame.h"
#include "vtkPlusCommonExport.h"
#include "vtkPlusSequenceIOBase.h"

class vtkPlusTrackedFrameList;

/*!
  \class vtkPlusSequenceIO
  \brief Class to abstract away specific sequence file read/write details
  \ingroup PlusLibCommon
*/
class vtkPlusCommonExport vtkPlusSequenceIO : public vtkObject
{
public:
  /*! Write object contents into file */
  static PlusStatus Write(const std::string& filename, PlusTrackedFrame* frame, US_IMAGE_ORIENTATION orientationInFile = US_IMG_ORIENT_MF, bool useCompression = true, bool EnableImageDataWrite = true);

  /*! Write object contents into file */
  static PlusStatus Write(const std::string& filename, vtkPlusTrackedFrameList* frameList, US_IMAGE_ORIENTATION orientationInFile = US_IMG_ORIENT_MF, bool useCompression = true, bool EnableImageDataWrite = true);

  /*! Read file contents into the object */
  static PlusStatus Read(const std::string& filename, vtkPlusTrackedFrameList* frameList);

  /*! Create a handler for a given filetype */
  static vtkPlusSequenceIOBase* CreateSequenceHandlerForFile(const std::string& filename);

protected:
  vtkPlusSequenceIO();
  virtual ~vtkPlusSequenceIO();
};

#endif // __vtkPlusSequenceIO_h 
