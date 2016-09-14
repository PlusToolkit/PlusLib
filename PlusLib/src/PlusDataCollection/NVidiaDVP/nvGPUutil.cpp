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

#include "PlusCommon.h"
#include "nvGPUutil.h"
#include "glExtensions.h"

#if WIN32
//----------------------------------------------------------------------------
bool CreateDummyGLWindowWin32( HWND* hWnd, HGLRC* hGLRC )
{
  HINSTANCE hInstance = GetModuleHandle( NULL ); // Need a handle to this process instance
  // Create the window class
  WNDCLASSEX  wc;           // Windows Class Structure
  wc.cbSize     = sizeof( WNDCLASSEX );
  wc.style      = CS_HREDRAW | CS_VREDRAW | CS_OWNDC; // Redraw On Size, And Own DC For Window.
  wc.lpfnWndProc    = DefWindowProc;          // WndProc Handles Messages, we'll pass a static version, which will receive a 'this' pointer
  wc.cbClsExtra   = 0;                  // No Extra Window Data
  wc.cbWndExtra   = 0;                  // No Extra Window Data
  wc.hInstance    = hInstance;              // Set The Instance
  wc.hIcon      = NULL;                 // Load The Default Icon
  wc.hIconSm        = NULL;
  wc.hCursor      = LoadCursor( hInstance, IDC_ARROW ); // Load The Arrow Pointer
  wc.hbrBackground  = NULL;                 // No Background Required For GL
  wc.lpszMenuName   = NULL;                 // We Don't Want A Menu
  wc.lpszClassName  = "Dummy";            // Set The Class Name

  // register the window class
  RegisterClassEx( &wc );

  // Call the windows function to create the window.  The
  *hWnd = CreateWindowEx( NULL, "Dummy", NULL, NULL, 0, 0, 1, 1,  NULL, NULL, NULL, NULL );

  // Get the windows device context
  HDC hDC = GetDC( *hWnd );

  ///// STEP 2: Now we need to create an OpenGL context for this window

  // We need a pixel format descriptor.  A PFD tells how OpenGL draws
  static  PIXELFORMATDESCRIPTOR pfd =
  {
    sizeof( PIXELFORMATDESCRIPTOR ),      // Size Of This Pixel Format Descriptor
    1,                      // Version Number
    PFD_DRAW_TO_WINDOW |            // Format Must Support Window
    PFD_SUPPORT_OPENGL |            // Format Must Support OpenGL
    PFD_DOUBLEBUFFER,             // Must Support Double Buffering
    PFD_TYPE_RGBA,                // Request An RGBA Format
    8,                        // Select Our Color Depth
    0, 0, 0, 0, 0, 0,             // Color Bits Ignored
    0,                      // No Alpha Buffer
    0,                      // Shift Bit Ignored
    0,                      // No Accumulation Buffer
    0, 0, 0, 0,                 // Accumulation Bits Ignored
    16,                     // 16Bit Z-Buffer (Depth Buffer)
    0,                      // No Stencil Buffer
    0,                      // No Auxiliary Buffer
    PFD_MAIN_PLANE,               // Main Drawing Layer
    0,                      // Reserved
    0, 0, 0                   // Layer Masks Ignored
  };

  int pixelformat;
  // create the pixel pixel format descriptor
  if ( ( pixelformat = ChoosePixelFormat( hDC, &pfd ) ) == 0 )
  {
    return false;
  }
  //  set the pixel format descriptor
  if ( SetPixelFormat( hDC, pixelformat, &pfd ) == FALSE )
  {
    return false;
  }
  // Create a wGL rendering context
  *hGLRC = wglCreateContext( hDC );
  //  Activate the rendering context
  wglMakeCurrent( hDC, *hGLRC );

  return true;
}

//----------------------------------------------------------------------------
bool DestroyGLWindowWin32( HWND* hWnd, HGLRC* hGLRC )
{
  HDC hDC = GetDC( *hWnd );
  wglMakeCurrent( NULL, NULL );
  wglDeleteContext( *hGLRC );
  if( ReleaseDC( *hWnd, hDC ) == 0 )
  {
    return false;
  }
  if( DestroyWindow( *hWnd ) == FALSE )
  {
    return false;
  }
  return true;
}

#endif

//----------------------------------------------------------------------------
CNvGpuTopology::CNvGpuTopology()
{
  m_bInitialized = false;
  m_nGpu = 0;
  if( Init() )
  {
    m_bInitialized = true;
  }
}

//----------------------------------------------------------------------------
CNvGpuTopology::~CNvGpuTopology()
{
  if( !m_bInitialized )
  {
    return;
  }
  for( int i = 0; i < m_nGpu; i++ )
  {
    delete m_lGpu[i];
    m_lGpu[i] = NULL;
  }
}

//----------------------------------------------------------------------------
bool CNvGpuTopology::Init()
{
  if( m_bInitialized )
  {
    return true;
  }

#if WIN32
  HWND hWnd;
  HGLRC hGLRC;

  if( CreateDummyGLWindowWin32( &hWnd, &hGLRC ) == false )
  {
    return false;
  }
#elif __linux__

#endif

  if( !loadAffinityExtension() )
  {
    LOG_ERROR( "Could not load OpenGL Affinity extension" );
    return false;
  }

  LOG_INFO( "Listing GPUs available for OpenGL GPU Affinity" );

  unsigned int GPUIdx = 0;

  GPU_DEVICE gpuDevice;
  gpuDevice.cb = sizeof( gpuDevice );
  HGPUNV hGPU;
  bool bDisplay;
  bool bPrimary;
  while( wglEnumGpusNV( GPUIdx, &hGPU ) ) // First call this function to get a handle to the gpu
  {
    // wglEnumPGUsNV will fails if DeviceIdx > the available devices
    LOG_DEBUG( "GPU# " << GPUIdx << ":" );
    bDisplay = false;
    bPrimary = false;

    // Now get the detailed information about this device:
    //    How many displays it's attached to and whether any of them
    int DisplayDeviceIdx = 0;
    while( wglEnumGpuDevicesNV( hGPU, DisplayDeviceIdx, &gpuDevice ) )
    {
      LOG_DEBUG( " Display# " << DisplayDeviceIdx );
      LOG_DEBUG( "   Name: " << gpuDevice.DeviceName );
      LOG_DEBUG( "   String: " << gpuDevice.DeviceString );

      bDisplay = true;

      if( gpuDevice.Flags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP )
      {
        LOG_DEBUG( "   Attached to the desktop: LEFT=" << gpuDevice.rcVirtualScreen.left << ", RIGHT=" << gpuDevice.rcVirtualScreen.right
                   << ", TOP=" << gpuDevice.rcVirtualScreen.top << ", BOTTOM=" << gpuDevice.rcVirtualScreen.bottom );
      }
      else
      {
        LOG_DEBUG( "   Not attached to the desktop." );
      }

      // See if it's the primary GPU
      if( gpuDevice.Flags & DISPLAY_DEVICE_PRIMARY_DEVICE )
      {
        LOG_DEBUG( "   This is the PRIMARY Display Device." );
        bPrimary = true;
      }
      DisplayDeviceIdx++;

    }
    CNvGpu* gpu = new CNvGpu();
    gpu->Init( hGPU, bPrimary, bDisplay );

    m_lGpu[GPUIdx] = gpu;

    GPUIdx++;
  }

  m_nGpu = GPUIdx;
  m_bInitialized = true;

#if WIN32
  // We can kill the dummy window now
  if( DestroyGLWindowWin32( &hWnd, &hGLRC ) == false )
  {
    return false;
  }
#elif __linux__

#endif

  return true;
}

//----------------------------------------------------------------------------
CNvGpuTopology& CNvGpuTopology::Instance()
{
  static CNvGpuTopology instance;
  return instance;
}

//----------------------------------------------------------------------------
int CNvGpuTopology::GetNumGpu()
{
  return m_nGpu;
}

//----------------------------------------------------------------------------
CNvGpu* CNvGpuTopology::GetGpu( int index )
{
  if( index >= 0 && index < m_nGpu )
  {
    return m_lGpu[index];
  }
  return NULL;
}

//----------------------------------------------------------------------------
int CNvGpuTopology::GetPrimaryGpuIndex()
{
  for( int i = 0; i < m_nGpu; i++ )
  {
    if( m_lGpu[i]->isPrimary() )
    {
      return i;
    }
  }
  return 0;
}

//----------------------------------------------------------------------------
CNvGpu* CNvGpuTopology::GetPrimaryGpu()
{
  for( int i = 0; i < m_nGpu; i++ )
  {
    if( m_lGpu[i]->isPrimary() )
    {
      return m_lGpu[i];
    }
  }
  return NULL;
}

//----------------------------------------------------------------------------
CNvGpu::CNvGpu()
{

}

//----------------------------------------------------------------------------
CNvGpu::~CNvGpu()
{

}

//----------------------------------------------------------------------------
bool CNvGpu::Init( HGPUNV gpuAffinityHandle, bool bPrimary, bool bDisplay )
{
  m_hGpu = gpuAffinityHandle;
  m_bPrimary = bPrimary;
  m_bDisplay = bDisplay;
  return true;
}

//----------------------------------------------------------------------------
bool CNvGpu::isPrimary()
{
  return m_bPrimary;
}

//----------------------------------------------------------------------------
bool CNvGpu::isDisplay()
{
  return m_bDisplay;
}

//----------------------------------------------------------------------------
HGPUNV CNvGpu::getAffinityHandle()
{
  return m_hGpu;
}