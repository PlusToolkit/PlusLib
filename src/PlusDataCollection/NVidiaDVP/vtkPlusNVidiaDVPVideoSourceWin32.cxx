/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

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

vtkStandardNewMacro(vtkPlusNvidiaDVPVideoSource);

//----------------------------------------------------------------------------
vtkPlusNvidiaDVPVideoSource::vtkPlusNvidiaDVPVideoSource()
  : FrameNumber(0)
  , EnableGPUCPUCopy(false)
  , VideoSize( { 0, 0, 1 })
{
  // No callback function provided by the device, so the data capture thread will be used to poll the hardware and add new items to the buffer
  this->StartThreadForInternalUpdates = true;
  this->AcquisitionRate = 30;
}

//----------------------------------------------------------------------------
vtkPlusNvidiaDVPVideoSource::~vtkPlusNvidiaDVPVideoSource()
{
  if (this->Connected)
  {
    this->Disconnect();
  }
}

//----------------------------------------------------------------------------
void vtkPlusNvidiaDVPVideoSource::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusNvidiaDVPVideoSource::InternalConnect()
{
  LOG_TRACE("vtkPlusPhilips3DProbeVideoSource::InternalConnect");

  NvGPU = CNvGpuTopology::Instance().GetGpu(NvOptions.captureGPU);
  if (NvGPU == nullptr)
  {
    LOG_ERROR("Unable to retrieve NvGPU.");
    return PLUS_FAIL;
  }

  if (SetupSDIDevices() == E_FAIL)
  {
    return PLUS_FAIL;
  }

  SetupGL();

  if (StartSDIPipeline() == E_FAIL)
  {
    return PLUS_FAIL;
  }

  CPUFrame = new unsigned char[NvSDIin.GetBufferObjectPitch(0) * VideoSize[1]];

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusNvidiaDVPVideoSource::InternalDisconnect()
{
  LOG_TRACE("vtkPlusNvidiaDVPVideoSource::InternalDisconnect");

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

  if (EnableGPUCPUCopy)
  {
    CopyGPUToCPU();
    if (OutputDataSource->AddItem((void*)CPUFrame,
                                  OutputDataSource->GetInputImageOrientation(),
                                  OutputDataSource->GetInputFrameSize(),
                                  VTK_UNSIGNED_CHAR, // TODO : scalar type from nvidia video format
                                  1, // TODO : num components from nvidia video format
                                  US_IMG_BRIGHTNESS, // TODO : img type from nvidia video format
                                  0,
                                  this->FrameNumber) != PLUS_SUCCESS)
    {
      LOG_ERROR("Unable to capture frame from SDI capture.");
      return PLUS_FAIL;
    }
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusNvidiaDVPVideoSource::ReadConfiguration(vtkXMLDataElement* rootConfigElement)
{
  LOG_TRACE("vtkPlusNvidiaDVPVideoSource::ReadConfiguration");
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);

  XML_READ_BOOL_ATTRIBUTE_OPTIONAL(EnableGPUCPUCopy, deviceConfig);

  int numGPUs;
  // Note, this function enumerates GPUs which are both CUDA & GLAffinity capable (i.e. newer Quadros)
  numGPUs = CNvGpuTopology::Instance().GetNumGpu();

  if (numGPUs <= 0)
  {
    LOG_ERROR("Unable to obtain system GPU topology.");
    return PLUS_FAIL;
  }

  int numCaptureDevices = CNvSDIinTopology::Instance().GetNumDevice();

  if (numCaptureDevices <= 0)
  {
    LOG_ERROR("Unable to obtain system Capture topology.");
    return PLUS_FAIL;
  }

  // Set the defaults for all the relevant options
  NvOptions.dualLink = false;
  NvOptions.expansionEnable = false;
  NvOptions.sampling = NVVIOCOMPONENTSAMPLING_422;
  NvOptions.bitsPerComponent = 8;
  NvOptions.captureDevice = 0;
  NvOptions.captureGPU = CNvGpuTopology::Instance().GetPrimaryGpuIndex();
  NvOptions.console = false;
  NvOptions.numFrames = 0;
  NvOptions.log = false;
  NvOptions.repeat = false;
  NvOptions.fps = false;
  NvOptions.videoInfo = false;

  // Get framelock flag
  if (deviceConfig->GetAttribute("Framelock") != NULL)
  {
    NvOptions.frameLock = false;
    if (STRCASECMP(deviceConfig->GetAttribute("Framelock"), "TRUE") == 0)
    {
      NvOptions.frameLock = true;
    }
  }

  // Set blocking flag
  if (deviceConfig->GetAttribute("Blocking") != NULL)
  {
    NvOptions.block = false;
    if (STRCASECMP(deviceConfig->GetAttribute("Blocking"), "TRUE") == 0)
    {
      NvOptions.block = true;
    }
  }

  // Set FSAA flag
  if (deviceConfig->GetAttribute("FSAA") != NULL)
  {
    NvOptions.fsaa = atoi(deviceConfig->GetAttribute("FSAA"));
  }

  // Set frame/field enum
  if (deviceConfig->GetAttribute("Field") != NULL)
  {
    NvOptions.field = false;
    if (STRCASECMP(deviceConfig->GetAttribute("Field"), "TRUE") == 0)
    {
      NvOptions.field = true;
    }
  }

  // Colorspace conversion scale vector
  if (deviceConfig->GetAttribute("ConversionScaleFactor") != NULL)
  {
    double tmpValue[3];
    if (deviceConfig->GetVectorAttribute("ConversionScaleFactor", 3, tmpValue) == 3)
    {
      NvOptions.cscScale[0] = tmpValue[0];
      NvOptions.cscScale[1] = tmpValue[1];
      NvOptions.cscScale[2] = tmpValue[2];
    }
  }

  // Colorspace conversion offset vector
  if (deviceConfig->GetAttribute("ConversionOffsetVector") != NULL)
  {
    double tmpValue[3];
    if (deviceConfig->GetVectorAttribute("ConversionOffsetVector", 3, tmpValue) == 3)
    {
      NvOptions.cscOffset[0] = tmpValue[0];
      NvOptions.cscOffset[1] = tmpValue[1];
      NvOptions.cscOffset[2] = tmpValue[2];
    }
  }

  // Colorspace conversion matrix
  if (deviceConfig->GetAttribute("ConversionMatrix") != NULL)
  {
    float tmpValue[9];
    if (deviceConfig->GetVectorAttribute("ConversionMatrix", 9, tmpValue) == 9)
    {
      NvOptions.cscMatrix[0][0] = tmpValue[0];
      NvOptions.cscMatrix[0][1] = tmpValue[1];
      NvOptions.cscMatrix[0][2] = tmpValue[2];

      NvOptions.cscMatrix[1][0] = tmpValue[3];
      NvOptions.cscMatrix[1][1] = tmpValue[4];
      NvOptions.cscMatrix[1][2] = tmpValue[5];

      NvOptions.cscMatrix[2][0] = tmpValue[6];
      NvOptions.cscMatrix[2][1] = tmpValue[7];
      NvOptions.cscMatrix[2][2] = tmpValue[8];
    }
  }

  // Gamma correction vector values
  if (deviceConfig->GetAttribute("GammaCorrection") != NULL)
  {
    float tmpValue[3];
    if (deviceConfig->GetVectorAttribute("GammaCorrection", 3, tmpValue) == 3)
    {
      NvOptions.gamma[0] = tmpValue[0];
      NvOptions.gamma[1] = tmpValue[1];
      NvOptions.gamma[2] = tmpValue[2];
    }
    if (NvOptions.gamma[0] < 0.5 || NvOptions.gamma[0] > 6.0 ||
        NvOptions.gamma[1] < 0.5 || NvOptions.gamma[1] > 6.0 ||
        NvOptions.gamma[2] < 0.5 || NvOptions.gamma[2] > 6.0)
    {
      LOG_ERROR("Illegal gamma values specified.");
      return PLUS_FAIL;
    }
  }

  // Flip queue length
  int flipQueueLength;
  if (deviceConfig->GetScalarAttribute("FlipQueueLength", flipQueueLength) != NULL)
  {
    NvOptions.flipQueueLength = flipQueueLength;
    if (NvOptions.flipQueueLength < 2 || NvOptions.flipQueueLength > 7)
    {
      LOG_ERROR("Illegal number of flip queue buffers specified.");
      return PLUS_FAIL;
    }
  }

  // GPU
  int gpu;
  if (deviceConfig->GetScalarAttribute("GPU", gpu) != NULL)
  {
    NvOptions.gpu = gpu;
    if (NvOptions.gpu > 2)
    {
      LOG_ERROR("Illegal GPU specified.");
      return PLUS_FAIL;
    }
  }

  // Video format
  std::string videoFormat;
  XML_READ_STRING_ATTRIBUTE_NONMEMBER_OPTIONAL(VideoFormat, videoFormat, deviceConfig);
  if (videoFormat == "487i5994_259")
  {
    VideoSize[1] = 487;
    NvOptions.videoFormat = NVVIOSIGNALFORMAT_487I_59_94_SMPTE259_NTSC;
  }
  else if (videoFormat == "576i5000_259")
  {
    VideoSize[1] = 576;
    NvOptions.videoFormat = NVVIOSIGNALFORMAT_576I_50_00_SMPTE259_PAL;
  }
  else if (videoFormat == "720p2398_296")
  {
    VideoSize[1] = 720;
    NvOptions.videoFormat = NVVIOSIGNALFORMAT_720P_23_98_SMPTE296;
  }
  else if (videoFormat == "720p2400_296")
  {
    VideoSize[1] = 720;
    NvOptions.videoFormat = NVVIOSIGNALFORMAT_720P_24_00_SMPTE296;
  }
  else if (videoFormat == "720p2500_296")
  {
    VideoSize[1] = 720;
    NvOptions.videoFormat = NVVIOSIGNALFORMAT_720P_25_00_SMPTE296;
  }
  else if (videoFormat == "720p2997_296")
  {
    VideoSize[1] = 720;
    NvOptions.videoFormat = NVVIOSIGNALFORMAT_720P_29_97_SMPTE296;
  }
  else if (videoFormat == "720p3000_296")
  {
    VideoSize[1] = 720;
    NvOptions.videoFormat = NVVIOSIGNALFORMAT_720P_30_00_SMPTE296;
  }
  else if (videoFormat == "720p5000_296")
  {
    VideoSize[1] = 720;
    NvOptions.videoFormat = NVVIOSIGNALFORMAT_720P_50_00_SMPTE296;
  }
  else if (videoFormat == "720p5994_296")
  {
    VideoSize[1] = 720;
    NvOptions.videoFormat = NVVIOSIGNALFORMAT_720P_59_94_SMPTE296;
  }
  else if (videoFormat == "720p6000_296")
  {
    VideoSize[1] = 720;
    NvOptions.videoFormat = NVVIOSIGNALFORMAT_720P_60_00_SMPTE296;
  }
  else if (videoFormat == "1035i5994_260")
  {
    VideoSize[1] = 1035;
    NvOptions.videoFormat = NVVIOSIGNALFORMAT_1035I_59_94_SMPTE260;
  }
  else if (videoFormat == "1035i6000_260")
  {
    VideoSize[1] = 1035;
    NvOptions.videoFormat = NVVIOSIGNALFORMAT_1035I_60_00_SMPTE260;
  }
  else if (videoFormat == "1080i4796_274")
  {
    VideoSize[1] = 1080;
    NvOptions.videoFormat = NVVIOSIGNALFORMAT_1080I_47_96_SMPTE274;
  }
  else if (videoFormat == "1080i4800_274")
  {
    VideoSize[1] = 1080;
    NvOptions.videoFormat = NVVIOSIGNALFORMAT_1080I_48_00_SMPTE274;
  }
  else if (videoFormat == "1080i5000_295")
  {
    VideoSize[1] = 1080;
    NvOptions.videoFormat = NVVIOSIGNALFORMAT_1080I_50_00_SMPTE295;
  }
  else if (videoFormat == "1080i5000_274")
  {
    VideoSize[1] = 1080;
    NvOptions.videoFormat = NVVIOSIGNALFORMAT_1080I_50_00_SMPTE274;
  }
  else if (videoFormat == "1080i5994_274")
  {
    VideoSize[1] = 1080;
    NvOptions.videoFormat = NVVIOSIGNALFORMAT_1080I_59_94_SMPTE274;
  }
  else if (videoFormat == "1080i6000_274")
  {
    VideoSize[1] = 1080;
    NvOptions.videoFormat = NVVIOSIGNALFORMAT_1080I_60_00_SMPTE274;
  }
  else if (videoFormat == "1080psf23976_274")
  {
    VideoSize[1] = 1080;
    NvOptions.videoFormat = NVVIOSIGNALFORMAT_1080PSF_23_98_SMPTE274;
  }
  else if (videoFormat == "1080psf2398_274")
  {
    VideoSize[1] = 1080;
    NvOptions.videoFormat = NVVIOSIGNALFORMAT_1080PSF_23_98_SMPTE274;
  }
  else if (videoFormat == "1080psf2400_274")
  {
    VideoSize[1] = 1080;
    NvOptions.videoFormat = NVVIOSIGNALFORMAT_1080PSF_24_00_SMPTE274;
  }
  else if (videoFormat == "1080psf2500_274")
  {
    VideoSize[1] = 1080;
    NvOptions.videoFormat = NVVIOSIGNALFORMAT_1080PSF_25_00_SMPTE274;
  }
  else if (videoFormat == "1080psf2997_274")
  {
    VideoSize[1] = 1080;
    NvOptions.videoFormat = NVVIOSIGNALFORMAT_1080PSF_29_97_SMPTE274;
  }
  else if (videoFormat == "1080psf3000_274")
  {
    VideoSize[1] = 1080;
    NvOptions.videoFormat = NVVIOSIGNALFORMAT_1080PSF_30_00_SMPTE274;
  }
  else if (videoFormat == "1080p23976_274")
  {
    VideoSize[1] = 1080;
    NvOptions.videoFormat = NVVIOSIGNALFORMAT_1080P_23_976_SMPTE274;
  }
  else if (videoFormat == "1080p2400_274")
  {
    VideoSize[1] = 1080;
    NvOptions.videoFormat = NVVIOSIGNALFORMAT_1080P_24_00_SMPTE274;
  }
  else if (videoFormat == "1080p2500_274")
  {
    VideoSize[1] = 1080;
    NvOptions.videoFormat = NVVIOSIGNALFORMAT_1080P_25_00_SMPTE274;
  }
  else if (videoFormat == "1080p2997_274")
  {
    VideoSize[1] = 1080;
    NvOptions.videoFormat = NVVIOSIGNALFORMAT_1080P_29_97_SMPTE274;
  }
  else if (videoFormat == "1080p3000_274")
  {
    VideoSize[1] = 1080;
    NvOptions.videoFormat = NVVIOSIGNALFORMAT_1080P_30_00_SMPTE274;
  }
  else if (videoFormat == "2048i4796_372")
  {
    VideoSize[1] = 2048;
    NvOptions.videoFormat = NVVIOSIGNALFORMAT_2048I_47_96_SMPTE372;
  }
  else if (videoFormat == "2048i4800_372")
  {
    VideoSize[1] = 2048;
    NvOptions.videoFormat = NVVIOSIGNALFORMAT_2048I_48_00_SMPTE372;
  }
  else if (videoFormat == "2048i5000_372")
  {
    VideoSize[1] = 2048;
    NvOptions.videoFormat = NVVIOSIGNALFORMAT_2048I_50_00_SMPTE372;
  }
  else if (videoFormat == "2048i5994_372")
  {
    VideoSize[1] = 2048;
    NvOptions.videoFormat = NVVIOSIGNALFORMAT_2048I_59_94_SMPTE372;
  }
  else if (videoFormat == "2048i6000_372")
  {
    VideoSize[1] = 2048;
    NvOptions.videoFormat = NVVIOSIGNALFORMAT_2048I_60_00_SMPTE372;
  }
  else if (videoFormat == "2048p2398_372")
  {
    VideoSize[1] = 2048;
    NvOptions.videoFormat = NVVIOSIGNALFORMAT_2048P_23_98_SMPTE372;
  }
  else if (videoFormat == "2048p2400_372")
  {
    VideoSize[1] = 2048;
    NvOptions.videoFormat = NVVIOSIGNALFORMAT_2048P_24_00_SMPTE372;
  }
  else if (videoFormat == "2048p2500_372")
  {
    VideoSize[1] = 2048;
    NvOptions.videoFormat = NVVIOSIGNALFORMAT_2048P_25_00_SMPTE372;
  }
  else if (videoFormat == "2048p2997_372")
  {
    VideoSize[1] = 2048;
    NvOptions.videoFormat = NVVIOSIGNALFORMAT_2048P_29_97_SMPTE372;
  }
  else if (videoFormat == "2048p3000_372")
  {
    VideoSize[1] = 2048;
    NvOptions.videoFormat = NVVIOSIGNALFORMAT_2048P_30_00_SMPTE372;
  }
  else
  {
    LOG_WARNING("Invalid video format in configuration file.");
    NvOptions.videoFormat = NVVIOSIGNALFORMAT_NONE;
  }

  // Data format
  std::string dataFormat;
  XML_READ_STRING_ATTRIBUTE_NONMEMBER_OPTIONAL(DataFormat, dataFormat, deviceConfig);
  if (dataFormat == "r8g8b8_to_ycrcb444")
  {
    NvOptions.dataFormat = NVVIODATAFORMAT_R8G8B8_TO_YCRCB444;
  }
  else if (dataFormat == "r8g8b8a8_to_ycrcba4444")
  {
    NvOptions.dataFormat = NVVIODATAFORMAT_R8G8B8A8_TO_YCRCBA4444;
  }
  else if (dataFormat == "r8g8b8_to_ycrcb422")
  {
    NvOptions.dataFormat = NVVIODATAFORMAT_R8G8B8_TO_YCRCB422;
  }
  else if (dataFormat == "r8g8b8a8_to_ycrcb4224")
  {
    NvOptions.dataFormat = NVVIODATAFORMAT_R8G8B8A8_TO_YCRCBA4224;
  }
  else if (dataFormat == "r8g8b8_to_rgb444")
  {
    NvOptions.dataFormat = NVVIODATAFORMAT_X8X8X8_444_PASSTHRU;
  }
  else if (dataFormat == "r8g8b8a8_to_rgb4444")
  {
    NvOptions.dataFormat = NVVIODATAFORMAT_X8X8X8_444_PASSTHRU;
  }
  else if (dataFormat == "y10cr10cb10_to_ycrcb444")
  {
    NvOptions.dataFormat = NVVIODATAFORMAT_X10X10X10_444_PASSTHRU;
  }
  else if (dataFormat == "y10cr8cb8_to_ycrcb444")
  {
    NvOptions.dataFormat = NVVIODATAFORMAT_X10X10X10_444_PASSTHRU;
  }
  else if (dataFormat == "y10cr8cb8a10_to_ycrcb4444")
  {
    NvOptions.dataFormat = NVVIODATAFORMAT_X10X8X8A10_4444_PASSTHRU;
  }
  else if (dataFormat == "dual_r8g8b8_to_dual_ycrcb422")
  {
    NvOptions.dataFormat = NVVIODATAFORMAT_DUAL_R8G8B8_TO_DUAL_YCRCB422;
  }
  else if (dataFormat == "dual_y8cr8cb8_to_dual_ycrcb422")
  {
    NvOptions.dataFormat = NVVIODATAFORMAT_DUAL_X8X8X8_TO_DUAL_422_PASSTHRU;
  }
  else if (dataFormat == "r10g10b10_to_ycrcb422")
  {
    NvOptions.dataFormat = NVVIODATAFORMAT_R10G10B10_TO_YCRCB422;
  }
  else if (dataFormat == "r10g10b10_to_ycrcb444")
  {
    NvOptions.dataFormat = NVVIODATAFORMAT_R10G10B10_TO_YCRCB422;
  }
  else if (dataFormat == "y12cr12cb12_to_ycrcb444")
  {
    NvOptions.dataFormat = NVVIODATAFORMAT_X12X12X12_444_PASSTHRU;
  }
  else if (dataFormat == "y12cr12cb12_to_ycrcb422")
  {
    NvOptions.dataFormat = NVVIODATAFORMAT_X12X12X12_422_PASSTHRU;
  }
  else if (dataFormat == "y10cr10cb10_to_ycrcb422")
  {
    NvOptions.dataFormat = NVVIODATAFORMAT_Y10CR10CB10_TO_YCRCB422;
  }
  else if (dataFormat == "y8cr8cb8_to_ycrcb422")
  {
    NvOptions.dataFormat = NVVIODATAFORMAT_Y8CR8CB8_TO_YCRCB422;
  }
  else if (dataFormat == "y10cr8cb8a10_to_ycrcba4224")
  {
    NvOptions.dataFormat = NVVIODATAFORMAT_Y10CR8CB8A10_TO_YCRCBA4224;
  }
  else if (dataFormat == "r10g10b10_to_rgb444")
  {
    NvOptions.dataFormat = NVVIODATAFORMAT_R10G10B10_TO_RGB444;
  }
  else if (dataFormat == "r12g12b12_to_rgb444")
  {
    NvOptions.dataFormat = NVVIODATAFORMAT_X12X12X12_444_PASSTHRU;
  }
  else
  {
    NvOptions.dataFormat = NVVIODATAFORMAT_R8G8B8A8_TO_YCRCBA4224;
  }

  // Sync source
  if (deviceConfig->GetAttribute("SyncSource") != NULL)
  {
    std::string syncSource(deviceConfig->GetAttribute("SyncSource"));
    if (syncSource == "bi")
    {
      NvOptions.syncType = NVVIOCOMPSYNCTYPE_BILEVEL;
      NvOptions.syncSource = NVVIOSYNCSOURCE_COMPSYNC;
      NvOptions.syncEnable = TRUE;
    }
    else if (syncSource == "tri")
    {
      NvOptions.syncType = NVVIOCOMPSYNCTYPE_TRILEVEL;
      NvOptions.syncSource = NVVIOSYNCSOURCE_COMPSYNC;
      NvOptions.syncEnable = TRUE;
    }
    else if (syncSource == "auto")
    {
      NvOptions.syncType = NVVIOCOMPSYNCTYPE_AUTO;
      NvOptions.syncSource = NVVIOSYNCSOURCE_COMPSYNC;
      NvOptions.syncEnable = TRUE;
    }
    else if (syncSource == "sdi")
    {
      NvOptions.syncSource = NVVIOSYNCSOURCE_SDISYNC;
      NvOptions.syncEnable = TRUE;
    }
    else
    {
      NvOptions.syncEnable = FALSE;
    }
  }

  // Alpha
  if (deviceConfig->GetAttribute("Alpha") != NULL)
  {
    NvOptions.alphaComp = false;
    if (STRCASECMP(deviceConfig->GetAttribute("Alpha"), "TRUE") == 0)
    {
      NvOptions.alphaComp = true;
    }
  }

  // Get compositing types and parameters
  if (deviceConfig->GetAttribute("cr") != NULL)
  {
    NvOptions.crComp = true;
    int tmpValue[4];
    if (deviceConfig->GetVectorAttribute("cr", 4, tmpValue) == 4)
    {
      NvOptions.crCompRange[0] = tmpValue[0];
      NvOptions.crCompRange[1] = tmpValue[1];
      NvOptions.crCompRange[2] = tmpValue[2];
      NvOptions.crCompRange[3] = tmpValue[3];
    }
  }

  if (deviceConfig->GetAttribute("cb") != NULL)
  {
    NvOptions.cbComp = true;
    int tmpValue[4];
    if (deviceConfig->GetVectorAttribute("cb", 4, tmpValue) == 4)
    {
      NvOptions.cbCompRange[0] = tmpValue[0];
      NvOptions.cbCompRange[1] = tmpValue[1];
      NvOptions.cbCompRange[2] = tmpValue[2];
      NvOptions.cbCompRange[3] = tmpValue[3];
    }
  }

  if (deviceConfig->GetAttribute("y") != NULL)
  {
    NvOptions.yComp = true;
    int tmpValue[4];
    if (deviceConfig->GetVectorAttribute("y", 4, tmpValue) == 4)
    {
      NvOptions.yCompRange[0] = tmpValue[0];
      NvOptions.yCompRange[1] = tmpValue[1];
      NvOptions.yCompRange[2] = tmpValue[2];
      NvOptions.yCompRange[3] = tmpValue[3];
    }
  }

  // Horizontal and vertical delay
  int horizontalDelay;
  if (deviceConfig->GetScalarAttribute("HorizontalDelay", horizontalDelay) != NULL)
  {
    NvOptions.hDelay = horizontalDelay;
  }
  int verticalDelay;
  if (deviceConfig->GetScalarAttribute("VerticalDelay", verticalDelay) != NULL)
  {
    NvOptions.vDelay = verticalDelay;
  }

  // Capture gpu
  int captureGPU;
  if (deviceConfig->GetScalarAttribute("CaptureGPU", captureGPU) != NULL)
  {
    NvOptions.captureGPU = captureGPU;
  }

  // Capture device
  int captureDevice;
  if (deviceConfig->GetScalarAttribute("CaptureDevice", captureDevice) != NULL)
  {
    NvOptions.captureDevice = captureDevice;
  }

  // Sampling
  std::string sampling;
  XML_READ_STRING_ATTRIBUTE_NONMEMBER_OPTIONAL(Sampling, sampling, deviceConfig);
  if (sampling == "422")
  {
    NvOptions.sampling = NVVIOCOMPONENTSAMPLING_422;
  }
  else if (sampling == "444")
  {
    NvOptions.sampling = NVVIOCOMPONENTSAMPLING_444;
  }
  else if (sampling == "4224")
  {
    NvOptions.sampling = NVVIOCOMPONENTSAMPLING_4224;
  }
  else if (sampling == "4444")
  {
    NvOptions.sampling = NVVIOCOMPONENTSAMPLING_4444;
  }

  // Bits per component
  int bitsPerComponent;
  if (deviceConfig->GetScalarAttribute("BitsPerComponent", bitsPerComponent) != NULL)
  {
    NvOptions.bitsPerComponent = bitsPerComponent;
  }

  switch (NvOptions.sampling)
  {
    case NVVIOCOMPONENTSAMPLING_422:
      if (NvOptions.bitsPerComponent == 8)
      {
        VideoBufferFormat = GL_YCBYCR8_422_NV;
      }
      else if (NvOptions.bitsPerComponent == 10)
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
      if (NvOptions.bitsPerComponent == 8)
      {
        VideoBufferFormat = GL_YCBAYCR8A_4224_NV;
      }
      else if (NvOptions.bitsPerComponent == 10)
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
      if (NvOptions.bitsPerComponent == 8)
      {
        VideoBufferFormat = GL_RGB8;
      }
      else if (NvOptions.bitsPerComponent == 10)
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
      if (NvOptions.bitsPerComponent == 8)
      {
        VideoBufferFormat = GL_RGBA8;
      }
      else if (NvOptions.bitsPerComponent == 10)
      {
        VideoBufferFormat = GL_RGBA12;
      }
      else //12 bit
      {
        VideoBufferFormat = GL_RGBA12;
      }
      break;
  }

  if (NvOptions.captureDevice >= numCaptureDevices)
  {
    LOG_ERROR("Selected Capture Device is out of range.");
    return PLUS_FAIL;
  }
  if (NvOptions.captureGPU >= numGPUs)
  {
    LOG_ERROR("Selected Capture GPU is out of range.");
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusNvidiaDVPVideoSource::WriteConfiguration(vtkXMLDataElement* rootConfig)
{
  LOG_TRACE("vtkPlusNvidiaDVPVideoSource::WriteConfiguration");
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING(deviceConfig, rootConfig);

  XML_WRITE_BOOL_ATTRIBUTE(EnableGPUCPUCopy, deviceConfig);

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusNvidiaDVPVideoSource::NotifyConfigured()
{
  if (this->OutputChannels.size() != 1 && this->EnableGPUCPUCopy)
  {
    LOG_ERROR("Incorrect configuration. GPU/CPU copy and OutputChannel configuration are incompatible.");
    this->SetCorrectlyConfigured(false);
    return PLUS_FAIL;
  }

  if (this->EnableGPUCPUCopy && this->GetFirstVideoSource(OutputDataSource) != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to find video source. Device needs a video buffer to put new frames into when copying frames from the GPU.");
    this->SetCorrectlyConfigured(false);
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
  if (SetupSDIinDevices() != S_OK)
  {
    LOG_ERROR("Error setting up video capture.");
    return E_FAIL;
  }

  return S_OK;
}

//-----------------------------------------------------------------------------
HRESULT vtkPlusNvidiaDVPVideoSource::StartSDIPipeline()
{
  // Start video capture
  if (NvSDIin.StartCapture() != S_OK)
  {
    LOG_ERROR("Error starting video capture.");
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
  NvSDIin.Init(&NvOptions);

  // Initialize the video capture device.
  if (NvSDIin.SetupDevice(NvOptions.captureDevice) != S_OK)
  {
    return E_FAIL;
  }

  VideoSize[0] = NvSDIin.GetWidth();
  VideoSize[1] = NvSDIin.GetHeight();

  return S_OK;
}

//-----------------------------------------------------------------------------
HRESULT vtkPlusNvidiaDVPVideoSource::SetupSDIinGL()
{
  GLuint gpuVideoSlot = 1;

  NvSDIin.BindDevice(gpuVideoSlot, HandleDC);

  glGenBuffers(NvSDIin.GetNumStreams(), VideoBufferObject);

  int videoBufferPitch = 0;
  for (unsigned int i = 0; i < NvSDIin.GetNumStreams(); i++)
  {
    NvSDIin.BindVideoFrameBuffer(VideoBufferObject[i], VideoBufferFormat, i);
    videoBufferPitch = NvSDIin.GetBufferObjectPitch(i);

    // Allocate required space in video capture buffer
    glBindBuffer(GL_VIDEO_BUFFER_NV, VideoBufferObject[i]);
    assert(glGetError() == GL_NO_ERROR);

    glBufferData(GL_VIDEO_BUFFER_NV, videoBufferPitch * VideoHeight, NULL, GL_STREAM_COPY);
    assert(glGetError() == GL_NO_ERROR);
  }

  return S_OK;
}

//-----------------------------------------------------------------------------
GLboolean vtkPlusNvidiaDVPVideoSource::SetupGL()
{
  HGPUNV gpuMask[2];
  gpuMask[0] = NvGPU->getAffinityHandle();
  gpuMask[1] = NULL;
  if (!(HandleDC = wglCreateAffinityDCNV(gpuMask)))
  {
    LOG_WARNING("Unable to create GPU affinity DC.");
  }

  PIXELFORMATDESCRIPTOR pfd =             // pfd Tells Windows How We Want Things To Be
  {
    sizeof(PIXELFORMATDESCRIPTOR),       // Size Of This Pixel Format Descriptor
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
  GLuint pf = ChoosePixelFormat(HandleDC, &pfd);
  BOOL result = SetPixelFormat(HandleDC, pf, &pfd);
  if (result == FALSE)
  {
    return GL_FALSE;
  }
  // Create rendering context from the affinity device context
  HandleGLRC = wglCreateContext(HandleDC);

  // Make window rendering context current.
  wglMakeCurrent(HandleDC, HandleGLRC);

  //load the required OpenGL extensions:
  if (!loadCaptureVideoExtension() || !loadTimerQueryExtension() || !loadBufferObjectExtension())
  {
    LOG_ERROR("Could not load the required OpenGL extensions.");
    return false;
  }

  glClearColor(0.0, 0.0, 0.0, 0.0);
  glClearDepth(1.0);

  glDisable(GL_DEPTH_TEST);

  glDisable(GL_TEXTURE_1D);
  glDisable(GL_TEXTURE_2D);

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

  if (numFails < 100)
  {
    // Capture the video to a buffer object
    ret = NvSDIin.Capture(&sequenceNum, &captureTime);
    if (sequenceNum - prevSequenceNum > 1)
    {
      LOG_WARNING("glVideoCaptureNV: Dropped " << sequenceNum - prevSequenceNum << " frames.");
    }
    prevSequenceNum = sequenceNum;
    switch (ret)
    {
      case GL_SUCCESS_NV:
        LOG_DEBUG("Frame: " << sequenceNum << ". gpuTime: " << NvSDIin.m_gpuTime  << ". gviTime: " << NvSDIin.m_gviTime << ".");
        numFails = 0;
        break;
      case GL_PARTIAL_SUCCESS_NV:
        LOG_WARNING("glVideoCaptureNV: GL_PARTIAL_SUCCESS_NV.");
        numFails = 0;
        break;
      case GL_FAILURE_NV:
        LOG_ERROR("glVideoCaptureNV: GL_FAILURE_NV - Video capture failed.");
        numFails++;
        break;
      default:
        LOG_ERROR("glVideoCaptureNV: Unknown return value.");
        break;
    }
  }
  // The incoming signal format or some other error occurred during
  // capture, shutdown and try to restart capture.
  else
  {
    if (numTries == 0)
    {
      StopSDIPipeline();
      CleanupSDIDevices();
      CleanupGL();
    }
    // Initialize the video capture device.
    if (NvSDIin.SetupDevice(NvOptions.captureDevice) != S_OK)
    {
      numTries++;
      return GL_FAILURE_NV;
    }

    // Reinitialize OpenGL.
    SetupGL();
    StartSDIPipeline();
    numFails = 0;
    numTries = 0;
    return GL_FAILURE_NV;
  }
  return ret;
}

//-----------------------------------------------------------------------------
HRESULT vtkPlusNvidiaDVPVideoSource::CleanupSDIinGL()
{
  for (unsigned int i = 0; i < NvSDIin.GetNumStreams(); i++)
  {
    NvSDIin.UnbindVideoFrameBuffer(i);
  }
  NvSDIin.UnbindDevice();
  glDeleteBuffers(NvSDIin.GetNumStreams(), VideoBufferObject);
  return S_OK;
}

//-----------------------------------------------------------------------------
GLboolean vtkPlusNvidiaDVPVideoSource::CleanupGL()
{
  CleanupSDIinGL();

  // Delete OpenGL rendering context.
  wglMakeCurrent(NULL, NULL);

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
  glBindBuffer(GL_VIDEO_BUFFER_NV, VideoBufferObject[0]);

  // Transfer contents of video buffer(s) to system memory
  glGetBufferSubData(GL_VIDEO_BUFFER_NV, 0, NvSDIin.GetBufferObjectPitch(0) * VideoSize[1], CPUFrame);

  return S_OK;
}