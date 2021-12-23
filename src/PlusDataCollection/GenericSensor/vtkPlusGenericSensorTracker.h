/*=Plus=header=begin======================================================
  Progra  : Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusGenericSensorTracker_h
#define __vtkPlusGenericSensorTracker_h

#include "vtkPlusDataCollectionExport.h"
#include "vtkPlusDevice.h"

/*!
  \class vtkPlusGenericSensorTracker
  \brief Interface class to collect sensor data in a generic way
  \ingroup PlusLibDataCollection

  For now, the following sensor types are supported:
  - Accelerometer
*/
class vtkPlusDataCollectionExport vtkPlusGenericSensorTracker : public vtkPlusDevice
{
public:

  static vtkPlusGenericSensorTracker* New();

  vtkTypeMacro(vtkPlusGenericSensorTracker, vtkPlusDevice);
  void PrintSelf(ostream& os, vtkIndent indent) override;
  PlusStatus ReadConfiguration(vtkXMLDataElement* config) override;

  bool IsTracker() const
  {
    return true;
  }

  PlusStatus InternalConnect() override;
  PlusStatus InternalDisconnect() override;
  PlusStatus Probe() override;
  PlusStatus InternalUpdate() override;

protected:
  vtkPlusGenericSensorTracker();
  ~vtkPlusGenericSensorTracker();

private:
  vtkPlusGenericSensorTracker(const vtkPlusGenericSensorTracker&);
  void operator=(const vtkPlusGenericSensorTracker&);

  class vtkInternal;
  vtkInternal* Internal;
};

#endif