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
    Virtual channel switchers output only one channel
  */
  PlusStatus GetChannel(vtkPlusChannel* &aChannel) const;

  virtual double GetAcquisitionRate() const;

  /*! Read main configuration from xml data */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement*);

  virtual PlusStatus NotifyConfigured();

  vtkGetObjectConstMacro(OutputChannel, vtkPlusChannel);

  virtual bool IsTracker() const { return false; }
  virtual bool IsVirtual() const { return true; }

protected:
  virtual PlusStatus InternalUpdate();

  PlusStatus SelectActiveChannel();

  PlusStatus CopyInputChannelToOutputChannel();

  vtkVirtualSwitcher();
  virtual ~vtkVirtualSwitcher();

  vtkGetObjectMacro(CurrentActiveInputChannel, vtkPlusChannel);
  vtkSetObjectMacro(CurrentActiveInputChannel, vtkPlusChannel);

  vtkSetObjectMacro(OutputChannel, vtkPlusChannel);

  vtkPlusChannel*                    CurrentActiveInputChannel;
  std::map<vtkPlusChannel*, double>  LastRecordedTimestampMap;
  vtkPlusChannel*                    OutputChannel;

  unsigned long FramesWhileInactive;

private:
  vtkVirtualSwitcher(const vtkVirtualSwitcher&);
  void operator=(const vtkVirtualSwitcher&);
};

#endif //__vtkVirtualSwitcher_h