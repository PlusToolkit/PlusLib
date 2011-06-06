/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkVideoFrame2.h,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

  Author: Danielle Pace
          Robarts Research Institute and The University of Western Ontario

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information. 

=========================================================================*/
// .NAME vtkVideoFrame2 - Store images in a variety of pixel formats
// .SECTION Description
// vtkVideoFrame2 is a structure for storing images in formats that are
// compatible with different video framegrabbers and video compression
// codecs.  The structure also contains all of the information necessary
// to convert the frame into the Windows DIB format.  As an example of
// the flexibility provided, the images can be stored in a variety of
// RGB and YUV formats, in top-down or bottom-up row ordering, and with
// or without 4-byte row padding.
// .SECTION See Also
// vtkVideoBuffer2 vtkVideoSource2 vtkWin32VideoSource2 vtkMILVideoSource2

#ifndef __vtkVideoFrame2_h
#define __vtkVideoFrame2_h

#include "vtkObject.h"

// Standard values defined in vtkSystemIncludes.h, these constants
// are equal to their OpenGL equivalents.  These are the formats
// that vtkImageData can support.
// #define VTK_LUMINANCE     1
// #define VTK_LUMINANCE_ALPHA 2
// #define VTK_RGB           3
// #define VTK_RGBA          4

// New values that can be used as the PixelFormat in a vtkVideoFrame2.
// These are common formats supported by frame grabbers.
// The constant values are based on OpenGL extensions.
#define VTK_BGR           0x80E0   // equal to OpenGL BGR_EXT 
#define VTK_BGRA          0x80E1   // equal to OpenGL BGRA_EXT
#define VTK_UYVY          0xC012   // 422 style 16-bit YUV
#define VTK_BI_UYVY       0x59565955 // from vtkWin32VideoSource
// #define VTK_YUV           0xC003   // 444 style YUV
// #define VTK_YUVA          0xC004   // 4444 style YUVA

// Frame grabber type is used by UnpackRasterLine
#define FG_BASE    1
#define FG_MIL    2
#define FG_WIN32  3


class VTK_EXPORT vtkVideoFrame2 : public vtkObject
{
public:
  static vtkVideoFrame2 *New();
  vtkTypeRevisionMacro(vtkVideoFrame2,vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Set/Get the full-frame size.  This must be an allowed size for the
  // device, the device may either refuse a request for an illegal frame
  // size or automatically choose a new frame size.
  // The default is usually 320x240x1, but can be device specific.  
  // The 'depth' should always be 1 (unless you actually have a device that
  // can handle 3D acquisition).
  virtual void SetFrameSize(int x, int y, int z);
  virtual void SetFrameSize(int xyz[3]) {
    this->SetFrameSize(xyz[0], xyz[1], xyz[2]); };
  vtkGetVector3Macro(FrameSize, int);

  // Description:
  // Set/Get the extent of the frame after it has been clipped.  The default
  // is [0, 319, 0, 239, 0, 0] but can be device specific.  The 'depth'
  // should always be [0,0], unless you actually have a device that can handle
  // 3D acquisition).
  vtkSetVector6Macro(FrameExtent, int);
  vtkGetVector6Macro(FrameExtent, int);

  // Description:
  // Set/Get a constant that describes the format of the pixels stored in the
  // frame (VTK_LUMINANCE, VTK_LUMINANCE_ALPHA, VTK_RGB or VTK_RGBA).  The
  // default is VTK_LUMINANCE.
  vtkSetMacro(PixelFormat, unsigned int);
  vtkGetMacro(PixelFormat, unsigned int);

  // Description:
  // Set/Get the number of bits required to store one pixel (this must be
  // divisible by 8).  The default is 8.
  virtual void SetBitsPerPixel(int bpp);
  vtkGetMacro(BitsPerPixel, int);

  // Description:
  // Set/Get the number of bytes in each row of the image is padded such that it
  // is divisible by the RowAlignment (this must be either 1 or 4).  A
  // RowAlignment of 4 is generally preferable, because video compressors
  // often require a row alignment of 4.  The default is 1.
  virtual void SetRowAlignment(int row);
  vtkGetMacro(RowAlignment, int);

  // Description:
  // Specify that the frame is stored top-down, instead of the
  // default bottom-up.  When the frame is copied to a vtkImageData,
  // the result will always be stored bottom-up.
  vtkSetMacro(TopDown, int);
  vtkGetMacro(TopDown, int);
  vtkBooleanMacro(TopDown, int);
  
  // Description:
  // For RGBA output only (4 scalar components), set/get the opacity.  This
  // will not modify the existing contents of the framebuffer, only
  // subsequently grabbed frames.  The default is 1.0.
  vtkSetMacro(Opacity,float);
  vtkGetMacro(Opacity,float);

  // Description:
  // For vtkWin32VideoSource2
  vtkSetMacro(Compression,int);
  vtkGetMacro(Compression,int);

  // The frame grabber type determines how the raster line is unpacked
  vtkSetMacro(FrameGrabberType,int);
  vtkGetMacro(FrameGrabberType,int);

  // Description:
  // Allocate enough memory to hold the frame.  This should be called
  // after the FrameExtent, BitsPerPixel, and RowAlignment have been set.
  // It is the user's responsiblity to call Allocate() after all of the
  // parameters have been set, and again after any subsequent changes to
  // the parameters.
  virtual int Allocate();

  // Description:
  // Tell the VideoFrame to use the specified memory array, instead
  // of its own.  The memorySize must be set to the amount of memory
  // required for one frame, in bytes.  If "save" is set, then the video
  // frame will not delete the memory when it is destructed.  The class
  // uses the actual array provided; it does not copy the data from the
  // supplied array.
  virtual void SetVoidArray(void *array, vtkIdType memorySize, int save);

  // Description:
  // The number of bytes in the whole frame, including any padding for row
  // alignment.  This is set automatically when Allocate() or SetVoidArray()
  // is called.
  virtual vtkIdType GetBytesInFrame() { return this->BytesInFrame; };

  // Description:
  // Get a void pointer to the ith pixel in the frame, starting at bottom
  // row of the frame.  This is a tricky method to use for UYVY since
  // you must consider whether you are starting on a UY or a VY pixel.
  virtual void *GetVoidPointer(vtkIdType i);

  // Description:
  // Copy the data into a new array (e.g. into an allocated vtkImageData),
  // performing any necessary conversion.  ClipExtent is the clip region to
  // apply to the frame's array before it is copied to the new array.  ArrayExtent
  // should be the whole extent for the new array. The output array must be a
  // byte array and the array color format must be VTK_LUMINANCE,
  // VTK_RGB or VTK_RGBA.  Supported BitsPerPixel/arrayFormat combinations
  // are as follows:
  // BitsPerPixel = 8: VTK_LUMINANCE, VTK_RGB, VTK_RGBA
  // BitsPerPixel = 16, PixelFormat = UYVY: VTK_LUMINANCE, VTK_RGB, VTK_RGBA
  // BitsPerPixel = 16, PixelFormat = BGR or BGRA: VTK_RGB, VTK_RGBA
  // BitsPerPixel = 24: VTK_LUMINANCE, VTK_RGB, VTK_RGBA
  // BitsPerPixel = 32: VTK_LUMINANCE, VTK_RGB, VTK_RGBA
  // If the new array pointer points to the first index and the ArrayExtent is 
  // smaller than ClipRegion, the frame's array will be further cliped to the size
  // specified by ArrayExtent.
  virtual bool CopyData(void *array, const int clipRegion[6], const int arrayExtent[6],
                int arrayFormat);

  // Description:
  // Create an empty frame with the same information as this one.
  vtkVideoFrame2 *MakeObject();

protected:
  vtkVideoFrame2();
  ~vtkVideoFrame2();  

  // Description:
  // Used by CopyData().  If some component conversion is required, it is
  // done here.
  virtual void UnpackRasterLine(unsigned char *outptr, unsigned char *inptr, 
                        int start, int count, int outputFormat);

  // Description:
  // Allocate a frame consisting of the specified number of bytes
  // (if possible, this will re-use the existing data space).
  virtual int Allocate(vtkIdType size);

  // Description:
  // Calculates the amount of memory based on the frame extent, pixel format
  // and row alignment.
  virtual vtkIdType CalculateActualMemorySize();

  int FrameSize[3];
  int FrameExtent[6];
  unsigned int PixelFormat;
  int BitsPerPixel;
  int RowAlignment;
  int TopDown;
  float Opacity;
  // total number of bytes in the frame
  vtkIdType BytesInFrame;
  unsigned char *Array;
  // for user-supplied arrays
  int SaveUserArray;
  int Compression; // for Win32VideoSource
  int FrameGrabberType;

private:
  vtkVideoFrame2(const vtkVideoFrame2&);  // Not implemented.
  void operator=(const vtkVideoFrame2&);  // Not implemented.
};

#endif
