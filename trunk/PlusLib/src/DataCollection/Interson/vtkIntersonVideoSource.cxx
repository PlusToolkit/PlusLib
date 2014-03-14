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



#include "vtkUSImagingParameters.h"

//#define XSIZE 800						// Image window size 
//#define YSIZE 512					    //		XSIZE must be divisible by four!

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
  static const int samplesPerLine = 2048;

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
    int tgc[samplesPerLine]={0};
    int b = tgcMin; 
    float m = (float) (tgcMax - tgcMin) / samplesPerLine;
    for (int x = 0; x < samplesPerLine; x++)
    {
      tgc[x] = (int) (m * (float) x) + b;
    }
    bmSetTGC(tgc);
  }


  //----------------------------------------------------------------------------
  void vtkIntersonVideoSource::vtkInternal::CreateLinearTGC(int initialTGC, int midTGC, int farTGC)
  {

    /*  A linear TGC function is created. The initial point is initialTGC, then is linear until the
	middle point (midTGC) and then linear until the maximum depth where the compensationis equal to farTGC*/

    int tgc[samplesPerLine]={0};
    double firstSlope = (double) (midTGC-initialTGC ) / (samplesPerLine/2);
	double secondSlope = (double) (farTGC-midTGC ) / (samplesPerLine/2);
    for (int x = 0; x < samplesPerLine/2; x++)
    {
      tgc[x] = (int) (firstSlope * (double) x) + initialTGC;
	  tgc[samplesPerLine/2 +x] = (int) (secondSlope * (double) x) + midTGC;
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
  void CreateLinearLUT(BYTE lut[], int Level, int Window)
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

//----------------------------------------------------------------------------
  void CreateLUT(BYTE lut[], int Brightness, int Contrast, int Level, int Window)
  {

	int center = Window / 2;				// center of window
    int left = Level - center;				// left of window
    int right = Level + center;				// right of window
   for (int x=0; x <= 255; x++) 
   {
     int y = (int) ((float)Contrast/256.0f * (float)(x-128) + Brightness - 64);
     if (y<left)
     {
       y=0;
     }
     else if (y>right)
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
  //this->SoundVelocity = 1540.0;
  this->ClockDivider = 1;
  this->PulsFrequencyDivider = 2;

  this->Brightness = 128; //192
  this->Contrast = 256; //128
  this->LutCenter = 128; //192
  this->LutWindow = 256; //128
  this->MinTGC = 0;
  this->MaxTGC = 150;

  this->InitialGain =0;
  this->MidGain=0;
  this->FarGain=0;

  this->ImageSize[0]=800;
  this->ImageSize[1]=512;

  this->PulseVoltage=30.0f;

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
PlusStatus vtkIntersonVideoSource::InitializeDIB(bmBITMAPINFO *bmi)
{
  bmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  bmi->bmiHeader.biWidth = this->ImageSize[0];
  bmi->bmiHeader.biHeight = -this->ImageSize[1];
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
  bmi->bmiHeader.biSizeImage = ((this->ImageSize[0]*8+31)&~31)/8 * this->ImageSize[1]; 

  for( int i=0; i<256; i++ )
  {
    bmi->bmiColors[i].rgbRed = i;
    bmi->bmiColors[i].rgbBlue = i;
    bmi->bmiColors[i].rgbGreen = i;
    bmi->bmiColors[i].rgbReserved = 0;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkIntersonVideoSource::InternalConnect()
{
  LOG_TRACE( "vtkIntersonVideoSource::InternalConnect" );

  PVOID display = bmInitializeDisplay(this->ImageSize[0]*this->ImageSize[1]);
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
  usbSetWindowDepth(this->Internal->ProbeHandle, this->ImageSize[1]);
  // set the assumed velocity (m/s)
  double soundVelocity = -1 ;
  this->ImagingParameters->GetSoundVelocity(soundVelocity);
  usbSetVelocity(this->Internal->ProbeHandle, soundVelocity );
  double depth = -1;
  this->ImagingParameters->GetDepthMm(depth);
  this->SetDepthMm(depth);
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
  usbSetPulseVoltage(this->PulseVoltage);

  POINT ptCenter;		// Points for Zoomed Display
  ptCenter.x = this->ImageSize[0]/2;
  ptCenter.y = this->ImageSize[1]/2;
  int rotation=0;

  if (bmCalculateDisplay (this->ImageSize[0], this->ImageSize[1], ptCenter, this->Internal->ProbeHandle, this->ImageSize[0], rotation) == ERROR)
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
  aSource->GetBuffer()->SetFrameSize(this->ImageSize[0], this->ImageSize[1]); 

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
    this->ImageSize[0]+2*GetSystemMetrics(SM_CXFIXEDFRAME),
    this->ImageSize[1]+2*GetSystemMetrics(SM_CYFIXEDFRAME)+GetSystemMetrics(SM_CYBORDER)+GetSystemMetrics(SM_CYSIZE),
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

  double gain[3]={-1,-1,-1};
  this->ImagingParameters->GetGainPercent(gain);
  this->SetGainPercent(gain);

  BYTE lut[256];
  double intensity=-1; double contrast = -1;
  this->ImagingParameters->GetIntensity(intensity);
  this->ImagingParameters->GetContrast(contrast);
  if (intensity >=0)
  {
	  this->Brightness = (int)intensity;
  }
  if (contrast >=0)
  {
	  this->Contrast = (int)contrast;
  }
  this->Internal->CreateLUT(lut, this->Brightness, this->Contrast, this->LutCenter, this->LutWindow);
  bmCreatebLUT(lut);
  //this->Internal->CreateLinearTGC(this->MinTGC,this->MaxTGC );
  //bmTurnOnTGC();
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

  int frameSizeInPx[2]={this->ImageSize[0],this->ImageSize[1]};

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
	  << ", probe sample frequency (Hz): " << usbProbeSampleFrequency(this->Internal->ProbeHandle)
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
    LOG_ERROR("Unable to configure Interson video source! (XML data element is NULL)"); 
    return PLUS_FAIL; 
  }

  Superclass::ReadConfiguration(config); 

  vtkXMLDataElement* deviceConfig = this->FindThisDeviceElement(config);
  if (deviceConfig == NULL) 
  {
    LOG_ERROR("Unable to find ImageAcquisition element in configuration XML structure!");
    return PLUS_FAIL;
  }

  int imageSize[2]={0,0};
  if (deviceConfig->GetVectorAttribute("ImageSize", 2, imageSize))
  {
    this->SetImageSize(imageSize);
  }

  double sector = -1; 
  if ( deviceConfig->GetScalarAttribute("SectorPercent", sector)) 
  {
    this->ImagingParameters->SetSectorPercent(sector); 
  }

  double gainPercent[3] = {0,0,0}; //-1; 
  if ( deviceConfig->GetVectorAttribute("GainPercent", 3, gainPercent)) 
  {
    this->ImagingParameters->SetGainPercent(gainPercent); 
  }

  int intensity = -1; 
  if ( deviceConfig->GetScalarAttribute("Intensity", intensity)) 
  {
    this->ImagingParameters->SetIntensity(intensity); 
  }

  int contrast = -1; 
  if ( deviceConfig->GetScalarAttribute("Contrast", contrast)) 
  {
    this->ImagingParameters->SetContrast(contrast); 
  }

  double dynRange = -1; 
  if ( deviceConfig->GetScalarAttribute("DynRangeDb", dynRange)) 
  {
    this->ImagingParameters->SetDynRangeDb(dynRange); 
  }

  double zoom = -1; 
  if ( deviceConfig->GetScalarAttribute("ZoomFactor", zoom)) 
  {
    this->ImagingParameters->SetZoomFactor(zoom); 
  }

  double frequency = -1; 
  if ( deviceConfig->GetScalarAttribute("FrequencyMhz", frequency)) 
  {
    this->ImagingParameters->SetFrequencyMhz(frequency); 
  }

  double depthMm = -1; 
  if ( deviceConfig->GetScalarAttribute("DepthMm", depthMm)) 
  {
    this->ImagingParameters->SetDepthMm(depthMm); 
  }

  double soundVelocity = -1; 
  if ( deviceConfig->GetScalarAttribute("SoundVelocity", soundVelocity)) 
  {
    this->ImagingParameters->SetSoundVelocity(soundVelocity); 
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

//----------------------------------------------------------------------------
PlusStatus vtkIntersonVideoSource::SetDisplayZoom(double zoom)
{
  bmSetDisplayZoom(zoom); 
  LOG_TRACE("New zoom is " << bmDisplayZoom()); 
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkIntersonVideoSource::GetSampleFrequency(double& aFreq)
{
  aFreq = usbProbeSampleFrequency(this->Internal->ProbeHandle);
  LOG_TRACE("Current frequency is " << aFreq); 
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkIntersonVideoSource::SetProbeFrequency(double freq)
{
  usbSetProbeFrequency(freq);
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkIntersonVideoSource::GetProbeVelocity(double& aVel)
{
  aVel = usbProbeVelocity(this->Internal->ProbeHandle);
  LOG_TRACE("Current velocity is " << aVel); 
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkIntersonVideoSource::SetWindowDepth(int height)
{
  usbSetWindowDepth(this->Internal->ProbeHandle,height);
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkIntersonVideoSource::SetDepthMm(double depthMm)
{
  // to be implemented
  std::vector<pair<double,double>> allowedModes;
  this->GetProbeAllowedModes(allowedModes);
  int numberOfAllowedModes = allowedModes.size();
  std::vector<int> possibleModes;
  double choosedFrequency = -1;
  double choosedDepth = -1;
  for (int i=0;i<numberOfAllowedModes;i++)
  {
    if(allowedModes[i].second==depthMm/10)
	{
	  possibleModes.push_back(i);	
	}
  }
  if(possibleModes.size()==1)
  {
	choosedFrequency = allowedModes[possibleModes[0]].first ;
	choosedDepth = allowedModes[possibleModes[0]].second ;
  }
  else if(possibleModes.size()==0)
  {
	choosedFrequency = allowedModes[0].first ;
	choosedDepth = allowedModes[0].second ;
	LOG_INFO("The probe does not allow the required depth." << choosedDepth << " cm depth was chosed instead." );
  }

  if (choosedDepth==5 || choosedDepth==10 || choosedDepth==15 || choosedDepth==20)
  {
	// select the 30MHz clock
    usbSet30MHzClock(this->Internal->ProbeHandle);
	this->ClockFrequency = 30;
	this->ClockDivider = choosedDepth/5;
	// set the clock divider for
    // 1:  ~5cm @ 30MHz and 1540m/s velocity
    // 3: ~15cm @ 30MHz;
    // 4: ~20cm @ 30MHz
  }
  else
  {
	// select the 48MHz clock
    usbSet48MHzClock(this->Internal->ProbeHandle);
	this->ClockFrequency = 48;
	this->ClockDivider = choosedDepth/3;
  }

  this->PulsFrequencyDivider = this->ClockFrequency/choosedFrequency;
  usbSetPulseFrequency(this->PulsFrequencyDivider);
  usbSetClockDivider(this->Internal->ProbeHandle, this->ClockDivider);
  this->ImagingParameters->SetDepthMm(choosedDepth*100);
  this->ImagingParameters->SetFrequencyMhz(choosedFrequency);
  double clockDivider = usbClockDivider(); 
  //pulseFrequency = usbPulseFrequency();



  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkIntersonVideoSource::SetImageSize(int imageSize[2])
{
  this->ImageSize[0]= imageSize[0];
  this->ImageSize[1]= imageSize[1];
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkIntersonVideoSource::SetFrequencyMhz(double freq)
{
	// to be implemented
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkIntersonVideoSource::SetGainPercent(double gainPercent[3])
{
  
  /* The following commented code is useful when using an RF probe with an analog TGC control.
  It sets the value, in dB, for the gain at the  last sample taken.   */
	/*
	  initialGain = usbInitialGain();
      midGain = usbMidGain();
      farGain = usbFarGain();
      usbSetInitialGain(this->InitialGain);
      usbSetMidGain(this->MidGain);
      usbSetFarGain(this->FarGain);
      initialGain = usbInitialGain();
      midGain = usbMidGain();
      farGain = usbFarGain();
     */
  /* If the above code is executed the gain values are changed but it has no effect on the image. Probably it is beacause the probe
  does not have analog TGC control.
  The code below sets a linear TGC curve based on three values (initial, middle and end) of the curve.*/

  double maximumGain=255; 
  if (gainPercent[0]>=0 && gainPercent[1]>=0 && gainPercent[2]>=0)
  {
    this->InitialGain = gainPercent[0] * maximumGain /100 ;
    this->MidGain = gainPercent[1] * maximumGain /100 ;
    this->FarGain = gainPercent[2] * maximumGain /100 ;
  }

  this->Internal->CreateLinearTGC(this->InitialGain,this->MidGain,this->FarGain); 

  bmTurnOnTGC();
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkIntersonVideoSource::SetZoomFactor(double gainPercent)
{
	// to be implemented
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkIntersonVideoSource::GetProbeAllowedModes(std::vector<pair<double,double>>& allowedModes)
{
  
  double clockDivider = usbClockDivider();	
  double sampleFrequency = usbProbeSampleFrequency(this->Internal->ProbeHandle);
  double divider = usbPulseFrequency();
  unsigned is24MHz, is15MHz, is12MHz, is8MHz, is7_5MHz, is6MHz, is5MHz, is3_75MHz; 
  std::pair<double,double> pair;
  is24MHz = usbProbeAllowedModes(this->Internal->ProbeHandle) & P24MHZ; 
  if (is24MHz & CLKDIV1) { 
	  // now we know the 24 MHz pulse frequency is good and 3 cm mode is good 
	  LOG_INFO("24 Mhz pulse frequency and 3 cm depth is an allowed mode for this probe");	  
	  pair.first = 24;
	  pair.second = 3;
	  allowedModes.push_back(pair);
  }
  if (is24MHz & CLKDIV2) { 
	  // now we know the 24 MHz pulse frequency is good and 6 cm mode is good 
	  LOG_INFO("24 Mhz pulse frequency and 6 cm depth is an allwoed mode for this probe");
  }
  if (is24MHz & CLKDIV3) { 
	  // now we know the 24 MHz pulse frequency is good and 9 cm mode is good 
	  LOG_INFO("24 Mhz pulse frequency and 9 cm depth is an allwoed mode for this probe");
  }
  if (is24MHz & CLKDIV4) { 
	  // now we know the 24 MHz pulse frequency is good and 12 cm mode is good 
	  LOG_INFO("24 Mhz pulse frequency and 12 cm depth is an allowed mode for this probe");
  }

  is15MHz = usbProbeAllowedModes(this->Internal->ProbeHandle) & P15MHZ; 
  if (is15MHz & CLKDIV1) { 
	  // now we know the 15 MHz pulse frequency is good and 5 cm mode is good 
	  LOG_INFO("15 Mhz pulse frequency and 5 cm depth is an allowed mode for this probe");
  }
  if (is15MHz & CLKDIV2) { 
	  // now we know the 15 MHz pulse frequency is good and 10 cm mode is good 
	  LOG_INFO("15 Mhz pulse frequency and 10 cm depth is an allowed mode for this probe");
  }
  if (is15MHz & CLKDIV3) { 
	  // now we know the 15 MHz pulse frequency is good and 15 cm mode is good 
	  LOG_INFO("15 Mhz pulse frequency and 15 cm depth is an allowed mode for this probe");
  }
  if (is15MHz & CLKDIV4) { 
	  // now we know the 15 MHz pulse frequency is good and 20 cm mode is good 
	  LOG_INFO("15 Mhz pulse frequency and 20 cm depth is an allowed mode for this probe");
  }

  is12MHz = usbProbeAllowedModes(this->Internal->ProbeHandle) & P12MHZ; 
  if (is12MHz & CLKDIV1) { 
	  // now we know the 12 MHz pulse frequency is good and 5 cm mode is good 
	  LOG_INFO("12 Mhz pulse frequency and 5 cm depth is an allowed mode for this probe");
  }
  if (is12MHz & CLKDIV2) { 
	  // now we know the 12 MHz pulse frequency is good and 10 cm mode is good 
	  LOG_INFO("12 Mhz pulse frequency and 10 cm depth is an allowed mode for this probe");
  }
  if (is12MHz & CLKDIV3) { 
	  // now we know the 12 MHz pulse frequency is good and 15 cm mode is good 
	  LOG_INFO("12 Mhz pulse frequency and 15 cm depth is an allowed mode for this probe");
  }
  if (is12MHz & CLKDIV4) { 
	  // now we know the 12 MHz pulse frequency is good and 20 cm mode is good 
	  LOG_INFO("12 Mhz pulse frequency and 20 cm depth is an allowed mode for this probe");
  }
  is8MHz = usbProbeAllowedModes(this->Internal->ProbeHandle) & P8MHZ; 
  if (is8MHz & CLKDIV1) { 
	  // now we know the 8 MHz pulse frequency is good and 3 cm mode is good 
	  LOG_INFO("8 Mhz pulse frequency and 3 cm depth is an allowed mode for this probe");
  }
  if (is8MHz & CLKDIV2) { 
	  // now we know the 8 MHz pulse frequency is good and 6 cm mode is good 
	  LOG_INFO("8 Mhz pulse frequency and 6 cm depth is an allowed mode for this probe");
  }
  if (is8MHz & CLKDIV3) { 
	  // now we know the 8 MHz pulse frequency is good and 9 cm mode is good 
	  LOG_INFO("8 Mhz pulse frequency and 9 cm depth is an allowed mode for this probe");
  }
  if (is8MHz & CLKDIV4) { 
	  // now we know the 8 MHz pulse frequency is good and 12 cm mode is good 
	  LOG_INFO("8 Mhz pulse frequency and 12 cm depth is an allowed mode for this probe");
  }
  is7_5MHz = usbProbeAllowedModes(this->Internal->ProbeHandle) & P7_5MHZ; 
  if (is7_5MHz & CLKDIV1) { 
	  // now we know the 7.5 MHz pulse frequency is good and 5 cm mode is good 
	  LOG_INFO("7.5 Mhz pulse frequency and 5 cm depth is an allowed mode for this probe");
  }
  if (is7_5MHz & CLKDIV2) { 
	  // now we know the 7.5 MHz pulse frequency is good and 10 cm mode is good 
	  LOG_INFO("7.5 Mhz pulse frequency and 10 cm depth is an allowed mode for this probe");
  }
  if (is7_5MHz & CLKDIV3) { 
	  // now we know the 7.5 MHz pulse frequency is good and 15 cm mode is good 
	  LOG_INFO("7.5 Mhz pulse frequency and 15 cm depth is an allowed mode for this probe");
  }
  if (is7_5MHz & CLKDIV4) { 
	  // now we know the 7.5 MHz pulse frequency is good and 20 cm mode is good 
	  LOG_INFO("7.5 Mhz pulse frequency and 20 cm depth is an allowed mode for this probe");
  }
  is6MHz = usbProbeAllowedModes(this->Internal->ProbeHandle) & P6MHZ; 
  if (is6MHz & CLKDIV1) { 
	  // now we know the 6 MHz pulse frequency is good and 3 cm mode is good 
	  LOG_INFO("6 Mhz pulse frequency and 3 cm depth is an allowed mode for this probe");
  }
  if (is6MHz & CLKDIV2) { 
	  // now we know the 6 MHz pulse frequency is good and 6 cm mode is good 
	  LOG_INFO("6 Mhz pulse frequency and 6 cm depth is an allowed mode for this probe");
  }
  if (is6MHz & CLKDIV3) { 
	  // now we know the 6 MHz pulse frequency is good and 9 cm mode is good
	  LOG_INFO("6 Mhz pulse frequency and 9 cm depth is an allowed mode for this probe");
  }
  if (is6MHz & CLKDIV4) { 
	  // now we know the 6 MHz pulse frequency is good and 12 cm mode is good 
	  LOG_INFO("6 Mhz pulse frequency and 12 cm depth is an allowed mode for this probe");
  }
  is5MHz = usbProbeAllowedModes(this->Internal->ProbeHandle) & P5MHZ; 
  if (is5MHz & CLKDIV1) { 
	  // now we know the 5 MHz pulse frequency is good and 5 cm mode is good 
	  LOG_INFO("5 Mhz pulse frequency and 5 cm depth is an allowed mode for this probe");
  }
  if (is5MHz & CLKDIV2) { 
	  // now we know the 5 MHz pulse frequency is good and 10 cm mode is good 
	  LOG_INFO("5 Mhz pulse frequency and 10 cm depth is an allowed mode for this probe");
  }
  if (is5MHz & CLKDIV3) { 
	  // now we know the 5 MHz pulse frequency is good and 15 cm mode is good 
	  LOG_INFO("5 Mhz pulse frequency and 15 cm depth is an allowed mode for this probe");
  }
  if (is5MHz & CLKDIV4) { 
	  // now we know the 5 MHz pulse frequency is good and 20 cm mode is good 
	  LOG_INFO("5 Mhz pulse frequency and 20 cm depth is an allowed mode for this probe");
  }
  is3_75MHz = usbProbeAllowedModes(this->Internal->ProbeHandle) & P3_75MHZ; 
  if (is3_75MHz & CLKDIV1) { 
	  // now we know the 3.75 MHz pulse frequency is good and 5 cm mode is good 
	  LOG_INFO("3.75 Mhz pulse frequency and 5 cm depth is an allowed mode for this probe");
	  pair.first = 3.75;
	  pair.second = 5;
	  allowedModes.push_back(pair);
  }
  if (is3_75MHz & CLKDIV2) { 
	  // now we know the 3.75 MHz pulse frequency is good and 10 cm mode is good 
	  LOG_INFO("3.75 Mhz pulse frequency and 10 cm depth is an allowed mode for this probe");
	  pair.first = 3.75;
	  pair.second = 10;
	  allowedModes.push_back(pair);
  }
  if (is3_75MHz & CLKDIV3) { 
	  // now we know the 3.75 MHz pulse frequency is good and 15 cm mode is good
	  pair.first = 3.75;
	  pair.second = 15;
	  allowedModes.push_back(pair);
	  LOG_INFO("3.75 Mhz pulse frequency and 15 cm depth is an allowed mode for this probe");
  }
  if (is3_75MHz & CLKDIV4) { 
	  // now we know the 3.75 MHz pulse frequency is good and 20 cm mode is good
	  pair.first = 3.75;
	  pair.second = 20;
	  allowedModes.push_back(pair);
	  LOG_INFO("3.75 Mhz pulse frequency and 20 cm depth is an allowed mode for this probe");
  }
  //allowedModes = usbProbeAllowedModes(this->Internal->ProbeHandle);
  //LOG_TRACE("Allowed modes are" << allowedModes); 
  return PLUS_SUCCESS;
}
