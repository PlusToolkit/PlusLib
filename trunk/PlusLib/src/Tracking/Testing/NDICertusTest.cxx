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

	fprintf( stdout, "Determining system configuration using system.nif.\n" );

	/*
	* Determine the system configuration in the default manner
	* without any error logging. This writes the file system.nif
	* in the standard ndigital directory.
	*/

	/*
	* Set optional processing flags (this overides the settings in Optotrak.INI).
	*/
	fprintf( stdout, "...OptotrakSetProcessingFlags\n" );
	if( OptotrakSetProcessingFlags( OPTO_LIB_POLL_REAL_DATA |
		OPTO_CONVERT_ON_HOST |
		OPTO_RIGID_ON_HOST ) )
	{
		goto ERROR_EXIT;
	} /* if */

	fprintf( stdout, "...TransputerDetermineSystemCfg\n" );
	if( TransputerDetermineSystemCfg( NULL ) )
	{
		fprintf( stderr, "Error in determining the system parameters.\n" );
		goto ERROR_EXIT;
	} /* if */

	vtkAccurateTimer::Delay(1); 

	/*
	* Load the system of processors.
	*/
	fprintf( stdout, "...TransputerLoadSystem\n" );
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
	fprintf( stdout, "...TransputerInitializeSystem\n" );
	if( TransputerInitializeSystem( OPTO_LOG_ERRORS_FLAG | OPTO_LOG_MESSAGES_FLAG ) )
	{
		goto ERROR_EXIT;
	} /* if */

	/*
	* Load the standard camera parameters.
	*/
	fprintf( stdout, "...OptotrakLoadCameraParameters\n" );
	if( OptotrakLoadCameraParameters( "standard" ) )
	{
		goto ERROR_EXIT;
	} /* if */

	/*
	* Set up a collection for the Optotrak.
	*/
	fprintf( stdout, "...OptotrakSetupCollection\n" );
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
	fprintf( stdout, "...OptotrakActivateMarkers\n" );
	if( OptotrakActivateMarkers() )
	{
		goto ERROR_EXIT;
	} /* if */
	vtkAccurateTimer::Delay(1); 

	/*
	* Get and display ten frames of 3D data.
	*/
	fprintf( stdout, "\n\nSample Program Results:\n\n" );
	fprintf( stdout, "\n\n3D Data Display\n" );
	for( uFrameCnt = 0; uFrameCnt < 10; ++uFrameCnt )
	{
		/*
		* Get a frame of data.
		*/
		fprintf( stdout, "\n" );
		if( DataGetLatest3D( &uFrameNumber, &uElements, &uFlags, p3dData ) )
		{
			goto ERROR_EXIT;
		} /* if */

		/*
		* Print out the data.
		*/
		fprintf( stdout, "Frame Number: %8u\n", uFrameNumber );
		fprintf( stdout, "Elements    : %8u\n", uElements );
		fprintf( stdout, "Flags       : 0x%04x\n", uFlags );
		//     for( uMarkerCnt = 0; uMarkerCnt < NUM_MARKERS; ++uMarkerCnt )
		//     {
		//DisplayMarker( uMarkerCnt + 1, p3dData[ uMarkerCnt] );
		//     } /* for */
	} /* for */
	fprintf( stdout, "\n" );

	/*
	* De-activate the markers.
	*/
	fprintf( stdout, "...OptotrakDeActivateMarkers\n" );
	if( OptotrakDeActivateMarkers() )
	{
		goto ERROR_EXIT;
	} /* if */

	/*
	* Stop the collection.
	*/
	fprintf( stdout, "...OptotrakStopCollection\n" );
	OptotrakStopCollection( );

	/*
	* Shutdown the processors message passing system.
	*/
	fprintf( stdout, "...TransputerShutdownSystem\n" );
	if( TransputerShutdownSystem() )
	{
		goto ERROR_EXIT;
	} /* if */

	vtkAccurateTimer::Delay(1); 

	/*
	* Set the API to use internal storage for the system
	* configuration.
	*/
	fprintf( stdout, "\nDetermining system configuration using internal strings.\n" );

	/*
	* Set optional processing flags (this overides the settings in Optotrak.INI).
	*/
	fprintf( stdout, "...OptotrakSetProcessingFlags\n" );
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
	fprintf( stdout, "...TransputerDetermineSystemCfg\n" );
	if( TransputerDetermineSystemCfg( pszLogFileName ) )
	{
		fprintf( stderr, "Error in determining the system parameters.\n" );
		goto ERROR_EXIT;
	}

	vtkAccurateTimer::Delay(1); 

	/*
	* Load the system of processors.
	*/
	fprintf( stdout, "...TransputerLoadSystem\n" );
	if( TransputerLoadSystem( "system" ) != OPTO_NO_ERROR_CODE )
	{
		goto ERROR_EXIT;
	} /* if */

	vtkAccurateTimer::Delay(1); 

	/*
	* Initialize the processors system again in the usual manner.
	*/
	fprintf( stdout, "...TransputerInitializeSystem\n" );
	if( TransputerInitializeSystem( OPTO_LOG_ERRORS_FLAG | OPTO_LOG_MESSAGES_FLAG ) )
	{
		goto ERROR_EXIT;
	} /* if */

	/*
	* Load the standard camera parameters.
	*/
	fprintf( stdout, "...OptotrakLoadCameraParameters\n" );
	if( OptotrakLoadCameraParameters( "standard" ) )
	{
		goto ERROR_EXIT;
	} /* if */

	/*
	* Set up a collection for the Optotrak.
	*/
	fprintf( stdout, "...OptotrakSetupCollection\n" );
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
	fprintf( stdout, "...OptotrakActivateMarkers\n" );
	if( OptotrakActivateMarkers() )
	{
		goto ERROR_EXIT;
	} /* if */

	vtkAccurateTimer::Delay(1); 

	/*
	* Get and display ten frames of 3D data.
	*/
	fprintf( stdout, "\n\nSample Program Results:\n\n" );
	fprintf( stdout, "\n\n3D Data Display\n" );
	for( uFrameCnt = 0; uFrameCnt < 10; ++uFrameCnt )
	{
		/*
		* Get a frame of data.
		*/
		fprintf( stdout, "\n" );
		if( DataGetLatest3D( &uFrameNumber, &uElements, &uFlags, p3dData ) )
		{
			goto ERROR_EXIT;
		} /* if */

		/*
		* Print out the data.
		*/
		fprintf( stdout, "Frame Number: %8u\n", uFrameNumber );
		fprintf( stdout, "Elements    : %8u\n", uElements );
		fprintf( stdout, "Flags       : 0x%04x\n", uFlags );
		//     for( uMarkerCnt = 0; uMarkerCnt < NUM_MARKERS; ++uMarkerCnt )
		//     {
		//DisplayMarker( uMarkerCnt + 1, p3dData[ uMarkerCnt] );
		//     } /* for */
	} /* for */
	fprintf( stdout, "\n" );

	/*
	* De-activate the markers.
	*/
	fprintf( stdout, "...OptotrakDeActivateMarkers\n" );
	if( OptotrakDeActivateMarkers() )
	{
		goto ERROR_EXIT;
	} /* if */

	/*
	* Stop the collection.
	*/
	fprintf( stdout, "...OptotrakStopCollection\n" );
	OptotrakStopCollection( );

	/*
	* Shutdown the processors message passing system.
	*/
	fprintf( stdout, "...TransputerShutdownSystem\n" );
	if( TransputerShutdownSystem() )
	{
		goto ERROR_EXIT;
	} /* if */

	/*
	* Exit the program.
	*/
	fprintf( stdout, "\nProgram execution complete.\n" );
	exit( 0 );

ERROR_EXIT:
	/*
	* Indicate that an error has occurred
	*/
	fprintf( stdout, "\nAn error has occurred during execution of the program.\n" );
	if( OptotrakGetErrorString( szNDErrorString,
		MAX_ERROR_STRING_LENGTH + 1 ) == 0 )
	{
		fprintf( stdout, szNDErrorString );
	} /* if */

	fprintf( stdout, "\n\n...TransputerShutdownSystem\n" );
	TransputerShutdownSystem();

	return  1 ;

}

