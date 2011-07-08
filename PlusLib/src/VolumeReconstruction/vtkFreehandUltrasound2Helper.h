/*=========================================================================

Program:   Visualization Toolkit
Module:    $RCSfile: vtkFreehandUltrasound2Helper.h,v $
Language:  C++
Date:      $Date: 2009/07/20 18:48:07 $
Version:   $Revision: 1.14 $
Thanks:    Thanks to David G. Gobbi who developed this class. 
Thanks:    Thanks to Danielle Pace who developed this class.

==========================================================================

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
// vtkVideoSource2, vtkTracker, vtkTrackerTool

#ifndef __vtkFreehandUltrasound2Helper_h
#define __vtkFreehandUltrasound2Helper_h

#include "PlusConfigure.h"
#include "vtkTimerLog.h"
#include "vtkMatrix4x4.h"
#include "vtkVideoSource2.h"
#include "vtkTransform.h"
#include "vtkImageThreshold.h"
#include "vtkImageClip.h"
#include "vtkImageData.h"
#include "vtkTrackerBuffer.h"
#include "vtkVideoSource2.h"
#include "vtkTrackerTool.h"
#include "fixed.h"
#include "vtkSmartPointer.h"
#include "vtkMultiThreader.h"

class vtkFreehandUltrasound2;

// includes for sleep
#ifdef _WIN32
#include "vtkWindows.h"
#else
#include <time.h>
#endif

// includes for mkdir
#ifdef _WIN32
#include <direct.h>
#else
#include <sys/types.h>
#include <sys/stat.h>
#endif

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
// SLEEP CODE
//***************************************************************************

//----------------------------------------------------------------------------
// platform-independent sleep function (duration in seconds)
static inline void vtkSleep(double duration)
{
    duration = duration; // avoid warnings
    // sleep according to OS preference
    vtkAccurateTimer::Delay(duration); 
}

//----------------------------------------------------------------------------
// Sleep until the specified absolute time has arrived.
// You must pass a handle to the current thread.  
// If '0' is returned, then the thread was aborted before or during the wait.
static int vtkThreadSleep(ThreadInfoStruct *data, double time)
{

    for (;;)
    {
        // slice 10 millisecs off the time, since this is how long it will
        // take for this thread to start executing once it has been
        // re-scheduled
        double remaining = time - vtkAccurateTimer::GetSystemTime() - 0.01;

        // check to see if we have reached the specified time
        if (remaining <= 0)
        {
            return 1;
        }
        // check the ActiveFlag at least every 0.1 seconds
        if (remaining > 0.1)
        {
            remaining = 0.1;
        }

        // check to see if we are being told to quit 
        if (*(data->ActiveFlag) == 0)
        {
            return 0;
        }

        vtkSleep(remaining);
    }

    return 1;
}

//****************************************************************************
// INTERPOLATION CODE
//****************************************************************************

//----------------------------------------------------------------------------
// Sets interpolate (pointer to a function) to match the current interpolation
// mode - used for unoptimized versions only
template <class F, class T>
static void vtkGetUltraInterpFunc(vtkFreehandUltrasound2 *self, 
                                  int (**interpolate)(F *point, 
                                  T *inPtr, T *outPtr,
                                  unsigned short *accPtr,
                                  int numscalars, 
                                  int outExt[6], 
                                  int outInc[3]))
{
    switch (self->GetInterpolationMode())
    {
    case VTK_FREEHAND_NEAREST:
        *interpolate = &vtkNearestNeighborInterpolation;
        break;
    case VTK_FREEHAND_LINEAR:
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
static inline void vtkFreehand2OptimizedNNHelper(int r1, int r2,
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

        for (int idX = r1; idX <= r2; idX++)
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
        for (int idX = r1; idX <= r2; idX++)
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
static inline void vtkFreehand2OptimizedNNHelper(int r1, int r2,
                                                 fixed *outPoint,
                                                 fixed *outPoint1, fixed *xAxis,
                                                 T *&inPtr, T *outPtr,
                                                 int *outExt, int *outInc,
                                                 int numscalars, 
                                                 unsigned short *accPtr)
{
    outPoint[0] = outPoint1[0] + r1*xAxis[0] - outExt[0];
    outPoint[1] = outPoint1[1] + r1*xAxis[1] - outExt[2];
    outPoint[2] = outPoint1[2] + r1*xAxis[2] - outExt[4];

    // Nearest-Neighbor, no extent checks, with accumulation
    if (accPtr)
    {
        for (int idX = r1; idX <= r2; idX++)
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
        for (int idX = r1; idX <= r2; idX++)
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
// check a matrix to see whether it is the identity matrix
static int vtkIsIdentityMatrix(vtkMatrix4x4 *matrix)
{
    static double identity[16] = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1};
    int i,j;

    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 4; j++)
        {
            if (matrix->GetElement(i,j) != identity[4*i+j])
            {
                return 0;
            }
        }
    }
    return 1;
}

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
static void vtkUltraFindExtentHelper(int &r1, int &r2, int sign, int *inExt)
{
    if (sign < 0)
    {
        int i = r1;
        r1 = r2;
        r2 = i;
    }

    // bound r1,r2 within reasonable limits
    if (r1 < inExt[0]) 
    {
        r1 = inExt[0];
    }
    if (r2 > inExt[1]) 
    {
        r2 = inExt[1];
    }
    if (r1 > r2) 
    {
        r1 = inExt[0];
        r2 = inExt[0]-1;
    }
}  

//----------------------------------------------------------------------------
template <class F>
static void vtkUltraFindExtent(int& r1, int& r2, F *point, F *xAxis, 
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

    r1 = intersectionLow(point,xAxis,sign,inMin,ix,inExt);
    r2 = intersectionHigh(point,xAxis,sign,inMax,ix,inExt);

    // find points of intersections
    // first, find w-value for perspective (will usually be 1)
    for (i = 0; i < 3; i++)
    {
        p1 = point[i]+r1*xAxis[i];
        p2 = point[i]+r2*xAxis[i];

        indx1[i] = vtkUltraRound(p1);
        indx2[i] = vtkUltraRound(p2);
    }

    // passed through x face, check opposing face
    if (isBounded(point,xAxis,inMin,inMax,ix,r1))
    {
        if (isBounded(point,xAxis,inMin,inMax,ix,r2))
        {
            vtkUltraFindExtentHelper(r1,r2,sign[ix],inExt);
            return;
        }

        // check y face
        if (indx2[iy] < inMin[iy])
        {
            r2 = intersectionLow(point,xAxis,sign,inMin,iy,inExt);
            if (isBounded(point,xAxis,inMin,inMax,iy,r2))
            {
                vtkUltraFindExtentHelper(r1,r2,sign[ix],inExt);
                return;
            }
        }

        // check other y face
        else if (indx2[iy] > inMax[iy])
        {
            r2 = intersectionHigh(point,xAxis,sign,inMax,iy,inExt);
            if (isBounded(point,xAxis,inMin,inMax,iy,r2))
            {
                vtkUltraFindExtentHelper(r1,r2,sign[ix],inExt);
                return;
            }
        }

        // check z face
        if (indx2[iz] < inMin[iz])
        {
            r2 = intersectionLow(point,xAxis,sign,inMin,iz,inExt);
            if (isBounded(point,xAxis,inMin,inMax,iz,r2))
            {
                vtkUltraFindExtentHelper(r1,r2,sign[ix],inExt);
                return;
            }
        }

        // check other z face
        else if (indx2[iz] > inMax[iz])
        {
            r2 = intersectionHigh(point,xAxis,sign,inMax,iz,inExt);
            if (isBounded(point,xAxis,inMin,inMax,iz,r2))
            {
                vtkUltraFindExtentHelper(r1,r2,sign[ix],inExt);
                return;
            }
        }
    }

    // passed through the opposite x face
    if (isBounded(point,xAxis,inMin,inMax,ix,r2))
    {
        // check y face
        if (indx1[iy] < inMin[iy])
        {
            r1 = intersectionLow(point,xAxis,sign,inMin,iy,inExt);
            if (isBounded(point,xAxis,inMin,inMax,iy,r1))
            {
                vtkUltraFindExtentHelper(r1,r2,sign[ix],inExt);
                return;
            }
        }
        // check other y face
        else if (indx1[iy] > inMax[iy])
        {
            r1 = intersectionHigh(point,xAxis,sign,inMax,iy,inExt);
            if (isBounded(point,xAxis,inMin,inMax,iy,r1))
            {
                vtkUltraFindExtentHelper(r1,r2,sign[ix],inExt);
                return;
            }
        }

        // check other y face
        if (indx1[iz] < inMin[iz])
        {
            r1 = intersectionLow(point,xAxis,sign,inMin,iz,inExt);
            if (isBounded(point,xAxis,inMin,inMax,iz,r1))
            {
                vtkUltraFindExtentHelper(r1,r2,sign[ix],inExt);
                return;
            }
        }
        // check other z face
        else if (indx1[iz] > inMax[iz])
        {
            r1 = intersectionHigh(point,xAxis,sign,inMax,iz,inExt);
            if (isBounded(point,xAxis,inMin,inMax,iz,r1))
            {
                vtkUltraFindExtentHelper(r1,r2,sign[ix],inExt);
                return;
            }
        }
    }

    // line might pass through bottom face
    if ((indx1[iy] >= inMin[iy] && indx2[iy] < inMin[iy]) ||
        (indx1[iy] < inMin[iy] && indx2[iy] >= inMin[iy]))
    {
        r1 = intersectionLow(point,xAxis,sign,inMin,iy,inExt);
        if (isBounded(point,xAxis,inMin,inMax,iy,r1))
        {
            // line might pass through top face
            if ((indx1[iy] <= inMax[iy] && indx2[iy] > inMax[iy]) ||
                (indx1[iy] > inMax[iy] && indx2[iy] <= inMax[iy]))
            { 
                r2 = intersectionHigh(point,xAxis,sign,inMax,iy,inExt);
                if (isBounded(point,xAxis,inMin,inMax,iy,r2))
                {
                    vtkUltraFindExtentHelper(r1,r2,sign[iy],inExt);
                    return;
                }
            }

            // line might pass through in-to-screen face
            if (indx1[iz] < inMin[iz] && indx2[iy] < inMin[iy] ||
                indx2[iz] < inMin[iz] && indx1[iy] < inMin[iy])
            { 
                r2 = intersectionLow(point,xAxis,sign,inMin,iz,inExt);
                if (isBounded(point,xAxis,inMin,inMax,iz,r2))
                {
                    vtkUltraFindExtentHelper(r1,r2,sign[iy],inExt);
                    return;
                }
            }
            // line might pass through out-of-screen face
            else if (indx1[iz] > inMax[iz] && indx2[iy] < inMin[iy] ||
                indx2[iz] > inMax[iz] && indx1[iy] < inMin[iy])
            {
                r2 = intersectionHigh(point,xAxis,sign,inMax,iz,inExt);
                if (isBounded(point,xAxis,inMin,inMax,iz,r2))
                {
                    vtkUltraFindExtentHelper(r1,r2,sign[iy],inExt);
                    return;
                }
            } 
        }
    }

    // line might pass through top face
    if ((indx1[iy] <= inMax[iy] && indx2[iy] > inMax[iy]) ||
        (indx1[iy] > inMax[iy] && indx2[iy] <= inMax[iy]))
    {
        r2 = intersectionHigh(point,xAxis,sign,inMax,iy,inExt);
        if (isBounded(point,xAxis,inMin,inMax,iy,r2))
        {
            // line might pass through in-to-screen face
            if (indx1[iz] < inMin[iz] && indx2[iy] > inMax[iy] ||
                indx2[iz] < inMin[iz] && indx1[iy] > inMax[iy])
            {
                r1 = intersectionLow(point,xAxis,sign,inMin,iz,inExt);
                if (isBounded(point,xAxis,inMin,inMax,iz,r1))
                {
                    vtkUltraFindExtentHelper(r1,r2,sign[iy],inExt);
                    return;
                }
            }
            // line might pass through out-of-screen face
            else if (indx1[iz] > inMax[iz] && indx2[iy] > inMax[iy] || 
                indx2[iz] > inMax[iz] && indx1[iy] > inMax[iy])
            {
                r1 = intersectionHigh(point,xAxis,sign,inMax,iz,inExt);
                if (isBounded(point,xAxis,inMin,inMax,iz,r1))
                {
                    vtkUltraFindExtentHelper(r1,r2,sign[iy],inExt);
                    return;
                }
            }
        } 
    }

    // line might pass through in-to-screen face
    if ((indx1[iz] >= inMin[iz] && indx2[iz] < inMin[iz]) ||
        (indx1[iz] < inMin[iz] && indx2[iz] >= inMin[iz]))
    {
        r1 = intersectionLow(point,xAxis,sign,inMin,iz,inExt);
        if (isBounded(point,xAxis,inMin,inMax,iz,r1))
        {
            // line might pass through out-of-screen face
            if (indx1[iz] > inMax[iz] || indx2[iz] > inMax[iz])
            {
                r2 = intersectionHigh(point,xAxis,sign,inMax,iz,inExt);
                if (isBounded(point,xAxis,inMin,inMax,iz,r2))
                {
                    vtkUltraFindExtentHelper(r1,r2,sign[iz],inExt);
                    return;
                }
            }
        }
    }

    r1 = inExt[0];
    r2 = inExt[0] - 1;
}

//****************************************************************************
// I/0
//****************************************************************************

//----------------------------------------------------------------------------
// Combines a directory and a file to make a complete path
// directory = the directory, file = the filename, n = the number of characters
// in the array cp, result is stored in cp
static char *vtkJoinPath2(char *cp, int n, const char *directory, const char *file)
{
    int dlen = strlen(directory);
    int flen = strlen(file);

    if (n < (dlen + flen + 2))
    {
        return 0;
    }

    strncpy(cp,directory,n);
#ifdef _WIN32
    strncpy(cp+dlen,"\\",n-dlen);
#else
    strncpy(cp+dlen,"/",n-dlen);
#endif
    strncpy(cp+dlen+1,file,n-dlen);

    return cp;
}

//----------------------------------------------------------------------------
// Eats leading whitespace
static char *vtkFreehandUltrasound2EatWhitespace(char *text)
{
    int i = 0;

    for (i = 0; i < 128; i++)
    {
        switch (*text)
        {
        case ' ':
        case '\t':
        case '\r':
        case '\n':
            text++;
            break;
        default:
            return text;
            break;
        }
    }

    return 0;
}

//----------------------------------------------------------------------------
// Eats leading whitespace
static char *vtkFreehandUltrasound2EatWhitespaceWithEquals(char *text)
{
    int i = 0;

    for (i = 0; i < 128; i++)
    {
        switch (*text)
        {
        case ' ':
        case '\t':
        case '\r':
        case '\n':
        case '=':
            text++;
            break;
        default:
            return text;
            break;
        }
    }

    return 0;
}

//----------------------------------------------------------------------------
// T is either integer or double
template <class T>
int vtkExtractArrayComponentsFromString(char *text, T *arrayToFill, int numIndices)
{

    char delims[] = " ";
    char *result = NULL;
    T temp;

    text = vtkFreehandUltrasound2EatWhitespace(text);

    // loop through each component
    for (int i = 0; i < numIndices; i++)
    {

        // find the next token
        if (i == 0)
        {
            result = strtok(text, delims);
        }
        else
        {
            result = strtok(NULL, delims);
        }
        result = vtkFreehandUltrasound2EatWhitespace(result);

        if (result != NULL)
        {
            temp = (T) atof(result);
            arrayToFill[i] = temp;
        }
        else
        {
            return 0;
        }

    }

    return 1;

}

//****************************************************************************
// REAL-TIME RECONSTRUCTION - NOT OPTIMIZED
//****************************************************************************

//----------------------------------------------------------------------------
// Actually inserts the slice - executes the filter for any type of data, for
// no optimization
// Given an input and output region, execute the filter algorithm to fill the
// output from the input - no optimization.
// (this one function is pretty much the be-all and end-all of the
// filter)
template <class T>
static void vtkFreehandUltrasound2InsertSlice(vtkFreehandUltrasound2 *self,
                                              vtkImageData *outData,
                                              T *outPtr,
                                              unsigned short *accPtr,
                                              vtkImageData *inData,
                                              T *inPtr,
                                              int inExt[6],
                                              vtkMatrix4x4 *matrix)
{

    // local variables
    int numscalars;
    int idX, idY, idZ;
    int inIncX, inIncY, inIncZ;
    int outExt[6], outInc[3], clipExt[6];
    vtkFloatingPointType inSpacing[3], inOrigin[3];
    // the resulting point in the output volume (outPoint) from a point in the input slice
    // (inpoint)
    double outPoint[4], inPoint[4]; 

    // pointer to the nearest neighbor or trilinear interpolation function
    int (*interpolate)(double *point, T *inPtr, T *outPtr,
        unsigned short *accPtr, int numscalars, int outExt[6], int outInc[3]);

    // slice spacing and origin
    inData->GetSpacing(inSpacing);
    inData->GetOrigin(inOrigin);
    // number of pixels in the x and y directions b/w the fan origin and the slice origin
    double xf = (self->GetFanOrigin()[0]-inOrigin[0])/inSpacing[0];
    double yf = (self->GetFanOrigin()[1]-inOrigin[1])/inSpacing[1]; 
    // fan depth squared 
    double d2 = self->GetFanDepth()*self->GetFanDepth();
    // absolute value of slice spacing
    double xs = fabs((double)(inSpacing[0]));
    double ys = fabs((double)(inSpacing[1]));
    // tan of the left and right fan angles
    double ml = tan(vtkMath::RadiansFromDegrees(self->GetFanAngles()[0]))/xs*ys;
    double mr = tan(vtkMath::RadiansFromDegrees(self->GetFanAngles()[1]))/xs*ys;
    // the tan of the right fan angle is always greater than the left one
    if (ml > mr)
    {
        double tmp = ml; ml = mr; mr = tmp;
    }
    // get the clip rectangle as an extent
    self->GetClipExtent(clipExt, inOrigin, inSpacing, inExt);

    // find maximum output range = output extent
    outData->GetExtent(outExt);

    // Get increments to march through data - ex move from the end of one x scanline of data to the
    // start of the next line
    outData->GetIncrements(outInc);
    inData->GetContinuousIncrements(inExt, inIncX, inIncY, inIncZ);
    numscalars = inData->GetNumberOfScalarComponents();

    // Set interpolation method - nearest neighbor or trilinear
    vtkGetUltraInterpFunc(self,&interpolate);

    // Loop through  slice pixels in the input extent and put them into the output volume
    for (idZ = inExt[4]; idZ <= inExt[5]; idZ++)
    {
        for (idY = inExt[2]; idY <= inExt[3]; idY++)
        {
            for (idX = inExt[0]; idX <= inExt[1]; idX++)
            {

                // if we are within the current clip extent
                if (idX >= clipExt[0] && idX <= clipExt[1] && 
                    idY >= clipExt[2] && idY <= clipExt[3])
                {
                    // current x/y index minus num pixels in the x/y direction b/w the fan origin and the slice origin
                    double x = (idX-xf);
                    double y = (idY-yf);

                    // if we are within the fan
                    if (((ml == 0 && mr == 0) || y > 0 &&
                        ((x*x)*(xs*xs)+(y*y)*(ys*ys) < d2 && x/y >= ml && x/y <= mr)))
                    {  
                        inPoint[0] = idX;
                        inPoint[1] = idY;
                        inPoint[2] = idZ;
                        inPoint[3] = 1;

                        //recall matrix = the index matrix --> transform voxels in the slice to indices in the output
                        //formula: outPoint = matrix * inPoint
                        matrix->MultiplyPoint(inPoint,outPoint);

                        // deal with w (homogeneous transform) if the transform was a perspective transform
                        outPoint[0] /= outPoint[3]; 
                        outPoint[1] /= outPoint[3]; 
                        outPoint[2] /= outPoint[3];
                        outPoint[3] = 1;

                        // interpolation functions return 1 if the interpolation was successful, 0 otherwise
                        int hit = interpolate(outPoint, inPtr, outPtr, accPtr, numscalars, 
                            outExt, outInc);

                        // increment the number of pixels inserted
                        self->IncrementPixelCount(0, hit);
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
static void vtkOptimizedInsertSlice(vtkFreehandUltrasound2 *self, // the freehand us
                                    vtkImageData *outData, // the output volume
                                    T *outPtr, // scalar pointer to the output volume over the output extent
                                    unsigned short *accPtr, // scalar pointer to the accumulation buffer over the output extent
                                    vtkImageData *inData, // input slice
                                    T *inPtr, // scalar pointer to the input volume over the input slice extent
                                    int inExt[6], // input slice extent (could have been split for threading)
                                    F matrix[4][4], // index matrix, output indices -> input indices
                                    int threadId) // current thread id
{
    int prevPixelCount = self->GetPixelCount();

    // local variables
    int id = 0;
    int i, numscalars; // numscalars = number of scalar components in the input image
    int idX, idY, idZ; // the x, y, and z pixel of the input image
    int inIncX, inIncY, inIncZ; // increments for the input extent
    int outExt[6]; // output extent
    int outMax[3], outMin[3]; // the max and min values of the output extents -
    // if outextent = (x0, x1, y0, y1, z0, z1), then
    // outMax = (x1, y1, z1) and outMin = (x0, y0, z0)
    int outInc[3]; // increments for the output extent
    int clipExt[6];
    unsigned long count = 0;
    unsigned long target;
    int r1,r2;
    // outPoint0, outPoint1, outPoint is a fancy way of incremetally multiplying the input point by
    // the index matrix to get the output point...  Outpoint is the result
    F outPoint0[3]; // temp, see above
    F outPoint1[3]; // temp, see above
    F outPoint[3]; // this is the final output point, created using Output0 and Output1
    F xAxis[3], yAxis[3], zAxis[3], origin[3]; // the index matrix (transform), broken up into axes and an origin
    vtkFloatingPointType inSpacing[3],inOrigin[3]; // input spacing and origin

    // input spacing and origin
    inData->GetSpacing(inSpacing);
    inData->GetOrigin(inOrigin);

    // number of pixels in the x and y directions b/w the fan origin and the slice origin
    double xf = (self->GetFanOrigin()[0]-inOrigin[0])/inSpacing[0];
    double yf = (self->GetFanOrigin()[1]-inOrigin[1])/inSpacing[1];

    // fan depth squared
    double d2 = self->GetFanDepth()*self->GetFanDepth();
    // input spacing in the x and y directions
    double xs = inSpacing[0];
    double ys = inSpacing[1];
    // tan of the left and right fan angles
    double ml = tan(vtkMath::RadiansFromDegrees(self->GetFanAngles()[0]))/xs*ys;
    double mr = tan(vtkMath::RadiansFromDegrees(self->GetFanAngles()[1]))/xs*ys;
    // the tan of the right fan angle is always greater than the left one
    if (ml > mr)
    {
        double tmp = ml; ml = mr; mr = tmp;
    }

    // get the clip rectangle as an extent
    self->GetClipExtent(clipExt, inOrigin, inSpacing, inExt);

    // find maximum output range
    outData->GetExtent(outExt);

    for (i = 0; i < 3; i++)
    {
        outMin[i] = outExt[2*i];
        outMax[i] = outExt[2*i+1];
    }

    target = (unsigned long)
        ((inExt[5]-inExt[4]+1)*(inExt[3]-inExt[2]+1)/50.0);
    target++;

    int wExtent[6]; // output whole extent
    outData->GetWholeExtent(wExtent);
    outData->GetIncrements(outInc);
    inData->GetContinuousIncrements(inExt, inIncX, inIncY, inIncZ);
    numscalars = inData->GetNumberOfScalarComponents();

    // break matrix into a set of axes plus an origin
    // (this allows us to calculate the transform Incrementally)
    for (i = 0; i < 3; i++)
    {
        xAxis[i]  = matrix[i][0]; // remember that the matrix is the indexMatrix, and transforms
        yAxis[i]  = matrix[i][1];  // output pixels to input pixels
        zAxis[i]  = matrix[i][2];
        origin[i] = matrix[i][3];
    }

    static int firstFrame = 1;
    int flipped = self->GetImageFlipped();

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

    // Loop through INPUT pixels - remember this is a 3D cube represented by the input extent
    for (idZ = inExt[4]; idZ <= inExt[5]; idZ++) // for each image...
    {
        outPoint0[0] = origin[0]+idZ*zAxis[0]; // incremental transform
        outPoint0[1] = origin[1]+idZ*zAxis[1];
        outPoint0[2] = origin[2]+idZ*zAxis[2];

        for (idY = inExt[2]; idY <= inExt[3]; idY++) // for each horizontal line in the image...
        {
            outPoint1[0] = outPoint0[0]+idY*yAxis[0]; // incremental transform
            outPoint1[1] = outPoint0[1]+idY*yAxis[1];
            outPoint1[2] = outPoint0[2]+idY*yAxis[2];

            if (!id)
            {
                if (!(count%target)) 
                {
                    self->UpdateProgress(count/(50.0*target));  // progress between 0 and 1
                }
                count++;
            }

            // find intersections of x raster line with the output extent

            // this only changes r1 and r2
            vtkUltraFindExtent(r1,r2,outPoint1,xAxis,outMin,outMax,inExt);

            // next, handle the 'fan' shape of the input
            //double y = (yf - idY);;
            //if (ys < 0)
            //{
            //  y = -y;
            // }

            double y;
            if (flipped)
            {
                y = yf - idY;
            }
            else
            {
                y = idY - yf;
            }

            // first, check the angle range of the fan - choose r1 and r2 based
            // on the triangle that the fan makes from the fan origin to the bottom
            // line of the video image
            if (!(ml == 0 && mr == 0))
            {
                // equivalent to: r1 < vtkUltraCeil(ml*y + xf + 1)
                // this is what the radius would be based on tan(fanAngle)
                if (r1 < -vtkUltraFloor(-(ml*y + xf + 1)))
                {
                    r1 = -vtkUltraFloor(-(ml*y + xf + 1));
                }
                if (r2 > vtkUltraFloor(mr*y + xf - 1))
                {
                    r2 = vtkUltraFloor(mr*y + xf - 1);
                }

                // next, check the radius of the fan - crop the triangle to the fan
                // depth
                double dx = (d2 - (y*y)*(ys*ys))/(xs*xs);

                // if we are outside the fan's radius, ex at the bottom lines
                if (dx < 0)
                {
                    r1 = inExt[0];
                    r2 = inExt[0]-1;
                }
                // if we are within the fan's radius, we have to adjust if we are in
                // the "ellipsoidal" (bottom) part of the fan instead of the top
                // "triangular" part
                else
                {
                    dx = sqrt(dx);
                    // this is what r1 would be if we calculated it based on the
                    // pythagorean theorem
                    if (r1 < -vtkUltraFloor(-(xf - dx + 1)))
                    {
                        r1 = -vtkUltraFloor(-(xf - dx + 1));
                    }
                    if (r2 > vtkUltraFloor(xf + dx - 1))
                    {
                        r2 = vtkUltraFloor(xf + dx - 1);
                    }
                }
            }

            // bound to the ultrasound clip rectangle
            if (r1 < clipExt[0])
            {
                r1 = clipExt[0];
            }
            if (r2 > clipExt[1])
            {
                r2 = clipExt[1];
            }

            if (r1 > r2) // TODO synchrograb adds || idY < clipExt[2] || idY > clipExt[3])
            {
                r1 = inExt[0];
                r2 = inExt[0]-1;
            }

            // skip the portion of the slice to the left of the fan
            for (idX = inExt[0]; idX < r1; idX++)
            {
                inPtr += numscalars;
            }
            // multiplying the input point by the transform will give you fractional pixels,
            // so we need interpolation

            // interpolating linearly (code 1)
            if (self->GetInterpolationMode() == VTK_FREEHAND_LINEAR)
            { 

                for (idX = r1; idX <= r2; idX++) // for all of the x pixels within the fan
                {
                    outPoint[0] = outPoint1[0] + idX*xAxis[0];
                    outPoint[1] = outPoint1[1] + idX*xAxis[1];
                    outPoint[2] = outPoint1[2] + idX*xAxis[2];

                    int hit = vtkTrilinearInterpolation(outPoint, inPtr, outPtr, accPtr, 
                        numscalars, outExt, outInc); // hit is either 1 or 0

                    inPtr += numscalars; // go to the next x pixel
                    self->IncrementPixelCount(threadId, hit);
                }
            }

            // interpolating with nearest neighbor (code 0)
            else 
            {
                vtkFreehand2OptimizedNNHelper(r1, r2, outPoint, outPoint1, xAxis, 
                    inPtr, outPtr, outExt, outInc,
                    numscalars, accPtr);
                // we added all the pixels between r1 and r2, so increment our count of the number of pixels added
                self->IncrementPixelCount(threadId, r2-r1+1); 
            }

            // skip the portion of the slice to the right of the fan
            for (idX = r2+1; idX <= inExt[1]; idX++)
            {
                inPtr += numscalars;
            }

            inPtr += inIncY; // move to the next line
        }
        inPtr += inIncZ; // move to the next image
    }
}

//****************************************************************************
// THE MAIN LOOP FOR THE RECONSTRUCTION
//****************************************************************************

//----------------------------------------------------------------------------
// This function is run in a background thread to perform the reconstruction.
// By running it in the background, it doesn't interfere with the display
// of the partially reconstructed volume.
static void *vtkReconstructionThread(ThreadInfoStruct *data)
{

    vtkFreehandUltrasound2 *self = (vtkFreehandUltrasound2 *)(data->UserData);

    double prevtimes[10];
    double currtime = 0;  // most recent timestamp for reconstruction time
    double lastcurrtime = 0;  // previous timestamp for reconstructin time
    double timestamp = 0;  // video timestamp, corrected for lag
    double videolag = self->GetVideoLag();
    int i;

    for (i = 0; i < 10; i++)
    {
        prevtimes[i] = 0.0;
    }

    // the tracker tool provides the position of each inserted slice
    // check that tracker tool exists
    if (!self->GetTrackerTool())
    {
        printf("Reconstruction thread couldn't find tracker tool\n");
        return NULL;
    }
    else
    {
        //printf("Found Tracker Tool\n");
    }

    // tracker tool transforms
    vtkMatrix4x4 *matrix = self->GetSliceAxes();

    // get the buffer for the tracking information
    // is the buffer for the tracker tool if we are doing a real-time reconstruction
    // is the buffer given in self->TrackerBuffer if we are doing non-real-time reconstruction
    vtkTrackerBuffer *buffer = self->GetTrackerTool()->GetBuffer();
    if (!self->RealTimeReconstruction)
    { 
        buffer = self->TrackerBuffer;
    }

    // get the video for the video information, and the current 2D slice
    vtkVideoSource2 *video = self->GetVideoSource();
    vtkImageData *inData = self->GetSlice();

    // wait for video to start before we start the reconstruction
    // (i.e. wait for timestamp to change)
    if (video && self->RealTimeReconstruction)
    {
        while (lastcurrtime == 0 || currtime == lastcurrtime)
        {
            int clipExt[6];
            self->GetClipExtent(clipExt, inData->GetOrigin(), inData->GetSpacing(),
                inData->GetWholeExtent()); 
            // TODO clip extent implementation - 3DPanoramicVolumeReconstructor has SetUpdateExtent(clipExt) instead of
            // SetUpdateExtentToWholeExtent()
            inData->SetUpdateExtentToWholeExtent();
            inData->Update();
            self->UpdateAccumulationBuffers();

            lastcurrtime = currtime;
            currtime = video->GetFrameTimeStamp();
            double timenow = vtkAccurateTimer::GetSystemTime();
            double sleepuntil = currtime + 0.010;
            if (sleepuntil > timenow)
            {
                vtkThreadSleep(data, sleepuntil);
            }
        }
    }

    // The reconstruction loop!
    // Loop continuously until reconstruction thread is halted
    double starttime = 0;
    vtkSmartPointer<vtkTransform> tempTransform = vtkSmartPointer<vtkTransform>::New();
    vtkSmartPointer<vtkMatrix4x4> sliceAxesInverseMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
    int rot; // current rotation in degrees
    int rotating = self->GetRotating();
    vtkImageClip* clipper = self->GetRotationClipper();
    vtkImageThreshold* thresholder = self->GetRotationThresholder();
    double sleeptime = self->GetSleepTime();
    int maxRotation = self->GetMaximumRotationChange();
    int flags;
    int problemWithThisSlice;
    int insertSliceNow = 0;
    double frametime = currtime;
    double lastframetime;

    for (i = 0;;)
    {
        lastcurrtime = currtime;
        lastframetime = frametime;

        problemWithThisSlice = 0; // so far, there's no problem yet!

        // update the slice data - if reconstructing in real time, this is the only place
        // where this->Slice is actually updated - this is because even though we grab
        // from the video source in multiple places, we only do the Update() here!  Video
        // source only copies from buffer to output on the updates()
        // TODO clip extent implementation - 3DPanoramicVolumeReconstructor has SetUpdateExtent(clipExt) instead of
        // SetUpdateExtentToWholeExtent()
        int clipExt[6];
        self->GetClipExtent(clipExt, inData->GetOrigin(), inData->GetSpacing(),  inData->GetWholeExtent()); // TODO clip extent implementation - clip extent
        inData->SetUpdateExtentToWholeExtent();
        insertSliceNow = self->UpdateSlice(inData);
        //inData->Update();

        // get the updated timestamp
        // if not triggering, this is the timestamp for the video data, so that the tracking
        // transform corresponds with the video frames
        // if triggering, we want the tracking transform to correspond to the signal box time instead
        if (video)
        {
            currtime = self->CalculateCurrentVideoTime(inData);
            timestamp = currtime - videolag;
        }

        if (starttime == 0)
        {
            starttime = timestamp;
        }

        // if we have a saved slice to insert before the current one, then insert it now
        self->ReconstructOldSlice((lastcurrtime - videolag), inData);

        // Get the tracking transform, using temporal calibration if applicable
        // recall that matrix = this->SliceAxes
        buffer->Lock();
        flags = 0;
        if (video && (videolag > 0.0 || !self->RealTimeReconstruction)) // TODO add "or retrospective" for when doing offline recon with retro gating?
        {
            flags = buffer->GetFlagsAndCalibratedMatrixFromTime(matrix, timestamp);
        }
        else
        {
            buffer->GetCalibratedMatrix(matrix, 0);
            flags = buffer->GetFlags(0);
            if (!video)
            {
                currtime = buffer->GetTimeStamp(0);
            }
        }
        buffer->Unlock();

        // get the rotation and apply 
        if (rotating)
        {
            clipper->SetInput(inData);
            thresholder->SetInput(clipper->GetOutput());
            thresholder->Update();
            rot = self->CalculateFanRotationValue();

            // ignore rotations of -1
            if (rot == -1)
            {
                self->SetPreviousFanRotation(self->GetFanRotation());
                problemWithThisSlice = 1;
            }
            // ignore first rotation greater than maxRotation, but keep subsequent ones
            else if (abs(self->GetPreviousFanRotation() - rot) > maxRotation)
            {
                self->SetPreviousFanRotation(rot); // this is the addition
                self->SetFanRotation(rot); // this is the addition
                problemWithThisSlice = 1;
            }
            // good slices
            else
            {
                self->SetPreviousFanRotation(self->GetFanRotation());
                self->SetFanRotation(rot);
                problemWithThisSlice = 0;
            }

            /*// ignore rotations of -1 (error flag) - assume that there is no change in
            // rotation, but don't insert the slice anyways
            if (rot >= 0)
            {
            self->SetPreviousFanRotation(self->GetFanRotation());
            self->SetFanRotation(rot);
            // don't insert slices for rotation changes greater than maxRotation
            if (abs(self->GetPreviousFanRotation() - rot) > maxRotation)
            {
            problemWithThisSlice = 1;
            }
            }
            else
            {
            problemWithThisSlice = 1;
            }*/

        }

        // now use the rotation to change the SliceTransform (vtkTransform)
        // (if not rotating, then fan rotation and previous fan rotation are zero and don't do anything)
        // TODO this probably won't work if there is an actual set slice transform here...
        vtkMatrix4x4::Invert(matrix, sliceAxesInverseMatrix);
        if (self->GetSliceTransform())
        {
            //if (self->GetFanRotation() != self->GetPreviousFanRotation()) // messes up big jumps
            //{
            // formula: sliceAxes * sliceTransform(rotation) * inv(sliceAxes)
            tempTransform = (vtkTransform *) (self->GetSliceTransform());
            tempTransform->Identity();
            tempTransform->RotateY(self->GetFanRotation());
            tempTransform->PostMultiply();
            tempTransform->Concatenate(matrix);
            tempTransform->PreMultiply();
            tempTransform->Concatenate(sliceAxesInverseMatrix);
            //}
        }

        // sleep if we don't have an updated video slice
        if ( self->RealTimeReconstruction )
        {
            frametime = video->GetFrameTimeStamp();
        }
        else
        {
            frametime = timestamp; 
        }

        //if (currtime == lastcurrtime && self->RealTimeReconstruction && !insertSliceNow)
        if (frametime == lastframetime && self->RealTimeReconstruction && !insertSliceNow)
        {
            double timenow = vtkAccurateTimer::GetSystemTime();
            //double sleepuntil = currtime + sleeptime;
            double sleepuntil = frametime + sleeptime;
            if (sleepuntil > timenow)
            {
                // return if abort occurred during sleep
                if (vtkThreadSleep(data, sleepuntil) == 0)
                {
                    return NULL;
                }
            }
        }

        // sleep if tool is not tracking properly
        else if (flags & (TR_MISSING | TR_OUT_OF_VIEW | TR_REQ_TIMEOUT))
        {
            insertSliceNow = 0;
            printf("\ntracker out of view\n");
            double timenow = vtkAccurateTimer::GetSystemTime();
            //double sleepuntil = currtime + sleeptime;
            double sleepuntil = frametime + sleeptime;
            if (sleepuntil > timenow)
            {
                // return if abort occurred during sleep
                if (vtkThreadSleep(data, sleepuntil) == 0)
                {
                    return NULL;
                }
            }
        }

        // do the reconstruction if we have an updated frame and tool is tracking
        else
        {
            insertSliceNow = 0;
            // insert the slice!
            if (!problemWithThisSlice)
            {
                self->ReconstructSlice(timestamp, inData);
            }

            // calculate frame rate using computer clock, not timestamps
            // --> current reconstruction rate over last 10 updates
            //double tmptime = currtime;
            double tmptime = frametime;
            if (!self->RealTimeReconstruction)
            { 
                tmptime = vtkAccurateTimer::GetSystemTime();
            }
            double difftime = tmptime - prevtimes[i%10];
            prevtimes[i%10] = tmptime;
            if (i > 10 && difftime != 0)
            {
                self->ReconstructionRate = (10.0/difftime);
            }
            i++;
        }

        // check to see if we are being told to quit 
        int activeFlag = *(data->ActiveFlag);
        if (activeFlag == 0)
        {
            return NULL;
        }

        // if we are not doing a real time reconstruction, sleep for a millisecond
        // to give the main application thread some time, and seek to the next video frame
        if (!self->RealTimeReconstruction)
        {
            vtkSleep(0.001);

            if (video)
            {
                if (--self->ReconstructionFrameCount == 0)
                {
                    return NULL;
                }
            }
        }
    }
}



#endif
