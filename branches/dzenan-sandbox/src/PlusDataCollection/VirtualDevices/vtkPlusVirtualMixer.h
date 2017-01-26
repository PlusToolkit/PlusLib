/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusVirtualMixer_h
#define __vtkPlusVirtualMixer_h

#include "vtkPlusDataCollectionExport.h"

#include "vtkPlusDevice.h"

class vtkPlusChannel;

/*!
\class vtkPlusVirtualMixer 
\brief 

\ingroup PlusLibDataCollection
*/
class vtkPlusDataCollectionExport vtkPlusVirtualMixer : public vtkPlusDevice
{
public:
  static vtkPlusVirtualMixer *New();
  vtkTypeMacro(vtkPlusVirtualMixer,vtkPlusDevice);
  virtual void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

  /*! Answer if device is a tracker */
  virtual bool IsTracker() const;

  virtual bool IsVirtual() const { return true; }

  /*!
    Return whether or not the device can be reset
  */
  virtual bool IsResettable();

  /*! Read main configuration from xml data */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement*);

  // Virtual stream mixers output only one stream
  vtkPlusChannel* GetChannel() const;

  /*! Reset any input devices */
  virtual PlusStatus Reset();

  virtual PlusStatus NotifyConfigured();

  virtual double GetAcquisitionRate() const;

protected:
  vtkPlusVirtualMixer();
  virtual ~vtkPlusVirtualMixer();

private:
  vtkPlusVirtualMixer(const vtkPlusVirtualMixer&);  // Not implemented.
  void operator=(const vtkPlusVirtualMixer&);  // Not implemented. 
};

#endif
