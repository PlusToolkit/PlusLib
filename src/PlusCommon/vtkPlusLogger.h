/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusLogger_h
#define __vtkPlusLogger_h

// IGSIO includes
#include <vtkIGSIOLogger.h>

// PlusCommon includes
#include "vtkPlusCommonExport.h"

/*!
  \class vtkPlusLogger
  \brief Class to abstract away specific sequence file read/write details
  \ingroup PlusLibCommon
*/
class vtkPlusCommonExport vtkPlusLogger : public vtkIGSIOLogger
{
public:
  static vtkIGSIOLogger* Instance();

private:
  vtkPlusLogger();
  ~vtkPlusLogger();
};

#endif // __vtkPlusLogger_h 
