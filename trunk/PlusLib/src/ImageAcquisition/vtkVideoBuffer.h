/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

/*=========================================================================
The following copyright notice is applicable to parts of this file:
Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
All rights reserved.
See Copyright.txt or http://www.kitware.com/Copyright.htm for details.
Authors include: Danielle Pace
(Robarts Research Institute and The University of Western Ontario)
=========================================================================*/ 

#ifndef __vtkVideoBuffer_h
#define __vtkVideoBuffer_h

#include "PlusConfigure.h"
#include "vtkObject.h"

#include "PlusVideoFrame.h"

#include "vtkTimestampedCircularBuffer.h"
#include "TrackedFrame.h"

class vtkImageData; 
class vtkTrackedFrameList;

/*!
  \class VideoBufferItem 
  \brief Stores a single video frame
  \ingroup PlusLibImageAcquisition
*/
class VTK_EXPORT VideoBufferItem : public TimestampedBufferItem
{
public:

  VideoBufferItem();
  virtual ~VideoBufferItem();

  VideoBufferItem(const VideoBufferItem& videoBufferItem); 
  VideoBufferItem& operator=(VideoBufferItem const&videoItem); 

  /*! Copy video buffer item */
  PlusStatus DeepCopy(VideoBufferItem* videoBufferItem); 
  
  PlusVideoFrame& GetFrame() { return this->Frame; };

private:
  PlusVideoFrame Frame;
};

#define UNDEFINED_TIMESTAMP DBL_MAX

/*!
  \class vtkVideoBuffer 
  \brief Store a collection of video frames

  vtkVideoBuffer is a structure for storing the last N number of
  timestamped video frames that are captured using a video source.

  \sa vtkPlusVideoSource vtkWin32VideoSource2 vtkMILVideoSource2

  \ingroup PlusLibImageAcquisition
*/
class VTK_EXPORT vtkVideoBuffer : public vtkObject
{
public:	

  enum TIMESTAMP_FILTERING_OPTION
  {
    READ_FILTERED_AND_UNFILTERED_TIMESTAMPS = 0,
    READ_UNFILTERED_COMPUTE_FILTERED_TIMESTAMPS,
    READ_FILTERED_IGNORE_UNFILTERED_TIMESTAMPS
  };

  static vtkVideoBuffer *New();
  vtkTypeRevisionMacro(vtkVideoBuffer,vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);

  /*!
    Set the size of the buffer, i.e. the maximum number of
    video frames that it will hold.  The default is 30.
  */
  virtual PlusStatus SetBufferSize(int n);
  /*! Get the size of the buffer */
  virtual int GetBufferSize(); 

  /*!
    Add a frame plus a timestamp to the buffer with frame index.
    If the timestamp is  less than or equal to the previous timestamp,
    or if the frame's format doesn't match the buffer's frame format,
    then the frame is not added to the buffer.
  */
  virtual PlusStatus AddItem(vtkImageData* frame, US_IMAGE_ORIENTATION usImageOrientation, long frameNumber, double unfilteredTimestamp=UNDEFINED_TIMESTAMP, 
    double filteredTimestamp=UNDEFINED_TIMESTAMP, TrackedFrame::FieldMapType* customFields = NULL); 
  /*!
    Add a frame plus a timestamp to the buffer with frame index.
    If the timestamp is  less than or equal to the previous timestamp,
    or if the frame's format doesn't match the buffer's frame format,
    then the frame is not added to the buffer.
  */
  virtual PlusStatus AddItem(const PlusVideoFrame* frame, US_IMAGE_ORIENTATION usImageOrientation, long frameNumber, double unfilteredTimestamp=UNDEFINED_TIMESTAMP, 
    double filteredTimestamp=UNDEFINED_TIMESTAMP, TrackedFrame::FieldMapType* customFields = NULL); 
  /*!
    Add a frame plus a timestamp to the buffer with frame index.
    Additionally an optional field name&value can be added,
    which will be saved as a custom field of the added item.
    If the timestamp is  less than or equal to the previous timestamp,
    or if the frame's format doesn't match the buffer's frame format,
    then the frame is not added to the buffer.
  */
  virtual PlusStatus AddItem(void* imageDataPtr, US_IMAGE_ORIENTATION  usImageOrientation, const int frameSizeInPx[2], PlusCommon::ITKScalarPixelType pixelType, 
    int	numberOfBytesToSkip, long   frameNumber, double unfilteredTimestamp=UNDEFINED_TIMESTAMP, double filteredTimestamp=UNDEFINED_TIMESTAMP, 
    TrackedFrame::FieldMapType* customFields = NULL); 

  /*! Get a frame with the specified frame uid from the buffer */
  virtual ItemStatus GetVideoBufferItem(BufferItemUidType uid, VideoBufferItem* bufferItem);
  /*! Get the most recent frame from the buffer */
  virtual ItemStatus GetLatestVideoBufferItem(VideoBufferItem* bufferItem) { return this->GetVideoBufferItem( this->GetLatestItemUidInBuffer(), bufferItem); }; 
  /*! Get the oldest frame from buffer */
  virtual ItemStatus GetOldestVideoBufferItem(VideoBufferItem* bufferItem) { return this->GetVideoBufferItem( this->GetOldestItemUidInBuffer(), bufferItem); }; 
  /*! Get a frame that was acquired at the specified time from buffer */
  virtual ItemStatus GetVideoBufferItemFromTime( double time, VideoBufferItem* bufferItem); 

  /*! Get latest timestamp in the buffer */
  virtual ItemStatus GetLatestTimeStamp( double& latestTimestamp );  

  /*! Get oldest timestamp in the buffer */
  virtual ItemStatus GetOldestTimeStamp( double& oldestTimestamp );  

  /*! Get video buffer item timestamp */
  virtual ItemStatus GetTimeStamp( BufferItemUidType uid, double& timestamp); 

  /*! Get buffer item unique ID */
  virtual BufferItemUidType GetOldestItemUidInBuffer() { return this->VideoBuffer->GetOldestItemUidInBuffer(); }
  virtual BufferItemUidType GetLatestItemUidInBuffer() { return this->VideoBuffer->GetLatestItemUidInBuffer(); }
  virtual ItemStatus GetItemUidFromTime(double time, BufferItemUidType& uid) { return this->VideoBuffer->GetItemUidFromTime(time, uid); }

  /*! Set the local time offset in seconds (global = local + offset) */
  virtual void SetLocalTimeOffsetSec(double offsetSec);
  /*! Get the local time offset in seconds (global = local + offset) */
  virtual double GetLocalTimeOffsetSec();

  /*! Get the number of items in the buffer */
  virtual int GetNumberOfItems() { return this->VideoBuffer->GetNumberOfItems(); }

  /*!
    Get the frame rate from the buffer based on the number of frames in the buffer and the elapsed time.
    Ideal frame rate shows the mean of the frame periods in the buffer based on the frame 
    number difference (aka the device frame rate)
  */
  virtual double GetFrameRate( bool ideal = false) { return this->VideoBuffer->GetFrameRate(ideal); }

  /*! Make this buffer into a copy of another buffer.  You should Lock both of the buffers before doing this. */
  virtual void DeepCopy(vtkVideoBuffer* buffer); 

  /*! Clear buffer (set the buffer pointer to the first element) */
  virtual void Clear(); 

  /*! Set number of items used for timestamp filtering (with LSQR mimimizer) */
  virtual void SetAveragedItemsForFiltering(int averagedItemsForFiltering); 

  /*! Set recording start time */
  virtual void SetStartTime( double startTime ); 
  /*! Get recording start time */
  virtual double GetStartTime(); 

  /*! Get the table report of the timestamped buffer  */
  virtual PlusStatus GetTimeStampReportTable(vtkTable* timeStampReportTable); 

  /*! Set the frame size in pixel  */
  PlusStatus SetFrameSize(int x, int y); 
  /*! Set the frame size in pixel  */
  PlusStatus SetFrameSize(int frameSize[2]); 
  /*! Get the frame size in pixel  */
  vtkGetVector2Macro(FrameSize, int); 

  /*! Set the pixel type */
  PlusStatus SetPixelType(PlusCommon::ITKScalarPixelType pixelType); 
  /*! Get the pixel type */
  vtkGetMacro(PixelType, PlusCommon::ITKScalarPixelType); 

  /*! Get the number of bytes per pixel */
  int GetNumberOfBytesPerPixel();

  /*! Copy images from a tracked frame buffer. It is useful when data is stored in a metafile and the data is needed as a vtkVideoBuffer. */
  PlusStatus CopyImagesFromTrackedFrameList(vtkTrackedFrameList *sourceTrackedFrameList, TIMESTAMP_FILTERING_OPTION timestampFiltering);

  /*! Dump the current state of the video buffer to metafile */
  virtual PlusStatus WriteToMetafile( const char* outputFolder, const char* metaFileName, bool useCompression = false ); 

protected:
  vtkVideoBuffer();
  ~vtkVideoBuffer();

  /*! Update video buffer by setting the frame format for each frame  */
  virtual PlusStatus AllocateMemoryForFrames(); 

  /*! 
    Compares frame format with new frame imaging parameters.
    \return true if current buffer frame format matches the method arguments, otherwise false
  */
  virtual bool CheckFrameFormat( const int frameSizeInPx[2], PlusCommon::ITKScalarPixelType pixelType ); 

  /*! Image frame size in pixel */
  int FrameSize[2]; 
  
  /*! Image pixel type */
  PlusCommon::ITKScalarPixelType PixelType; 

  typedef vtkTimestampedCircularBuffer<VideoBufferItem> VideoBufferType;
  /*! Timestamped circular buffer that stores the last N frames */
  VideoBufferType* VideoBuffer; 

private:
  vtkVideoBuffer(const vtkVideoBuffer&);
  void operator=(const vtkVideoBuffer&);
};




#endif
