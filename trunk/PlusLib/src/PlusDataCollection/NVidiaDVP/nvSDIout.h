#ifndef NVSDIOUT_H
#define NVSDIOUT_H

#if WIN32
#include <windows.h>
#endif
#include <assert.h>
#include <stdio.h>

#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glu.h>
#include <GL/wglext.h>

#include "nvapi.h"
#include "nvGPUutil.h"

struct nvOptions;

//----------------------------------------------------------------------------
typedef class CNvSDIoutGpu : public virtual CNvGpu
{
public:
  CNvSDIoutGpu();
  virtual ~CNvSDIoutGpu();

  bool init( HGPUNV gpuAffinityHandle, bool bPrimary, bool bDisplay, bool bSDIOutput, NvVioHandle hVioHandle );
  bool isSDIoutput();
  NvVioHandle getVioHandle();

protected:
  bool m_bSDIoutput;
  NvVioHandle m_hVioHandle;
} CNvSDIoutGpu;

//----------------------------------------------------------------------------
typedef class CNvSDIoutGpuTopology : public virtual CNvGpuTopology
{
public:
  static CNvSDIoutGpuTopology& instance();
  virtual CNvSDIoutGpu* getGpu( int index );
  virtual CNvSDIoutGpu* getPrimaryGpu();

protected:
  CNvSDIoutGpuTopology();
  virtual ~CNvSDIoutGpuTopology();

  bool init();
} CNvSDIoutGpuTopology;

//----------------------------------------------------------------------------
typedef class CNvSDIout
{
public:
  CNvSDIout();
  ~CNvSDIout();

  HRESULT Init( nvOptions* options, CNvSDIoutGpu* SdiOutGpu = NULL );
  HRESULT Cleanup();

  inline NvVioHandle getHandle() {return m_vioHandle; };

  inline unsigned int GetWidth() {return m_videoWidth; };
  inline unsigned int GetHeight() {return m_videoHeight; };

  inline float GetFrameRate() {return m_frameRate; };

  inline BOOL IsInterlaced() {return m_bInterlaced; };

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