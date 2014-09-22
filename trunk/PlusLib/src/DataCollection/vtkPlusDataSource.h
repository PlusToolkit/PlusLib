/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

// .NAME vtkPlusDataSource - interfaces VTK to a handheld 3D positioning tool or video source
// .SECTION Description
// The vtkPlusDataSource provides an interface between a tracked object or video stream in
// the real world and a virtual object.
// .SECTION see also
// vtkPlusDevice vtkPlusBuffer vtkPlusChannel

#ifndef __vtkPlusDataSource_h
#define __vtkPlusDataSource_h

#include "vtkDataCollectionExport.h"

#include "vtkObject.h"
#include "vtkPlusDevice.h"

/*!
\class vtkPlusDataSource 
\brief Interface to a handheld 3D positioning tool or video source

\ingroup PlusLibDataCollection
*/

class vtkPlusBuffer;
enum DataSourceType
{
  DATA_SOURCE_TYPE_NONE,
  DATA_SOURCE_TYPE_TOOL,
  DATA_SOURCE_TYPE_VIDEO,
};

class vtkDataCollectionExport vtkPlusDataSource : public vtkObject
{
public:

  static vtkPlusDataSource *New();
  vtkTypeMacro(vtkPlusDataSource,vtkObject);
  virtual void PrintSelf(ostream& os, vtkIndent indent);

  /*! Read main configuration from xml data */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* toolElement, bool requirePortNameInSourceConfiguration = false, bool requireImageOrientationInChannelConfiguration = false, const char* aDescriptiveNameForBuffer = NULL); 
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* toolElement); 
  virtual PlusStatus WriteCompactConfiguration(vtkXMLDataElement* toolElement); 

  /*! Set source Id. SourceId is used to identify the data source among all the data sources provided by the device 
  therefore it must be unique */
  PlusStatus SetSourceId(const char* toolSourceId);

  /*! Set reference name. Reference name is used to convey context about the coordinate frame that the tool is based */
  PlusStatus SetReferenceName(const char* referenceName);

  std::string GetTransformName() const;

  /*! Set port name. Port name is used to identify the source among all the sources provided by the device 
  therefore it must be unique */
  PlusStatus SetPortName(const char* portName);

  /*! Get the buffer */
  virtual vtkPlusBuffer* GetBuffer() const { return this->Buffer; }

  /*! Get the device which owns this source. */
  // TODO : consider a re-design of this idea
  void SetDevice(vtkPlusDevice* _arg){ this->Device = _arg; }
  vtkPlusDevice* GetDevice(){ return this->Device; }

  /*! Get port name. Port name is used to identify the tool among all the tools provided by the tracker device. */
  vtkGetStringMacro(PortName);

  vtkGetMacro(PortImageOrientation, US_IMAGE_ORIENTATION);
  vtkSetMacro(PortImageOrientation, US_IMAGE_ORIENTATION);

  /*! Get type: vidoe or tool. */
  DataSourceType GetType() const;
  void SetType(DataSourceType aType);

  /*! Get the frame number (some devices have frame numbering, otherwise just increment if new frame received) */
  vtkGetMacro(FrameNumber, unsigned long);
  vtkSetMacro(FrameNumber, unsigned long);

  /*! Get source id */
  vtkGetStringMacro(SourceId); 
  /*! Get the reference coordinate frame name */
  vtkGetStringMacro(ReferenceCoordinateFrameName);
    
  /*!
    Get a custom property string.
    If the property is not defined then an empty string is returned.
  */
  std::string GetCustomProperty(const std::string& propertyName);

  /*!
    Set a custom property string.
    Custom properties are useful because custom information can be stored for each tool
    in the same class object where standard properties of the tool are stored.
  */
  void SetCustomProperty(const std::string& propertyName, const std::string& propertyValue);

  /*! Make this tracker into a copy of another tracker. You should lock both of the tracker buffers before doing this. */
  void DeepCopy(vtkPlusDataSource *source);

protected:
  vtkPlusDataSource();
  ~vtkPlusDataSource();

  vtkPlusDevice *Device;

  char *PortName;
  /*! The orientation of the image outputted by the device */
  US_IMAGE_ORIENTATION PortImageOrientation; 

  DataSourceType Type;

  unsigned long FrameNumber; 

  char *SourceId; 
  char *ReferenceCoordinateFrameName;

  vtkPlusBuffer* Buffer;

  std::map< std::string, std::string > CustomProperties;

private:
  vtkPlusDataSource(const vtkPlusDataSource&);
  void operator=(const vtkPlusDataSource&);  

};

#endif







