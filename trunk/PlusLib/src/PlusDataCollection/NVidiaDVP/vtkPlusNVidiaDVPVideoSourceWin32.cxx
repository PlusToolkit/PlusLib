/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

/*=========================================================================
The following copyright notice is applicable to parts of this file:
Copyright (c) 2014, Robarts Research Institute, The University of Western Ontario, London, Ontario, Canada
All rights reserved.
Authors include:
* Adam Rankin (Robarts Research Institute and The University of Western Ontario)
=========================================================================*/

// Plus includes
#include "PlusConfigure.h"
#include "vtkPlusNvidiaDVPVideoSourceWin32.h"
#include "vtkPlusDataSource.h"

// VTK includes
#include <vtkObjectFactory.h>

// NVidia DVP includes
#include <DVPAPI.h>
#include <dvpapi_gl.h>
#include "nvGPUutil.h"
#include "glExtensions.h"

// NV-API device control API
#include <nvapi.h>

//----------------------------------------------------------------------------

vtkStandardNewMacro( vtkPlusNvidiaDVPVideoSource );

//----------------------------------------------------------------------------
vtkPlusNvidiaDVPVideoSource::vtkPlusNvidiaDVPVideoSource()
  : FrameNumber( 0 )
  , EnableGPUCPUCopy( false )
{
  // No callback function provided by the device, so the data capture thread will be used to poll the hardware and add new items to the buffer
  this->StartThreadForInternalUpdates = true;
  this->AcquisitionRate = 30;
}

//----------------------------------------------------------------------------
vtkPlusNvidiaDVPVideoSource::~vtkPlusNvidiaDVPVideoSource()
{
  if ( this->Connected )
  {
    this->Disconnect();
  }
}

//----------------------------------------------------------------------------
void vtkPlusNvidiaDVPVideoSource::PrintSelf( ostream& os, vtkIndent indent )
{
  this->Superclass::PrintSelf( os, indent );
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusNvidiaDVPVideoSource::InternalConnect()
{
  LOG_TRACE( "vtkPlusPhilips3DProbeVideoSource::InternalConnect" );

  NvGPU = CNvGpuTopology::instance().getGpu( NvOptions.captureGPU );
  if ( NvGPU == nullptr )
  {
    LOG_ERROR( "Unable to retrieve NvGPU." );
    return PLUS_FAIL;
  }

  if( SetupSDIDevices() == E_FAIL )
  {
    return PLUS_FAIL;
  }

  SetupGL();

  if ( StartSDIPipeline() == E_FAIL )
  {
    return PLUS_FAIL;
  }

  CPUFrame = new unsigned char[NvSDIin.GetBufferObjectPitch( 0 ) * VideoHeight];

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusNvidiaDVPVideoSource::InternalDisconnect()
{
  LOG_TRACE( "vtkPlusNvidiaDVPVideoSource::InternalDisconnect" );

  Shutdown();

  delete [] CPUFrame;
  CPUFrame = nullptr;

  NvGPU = nullptr;

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusNvidiaDVPVideoSource::InternalUpdate()
{
  CaptureVideo();

  if ( EnableGPUCPUCopy )
  {
    CopyGPUToCPU();
    if ( OutputDataSource->AddItem( ( void* )CPUFrame,
                                    OutputDataSource->GetInputImageOrientation(),
                                    OutputDataSource->GetInputFrameSize(),
                                    VTK_UNSIGNED_CHAR, // TODO : scalar type from nvidia video format
                                    1, // TODO : num components from nvidia video format
                                    US_IMG_BRIGHTNESS, // TODO : img type from nvidia video format
                                    0,
                                    this->FrameNumber ) != PLUS_SUCCESS )
    {
      LOG_ERROR( "Unable to capture frame from SDI capture." );
      return PLUS_FAIL;
    }
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusNvidiaDVPVideoSource::ReadConfiguration( vtkXMLDataElement* rootConfigElement )
{
  LOG_TRACE( "vtkPlusNvidiaDVPVideoSource::ReadConfiguration" );
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING( deviceConfig, rootConfigElement );

  XML_READ_BOOL_ATTRIBUTE_OPTIONAL( EnableGPUCPUCopy, deviceConfig );

  int numGPUs;
  // Note, this function enumerates GPUs which are both CUDA & GLAffinity capable (i.e. newer Quadros)
  numGPUs = CNvGpuTopology::instance().getNumGpu();

  if ( numGPUs <= 0 )
  {
    LOG_ERROR( "Unable to obtain system GPU topology." );
    return PLUS_FAIL;
  }

  int numCaptureDevices = CNvSDIinTopology::instance().getNumDevice();

  if ( numCaptureDevices <= 0 )
  {
    LOG_ERROR( "Unable to obtain system Capture topology." );
    return PLUS_FAIL;
  }

  // Set the defaults for all the relevant options
  NvOptions.dualLink = false;
  NvOptions.expansionEnable = false;
  NvOptions.sampling = NVVIOCOMPONENTSAMPLING_422;
  NvOptions.bitsPerComponent = 8;
  NvOptions.captureDevice = 0;
  NvOptions.captureGPU = CNvGpuTopology::instance().getPrimaryGpuIndex();

  // TODO : get options from xml instead of command line
  ParseCommandLine( szCmdLine, &NvOptions ); //get the user config
  switch ( NvOptions.sampling )
  {
  case NVVIOCOMPONENTSAMPLING_422:
    if ( NvOptions.bitsPerComponent == 8 )
    {
      VideoBufferFormat = GL_YCBYCR8_422_NV;
    }
    else if ( NvOptions.bitsPerComponent == 10 )
    {
      VideoBufferFormat = GL_Z6Y10Z6CB10Z6Y10Z6CR10_422_NV;
    }
    else //12 bit
    {
      NvOptions.dualLink = true;
      VideoBufferFormat = GL_Z4Y12Z4CB12Z4Y12Z4CR12_422_NV;
    }
    break;
  case NVVIOCOMPONENTSAMPLING_4224:
    NvOptions.dualLink = true;
    if ( NvOptions.bitsPerComponent == 8 )
    {
      VideoBufferFormat = GL_YCBAYCR8A_4224_NV;
    }
    else if ( NvOptions.bitsPerComponent == 10 )
    {
      VideoBufferFormat = GL_Z6Y10Z6CB10Z6A10Z6Y10Z6CR10Z6A10_4224_NV;
    }
    else //12 bit
    {
      VideoBufferFormat = GL_Z4Y12Z4CB12Z4A12Z4Y12Z4CR12Z4A12_4224_NV;
    }
    break;
  case NVVIOCOMPONENTSAMPLING_444:
    NvOptions.dualLink = true;
    if ( NvOptions.bitsPerComponent == 8 )
    {
      VideoBufferFormat = GL_RGB8;
    }
    else if ( NvOptions.bitsPerComponent == 10 )
    {
      VideoBufferFormat = GL_RGB10;
    }
    else //12 bit
    {
      VideoBufferFormat = GL_Z4Y12Z4CB12Z4CR12_444_NV;
    }
    VideoBufferFormat = GL_RGB8;
    break;
  case NVVIOCOMPONENTSAMPLING_4444:
    NvOptions.dualLink = true;
    if ( NvOptions.bitsPerComponent == 8 )
    {
      VideoBufferFormat = GL_RGBA8;
    }
    else if ( NvOptions.bitsPerComponent == 10 )
    {
      VideoBufferFormat = GL_RGBA12;
    }
    else //12 bit
    {
      VideoBufferFormat = GL_RGBA12;
    }
    break;
  }

  if ( NvOptions.captureDevice >= numCaptureDevices )
  {
    LOG_ERROR( "Selected Capture Device is out of range." );
    return PLUS_FAIL;
  }
  if ( NvOptions.captureGPU >= numGPUs )
  {
    LOG_ERROR( "Selected Capture GPU is out of range." );
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusNvidiaDVPVideoSource::WriteConfiguration( vtkXMLDataElement* rootConfig )
{
  LOG_TRACE( "vtkPlusNvidiaDVPVideoSource::WriteConfiguration" );
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING( deviceConfig, rootConfig );

  XML_WRITE_BOOL_ATTRIBUTE( EnableGPUCPUCopy, deviceConfig );

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusNvidiaDVPVideoSource::NotifyConfigured()
{
  if ( this->OutputChannels.size() != 1 && this->EnableGPUCPUCopy ||
       this->OutputChannels.size() > 0 && !this->EnableGPUCPUCopy )
  {
    LOG_ERROR( "Incorrect configuration. GPU/CPU copy and OutputChannel configuration are incompatible." );
    this->SetCorrectlyConfigured( false );
    return PLUS_FAIL;
  }

  if( this->EnableGPUCPUCopy && this->GetFirstVideoSource( OutputDataSource ) != PLUS_SUCCESS )
  {
    LOG_ERROR( "Unable to find video source. Device needs a video buffer to put new frames into when copying frames from the GPU." );
    this->SetCorrectlyConfigured( false );
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
bool vtkPlusNvidiaDVPVideoSource::IsTracker() const
{
  return false;
}


//-----------------------------------------------------------------------------
HRESULT vtkPlusNvidiaDVPVideoSource::SetupSDIDevices()
{
  if ( SetupSDIinDevices() != S_OK )
  {
    LOG_ERROR( "Error setting up video capture." );
    return E_FAIL;
  }

  return S_OK;
}

//-----------------------------------------------------------------------------
HRESULT vtkPlusNvidiaDVPVideoSource::StartSDIPipeline()
{
  // Start video capture
  if ( NvSDIin.StartCapture() != S_OK )
  {
    LOG_ERROR( "Error starting video capture." );
    return E_FAIL;
  }
  return S_OK;
}

//-----------------------------------------------------------------------------
HRESULT vtkPlusNvidiaDVPVideoSource::StopSDIPipeline()
{
  NvSDIin.EndCapture();
  return S_OK;
}

//-----------------------------------------------------------------------------
HRESULT vtkPlusNvidiaDVPVideoSource::SetupSDIinDevices()
{
  NvSDIin.Init( &NvOptions );

  // Initialize the video capture device.
  if ( NvSDIin.SetupDevice( true, NvOptions.captureDevice ) != S_OK )
  {
    return E_FAIL;
  }

  VideoWidth = NvSDIin.GetWidth();
  VideoHeight = NvSDIin.GetHeight();

  return S_OK;
}

//-----------------------------------------------------------------------------
HRESULT vtkPlusNvidiaDVPVideoSource::SetupSDIinGL()
{
  GLuint gpuVideoSlot = 1;

  NvSDIin.BindDevice( gpuVideoSlot, HandleDC );

  glGenBuffers( NvSDIin.GetNumStreams(), VideoBufferObject );

  int videoBufferPitch = 0;
  for ( unsigned int i = 0; i < NvSDIin.GetNumStreams(); i++ )
  {
    NvSDIin.BindVideoFrameBuffer( VideoBufferObject[i], VideoBufferFormat, i );
    videoBufferPitch = NvSDIin.GetBufferObjectPitch( i );

    // Allocate required space in video capture buffer
    glBindBuffer( GL_VIDEO_BUFFER_NV, VideoBufferObject[i] );
    assert( glGetError() == GL_NO_ERROR );

    glBufferData( GL_VIDEO_BUFFER_NV, videoBufferPitch * VideoHeight, NULL, GL_STREAM_COPY );
    assert( glGetError() == GL_NO_ERROR );
  }

  return S_OK;
}

//-----------------------------------------------------------------------------
GLboolean vtkPlusNvidiaDVPVideoSource::SetupGL()
{
  HGPUNV gpuMask[2];
  gpuMask[0] = NvGPU->getAffinityHandle();
  gpuMask[1] = NULL;
  if ( !( HandleDC = wglCreateAffinityDCNV( gpuMask ) ) )
  {
    LOG_WARNING( "Unable to create GPU affinity DC." );
  }

  PIXELFORMATDESCRIPTOR pfd =             // pfd Tells Windows How We Want Things To Be
  {
    sizeof( PIXELFORMATDESCRIPTOR ),     // Size Of This Pixel Format Descriptor
    1,                        // Version Number
    PFD_DRAW_TO_WINDOW |              // Format Must Support Window
    PFD_SUPPORT_OPENGL |              // Format Must Support OpenGL
    PFD_DOUBLEBUFFER,               // Must Support Double Buffering
    PFD_TYPE_RGBA,                  // Request An RGBA Format
    24,                       // Select Our Color Depth
    0, 0, 0, 0, 0, 0,               // Color Bits Ignored
    1,                        // Alpha Buffer
    0,                        // Shift Bit Ignored
    0,                        // No Accumulation Buffer
    0, 0, 0, 0,                   // Accumulation Bits Ignored
    24,                       // 24 Bit Z-Buffer (Depth Buffer)
    8,                        // 8 Bit Stencil Buffer
    0,                        // No Auxiliary Buffer
    PFD_MAIN_PLANE,                 // Main Drawing Layer
    0,                        // Reserved
    0, 0, 0                     // Layer Masks Ignored
  };
  GLuint pf = ChoosePixelFormat( HandleDC, &pfd );
  BOOL result = SetPixelFormat( HandleDC, pf, &pfd );
  if ( result == FALSE )
  {
    return GL_FALSE;
  }
  // Create rendering context from the affinity device context
  HandleGLRC = wglCreateContext( HandleDC );

  // Make window rendering context current.
  wglMakeCurrent( HandleDC, HandleGLRC );

  //load the required OpenGL extensions:
  if ( !loadCaptureVideoExtension() || !loadTimerQueryExtension() || !loadBufferObjectExtension() )
  {
    LOG_ERROR( "Could not load the required OpenGL extensions." );
    return false;
  }

  glClearColor( 0.0, 0.0, 0.0, 0.0 );
  glClearDepth( 1.0 );

  glDisable( GL_DEPTH_TEST );

  glDisable( GL_TEXTURE_1D );
  glDisable( GL_TEXTURE_2D );

  SetupSDIinGL();
  return GL_TRUE;
}

//-----------------------------------------------------------------------------
GLenum vtkPlusNvidiaDVPVideoSource::CaptureVideo()
{
  static GLuint64EXT captureTime;
  GLuint sequenceNum;
  static GLuint prevSequenceNum = 0;
  GLenum ret;
  static int numFails = 0;
  static int numTries = 0;
  static bool bShowMessageBox = true;

  if ( numFails < 100 )
  {
    // Capture the video to a buffer object
    ret = NvSDIin.Capture( &sequenceNum, &captureTime );
    if ( sequenceNum - prevSequenceNum > 1 )
    {
      LOG_WARNING( "glVideoCaptureNV: Dropped " << sequenceNum - prevSequenceNum << " frames." );
    }
    prevSequenceNum = sequenceNum;
    switch ( ret )
    {
    case GL_SUCCESS_NV:
      LOG_DEBUG( "Frame: " << sequenceNum << ". gpuTime: " << NvSDIin.m_gpuTime  << ". gviTime: " << NvSDIin.m_gviTime << "." );
      numFails = 0;
      break;
    case GL_PARTIAL_SUCCESS_NV:
      LOG_WARNING( "glVideoCaptureNV: GL_PARTIAL_SUCCESS_NV." );
      numFails = 0;
      break;
    case GL_FAILURE_NV:
      LOG_ERROR( "glVideoCaptureNV: GL_FAILURE_NV - Video capture failed." );
      numFails++;
      break;
    default:
      LOG_ERROR( "glVideoCaptureNV: Unknown return value." );
      break;
    }
  }
  // The incoming signal format or some other error occurred during
  // capture, shutdown and try to restart capture.
  else
  {
    if ( numTries == 0 )
    {
      StopSDIPipeline();
      CleanupSDIDevices();
      CleanupGL();
    }
    // Initialize the video capture device.
    if ( NvSDIin.SetupDevice( bShowMessageBox, NvOptions.captureDevice ) != S_OK )
    {
      bShowMessageBox = false;
      numTries++;
      return GL_FAILURE_NV;
    }
    // Reinitialize OpenGL.
    SetupGL();
    StartSDIPipeline();
    numFails = 0;
    numTries = 0;
    bShowMessageBox = true;
    return GL_FAILURE_NV;
  }
  return ret;
}

//-----------------------------------------------------------------------------
HRESULT vtkPlusNvidiaDVPVideoSource::CleanupSDIinGL()
{
  for ( unsigned int i = 0; i < NvSDIin.GetNumStreams(); i++ )
  {
    NvSDIin.UnbindVideoFrameBuffer( i );
  }
  NvSDIin.UnbindDevice();
  glDeleteBuffers( NvSDIin.GetNumStreams(), VideoBufferObject );
  return S_OK;
}

//-----------------------------------------------------------------------------
GLboolean vtkPlusNvidiaDVPVideoSource::CleanupGL()
{
  CleanupSDIinGL();

  // Delete OpenGL rendering context.
  wglMakeCurrent( NULL, NULL );

  return S_OK;
}

//-----------------------------------------------------------------------------
HRESULT vtkPlusNvidiaDVPVideoSource::CleanupSDIDevices()
{
  return S_OK;
}

//-----------------------------------------------------------------------------
void vtkPlusNvidiaDVPVideoSource::Shutdown()
{
  StopSDIPipeline();
  CleanupGL();
  CleanupSDIDevices();
}

//-----------------------------------------------------------------------------
HRESULT vtkPlusNvidiaDVPVideoSource::CopyGPUToCPU()
{
  glBindBuffer( GL_VIDEO_BUFFER_NV, VideoBufferObject[0] );

  // Transfer contents of video buffer(s) to system memory
  glGetBufferSubData( GL_VIDEO_BUFFER_NV, 0, NvSDIin.GetBufferObjectPitch( 0 ) * VideoHeight, CPUFrame );

  return S_OK;
}