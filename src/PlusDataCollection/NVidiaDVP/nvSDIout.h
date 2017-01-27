/***************************************************************************\
|*                                                                           *|
|*      Copyright 2007 NVIDIA Corporation.  All rights reserved.             *|
|*                                                                           *|
|*   NOTICE TO USER:                                                         *|
|*                                                                           *|
|*   This source code is subject to NVIDIA ownership rights under U.S.       *|
|*   and international Copyright laws.  Users and possessors of this         *|
|*   source code are hereby granted a nonexclusive, royalty-free             *|
|*   license to use this code in individual and commercial software.         *|
|*                                                                           *|
|*   NVIDIA MAKES NO REPRESENTATION ABOUT THE SUITABILITY OF THIS SOURCE     *|
|*   CODE FOR ANY PURPOSE. IT IS PROVIDED "AS IS" WITHOUT EXPRESS OR         *|
|*   IMPLIED WARRANTY OF ANY KIND. NVIDIA DISCLAIMS ALL WARRANTIES WITH      *|
|*   REGARD TO THIS SOURCE CODE, INCLUDING ALL IMPLIED WARRANTIES OF         *|
|*   MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR          *|
|*   PURPOSE. IN NO EVENT SHALL NVIDIA BE LIABLE FOR ANY SPECIAL,            *|
|*   INDIRECT, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES          *|
|*   WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN      *|
|*   AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING     *|
|*   OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOURCE      *|
|*   CODE.                                                                   *|
|*                                                                           *|
|*   U.S. Government End Users. This source code is a "commercial item"      *|
|*   as that term is defined at 48 C.F.R. 2.101 (OCT 1995), consisting       *|
|*   of "commercial computer  software" and "commercial computer software    *|
|*   documentation" as such terms are used in 48 C.F.R. 12.212 (SEPT 1995)   *|
|*   and is provided to the U.S. Government only as a commercial end item.   *|
|*   Consistent with 48 C.F.R.12.212 and 48 C.F.R. 227.7202-1 through        *|
|*   227.7202-4 (JUNE 1995), all U.S. Government End Users acquire the       *|
|*   source code with only those rights set forth herein.                    *|
|*                                                                           *|
|*   Any use of this source code in individual and commercial software must  *|
|*   include, in the user documentation and internal comments to the code,   *|
|*   the above Disclaimer and U.S. Government End Users Notice.              *|
|*                                                                           *|
|*                                                                           *|
\***************************************************************************/

#ifndef NVSDIOUT_H
#define NVSDIOUT_H

#if WIN32
#include <windows.h>
#endif
#include <assert.h>
#include <stdio.h>

#include "nvConfigure.h"
#include <GL/glu.h>

#include "nvapi.h"
#include "nvGPUutil.h"

struct nvOptions;

//----------------------------------------------------------------------------
typedef class CNvSDIoutGpu : public virtual CNvGpu
{
public:
  CNvSDIoutGpu();
  virtual ~CNvSDIoutGpu();

  bool Init( HGPUNV gpuAffinityHandle, bool bPrimary, bool bDisplay, bool bSDIOutput, NvVioHandle hVioHandle );
  bool IsSDIoutput();
  NvVioHandle GetVioHandle();

protected:
  bool m_bSDIoutput;
  NvVioHandle m_hVioHandle;
} CNvSDIoutGpu;

//----------------------------------------------------------------------------
typedef class CNvSDIoutGpuTopology : public virtual CNvGpuTopology
{
public:
  static CNvSDIoutGpuTopology& Instance();
  virtual CNvSDIoutGpu* GetGpu( int index );
  virtual CNvSDIoutGpu* GetPrimaryGpu();

protected:
  CNvSDIoutGpuTopology();
  virtual ~CNvSDIoutGpuTopology();

  bool Init();
} CNvSDIoutGpuTopology;

//----------------------------------------------------------------------------
typedef class CNvSDIout
{
public:
  CNvSDIout();
  ~CNvSDIout();

  HRESULT Init( nvOptions* options, CNvSDIoutGpu* SdiOutGpu = NULL );
  HRESULT Cleanup();

  NvVioHandle GetHandle();

  unsigned int GetWidth();
  unsigned int GetHeight();

  float GetFrameRate();

  BOOL IsInterlaced();

  HRESULT DisplayVideoStatus();
  HRESULT DisplaySignalFormatInfo();
  HRESULT DisplayDataFormatInfo();
  HRESULT GetFrameRate( float* rate );

  HRESULT SetCSC( NVVIOCOLORCONVERSION* csc, bool enable );
  HRESULT GetCSC( NVVIOCOLORCONVERSION* csc, bool* enable );

  HRESULT GetSyncDelay( NVVIOSYNCDELAY* delay );
  HRESULT SetSyncDelay( NVVIOSYNCDELAY* delay );

private:
  NvVioHandle m_vioHandle;        // Video device handle

  float m_frameRate;              // Video frame rate

  unsigned int m_videoWidth;      // Video format resolution in pixels
  unsigned int m_videoHeight;     // Video format resolution in lines

  BOOL m_bInterlaced;             // Interlaced flag.
} CNvSDIout;

#endif NVSDIOUT_H