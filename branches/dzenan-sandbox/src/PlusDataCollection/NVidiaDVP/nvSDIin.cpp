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
#include "nvGPUutil.h"
#include "nvSDIin.h"
#include "nvSDIutil.h"

//----------------------------------------------------------------------------
CNvSDIinTopology::CNvSDIinTopology()
{
  m_bInitialized = false;
  m_nDevice = 0;
  if( Init() )
  {
    m_bInitialized = true;
  }
}

//----------------------------------------------------------------------------
CNvSDIinTopology::~CNvSDIinTopology()
{
  if( !m_bInitialized )
  {
    return;
  }
  for( int i = 0; i < m_nDevice; i++ )
  {
    delete m_lDevice[i];
    m_lDevice[i] = NULL;
  }
}

//----------------------------------------------------------------------------
CNvSDIinTopology& CNvSDIinTopology::Instance()
{
  static CNvSDIinTopology instance;
  return instance;
}

//----------------------------------------------------------------------------
int CNvSDIinTopology::GetNumDevice()
{
  return m_nDevice;
}

//----------------------------------------------------------------------------
NVVIOTOPOLOGYTARGET* CNvSDIinTopology::GetDevice( int index )
{
  if( index >= 0 && index < m_nDevice )
  {
    return m_lDevice[index];
  }
  return NULL;
}

//----------------------------------------------------------------------------
bool CNvSDIinTopology::Init()
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

  //load all the required extensions:
  //video capture
  if( !loadCaptureVideoExtension() )
  {
    LOG_ERROR( "Could not load OpenGL Video Capture extension." );
    return false;
  }
  //timer query
  if( !loadTimerQueryExtension() )
  {
    LOG_ERROR( "Could not load OpenGL timer query extension." );
    return false;
  }
  //timer query
  if( !loadBufferObjectExtension() )
  {
    LOG_ERROR( "Could not load OpenGL buffer object extension." );
    return false;
  }

  NVVIOCAPS l_vioCaps;
  NVVIOTOPOLOGY l_vioTopos;
  NvAPI_Status ret = NVAPI_OK;

  // NVAPI Initialization of Video Capture Device.
  if ( NvAPI_Initialize() != NVAPI_OK )
  {
    LOG_ERROR( "Error Initializing NVAPI." );
    return false;
  }

  // Query Available Video I/O Topologies
  memset( &l_vioTopos, 0, sizeof( l_vioTopos ) );
  l_vioTopos.version = NVVIOTOPOLOGY_VER;
  if ( NvAPI_VIO_QueryTopology( &l_vioTopos ) != NVAPI_OK )
  {
    LOG_ERROR( "Video I/O Unsupported." );
    return false;
  }

  // Cycle through all SDI topologies looking for the first
  // available SDI input device.
  unsigned int i = 0;
  m_nDevice = 0;
  while ( i < l_vioTopos.vioTotalDeviceCount )
  {
    // Get video I/O capabilities for current video I/O target.
    memset( &l_vioCaps, 0, sizeof( l_vioCaps ) );
    l_vioCaps.version = NVVIOCAPS_VER;
    if ( NvAPI_VIO_GetCapabilities( l_vioTopos.vioTarget[i].hVioHandle, &l_vioCaps ) != NVAPI_OK )
    {
      ++i;
      continue;
    }

    // If video input device found, set flag.
    if ( l_vioCaps.adapterCaps & NVVIOCAPS_VIDIN_SDI )
    {
      m_lDevice[m_nDevice] = new NVVIOTOPOLOGYTARGET;
      *m_lDevice[m_nDevice] = l_vioTopos.vioTarget[i];
      m_nDevice++;
    }

    i++;
  }

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
CNvSDIin::CNvSDIin()
{
  m_device = NULL;
  m_hDC = NULL;
  m_vioHandle = NULL;
  m_videoSlot = 0;
  // Setup CSC for each stream.

  float scale = 1.0f;

  m_cscMax[0] = 5000;
  m_cscMax[1] = 5000;
  m_cscMax[2] = 5000;
  m_cscMax[3] = 5000;
  m_cscMin[0] = 0;
  m_cscMin[1] = 0;
  m_cscMin[2] = 0;
  m_cscMin[3] = 0;

  // Initialize matrix to the identity.
  m_cscMat[0][0] = scale;
  m_cscMat[0][1] = 0;
  m_cscMat[0][2] = 0;
  m_cscMat[0][3] = 0;
  m_cscMat[1][0] = 0;
  m_cscMat[1][1] = scale;
  m_cscMat[1][2] = 0;
  m_cscMat[1][3] = 0;
  m_cscMat[2][0] = 0;
  m_cscMat[2][1] = 0;
  m_cscMat[2][2] = scale;
  m_cscMat[2][3] = 0;
  m_cscMat[3][0] = 0;
  m_cscMat[3][1] = 0;
  m_cscMat[3][2] = 0;
  m_cscMat[3][3] = scale;

  m_bCaptureStarted = false;
}

//----------------------------------------------------------------------------
CNvSDIin::~CNvSDIin()
{
}

//----------------------------------------------------------------------------
void CNvSDIin::SetCSCParams( GLfloat* cscMat, GLfloat* cscOffset, GLfloat* cscMin, GLfloat* cscMax )
{
  memcpy( m_cscMat, cscMat, sizeof( GLfloat ) * 16 );
  memcpy( m_cscOffset, cscOffset, sizeof( GLfloat ) * 4 );
  memcpy( m_cscMin, cscMin, sizeof( GLfloat ) * 4 );
  memcpy( m_cscMax, cscMax, sizeof( GLfloat ) * 4 );
}

//----------------------------------------------------------------------------
NvVioHandle CNvSDIin::GetVioHandle()
{
  return m_vioHandle;
}

//----------------------------------------------------------------------------
unsigned int CNvSDIin::GetWidth()
{
  return m_videoWidth;
}

//----------------------------------------------------------------------------
unsigned int CNvSDIin::GetHeight()
{
  return m_videoHeight;
}

//----------------------------------------------------------------------------
void CNvSDIin::DumpChannelStatus( NVVIOCHANNELSTATUS jack )
{
  LOG_INFO( "Link ID: " << LinkIDToString( jack.linkID ).c_str() );
  LOG_INFO( "SMPTE 352: " << jack.smpte352 );
  LOG_INFO( "Signal Format: " << SignalFormatToString( jack.signalFormat ).c_str() );
  LOG_INFO( "Sampling Format: " << ComponentSamplingFormatToString( jack.samplingFormat ).c_str() );
  LOG_INFO( "Color Space: " << ColorSpaceToString( jack.colorSpace ).c_str() );
  LOG_INFO( "Bits Per Component: " << jack.bitsPerComponent );
  LOG_INFO( "SMPTE 352 Payload ID: " << jack.smpte352 );
}

//----------------------------------------------------------------------------
void CNvSDIin::DumpStreamStatus( NVVIOSTREAM stream )
{
  LOG_INFO( "Links: " );
  for( unsigned int i = 0; i < stream.numLinks; i++ )
  {
    LOG_INFO( "Jack: " << stream.links[i].jack << " Channel: " << stream.links[i].channel );
  }
  LOG_INFO( "Sampling: " << ComponentSamplingFormatToString( stream.sampling ).c_str() );
  LOG_INFO( "Expansion Enable: " << stream.expansionEnable );
  LOG_INFO( "Bits Per Component: " << stream.bitsPerComponent );
}

//----------------------------------------------------------------------------
HRESULT CNvSDIin::Init( nvOptions* options )
{
  if( options )
  {
    m_bDualLink = options->dualLink;
    m_Sampling = options->sampling;
    m_BitsPerComponent = options->bitsPerComponent;
    m_ExpansionEnable = options->expansionEnable;
    if( m_Sampling == NVVIOCOMPONENTSAMPLING_444 || m_Sampling == NVVIOCOMPONENTSAMPLING_4444 )
    {
      m_ExpansionEnable = false;
    }
  }
  else
  {
    m_bDualLink = false;
    m_Sampling = NVVIOCOMPONENTSAMPLING_422;
    m_BitsPerComponent = 8;
    m_ExpansionEnable = true;
  }

  return S_OK;
}

//----------------------------------------------------------------------------
HRESULT CNvSDIin::GetVideoInState( NVVIOCONFIG_V1* vioConfig, NVVIOSTATUS* vioStatus )
{
  NvAPI_Status ret = NVAPI_OK;

  memset( vioStatus, 0, sizeof( NVVIOSTATUS ) );
  vioStatus->version = NVVIOSTATUS_VER;
  if ( NvAPI_VIO_Status( m_vioHandle, vioStatus ) != NVAPI_OK )
  {
    LOG_ERROR( "Cannot get status of SDI input device." );
    return E_FAIL;
  }

  // Cycle through the jacks and display the status of each active channel.
  for ( unsigned int i = 0; i < NVAPI_MAX_VIO_JACKS; i++ )
  {
    for ( unsigned int j = 0; j < NVAPI_MAX_VIO_CHANNELS_PER_JACK; j++ )
    {
      DumpChannelStatus( vioStatus->vioStatus.inStatus.vidIn[i][j] );
    }
  }

  // Get stream configuration
  memset( vioConfig, 0, sizeof( NVVIOCONFIG_V1 ) );
  vioConfig->version = NVVIOCONFIG_VER1;
  vioConfig->nvvioConfigType = NVVIOCONFIGTYPE_IN;
  vioConfig->fields = NVVIOCONFIG_SIGNALFORMAT | NVVIOCONFIG_STREAMS;
  if ( NvAPI_VIO_GetConfig( m_vioHandle, ( NVVIOCONFIG* )vioConfig ) != NVAPI_OK )
  {
    LOG_ERROR( "Cannot get configuration of SDI input device." );
    return E_FAIL;
  }

  // Display stream configuration of input device.
  LOG_DEBUG( "Number of Streams: " << vioConfig->vioConfig.inConfig.numStreams );
  LOG_DEBUG( "Signal Format: " << SignalFormatToString( vioConfig->vioConfig.inConfig.signalFormat ).c_str() );

  // Display the configuration of each stream.
  for ( unsigned int i = 0; i < vioConfig->vioConfig.inConfig.numStreams; i++ )
  {
    DumpStreamStatus( vioConfig->vioConfig.inConfig.streams[i] );
  }

  return S_OK;
}

//----------------------------------------------------------------------------
HRESULT CNvSDIin::SetVideoConfig()
{
  NVVIOCONFIG_V1 l_vioConfig;
  NVVIOSTATUS l_vioStatus;
  // Get initial device state.
  GetVideoInState( &l_vioConfig, &l_vioStatus );

  // Calculate the number of active streams.  For now, this is the
  // number of streams that we will draw.  3G formats can have upto
  // two channels per jack.  For now, only look at the first channel.
  m_numStreams = 0;
  for ( unsigned int i = 0; i < NVAPI_MAX_VIO_JACKS; i++ )
  {
    if ( l_vioStatus.vioStatus.inStatus.vidIn[i][0].signalFormat != NVVIOSIGNALFORMAT_NONE )
    {
      m_activeJacks[m_numStreams] = i;
      m_numStreams++;
    }
  }

  // Return an error if there are no active streams detected.
  if ( m_numStreams == 0 )
  {
    LOG_ERROR( "No active video input input streams detected." );
    return E_FAIL;
  }

  // Now, set the config that we really want here.
  memset( &l_vioConfig, 0, sizeof( l_vioConfig ) );
  l_vioConfig.version = NVVIOCONFIG_VER1;
  l_vioConfig.nvvioConfigType = NVVIOCONFIGTYPE_IN;

  // Set signal format for capture to the detected signal format.
  // on the first channel of the first active jack.
  for ( unsigned int i = 0; i < NVAPI_MAX_VIO_JACKS; i++ )
  {
    for ( unsigned int j = 0; j < NVAPI_MAX_VIO_CHANNELS_PER_JACK; j++ )
    {
      if ( l_vioStatus.vioStatus.inStatus.vidIn[i][j].signalFormat != NVVIOSIGNALFORMAT_NONE )
      {
        l_vioConfig.vioConfig.inConfig.signalFormat = l_vioStatus.vioStatus.inStatus.vidIn[i][j].signalFormat;
      }
    }
  }

  int numLinks = 1;
  if( m_bDualLink )
  {
    m_numStreams >>= 1;
    numLinks = 2;
  }

  l_vioConfig.fields = NVVIOCONFIG_SIGNALFORMAT;

  // Define streams.
  l_vioConfig.vioConfig.inConfig.numStreams = m_numStreams;

  l_vioConfig.fields |= NVVIOCONFIG_STREAMS;
  l_vioConfig.vioConfig.inConfig.numRawCaptureImages = NVAPI_GVI_DEFAULT_RAW_CAPTURE_IMAGES; //////

  switch( l_vioConfig.vioConfig.inConfig.signalFormat )
  {
  case NVVIOSIGNALFORMAT_1080P_50_00_SMPTE274_3G_LEVEL_A:
  case NVVIOSIGNALFORMAT_1080P_59_94_SMPTE274_3G_LEVEL_A:
  case NVVIOSIGNALFORMAT_1080P_60_00_SMPTE274_3G_LEVEL_A:
  case NVVIOSIGNALFORMAT_1080P_60_00_SMPTE274_3G_LEVEL_B:
  case NVVIOSIGNALFORMAT_1080I_60_00_SMPTE274_3G_LEVEL_B:
  case NVVIOSIGNALFORMAT_2048I_60_00_SMPTE372_3G_LEVEL_B:
  case NVVIOSIGNALFORMAT_1080P_50_00_SMPTE274_3G_LEVEL_B:
  case NVVIOSIGNALFORMAT_1080I_50_00_SMPTE274_3G_LEVEL_B:
  case NVVIOSIGNALFORMAT_2048I_50_00_SMPTE372_3G_LEVEL_B:
  case NVVIOSIGNALFORMAT_1080P_30_00_SMPTE274_3G_LEVEL_B:
  case NVVIOSIGNALFORMAT_2048P_30_00_SMPTE372_3G_LEVEL_B:
  case NVVIOSIGNALFORMAT_1080P_25_00_SMPTE274_3G_LEVEL_B:
  case NVVIOSIGNALFORMAT_2048P_25_00_SMPTE372_3G_LEVEL_B:
  case NVVIOSIGNALFORMAT_1080P_24_00_SMPTE274_3G_LEVEL_B:
  case NVVIOSIGNALFORMAT_2048P_24_00_SMPTE372_3G_LEVEL_B:
  case NVVIOSIGNALFORMAT_1080I_48_00_SMPTE274_3G_LEVEL_B:
  case NVVIOSIGNALFORMAT_2048I_48_00_SMPTE372_3G_LEVEL_B:
  case NVVIOSIGNALFORMAT_1080P_59_94_SMPTE274_3G_LEVEL_B:
  case NVVIOSIGNALFORMAT_1080I_59_94_SMPTE274_3G_LEVEL_B:
  case NVVIOSIGNALFORMAT_2048I_59_94_SMPTE372_3G_LEVEL_B:
  case NVVIOSIGNALFORMAT_1080P_29_97_SMPTE274_3G_LEVEL_B:
  case NVVIOSIGNALFORMAT_2048P_29_97_SMPTE372_3G_LEVEL_B:
  case NVVIOSIGNALFORMAT_1080P_23_98_SMPTE274_3G_LEVEL_B:
  case NVVIOSIGNALFORMAT_2048P_23_98_SMPTE372_3G_LEVEL_B:
  case NVVIOSIGNALFORMAT_1080I_47_96_SMPTE274_3G_LEVEL_B:
  case NVVIOSIGNALFORMAT_2048I_47_96_SMPTE372_3G_LEVEL_B:
    // Verify 3G configuration.  3G is only supported on physical jacks 1 and 3,
    // logical jacks 0 and 2.  Display a warning message if a 3G signal is
    // connected to jacks other than those jacks.
    if ( ( m_activeJacks[0] == 1 ) || ( m_activeJacks[0] == 3 ) ||
         ( m_activeJacks[1] == 1 ) || ( m_activeJacks[1] == 3 ) )
    {
      LOG_WARNING( "3G capture supported on Jacks 1 and 3 only." );
    }

    for ( unsigned int i = 0; i < m_numStreams; i++ )
    {
      l_vioConfig.vioConfig.inConfig.streams[i].sampling = m_Sampling;
      l_vioConfig.vioConfig.inConfig.streams[i].bitsPerComponent = m_BitsPerComponent;
      l_vioConfig.vioConfig.inConfig.streams[i].expansionEnable = m_ExpansionEnable;
      l_vioConfig.vioConfig.inConfig.streams[i].numLinks = 2;
      l_vioConfig.vioConfig.inConfig.streams[i].links[0].jack = m_activeJacks[i];
      l_vioConfig.vioConfig.inConfig.streams[i].links[0].channel = 0;
      l_vioConfig.vioConfig.inConfig.streams[i].links[1].jack = m_activeJacks[i];
      l_vioConfig.vioConfig.inConfig.streams[i].links[1].channel = 1;
    }

    break;
  default:
    switch( m_numStreams )
    {
    case 4: //then the signal cannot be dual link
      l_vioConfig.vioConfig.inConfig.streams[3].sampling = m_Sampling;
      l_vioConfig.vioConfig.inConfig.streams[3].bitsPerComponent = m_BitsPerComponent;
      l_vioConfig.vioConfig.inConfig.streams[3].expansionEnable = m_ExpansionEnable;
      l_vioConfig.vioConfig.inConfig.streams[3].numLinks = 1;
      l_vioConfig.vioConfig.inConfig.streams[3].links[0].jack = 3;
      l_vioConfig.vioConfig.inConfig.streams[3].links[0].channel = 0;
    case 3:
      l_vioConfig.vioConfig.inConfig.streams[2].sampling = m_Sampling;
      l_vioConfig.vioConfig.inConfig.streams[2].bitsPerComponent = m_BitsPerComponent;
      l_vioConfig.vioConfig.inConfig.streams[2].expansionEnable = m_ExpansionEnable;
      l_vioConfig.vioConfig.inConfig.streams[2].numLinks = 1;
      l_vioConfig.vioConfig.inConfig.streams[2].links[0].jack = m_activeJacks[2];
      l_vioConfig.vioConfig.inConfig.streams[2].links[0].channel = 0;
    case 2:
      l_vioConfig.vioConfig.inConfig.streams[1].sampling = m_Sampling;
      l_vioConfig.vioConfig.inConfig.streams[1].bitsPerComponent = m_BitsPerComponent;
      l_vioConfig.vioConfig.inConfig.streams[1].expansionEnable = m_ExpansionEnable;
      l_vioConfig.vioConfig.inConfig.streams[1].numLinks = numLinks;
      if( numLinks == 2 )
      {
        l_vioConfig.vioConfig.inConfig.streams[1].links[1].jack = 3;
        l_vioConfig.vioConfig.inConfig.streams[1].links[1].channel = 0;
        l_vioConfig.vioConfig.inConfig.streams[1].links[0].jack = 2;
        l_vioConfig.vioConfig.inConfig.streams[1].links[0].channel = 0;
      }
      else
      {
        l_vioConfig.vioConfig.inConfig.streams[1].links[0].jack = m_activeJacks[1];
        l_vioConfig.vioConfig.inConfig.streams[1].links[0].channel = 0;
      }
    case 1:
      l_vioConfig.vioConfig.inConfig.streams[0].sampling = m_Sampling;
      l_vioConfig.vioConfig.inConfig.streams[0].bitsPerComponent = m_BitsPerComponent;
      l_vioConfig.vioConfig.inConfig.streams[0].expansionEnable = m_ExpansionEnable;
      l_vioConfig.vioConfig.inConfig.streams[0].numLinks = numLinks;
      if( numLinks == 2 )
      {
        l_vioConfig.vioConfig.inConfig.streams[0].links[1].jack = 1;
        l_vioConfig.vioConfig.inConfig.streams[0].links[1].channel = 0;
        l_vioConfig.vioConfig.inConfig.streams[0].links[0].jack = 0;
        l_vioConfig.vioConfig.inConfig.streams[0].links[0].channel = 0;
      }
      else
      {
        l_vioConfig.vioConfig.inConfig.streams[0].links[0].jack = m_activeJacks[0];
        l_vioConfig.vioConfig.inConfig.streams[0].links[0].channel = 0;
      }
      break;
    case 0:
    default:
      LOG_ERROR( "Cannot configure streams, no active inputs detected." );
      return E_FAIL;
      break;
    } // switch
    break;
  } // switch

  NvAPI_Status stat = NVAPI_OK;
  stat = NvAPI_VIO_SetConfig( m_vioHandle, ( NVVIOCONFIG* )&l_vioConfig );
  if ( stat != NVAPI_OK )
  {
    LOG_ERROR( "Cannot set configuration of SDI input device." );
    return E_FAIL;
  }

  if ( ( NvAPI_VIO_GetConfig( m_vioHandle, ( NVVIOCONFIG* )&l_vioConfig ) != NVAPI_OK ) )
  {
    LOG_ERROR( "Cannot get configuration of SDI input device." );
    return E_FAIL;
  }

  return S_OK;
}

//----------------------------------------------------------------------------
HRESULT CNvSDIin::SetupDevice( int deviceNumber )
{
  NVVIOCONFIG_V1 l_vioConfig;
  NVVIOSTATUS l_vioStatus;
  NvAPI_Status ret = NVAPI_OK;

  if( CNvSDIinTopology::Instance().GetNumDevice() == 0 ) //just in case the app failed to scan the topology
  {
    LOG_ERROR( "No SDI video input devices found." );
    return E_FAIL;
  }
  m_deviceNumber = deviceNumber;
  NVVIOTOPOLOGYTARGET* device = CNvSDIinTopology::Instance().GetDevice( deviceNumber );
  if( device == NULL )
  {
    LOG_ERROR( "Unable to set the selected device." );
    return E_FAIL;
  }

  m_vioID = device->vioId;
  m_vioHandle = device->hVioHandle;

  // Get initial device state.
  if FAILED( SetVideoConfig() )
  {
    return E_FAIL;
  }

  // TODO: At this point, need to query the status again and confirm that
  // all ports are configured for the same signal format.  SDI capture cannot
  // succeed if all input ports are not configured and detecting the same
  // signal format.
  GetVideoInState( &l_vioConfig, &l_vioStatus );

  // Get width and height of video signal format.  Long term this
  // may be queried from OpenGL, but for now, need to get this from
  // the control API.
  NVVIOSIGNALFORMATDETAIL l_vioSignalFormatDetail;
  memset( &l_vioSignalFormatDetail, 0, sizeof( l_vioSignalFormatDetail ) );

  ULONG l_vioSignalFormatIndex = ( ULONG )NVVIOSIGNALFORMAT_NONE;

  // Enumerate all signal formats until we find the one we are looking
  // for, the enumeration ends, or there is an error.
  while( 1 )
  {
    ret = NvAPI_VIO_EnumSignalFormats( m_vioHandle,
                                       l_vioSignalFormatIndex,
                                       &l_vioSignalFormatDetail );
    if ( ret == NVAPI_END_ENUMERATION || ret != NVAPI_OK )
    {
      return E_FAIL;
    }
    l_vioSignalFormatIndex++;

    // We found the signal format that we were looking for so break.
    if ( l_vioSignalFormatDetail.signalFormat ==
         l_vioConfig.vioConfig.inConfig.signalFormat )
    {
      m_signalFormatDetail = l_vioSignalFormatDetail;
      break;
    }
  }

  m_videoFormat = l_vioSignalFormatDetail.signalFormat;

  // Cache framerate so that it may later be compared with the actual
  // achievable framerate.
  if ( l_vioSignalFormatDetail.videoMode.interlaceMode == NVVIOINTERLACEMODE_INTERLACE )
  {
    m_fFrameRate = l_vioSignalFormatDetail.videoMode.fFrameRate / 2.0f;
  }
  else
  {
    m_fFrameRate = l_vioSignalFormatDetail.videoMode.fFrameRate;
  }

  // Get video width and height.  Should be the same for all streams.

  m_videoWidth = l_vioSignalFormatDetail.videoMode.horizontalPixels;
  m_videoHeight = l_vioSignalFormatDetail.videoMode.verticalLines;


  return S_OK;
}

//----------------------------------------------------------------------------
HRESULT CNvSDIin::BindDevice( GLuint videoSlot, HDC hDC )
{
  BOOL bRet;
  HVIDEOINPUTDEVICENV* videoDevices;
  m_hDC = hDC;

  UINT numDevices = wglEnumerateVideoCaptureDevicesNV( m_hDC, NULL );
  if ( numDevices <= 0 )
  {
    LOG_ERROR( "wglEnumerateVideoDevicesNV() did not return any devices." );
    return E_FAIL;
  }

  assert( glGetError() == GL_NO_ERROR );

  videoDevices = ( HVIDEOINPUTDEVICENV* )malloc( numDevices *
                 sizeof( videoDevices[0] ) );

  if ( !videoDevices )
  {
    LOG_ERROR( "Memory allocation failed." );
    return E_FAIL;
  }

  assert( glGetError() == GL_NO_ERROR );

  if ( numDevices != wglEnumerateVideoCaptureDevicesNV( m_hDC, videoDevices ) )
  {
    free( videoDevices );
    LOG_ERROR( "Inconsistent results from wglEnumerateVideoDevicesNV()" );
    return E_FAIL;
  }

  // find a device that matches the m_vioID that we got from nvapi and lock it
  for ( UINT i = 0; i < numDevices; ++i )
  {
    int uniqueID;
    bRet = wglQueryVideoCaptureDeviceNV( m_hDC, videoDevices[i],
                                         WGL_UNIQUE_ID_NV, &uniqueID );
    if( bRet && uniqueID == m_vioID )
    {
      BOOL bLocked;
      bLocked = wglLockVideoCaptureDeviceNV( m_hDC, videoDevices[i] );
      assert( glGetError() == GL_NO_ERROR );
      if ( bLocked )
      {
        m_device = videoDevices[i];
        break;
      }
    }
  }

  free( videoDevices );
  int numSlots;
  wglQueryCurrentContextNV( WGL_NUM_VIDEO_CAPTURE_SLOTS_NV, &numSlots );
  if ( m_device == NULL )
  {
    // No lockable devices found
    LOG_ERROR( "No lockable video capture device found." );
    return E_FAIL;
  }
  m_videoSlot = videoSlot;

  // wglBindVideoCaptureDeviceNV needs a context current
  bRet = wglBindVideoCaptureDeviceNV( m_videoSlot, m_device );
  GLenum err = glGetError();

  assert( bRet && "Failed trying to bind the video capture device!" );
  return bRet ? S_OK : E_FAIL;
}

//----------------------------------------------------------------------------
/*
HRESULT CNvSDIin::BindVideoObjects()
{
  if(m_signalFormatDetail.videoMode.interlaceMode != NVVIOINTERLACEMODE_INTERLACE)
  {
    m_bCaptureFields = false;
    m_bCaptureStackedFields = false;
  }
  if(m_bCaptureFields)
  {
    m_videoHeight = m_signalFormatDetail.videoMode.verticalLines>>1;
  }

  m_numVideoObjects = m_configOptions.numStreams;
  if(m_bCaptureFields)
    m_numVideoObjects <<= 1;

  if(m_bCaptureBuffers)
  {
    // Create video buffer objects for each stream
    glGenBuffersARB(m_numVideoObjects, m_vidBufObj);
    assert(glGetError() == GL_NO_ERROR);
    for (UINT i = 0; i < m_configOptions.numStreams; i++) {
      glVideoCaptureStreamParameterivNV(m_videoSlot, i,
        GL_VIDEO_BUFFER_INTERNAL_FORMAT_NV,
        &m_captureInternalFormat);
      assert(glGetError() == GL_NO_ERROR);
      UINT objInd = i;
      if(m_bCaptureFields) //setup 2 objects for capturing fields
      {
        //setup upper field object
        objInd = 2*i;
        glGetVideoCaptureStreamivNV(m_videoSlot, i, GL_VIDEO_BUFFER_PITCH_NV,
          &m_bufPitch[i]);
        assert(glGetError() == GL_NO_ERROR);

        glBindBufferARB(GL_VIDEO_BUFFER_NV, m_vidBufObj[objInd]);
        assert(glGetError() == GL_NO_ERROR);
        // Allocate required space in video capture buffer
        //in the case of the following format the upper field has one extra line!!!
        if(m_signalFormatDetail.signalFormat == NVVIOSIGNALFORMAT_487I_59_94_SMPTE259_NTSC)
          glBufferDataARB(GL_VIDEO_BUFFER_NV, m_bufPitch[i] * (m_videoHeight+1),
          NULL, GL_STREAM_COPY);
        else
          glBufferDataARB(GL_VIDEO_BUFFER_NV, m_bufPitch[i] * m_videoHeight,
          NULL, GL_STREAM_COPY);

        assert(glGetError() == GL_NO_ERROR);
        glBindVideoCaptureStreamBufferNV(m_videoSlot, i, GL_FIELD_UPPER_NV, 0);
        assert(glGetError() == GL_NO_ERROR);
        //setup lower field object
        objInd = 2*i+1;
        glGetVideoCaptureStreamivNV(m_videoSlot, i, GL_VIDEO_BUFFER_PITCH_NV,
          &m_bufPitch[objInd]);
        glBindBufferARB(GL_VIDEO_BUFFER_NV, m_vidBufObj[objInd]);
        assert(glGetError() == GL_NO_ERROR);
        // Allocate required space in video capture buffer
        glBufferDataARB(GL_VIDEO_BUFFER_NV, m_bufPitch[i] * m_videoHeight,
          NULL, GL_STREAM_COPY);
        assert(glGetError() == GL_NO_ERROR);
        glBindVideoCaptureStreamBufferNV(m_videoSlot, i, GL_FIELD_LOWER_NV, 0);
        assert(glGetError() == GL_NO_ERROR);
      }
      else
      {
        if(m_bCaptureStackedFields)
        {
          glGetVideoCaptureStreamivNV(m_videoSlot, i, GL_VIDEO_BUFFER_PITCH_NV,
            &m_bufPitch[objInd]);

          glBindBufferARB(GL_VIDEO_BUFFER_NV, m_vidBufObj[objInd]);
          assert(glGetError() == GL_NO_ERROR);
          // Allocate required space in video capture buffer
          glBufferDataARB(GL_VIDEO_BUFFER_NV, m_bufPitch[i] * m_videoHeight,
            NULL, GL_STREAM_COPY);
          assert(glGetError() == GL_NO_ERROR);
          if(m_signalFormatDetail.signalFormat == NVVIOSIGNALFORMAT_487I_59_94_SMPTE259_NTSC)
          {
            glBindVideoCaptureStreamBufferNV(m_videoSlot, i, GL_FIELD_UPPER_NV, 0);
            glBindVideoCaptureStreamBufferNV(m_videoSlot, i, GL_FIELD_LOWER_NV,  m_bufPitch[i] *((m_videoHeight>>1)+1));
          }
          else
          {
            glBindVideoCaptureStreamBufferNV(m_videoSlot, i, GL_FIELD_UPPER_NV, 0);
            glBindVideoCaptureStreamBufferNV(m_videoSlot, i, GL_FIELD_LOWER_NV,  m_bufPitch[i] *(m_videoHeight>>1));
          }
          assert(glGetError() == GL_NO_ERROR);
        }
        else
        {
          glGetVideoCaptureStreamivNV(m_videoSlot, i, GL_VIDEO_BUFFER_PITCH_NV,
            &m_bufPitch[objInd]);

          glBindBufferARB(GL_VIDEO_BUFFER_NV, m_vidBufObj[objInd]);
          assert(glGetError() == GL_NO_ERROR);
          // Allocate required space in video capture buffer
          glBufferDataARB(GL_VIDEO_BUFFER_NV, m_bufPitch[i] * m_videoHeight,
            NULL, GL_STREAM_COPY);
          assert(glGetError() == GL_NO_ERROR);
          glBindVideoCaptureStreamBufferNV(m_videoSlot, i, GL_FRAME_NV, 0);
          assert(glGetError() == GL_NO_ERROR);
        }
      }
    }
    if(m_bStreamToTextures)
    {
      //create the textures to go with the buffers
      glGenTextures(m_numVideoObjects, m_videoTextures);
      assert(glGetError() == GL_NO_ERROR);

      for (unsigned int i = 0; i < m_numVideoObjects; i++) {
        glBindTexture(GL_TEXTURE_RECTANGLE_NV, m_videoTextures[i]);
        assert(glGetError() == GL_NO_ERROR);
        glTexParameterf(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        assert(glGetError() == GL_NO_ERROR);
        //!!!!In the case of GL_LUMINANCE8 format, the buffer pitch is padded to be 64 byte aligned when we capture NTSC/PAL video
        //!!!!For any other video buffer format please comment the pixelStorei lines.
        if(m_captureInternalFormat == GL_LUMINANCE8)
        {
          glPixelStorei(GL_PACK_ROW_LENGTH,m_bufPitch[i]);
          glPixelStorei(GL_UNPACK_ROW_LENGTH,m_bufPitch[i]);
        }
        glTexImage2D(GL_TEXTURE_RECTANGLE_NV, 0, m_textureInternalFormat, m_videoWidth, m_videoHeight,
               0, m_pixelFormat, GL_UNSIGNED_BYTE, NULL);
        assert(glGetError() == GL_NO_ERROR);
      }

    }
    return S_OK;
  }
  else //capture to texture directly
  {
    glGenTextures(m_numVideoObjects, m_videoTextures);
    assert(glGetError() == GL_NO_ERROR);

    for (UINT i = 0; i < m_configOptions.numStreams; i++) {
      UINT objInd = i;
      if(m_bCaptureFields)//setup 2 objects for capturing fields
      {
        //setup upper field object
        objInd = 2*i;
        glBindTexture(GL_TEXTURE_RECTANGLE_NV, m_videoTextures[objInd]);
        assert(glGetError() == GL_NO_ERROR);
        glTexParameterf(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        assert(glGetError() == GL_NO_ERROR);
         //in the case of the following format the upper field has one extra line!!!
        if(m_signalFormatDetail.signalFormat == NVVIOSIGNALFORMAT_487I_59_94_SMPTE259_NTSC && m_bCaptureFields)
          glTexImage2D(GL_TEXTURE_RECTANGLE_NV, 0, m_textureInternalFormat, m_videoWidth, m_videoHeight+1,
               0, m_pixelFormat, GL_UNSIGNED_BYTE, NULL);
        else
          glTexImage2D(GL_TEXTURE_RECTANGLE_NV, 0, m_textureInternalFormat, m_videoWidth, m_videoHeight,
               0, m_pixelFormat, GL_UNSIGNED_BYTE, NULL);

        assert(glGetError() == GL_NO_ERROR);
        glBindVideoCaptureStreamTextureNV(m_videoSlot, i, GL_FIELD_UPPER_NV, GL_TEXTURE_RECTANGLE_NV,m_videoTextures[objInd]);
        assert(glGetError() == GL_NO_ERROR);
        //setup lower field object
        objInd = 2*i+1;
        glBindTexture(GL_TEXTURE_RECTANGLE_NV, m_videoTextures[objInd]);
        assert(glGetError() == GL_NO_ERROR);
        glTexParameterf(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        assert(glGetError() == GL_NO_ERROR);

        glTexImage2D(GL_TEXTURE_RECTANGLE_NV, 0, m_textureInternalFormat, m_videoWidth, m_videoHeight,
               0, m_pixelFormat, GL_UNSIGNED_BYTE, NULL);
        assert(glGetError() == GL_NO_ERROR);
        glBindVideoCaptureStreamTextureNV(m_videoSlot, i, GL_FIELD_LOWER_NV, GL_TEXTURE_RECTANGLE_NV,m_videoTextures[objInd]);
        assert(glGetError() == GL_NO_ERROR);
      }
      else
      {
        glBindTexture(GL_TEXTURE_RECTANGLE_NV, m_videoTextures[objInd]);
        assert(glGetError() == GL_NO_ERROR);
        glTexParameterf(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        assert(glGetError() == GL_NO_ERROR);

        glTexImage2D(GL_TEXTURE_RECTANGLE_NV, 0, m_textureInternalFormat, m_videoWidth, m_videoHeight,
               0, m_pixelFormat, GL_UNSIGNED_BYTE, NULL);
        assert(glGetError() == GL_NO_ERROR);
        glBindVideoCaptureStreamTextureNV(m_videoSlot, i, GL_FRAME_NV, GL_TEXTURE_RECTANGLE_NV,m_videoTextures[objInd]);
        assert(glGetError() == GL_NO_ERROR);
      }
    }
    return S_OK;
  }
}
*/

//----------------------------------------------------------------------------
HRESULT CNvSDIin::BindVideoTexture( const GLuint videoTexture, unsigned int stream )
{
  if( stream >= 0 && stream < m_numStreams )
  {
    glBindVideoCaptureStreamTextureNV( m_videoSlot, stream, GL_FRAME_NV, GL_TEXTURE_RECTANGLE_NV, videoTexture );
    assert( glGetError() == GL_NO_ERROR );
  }
  return S_OK;
}

//----------------------------------------------------------------------------
HRESULT CNvSDIin::UnbindVideoTexture( unsigned int stream )
{
  if( stream >= 0 && stream < m_numStreams )
  {
    glBindVideoCaptureStreamTextureNV( m_videoSlot, stream, GL_FRAME_NV, GL_TEXTURE_RECTANGLE_NV, 0 );
    assert( glGetError() == GL_NO_ERROR );
  }
  return S_OK;
}

//----------------------------------------------------------------------------
HRESULT CNvSDIin::BindVideoFrameBuffer( GLuint videoBuffer, GLint videoBufferFormat, unsigned int stream )
{
  if( stream >= 0 && stream < m_numStreams )
  {
    //set the buffer format
    glVideoCaptureStreamParameterivNV( m_videoSlot, stream,
                                       GL_VIDEO_BUFFER_INTERNAL_FORMAT_NV,
                                       &videoBufferFormat );
    assert( glGetError() == GL_NO_ERROR );
    glBindBuffer( GL_VIDEO_BUFFER_NV, videoBuffer );
    assert( glGetError() == GL_NO_ERROR );
    glBindVideoCaptureStreamBufferNV( m_videoSlot, stream, GL_FRAME_NV, 0 );
    assert( glGetError() == GL_NO_ERROR );
  }

  return S_OK;
}

//----------------------------------------------------------------------------
int CNvSDIin::GetBufferObjectPitch( unsigned int streamIndex )
{
  GLint bufferPitch = 0;
  if( streamIndex >= 0 && streamIndex < m_numStreams )
  {
    // Get the video buffer pitch
    glGetVideoCaptureStreamivNV( m_videoSlot, streamIndex, GL_VIDEO_BUFFER_PITCH_NV,
                                 &bufferPitch );
    assert( glGetError() == GL_NO_ERROR );
  }
  return bufferPitch;
}

//----------------------------------------------------------------------------
unsigned int CNvSDIin::GetNumStreams()
{
  return m_numStreams;
}

//----------------------------------------------------------------------------
NVVIOSIGNALFORMAT CNvSDIin::GetSignalFormat()
{
  return m_videoFormat;
}

//----------------------------------------------------------------------------
HRESULT CNvSDIin::UnbindVideoFrameBuffer( unsigned int stream )
{
  return S_OK;
}

//----------------------------------------------------------------------------
HRESULT CNvSDIin::StartCapture()
{
  for ( NvU32 i = 0; i < m_numStreams; i++ )
  {
    // Set the buffer object capture data format for each stream.

    glVideoCaptureStreamParameterfvNV( m_videoSlot, i,
                                       GL_VIDEO_COLOR_CONVERSION_MATRIX_NV,
                                       &m_cscMat[0][0] );
    assert( glGetError() == GL_NO_ERROR );

    glVideoCaptureStreamParameterfvNV( m_videoSlot, i,
                                       GL_VIDEO_COLOR_CONVERSION_MAX_NV, &m_cscMax[0] );
    assert( glGetError() == GL_NO_ERROR );

    glVideoCaptureStreamParameterfvNV( m_videoSlot, i,
                                       GL_VIDEO_COLOR_CONVERSION_MIN_NV, &m_cscMin[0] );
    assert( glGetError() == GL_NO_ERROR );

    glVideoCaptureStreamParameterfvNV( m_videoSlot, i,
                                       GL_VIDEO_COLOR_CONVERSION_OFFSET_NV, &m_cscOffset[0] );
    assert( glGetError() == GL_NO_ERROR );
  }

  // Start video capture
  glBeginVideoCaptureNV( m_videoSlot );
  glGenQueries( 1, &m_captureTimeQuery );
  GLenum err = glGetError();
  assert( err == GL_NO_ERROR );
  if( err == GL_NO_ERROR )
  {
    m_bCaptureStarted = true;
  }
  return S_OK;
}

//----------------------------------------------------------------------------
GLenum CNvSDIin::Capture( GLuint* sequenceNum, GLuint64EXT* captureTime )
{
  GLenum ret;
#ifdef MEASURE_PERFORMANCE
  GLuint64EXT captureTimeStart;
  GLuint64EXT captureTimeEnd;
#endif

  // Capture the video to a buffer object
#ifdef MEASURE_PERFORMANCE
  //glBeginQuery(GL_TIME_ELAPSED_EXT,m_captureTimeQuery);
  glGetInteger64v( GL_CURRENT_TIME_NV, ( GLint64* )&captureTimeStart );
#endif

  ret = glVideoCaptureNV( m_videoSlot, sequenceNum, captureTime );

  m_gviTime = 0;
  m_gpuTime = 0;
#ifdef MEASURE_PERFORMANCE
  glGetInteger64v( GL_CURRENT_TIME_NV, ( GLint64* )&captureTimeEnd );

  m_gviTime = ( captureTimeEnd - *captureTime ) * .000000001;

  GLuint64EXT timeElapsed = 0;
  m_gpuTime = timeElapsed * .000000001;
#endif
  assert( glGetError() == GL_NO_ERROR );
  return ret;
}

//----------------------------------------------------------------------------
HRESULT CNvSDIin::EndCapture()
{
  if( m_bCaptureStarted )
  {
    glEndVideoCaptureNV( m_videoSlot );
    glDeleteQueries( 1, &m_captureTimeQuery );
    assert( glGetError() == GL_NO_ERROR );
    m_bCaptureStarted = false;
  }
  return S_OK;
}

//----------------------------------------------------------------------------
HRESULT CNvSDIin::UnbindDevice()
{
  if( m_hDC )
  {
    BOOL bRet;
    bRet = wglBindVideoCaptureDeviceNV( m_videoSlot, NULL );
    assert( bRet && "Failed trying to unbind the video capture device!" );
    bRet = wglReleaseVideoCaptureDeviceNV( m_hDC, m_device );
    assert( bRet && "Failed trying to release the video capture device!" );
    m_hDC = NULL;
  }
  return S_OK;
}

//----------------------------------------------------------------------------
HRESULT CNvSDIin::Cleanup()
{
  EndCapture();
  UnbindDevice();
  return S_OK;
}

//----------------------------------------------------------------------------
HRESULT CNvSDIin::GetFrameRate( float* rate )
{
  *rate = m_fFrameRate;

  return S_OK;
}

//----------------------------------------------------------------------------
int CNvSDIin::GetDeviceNumber()
{
  return m_deviceNumber;
}