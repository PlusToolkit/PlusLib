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

#include "nvSDIutil.h"

std::string SignalFormatToString( NVVIOSIGNALFORMAT format )
{
  std::string str;

  switch( format )
  {
  case NVVIOSIGNALFORMAT_NONE:
    str = "None";
    break;

  case NVVIOSIGNALFORMAT_487I_59_94_SMPTE259_NTSC:
    str = "487i 59.95 (SMPTE 259)";
    break;

  case NVVIOSIGNALFORMAT_576I_50_00_SMPTE259_PAL:
    str = "576i 50.00 (SMPTE 259)";
    break;

  case NVVIOSIGNALFORMAT_720P_59_94_SMPTE296:
    str = "720p 59.94 (SMPTE 296)";
    break;

  case NVVIOSIGNALFORMAT_720P_60_00_SMPTE296:
    str = "720p 60.00 (SMPTE 296)";
    break;

  case NVVIOSIGNALFORMAT_1035I_59_94_SMPTE260:
    str = "1035i 59.94 (SMPTE 260)";
    break;

  case NVVIOSIGNALFORMAT_1035I_60_00_SMPTE260:
    str = "1035i 60.00 (SMPTE 260)";
    break;

  case NVVIOSIGNALFORMAT_1080I_50_00_SMPTE295:
    str = "1080i 50.00 (SMPTE 295)";
    break;

  case NVVIOSIGNALFORMAT_1080I_50_00_SMPTE274:
    str = "1080i 50.00 (SMPTE 274)";
    break;

  case NVVIOSIGNALFORMAT_1080I_59_94_SMPTE274:
    str = "1080i 59.94 (SMPTE 274)";
    break;

  case NVVIOSIGNALFORMAT_1080I_60_00_SMPTE274:
    str = "1080i 60.00 (SMPTE 274)";
    break;

  case NVVIOSIGNALFORMAT_1080P_23_976_SMPTE274:
    str = "1080p 23.976 (SMPTE 274)";
    break;

  case NVVIOSIGNALFORMAT_1080P_24_00_SMPTE274:
    str = "1080p 24.00 (SMPTE 274)";
    break;

  case NVVIOSIGNALFORMAT_1080P_25_00_SMPTE274:
    str = "1080p 25.00 (SMPTE 274)";
    break;

  case NVVIOSIGNALFORMAT_1080P_29_97_SMPTE274:
    str = "1080p 29.97 (SMPTE 274)";
    break;

  case NVVIOSIGNALFORMAT_1080P_30_00_SMPTE274:
    str = "1080p 30.00 (SMPTE 274)";
    break;

  case NVVIOSIGNALFORMAT_720P_50_00_SMPTE296:
    str = "720p 50.00 (SMPTE 296)";
    break;

  case NVVIOSIGNALFORMAT_1080I_48_00_SMPTE274:
    str = "1080i 48.00 (SMPTE 274)";
    break;

  case NVVIOSIGNALFORMAT_1080I_47_96_SMPTE274:
    str = "1080i 47.96 (SMPTE 274)";
    break;

  case NVVIOSIGNALFORMAT_720P_30_00_SMPTE296:
    str = "720p 30.00 (SMPTE 296)";
    break;

  case NVVIOSIGNALFORMAT_720P_29_97_SMPTE296:
    str = "720p 29.97 (SMPTE 296)";
    break;

  case NVVIOSIGNALFORMAT_720P_25_00_SMPTE296:
    str = "720p 25.00 (SMPTE 296)";
    break;

  case NVVIOSIGNALFORMAT_720P_24_00_SMPTE296:
    str = "720p 24.00 (SMPTE 296)";
    break;

  case NVVIOSIGNALFORMAT_720P_23_98_SMPTE296:
    str = "720p 23.98 (SMPTE 296)";
    break;

  case NVVIOSIGNALFORMAT_1080PSF_25_00_SMPTE274:
    str = "1080PsF 25.00 (SMPTE 274)";
    break;

  case NVVIOSIGNALFORMAT_1080PSF_29_97_SMPTE274:
    str = "1080PsF 29.97 (SMPTE 274)";
    break;

  case NVVIOSIGNALFORMAT_1080PSF_30_00_SMPTE274:
    str = "1080PsF 30.00 (SMPTE 274)";
    break;

  case NVVIOSIGNALFORMAT_1080PSF_24_00_SMPTE274:
    str = "1080PsF 24.00 (SMPTE 274)";
    break;

  case NVVIOSIGNALFORMAT_1080PSF_23_98_SMPTE274:
    str = "1080PsF 23.98 (SMPTE 274)";
    break;

  case NVVIOSIGNALFORMAT_2048P_30_00_SMPTE372:
    str = "2048p 30.00 (SMPTE 372)";
    break;

  case NVVIOSIGNALFORMAT_2048P_29_97_SMPTE372:
    str = "2048p 29.97 (SMPTE 372)";
    break;

  case NVVIOSIGNALFORMAT_2048I_60_00_SMPTE372:
    str = "2048i 60.00 (SMPTE 372)";
    break;

  case NVVIOSIGNALFORMAT_2048I_59_94_SMPTE372:
    str = "2048i 59.94 (SMPTE 372)";
    break;

  case NVVIOSIGNALFORMAT_2048P_25_00_SMPTE372:
    str = "2048p 25.00 (SMPTE 372)";
    break;

  case NVVIOSIGNALFORMAT_2048I_50_00_SMPTE372:
    str = "2048i 50.00 (SMPTE 372)";
    break;

  case NVVIOSIGNALFORMAT_2048P_24_00_SMPTE372:
    str = "2048p 24.00 (SMPTE 372)";
    break;

  case NVVIOSIGNALFORMAT_2048P_23_98_SMPTE372:
    str = "2048p 23.98 (SMPTE 372)";
    break;

  case NVVIOSIGNALFORMAT_2048I_48_00_SMPTE372:
    str = "2048i 48.00 (SMPTE 372)";
    break;

  case NVVIOSIGNALFORMAT_2048I_47_96_SMPTE372:
    str = "2048i 47.97 (SMPTE 372)";
    break;

  case NVVIOSIGNALFORMAT_1080P_50_00_SMPTE274_3G_LEVEL_A:
    str = "1080p 50.00Hz (SMPTE274) 3G Level A";
    break;

  case NVVIOSIGNALFORMAT_1080P_59_94_SMPTE274_3G_LEVEL_A:
    str = "1080p 59.94Hz (SMPTE274) 3G Level A";
    break;

  case NVVIOSIGNALFORMAT_1080P_60_00_SMPTE274_3G_LEVEL_A:
    str = "1080p 60.00Hz (SMPTE274) 3G Level A";
    break;

  case NVVIOSIGNALFORMAT_1080P_60_00_SMPTE274_3G_LEVEL_B:
    str = "1080p 60.00Hz (SMPTE274) 3G Level B";
    break;

  case NVVIOSIGNALFORMAT_1080I_60_00_SMPTE274_3G_LEVEL_B:
    str = "1080i 60.00Hz (SMPTE274) 3G Level B";
    break;

  case NVVIOSIGNALFORMAT_2048I_60_00_SMPTE372_3G_LEVEL_B:
    str = "2048i 60.00Hz (SMPTE372) 3G Level B";
    break;

  case NVVIOSIGNALFORMAT_1080P_50_00_SMPTE274_3G_LEVEL_B:
    str = "1080p 50.00Hz (SMPTE274) 3G Level B";
    break;

  case NVVIOSIGNALFORMAT_1080I_50_00_SMPTE274_3G_LEVEL_B:
    str = "1080i 50.00Hz (SMPTE274) 3G Level B";
    break;

  case NVVIOSIGNALFORMAT_2048I_50_00_SMPTE372_3G_LEVEL_B:
    str = "2048i 50.00Hz (SMPTE372) 3G Level B";
    break;

  case NVVIOSIGNALFORMAT_1080P_30_00_SMPTE274_3G_LEVEL_B:
    str = "1080p 30.00Hz (SMPTE274) 3G Level B";
    break;

  case NVVIOSIGNALFORMAT_2048P_30_00_SMPTE372_3G_LEVEL_B:
    str = "2048p 30.00Hz (SMPTE372) 3G Level B";
    break;

  case NVVIOSIGNALFORMAT_1080P_25_00_SMPTE274_3G_LEVEL_B:
    str = "1080p 25.00Hz (SMPTE274) 3G Level B";
    break;

  case NVVIOSIGNALFORMAT_2048P_25_00_SMPTE372_3G_LEVEL_B:
    str = "2048p 25.00Hz (SMPTE372) 3G Level B";
    break;

  case NVVIOSIGNALFORMAT_1080P_24_00_SMPTE274_3G_LEVEL_B:
    str = "1080p 24.00Hz (SMPTE274) 3G Level B";
    break;

  case NVVIOSIGNALFORMAT_2048P_24_00_SMPTE372_3G_LEVEL_B:
    str = "2048p 24.00Hz (SMPTE372) 3G Level B";
    break;

  case NVVIOSIGNALFORMAT_1080I_48_00_SMPTE274_3G_LEVEL_B:
    str = "1080i 48.00Hz (SMPTE274) 3G Level B";
    break;

  case NVVIOSIGNALFORMAT_2048I_48_00_SMPTE372_3G_LEVEL_B:
    str = "2048i 48.00Hz (SMPTE372) 3G Level B";
    break;

  case NVVIOSIGNALFORMAT_1080P_59_94_SMPTE274_3G_LEVEL_B:
    str = "1080p 59.94Hz (SMPTE274) 3G Level B";
    break;

  case NVVIOSIGNALFORMAT_1080I_59_94_SMPTE274_3G_LEVEL_B:
    str = "1080i 59.94Hz (SMPTE274) 3G Level B";
    break;

  case NVVIOSIGNALFORMAT_2048I_59_94_SMPTE372_3G_LEVEL_B:
    str = "2048i 59.94Hz (SMPTE372) 3G Level B";
    break;

  case NVVIOSIGNALFORMAT_1080P_29_97_SMPTE274_3G_LEVEL_B:
    str = "1080p 29.97Hz (SMPTE274) 3G Level B";
    break;

  case NVVIOSIGNALFORMAT_2048P_29_97_SMPTE372_3G_LEVEL_B:
    str = "2048p 29.97Hz (SMPTE372) 3G Level B";
    break;

  case NVVIOSIGNALFORMAT_1080P_23_98_SMPTE274_3G_LEVEL_B:
    str = "1080p 29.98Hz (SMPTE274) 3G Level B";
    break;

  case NVVIOSIGNALFORMAT_2048P_23_98_SMPTE372_3G_LEVEL_B:
    str = "2048p 29.98Hz (SMPTE372) 3G Level B";
    break;

  case NVVIOSIGNALFORMAT_1080I_47_96_SMPTE274_3G_LEVEL_B:
    str = "1080i 47.96Hz (SMPTE274) 3G Level B";
    break;

  case NVVIOSIGNALFORMAT_2048I_47_96_SMPTE372_3G_LEVEL_B:
    str = "2048i 47.96Hz (SMPTE372) 3G Level B";
    break;

  default:
    str = "Undefined";
    break;
  }

  return str;
}

std::string DataFormatToString( NVVIODATAFORMAT format )
{
  std::string str;

  switch ( format )
  {
  case NVVIODATAFORMAT_UNKNOWN:
    str = "Unknown";
    break;

  case NVVIODATAFORMAT_R8G8B8_TO_YCRCB444:
    str = "R8G8B8 to YCrCb 4:4:4";
    break;

  case NVVIODATAFORMAT_R8G8B8A8_TO_YCRCBA4444:
    str = "R8G8B8 to YCrCbA 4:4:4:4";
    break;

  case NVVIODATAFORMAT_R8G8B8Z10_TO_YCRCBZ4444:
    str = "R8G8B8Z10 to YCrCbZ 4:4:4:4";
    break;

  case NVVIODATAFORMAT_R8G8B8_TO_YCRCB422:
    str = "R8G8B8 to YCrCb 4:2:2";
    break;

  case NVVIODATAFORMAT_R8G8B8A8_TO_YCRCBA4224:
    str = "R8G8B8A8 to YCrCbA 4:2:2:4";
    break;

  case NVVIODATAFORMAT_R8G8B8Z10_TO_YCRCBZ4224:
    str = "R8G8B8Z10 to YCrCbZ 4:2:2:4";
    break;

  case NVVIODATAFORMAT_X8X8X8_444_PASSTHRU:
    str = "X8X8X8 to XXX 4:4:4 Pass Thru";
    break;

  case NVVIODATAFORMAT_X8X8X8A8_4444_PASSTHRU:
    str = "X8X8X8X8 to XXXX 4:4:4:4 Pass Thru";
    break;

  case NVVIODATAFORMAT_X8X8X8Z10_4444_PASSTHRU:
    str = "X8X8X8Z10 to XXXZ 4:4:4:4 Pass Thru";
    break;

  case NVVIODATAFORMAT_X10X10X10_444_PASSTHRU:
    str = "X10X10X10 to XXX 4:4:4 Pass Thru";
    break;

  case NVVIODATAFORMAT_X10X8X8_444_PASSTHRU:
    str = "X10X8X8 to XXX 4:4:4 Pass Thru";
    break;

  case NVVIODATAFORMAT_X10X8X8A10_4444_PASSTHRU:
    str = "X10X8X8A10 to XXXA 4:4:4:4 Pass Thru";
    break;

  case NVVIODATAFORMAT_X10X8X8Z10_4444_PASSTHRU:
    str = "X10X8X8Z10 to XXXZ 4:4:4:4 Pass Thru";
    break;

  case NVVIODATAFORMAT_DUAL_R8G8B8_TO_DUAL_YCRCB422:
    str = "Dual R8G8B8 to Dual YCrCb 4:2:2";
    break;

  case NVVIODATAFORMAT_DUAL_X8X8X8_TO_DUAL_422_PASSTHRU:
    str = "Dual X8X8X8 to Dual XXX 4:2:2 Pass Thru";
    break;

  case NVVIODATAFORMAT_R10G10B10_TO_YCRCB422:
    str = "R10G10B10 to YCrCb 4:2:2";
    break;

  case NVVIODATAFORMAT_X12X12X12_444_PASSTHRU:
    str = "X12X12X12 to XXX 4:4:4 Pass Thru";
    break;

  case NVVIODATAFORMAT_Y10CR10CB10_TO_YCRCB422:
    str = "Y10Cr10Cb10 to YCrCb 4:2:2";
    break;

  case NVVIODATAFORMAT_Y8CR8CB8_TO_YCRCB422:
    str = "Y8Cr8Cb8 to YCrCb 4:2:2";
    break;

  case NVVIODATAFORMAT_Y10CR8CB8A10_TO_YCRCBA4224:
    str = "Y10Cr8Cb8A10 to YCrCb 4:2:2:4";
    break;

  case NVVIODATAFORMAT_R10G10B10_TO_RGB444:
    str = "R10G10B10 to RGB 4:4:4";
    break;

  case NVVIODATAFORMAT_R12G12B12_TO_YCRCB444:
    str = "R12G12B12 to YCrCb 4:4:4";
    break;

  case NVVIODATAFORMAT_R12G12B12_TO_YCRCB422:
    str = "R12G12B12 to YCrCb 4:2:2";
    break;

  case NVVIODATAFORMAT_X12X12X12_422_PASSTHRU:
    str = "X12X12X12 to XXX 4:4:4 Pass Thru";
    break;

  default:
    str = "Unknown";
    break;
  }

  return str;
}

std::string SyncStatusToString( NVVIOSYNCSTATUS status )
{
  switch ( status )
  {
  case NVVIOSYNCSTATUS_OFF:
    return( "NVVIOSYNCSTATUS_OFF" );
    break;
  case NVVIOSYNCSTATUS_ERROR:
    return( "NVVIOSYNCSTATUS_ERROR" );
    break;
  case NVVIOSYNCSTATUS_SYNCLOSS:
    return( "NVVIOSYNCSTATUS_SYNCLOSS" );
    break;
  case NVVIOSYNCSTATUS_COMPOSITE:
    return( "NVVIOSYNCSTATUS_COMPOSITE" );
    break;
  case NVVIOSYNCSTATUS_SDI_SD:
    return( "NVVIOSYNCSTATUS_SDI_SD" );
    break;
  case NVVIOSYNCSTATUS_SDI_HD:
    return( "NVVIOSYNCSTATUS_SDI_HD" );
    break;
  default:
    return( "NVVIOSYNCSTATUS_ERROR" );
  }
}

std::string SyncSourceToString( NVVIOSYNCSOURCE source )
{
  switch ( source )
  {
  case NVVIOSYNCSOURCE_SDISYNC:
    return( "NVVIOSYNCSOURCE_SDISYNC" );
    break;

  case NVVIOSYNCSOURCE_COMPSYNC:
    return( "NVVIOSYNCSOURCE_COMPSYNC" );
    break;
  default:
    return( "NVVIOSYNCSOURCE_NONE" );
  }
}

std::string ComponentSamplingFormatToString( NVVIOCOMPONENTSAMPLING sampling )
{
  switch( sampling )
  {
  case NVVIOCOMPONENTSAMPLING_4444:
    return( "NVVIOCOMPONENTSAMPLING_4444" );
    break;
  case NVVIOCOMPONENTSAMPLING_4224:
    return( "NVVIOCOMPONENTSAMPLING_4224" );
    break;
  case NVVIOCOMPONENTSAMPLING_444:
    return( "NVVIOCOMPONENTSAMPLING_444" );
    break;
  case NVVIOCOMPONENTSAMPLING_422:
    return( "NVVIOCOMPONENTSAMPLING_422" );
    break;
  default:
    return( "Unknown" );
    break;
  }
}

std::string ColorSpaceToString( NVVIOCOLORSPACE space )
{
  switch( space )
  {
  case NVVIOCOLORSPACE_YCBCR:
    return( "NVVIOCOLORSPACE_YCBCR" );
    break;
  case NVVIOCOLORSPACE_YCBCRA:
    return( "NVVIOCOLORSPACE_YCBCRA" );
    break;
  case NVVIOCOLORSPACE_YCBCRD:
    return( "NVVIOCOLORSPACE_YCBCRD" );
    break;
  case NVVIOCOLORSPACE_GBR:
    return( "NVVIOCOLORSPACE_GBR" );
    break;
  case NVVIOCOLORSPACE_GBRA:
    return( "NVVIOCOLORSPACE_GBRA" );
    break;
  case NVVIOCOLORSPACE_GBRD:
    return( "NVVIOCOLORSPACE_GBRD" );
    break;
  case NVVIOCOLORSPACE_UNKNOWN:
  default:
    return( "NVVIOCOLORSPACE_UNKNOWN" );
    break;
  }
}

std::string LinkIDToString( NVVIOLINKID id )
{
  switch( id )
  {
  case NVVIOLINKID_A:
    return( "NVVIOLINKID_A" );
    break;
  case NVVIOLINKID_B:
    return( "NVVIOLINKID_B" );
    break;
  case NVVIOLINKID_C:
    return( "NVVIOLINKID_C" );
    break;
  case NVVIOLINKID_D:
    return( "NVVIOLINKID_D" );
    break;
  default:
    return( "Unknown" );
    break;
  }
}

/*
//This function scans current HW topology
int ScanHW(HNVGPUVIO * gpuList, int * PrimaryIdx)
{
  // First we need to create a dummy invisible window, otherwise none of the wgl functions will
  // be available and we can't set up GLAffinity Contexts.  This is 'hacky' but it's the only way
  HWND hWnd;
  HGLRC hGLRC;
  CreateDummyGLWindow(&hWnd,&hGLRC);
  if(!loadExtensionFunctions())
  {
    printf("Could not load OpenGL extensions\n");
    return 0;
  }

  // Query available VIO topologies.
  // Initialize NVAPI
  if (NvAPI_Initialize() != NVAPI_OK) {
    return -1;
  }
  // Fail if there are no VIO topologies or devices available.
  NVVIOTOPOLOGY l_vioTopologies;
  memset(&l_vioTopologies, 0, sizeof(l_vioTopologies));
  l_vioTopologies.version = NVVIOTOPOLOGY_VER;

  if (NvAPI_VIO_QueryTopology(&l_vioTopologies) != NVAPI_OK) {
    return -1;
  }

  if (l_vioTopologies.vioTotalDeviceCount == 0) {
    return -1;
  }

  //if(!loadExtensionFunctions())
  //  return 0;
  printf("\n\nListing GPUs available for OpenGL GPU Affinity\n");
  int numGPUs = 0;
  unsigned int GPUIdx = 0;
  GPU_DEVICE gpuDevice;
  gpuDevice.cb = sizeof(gpuDevice);
  HGPUNV hGPU;
  HNVGPUVIO gpu_vio;
  while(wglEnumGpusNV(GPUIdx,&hGPU))  // First call this function to get a handle to the gpu
  {                     // wglEnumPGUsNV will fails if DeviceIdx > the available devices
        printf("GPU# %d:\n",GPUIdx);
    gpu_vio.hGpu = hGPU;
    gpu_vio.hasSDIOutput = false;
    gpu_vio.vioHandle = NULL;
    gpu_vio.hasDisplay = false;
    gpu_vio.isPrimary = false;
    // Now get the detailed information about this device
        int DisplayDeviceIdx = 0;
    while(wglEnumGpuDevicesNV(hGPU,DisplayDeviceIdx,&gpuDevice))
    {

            printf(" Display# %d\n",DisplayDeviceIdx);
      printf("   Name: %s\n",gpuDevice.DeviceName);
      printf("   String: %s\n",gpuDevice.DeviceString);

      NvDisplayHandle hNvDisplay;
      NvAPI_Status status;
      status = NvAPI_GetAssociatedNvidiaDisplayHandle(gpuDevice.DeviceName,&hNvDisplay);
      if(status != NVAPI_OK)
        break;

      gpu_vio.gpuDevice = gpuDevice;
      gpu_vio.hasDisplay = true;

      NvU32 count = 0;
      NvPhysicalGpuHandle hNvPhysicalGPU;
      status = NvAPI_GetPhysicalGPUsFromDisplay(hNvDisplay,&hNvPhysicalGPU,&count);
      if(status != NVAPI_OK)
        break;

      // Cycle through all SDI topologies looking for the first
      // available SDI output device topology.
      BOOL l_bFound = FALSE;
      unsigned int i = 0;
      NVVIOCAPS l_vioCaps;
      while ((i < l_vioTopologies.vioTotalDeviceCount) && (!l_bFound)) {

        if(l_vioTopologies.vioTarget[i].hPhysicalGpu == hNvPhysicalGPU){
          // Get video I/O capabilities for current video I/O target.
          memset(&l_vioCaps, 0, sizeof(l_vioCaps));
          l_vioCaps.version = NVVIOCAPS_VER;
          if (NvAPI_VIO_GetCapabilities(l_vioTopologies.vioTarget[i].hVioHandle,
                    &l_vioCaps) != NVAPI_OK) {
            continue;
          }
          //MessageBox(NULL, "Video I/O Unsupported.", "Error", MB_OK);
          //return E_FAIL;
          // If video output device found, save VIO handle and set flag.
          if (l_vioCaps.adapterCaps & NVVIOCAPS_VIDOUT_SDI) {
            gpu_vio.hasSDIOutput = true;
            gpu_vio.vioHandle = l_vioTopologies.vioTarget[i].hVioHandle;
            l_bFound = TRUE;
          } else {
            i++;
          }
        }
        else
        {
          i++;

        }
      } // while i < vioTotalDeviceCount

      if(l_bFound)
        printf("   SDI out: yes\n");
      else
      {
        printf("   SDI out: no\n");
      }
      if(gpuDevice.Flags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP)
      {
                printf("   Attached to the desktop: LEFT=%d, RIGHT=%d, TOP=%d, BOTTOM=%d\n",
          gpuDevice.rcVirtualScreen.left, gpuDevice.rcVirtualScreen.right, gpuDevice.rcVirtualScreen.top, gpuDevice.rcVirtualScreen.bottom);
      }
      else
      {
        printf("   Not attached to the desktop\n");
      }

            // See if it's the primary GPU
            if(gpuDevice.Flags & DISPLAY_DEVICE_PRIMARY_DEVICE)
      {
        printf("   This is the PRIMARY Display Device\n");
                *PrimaryIdx = GPUIdx;
        gpu_vio.isPrimary = true;
      }
      DisplayDeviceIdx++;

    }
    gpuList[numGPUs].hasDisplay = gpu_vio.hasDisplay;
    gpuList[numGPUs].hasSDIOutput = gpu_vio.hasSDIOutput;
    gpuList[numGPUs].hGpu = gpu_vio.hGpu;
    gpuList[numGPUs].vioHandle = gpu_vio.vioHandle;
    gpuList[numGPUs].gpuDevice.cb = gpuDevice.cb;
    gpuList[numGPUs].gpuDevice.rcVirtualScreen = gpuDevice.rcVirtualScreen;
    gpuList[numGPUs].gpuDevice.Flags = gpuDevice.Flags;
    memcpy(gpuList[numGPUs].gpuDevice.DeviceName,gpuDevice.DeviceName,32);
    memcpy(gpuList[numGPUs].gpuDevice.DeviceString,gpuDevice.DeviceString,128);

    gpuList[numGPUs] = gpu_vio;

    numGPUs++;
    GPUIdx++;
  }
  // We can kill the dummy window now
  HDC hDC=GetDC(hWnd);
  wglMakeCurrent(NULL,NULL);
  wglDeleteContext(hGLRC);
  ReleaseDC(hWnd,hDC);
  DestroyWindow(hWnd);

   //if(glerr != GL_NO_ERROR)
  //  return 0;

  return(numGPUs);
}
*/