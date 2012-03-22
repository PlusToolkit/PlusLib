/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

/*=========================================================================
The following copyright notice is applicable to parts of this file:
Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
All rights reserved.
See Copyright.txt or http://www.kitware.com/Copyright.htm for details.
Authors include: Danielle Pace
=========================================================================*/ 

#include "vtkWin32VideoSource2.h"
#include "PlusConfigure.h"
#include "vtkCriticalSection.h"
#include "vtkObjectFactory.h"
#include "vtkUnsignedCharArray.h"
#include "vtkVideoBuffer.h"
#include "ConvertYuy2ToRgb.h"


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
  //----------------------------------------------------------------------------
  vtkWin32VideoSource2Internal()
  {
    CapWnd = NULL;
    ParentWnd = NULL;
    BitMapInfoSize = 0;
    BitMapInfoPtr = NULL;
  }
  //----------------------------------------------------------------------------
  virtual ~vtkWin32VideoSource2Internal()
  {
    delete [] (char *)(BitMapInfoPtr);
    BitMapInfoPtr = NULL;
    BitMapInfoSize = 0;
  }
  //----------------------------------------------------------------------------
  PlusStatus GetBitmapInfoFromCaptureDevice()
  {
    if (CapWnd==NULL)
    {
      LOG_ERROR("Cannot get bitmap info, capture window has not been created yet");
      return PLUS_FAIL;
    }
    int formatSize = capGetVideoFormatSize(CapWnd);
    if (formatSize > this->BitMapInfoSize)
    {
      delete [] ((char *)BitMapInfoPtr);
      BitMapInfoPtr = (LPBITMAPINFO) new char[formatSize];
      BitMapInfoSize = formatSize;
    }
    if (!capGetVideoFormat(CapWnd,BitMapInfoPtr,formatSize))
    {
      LOG_ERROR("Cannot get bitmap info from capture window");
      return PLUS_FAIL;
    }
    return PLUS_SUCCESS;
  }
  //----------------------------------------------------------------------------
  PlusStatus SetBitmapInfoInCaptureDevice()
  {
    if (!capSetVideoFormat(CapWnd,BitMapInfoPtr,BitMapInfoSize))
    {
      LOG_ERROR("Cannot get bitmap info from capture window");
      return PLUS_FAIL;
    }
    return PLUS_SUCCESS;
  }

  HWND CapWnd;
  HWND ParentWnd;
  CAPSTATUS CapStatus;
  CAPDRIVERCAPS CapDriverCaps;
  CAPTUREPARMS CaptureParms;
  LPBITMAPINFO BitMapInfoPtr;
  int BitMapInfoSize;
};

// VFW compressed formats are listed at http://www.webartz.com/fourcc/
static const long VTK_BI_UYVY=0x59565955;
static const long VTK_BI_YUY2=0x32595559;

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

  this->FrameRate = 30;
  this->Buffer->SetFrameSize(640,480);
  this->SetFrameBufferSize(200);

  this->WndClassName=NULL;

  this->Preview = 0;
  this->FrameIndex = 0;
}

//----------------------------------------------------------------------------
vtkWin32VideoSource2::~vtkWin32VideoSource2()
{ 
  this->vtkWin32VideoSource2::ReleaseSystemResources();

  delete this->Internal;
  this->Internal=NULL;
}

//----------------------------------------------------------------------------
void vtkWin32VideoSource2::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "Preview: " << (this->Preview ? "On\n" : "Off\n");
}

//----------------------------------------------------------------------------
LONG FAR PASCAL vtkWin32VideoSource2WinProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  vtkWin32VideoSource2 *self = (vtkWin32VideoSource2 *)(vtkGetWindowLong(hwnd,vtkGWL_USERDATA));
  switch(message) 
  {
  case WM_MOVE:
    LOG_TRACE("WM_MOVE");
    break;
  case WM_SIZE:
    LOG_TRACE("WM_SIZE");
    break;
  case WM_DESTROY:
    LOG_TRACE("WM_DESTROY");
    self->OnParentWndDestroy();
    break;
  case WM_CLOSE:
    LOG_TRACE("WM_CLOSE");
    self->PreviewOff();
    return 0;
  }
  return DefWindowProc(hwnd, message, wParam, lParam);
}

//----------------------------------------------------------------------------
LRESULT PASCAL vtkWin32VideoSource2CapControlProc(HWND hwndC, int nState)
{
  vtkWin32VideoSource2 *self = (vtkWin32VideoSource2 *)(capGetUserData(hwndC));
  if (nState == CONTROLCALLBACK_PREROLL)
  {
    LOG_TRACE("controlcallback preroll");
  }
  else if (nState == CONTROLCALLBACK_CAPTURING)
  {
    LOG_TRACE("controlcallback capturing");
  }
  return TRUE;
}

//----------------------------------------------------------------------------
LRESULT PASCAL vtkWin32VideoSource2CallbackProc(HWND hwndC, LPVIDEOHDR lpVideoHeader)
{
  vtkWin32VideoSource2 *self = (vtkWin32VideoSource2 *)(capGetUserData(hwndC));
  self->AddFrameToBuffer(lpVideoHeader);
  return 0;
}

//----------------------------------------------------------------------------
// this callback is left in for debug purposes
LRESULT PASCAL vtkWin32VideoSource2StatusCallbackProc(HWND vtkNotUsed(hwndC), int nID, LPCSTR vtkNotUsed(lpsz))
{
  //vtkWin32VideoSource2 *self = (vtkWin32VideoSource2 *)(capGetUserData(hwndC));
  if (nID == IDS_CAP_BEGIN)
  {
    LOG_TRACE("start of capture");
  }
  if (nID == IDS_CAP_END)
  {
    LOG_TRACE("end of capture");
  }
  return 1;
}

//----------------------------------------------------------------------------
LRESULT PASCAL vtkWin32VideoSource2ErrorCallbackProc(HWND hwndC, int ErrID, LPSTR lpErrorText)
{
  if (ErrID)
  {
    LOG_ERROR("Video for Windows error: #"<<ErrID);
  }
  return 1;
}

//----------------------------------------------------------------------------
PlusStatus vtkWin32VideoSource2::InternalConnect()
{
  if (this->GetConnected())
  {
    // already connected
    return PLUS_SUCCESS;
  }

  // It is necessary to create not one, but two windows in order to
  // do frame grabbing under VFW.  Why do we need any?

  // get necessary process info
  HINSTANCE hinstance = GetModuleHandle(NULL);

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

  bool registrationSuccessful=false;
  const int MAX_WINDOW_CLASS_REGISTRATION_ATTEMPTS=32;
  for (int i = 1; i <= MAX_WINDOW_CLASS_REGISTRATION_ATTEMPTS; i++)
  {
    if (RegisterClass(&wc))
    {
      SetWndClassName(wc.lpszClassName);
      registrationSuccessful=true;
      break;
    }
    // try with a slightly different name at each registration attempt   
    std::ostrstream windowName;
    windowName << "VTKVideo " << i << std::ends;
    wc.lpszClassName = windowName.str();
  }
  if (!registrationSuccessful)
  {
    LOG_ERROR("Initialize: failed to register VTKVideo class (" << GetLastError() << ")");
    return PLUS_FAIL;
  }

  DWORD style = WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_CLIPCHILDREN|WS_CLIPSIBLINGS;

  if (this->Preview)
  {
    style |= WS_VISIBLE;
  }

  // set up the parent window, but don't show it
  int frameSize[2]={0,0};
  this->Buffer->GetFrameSize(frameSize);

  this->Internal->ParentWnd = CreateWindow(this->WndClassName,"Plus video capture window", style, 0, 0, 
    frameSize[0]+2*GetSystemMetrics(SM_CXFIXEDFRAME),
    frameSize[1]+2*GetSystemMetrics(SM_CYFIXEDFRAME)+GetSystemMetrics(SM_CYBORDER)+GetSystemMetrics(SM_CYSIZE),
    NULL,NULL,hinstance,NULL);

  if (!this->Internal->ParentWnd) 
  {
    LOG_ERROR("Initialize: failed to create window (" << GetLastError() << ")");
    return PLUS_FAIL;
  }

  // set the user data to 'this'
  vtkSetWindowLong(this->Internal->ParentWnd,vtkGWL_USERDATA,(LONG)this);

  // Create the capture window
  this->Internal->CapWnd = capCreateCaptureWindow("Capture", WS_CHILD|WS_VISIBLE, 0, 0, 
    frameSize[0], frameSize[1], this->Internal->ParentWnd,1);

  if (!this->Internal->CapWnd) 
  {
    LOG_ERROR("Initialize: failed to create capture window (" << GetLastError() << ")");
    this->ReleaseSystemResources();
    return PLUS_FAIL;
  }

  // connect to the driver
  if (!capDriverConnect(this->Internal->CapWnd,0))
  {
    LOG_ERROR("Initialize: couldn't connect to driver (" << GetLastError() << ")");
    this->ReleaseSystemResources();
    return PLUS_FAIL;
  }

  capDriverGetCaps(this->Internal->CapWnd,&this->Internal->CapDriverCaps,sizeof(CAPDRIVERCAPS));

  // set up the video capture format
  this->Internal->GetBitmapInfoFromCaptureDevice();
  this->Internal->BitMapInfoPtr->bmiHeader.biWidth = frameSize[0];
  this->Internal->BitMapInfoPtr->bmiHeader.biHeight = frameSize[1];
  if (this->Internal->SetBitmapInfoInCaptureDevice()!=PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to set requested frame size in the capture device");
    this->ReleaseSystemResources();
    return PLUS_FAIL;
  }

  // set the capture parameters
  capCaptureGetSetup(this->Internal->CapWnd,&this->Internal->CaptureParms,sizeof(CAPTUREPARMS));

  if (this->FrameRate > 0)
  {
    this->Internal->CaptureParms.dwRequestMicroSecPerFrame = int(1000000/this->FrameRate);
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

  if (!capCaptureSetSetup(this->Internal->CapWnd,&this->Internal->CaptureParms,sizeof(CAPTUREPARMS)))
  {
    LOG_ERROR("Initialize: setup of capture parameters failed (" << GetLastError() << ")");
    this->ReleaseSystemResources();
    return PLUS_FAIL;
  }

  // set user data for callbacks
  if (!capSetUserData(this->Internal->CapWnd,(long)this))
  {
    LOG_ERROR("Initialize: couldn't set user data for callback (" << GetLastError() << ")");
    this->ReleaseSystemResources();
    return PLUS_FAIL;
  }

  // install the callback to precisely time beginning of grab
  if (!capSetCallbackOnCapControl(this->Internal->CapWnd, &vtkWin32VideoSource2CapControlProc))
  {
    LOG_ERROR("Initialize: couldn't set control callback (" << GetLastError() << ")");
    this->ReleaseSystemResources();
    return PLUS_FAIL;
  }    

  // install the callback to copy frames into the buffer on sync grabs
  if (!capSetCallbackOnFrame(this->Internal->CapWnd,&vtkWin32VideoSource2CallbackProc))
  {
    LOG_ERROR("Initialize: couldn't set frame callback (" << GetLastError() << ")");
    this->ReleaseSystemResources();
    return PLUS_FAIL;
  }
  // install the callback to copy frames into the buffer on stream grabs
  if (!capSetCallbackOnVideoStream(this->Internal->CapWnd,&vtkWin32VideoSource2CallbackProc))
  {
    LOG_ERROR("Initialize: couldn't set stream callback (" << GetLastError() << ")");
    this->ReleaseSystemResources();
    return PLUS_FAIL;
  }
  // install the callback to get info on start/end of streaming
  if (!capSetCallbackOnStatus(this->Internal->CapWnd, &vtkWin32VideoSource2StatusCallbackProc))
  {
    LOG_ERROR("Initialize: couldn't set status callback (" << GetLastError() << ")");
    this->ReleaseSystemResources();
    return PLUS_FAIL;
  }
  // install the callback to send messages to user
  if (!capSetCallbackOnError(this->Internal->CapWnd, &vtkWin32VideoSource2ErrorCallbackProc))
  {
    LOG_ERROR("Initialize: couldn't set error callback (" << GetLastError() << ")");
    this->ReleaseSystemResources();
    return PLUS_FAIL;
  }

  capOverlay(this->Internal->CapWnd,TRUE);
  
  // update framebuffer again to reflect any changes which
  // might have occurred
  this->UpdateFrameBuffer();

  return PLUS_SUCCESS;
}

PlusStatus vtkWin32VideoSource2::InternalDisconnect()
{
  if (this->Internal->CapWnd)
  {
    LOG_DEBUG("capDriverDisconnect(this->Internal->CapWnd)");
    capDriverDisconnect(this->Internal->CapWnd);
    LOG_DEBUG("DestroyWindow(this->Internal->CapWnd)");
    DestroyWindow(this->Internal->CapWnd);
    this->Internal->CapWnd = NULL;
  }
  if (this->WndClassName!=NULL && this->WndClassName[0] != '\0')
  {
    // window class name is valid
    UnregisterClass(this->WndClassName,GetModuleHandle(NULL));
    SetWndClassName("");
  }

  return PLUS_SUCCESS;
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

  if (GetConnected())
  {
    if (this->Internal->CapWnd == NULL || this->Internal->ParentWnd == NULL)
    {
      LOG_ERROR("Capture windows have not been intialized");
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
  Disconnect();
  this->Internal->ParentWnd = NULL;
}

//----------------------------------------------------------------------------
// copy the Device Independent Bitmap from the VFW framebuffer into the
// vtkPlusVideoSource framebuffer (don't do the unpacking yet)
PlusStatus vtkWin32VideoSource2::AddFrameToBuffer(void* lpVideoHeader)
{
  int inputCompression = this->Internal->BitMapInfoPtr->bmiHeader.biCompression;

  bool vFlip=false;
  switch (inputCompression)
  {  
  case VTK_BI_YUY2:
    vFlip=true;
    break;
  case VTK_BI_UYVY:
    vFlip=true;
    break;
  case BI_RGB:
    vFlip=false;
    break;
  default:
    char fourcchex[16], fourcc[8];
    sprintf(fourcchex,"0x%08x",inputCompression);
    for (int i = 0; i < 4; i++)
    {
      fourcc[i] = (inputCompression >> (8*i)) & 0xff;
      if (!isprint(fourcc[i]))
      {
        fourcc[i] = '?';
      }
    }
    fourcc[4] = '\0';
    LOG_ERROR("AddFrameToBuffer: video compression mode " << fourcchex << " \"" << fourcc << "\": can't grab");
    return PLUS_FAIL;
  }

  LPVIDEOHDR lpVHdr = static_cast<LPVIDEOHDR>(lpVideoHeader);
  LOG_TRACE("Grabbed");

  // the VIDEOHDR has the following contents, for quick ref:
  //
  // lpData                 pointer to locked data buffer
  // dwBufferLength         Length of data buffer
  // dwBytesUsed            Bytes actually used
  // dwTimeCaptured         Milliseconds from start of stream
  // dwUser                 for client's use
  // dwFlags                assorted flags (see VFW.H)
  // dwReserved[4]          reserved for driver

  unsigned char *inputPixelsPtr = lpVHdr->lpData;

  this->FrameIndex++;
  double indexTime = this->Buffer->GetStartTime() + 0.001 * lpVHdr->dwTimeCaptured;

  unsigned char* outputPixelsPtr=(unsigned char*)this->UncompressedVideoFrame.GetBufferPointer();

  int inputBitsPerPixel=this->Internal->BitMapInfoPtr->bmiHeader.biBitCount;
  int outputBitsPerPixel=8;
  int outputRowAlignment=1;
  int outputFrameSize[2]={0,0};
  this->UncompressedVideoFrame.GetFrameSize(outputFrameSize);
  int outBytesPerRow = outputFrameSize[0] * this->UncompressedVideoFrame.GetNumberOfBytesPerPixel();
  outBytesPerRow += outBytesPerRow % outputRowAlignment;
  int inBytesPerRow = this->Internal->BitMapInfoPtr->bmiHeader.biWidth * (inputBitsPerPixel/8);
  outBytesPerRow += outBytesPerRow % 4;
  int rows = outputFrameSize[1];

  // uncompress or simply copy the DIB
  switch (inputCompression)
  {
  case BI_RGB:
    if (outBytesPerRow == inBytesPerRow)
    {
      memcpy(outputPixelsPtr,inputPixelsPtr,inBytesPerRow*rows);
    }
    else
    {
      while (--rows >= 0)
      {
        memcpy(outputPixelsPtr,inputPixelsPtr,outBytesPerRow);
        outputPixelsPtr += outBytesPerRow;
        inputPixelsPtr += inBytesPerRow;
      }
    }
    break;
  case VTK_BI_YUY2:
    YUV422P_to_gray(outputFrameSize[0], outputFrameSize[1], inputPixelsPtr, outputPixelsPtr);
    break;
  default:
    LOG_ERROR("Unknown frame format");
  }
  
  //PlusStatus status = this->Buffer->AddItem(&this->UncompressedVideoFrame, this->GetUsImageOrientation(), this->FrameIndex, indexTime, indexTime); 
  PlusStatus status = this->Buffer->AddItem(&this->UncompressedVideoFrame, this->GetUsImageOrientation(), this->FrameIndex, indexTime, indexTime); 

  this->Modified();
  return status;
}

//----------------------------------------------------------------------------
PlusStatus vtkWin32VideoSource2::InternalGrab()
{
  // just request the grab, the callback does the rest
  if (!capGrabFrameNoStop(this->Internal->CapWnd))
  {
    LOG_ERROR("Initialize: failed to request a single frame grab (" << GetLastError() << ")");
    return PLUS_FAIL;
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkWin32VideoSource2::InternalStartRecording()
{
  if (!capCaptureSequenceNoFile(this->Internal->CapWnd))
  {
    LOG_ERROR("Initialize: failed to request continuous frame grabbing (" << GetLastError() << ")");
    return PLUS_FAIL;
  }
  this->FrameIndex = 0;
  double startTime = vtkAccurateTimer::GetSystemTime();
  this->Buffer->SetStartTime(startTime);
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkWin32VideoSource2::InternalStopRecording()
{
  if (!capCaptureStop(this->Internal->CapWnd))
  {
    LOG_ERROR("Initialize: failed to request continuous frame grabbing (" << GetLastError() << ")");
    return PLUS_FAIL;
  }
  return PLUS_SUCCESS; 
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
PlusStatus vtkWin32VideoSource2::VideoFormatDialog()
{
  if (!GetConnected())
  {
    LOG_ERROR("vtkWin32VideoSource2::VideoFormatDialog failed, need to connect to the device first");
    return PLUS_FAIL;
  }

  //if (!this->Internal->CapDriverCaps.fHasDlgVideoFormat)
  //  {
  //  LOG_ERROR("vtkWin32VideoSource2::VideoFormatDialog failed, the video device has no Format dialog.");
  //  return PLUS_FAIL;
  //  }

  capGetStatus(this->Internal->CapWnd,&this->Internal->CapStatus,sizeof(CAPSTATUS));
  if (this->Internal->CapStatus.fCapturingNow)
  {
    LOG_ERROR("vtkWin32VideoSource2::VideoFormatDialog failed, can't alter video format while grabbing");    
    return PLUS_FAIL;
  } 

  int success = capDlgVideoFormat(this->Internal->CapWnd);
  if (!success)
  {
    LOG_ERROR("vtkWin32VideoSource2::VideoFormatDialog failed (" << GetLastError() << ")");
    return PLUS_FAIL;
  }
  this->UpdateFrameBuffer();  
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkWin32VideoSource2::VideoSourceDialog()
{
  //if (!this->Internal->CapDriverCaps.fHasDlgVideoSource)
  //  {
  //  LOG_ERROR("vtkWin32VideoSource2::VideoFormatDialog failed, the video device has no Source dialog.");
  //  return PLUS_FAIL;
  //  }

  capGetStatus(this->Internal->CapWnd,&this->Internal->CapStatus,sizeof(CAPSTATUS));
  if (this->Internal->CapStatus.fCapturingNow)
  {
    LOG_ERROR("vtkWin32VideoSource2::VideoFormatDialog failed, can't alter video source while grabbing");    
    return PLUS_FAIL;
  }

  int success = capDlgVideoSource(this->Internal->CapWnd);
  if (!success)
  {
    LOG_ERROR("vtkWin32VideoSource2::VideoSourceDialog failed (" << GetLastError() << ")");
    return PLUS_FAIL;
  }
  return this->UpdateFrameBuffer();
}

//----------------------------------------------------------------------------
PlusStatus vtkWin32VideoSource2::SetFrameSize(int x, int y)
{
  if (this->Superclass::SetFrameSize(x,y)!=PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }
  if (this->GetConnected())
  {
    // set up the video capture format
    this->Internal->GetBitmapInfoFromCaptureDevice();
    int frameSize[2]={0,0};
    this->Buffer->GetFrameSize(frameSize);
    this->Internal->BitMapInfoPtr->bmiHeader.biWidth = frameSize[0];
    this->Internal->BitMapInfoPtr->bmiHeader.biHeight = frameSize[1];
    if (this->Internal->SetBitmapInfoInCaptureDevice()!=PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to set requested frame size in the capture device");
      return PLUS_FAIL;
    }
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkWin32VideoSource2::SetFrameRate(float rate)
{
  if (rate == this->FrameRate)
  {
    // no change
    return PLUS_SUCCESS;
  }

  this->FrameRate = rate;

  if (GetConnected())
  {
    capCaptureGetSetup(this->Internal->CapWnd,&this->Internal->CaptureParms,sizeof(CAPTUREPARMS));
    if (this->FrameRate > 0)
    {
      this->Internal->CaptureParms.dwRequestMicroSecPerFrame = int(1000000/this->FrameRate);
    }
    else
    {
      this->Internal->CaptureParms.dwRequestMicroSecPerFrame = 0;
    }
    capCaptureSetSetup(this->Internal->CapWnd,&this->Internal->CaptureParms,sizeof(CAPTUREPARMS));
  }
  
  this->Modified();
  return PLUS_SUCCESS;  
}

//----------------------------------------------------------------------------
PlusStatus vtkWin32VideoSource2::SetOutputFormat(int format)
{
  // convert color format to number of scalar components
  int numComponents=0;
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
    return PLUS_FAIL;
  }

  if (numComponents!=1)
  {
    LOG_ERROR("Currently only 1 component image output is supported. Requested "<<numComponents<<" components");
    return PLUS_FAIL;
  }

  this->Buffer->SetPixelType(itk::ImageIOBase::UCHAR);

  if (this->GetConnected())
  {
    // set up the video capture format
    this->Internal->GetBitmapInfoFromCaptureDevice();
    // TODO: update this->Internal->BitMapInfoPtr->bmiHeader
    if (this->Internal->SetBitmapInfoInCaptureDevice()!=PLUS_SUCCESS)
    {
      LOG_ERROR("Failed to set requested frame size in the capture device");
      return PLUS_FAIL;
    }
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkWin32VideoSource2::UpdateFrameBuffer()
{
  // get the real video format
  this->Internal->GetBitmapInfoFromCaptureDevice();

  int width = this->Internal->BitMapInfoPtr->bmiHeader.biWidth;
  int height = this->Internal->BitMapInfoPtr->bmiHeader.biHeight;
  PlusCommon::ITKScalarPixelType pixelType=itk::ImageIOBase::UCHAR; // always convert output to 8-bit grayscale
  
  this->Buffer->SetFrameSize(width, height);
  this->Buffer->SetPixelType(pixelType); 

  int frameSize[2]={width, height};
  this->UncompressedVideoFrame.AllocateFrame(frameSize,pixelType);

  /*
  int bpp = inputBitsPerPixel;
  int compression = this->Internal->BitMapInfoPtr->bmiHeader.biCompression;

  if (bpp==8 && compression==BI_RGB)
  {
    this->Buffer->SetPixelType(itk::ImageIOBase::UCHAR);
  }
  else
  {
    LOG_ERROR("Unsupported video format: bpp="<<bpp<<", compression="<<compression);
    return PLUS_FAIL;
  }
  */

  /*
  if (bpp != outputBitsPerPixel)
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

  if (bpp != outputBitsPerPixel ||
  {
    this->Buffer->GetFrameFormat()->SetBitsPerPixel(bpp);
    this->Buffer->GetFrameFormat()->SetFrameSize(width, height, frameSize[2]);    

  }
  */
  return PLUS_SUCCESS;
}

