//*****************************************************************************//
//*****************************************************************************//
//
//	Source: api\atc3dgm
//
//	Author: crobertson
//
//	Revision for ATC3DGm 33.0.14.8
//				
//
//	Date: 02/10/2010
//
//	COPYRIGHT:		COPYRIGHT ASCENSION TECHNOLOGY CORPORATION - 2010
//
//*****************************************************************************//
//*****************************************************************************//


#ifndef ATC3DGm_H   // JPB bumper to prevent multiple-inclusion
#define ATC3DGm_H


#define CPLUSPLUS

#ifdef LINUX
#define ATC3DGm_API 
#else
#ifdef DEF_FILE
	#ifdef ATC3DGm_EXPORTS
	#define ATC3DGm_API
	#else
	#define ATC3DGm_API
	#endif
#else
	#ifdef CPLUSPLUS
		#ifdef ATC3DGm_EXPORTS
		#define ATC3DGm_API __declspec(dllexport)
		#else
		#define ATC3DGm_API __declspec(dllimport)
		#endif
	#else
		#ifdef ATC3DGm_EXPORTS
		#define ATC3DGm_API extern "C" __declspec(dllexport)
		#else
		#define ATC3DGm_API extern "C" __declspec(dllimport)
		#endif
	#endif
#endif
#endif
 


 
/*****************************************************************************
							ENUMERATED CONSTANTS
 *****************************************************************************/

//*****************************************************************************
//	
//	ERROR MESSAGE format is as follows:
//	===================================
//
//	All commands return a 32-bit integer with the following bit definitions:
//
//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1 0 0 0 0 0 0 0 0 0 0
//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//  +-+-+-+-+-+-+-----------+-------+-------------------------------+
//  |E|W|X|R|B|M|  Reserved |Address|             Code              |
//  +-+-+-+-+-+-+-----------+-------+-------------------------------+
//
//  where
//
//		E - indicates an ERROR
//			(Operation either cannot or will not proceed as intended
//				e.g. EEPROM error of some kind. Requires hardware reset
//				of system and/or replacement of hardware)
//		W - indicates a WARNING
//			(Operation may proceed but remedial action needs to be taken
//				e.g. Sensor has been removed. Fix by replacing Sensor)
//      X - indicates Transmitter related message
//      R - indicates Sensor related message
//			(If neither X nor R is set then the message is a SYSTEM message)
//		B - indicates message originated in the BIRD hardware
//		M - indicates there are more messages pending (no longer used)
//
//		Address gives the index number of the device generating the message
//			(Driver and system messages always have address 0)
//
//      Code - is the status code as enumerated in BIRD_ERROR_CODES 
//
//*****************************************************************************
enum BIRD_ERROR_CODES
{
											//	ERROR CODE DISPOSITION
											//    |		(Some error codes have been retired.
											//    |      The column below describes which codes 
											//	  |      have been retired and why. O = Obolete,
											//    V      I = handled internally)
	BIRD_ERROR_SUCCESS=0,					//00 < > No error	
	BIRD_ERROR_PCB_HARDWARE_FAILURE,		//01 < > indeterminate failure on PCB
	BIRD_ERROR_TRANSMITTER_EEPROM_FAILURE,	//02 <I> transmitter bad eeprom
	BIRD_ERROR_SENSOR_SATURATION_START,		//03 <I> sensor has gone into saturation
	BIRD_ERROR_ATTACHED_DEVICE_FAILURE,		//04 <O> either a sensor or transmitter reports bad
	BIRD_ERROR_CONFIGURATION_DATA_FAILURE,	//05 <O> device EEPROM detected but corrupt
	BIRD_ERROR_ILLEGAL_COMMAND_PARAMETER,	//06 < > illegal PARAMETER_TYPE passed to driver
	BIRD_ERROR_PARAMETER_OUT_OF_RANGE,		//07 < > PARAMETER_TYPE legal, but PARAMETER out of range
	BIRD_ERROR_NO_RESPONSE,					//08 <O> no response at all from target card firmware
	BIRD_ERROR_COMMAND_TIME_OUT,			//09 < > time out before response from target board
	BIRD_ERROR_INCORRECT_PARAMETER_SIZE,	//10 < > size of parameter passed is incorrect
	BIRD_ERROR_INVALID_VENDOR_ID,			//11 <O> driver started with invalid PCI vendor ID
	BIRD_ERROR_OPENING_DRIVER,				//12 < > couldn't start driver
	BIRD_ERROR_INCORRECT_DRIVER_VERSION,	//13 < > wrong driver version found
	BIRD_ERROR_NO_DEVICES_FOUND,			//14 < > no BIRDs were found anywhere
	BIRD_ERROR_ACCESSING_PCI_CONFIG,		//15 < > couldn't access BIRDs config space
	BIRD_ERROR_INVALID_DEVICE_ID,			//16 < > device ID out of range
	BIRD_ERROR_FAILED_LOCKING_DEVICE,		//17 < > couldn't lock driver
	BIRD_ERROR_BOARD_MISSING_ITEMS,			//18 < > config space items missing
	BIRD_ERROR_NOTHING_ATTACHED,			//19 <O> card found but no sensors or transmitters attached
	BIRD_ERROR_SYSTEM_PROBLEM,				//20 <O> non specific system problem
	BIRD_ERROR_INVALID_SERIAL_NUMBER,		//21 <O> serial number does not exist in system
	BIRD_ERROR_DUPLICATE_SERIAL_NUMBER,		//22 <O> 2 identical serial numbers passed in set command
	BIRD_ERROR_FORMAT_NOT_SELECTED,			//23 <O> data format not selected yet
	BIRD_ERROR_COMMAND_NOT_IMPLEMENTED,		//24 < > valid command, not implemented yet
	BIRD_ERROR_INCORRECT_BOARD_DEFAULT,		//25 < > incorrect response to reading parameter
	BIRD_ERROR_INCORRECT_RESPONSE,			//26 <O> response received, but data,values in error
	BIRD_ERROR_NO_TRANSMITTER_RUNNING,		//27 < > there is no transmitter running
	BIRD_ERROR_INCORRECT_RECORD_SIZE,		//28 < > data record size does not match data format size
	BIRD_ERROR_TRANSMITTER_OVERCURRENT,		//29 <I> transmitter over-current detected
	BIRD_ERROR_TRANSMITTER_OPEN_CIRCUIT,	//30 <I> transmitter open circuit or removed
	BIRD_ERROR_SENSOR_EEPROM_FAILURE,		//31 <I> sensor bad eeprom
	BIRD_ERROR_SENSOR_DISCONNECTED,			//32 <I> previously good sensor has been removed
	BIRD_ERROR_SENSOR_REATTACHED,			//33 <I> previously good sensor has been reattached
	BIRD_ERROR_NEW_SENSOR_ATTACHED,			//34 <O> new sensor attached
	BIRD_ERROR_UNDOCUMENTED,				//35 <I> undocumented error code received from bird
	BIRD_ERROR_TRANSMITTER_REATTACHED,		//36 <I> previously good transmitter has been reattached
	BIRD_ERROR_WATCHDOG,					//37 < > watchdog timeout
	BIRD_ERROR_CPU_TIMEOUT_START,			//38 <I> CPU ran out of time executing algorithm (start)
	BIRD_ERROR_PCB_RAM_FAILURE,				//39 <I> BIRD on-board RAM failure
	BIRD_ERROR_INTERFACE,					//40 <I> BIRD PCI interface error
	BIRD_ERROR_PCB_EPROM_FAILURE,			//41 <I> BIRD on-board EPROM failure
	BIRD_ERROR_SYSTEM_STACK_OVERFLOW,		//42 <I> BIRD program stack overrun
	BIRD_ERROR_QUEUE_OVERRUN,				//43 <I> BIRD error message queue overrun
	BIRD_ERROR_PCB_EEPROM_FAILURE,			//44 <I> PCB bad EEPROM
	BIRD_ERROR_SENSOR_SATURATION_END,		//45 <I> Sensor has gone out of saturation
	BIRD_ERROR_NEW_TRANSMITTER_ATTACHED,	//46 <O> new transmitter attached
	BIRD_ERROR_SYSTEM_UNINITIALIZED,		//47 < > InitializeBIRDSystem not called yet
	BIRD_ERROR_12V_SUPPLY_FAILURE,			//48 <I > 12V Power supply not within specification
	BIRD_ERROR_CPU_TIMEOUT_END,				//49 <I> CPU ran out of time executing algorithm (end)
	BIRD_ERROR_INCORRECT_PLD,				//50 < > PCB PLD not compatible with this API DLL
	BIRD_ERROR_NO_TRANSMITTER_ATTACHED,		//51 < > No transmitter attached to this ID
	BIRD_ERROR_NO_SENSOR_ATTACHED,			//52 < > No sensor attached to this ID

	// new error codes added 2/27/03 
	// (Version 1,31,5,01)  multi-sensor, synchronized
	BIRD_ERROR_SENSOR_BAD,					//53 < > Non-specific hardware problem
	BIRD_ERROR_SENSOR_SATURATED,			//54 < > Sensor saturated error
	BIRD_ERROR_CPU_TIMEOUT,					//55 < > CPU unable to complete algorithm on current cycle
	BIRD_ERROR_UNABLE_TO_CREATE_FILE,		//56 < > Could not create and open file for saving setup
	BIRD_ERROR_UNABLE_TO_OPEN_FILE,			//57 < > Could not open file for restoring setup
	BIRD_ERROR_MISSING_CONFIGURATION_ITEM,	//58 < > Mandatory item missing from configuration file
	BIRD_ERROR_MISMATCHED_DATA,				//59 < > Data item in file does not match system value
	BIRD_ERROR_CONFIG_INTERNAL,				//60 < > Internal error in config file handler
	BIRD_ERROR_UNRECOGNIZED_MODEL_STRING,	//61 < > Board does not have a valid model string
	BIRD_ERROR_INCORRECT_SENSOR,			//62 < > Invalid sensor type attached to this board
	BIRD_ERROR_INCORRECT_TRANSMITTER,		//63 < > Invalid transmitter type attached to this board

	// new error code added 1/18/05
	// (Version 1.31.5.22) 
	//		multi-sensor, 
	//		synchronized-fluxgate, 
	//		integrating micro-sensor,
	//		flat panel transmitter
	BIRD_ERROR_ALGORITHM_INITIALIZATION,	//64 < > Flat panel algorithm initialization failed

	// new error code for multi-sync
	BIRD_ERROR_LOST_CONNECTION,				//65 < > USB connection has been lost
	BIRD_ERROR_INVALID_CONFIGURATION,		//66 < > Invalid configuration

	// VPD error code
	BIRD_ERROR_TRANSMITTER_RUNNING,			//67 < > TX running while reading/writing VPD

	BIRD_ERROR_MAXIMUM_VALUE = 0x7F			//	     ## value = number of error codes ##
};

// error message defines
#define	ERROR_FLAG					0x80000000
#define	WARNING_FLAG				0x40000000

#define	XMTR_ERROR_SOURCE			0x20000000
#define	RCVR_ERROR_SOURCE			0x10000000
#define	BIRD_ERROR_SOURCE			0x08000000

#define DIAG_ERROR_SOURCE			0x04000000

// SYSTEM error = none of the above

// NOTE: The MULTIPLE_ERRORS flag is no longer generated
// It has been left in for backwards compatibility
#define	MULTIPLE_ERRORS				0x04000000

// DEVICE STATUS ERROR BIT DEFINITIONS
#define VALID_STATUS				0x00000000
#define GLOBAL_ERROR				0x00000001
#define	NOT_ATTACHED				0x00000002
#define	SATURATED					0x00000004
#define	BAD_EEPROM					0x00000008
#define	HARDWARE					0x00000010
#define	NON_EXISTENT				0x00000020
#define	UNINITIALIZED				0x00000040
//#define NO_TRANSMITTER				0x00000080
#define NO_TRANSMITTER_RUNNING		0x00000080
#define	BAD_12V						0x00000100
#define	CPU_TIMEOUT					0x00000200
#define	INVALID_DEVICE				0x00000400
#define NO_TRANSMITTER_ATTACHED		0x00000800
#define OUT_OF_MOTIONBOX			0x00001000
#define ALGORITHM_INITIALIZING		0x00002000

#define	TRUE	1
#define	FALSE	0






enum MESSAGE_TYPE
{	
	SIMPLE_MESSAGE,							// short string describing error code
	VERBOSE_MESSAGE,							// long string describing error code
};

enum TRANSMITTER_PARAMETER_TYPE
{
	SERIAL_NUMBER_TX,		// attached transmitter's serial number
	REFERENCE_FRAME,		// structure of type DOUBLE_ANGLES_RECORD
	XYZ_REFERENCE_FRAME,	// boolean value to select/deselect mode
	VITAL_PRODUCT_DATA_TX,	// single byte parameter to be read/write from VPD section of xmtr EEPROM
	MODEL_STRING_TX,		// 11 byte null terminated character string
	PART_NUMBER_TX,			// 16 byte null terminated character string
	END_OF_TX_LIST
};

enum SENSOR_PARAMETER_TYPE
{							
	DATA_FORMAT,			// enumerated constant of type DATA_FORMAT_TYPE
	ANGLE_ALIGN,			// structure of type DOUBLE_ANGLES_RECORD
	HEMISPHERE,				// enumerated constant of type HEMISPHERE_TYPE
	FILTER_AC_WIDE_NOTCH,	// boolean value to select/deselect filter
	FILTER_AC_NARROW_NOTCH,	// boolean value to select/deselect filter
	FILTER_DC_ADAPTIVE,		// double value in range 0.0 (no filtering) to 1.0 (max)
	FILTER_ALPHA_PARAMETERS,// structure of type ADAPTIVE_PARAMETERS
	FILTER_LARGE_CHANGE,	// boolean value to select/deselect filter
	QUALITY,				// structure of type QUALITY_PARAMETERS
	SERIAL_NUMBER_RX,		// attached sensor's serial number
	SENSOR_OFFSET,			// structure of type DOUBLE_POSITION_RECORD
	VITAL_PRODUCT_DATA_RX,	// single byte parameter to be read/write from VPD section of sensor EEPROM
	VITAL_PRODUCT_DATA_PREAMP,	// single byte parameter to be read/write from VPD section of preamp EEPROM
	MODEL_STRING_RX,		// 11 byte null terminated character string
	PART_NUMBER_RX,			// 16 byte null terminated character string
	MODEL_STRING_PREAMP,	// 11 byte null terminated character string
	PART_NUMBER_PREAMP,		// 16 byte null terminated character string
	END_OF_RX_LIST
};

enum BOARD_PARAMETER_TYPE
{
	SERIAL_NUMBER_PCB,		// installed board's serial number
	BOARD_SOFTWARE_REVISIONS,	// Extra SW_REV's added 10-5-06 JBD
	POST_ERROR_PCB,
	DIAGNOSTIC_TEST_PCB,


	VITAL_PRODUCT_DATA_PCB,	// single byte parameter to be read/write from VPD section of xmtr EEPROM
	MODEL_STRING_PCB,		// 11 byte null terminated character string
	PART_NUMBER_PCB,		// 16 byte null terminated character string
	END_OF_PCB_LIST_BRD
};

enum SYSTEM_PARAMETER_TYPE
{
	SELECT_TRANSMITTER,		// short int equal to transmitterID of selected transmitter
	POWER_LINE_FREQUENCY,	// double value (range is hardware dependent)
	AGC_MODE,				// enumerated constant of type AGC_MODE_TYPE
	MEASUREMENT_RATE,		// double value (range is hardware dependent)
	MAXIMUM_RANGE,			// double value (range is hardware dependent)
	METRIC,					// boolean value to select metric units for position
	VITAL_PRODUCT_DATA,
	POST_ERROR,
	DIAGNOSTIC_TEST,
	REPORT_RATE,			// single byte 1-127			
	COMMUNICATIONS_MEDIA,	// Media structure
	LOGGING,				// Boolean
	RESET,					// Boolean
	AUTOCONFIG,				// BYTE 1-127
	END_OF_LIST				// end of list place holder
};

enum COMMUNICATIONS_MEDIA_TYPE
{
	UNKNOWN,
	USB,					// Auto select USB driver
	RS232,					// Force to RS232
	TCPIP					// Force to TCP/IP
};

enum FILTER_OPTION
{
	NO_FILTER,
	DEFAULT_FLOCK_FILTER
};

enum HEMISPHERE_TYPE
{
	FRONT,
	BACK,
	TOP,
	BOTTOM,
	LEFT,
	RIGHT
};

enum AGC_MODE_TYPE
{
	TRANSMITTER_AND_SENSOR_AGC,		// Old style normal addressing mode
	SENSOR_AGC_ONLY					// Old style extended addressing mode
};

enum DATA_FORMAT_TYPE
{
	NO_FORMAT_SELECTED=0,

	// SHORT (integer) formats
	SHORT_POSITION,
	SHORT_ANGLES,
	SHORT_MATRIX,
	SHORT_QUATERNIONS,
	SHORT_POSITION_ANGLES,
	SHORT_POSITION_MATRIX,
	SHORT_POSITION_QUATERNION,

	// DOUBLE (floating point) formats
	DOUBLE_POSITION,
	DOUBLE_ANGLES,
	DOUBLE_MATRIX,
	DOUBLE_QUATERNIONS,
	DOUBLE_POSITION_ANGLES,		// system default
	DOUBLE_POSITION_MATRIX,
	DOUBLE_POSITION_QUATERNION,

	// DOUBLE (floating point) formats with time stamp appended
	DOUBLE_POSITION_TIME_STAMP,
	DOUBLE_ANGLES_TIME_STAMP,
	DOUBLE_MATRIX_TIME_STAMP,
	DOUBLE_QUATERNIONS_TIME_STAMP,
	DOUBLE_POSITION_ANGLES_TIME_STAMP,
	DOUBLE_POSITION_MATRIX_TIME_STAMP,
	DOUBLE_POSITION_QUATERNION_TIME_STAMP,

	// DOUBLE (floating point) formats with time stamp appended and quality #
	DOUBLE_POSITION_TIME_Q,
	DOUBLE_ANGLES_TIME_Q,
	DOUBLE_MATRIX_TIME_Q,
	DOUBLE_QUATERNIONS_TIME_Q,
	DOUBLE_POSITION_ANGLES_TIME_Q,
	DOUBLE_POSITION_MATRIX_TIME_Q,
	DOUBLE_POSITION_QUATERNION_TIME_Q,

	// These DATA_FORMAT_TYPE codes contain every format in a single structure
	SHORT_ALL,
	DOUBLE_ALL,
	DOUBLE_ALL_TIME_STAMP,
	DOUBLE_ALL_TIME_STAMP_Q,
	DOUBLE_ALL_TIME_STAMP_Q_RAW,	// this format contains a raw data matrix and
									// is for factory use only...

	// DOUBLE (floating point) formats with time stamp appended, quality # and button
	DOUBLE_POSITION_ANGLES_TIME_Q_BUTTON,
	DOUBLE_POSITION_MATRIX_TIME_Q_BUTTON,
	DOUBLE_POSITION_QUATERNION_TIME_Q_BUTTON,

	// New types for button and wrapper
	DOUBLE_POSITION_ANGLES_MATRIX_QUATERNION_TIME_Q_BUTTON,

	MAXIMUM_FORMAT_CODE
};

enum BOARD_TYPES
{	
	ATC3DG_MEDSAFE,					// Standalone, DSP, 4 sensor
	PCIBIRD_STD1,					// single standard sensor
	PCIBIRD_STD2,					// dual standard sensor
	PCIBIRD_8mm1,					// single 8mm sensor
	PCIBIRD_8mm2,					// dual 8mm sensor
	PCIBIRD_2mm1,					// single 2mm sensor (microsensor)
	PCIBIRD_2mm2,					// dual 2mm sensor (microsensor)
	PCIBIRD_FLAT,					// flat transmitter, 8mm
	PCIBIRD_FLAT_MICRO1,			// flat transmitter, single TEM sensor (all types)
	PCIBIRD_FLAT_MICRO2,			// flat transmitter, dual TEM sensor (all types)
	PCIBIRD_DSP4,					// Standalone, DSP, 4 sensor
	PCIBIRD_UNKNOWN,				// default
	ATC3DG_BB						// BayBird
};

enum DEVICE_TYPES
{
	STANDARD_SENSOR,				// 25mm standard sensor
	TYPE_800_SENSOR,				// 8mm sensor
	STANDARD_TRANSMITTER,			// TX for 25mm sensor
	MINIBIRD_TRANSMITTER,			// TX for 8mm sensor
	SMALL_TRANSMITTER,				// "compact" transmitter
	TYPE_500_SENSOR,				// 5mm sensor
	TYPE_180_SENSOR,				// 1.8mm microsensor
	TYPE_130_SENSOR,				// 1.3mm microsensor
	TYPE_TEM_SENSOR,				// 1.8mm, 1.3mm, 0.Xmm microsensors
	UNKNOWN_SENSOR,					// default
	UNKNOWN_TRANSMITTER,			// default
	TYPE_800_BB_SENSOR,				// BayBird sensor
	TYPE_800_BB_STD_TRANSMITTER,	// BayBird standard TX
	TYPE_800_BB_SMALL_TRANSMITTER,	// BayBird small TX
	TYPE_090_BB_SENSOR				// Baybird 0.9 mm sensor
};

// Async and Sync sensor id parameter
#define ALL_SENSORS 0xffff
/*****************************************************************************
							TYPEDEF DEFINITIONS
 *****************************************************************************/

#ifndef BASETYPES
#define BASETYPES
typedef unsigned long	ULONG;
typedef ULONG			*PULONG;
typedef unsigned short	USHORT;
typedef USHORT			*PUSHORT;
typedef short			SHORT;
typedef SHORT			*PSHORT;
typedef unsigned char	UCHAR;
typedef UCHAR			*PUCHAR;
typedef char			*PSZ;
#endif  /* !BASETYPES */

typedef char			CHAR;
typedef const CHAR		*LPCSTR, *PCSTR;
//typedef LPCSTR			LPCTSTR;
typedef int				BOOL;
typedef	ULONG			DEVICE_STATUS;

typedef unsigned char	BYTE;
typedef unsigned short  WORD;
typedef unsigned long   DWORD;

/*****************************************************************************
							STRUCTURE DEFINITIONS
 *****************************************************************************/

typedef struct tagTRANSMITTER_CONFIGURATION
{
	ULONG				serialNumber;
	USHORT				boardNumber;
	USHORT				channelNumber;
	enum DEVICE_TYPES	type;
	BOOL				attached;
} TRANSMITTER_CONFIGURATION;

typedef struct tagSENSOR_CONFIGURATION
{
	ULONG				serialNumber;
	USHORT				boardNumber;
	USHORT				channelNumber;
	enum DEVICE_TYPES	type;
	BOOL				attached;
} SENSOR_CONFIGURATION;

typedef struct tagBOARD_CONFIGURATION
{
	ULONG				serialNumber;
	enum BOARD_TYPES	type;
	USHORT				revision;
	USHORT				numberTransmitters;
	USHORT				numberSensors;
	USHORT				firmwareNumber;
	USHORT				firmwareRevision;
	char				modelString[10];

} BOARD_CONFIGURATION;

typedef struct tagSYSTEM_CONFIGURATION
{
	double				measurementRate;
	double				powerLineFrequency;
	double				maximumRange;
	enum AGC_MODE_TYPE	agcMode;
	int					numberBoards;
	int					numberSensors;
	int					numberTransmitters;
	int					transmitterIDRunning;
	BOOL				metric;
} SYSTEM_CONFIGURATION;

typedef struct tagCOMMUNICATIONS_MEDIA_PARAMETERS
{
	COMMUNICATIONS_MEDIA_TYPE	mediaType;
	union
	{
		struct
		{
			CHAR	comport[64];
		} rs232;
		struct
		{
			USHORT	port;
			CHAR	ipaddr[64];
		} tcpip;
	};
} COMMUNICATIONS_MEDIA_PARAMETERS;

typedef struct tagADAPTIVE_PARAMETERS
{
	USHORT				alphaMin[7];
	USHORT				alphaMax[7];
	USHORT				vm[7];
	BOOL				alphaOn;
} ADAPTIVE_PARAMETERS;

typedef struct tagQUALITY_PARAMETERS
{
	SHORT				error_slope;
	SHORT				error_offset;
	USHORT				error_sensitivity;
	USHORT				filter_alpha;
} QUALITY_PARAMETERS;

// New Drivebay system parameter structure(s)
typedef struct tagVPD_COMMAND_PARAMETER
{
	USHORT				address;
	UCHAR				value;
} VPD_COMMAND_PARAMETER;

typedef struct tagPOST_ERROR_PARAMETER
{
	USHORT				error;
	UCHAR				channel;
	UCHAR				fatal;
	UCHAR				moreErrors;
} POST_ERROR_PARAMETER;

typedef struct tagDIAGNOSTIC_TEST_PARAMETER
{
	UCHAR				suite;		// User sets this, 0 - All Suites
	UCHAR				test;		// User sets this, 0 - All Tests

	UCHAR				suites;		// set, returns suites run
									// get, returns num of suites, not used for # test query
	UCHAR				tests;		// set, returns tests run in the given suite
									// get, returns num of tests for given suite, not used for suite query
	PUCHAR				pTestName;  // User supplied ptr to 64 bytes, we fill it in
	USHORT				testNameLen;

	USHORT				diagError;	// set only, result of diagnostic execution
} DIAGNOSTIC_TEST_PARAMETER;

typedef struct tagBOARD_REVISIONS
{
	USHORT	boot_loader_sw_number;		// example 3.1 -> revision is 3, number is 1
	USHORT	boot_loader_sw_revision;
	USHORT	mdsp_sw_number;
	USHORT	mdsp_sw_revision;
	USHORT	nondipole_sw_number;
	USHORT	nondipole_sw_revision;
	USHORT	fivedof_sw_number;
	USHORT	fivedof_sw_revision;
	USHORT	sixdof_sw_number;
	USHORT	sixdof_sw_revision;
	USHORT	dipole_sw_number;
	USHORT	dipole_sw_revision;
} BOARD_REVISIONS;

//
// Data formatting structures
//
typedef struct tagSHORT_POSITION
{
	short	x;
	short	y;
	short	z;
} SHORT_POSITION_RECORD;

typedef struct tagSHORT_ANGLES
{
	short	a;			// azimuth
	short	e;			// elevation
	short	r;			// roll
} SHORT_ANGLES_RECORD;

typedef struct tagSHORT_MATRIX
{
	short	s[3][3];
} SHORT_MATRIX_RECORD;

typedef struct tagSHORT_QUATERNIONS
{
	short	q[4];
} SHORT_QUATERNIONS_RECORD;

typedef struct tagSHORT_POSITION_ANGLES
{
	short	x;
	short	y;
	short	z;
	short	a;
	short	e;
	short	r;
} SHORT_POSITION_ANGLES_RECORD;

typedef struct tagSHORT_POSITION_MATRIX
{
	short	x;
	short	y;
	short	z;
	short	s[3][3];
} SHORT_POSITION_MATRIX_RECORD;

typedef struct tagSHORT_POSITION_QUATERNION
{
	short	x;
	short	y;
	short	z;
	short	q[4];
} SHORT_POSITION_QUATERNION_RECORD;

typedef struct tagDOUBLE_POSITION
{
	double	x;
	double	y;
	double	z;
} DOUBLE_POSITION_RECORD;

typedef struct tagDOUBLE_ANGLES
{
	double	a;			// azimuth
	double	e;			// elevation
	double	r;			// roll
} DOUBLE_ANGLES_RECORD;

typedef struct tagDOUBLE_MATRIX
{
	double	s[3][3];
} DOUBLE_MATRIX_RECORD;

typedef struct tagDOUBLE_QUATERNIONS
{
	double	q[4];
} DOUBLE_QUATERNIONS_RECORD;

typedef struct tagDOUBLE_POSITION_ANGLES
{
	double	x;
	double	y;
	double	z;
	double	a;
	double	e;
	double	r;
} DOUBLE_POSITION_ANGLES_RECORD;

typedef struct tagDOUBLE_POSITION_MATRIX
{
	double	x;
	double	y;
	double	z;
	double	s[3][3];
} DOUBLE_POSITION_MATRIX_RECORD;

typedef struct tagDOUBLE_POSITION_QUATERNION
{
	double	x;
	double	y;
	double	z;
	double	q[4];
} DOUBLE_POSITION_QUATERNION_RECORD;

typedef struct tagDOUBLE_POSITION_TIME_STAMP
{
	double	x;
	double	y;
	double	z;
	double	time;
} DOUBLE_POSITION_TIME_STAMP_RECORD;

typedef struct tagDOUBLE_ANGLES_TIME_STAMP
{
	double	a;			// azimuth
	double	e;			// elevation
	double	r;			// roll
	double	time;
} DOUBLE_ANGLES_TIME_STAMP_RECORD;

typedef struct tagDOUBLE_MATRIX_TIME_STAMP
{
	double	s[3][3];
	double	time;
} DOUBLE_MATRIX_TIME_STAMP_RECORD;

typedef struct tagDOUBLE_QUATERNIONS_TIME_STAMP
{
	double	q[4];
	double	time;
} DOUBLE_QUATERNIONS_TIME_STAMP_RECORD;

typedef struct tagDOUBLE_POSITION_ANGLES_TIME_STAMP
{
	double	x;
	double	y;
	double	z;
	double	a;
	double	e;
	double	r;
	double	time;
} DOUBLE_POSITION_ANGLES_TIME_STAMP_RECORD;

typedef struct tagDOUBLE_POSITION_MATRIX_STAMP_RECORD
{
	double	x;
	double	y;
	double	z;
	double	s[3][3];
	double	time;
} DOUBLE_POSITION_MATRIX_TIME_STAMP_RECORD;

typedef struct tagDOUBLE_POSITION_QUATERNION_STAMP_RECORD
{
	double	x;
	double	y;
	double	z;
	double	q[4];
	double	time;
} DOUBLE_POSITION_QUATERNION_TIME_STAMP_RECORD;

typedef struct tagDOUBLE_POSITION_TIME_Q
{
	double	x;
	double	y;
	double	z;
	double	time;
	USHORT	quality;		
} DOUBLE_POSITION_TIME_Q_RECORD;

typedef struct tagDOUBLE_ANGLES_TIME_Q
{
	double	a;			// azimuth
	double	e;			// elevation
	double	r;			// roll
	double	time;
	USHORT	quality;		
} DOUBLE_ANGLES_TIME_Q_RECORD;

typedef struct tagDOUBLE_MATRIX_TIME_Q
{
	double	s[3][3];
	double	time;
	USHORT	quality;		
} DOUBLE_MATRIX_TIME_Q_RECORD;

typedef struct tagDOUBLE_QUATERNIONS_TIME_Q
{
	double	q[4];
	double	time;
	USHORT	quality;		
} DOUBLE_QUATERNIONS_TIME_Q_RECORD;

typedef struct tagDOUBLE_POSITION_ANGLES_TIME_Q_RECORD
{
	double	x;
	double	y;
	double	z;
	double	a;
	double	e;
	double	r;
	double	time;
	USHORT	quality;		
} DOUBLE_POSITION_ANGLES_TIME_Q_RECORD;

typedef struct tagDOUBLE_POSITION_MATRIX_TIME_Q_RECORD
{
	double	x;
	double	y;
	double	z;
	double	s[3][3];
	double	time;
	USHORT	quality;		
} DOUBLE_POSITION_MATRIX_TIME_Q_RECORD;

typedef struct tagDOUBLE_POSITION_QUATERNION_TIME_Q_RECORD
{
	double	x;
	double	y;
	double	z;
	double	q[4];
	double	time;
	USHORT	quality;		
} DOUBLE_POSITION_QUATERNION_TIME_Q_RECORD;

typedef struct tagDOUBLE_POSITION_ANGLES_TIME_Q_BUTTON_RECORD
{
	double	x;
	double	y;
	double	z;
	double	a;
	double	e;
	double	r;
	double	time;
	USHORT	quality;
	USHORT	button;
} DOUBLE_POSITION_ANGLES_TIME_Q_BUTTON_RECORD;

typedef struct tagDOUBLE_POSITION_MATRIX_TIME_Q_BUTTON_RECORD
{
	double	x;
	double	y;
	double	z;
	double	s[3][3];
	double	time;
	USHORT	quality;		
	USHORT	button;
} DOUBLE_POSITION_MATRIX_TIME_Q_BUTTON_RECORD;

typedef struct tagDOUBLE_POSITION_QUATERNION_TIME_Q_BUTTON_RECORD
{
	double	x;
	double	y;
	double	z;
	double	q[4];
	double	time;
	USHORT	quality;		
	USHORT	button;
} DOUBLE_POSITION_QUATERNION_TIME_Q_BUTTON_RECORD;

typedef struct tagDOUBLE_POSITION_ANGLES_MATRIX_QUATERNION_TIME_Q_BUTTON_RECORD
{
	double	x;
	double	y;
	double	z;
	double	a;
	double	e;
	double	r;
	double	s[3][3];
	double	q[4];
	double	time;
	USHORT	quality;
	USHORT	button;
} DOUBLE_POSITION_ANGLES_MATRIX_QUATERNION_TIME_Q_BUTTON_RECORD;

typedef struct tagSHORT_ALL_RECORD
{
	short	x;
	short	y;
	short	z;
	short	a;			// azimuth
	short	e;			// elevation
	short	r;			// roll
	short	s[3][3];
	short	q[4];
}SHORT_ALL_RECORD;

typedef struct tagDOUBLE_ALL_RECORD
{
	double	x;
	double	y;
	double	z;
	double	a;			// azimuth
	double	e;			// elevation
	double	r;			// roll
	double	s[3][3];
	double	q[4];
}DOUBLE_ALL_RECORD;

typedef struct tagDOUBLE_ALL_TIME_STAMP_RECORD
{
	double	x;
	double	y;
	double	z;
	double	a;			// azimuth
	double	e;			// elevation
	double	r;			// roll
	double	s[3][3];
	double	q[4];
	double	time;
}DOUBLE_ALL_TIME_STAMP_RECORD;

typedef struct tagDOUBLE_ALL_TIME_STAMP_Q_RECORD
{
	double	x;
	double	y;
	double	z;
	double	a;			// azimuth
	double	e;			// elevation
	double	r;			// roll
	double	s[3][3];
	double	q[4];
	double	time;
	USHORT	quality;		
}DOUBLE_ALL_TIME_STAMP_Q_RECORD;

typedef struct tagDOUBLE_ALL_TIME_STAMP_Q_RAW_RECORD
{
	double	x;
	double	y;
	double	z;
	double	a;			// azimuth
	double	e;			// elevation
	double	r;			// roll
	double	s[3][3];
	double	q[4];
	double	time;
	USHORT	quality;		
	double	raw[3][3];
}DOUBLE_ALL_TIME_STAMP_Q_RAW_RECORD;




/*****************************************************************************
							FUNCTION PROTOTYPES
 *****************************************************************************/

/*
	InitializeBIRDSystem	Starts and initializes driver, resets
						hardware and interrogates hardware. Builds
						local database of system resources.

	Parameters Passed:  none

	Return Value:       error status

    Remarks:			Can be used anytime a catastrophic failure
						has occurred and the system needs to be
						restarted.

*/
ATC3DGm_API int InitializeBIRDSystem(void);

/*
	GetBIRDSystemConfiguration

	Parameters Passed:  SYSTEM_CONFIGURATION* 

	Return Value:       error status

    Remarks:            Returns SYSTEM_CONFIGURATION structure
						It contains values equal to the number of
						transmitters, sensors and boards detected
						in the system. (The board information is for 
						test/diagnostic purposes, all commands
						reference sensors and transmitters only) Once 
						the number of devices is known, (e.g. n) the 
						range of IDs for those devices becomes 0..(n-1)
*/
ATC3DGm_API int GetBIRDSystemConfiguration(
	SYSTEM_CONFIGURATION* systemConfiguration
	);

/*
	GetTransmitterConfiguration

	Parameters Passed:	USHORT transmitterID
						TRANSMITTER_CONFIGURATION *transmitterConfiguration

	Return Value:       error status

    Remarks:			After getting system config the user can then pass 
						the index of a transmitter of interest to this function
						which will then return the config for that transmitter.
						Items of interest are the serial number and status.

*/
ATC3DGm_API int GetTransmitterConfiguration(
	USHORT transmitterID,
	TRANSMITTER_CONFIGURATION* transmitterConfiguration
	);

/*
	GetSensorConfiguration

	Parameters Passed:  USHORT sensorID,
						SENSOR_CONFIGURATION* sensorConfiguration

	Return Value:       error status

    Remarks:			Similar to the transmitter function.            

*/
ATC3DGm_API int GetSensorConfiguration(
	USHORT sensorID,
	SENSOR_CONFIGURATION* sensorConfiguration
	);

/*
	GetBoardConfiguration

	Parameters Passed:  USHORT boardID,
						BOARD_CONFIGURATION* boardConfiguration

	Return Value:       error status

    Remarks:			Similar to the Sensor and Transmitter
						functions. Also returns information on
						how many sensors and transmitters are
						attached. NOTE: Board information is not
						needed during normal operation this is
						only provided for "accounting" purposes.

*/
ATC3DGm_API int GetBoardConfiguration(
	USHORT boardID,
	BOARD_CONFIGURATION* boardConfiguration
	);

/*
	GetAsynchronousRecord

	Parameters Passed:  USHORT sensorID,
						void *pRecord,
						int	recordSize

	Return Value:       error status

    Remarks:			Returns data immediately in the currently 
						selected format from the last measurement 
						cycle. Requires user providing a buffer large 
						enough for the result otherwise an error is 
						generated and data lost.
						(Old style BIRD POINT mode)            

*/
ATC3DGm_API int GetAsynchronousRecord(
	USHORT sensorID,
	void *pRecord,
	int	recordSize
	);

/*
	GetSynchronousRecord

	Parameters Passed:  USHORT sensorID,
						void *pRecord,
						int	recordSize

	Return Value:       error status

    Remarks:			Returns a record after next measurement cycle. Puts 
						system into mode where records are generated 1/cycle.
						Will return one and only one record per measurement
						cycle. Will queue the records for each measurement
						cycle if command not issued sufficiently often. If 
						command issued too often will time-out with no data.
						(old style BIRD STREAM mode)            

*/
ATC3DGm_API int GetSynchronousRecord(
	USHORT sensorID,
	void *pRecord,
	int	recordSize
	);



/*
	GetSystemParameter

	Parameters Passed:  PARAMETER_TYPE	parameterType,
						void			*pBuffer,
						int				bufferSize

	Return Value:       error status

    Remarks:			When a properly enumerated parameter type constant
						is used, the command will return the parameter value
						to the buffer provided by the user. An error is
						generated if the buffer is incorrectly sized

*/
ATC3DGm_API int GetSystemParameter(
	enum SYSTEM_PARAMETER_TYPE	parameterType,
	void						*pBuffer,
	int							bufferSize
	);


/*
	SetSystemParameter

	Parameters Passed:	PARAMETER_TYPE	parameterType,
						void			*pBuffer,
						int				bufferSize

	Return Value:       error status

    Remarks:			Similar to GetSystemParameter but allows user
						to set (write) the parameter.

*/
ATC3DGm_API int SetSystemParameter(
	enum SYSTEM_PARAMETER_TYPE	parameterType,
	void						*pBuffer,
	int							bufferSize
	);


/*
	GetSensorParameter

	Parameters Passed:  USHORT			sensorID,
						PARAMETER_TYPE	parameterType,
						void			*pBuffer,
						int				bufferSize

	Return Value:       error status

    Remarks:			When a sensor is selected with a valid index (ID) 
						and a properly enumerated parameter type constant
						is used, the command will return the parameter value
						to the buffer provided by the user. An error is
						generated if the buffer is incorrectly sized

*/
ATC3DGm_API int GetSensorParameter(
	USHORT						sensorID,
	enum SENSOR_PARAMETER_TYPE	parameterType,
	void						*pBuffer,
	int							bufferSize
	);


/*
	SetSensorParameter

	Parameters Passed:	USHORT			sensorID,
						PARAMETER_TYPE	parameterType,
						void			*pBuffer,
						int				bufferSize

	Return Value:       error status

    Remarks:			Similar to GetSensorParameter but allows user
						to set (write) the parameter.

*/
ATC3DGm_API int SetSensorParameter(
	USHORT						sensorID,
	enum SENSOR_PARAMETER_TYPE	parameterType,
	void						*pBuffer,
	int							bufferSize
	);


/*
	GetTransmitterParameter

	Parameters Passed:	USHORT			transmitterID,
						PARAMETER_TYPE	parameterType,
						void			*pBuffer,
						int				bufferSize

	Return Value:       error status

    Remarks:			Same as Sensor command            

*/
ATC3DGm_API int GetTransmitterParameter(
	USHORT							transmitterID,
	enum TRANSMITTER_PARAMETER_TYPE	parameterType,
	void							*pBuffer,
	int								bufferSize
	);




/*
	SetTransmitterParameter

	Parameters Passed:  USHORT			transmitterID,
						PARAMETER_TYPE	parameterType,
						void			*pBuffer,
						int				bufferSize

	Return Value:       error status

    Remarks:			Same as sensor command            

*/
ATC3DGm_API int SetTransmitterParameter(
	USHORT							transmitterID,
	enum TRANSMITTER_PARAMETER_TYPE	parameterType,
	void							*pBuffer,
	int								bufferSize
	);

/*
	GetBIRDError

	Parameters Passed:  none

	Return Value:       error status

    Remarks:			Returns next error in queue 
						if available

*/
ATC3DGm_API int GetBIRDError(
	void
	);

#define GetPOSTError	GetBIRDError
#define GetDIAGError	GetBIRDError

/*
	GetErrorText

	Parameters Passed:  int errorCode
						char *pBuffer
						int bufferSize
						int type

	Return Value:       error status as a text string

    Remarks:			ErrorCode contains the error code returned from 
						either a command or in response to GetBIRDError
						and which is to be described by a text string.
						Pass a pointer pBuffer to a buffer to contain
						the result of the command. The size of the
						buffer is contained in bufferSize. The type
						parameter is an enumerated constant of
						the type MESSAGE_TYPE.

*/
ATC3DGm_API int GetErrorText(
	int					errorCode,
	char				*pBuffer,
	int					bufferSize,
	enum MESSAGE_TYPE	type
	);

/*

*/
ATC3DGm_API DEVICE_STATUS GetSensorStatus(
	USHORT sensorID
	);

/*

*/
ATC3DGm_API DEVICE_STATUS GetTransmitterStatus(
	USHORT transmitterID
	);

/*

*/
ATC3DGm_API DEVICE_STATUS GetBoardStatus(
	USHORT boardID
	);

/*

*/
ATC3DGm_API DEVICE_STATUS GetSystemStatus(
	// no id required
	);

/*

*/
ATC3DGm_API int SaveSystemConfiguration(
	LPCSTR	lpFileName
	);

/*

*/
ATC3DGm_API int RestoreSystemConfiguration(
	LPCSTR	lpFileName
	);

/*

*/
ATC3DGm_API int GetBoardParameter(
	USHORT						boardID,
	enum BOARD_PARAMETER_TYPE	parameterType,
	void						*pBuffer,
	int							bufferSize
	);

/*

*/
ATC3DGm_API int SetBoardParameter(
	USHORT						boardID,
	enum BOARD_PARAMETER_TYPE	parameterType,
	void						*pBuffer,
	int							bufferSize
	);

/*

*/
ATC3DGm_API int CloseBIRDSystem(void);

#endif // ATC3DGm_H

