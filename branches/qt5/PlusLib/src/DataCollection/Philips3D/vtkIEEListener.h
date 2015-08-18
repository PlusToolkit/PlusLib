/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __IEEListener_h
#define __IEEListener_h

#include "vtkDataCollectionExport.h"

#include "vtkObject.h"
#include "StreamMgr.h"

class vtkDataCollectionExport vtkIEEListener : public vtkObject
{
public:
  vtkTypeMacro(vtkIEEListener, vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);   
  static vtkIEEListener* New();

  void SetMachineName(const std::string& machineName);
  void SetPortNumber(unsigned int port);

  /*!
    Connect to a streaming ie33 device
    \param callback the function to call when new data
  */
  PlusStatus Connect(CLIENT_POSTSCANCONVERT_CALLBACK callback);
  /*!
    Disconnect from the ie33 device
  */
  PlusStatus Disconnect();

  /*! Query the device to see if it's connected */
  bool IsConnected();

protected:
  vtkIEEListener();
  ~vtkIEEListener();

protected:
  CStreamMgr* StreamManager;
  std::string MachineName;
  unsigned int Port;
  bool Connected;

private:
  vtkIEEListener(const vtkIEEListener&);  // Not implemented.
  void operator=(const vtkIEEListener&);  // Not implemented.
};

#endif //__IEEListener_h