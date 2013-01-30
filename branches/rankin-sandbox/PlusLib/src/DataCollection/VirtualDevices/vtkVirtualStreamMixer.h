/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkVirtualStreamMixer_h
#define __vtkVirtualStreamMixer_h

#include "vtkPlusDevice.h"
#include "vtkPlusChannel.h"
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

  /*! Read main configuration from xml data */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement*);

  // Virtual stream mixers output only one stream
  vtkPlusChannel* GetStream() const;

  virtual PlusStatus NotifyConfigured();

  /*! Set tool local time offsets in the input streams that contain tools */
  virtual void SetToolLocalTimeOffsetSec( double aTimeOffsetSec );

  /*! Get tool local time offset from the input streams that contains tools */
  virtual double GetToolLocalTimeOffsetSec();

  /*! Set tool local time offsets in the input streams that contain images */
  virtual void SetVideoLocalTimeOffsetSec( double aTimeOffsetSec );

  /*! Get tool local time offset from the input streams that contains images */
  virtual double GetVideoLocalTimeOffsetSec();

  virtual double GetAcquisitionRate() const;
protected:
  vtkVirtualStreamMixer();
  virtual ~vtkVirtualStreamMixer();

  vtkGetObjectConstMacro(OutputStream, vtkPlusChannel);
  vtkSetObjectMacro(OutputStream, vtkPlusChannel);

  vtkPlusChannel*  OutputStream;

private:
  vtkVirtualStreamMixer(const vtkVirtualStreamMixer&);  // Not implemented.
  void operator=(const vtkVirtualStreamMixer&);  // Not implemented. 
};

#endif
