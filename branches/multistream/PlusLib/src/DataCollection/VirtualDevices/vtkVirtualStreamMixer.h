/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkVirtualStreamMixer_h
#define __vtkVirtualStreamMixer_h

#include "vtkPlusDevice.h"
#include <string>

/*!
\class vtkVirtualStreamMixer 
\brief 

\ingroup PlusLibDataCollection
*/
class VTK_EXPORT vtkVirtualStreamMixer : public vtkPlusDevice
{
public:
  static vtkVirtualStreamMixer *New();
  vtkTypeRevisionMacro(vtkVirtualStreamMixer,vtkPlusDevice);
  void PrintSelf(ostream& os, vtkIndent indent);     

  virtual bool IsTracker() const { return false; }

  // Virtual stream mixers output only one stream
  vtkPlusStream* GetStream() const;

  virtual PlusStatus Reset();

  virtual PlusStatus NotifyConfigured();

  virtual double GetAcquisitionRate() const;
protected:
  virtual PlusStatus InternalUpdate();

  vtkVirtualStreamMixer();
  virtual ~vtkVirtualStreamMixer();

private:
  vtkVirtualStreamMixer(const vtkVirtualStreamMixer&);  // Not implemented.
  void operator=(const vtkVirtualStreamMixer&);  // Not implemented. 
};

#endif
