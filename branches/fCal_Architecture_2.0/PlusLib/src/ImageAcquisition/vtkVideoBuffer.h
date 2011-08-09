/*=========================================================================

Program:   Visualization Toolkit
Module:    $RCSfile: vtkVideoBuffer.h,v $

Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
All rights reserved.
See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

Author: Danielle Pace
Robarts Research Institute and The University of Western Ontario

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information. 

=========================================================================*/
// .NAME vtkVideoBuffer - Store a collection of video frames
// .SECTION Description
// vtkVideoBuffer is a structure for holding video frames that are
// captured either from a vtkRenderWindow or from some other video
// source.  The buffer can be locked, to allow some video frames to
// be written to while other frames are being read from.  Hopefully
// an additional class will be written that will take a vtkVideoBuffer
// and compress it into a movie file.
// .SECTION See Also
// vtkVideoSource2 vtkWin32VideoSource2 vtkMILVideoSource2

#ifndef __vtkVideoBuffer_h
#define __vtkVideoBuffer_h

#include "PlusConfigure.h"
#include "vtkObject.h"

#include "vtkObjectFactory.h"
#include "vtkTimestampedCircularBuffer.h"
#include "itkImage.h"
#include "UsImageConverterCommon.h"

class vtkVideoBufferObject; 
class vtkImageData; 
class TimestampedBufferItem; 

class VTK_EXPORT VideoBufferItem : public TimestampedBufferItem
{
public:
  typedef UsImageConverterCommon::PixelType PixelType;
  typedef UsImageConverterCommon::ImageType ImageType;

  VideoBufferItem(); 
  ~VideoBufferItem(); 
  VideoBufferItem(const VideoBufferItem& videoBufferItem); 
  VideoBufferItem& VideoBufferItem::operator=(VideoBufferItem const&videoItem); 

  // Copy video buffer item 
  PlusStatus DeepCopy(VideoBufferItem* videoBufferItem); 

  // Set/get video frame 
  // Caller should clean the image data from memory after this call. 
  PlusStatus SetFrame(const ImageType::Pointer& frame); 
  PlusStatus SetFrame(vtkImageData* frame); 
  PlusStatus SetFrame(unsigned char *imageDataPtr, 
    const int frameSizeInPx[3],
    int numberOfBitsPerPixel, 
    int	numberOfBytesToSkip ); 

  PlusStatus AllocateFrame(int imageSize[2]); 

  unsigned long GetFrameSizeInBytes(); 

  ImageType::Pointer GetFrame() const { return this->Frame; }

protected:
  ImageType::Pointer Frame; 
}; 


class VTK_EXPORT vtkVideoBuffer : public vtkObject
{
public:	

  static vtkVideoBuffer *New();
  vtkTypeRevisionMacro(vtkVideoBuffer,vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Set/Get the size of the buffer, i.e. the maximum number of
  // video frames that it will hold.  The default is 30.
  virtual PlusStatus SetBufferSize(int n);
  virtual int GetBufferSize(); 

  // Description:
  // Update video buffer by setting the frame format for each frame 
  virtual void UpdateBufferFrameFormats(); 

  // Description:
  // Add a frame plus a timestamp to the buffer with frame index.  If the timestamp is
  // less than or equal to the previous timestamp, or if the frame's format
  // doesn't match the buffer's frame format, then nothing will be done.
  virtual PlusStatus AddTimeStampedItem(unsigned char* imageDataPtr,                               
    US_IMAGE_ORIENTATION  usImageOrientation, 
    const int    frameSizeInPx[2],
    int    numberOfBitsPerPixel, 
    int	numberOfBytesToSkip, 
    double unfilteredTimestamp, 
    long   frameNumber); 

  virtual PlusStatus AddTimeStampedItem(unsigned char* imageDataPtr,                               
    US_IMAGE_ORIENTATION  usImageOrientation, 
    const int    frameSizeInPx[2],
    int    numberOfBitsPerPixel, 
    int	numberOfBytesToSkip, 
    double unfilteredTimestamp, 
    double filteredTimestamp, 
    long   frameNumber); 

  virtual PlusStatus AddItem(unsigned char* imageDataPtr,                               
    US_IMAGE_ORIENTATION  usImageOrientation, 
    const int    frameSizeInPx[2],
    int    numberOfBitsPerPixel, 
    int	numberOfBytesToSkip, 
    long   frameNumber); 

  virtual PlusStatus AddItem(vtkImageData* frame, US_IMAGE_ORIENTATION usImageOrientation, long frameNumber); 
  virtual PlusStatus AddTimeStampedItem(vtkImageData* frame, US_IMAGE_ORIENTATION usImageOrientation, double unfilteredTimestamp, long frameNumber); 
  virtual PlusStatus AddTimeStampedItem(vtkImageData* frame, US_IMAGE_ORIENTATION usImageOrientation, double unfilteredTimestamp, double filteredTimestamp, long frameNumber); 

  // Description:
  // Get tracker item from buffer 
  virtual ItemStatus GetVideoBufferItem(BufferItemUidType uid, VideoBufferItem* bufferItem);
  virtual ItemStatus GetLatestVideoBufferItem(VideoBufferItem* bufferItem) { return this->GetVideoBufferItem( this->GetLatestItemUidInBuffer(), bufferItem); }; 
  virtual ItemStatus GetOldestVideoBufferItem(VideoBufferItem* bufferItem) { return this->GetVideoBufferItem( this->GetOldestItemUidInBuffer(), bufferItem); }; 
  virtual ItemStatus GetVideoBufferItemFromTime( double time, VideoBufferItem* bufferItem); 

  // Description:
  // Get latest timestamp in the buffer 
  virtual ItemStatus GetLatestTimeStamp( double& latestTimestamp );  

  // Description:
  // Get oldest timestamp in the buffer 
  virtual ItemStatus GetOldestTimeStamp( double& oldestTimestamp );  

  // Description:
  // Get video buffer item timestamp 
  virtual ItemStatus GetTimeStamp( BufferItemUidType uid, double& timestamp); 

  // Description:
  // Get buffer item unique ID 
  virtual BufferItemUidType GetOldestItemUidInBuffer() { return this->VideoBuffer->GetOldestItemUidInBuffer(); }
  virtual BufferItemUidType GetLatestItemUidInBuffer() { return this->VideoBuffer->GetLatestItemUidInBuffer(); }
  virtual ItemStatus GetItemUidFromTime(double time, BufferItemUidType& uid) { return this->VideoBuffer->GetItemUidFromTime(time, uid); }

  // Description:	
  // Get/Set the local time offset (global = local + offset)
  virtual void SetLocalTimeOffset(double offset);
  virtual double GetLocalTimeOffset();

  // Description:
  // Get the number of items in the buffer
  virtual int GetNumberOfItems() { return this->VideoBuffer->GetNumberOfItems(); }

  // Description:
  // Get the frame rate from the buffer based on the number of frames in the buffer
  // and the elapsed time.
  // Ideal frame rate shows the mean of the frame periods in the buffer based on the frame 
  // number difference (aka the device frame rate)
  virtual double GetFrameRate( bool ideal = false) { return this->VideoBuffer->GetFrameRate(ideal); }

  // Description:
  // Make this buffer into a copy of another buffer.  You should
  // Lock both of the buffers before doing this.
  virtual void DeepCopy(vtkVideoBuffer* buffer); 

  // Description:
  // Clear buffer (set the buffer pointer to the first element)
  virtual void Clear(); 

  // Description:
  // Set/Get number of items used for timestamp filtering (with LSQR mimimizer)
  virtual void SetAveragedItemsForFiltering(int averagedItemsForFiltering); 

  // Description:
  // Set/get recording start time
  virtual void SetStartTime( double startTime ); 
  virtual double GetStartTime(); 

  // Description: 
  // Get the table report of the timestamped buffer 
  virtual PlusStatus GetTimeStampReportTable(vtkTable* timeStampReportTable); 

  // Description:
  // Set/get frame size in pixel 
  vtkSetVector2Macro(FrameSize, int); 
  vtkGetVector2Macro(FrameSize, int); 

  // Description:
  // Set/get pixel size in bits 
  vtkSetMacro(NumberOfBitsPerPixel, int); 
  vtkGetMacro(NumberOfBitsPerPixel, int); 

protected:
  vtkVideoBuffer();
  ~vtkVideoBuffer();

  // Description:
  // Compares frame format with new frame imaging parameters
  // Returns true if it matches, otherwise false
  virtual bool CheckFrameFormat( const int frameSizeInPx[2], int numberOfBitsPerPixel ); 

  int FrameSize[2]; 
  int NumberOfBitsPerPixel; 

  typedef vtkTimestampedCircularBuffer<VideoBufferItem> VideoBufferType;
  VideoBufferType* VideoBuffer; 

private:
  vtkVideoBuffer(const vtkVideoBuffer&);
  void operator=(const vtkVideoBuffer&);
};




#endif
