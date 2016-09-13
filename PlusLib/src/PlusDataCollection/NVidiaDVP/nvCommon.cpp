#include "PlusCommon.h"
#include "nvCommon.h"
#include <string.h>

// Get next string from command line
static void getNextWord( char** szCmdLine, char* buffer )
{
  while ( isspace( **szCmdLine ) )
  {
    ++( *szCmdLine );
  }

  while ( !isspace( **szCmdLine ) && ( **szCmdLine ) ) { *( buffer++ ) = *( ( *szCmdLine )++ ); }
  *buffer = 0;
}

// Parse command line.
bool ParseCommandLine( char* szCmdLine[], nvOptions* op )
{
  char szBuffer[100];

  // Parse command line arguments
  while ( *szCmdLine )
  {
    // Grab next token.
    getNextWord( szCmdLine, szBuffer );

    // Break at end of command line.
    if ( strlen( szBuffer ) == 0 )
    {
      break;

      // Set framelock flag.
    }
    else if ( !_stricmp( "-fl", szBuffer ) )
    {
      op->frameLock = true;

      // Set blocking flag.
    }
    else if ( !_stricmp( "-b", szBuffer ) )
    {
      op->block = true;

      // Set FSAA flag.
    }
    else if ( !_stricmp( "-fsaa", szBuffer ) )
    {
      getNextWord( szCmdLine, szBuffer );
      op->fsaa = atoi( szBuffer );

      // Set frame/field enum.
    }
    else if ( !_stricmp( "-fd", szBuffer ) )
    {
      op->field = true;

      // Colorspace conversion scale vector
    }
    else if ( !_stricmp( "-cscScale", szBuffer ) )
    {
      getNextWord( szCmdLine, szBuffer );
      op->cscScale[0] = atof( szBuffer );
      getNextWord( szCmdLine, szBuffer );
      op->cscScale[1] = atof( szBuffer );
      getNextWord( szCmdLine, szBuffer );
      op->cscScale[2] = atof( szBuffer );
      op->cscEnable = true;

      // Colorspace conversion offset vector
    }
    else if ( !_stricmp( "-cscOffset", szBuffer ) )
    {
      getNextWord( szCmdLine, szBuffer );
      op->cscOffset[0] = atof( szBuffer );
      getNextWord( szCmdLine, szBuffer );
      op->cscOffset[1] = atof( szBuffer );
      getNextWord( szCmdLine, szBuffer );
      op->cscOffset[2] = atof( szBuffer );
      op->cscEnable = true;

      // Colorspace conversion matrix
    }
    else if ( !_stricmp( "-cscMatrix", szBuffer ) )
    {
      getNextWord( szCmdLine, szBuffer );
      op->cscMatrix[0][0] = atof( szBuffer );
      getNextWord( szCmdLine, szBuffer );
      op->cscMatrix[0][1] = atof( szBuffer );
      getNextWord( szCmdLine, szBuffer );
      op->cscMatrix[0][2] = atof( szBuffer );
      getNextWord( szCmdLine, szBuffer );
      op->cscMatrix[1][0] = atof( szBuffer );
      getNextWord( szCmdLine, szBuffer );
      op->cscMatrix[1][1] = atof( szBuffer );
      getNextWord( szCmdLine, szBuffer );
      op->cscMatrix[1][2] = atof( szBuffer );
      getNextWord( szCmdLine, szBuffer );
      op->cscMatrix[2][0] = atof( szBuffer );
      getNextWord( szCmdLine, szBuffer );
      op->cscMatrix[2][1] = atof( szBuffer );
      getNextWord( szCmdLine, szBuffer );
      op->cscMatrix[2][2] = atof( szBuffer );
      op->cscEnable = true;

      // Gamma correction vector values.
    }
    else if ( !_stricmp( "-gamma", szBuffer ) )
    {
      getNextWord( szCmdLine, szBuffer );
      op->gamma[0] = atof( szBuffer );
      getNextWord( szCmdLine, szBuffer );
      op->gamma[1] = atof( szBuffer );
      getNextWord( szCmdLine, szBuffer );
      op->gamma[2] = atof( szBuffer );
      if ( ( op->gamma[0] < 0.5 ) || ( op->gamma[0] > 6.0 ) ||
           ( op->gamma[1] < 0.5 ) || ( op->gamma[1] > 6.0 ) ||
           ( op->gamma[2] < 0.5 ) || ( op->gamma[2] > 6.0 ) )
      {
        LOG_ERROR( "Illegal gamma values specified." );
        return false;
      }

      // Flip queue length
    }
    else if ( !_stricmp( "-fql", szBuffer ) )
    {
      getNextWord( szCmdLine, szBuffer );
      op->flipQueueLength = atoi( szBuffer );
      if ( ( op->flipQueueLength < 2 ) || ( op->flipQueueLength > 7 ) )
      {
        LOG_ERROR( "Illegal number of flip queue buffers specified." );
        return false;
      }

      // GPU
    }
    else if ( !_stricmp( "-gpu", szBuffer ) )
    {
      getNextWord( szCmdLine, szBuffer );
      op->gpu = atoi( szBuffer );
      if ( op->gpu > 2 )
      {
        LOG_ERROR( "Illegal GPU specified." );
        return false;
      }


      // Get video format - default is 1080i.
    }
    else if ( !_stricmp( "-vf", szBuffer ) )
    {
      getNextWord( szCmdLine, szBuffer );
      if ( !_stricmp( "487i5994_259", szBuffer ) )
      {
        op->videoFormat = NVVIOSIGNALFORMAT_487I_59_94_SMPTE259_NTSC;
      }
      else if ( !_stricmp( "576i5000_259", szBuffer ) )
      {
        op->videoFormat = NVVIOSIGNALFORMAT_576I_50_00_SMPTE259_PAL;
      }
      else if ( !_stricmp( "720p2398_296", szBuffer ) )
      {
        op->videoFormat = NVVIOSIGNALFORMAT_720P_23_98_SMPTE296;
      }
      else if ( !_stricmp( "720p2400_296", szBuffer ) )
      {
        op->videoFormat = NVVIOSIGNALFORMAT_720P_24_00_SMPTE296;
      }
      else if ( !_stricmp( "720p2500_296", szBuffer ) )
      {
        op->videoFormat = NVVIOSIGNALFORMAT_720P_25_00_SMPTE296;
      }
      else if ( !_stricmp( "720p2997_296", szBuffer ) )
      {
        op->videoFormat = NVVIOSIGNALFORMAT_720P_29_97_SMPTE296;
      }
      else if ( !_stricmp( "720p3000_296", szBuffer ) )
      {
        op->videoFormat = NVVIOSIGNALFORMAT_720P_30_00_SMPTE296;
      }
      else if ( !_stricmp( "720p5000_296", szBuffer ) )
      {
        op->videoFormat = NVVIOSIGNALFORMAT_720P_50_00_SMPTE296;
      }
      else if ( !_stricmp( "720p5994_296", szBuffer ) )
      {
        op->videoFormat = NVVIOSIGNALFORMAT_720P_59_94_SMPTE296;
      }
      else if ( !_stricmp( "720p6000_296", szBuffer ) )
      {
        op->videoFormat = NVVIOSIGNALFORMAT_720P_60_00_SMPTE296;
      }
      else if ( !_stricmp( "1035i5994_260", szBuffer ) )
      {
        op->videoFormat = NVVIOSIGNALFORMAT_1035I_59_94_SMPTE260;
      }
      else if ( !_stricmp( "1035i6000_260", szBuffer ) )
      {
        op->videoFormat = NVVIOSIGNALFORMAT_1035I_60_00_SMPTE260;

      }
      else if ( !_stricmp( "1080i4796_274", szBuffer ) )
      {
        op->videoFormat = NVVIOSIGNALFORMAT_1080I_47_96_SMPTE274;
      }
      else if ( !_stricmp( "1080i4800_274", szBuffer ) )
      {
        op->videoFormat = NVVIOSIGNALFORMAT_1080I_48_00_SMPTE274;
      }
      else if ( !_stricmp( "1080i5000_295", szBuffer ) )
      {
        op->videoFormat = NVVIOSIGNALFORMAT_1080I_50_00_SMPTE295;
      }
      else if ( !_stricmp( "1080i5000_274", szBuffer ) )
      {
        op->videoFormat = NVVIOSIGNALFORMAT_1080I_50_00_SMPTE274;
      }
      else if ( !_stricmp( "1080i5994_274", szBuffer ) )
      {
        op->videoFormat = NVVIOSIGNALFORMAT_1080I_59_94_SMPTE274;
      }
      else if ( !_stricmp( "1080i6000_274", szBuffer ) )
      {
        op->videoFormat = NVVIOSIGNALFORMAT_1080I_60_00_SMPTE274;
      }
      else if ( !_stricmp( "1080psf23976_274", szBuffer ) )
      {
        op->videoFormat = NVVIOSIGNALFORMAT_1080PSF_23_98_SMPTE274;
      }
      else if ( !_stricmp( "1080psf2398_274", szBuffer ) )
      {
        op->videoFormat = NVVIOSIGNALFORMAT_1080PSF_23_98_SMPTE274;
      }
      else if ( !_stricmp( "1080psf2400_274", szBuffer ) )
      {
        op->videoFormat = NVVIOSIGNALFORMAT_1080PSF_24_00_SMPTE274;
      }
      else if ( !_stricmp( "1080psf2500_274", szBuffer ) )
      {
        op->videoFormat = NVVIOSIGNALFORMAT_1080PSF_25_00_SMPTE274;
      }
      else if ( !_stricmp( "1080psf2997_274", szBuffer ) )
      {
        op->videoFormat = NVVIOSIGNALFORMAT_1080PSF_29_97_SMPTE274;
      }
      else if ( !_stricmp( "1080psf3000_274", szBuffer ) )
      {
        op->videoFormat = NVVIOSIGNALFORMAT_1080PSF_30_00_SMPTE274;
      }
      else if ( !_stricmp( "1080p23976_274", szBuffer ) )
      {
        op->videoFormat = NVVIOSIGNALFORMAT_1080P_23_976_SMPTE274;
      }
      else if ( !_stricmp( "1080p2400_274", szBuffer ) )
      {
        op->videoFormat = NVVIOSIGNALFORMAT_1080P_24_00_SMPTE274;
      }
      else if ( !_stricmp( "1080p2500_274", szBuffer ) )
      {
        op->videoFormat = NVVIOSIGNALFORMAT_1080P_25_00_SMPTE274;
      }
      else if ( !_stricmp( "1080p2997_274", szBuffer ) )
      {
        op->videoFormat = NVVIOSIGNALFORMAT_1080P_29_97_SMPTE274;
      }
      else if ( !_stricmp( "1080p3000_274", szBuffer ) )
      {
        op->videoFormat = NVVIOSIGNALFORMAT_1080P_30_00_SMPTE274;
      }
      else if ( !_stricmp( "2048i4796_372", szBuffer ) )
      {
        op->videoFormat = NVVIOSIGNALFORMAT_2048I_47_96_SMPTE372;
      }
      else if ( !_stricmp( "2048i4800_372", szBuffer ) )
      {
        op->videoFormat = NVVIOSIGNALFORMAT_2048I_48_00_SMPTE372;
      }
      else if ( !_stricmp( "2048i5000_372", szBuffer ) )
      {
        op->videoFormat = NVVIOSIGNALFORMAT_2048I_50_00_SMPTE372;
      }
      else if ( !_stricmp( "2048i5994_372", szBuffer ) )
      {
        op->videoFormat = NVVIOSIGNALFORMAT_2048I_59_94_SMPTE372;
      }
      else if ( !_stricmp( "2048i6000_372", szBuffer ) )
      {
        op->videoFormat = NVVIOSIGNALFORMAT_2048I_60_00_SMPTE372;
      }
      else if ( !_stricmp( "2048p2398_372", szBuffer ) )
      {
        op->videoFormat = NVVIOSIGNALFORMAT_2048P_23_98_SMPTE372;
      }
      else if ( !_stricmp( "2048p2400_372", szBuffer ) )
      {
        op->videoFormat = NVVIOSIGNALFORMAT_2048P_24_00_SMPTE372;
      }
      else if ( !_stricmp( "2048p2500_372", szBuffer ) )
      {
        op->videoFormat = NVVIOSIGNALFORMAT_2048P_25_00_SMPTE372;
      }
      else if ( !_stricmp( "2048p2997_372", szBuffer ) )
      {
        op->videoFormat = NVVIOSIGNALFORMAT_2048P_29_97_SMPTE372;
      }
      else if ( !_stricmp( "2048p3000_372", szBuffer ) )
      {
        op->videoFormat = NVVIOSIGNALFORMAT_2048P_30_00_SMPTE372;
      }
      else
      {
        op->videoFormat = NVVIOSIGNALFORMAT_NONE;
      }

      // Get data format - default is RGBA4444
    }
    else if ( !_stricmp( "-df", szBuffer ) )
    {
      getNextWord( szCmdLine, szBuffer );
      if ( !_stricmp( "r8g8b8_to_ycrcb444", szBuffer ) )
      {
        op->dataFormat = NVVIODATAFORMAT_R8G8B8_TO_YCRCB444;
      }
      else if ( !_stricmp( "r8g8b8a8_to_ycrcba4444", szBuffer ) )
      {
        op->dataFormat =  NVVIODATAFORMAT_R8G8B8A8_TO_YCRCBA4444;
      }
      else if ( !_stricmp( "r8g8b8_to_ycrcb422", szBuffer ) )
      {
        op->dataFormat = NVVIODATAFORMAT_R8G8B8_TO_YCRCB422;
      }
      else if ( !_stricmp( "r8g8b8a8_to_ycrcb4224", szBuffer ) )
      {
        op->dataFormat = NVVIODATAFORMAT_R8G8B8A8_TO_YCRCBA4224;
      }
      else if ( !_stricmp( "r8g8b8_to_rgb444", szBuffer ) )
      {
        op->dataFormat =  NVVIODATAFORMAT_X8X8X8_444_PASSTHRU ;
      }
      else if ( !_stricmp( "r8g8b8a8_to_rgb4444", szBuffer ) )
      {
        op->dataFormat = NVVIODATAFORMAT_X8X8X8_444_PASSTHRU;
      }
      else if ( !_stricmp( "y10cr10cb10_to_ycrcb444", szBuffer ) )
      {
        op->dataFormat = NVVIODATAFORMAT_X10X10X10_444_PASSTHRU;
      }
      else if ( !_stricmp( "y10cr8cb8_to_ycrcb444", szBuffer ) )
      {
        op->dataFormat = NVVIODATAFORMAT_X10X10X10_444_PASSTHRU;
      }
      else if ( !_stricmp( "y10cr8cb8a10_to_ycrcb4444", szBuffer ) )
      {
        op->dataFormat = NVVIODATAFORMAT_X10X8X8A10_4444_PASSTHRU;
      }
      else if ( !_stricmp( "dual_r8g8b8_to_dual_ycrcb422", szBuffer ) )
      {
        op->dataFormat = NVVIODATAFORMAT_DUAL_R8G8B8_TO_DUAL_YCRCB422;
      }
      else if ( !_stricmp( "dual_y8cr8cb8_to_dual_ycrcb422", szBuffer ) )
      {
        op->dataFormat = NVVIODATAFORMAT_DUAL_X8X8X8_TO_DUAL_422_PASSTHRU;
      }
      else if ( !_stricmp( "r10g10b10_to_ycrcb422", szBuffer ) )
      {
        op->dataFormat = NVVIODATAFORMAT_R10G10B10_TO_YCRCB422;
      }
      else if ( !_stricmp( "r10g10b10_to_ycrcb444", szBuffer ) )
      {
        op->dataFormat = NVVIODATAFORMAT_R10G10B10_TO_YCRCB422;
      }
      else if ( !_stricmp( "y12cr12cb12_to_ycrcb444", szBuffer ) )
      {
        op->dataFormat = NVVIODATAFORMAT_X12X12X12_444_PASSTHRU;
      }
      else if ( !_stricmp( "y12cr12cb12_to_ycrcb422", szBuffer ) )
      {
        op->dataFormat =  NVVIODATAFORMAT_X12X12X12_422_PASSTHRU;
      }
      else if ( !_stricmp( "y10cr10cb10_to_ycrcb422", szBuffer ) )
      {
        op->dataFormat = NVVIODATAFORMAT_Y10CR10CB10_TO_YCRCB422;
      }
      else if ( !_stricmp( "y8cr8cb8_to_ycrcb422", szBuffer ) )
      {
        op->dataFormat = NVVIODATAFORMAT_Y8CR8CB8_TO_YCRCB422;
      }
      else if ( !_stricmp( "y10cr8cb8a10_to_ycrcba4224", szBuffer ) )
      {
        op->dataFormat = NVVIODATAFORMAT_Y10CR8CB8A10_TO_YCRCBA4224;
      }
      else if ( !_stricmp( "r10g10b10_to_rgb444", szBuffer ) )
      {
        op->dataFormat = NVVIODATAFORMAT_R10G10B10_TO_RGB444;
      }
      else if ( !_stricmp( "r12g12b12_to_rgb444", szBuffer ) )
      {
        op->dataFormat = NVVIODATAFORMAT_X12X12X12_444_PASSTHRU;
      }
      else
      {
        op->dataFormat = NVVIODATAFORMAT_R8G8B8A8_TO_YCRCBA4224;
      }

      // Get sync source - default is none
    }
    else if ( !_stricmp( "-ss", szBuffer ) )
    {
      getNextWord( szCmdLine, szBuffer );
      if ( !_stricmp( "bi", szBuffer ) )
      {
        op->syncType = NVVIOCOMPSYNCTYPE_BILEVEL;
        op->syncSource = NVVIOSYNCSOURCE_COMPSYNC;
        op->syncEnable = TRUE;
      }
      else if ( !_stricmp( "tri", szBuffer ) )
      {
        op->syncType = NVVIOCOMPSYNCTYPE_TRILEVEL;
        op->syncSource = NVVIOSYNCSOURCE_COMPSYNC;
        op->syncEnable = TRUE;
      }
      else if ( !_stricmp( "auto", szBuffer ) )
      {
        op->syncType = NVVIOCOMPSYNCTYPE_AUTO;
        op->syncSource = NVVIOSYNCSOURCE_COMPSYNC;
        op->syncEnable = TRUE;
      }
      else if ( !_stricmp( "sdi", szBuffer ) )
      {
        op->syncSource = NVVIOSYNCSOURCE_SDISYNC;
        op->syncEnable = TRUE;
      }
      else
      {
        op->syncEnable = FALSE;
      }

      // Get compositing types and parameters
    }
    else if ( !_stricmp( "-alpha", szBuffer ) )
    {
      op->alphaComp = TRUE;
    }
    else if ( !_stricmp( "-cr", szBuffer ) )
    {
      op->crComp = TRUE;
      getNextWord( szCmdLine, szBuffer );
      op->crCompRange[0] = atoi( szBuffer );
      getNextWord( szCmdLine, szBuffer );
      op->crCompRange[1] = atoi( szBuffer );
      getNextWord( szCmdLine, szBuffer );
      op->crCompRange[2] = atoi( szBuffer );
      getNextWord( szCmdLine, szBuffer );
      op->crCompRange[3] = atoi( szBuffer );
    }
    else if ( !_stricmp( "-cb", szBuffer ) )
    {
      op->cbComp = TRUE;
      getNextWord( szCmdLine, szBuffer );
      op->cbCompRange[0] = atoi( szBuffer );
      getNextWord( szCmdLine, szBuffer );
      op->cbCompRange[1] = atoi( szBuffer );
      getNextWord( szCmdLine, szBuffer );
      op->cbCompRange[2] = atoi( szBuffer );
      getNextWord( szCmdLine, szBuffer );
      op->cbCompRange[3] = atoi( szBuffer );
    }
    else if ( !_stricmp( "-y", szBuffer ) )
    {
      op->yComp = TRUE;
      getNextWord( szCmdLine, szBuffer );
      op->yCompRange[0] = atoi( szBuffer );
      getNextWord( szCmdLine, szBuffer );
      op->yCompRange[1] = atoi( szBuffer );
      getNextWord( szCmdLine, szBuffer );
      op->yCompRange[2] = atoi( szBuffer );
      getNextWord( szCmdLine, szBuffer );
      op->yCompRange[3] = atoi( szBuffer );

      // Get test pattern - default is colorbars.
    }
    else if ( !_stricmp( "-tp", szBuffer ) )
    {
      getNextWord( szCmdLine, szBuffer );
      if ( !_stricmp( "colorbar", szBuffer ) )
      {
        op->testPattern = TEST_PATTERN_RGB_COLORBARS_100;
      }
      else if ( !_stricmp( "colorbars8_75", szBuffer ) )
      {
        op->testPattern = TEST_PATTERN_COLORBARS8_75;
      }
      else if ( !_stricmp( "colorbars8_100", szBuffer ) )
      {
        op->testPattern = TEST_PATTERN_COLORBARS8_100;
      }
      else if ( !_stricmp( "colorbars10_75", szBuffer ) )
      {
        op->testPattern = TEST_PATTERN_COLORBARS10_75;
      }
      else if ( !_stricmp( "colorbars10_100", szBuffer ) )
      {
        op->testPattern = TEST_PATTERN_COLORBARS10_100;
      }
      else if ( !_stricmp( "rgb_bars_100", szBuffer ) )
      {
        op->testPattern = TEST_PATTERN_RGB_COLORBARS_100;
      }
      else if ( !_stricmp( "rgb_bars_75", szBuffer ) )
      {
        op->testPattern = TEST_PATTERN_RGB_COLORBARS_75;
      }
      else if ( !_stricmp( "ycrcb_bars", szBuffer ) )
      {
        op->testPattern = TEST_PATTERN_YCRCB_COLORBARS;
      }
      else if ( !_stricmp( "frame", szBuffer ) )
      {
        op->testPattern = TEST_PATTERN_FRAME;
      }
      else if ( !_stricmp( "logo", szBuffer ) )
      {
        op->testPattern = TEST_PATTERN_LOGO;
      }
      else if ( !_stricmp( "sphere", szBuffer ) )
      {
        op->testPattern = TEST_PATTERN_SPHERE;
      }
      else if ( !_stricmp( "ramp8", szBuffer ) )
      {
        op->testPattern = TEST_PATTERN_RAMP8;
      }
      else if ( !_stricmp( "ramp16", szBuffer ) )
      {
        op->testPattern = TEST_PATTERN_RAMP16;
      }
      else if ( !_stricmp( "bar", szBuffer ) )
      {
        op->testPattern = TEST_PATTERN_BAR;
      }
      else if ( !_stricmp( "lines", szBuffer ) )
      {
        op->testPattern = TEST_PATTERN_LINES;
      }
      else
      {
        op->testPattern = TEST_PATTERN_RGB_COLORBARS_100;
      }

    }
    else if ( !_stricmp( "-n", szBuffer ) )
    {
      getNextWord( szCmdLine, szBuffer );
      op->numFrames = atoi( szBuffer );

    }
    else if ( !_stricmp( "-i", szBuffer ) )
    {
      getNextWord( szCmdLine, szBuffer );
      strncpy( op->filename, szBuffer, strlen( szBuffer ) );

    }
    else if ( !_stricmp( "-r", szBuffer ) )
    {
      getNextWord( szCmdLine, szBuffer );
      op->repeat = atoi( szBuffer );

    }
    else if ( !_stricmp( "-c", szBuffer ) )
    {
      op->console = TRUE;

    }
    else if ( !_stricmp( "-l", szBuffer ) )
    {
      op->log = TRUE;

    }
    else if ( !_stricmp( "-fps", szBuffer ) )
    {
      op->fps = TRUE;

    }
    else if ( !_stricmp( "-v", szBuffer ) )
    {
      op->videoInfo = TRUE;

      // Horizontal and vertical sync delay
    }
    else if ( !_stricmp( "-hd", szBuffer ) )
    {
      getNextWord( szCmdLine, szBuffer );
      op->hDelay = atoi( szBuffer );

    }
    else if ( !_stricmp( "-vd", szBuffer ) )
    {
      getNextWord( szCmdLine, szBuffer );
      op->vDelay = atoi( szBuffer );

      // Output region for desktop scanout (x, y, w, h)
    }
    else if ( !_stricmp( "-ox", szBuffer ) )
    {
      getNextWord( szCmdLine, szBuffer );
      op->x = atoi( szBuffer );

    }
    else if ( !_stricmp( "-oy", szBuffer ) )
    {
      getNextWord( szCmdLine, szBuffer );
      op->y = atoi( szBuffer );

    }
    else if ( !_stricmp( "-width", szBuffer ) )
    {
      getNextWord( szCmdLine, szBuffer );
      op->width = atoi( szBuffer );

    }
    else if ( !_stricmp( "-height", szBuffer ) )
    {
      getNextWord( szCmdLine, szBuffer );
      op->height = atoi( szBuffer );

      // PCM audio filename
    }
    else if ( !_stricmp( "-audiofile", szBuffer ) )
    {
      getNextWord( szCmdLine, szBuffer );
      strncpy( op->audioFile, szBuffer, strlen( szBuffer ) );

      // Number of audio channels
    }
    else if ( !_stricmp( "-audiochannels", szBuffer ) )
    {
      getNextWord( szCmdLine, szBuffer );
      op->audioChannels = atoi( szBuffer );

      // Number of bits per audio sample
    }
    else if ( !_stricmp( "-audiobits", szBuffer ) )
    {
      getNextWord( szCmdLine, szBuffer );
      op->audioBits = atoi( szBuffer );

      // Set full screen flag
    }
    else if ( !_stricmp( "-fullscreen", szBuffer ) )
    {
      op->fullScreen = TRUE;

      //Capture settings//

    }
    else if ( !_stricmp( "-captureGPU", szBuffer ) )
    {
      getNextWord( szCmdLine, szBuffer );
      op->captureGPU = atoi( szBuffer );
    }
    else if ( !_stricmp( "-captureDevice", szBuffer ) )
    {
      getNextWord( szCmdLine, szBuffer );
      op->captureDevice = atoi( szBuffer );
      //}else if (!_stricmp("-dualLink", szBuffer)) {
      //  op->dualLink = true;
    }
    else if ( !_stricmp( "-sampling", szBuffer ) )
    {
      getNextWord( szCmdLine, szBuffer );
      if ( !_stricmp( "422", szBuffer ) )
      { op->sampling = NVVIOCOMPONENTSAMPLING_422; }
      else if( !_stricmp( "444", szBuffer ) )
      { op->sampling = NVVIOCOMPONENTSAMPLING_444; }
      else if( !_stricmp( "4224", szBuffer ) )
      { op->sampling = NVVIOCOMPONENTSAMPLING_4224; }
      else if( !_stricmp( "4444", szBuffer ) )
      { op->sampling = NVVIOCOMPONENTSAMPLING_4444; }
    }
    else if ( !_stricmp( "-bitsPerComponent", szBuffer ) )
    {
      getNextWord( szCmdLine, szBuffer );
      op->bitsPerComponent = atoi( szBuffer );
    }
    else
    {
      return E_FAIL;
    }
  }
  return S_OK;
}