/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

/*****************************************************************
Name:             SAMPLE20.C

Description:

Optotrak Sample Program #20.

- Set processing flags to generate system configuration externally.
- Generate the system configuration externally and load the processors code.
- Initiate communications with the Optotrak System.
- Load the appropriate camera parameters.
- Set up an Optotrak collection.
- Activate the markers.
- Request/receive/display 10 frames of real-time 3D data.
- De-activate the markers.
- Stop the Optotrak collection.
- Disconnect from the Optotrak System.
- Set processing flags to generate system configuration internally.
- Generate the system configuration internally and load the processors code.
- Initiate communications with the Optotrak System.
- Load the appropriate camera parameters.
- Set up an Optotrak collection.
- Activate the markers.
- Request/receive/display 10 frames of real-time 3D data.
- De-activate the markers.
- Stop the Optotrak collection.
- Disconnect from the Optotrak System.
- Set processing flags to perform data conversions on the host computer.

*****************************************************************/

/*****************************************************************
C Library Files Included
*****************************************************************/
#include "PlusConfigure.h"
#include <stdio.h>
#include <stdlib.h>

#ifdef _MSC_VER
#include <string.h>
#elif __BORLANDC__
#include <dos.h>
extern unsigned _stklen = 16000;
#elif __WATCOMC__
#include <dos.h>
#endif

#if defined(_WIN32) && !defined(__WINDOWS_H)
#define __WINDOWS_H
#include "Windows.h"
#endif

/*****************************************************************
ND Library Files Included
*****************************************************************/
#include "ndtypes.h"
#include "ndpack.h"
#include "ndopto.h"

/*****************************************************************
Application Files Included
*****************************************************************/
//#include "ot_aux.h"

/*****************************************************************
Defines:
*****************************************************************/
#define NUM_MARKERS 6

/*****************************************************************
Name:               main

Input Values:
int
argc        :Number of command line parameters.
unsigned char
*argv[]     :Pointer array to each parameter.

Output Values:
None.

Return Value:
None.

Description:

Main program routine performs all steps listed in the above
program description.

*****************************************************************/
int main( int argc, unsigned char *argv[] )
{
  char
    pszLogFileName[] = "CfgLog.txt",
    szNDErrorString[MAX_ERROR_STRING_LENGTH + 1];
  unsigned int
    uFrameCnt,
    uFrameNumber,
    uElements,
    uFlags;
  static Position3d
    p3dData[NUM_MARKERS];

  std::cout << "Determining system configuration using system.nif." << std::endl;

  /*
  * Determine the system configuration in the default manner
  * without any error logging. This writes the file system.nif
  * in the standard ndigital directory.
  */

  /*
  * Set optional processing flags (this overides the settings in Optotrak.INI).
  */
  std::cout << "...OptotrakSetProcessingFlags" << std::endl;

  if( OptotrakSetProcessingFlags( OPTO_LIB_POLL_REAL_DATA |
    OPTO_CONVERT_ON_HOST |
    OPTO_RIGID_ON_HOST ) )
  {
    goto ERROR_EXIT;
  } /* if */

  std::cout << "...TransputerDetermineSystemCfg" << std::endl;
  if( TransputerDetermineSystemCfg( NULL ) )
  {
    std::cerr <<  "Error in determining the system parameters." << std::endl;
    goto ERROR_EXIT;
  } /* if */

  vtkAccurateTimer::Delay(1); 

  /*
  * Load the system of processors.
  */
  std::cout << "...TransputerLoadSystem" << std::endl;
  if( TransputerLoadSystem( "system" ) != OPTO_NO_ERROR_CODE )
  {
    goto ERROR_EXIT;
  } /* if */

  /*
  * Wait one second to let the system finish loading.
  */
  vtkAccurateTimer::Delay(1); 

  /*
  * Initialize the processors system.
  */
  std::cout << "...TransputerInitializeSystem" << std::endl;
  if( TransputerInitializeSystem( OPTO_LOG_ERRORS_FLAG | OPTO_LOG_MESSAGES_FLAG ) )
  {
    goto ERROR_EXIT;
  } /* if */

  /*
  * Load the standard camera parameters.
  */
  std::cout << "...OptotrakLoadCameraParameters" << std::endl;
  if( OptotrakLoadCameraParameters( "standard" ) )
  {
    goto ERROR_EXIT;
  } /* if */

  /*
  * Set up a collection for the Optotrak.
  */
  std::cout << "...OptotrakSetupCollection" << std::endl;
  if( OptotrakSetupCollection(
    NUM_MARKERS,    /* Number of markers in the collection. */
    (float)100.0,   /* Frequency to collect data frames at. */
    (float)2500.0,  /* Marker frequency for marker maximum on-time. */
    30,             /* Dynamic or Static Threshold value to use. */
    160,            /* Minimum gain code amplification to use. */
    0,              /* Stream mode for the data buffers. */
    (float)0.35,    /* Marker Duty Cycle to use. */
    (float)7.0,     /* Voltage to use when turning on markers. */
    (float)1.0,     /* Number of seconds of data to collect. */
    (float)0.0,     /* Number of seconds to pre-trigger data by. */
    OPTOTRAK_NO_FIRE_MARKERS_FLAG | OPTOTRAK_BUFFER_RAW_FLAG | OPTOTRAK_GET_NEXT_FRAME_FLAG ) )
  {
    goto ERROR_EXIT;
  } /* if */

  /*
  * Wait one second to let the camera adjust.
  */
  vtkAccurateTimer::Delay(1); 

  /*
  * Activate the markers.
  */
  std::cout << "...OptotrakActivateMarkers" << std::endl;
  if( OptotrakActivateMarkers() )
  {
    goto ERROR_EXIT;
  } /* if */
  vtkAccurateTimer::Delay(1); 

  /*
  * Get and display ten frames of 3D data.
  */
  std::cout << std::endl << std::endl << "Sample Program Results:" << std::endl << std::endl;
  std::cout << std::endl << std::endl << "3D Data Display" << std::endl;
  for( uFrameCnt = 0; uFrameCnt < 10; ++uFrameCnt )
  {
    /*
    * Get a frame of data.
    */
    std::cout << std::endl;
    if( DataGetLatest3D( &uFrameNumber, &uElements, &uFlags, p3dData ) )
    {
      goto ERROR_EXIT;
    } /* if */

    /*
    * Print out the data.
    */
    std::cout << "Frame Number: " << uFrameNumber << std::endl;
    std::cout << "Elements    : " << uElements << std::endl;
    std::cout << "Flags       : " << std::hex << uFlags << std::endl;
    //     for( uMarkerCnt = 0; uMarkerCnt < NUM_MARKERS; ++uMarkerCnt )
    //     {
    //DisplayMarker( uMarkerCnt + 1, p3dData[ uMarkerCnt] );
    //     } /* for */
  } /* for */
  std::cout << std::endl;

  /*
  * De-activate the markers.
  */
  std::cout << "...OptotrakDeActivateMarkers" << std::endl;
  if( OptotrakDeActivateMarkers() )
  {
    goto ERROR_EXIT;
  } /* if */

  /*
  * Stop the collection.
  */
  std::cout << "...OptotrakStopCollection" << std::endl;
  OptotrakStopCollection( );

  /*
  * Shutdown the processors message passing system.
  */
  std::cout << "...TransputerShutdownSystem" << std::endl;
  if( TransputerShutdownSystem() )
  {
    goto ERROR_EXIT;
  } /* if */

  vtkAccurateTimer::Delay(1); 

  /*
  * Set the API to use internal storage for the system
  * configuration.
  */
  std::cout << std::endl << "Determining system configuration using internal strings." << std::endl;

  /*
  * Set optional processing flags (this overides the settings in Optotrak.INI).
  */
  std::cout << "...OptotrakSetProcessingFlags" << std::endl;
  if( OptotrakSetProcessingFlags( OPTO_USE_INTERNAL_NIF |
    OPTO_LIB_POLL_REAL_DATA |
    OPTO_CONVERT_ON_HOST |
    OPTO_RIGID_ON_HOST ) )
  {
    goto ERROR_EXIT;
  } /* if */


  /*
  * Determine the system configuration again, but this time
  * with error logging.
  */
  std::cout << "...TransputerDetermineSystemCfg" << std::endl;
  if( TransputerDetermineSystemCfg( pszLogFileName ) )
  {
    std::cerr << "Error in determining the system parameters." << std::endl;
    goto ERROR_EXIT;
  }

  vtkAccurateTimer::Delay(1); 

  /*
  * Load the system of processors.
  */
  std::cout << "...TransputerLoadSystem" << std::endl;
  if( TransputerLoadSystem( "system" ) != OPTO_NO_ERROR_CODE )
  {
    goto ERROR_EXIT;
  } /* if */

  vtkAccurateTimer::Delay(1); 

  /*
  * Initialize the processors system again in the usual manner.
  */
  std::cout << "...TransputerInitializeSystem" << std::endl;
  if( TransputerInitializeSystem( OPTO_LOG_ERRORS_FLAG | OPTO_LOG_MESSAGES_FLAG ) )
  {
    goto ERROR_EXIT;
  } /* if */

  /*
  * Load the standard camera parameters.
  */
  std::cout << "...OptotrakLoadCameraParameters" << std::endl;
  if( OptotrakLoadCameraParameters( "standard" ) )
  {
    goto ERROR_EXIT;
  } /* if */

  /*
  * Set up a collection for the Optotrak.
  */
  std::cout << "...OptotrakSetupCollection" << std::endl;
  if( OptotrakSetupCollection(
    NUM_MARKERS,    /* Number of markers in the collection. */
    (float)100.0,   /* Frequency to collect data frames at. */
    (float)2500.0,  /* Marker frequency for marker maximum on-time. */
    30,             /* Dynamic or Static Threshold value to use. */
    160,            /* Minimum gain code amplification to use. */
    0,              /* Stream mode for the data buffers. */
    (float)0.35,    /* Marker Duty Cycle to use. */
    (float)7.0,     /* Voltage to use when turning on markers. */
    (float)1.0,     /* Number of seconds of data to collect. */
    (float)0.0,     /* Number of seconds to pre-trigger data by. */
    OPTOTRAK_NO_FIRE_MARKERS_FLAG | OPTOTRAK_BUFFER_RAW_FLAG | OPTOTRAK_GET_NEXT_FRAME_FLAG ) )
  {
    goto ERROR_EXIT;
  } /* if */

  /*
  * Wait one second to let the camera adjust.
  */
  vtkAccurateTimer::Delay(1); 

  /*
  * Activate the markers.
  */
  std::cout << "...OptotrakActivateMarkers" << std::endl;
  if( OptotrakActivateMarkers() )
  {
    goto ERROR_EXIT;
  } /* if */

  vtkAccurateTimer::Delay(1); 

  /*
  * Get and display ten frames of 3D data.
  */
  std::cout << std::endl << std::endl << "Sample Program Results:" << std::endl << std::endl;
  std::cout << std::endl << std::endl << "3D Data Display" << std::endl;
  for( uFrameCnt = 0; uFrameCnt < 10; ++uFrameCnt )
  {
    /*
    * Get a frame of data.
    */
    std::cout << std::endl;
    if( DataGetLatest3D( &uFrameNumber, &uElements, &uFlags, p3dData ) )
    {
      goto ERROR_EXIT;
    } /* if */

    /*
    * Print out the data.
    */
    std::cout << "Frame Number: " << uFrameNumber << std::endl;
    std::cout << "Elements    : " << uElements << std::endl;
    std::cout << "Flags       : " << std::hex << uFlags << std::endl;
  }
  std::cout << std::endl;

  /*
  * De-activate the markers.
  */
  std::cout << "...OptotrakDeActivateMarkers" << std::endl;
  if( OptotrakDeActivateMarkers() )
  {
    goto ERROR_EXIT;
  } /* if */

  /*
  * Stop the collection.
  */
  std::cout << "...OptotrakStopCollection" << std::endl;
  OptotrakStopCollection( );

  /*
  * Shutdown the processors message passing system.
  */
  std::cout << "...TransputerShutdownSystem" << std::endl;
  if( TransputerShutdownSystem() )
  {
    goto ERROR_EXIT;
  } /* if */

  /*
  * Exit the program.
  */
  std::cout << std::endl << "Program execution complete." << std::endl;
  exit( 0 );

ERROR_EXIT:
  /*
  * Indicate that an error has occurred
  */
  std::cout << std::endl << "An error has occurred during execution of the program." << std::endl;
  if( OptotrakGetErrorString( szNDErrorString,
    MAX_ERROR_STRING_LENGTH + 1 ) == 0 )
  {
    std::cout << szNDErrorString;
  } /* if */

  std::cout << std::endl << std::endl << "...TransputerShutdownSystem" << std::endl;
  TransputerShutdownSystem();

  return  1 ;

}

