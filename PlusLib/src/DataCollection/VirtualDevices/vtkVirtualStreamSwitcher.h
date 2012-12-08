/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkVirtualStreamSwitcher_h
#define __vtkVirtualStreamSwitcher_h

#include "vtkPlusDevice.h"
#include "vtkPlusStream.h"
#include <string>

/*!
\class vtkVirtualStreamSwitcher
\brief 

\ingroup PlusLibDataCollection
*/
class VTK_EXPORT vtkVirtualStreamSwitcher : public vtkPlusDevice
{
public:
  static vtkVirtualStreamSwitcher *New();
  vtkTypeRevisionMacro(vtkVirtualStreamSwitcher, vtkPlusDevice);
  void PrintSelf(ostream& os, vtkIndent indent);     

  virtual bool IsTracker() const { return false; }

  /*!
    Virtual stream switchers output only one stream
  */
  PlusStatus GetStream(vtkPlusStream* &aStream) const;

  virtual double GetAcquisitionRate() const;

  /*! Read main configuration from xml data */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement*);

  virtual PlusStatus NotifyConfigured();

  vtkGetObjectConstMacro(OutputStream, vtkPlusStream);

protected:
  virtual PlusStatus InternalUpdate();

  PlusStatus SelectActiveStream();

  PlusStatus CopyInputStreamToOutputStream();

  vtkVirtualStreamSwitcher();
  virtual ~vtkVirtualStreamSwitcher();

  vtkGetObjectMacro(CurrentActiveInputStream, vtkPlusStream);
  vtkSetObjectMacro(CurrentActiveInputStream, vtkPlusStream);

  vtkSetObjectMacro(OutputStream, vtkPlusStream);

  vtkPlusStream*                    CurrentActiveInputStream;
  std::map<vtkPlusStream*, double>  LastRecordedTimestampMap;
  vtkPlusStream*                    OutputStream;

  unsigned long FramesWhileInactive;

private:
  vtkVirtualStreamSwitcher(const vtkVirtualStreamSwitcher&);
  void operator=(const vtkVirtualStreamSwitcher&);
};

#endif //__vtkVirtualStreamSwitcher_h