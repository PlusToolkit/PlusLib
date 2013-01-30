/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#include "PlusConfigure.h"
#include "vtkObjectFactory.h"
#include "vtkSingleWallCalibration.h"

//------------------------------------------------------------------------

vtkCxxRevisionMacro(vtkSingleWallCalibration, "$Revision: 1.0$");
vtkStandardNewMacro(vtkSingleWallCalibration);

//------------------------------------------------------------------------

vtkSingleWallCalibration::vtkSingleWallCalibration()
{
}

//------------------------------------------------------------------------

vtkSingleWallCalibration::~vtkSingleWallCalibration()
{
}

//----------------------------------------------------------------------------

void vtkSingleWallCalibration::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);

  //os << indent << "Connected: " << (this->Connected ? "Yes\n" : "No\n");
}