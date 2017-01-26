/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusVirtualSwitcher_h
#define __vtkPlusVirtualSwitcher_h

#include "vtkPlusDataCollectionExport.h"

#include "vtkPlusDevice.h"
#include "vtkPlusChannel.h"

/*!
\class vtkPlusVirtualSwitcher
\brief 

\ingroup PlusLibDataCollection
*/
class vtkPlusDataCollectionExport vtkPlusVirtualSwitcher : public vtkPlusDevice
{
public:
  static vtkPlusVirtualSwitcher *New();
  vtkTypeMacro(vtkPlusVirtualSwitcher, vtkPlusDevice);
  virtual void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

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

  vtkPlusVirtualSwitcher();
  virtual ~vtkPlusVirtualSwitcher();

  vtkGetObjectMacro(CurrentActiveInputChannel, vtkPlusChannel);
  vtkSetObjectMacro(CurrentActiveInputChannel, vtkPlusChannel);

  vtkSetObjectMacro(OutputChannel, vtkPlusChannel);

  vtkPlusChannel*                    CurrentActiveInputChannel;
  std::map<vtkPlusChannel*, double>  LastRecordedTimestampMap;
  vtkPlusChannel*                    OutputChannel;

  unsigned long FramesWhileInactive;

private:
  vtkPlusVirtualSwitcher(const vtkPlusVirtualSwitcher&);
  void operator=(const vtkPlusVirtualSwitcher&);
};

#endif //__vtkPlusVirtualSwitcher_h