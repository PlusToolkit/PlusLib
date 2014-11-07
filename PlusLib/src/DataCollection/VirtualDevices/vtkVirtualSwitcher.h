/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkVirtualSwitcher_h
#define __vtkVirtualSwitcher_h

#include "vtkDataCollectionExport.h"

#include "vtkPlusDevice.h"
#include "vtkPlusChannel.h"

/*!
\class vtkVirtualSwitcher
\brief 

\ingroup PlusLibDataCollection
*/
class vtkDataCollectionExport vtkVirtualSwitcher : public vtkPlusDevice
{
public:
  static vtkVirtualSwitcher *New();
  vtkTypeMacro(vtkVirtualSwitcher, vtkPlusDevice);
  void PrintSelf(ostream& os, vtkIndent indent);

  /*!
    Virtual stream switchers output only one stream
  */
  PlusStatus GetStream(vtkPlusChannel* &aStream) const;

  virtual double GetAcquisitionRate() const;

  /*! Read main configuration from xml data */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement*);

  virtual PlusStatus NotifyConfigured();

  vtkGetObjectConstMacro(OutputStream, vtkPlusChannel);

  virtual bool IsTracker() const { return false; }
  virtual bool IsVirtual() const { return true; }

protected:
  virtual PlusStatus InternalUpdate();

  PlusStatus SelectActiveStream();

  PlusStatus CopyInputStreamToOutputStream();

  vtkVirtualSwitcher();
  virtual ~vtkVirtualSwitcher();

  vtkGetObjectMacro(CurrentActiveInputStream, vtkPlusChannel);
  vtkSetObjectMacro(CurrentActiveInputStream, vtkPlusChannel);

  vtkSetObjectMacro(OutputStream, vtkPlusChannel);

  vtkPlusChannel*                    CurrentActiveInputStream;
  std::map<vtkPlusChannel*, double>  LastRecordedTimestampMap;
  vtkPlusChannel*                    OutputStream;

  unsigned long FramesWhileInactive;

private:
  vtkVirtualSwitcher(const vtkVirtualSwitcher&);
  void operator=(const vtkVirtualSwitcher&);
};

#endif //__vtkVirtualSwitcher_h