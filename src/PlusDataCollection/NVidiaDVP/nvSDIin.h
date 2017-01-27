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

#ifndef NVSDIIN_H
#define NVSDIIN_H

#if WIN32
#include <windows.h>
#endif
#include <assert.h>
#include <stdio.h>

#include "nvConfigure.h"
#include <GL/glu.h>

#include "nvapi.h"

// Definitions
#define MAX_VIDEO_STREAMS 4
#define MEASURE_PERFORMANCE

struct nvOptions;

//----------------------------------------------------------------------------
typedef class CNvSDIinTopology
{
public:
  static CNvSDIinTopology& Instance();
  NVVIOTOPOLOGYTARGET* GetDevice( int index );
  int GetNumDevice();

protected:
  CNvSDIinTopology();
  virtual ~CNvSDIinTopology();

  bool Init();

protected:
  NVVIOTOPOLOGYTARGET* m_lDevice[NVAPI_MAX_VIO_DEVICES];
  bool m_bInitialized;
  int m_nDevice;
} CNvSDIinTopology;

//----------------------------------------------------------------------------
// CNvSDIin - class that allows to capture one or multiple streams into a texture/video buffer object
typedef class CNvSDIin
{
public:
  float m_gviTime;
  float m_gpuTime;

  CNvSDIin();
  ~CNvSDIin();

  HRESULT Init( nvOptions* options = NULL );

  HRESULT SetupDevice( int deviceNumber = 0 ); //sets up a particular capture device
  //it is up to the developer to make sure that the same video slot is not used twice for two different capture devices
  //on one context. Later this accounting can be made transparent to the user and done in the class
  HRESULT BindDevice( GLuint videoSlot, HDC hDC );

  HRESULT BindVideoTexture( GLuint videoTexture, unsigned int stream );
  HRESULT UnbindVideoTexture( unsigned int stream );

  HRESULT BindVideoFrameBuffer( GLuint videoBuffer, GLint videoBufferFormat, unsigned int stream );
  HRESULT UnbindVideoFrameBuffer( unsigned int stream );

  HRESULT StartCapture();
  GLenum Capture( GLuint* sequenceNum, GLuint64EXT* captureTime );
  HRESULT EndCapture();

  HRESULT Cleanup();
  HRESULT UnbindDevice();

  void SetCSCParams( GLfloat* cscMat, GLfloat* cscOffset, GLfloat* cscMin, GLfloat* cscMax );

  NvVioHandle GetVioHandle();
  unsigned int GetWidth();
  unsigned int GetHeight();
  int GetBufferObjectPitch( unsigned int streamIndex );
  unsigned int GetNumStreams();
  NVVIOSIGNALFORMAT GetSignalFormat();
  HRESULT GetFrameRate( float* rate );
  int GetDeviceNumber();

protected:
  // Get video input state
  HRESULT GetVideoInState( NVVIOCONFIG_V1* vioConfig, NVVIOSTATUS* vioStatus );
  HRESULT SetVideoConfig();
  void DumpChannelStatus( NVVIOCHANNELSTATUS jack );
  void DumpStreamStatus( NVVIOSTREAM stream );

protected:
  //Capture Interpretation settings
  bool m_bDualLink;
  NVVIOCOMPONENTSAMPLING m_Sampling;
  int m_BitsPerComponent;
  int m_ExpansionEnable;

  //Capture detected settings
  unsigned int m_videoWidth;                          // Video format resolution in pixels
  unsigned int m_videoHeight;                         // Video format resolution in lines
  float m_fFrameRate;                                 // Video frame rate
  NVVIOSIGNALFORMAT m_videoFormat;
  unsigned int m_numStreams;
  unsigned int m_activeJacks[NVAPI_MAX_VIO_JACKS];
  NVVIOSIGNALFORMATDETAIL m_signalFormatDetail;       // Video Signal Details

  HDC m_hDC;                                          // GPU device context
  GLuint m_videoSlot;                                 // GPU Video slot

  HVIDEOINPUTDEVICENV m_device;                       // Video input device
  NvU32 m_vioID;                                      // Unique video device ID
  NvVioHandle m_vioHandle;                            // Video device handle
  int m_deviceNumber;

  //GPU object settings
  GLfloat m_cscMat[4][4];
  GLfloat m_cscMax[4];
  GLfloat m_cscMin[4];
  GLfloat m_cscOffset[4];

  bool m_bCaptureStarted;                             // Set to true when glBeginVideoCaptureNV had successfully completed

  GLuint m_captureTimeQuery;
} CNvSDIin;

#endif NVSDIIN_H