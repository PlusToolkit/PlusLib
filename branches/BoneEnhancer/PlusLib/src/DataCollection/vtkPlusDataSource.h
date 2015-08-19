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
#include "vtkPlusBuffer.h"
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
  typedef std::map< std::string, std::string > CustomPropertyMap;
  typedef CustomPropertyMap::iterator CustomPropertyMapIterator;
  typedef CustomPropertyMap::const_iterator CustomPropertyMapConstIterator;

public:

  static vtkPlusDataSource *New();
  vtkTypeMacro(vtkPlusDataSource,vtkObject);
  virtual void PrintSelf(ostream& os, vtkIndent indent);

  /*! Read main configuration from xml data */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* toolElement, bool requirePortNameInSourceConfiguration = false, bool requireImageOrientationInChannelConfiguration = false, const char* aDescriptiveNameForBuffer = NULL); 
  /*! Write main configuration to xml data */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* toolElement); 
  /*! WriteCompactConfiguration is called when a channel is populating its tool data source links, most usages should call WriteConfiguration */
  virtual PlusStatus WriteCompactConfiguration(vtkXMLDataElement* toolElement); 

  /*! Set source Id. SourceId is used to identify the data source among all the data sources provided by the device 
  therefore it must be unique */
  PlusStatus SetSourceId(const char* toolSourceId);
  /*! Get source id */
  vtkGetStringMacro(SourceId);

  /*! Set reference name. Reference name is used to convey context about the coordinate frame that the tool is based */
  PlusStatus SetReferenceCoordinateFrameName(const char* referenceName);
  /*! Get the reference coordinate frame name */
  vtkGetStringMacro(ReferenceCoordinateFrameName);

  std::string GetTransformName() const;

  /*! Set port name. Port name is used to identify the source among all the sources provided by the device 
  therefore it must be unique */
  PlusStatus SetPortName(const char* portName);

  /*! Set the image type. Does not convert the pixel values. */
  PlusStatus SetImageType(US_IMAGE_TYPE imageType); 
  /*! Get the image type (B-mode, RF, ...) */
  US_IMAGE_TYPE GetImageType();

  /*! Set the non-clipped input frame size in pixel  */
  PlusStatus SetInputFrameSize(int x, int y, int z); 
  /*! Set the non-clipped input frame size in pixel  */
  PlusStatus SetInputFrameSize(int frameSize[3]); 
  /*! Get the input frame size in pixel  */
  vtkGetVector3Macro(InputFrameSize,int);

  /*! 
	Get the oputput frame size in pixel. If no clipping rectangle is set then the output
	frame size is the clipping rectangle size; otherwise it is the input frame size
  */
  virtual int* GetOutputFrameSize();
  virtual PlusStatus GetOutputFrameSize(int &_arg1, int &_arg2, int &_arg3);
  virtual PlusStatus GetOutputFrameSize (int _arg[3]);

  /*! Set recording start time */
  virtual void SetStartTime( double startTime ); 
  /*! Get recording start time */
  virtual double GetStartTime(); 

  /*! Get the number of items in the buffer */
  virtual int GetNumberOfItems();

  /*! Get the index assigned by the data acquisition system (usually a counter) from the buffer by frame UID. */
  virtual ItemStatus GetIndex(const BufferItemUidType uid, unsigned long &index);

  /*!
    Get the frame rate from the buffer based on the number of frames in the buffer and the elapsed time.
    Ideal frame rate shows the mean of the frame periods in the buffer based on the frame 
    number difference (aka the device frame rate).
    If framePeriodStdevSecPtr is not null, then the standard deviation of the frame period is computed as well (in seconds) and
    stored at the specified address.
  */
  virtual double GetFrameRate( bool ideal = false, double *framePeriodStdevSecPtr=NULL);

  /*! Get buffer item unique ID */
  virtual BufferItemUidType GetOldestItemUidInBuffer();
  virtual BufferItemUidType GetLatestItemUidInBuffer();
  virtual ItemStatus GetItemUidFromTime(double time, BufferItemUidType& uid);

  /*! Returns true if the latest item contains valid video data */
  virtual bool GetLatestItemHasValidVideoData();

  /*! Returns true if the latest item contains valid transform data */
  virtual bool GetLatestItemHasValidTransformData();

  /*! Get a frame with the specified frame uid from the buffer */
  virtual ItemStatus GetStreamBufferItem(BufferItemUidType uid, StreamBufferItem* bufferItem);
  /*! Get the most recent frame from the buffer */
  virtual ItemStatus GetLatestStreamBufferItem(StreamBufferItem* bufferItem);
  /*! Get the oldest frame from buffer */
  virtual ItemStatus GetOldestStreamBufferItem(StreamBufferItem* bufferItem);
  /*! Get a frame that was acquired at the specified time from buffer */
  virtual ItemStatus GetStreamBufferItemFromTime( double time, StreamBufferItem* bufferItem, vtkPlusBuffer::DataItemTemporalInterpolationType interpolation);

  /*! Make a copy of the buffer */
  virtual PlusStatus DeepCopyBufferTo(vtkPlusBuffer& bufferToFill);

  /*! Clear buffer (set the buffer pointer to the first element) */
  virtual void Clear();

  /*! Dump the current state of the video buffer to metafile */
  virtual PlusStatus WriteToMetafile( const char* filename, bool useCompression = false ); 

  /*! Get the table report of the timestamped buffer  */
  virtual PlusStatus GetTimeStampReportTable(vtkTable* timeStampReportTable); 

  /*! If TimeStampReporting is enabled then all filtered and unfiltered timestamp values will be saved in a table for diagnostic purposes. */
  void SetTimeStampReporting(bool enable);
  /*! If TimeStampReporting is enabled then all filtered and unfiltered timestamp values will be saved in a table for diagnostic purposes. */
  bool GetTimeStampReporting();

  /*!
    Set the size of the buffer, i.e. the maximum number of
    video frames that it will hold.  The default is 30.
  */
  virtual PlusStatus SetBufferSize(int n);
  /*! Get the size of the buffer */
  virtual int GetBufferSize(); 

  /*! Get latest timestamp in the buffer */
  virtual ItemStatus GetLatestTimeStamp( double& latestTimestamp );  

  /*! Get oldest timestamp in the buffer */
  virtual ItemStatus GetOldestTimeStamp( double& oldestTimestamp );  

  /*! Get video buffer item timestamp */
  virtual ItemStatus GetTimeStamp( BufferItemUidType uid, double& timestamp);

  /*! Set the local time offset in seconds (global = local + offset) */
  virtual void SetLocalTimeOffsetSec(double offsetSec);
  /*! Get the local time offset in seconds (global = local + offset) */
  virtual double GetLocalTimeOffsetSec();

  /*!
    Add a frame plus a timestamp to the buffer with frame index.
    If the timestamp is  less than or equal to the previous timestamp,
    or if the frame's format doesn't match the buffer's frame format,
    then the frame is not added to the buffer.
  */
  virtual PlusStatus AddItem(vtkImageData* frame, US_IMAGE_ORIENTATION usImageOrientation, US_IMAGE_TYPE imageType, long frameNumber, double unfilteredTimestamp=UNDEFINED_TIMESTAMP, 
    double filteredTimestamp=UNDEFINED_TIMESTAMP, const TrackedFrame::FieldMapType* customFields = NULL); 

  /*!
    Add a frame plus a timestamp to the buffer with frame index.
    If the timestamp is  less than or equal to the previous timestamp,
    or if the frame's format doesn't match the buffer's frame format,
    then the frame is not added to the buffer.
  */
  virtual PlusStatus AddItem(const PlusVideoFrame* frame, long frameNumber, double unfilteredTimestamp=UNDEFINED_TIMESTAMP, 
    double filteredTimestamp=UNDEFINED_TIMESTAMP, const TrackedFrame::FieldMapType* customFields = NULL); 

  /*!
    Add a frame plus a timestamp to the buffer with frame index.
    Additionally an optional field name&value can be added,
    which will be saved as a custom field of the added item.
    If the timestamp is  less than or equal to the previous timestamp,
    or if the frame's format doesn't match the buffer's frame format,
    then the frame is not added to the buffer.
  */
  virtual PlusStatus AddItem(void* imageDataPtr, US_IMAGE_ORIENTATION  usImageOrientation, const int frameSizeInPx[3], PlusCommon::VTKScalarPixelType pixelType, int numberOfScalarComponents, US_IMAGE_TYPE imageType, 
    int  numberOfBytesToSkip, long   frameNumber, double unfilteredTimestamp=UNDEFINED_TIMESTAMP, double filteredTimestamp=UNDEFINED_TIMESTAMP, 
    const TrackedFrame::FieldMapType* customFields = NULL);

    /*!
    Add a matrix plus status to the list, with an exactly known timestamp value (e.g., provided by a high-precision hardware timer).
    If the timestamp is less than or equal to the previous timestamp, then nothing  will be done.
    If filteredTiemstamp argument is undefined then the filtered timestamp will be computed from the input unfiltered timestamp.
  */
  PlusStatus AddTimeStampedItem(vtkMatrix4x4 *matrix, ToolStatus status, unsigned long frameNumber, double unfilteredTimestamp, double filteredTimestamp=UNDEFINED_TIMESTAMP);

  /*! Get the device which owns this source. */
  // TODO : consider a re-design of this idea
  void SetDevice(vtkPlusDevice* _arg){ this->Device = _arg; }
  vtkPlusDevice* GetDevice(){ return this->Device; }

  /*! Get port name. Port name is used to identify the tool among all the tools provided by the tracker device. */
  vtkGetStringMacro(PortName);

  /*! Set the pixel type */
  PlusStatus SetPixelType(PlusCommon::VTKScalarPixelType pixelType); 
  /*! Get the pixel type */
  virtual PlusCommon::VTKScalarPixelType GetPixelType();

  /*! Set the number of scalar components */
  PlusStatus SetNumberOfScalarComponents(int numberOfScalarComponents); 
  /*! Get the number of scalar components*/
  virtual int GetNumberOfScalarComponents();

    /*!
    Get the number of bytes per pixel
    It is the number of bytes per scalar multiplied by the number of scalar components.
  */
  int GetNumberOfBytesPerPixel();

  /*!
    Set the image orientation of the input data (MF, MN, ...).
    The internal buffer's image orientation and the output is always the standard MF (for B-mode) or FM (for RF-mode),
    so the input images are reoriented as needed when AddItem(...) is called.
    This method is necessary for initializing the storage buffers and also serves as a convenient storage for the
    image orientation of the hardware device.
  */
  virtual PlusStatus SetInputImageOrientation(US_IMAGE_ORIENTATION imageOrientation); 
  /*!
    Get the image orientation of the input data (MF, MN, ...).
    The internal buffer's image orientation and the output is always the standard MF (for B-mode) or FM (for RF-mode).
  */
  virtual US_IMAGE_ORIENTATION GetInputImageOrientation();
  
  /*!
    Set the image orientation of the internal buffer.
    It has to be MF for all imaging modes except RF (for that the orientation has to be FM).
  */
  virtual PlusStatus SetOutputImageOrientation(US_IMAGE_ORIENTATION imageOrientation);
  virtual US_IMAGE_ORIENTATION GetOutputImageOrientation();

  /*! Get type: vidoe or tool. */
  DataSourceType GetType() const;
  void SetType(DataSourceType aType);

  /*! Get the frame number (some devices have frame numbering, otherwise just increment if new frame received) */
  vtkGetMacro(FrameNumber, unsigned long);
  vtkSetMacro(FrameNumber, unsigned long);
    
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

  /*!
    Get the clip rectangle size to apply to the image in pixel coordinates.
    If the ClipRectangleSize is (0,0) then the values are ignored and the whole frame is captured.
  */
  vtkGetVector3Macro(ClipRectangleSize,int);

  /*!
    Get the clip rectangle origin to apply to the image in pixel coordinates.
    If the ClipRectangleSize is (0,0) then the whole frame is captured.
  */
  vtkGetVector3Macro(ClipRectangleOrigin,int);

  /*!
    Set the clip rectangle size to apply to the image in pixel coordinates.
    If the ClipRectangleSize is (0,0) then the values are ignored and the whole frame is captured.
    Width of the ClipRectangle typically have to be a multiple of 4.
  */
  vtkSetVector3Macro(ClipRectangleSize,int);

  /*!
    Set the clip rectangle origin to apply to the image in pixel coordinates.
    If the ClipRectangleSize is (0,0) then the whole frame is captured.
  */
  vtkSetVector3Macro(ClipRectangleOrigin,int);

protected:
  /*! Access the data buffer */
  virtual vtkPlusBuffer* GetBuffer();

protected:
  vtkPlusDataSource();
  ~vtkPlusDataSource();

  vtkPlusDevice *Device;

  char *PortName;
  /*! The orientation of the image outputted by the device */
  US_IMAGE_ORIENTATION InputImageOrientation; 

  DataSourceType Type;

  unsigned long FrameNumber; 

  char *SourceId; 
  char *ReferenceCoordinateFrameName;

  vtkPlusBuffer* Buffer;

  CustomPropertyMap CustomProperties;

  /*! Crop rectangle origin for this data source */
  int ClipRectangleOrigin[3];
  /*! Crop rectangle size for this data source */
  int ClipRectangleSize[3];

  int InputFrameSize[3];

private:
  vtkPlusDataSource(const vtkPlusDataSource&);
  void operator=(const vtkPlusDataSource&);  

};

#endif







