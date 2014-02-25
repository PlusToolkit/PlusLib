/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __PixelCodec_h
#define __PixelCodec_h

#include "PlusConfigure.h"

// Helper macros for YUY2 conversion (source: http://sundararajana.blogspot.ca/2007/12/yuy2-to-rgb24-conversion.html)
#define FIXNUM 16
#define FIX(a, b) ((int)((a)*(1<<(b))))
#define UNFIX(a, b) ((a+(1<<(b-1)))>>(b))
// Approximate 255 by 256
#define ICCIRUV(x) (((x)<<8)/224)
#define ICCIRY(x) ((((x)-16)<<8)/219)
// Clip out-range values
#define CLIP(t) (((t)>255)?255:(((t)<0)?0:(t)))
#define GET_R_FROM_YUV(y, u, v) UNFIX((FIX(1.0, FIXNUM)*(y) + FIX(1.402, FIXNUM)*(v)), FIXNUM)
#define GET_G_FROM_YUV(y, u, v) UNFIX((FIX(1.0, FIXNUM)*(y) + FIX(-0.344, FIXNUM)*(u) + FIX(-0.714, FIXNUM)*(v)), FIXNUM)
#define GET_B_FROM_YUV(y, u, v) UNFIX((FIX(1.0, FIXNUM)*(y) + FIX(1.772, FIXNUM)*(u)), FIXNUM)
#define GET_Y_FROM_RGB(r, g, b) UNFIX((FIX(0.299, FIXNUM)*(r) + FIX(0.587, FIXNUM)*(g) + FIX(0.114, FIXNUM)*(b)), FIXNUM)
#define GET_U_FROM_RGB(r, g, b) UNFIX((FIX(-0.169, FIXNUM)*(r) + FIX(-0.331, FIXNUM)*(g) + FIX(0.500, FIXNUM)*(b)), FIXNUM)
#define GET_V_FROM_RGB(r, g, b) UNFIX((FIX(0.500, FIXNUM)*(r) + FIX(-0.419, FIXNUM)*(g) + FIX(-0.081, FIXNUM)*(b)), FIXNUM)

// VFW compressed formats are listed at http://www.webartz.com/fourcc/
static const long VTK_BI_UYVY=0x59565955;
static const long VTK_BI_YUY2=0x32595559;

/*!
\class PixelCodec
\brief A utility class that contains static functions for converting between various pixel encodings
\ingroup PlusLibCommon
*/
class VTK_EXPORT PixelCodec
{
public:
  enum ComponentOrdering
  {
    ComponentOrder_RGB,
    ComponentOrder_BGR
  };

  enum PixelEncoding
  {
    PixelEncoding_ERROR,
    PixelEncoding_YUY2,
    PixelEncoding_BMP,
    PixelEncoding_MJPG
  };

  //----------------------------------------------------------------------------
  static bool IsConvertToGraySupported(int inputCompression)
  {
    switch (inputCompression)
    {  
    case VTK_BI_YUY2: return true;
    case BI_RGB: return true;
    case BI_JPEG: return true;
    default:
      return false;
    }
  }

  //----------------------------------------------------------------------------
  static bool IsConvertToGraySupported(PixelEncoding inputCompression)
  {
    switch (inputCompression)
    {  
    case PixelEncoding_BMP: return true;
    case PixelEncoding_YUY2: return true;
    case PixelEncoding_MJPG: return true;
    default:
      return false;
    }
  }

  //----------------------------------------------------------------------------
  static std::string GetCompressionModeAsString(int inputCompression)
  {    
    char fourccHex[16]={0};
    sprintf(fourccHex,"0x%08x",inputCompression);
    std::string fourcc="????";
    for (int i = 0; i < 4; i++)
    {
      fourcc[i] = (unsigned char)(inputCompression >> (8*i)) & 0xff;
      if (!isprint(fourcc[i]))
      {
        fourcc[i] = '?';
      }
    }
    std::string output=fourcc+"("+std::string(fourccHex)+")";
    return output;
  }

  //----------------------------------------------------------------------------
  static std::string GetCompressionModeAsString(PixelEncoding inputCompression)
  {    
    switch(inputCompression)
    {
    case PixelEncoding_BMP:
      return "BMP";
      break;
    case PixelEncoding_YUY2:
      return "YUY2";
      break;
    case PixelEncoding_MJPG:
      return "MJPG";
      break;
    default:
      LOG_ERROR("Unknown pixel format.");
      return "Unknown";
    }
  }

  //----------------------------------------------------------------------------
  static inline PlusStatus ConvertToGray(int inputCompression, int width, int height, unsigned char *s,unsigned char *d)
  {
    switch (inputCompression)
    {
    case BI_RGB:
      // decode the grabbed image to the requested output image type
      Rgb24ToGray(width, height, s, d);
      break;
    case VTK_BI_YUY2:
      // decode the grabbed image to the requested output image type
      Yuv422pToGray(width, height, s, d);
      break;
    case BI_JPEG:
      // TODO
      break;
    default:
      LOG_ERROR("Unknown compression type: "<<inputCompression);
      return PLUS_FAIL;
    }
    return PLUS_SUCCESS;
  }

  //----------------------------------------------------------------------------
  static inline PlusStatus ConvertToGray(PixelEncoding inputCompression, int width, int height, unsigned char *s,unsigned char *d)
  {
    switch (inputCompression)
    {
    case PixelEncoding_BMP:
      // decode the grabbed image to the requested output image type
      Rgb24ToGray(width, height, s, d);
      break;
    case PixelEncoding_YUY2:
      // decode the grabbed image to the requested output image type
      Yuv422pToGray(width, height, s, d);
      break;
    case PixelEncoding_MJPG:
      // TODO
      break;
    default:
      LOG_ERROR("Unknown compression type: "<<inputCompression);
      return PLUS_FAIL;
    }
    return PLUS_SUCCESS;
  }

  //----------------------------------------------------------------------------
  static inline PlusStatus ConvertToBmp24(ComponentOrdering outputOrdering, PixelEncoding inputCompression, int width, int height, unsigned char *s, unsigned char *d)
  {
    switch (inputCompression)
    {
    case PixelEncoding_BMP:
      // Nothing to do, copy out
      memcpy(d, s, width*height*3);
      return PLUS_SUCCESS;
      break;
    case PixelEncoding_YUY2:
      // decode the grabbed image to the requested output image type
      return Yuv422pToBmp24(outputOrdering, width, height, s, d);
      break;
    case PixelEncoding_MJPG:
      break;
    default:
      LOG_ERROR("Unknown compression type: " << inputCompression);
      return PLUS_FAIL;
    }
    return PLUS_SUCCESS;
  }

  //----------------------------------------------------------------------------
  /*!
  Convert from RGB24 to grayscale
  Note that this method computes the intensity (simple averaging of the RGB components).
  This is not equivalent with the perceived luminance of color images (e.g., 0.21R + 0.72G + 0.07B or 0.30R + 0.59G + 0.11B)
  */
  static inline void Rgb24ToGray(int width, int height, unsigned char *s,unsigned char *d)
  {
    int totalLen=width*height;
    for (int i=0; i<totalLen; i++)
    {
      *d=((unsigned short)(s[0])+s[1]+s[2])/3;
      d++;
      s+=3;
    }
  } 

  //----------------------------------------------------------------------------
  /*! Conversion from YUV to RGB space
  Uses integer math, which is faster but more complex to read
  Equivalent with the following floating point math (simpler but slower)
  int Y = yuv[0] - 16;
  int U = yuv[1] - 128;
  int V = yuv[2] - 128;

  int R = 1.164*Y + 1.596*V           + 0.5;
  int G = 1.164*Y - 0.813*V - 0.391*U + 0.5;
  int B = 1.164*Y           + 2.018*U + 0.5;
  */
  static inline void YuvToRgbPixel(ComponentOrdering outputOrdering, unsigned char *yuv, unsigned char *rgb)
  {   
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

    // clamp
    if (R < 0) { R = 0; }
    if (G < 0) { G = 0; }
    if (B < 0) { B = 0; }

    if (R > 255) { R = 255; };
    if (G > 255) { G = 255; };
    if (B > 255) { B = 255; };

    // output
    rgb[0] = (outputOrdering == ComponentOrder_BGR ? B : R);
    rgb[1] = G;
    rgb[2] = (outputOrdering == ComponentOrder_BGR ? R : B);
  }

  //----------------------------------------------------------------------------
  /*!
  YUY2 conversion to RGB24.
  YUY2 coding is typically used for webcams
  source: http://sundararajana.blogspot.ca/2007/12/yuy2-to-rgb24-conversion.html
  */
  static PlusStatus Yuv422pToBmp24(ComponentOrdering outputOrdering, int width, int height, unsigned char *s,unsigned char *d)
  {
    int i;
    unsigned char *p_dest;
    unsigned char y1, u, y2, v;
    int Y1, Y2, U, V;
    unsigned char r, g, b;

    p_dest = d;

    int size = height * (width / 2);
    unsigned long srcIndex = 0;
    unsigned long dstIndex = 0;

    for(i = 0 ; i < size ; i++)
    {
      y1 = s[srcIndex];
      u = s[srcIndex+ 1];
      y2 = s[srcIndex+ 2];
      v = s[srcIndex+ 3];

      Y1 = ICCIRY(y1);
      U = ICCIRUV(u - 128);
      Y2 = ICCIRY(y2);
      V = ICCIRUV(v - 128);

      r = CLIP(GET_R_FROM_YUV(Y1, U, V));
      g = CLIP(GET_G_FROM_YUV(Y1, U, V));
      b = CLIP(GET_B_FROM_YUV(Y1, U, V));

      p_dest[dstIndex] = (outputOrdering == ComponentOrder_BGR ? b : r);
      p_dest[dstIndex + 1] = g;
      p_dest[dstIndex + 2] = (outputOrdering == ComponentOrder_BGR ? r : b);

      dstIndex += 3;

      r = CLIP(GET_R_FROM_YUV(Y2, U, V));
      g = CLIP(GET_G_FROM_YUV(Y2, U, V));
      b = CLIP(GET_B_FROM_YUV(Y2, U, V));

      p_dest[dstIndex] = (outputOrdering == ComponentOrder_BGR ? b : r);
      p_dest[dstIndex + 1] = g;
      p_dest[dstIndex + 2] = (outputOrdering == ComponentOrder_BGR ? r : b);

      dstIndex += 3;
      srcIndex += 4;
    }

    return PLUS_SUCCESS;
  }

  //----------------------------------------------------------------------------
  /*!
  YUY2 conversion to grayscale.
  YUY2 coding is typically used for webcams
  source: http://sundararajana.blogspot.ca/2007/12/yuy2-to-rgb24-conversion.html
  */
  static void Yuv422pToGray(int width, int height, unsigned char *s,unsigned char *d)
  {
    int i;
    unsigned char *p_dest;
    unsigned char y1, u, y2, v;
    int Y1, Y2, U, V;
    unsigned char r, g, b;

    p_dest = d;

    int size = height * (width / 2);
    unsigned long srcIndex = 0;
    unsigned long dstIndex = 0;

    for(i = 0 ; i < size ; i++)
    {
      y1 = s[srcIndex];
      u = s[srcIndex+ 1];
      y2 = s[srcIndex+ 2];
      v = s[srcIndex+ 3];

      Y1 = ICCIRY(y1);
      U = ICCIRUV(u - 128);
      Y2 = ICCIRY(y2);
      V = ICCIRUV(v - 128);

      r = CLIP(GET_R_FROM_YUV(Y1, U, V));
      g = CLIP(GET_G_FROM_YUV(Y1, U, V));
      b = CLIP(GET_B_FROM_YUV(Y1, U, V));

      p_dest[dstIndex] = (int(b)+g+r)/3;
      dstIndex ++;

      r = CLIP(GET_R_FROM_YUV(Y2, U, V));
      g = CLIP(GET_G_FROM_YUV(Y2, U, V));
      b = CLIP(GET_B_FROM_YUV(Y2, U, V));

      p_dest[dstIndex] = (int(b)+g+r)/3;

      dstIndex ++;
      srcIndex += 4;
    }
  }  

private:
  PixelCodec(); // prevent instantiation
};


#endif  //__PixelCodec_h
