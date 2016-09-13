
#ifndef NVGPUUTIL_H
#define NVGPUUTIL_H

#if WIN32
#include <windows.h>
#endif
#include <assert.h>
#include <stdio.h>

#include <string>
#include <iostream>

#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glu.h>
#include <GL/wglext.h>

#define MAX_GPUS 4

//----------------------------------------------------------------------------
typedef class CNvGpu
{
public:
  CNvGpu();
  virtual ~CNvGpu();

  bool init( HGPUNV gpuAffinityHandle, bool bPrimary, bool bDisplay );
  bool isPrimary();
  bool isDisplay();
  HGPUNV getAffinityHandle();

protected:
  HGPUNV m_hGpu;
  bool m_bPrimary; //contains the desktop
  bool m_bDisplay;
} CNvGPU;

//----------------------------------------------------------------------------
typedef class CNvGpuTopology
{
public:
  static CNvGpuTopology& instance();
  virtual CNvGpu* getGpu( int index );
  virtual CNvGpu* getPrimaryGpu();
  int getNumGpu();
  int getPrimaryGpuIndex();

protected:
  CNvGpuTopology();
  virtual ~CNvGpuTopology();
  bool init();

protected:
  CNvGpu* m_lGpu[MAX_GPUS];
  int m_nGpu;
  bool m_bInitialized;
} CNvGpuTopology;

#if WIN32
bool CreateDummyGLWindowWin32( HWND* hWnd, HGLRC* hGLRC );
bool DestroyGLWindowWin32( HWND* hWnd, HGLRC* hGLRC );
#endif

#endif