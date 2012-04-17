/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

/*=========================================================================
The following copyright notice is applicable to parts of this file:

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

/*!
  \file vtkPasteSliceIntoVolumeHelperOptimized.h
  \brief Optimized helper functions for pasting slice into volume

  Contains optimized interpolation and slice insertion functions for vtkPasteSliceIntoVolume

  \sa vtkPasteSliceIntoVolume, vtkPasteSliceIntoVolumeHelperCommon, vtkPasteSliceIntoVolumeHelperOptimized, vtkPasteSliceIntoVolumeHelperUnoptimized
  \ingroup PlusLibVolumeReconstruction
*/

#ifndef __vtkPasteSliceIntoVolumeHelperOptimized_h
#define __vtkPasteSliceIntoVolumeHelperOptimized_h

#include "vtkPasteSliceIntoVolumeHelperCommon.h"
#include "fixed.h"

//----------------------------------------------------------------------------
/*! 
  Find approximate intersection of line with the plane
  x = x_min, y = y_min, or z = z_min (lower limit of data extent) 
*/
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
/*! Find the point just inside the extent */
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
      PlusMath::Round(p) < limit[ai])
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
      PlusMath::Round(p) >= limit[ai])
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
/*! Find the point just inside the extent, for x = x_max */
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
      PlusMath::Round(p) > limit[ai])
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
      PlusMath::Round(p) <= limit[ai])
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
static int isBounded(F *point, F *xAxis, int *inMin, int *inMax, int ai, int r)
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

  int bp = PlusMath::Round(fbp);
  int cp = PlusMath::Round(fcp);

  return (bp >= inMin[bi] && bp <= inMax[bi] &&
    cp >= inMin[ci] && cp <= inMax[ci]);
}

//----------------------------------------------------------------------------
/*!
  This huge mess finds out where the current output raster
  line intersects the input volume
*/
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

    indx1[i] = PlusMath::Round(p1);
    indx2[i] = PlusMath::Round(p2);
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



//----------------------------------------------------------------------------
/*! Optimized nearest neighbor interpolation, without integer mathematics */
template<class T>
static inline void vtkFreehand2OptimizedNNHelper(int xIntersectionPixStart, int xIntersectionPixEnd,
                                                 double *outPoint,
                                                 double *outPoint1,
                                                 double *xAxis,
                                                 T *&inPtr, T *outPtr,
                                                 int *outExt, int *outInc,
                                                 int numscalars, vtkPasteSliceIntoVolume::CalculationType calculationMode, 
                                                 unsigned short *accPtr)
{
  switch (calculationMode) {
  case  vtkPasteSliceIntoVolume::WEIGHTED_AVERAGE :
    // with compounding
    if (accPtr)
    {

      for (int idX = xIntersectionPixStart; idX <= xIntersectionPixEnd; idX++)
      {
        outPoint[0] = outPoint1[0] + idX*xAxis[0]; 
        outPoint[1] = outPoint1[1] + idX*xAxis[1];
        outPoint[2] = outPoint1[2] + idX*xAxis[2];

        int outIdX = PlusMath::Round(outPoint[0]) - outExt[0];
        int outIdY = PlusMath::Round(outPoint[1]) - outExt[2];
        int outIdZ = PlusMath::Round(outPoint[2]) - outExt[4];

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
        unsigned short *accPtr1 = accPtr + (inc/outInc[0]); // removed cast to unsigned short because it might cause loss in larger numbers
        unsigned short newa = *accPtr1 + ((unsigned short)(255)); 
        int i = numscalars;
        do 
        {
          i--;
          *outPtr1 = ((*inPtr++)*255 + (*outPtr1)*(*accPtr1))/newa;
          outPtr1++;
        }
        while (i);

        *outPtr1 = (T)OPAQUE_ALPHA;
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

        int outIdX = PlusMath::Round(outPoint[0]) - outExt[0];
        int outIdY = PlusMath::Round(outPoint[1]) - outExt[2];
        int outIdZ = PlusMath::Round(outPoint[2]) - outExt[4];

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
        *outPtr1 = (T)OPAQUE_ALPHA;
      }
    }
    break;
  case  vtkPasteSliceIntoVolume::MAXIMUM :
    // with compounding
    if (accPtr)
    {

      for (int idX = xIntersectionPixStart; idX <= xIntersectionPixEnd; idX++)
      {
        outPoint[0] = outPoint1[0] + idX*xAxis[0]; 
        outPoint[1] = outPoint1[1] + idX*xAxis[1];
        outPoint[2] = outPoint1[2] + idX*xAxis[2];

        int outIdX = PlusMath::Round(outPoint[0]) - outExt[0];
        int outIdY = PlusMath::Round(outPoint[1]) - outExt[2];
        int outIdZ = PlusMath::Round(outPoint[2]) - outExt[4];

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
        unsigned short *accPtr1 = accPtr + (inc/outInc[0]); // removed cast to unsigned short because it might cause loss in larger numbers
        int i = numscalars;
        do 
        {
          i--;
          if (*outPtr1 < *inPtr)
            *outPtr1 = *inPtr;
          outPtr1++;
          inPtr++;
        }
        while (i);

        *outPtr1 = (T)OPAQUE_ALPHA;
        *accPtr1 = (unsigned short)255;
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

        int outIdX = PlusMath::Round(outPoint[0]) - outExt[0];
        int outIdY = PlusMath::Round(outPoint[1]) - outExt[2];
        int outIdZ = PlusMath::Round(outPoint[2]) - outExt[4];

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
          if (*outPtr1 < *inPtr)
            *outPtr1 = *inPtr;
          inPtr++;
          outPtr1++;
        }
        while (i);
        *outPtr1 = (T)OPAQUE_ALPHA;
      }
    }
    break;
  }
}

//----------------------------------------------------------------------------
/*! 
  Optimized nearest neighbor interpolation, specifically optimized for fixed
  point (i.e. integer) mathematics
*/
template <class T>
static inline void vtkFreehand2OptimizedNNHelper(int xIntersectionPixStart, int xIntersectionPixEnd,
                                                 fixed *outPoint,
                                                 fixed *outPoint1, fixed *xAxis,
                                                 T *&inPtr, T *outPtr,
                                                 int *outExt, int *outInc,
                                                 int numscalars, vtkPasteSliceIntoVolume::CalculationType calculationMode,
                                                 unsigned short *accPtr)
{
  outPoint[0] = outPoint1[0] + xIntersectionPixStart*xAxis[0] - outExt[0];
  outPoint[1] = outPoint1[1] + xIntersectionPixStart*xAxis[1] - outExt[2];
  outPoint[2] = outPoint1[2] + xIntersectionPixStart*xAxis[2] - outExt[4];

  switch (calculationMode) {
  case  vtkPasteSliceIntoVolume::WEIGHTED_AVERAGE :
    // Nearest-Neighbor, no extent checks, with accumulation
    if (accPtr)
    {
      for (int idX = xIntersectionPixStart; idX <= xIntersectionPixEnd; idX++)
      {
        int outIdX = PlusMath::Round(outPoint[0]);
        int outIdY = PlusMath::Round(outPoint[1]);
        int outIdZ = PlusMath::Round(outPoint[2]);

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
        unsigned short *accPtr1 = accPtr + (inc/outInc[0]);
        unsigned short newa = *accPtr1 + ((unsigned short)(255));
        int i = numscalars;
        do 
        {
          i--;
          *outPtr1 = ((*inPtr++)*255 + (*outPtr1)*(*accPtr1))/newa;
          outPtr1++;
        }
        while (i);

        *outPtr1 = (T)OPAQUE_ALPHA;
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
        int outIdX = PlusMath::Round(outPoint[0]);
        int outIdY = PlusMath::Round(outPoint[1]);
        int outIdZ = PlusMath::Round(outPoint[2]);

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
        *outPtr1 = (T)OPAQUE_ALPHA;

        outPoint[0] += xAxis[0];
        outPoint[1] += xAxis[1];
        outPoint[2] += xAxis[2];
      }
    } 
    break;
  case  vtkPasteSliceIntoVolume::MAXIMUM :
    // Nearest-Neighbor, no extent checks, with accumulation
    if (accPtr)
    {
      for (int idX = xIntersectionPixStart; idX <= xIntersectionPixEnd; idX++)
      {
        int outIdX = PlusMath::Round(outPoint[0]);
        int outIdY = PlusMath::Round(outPoint[1]);
        int outIdZ = PlusMath::Round(outPoint[2]);

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
        unsigned short *accPtr1 = accPtr + (inc/outInc[0]);
        int i = numscalars;
        do 
        {
          i--;
          if (*outPtr1 < *inPtr)
            *outPtr1 = *inPtr;
          outPtr1++;
          inPtr++;
        }
        while (i);

        *outPtr1 = (T)OPAQUE_ALPHA;
        *accPtr1 = (unsigned short)255;

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
        int outIdX = PlusMath::Round(outPoint[0]);
        int outIdY = PlusMath::Round(outPoint[1]);
        int outIdZ = PlusMath::Round(outPoint[2]);

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
          if (*outPtr1 < *inPtr)
            *outPtr1 = *inPtr;
          outPtr1++;
          inPtr++;
        }
        while (i);
        *outPtr1 = (T)OPAQUE_ALPHA;

        outPoint[0] += xAxis[0];
        outPoint[1] += xAxis[1];
        outPoint[2] += xAxis[2];
      }
    }
    break;
  }
}

//----------------------------------------------------------------------------
/*! Actually inserts the slice, with optimization */
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
                                    vtkPasteSliceIntoVolume::InterpolationType interpolationMode,
                                    vtkPasteSliceIntoVolume::CalculationType calculationMode)
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
        // equivalent to: xIntersectionPixStart < PlusMath::Ceil(fanLinePixelRatioLeft*y + fanOriginInPixels[0] + 1)
        // this is what the radius would be based on tan(fanAngle)
        if (xIntersectionPixStart < -PlusMath::Floor(-(fanLinePixelRatioLeft*y + fanOriginInPixels[0] + 1)))
        {
          xIntersectionPixStart = -PlusMath::Floor(-(fanLinePixelRatioLeft*y + fanOriginInPixels[0] + 1));
        }
        if (xIntersectionPixEnd > PlusMath::Floor(fanLinePixelRatioRight*y + fanOriginInPixels[0] - 1))
        {
          xIntersectionPixEnd = PlusMath::Floor(fanLinePixelRatioRight*y + fanOriginInPixels[0] - 1);
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
          if (xIntersectionPixStart < -PlusMath::Floor(-(fanOriginInPixels[0] - dx + 1)))
          {
            xIntersectionPixStart = -PlusMath::Floor(-(fanOriginInPixels[0] - dx + 1));
          }
          if (xIntersectionPixEnd > PlusMath::Floor(fanOriginInPixels[0] + dx - 1))
          {
            xIntersectionPixEnd = PlusMath::Floor(fanOriginInPixels[0] + dx - 1);
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

          int hit = vtkTrilinearInterpolation(outPoint, inPtr, outPtr, accPtr, numscalars, calculationMode, outExt, outInc); // hit is either 1 or 0

          inPtr += numscalars; // go to the next x pixel
        }
      }      
      else 
      {
        // interpolating with nearest neighbor
        vtkFreehand2OptimizedNNHelper(xIntersectionPixStart, xIntersectionPixEnd, outPoint, outPoint1, xAxis, 
          inPtr, outPtr, outExt, outInc,
          numscalars, calculationMode, accPtr);
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
