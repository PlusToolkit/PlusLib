#ifndef NVSDIIN_H
#define NVSDIIN_H

#if WIN32
#include <windows.h>
#endif
#include <assert.h>
#include <stdio.h>

#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glu.h>
#include <GL/wglext.h>

#include "nvapi.h"

// Definitions
#define MAX_VIDEO_STREAMS 4
#define MEASURE_PERFORMANCE

struct nvOptions;

//----------------------------------------------------------------------------
typedef class CNvSDIinTopology
{
public:
  static CNvSDIinTopology& instance();
  NVVIOTOPOLOGYTARGET* getDevice( int index );
  int getNumDevice();

protected:
  CNvSDIinTopology();
  virtual ~CNvSDIinTopology();

  bool init();

protected:
  NVVIOTOPOLOGYTARGET* m_lDevice[NVAPI_MAX_VIO_DEVICES];
  bool m_bInitialized;
  int m_nDevice;
} CNvSDIinTopology;

//----------------------------------------------------------------------------
// CNvSDIin - class that allows to capture one or multiple streams into a texture/video buffer object
typedef class CNvSDIin
{
public:
  float m_gviTime;
  float m_gpuTime;

  CNvSDIin();
  ~CNvSDIin();

  HRESULT Init( nvOptions* options = NULL );

  HRESULT SetupDevice( bool bShowMessageBox = true, int deviceNumber = 0 ); //sets up a particular capture device
  //it is up to the developer to make sure that the same video slot is not used twice for two different capture devices
  //on one context. Later this accounting can be made transparent to the user and done in the class
  HRESULT BindDevice( GLuint videoSlot, HDC hDC );

  HRESULT BindVideoTexture( GLuint videoTexture, int stream );
  HRESULT UnbindVideoTexture( int stream );

  HRESULT BindVideoFrameBuffer( GLuint videoBuffer, GLint videoBufferFormat, int stream );
  HRESULT UnbindVideoFrameBuffer( int stream );

  HRESULT StartCapture();
  GLenum Capture( GLuint* sequenceNum, GLuint64EXT* captureTime );
  HRESULT EndCapture();

  HRESULT Cleanup();
  HRESULT UnbindDevice();

  void SetCSCParams( GLfloat* cscMat, GLfloat* cscOffset, GLfloat* cscMin, GLfloat* cscMax );

  NvVioHandle GetVioHandle();
  unsigned int GetWidth();
  unsigned int GetHeight();
  int GetBufferObjectPitch( int streamIndex );
  unsigned int GetNumStreams();
  NVVIOSIGNALFORMAT GetSignalFormat();
  HRESULT GetFrameRate( float* rate );
  int GetDeviceNumber();

protected:
  // Get video input state
  HRESULT getVideoInState( NVVIOCONFIG_V1* vioConfig, NVVIOSTATUS* vioStatus );
  HRESULT setVideoConfig( bool bShowMessageBox );
  void DumpChannelStatus( NVVIOCHANNELSTATUS jack );
  void DumpStreamStatus( NVVIOSTREAM stream );

protected:
  //Capture Interpretation settings
  bool m_bDualLink;
  NVVIOCOMPONENTSAMPLING m_Sampling;
  int m_BitsPerComponent;
  int m_ExpansionEnable;

  //Capture detected settings
  unsigned int m_videoWidth;                          // Video format resolution in pixels
  unsigned int m_videoHeight;                         // Video format resolution in lines
  float m_fFrameRate;                                 // Video frame rate
  NVVIOSIGNALFORMAT m_videoFormat;
  unsigned int m_numStreams;
  unsigned int m_activeJacks[NVAPI_MAX_VIO_JACKS];
  NVVIOSIGNALFORMATDETAIL m_signalFormatDetail;       // Video Signal Details

  HDC m_hDC;                                          // GPU device context
  GLuint m_videoSlot;                                 // GPU Video slot

  HVIDEOINPUTDEVICENV m_device;                       // Video input device
  NvU32 m_vioID;                                      // Unique video device ID
  NvVioHandle m_vioHandle;                            // Video device handle
  int m_deviceNumber;

  //GPU object settings
  GLfloat m_cscMat[4][4];
  GLfloat m_cscMax[4];
  GLfloat m_cscMin[4];
  GLfloat m_cscOffset[4];

  bool m_bCaptureStarted;                             // Set to true when glBeginVideoCaptureNV had successfully completed

  GLuint m_captureTimeQuery;
} CNvSDIin;

#endif NVSDIIN_H