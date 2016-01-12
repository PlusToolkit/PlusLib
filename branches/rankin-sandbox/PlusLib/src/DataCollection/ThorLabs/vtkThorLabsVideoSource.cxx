/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#include "PlusConfigure.h"
#include "vtkObjectFactory.h"
#include "vtkPlusChannel.h"
#include "vtkPlusDataSource.h"
#include "vtkUnsignedCharArray.h"
#include "vtkThorLabsVideoSource.h"

//#include "c:\\Users\\lasso\\devel\\PLTools\\ThorLabs\\VISA\\Win64\\TLCCS\\TLCCS.c"

//#include <ansi_c.h> 
//#include <stdio.h>      // stdio for file operations
//#include <time.h>       // time stamps

// ThorLabs SDK headers
#include <visa.h>
#include "TLCCS.h"

class vtkThorLabsVideoSourceInternal
{
public:
  //----------------------------------------------------------------------------
  vtkThorLabsVideoSourceInternal()
  : InstrumentHandle(VI_NULL)
  {
  }
  //----------------------------------------------------------------------------
  virtual ~vtkThorLabsVideoSourceInternal()
  {
  }
  //----------------------------------------------------------------------------
  std::string GetThorLabsErrorString(ViStatus err)
  {
    ViChar ebuf[TLCCS_ERR_DESCR_BUFFER_SIZE];
    tlccs_error_message (this->InstrumentHandle, err, ebuf);
    return ebuf;
  }

  ViSession   InstrumentHandle;
  ViReal64    WavelengthIntensityDataBuffer[2*TLCCS_NUM_PIXELS]; // TLCCS_NUM_PIXELS wavelength values followed by TLCCS_NUM_PIXELS intensity values
};

vtkStandardNewMacro(vtkThorLabsVideoSource);

//----------------------------------------------------------------------------
vtkThorLabsVideoSource::vtkThorLabsVideoSource()
: Internal(new vtkThorLabsVideoSourceInternal)
, InstrumentName(NULL)
, IntegrationTimeSec(0.050)
, FrameIndex(0)
{
  this->RequireImageOrientationInConfiguration = false;
  this->StartThreadForInternalUpdates = true;
}

//----------------------------------------------------------------------------
vtkThorLabsVideoSource::~vtkThorLabsVideoSource()
{ 
  delete this->Internal;
  this->Internal = NULL;
  SetInstrumentName(NULL);
}

//----------------------------------------------------------------------------
void vtkThorLabsVideoSource::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
PlusStatus vtkThorLabsVideoSource::InternalConnect()
{
  if (this->GetConnected())
  {
    // already connected
    return PLUS_SUCCESS;
  }

  ViStatus err = VI_SUCCESS;

  std::string selectedInstrumentName;
  if (this->InstrumentName!=NULL && this->InstrumentName[0]!='\0')
  {
    selectedInstrumentName = this->InstrumentName;
  }
  if (this->InstrumentName==NULL || this->InstrumentName[0]=='\0')
  {
    LOG_DEBUG("Scanning for CCS instruments...");
    ViSession resourceManager = VI_NULL;
    err = viOpenDefaultRM(&resourceManager);
    if (err!=VI_SUCCESS)
    {
      LOG_ERROR("Error opening ThorLabs resource manager: "<<this->Internal->GetThorLabsErrorString(err));
      return PLUS_FAIL;
    }  
    ViChar instrumentNameBuffer[VI_FIND_BUFLEN]={0};
    ViUInt32 numberOfInstrumentsFound = 0;
    err = viFindRsrc(resourceManager, TLCCS_FIND_PATTERN, VI_NULL, &numberOfInstrumentsFound, instrumentNameBuffer);
    if (err!=VI_SUCCESS)
    {
      viClose(resourceManager);
      LOG_ERROR("Error finding ThorLabs resource devices: "<<this->Internal->GetThorLabsErrorString(err));
      return PLUS_FAIL;
    }
    LOG_DEBUG("Found "<<numberOfInstrumentsFound<<" instrument(s). Selected instrument: "<<instrumentNameBuffer);
    selectedInstrumentName = instrumentNameBuffer;
    viClose(resourceManager);
  }

  err = tlccs_init((char*)selectedInstrumentName.c_str(), VI_OFF, VI_OFF, &this->Internal->InstrumentHandle);
  if (err!=VI_SUCCESS)
  {
    LOG_ERROR("Error opening session to ThorLabs device: "<<this->Internal->GetThorLabsErrorString(err));
    return PLUS_FAIL;
  }

  err = tlccs_setIntegrationTime(this->Internal->InstrumentHandle, this->IntegrationTimeSec);
  if (err!=VI_SUCCESS)
  {
    LOG_ERROR("Error setting integration time "<< this->IntegrationTimeSec <<"sec in ThorLabs device: "<<this->Internal->GetThorLabsErrorString(err));
    tlccs_close(this->Internal->InstrumentHandle);
    return PLUS_FAIL;
  }
  
  memset(this->Internal->WavelengthIntensityDataBuffer, 0, sizeof(this->Internal->WavelengthIntensityDataBuffer));
  err = tlccs_getWavelengthData (this->Internal->InstrumentHandle, TLCCS_CAL_DATA_SET_FACTORY, this->Internal->WavelengthIntensityDataBuffer, NULL, NULL);
  if (err!=VI_SUCCESS)
  {
    LOG_ERROR("Error getting wavelength values in ThorLabs device: "<<this->Internal->GetThorLabsErrorString(err));
    tlccs_close(this->Internal->InstrumentHandle);
    return PLUS_FAIL;
  }

  // Set frame size (image contains one single line)
  vtkPlusDataSource* aSource(NULL);
  if( this->GetFirstVideoSource(aSource) != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to access video source in vtkThorLabsVideoSource. Critical failure.");
    tlccs_close(this->Internal->InstrumentHandle);
    return PLUS_FAIL;
  }

  aSource->SetInputFrameSize(TLCCS_NUM_PIXELS, 2, 1);
  aSource->SetPixelType(VTK_DOUBLE);
  aSource->SetNumberOfScalarComponents(1);

  return PLUS_SUCCESS;
}

PlusStatus vtkThorLabsVideoSource::InternalDisconnect()
{
  // close camera
  tlccs_close(this->Internal->InstrumentHandle);
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkThorLabsVideoSource::InternalUpdate()
{
  // request device status
  ViInt32 status = 0;
  ViStatus err = tlccs_getDeviceStatus(this->Internal->InstrumentHandle, &status);
  if (err!=VI_SUCCESS)
  {
    LOG_ERROR("Error getting device status in ThorLabs device: "<<this->Internal->GetThorLabsErrorString(err));
    return PLUS_FAIL;
  }

  // camera is idle -> we can trigger a scan
  if(status & TLCCS_STATUS_SCAN_IDLE)
  {
    // trigger scan
    err = tlccs_startScan(this->Internal->InstrumentHandle);
    if (err!=VI_SUCCESS)
    {
      LOG_ERROR("Error starting scan in ThorLabs device: "<<this->Internal->GetThorLabsErrorString(err));
      return PLUS_FAIL;
    }
  }

  // camera has data available for transfer
  if(status & TLCCS_STATUS_SCAN_TRANSFER)
  {
    double* intensityDataBuffer = this->Internal->WavelengthIntensityDataBuffer+TLCCS_NUM_PIXELS; // pointer to the second image line
    err = tlccs_getScanData(this->Internal->InstrumentHandle, intensityDataBuffer);
    if (err!=VI_SUCCESS)
    {
      LOG_ERROR("Error getting scan data in ThorLabs device: "<<this->Internal->GetThorLabsErrorString(err));
      return PLUS_FAIL;
    }

    vtkPlusDataSource* aSource(NULL);
    if( this->GetFirstVideoSource(aSource) != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to retrieve the video source in the ThorLabs device.");
      return PLUS_FAIL;
    }
    this->FrameIndex++;
    int frameSizeInPx[3] = {TLCCS_NUM_PIXELS, 2, 1};
    aSource->AddItem(this->Internal->WavelengthIntensityDataBuffer, US_IMG_ORIENT_MF, frameSizeInPx, VTK_DOUBLE, 1, US_IMG_BRIGHTNESS, 0, this->FrameIndex);
    this->Modified();
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkThorLabsVideoSource::InternalStartRecording()
{
  this->FrameIndex = 0;
  //double startTime = vtkAccurateTimer::GetSystemTime();
  //this->Buffer->SetStartTime(startTime);
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkThorLabsVideoSource::InternalStopRecording()
{
  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus vtkThorLabsVideoSource::NotifyConfigured()
{
  if( this->OutputChannels.size() > 1 )
  {
    LOG_WARNING("ThorLabsVideoSource is expecting one output channel and there are " << this->OutputChannels.size() << " channels. First output channel will be used.");
    return PLUS_FAIL;
  }

  if( this->OutputChannels.size() == 0 )
  {
    LOG_ERROR("No output channels defined for ThorLabs video source. Cannot proceed." );
    this->CorrectlyConfigured = false;
    return PLUS_FAIL;
  }

  vtkPlusDataSource* aSource(NULL);
  if( this->GetFirstVideoSource(aSource) != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to retrieve the video source in the ThorLabs device.");
    return PLUS_FAIL;
  }

  // If input image orientation is not set (it is not required) then
  // set it to MF by default
  if (aSource->GetInputImageOrientation()==US_IMG_ORIENT_XX)
  {
    aSource->SetInputImageOrientation(US_IMG_ORIENT_MF);
  }

  return PLUS_SUCCESS;
}
