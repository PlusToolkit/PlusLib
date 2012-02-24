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
  \file vtkPasteSliceIntoVolumeHelperCommon.h
  \brief Helper functions for pasting slice into volume

  Contains common interpolation and clipping functions for vtkPasteSliceIntoVolume

  \sa vtkPasteSliceIntoVolume, vtkPasteSliceIntoVolumeHelperOptimized, vtkPasteSliceIntoVolumeHelperUnoptimized
  \ingroup PlusLibVolumeReconstruction
*/
#ifndef __vtkPasteSliceIntoVolumeHelperCommon_h
#define __vtkPasteSliceIntoVolumeHelperCommon_h

#include "PlusConfigure.h"

#include "vtkImageData.h"

#include "PlusMath.h"


/*!
  Implements trilinear interpolation

  Does reverse trilinear interpolation. Trilinear interpolation would use 
  the pixel values to interpolate something in the middle we have the 
  something in the middle and want to spread it to the discrete pixel 
  values around it, in an interpolated way

  Do trilinear interpolation of the input data 'inPtr' of extent 'inExt'
  at the 'point'.  The result is placed at 'outPtr'.  
  If the lookup data is beyond the extent 'inExt', set 'outPtr' to
  the background color 'background'.  
  The number of scalar components in the data is 'numscalars'
*/
template <class F, class T>
static int vtkTrilinearInterpolation(F *point, T *inPtr, T *outPtr,
                                     unsigned short *accPtr, int numscalars, 
                                     int outExt[6], int outInc[3])
{
  F fx, fy, fz;

  // convert point[0] into integer component and a fraction
  int outIdX0 = PlusMath::Floor(point[0], fx);
  // point[0] is unchanged, outIdX0 is the integer (floor), fx is the float
  int outIdY0 = PlusMath::Floor(point[1], fy);
  int outIdZ0 = PlusMath::Floor(point[2], fz);

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
        accPtrTmp = accPtr+ ((idx[j]/outInc[0])); // removed cast to unsigned short - prevented larger increments in Z direction
        f = fdx[j];
        r = F((*accPtrTmp)/255.);	// added division by float or double, since this always returned 0 otherwise
        a = f + r;

        int i = numscalars;
        do
        {
          i--;
          PlusMath::Round((f*(*inPtrTmp++) + r*(*outPtrTmp))/a, *outPtrTmp);
          outPtrTmp++;
        }
        while (i);

        *accPtrTmp = 65535;
        *outPtrTmp = 255;
        a *= 255;
        // don't allow accumulation buffer overflow
        if (a < F(65535))
        {
          PlusMath::Round(a, *accPtrTmp);
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
            PlusMath::Round(f*(*inPtrTmp++) + r*(*outPtrTmp), *outPtrTmp);
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


//----------------------------------------------------------------------------
/*!
  Convert the ClipRectangle into a clip extent that can be applied to the
  input data - number of pixels (+ or -) from the origin (the z component 
  is copied from the inExt parameter)

  \param clipExt {x0, x1, y0, y1, z0, z1} the "output" of this function is to change this array
  \param inOrigin = {x, y, z} the origin in mm
  \param inSpacing = {x, y, z} the spacing in mm
  \param inExt = {x0, x1, y0, y1, z0, z1} min/max possible extent, in pixels
  \param clipRectangleOrigin = {x, y} origin of the clipping rectangle in the image, in pixels
  \param clipRectangleSize = {x, y} size of the clipping rectangle in the image, in pixels
*/
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

#endif
