/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

/*!
  YUY2 conversion to RGB24 or grayscale.
  YUY2 coding is typically used for webcams
  source: http://sundararajana.blogspot.ca/2007/12/yuy2-to-rgb24-conversion.html
*/

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

bool YUV422P_to_RGB24V2(int width, int height, unsigned char *s,unsigned char *d)
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

  try
  {

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


      p_dest[dstIndex] = b;
      p_dest[dstIndex + 1] = g;
      p_dest[dstIndex + 2] = r;




      dstIndex += 3;

      r = CLIP(GET_R_FROM_YUV(Y2, U, V));
      g = CLIP(GET_G_FROM_YUV(Y2, U, V));
      b = CLIP(GET_B_FROM_YUV(Y2, U, V));

      p_dest[dstIndex] = b;
      p_dest[dstIndex + 1] = g;
      p_dest[dstIndex + 2] = r;

      dstIndex += 3;

      srcIndex += 4;
    }

    return true;
  }
  catch(...)
  {
    OutputDebugString("\n YUV422P to RGB24V2 Failed");
    return false;
  }

} 


bool YUV422P_to_gray(int width, int height, unsigned char *s,unsigned char *d)
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

  try
  {

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

    return true;
  }
  catch(...)
  {
    OutputDebugString("\n YUV422P to RGB24V2 Failed");
    return false;
  }

} 
