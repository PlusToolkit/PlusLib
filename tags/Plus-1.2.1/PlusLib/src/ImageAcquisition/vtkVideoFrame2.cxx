/*=========================================================================

Program:   Visualization Toolkit
Module:    $RCSfile: vtkVideoFrame2.cxx,v $

Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
All rights reserved.
See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

Author: Danielle Pace
Robarts Research Institute and The University of Western Ontario

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information. 

=========================================================================*/
#include "PlusConfigure.h"
#include "vtkVideoFrame2.h"
#include "vtkObjectFactory.h"

// -----------------------------------------------------
// for reference, the BITMAPINFOHEADER for Windows is like so:
// bih.biSize = sizeof(BITMAPINFOHEADER); 
// bih.biWidth = this->FrameSize[0];
// bih.biHeight = this->FrameSize[1]; 
// bih.biPlanes = 1; 
// bih.biCompression = this->FourCC;
// bih.biBitcount = this->BitsPerPixel;
// bih.biSizeImage = this->ByteCount; 
// bih.biXPelsPerMeter = 0;
// bih.biYPelsPerMeter = 0; 
// bih.biClrUsed = 256;
// bih.biClrImportant = 256; 

// -----------------------------------------------------
// FOURCC info: (also see http://www.webartz.com/fourcc/)

// ============ info on the various RGB modes ===========

// --- info on BI_RGB and RGB mode (the two modes are identical) ---

// RGB 32 bit is 8:8:8:8 B:G:R:X when data is read byte-by-byte.
// If data is read in 32-bit chunks, it is X:R:G:B on little-endian
// and B:G:R:X on big-endian.
// This is also a very safe mode (the X is undefined, usually zero).

// RGB 24 bit is 8:8:8 B:G:R when data is read byte-by-byte,
// and each row is padded to a 32-bit boundary.
// This is the most common uncompressed mode.

// RGB 16 bit is 1:5:5:5 X:R:G:B on little-ending when read in 16-bit chunks,
// must be byte swapped on big-endian.  I assume that rows must be padded
// to 32-bit boundaries.  The 5:6:5 format is not supported. 

// RGB 1, 2, 4, and 8 bit (palettized modes) are not supported.

// --- info on RGBT mode ---

// RGBT 32-bit mode is like RGB mode, except the 'X' is zero to indicate
// transparency or set to indicate opacity.

// --- info on RGBA mode ---

// RGBA 32 bit is 8:8:8:8 B:G:R:A when read byte-by-byte or when read
// as 32 bit chunks on big-endian, and is A:R:G:B when read as 32-bit
// chunks on little-endian.  Consider using 32-bit RGB instead, unless
// you really need to preserve the alpha component.  

// RGBA 16-bit is not supported.

// --- info on BI_BITFIELDS ---

// I have chosen not to support this mode for now, and instead just
// support the most popular ordering of R:G:B components for each
// format.

// ============ info on the YUV modes ===========

// Only 16-bit YUV modes are supported, 12-bit and 24-bit modes are not.
// Planar YUV modes are not supported, neither are interlaced modes.
// In fact, only the most popular mode is supported at this time:

// UYVY is top-down by default (starting at the top of the frame, not
// the bottom like most DIB formats).  The pixels should always be
// arranged in pairs, one U:Y 8:8 pixel followed by a V:Y 8:8 pixel.
// The UV values are used together to set the color for both pixels.

// Most because video compression codecs use YUV almost exclusively
// as their internal format, codecs can create & compress UYVY data
// faster than RGB data.

// ============ info on the compressed modes ===========

// No compressed modes are supported at this time.  The only one
// that is likely to be supported in the future is MJPG (motion-JPEG)
// because it is a format that is supported by many frame grabbers.

vtkCxxRevisionMacro(vtkVideoFrame2, "$Revision: 1.2 $");
vtkStandardNewMacro(vtkVideoFrame2);

//----------------------------------------------------------------------------
vtkVideoFrame2::vtkVideoFrame2()
{
    this->FrameSize[0] = 320;
    this->FrameSize[1] = 240;
    this->FrameSize[2] = 1;

    this->FrameExtent[0] = 0;
    this->FrameExtent[1] = 319;
    this->FrameExtent[2] = 0;
    this->FrameExtent[3] = 239;
    this->FrameExtent[4] = 0;
    this->FrameExtent[5] = 0;

    this->PixelFormat = VTK_LUMINANCE;
    this->BitsPerPixel = 8; 
    this->RowAlignment = 1;
    this->TopDown = 0;

    this->BytesInFrame = 0;
    this->Array = NULL;
    this->SaveUserArray = 0;
    this->Opacity = 1.0;
    this->Compression = 0;
    this->FrameGrabberType = FG_BASE;
}

//----------------------------------------------------------------------------
// Deletes the array if SaveUserArray is set to false.
vtkVideoFrame2::~vtkVideoFrame2()
{
    if ((this->Array) && (!this->SaveUserArray))
    {
        delete [] this->Array;
    }
}

//----------------------------------------------------------------------------
void vtkVideoFrame2::PrintSelf(ostream& os, vtkIndent indent)
{
    int idx;

    this->Superclass::PrintSelf(os,indent);

    os << indent << "FrameSize: (" << this->FrameSize[0];
    for (idx = 1; idx < 3; ++idx)
    {
        os << ", " << this->FrameSize[idx];
    }
    os << ")\n";

    os << indent << "FrameExtent: (" << this->FrameExtent[0];
    for (idx = 1; idx < 6; ++idx)
    {
        os << ", " << this->FrameExtent[idx];
    }
    os << ")\n";

    os << indent << "PixelFormat: " <<
        (this->PixelFormat == VTK_RGBA ? "RGBA" :
        (this->PixelFormat == VTK_RGB ? "RGB" :
        (this->PixelFormat == VTK_LUMINANCE_ALPHA ? "LuminanceAlpha" :
        (this->PixelFormat == VTK_LUMINANCE ? "Luminance" : "Unknown"))))
        << "\n";

    os << indent << "BitsPerPixel: " << this->BitsPerPixel << "\n";

    os << indent << "RowAlignment: " << this->RowAlignment << "\n";

    os << indent << "TopDown: " << (this->TopDown ? "On\n" : "Off\n");

    os << indent << "BytesInFrame: " << this->BytesInFrame << "\n";

    os << indent << "SaveUserArray: " << (this->SaveUserArray ? "On\n" : "Off\n");

    os << indent << "Opacity: " << this->Opacity << "\n";

    os << indent << "FrameGrabberType: " <<
        (this->FrameGrabberType == FG_BASE ? "BASE" :
        (this->FrameGrabberType == FG_MIL ? "MIL" :
        (this->FrameGrabberType == FG_WIN32 ? "WIN32" : "Unknown")));
}

//----------------------------------------------------------------------------
void vtkVideoFrame2::SetRowAlignment(int row)
{
    if (row == 1 || row == 4)
    {
        this->RowAlignment = row;
    }
    else
    {
        LOG_ERROR("SetRowAlignment:  Row alignment must be either 1 or 4");
    }
}

//----------------------------------------------------------------------------
void vtkVideoFrame2::SetFrameSize (int x, int y, int z)
{
    if (x < 1 || y < 1 || z < 1)
    {
        LOG_ERROR("SetFrameSize: Illegal frame size");
        return;
    }

    this->FrameSize[0] = x;
    this->FrameSize[1] = y;
    this->FrameSize[2] = z;
}

//----------------------------------------------------------------------------
void vtkVideoFrame2::SetBitsPerPixel(int bpp)
{
    if (bpp > 0)
    {
        this->BitsPerPixel = bpp;
    }
}

//----------------------------------------------------------------------------
// Allocate memory for this array. Delete old storage only if necessary.
// This should only be used when the memory size is different from that
// specified by the frame extent, pixel format and row alignment.
int vtkVideoFrame2::Allocate(vtkIdType size)
{
    if (size > this->BytesInFrame || this->SaveUserArray)
    {
        if ((this->Array) && (!this->SaveUserArray))
        {
            delete [] this->Array;
        }

        // make the new array and check to make sure that it was
        // created properly
        if ((this->Array = new unsigned char[size]) == NULL)
        {
            return 0;
        }
    }

    this->BytesInFrame = size;

    return 1;
}

//----------------------------------------------------------------------------
// Allocate memory based on the frame size, pixel format and row alignment.
int vtkVideoFrame2::Allocate()
{
    vtkIdType size = this->CalculateActualMemorySize();
    return this->Allocate(size);
}

//----------------------------------------------------------------------------
// Calculate the memory required to store one frame based on the frame size,
// pixel format and row alignment.
vtkIdType vtkVideoFrame2::CalculateActualMemorySize()
{
    vtkIdType size;

    // number of bytes required to hold a row of pixels
    size = ((this->FrameExtent[1] - this->FrameExtent[0] + 1)*this->BitsPerPixel + 7)/8;
    // pad as required
    size = (size + this->RowAlignment - 1)/this->RowAlignment*this->RowAlignment;
    // get the size of a frame
    size *= (this->FrameExtent[3] - this->FrameExtent[2]+1);
    size *= (this->FrameExtent[5] - this->FrameExtent[4]+1);

    return size;
}

//----------------------------------------------------------------------------
// Makes a new videoFrame2 with the same attributes as this one, but with no data
vtkVideoFrame2 *vtkVideoFrame2::MakeObject()
{
    vtkVideoFrame2 *a = vtkVideoFrame2::New();
    a->SetFrameSize(this->FrameSize);
    a->SetFrameExtent(this->FrameExtent);
    a->SetPixelFormat(this->PixelFormat);
    a->SetBitsPerPixel(this->BitsPerPixel);
    a->SetRowAlignment(this->RowAlignment);
    a->SetTopDown(this->TopDown);
    a->SetOpacity(this->Opacity);
    a->SetCompression(this->Compression);
    a->SetFrameGrabberType(this->FrameGrabberType);

    return a;
}

//----------------------------------------------------------------------------
void vtkVideoFrame2::DeepCopy(vtkVideoFrame2* frame)
{
    this->SetFrameSize( frame->GetFrameSize() ); 
    this->SetFrameExtent( frame->GetFrameExtent() ); 
    this->SetPixelFormat( frame->GetPixelFormat() ); 
    this->SetBitsPerPixel( frame->GetBitsPerPixel() ); 
    this->SetRowAlignment( frame->GetRowAlignment() ); 
    this->SetTopDown( frame->GetTopDown() ); 
    this->SetOpacity( frame->GetOpacity() ); 
    this->SetCompression( frame->GetCompression() ); 
    this->SetFrameGrabberType( frame->GetFrameGrabberType() ); 

    this->Allocate(); 

    if ( frame->Array != NULL )
    {
        if (!frame->CopyData(this->GetVoidPointer(0), this->GetFrameExtent(), this->GetFrameExtent(), this->GetPixelFormat()))
        {
            LOG_ERROR("Cannot copy video frame data!");
        }
    }
}

//----------------------------------------------------------------------------
bool vtkVideoFrame2::CheckFrameFormat( vtkVideoFrame2* frameFormat )
{
	if ( frameFormat == NULL )
	{
		//LOG_DEBUG("Frame format and buffer frame format does not match!"); 
		return false;
	}

	int frameSize[3]={0};
	int frameExtent[6]={0};
	this->GetFrameSize(frameSize);
	this->GetFrameExtent(frameExtent);

	int frameFormatSize[3]={0};
	int frameFormatExtent[6]={0};
	frameFormat->GetFrameSize(frameFormatSize);
	frameFormat->GetFrameExtent(frameFormatExtent);

	if (frameSize[0] == frameFormatSize[0] &&
		frameSize[1] == frameFormatSize[1] &&
		frameSize[2] == frameFormatSize[2] &&
		frameExtent[0] == frameFormatExtent[0] &&
		frameExtent[1] == frameFormatExtent[1] &&
		frameExtent[2] == frameFormatExtent[2] &&
		frameExtent[3] == frameFormatExtent[3] &&
		frameExtent[4] == frameFormatExtent[4] &&
		frameExtent[5] == frameFormatExtent[5] &&
		frameFormat->GetPixelFormat() == this->GetPixelFormat() &&
		frameFormat->GetBitsPerPixel() == this->GetBitsPerPixel() &&
		frameFormat->GetRowAlignment() == this->GetRowAlignment() &&
		frameFormat->GetTopDown() == this->GetTopDown() &&
		frameFormat->GetBytesInFrame() == this->GetBytesInFrame() &&
		frameFormat->GetOpacity() == this->GetOpacity() &&
		frameFormat->GetCompression() == this->GetCompression() &&
		frameFormat->GetFrameGrabberType() == this->GetFrameGrabberType() )
	{
		return true;
	}

	//LOG_DEBUG("Frame format and buffer frame format does not match!"); 
	return false;
}

//----------------------------------------------------------------------------
// This method lets the user specify the data to be held by the array.
// The array argument is a pointer to the data.  Size is the size of 
// the array supplied by the user (in bytes).  Set save to 1 to keep the class
// from deleting the array when it cleans up or reallocates memory.
void vtkVideoFrame2::SetVoidArray(void* newArray, vtkIdType size, int save)
{
    vtkIdType correctSize = this->CalculateActualMemorySize();
    if (size != correctSize)
    {
        LOG_ERROR("SetVoidArray: Size does not match frame attributes");
        return;
    }

    if ((this->Array) && (!this->SaveUserArray))
    {
        delete [] this->Array;
    }

    this->Array = (unsigned char *)newArray;
    this->BytesInFrame = size;
    this->SaveUserArray = save;
}

//----------------------------------------------------------------------------
// Get a pointer to a specific pixel
void *vtkVideoFrame2::GetVoidPointer(vtkIdType i)
{
    if (i == 0)
    {
        return this->Array;
    }

    if (i < 0)
    {
        return 0;
    }

    // divide bits by 8 to get bytes per pixel
    int bytesPerPixel = (this->BitsPerPixel+7) >> 3;
    // number of bytes required to hold a row of pixels
    vtkIdType bytesPerRow = (this->FrameExtent[1] - this->FrameExtent[0] + 1)*bytesPerPixel;
    // pad as required
    int alignment = this->RowAlignment;
    bytesPerRow = ((bytesPerRow + alignment - 1)/alignment)*alignment;

    // current row index into the image
    vtkIdType j = i / (this->FrameExtent[1] - this->FrameExtent[0] + 1);
    // current column index into the image
    i = i - j*(this->FrameExtent[1] - this->FrameExtent[0] + 1);

    // convert i into actual byte index into image
    i = j*bytesPerRow + i*bytesPerPixel; 

    return (void *)(((char *)this->Array) + i);
}

// Line order:
//  VTK format: LINES_BOTTOM_UP
//  ITK format: LINES_TOP_DOWN
//----------------------------------------------------------------------------
// Copies data from the frame's array to the array pointed to by arrayPtr.
bool vtkVideoFrame2::CopyData(void *arrayPtr, const int clipExtent[6],
                              const int outExtent[6],
                              int outFormat,
                              LineOrder requestedLineOrder/*=LINES_BOTTOM_UP*/)
{

    if (outFormat != VTK_LUMINANCE && outFormat != VTK_RGB && outFormat != VTK_RGBA)
    {
        LOG_ERROR("Output format " << outFormat << " is not supported");
        return false;
    }

    unsigned char *outPtr = reinterpret_cast<unsigned char *>(arrayPtr);
    unsigned char *inPtr = this->Array;
    if (outPtr==NULL)
    {
        LOG_ERROR("CopyData output is invalid");
        return false;
    }
    if (inPtr==NULL)
    {
        LOG_ERROR("CopyData input is invalid");
        return false;
    }

    int i, j;

    int inPadX = clipExtent[0];
    int inPadY = clipExtent[2];
    int inPadZ = clipExtent[4];

    // size of the clip region
    int outX = clipExtent[1] - clipExtent[0] + 1;
    int outY = clipExtent[3] - clipExtent[2] + 1;
    int outZ = clipExtent[5] - clipExtent[4] + 1;

    // size of the output extent
    int outExtentX = outExtent[1] - outExtent[0] + 1;
    int outExtentY = outExtent[3] - outExtent[2] + 1;
    int outExtentZ = outExtent[5] - outExtent[4] + 1;

    // if the image given is smaller than the clip region, then
    // we won't use all of the clip region
    if (outExtentX < outX)
    {
        outX = outExtentX;
    }
    if (outExtentY < outY)
    {
        outY = outExtentY;
    }
    if (outExtentZ < outZ)
    {
        outZ = outExtentZ;
    }

    // get input increments
    int inIncX = (this->BitsPerPixel + 7) >> 3;
    int frameExtentY = this->FrameExtent[1] - this->FrameExtent[0] + 1;
    int inIncY = frameExtentY*inIncX;
    int alignment = this->RowAlignment;
    inIncY = ((inIncY + alignment - 1)/alignment)*alignment;
    int inIncZ = inIncY*(this->FrameExtent[3] - this->FrameExtent[2]+1);

    // get output increments
    int outIncX = 1;
    if (outFormat == VTK_RGB)
    {
        outIncX = 3;
    }
    else if (outFormat == VTK_RGBA)
    {
        outIncX = 4;
    }
    int outIncY = outIncX*outExtentX;
    int outIncZ = outIncY*outExtentY;

    unsigned char *inPtrTmp;
    unsigned char *outPtrTmp;

    // a very small number of standard formats start at the top of the frame
    // (usually from frame grabbers), the rest all start at the bottom
    bool reverseLineOrder=false;
    if (this->TopDown && requestedLineOrder==LINES_BOTTOM_UP ||
        !this->TopDown && requestedLineOrder==LINES_TOP_DOWN)
    {
        reverseLineOrder=true;
    }

    if (reverseLineOrder)
    {
        // apply a vertical flip while copying to output
        //inPtr += inIncZ*inPadZ+inIncY*(frameExtentY-clipExtent[3]-1);
        inPtr += inIncZ*inPadZ+inIncY*inPadY;

        for (i = 0; i < outZ; i++)
        {
            inPtrTmp = inPtr;
            outPtrTmp = outPtr + outIncY*outY;
            for (j = 0; j < outY; j++)
            {
                outPtrTmp -= outIncY;
                if (outX > 0)
                {
                    this->UnpackRasterLine(outPtrTmp,inPtrTmp,inPadX,outX,outFormat);
                }
                inPtrTmp += inIncY;
            }
            outPtr += outIncZ;
            inPtr += inIncZ;
        }
    }
    else
    {// don't apply a vertical flip
        inPtr += inIncZ*inPadZ+inIncY*inPadY;

        for (i = 0; i < outZ; i++)
        {
            inPtrTmp = inPtr;
            outPtrTmp = outPtr;
            for (j = 0; j < outY; j++)
            {
                if (outX > 0) 
                {
                    this->UnpackRasterLine(outPtrTmp,inPtrTmp,inPadX,outX,outFormat);
                }
                outPtrTmp += outIncY;
                inPtrTmp += inIncY;
            }
            outPtr += outIncZ;
            inPtr += inIncZ;
        }
    }
    return true;
}

//----------------------------------------------------------------------------
// codecs
static inline void vtkYUVToRGB(unsigned char *yuv, unsigned char *rgb)
{ 
    /* // floating point 
    int Y = yuv[0] - 16;
    int U = yuv[1] - 128;
    int V = yuv[2] - 128;

    int R = 1.164*Y + 1.596*V           + 0.5;
    int G = 1.164*Y - 0.813*V - 0.391*U + 0.5;
    int B = 1.164*Y           + 2.018*U + 0.5;
    */

    // integer math
    int Y = (yuv[0] - 16)*76284;
    int U = yuv[1] - 128;
    int V = yuv[2] - 128;

    int R = Y + 104595*V           ;
    int G = Y -  53281*V -  25625*U;
    int B = Y            + 132252*U;

    // round
    R += 32768;
    G += 32768;
    B += 32768;

    // shift
    R >>= 16;
    G >>= 16;
    B >>= 16;

    // clamp, avoid using 'if' statements because they can
    // mess up the CPU pipeline if branches are mispredicted
    rgb[0] = (R & -(R > 0)) | -(R > 255);
    rgb[1] = (G & -(G > 0)) | -(G > 255);
    rgb[2] = (B & -(B > 0)) | -(B > 255);
}

//----------------------------------------------------------------------------
void vtkVideoFrame2::UnpackRasterLine(unsigned char *outptr,
                                      unsigned char *inptr, 
                                      int start, int count, int outputFormat)
{

    unsigned char alpha = (unsigned char)(this->Opacity * 255);
    int format = this->PixelFormat;
    int i;

    switch (this->BitsPerPixel)
    {
    case 1:
        {
            int rawBits;
            inptr += start/8;
            i = start % 8;
            while (count >= 0)
            { 
                rawBits = *inptr++;
                for (; i < 8 && --count >= 0; i++)
                {
                    *outptr++ = -((rawBits >> i) & 0x01);
                }
                i = 0;
            }
        }
        break;
    case 4:
        {
            int rawNibbles;
            inptr += start/2;
            i = start % 2;
            while (count >= 0)
            { 
                rawNibbles = *inptr++;
                for (; i < 8 && --count >= 0; i += 4)
                {
                    *outptr++ = ((rawNibbles >> i) & 0x0f) << 4;
                }
                i = 0;
            }
        }
        break;
    case 8:
        {
            inptr += start;
            switch (outputFormat)
            {
            case VTK_LUMINANCE:
                {
                    while (--count >= 0)
                    {
                        *outptr++ = *inptr++; // direct copy
                    }
                }
                break;
                // VTK_RGB and VTK_RGBA are grayscale when BitsPerPixel = 8
            case VTK_RGB:
                {
                    outptr += 3;
                    while (--count >= 0)
                    {
                        *--outptr = *inptr;
                        *--outptr = *inptr;
                        *--outptr = *inptr;
                        inptr++;
                        outptr += 6;
                    }
                }
                break;
            case VTK_RGBA:
                {
                    outptr += 4;
                    while (--count >= 0)
                    {
                        *--outptr = alpha;
                        *--outptr = *inptr;
                        *--outptr = *inptr;
                        *--outptr = *inptr;
                        inptr++;
                        outptr += 8;
                    }
                }
                break;
            }
        }
        break;
    case 16:
        {
            inptr += 2*start;
            if (format == VTK_UYVY || this->Compression == VTK_BI_UYVY)
            {
                switch (outputFormat)
                {
                case VTK_LUMINANCE:
                    { // unpack UY half-megapixel to one Y pixel
                        while (--count >= 0)
                        {
                            inptr++;
                            *outptr++ = *inptr++;
                        }            
                    }
                case VTK_RGB:
                case VTK_RGBA:
                    { // unpack UYVY megapixel to two RGB or RGBA pixels
                        unsigned char YUV[3];
                        //int finish = start + count;
                        int odd = (start % 2 == 1);
                        if (count > 0) { YUV[1+odd] = inptr[0]; }
                        if (count > 1) { YUV[0]     = inptr[1]; }
                        if (count > 2) { YUV[2-odd] = inptr[2]; }
                        while (--count >= 0)
                        {
                            YUV[1+odd] = *inptr++;
                            YUV[0] = *inptr++;
                            odd = !odd;
                            vtkYUVToRGB(YUV,(unsigned char *)outptr);
                            outptr += 3;
                            if (outputFormat == VTK_RGB) 
                            { 
                                continue; 
                            }
                            *outptr++ = alpha;
                        }
                    }
                }
            }
            else
            {
                unsigned short rawWord;
                unsigned short *shptr = (unsigned short *)inptr;
                switch (outputFormat)
                {
                case VTK_RGB:
                    { // unpack 16-bit 1:5:5:5 XRGB to 24 bit RGB
                        while (--count >= 0)
                        {
                            rawWord = *shptr++;
                            *outptr++ = (rawWord & 0x7c00) >> 7;
                            *outptr++ = (rawWord & 0x03e0) >> 2;
                            *outptr++ = (rawWord & 0x001f) << 3;
                        }
                    }
                    break;
                case VTK_RGBA:
                    { // unpack 16-bit 1:5:5:5 XRGB to 32 bit RGBA
                        while (--count >= 0)
                        {
                            rawWord = *shptr++;
                            *outptr++ = (rawWord & 0x7c00) >> 7;
                            *outptr++ = (rawWord & 0x03e0) >> 2;
                            *outptr++ = (rawWord & 0x001f) << 3;
                            *outptr++ = alpha;
                        }
                        break;
                    }
                }
            }
        }
    case 24:
        {
            inptr += 3*start;
            switch (outputFormat)
            {
            case VTK_RGB:
                {
                    switch (this->FrameGrabberType)
                    {
                    case FG_BASE:
                    case FG_MIL:
                        {
                            while (--count >= 0)
                            {
                                *outptr++ = *inptr++;
                                *outptr++ = *inptr++;
                                *outptr++ = *inptr++;
                            }
                        }
                        break;
                    case FG_WIN32:
                        {
                            // must do BGR to RGB conversion because BGR is default format for
                            // 24-bit framegrabbers
                            outptr += 3;
                            while (--count >= 0)
                            {
                                *--outptr = *inptr++;
                                *--outptr = *inptr++;
                                *--outptr = *inptr++;
                                outptr += 6;
                            }
                        }
                        break;
                    }
                }
                break;
            case VTK_RGBA:
                {
                    switch (this->FrameGrabberType)
                    {
                    case FG_BASE:
                    case FG_MIL:
                        {
                            while (--count >= 0)
                            {
                                *outptr++ = *inptr++;
                                *outptr++ = *inptr++;
                                *outptr++ = *inptr++;
                                *outptr++ = alpha;
                            }
                        }
                        break;
                    case FG_WIN32:
                        {
                            // must do BGR to RGBX conversion
                            outptr += 4;
                            while (--count >= 0)
                            {
                                *--outptr = alpha;
                                *--outptr = *inptr++;
                                *--outptr = *inptr++;
                                *--outptr = *inptr++;
                                outptr += 8;
                            }
                        }
                        break;
                    }
                }
                break;
            }
        }
        break;
    case 32:
        inptr += 4*start;
        switch (outputFormat)
        {
        case VTK_RGB:
            { // must do BGRX to RGB conversion
                outptr += 3;
                while (--count >= 0)
                {
                    *--outptr = *inptr++;
                    *--outptr = *inptr++;
                    *--outptr = *inptr++;
                    inptr++; // skip over the X component in the input
                    outptr += 6;
                }
            }
            break;
        case VTK_RGBA:
            {
                switch (this->FrameGrabberType)
                {
                case FG_BASE:
                case FG_MIL:
                    {
                        while (--count >= 0)
                        {
                            *outptr++ = *inptr++;
                            *outptr++ = *inptr++;
                            *outptr++ = *inptr++;
                            *outptr++ = 255;
                        }
                    }
                    break;
                case FG_WIN32:
                    {
                        // must do BGR to RGBX conversion
                        outptr += 4;
                        while (--count >= 0)
                        {
                            *--outptr = alpha;
                            *--outptr = *inptr++;
                            *--outptr = *inptr++;
                            *--outptr = *inptr++;
                            outptr += 8;
                        }
                    }
                    break;
                }
            }
            break;
        }
        break;
    }
}
