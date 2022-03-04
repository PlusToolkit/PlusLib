/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "vtkPlusLandmarkDetectionAlgo.h"


vtkStandardNewMacro( vtkPlusLandmarkDetectionAlgo );

//-----------------------------------------------------------------------------
vtkPlusLandmarkDetectionAlgo::vtkPlusLandmarkDetectionAlgo()
{
}

//-----------------------------------------------------------------------------
vtkPlusLandmarkDetectionAlgo::~vtkPlusLandmarkDetectionAlgo()
{
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusLandmarkDetectionAlgo::ReadConfiguration(vtkXMLDataElement* aConfig)
{
  XML_FIND_NESTED_ELEMENT_REQUIRED(PhantomLandmarkLandmarkDetectionElement, aConfig, "vtkPlusPhantomLandmarkRegistrationAlgo");
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(double, AcquisitionRate, PhantomLandmarkLandmarkDetectionElement);

  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(double, FilterWindowTimeSec, PhantomLandmarkLandmarkDetectionElement);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(double, DetectionTimeSec, PhantomLandmarkLandmarkDetectionElement);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(double, StylusShaftMinimumDisplacementThresholdMm, PhantomLandmarkLandmarkDetectionElement);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(double, StylusTipMaximumDisplacementThresholdMm, PhantomLandmarkLandmarkDetectionElement);

  unsigned int filterWindowSize = 0;
  unsigned int numberOfWindows = 0;
  if (ComputeFilterWindowSize(filterWindowSize) == IGSIO_FAIL || ComputeNumberOfWindows(numberOfWindows) == IGSIO_FAIL)
  {
    return IGSIO_FAIL;
  }

  LOG_DEBUG("AcquisitionRate = " << AcquisitionRate << "[fps] WindowTimeSec = " << FilterWindowTimeSec << "[s] DetectionTimeSec = " << DetectionTimeSec << "[s]");
  LOG_DEBUG("NumberOfWindows = " << numberOfWindows << " WindowSize = " << filterWindowSize << " MinimunDistanceBetweenLandmarksMm = " << MinimunDistanceBetweenLandmarksMm << "[mm] LandmarkThreshold " << StylusTipMaximumDisplacementThresholdMm << "[mm]");

  return PLUS_SUCCESS;
}
