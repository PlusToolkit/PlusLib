/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __IEEListener_h
#define __IEEListener_h

#include "StreamMgr.h"
#include "vtkPlusDataCollectionExport.h"
#include "vtkObject.h"
#include "vtkIGSIOLogger.h"

class vtkPlusDataCollectionExport vtkPlusIEEListener : public vtkObject
{
public:
  vtkTypeMacro(vtkPlusIEEListener, vtkObject);
  virtual void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;
  static vtkPlusIEEListener* New();
  static vtkPlusIEEListener* New(bool forceZQuantize, double resolutionFactor, bool integerZ, bool isotropic, bool quantizeDim, int zDecimation, bool set4PtFIR, int latAndElevSmoothingIndex);

  void SetMachineName(const std::string& machineName);
  void SetPortNumber(unsigned int port);

  /*!
    Connect to a streaming ie33 device
    \param callback the function to call when new data
  */
  PlusStatus Connect(CLIENT_POSTSCANCONVERT_CALLBACK callback, vtkIGSIOLogger::LogLevelType logType = vtkIGSIOLogger::LOG_LEVEL_ERROR);
  /*!
    Disconnect from the ie33 device
  */
  PlusStatus Disconnect();

  /*! Query the device to see if it's connected */
  bool IsConnected();

protected:
  vtkPlusIEEListener();
  vtkPlusIEEListener(bool forceZQuantize, double resolutionFactor, bool integerZ, bool isotropic, bool quantizeDim, int zDecimation, bool set4PtFIR, int latAndElevSmoothingIndex);
  ~vtkPlusIEEListener();

protected:
  CStreamMgr* StreamManager;
  std::string MachineName;
  unsigned int Port;
  bool Connected;

private:
  vtkPlusIEEListener(const vtkPlusIEEListener&);
  void operator=(const vtkPlusIEEListener&);
};

#endif //__IEEListener_h