/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkIntersonVideoSource.h"

#include "vtkImageData.h"
#include "vtkObjectFactory.h"

#include "vtkPlusChannel.h"
#include "vtkPlusDataSource.h"
#include "vtkPlusBuffer.h"

#include "BmodeDLL.h"
#include "usbProbeDLL_net.h"

#include "vtkUSImagingParameters.h"

#define XSIZE 800						// Image window size 
#define YSIZE 512					    //		XSIZE must be divisible by four!

// Interson's OEM ID (probes released by Interson have this OEM ID)
#define OEM_ID_INTERSON 0x00

vtkCxxRevisionMacro(vtkIntersonVideoSource, "$Revision: 1.0$");
vtkStandardNewMacro(vtkIntersonVideoSource);

class vtkIntersonVideoSource::vtkInternal
{
public:
  vtkIntersonVideoSource *External;

  HWND ImageWindowHandle;
  HBITMAP DataHandle;
  HANDLE ProbeHandle;    
  std::vector<unsigned char> MemoryBitmapBuffer;
  BITMAP Bitmap;
  bmBITMAPINFO	BitmapInfo;
  BYTE *RfDataBuffer;

  //----------------------------------------------------------------------------
  vtkIntersonVideoSource::vtkInternal::vtkInternal(vtkIntersonVideoSource* external) 
    : External(external)
    , RfDataBuffer(NULL)
  {

  }

  //----------------------------------------------------------------------------
  virtual vtkIntersonVideoSource::vtkInternal::~vtkInternal() 
  {    
    this->External = NULL;
  }  

  //----------------------------------------------------------------------------
  void vtkIntersonVideoSource::vtkInternal::CreateLinearTGC(int tgcMin, int tgcMax)
  {
    int tgc[2048]={0};
    int b = tgcMin; 
    float m = (float) (tgcMax - tgcMin) / 2048.0f;
    for (int x = 0; x < 2048; x++)
    {
      tgc[x] = (int) (m * (float) x) + b;
    }
    bmSetTGC(tgc);
  }

  //----------------------------------------------------------------------------
  static LRESULT CALLBACK vtkIntersonVideoSource::vtkInternal::ImageWindowProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
  {
    vtkIntersonVideoSource::vtkInternal* self=(vtkIntersonVideoSource::vtkInternal*)GetWindowLongPtr(hwnd, GWL_USERDATA);
    return DefWindowProc (hwnd, iMsg, wParam, lParam) ;
  }

  //----------------------------------------------------------------------------
  void CreateLUT(BYTE lut[], int Level, int Window)
  {
    int center = Window / 2;				// center of window
    int left = Level - center;				// left of window
    int right = Level + center;				// right of window

    // everything to our left is black
    for (int x=0; x<left; x++)
    {
      lut[x] = 0;		
    }

    // everything to our right is white
    for (int x=right+1; x < 256; x++)
    {
      lut[x] = 255;
    }

    // everything in between is on the line
    float m = 255.0f / ((float) Window);

    int startX=left;
    if (startX<0)
    {
      startX=0;
    }
    int endX=right;
    if (endX>255)
    {
      endX=255;
    }
    for (int x=startX; x <= endX; x++) 
    {
      int y = (int) (m * (float)(x-left) + 0.5f);
      if (y<0)
      {
        y=0;
      }
      else if (y>255)
      {
        y=255;
      }
      lut[x] = y;
    }
  }

};


//----------------------------------------------------------------------------
vtkIntersonVideoSource::vtkIntersonVideoSource()
: Interpolate(true)
, BidirectionalScan(false)
, Frozen(true)
{
  this->Internal = new vtkInternal(this);

  this->RequireImageOrientationInConfiguration = true;
  this->RequireFrameBufferSizeInDeviceSetConfiguration = true;
  this->RequireAcquisitionRateInDeviceSetConfiguration = false;
  this->RequireAveragedItemsForFilteringInDeviceSetConfiguration = false;
  this->RequireLocalTimeOffsetSecInDeviceSetConfiguration = false;
  this->RequireUsImageOrientationInDeviceSetConfiguration = true;
  this->RequireRfElementInDeviceSetConfiguration = false;

  this->ImagingParameters = new vtkUsImagingParameters(this);

  // No callback function provided by the device, so the data capture thread will be used to poll the hardware and add new items to the buffer
  this->StartThreadForInternalUpdates=true;
}

//----------------------------------------------------------------------------
vtkIntersonVideoSource::~vtkIntersonVideoSource()
{ 
  if( !this->Connected )
  {
    this->Disconnect();
  }

  delete this->Internal;
  this->Internal=NULL;
}

//----------------------------------------------------------------------------
void vtkIntersonVideoSource::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

}

//----------------------------------------------------------------------------
int CALLBACK ProbeAttached()
{
  LOG_INFO("Probe attached");
  return 0;
}

//----------------------------------------------------------------------------
int CALLBACK ProbeDetached()
{
  LOG_INFO("Probe detached");
  return 0;
}

//----------------------------------------------------------------------------
void InitializeDIB(bmBITMAPINFO *bmi)
{
  bmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  bmi->bmiHeader.biWidth = XSIZE;
  bmi->bmiHeader.biHeight = -YSIZE;
  bmi->bmiHeader.biPlanes = 1;
  bmi->bmiHeader.biBitCount = 8;
  bmi->bmiHeader.biCompression = 0;
  bmi->bmiHeader.biXPelsPerMeter = 0;
  bmi->bmiHeader.biYPelsPerMeter = 0;
  bmi->bmiHeader.biClrUsed = 0;
  bmi->bmiHeader.biClrImportant = 0;

  // Compute the number of bytes in the array of color  
  // indices and store the result in biSizeImage.  
  // The width must be DWORD aligned unless the bitmap is RLE compressed. 
  bmi->bmiHeader.biSizeImage = ((XSIZE*8+31)&~31)/8 * YSIZE; 

  for( int i=0; i<256; i++ )
  {
    bmi->bmiColors[i].rgbRed = i;
    bmi->bmiColors[i].rgbBlue = i;
    bmi->bmiColors[i].rgbGreen = i;
    bmi->bmiColors[i].rgbReserved = 0;
  }
}

//----------------------------------------------------------------------------
PlusStatus vtkIntersonVideoSource::InternalConnect()
{
  LOG_TRACE( "vtkIntersonVideoSource::InternalConnect" );

  PVOID display = bmInitializeDisplay(XSIZE*YSIZE);
  if (display == NULL)
  {
    LOG_ERROR("Could not initialize the display");
    return PLUS_FAIL;
  }

  // Turn on USB data synchronization checking
  usbTurnOnSync();

  InitializeDIB(&this->Internal->BitmapInfo);

  LOG_DEBUG("Interson Bmode DLL version "<<bmDLLVer()<<", USB probe DLL version "<<usbDLLVer());

  usbSetProbeAttachCallback(&ProbeAttached);
  usbSetProbeDetachCallback(&ProbeDetached);

  TCHAR errorStatus[256]={0};
  ULONG status = usbFindProbes(errorStatus);
  LOG_DEBUG("Find USB probes: status="<<status<<", details: "<<errorStatus);
  if (status != ERROR_SUCCESS)
  {
    LOG_ERROR("Interson finding probes failed");
    return PLUS_FAIL;
  }

  usbInitializeProbes();

  int numberOfAttachedProbes=usbNumberAttachedProbes();
  LOG_DEBUG("Number of attached probes: "<<numberOfAttachedProbes);
  if (numberOfAttachedProbes==0)
  {
    LOG_ERROR("No Interson probes are attached");
    return PLUS_FAIL;
  }
  if (numberOfAttachedProbes>1)
  {
    LOG_WARNING("Multiple Interson probes are attached, using the first one");
  }

  BYTE currentOemId=usbProbeOEMID();
  if (currentOemId != OEM_ID_INTERSON)
  {
    LOG_WARNING("Probe OEM ID mismatch: "<<OEM_ID_INTERSON<<" (Interson) is expected, got "<<currentOemId);
  }

  // get the first probe
  usbProbeHandle(0, &this->Internal->ProbeHandle); 
  // if there is hardware attached, this enables it
  usbSelectProbe(this->Internal->ProbeHandle);
  // set the display window depth for this probe
  usbSetWindowDepth(this->Internal->ProbeHandle, YSIZE);
  // set the assumed velocity (m/s)
  usbSetVelocity(this->Internal->ProbeHandle, 1540.0);
  // select the 30MHz clock
  usbSet30MHzClock(this->Internal->ProbeHandle);
  // set the clock divider for
  // 1:  ~5cm @ 30MHz and 1540m/s velocity
  // 3: ~15cm @ 30MHz;
  // 4: ~20cm @ 30MHz
  usbSetClockDivider(this->Internal->ProbeHandle, 3);

  //this->SetProbeFrequency(5000);

  // Setup the display offsets now that we have the probe and DISPLAY data
  bmSetDisplayOffset(0);

  if (usbSetCineBuffers(32) != 32)
  {
    LOG_ERROR("Could not allocate Cine buffers.");
    return PLUS_FAIL;
  }	
  this->Internal->RfDataBuffer = usbCurrentCineFrame();  

  usbSetUnidirectionalMode();
  usbSetPulseVoltage(100.0f);

  POINT ptCenter;		// Points for Zoomed Display
  ptCenter.x = XSIZE/2;
  ptCenter.y = YSIZE/2;
  int rotation=0;

  if (bmCalculateDisplay (XSIZE, YSIZE, ptCenter, this->Internal->ProbeHandle, XSIZE, rotation) == ERROR)
  {
    LOG_ERROR("CalculateDisplay failed");
  }

  usbProbeNameString probeName={0};
  usbProbeName(this->Internal->ProbeHandle, probeName);
  LOG_DEBUG("Interson probe name: "<<probeName<<", ID: "<<usbProbeID(this->Internal->ProbeHandle));

  vtkPlusDataSource* aSource=NULL;
  if( this->GetFirstActiveOutputVideoSource(aSource) != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to retrieve the video source in the IntersonVideo device.");
    return PLUS_FAIL;
  }
  
  // Clear buffer on connect because the new frames that we will acquire might have a different size 
  aSource->GetBuffer()->Clear();
  aSource->GetBuffer()->SetPixelType( VTK_UNSIGNED_CHAR );  
  aSource->GetBuffer()->SetFrameSize(XSIZE, YSIZE); 

  HINSTANCE hInst = GetModuleHandle(NULL);

  WNDCLASSEX		wndclass;
  wndclass.cbSize        = sizeof (wndclass);
  wndclass.style         = CS_HREDRAW | CS_VREDRAW | CS_BYTEALIGNCLIENT;
  wndclass.lpfnWndProc   = vtkIntersonVideoSource::vtkInternal::ImageWindowProc;
  wndclass.cbClsExtra    = 0;
  wndclass.cbWndExtra    = 0;
  wndclass.hInstance     = hInst;
  wndclass.hIcon         = NULL;
  wndclass.hCursor       = NULL;
  wndclass.hbrBackground = (HBRUSH) GetStockObject (BLACK_BRUSH);
  wndclass.lpszMenuName  = NULL ;
  wndclass.lpszClassName = TEXT("ImageWindow");
  wndclass.hIconSm       = NULL;
  RegisterClassEx(&wndclass);

  this->Internal->ImageWindowHandle = CreateWindow( TEXT("ImageWindow"), TEXT("Ultrasound"), 
    WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_CLIPCHILDREN|WS_CLIPSIBLINGS, 0, 0,
    XSIZE+2*GetSystemMetrics(SM_CXFIXEDFRAME),
    YSIZE+2*GetSystemMetrics(SM_CYFIXEDFRAME)+GetSystemMetrics(SM_CYBORDER)+GetSystemMetrics(SM_CYSIZE),
    NULL, NULL, hInst, NULL);  

  if (this->Internal->ImageWindowHandle==NULL)
  {
    LOG_ERROR("Failed to create capture window");
    return PLUS_FAIL;
  }

  SetWindowLongPtr(this->Internal->ImageWindowHandle, GWL_USERDATA, (LONG)this->Internal);

  // Create a bitmap for use in our DIB
  HDC hdc = GetDC(this->Internal->ImageWindowHandle) ;
  RECT rect;
  GetClientRect (this->Internal->ImageWindowHandle, &rect) ;
  int cx	= rect.right - rect.left;
  int cy	= rect.bottom - rect.top;
  this->Internal->DataHandle = CreateCompatibleBitmap (hdc, cx, cy);  
  GetObject (this->Internal->DataHandle, sizeof (BITMAP), (LPVOID) &this->Internal->Bitmap) ;
  // zero indexed window including borders
  size_t toAllocate=(this->Internal->Bitmap.bmWidth+16) * (this->Internal->Bitmap.bmHeight+4);
  this->Internal->MemoryBitmapBuffer.resize(toAllocate,0);

  BYTE lut[256];
  this->Internal->CreateLUT(lut, 192, 128);
	bmCreatebLUT(lut) ;

  this->Internal->CreateLinearTGC(0,150);
  bmTurnOnTGC();

  return PLUS_SUCCESS;
}


//----------------------------------------------------------------------------
PlusStatus vtkIntersonVideoSource::InternalDisconnect()
{
  LOG_DEBUG("Disconnect from Interson");

  this->StopRecording();

  usbProbe (STOP) ;
  Sleep(250); // allow time for the imaging to stop

  usbStopHardware();
  bmCloseDisplay();

  DeleteObject(this->Internal->ProbeHandle);
  DeleteObject(this->Internal->DataHandle);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkIntersonVideoSource::InternalStartRecording()
{
  Freeze(false);
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkIntersonVideoSource::InternalStopRecording()
{ 
  Freeze(true);
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkIntersonVideoSource::WaitForFrame()
{
  bool nextFrameReady = (usbWaitFrame()==1);
  DWORD	usbErrorCode = usbError();

  switch (usbErrorCode)
  {
  case USB_SUCCESS:
    break;
  case USB_FAILED:
    LOG_ERROR("USB: FAILURE. Probe was removed?");
    return PLUS_FAIL;
  case USB_TIMEOUT2A:
  case USB_TIMEOUT2B:
  case USB_TIMEOUT6A:
  case USB_TIMEOUT6B:
    if (nextFrameReady) // timeout is fine if we're in synchronized mode, so only log error if next frame is ready
    {
      LOG_WARNING("USB timeout");
    }
    break;
  case USB_NOTSEQ:
    LOG_ERROR("Lost Probe Synchronization. Please check probe cables and restart.");
    break;
  case USB_STOPPED:
    LOG_ERROR("USB: Stopped. Check probe and restart.");
    break;
  default:
    LOG_ERROR("USB: Unknown USB error: "<<usbErrorCode);
    break;
  } 
    
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkIntersonVideoSource::InternalUpdate()
{
  if (!this->Recording)
  {
    // drop the frame, we are not recording data now
    return PLUS_SUCCESS;
  }

  if (usbProbeButtonPressed())
  {
    // TODO: add support for sending the button press info through OpenIGTLink
  }

  WaitForFrame();

  this->Internal->RfDataBuffer = usbCurrentCineFrame();

  bmDrawImage(this->Internal->ImageWindowHandle, this->Internal->RfDataBuffer, this->Internal->Bitmap, &(this->Internal->MemoryBitmapBuffer[0]), &this->Internal->BitmapInfo, this->Interpolate, this->BidirectionalScan);

  this->FrameNumber++; 

  vtkPlusDataSource* aSource=NULL;
  if( this->GetFirstActiveOutputVideoSource(aSource) != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to retrieve the video source in the ICCapturing device.");
    return PLUS_FAIL;
  }

  usbProbeNameString probeName={0};
  float depthScale = -1;
  usbProbeName(this->Internal->ProbeHandle, probeName);
  usbProbeDepthScale(this->Internal->ProbeHandle,&depthScale);
  
  int frameSizeInPx[2]={XSIZE,YSIZE};
 

  // If the buffer is empty, set the pixel type and frame size to the first received properties 
  if ( aSource->GetBuffer()->GetNumberOfItems() == 0 )
  {
    LOG_DEBUG("Set up BK ProFocus image buffer");
    aSource->GetBuffer()->SetPixelType(VTK_UNSIGNED_CHAR);
    aSource->GetBuffer()->SetImageType(US_IMG_BRIGHTNESS);
    aSource->GetBuffer()->SetFrameSize( frameSizeInPx );
    aSource->GetBuffer()->SetImageOrientation(US_IMG_ORIENT_MF);

    LOG_INFO("Frame size: " << frameSizeInPx[0] << "x" << frameSizeInPx[1]
    << ", pixel type: " << vtkImageScalarTypeNameMacro(aSource->GetBuffer()->GetPixelType())
	  << ", frequency (Hz): " << usbProbeSampleFrequency(this->Internal->ProbeHandle)
	  << ", probe name: " << probeName
	  << ", display zoom: " << bmDisplayZoom()
	  << ", probe depth scale (mm/sample):" << depthScale
      << ", device image orientation: " << PlusVideoFrame::GetStringFromUsImageOrientation(aSource->GetPortImageOrientation())
      << ", buffer image orientation: " << PlusVideoFrame::GetStringFromUsImageOrientation(aSource->GetBuffer()->GetImageOrientation()));
  }
  
  if( aSource->GetBuffer()->AddItem((void*)&(this->Internal->MemoryBitmapBuffer[0]), aSource->GetPortImageOrientation(),
    frameSizeInPx, VTK_UNSIGNED_CHAR, 1, US_IMG_BRIGHTNESS, 0, this->FrameNumber) != PLUS_SUCCESS )
  {
    LOG_ERROR("Error adding item to video source " << aSource->GetSourceId());
    return PLUS_FAIL;
  }
  this->Modified();

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkIntersonVideoSource::ReadConfiguration(vtkXMLDataElement* config)
{
  LOG_TRACE("vtkIntersonVideoSource::ReadConfiguration"); 
  if ( config == NULL )
  {
    LOG_ERROR("Unable to configure Epiphan video source! (XML data element is NULL)"); 
    return PLUS_FAIL; 
  }

  Superclass::ReadConfiguration(config); 

  vtkXMLDataElement* imageAcquisitionConfig = this->FindThisDeviceElement(config);
  if (imageAcquisitionConfig == NULL) 
  {
    LOG_ERROR("Unable to find ImageAcquisition element in configuration XML structure!");
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkIntersonVideoSource::WriteConfiguration(vtkXMLDataElement* config)
{
  // Write superclass configuration
  Superclass::WriteConfiguration(config); 

  if ( config == NULL )
  {
    LOG_ERROR("Config is invalid");
    return PLUS_FAIL;
  }

  vtkXMLDataElement* imageAcquisitionConfig = this->FindThisDeviceElement(config);
  if (imageAcquisitionConfig == NULL) 
  {
    LOG_ERROR("Cannot find ImageAcquisition element in XML tree!");
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}


//----------------------------------------------------------------------------
PlusStatus vtkIntersonVideoSource::NotifyConfigured()
{
  if( this->OutputChannels.size() > 1 )
  {
    LOG_WARNING("vtkIntersonVideoSource is expecting one output channel and there are " << this->OutputChannels.size() << " channels. First output channel will be used.");
  }

  if( this->OutputChannels.empty() )
  {
    LOG_ERROR("No output channels defined for vtkIntersonVideoSource. Cannot proceed." );
    this->CorrectlyConfigured = false;
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkIntersonVideoSource::Freeze(bool freeze)
{
  if (!usbHardwareDetected())
  {
    LOG_ERROR("Freeze failed, no hardware is detected");
    return PLUS_FAIL;
  }
  this->Frozen=freeze;
  if (this->Frozen)
  {
    usbProbe(STOP);
  }
  else
  {
    usbClearCineBuffers();
    usbSetProbeFrequency(usbProbeScanFrequency(this->Internal->ProbeHandle));
    usbProbe(RUN);
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
std::string vtkIntersonVideoSource::GetSdkVersion()
{
  std::ostrstream versionString;
  versionString << "Interson Bmode DLL v"<<bmDLLVer()<<", USB Probe DLL v"<<usbDLLVer()<< std::ends;
  return versionString.str();
}

PlusStatus vtkIntersonVideoSource::SetDisplayZoom(float zoom)
{
  bmSetDisplayZoom(zoom); 
  LOG_TRACE("New zoom is %4.2f\n", bmDisplayZoom()); 
  return PLUS_SUCCESS;
}

PlusStatus vtkIntersonVideoSource::GetSampleFrequency(float& aFreq)
{
  aFreq = usbProbeSampleFrequency(this->Internal->ProbeHandle);
  LOG_TRACE("Current frequency is" << aFreq); 
  return PLUS_SUCCESS;
}


PlusStatus vtkIntersonVideoSource::SetProbeFrequency(float freq)
{
  usbSetProbeFrequency(freq);
  return PLUS_SUCCESS;
}

PlusStatus vtkIntersonVideoSource::GetProbeVelocity(float& aVel)
{
  aVel = usbProbeVelocity(this->Internal->ProbeHandle);
  LOG_TRACE("Current velocity is" << aVel); 
  return PLUS_SUCCESS;
}

PlusStatus vtkIntersonVideoSource::SetWindowDepth(int height)
{
  usbSetWindowDepth(this->Internal->ProbeHandle,height);
  return PLUS_SUCCESS;
}
