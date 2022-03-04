/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#include "vtkPlusPivotCalibrationAlgo.h"

vtkStandardNewMacro(vtkPlusPivotCalibrationAlgo);

//-----------------------------------------------------------------------------
vtkPlusPivotCalibrationAlgo::vtkPlusPivotCalibrationAlgo()
{
}

//-----------------------------------------------------------------------------
vtkPlusPivotCalibrationAlgo::~vtkPlusPivotCalibrationAlgo()
{
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusPivotCalibrationAlgo::ReadConfiguration(vtkXMLDataElement* aConfig)
{
  XML_FIND_NESTED_ELEMENT_REQUIRED(pivotCalibrationElement, aConfig, "vtkPlusPivotCalibrationAlgo");
  XML_READ_CSTRING_ATTRIBUTE_REQUIRED(ObjectMarkerCoordinateFrame, pivotCalibrationElement);
  XML_READ_CSTRING_ATTRIBUTE_REQUIRED(ReferenceCoordinateFrame, pivotCalibrationElement);
  XML_READ_CSTRING_ATTRIBUTE_REQUIRED(ObjectPivotPointCoordinateFrame, pivotCalibrationElement);
  return PLUS_SUCCESS;
}
