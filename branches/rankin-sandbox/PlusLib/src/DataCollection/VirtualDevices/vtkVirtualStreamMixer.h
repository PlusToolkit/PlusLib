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

  /*! Read main configuration from xml data */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement*);

  // Virtual stream mixers output only one stream
  vtkPlusStream* GetStream() const;

  virtual PlusStatus NotifyConfigured();

  /*! Set tool local time offsets in the input streams that contain tools */
  virtual void SetToolLocalTimeOffsetSec( double aTimeOffsetSec );

  /*! Get tool local time offset from the input streams that contains tools */
  virtual double GetToolLocalTimeOffsetSec();

  /*! Set tool local time offsets in the input streams that contain images */
  virtual void SetImageLocalTimeOffsetSec( double aTimeOffsetSec );

  /*! Get tool local time offset from the input streams that contains images */
  virtual double GetImageLocalTimeOffsetSec();

  virtual double GetAcquisitionRate() const;
protected:
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
