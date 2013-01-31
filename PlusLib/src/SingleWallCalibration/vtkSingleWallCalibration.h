/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __vtkSingleWallCalibration_h
#define __vtkSingleWallCalibration_h

#include "vtkObject.h"

/*!
  \class vtkSingleWallCalibration 

  \brief fill this in with details

  \ingroup PlusLibDataCollection
*/
class VTK_EXPORT vtkSingleWallCalibration : public vtkObject
{
public:
  static vtkSingleWallCalibration *New();
  vtkTypeRevisionMacro(vtkSingleWallCalibration, vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);

  vtkSingleWallCalibration();
  ~vtkSingleWallCalibration();

private:
  vtkSingleWallCalibration(const vtkSingleWallCalibration&);  // Not implemented.
  void operator=(const vtkSingleWallCalibration&);  // Not implemented. 
};

#endif
