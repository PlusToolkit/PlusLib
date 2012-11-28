/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkVirtualStreamMixer_h
#define __vtkVirtualStreamMixer_h

#include "vtkPlusDevice.h"
#include "vtkPlusStream.h"
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

  /*! Read main configuration from xml data */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement*);

  // Virtual stream mixers output only one stream
  vtkPlusStream* GetStream() const;

  virtual PlusStatus NotifyConfigured();

  virtual double GetAcquisitionRate() const;
protected:
  virtual void InternalWriteOutputStreams(vtkXMLDataElement* rootXMLElement);
  virtual void InternalWriteInputStreams(vtkXMLDataElement* rootXMLElement);

  vtkVirtualStreamMixer();
  virtual ~vtkVirtualStreamMixer();

  vtkGetObjectConstMacro(OutputStream, vtkPlusStream);
  vtkSetObjectMacro(OutputStream, vtkPlusStream);

  vtkPlusStream*  OutputStream;

private:
  vtkVirtualStreamMixer(const vtkVirtualStreamMixer&);  // Not implemented.
  void operator=(const vtkVirtualStreamMixer&);  // Not implemented. 
};

#endif
