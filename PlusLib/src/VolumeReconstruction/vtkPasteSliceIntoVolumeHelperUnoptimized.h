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
  \file vtkPasteSliceIntoVolumeHelperUnoptimized.h
  \brief Unoptimized helper functions for pasting slice into volume

  Contains unoptimized interpolation and slice insertion functions for vtkPasteSliceIntoVolume

  \sa vtkPasteSliceIntoVolume, vtkPasteSliceIntoVolumeHelperCommon, vtkPasteSliceIntoVolumeHelperOptimized, vtkPasteSliceIntoVolumeHelperUnoptimized
  \ingroup PlusLibVolumeReconstruction
*/

#ifndef __vtkPasteSliceIntoVolumeHelperUnoptimized_h
#define __vtkPasteSliceIntoVolumeHelperUnoptimized_h

#include "vtkPasteSliceIntoVolumeHelperCommon.h"
#include "vtkMatrix4x4.h"

/*!
  Non-optimized nearest neighbor interpolation.

  In the un-optimized version, each output voxel
  is converted into a set of look-up indices for the input data;
  then, the indices are checked to ensure they lie within the
  input data extent.

  In the optimized versions, the check is done in reverse:
  it is first determined which output voxels map to look-up indices
  within the input data extent.  Then, further calculations are
  done only for those voxels.  This means that 1) minimal work
  is done for voxels which map to regions outside fo the input
  extent (they are just set to the background color) and 2)
  the inner loops of the look-up and interpolation are
  tightened relative to the un-uptimized version.

  Do nearest-neighbor interpolation of the input data 'inPtr' of extent 
  'inExt' at the 'point'.  The result is placed at 'outPtr'.  
  If the lookup data is beyond the extent 'inExt', set 'outPtr' to
  the background color 'background'.  
  The number of scalar components in the data is 'numscalars'
*/
template <class F, class T>
static int vtkNearestNeighborInterpolation(F *point, T *inPtr, T *outPtr,
                                           unsigned short *accPtr, 
                                           int numscalars, vtkPasteSliceIntoVolume::CalculationType calculationMode,
                                           int outExt[6], vtkIdType outInc[3], unsigned int* accOverflowCount)
{
  int i;
  // The nearest neighbor interpolation occurs here
  // The output point is the closest point to the input point - rounding
  // to get closest point
  int outIdX = PlusMath::Round(point[0])-outExt[0];
  int outIdY = PlusMath::Round(point[1])-outExt[2];
  int outIdZ = PlusMath::Round(point[2])-outExt[4];

  // fancy way of checking bounds
  if ((outIdX | (outExt[1]-outExt[0] - outIdX) |
    outIdY | (outExt[3]-outExt[2] - outIdY) |
    outIdZ | (outExt[5]-outExt[4] - outIdZ)) >= 0)
  {
    int inc = outIdX*outInc[0]+outIdY*outInc[1]+outIdZ*outInc[2];
    outPtr += inc;
    if (calculationMode == vtkPasteSliceIntoVolume::MAXIMUM)
    {
      if (accPtr) // accumulation buffer: do compounding
      {
        accPtr += inc/outInc[0];
        int newa = *accPtr + ACCUMULATION_MULTIPLIER;
        for (i = 0; i < numscalars; i++)
        {
          if (*inPtr > *outPtr)
            *outPtr = *inPtr;
          inPtr++;
          outPtr++;
        }
        *outPtr = (T)OPAQUE_ALPHA; // set the alpha value to opaque
        *accPtr = ACCUMULATION_MAXIMUM; // set to 0xFFFF by default for overflow protection
        if (newa < ACCUMULATION_MAXIMUM)
        {
          *accPtr = newa;
        }
      }
      else
      {
        for (i = 0; i < numscalars; i++)
        {
          if (*inPtr > *outPtr)
            *outPtr = *inPtr;
          outPtr++;
          inPtr++;
        }
        *outPtr = (T)OPAQUE_ALPHA;
      }
    }
    else {
      if (accPtr) // accumulation buffer: do compounding
      {
        accPtr += inc/outInc[0];
        if (*accPtr <= ACCUMULATION_THRESHOLD) { // no overflow, act normally
          int newa = *accPtr + ACCUMULATION_MULTIPLIER;
          if (newa > ACCUMULATION_THRESHOLD)
            (*accOverflowCount) += 1;
          for (i = 0; i < numscalars; i++)
          {
            *outPtr = ((*inPtr++)*ACCUMULATION_MULTIPLIER + (*outPtr)*(*accPtr))/newa;
            outPtr++;
          }
          *outPtr = (T)OPAQUE_ALPHA; // set the alpha value to opaque
          *accPtr = ACCUMULATION_MAXIMUM; // set to 0xFFFF by default for overflow protection
          if (newa < ACCUMULATION_MAXIMUM)
          {
            *accPtr = newa;
          }
        } else { // overflow, use recursive filtering with 255/256 and 1/256 as the weights, since 255 voxels have been inserted so far
          *outPtr = (T)(0.99609375 * (*inPtr++) + 0.00390625 * (*outPtr));
        }
      }
      // no accumulation buffer, replace what was there before
      else
      {
        for (i = 0; i < numscalars; i++)
        {
          *outPtr++ = *inPtr++;
        }
        *outPtr = (T)OPAQUE_ALPHA;
      }
    }
    return 1;
  }
  return 0;
} 

//----------------------------------------------------------------------------
/*!
  Actually inserts the slice - executes the filter for any type of data, without optimization
  Given an input and output region, execute the filter algorithm to fill the
  output from the input - no optimization.
  (this one function is pretty much the be-all and end-all of the filter)
*/
template <class T>
static void vtkUnoptimizedInsertSlice(vtkImageData *outData, T *outPtr, unsigned short *accPtr, vtkImageData *inData, T *inPtr, int inExt[6], vtkMatrix4x4 *matrix,
  double clipRectangleOrigin[2],double clipRectangleSize[2], double fanAngles[2], double fanOrigin[2], double fanDepth, vtkPasteSliceIntoVolume::InterpolationType interpolationMode, vtkPasteSliceIntoVolume::CalculationType calculationMode, unsigned int * accOverflowCount)
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
  vtkIdType outInc[3] ={0};
  outData->GetIncrements(outInc);
  vtkIdType inIncX=0, inIncY=0, inIncZ=0;
  inData->GetContinuousIncrements(inExt, inIncX, inIncY, inIncZ);
  int numscalars = inData->GetNumberOfScalarComponents();

  // Set interpolation method - nearest neighbor or trilinear  
  int (*interpolate)(double *, T *, T *, unsigned short *, int, vtkPasteSliceIntoVolume::CalculationType, int a[6], vtkIdType b[3], unsigned int *)=NULL; // pointer to the nearest neighbor or trilinear interpolation function  
  switch (interpolationMode)
  {
  case vtkPasteSliceIntoVolume::NEAREST_NEIGHBOR_INTERPOLATION:
    interpolate = &vtkNearestNeighborInterpolation;
    break;
  case vtkPasteSliceIntoVolume::LINEAR_INTERPOLATION:
    interpolate = &vtkTrilinearInterpolation;
    break;
  default:
    LOG_ERROR("Unkown interpolation mode: "<<interpolationMode);
    return;
  }

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
            int hit = interpolate(outPoint, inPtr, outPtr, accPtr, numscalars, calculationMode, outExt, outInc, accOverflowCount);
          }
        }

        inPtr += numscalars; 
      }
      inPtr += inIncY;
    }
    inPtr += inIncZ;
  }
}


#endif
