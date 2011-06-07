/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkWin32VideoSource2.cxx,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

  Author: Danielle Pace
          Robarts Research Institute and The University of Western Ontario

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkWin32VideoSource2.h"
#include "PlusConfigure.h"
#include "vtkCriticalSection.h"
#include "vtkObjectFactory.h"
#include "vtkUnsignedCharArray.h"
#include "vtkVideoBuffer2.h"
#include "vtkVideoFrame2.h"


#include <ctype.h>

// because of warnings in windows header push and pop the warning level
#ifdef _MSC_VER
#pragma warning (push, 3)
#endif

#include "vtkWindows.h"
#include <winuser.h>
#include <vfw.h>

#ifdef _MSC_VER
#pragma warning (pop)
#endif

class vtkWin32VideoSource2Internal
{
public:
  vtkWin32VideoSource2Internal() {}
  HWND CapWnd;
  HWND ParentWnd;
  CAPSTATUS CapStatus;
  CAPDRIVERCAPS CapDriverCaps;
  CAPTUREPARMS CaptureParms;
  LPBITMAPINFO BitMapPtr;
};

// VFW compressed formats are listed at http://www.webartz.com/fourcc/
#define VTK_BI_UYVY 0x59565955

vtkCxxRevisionMacro(vtkWin32VideoSource2, "$Revision: 1.1 $");
vtkStandardNewMacro(vtkWin32VideoSource2);

#if ( _MSC_VER >= 1300 ) // Visual studio .NET
#pragma warning ( disable : 4311 )
#pragma warning ( disable : 4312 )
#  define vtkGetWindowLong GetWindowLongPtr
#  define vtkSetWindowLong SetWindowLongPtr
#  define vtkGWL_USERDATA GWLP_USERDATA
#else // regular Visual studio 
#  define vtkGetWindowLong GetWindowLong
#  define vtkSetWindowLong SetWindowLong
#  define vtkGWL_USERDATA GWL_USERDATA
#endif // 

//----------------------------------------------------------------------------
vtkWin32VideoSource2::vtkWin32VideoSource2()
{
  this->Internal = new vtkWin32VideoSource2Internal;
  this->Initialized = 0;

  this->FrameRate = 30;
  this->Buffer->GetFrameFormat()->SetPixelFormat(VTK_RGB);
  this->NumberOfScalarComponents = 3;
  this->Buffer->GetFrameFormat()->SetBitsPerPixel(24);
  this->Buffer->GetFrameFormat()->SetTopDown(0);
  this->Buffer->GetFrameFormat()->SetRowAlignment(4);
  for (int i = 0; i < this->Buffer->GetBufferSize(); i++)
    {
    this->Buffer->GetFrame(i)->SetPixelFormat(VTK_RGB);
    this->Buffer->GetFrame(i)->SetBitsPerPixel(24);
    this->Buffer->GetFrame(i)->SetTopDown(0);
    this->Buffer->GetFrame(i)->SetRowAlignment(4);
    this->Buffer->GetFrame(i)->Allocate();
    }

  this->Buffer->GetFrameFormat()->SetFrameGrabberType(FG_WIN32);
  for (int i = 0; i < this->Buffer->GetBufferSize(); i++)
    {
    this->Buffer->GetFrame(i)->SetFrameGrabberType(FG_WIN32);
    }

  this->Internal->CapWnd = NULL;
  this->Internal->ParentWnd = NULL;
  this->BitMapSize = 0;
  this->Internal->BitMapPtr = NULL;
  this->WndClassName[0] = '\0';

  this->Preview = 0;
}

//----------------------------------------------------------------------------
vtkWin32VideoSource2::~vtkWin32VideoSource2()
{ 
  this->vtkWin32VideoSource2::ReleaseSystemResources();

  if (this->Internal->BitMapPtr != NULL)
    {
    delete [] (char *)(this->Internal->BitMapPtr);
    }
  this->Internal->BitMapPtr = NULL;
  this->BitMapSize = 0;
  delete this->Internal;
}

//----------------------------------------------------------------------------
void vtkWin32VideoSource2::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "Preview: " << (this->Preview ? "On\n" : "Off\n");
}

//----------------------------------------------------------------------------
// This is empty for now because we aren't displaying the capture window
LONG FAR PASCAL
vtkWin32VideoSource2WinProc(HWND hwnd, UINT message, 
                           WPARAM wParam, LPARAM lParam)
{
  vtkWin32VideoSource2 *self = (vtkWin32VideoSource2 *)\
    (vtkGetWindowLong(hwnd,vtkGWL_USERDATA));

  switch(message) {
  
  case WM_MOVE:
    //cerr << "WM_MOVE\n";
    break;

  case WM_SIZE:
    //cerr << "WM_SIZE\n";
    break;
    
  case WM_DESTROY:
    //cerr << "WM_DESTROY\n";
    self->OnParentWndDestroy();
    break;

  case WM_CLOSE:
    //cerr << "WM_CLOSE\n";
    self->PreviewOff();
    return 0;
  }

  return(DefWindowProc(hwnd, message, wParam, lParam));
}

//----------------------------------------------------------------------------
LRESULT PASCAL vtkWin32VideoSource2CapControlProc(HWND hwndC, int nState)
{
  vtkWin32VideoSource2 *self = (vtkWin32VideoSource2 *)(capGetUserData(hwndC));

  if (nState == CONTROLCALLBACK_PREROLL)
    {
    //cerr << "controlcallback preroll\n";
    self->SetStartTimeStamp(vtkAccurateTimer::GetSystemTime());      
    }
  else if (nState == CONTROLCALLBACK_CAPTURING)
    {
    //cerr << "controlcallback capturing\n";
    }

  return TRUE;
}

//----------------------------------------------------------------------------
LRESULT PASCAL vtkWin32VideoSource2CallbackProc(HWND hwndC, LPVIDEOHDR lpVHdr)
{
  vtkWin32VideoSource2 *self = (vtkWin32VideoSource2 *)(capGetUserData(hwndC));
  self->LocalInternalGrab(lpVHdr);

  return 0;
}

//----------------------------------------------------------------------------
// this callback is left in for debug purposes
LRESULT PASCAL vtkWin32VideoSource2StatusCallbackProc(HWND vtkNotUsed(hwndC), 
                                                     int nID, 
                                                     LPCSTR vtkNotUsed(lpsz))
{
  //vtkWin32VideoSource2 *self = (vtkWin32VideoSource2 *)(capGetUserData(hwndC));

  if (nID == IDS_CAP_BEGIN)
    {
    //cerr << "start of capture\n";
    }

  if (nID == IDS_CAP_END)
    {
    //cerr << "end of capture\n";
    }

  return 1;
}

//----------------------------------------------------------------------------
LRESULT PASCAL vtkWin32VideoSource2ErrorCallbackProc(HWND hwndC,
                                                    int ErrID, 
                                                    LPSTR lpErrorText)
{
  if (ErrID)
    {
    char buff[84];
    sprintf(buff,"Error# %d",ErrID);
    MessageBox(hwndC,lpErrorText, buff, MB_OK | MB_ICONEXCLAMATION);
    //vtkGenericWarningMacro(<< buff << ' ' << lpErrorText);
    }
  return 1;
}

//----------------------------------------------------------------------------
void vtkWin32VideoSource2::Initialize()
{
  int i;

  if (this->Initialized)
    {
    return;
    }

  // Preliminary update of frame buffer, just in case we don't get
  // though the initialization but need the framebuffer for Updates
  this->UpdateFrameBuffer();

  // It is necessary to create not one, but two windows in order to
  // do frame grabbing under VFW.  Why do we need any?

  // get necessary process info
  HINSTANCE hinstance = GetModuleHandle(NULL);

  strcpy(this->WndClassName,"VTKVideo");

  // set up a class for the main window
  WNDCLASS wc;
  wc.lpszClassName = this->WndClassName;
  wc.hInstance = hinstance;
  wc.lpfnWndProc = reinterpret_cast<WNDPROC>(&vtkWin32VideoSource2WinProc);
  wc.hCursor = LoadCursor(NULL,IDC_ARROW);
  wc.hIcon = NULL;
  wc.lpszMenuName = NULL;
  wc.hbrBackground = NULL;
  wc.style = CS_HREDRAW|CS_VREDRAW;
  wc.cbClsExtra = sizeof(void *);
  wc.cbWndExtra = 0;
    
  for (i = 1; i <= 10; i++)
    {
    if (RegisterClass(&wc))
      {
      break;
      }
    // try again with a slightly different name
    sprintf(this->WndClassName,"VTKVideo %d",i);
    }
    
  if (i > 32)
    {
    LOG_ERROR("Initialize: failed to register VTKVideo class"\
                    << " (" << GetLastError() << ")");
    return;
    }

  DWORD style = WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|
                WS_CLIPCHILDREN|WS_CLIPSIBLINGS;

  if (this->Preview)
    {
    style |= WS_VISIBLE;
    }

  // set up the parent window, but don't show it
  int frameSize[3];
  this->Buffer->GetFrameFormat()->GetFrameSize(frameSize);

  this->Internal->ParentWnd = CreateWindow(
                this->WndClassName,
                "VTK Video Window",
                style,
                0, 0, 
                frameSize[0]+2*GetSystemMetrics(SM_CXFIXEDFRAME), 
                frameSize[1]+2*GetSystemMetrics(SM_CYFIXEDFRAME)
                                  +GetSystemMetrics(SM_CYBORDER)
                                  +GetSystemMetrics(SM_CYSIZE),
                NULL,
                NULL,
                hinstance,
                NULL);
    
  if (!this->Internal->ParentWnd) 
    {
    LOG_ERROR("Initialize: failed to create window"\
                    << " (" << GetLastError() << ")");
    return;
    }

  // set the user data to 'this'
  vtkSetWindowLong(this->Internal->ParentWnd,vtkGWL_USERDATA,(LONG)this);

  // Create the capture window
  this->Internal->CapWnd = capCreateCaptureWindow("Capture",
                      WS_CHILD|WS_VISIBLE, 0, 0, 
                      frameSize[0], frameSize[1],
                      this->Internal->ParentWnd,1);

  if (!this->Internal->CapWnd) 
    {
    LOG_ERROR("Initialize: failed to create capture window"\
                    << " (" << GetLastError() << ")");
    this->ReleaseSystemResources();
    return;
    }

  // connect to the driver
  if (!capDriverConnect(this->Internal->CapWnd,0))
    {
    LOG_ERROR("Initialize: couldn't connect to driver"\
                    << " (" << GetLastError() << ")");
    this->ReleaseSystemResources();
    return;
    }

  capDriverGetCaps(this->Internal->CapWnd,&this->Internal->CapDriverCaps,sizeof(CAPDRIVERCAPS));
  
  // set up the video format
  this->DoVFWFormatSetup();

  // set the capture parameters
  capCaptureGetSetup(this->Internal->CapWnd,&this->Internal->CaptureParms,sizeof(CAPTUREPARMS));
    
  if (this->FrameRate > 0)
    {
    this->Internal->CaptureParms.dwRequestMicroSecPerFrame = 
                                    int(1000000/this->FrameRate);
    }
  else
    {
    this->Internal->CaptureParms.dwRequestMicroSecPerFrame = 0;
    }

  this->Internal->CaptureParms.fMakeUserHitOKToCapture = FALSE;
  this->Internal->CaptureParms.fYield = 1;
  this->Internal->CaptureParms.fCaptureAudio = FALSE;
  this->Internal->CaptureParms.vKeyAbort = 0x00;
  this->Internal->CaptureParms.fAbortLeftMouse = FALSE;
  this->Internal->CaptureParms.fAbortRightMouse = FALSE;
  this->Internal->CaptureParms.fLimitEnabled = FALSE;
  this->Internal->CaptureParms.wNumAudioRequested = 0;
  this->Internal->CaptureParms.wPercentDropForError = 100;
  this->Internal->CaptureParms.dwAudioBufferSize = 0;
  this->Internal->CaptureParms.AVStreamMaster = AVSTREAMMASTER_NONE;
  
  if (!capCaptureSetSetup(this->Internal->CapWnd,&this->Internal->CaptureParms,
                            sizeof(CAPTUREPARMS)))
    {
    LOG_ERROR("Initialize: setup of capture parameters failed"\
                    << " (" << GetLastError() << ")");
    this->ReleaseSystemResources();
    return;
    }

  // set user data for callbacks
  if (!capSetUserData(this->Internal->CapWnd,(long)this))
    {
    LOG_ERROR("Initialize: couldn't set user data for callback"\
                    << " (" << GetLastError() << ")");
    this->ReleaseSystemResources();
    return;
    }
    
  // install the callback to precisely time beginning of grab
  if (!capSetCallbackOnCapControl(this->Internal->CapWnd,
                                  &vtkWin32VideoSource2CapControlProc))
    {
    LOG_ERROR("Initialize: couldn't set control callback"\
                    << " (" << GetLastError() << ")");
    this->ReleaseSystemResources();
    return;
    }    

  // install the callback to copy frames into the buffer on sync grabs
  if (!capSetCallbackOnFrame(this->Internal->CapWnd,
                             &vtkWin32VideoSource2CallbackProc))
    {
    LOG_ERROR("Initialize: couldn't set frame callback"\
                    << " (" << GetLastError() << ")");
    this->ReleaseSystemResources();
    return;
    }
  // install the callback to copy frames into the buffer on stream grabs
  if (!capSetCallbackOnVideoStream(this->Internal->CapWnd,
                                   &vtkWin32VideoSource2CallbackProc))
    {
    LOG_ERROR("Initialize: couldn't set stream callback"\
                    << " (" << GetLastError() << ")");
    this->ReleaseSystemResources();
    return;
    }
  // install the callback to get info on start/end of streaming
  if (!capSetCallbackOnStatus(this->Internal->CapWnd,
                             &vtkWin32VideoSource2StatusCallbackProc))
    {
    LOG_ERROR("Initialize: couldn't set status callback"\
                    << " (" << GetLastError() << ")");
    this->ReleaseSystemResources();
    return;
    }
  // install the callback to send messages to user
  if (!capSetCallbackOnError(this->Internal->CapWnd,
                             &vtkWin32VideoSource2ErrorCallbackProc))
    {
    LOG_ERROR("Initialize: couldn't set error callback"\
                    << " (" << GetLastError() << ")");
    this->ReleaseSystemResources();
    return;
    }
  
  capOverlay(this->Internal->CapWnd,TRUE);

  // update framebuffer again to reflect any changes which
  // might have occurred
  this->UpdateFrameBuffer();

  this->Initialized = 1;
}

//----------------------------------------------------------------------------
void vtkWin32VideoSource2::SetPreview(int p)
{
  if (this->Preview == p)
    {
    return;
    }

  this->Preview = p;
  this->Modified();
  
  if (this->Internal->CapWnd == NULL || this->Internal->ParentWnd == NULL)
    {
    return;
    }

  if (p)
    {
    ShowWindow(this->Internal->ParentWnd,SW_SHOWNORMAL);
    }
  else
    {
    ShowWindow(this->Internal->ParentWnd,SW_HIDE);
    }
}

//----------------------------------------------------------------------------
void vtkWin32VideoSource2::ReleaseSystemResources()
{
  // destruction of ParentWnd causes OnParentWndDestroy to be called
  if (this->Internal->ParentWnd)
    {
    DestroyWindow(this->Internal->ParentWnd);
    }
}

//----------------------------------------------------------------------------
void vtkWin32VideoSource2::OnParentWndDestroy()
{
  if (this->Recording)
    {
    this->Stop();
    }  

  if (this->Internal->CapWnd)
    {
    //MessageBox(this->Internal->ParentWnd, "capDriverDisconnect(this->Internal->CapWnd)", "", MB_OK | MB_ICONEXCLAMATION);
    capDriverDisconnect(this->Internal->CapWnd);
    //MessageBox(this->Internal->ParentWnd, "DestroyWindow(this->Internal->CapWnd)", "", MB_OK | MB_ICONEXCLAMATION);
    DestroyWindow(this->Internal->CapWnd);
    this->Internal->CapWnd = NULL;
  }
  if (this->WndClassName[0] != '\0')
    {
    UnregisterClass(this->WndClassName,GetModuleHandle(NULL));
    this->WndClassName[0] = '\0';
    }

  this->Internal->ParentWnd = NULL;
  this->Initialized = 0;
}

//----------------------------------------------------------------------------
// copy the Device Independent Bitmap from the VFW framebuffer into the
// vtkVideoSource2 framebuffer (don't do the unpacking yet)
void vtkWin32VideoSource2::LocalInternalGrab(void* lpptr)
{
  LPVIDEOHDR lpVHdr = static_cast<LPVIDEOHDR>(lpptr);
  // cerr << "Grabbed\n";

  // the VIDEOHDR has the following contents, for quick ref:
  //
  // lpData                 pointer to locked data buffer
  // dwBufferLength         Length of data buffer
  // dwBytesUsed            Bytes actually used
  // dwTimeCaptured         Milliseconds from start of stream
  // dwUser                 for client's use
  // dwFlags                assorted flags (see VFW.H)
  // dwReserved[4]          reserved for driver

  unsigned char *cptrDIB = lpVHdr->lpData;

  // get a thread lock on the frame buffer
  this->Buffer->Lock();
 
  if (this->AutoAdvance)
    {
    this->AdvanceFrameBuffer(1);
    if (this->FrameIndex + 1 < this->Buffer->GetBufferSize())
      {
      this->FrameIndex++;
      }
    }
  
  this->FrameCount++;

  double indexTime = this->StartTimeStamp + 0.001 * lpVHdr->dwTimeCaptured;

  unsigned char *ptr = reinterpret_cast<unsigned char *>(this->Buffer->GetFrame(0)->GetVoidPointer(0));

  // the DIB has rows which are multiples of 4 bytes
  int frameExtent[6];
  this->Buffer->GetFrameFormat()->GetFrameExtent(frameExtent);
  int frameSize[3];
  this->Buffer->GetFrameFormat()->GetFrameSize(frameSize);

  int outBytesPerRow = ((frameExtent[1]- frameExtent[0]+1) * this->Buffer->GetFrameFormat()->GetBitsPerPixel() + 7)/8;
  outBytesPerRow += outBytesPerRow % this->Buffer->GetFrameFormat()->GetRowAlignment();
  int inBytesPerRow = frameSize[0] * (this->Internal->BitMapPtr->bmiHeader.biBitCount/8);
  outBytesPerRow += outBytesPerRow % 4;
  int rows = frameExtent[3]-frameExtent[2]+1;

  cptrDIB += frameExtent[0]*(this->Internal->BitMapPtr->bmiHeader.biBitCount/8);
  cptrDIB +=frameExtent[2]*inBytesPerRow;

  // uncompress or simply copy the DIB
  switch (this->Internal->BitMapPtr->bmiHeader.biCompression)
    {
    case BI_RGB:
    case VTK_BI_UYVY:
      if (outBytesPerRow == inBytesPerRow)
        {
        memcpy(ptr,cptrDIB,inBytesPerRow*rows);
        }
      else
        {
        while (--rows >= 0)
          {
          memcpy(ptr,cptrDIB,outBytesPerRow);
          ptr += outBytesPerRow;
          cptrDIB += inBytesPerRow;
          }
        }
      break;
    case BI_RLE8:  // not handled
    case BI_RLE4:
    case BI_BITFIELDS:
      break;
    }

  // add the new frame and the current time to the buffer
  this->Buffer->AddItem(this->Buffer->GetFrame(0), indexTime);

  this->Modified();

  this->Buffer->Unlock();
}

//----------------------------------------------------------------------------
void vtkWin32VideoSource2::Grab()
{
  if (this->Recording)
    {
    return;
    }

  // ensure that the frame buffer is properly initialized
  this->Initialize();
  if (!this->Initialized)
    {
    return;
    }

  // just do the grab, the callback does the rest
  this->SetStartTimeStamp(vtkAccurateTimer::GetSystemTime());
  capGrabFrameNoStop(this->Internal->CapWnd);
}

//----------------------------------------------------------------------------
void vtkWin32VideoSource2::Record()
{
  this->Initialize();
  if (!this->Initialized)
    {
    return;
    }

  if (!this->Recording)
    {
    this->Recording = 1;
    this->Modified();
    capCaptureSequenceNoFile(this->Internal->CapWnd);
    }
}
        
//----------------------------------------------------------------------------
void vtkWin32VideoSource2::Stop()
{
  if (this->Recording)
    {
    this->Recording = 0;
    this->Modified();

    capCaptureStop(this->Internal->CapWnd);
    }
}

//----------------------------------------------------------------------------
// codecs

static inline void vtkYUVToRGB(unsigned char *yuv, unsigned char *rgb)
{ 
  /* // floating point 
  int Y = yuv[0] - 16;
  int U = yuv[1] - 128;
  int V = yuv[2] - 128;

  int R = 1.164*Y + 1.596*V           + 0.5;
  int G = 1.164*Y - 0.813*V - 0.391*U + 0.5;
  int B = 1.164*Y           + 2.018*U + 0.5;
  */

  // integer math
  int Y = (yuv[0] - 16)*76284;
  int U = yuv[1] - 128;
  int V = yuv[2] - 128;

  int R = Y + 104595*V           ;
  int G = Y -  53281*V -  25625*U;
  int B = Y            + 132252*U;

  // round
  R += 32768;
  G += 32768;
  B += 32768;

  // shift
  R >>= 16;
  G >>= 16;
  B >>= 16;

  // clamp
  if (R < 0) { R = 0; }
  if (G < 0) { G = 0; }
  if (B < 0) { B = 0; }

  if (R > 255) { R = 255; };
  if (G > 255) { G = 255; };
  if (B > 255) { B = 255; };

  // output
  rgb[0] = R;
  rgb[1] = G;
  rgb[2] = B;
}

//----------------------------------------------------------------------------
void vtkWin32VideoSource2::VideoFormatDialog()
{
  this->Initialize();
  if (!this->Initialized)
    {
    return;
    }

  //if (!this->Internal->CapDriverCaps.fHasDlgVideoFormat)
  //  {
  //  MessageBox(this->Internal->ParentWnd,"The video device has no Format dialog.","", 
  //             MB_OK | MB_ICONEXCLAMATION);
  //  return;
  //  }

  capGetStatus(this->Internal->CapWnd,&this->Internal->CapStatus,sizeof(CAPSTATUS));
  if (this->Internal->CapStatus.fCapturingNow)
    {
    MessageBox(this->Internal->ParentWnd, "Can't alter video format while grabbing.","", 
               MB_OK | MB_ICONEXCLAMATION);
    return;
    } 

  int success = capDlgVideoFormat(this->Internal->CapWnd);
  if (success)
    {
    this->Buffer->Lock();
    this->DoVFWFormatCheck();
    this->Buffer->Unlock();
    }
}

//----------------------------------------------------------------------------
void vtkWin32VideoSource2::VideoSourceDialog()
{
  this->Initialize();
  if (!this->Initialized)
    {
    return;
    }

  //if (!this->Internal->CapDriverCaps.fHasDlgVideoSource)
  //  {
  //  MessageBox(this->Internal->ParentWnd,"The video device has no Source dialog.","", 
  //             MB_OK | MB_ICONEXCLAMATION);
  //  return;
  //  }

  capGetStatus(this->Internal->CapWnd,&this->Internal->CapStatus,sizeof(CAPSTATUS));
  if (this->Internal->CapStatus.fCapturingNow)
    {
    MessageBox(this->Internal->ParentWnd, "Can't alter video source while grabbing.","", 
               MB_OK | MB_ICONEXCLAMATION);
    return;
    } 

  int success = capDlgVideoSource(this->Internal->CapWnd);
  if (success)
    {
    this->Buffer->Lock();
    this->DoVFWFormatCheck();
    this->Buffer->Unlock();
    }
}

//----------------------------------------------------------------------------
// try for the specified frame size
void vtkWin32VideoSource2::SetFrameSize(int x, int y, int z)
{
  int frameSize[3];
  this->Buffer->GetFrameFormat()->GetFrameSize(frameSize);

  if (x == frameSize[0] && 
      y == frameSize[1] && 
      z == frameSize[2])
    {
    return;
    }

  if (x < 1 || y < 1 || z != 1) 
    {
    LOG_ERROR("SetFrameSize: Illegal frame size");
    return;
    }

  this->Buffer->GetFrameFormat()->SetFrameSize(x,y,z);
  this->Modified();

  if (this->Initialized) 
    {
    this->Buffer->Lock();
    this->UpdateFrameBuffer();
    this->DoVFWFormatSetup();
    this->Buffer->Unlock();
    }
}

//----------------------------------------------------------------------------
void vtkWin32VideoSource2::SetFrameRate(float rate)
{
  if (rate == this->FrameRate)
    {
    return;
    }

  this->FrameRate = rate;
  this->Modified();

  if (this->Initialized)
    {
    capCaptureGetSetup(this->Internal->CapWnd,&this->Internal->CaptureParms,sizeof(CAPTUREPARMS));
    if (this->FrameRate > 0)
      {
      this->Internal->CaptureParms.dwRequestMicroSecPerFrame = 
                            int(1000000/this->FrameRate);
      }
    else
      {
      this->Internal->CaptureParms.dwRequestMicroSecPerFrame = 0;
      }
    capCaptureSetSetup(this->Internal->CapWnd,&this->Internal->CaptureParms,sizeof(CAPTUREPARMS));
    }
}

//----------------------------------------------------------------------------
void vtkWin32VideoSource2::SetOutputFormat(int format)
{
  if (format == (int)(this->Buffer->GetFrameFormat()->GetPixelFormat()))
    {
    return;
    }

  this->Buffer->GetFrameFormat()->SetPixelFormat((unsigned int)format);

  // convert color format to number of scalar components
  int numComponents;

  switch (format)
    {
    case VTK_RGBA:
      numComponents = 4;
      break;
    case VTK_RGB:
      numComponents = 3;
      break;
    case VTK_LUMINANCE:
      numComponents = 1;
      break;
    default:
      numComponents = 0;
      LOG_ERROR("SetOutputFormat: Unrecognized color format.");
      break;
    }
  this->NumberOfScalarComponents = numComponents;

  if (this->Buffer->GetFrameFormat()->GetBitsPerPixel() != numComponents*8)
    {
    this->Buffer->Lock();
    this->Buffer->GetFrameFormat()->SetBitsPerPixel(numComponents*8);
    if (this->Initialized)
      {
      this->UpdateFrameBuffer();
      this->DoVFWFormatSetup();
      }
    this->Buffer->Unlock();
    }

  this->Modified();
}

//----------------------------------------------------------------------------
// check the current video format and set up the VTK video framebuffer to match
void vtkWin32VideoSource2::DoVFWFormatCheck()
{
  // get the real video format
  int formatSize = capGetVideoFormatSize(this->Internal->CapWnd);
  if (formatSize > this->BitMapSize)
    {
    if (this->Internal->BitMapPtr)
      {
      delete [] ((char *)this->Internal->BitMapPtr);
      }
    this->Internal->BitMapPtr = (LPBITMAPINFO) new char[formatSize];
    this->BitMapSize = formatSize;
    }
  capGetVideoFormat(this->Internal->CapWnd,this->Internal->BitMapPtr,formatSize);
  
  int frameSize[3];
  this->Buffer->GetFrameFormat()->GetFrameSize(frameSize);

  int bpp = this->Internal->BitMapPtr->bmiHeader.biBitCount;
  int width = this->Internal->BitMapPtr->bmiHeader.biWidth;
  int height = frameSize[1] = this->Internal->BitMapPtr->bmiHeader.biHeight;
  int compression = this->Internal->BitMapPtr->bmiHeader.biCompression;
  this->Buffer->GetFrameFormat()->SetCompression(compression);
  for (int i = 0; i < this->Buffer->GetBufferSize(); i++)
    {
    this->Buffer->GetFrame(0)->SetCompression(compression);
    }

  if (compression == VTK_BI_UYVY)
    {
    this->Buffer->GetFrameFormat()->SetTopDown(1);
    for (int i = 0; i < this->Buffer->GetBufferSize(); i++)
      {
      this->Buffer->GetFrame(i)->SetTopDown(1);
      }
    }
  else if (compression == BI_RGB)
    {
    this->Buffer->GetFrameFormat()->SetTopDown(0);
    for (int i = 0; i < this->Buffer->GetBufferSize(); i++)
      {
      this->Buffer->GetFrame(i)->SetTopDown(0);
      }
    }
  else
    {
    char fourcchex[16], fourcc[8];
    sprintf(fourcchex,"0x%08x",compression);
    for (int i = 0; i < 4; i++)
      {
      fourcc[i] = (compression >> (8*i)) & 0xff;
      if (!isprint(fourcc[i]))
        {
        fourcc[i] = '?';
        }
      }
    fourcc[4] = '\0';
    LOG_WARNING(<< "DoVFWFormatCheck: video compression mode " <<
                    fourcchex << " \"" << fourcc << "\": can't grab");
    }

  if (bpp != this->Buffer->GetFrameFormat()->GetBitsPerPixel())
    {
    switch (bpp)
      {
      case 1:
      case 4:
      case 8:
        this->Buffer->GetFrameFormat()->SetPixelFormat(VTK_LUMINANCE);
        this->NumberOfScalarComponents = 1;
        break;
      case 16: 
        if (compression != VTK_BI_UYVY)
          {
          this->Buffer->GetFrameFormat()->SetPixelFormat(VTK_RGB);
          this->NumberOfScalarComponents = 3;
          }
        break;
      case 24:
      case 32:
        if (this->Buffer->GetFrameFormat()->GetPixelFormat() != VTK_RGBA)
          {
          this->Buffer->GetFrameFormat()->SetPixelFormat(VTK_RGB);
          this->NumberOfScalarComponents = 3;
          }
        break;
      }
    }

  if (bpp != this->Buffer->GetFrameFormat()->GetBitsPerPixel() ||
      frameSize[0] != width ||
      frameSize[1] != height)
    {
    this->Buffer->GetFrameFormat()->SetBitsPerPixel(bpp);
    this->Buffer->GetFrameFormat()->SetFrameSize(width, height, frameSize[2]);
    this->Modified();
    this->UpdateFrameBuffer();
    }
}

//----------------------------------------------------------------------------
void vtkWin32VideoSource2::DoVFWFormatSetup()
{
  static int colorBits[3] = { 24, 32, 16 };
  static int greyBits[3] = { 8, 4, 1 };
  int i, bytesPerRow, bitCount;

  // get the real video format
  int formatSize = capGetVideoFormatSize(this->Internal->CapWnd);
  if (formatSize > this->BitMapSize)
    {
    if (this->Internal->BitMapPtr)
      {
      delete [] ((char *)this->Internal->BitMapPtr);
      }
    this->Internal->BitMapPtr = (LPBITMAPINFO) new char[formatSize];
    this->BitMapSize = formatSize;
    }
  capGetVideoFormat(this->Internal->CapWnd,this->Internal->BitMapPtr,formatSize);
  
  // set the format of the captured frames
  int frameSize[3];
  this->Buffer->GetFrameFormat()->GetFrameSize(frameSize);
  this->Internal->BitMapPtr->bmiHeader.biWidth = frameSize[0];
  this->Internal->BitMapPtr->bmiHeader.biHeight = frameSize[1];
  this->Internal->BitMapPtr->bmiHeader.biCompression = BI_RGB;
  this->Internal->BitMapPtr->bmiHeader.biClrUsed = 0;
  this->Internal->BitMapPtr->bmiHeader.biClrImportant = 0;
  
  for (i = 0; i < 4; i++)
    { // try for a 
    if (this->Buffer->GetFrameFormat()->GetPixelFormat() == VTK_RGBA || this->Buffer->GetFrameFormat()->GetPixelFormat() == VTK_RGB)
      {
      bitCount = colorBits[i];
      }
    else
      {
      bitCount = greyBits[i];
      }
    bytesPerRow = (frameSize[0]*bitCount+7)/8;
    bytesPerRow += bytesPerRow % this->Buffer->GetFrameFormat()->GetRowAlignment();
    this->Internal->BitMapPtr->bmiHeader.biBitCount = bitCount;
    this->Internal->BitMapPtr->bmiHeader.biSizeImage = bytesPerRow*frameSize[1];
    if (capSetVideoFormat(this->Internal->CapWnd,this->Internal->BitMapPtr,
                          sizeof(BITMAPINFOHEADER)))
      {
      break;
      }
    }
  if (i > 4)
    {
    LOG_WARNING(<< "DoVFWFormatSetup: invalid video format for device"\
                    << " (" << GetLastError() << ")");
    }
  this->DoVFWFormatCheck();
}











