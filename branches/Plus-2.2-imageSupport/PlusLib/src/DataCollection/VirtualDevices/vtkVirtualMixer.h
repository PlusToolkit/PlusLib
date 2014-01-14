/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkVirtualMixer_h
#define __vtkVirtualMixer_h

#include "vtkPlusDevice.h"
#include "vtkPlusChannel.h"
#include <string>

/*!
\class vtkVirtualMixer 
\brief 

\ingroup PlusLibDataCollection
*/
class VTK_EXPORT vtkVirtualMixer : public vtkPlusDevice
{
public:
  static vtkVirtualMixer *New();
  vtkTypeRevisionMacro(vtkVirtualMixer,vtkPlusDevice);
  void PrintSelf(ostream& os, vtkIndent indent);

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
  vtkVirtualMixer();
  virtual ~vtkVirtualMixer();

private:
  vtkVirtualMixer(const vtkVirtualMixer&);  // Not implemented.
  void operator=(const vtkVirtualMixer&);  // Not implemented. 
};

#endif
