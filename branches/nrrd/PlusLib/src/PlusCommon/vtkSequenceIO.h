/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkSequenceIO_h
#define __vtkSequenceIO_h

#include "PlusCommon.h"
#include "PlusVideoFrame.h"
#include "vtkPlusCommonExport.h"
#include "vtkSequenceIOBase.h"

class vtkTrackedFrameList;

/*!
  \class vtkSequenceIO
  \brief Class to abstract away specific sequence file read/write details
  \ingroup PlusLibCommon
*/
class vtkPlusCommonExport vtkSequenceIO : public vtkObject
{
public:
  /*! Write object contents into file */
  static PlusStatus Write(const std::string& filename, vtkTrackedFrameList* frameList, US_IMAGE_ORIENTATION orientationInFile=US_IMG_ORIENT_MF, bool useCompression=true, bool EnableImageDataWrite=true);

  /*! Read file contents into the object */
  static PlusStatus Read(const std::string& filename, vtkTrackedFrameList* frameList);

  /*! Create a handler for a given filetype */
  static vtkSequenceIOBase* CreateSequenceHandlerForFile(const std::string& filename);

protected:
  vtkSequenceIO();
  virtual ~vtkSequenceIO();
};

#endif // __vtkSequenceIO_h 
