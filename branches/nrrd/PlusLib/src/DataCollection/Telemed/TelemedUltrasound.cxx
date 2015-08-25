/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/


#include "PlusConfigure.h"

#include "TelemedUltrasound.h"

#include <math.h>
#include <string>

#define SAFE_RELEASE(x) { if (x) x->Release(); x = NULL; }

//----------------------------------------------------------------------------
TelemedUltrasound::TelemedUltrasound()
{
  this->ImageWindowHandle = NULL;

  m_FrameSize[0] = 0;
  m_FrameSize[1] = 0;

  m_MaximumFrameSize[0] = 512;
  m_MaximumFrameSize[1] = 512;

  m_refCount = 0;
  m_usgfw2 = NULL;
  m_data_view = NULL;
  m_probe = NULL;
  m_mixer_control = NULL;
  m_depth_ctrl = NULL;
  m_b_power_ctrl = NULL;
  m_b_gain_ctrl = NULL;
  m_b_dynrange_ctrl = NULL;
  m_b_frequency_ctrl = NULL;
  m_usg_control_change_cpnt = NULL;
  m_usg_control_change_cpnt_cookie = 0;
  m_usg_device_change_cpnt = NULL;
  m_usg_device_change_cpnt_cookie = 0;

  CoInitialize(NULL);
}

//----------------------------------------------------------------------------
TelemedUltrasound::~TelemedUltrasound()
{
  Disconnect(); // just in case it was forgot to be called
  CoUninitialize();
}

//----------------------------------------------------------------------------
static LRESULT CALLBACK ImageWindowProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
  TelemedUltrasound* self = (TelemedUltrasound*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
  return DefWindowProc (hwnd, iMsg, wParam, lParam) ;
}

//----------------------------------------------------------------------------
void TelemedUltrasound::SetMaximumFrameSize(int maxFrameSize[2])
{
  m_MaximumFrameSize[0]=maxFrameSize[0];
  m_MaximumFrameSize[1]=maxFrameSize[1];
}

//----------------------------------------------------------------------------
PlusStatus TelemedUltrasound::Connect()
{
  HINSTANCE hInst = GetModuleHandle(NULL);

  WNDCLASSEX		wndclass;
  wndclass.cbSize        = sizeof (wndclass);
  wndclass.style         = CS_HREDRAW | CS_VREDRAW | CS_BYTEALIGNCLIENT;
  wndclass.lpfnWndProc   = ImageWindowProc;
  wndclass.cbClsExtra    = 0;
  wndclass.cbWndExtra    = 0;
  wndclass.hInstance     = hInst;
  wndclass.hIcon         = NULL;
  wndclass.hCursor       = NULL;
  wndclass.hbrBackground = (HBRUSH) GetStockObject (BLACK_BRUSH);
  wndclass.lpszMenuName  = NULL;
  wndclass.lpszClassName = TEXT("TelemedImageWindow");
  wndclass.hIconSm       = NULL;
  RegisterClassEx(&wndclass);

  this->ImageWindowHandle = CreateWindow( TEXT("TelemedImageWindow"), TEXT("Ultrasound"), 
    WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_CLIPCHILDREN|WS_CLIPSIBLINGS, 0, 0,
    m_MaximumFrameSize[0]+2*GetSystemMetrics(SM_CXFIXEDFRAME),
    m_MaximumFrameSize[1]+2*GetSystemMetrics(SM_CYFIXEDFRAME)+GetSystemMetrics(SM_CYBORDER)+GetSystemMetrics(SM_CYSIZE),
    NULL, NULL, hInst, NULL);  

  if (this->ImageWindowHandle==NULL)
  {
    LOG_ERROR("Failed to create capture window");
    return PLUS_FAIL;
  }

  SetWindowLongPtr(this->ImageWindowHandle, GWLP_USERDATA, (LONG)this);

  // Create a bitmap for use in our DIB
  HDC hdc = GetDC(this->ImageWindowHandle) ;
  RECT rect;
  GetClientRect (this->ImageWindowHandle, &rect) ;
  int cx	= rect.right - rect.left;
  int cy	= rect.bottom - rect.top;
  this->DataHandle = CreateCompatibleBitmap (hdc, cx, cy);  
  GetObject (this->DataHandle, sizeof (BITMAP), (LPVOID) &this->Bitmap) ;
  // zero indexed window including borders
  size_t toAllocate=(this->Bitmap.bmWidth+16) * (this->Bitmap.bmHeight+4);
  this->MemoryBitmapBuffer.resize(toAllocate,0);

  CreateUsgControls();

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
void TelemedUltrasound::Disconnect()
{
  ReleaseUsgControls(true);

  if (this->ImageWindowHandle)
  {
    LOG_DEBUG("DestroyWindow(this->ImageWindowHandle)");
    DestroyWindow(this->ImageWindowHandle);
    this->ImageWindowHandle = NULL;
    UnregisterClass(TEXT("TelemedImageWindow"),GetModuleHandle(NULL));
  }
}

//----------------------------------------------------------------------------
void TelemedUltrasound::FreezeDevice(bool freeze)
{
  std::cout<< "TelemedUltrasound::FreezeDevice("<<freeze<<")" <<std::endl;
  if (m_data_view == NULL) return;
  if(freeze)
  {
    m_data_view->put_ScanState(SCAN_STATE_FREEZE);
  }
  else
  {
    m_data_view->put_ScanState(SCAN_STATE_RUN);
  }
}

//----------------------------------------------------------------------------
/* **********************************************************************
RETURNS:
HPALETTE - a handle to a spectrum palette - NULL on failure

REMARKS:
This function was stolen from Mike Irvine's SEEDIB sample. There's
no source code comments in his version either :)

This function will build a palette with a spectrum of colors.  It is
useful when you want to display a number of DIBs each with a different
palette yet still have an a good selection of colors to which the 
DIBs' colors will be mapped.

********************************************************************** */

/* **********************************************************************
LPBITMAPINFO DSGetBITMAPINFOForDIBSection(HBITMAP)

PARAMETERS:
HBITMAP - a DIBSection for which to get the BITMAPINFO

RETURNS:
LPBITMAPINFO - a pointer to a newly allocated memory block containing
the BITMAPINFO structure which describes the DIBSection.

REMARKS:
Allocates and returns a pointer to a BITMAPINFO for the DIBSection.
The returned BITMAPINFO contains a color table, even if the 
bit-depth does not require one - this way the caller can override
the optional color table if need be.
The caller is responsible for de-allocating the memory block using
the CRT function free().

********************************************************************** */

LPBITMAPINFO DSGetBITMAPINFOForDIBSection(HBITMAP hBitmap)
{
  DIBSECTION      ds;
  GetObject(hBitmap, sizeof(DIBSECTION), &ds);

  // load the header and the bitmasks if present
  // per function comments above, we allocate space for a color 
  // table even if it is not needed 
  LPRGBQUAD prgb = NULL;
  LPBITMAPINFO pbmi = NULL;
  if(ds.dsBmih.biCompression == BI_BITFIELDS)
  {   // has a bitmask - be sure to allocate for and copy them
    pbmi = (LPBITMAPINFO)malloc(sizeof(BITMAPINFOHEADER) + (3*sizeof(DWORD)) + (256*sizeof(RGBQUAD)));
    if ( !pbmi )
    {
      return (LPBITMAPINFO)NULL;
    }
    CopyMemory(&(pbmi->bmiHeader), &(ds.dsBmih), sizeof(BITMAPINFOHEADER) + (3*sizeof(DWORD)));
    prgb = (LPRGBQUAD)&(pbmi->bmiColors[3]);
  }
  else
  {   // no bitmask - just the header and color table
    pbmi = (LPBITMAPINFO)malloc(sizeof(BITMAPINFOHEADER) + (256*sizeof(RGBQUAD)));
    if ( !pbmi )
    {
      return (LPBITMAPINFO)NULL;
    }
    CopyMemory(&(pbmi->bmiHeader), &(ds.dsBmih), sizeof(BITMAPINFOHEADER));
    prgb = pbmi->bmiColors;
  }

  // at this point, prgb points to the color table, even 
  // if bitmasks are present

  // Now for the color table
  if((ds.dsBm.bmBitsPixel * ds.dsBm.bmPlanes) <= 8)
  {
    // the DIBSection is 256 color or less (has color table)
    HDC hMemDC = CreateCompatibleDC(NULL);
    HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);
    GetDIBColorTable(hMemDC, 0, 1<<(ds.dsBm.bmBitsPixel*ds.dsBm.bmPlanes), prgb);
    SelectObject(hMemDC, hOldBitmap);
    DeleteDC(hMemDC);
  }
  else
  {
    // the DIBSection is >8bpp (has no color table) so create a simple (grayscale) color table
    const int PALETTE_SIZE=256;
    for(int i=0;i<PALETTE_SIZE;i++)
    {
      prgb[i].rgbRed = i;
      prgb[i].rgbGreen = i;
      prgb[i].rgbBlue = i;
      prgb[i].rgbReserved = 0;
    }
    pbmi->bmiHeader.biClrUsed = PALETTE_SIZE;
  }

  return pbmi;
}

#define BYTESPERLINE(Width, BPP) ((WORD)((((DWORD)(Width) * (DWORD)(BPP) + 31) >> 5)) << 2)

//----------------------------------------------------------------------------
unsigned char* TelemedUltrasound::CaptureFrame()
{
  if (m_data_view == NULL || m_mixer_control == NULL)
  {
    return NULL;
  }

  /*
  {
    // repaint ultrasound (important if we are in freeze)
    HDC hdc = ::GetDC(m_usg_wnd);
    m_mixer_control->Repaint((LONG)m_usg_wnd,(LONG)hdc);
    ::ReleaseDC(m_usg_wnd,hdc);
  }
  */

  HBITMAP bmp = NULL;
  m_mixer_control->GetCurrentBitmap((LONG*)&bmp);
  if (bmp == NULL)
  {
    return NULL;
  }

  int h = 0;
  int pitch = 0;
  {
    LPBITMAPINFO pbmi = DSGetBITMAPINFOForDIBSection(bmp);
    if (pbmi == NULL)
    {
      ::DeleteObject(bmp);
      return NULL;
    }
    h = abs(pbmi->bmiHeader.biHeight);
    pitch = BYTESPERLINE(abs(pbmi->bmiHeader.biWidth), pbmi->bmiHeader.biBitCount);
    m_FrameSize[0] = abs(pbmi->bmiHeader.biWidth);
    m_FrameSize[1] = abs(pbmi->bmiHeader.biHeight);
    free(pbmi);
  }

  int cbBuffer = pitch * h;
  if (m_FrameBuffer.size()!=cbBuffer)
  {
    m_FrameBuffer.resize(cbBuffer);
  }
  unsigned char *pBuffer = NULL;
  if (!m_FrameBuffer.empty())
  {
    pBuffer = &(m_FrameBuffer[0]);
  }
  if (pBuffer == NULL)
  {
    ::DeleteObject(bmp);
    return NULL;
  }

  // get a pointer which points to the image bits for a DIBSection
  DIBSECTION ds;
  if (!GetObject(bmp, sizeof(DIBSECTION), &ds))
  {
    LOG_ERROR("Failed to get pointer to bitmap");
    ::DeleteObject(bmp);
    return NULL;
  }
  LPBYTE src = (LPBYTE)ds.dsBm.bmBits;

  memcpy(pBuffer, src, pitch * h);
  
  ::DeleteObject(bmp);

  return pBuffer;
}

//----------------------------------------------------------------------------
void TelemedUltrasound::CreateUsgControl( IUsgDataView* data_view, const IID& type_id, ULONG scan_mode, ULONG stream_id, void** ctrl )
{
  IUsgControl* ctrl2 = NULL;
  if (data_view == NULL)
  {
    return;
  }
  data_view->GetControlObj( &type_id, scan_mode, stream_id, &ctrl2 );
  if (ctrl2 != NULL)
  {
    HRESULT hr = ctrl2->QueryInterface( type_id, (void**)ctrl );
    if (hr != S_OK)
    {
      *ctrl = NULL;
    }
    SAFE_RELEASE(ctrl2);
  }
}

//----------------------------------------------------------------------------
void TelemedUltrasound::CreateUsgControls()
{
  HRESULT hr = S_OK;
  IUnknown* tmp_obj = NULL;

  do
  {
    CoInitialize(NULL);

    IConnectionPointContainer* cpc = NULL;

    // create main Usgfw2 library object only once and do not destroy till software closing;
    // it is required in order to receive probe/beamformer arrive/remove events
    if (m_usgfw2 == NULL)
    {
      // create main Usgfw2 library object
      hr = CoCreateInstance(CLSID_Usgfw2, NULL, CLSCTX_INPROC_SERVER, IID_IUsgfw2,(LPVOID*) &m_usgfw2);
      if (hr != S_OK)
      {
        m_usgfw2 = NULL;
        break;
      }

      // get connection points container
      hr = m_usgfw2->QueryInterface(IID_IConnectionPointContainer, (void**)&cpc);
      if (hr != S_OK)
        cpc = NULL;

      // find connection point
      if (cpc != NULL)
        hr = cpc->FindConnectionPoint(IID_IUsgDeviceChangeSink, &m_usg_device_change_cpnt);

      if (hr != S_OK)
      {
        m_usg_device_change_cpnt = NULL;
        m_usg_device_change_cpnt_cookie = 0;
      }
      SAFE_RELEASE(cpc);

      // attach
      if (m_usg_device_change_cpnt != NULL)
      {
        hr = m_usg_device_change_cpnt->Advise((IUnknown*)((IUsgDeviceChangeSink*)this), &m_usg_device_change_cpnt_cookie);
      }

    }

    IUsgCollection* probes_collection = NULL;

    // get collection of connected probes
    tmp_obj = NULL;
    m_usgfw2->get_ProbesCollection(&tmp_obj);
    if (tmp_obj == NULL)
    {
      probes_collection = NULL;
      break;
    }
    hr = tmp_obj->QueryInterface(IID_IUsgCollection,(void**)&probes_collection);
    tmp_obj->Release();
    if ( (hr != S_OK) || (probes_collection == NULL) )
    {
      probes_collection = NULL;
      break;
    }

    // get the number of conected probes
    LONG probes_count = 0;
    probes_collection->get_Count(&probes_count);
    if (probes_count == 0)
    {
      probes_collection->Release();
      probes_collection = NULL;
      break;
    }

    // get first available probe
    tmp_obj = NULL;
    probes_collection->Item(0,&tmp_obj);
    probes_collection->Release();
    probes_collection = NULL;
    if (tmp_obj == NULL)
    {
      m_probe = NULL;
      break;
    }
    hr = tmp_obj->QueryInterface(IID_IProbe,(void**)&m_probe);
    tmp_obj->Release();
    if ( (hr != S_OK) || (m_probe == NULL) )
    {
      m_probe = NULL;
      break;
    }

    // create main ultrasound scanning object for selected probe
    m_usgfw2->CreateDataView(m_probe, &m_data_view);
    if (m_data_view == NULL)
    {
      break;
    }

    m_data_view->put_ScanState(SCAN_STATE_STOP);

    IUsgScanMode* mode = NULL;
    m_data_view->GetScanModeObj(SCAN_MODE_B,&mode);
    if (mode == NULL)
    {
      break;
    }

    // get mixer control
    mode->GetMixerControl(SCAN_MODE_B,0,&m_mixer_control);
    mode->Release();
    mode = NULL;
    if (m_mixer_control == NULL)
    {
      break;
    }

    // set B scanning mode
    m_data_view->put_ScanMode(SCAN_MODE_B);

    // set ultrasound output window (panel)
    m_mixer_control->SetOutputWindow((LONG)this->ImageWindowHandle);

    tagRECT rect1;
    rect1.left = 0;
    rect1.top = 0;
    rect1.right = rect1.left + m_MaximumFrameSize[0];
    rect1.bottom = rect1.top + m_MaximumFrameSize[1];

    // set ultrasound output rectangle
    m_mixer_control->SetOutputRect(&rect1);

    // set background color that surrounds ultrasound image to blue
    tagPALETTEENTRY clr1;
    clr1.peRed		= 0;
    clr1.peGreen	= 0;
    clr1.peBlue		= 255;
    clr1.peFlags	= 0;
    m_mixer_control->put_BkColor(clr1);

    // create depth control
    tmp_obj = NULL;
    CreateUsgControl( m_data_view, IID_IUsgDepth, SCAN_MODE_B, 0, (void**)&tmp_obj );
    if (tmp_obj != NULL)
      m_depth_ctrl = (IUsgDepth*)tmp_obj;
    else
      m_depth_ctrl = NULL;

    // create power control
    tmp_obj = NULL;
    CreateUsgControl( m_data_view, IID_IUsgPower, SCAN_MODE_B, 0, (void**)&tmp_obj );
    if (tmp_obj != NULL)
      m_b_power_ctrl = (IUsgPower*)tmp_obj;
    else
      m_b_power_ctrl = NULL;

    // create B mode gain control
    tmp_obj = NULL;
    CreateUsgControl( m_data_view, IID_IUsgGain, SCAN_MODE_B, 0, (void**)&tmp_obj );
    if (tmp_obj != NULL)
      m_b_gain_ctrl = (IUsgGain*)tmp_obj;
    else
      m_b_gain_ctrl = NULL;

    // create B mode dynamic range control
    tmp_obj = NULL;
    CreateUsgControl( m_data_view, IID_IUsgDynamicRange, SCAN_MODE_B, 0, (void**)&tmp_obj );
    if (tmp_obj != NULL)
      m_b_dynrange_ctrl = (IUsgDynamicRange*)tmp_obj;
    else
      m_b_dynrange_ctrl = NULL;

    // create B mode frequency control
    tmp_obj = NULL;
    CreateUsgControl( m_data_view, IID_IUsgProbeFrequency3, SCAN_MODE_B, 0, (void**)&tmp_obj );
    if (tmp_obj != NULL)
      m_b_frequency_ctrl = (IUsgProbeFrequency3*)tmp_obj;
    else
      m_b_frequency_ctrl = NULL;

    // attach to control value change connection point in order to be informed about changed values

    // get container of connection points
    hr = m_data_view->QueryInterface(IID_IConnectionPointContainer, (void**)&cpc);
    if (hr != S_OK)
      cpc = NULL;

    // find connection point
    if (cpc != NULL)
      hr = cpc->FindConnectionPoint(IID_IUsgCtrlChangeCommon, &m_usg_control_change_cpnt);

    if (hr != S_OK)
    {
      m_usg_control_change_cpnt = NULL;
      m_usg_control_change_cpnt_cookie = 0;
    }
    SAFE_RELEASE(cpc);

    // attach
    if (m_usg_control_change_cpnt != NULL)
      hr = m_usg_control_change_cpnt->Advise((IUnknown*)((IUsgCtrlChangeCommon*)this), &m_usg_control_change_cpnt_cookie);

    // start ultrasound scanning (Run)
    m_data_view->put_ScanState(SCAN_STATE_RUN);

    return;
  } while (false);

  ReleaseUsgControls(false);
} // void TelemedUltrasound::CreateUsgControls()

//----------------------------------------------------------------------------
// NOT IMPLEMENTED. MUST USE OnControlChangedBSTR.
HRESULT TelemedUltrasound::OnControlChanged(REFIID riidCtrl, ULONG scanMode, LONG streamId, IUsgControl *pControlObj, LONG dispId, LONG flags)
{
  return S_OK;
};
 
//----------------------------------------------------------------------------
std::wstring GuidToWstring(const GUID& guid)
{
  wchar_t guid_string[37];
  swprintf_s(
    guid_string, sizeof(guid_string) / sizeof(guid_string[0]),
    L"%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
    guid.Data1, guid.Data2, guid.Data3,
    guid.Data4[0], guid.Data4[1], guid.Data4[2],
    guid.Data4[3], guid.Data4[4], guid.Data4[5],
    guid.Data4[6], guid.Data4[7]);
  return guid_string;
}

//----------------------------------------------------------------------------
LPCWSTR TelemedUltrasound::GetInterfaceNameByGuid(BSTR ctrlGUID)
{
  std::wstring ctrlGUIDws(ctrlGUID, SysStringLen(ctrlGUID));
  
  if(ctrlGUIDws.compare(GuidToWstring(IID_IUsgPower)) == 0) return L"IUsgPower";
  if(ctrlGUIDws.compare(GuidToWstring(IID_IUsgGain)) == 0) return L"IUsgGain";
  if(ctrlGUIDws.compare(GuidToWstring(IID_IUsgDepth)) == 0) return L"IUsgDepth";
  if(ctrlGUIDws.compare(GuidToWstring(IID_IUsgRejection2)) == 0) return L"IUsgRejection2";
  if(ctrlGUIDws.compare(GuidToWstring(IID_IUsgFocus)) == 0) return L"IUsgFocus";
  if(ctrlGUIDws.compare(GuidToWstring(IID_IUsgDynamicRange)) == 0) return L"IUsgDynamicRange";
  if(ctrlGUIDws.compare(GuidToWstring(IID_IUsgProbeFrequency2)) == 0) return L"IUsgProbeFrequency2";
  if(ctrlGUIDws.compare(GuidToWstring(IID_IUsgProbeFrequency3)) == 0) return L"IUsgProbeFrequency3";
  if(ctrlGUIDws.compare(GuidToWstring(IID_IUsgViewArea)) == 0) return L"IUsgViewArea";
  if(ctrlGUIDws.compare(GuidToWstring(IID_IUsgImageEnhancement)) == 0) return L"IUsgImageEnhancement";
  if(ctrlGUIDws.compare(GuidToWstring(IID_IUsgClearView)) == 0) return L"IUsgClearView";
  if(ctrlGUIDws.compare(GuidToWstring(IID_IUsgFrameAvg)) == 0) return L"IUsgFrameAvg";
  if(ctrlGUIDws.compare(GuidToWstring(IID_IUsgImageOrientation)) == 0) return L"IUsgImageOrientation";
  if(ctrlGUIDws.compare(GuidToWstring(IID_IUsgLineDensity)) == 0) return L"IUsgLineDensity";
  if(ctrlGUIDws.compare(GuidToWstring(IID_IUsgPalette)) == 0) return L"IUsgPalette";
  if(ctrlGUIDws.compare(GuidToWstring(IID_IUsgTgc)) == 0) return L"IUsgTgc";
  if(ctrlGUIDws.compare(GuidToWstring(IID_IUsgImageProperties)) == 0) return L"IUsgImageProperties";
  if(ctrlGUIDws.compare(GuidToWstring(IID_IUsgMixerControl)) == 0) return L"IUsgMixerControl";
  if(ctrlGUIDws.compare(GuidToWstring(IID_IUsgScanLineSelector)) == 0) return L"IUsgScanLineSelector";
  if(ctrlGUIDws.compare(GuidToWstring(IID_IUsgSweepSpeed)) == 0) return L"IUsgSweepSpeed";
  if(ctrlGUIDws.compare(GuidToWstring(IID_IUsg3dVolumeDensity)) == 0) return L"IUsg3dVolumeDensity";
  if(ctrlGUIDws.compare(GuidToWstring(IID_IUsg3dVolumeSize)) == 0) return L"IUsg3dVolumeSize";
  if(ctrlGUIDws.compare(GuidToWstring(IID_IUsgSweepMode)) == 0) return L"IUsgSweepMode";
  if(ctrlGUIDws.compare(GuidToWstring(IID_IUsgScanLineProperties)) == 0) return L"IUsgScanLineProperties";
  if(ctrlGUIDws.compare(GuidToWstring(IID_IUsgDopplerPRF)) == 0) return L"IUsgDopplerPRF";
  if(ctrlGUIDws.compare(GuidToWstring(IID_IUsgBioeffectsIndices)) == 0) return L"IUsgBioeffectsIndices";
  if(ctrlGUIDws.compare(GuidToWstring(IID_IUsgDopplerColorPriority)) == 0) return L"IUsgDopplerColorPriority";
  if(ctrlGUIDws.compare(GuidToWstring(IID_IUsgDopplerSignalScale)) == 0) return L"IUsgDopplerSignalScale";
  if(ctrlGUIDws.compare(GuidToWstring(IID_IUsgDopplerSignalSmooth)) == 0) return L"IUsgDopplerSignalSmooth";
  if(ctrlGUIDws.compare(GuidToWstring(IID_IUsgDopplerWindow)) == 0) return L"IUsgDopplerWindow";
  if(ctrlGUIDws.compare(GuidToWstring(IID_IUsgDopplerSampleVolume)) == 0) return L"IUsgDopplerSampleVolume";
  if(ctrlGUIDws.compare(GuidToWstring(IID_IUsgDopplerWallFilter)) == 0) return L"IUsgDopplerWallFilter";
  if(ctrlGUIDws.compare(GuidToWstring(IID_IUsgDopplerSteerAngle)) == 0) return L"IUsgDopplerSteerAngle";
  if(ctrlGUIDws.compare(GuidToWstring(IID_IUsgDopplerCorrectionAngle)) == 0) return L"IUsgDopplerCorrectionAngle";
  if(ctrlGUIDws.compare(GuidToWstring(IID_IUsgDopplerColorMap)) == 0) return L"IUsgDopplerColorMap";
  if(ctrlGUIDws.compare(GuidToWstring(IID_IUsgDopplerColorThreshold)) == 0) return L"IUsgDopplerColorThreshold";
  if(ctrlGUIDws.compare(GuidToWstring(IID_IUsgDopplerBaseLine)) == 0) return L"IUsgDopplerBaseLine";
  if(ctrlGUIDws.compare(GuidToWstring(IID_IUsgDopplerPacketSize)) == 0) return L"IUsgDopplerPacketSize";
  if(ctrlGUIDws.compare(GuidToWstring(IID_IUsgPulsesNumber)) == 0) return L"IUsgPulsesNumber";
  if(ctrlGUIDws.compare(GuidToWstring(IID_IUsgCineStream)) == 0) return L"IUsgCineStream";
  if(ctrlGUIDws.compare(GuidToWstring(IID_IUsgCineSink)) == 0) return L"IUsgCineSink";
  if(ctrlGUIDws.compare(GuidToWstring(IID_IUsgSpatialFilter)) == 0) return L"IUsgSpatialFilter";
  if(ctrlGUIDws.compare(GuidToWstring(IID_IUsgDopplerColorTransparency)) == 0) return L"IUsgDopplerColorTransparency";
  if(ctrlGUIDws.compare(GuidToWstring(IID_IUsgAudioVolume)) == 0) return L"IUsgAudioVolume";
  if(ctrlGUIDws.compare(GuidToWstring(IID_IUsgDopplerSpectralAvg)) == 0) return L"IUsgDopplerSpectralAvg";
  if(ctrlGUIDws.compare(GuidToWstring(IID_IUsgCompoundFrames)) == 0) return L"IUsgCompoundFrames";
  if(ctrlGUIDws.compare(GuidToWstring(IID_IUsgCompoundAngle)) == 0) return L"IUsgCompoundAngle";
  if(ctrlGUIDws.compare(GuidToWstring(IID_IUsgScanType)) == 0) return L"IUsgScanType";
  if(ctrlGUIDws.compare(GuidToWstring(IID_IUsgSteeringAngle)) == 0) return L"IUsgSteeringAngle";
  if(ctrlGUIDws.compare(GuidToWstring(IID_IUsgViewAngle)) == 0) return L"IUsgViewAngle";
  return L"";
}

//----------------------------------------------------------------------------
LPCWSTR TelemedUltrasound::GetModeNameById(LONG scanMode)
{
  if (scanMode == SCAN_MODE_UNKNOWN) return L"UNKNOWN";
  if (scanMode == SCAN_MODE_B) return L"B";
  if (scanMode == SCAN_MODE_M) return L"M";
  if (scanMode == SCAN_MODE_A) return L"A";
  if (scanMode == SCAN_MODE_PW) return L"PW";
  if (scanMode == SCAN_MODE_BM) return L"BM";
  if (scanMode == SCAN_MODE_BA) return L"BA";
  if (scanMode == SCAN_MODE_BPW) return L"BPW";
  if (scanMode == SCAN_MODE_CFM) return L"CFM";
  if (scanMode == SCAN_MODE_BCFM) return L"BCFM";
  if (scanMode == SCAN_MODE_BCFM_PW) return L"BCFM_PW";
  if (scanMode == SCAN_MODE_PDI) return L"PDI";
  if (scanMode == SCAN_MODE_BPDI) return L"BPDI";
  if (scanMode == SCAN_MODE_BPDI_PW) return L"BPDI_PW";
  if (scanMode == SCAN_MODE_DPDI) return L"DPDI";
  if (scanMode == SCAN_MODE_BDPDI) return L"BDPDI";
  if (scanMode == SCAN_MODE_BDPDI_PW) return L"BDPDI_PW";
  if (scanMode == SCAN_MODE_B2) return L"B2";
  if (scanMode == SCAN_MODE_PWAUDIO) return L"PWAUDIO";
  if (scanMode == SCAN_MODE_3D) return L"3D";
  if (scanMode == SCAN_MODES_CUSTOM) return L"CUSTOM";
  return L"";

} // std::string TelemedUltrasound::GetModeNameById

//----------------------------------------------------------------------------
// if scanning parameters are changed - update user interface controls
HRESULT TelemedUltrasound::OnControlChangedBSTR(BSTR ctrlGUID, LONG scanMode, LONG streamId, IUsgControl *pControlObject, LONG dispId, LONG flags)
{
  std::wstring ctrlGUIDws(ctrlGUID, SysStringLen(ctrlGUID));

  if(ctrlGUIDws.compare(GuidToWstring(IID_IUsgDepth))==0)
  {
    //DepthUpdateGUI();
  }
  else if(ctrlGUIDws.compare(GuidToWstring(IID_IUsgGain))==0)
  {
    //B_GainUpdateGUI();
  }
  else if(ctrlGUIDws.compare(GuidToWstring(IID_IUsgProbeFrequency3))==0)
  {
    //B_FrequencyUpdateGUI();
  }

  std::wstring interfaceName = GetInterfaceNameByGuid(ctrlGUID);

  if (!interfaceName.empty())
  {
    //str.Format(L"OnControlChangedBSTR; %s; scanMode=%s; streamId=%d; dispId=%d; flags=%d;", str1, GetModeNameById(scanMode), streamId, dispId, flags);
    //WriteToLog(str);
  }
  else
  {
    //str.Format(L"OnControlChangedBSTR; ctrlGUID=%s; scanMode=%s; streamId=%d; dispId=%d; flags=%d;", CComBSTR(ctrlGUID), GetModeNameById(scanMode), streamId, dispId, flags);
    //WriteToLog(str);
  }

  return S_OK;

} // HRESULT TelemedUltrasound::OnControlChangedBSTR

// begin: IUnknown

//----------------------------------------------------------------------------
STDMETHODIMP_(ULONG) TelemedUltrasound::AddRef()
{
  ++m_refCount;
  return m_refCount;
}

//----------------------------------------------------------------------------
STDMETHODIMP_(ULONG) TelemedUltrasound::Release()
{
  --m_refCount;
  return m_refCount;
}

//----------------------------------------------------------------------------
STDMETHODIMP TelemedUltrasound::QueryInterface(REFIID riid, void** ppv)
{
  if (riid == IID_IUnknown || riid == IID_IUsgCtrlChangeCommon)
  {
    *ppv = (IUsgCtrlChangeCommon*)this;
    return S_OK;
  }
  else if (riid == IID_IUsgDeviceChangeSink)
  {
    *ppv = (IUsgDeviceChangeSink*)this;
    return S_OK;
  }
  else if (riid == IID_IDispatch)
  {
    *ppv = (IDispatch*)this;
    return S_OK;
  }
  else
    return E_NOINTERFACE;
}

// end: IUnknown


// begin: IDispatch

//----------------------------------------------------------------------------
HRESULT TelemedUltrasound::GetTypeInfoCount(UINT *pctinfo)
{
  if (pctinfo == NULL) return E_INVALIDARG;
  *pctinfo = 0;
  return S_OK;
}

//----------------------------------------------------------------------------
HRESULT TelemedUltrasound::GetTypeInfo(UINT itinfo, LCID lcid, ITypeInfo** pptinfo)
{
  if (pptinfo == NULL) return E_INVALIDARG;
  *pptinfo = NULL;
  if (itinfo != 0) return DISP_E_BADINDEX;
  return S_OK;
}

//----------------------------------------------------------------------------
HRESULT TelemedUltrasound::GetIDsOfNames(const IID &riid, LPOLESTR* rgszNames, UINT cNames, LCID lcid, DISPID* rgdispid)
{
  // this is not used - must use the same fixed dispid's from Usgfw2 idl file
  return S_OK;
}

//----------------------------------------------------------------------------
HRESULT TelemedUltrasound::Invoke(DISPID dispIdMember, const IID &riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr)
{
  // need 2 arguments for device messages
  if ( (dispIdMember >= 1) && (dispIdMember <= 6) && (pDispParams->cArgs == 2) )
  {
    IUnknown *unkn = NULL;
    ULONG *res = NULL;

    VARIANTARG* p1;
    VARIANTARG* p;
    p1 = pDispParams->rgvarg;

    p = p1;
    if (p->vt == (VT_BYREF|VT_UI4))
      res = p->pulVal;
    p1++;

    p = p1;
    if (p->vt == VT_UNKNOWN)
      unkn = (IUnknown*)(p->punkVal);

    if (dispIdMember == 1)
      OnProbeArrive(unkn, res);
    else if (dispIdMember == 2)
      OnBeamformerArrive(unkn, res);
    else if (dispIdMember == 3)
      OnProbeRemove(unkn, res);
    else if (dispIdMember == 4)
      OnBeamformerRemove(unkn, res);
    else if (dispIdMember == 5)
      OnProbeStateChanged(unkn, res);
    else if (dispIdMember == 6)
      OnBeamformerStateChanged(unkn, res);
  }

  // need 6 arguments for control messages
  else if ( (dispIdMember == 2) && (pDispParams->cArgs == 6) )
  {
    BSTR ctrlGUID = NULL;
    LONG scanMode = 0;
    LONG streamId = 0;
    IUsgControl *pControlObject = NULL;
    LONG dispId = 0;
    LONG flags = 0;

    VARIANTARG* p1;
    VARIANTARG* p;
    p1 = pDispParams->rgvarg;

    p = p1;
    flags = p->lVal;
    p1++;

    p = p1;
    dispId = p->lVal;
    p1++;

    p = p1;
    pControlObject = (IUsgControl*)(p->pdispVal);
    p1++;

    p = p1;
    streamId = p->lVal;
    p1++;

    p = p1;
    scanMode = p->lVal;
    p1++;

    p = p1;
    ctrlGUID = p->bstrVal;
    p1++;

    OnControlChangedBSTR(ctrlGUID,scanMode,streamId,pControlObject,dispId,flags);
  }

  return S_OK;

} // HRESULT TelemedUltrasound::Invoke

// end: IDispatch

//----------------------------------------------------------------------------
void TelemedUltrasound::ReleaseUsgControls(bool release_usgfw2)
{
  if (m_data_view != NULL)
  {
    m_data_view->put_ScanState(SCAN_STATE_STOP);
  }
  if (m_usg_control_change_cpnt != NULL)
  {
    m_usg_control_change_cpnt->Unadvise(m_usg_control_change_cpnt_cookie);
    m_usg_control_change_cpnt_cookie = 0;
    SAFE_RELEASE(m_usg_control_change_cpnt);
  }
  SAFE_RELEASE(m_depth_ctrl);
  SAFE_RELEASE(m_b_power_ctrl);
  SAFE_RELEASE(m_b_gain_ctrl);
  SAFE_RELEASE(m_b_dynrange_ctrl);
  SAFE_RELEASE(m_b_frequency_ctrl);
  SAFE_RELEASE(m_mixer_control);
  SAFE_RELEASE(m_data_view);
  SAFE_RELEASE(m_probe);

  if (release_usgfw2)
  {
    if (m_usg_device_change_cpnt != NULL)
    {
      m_usg_device_change_cpnt->Unadvise(m_usg_device_change_cpnt_cookie);
      m_usg_device_change_cpnt_cookie = 0;
      SAFE_RELEASE(m_usg_device_change_cpnt);
    }
    SAFE_RELEASE(m_usgfw2);
  }

}

//----------------------------------------------------------------------------
PlusStatus TelemedUltrasound::GetDepthMm(double &depthMm)
{
  if (m_depth_ctrl == NULL)
  {
    LOG_ERROR("TelemedUltrasound::GetDepthMm failed: not connected to device");
    return PLUS_FAIL;
  }
  LONG val;
  if (m_depth_ctrl->get_Current(&val) != S_OK)
  {
    LOG_ERROR("TelemedUltrasound::GetDepthMm failed: failed to retrieve parameter value");
    return PLUS_FAIL;
  }
  depthMm = val;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus TelemedUltrasound::SetDepthMm(double depthMm)
{
  if (m_depth_ctrl == NULL)
  {
    LOG_ERROR("TelemedUltrasound::SetDepthMm failed: not connected to hardware interface");
    return PLUS_FAIL;
  }

  LONG currentDepthMm=0;
  if (m_depth_ctrl->get_Current(&currentDepthMm) != S_OK)
  {
    LOG_ERROR("TelemedUltrasound::SetDepthMm failed: failed to get value from device");
    return PLUS_FAIL;
  }

  if (fabs(depthMm-currentDepthMm)>0.1)
  {
    currentDepthMm = depthMm;
    m_depth_ctrl->put_Current(currentDepthMm);
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus TelemedUltrasound::GetFrequencyMhz(double &freqMhz)
{
  if (m_b_frequency_ctrl == NULL)
  {
    LOG_ERROR("TelemedUltrasound::GetFrequencyMHz failed: not connected to device");
    return PLUS_FAIL;
  }
  FREQUENCY3 val;
  if (m_b_frequency_ctrl->get_Current(&val) != S_OK)
  {
    LOG_ERROR("TelemedUltrasound::GetFrequencyMHz failed: failed to retrieve parameter value");
    return PLUS_FAIL;
  }
  freqMhz = (double)(val.nFrequency)/1000000.0;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus TelemedUltrasound::SetFrequencyMhz(double freqMhz)
{
  if (m_b_frequency_ctrl == NULL)
  {
    LOG_ERROR("TelemedUltrasound::SetFrequencyMHz failed: not connected to device");
    return PLUS_FAIL;
  }

  FREQUENCY3 val_cur;
  m_b_frequency_ctrl->get_Current(&val_cur);
  
  int val_requested_nFrequency = freqMhz*1000000.0;
  if (val_cur.nFrequency==val_requested_nFrequency)
  {
    return PLUS_SUCCESS;
  }

  IUsgValues* usg_values = NULL;
  m_b_frequency_ctrl->get_Values(&usg_values);
  if (usg_values == NULL)
  {
    LOG_ERROR("TelemedUltrasound::SetFrequencyMHz failed: failed to retrieve valid parameter values");
    return PLUS_FAIL;
  }

  // Find the closest item
  LONG val_closest_index = -1;
  LONG val_closest_nFrequency = 0;
  LONG val_count = 0;
  usg_values->get_Count(&val_count);
  for (LONG i1=0; i1<val_count; i1++)
  {
    VARIANT item;
    usg_values->Item(i1, &item);
    if (item.vt == VT_RECORD)
    {
      FREQUENCY3 val;
      val = *((FREQUENCY3*)item.pvRecord);
      if ((val_cur.nThiMode == val.nThiMode) &&
        (val_closest_index<0 || abs(val.nFrequency-val_requested_nFrequency)<abs(val_closest_nFrequency-val_requested_nFrequency)))
      {
        // found a value that is more similar to the requested one
        val_closest_index = i1;
        val_closest_nFrequency = val.nFrequency;
      }
    }
    VariantClear(&item);
  }

  // Set the closest item
  PlusStatus status=PLUS_SUCCESS;
  if (val_closest_index >= 0)
  {
    VARIANT item;
    usg_values->Item( val_closest_index, &item );
    if (item.vt == VT_RECORD)
    {
      val_cur = *((FREQUENCY3*)item.pvRecord);
    }
    m_b_frequency_ctrl->put_Current(val_cur);
    VariantClear(&item);
  }
  else
  {
    LOG_ERROR("TelemedUltrasound::SetFrequencyMHz failed: failed to find a suitable frequency setting");
    status=PLUS_FAIL;
  }

  SAFE_RELEASE(usg_values);
  return status;
}

//----------------------------------------------------------------------------
PlusStatus TelemedUltrasound::SetGainPercent(double gainPercent)
{
  if (m_b_gain_ctrl == NULL)
  {
    LOG_ERROR("TelemedUltrasound::GainPercent failed: not connected to hardware interface");
    return PLUS_FAIL;
  }
  LONG currentGainPercent=0;
  m_b_gain_ctrl->get_Current(&currentGainPercent);
  if (fabs(gainPercent-currentGainPercent)>0.1)
  {
    currentGainPercent = gainPercent;
    m_b_gain_ctrl->put_Current(currentGainPercent);
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus TelemedUltrasound::GetGainPercent(double &gainPercent)
{
  if (m_b_gain_ctrl == NULL)
  {
    LOG_ERROR("TelemedUltrasound::GainPercent failed: not connected to hardware interface");
    return PLUS_FAIL;
  }
  LONG currentGainPercent=0;
  if (m_depth_ctrl->get_Current(&currentGainPercent) != S_OK)
  {
    LOG_ERROR("TelemedUltrasound::GainPercent failed: failed to get value from device");
    return PLUS_FAIL;
  }
  gainPercent = currentGainPercent;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus TelemedUltrasound::SetPowerPercent(double powerPercent)
{
  if (m_b_power_ctrl == NULL)
  {
    LOG_ERROR("TelemedUltrasound::PowerPercent failed: not connected to hardware interface");
    return PLUS_FAIL;
  }
  LONG currentPowerPercent=0;
  m_b_power_ctrl->get_Current(&currentPowerPercent);
  if (fabs(powerPercent-currentPowerPercent)>0.1)
  {
    currentPowerPercent = powerPercent;
    m_b_power_ctrl->put_Current(currentPowerPercent);
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus TelemedUltrasound::GetPowerPercent(double &powerPercent)
{
  if (m_b_power_ctrl == NULL)
  {
    LOG_ERROR("TelemedUltrasound::PowerPercent failed: not connected to hardware interface");
    return PLUS_FAIL;
  }
  LONG currentPowerPercent=0;
  if (m_b_power_ctrl->get_Current(&currentPowerPercent) != S_OK)
  {
    LOG_ERROR("TelemedUltrasound::PowerPercent failed: failed to get value from device");
    return PLUS_FAIL;
  }
  powerPercent = currentPowerPercent;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus TelemedUltrasound::SetDynRangeDb(double dynRangeDb)
{
  if (m_b_power_ctrl == NULL)
  {
    LOG_ERROR("TelemedUltrasound::DynRangeDb failed: not connected to hardware interface");
    return PLUS_FAIL;
  }
  LONG currentDynRangeDb=0;
  m_b_dynrange_ctrl->get_Current(&currentDynRangeDb);
  if (fabs(dynRangeDb-currentDynRangeDb)>0.1)
  {
    currentDynRangeDb = dynRangeDb;
    m_b_dynrange_ctrl->put_Current(currentDynRangeDb);
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus TelemedUltrasound::GetDynRangeDb(double &dynRangeDb)
{
  if (m_b_power_ctrl == NULL)
  {
    LOG_ERROR("TelemedUltrasound::DynRangeDb failed: not connected to hardware interface");
    return PLUS_FAIL;
  }
  LONG currentDynRangeDb=0;
  if (m_b_dynrange_ctrl->get_Current(&currentDynRangeDb) != S_OK)
  {
    LOG_ERROR("TelemedUltrasound::DynRangeDb failed: failed to get value from device");
    return PLUS_FAIL;
  }
  dynRangeDb = currentDynRangeDb;
  return PLUS_SUCCESS;
}

// NOTE.
// Here we assume that is used one beamformer with one probe connector.
// If you plan to use beamformer with several probe connectors or several beamformers
// connected at the same time, you will need to store somewhere what probe is active and
// take this into account when arrive messages about probe arrive/remove.
// In most cases you need to do initialization when you do not have active probe and arrives
// new probe. And do deinitialization when is disconnected active probe.
// But if you perform scanning with one probe, and to second connector is
// connected/disconnected another probe, then most likely you do not need to stop scanning or
// reinitialize anything.

//----------------------------------------------------------------------------
HRESULT TelemedUltrasound::OnProbeArrive(IUnknown *pUsgProbe, ULONG *reserved)
{
  std::string str;
  // *reserved - index in Usgfw2 ProbesCollection
  //str.Format(L"OnProbeArrive; idx=%d;", *reserved);
  //WriteToLog(str);

  CreateUsgControls();

  return S_OK;
}

//----------------------------------------------------------------------------
HRESULT TelemedUltrasound::OnProbeRemove(IUnknown *pUsgProbe, ULONG *reserved)
{
  std::string str;
  // *reserved - index in Usgfw2 ProbesCollection
  //str.Format(L"OnProbeRemove; idx=%d;", *reserved);
  //WriteToLog(str);
  ReleaseUsgControls(false);
  return S_OK;
}

//----------------------------------------------------------------------------
HRESULT TelemedUltrasound::OnBeamformerRemove(IUnknown *pUsgBeamformer, ULONG *reserved)
{
  std::string str;
  // *reserved - index in Usgfw2 BeamformersCollection
  //str.Format(L"OnBeamformerRemove; idx=%d;", *reserved);
  //WriteToLog(str);
  return S_OK;
}

//----------------------------------------------------------------------------
HRESULT TelemedUltrasound::OnBeamformerArrive(IUnknown *pUsgBeamformer, ULONG *reserved)
{
  std::string str;
  // *reserved - index in Usgfw2 BeamformersCollection
  //str.Format(L"OnBeamformerArrive; idx=%d;", *reserved);
  //WriteToLog(str);
  return S_OK;
}

//----------------------------------------------------------------------------
HRESULT TelemedUltrasound::OnProbeStateChanged(IUnknown *pUsgProbe, ULONG *reserved)
{
  //WriteToLog(L"OnProbeStateChanged");
  return S_OK;
}

//----------------------------------------------------------------------------
HRESULT TelemedUltrasound::OnBeamformerStateChanged(IUnknown *pUsgBeamformer, ULONG *reserved)
{
  //WriteToLog(L"OnBeamformerStateChanged");
  return S_OK;
}
