/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkSequenceIOCommon_h
#define __vtkSequenceIOCommon_h

#include "PlusCommon.h"
#include "PlusVideoFrame.h"
#include "vtkPlusCommonExport.h"
#include "vtkSequenceIOBase.h"

class vtkTrackedFrameList;

/*!
  \class vtkSequenceIOCommon
  \brief Class to abstract away specific sequence file read/write details
  \ingroup PlusLibCommon
*/
class vtkPlusCommonExport vtkSequenceIOCommon : public vtkObject
{
public:
  /*! Write object contents into file */
  static PlusStatus Write(const std::string& filename, vtkTrackedFrameList* frameList, US_IMAGE_ORIENTATION orientationInFile=US_IMG_ORIENT_MF, bool useCompression=true, bool EnableImageDataWrite=true);

  /*! Read file contents into the object */
  static PlusStatus Read(const std::string& filename, vtkTrackedFrameList* frameList);

  /*! Create a handler for a given filetype */
  static vtkSequenceIOBase* CreateSequenceHandlerForFile(const std::string& filename);

protected:
  vtkSequenceIOCommon();
  virtual ~vtkSequenceIOCommon();
};

#endif // __vtkSequenceIOCommon_h 
