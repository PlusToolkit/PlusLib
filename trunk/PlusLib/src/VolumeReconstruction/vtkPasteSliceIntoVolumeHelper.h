/*=========================================================================

Copyright (c) 2000-2007 Atamai, Inc.
Copyright (c) 2008-2009 Danielle Pace

Use, modification and redistribution of the software, in source or
binary forms, are permitted provided that the following terms and
conditions are met:

1) Redistribution of the source code, in verbatim or modified
form, must retain the above copyright notice, this license,
the following disclaimer, and any notices that refer to this
license and/or the following disclaimer.

2) Redistribution in binary form must include the above copyright
notice, a copy of this license and the following disclaimer
in the documentation or with other materials provided with the
distribution.

3) Modified copies of the source code must be clearly marked as such,
and must not be misrepresented as verbatim copies of the source code.

THE COPYRIGHT HOLDERS AND/OR OTHER PARTIES PROVIDE THE SOFTWARE "AS IS"
WITHOUT EXPRESSED OR IMPLIED WARRANTY INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE.  IN NO EVENT SHALL ANY COPYRIGHT HOLDER OR OTHER PARTY WHO MAY
MODIFY AND/OR REDISTRIBUTE THE SOFTWARE UNDER THE TERMS OF THIS LICENSE
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, LOSS OF DATA OR DATA BECOMING INACCURATE
OR LOSS OF PROFIT OR BUSINESS INTERRUPTION) ARISING IN ANY WAY OUT OF
THE USE OR INABILITY TO USE THE SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGES.

=========================================================================*/
// .NAME vtkFreehandUltrasound2 - real-time freehand ultrasound reconstruction
// .SECTION Description
// vtkFreehandUltrasound2 will incrementally compound ultrasound images into a
// reconstruction volume, given a transform which specifies the location of
// each ultrasound slice.  An alpha component is appended to the output to
// specify the coverage of each pixel in the output volume (i.e. whether or
// not a voxel has been touched by the reconstruction)
// .SECTION see also
// vtkPlusVideoSource, vtkTracker, vtkTrackerTool

#ifndef __vtkPasteSliceIntoVolumeHelper_h
#define __vtkPasteSliceIntoVolumeHelper_h

#include "PlusConfigure.h"
#include "vtkTimerLog.h"
#include "vtkMatrix4x4.h"
#include "vtkPlusVideoSource.h"
#include "vtkTransform.h"
#include "vtkImageThreshold.h"
#include "vtkImageClip.h"
#include "vtkImageData.h"
#include "vtkTrackerBuffer.h"
#include "vtkPlusVideoSource.h"
#include "vtkTrackerTool.h"
#include "fixed.h"
#include "vtkSmartPointer.h"
#include "vtkMultiThreader.h"
#include "vtkVideoBuffer.h"


//****************************************************************************
// ROUNDING CODE
//****************************************************************************

//----------------------------------------------------------------------------
// rounding functions, split and optimized for each type
// (because we don't want to round if the result is a float!)

// in the case of a tie between integers, the larger integer wins.

// The 'floor' function on x86 and mips is many times slower than these
// and is used a lot in this code, optimize for different CPU architectures
// static inline int vtkUltraFloor(double x)
// {
// #if defined mips || defined sparc
//   return (int)((unsigned int)(x + 2147483648.0) - 2147483648U);
// #elif defined i386
//   double tempval = (x - 0.25) + 3377699720527872.0; // (2**51)*1.5
//   return ((int*)&tempval)[0] >> 1;
// #else
//   return int(floor(x));
// #endif
// }

static inline int vtkUltraFloor(double x)
{
#if defined mips || defined sparc || defined __ppc__
  x += 2147483648.0;
  unsigned int i = (unsigned int)(x);
  return (int)(i - 2147483648U);
#elif defined i386 || defined _M_IX86
  union { double d; unsigned short s[4]; unsigned int i[2]; } dual;
  dual.d = x + 103079215104.0;  // (2**(52-16))*1.5
  return (int)((dual.i[1]<<16)|((dual.i[0])>>16));
#elif defined ia64 || defined __ia64__ || defined IA64
  x += 103079215104.0;
  long long i = (long long)(x);
  return (int)(i - 103079215104LL);
#else
  double y = floor(x);
  return (int)(y);
#endif
}

static inline int vtkUltraCeil(double x)
{
  return -vtkUltraFloor(-x - 1.0) - 1;
}

static inline int vtkUltraRound(double x)
{
  return vtkUltraFloor(x + 0.5);
}

static inline int vtkUltraFloor(float x)
{
  return vtkUltraFloor((double)x);
}

static inline int vtkUltraCeil(float x)
{
  return vtkUltraCeil((double)x);
}

static inline int vtkUltraRound(float x)
{
  return vtkUltraRound((double)x);
}

static inline int vtkUltraFloor(fixed x)
{
  return x.floor();
}

static inline int vtkUltraCeil(fixed x)
{
  return x.ceil();
}

static inline int vtkUltraRound(fixed x)
{
  return x.round();
}

// convert a float into an integer plus a fraction
template <class F>
static inline int vtkUltraFloor(F x, F &f)
{
  int ix = vtkUltraFloor(x);
  f = x - ix;
  return ix;
}

template <class F, class T>
static inline void vtkUltraRound(F val, T& rnd)
{
  rnd = vtkUltraRound(val);
}

//****************************************************************************
// INTERPOLATION CODE
//****************************************************************************

//----------------------------------------------------------------------------
// Sets interpolate (pointer to a function) to match the current interpolation
// mode - used for unoptimized versions only
template <class F, class T>
static void vtkGetUltraInterpFunc(vtkPasteSliceIntoVolume::InterpolationType interpolationMode, 
                                  int (**interpolate)(F *point, T *inPtr, T *outPtr, unsigned short *accPtr, int numscalars, int outExt[6], int outInc[3])
                                  )
{
  switch (interpolationMode)
  {
  case vtkPasteSliceIntoVolume::NEAREST_NEIGHBOR_INTERPOLATION:
    *interpolate = &vtkNearestNeighborInterpolation;
    break;
  case vtkPasteSliceIntoVolume::LINEAR_INTERPOLATION:
    *interpolate = &vtkTrilinearInterpolation;
    break;
  }
}

///////////////// NEAREST NEIGHBOR INTERPOLATION ///////////////////////
// In the un-optimized version, each output voxel
// is converted into a set of look-up indices for the input data;
// then, the indices are checked to ensure they lie within the
// input data extent.

// In the optimized versions, the check is done in reverse:
// it is first determined which output voxels map to look-up indices
// within the input data extent.  Then, further calculations are
// done only for those voxels.  This means that 1) minimal work
// is done for voxels which map to regions outside fo the input
// extent (they are just set to the background color) and 2)
// the inner loops of the look-up and interpolation are
// tightened relative to the un-uptimized version.
////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
// vtkNearestNeighborInterpolation - NOT OPTIMIZED
// Do nearest-neighbor interpolation of the input data 'inPtr' of extent 
// 'inExt' at the 'point'.  The result is placed at 'outPtr'.  
// If the lookup data is beyond the extent 'inExt', set 'outPtr' to
// the background color 'background'.  
// The number of scalar components in the data is 'numscalars'
template <class F, class T>
static int vtkNearestNeighborInterpolation(F *point, T *inPtr, T *outPtr,
                                           unsigned short *accPtr, 
                                           int numscalars, 
                                           int outExt[6], int outInc[3])
{
  int i;
  // The nearest neighbor interpolation occurs here
  // The output point is the closest point to the input point - rounding
  // to get closest point
  int outIdX = vtkUltraRound(point[0])-outExt[0];
  int outIdY = vtkUltraRound(point[1])-outExt[2];
  int outIdZ = vtkUltraRound(point[2])-outExt[4];

  // fancy way of checking bounds
  if ((outIdX | (outExt[1]-outExt[0] - outIdX) |
    outIdY | (outExt[3]-outExt[2] - outIdY) |
    outIdZ | (outExt[5]-outExt[4] - outIdZ)) >= 0)
  {
    int inc = outIdX*outInc[0]+outIdY*outInc[1]+outIdZ*outInc[2];
    outPtr += inc;
    // accumulation buffer: do compounding
    if (accPtr)
    {
      accPtr += inc/outInc[0];
      int newa = *accPtr + 255;
      for (i = 0; i < numscalars; i++)
      {
        *outPtr = ((*inPtr++)*255 + (*outPtr)*(*accPtr))/newa;
        outPtr++;
      }
      *outPtr = 255;
      *accPtr = 65535;
      if (newa < 65535)
      {
        *accPtr = newa;
      }
    }
    // no accumulation buffer, replace what was there before
    else
    {
      for (i = 0; i < numscalars; i++)
      {
        *outPtr++ = *inPtr++;
      }
      *outPtr = 255;
    }
    return 1;
  }
  return 0;
} 

//----------------------------------------------------------------------------
// vtkFreehand2OptimizedNNHelper - OPTIMIZED, WITHOUT INTEGER MATHEMATICS
// Optimized nearest neighbor interpolation
template<class T>
static inline void vtkFreehand2OptimizedNNHelper(int xIntersectionPixStart, int xIntersectionPixEnd,
                                                 double *outPoint,
                                                 double *outPoint1,
                                                 double *xAxis,
                                                 T *&inPtr, T *outPtr,
                                                 int *outExt, int *outInc,
                                                 int numscalars, 
                                                 unsigned short *accPtr)
{
  // with compounding
  if (accPtr)
  {

    for (int idX = xIntersectionPixStart; idX <= xIntersectionPixEnd; idX++)
    {
      outPoint[0] = outPoint1[0] + idX*xAxis[0]; 
      outPoint[1] = outPoint1[1] + idX*xAxis[1];
      outPoint[2] = outPoint1[2] + idX*xAxis[2];

      int outIdX = vtkUltraRound(outPoint[0]) - outExt[0];
      int outIdY = vtkUltraRound(outPoint[1]) - outExt[2];
      int outIdZ = vtkUltraRound(outPoint[2]) - outExt[4];

      // bounds checking turned off to improve performance
      //if (outIdX < 0 || outIdX > outExt[1] - outExt[0] ||
      //    outIdY < 0 || outIdY > outExt[3] - outExt[2] ||
      //    outIdZ < 0 || outIdZ > outExt[5] - outExt[4])
      //  {
      //  cerr << "out of bounds!!!\n";
      //  inPtr += numscalars;
      //  return;
      //  }

      int inc = outIdX*outInc[0] + outIdY*outInc[1] + outIdZ*outInc[2];
      T *outPtr1 = outPtr + inc;
      // divide by outInc[0] to accomodate for the difference
      // in the number of scalar pointers between the output
      // and the accumulation buffer
      unsigned short *accPtr1 = accPtr + ((unsigned short)(inc/outInc[0]));
      unsigned short newa = *accPtr1 + ((unsigned short)(255)); 
      int i = numscalars;
      do 
      {
        i--;
        *outPtr1 = ((*inPtr++)*255 + (*outPtr1)*(*accPtr1))/newa;
        outPtr1++;
      }
      while (i);

      *outPtr1 = 255;
      *accPtr1 = 65535;
      if (newa < 65535)
      {
        *accPtr1 = newa;
      }
    }
  }

  // not compounding
  else
  {
    for (int idX = xIntersectionPixStart; idX <= xIntersectionPixEnd; idX++)
    {
      outPoint[0] = outPoint1[0] + idX*xAxis[0]; 
      outPoint[1] = outPoint1[1] + idX*xAxis[1];
      outPoint[2] = outPoint1[2] + idX*xAxis[2];

      int outIdX = vtkUltraRound(outPoint[0]) - outExt[0];
      int outIdY = vtkUltraRound(outPoint[1]) - outExt[2];
      int outIdZ = vtkUltraRound(outPoint[2]) - outExt[4];

      // bounds checking turned off to improve performance
      //if (outIdX < 0 || outIdX > outExt[1] - outExt[0] ||
      //    outIdY < 0 || outIdY > outExt[3] - outExt[2] ||
      //    outIdZ < 0 || outIdZ > outExt[5] - outExt[4])
      //  {
      //  cerr << "out of bounds!!!\n";
      //  inPtr += numscalars;
      //  return;
      //  }

      int inc = outIdX*outInc[0] + outIdY*outInc[1] + outIdZ*outInc[2];
      T *outPtr1 = outPtr + inc;
      int i = numscalars;
      do
      {
        i--;
        // copy the input pointer value into the output pointer (this is where the intensities get copied)
        *outPtr1++ = *inPtr++;
      }
      while (i);
      *outPtr1 = 255;
    }
  } 
}

//----------------------------------------------------------------------------
// vtkFreehand2OptimizedNNHelper - OPTIMIZED, WITH INTEGER MATHEMATICS
// Optimized nearest neighbor interpolation, specifically optimized for fixed
// point (i.e. integer) mathematics
// Same as above, but with fixed type
template <class T>
static inline void vtkFreehand2OptimizedNNHelper(int xIntersectionPixStart, int xIntersectionPixEnd,
                                                 fixed *outPoint,
                                                 fixed *outPoint1, fixed *xAxis,
                                                 T *&inPtr, T *outPtr,
                                                 int *outExt, int *outInc,
                                                 int numscalars, 
                                                 unsigned short *accPtr)
{
  outPoint[0] = outPoint1[0] + xIntersectionPixStart*xAxis[0] - outExt[0];
  outPoint[1] = outPoint1[1] + xIntersectionPixStart*xAxis[1] - outExt[2];
  outPoint[2] = outPoint1[2] + xIntersectionPixStart*xAxis[2] - outExt[4];

  // Nearest-Neighbor, no extent checks, with accumulation
  if (accPtr)
  {
    for (int idX = xIntersectionPixStart; idX <= xIntersectionPixEnd; idX++)
    {
      int outIdX = vtkUltraRound(outPoint[0]);
      int outIdY = vtkUltraRound(outPoint[1]);
      int outIdZ = vtkUltraRound(outPoint[2]);

      // bounds checking turned off to improve performance
      //if (outIdX < 0 || outIdX > outExt[1] - outExt[0] ||
      //    outIdY < 0 || outIdY > outExt[3] - outExt[2] ||
      //    outIdZ < 0 || outIdZ > outExt[5] - outExt[4])
      //  {
      //  cerr << "out of bounds!!!\n";
      //  inPtr += numscalars;
      //  return;
      //  }

      int inc = outIdX*outInc[0] + outIdY*outInc[1] + outIdZ*outInc[2];
      T *outPtr1 = outPtr + inc;
      // divide by outInc[0] to accomodate for the difference
      // in the number of scalar pointers between the output
      // and the accumulation buffer
      unsigned short *accPtr1 = accPtr + ((unsigned short)(inc/outInc[0]));
      unsigned short newa = *accPtr1 + ((unsigned short)(255));
      int i = numscalars;
      do 
      {
        i--;
        *outPtr1 = ((*inPtr++)*255 + (*outPtr1)*(*accPtr1))/newa;
        outPtr1++;
      }
      while (i);

      *outPtr1 = 255;
      *accPtr1 = 65535;
      if (newa < 65535)
      {
        *accPtr1 = newa;
      }

      outPoint[0] += xAxis[0];
      outPoint[1] += xAxis[1];
      outPoint[2] += xAxis[2];
    }
  }

  // Nearest-Neighbor, no extent checks, no accumulation
  else
  {
    for (int idX = xIntersectionPixStart; idX <= xIntersectionPixEnd; idX++)
    {
      int outIdX = vtkUltraRound(outPoint[0]);
      int outIdY = vtkUltraRound(outPoint[1]);
      int outIdZ = vtkUltraRound(outPoint[2]);

      // bounds checking turned off to improve performance
      //if (outIdX < 0 || outIdX > outExt[1] - outExt[0] ||
      //    outIdY < 0 || outIdY > outExt[3] - outExt[2] ||
      //    outIdZ < 0 || outIdZ > outExt[5] - outExt[4])
      //  {
      //  cerr << "out of bounds!!!\n";
      //  inPtr += numscalars;
      //  return;
      //  }

      int inc = outIdX*outInc[0] + outIdY*outInc[1] + outIdZ*outInc[2];
      T *outPtr1 = outPtr + inc;
      int i = numscalars;
      do
      {
        i--;
        *outPtr1++ = *inPtr++;
      }
      while (i);
      *outPtr1 = 255;

      outPoint[0] += xAxis[0];
      outPoint[1] += xAxis[1];
      outPoint[2] += xAxis[2];
    }
  } 
}

////////////////////// TRILINEAR INTERPOLATION //////////////////////////
// does reverse trilinear interpolation
// trilinear interpolation would use the pixel values to interpolate something
// in the middle we have the something in the middle and want to spread it to
// the discrete pixel values around it, in an interpolated way
/////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
// Do trilinear interpolation of the input data 'inPtr' of extent 'inExt'
// at the 'point'.  The result is placed at 'outPtr'.  
// If the lookup data is beyond the extent 'inExt', set 'outPtr' to
// the background color 'background'.  
// The number of scalar components in the data is 'numscalars'
template <class F, class T>
static int vtkTrilinearInterpolation(F *point, T *inPtr, T *outPtr,
                                     unsigned short *accPtr, int numscalars, 
                                     int outExt[6], int outInc[3])
{
  F fx, fy, fz;

  // convert point[0] into integer component and a fraction
  int outIdX0 = vtkUltraFloor(point[0], fx);
  // point[0] is unchanged, outIdX0 is the integer (floor), fx is the float
  int outIdY0 = vtkUltraFloor(point[1], fy);
  int outIdZ0 = vtkUltraFloor(point[2], fz);

  int outIdX1 = outIdX0 + (fx != 0); // ceiling
  int outIdY1 = outIdY0 + (fy != 0);
  int outIdZ1 = outIdZ0 + (fz != 0);

  // at this point in time we have the floor (outIdX0), the ceiling (outIdX1)
  // and the fractional component (fx) for x, y and z

  // bounds check
  if ((outIdX0 | (outExt[1]-outExt[0] - outIdX1) |
    outIdY0 | (outExt[3]-outExt[2] - outIdY1) |
    outIdZ0 | (outExt[5]-outExt[4] - outIdZ1)) >= 0)
  {
    // do reverse trilinear interpolation
    int factX0 = outIdX0*outInc[0];
    int factY0 = outIdY0*outInc[1];
    int factZ0 = outIdZ0*outInc[2];
    int factX1 = outIdX1*outInc[0];
    int factY1 = outIdY1*outInc[1];
    int factZ1 = outIdZ1*outInc[2];

    int factY0Z0 = factY0 + factZ0;
    int factY0Z1 = factY0 + factZ1;
    int factY1Z0 = factY1 + factZ0;
    int factY1Z1 = factY1 + factZ1;

    // increment between the output pointer and the 8 pixels to work on
    int idx[8];
    idx[0] = factX0 + factY0Z0;
    idx[1] = factX0 + factY0Z1;
    idx[2] = factX0 + factY1Z0;
    idx[3] = factX0 + factY1Z1;
    idx[4] = factX1 + factY0Z0;
    idx[5] = factX1 + factY0Z1;
    idx[6] = factX1 + factY1Z0;
    idx[7] = factX1 + factY1Z1;

    // remainders from the fractional components - difference between the fractional value and the ceiling
    F rx = 1 - fx;
    F ry = 1 - fy;
    F rz = 1 - fz;

    F ryrz = ry*rz;
    F ryfz = ry*fz;
    F fyrz = fy*rz;
    F fyfz = fy*fz;

    F fdx[8];
    fdx[0] = rx*ryrz;
    fdx[1] = rx*ryfz;
    fdx[2] = rx*fyrz;
    fdx[3] = rx*fyfz;
    fdx[4] = fx*ryrz;
    fdx[5] = fx*ryfz;
    fdx[6] = fx*fyrz;
    fdx[7] = fx*fyfz;

    F f, r, a;
    T *inPtrTmp, *outPtrTmp;

    // do compounding
    if (accPtr)
    {
      unsigned short *accPtrTmp;

      // loop over the eight voxels
      int j = 8;
      do 
      {
        j--;
        if (fdx[j] == 0)
        {
          continue;
        }
        inPtrTmp = inPtr;
        outPtrTmp = outPtr+idx[j];
        accPtrTmp = accPtr+ ((unsigned short)(idx[j]/outInc[0]));
        f = fdx[j];
        r = F((*accPtrTmp)/255);
        a = f + r;

        int i = numscalars;
        do
        {
          i--;
          vtkUltraRound((f*(*inPtrTmp++) + r*(*outPtrTmp))/a, *outPtrTmp);
          outPtrTmp++;
        }
        while (i);

        *accPtrTmp = 65535;
        *outPtrTmp = 255;
        a *= 255;
        // don't allow accumulation buffer overflow
        if (a < F(65535))
        {
          vtkUltraRound(a, *accPtrTmp);
        }
      }
      while (j);
    }

    // no compounding
    else 
    {
      // loop over the eight voxels
      int j = 8;
      do
      {
        j--;
        if (fdx[j] == 0)
        {
          continue;
        }
        inPtrTmp = inPtr;
        outPtrTmp = outPtr+idx[j];
        // if alpha is nonzero then the pixel was hit before, so
        //  average with previous value
        if (outPtrTmp[numscalars])
        {
          f = fdx[j];
          F r = 1 - f;
          int i = numscalars;
          do
          {
            i--;
            vtkUltraRound(f*(*inPtrTmp++) + r*(*outPtrTmp), *outPtrTmp);
            outPtrTmp++;
          }
          while (i);
        }
        // alpha is zero, so just insert the new value
        else
        {
          int i = numscalars;
          do
          {
            i--;
            *outPtrTmp++ = *inPtrTmp++;
          }
          while (i);
        }          
        *outPtrTmp = 255;
      }
      while (j);
    }
    return 1;
  }
  // if bounds check fails
  return 0;
}     


//****************************************************************************
// HELPER FUNCTIONS FOR THE RECONSTRUCTION
//****************************************************************************

//----------------------------------------------------------------------------
// find approximate intersection of line with the plane x = x_min,
// y = y_min, or z = z_min (lower limit of data extent) 
template<class F>
static inline
int intersectionHelper(F *point, F *axis, int *limit, int ai, int *inExt)
{
  F rd = limit[ai]*point[3]-point[ai]  + 0.5; 

  if (rd < inExt[0])
  { 
    return inExt[0];
  }
  else if (rd > inExt[1])
  {
    return inExt[1];
  }
  else
  {
    return int(rd);
  }
}

//----------------------------------------------------------------------------
// find the point just inside the extent
template <class F>
static int intersectionLow(F *point, F *axis, int *sign,
                           int *limit, int ai, int *inExt)
{
  // approximate value of r
  int r = intersectionHelper(point,axis,limit,ai,inExt);

  // move back and forth to find the point just inside the extent
  for (;;)
  {
    F p = point[ai]+r*axis[ai];

    if ((sign[ai] < 0 && r > inExt[0] ||
      sign[ai] > 0 && r < inExt[1]) && 
      vtkUltraRound(p) < limit[ai])
    {
      r += sign[ai];
    }
    else
    {
      break;
    }
  }

  for (;;)
  {
    F p = point[ai]+(r-sign[ai])*axis[ai];

    if ((sign[ai] > 0 && r > inExt[0] ||
      sign[ai] < 0 && r < inExt[1]) && 
      vtkUltraRound(p) >= limit[ai])
    {
      r -= sign[ai];
    }
    else
    {
      break;
    }
  }

  return r;
}

//----------------------------------------------------------------------------
// same as above, but for x = x_max
template <class F>
static int intersectionHigh(F *point, F *axis, int *sign, 
                            int *limit, int ai, int *inExt)
{
  // approximate value of r
  int r = intersectionHelper(point,axis,limit,ai,inExt);

  // move back and forth to find the point just inside the extent
  for (;;)
  {
    F p = point[ai]+r*axis[ai];

    if ((sign[ai] > 0 && r > inExt[0] ||
      sign[ai] < 0 && r < inExt[1]) &&
      vtkUltraRound(p) > limit[ai])
    {
      r -= sign[ai];
    }
    else
    {
      break;
    }
  }

  for (;;)
  {
    F p = point[ai]+(r+sign[ai])*axis[ai];

    if ((sign[ai] < 0 && r > inExt[0] ||
      sign[ai] > 0 && r < inExt[1]) && 
      vtkUltraRound(p) <= limit[ai])
    {
      r += sign[ai];
    }
    else
    {
      break;
    }
  }

  return r;
}

//----------------------------------------------------------------------------
template <class F>
static int isBounded(F *point, F *xAxis, int *inMin, 
                     int *inMax, int ai, int r)
{
  int bi = ai+1; 
  int ci = ai+2;
  if (bi > 2) 
  { 
    bi -= 3; // coordinate index must be 0, 1 or 2 
  } 
  if (ci > 2)
  { 
    ci -= 3;
  }

  F fbp = point[bi]+r*xAxis[bi];
  F fcp = point[ci]+r*xAxis[ci];

  int bp = vtkUltraRound(fbp);
  int cp = vtkUltraRound(fcp);

  return (bp >= inMin[bi] && bp <= inMax[bi] &&
    cp >= inMin[ci] && cp <= inMax[ci]);
}

//----------------------------------------------------------------------------
// This huge mess finds out where the current output raster
// line intersects the input volume
static void vtkUltraFindExtentHelper(int &xIntersectionPixStart, int &xIntersectionPixEnd, int sign, int *inExt)
{
  if (sign < 0)
  {
    int i = xIntersectionPixStart;
    xIntersectionPixStart = xIntersectionPixEnd;
    xIntersectionPixEnd = i;
  }

  // bound xIntersectionPixStart,xIntersectionPixEnd within reasonable limits
  if (xIntersectionPixStart < inExt[0]) 
  {
    xIntersectionPixStart = inExt[0];
  }
  if (xIntersectionPixEnd > inExt[1]) 
  {
    xIntersectionPixEnd = inExt[1];
  }
  if (xIntersectionPixStart > xIntersectionPixEnd) 
  {
    xIntersectionPixStart = inExt[0];
    xIntersectionPixEnd = inExt[0]-1;
  }
}  

//----------------------------------------------------------------------------
template <class F>
static void vtkUltraFindExtent(int& xIntersectionPixStart, int& xIntersectionPixEnd, F *point, F *xAxis, 
                               int *inMin, int *inMax, int *inExt)
{
  int i, ix, iy, iz;
  int sign[3];
  int indx1[4],indx2[4];
  F p1,p2;

  // find signs of components of x axis 
  // (this is complicated due to the homogeneous coordinate)
  for (i = 0; i < 3; i++)
  {
    p1 = point[i];

    p2 = point[i]+xAxis[i];

    if (p1 <= p2)
    {
      sign[i] = 1;
    }
    else 
    {
      sign[i] = -1;
    }
  } 

  // order components of xAxis from largest to smallest
  ix = 0;
  for (i = 1; i < 3; i++)
  {
    if (((xAxis[i] < 0) ? (-xAxis[i]) : (xAxis[i])) >
      ((xAxis[ix] < 0) ? (-xAxis[ix]) : (xAxis[ix])))
    {
      ix = i;
    }
  }

  iy = ((ix > 1) ? ix-2 : ix+1);
  iz = ((ix > 0) ? ix-1 : ix+2);

  if (((xAxis[iy] < 0) ? (-xAxis[iy]) : (xAxis[iy])) >
    ((xAxis[iz] < 0) ? (-xAxis[iz]) : (xAxis[iz])))
  {
    i = iy;
    iy = iz;
    iz = i;
  }

  xIntersectionPixStart = intersectionLow(point,xAxis,sign,inMin,ix,inExt);
  xIntersectionPixEnd = intersectionHigh(point,xAxis,sign,inMax,ix,inExt);

  // find points of intersections
  // first, find w-value for perspective (will usually be 1)
  for (i = 0; i < 3; i++)
  {
    p1 = point[i]+xIntersectionPixStart*xAxis[i];
    p2 = point[i]+xIntersectionPixEnd*xAxis[i];

    indx1[i] = vtkUltraRound(p1);
    indx2[i] = vtkUltraRound(p2);
  }

  // passed through x face, check opposing face
  if (isBounded(point,xAxis,inMin,inMax,ix,xIntersectionPixStart))
  {
    if (isBounded(point,xAxis,inMin,inMax,ix,xIntersectionPixEnd))
    {
      vtkUltraFindExtentHelper(xIntersectionPixStart,xIntersectionPixEnd,sign[ix],inExt);
      return;
    }

    // check y face
    if (indx2[iy] < inMin[iy])
    {
      xIntersectionPixEnd = intersectionLow(point,xAxis,sign,inMin,iy,inExt);
      if (isBounded(point,xAxis,inMin,inMax,iy,xIntersectionPixEnd))
      {
        vtkUltraFindExtentHelper(xIntersectionPixStart,xIntersectionPixEnd,sign[ix],inExt);
        return;
      }
    }

    // check other y face
    else if (indx2[iy] > inMax[iy])
    {
      xIntersectionPixEnd = intersectionHigh(point,xAxis,sign,inMax,iy,inExt);
      if (isBounded(point,xAxis,inMin,inMax,iy,xIntersectionPixEnd))
      {
        vtkUltraFindExtentHelper(xIntersectionPixStart,xIntersectionPixEnd,sign[ix],inExt);
        return;
      }
    }

    // check z face
    if (indx2[iz] < inMin[iz])
    {
      xIntersectionPixEnd = intersectionLow(point,xAxis,sign,inMin,iz,inExt);
      if (isBounded(point,xAxis,inMin,inMax,iz,xIntersectionPixEnd))
      {
        vtkUltraFindExtentHelper(xIntersectionPixStart,xIntersectionPixEnd,sign[ix],inExt);
        return;
      }
    }

    // check other z face
    else if (indx2[iz] > inMax[iz])
    {
      xIntersectionPixEnd = intersectionHigh(point,xAxis,sign,inMax,iz,inExt);
      if (isBounded(point,xAxis,inMin,inMax,iz,xIntersectionPixEnd))
      {
        vtkUltraFindExtentHelper(xIntersectionPixStart,xIntersectionPixEnd,sign[ix],inExt);
        return;
      }
    }
  }

  // passed through the opposite x face
  if (isBounded(point,xAxis,inMin,inMax,ix,xIntersectionPixEnd))
  {
    // check y face
    if (indx1[iy] < inMin[iy])
    {
      xIntersectionPixStart = intersectionLow(point,xAxis,sign,inMin,iy,inExt);
      if (isBounded(point,xAxis,inMin,inMax,iy,xIntersectionPixStart))
      {
        vtkUltraFindExtentHelper(xIntersectionPixStart,xIntersectionPixEnd,sign[ix],inExt);
        return;
      }
    }
    // check other y face
    else if (indx1[iy] > inMax[iy])
    {
      xIntersectionPixStart = intersectionHigh(point,xAxis,sign,inMax,iy,inExt);
      if (isBounded(point,xAxis,inMin,inMax,iy,xIntersectionPixStart))
      {
        vtkUltraFindExtentHelper(xIntersectionPixStart,xIntersectionPixEnd,sign[ix],inExt);
        return;
      }
    }

    // check other y face
    if (indx1[iz] < inMin[iz])
    {
      xIntersectionPixStart = intersectionLow(point,xAxis,sign,inMin,iz,inExt);
      if (isBounded(point,xAxis,inMin,inMax,iz,xIntersectionPixStart))
      {
        vtkUltraFindExtentHelper(xIntersectionPixStart,xIntersectionPixEnd,sign[ix],inExt);
        return;
      }
    }
    // check other z face
    else if (indx1[iz] > inMax[iz])
    {
      xIntersectionPixStart = intersectionHigh(point,xAxis,sign,inMax,iz,inExt);
      if (isBounded(point,xAxis,inMin,inMax,iz,xIntersectionPixStart))
      {
        vtkUltraFindExtentHelper(xIntersectionPixStart,xIntersectionPixEnd,sign[ix],inExt);
        return;
      }
    }
  }

  // line might pass through bottom face
  if ((indx1[iy] >= inMin[iy] && indx2[iy] < inMin[iy]) ||
    (indx1[iy] < inMin[iy] && indx2[iy] >= inMin[iy]))
  {
    xIntersectionPixStart = intersectionLow(point,xAxis,sign,inMin,iy,inExt);
    if (isBounded(point,xAxis,inMin,inMax,iy,xIntersectionPixStart))
    {
      // line might pass through top face
      if ((indx1[iy] <= inMax[iy] && indx2[iy] > inMax[iy]) ||
        (indx1[iy] > inMax[iy] && indx2[iy] <= inMax[iy]))
      { 
        xIntersectionPixEnd = intersectionHigh(point,xAxis,sign,inMax,iy,inExt);
        if (isBounded(point,xAxis,inMin,inMax,iy,xIntersectionPixEnd))
        {
          vtkUltraFindExtentHelper(xIntersectionPixStart,xIntersectionPixEnd,sign[iy],inExt);
          return;
        }
      }

      // line might pass through in-to-screen face
      if (indx1[iz] < inMin[iz] && indx2[iy] < inMin[iy] ||
        indx2[iz] < inMin[iz] && indx1[iy] < inMin[iy])
      { 
        xIntersectionPixEnd = intersectionLow(point,xAxis,sign,inMin,iz,inExt);
        if (isBounded(point,xAxis,inMin,inMax,iz,xIntersectionPixEnd))
        {
          vtkUltraFindExtentHelper(xIntersectionPixStart,xIntersectionPixEnd,sign[iy],inExt);
          return;
        }
      }
      // line might pass through out-of-screen face
      else if (indx1[iz] > inMax[iz] && indx2[iy] < inMin[iy] ||
        indx2[iz] > inMax[iz] && indx1[iy] < inMin[iy])
      {
        xIntersectionPixEnd = intersectionHigh(point,xAxis,sign,inMax,iz,inExt);
        if (isBounded(point,xAxis,inMin,inMax,iz,xIntersectionPixEnd))
        {
          vtkUltraFindExtentHelper(xIntersectionPixStart,xIntersectionPixEnd,sign[iy],inExt);
          return;
        }
      } 
    }
  }

  // line might pass through top face
  if ((indx1[iy] <= inMax[iy] && indx2[iy] > inMax[iy]) ||
    (indx1[iy] > inMax[iy] && indx2[iy] <= inMax[iy]))
  {
    xIntersectionPixEnd = intersectionHigh(point,xAxis,sign,inMax,iy,inExt);
    if (isBounded(point,xAxis,inMin,inMax,iy,xIntersectionPixEnd))
    {
      // line might pass through in-to-screen face
      if (indx1[iz] < inMin[iz] && indx2[iy] > inMax[iy] ||
        indx2[iz] < inMin[iz] && indx1[iy] > inMax[iy])
      {
        xIntersectionPixStart = intersectionLow(point,xAxis,sign,inMin,iz,inExt);
        if (isBounded(point,xAxis,inMin,inMax,iz,xIntersectionPixStart))
        {
          vtkUltraFindExtentHelper(xIntersectionPixStart,xIntersectionPixEnd,sign[iy],inExt);
          return;
        }
      }
      // line might pass through out-of-screen face
      else if (indx1[iz] > inMax[iz] && indx2[iy] > inMax[iy] || 
        indx2[iz] > inMax[iz] && indx1[iy] > inMax[iy])
      {
        xIntersectionPixStart = intersectionHigh(point,xAxis,sign,inMax,iz,inExt);
        if (isBounded(point,xAxis,inMin,inMax,iz,xIntersectionPixStart))
        {
          vtkUltraFindExtentHelper(xIntersectionPixStart,xIntersectionPixEnd,sign[iy],inExt);
          return;
        }
      }
    } 
  }

  // line might pass through in-to-screen face
  if ((indx1[iz] >= inMin[iz] && indx2[iz] < inMin[iz]) ||
    (indx1[iz] < inMin[iz] && indx2[iz] >= inMin[iz]))
  {
    xIntersectionPixStart = intersectionLow(point,xAxis,sign,inMin,iz,inExt);
    if (isBounded(point,xAxis,inMin,inMax,iz,xIntersectionPixStart))
    {
      // line might pass through out-of-screen face
      if (indx1[iz] > inMax[iz] || indx2[iz] > inMax[iz])
      {
        xIntersectionPixEnd = intersectionHigh(point,xAxis,sign,inMax,iz,inExt);
        if (isBounded(point,xAxis,inMin,inMax,iz,xIntersectionPixEnd))
        {
          vtkUltraFindExtentHelper(xIntersectionPixStart,xIntersectionPixEnd,sign[iz],inExt);
          return;
        }
      }
    }
  }

  xIntersectionPixStart = inExt[0];
  xIntersectionPixEnd = inExt[0] - 1;
}



//****************************************************************************
// REAL-TIME RECONSTRUCTION - NOT OPTIMIZED
//****************************************************************************

//----------------------------------------------------------------------------
// convert the ClipRectangle into a
// clip extent that can be applied to the input data - number of pixels (+ or -)
// from the origin (the z component is copied from the inExt parameter)
// 
// clipExt = {x0, x1, y0, y1, z0, z1} <-- the "output" of this function is to
//                                        change this array
// inOrigin = {x, y, z} <-- the origin in mm
// inSpacing = {x, y, z} <-- the spacing in mm
// inExt = {x0, x1, y0, y1, z0, z1} <-- min/max possible extent, in pixels
void GetClipExtent(int clipExt[6],
										   vtkFloatingPointType inOrigin[3],
										   vtkFloatingPointType inSpacing[3],
										   const int inExt[6],
                       double clipRectangleOrigin[2],
                       double clipRectangleSize[2])
{
	// Map the clip rectangle (millimetres) to pixels
	// --> number of pixels (+ or -) from the origin

	int x0 = (int)ceil((clipRectangleOrigin[0]-inOrigin[0])/inSpacing[0]);
	int x1 = (int)floor((clipRectangleOrigin[0]-inOrigin[0]+clipRectangleSize[0])/inSpacing[0]);
	int y0 = (int)ceil((clipRectangleOrigin[1]-inOrigin[1])/inSpacing[1]);
	int y1 = (int)floor((clipRectangleOrigin[1]-inOrigin[1]+clipRectangleSize[1])/inSpacing[1]);

	// Make sure that x0 <= x1 and y0 <= y1
	if (x0 > x1)
	{
		int tmp = x0; x0 = x1; x1 = tmp;
	}
	if (y0 > y1)
	{
		int tmp = y0; y0 = y1; y1 = tmp;
	}

	// make sure the clip extent lies within the input extent
	if (x0 < inExt[0])
	{
		x0 = inExt[0];
	}
	if (x1 > inExt[1])
	{
		x1 = inExt[1];
	}
	// clip extent was outside of range of input extent
	if (x0 > x1)
	{
		x0 = inExt[0];
		x1 = inExt[0]-1;
	}

	if (y0 < inExt[2])
	{
		y0 = inExt[2];
	}
	if (y1 > inExt[3])
	{
		y1 = inExt[3];
	}
	// clip extent was outside of range of input extent
	if (y0 > y1)
	{
		y0 = inExt[2];
		y1 = inExt[2]-1;
	}

	// Set the clip extent
	clipExt[0] = x0;
	clipExt[1] = x1;
	clipExt[2] = y0;
	clipExt[3] = y1;
	clipExt[4] = inExt[4];
	clipExt[5] = inExt[5];
}

//----------------------------------------------------------------------------
// Actually inserts the slice - executes the filter for any type of data, for
// no optimization
// Given an input and output region, execute the filter algorithm to fill the
// output from the input - no optimization.
// (this one function is pretty much the be-all and end-all of the
// filter)
template <class T>
static void vtkUnoptimizedInsertSlice(vtkImageData *outData, T *outPtr, unsigned short *accPtr, vtkImageData *inData, T *inPtr, int inExt[6], vtkMatrix4x4 *matrix,
  double clipRectangleOrigin[2],double clipRectangleSize[2], double fanAngles[2], double fanOrigin[2], double fanDepth, vtkPasteSliceIntoVolume::InterpolationType interpolationMode)
{

  LOG_TRACE("sliceToOutputVolumeMatrix="<<matrix->GetElement(0,0)<<" "<<matrix->GetElement(0,1)<<" "<<matrix->GetElement(0,2)<<" "<<matrix->GetElement(0,3)<<"; "
    <<matrix->GetElement(1,0)<<" "<<matrix->GetElement(1,1)<<" "<<matrix->GetElement(1,2)<<" "<<matrix->GetElement(1,3)<<"; "
    <<matrix->GetElement(2,0)<<" "<<matrix->GetElement(2,1)<<" "<<matrix->GetElement(2,2)<<" "<<matrix->GetElement(2,3)<<"; "
    <<matrix->GetElement(3,0)<<" "<<matrix->GetElement(3,1)<<" "<<matrix->GetElement(3,2)<<" "<<matrix->GetElement(3,3)
    );
      
  // slice spacing and origin
  vtkFloatingPointType inSpacing[3];  
  inData->GetSpacing(inSpacing);
  vtkFloatingPointType inOrigin[3];
  inData->GetOrigin(inOrigin);

  // number of pixels in the x and y directions between the fan origin and the slice origin  
  double fanOriginInPixels[2] =
  {
    (fanOrigin[0]-inOrigin[0])/inSpacing[0],
    (fanOrigin[1]-inOrigin[1])/inSpacing[1]
  };
  // fan depth squared 
  double fanDepthSquaredMm = fanDepth*fanDepth;

  // absolute value of slice spacing
  double inSpacingSquare[2]=
  {
    inSpacing[0]*inSpacing[0],
    inSpacing[1]*inSpacing[1]
  };

  double pixelAspectRatio=fabs(inSpacing[1]/inSpacing[0]);
  // tan of the left and right fan angles
  double fanLinePixelRatioLeft = tan(vtkMath::RadiansFromDegrees(fanAngles[0]))*pixelAspectRatio;
  double fanLinePixelRatioRight = tan(vtkMath::RadiansFromDegrees(fanAngles[1]))*pixelAspectRatio;
  // the tan of the right fan angle is always greater than the left one
  if (fanLinePixelRatioLeft > fanLinePixelRatioRight)
  {
    // swap left and right fan lines
    double tmp = fanLinePixelRatioLeft; 
    fanLinePixelRatioLeft = fanLinePixelRatioRight; 
    fanLinePixelRatioRight = tmp;
  }
  // get the clip rectangle as an extent
  int clipExt[6];
  GetClipExtent(clipExt, inOrigin, inSpacing, inExt, clipRectangleOrigin, clipRectangleSize);

  // find maximum output range = output extent
  int outExt[6];
  outData->GetExtent(outExt);

  // Get increments to march through data - ex move from the end of one x scanline of data to the
  // start of the next line
  int outInc[3];
  outData->GetIncrements(outInc);
  int inIncX, inIncY, inIncZ;
  inData->GetContinuousIncrements(inExt, inIncX, inIncY, inIncZ);
  int numscalars = inData->GetNumberOfScalarComponents();

  // Set interpolation method - nearest neighbor or trilinear  
  int (*interpolate)(double *, T *, T *, unsigned short *, int , int a[6], int b[3])=NULL; // pointer to the nearest neighbor or trilinear interpolation function  
  vtkGetUltraInterpFunc(interpolationMode,&interpolate);

  // Loop through  slice pixels in the input extent and put them into the output volume
  // the resulting point in the output volume (outPoint) from a point in the input slice
  // (inpoint)
  double outPoint[4];
  double inPoint[4]; 
  inPoint[3] = 1;
  for (int idZ = inExt[4]; idZ <= inExt[5]; idZ++)
  {
    for (int idY = inExt[2]; idY <= inExt[3]; idY++)
    {
      for (int idX = inExt[0]; idX <= inExt[1]; idX++)
      {

        // if we are within the current clip extent
        if (idX >= clipExt[0] && idX <= clipExt[1] && 
          idY >= clipExt[2] && idY <= clipExt[3])
        {
          // x and y are the current pixel coordinates in fan coordinate system (in pixels)
          double x = (idX-fanOriginInPixels[0]);
          double y = (idY-fanOriginInPixels[1]);

          // if we are within the fan
          if ( (fanLinePixelRatioLeft == 0 && fanLinePixelRatioRight == 0) /* rectangular clipping region */ ||
            (y>0) && (x*x*inSpacingSquare[0]+y*y*inSpacingSquare[1]<fanDepthSquaredMm) && (x/y>=fanLinePixelRatioLeft) && (x/y<=fanLinePixelRatioRight) /* fan clipping region */ )
          {  
            inPoint[0] = idX;
            inPoint[1] = idY;
            inPoint[2] = idZ;

            matrix->MultiplyPoint(inPoint,outPoint);

            // deal with w (homogeneous transform) if the transform was a perspective transform
            outPoint[0] /= outPoint[3]; 
            outPoint[1] /= outPoint[3]; 
            outPoint[2] /= outPoint[3];
            outPoint[3] = 1;

            // interpolation functions return 1 if the interpolation was successful, 0 otherwise
            int hit = interpolate(outPoint, inPtr, outPtr, accPtr, numscalars, outExt, outInc);
          }
        }

        inPtr += numscalars; 
      }
      inPtr += inIncY;
    }
    inPtr += inIncZ;
  }
}

//****************************************************************************
// REAL-TIME RECONSTRUCTION - OPTIMIZED
//****************************************************************************

//----------------------------------------------------------------------------
// Actually inserts the slice, with optimization.
template <class F, class T>
static void vtkOptimizedInsertSlice(vtkImageData *outData, // the output volume
                                    T *outPtr, // scalar pointer to the output volume over the output extent
                                    unsigned short *accPtr, // scalar pointer to the accumulation buffer over the output extent
                                    vtkImageData *inData, // input slice
                                    T *inPtr, // scalar pointer to the input volume over the input slice extent
                                    int inExt[6], // input slice extent (could have been split for threading)
                                    F matrix[4][4], // index matrix, output indices -> input indices
                                    double clipRectangleOrigin[2],
                                    double clipRectangleSize[2],
                                    double fanAngles[2],
                                    double fanOrigin[2],
                                    double fanDepth,
                                    vtkPasteSliceIntoVolume::InterpolationType interpolationMode)
{
  LOG_TRACE("sliceToOutputVolumeMatrix="<<(float)matrix[0][0]<<" "<<(float)matrix[0][1]<<" "<<(float)matrix[0][2]<<" "<<(float)matrix[0][3]<<"; "
    <<(float)matrix[1][0]<<" "<<(float)matrix[1][1]<<" "<<(float)matrix[1][2]<<" "<<(float)matrix[1][3]<<"; "
    <<(float)matrix[2][0]<<" "<<(float)matrix[2][1]<<" "<<(float)matrix[2][2]<<" "<<(float)matrix[2][3]<<"; "
    <<(float)matrix[3][0]<<" "<<(float)matrix[3][1]<<" "<<(float)matrix[3][2]<<" "<<(float)matrix[3][3]
    );
  
  //////

  // slice spacing and origin
  vtkFloatingPointType inSpacing[3];  
  inData->GetSpacing(inSpacing);
  vtkFloatingPointType inOrigin[3];
  inData->GetOrigin(inOrigin);

  // number of pixels in the x and y directions between the fan origin and the slice origin  
  double fanOriginInPixels[2] =
  {
    (fanOrigin[0]-inOrigin[0])/inSpacing[0],
    (fanOrigin[1]-inOrigin[1])/inSpacing[1]
  };
  // fan depth squared 
  double fanDepthSquaredMm = fanDepth*fanDepth;

  // absolute value of slice spacing
  double inSpacingSquare[2]=
  {
    inSpacing[0]*inSpacing[0],
    inSpacing[1]*inSpacing[1]
  };

  double pixelAspectRatio=fabs(inSpacing[1]/inSpacing[0]);
  // tan of the left and right fan angles
  double fanLinePixelRatioLeft = tan(vtkMath::RadiansFromDegrees(fanAngles[0]))*pixelAspectRatio;
  double fanLinePixelRatioRight = tan(vtkMath::RadiansFromDegrees(fanAngles[1]))*pixelAspectRatio;
  // the tan of the right fan angle is always greater than the left one
  if (fanLinePixelRatioLeft > fanLinePixelRatioRight)
  {
    // swap left and right fan lines
    double tmp = fanLinePixelRatioLeft; 
    fanLinePixelRatioLeft = fanLinePixelRatioRight; 
    fanLinePixelRatioRight = tmp;
  }

  // get the clip rectangle as an extent
  int clipExt[6];
  GetClipExtent(clipExt, inOrigin, inSpacing, inExt, clipRectangleOrigin, clipRectangleSize);

  // find maximum output range = output extent
  int outExt[6];
  outData->GetExtent(outExt);

  // Get increments to march through data - ex move from the end of one x scanline of data to the
  // start of the next line
  int outInc[3];
  outData->GetIncrements(outInc);
  int inIncX, inIncY, inIncZ;
  inData->GetContinuousIncrements(inExt, inIncX, inIncY, inIncZ);
  int numscalars = inData->GetNumberOfScalarComponents();

  //////

  int outMax[3];
  int outMin[3]; // the max and min values of the output extents -
  // if outextent = (x0, x1, y0, y1, z0, z1), then
  // outMax = (x1, y1, z1) and outMin = (x0, y0, z0)
  for (int i = 0; i < 3; i++)
  {
    outMin[i] = outExt[2*i];
    outMax[i] = outExt[2*i+1];
  }

  // outPoint0, outPoint1, outPoint is a fancy way of incremetally multiplying the input point by
  // the index matrix to get the output point...  Outpoint is the result
  F outPoint0[3]; // temp, see above
  F outPoint1[3]; // temp, see above
  F outPoint[3]; // this is the final output point, created using Output0 and Output1
  F xAxis[3], yAxis[3], zAxis[3], origin[3]; // the index matrix (transform), broken up into axes and an origin

  // break matrix into a set of axes plus an origin
  // (this allows us to calculate the transform Incrementally)
  for (int i = 0; i < 3; i++)
  {
    xAxis[i]  = matrix[i][0]; // remember that the matrix is the indexMatrix, and transforms
    yAxis[i]  = matrix[i][1];  // output pixels to input pixels
    zAxis[i]  = matrix[i][2];
    origin[i] = matrix[i][3];
  }

  ///////////////////////////////////////////////
  // LOOP through output pixels and turn them all white
  // little hack when making sure that you set extents/
  // origins right in AR environment
  /*T* tempOutPtr = outPtr;
  int outIncX, outIncY, outIncZ;
  outData->GetContinuousIncrements(outExt, outIncX, outIncY, outIncZ);
  int outNumScalars = outData->GetNumberOfScalarComponents();
  for (idZ = outExt[4]; idZ <= outExt[5]; idZ++)
  {
  for (idY = outExt[2]; idY <= outExt[3]; idY++)
  {
  for (idX = outExt[0]; idX <= outExt[1]; idX++)
  {
  *tempOutPtr = 255;
  tempOutPtr += outNumScalars;
  }
  tempOutPtr += outIncY;
  }
  tempOutPtr += outIncZ;
  }*/
  ///////////////////////////////////////////////

  int xIntersectionPixStart,xIntersectionPixEnd;

  // Loop through INPUT pixels - remember this is a 3D cube represented by the input extent
  for (int idZ = inExt[4]; idZ <= inExt[5]; idZ++) // for each image...
  {
    outPoint0[0] = origin[0]+idZ*zAxis[0]; // incremental transform
    outPoint0[1] = origin[1]+idZ*zAxis[1];
    outPoint0[2] = origin[2]+idZ*zAxis[2];

    for (int idY = inExt[2]; idY <= inExt[3]; idY++) // for each horizontal line in the image...
    {
      outPoint1[0] = outPoint0[0]+idY*yAxis[0]; // incremental transform
      outPoint1[1] = outPoint0[1]+idY*yAxis[1];
      outPoint1[2] = outPoint0[2]+idY*yAxis[2];

      // find intersections of x raster line with the output extent

      // this only changes xIntersectionPixStart and xIntersectionPixEnd
      vtkUltraFindExtent(xIntersectionPixStart,xIntersectionPixEnd,outPoint1,xAxis,outMin,outMax,inExt);

      // next, handle the 'fan' shape of the input
      double y = idY - fanOriginInPixels[1];

      // first, check the angle range of the fan - choose xIntersectionPixStart and xIntersectionPixEnd based
      // on the triangle that the fan makes from the fan origin to the bottom
      // line of the video image
      if (!(fanLinePixelRatioLeft == 0 && fanLinePixelRatioRight == 0))
      {
        // equivalent to: xIntersectionPixStart < vtkUltraCeil(fanLinePixelRatioLeft*y + fanOriginInPixels[0] + 1)
        // this is what the radius would be based on tan(fanAngle)
        if (xIntersectionPixStart < -vtkUltraFloor(-(fanLinePixelRatioLeft*y + fanOriginInPixels[0] + 1)))
        {
          xIntersectionPixStart = -vtkUltraFloor(-(fanLinePixelRatioLeft*y + fanOriginInPixels[0] + 1));
        }
        if (xIntersectionPixEnd > vtkUltraFloor(fanLinePixelRatioRight*y + fanOriginInPixels[0] - 1))
        {
          xIntersectionPixEnd = vtkUltraFloor(fanLinePixelRatioRight*y + fanOriginInPixels[0] - 1);
        }

        // next, check the radius of the fan - crop the triangle to the fan
        // depth
        double dx = (fanDepthSquaredMm - (y*y)*inSpacingSquare[1])/inSpacingSquare[0];

        // if we are outside the fan's radius, ex at the bottom lines
        if (dx < 0)
        {
          xIntersectionPixStart = inExt[0];
          xIntersectionPixEnd = inExt[0]-1;
        }
        // if we are within the fan's radius, we have to adjust if we are in
        // the "ellipsoidal" (bottom) part of the fan instead of the top
        // "triangular" part
        else
        {
          dx = sqrt(dx);
          // this is what xIntersectionPixStart would be if we calculated it based on the
          // pythagorean theorem
          if (xIntersectionPixStart < -vtkUltraFloor(-(fanOriginInPixels[0] - dx + 1)))
          {
            xIntersectionPixStart = -vtkUltraFloor(-(fanOriginInPixels[0] - dx + 1));
          }
          if (xIntersectionPixEnd > vtkUltraFloor(fanOriginInPixels[0] + dx - 1))
          {
            xIntersectionPixEnd = vtkUltraFloor(fanOriginInPixels[0] + dx - 1);
          }
        }
      }

      // bound to the ultrasound clip rectangle
      if (xIntersectionPixStart < clipExt[0])
      {
        xIntersectionPixStart = clipExt[0];
      }
      if (xIntersectionPixEnd > clipExt[1])
      {
        xIntersectionPixEnd = clipExt[1];
      }

      if (xIntersectionPixStart > xIntersectionPixEnd) // TODO synchrograb adds || idY < clipExt[2] || idY > clipExt[3])
      {
        xIntersectionPixStart = inExt[0];
        xIntersectionPixEnd = inExt[0]-1;
      }

      // skip the portion of the slice to the left of the fan
      for (int idX = inExt[0]; idX < xIntersectionPixStart; idX++)
      {
        inPtr += numscalars;
      }
      // multiplying the input point by the transform will give you fractional pixels,
      // so we need interpolation
      
      if (interpolationMode == vtkPasteSliceIntoVolume::LINEAR_INTERPOLATION)
      { 
        // interpolating linearly (code 1)
        for (int idX = xIntersectionPixStart; idX <= xIntersectionPixEnd; idX++) // for all of the x pixels within the fan
        {
          outPoint[0] = outPoint1[0] + idX*xAxis[0];
          outPoint[1] = outPoint1[1] + idX*xAxis[1];
          outPoint[2] = outPoint1[2] + idX*xAxis[2];

          int hit = vtkTrilinearInterpolation(outPoint, inPtr, outPtr, accPtr, numscalars, outExt, outInc); // hit is either 1 or 0

          inPtr += numscalars; // go to the next x pixel
        }
      }      
      else 
      {
        // interpolating with nearest neighbor
        vtkFreehand2OptimizedNNHelper(xIntersectionPixStart, xIntersectionPixEnd, outPoint, outPoint1, xAxis, 
          inPtr, outPtr, outExt, outInc,
          numscalars, accPtr);
        // we added all the pixels between xIntersectionPixStart and xIntersectionPixEnd, so increment our count of the number of pixels added
      }

      // skip the portion of the slice to the right of the fan
      for (int idX = xIntersectionPixEnd+1; idX <= inExt[1]; idX++)
      {
        inPtr += numscalars;
      }

      inPtr += inIncY; // move to the next line
    }
    inPtr += inIncZ; // move to the next image
  }
}

#endif
