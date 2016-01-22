/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusUsDevice_h
#define __vtkPlusUsDevice_h

#include "PlusCommon.h"
#include "PlusConfigure.h"
#include "vtkDataCollectionExport.h"
#include "vtkPlusDevice.h"
#include "vtkUSImagingParameters.h"

/*!
\class vtkPlusUsDevice 
\brief Abstract interface for ultrasound video devices

vtkPlusUsDevice is an abstract VTK interface to ultrasound imaging
systems.  Derived classes should override the SetNewImagingParametersDevice() method.

\ingroup PlusLibDataCollection
*/
class vtkDataCollectionExport vtkPlusUsDevice : public vtkPlusDevice
{
   
public:
  static vtkPlusUsDevice* New();
  vtkTypeMacro(vtkPlusUsDevice, vtkPlusDevice);
  void PrintSelf(ostream& os, vtkIndent indent);

  /*!
  Copy the new imaging parameters into the current parameter set
  It is up to subclasses to take the new imaging parameter set and apply it to their respective devices
  /param newImagingParameters class containing the new ultrasound imaging parameters
  */
  virtual PlusStatus SetNewImagingParameters(const vtkUsImagingParameters& newImagingParameters);

protected:
  vtkPlusUsDevice();
  virtual ~vtkPlusUsDevice();

  /// Store the requested imaging parameters
  vtkUsImagingParameters* RequestedImagingParameters;
  /// Store the current imaging parameters
  vtkUsImagingParameters* CurrentImagingParameters;

private:
  vtkPlusUsDevice(const vtkPlusUsDevice&);  // Not implemented.
  void operator=(const vtkPlusUsDevice&);  // Not implemented. 
};

#endif
