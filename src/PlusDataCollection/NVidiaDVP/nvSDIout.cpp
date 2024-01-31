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
#include "glExtensions.h"
#include "nvCommon.h"
#include "nvSDIout.h"
#include "nvSDIutil.h"

//----------------------------------------------------------------------------
CNvSDIout::CNvSDIout()
{
  m_vioHandle = NULL;
}

//----------------------------------------------------------------------------
CNvSDIout::~CNvSDIout()
{
}

//----------------------------------------------------------------------------
HRESULT CNvSDIout::Init( nvOptions* options, CNvSDIoutGpu* SDIoutGpu )
{
  NvAPI_Status l_ret;

  if( SDIoutGpu == NULL || SDIoutGpu->IsSDIoutput() != true )
  {
    //If the application didn't scan the system topology and selected a
    // gpu for output the the scanning has to be done here.
    // Initialize NVAPI
    if ( NvAPI_Initialize() != NVAPI_OK )
    {
      return E_FAIL;
    }

    // Query available VIO topologies.
    // Fail if there are no VIO topologies or devices available.
    NVVIOTOPOLOGY l_vioTopologies;
    memset( &l_vioTopologies, 0, sizeof( l_vioTopologies ) );
    l_vioTopologies.version = NVVIOTOPOLOGY_VER;

    if ( NvAPI_VIO_QueryTopology( &l_vioTopologies ) != NVAPI_OK )
    {
      return E_FAIL;
    }

    if ( l_vioTopologies.vioTotalDeviceCount == 0 )
    {
      return E_FAIL;
    }

    // Cycle through all SDI topologies looking for the first
    // available SDI output device topology.
    BOOL l_bFound = FALSE;
    unsigned int i = 0;
    NVVIOCAPS l_vioCaps;
    while ( ( i < l_vioTopologies.vioTotalDeviceCount ) && ( !l_bFound ) )
    {
      // Get video I/O capabilities for current video I/O target.
      memset( &l_vioCaps, 0, sizeof( l_vioCaps ) );
      l_vioCaps.version = NVVIOCAPS_VER;
      if ( NvAPI_VIO_GetCapabilities( l_vioTopologies.vioTarget[i].hVioHandle,
                                      &l_vioCaps ) != NVAPI_OK )
      {
        LOG_ERROR( "Video I/O Unsupported." );
        return E_FAIL;
      }

      // If video output device found, save VIO handle and set flag.
      if ( l_vioCaps.adapterCaps & NVVIOCAPS_VIDOUT_SDI )
      {
        m_vioHandle = l_vioTopologies.vioTarget[i].hVioHandle;
        l_bFound = TRUE;
      }
      else
      {
        i++;
      }
    } // while i < vioTotalDeviceCount

    // If no video output device found, return error.
    if ( !l_bFound )
    {
      LOG_ERROR( "No SDI video output devices found." );
      return E_FAIL;
    }
  }
  else
  {
    m_vioHandle = SDIoutGpu->GetVioHandle();
  }
  // Open the SDI device
  if ( NvAPI_VIO_Open( m_vioHandle, NVVIOCLASS_SDI, NVVIOOWNERTYPE_APPLICATION ) != NVAPI_OK )
  {
    return E_FAIL;
  }

  // Configure the SDI GVO device
  NVVIOCONFIG_V1 l_vioConfig;
  memset( &l_vioConfig, 0, sizeof( l_vioConfig ) );
  l_vioConfig.version = NVVIOCONFIG_VER1;
  l_vioConfig.fields = 0;
  l_vioConfig.nvvioConfigType = NVVIOCONFIGTYPE_OUT;

  // Configure signal and data format
  l_vioConfig.fields = NVVIOCONFIG_SIGNALFORMAT;
  l_vioConfig.vioConfig.outConfig.signalFormat = options->videoFormat;
  l_vioConfig.fields |= NVVIOCONFIG_DATAFORMAT;
  l_vioConfig.vioConfig.outConfig.dataFormat = options->dataFormat;

  // Send 8-bit colorbars as YCrCb on both channels, overriding any user settings.
  if ( ( options->testPattern == TEST_PATTERN_COLORBARS8_75 ) ||
       ( options->testPattern == TEST_PATTERN_COLORBARS8_100 ) ||
       ( options->testPattern == TEST_PATTERN_YCRCB_COLORBARS ) )
  {
    l_vioConfig.vioConfig.outConfig.dataFormat = NVVIODATAFORMAT_DUAL_X8X8X8_TO_DUAL_422_PASSTHRU;
  }

  // Send 10-bit colorbars as 10-bit YCrCb on a single channel, overriding any user settings.
  if ( ( options->testPattern == TEST_PATTERN_COLORBARS10_75 ) ||
       ( options->testPattern == TEST_PATTERN_COLORBARS10_100 ) )
  {
    l_vioConfig.vioConfig.outConfig.dataFormat = NVVIODATAFORMAT_X10X10X10_444_PASSTHRU;
  }

  // Send 8-bit ramp as 8-bit YCrCbA4224
  if ( options->testPattern == TEST_PATTERN_RAMP8 )
  {
    l_vioConfig.vioConfig.outConfig.dataFormat = NVVIODATAFORMAT_R8G8B8A8_TO_YCRCBA4224;
  }

  // Send 16-bit ramp as 16-bit YCrCb444
  if ( options->testPattern == TEST_PATTERN_RAMP16 )
  {
    l_vioConfig.vioConfig.outConfig.dataFormat = NVVIODATAFORMAT_R10G10B10_TO_YCRCB422;
  }

  // Set sync source if specified.  The sync source must be set
  // before a valid sync can be detected.
  if ( options->syncEnable )
  {
    l_vioConfig.vioConfig.outConfig.syncEnable = options->syncEnable;
    l_vioConfig.vioConfig.outConfig.syncSource = options->syncSource;

    switch( options->syncSource )
    {
    case NVVIOSYNCSOURCE_SDISYNC:
      l_vioConfig.fields |= NVVIOCONFIG_SYNCSOURCEENABLE;
      break;
    case NVVIOSYNCSOURCE_COMPSYNC:
      l_vioConfig.vioConfig.outConfig.compositeSyncType = NVVIOCOMPSYNCTYPE_AUTO;
      l_vioConfig.fields |= ( NVVIOCONFIG_SYNCSOURCEENABLE | NVVIOCONFIG_COMPOSITESYNCTYPE );
      break;
    } // switch
  }

  // Colorspace Conversion
  if ( options->cscEnable )
  {
    l_vioConfig.fields |= NVVIOCONFIG_CSCOVERRIDE;
    l_vioConfig.vioConfig.outConfig.cscOverride = TRUE;
    l_vioConfig.fields |= NVVIOCONFIG_COLORCONVERSION;

    // Offset
    l_vioConfig.vioConfig.outConfig.colorConversion.colorOffset[0] = options->cscOffset[0];
    l_vioConfig.vioConfig.outConfig.colorConversion.colorOffset[1] = options->cscOffset[1];
    l_vioConfig.vioConfig.outConfig.colorConversion.colorOffset[2] = options->cscOffset[2];

    // Scale
    l_vioConfig.vioConfig.outConfig.colorConversion.colorScale[0] = options->cscScale[0];
    l_vioConfig.vioConfig.outConfig.colorConversion.colorScale[1] = options->cscScale[1];
    l_vioConfig.vioConfig.outConfig.colorConversion.colorScale[2] = options->cscScale[2];
    l_vioConfig.vioConfig.outConfig.colorConversion.compositeSafe = TRUE;

    // Matrix
    l_vioConfig.vioConfig.outConfig.colorConversion.colorMatrix[0][0] = options->cscMatrix[0][0];
    l_vioConfig.vioConfig.outConfig.colorConversion.colorMatrix[0][1] = options->cscMatrix[0][1];
    l_vioConfig.vioConfig.outConfig.colorConversion.colorMatrix[0][2] = options->cscMatrix[0][2];
    l_vioConfig.vioConfig.outConfig.colorConversion.colorMatrix[1][0] = options->cscMatrix[1][0];
    l_vioConfig.vioConfig.outConfig.colorConversion.colorMatrix[1][1] = options->cscMatrix[1][1];
    l_vioConfig.vioConfig.outConfig.colorConversion.colorMatrix[1][2] = options->cscMatrix[1][2];
    l_vioConfig.vioConfig.outConfig.colorConversion.colorMatrix[2][0] = options->cscMatrix[2][0];
    l_vioConfig.vioConfig.outConfig.colorConversion.colorMatrix[2][1] = options->cscMatrix[2][1];
    l_vioConfig.vioConfig.outConfig.colorConversion.colorMatrix[2][2] = options->cscMatrix[2][2];
  }
  else
  {
    l_vioConfig.fields |= NVVIOCONFIG_CSCOVERRIDE;
    l_vioConfig.vioConfig.outConfig.cscOverride = FALSE;
  }

  // Gamma correction
  l_vioConfig.vioConfig.outConfig.gammaCorrection.version = NVVIOGAMMACORRECTION_VER;
  l_vioConfig.vioConfig.outConfig.gammaCorrection.fGammaValueR = options->gamma[0];
  l_vioConfig.vioConfig.outConfig.gammaCorrection.fGammaValueG = options->gamma[1];
  l_vioConfig.vioConfig.outConfig.gammaCorrection.fGammaValueB = options->gamma[2];
  l_vioConfig.fields |= NVVIOCONFIG_GAMMACORRECTION;

  // Set flip queue length
  l_vioConfig.fields |= NVVIOCONFIG_FLIPQUEUELENGTH;
  l_vioConfig.vioConfig.outConfig.flipQueueLength = options->flipQueueLength;

  // Set full color range
  l_vioConfig.fields |= NVVIOCONFIG_FULL_COLOR_RANGE;
  l_vioConfig.vioConfig.outConfig.enableFullColorRange = TRUE;

  // Set configuration.
  l_ret = NvAPI_VIO_SetConfig( m_vioHandle, ( NVVIOCONFIG* )&l_vioConfig );
  if ( l_ret != NVAPI_OK )
  {
    return E_FAIL;
  }

  //
  // Setup external sync here.
  //
  // Configure external sync parameters
  if ( options->syncEnable )
  {
    NvU32 l_wait;
    NVVIOSTATUS l_vioStatus;
    l_vioConfig.fields = 0; // reset fields

    // Trigger redetection of sync format
    if ( NvAPI_VIO_SyncFormatDetect( m_vioHandle, &l_wait ) != NVAPI_OK )
    {
      return E_FAIL;
    }

    // Wait for sync detection to complete
    Sleep( l_wait );

    // Get sync signal format
    l_vioStatus.version = NVVIOSTATUS_VER;
    if ( NvAPI_VIO_Status( m_vioHandle, &l_vioStatus ) != NVAPI_OK )
    {
      return E_FAIL;
    }

    // Verify that incoming sync signal is compatible outgoing video signal
    if ( !options->frameLock )
    {
      if ( l_vioStatus.vioStatus.outStatus.syncFormat !=
           l_vioConfig.vioConfig.outConfig.signalFormat )
      {
        LOG_ERROR( "Incoming sync does not match outgoing video signal." );
        return E_FAIL;
      }
      l_vioConfig.vioConfig.outConfig.frameLockEnable = FALSE;
      l_vioConfig.fields |= NVVIOCONFIG_FRAMELOCKENABLE;
    }
    else      // Framelock Case
    {
      NvU32 l_compatible;
      if ( NvAPI_VIO_IsFrameLockModeCompatible( m_vioHandle,
           l_vioStatus.vioStatus.outStatus.syncFormat,
           l_vioConfig.vioConfig.outConfig.signalFormat,
           &l_compatible ) != NVAPI_OK )
      {
        return E_FAIL;
      }

      if ( l_compatible )
      {
        l_vioConfig.vioConfig.outConfig.frameLockEnable = TRUE;
        l_vioConfig.fields |= NVVIOCONFIG_FRAMELOCKENABLE;
      }
      else
      {
        LOG_ERROR( "Incoming sync not compatible with outgoing video format." );
        return E_FAIL;
      }
    }

    l_vioConfig.vioConfig.outConfig.syncEnable = l_vioStatus.vioStatus.outStatus.syncEnable;
    l_vioConfig.vioConfig.outConfig.syncSource = l_vioStatus.vioStatus.outStatus.syncSource;

    switch( l_vioStatus.vioStatus.outStatus.syncSource )
    {
    case NVVIOSYNCSOURCE_SDISYNC:
      l_vioConfig.fields |= NVVIOCONFIG_SYNCSOURCEENABLE;
      break;
    case NVVIOSYNCSOURCE_COMPSYNC:
      l_vioConfig.vioConfig.outConfig.compositeSyncType = NVVIOCOMPSYNCTYPE_AUTO;
      l_vioConfig.fields |= ( NVVIOCONFIG_SYNCSOURCEENABLE | NVVIOCONFIG_COMPOSITESYNCTYPE );
      break;
    } // switch

    // Sync delay
    NVVIOSYNCDELAY l_vioSyncDelay;
    memset( &l_vioSyncDelay, 0, sizeof( l_vioSyncDelay ) );
    l_vioSyncDelay.version = NVVIOSYNCDELAY_VER;
    l_vioSyncDelay.horizontalDelay = options->hDelay;
    l_vioSyncDelay.verticalDelay = options->vDelay;
    l_vioConfig.fields |= NVVIOCONFIG_SYNCDELAY;
    l_vioConfig.vioConfig.outConfig.syncDelay = l_vioSyncDelay;

    // Setup external sync
    if ( NvAPI_VIO_SetConfig( m_vioHandle, ( NVVIOCONFIG* )&l_vioConfig ) != NVAPI_OK )
    {
      return E_FAIL;
    }
  }

  // Get video configuration.
  NVVIOSIGNALFORMATDETAIL l_vioSignalFormatDetail;
  memset( &l_vioSignalFormatDetail, 0, sizeof( l_vioSignalFormatDetail ) );

  ULONG l_vioSignalFormatIndex = ( ULONG )NVVIOSIGNALFORMAT_NONE;

  // Enumerate all signal formats until we find the one we are looking
  // for, the enumeration ends, or there is an error.
  while( 1 )
  {
    l_ret = NvAPI_VIO_EnumSignalFormats( m_vioHandle,
                                         l_vioSignalFormatIndex,
                                         &l_vioSignalFormatDetail );
    if ( l_ret == NVAPI_END_ENUMERATION || l_ret != NVAPI_OK )
    {
      return E_FAIL;
    }

    // We found the signal format that we were looking for so break.
    if ( l_vioSignalFormatDetail.signalFormat ==
         l_vioConfig.vioConfig.outConfig.signalFormat )
    {
      break;
    }

    l_vioSignalFormatIndex++;
  }

  // Set frame rate.  In the case of an interlaced signal format,
  // divide by 2.0 to calculate the frame rate.
  if ( l_vioSignalFormatDetail.videoMode.interlaceMode == NVVIOINTERLACEMODE_INTERLACE )
  {
    m_frameRate = l_vioSignalFormatDetail.videoMode.fFrameRate / 2.0f;
  }
  else
  {
    m_frameRate = l_vioSignalFormatDetail.videoMode.fFrameRate;
  }

  // Set width and height
  m_videoWidth = l_vioSignalFormatDetail.videoMode.horizontalPixels;
  m_videoHeight = l_vioSignalFormatDetail.videoMode.verticalLines;

  // Set interlace flag.
  if ( ( l_vioSignalFormatDetail.videoMode.interlaceMode == NVVIOINTERLACEMODE_INTERLACE )  ||
       ( l_vioSignalFormatDetail.videoMode.interlaceMode == NVVIOINTERLACEMODE_PSF ) )
  {
    m_bInterlaced = TRUE;
  }
  else
  {
    m_bInterlaced = FALSE;
  }

  return S_OK;
}

//----------------------------------------------------------------------------
HRESULT CNvSDIout::Cleanup()
{
  if ( NvAPI_VIO_Close( m_vioHandle, TRUE ) != NVAPI_OK )
  {
    return E_FAIL;
  }

  m_vioHandle = NULL;

  return S_OK;
}

//----------------------------------------------------------------------------
NvVioHandle CNvSDIout::GetHandle()
{
  return m_vioHandle;
}

//----------------------------------------------------------------------------
unsigned int CNvSDIout::GetWidth()
{
  return m_videoWidth;
}

//----------------------------------------------------------------------------
unsigned int CNvSDIout::GetHeight()
{
  return m_videoHeight;
}

//----------------------------------------------------------------------------
HRESULT CNvSDIout::DisplayVideoStatus()
{
  std::stringstream ss;

  // Get running state
  ss << "NvAPI_VIO_IsRunning(): ";

  if ( NvAPI_VIO_IsRunning( m_vioHandle ) == NVAPI_DRIVER_RUNNING )
  {
    ss << "NVAPI_DRIVER_RUNNING";
  }
  else if ( NvAPI_VIO_IsRunning( m_vioHandle ) == NVAPI_DRIVER_NOTRUNNING )
  {
    ss << "NVAPI_DRIVER_NOTRUNNING";
  }
  else
  {
    ss << "undefined";
  }

  // Query milliseconds to wait before getting incoming sync format
  NvU32 l_wait;

  // Trigger redetection of sync format
  if ( NvAPI_VIO_SyncFormatDetect( m_vioHandle, &l_wait ) != NVAPI_OK )
  {
    return E_FAIL;
  }

  // Wait for sync detection to complete
  Sleep( l_wait );

  // Get sync signal format
  NVVIOSTATUS l_vioStatus;
  l_vioStatus.version = NVVIOSTATUS_VER;
  if ( NvAPI_VIO_Status( m_vioHandle, &l_vioStatus ) != NVAPI_OK )
  {
    return E_FAIL;
  }

  ss << ". Video 1 Out: ";
  switch( l_vioStatus.vioStatus.outStatus.vid1Out )
  {
  case NVINPUTOUTPUTSTATUS_OFF:
    ss << "NVINPUTOUTPUTSTATUS_OFF";
    break;
  case NVINPUTOUTPUTSTATUS_ERROR:
    ss << "NVINPUTOUTPUTSTATUS_ERROR";
    break;
  case NVINPUTOUTPUTSTATUS_SDI_SD:
    ss << "NVINPUTOUTPUTSTATUS_SDI_SD";
    break;
  case NVINPUTOUTPUTSTATUS_SDI_HD:
    ss << "NVINPUTOUTPUTSTATUS_SDI_HD";
    break;
  default:
    ss << "undefined";
  }

  ss << ". Video 2 Out: ";
  switch( l_vioStatus.vioStatus.outStatus.vid2Out )
  {
  case NVINPUTOUTPUTSTATUS_OFF:
    ss << "NVINPUTOUTPUTSTATUS_OFF";
    break;
  case NVINPUTOUTPUTSTATUS_ERROR:
    ss << "NVINPUTOUTPUTSTATUS_ERROR";
    break;
  case NVINPUTOUTPUTSTATUS_SDI_SD:
    ss << "NVINPUTOUTPUTSTATUS_SDI_SD";
    break;
  case NVINPUTOUTPUTSTATUS_SDI_HD:
    ss << "NVINPUTOUTPUTSTATUS_SDI_HD";
    break;
  default:
    ss << "undefined";
  }

  if ( l_vioStatus.vioStatus.outStatus.syncEnable )
  {
    ss << ". Sync Source: ";
    switch( l_vioStatus.vioStatus.outStatus.syncSource )
    {
    case NVVIOSYNCSOURCE_SDISYNC:
      ss << "NVVIOSYNCSOURCE_SDISYNC";
      break;
    case NVVIOSYNCSOURCE_COMPSYNC:
      ss << "NVVIOSYNCSOURCE_COMPSYNC";
      break;
    default:
      ss << "undefined";
    }

    ss << ". Sync Format: ";
    switch( l_vioStatus.vioStatus.outStatus.syncFormat )
    {
    case NVVIOSIGNALFORMAT_NONE:
      ss << "NVVIOSIGNALFORMAT_NONE";
      break;
    case NVVIOSIGNALFORMAT_487I_59_94_SMPTE259_NTSC:
      ss << "NVVIOSIGNALFORMAT_487I_59_94_SMPTE259_NTSC";
      break;
    case NVVIOSIGNALFORMAT_576I_50_00_SMPTE259_PAL:
      ss << "NVVIOSIGNALFORMAT_576I_50_00_SMPTE259_PAL";
      break;
    case NVVIOSIGNALFORMAT_720P_59_94_SMPTE296:
      ss << "NVVIOSIGNALFORMAT_720P_59_94_SMPTE296";
      break;
    case NVVIOSIGNALFORMAT_720P_60_00_SMPTE296:
      ss << "NVVIOSIGNALFORMAT_720P_60_00_SMPTE296";
      break;
    case NVVIOSIGNALFORMAT_1035I_59_94_SMPTE260:
      ss << "NVVIOSIGNALFORMAT_1035I_59_94_SMPTE26";
      break;
    case NVVIOSIGNALFORMAT_1035I_60_00_SMPTE260:
      ss << "NVVIOSIGNALFORMAT_1035I_60_00_SMPTE260";
      break;
    case NVVIOSIGNALFORMAT_1080I_50_00_SMPTE295:
      ss << "NVVIOSIGNALFORMAT_1080I_50_00_SMPTE295";
      break;
    case NVVIOSIGNALFORMAT_1080I_50_00_SMPTE274:
      ss << "NVVIOSIGNALFORMAT_1080I_50_00_SMPTE274";
      break;
    case NVVIOSIGNALFORMAT_1080I_59_94_SMPTE274:
      ss << "NVVIOSIGNALFORMAT_1080I_59_94_SMPTE274";
      break;
    case NVVIOSIGNALFORMAT_1080I_60_00_SMPTE274:
      ss << "NVVIOSIGNALFORMAT_1080I_60_00_SMPTE274";
      break;
    case NVVIOSIGNALFORMAT_1080PSF_23_98_SMPTE274:
      ss << "NVVIOSIGNALFORMAT_1080PSF_23_98_SMPTE274";
      break;
    case NVVIOSIGNALFORMAT_1080PSF_24_00_SMPTE274:
      ss << "NVVIOSIGNALFORMAT_1080PSF_24_00_SMPTE274";
      break;
    case NVVIOSIGNALFORMAT_1080PSF_25_00_SMPTE274:
      ss << "NVVIOSIGNALFORMAT_1080PSF_25_00_SMPTE274";
      break;
    case NVVIOSIGNALFORMAT_1080P_23_976_SMPTE274:
      ss << "NVVIOSIGNALFORMAT_1080P_23_976_SMPTE274";
      break;
    case NVVIOSIGNALFORMAT_1080P_24_00_SMPTE274:
      ss << "NVVIOSIGNALFORMAT_1080P_24_00_SMPTE274";
      break;
    case NVVIOSIGNALFORMAT_1080P_25_00_SMPTE274:
      ss << "NVVIOSIGNALFORMAT_1080P_25_00_SMPTE274";
      break;
    case NVVIOSIGNALFORMAT_1080P_29_97_SMPTE274:
      ss << "NVVIOSIGNALFORMAT_1080P_29_97_SMPTE274";
      break;
    case NVVIOSIGNALFORMAT_1080P_30_00_SMPTE274:
      ss << "NVVIOSIGNALFORMAT_1080P_30_00_SMPTE274";
      break;
    default:
      ss << "undefined";
    }

    ss << ". Composite Sync In: ";
    switch( l_vioStatus.vioStatus.outStatus.compSyncIn )
    {
    case NVVIOSYNCSTATUS_OFF:
      ss << "NVVIOSYNCSTATUS_OFF";
      break;
    case NVVIOSYNCSTATUS_ERROR:
      ss << "NVVIOSYNCSTATUS_ERROR";
      break;
    case NVVIOSYNCSTATUS_SYNCLOSS:
      ss << "NVVIOSYNCSTATUS_SYNCLOSS";
      break;
    case NVVIOSYNCSTATUS_COMPOSITE:
      ss << "NVVIOSYNCSTATUS_COMPOSITE";
      break;
    case NVVIOSYNCSTATUS_SDI_SD:
      ss << "NVVIOSYNCSTATUS_SDI_SD";
      break;
    case NVVIOSYNCSTATUS_SDI_HD:
      ss << "NVVIOSYNCSTATUS_SDI_HD";
      break;
    }

    ss << ". SDI Sync In: ";
    switch( l_vioStatus.vioStatus.outStatus.sdiSyncIn )
    {
    case NVVIOSYNCSTATUS_OFF:
      ss << "NVVIOSYNCSTATUS_OFF";
      break;
    case NVVIOSYNCSTATUS_ERROR:
      ss << "NVVIOSYNCSTATUS_ERROR";
      break;
    case NVVIOSYNCSTATUS_SYNCLOSS:
      ss << "NVVIOSYNCSTATUS_SYNCLOSS";
      break;
    case NVVIOSYNCSTATUS_COMPOSITE:
      ss << "NVVIOSYNCSTATUS_COMPOSITE";
      break;
    case NVVIOSYNCSTATUS_SDI_SD:
      ss << "NVVIOSYNCSTATUS_SDI_SD";
      break;
    case NVVIOSYNCSTATUS_SDI_HD:
      ss << "NVVIOSYNCSTATUS_SDI_HD";
      break;
    }
  }
  else
  {
    ss << ". Sync Source: Disabled";
  }

  if ( l_vioStatus.vioStatus.outStatus.frameLockEnable )
  {
    ss << ". Framelock: Enabled";
  }
  else
  {
    ss << ". Framelock: Disabled";
  }

  if ( l_vioStatus.vioStatus.outStatus.outputVideoLocked )
  {
    ss << ". Output Video: Locked";
  }
  else
  {
    ss << ". Output Video: Not Locked";
  }

  LOG_INFO( ss.str() );

  return S_OK;
}

//----------------------------------------------------------------------------
HRESULT CNvSDIout::DisplaySignalFormatInfo()
{
  // Get signal format from GVO config.
  NVVIOCONFIG_V1 l_vioConfig;
  memset( &l_vioConfig, 0, sizeof( l_vioConfig ) );
  l_vioConfig.version = NVVIOCONFIG_VER1;
  l_vioConfig.fields = NVVIOCONFIG_SIGNALFORMAT;
  if ( NvAPI_VIO_GetConfig( m_vioHandle, ( NVVIOCONFIG* )&l_vioConfig ) != NVAPI_OK )
  {
    LOG_ERROR( "Unable to get video config." );
    return E_FAIL;
  }

  // Get signal format detail.
  NVVIOSIGNALFORMATDETAIL l_vioSignalFormatDetail;
  memset( &l_vioSignalFormatDetail, 0, sizeof( l_vioSignalFormatDetail ) );

  ULONG l_vioSignalFormatIndex = ( ULONG )NVVIOSIGNALFORMAT_NONE;

  // Enumerate all signal formats until we find the one we are looking
  // for, the enumeration ends, or there is an error.
  NvAPI_Status l_ret;
  while( 1 )
  {
    l_ret = NvAPI_VIO_EnumSignalFormats( m_vioHandle,
                                         l_vioSignalFormatIndex,
                                         &l_vioSignalFormatDetail );
    if ( l_ret == NVAPI_END_ENUMERATION || l_ret != NVAPI_OK )
    {
      return E_FAIL;
    } // if

    // We found the signal format that we were looking for so break.
    if ( l_vioSignalFormatDetail.signalFormat ==
         l_vioConfig.vioConfig.outConfig.signalFormat )
    {
      break;
    } // if

    l_vioSignalFormatIndex++;
  } // while

  std::stringstream ss;
  ss << SignalFormatToString( l_vioConfig.vioConfig.outConfig.signalFormat ).c_str();
  ss << ". Horizontal Pixels = " << l_vioSignalFormatDetail.videoMode.horizontalPixels;
  ss << ". Vertical Lines = " << l_vioSignalFormatDetail.videoMode.verticalLines;
  ss << ". Frame Rate = " << l_vioSignalFormatDetail.videoMode.fFrameRate;

  switch ( l_vioSignalFormatDetail.videoMode.interlaceMode )
  {
  case NVVIOINTERLACEMODE_PROGRESSIVE:
    ss << ". Interlace mode: Progressive";
    break;

  case NVVIOINTERLACEMODE_INTERLACE:
    ss << ". Interlace mode: Interlace";
    break;

  case NVVIOINTERLACEMODE_PSF:
    ss << ". Interlace mode: Progressive Segment Frame";
    break;
  }

  switch ( l_vioSignalFormatDetail.videoMode.videoStandard )
  {
  case NVVIOVIDEOSTANDARD_SMPTE259:
    ss << ". Video standard: SMPTE259";
    break;

  case NVVIOVIDEOSTANDARD_SMPTE260:
    ss << ". Video standard: SMPTE260";
    break;

  case NVVIOVIDEOSTANDARD_SMPTE274:
    ss << ". Video standard: SMPTE274";
    break;

  case NVVIOVIDEOSTANDARD_SMPTE295:
    ss << ". Video standard: SMPTE295";
    break;

  case NVVIOVIDEOSTANDARD_SMPTE296:
    ss << ". Video standard: SMPTE296";
    break;

  case NVVIOVIDEOSTANDARD_SMPTE372:
    ss << ". Video standard: SMPTE372";
    break;
  }

  switch ( l_vioSignalFormatDetail.videoMode.videoType )
  {
  case NVVIOVIDEOTYPE_HD:
    ss << ". Video type: High-Definition";
    break;

  case NVVIOVIDEOTYPE_SD:
    ss << ". Video type: Standard-Definition";
    break;
  }

  LOG_INFO( ss.str() );

  return S_OK;
}

//----------------------------------------------------------------------------
HRESULT CNvSDIout::DisplayDataFormatInfo()
{
  // Get data format from GVO config.
  NVVIOCONFIG_V1 l_vioConfig;
  memset( &l_vioConfig, 0, sizeof( l_vioConfig ) );
  l_vioConfig.version = NVVIOCONFIG_VER1;
  l_vioConfig.fields = NVVIOCONFIG_ALLFIELDS; //NVVIOCONFIG_DATAFORMAT;
  if ( NvAPI_VIO_GetConfig( m_vioHandle, ( NVVIOCONFIG* )&l_vioConfig ) != NVAPI_OK )
  {
    LOG_ERROR( "Unable to get video config." );
    return E_FAIL;
  }

  // Get data format detail.
  NVVIODATAFORMATDETAIL l_dataFormatDetail;
  if ( NvAPI_VIO_EnumDataFormats( m_vioHandle,
                                  l_vioConfig.vioConfig.outConfig.dataFormat,
                                  &l_dataFormatDetail ) != NVAPI_OK )
  {
    LOG_ERROR( "Unable to enum video data formats." );
    return E_FAIL;
  }

  std::stringstream ss;
  if ( l_dataFormatDetail.vioCaps != 0 )
  {
    if ( l_dataFormatDetail.vioCaps & NVVIOCAPS_VIDOUT_SDI )
    {
      ss << "VIDOUT_SDI" << std::endl;
    }
    if ( l_dataFormatDetail.vioCaps & NVVIOCAPS_OUTPUTMODE_DESKTOP )
    {
      ss << "OUTPUTMODE_DESKTOP" << std::endl;
    }
    if ( l_dataFormatDetail.vioCaps & NVVIOCAPS_OUTPUTMODE_OPENGL )
    {
      ss << "OUTPUTMODE_OPENGL" << std::endl;
    }
    if ( l_dataFormatDetail.vioCaps & NVVIOCAPS_SYNC_INTERNAL )
    {
      ss << "SYNC_INTERNAL" << std::endl;
    }
    if ( l_dataFormatDetail.vioCaps & NVVIOCAPS_SYNC_GENLOCK )
    {
      ss << "SYNC_GENLOCK" << std::endl;
    }
    if ( l_dataFormatDetail.vioCaps & NVVIOCAPS_SYNCSRC_SDI )
    {
      ss << "SYNCSRC_SDI" << std::endl;
    }
    if ( l_dataFormatDetail.vioCaps & NVVIOCAPS_SYNCSRC_COMP )
    {
      ss << "SYNCSRC_COMP" << std::endl;
    }
  }

  ss << DataFormatToString( l_dataFormatDetail.dataFormat ).c_str();

  LOG_INFO( ss.str() );

  return S_OK;
}

//----------------------------------------------------------------------------
HRESULT CNvSDIout::GetFrameRate( float* rate )
{
  *rate = 0.0f;

  // Get signal format from GVO config.
  NVVIOCONFIG_V1 l_vioConfig;
  memset( &l_vioConfig, 0, sizeof( l_vioConfig ) );
  l_vioConfig.version = NVVIOCONFIG_VER1;
  l_vioConfig.fields = NVVIOCONFIG_SIGNALFORMAT;
  if ( NvAPI_VIO_GetConfig( m_vioHandle, ( NVVIOCONFIG* )&l_vioConfig ) != NVAPI_OK )
  {
    LOG_ERROR( "Unable to get video config." );
    return E_FAIL;
  }

  // Enumerate all signal formats until we find the one we are looking
  // for, the enumeration ends, or there is an error.
  NVVIOSIGNALFORMATDETAIL l_vioSignalFormatDetail;
  memset( &l_vioSignalFormatDetail, 0, sizeof( l_vioSignalFormatDetail ) );

  ULONG l_vioSignalFormatIndex = ( ULONG )NVVIOSIGNALFORMAT_NONE;

  while( 1 )
  {
    NvAPI_Status l_ret = NVAPI_OK;
    l_ret = NvAPI_VIO_EnumSignalFormats( m_vioHandle,
                                         l_vioSignalFormatIndex,
                                         &l_vioSignalFormatDetail );
    if ( l_ret == NVAPI_END_ENUMERATION || l_ret != NVAPI_OK )
    {
      return E_FAIL;
    }

    // We found the signal format that we were looking for so break.
    if ( l_vioSignalFormatDetail.signalFormat ==
         l_vioConfig.vioConfig.outConfig.signalFormat )
    {
      break;
    }

    l_vioSignalFormatIndex++;
  }

  // Set rate
  *rate = l_vioSignalFormatDetail.videoMode.fFrameRate;

  return S_OK;
}

//----------------------------------------------------------------------------
float CNvSDIout::GetFrameRate()
{
  return m_frameRate;
}

//----------------------------------------------------------------------------
BOOL CNvSDIout::IsInterlaced()
{
  return m_bInterlaced;
}

//----------------------------------------------------------------------------
HRESULT CNvSDIout::SetCSC( NVVIOCOLORCONVERSION* csc, bool enable )
{
  NVVIOCONFIG_V1 l_vioConfig;
  NvAPI_Status l_ret;

  memset( &l_vioConfig, 0, sizeof( l_vioConfig ) );
  l_vioConfig.version = NVVIOCONFIG_VER1;

  l_vioConfig.fields = NVVIOCONFIG_CSCOVERRIDE;

  // If not enabled, simply set as disabled.
  // Otherwise, enable and set values.
  if ( !enable )
  {
    l_vioConfig.vioConfig.outConfig.cscOverride = FALSE;
  }
  else
  {
    l_vioConfig.vioConfig.outConfig.cscOverride = TRUE;

    l_vioConfig.fields |= NVVIOCONFIG_COLORCONVERSION;

    // Offset
    l_vioConfig.vioConfig.outConfig.colorConversion.colorOffset[0] = csc->colorOffset[0];
    l_vioConfig.vioConfig.outConfig.colorConversion.colorOffset[1] = csc->colorOffset[1];
    l_vioConfig.vioConfig.outConfig.colorConversion.colorOffset[2] = csc->colorOffset[2];

    // Scale
    l_vioConfig.vioConfig.outConfig.colorConversion.colorScale[0] = csc->colorScale[0];
    l_vioConfig.vioConfig.outConfig.colorConversion.colorScale[1] = csc->colorScale[1];
    l_vioConfig.vioConfig.outConfig.colorConversion.colorScale[2] = csc->colorScale[2];
    l_vioConfig.vioConfig.outConfig.colorConversion.compositeSafe = TRUE;

    // Matrix
    l_vioConfig.vioConfig.outConfig.colorConversion.colorMatrix[0][0] = csc->colorMatrix[0][0];
    l_vioConfig.vioConfig.outConfig.colorConversion.colorMatrix[0][1] = csc->colorMatrix[0][1];
    l_vioConfig.vioConfig.outConfig.colorConversion.colorMatrix[0][2] = csc->colorMatrix[0][2];
    l_vioConfig.vioConfig.outConfig.colorConversion.colorMatrix[1][0] = csc->colorMatrix[1][0];
    l_vioConfig.vioConfig.outConfig.colorConversion.colorMatrix[1][1] = csc->colorMatrix[1][1];
    l_vioConfig.vioConfig.outConfig.colorConversion.colorMatrix[1][2] = csc->colorMatrix[1][2];
    l_vioConfig.vioConfig.outConfig.colorConversion.colorMatrix[2][0] = csc->colorMatrix[2][0];
    l_vioConfig.vioConfig.outConfig.colorConversion.colorMatrix[2][1] = csc->colorMatrix[2][1];
    l_vioConfig.vioConfig.outConfig.colorConversion.colorMatrix[2][2] = csc->colorMatrix[2][2];
  }

  // Set configuration.
  l_ret = NvAPI_VIO_SetConfig( m_vioHandle, ( NVVIOCONFIG* )&l_vioConfig );
  if ( l_ret != NVAPI_OK )
  {
    // Display NVAPI error.
    NvAPI_ShortString l_desc;
    NvAPI_GetErrorMessage( l_ret, l_desc );
    LOG_ERROR( l_desc );

    return E_FAIL;
  }

  return S_OK;
}

//----------------------------------------------------------------------------
HRESULT CNvSDIout::GetCSC( NVVIOCOLORCONVERSION* csc, bool* enable )
{
  NVVIOCONFIG_V1 l_vioConfig;
  NvAPI_Status l_ret;

  memset( &l_vioConfig, 0, sizeof( l_vioConfig ) );
  l_vioConfig.version = NVVIOCONFIG_VER1;
  l_vioConfig.fields = NVVIOCONFIG_COLORCONVERSION | NVVIOCONFIG_CSCOVERRIDE;

  // Get configuration.
  l_ret = NvAPI_VIO_GetConfig( m_vioHandle, ( NVVIOCONFIG* )&l_vioConfig );
  if ( l_ret != NVAPI_OK )
  {
    return E_FAIL;
  }

  // Enable / disable state
  *enable = l_vioConfig.vioConfig.outConfig.cscOverride;

  // Offset
  csc->colorOffset[0] = l_vioConfig.vioConfig.outConfig.colorConversion.colorOffset[0];
  csc->colorOffset[1] = l_vioConfig.vioConfig.outConfig.colorConversion.colorOffset[1];
  csc->colorOffset[2] = l_vioConfig.vioConfig.outConfig.colorConversion.colorOffset[2];

  // Scale
  csc->colorScale[0] = l_vioConfig.vioConfig.outConfig.colorConversion.colorScale[0];
  csc->colorScale[1] = l_vioConfig.vioConfig.outConfig.colorConversion.colorScale[1];
  csc->colorScale[2] = l_vioConfig.vioConfig.outConfig.colorConversion.colorScale[2];

  // Composite safe?
  csc->compositeSafe = l_vioConfig.vioConfig.outConfig.colorConversion.compositeSafe;

  // Matrix
  csc->colorMatrix[0][0] = l_vioConfig.vioConfig.outConfig.colorConversion.colorMatrix[0][0];
  csc->colorMatrix[0][1] = l_vioConfig.vioConfig.outConfig.colorConversion.colorMatrix[0][1];
  csc->colorMatrix[0][2] = l_vioConfig.vioConfig.outConfig.colorConversion.colorMatrix[0][2];
  csc->colorMatrix[1][0] = l_vioConfig.vioConfig.outConfig.colorConversion.colorMatrix[1][0];
  csc->colorMatrix[1][1] = l_vioConfig.vioConfig.outConfig.colorConversion.colorMatrix[1][1];
  csc->colorMatrix[1][2] = l_vioConfig.vioConfig.outConfig.colorConversion.colorMatrix[1][2];
  csc->colorMatrix[2][0] = l_vioConfig.vioConfig.outConfig.colorConversion.colorMatrix[2][0];
  csc->colorMatrix[2][1] = l_vioConfig.vioConfig.outConfig.colorConversion.colorMatrix[2][1];
  csc->colorMatrix[2][2] = l_vioConfig.vioConfig.outConfig.colorConversion.colorMatrix[2][2];

  return S_OK;
}

//----------------------------------------------------------------------------
HRESULT CNvSDIout::SetSyncDelay( NVVIOSYNCDELAY* delay )
{
  NVVIOCONFIG_V1 l_vioConfig;
  NvAPI_Status l_ret;

  memset( &l_vioConfig, 0, sizeof( l_vioConfig ) );
  l_vioConfig.version = NVVIOCONFIG_VER1;
  l_vioConfig.fields = NVVIOCONFIG_SYNCDELAY;

  // Set delay
  l_vioConfig.vioConfig.outConfig.syncDelay.version = delay->version;
  l_vioConfig.vioConfig.outConfig.syncDelay.horizontalDelay = delay->horizontalDelay;
  l_vioConfig.vioConfig.outConfig.syncDelay.verticalDelay = delay->verticalDelay;

  // Set configuration.
  l_ret = NvAPI_VIO_SetConfig( m_vioHandle, ( NVVIOCONFIG* )&l_vioConfig );
  if ( l_ret != NVAPI_OK )
  {
    return E_FAIL;
  }

  return S_OK;
}

//----------------------------------------------------------------------------
HRESULT CNvSDIout::GetSyncDelay( NVVIOSYNCDELAY* delay )
{
  NVVIOCONFIG_V1 l_vioConfig;
  NvAPI_Status l_ret;

  memset( &l_vioConfig, 0, sizeof( l_vioConfig ) );
  l_vioConfig.version = NVVIOCONFIG_VER1;
  l_vioConfig.fields = NVVIOCONFIG_SYNCDELAY;

  // Get configuration.
  l_ret = NvAPI_VIO_GetConfig( m_vioHandle, ( NVVIOCONFIG* )&l_vioConfig );
  if ( l_ret != NVAPI_OK )
  {
    return E_FAIL;
  }

  // Get delay
  delay->version = l_vioConfig.vioConfig.outConfig.syncDelay.version;
  delay->horizontalDelay = l_vioConfig.vioConfig.outConfig.syncDelay.horizontalDelay;
  delay->verticalDelay = l_vioConfig.vioConfig.outConfig.syncDelay.verticalDelay;

  return S_OK;
}

//----------------------------------------------------------------------------
CNvSDIoutGpuTopology::CNvSDIoutGpuTopology()
{
  m_bInitialized = false;
  m_nGpu = 0;
  if( Init() )
  {
    m_bInitialized = true;
  }
}

//----------------------------------------------------------------------------
CNvSDIoutGpuTopology::~CNvSDIoutGpuTopology()
{
  if( m_bInitialized == false )
  {
    return;
  }
  for( int i = 0; i < m_nGpu; i++ )
  {
    delete dynamic_cast<CNvSDIoutGpu*>( m_lGpu[i] );
    m_lGpu[i] = NULL;
  }
}

//----------------------------------------------------------------------------
bool CNvSDIoutGpuTopology::Init()
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

  // Query available VIO topologies.
  // Initialize NVAPI
  if ( NvAPI_Initialize() != NVAPI_OK )
  {
    return false;
  }
  // Fail if there are no VIO topologies or devices available.
  NVVIOTOPOLOGY l_vioTopologies;
  memset( &l_vioTopologies, 0, sizeof( l_vioTopologies ) );
  l_vioTopologies.version = NVVIOTOPOLOGY_VER;

  if ( NvAPI_VIO_QueryTopology( &l_vioTopologies ) != NVAPI_OK )
  {
    return false;
  }

  if ( l_vioTopologies.vioTotalDeviceCount == 0 )
  {
    return false;
  }

  LOG_INFO( "Listing GPUs available for OpenGL GPU Affinity" );

  unsigned int GPUIdx = 0;

  GPU_DEVICE gpuDevice;
  gpuDevice.cb = sizeof( gpuDevice );
  HGPUNV hGPU;
  bool bDisplay;
  bool bPrimary;
  bool bSDIoutput;
  NvVioHandle hVioHandle;
  while( wglEnumGpusNV( GPUIdx, &hGPU ) ) // First call this function to get a handle to the gpu
  {
    // wglEnumPGUsNV will fails if DeviceIdx > the available devices
    LOG_DEBUG( "GPU# " << GPUIdx << ":" );
    bDisplay = false;
    bPrimary = false;
    bSDIoutput = false;
    hVioHandle = NULL;

    // Now get the detailed information about this device:
    //    How many displays it's attached to and whether any of them
    int DisplayDeviceIdx = 0;
    while( wglEnumGpuDevicesNV( hGPU, DisplayDeviceIdx, &gpuDevice ) )
    {
      LOG_DEBUG( " Display# " << DisplayDeviceIdx );
      LOG_DEBUG( "   Name: " << gpuDevice.DeviceName );
      LOG_DEBUG( "   String: " << gpuDevice.DeviceString );

      bDisplay = true;

      if ( gpuDevice.Flags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP )
      {
        LOG_DEBUG( "   Attached to the desktop: LEFT=" << gpuDevice.rcVirtualScreen.left << ", RIGHT=" << gpuDevice.rcVirtualScreen.right
                   << ", TOP=" << gpuDevice.rcVirtualScreen.top << ", BOTTOM=" << gpuDevice.rcVirtualScreen.bottom );
      }
      else
      {
        LOG_DEBUG( "   Not attached to the desktop." );
      }

      // See if it's the primary GPU
      if ( gpuDevice.Flags & DISPLAY_DEVICE_PRIMARY_DEVICE )
      {
        LOG_DEBUG( "   This is the PRIMARY Display Device." );
        bPrimary = true;
      }
      DisplayDeviceIdx++;

      NvDisplayHandle hNvDisplay;
      NvAPI_Status status;
      status = NvAPI_GetAssociatedNvidiaDisplayHandle( gpuDevice.DeviceName, &hNvDisplay );
      if( status != NVAPI_OK )
      {
        break;
      }

      NvU32 count = 0;
      NvPhysicalGpuHandle hNvPhysicalGPU;
      status = NvAPI_GetPhysicalGPUsFromDisplay( hNvDisplay, &hNvPhysicalGPU, &count );
      if( status != NVAPI_OK )
      {
        break;
      }

      // Cycle through all SDI topologies looking for the first
      // available SDI output device topology.
      BOOL l_bFound = FALSE;
      unsigned int i = 0;
      NVVIOCAPS l_vioCaps;
      while ( ( i < l_vioTopologies.vioTotalDeviceCount ) && ( !l_bFound ) )
      {
        if( l_vioTopologies.vioTarget[i].hPhysicalGpu == hNvPhysicalGPU )
        {
          // Get video I/O capabilities for current video I/O target.
          memset( &l_vioCaps, 0, sizeof( l_vioCaps ) );
          l_vioCaps.version = NVVIOCAPS_VER;
          if ( NvAPI_VIO_GetCapabilities( l_vioTopologies.vioTarget[i].hVioHandle,
                                          &l_vioCaps ) != NVAPI_OK )
          {
            continue;
          }

          // If video output device found, save VIO handle and set flag.
          if ( l_vioCaps.adapterCaps & NVVIOCAPS_VIDOUT_SDI )
          {
            bSDIoutput = true;
            hVioHandle = l_vioTopologies.vioTarget[i].hVioHandle;
            l_bFound = TRUE;
          }
          else
          {
            i++;
          }
        }
        else
        {
          i++;
        }
      }

      if( l_bFound )
      {
        LOG_DEBUG( "   SDI out: yes" );
      }
      else
      {
        LOG_DEBUG( "   SDI out: no" );
      }
      if( gpuDevice.Flags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP )
      {
        LOG_DEBUG( "   Attached to the desktop: LEFT=" << gpuDevice.rcVirtualScreen.left << ", RIGHT=" << gpuDevice.rcVirtualScreen.right
                   << ", TOP=" << gpuDevice.rcVirtualScreen.top << ", BOTTOM=" << gpuDevice.rcVirtualScreen.bottom );
      }
      else
      {
        LOG_DEBUG( "   Not attached to the desktop." );
      }
    }
    CNvSDIoutGpu* gpu = new CNvSDIoutGpu();
    gpu->Init( hGPU, bPrimary, bDisplay, bSDIoutput, hVioHandle );

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
CNvSDIoutGpuTopology& CNvSDIoutGpuTopology::Instance()
{
  static CNvSDIoutGpuTopology instance;
  instance.Init();
  return instance;
}

//----------------------------------------------------------------------------
CNvSDIoutGpu* CNvSDIoutGpuTopology::GetGpu( int index )
{
  if( index >= 0 && index < m_nGpu )
  {
    return dynamic_cast<CNvSDIoutGpu*>( m_lGpu[index] );
  }
  return NULL;
}

//----------------------------------------------------------------------------
CNvSDIoutGpu* CNvSDIoutGpuTopology::GetPrimaryGpu()
{
  for( int i = 0; i < m_nGpu; i++ )
  {
    if( m_lGpu[i]->isPrimary() )
    {
      return dynamic_cast<CNvSDIoutGpu*>( m_lGpu[i] );
    }
  }
  return NULL;
}

//----------------------------------------------------------------------------
CNvSDIoutGpu::CNvSDIoutGpu()
{

}

//----------------------------------------------------------------------------
CNvSDIoutGpu::~CNvSDIoutGpu()
{

}

//----------------------------------------------------------------------------
bool CNvSDIoutGpu::Init( HGPUNV gpuAffinityHandle, bool bPrimary, bool bDisplay, bool bSDIoutput, NvVioHandle hVioHandle )
{
  CNvGpu::Init( gpuAffinityHandle, bPrimary, bDisplay );
  m_bSDIoutput = bSDIoutput;
  m_hVioHandle = hVioHandle;
  return true;
}

//----------------------------------------------------------------------------
bool CNvSDIoutGpu::IsSDIoutput()
{
  return m_bSDIoutput;
}

//----------------------------------------------------------------------------
NvVioHandle CNvSDIoutGpu::GetVioHandle()
{
  return m_hVioHandle;
}
