/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

// .NAME vtkPlusStreamImage
// .SECTION Description
// The vtkPlusStreamTool provides an interface to an image outputted
// by a device.
// .SECTION see also
// vtkPlusDevice

#ifndef __vtkPlusStreamImage_h
#define __vtkPlusStreamImage_h

#include "vtkObject.h"
#include "vtkPlusDevice.h"
#include "vtkPlusStreamBuffer.h"

/*!
\class vtkPlusStreamImage 
\brief Interface to a image output from a device

\ingroup PlusLibDataCollection
*/
class VTK_EXPORT vtkPlusStreamImage : public vtkObject
{
public:

  static vtkPlusStreamImage *New();
  vtkTypeMacro(vtkPlusStreamImage,vtkObject);
  virtual void PrintSelf(ostream& os, vtkIndent indent);

  /*! Read main configuration from xml data */
  virtual PlusStatus ReadConfiguration(vtkSmartPointer<vtkXMLDataElement> imageElement, bool RequireAveragedItemsForFilteringInDeviceSetConfiguration); 
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* toolElement); 
  virtual PlusStatus WriteCompactConfiguration(vtkXMLDataElement* toolElement); 

  /*! Set image name. Image name is used to identify the image among all the images provided by the video device 
  therefore it must be unique and can be set only once */
  PlusStatus SetImageName(const char* toolName);

  /*! Get the image buffer */
  virtual vtkPlusStreamBuffer* GetBuffer() const { return this->Buffer; }

  /*! Get the tracker which owns this tool. */
  vtkGetObjectMacro(Device, vtkPlusDevice);

  /*! Get the frame number (some devices have frame numbering, otherwise just increment if new frame received) */
  vtkGetMacro(FrameNumber, unsigned long);
  vtkSetMacro(FrameNumber, unsigned long);

  vtkGetStringMacro(ImageName); 

  /*! Set device which owns this image */
  void SetDevice(vtkPlusDevice *device);
    
  /*! Make this image into a copy of another image. You should lock both of the image buffers before doing this. */
  void DeepCopy(vtkPlusStreamImage *image);

protected:
  vtkPlusStreamImage();
  ~vtkPlusStreamImage();

  vtkPlusDevice *Device;

  unsigned long FrameNumber; 

  char *ImageName; 

  vtkSmartPointer<vtkPlusStreamBuffer> Buffer;

private:
  vtkPlusStreamImage(const vtkPlusStreamImage&);
  void operator=(const vtkPlusStreamImage&);  

};

#endif







