/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkCapistranoVideoSource_h
#define __vtkCapistranoVideoSource_h

#include "vtkDataCollectionExport.h"

#include "vtkPlusDevice.h"

/*!
 \class vtkCapistranoVideoSource
 \brief Class for acquiring ultrasound images from Capistrano Labs USB
 ultrasound systems.

 Requires PLUS_USE_CAPISTRANO_VIDEO option in CMake.
 Requires the Capistrano cSDK2013 (SDK provided by Capistrano Labs).

 \ingroup PlusLibDataCollection.
*/
class vtkDataCollectionExport vtkCapistranoVideoSource: public vtkPlusDevice
{
public:
  static vtkCapistranoVideoSource * New();
  vtkTypeMacro(vtkCapistranoVideoSource, vtkPlusDevice);

  void PrintSelf(ostream& os, vtkIndent indent);   

protected:
  vtkCapistranoVideoSource();
  ~vtkCapistranoVideoSource();

private:
  vtkCapistranoVideoSource(const vtkCapistranoVideoSource &); // Not implemented
  void operator=(const vtkCapistranoVideoSource &); // Not implemented
};

#endif
