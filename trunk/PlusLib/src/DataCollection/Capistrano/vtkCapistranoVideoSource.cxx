/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkCapistranoVideoSource.h"

#include "usbprobedll_net.h"
#include "BmodeDLL.h"

vtkStandardNewMacro(vtkCapistranoVideoSource);

class vtkCapistranoVideoSource::vtkInternal
{
public:
  vtkCapistranoVideoSource * External;
  bmBITMAPINFO BitmapInfo;

  //----------------------------------------------------------------------------
  vtkCapistranoVideoSource::vtkInternal::vtkInternal(vtkCapistranoVideoSource * external):
    External( external )
  {
  }

  //----------------------------------------------------------------------------
  PlusStatus vtkCapistranoVideoSource::vtkInternal::InitializeDIB(int imageSize[2])
  {
    this->BitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    this->BitmapInfo.bmiHeader.biWidth = imageSize[0];
    this->BitmapInfo.bmiHeader.biHeight = -imageSize[1];
    this->BitmapInfo.bmiHeader.biPlanes = 1;
    this->BitmapInfo.bmiHeader.biBitCount = 8;
    this->BitmapInfo.bmiHeader.biCompression = 0;
    this->BitmapInfo.bmiHeader.biXPelsPerMeter = 0;
    this->BitmapInfo.bmiHeader.biYPelsPerMeter = 0;
    this->BitmapInfo.bmiHeader.biClrUsed = 0;
    this->BitmapInfo.bmiHeader.biClrImportant = 0;

    // Compute the number of bytes in the array of color  
    // indices and store the result in biSizeImage.  
    // The width must be DWORD aligned unless the bitmap is RLE compressed. 
    this->BitmapInfo.bmiHeader.biSizeImage = ((imageSize[0]*8+31)&~31)/8 * imageSize[1]; 

    for( int i = 0; i < 256; ++i )
    {
      this->BitmapInfo.bmiColors[i].rgbRed = i;
      this->BitmapInfo.bmiColors[i].rgbBlue = i;
      this->BitmapInfo.bmiColors[i].rgbGreen = i;
      this->BitmapInfo.bmiColors[i].rgbReserved = 0;
    }

    return PLUS_SUCCESS;
  }
};

//----------------------------------------------------------------------------
vtkCapistranoVideoSource::vtkCapistranoVideoSource()
{
  this->Internal = new vtkInternal(this);

  this->ImageSize[0]=800;
  this->ImageSize[1]=512;
}

//----------------------------------------------------------------------------
vtkCapistranoVideoSource::~vtkCapistranoVideoSource()
{
  if( !this->Connected )
    {
    this->Disconnect();
    }

  delete this->Internal;
  this->Internal=NULL;
}

//----------------------------------------------------------------------------
void vtkCapistranoVideoSource::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//-----------------------------------------------------------------------------
PlusStatus vtkCapistranoVideoSource::ReadConfiguration(vtkXMLDataElement* config)
{
  LOG_TRACE("vtkCapistranoVideoSource::ReadConfiguration"); 
  if ( config == NULL )
    {
    LOG_ERROR("Unable to configure Capistrano video source! (XML data element is NULL)"); 
    return PLUS_FAIL; 
    }

  Superclass::ReadConfiguration(config); 

  vtkXMLDataElement* deviceConfig = this->FindThisDeviceElement(config);
  if (deviceConfig == NULL) 
    {
    LOG_ERROR("Unable to find ImageAcquisition element in configuration XML structure!");
    return PLUS_FAIL;
    }

  XML_READ_VECTOR_ATTRIBUTE_OPTIONAL(int, 2, ImageSize, deviceConfig);

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkCapistranoVideoSource::WriteConfiguration(vtkXMLDataElement* rootConfigElement)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING(deviceConfig, rootConfigElement);

  deviceConfig->SetVectorAttribute("ImageSize", 2, this->ImageSize);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkCapistranoVideoSource::NotifyConfigured()
{
  if( this->OutputChannels.size() > 1 )
  {
    LOG_WARNING("vtkCapistranoVideoSource is expecting at most one output channel and there are " << this->OutputChannels.size() << " channels.");
  }

  if( this->OutputChannels.empty() )
  {
    LOG_ERROR("No output channels defined for vtkCapistranoVideoSource. Cannot proceed." );
    this->CorrectlyConfigured = false;
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
std::string vtkCapistranoVideoSource::GetSdkVersion()
{
  std::ostringstream versionString;
  versionString << "Capistrano BMode DLL version: " << bmDLLVer()
                << ", USB Probe DLL version: " << usbDLLVer() << std::ends;
  return versionString.str();
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
PlusStatus vtkCapistranoVideoSource::InternalConnect()
{
  LOG_TRACE( "vtkCapistranoVideoSource::InternalConnect" );

  LOG_DEBUG( "Capistrano BMode DLL version: " << bmDLLVer()
              << ", USB Probe DLL version: " << usbDLLVer());

  // These are commented out of usbprobedll_net.h in cSDK2013.
  //usbSetProbeAttachCallback(&ProbeAttached);
  //usbSetProbeDetachCallback(&ProbeDetached);

  char errorStatus[256] = {0};
  const ULONG status = usbFindProbes(errorStatus);
  LOG_DEBUG("Find USB probes status:" << status);
  if(status != ERROR_SUCCESS)
    {
    LOG_ERROR("Capistrano finding probes failed: " << errorStatus);
    return PLUS_FAIL;
    }

  const int winYSize = 800;
  const double sos = 1540.;
  usbInitializeProbes(winYSize, sos);

  // Whether we collect one or two frames per motor sweep.
  usbSetUnidirectionalMode();
  //usbSetBidirectionalMode();

  // Turn on USB data synchronization checking
  usbTurnOnSync();

  const int numberOfAttachedProbes = usbNumberAttachedProbes();
  LOG_DEBUG("Number of attached probes: " << numberOfAttachedProbes);
  if(numberOfAttachedProbes == 0)
    {
    LOG_ERROR("No Interson probes are attached");
    return PLUS_FAIL;
    }
  if(numberOfAttachedProbes > 1)
    {
    LOG_WARNING("Multiple Interson probes are attached, using the first one");
    }

  // With 80 MHz board oscillator, the sample rate is 40 MHz
  usbSetSampleClockDivider(2);

  HANDLE display = bmInitializeDisplay(this->ImageSize[0] * this->ImageSize[1]);
  if (display == NULL)
    {
    LOG_ERROR("Could not initialize the display");
    return PLUS_FAIL;
    }

  this->Internal->InitializeDIB(this->ImageSize);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkCapistranoVideoSource::InternalDisconnect()
{
  LOG_DEBUG("Disconnect from Capistrano");

  bmCloseDisplay();
}

//----------------------------------------------------------------------------
PlusStatus vtkCapistranoVideoSource::SetImageSize(int imageSize[2])
{
  this->ImageSize[0] = imageSize[0];
  this->ImageSize[1] = imageSize[1];
  return PLUS_SUCCESS;
}
