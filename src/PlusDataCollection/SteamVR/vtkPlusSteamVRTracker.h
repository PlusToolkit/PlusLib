/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusSteamVR_h
#define __vtkPlusSteamVR_h

#include "vtkPlusDataCollectionExport.h"
#include "vtkPlusDevice.h"

class vtkPlusDataSource;

namespace vr
{
  class IVRSystem;
}

class vtkPlusDataCollectionExport vtkPlusSteamVRTracker : public vtkPlusDevice
{
public:
  static vtkPlusSteamVRTracker* New();
  vtkTypeMacro(vtkPlusSteamVRTracker, vtkPlusDevice);
  void PrintSelf(ostream& os, vtkIndent indent);

  virtual bool IsTracker() const { return true; }

  virtual PlusStatus Probe();

protected:
  vtkPlusSteamVRTracker();
  ~vtkPlusSteamVRTracker();

protected:
  vtkPlusSteamVRTracker(const vtkPlusSteamVRTracker&);
  void operator=(const vtkPlusSteamVRTracker&);

  /*! Called after all devices have been configured / inputs & outputs are connected / collection of data not started*/
  virtual PlusStatus NotifyConfigured();

  virtual PlusStatus InternalConnect();
  virtual PlusStatus InternalDisconnect();
  virtual PlusStatus InternalUpdate();

  vtkPlusDataSource* HMDSource;
  vtkPlusDataSource* LeftControllerSource;
  vtkPlusDataSource* RightControllerSource;

  vr::IVRSystem* VRContext;

  double SteamVRConnectionTimeout;
};

#endif
