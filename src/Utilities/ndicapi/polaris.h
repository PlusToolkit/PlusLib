/*=======================================================================

  Program:   NDI Combined API C Interface Library
  Module:    $RCSfile: polaris.h,v $
  Creator:   David Gobbi <dgobbi@atamai.com>
  Language:  C++
  Author:    $Author: dgobbi $
  Date:      $Date: 2005/07/01 22:52:05 $
  Version:   $Revision: 1.2 $

==========================================================================

Copyright (c) 2000-2005 Atamai, Inc.

Use, modification and redistribution of the software, in source or
binary forms, are permitted provided that the following terms and
conditions are met:

1) Redistribution of the source code, in verbatim or modified
   form, must retain the above copyright notice, this license,
   the following disclaimer, and any notices that refer to this
   license and/or the following disclaimer.  

2) Redistribution in binary form must include the above copyright
   notice, a copy of this license and the following disclaimer
   in the documentation or with other materials provided with the
   distribution.

3) Modified copies of the source code must be clearly marked as such,
   and must not be misrepresented as verbatim copies of the source code.

THE COPYRIGHT HOLDERS AND/OR OTHER PARTIES PROVIDE THE SOFTWARE "AS IS"
WITHOUT EXPRESSED OR IMPLIED WARRANTY INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE.  IN NO EVENT SHALL ANY COPYRIGHT HOLDER OR OTHER PARTY WHO MAY
MODIFY AND/OR REDISTRIBUTE THE SOFTWARE UNDER THE TERMS OF THIS LICENSE
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, LOSS OF DATA OR DATA BECOMING INACCURATE
OR LOSS OF PROFIT OR BUSINESS INTERRUPTION) ARISING IN ANY WAY OUT OF
THE USE OR INABILITY TO USE THE SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGES.

=======================================================================*/

/*! \file polaris.h
  This file contains a complete C interface to the POLARIS API.
*/

#ifndef POLARIS_H
#define POLARIS_H 1

#define POLARIS_MAJOR_VERSION 3
#define POLARIS_MINOR_VERSION 2

#include "PlusConfigure.h"

#include "ndicapi.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

typedef struct ndicapi polaris;

/*=====================================================================*/
/*! \defgroup PolarisMethods Polaris Basic Methods 

   These are the primary set of methods for communicating with the POLARIS.
*/


/*! \ingroup PolarisMethods
  This function will return the name of the \em i th serial port device,
  i.e. \em i = 0 gives "COM1:" on Windows and "/dev/ttyS0" on linux.

  \param i  integer between 0 and 3

  \return   the name of a serial port device or 0

  If \em i is too large, the return value is zero.
*/
static char *plDeviceName(int i) { return ndiDeviceName(i); }

/*! \ingroup PolarisMethods
  Probe for the polaris on the specified device.  This will take the
  following steps:
  -# save the status of the device (baud rate etc)
  -# open the device at 9600 baud, 8 data bits, no parity, 1 stop bit
  -# send an "INIT:" command and check for the expected reply
  -# if the "INIT:" failed, send a serial break and re-try the "INIT:"
  -# if the "INIT:" succeeds, send "VER:0" and check for "Polaris"
  -# restore the device to its previous state and close the device.
 
  \param device  name of a valid serial port device
  
  \return one of:
  - PL_OKAY - probe was successful
  - PL_OPEN_ERROR   -  couldn't open the device
  - PL_BAD_COMM     -  device file is probably not a serial port
  - PL_WRITE_ERROR  -  I/O error while writing to the device
  - PL_READ_ERROR   -  I/O error while reading from device
  - PL_TIMEOUT      -  timeout while waiting for data
  - PL_PROBE_FAIL   -  POLARIS not found on this serial port
*/
static int plProbe(const char *device) { return ndiProbe(device); }

/*! \ingroup PolarisMethods
  Open communication with the POLARIS on the specified device.  This also
  sets serial port parameters to (9600 baud, 8N1, no handshaking) which
  are the defaults for the POLARIS.

  \param device  name of a valid serial port device
  
  \return  pointer to a polaris object, or zero if device couldn't be opened
*/
static polaris *plOpen(const char *device) { return ndiOpen(device); }

/*! \ingroup PolarisMethods
  Close communication with the POLARIS.  You should send a "COMM:00000"
  command to the POLARIS before you close communication so that you
  can resume communication on a subsequent call to plOpen() without
  having to reset the POLARIS.
*/
static void plClose(polaris *pol) { ndiClose(pol); }

/*! \ingroup PolarisMethods
  Set up multithreading to increase efficiency.

  \param pol    pointer to a valid POLARIS object
  \param mode   1 to turn on threading, 0 to turn off threading

  It can take milliseconds or even tens of milliseconds for the POLARIS
  to return the transform data after a GX command.  During this time,
  the application normally sits idle.

  One way to avoid this idle time is to spawn a separate thread to send
  the GX commands to the POLARIS and wait for the reply.  Then the
  application can continue to execute and simply pick up the GX reply
  when the POLARIS has finished sending it.

  When the thread mode is set, every time the application sends a GX
  command to the POLARIS during tracking mode the spawned thread will
  automatically begin resending the GX command as fast as possible.  
  Then when the application sends the next GX command, the thread will
  simply send the application the most recent reply instead of forcing
  the application to wait through a full command/reply cycle.

  If the application changes the reply mode for the GX command, then
  the thread will begin sending GX commands with the new reply format.
*/
static void plSetThreadMode(polaris *pol, int mode) { ndiSetThreadMode(pol,mode); }

/*! \ingroup PolarisMethods
  Send a command to the POLARIS using a printf-style format string.

  \param pol    pointer to a valid POLARIS object
  \param format a printf-style format string
  \param ...    format arguments as per the format string

  \return       the text reply from POLARIS with CRC chopped off

  The standard format of a POLARIS command is, for example, "INIT:" or
  "PENA:AD".  A CRC value and a carriage return will be appended to the
  command before it is sent to the POLARIS.
  
  This function will automatically recogize certain commands and behave
  accordingly:
  - NULL - A serial break will be sent to the POLARIS. 
  - "COMM:" - After the COMM is sent, the host computer serial port is
           adjusted to match the POLARIS.
  - "INIT:" - After the INIT is sent, communication will be paused
           for 100ms.
  - "GX:"   - The information returned by the GX command is stored and can
           be retrieved though the plGetGX() functions.
  - "PSTAT:" - The information returned by the PSTAT command is stored and
           can be retrieved through one of the plGetPSTAT() functions.
  - "SSTAT:" - The information returned by the SSTAT command is stored and
           can be retrieved through one of the plGetSSTAT() functions.    
  - "IRCHK:" - The information returned by the IRCHK command is stored and
           can be retrieved through the plGetIRCHK() functions.

  <p>The plGetError() function can be used to check whether an error
  occured or, alternatively, plSetErrorCallback() can be used to set
  a function that will be called whenever an error occurs.

  For convenience, there is a set of macros for sending commands to the
  POLARIS.  These are listed in \ref PolarisMacros.
*/
static char *plCommand(polaris *pol, const char *format, ...) {
  char *reply; va_list ap; va_start(ap,format);
  reply = ndiCommandVA(pol, format, ap); va_end(ap); return reply; }

/*! \ingroup PolarisMethods
  This function is identical in behaviour to plCommand(), except that
  it accepts a va_list instead of an argument list.  The use of va_list
  is described in the standard C header file "stdarg.h".
*/
static char *plCommandVA(polaris *pol, const char *format, va_list ap) {
  return ndiCommandVA(pol, format, ap); }

/*! \ingroup PolarisMethods
  Error callback type for use with plSetErrorCallback().
*/
typedef NDIErrorCallback PLErrorCallback;

/*! \ingroup PolarisMethods
  Set a function that will be called each time an error occurs.

  \param pol       pointer to a valid POLARIS object
  \param callback  a callback with the following signature:\n
    void callback(int errnum, char *error_description, void *userdata)
  \param userdata  data to send to the callback each time it is called
*/
static void plSetErrorCallback(polaris *pol,
                               PLErrorCallback callback,
                               void *userdata) {
  ndiSetErrorCallback(pol, callback, userdata); }

/*=====================================================================*/
/*! \defgroup PolarisMacros Polaris Command Macros 
  These are a set of macros that send commands to the POLARIS via
  plCommand().

  The plGetError() function can be used to determine whether an error
  occurred.

  The return value is a terminated string containing the reply from the
  POLARIS with the CRC and final carriage return chopped off.
*/

/*\{*/
/*!
  Send a serial break to reset the POLARIS.  If the reset was not
  successful, the error code will be set to PL_RESET_FAIL.
*/
#define plRESET(p) plCommand((p),NULL)

/*!
  Initialize the POLARIS.  The POLARIS must be initialized before any
  other commands are sent.
*/
#define plINIT(p) plCommand((p),"INIT:")

/*!
  Change the POLARIS communication parameters.  The host parameters will
  automatically be adjusted to match.  If the specified baud rate is
  not supported by the serial port, then the error code will be set to
  PL_BAD_COMM and the POLARIS will have to be reset before communication
  can continue.  Most modern UNIX systems accept all baud rates except 14400,
  and Windows systems support all baud rates.

  \param baud one of PL_9600, PL_14400, PL_19200, PL_38400, PL_57600, PL_115200
  \param dps  should usually be PL_8N1, the most common mode
  \param h   one of PL_HANDSHAKE or PL_NOHANDSHAKE
*/
#define plCOMM(p,baud,dps,h) plCommand((p),"COMM:%d%03d%d",(baud),(dps),(h))

/*!
  Write to a virtual SROM address on the specified port.
  The plPVWRFromFile() function provides a more convenient means
  of uploading tool descriptions.

  \param port one of '1', '2', '3' or 'A' to 'I'
  \param a an address between 0 and 960
  \param x 64-byte data array encoded as a 128-character hexidecimal string

  The plHexEncode() function can be used to encode the data into
  hexidecimal.
*/
#define plPVWR(p,port,a,x) plCommand((p),"PVWR:%c%04X%.128s",(port),(a),(x))

/*!
  Clear the virtual SROM for the specified port.  For a passive tool,
  this is equivalent to unplugging the tool.

  \param port one of '1', '2', '3' or 'A' to 'I'
*/
#define plPVCLR(p,port) plCommand((p),"PVCLR:%c",(port))

/*!
  Initialize the specified port.

  \param port one of '1', '2', '3' or 'A' to 'I'
*/
#define plPINIT(p,port) plCommand((p),"PINIT:%c",(port))

/*!
  Enable transform reporting on the specified port.

  \param port one of '1', '2', '3' or 'A' to 'I'
  \param mode one of PL_STATIC 'S', PL_DYNAMIC 'D' or PL_BUTTON_BOX 'B'
*/
#define plPENA(p,port,mode) plCommand((p),"PENA:%c%c",(port),(mode))

/*!
  Disable transform reporting on the specified port.

  \param port one of '1', '2', '3' or 'A' to 'I'  
*/
#define plPDIS(p,port) plCommand((p),"PDIS:%c",(port))

/*!
  Put the POLARIS into tracking mode.
*/
#define plTSTART(p) plCommand((p),"TSTART:")

/*!
  Take the POLARIS out of tracking mode.
*/
#define plTSTOP(p) plCommand((p),"TSTOP:")

/*!
  Request tracking information from the POLARIS.  This command is
  only available in tracking mode.

  \param mode a reply mode containing the following bits:
  - PL_XFORMS_AND_STATUS  0x0001 - transforms and status
  - PL_ADDITIONAL_INFO    0x0002 - additional tool transform info
  - PL_SINGLE_STRAY       0x0004 - stray active marker reporting
  - PL_FRAME_NUMBER       0x0008 - frame number for each tool
  - PL_PASSIVE            0x8000 - include information for ports 'A', 'B', 'C'
  - PL_PASSIVE_EXTRA      0x2000 - with PL_PASSIVE, ports 'A' to 'I'
  - PL_PASSIVE_STRAY      0x1000 - stray passive marker reporting

  <p>The GX command with the appropriate reply mode is used to update the
  data that is returned by the following functions:
  - int \ref plGetGXTransform(polaris *pol, int port, double transform[8])
  - int \ref plGetGXPortStatus(polaris *pol, int port)
  - int \ref plGetGXSystemStatus(polaris *pol)
  - int \ref plGetGXToolInfo(polaris *pol, int port)
  - int \ref plGetGXMarkerInfo(polaris *pol, int port, int marker)
  - int \ref plGetGXSingleStray(polaris *pol, int port, double coord[3])
  - unsigned long \ref plGetGXFrame(polaris *pol, int port)
  - int \ref plGetGXNumberOfPassiveStrays(polaris *pol)
  - int \ref plGetGXPassiveStray(polaris *pol, int i, double coord[3])
*/
#define plGX(p,mode) plCommand((p),"GX:%04X",(mode))

/*!
  Set a tool LED to a particular state.

  \param port   one of '1', '2' or '3' (active ports only)
  \param led    an integer between 1 and 3
  \param state  desired state: PL_BLANK ('B'), PL_FLASH 'F' or PL_SOLID 'S'

  This command can be used in tracking mode.
*/
#define plLED(p,port,led,s) plCommand((p),"LED:%c%d%c",(port),(led),(s))

/*!
  Cause the POLARIS to beep.

  \param n   the number of times to beep, an integer between 1 and 9

  A reply of "0" means that the POLARIS is already beeping and cannot
  service this beep request.

  This command can be used in tracking mode.
*/
#define plBEEP(p,n) plCommand((p),"BEEP:%i",(n))

/*!
  Get a string that describes the POLARIS firmware version.

  \param n   the processor to get the firmware revision of:
  - 0 - control firmware
  - 1 - left sensor firmware
  - 2 - right sensor firmware
  - 3 - TIU firmware
  - 4 - control firmware with enhanced versioning
*/
#define plVER(p,n) plCommand((p),"VER:%d",(n))

/*!
  Get a feature list for this POLARIS.

  \param mode  the desired reply mode
  - 0x00 - 32-bit feature summary encoded as 8 hexidecimal digits
  - 0x01 - the number of active tool ports as a single digit
  - 0x02 - the number of passive tool ports as a single digit
  - 0x03 - list of volumes available (see NDI documentation)
  - 0x04 - the number of ports that support tool-in-port current sensing

  <p>The feature summary bits are defined as follow:
  - 0x00000001 - active tool ports are available
  - 0x00000002 - passive tool ports are available
  - 0x00000004 - multiple volumes are available
  - 0x00000008 - tool-in-port current sensing is available
*/
#define plSFLIST(p,mode) plCommand((p),"SFLIST:%02X",(mode))

/*!
  Select from the different calibrated operating volumes available to
  the POLARIS.

  \param n  the volume to select, see plSFLIST()
*/
#define plVSEL(p,n) plCommand((p),"VSEL:%d",(n))

/*!
  Ask the POLARIS for information about the tool ports.

  \param format  a reply format mode composed of the following bits:
  - PL_BASIC           0x0001 - get port status and basic tool information
  - PL_TESTING         0x0002 - get current test for active tools
  - PL_PART_NUMBER     0x0004 - get a 20 character part number
  - PL_ACCESSORIES     0x0008 - get a summary of the tool accessories
  - PL_MARKER_TYPE     0x0010 - get the tool marker type
  - PL_PASSIVE         0x8000 - include information for ports 'A', 'B', 'C'
  - PL_PASSIVE_EXTRA   0x2000 - with PL_PASSIVE, ports 'A' to 'I'

  <p>The use of the PSTAT command with the appropriate reply format updates
  the information returned by the following commands:
  - int \ref plGetPSTATPortStatus(polaris *pol, int port)
  - int \ref plGetPSTATToolInfo(polaris *pol, int port,
                                   char information[30])
  - unsigned long \ref plGetPSTATCurrentTest(polaris *pol, int port)
  - int \ref plGetPSTATAccessories(polaris *pol, int port)
  - int \ref plGetPSTATMarkerType(polaris *pol, int port)

  <p>This command is not available during tracking mode.
*/
#define plPSTAT(p,format) plCommand((p),"PSTAT:%04X",(format))

/*!
  Request status information from the POLARIS.

  \param format  a reply format mode composed of the following bits:
  - PL_CONTROL  0x0001 - control processor information
  - PL_SENSORS  0x0002 - sensor processors
  - PL_TIU      0x0004 - TIU processor

  <p>The use of the SSTAT command with the appropriate reply format updates
  the information returned by the following commands:
  - int \ref plGetSSTATControl(polaris *pol)
  - int \ref plGetSSTATSensors(polaris *pol)
  - int \ref plGetSSTATTIU(polaris *pol)

  <p>This command is not available during tracking mode.
*/
#define plSSTAT(p,format) plCommand((p),"SSTAT:%04X",(format))

/*! Read the tool SROM. Use plHexDecode() to convert reply to binary. */
#define plPPRD(p,port,addr) plCommand((p),"PPRD:%c%04X",(port),(addr))

/*! Write the tool SROM. Use plHexEncode() to convert data to a hex string. */
#define plPPWR(p,port,a,x) plCommand((p),"PPWR:%c%04X%.128s",(port),(a),(x))

/*! Read the user part of the tool SROM. Use plHexDecode() convert reply. */
#define plPURD(p,port,addr) plCommand((p),"PURD:%c%04X",(port),(addr))

/*! Write the user part of the tool SROM. Use plHexEncode() to convert data. */
#define plPUWR(p,port,a,x) plCommand((p),"PPWR:%c%04X%.128s",(port),(a),(x))

/*! Select an SROM target by ID. */
#define plPSEL(p,port,id) plCommand((p),"PSEL:%c%s",(port),(id))

/*! Search for available SROM targets. */
#define plPSRCH(p,port) plCommand((p),"PSRCH:%c",(port))

/*! Set the virtual TOOL IN PORT and non-POLARIS tool signals for this port */
#define plPVTIP(p,port,tip,np) plCommand((p),"PVTIP:%c%d%d",(port),(tip),(np))

/*! Set default tool configuration prior to testing. */
#define plTTCFG(p,port) plCommand((p),"TTCFG:%c",(port))

/*! Perform a current test on this port.  */
#define plTCTST(p,port) plCommand((p),"TCTST:%c",(port))

/*!
  Put the POLARIS into diagnostic mode.
*/
#define plDSTART(p) plCommand((p),"DSTART:")

/*!
  Take the POLARIS out of diagnostic mode.
*/
#define plDSTOP(p) plCommand((p),"DSTOP:")

/*!
  Initialize the diagnostic environmental infrared checking system.
  This command must be called prior to using the IRCHK command.
*/
#define plIRINIT(p) plCommand((p),"IRINIT:")

/*!
  Check for sources of environmental infrared.  This command is only
  valid in diagnostic mode after an IRINIT command.

  \param mode  reply mode bits:
  - PL_DETECTED   0x0001 - return '1' if IR detected, else '0'
  - PL_SOURCES    0x0002 - locations of up to 20 sources per camera

  <p>The IRCHK command is used to update the information returned by the
  plGetIRCHKDetected() and plGetIRCHKSourceXY() functions.
*/
#define plIRCHK(p,mode) plCommand((p),"IRCHK:%04X",(mode))

/*!
  Set the infrared firing signature for the specified port.  This
  command is only available in diagnostic mode.

  \param port  the active tool port '1', '2' or '3'
  \param sig   a 32-bit integer whose bits specify what IREDS to fire
*/
#define plIRED(p,port,sig) plCommand((p),"IRED:%c%08X",(port),(sig))

/*!
  Get the 3D position and an error value for a single marker on a
  tool.  The mode should be set to 1 or 3.  This command is only
  available in diagnostic mode.

  \param port  the port number: '1', '2' or '3'
  \param mode  see NDI POLARIS documentation

  The IRED command should be used to set up a marker firing signature
  before this command is called.
*/
#define pl3D(p,port,mode) plCommand((p),"3D:%c%d",(port),(mode))

/*! 
  Write data from a ROM file into the virtual SROM for the specified port.

  \param pol       pointer to a valid polaris object
  \param port      one of '1', '2', '3' or 'A' to 'I'
  \param filename  file to read the SROM data from

  \return  PL_OKAY if the write was successful

  If the return value is not PL_OKAY but plGetError() returns PL_OKAY,
  then the ROM file could not be read and no information was written
  to the POLARIS.

  This function uses the PVWR command to write the SROM.  The total size
  of the virtual SROM is 1024 bytes.  If the file is shorter than this,
  then zeros will be written to the remaining space in the SROM.
*/
static int plPVWRFromFile(polaris *pol, int port, char *filename) {
  unsigned char buffer[1024];
  char hexdata[128];
  FILE *file=NULL;
  int addr;

	errno_t err = fopen_s (&file,filename, "rb");	
	if( err != 0)
	{
		LOG_ERROR("Error opening file: " << filename << " Error No.: " << err); 
		return -1;
	} 

  memset(buffer, 0, 1024);      /* clear buffer to zero */
  fread(buffer, 1, 1024, file); /* read at most 1k from file */
  if (ferror(file)) {
    fclose(file);
    return -1;
  }
  fclose(file);

  for (addr = 0; addr < 1024; addr += 64) { /* write in chunks of 64 bytes */
    plPVWR(pol, port, addr, ndiHexEncode(hexdata, &buffer[addr], 64));
    if (ndiGetError(pol) != NDI_OKAY) {
      return -1;
    }
  }
 
  return 0;
}
/*\}*/

/*=====================================================================*/
/*! \defgroup GetMethods Polaris Get Methods
  These functions are used to retrieve information that has been stored in
  the polaris object.  
  None of these functions communicate with the POLARIS itself,
  and none of them cause the error indicator to be set.

  The DeviceName and DeviceHandle are set when the device is
  first opened by plOpen().

  All other information is set by sending the appropriate command to
  the POLARIS through plCommand() or through one of the command macros.
  The information persists until the next time the command is sent.

  For example, if the PSTAT command is sent to the POLARIS then the
  information returned by the methods that start with GetPSTAT will
  be set and will subsequently remain unchanged until the next
  PSTAT command is sent.
*/

/*! \ingroup GetMethods
  Get error code from the last command.  An error code of PL_OKAY signals
  that no error occurred.  The error codes are listed in \ref ErrorCodes.
*/
static int plGetError(polaris *pol) { return ndiGetError(pol); }

/*! \ingroup GetMethods
  Get the name of the serial port device that the POLARIS is attached to.
*/
static char *plGetDeviceName(polaris *pol) { return ndiGetDeviceName(pol); }

typedef NDIFileHandle PLFileHandle;

/*! \ingroup GetMethods
  Get the device handle for the serial port that the POLARIS is attached
  to.  This device handle is the value returned by the UNIX open()
  function or the Win32 CreateFile() function.
*/
static PLFileHandle plGetDeviceHandle(polaris *pol) { return ndiGetDeviceHandle(pol); }

/*! \ingroup GetMethods
  Check the current theading mode.  The default is 0 (no multithreading).
*/
static int plGetThreadMode(polaris *pol) { return ndiGetThreadMode(pol); }

/*! \ingroup GetMethods
  Get the transformation for the specified port.
  The first four numbers are a quaternion, the next three numbers are
  the coodinates in millimetres, and the final number
  is a unitless error estimate.

  \param pol       pointer to a valid polaris object
  \param port      one of '1', '2', '3' or 'A' to 'I'
  \param transform space for the 8 numbers in the transformation
  
  \return one of the following: 
  - PL_OKAY if successful
  - PL_DISABLED if tool port is nonexistent or disabled
  - PL_MISSING if tool transform cannot be computed

  <p>If PL_DISABLED or PL_MISSING is returned, then the values in the
  supplied transform array will be left unchanged.

  The transforms for ports '1', '2', '3' are updated whenever the
  GX comand is sent with the PL_XFORMS_AND_STATUS (0x0001) bit set
  in the reply mode.
  The passive ports 'A', 'B', 'C' are updated if the PL_PASSIVE (0x8000)
  bit is also set, and ports 'A' though 'I' are updated if both
  PL_PASSIVE (0x8000) and PL_PASSIVE_EXTRA (0x2000) are also set.

  The transformation for any particular port will remain unchanged
  until it is updated by a GX command with an appropriate reply mode
  as specified above.
*/ 
static int plGetGXTransform(polaris *pol, int port, double transform[8]) {
  return ndiGetGXTransform(pol, port, transform); }

/*! \ingroup GetMethods
  Get the 8-bit status value for the specified port.

  \param pol       pointer to a valid polaris object
  \param port      one of '1', '2', '3' or 'A' to 'I'

  \return status bits or zero if there is no information:
  - PL_TOOL_IN_PORT        0x01
  - PL_SWITCH_1_ON         0x02
  - PL_SWITCH_2_ON         0x04
  - PL_SWITCH_3_ON         0x08
  - PL_INITIALIZED         0x10
  - PL_ENABLED             0x20
  - PL_OUT_OF_VOLUME       0x40
  - PL_PARTIALLY_IN_VOLUME 0x80

  The status of the ports is updated according to the same rules as
  specified for plGetGXTransform().
*/
static int plGetGXPortStatus(polaris *pol, int port) {
  return ndiGetGXPortStatus(pol, port); }

/*! \ingroup GetMethods
  Get an 8-bit status bitfield for the system.

  \param pol       pointer to a valid polaris object

  \return status bits or zero if there is no information:
  - PL_COMM_SYNC_ERROR            0x01
  - PL_TOO_MUCH_EXTERNAL_INFRARED 0x02
  - PL_COMM_CRC_ERROR             0x04

  <p>The system stutus information is updated whenever the GX command is
  called with the PL_XFORMS_AND_STATUS (0x0001) bit set in the reply mode.
*/
static int plGetGXSystemStatus(polaris *pol) {
  return ndiGetGXSystemStatus(pol); }

/*! \ingroup GetMethods
  Get additional information about the tool transformation.

  \param pol       pointer to a valid polaris object
  \param port      one of '1', '2', '3' or 'A' to 'I'

  \return status bits, or zero if there is no information available
  - PL_BAD_TRANSFORM_FIT   0x01
  - PL_NOT_ENOUGH_MARKERS  0x02
  - PL_TOOL_FACE_USED      0x70 - 3 bits give 8 possible faces

  <p>The tool information is only updated when the GX command is called with
  the PL_ADDITIONAL_INFO (0x0002) mode bit, and then only for the ports
  specified by the PL_PASSIVE (0x8000) and PL_PASSIVE_EXTRA (0x2000) bits.
*/
static int plGetGXToolInfo(polaris *pol, int port) {
  return ndiGetGXToolInfo(pol, port); }

/*! \ingroup GetMethods
  Get additional information about the tool markers.

  \param pol       pointer to a valid polaris object
  \param port      one of '1', '2', '3' or 'A' to 'I'
  \param marker    one of 'A' through 'T' for the 20 markers

  \return status bits, or zero if there is no information available
  - PL_MARKER_MISSING             0
  - PL_MARKER_EXCEEDED_MAX_ANGLE  1
  - PL_MARKER_EXCEEDED_MAX_ERROR  2
  - PL_MARKER_USED                3  

  <p>The tool marker information is only updated when the GX command is
  called with the PL_ADDITIONAL_INFO (0x0002) mode bit set, and then only
  for the ports specified by the PL_PASSIVE (0x8000) and
  PL_PASSIVE_EXTRA (0x2000) bits.
*/
static int plGetGXMarkerInfo(polaris *pol, int port, int marker) {
  return ndiGetGXMarkerInfo(pol, port, marker); }

/*! \ingroup GetMethods
  Get the coordinates of a stray marker on an active tool.
  This command is only meaningful for active tools that have a stray
  marker.

  \param pol       pointer to a valid polaris object
  \param port      one of '1', '2', '3'
  \param coord     array to hold the three coordinates

  \return the return value will be one of
  - PL_OKAY - values returned in coord
  - PL_DISABLED - port disabled or illegal port specified
  - PL_MISSING - stray marker is not visible to the POLARIS

  <p>The stray marker position is only updated when the GX command is
  called with the PL_SINGLE_STRAY (0x0004) bit set.
*/
static int plGetGXSingleStray(polaris *pol, int port, double coord[3]) {
  return ndiGetGXSingleStray(pol, port, coord); }

/*! \ingroup GetMethods
  Get the camera frame number for the latest transform.

  \param pol       pointer to a valid polaris object
  \param port      one of '1', '2', '3' (active ports only)

  \return a 32-bit frame number, or zero if no information was available

  The frame number is only updated when the GX command is called with
  the PL_FRAME_NUMBER (0x0008) bit, and then only for the ports specified
  by the PL_PASSIVE (0x8000) and PL_PASSIVE_EXTRA (0x2000) bits.
*/
static unsigned long plGetGXFrame(polaris *pol, int port) {
  return ndiGetGXFrame(pol, port); }

/*! \ingroup GetMethods
  Get the number of passive stray markers detected.

  \param pol       pointer to a valid polaris object
  \return          a number between 0 and 20
  
  The passive stray information is updated when a GX command is sent
  with the PL_PASSIVE_STRAY (0x1000) and PL_PASSIVE (0x8000) bits set
  in the reply mode.  The information persists until the next time GX is
  sent with these bits set.

  If no information is available, the return value is zero.
*/
static int plGetGXNumberOfPassiveStrays(polaris *pol) {
  return ndiGetGXNumberOfPassiveStrays(pol); }

/*! \ingroup GetMethods
  Copy the coordinates of the specified stray marker into the
  supplied array.

  \param pol       pointer to a valid polaris object
  \param i         a number between 0 and 19
  \param coord     array to hold the coordinates
  \return          one of:
  - PL_OKAY - information was returned in coord
  - PL_DISABLED - no stray marker reporting is available
  - PL_MISSING - marker number i is not visible

  <p>Use plGetGXNumberOfPassiveStrays() to get the number of stray
  markers that are visible.
  
  The passive stray marker coordinates are updated when a GX command
  is sent with the PL_PASSIVE_STRAY (0x1000) and PL_PASSIVE (0x8000)
  bits set in the reply mode.  The information persists until the next
  time GX is sent with these bits set.
*/
static int plGetGXPassiveStray(polaris *pol, int i, double coord[3]) {
  return ndiGetGXPassiveStray(pol, i, coord); }

/*! \ingroup GetMethods
  Get the 8-bit status value for the specified port.

  \param pol       pointer to a valid polaris object
  \param port      one of '1', '2', '3' or 'A' to 'I'

  \return a an integer composed of the following bits:
  - PL_TOOL_IN_PORT        0x01 - there is a tool in the port
  - PL_SWITCH_1_ON         0x02 - button 1 is pressed
  - PL_SWITCH_2_ON         0x04 - button 2 is pressed
  - PL_SWITCH_3_ON         0x08 - button 3 is pressed
  - PL_INITIALIZED         0x10 - tool port has been initialized
  - PL_ENABLED             0x20 - tool port is enabled for tracking
  - PL_CURRENT_DETECT      0x80 - tool sensed through current detection

  <p>If there is no available information for the specified port, or if
  an illegal port specifier value is used, the return value is zero.
  The return value is updated only when a PSTAT command is sent with
  the PL_BASIC (0x0001) bit set in the reply mode.
*/
static int plGetPSTATPortStatus(polaris *pol, int port) {
  return ndiGetPSTATPortStatus(pol, port); }

/*! \ingroup GetMethods
  Get a 30-byte string describing the tool in the specified port.

  \param pol         pointer to a valid polaris object
  \param port        one of '1', '2', '3' or 'A' to 'I'
  \param information array that information is returned in (the
                     resulting string is not null-terminated)

  \return  one of:
  - PL_OKAY - information was returned
  - PL_UNOCCUPIED - port is unoccupied or no information is available

  <p>The returned string will not be null-terminated by default.  You
  must set information[30] to 0 in order to terminate the string.
  If the port is unoccupied then the \em information string will read
  "UNOCCUPIED".
  
  The information is updated only when a PSTAT command is sent with
  the PL_BASIC (0x0001) bit set in the reply mode.
*/
static int plGetPSTATToolInfo(polaris *pol, int port, char information[30]) {
  return ndiGetPSTATToolInfo(pol, port, information); }

/*! \ingroup GetMethods
  Return the results of a current test on the IREDS on the specified
  tool.  

  \param  pol         pointer to a valid polaris object
  \param  port        one of '1', '2', '3'

  \return 32-bit integer (see NDI POLARIS documentation)

  The information is updated only when a PSTAT command is sent with
  the PL_TESTING (0x0002) bit set in the reply mode.
*/
static unsigned long plGetPSTATCurrentTest(polaris *pol, int port) {
  return ndiGetPSTATCurrentTest(pol, port); }

/*! \ingroup GetMethods
  Get a 20-byte string that contains the part number of the tool.

  \param pol         pointer to a valid polaris object
  \param port        one of '1', '2', '3' or 'A' to 'I'
  \param part        array that part number is returned in (the
                     resulting string is not null-terminated)

  \return  one of:
  - PL_OKAY - information was returned
  - PL_UNOCCUPIED - port is unoccupied or no information is available

  <p>If a terminated string is required, then set part[20] to '\0'
  before calling this function.

  The information is updated only when a PSTAT command is sent with
  the PL_PART_NUMBER (0x0004) bit set in the reply mode.
*/
static int plGetPSTATPartNumber(polaris *pol, int port, char part[20]) {
  return ndiGetPSTATPartNumber(pol, port, part); }

/*! \ingroup GetMethods
  Get the 8-bit value specifying the accessories for the specified tool.

  \param pol       pointer to a valid polaris object
  \param port      one of '1', '2', '3' (active ports only)

  \return a an integer composed of the following bits:
  - PL_TOOL_IN_PORT_SWITCH   0x01  - tool has tool-in-port switch
  - PL_SWITCH_1              0x02  - tool has button 1
  - PL_SWITCH_2              0x04  - tool has button 2
  - PL_SWITCH_3              0x08  - tool has button 3
  - PL_TOOL_TRACKING_LED     0x10  - tool has tracking LED
  - PL_LED_1                 0x20  - tool has LED 1
  - PL_LED_2                 0x40  - tool has LED 2
  - PL_LED_3                 0x80  - tool has LED 3

  <p>If there is no available information for the specified port, or if
  an illegal port specifier value is used, the return value is zero.
  The return value is always zero for passive ports.

  The return value is updated only when a PSTAT command is sent with
  the PL_ACCESSORIES (0x0008) bit set in the reply mode.
*/
static int plGetPSTATAccessories(polaris *pol, int port) {
  return plGetPSTATAccessories(pol, port); }

/*! \ingroup GetMethods
  Get an 8-bit value describing the marker type for the tool.
  The low three bits descibe the wavelength, and the high three
  bits are the marker type code.

  \param pol       pointer to a valid polaris object
  \param port      one of '1', '2', '3' (active ports only)

  \return  see NDI POLARIS documentation for more information:
  - low bits:
    - PL_950NM            0x00
    - PL_850NM            0x01
  - high bits:
    - PL_NDI_ACTIVE       0x08
    - PL_NDI_CERAMIC      0x10
    - PL_PASSIVE_ANY      0x20
    - PL_PASSIVE_SPHERE   0x28
    - PL_PASSIVE_DISC     0x30

  <p>If there is no available information for the specified port, or if
  an illegal port specifier value is used, the return value is zero.

  The return value is updated only when a PSTAT command is sent with
  the PL_MARKER_TYPE (0x0010) bit set in the reply mode.
*/
static int plGetPSTATMarkerType(polaris *pol, int port) {
  return ndiGetPSTATMarkerType(pol, port); }

/*! \ingroup GetMethods
  Get the status of the control processor.

  \param pol       pointer to a valid polaris object

  \return an int with the following bit definitions for errors:
  - PL_EPROM_CODE_CHECKSUM     0x01
  - PL_EPROM_SYSTEM_CHECKSUM   0x02

  <p>This information is updated only when the SSTAT command is sent
  with the PL_CONTROL (0x0001) bit set in the reply mode.
*/
static int plGetSSTATControl(polaris *pol) {
  return ndiGetSSTATControl(pol); }

/*! \ingroup GetMethods
  Get the status of the sensor processors.

  \param pol       pointer to a valid polaris object

  \return an int with the following bit definitions for errors:
  - PL_LEFT_ROM_CHECKSUM   0x01
  - PL_LEFT_SYNC_TYPE_1    0x02
  - PL_LEFT_SYNC_TYPE_2    0x04
  - PL_RIGHT_ROM_CHECKSUM  0x10
  - PL_RIGHT_SYNC_TYPE_1   0x20
  - PL_RIGHT_SYNC_TYPE_2   0x40

  <p>This information is updated only when the SSTAT command is sent
  with the PL_SENSORS (0x0002) bit set in the reply mode.
*/
static int plGetSSTATSensors(polaris *pol) {
  return ndiGetSSTATSensors(pol); }

/*! \ingroup GetMethods
  Get the status of the sensor processors.

  \param pol       pointer to a valid polaris object

  \return an int with the following bit definitions for errors:
  - PL_ROM_CHECKSUM        0x01
  - PL_OPERATING_VOLTAGES  0x02
  - PL_MARKER_SEQUENCING   0x04
  - PL_SYNC                0x08
  - PL_COOLING_FAN         0x10
  - PL_INTERNAL_ERROR      0x20

  <p>This information is updated only when the SSTAT command is sent
  with the PL_TIU (0x0004) bit set in the reply mode.
*/
static int plGetSSTATTIU(polaris *pol) {
  return ndiGetSSTATTIU(pol); }

/*! \ingroup GetMethods
  Check to see whether environmental infrared was detected.
 
  \param pol    pointer to a valid polaris object
  
  \return       1 if infrared was detected and 0 otherwise.

  This information is only updated if the IRCHK command is called
  with the PL_DETECTED (0x0001) format bit set.
*/
static int plGetIRCHKDetected(polaris *pol) {
  return ndiGetIRCHKDetected(pol); }

/*! \ingroup GetMethods
  Get the number of infrared sources seen by one of the two sensors.

  \param pol    pointer to a valid polaris object
  \param side   one of PL_LEFT or PL_RIGHT

  return  the number of infrared sources seen by the specified sensor

  This information is valid only immediately after the IRCHK command
  has been called with the PL_SOURCES (0x0002) format bit set.  Otherwise,
  the return value will be zero.
*/
static int plGetIRCHKNumberOfSources(polaris *pol, int side) {
  return ndiGetIRCHKNumberOfSources(pol, side); }

/*! \ingroup GetMethods
  Get the coordinates of one of the infrared sources seen by one of
  the two sensors. 

  \param pol    pointer to a valid polaris object
  \param side   one of PL_LEFT or PL_RIGHT
  \param xy     space to store the returned coordinates

  return  PL_OKAY or PL_MISSING

  If there is no available information, then the xy array will be left
  unchanged and the value PL_MISSING will be returned.  Otherwise,
  the return value will be PL_OKAY.

  This information is valid only immediately after the IRCHK command
  has been called with the PL_SOURCES (0x0002) format bit set.
*/
static int plGetIRCHKSourceXY(polaris *pol, int side, int i, double xy[2]) {
  return ndiGetIRCHKSourceXY(pol, side, i, xy); }

/*=====================================================================*/
/*! \defgroup ConversionFunctions Conversion Functions
  These are a set of helper functions for doing miscellaneous conversions
  to and from ascii.
*/

/*! \ingroup ConversionFunctions
  Convert an error code returned by plGetError() into a string that
  describes the error.  The error codes are listed in \ref ErrorCodes.

  An unrecognized error code will return "Unrecognized error code".
*/
static char *plErrorString(int errnum) {
  return ndiErrorString(errnum); }

/*! \ingroup ConversionFunctions
  Convert \em n characters of a hexidecimal string into an unsigned long.
  The conversion halts if a non-hexidecimal digit is found.

  The primary use of this function is decoding replies from the
  POLARIS.
*/
static unsigned long plHexToUnsignedLong(const char *cp, int n) {
  return ndiHexToUnsignedLong(cp, n); }

/*! \ingroup ConversionFunctions
  Convert \em n characters of a signed decimal string to a long.
  The first character in the string must be '+' or '-', otherwise
  the result will be zero.

  The primary use of this function is decoding replies from the
  POLARIS.
*/
static long plSignedToLong(const char *cp, int n) {
  return ndiSignedToLong(cp, n); }

/*! \ingroup ConversionFunctions
  This function is used to convert raw binary data into a stream of
  hexidecimal digits that can be sent to the POLARIS.
  The length of the output string will be twice the number of bytes
  in the input data, since each byte will be represented by two
  hexidecimal digits.

  As a convenience, the return value is a pointer to the hexidecimal
  string.  If the string must be terminated, then set cp[2*n] to '\0'
  before calling this function, otherwise the string will be left
  unterminated.
*/
static char *plHexEncode(char *cp, const void *data, int n) {
  return ndiHexEncode(cp, data, n); }

/*! \ingroup ConversionFunctions
  This function converts a hex-encoded string into binary data.
  This can be used to decode the SROM data sent from the POLARIS.
  The length of the input string must be twice the expected number
  of bytes in the output data, since each binary byte is ecoded by
  two hexidecimal digits.

  As a convenience, the return value is a pointer to the decoded data.
*/
static void *plHexDecode(void *data, const char *cp, int n) {
  return ndiHexDecode(data, cp, n); }

/*=====================================================================*/
/*! \defgroup ErrorCodes Polaris Error Codes 
  The POLARIS error code is set only by plCommand() or by 
  macros and functions that call plCommand().

  Error codes that equal to or less than 0xff are error codes reported
  by the POLARIS itself.  Error codes greater than 0xff are errors that
  are reported by the host computer.

  The error code is returned by plGetError() and the corresponding
  text is available by passing the code to plErrorString().
*/

/*\{*/
#define PL_OKAY            0x00  /*!<\brief No error */     
#define PL_INVALID         0x01  /*!<\brief Invalid command */
#define PL_TOO_LONG        0x02  /*!<\brief Command too long */
#define PL_TOO_SHORT       0x03  /*!<\brief Command too short */
#define PL_BAD_COMMAND_CRC 0x04  /*!<\brief Bad CRC calculated for command */
#define PL_INTERN_TIMEOUT  0x05  /*!<\brief Timeout on command execution */
#define PL_COMM_FAIL       0x06  /*!<\brief New communication parameters failed */
#define PL_PARAMETERS      0x07  /*!<\brief Incorrect number of command parameters */
#define PL_INVALID_PORT    0x08  /*!<\brief Invalid port selected */
#define PL_INVALID_MODE    0x09  /*!<\brief Invalid mode selected */
#define PL_INVALID_LED     0x0a  /*!<\brief Invalid LED selected */
#define PL_LED_STATE       0x0b  /*!<\brief Invalid LED state selected */
#define PL_BAD_MODE        0x0c  /*!<\brief Command invalid for current mode */
#define PL_NO_TOOL         0x0d  /*!<\brief No tool plugged in selected port */
#define PL_PORT_NOT_INIT   0x0e  /*!<\brief Selected port not initialized */
#define PL_PORT_DISABLED   0x0f  /*!<\brief Selected port not enabled */
#define PL_INITIALIZATION  0x10  /*!<\brief System not initialized */
#define PL_TSTOP_FAIL      0x11  /*!<\brief Failure to stop tracking */
#define PL_TSTART_FAIL     0x12  /*!<\brief Failure to start tracking */
#define PL_PINIT_FAIL      0x13  /*!<\brief Failure to initialize tool in port */
#define PL_CAMERA          0x14  /*!<\brief Invalid camera parameters */
#define PL_INIT_FAIL       0x15  /*!<\brief Failure to initialize POLARIS */
#define PL_DSTART_FAIL     0x16  /*!<\brief Failure to start diagnostic mode */
#define PL_DSTOP_FAIL      0x17  /*!<\brief Failure to stop diagnostic mode */
#define PL_IRCHK_FAIL      0x18  /*!<\brief Failure to determine environmental IR */
#define PL_FIRMWARE        0x19  /*!<\brief Failure to read firmware version */
#define PL_INTERNAL        0x1a  /*!<\brief Internal POLARIS error */
#define PL_IRINIT_FAIL     0x1b /*!<\brief Failure to initialize for IR diagnostics*/
#define PL_IRED_FAIL       0x1c  /*!<\brief Failure to set marker firing signature */
#define PL_SROM_FAIL       0x1d  /*!<\brief Failure to search for SROM IDs */
#define PL_SROM_READ       0x1e  /*!<\brief Failure to read SROM data */
#define PL_SROM_WRITE      0x1f  /*!<\brief Failure to write SROM data */
#define PL_SROM_SELECT     0x20  /*!<\brief Failure to select SROM */
#define PL_PORT_CURRENT    0x21  /*!<\brief Failure to perform tool current test */ 
#define PL_WAVELENGTH      0x22 /*!<\brief No camera parameters for this wavelength*/
#define PL_PARAMETER_RANGE 0x23  /*!<\brief Command parameter out of range */
#define PL_VOLUME          0x24  /*!<\brief No camera parameters for this volume */
#define PL_FEATURES        0x25  /*!<\brief Failure to determine supported features*/

#define PL_ENVIRONMENT     0xf1  /*!<\brief Too much environmental infrared */

#define PL_EPROM_READ      0xf6  /*!<\brief Failure to read Flash EPROM */
#define PL_EPROM_WRITE     0xf5  /*!<\brief Failure to write Flash EPROM */
#define PL_EPROM_ERASE     0xf4  /*!<\brief Failure to erase Flash EPROM */

/* error codes returned by the C api */

#define PL_BAD_CRC         0x0100  /*!<\brief Bad CRC received from POLARIS */
#define PL_OPEN_ERROR      0x0200  /*!<\brief Error opening serial device */
#define PL_BAD_COMM        0x0300  /*!<\brief Bad communication parameters for host*/
#define PL_TIMEOUT         0x0400  /*!<\brief POLARIS took >5 secs to reply */
#define PL_WRITE_ERROR     0x0500  /*!<\brief Device write error */
#define PL_READ_ERROR      0x0600  /*!<\brief Device read error */
#define PL_RESET_FAIL      0x0700  /*!<\brief POLARIS failed to reset on break */
#define PL_PROBE_FAIL      0x0800  /*!<\brief POLARIS not found on specified port */
/*\}*/


/* plCOMM() baud rates */
/*\{*/
#define  PL_9600     0          
#define  PL_14400    1 
#define  PL_19200    2
#define  PL_38400    3
#define  PL_57600    4
#define  PL_115200   5
/*\}*/

/* plCOMM() data bits, parity and stop bits */
/*\{*/
#define  PL_8N1      0              /* 8 data bits, no parity, 1 stop bit */
#define  PL_8N2      1              /* etc. */
#define  PL_8O1     10
#define  PL_8O2     11
#define  PL_8E1     20
#define  PL_8E2     21
#define  PL_7N1    100
#define  PL_7N2    101
#define  PL_7O1    110
#define  PL_7O2    111
#define  PL_7E1    120
#define  PL_7E2    121 
/*\}*/

/* plCOMM() hardware handshaking */
/*\{*/
#define  PL_NOHANDSHAKE  0
#define  PL_HANDSHAKE    1
/*\}*/

/* plPENA() tracking modes */
/*\{*/
#define  PL_STATIC      'S'    /* relatively immobile tool */ 
#define  PL_DYNAMIC     'D'    /* dynamic tool (e.g. probe) */
#define  PL_BUTTON_BOX  'B'    /* tool with no IREDs */
/*\}*/

/* plGX() reply mode bit definitions */
/*\{*/
#define  PL_XFORMS_AND_STATUS  0x0001  /* transforms and status */
#define  PL_ADDITIONAL_INFO    0x0002  /* additional tool transform info */
#define  PL_SINGLE_STRAY       0x0004  /* stray active marker reporting */
#define  PL_FRAME_NUMBER       0x0008  /* frame number for each tool */
#define  PL_PASSIVE            0x8000  /* report passive tool information */
#define  PL_PASSIVE_EXTRA      0x2000  /* add 6 extra passive tools */
#define  PL_PASSIVE_STRAY      0x1000  /* stray passive marker reporting */
/*\}*/

/* return values that give the reason behind missing data */
/*\{*/
#define PL_DISABLED        1  
#define PL_MISSING         2  
#define PL_UNOCCUPIED      3
/*\}*/

/* plGetGXPortStatus() and plGetPSTATPortStatus() return value bits */
/*\{*/
#define  PL_TOOL_IN_PORT        0x01
#define  PL_SWITCH_1_ON         0x02
#define  PL_SWITCH_2_ON         0x04
#define  PL_SWITCH_3_ON         0x08
#define  PL_INITIALIZED         0x10
#define  PL_ENABLED             0x20
#define  PL_OUT_OF_VOLUME       0x40 /* only for plGetGXPortStatus() */
#define  PL_PARTIALLY_IN_VOLUME 0x80 /* only for plGetGXPortStatus() */ 
#define  PL_CURRENT_DETECT      0x80 /* only for plGetPSTATPortStatus() */
/*\}*/

/* plGetGXSystemStatus() return value bits */
/*\{*/
#define  PL_COMM_SYNC_ERROR            0x01
#define  PL_TOO_MUCH_EXTERNAL_INFRARED 0x02
#define  PL_COMM_CRC_ERROR             0x04
/*\}*/

/* plGetGXToolInfo() return value bits */
/*\{*/
#define  PL_BAD_TRANSFORM_FIT   0x01
#define  PL_NOT_ENOUGH_MARKERS  0x02
#define  PL_TOOL_FACE_USED      0x70
/*\}*/

/* plGetGXMarkerInfo() return value bits */
/*\{*/
#define  PL_MARKER_MISSING             0
#define  PL_MARKER_EXCEEDED_MAX_ANGLE  1
#define  PL_MARKER_EXCEEDED_MAX_ERROR  2
#define  PL_MARKER_USED                3
/*\}*/

/* plLED() states */
/*\{*/
#define  PL_BLANK 'B'    /* off */
#define  PL_FLASH 'F'    /* flashing */
#define  PL_SOLID 'S'    /* on */
/*\}*/

/* plPSTAT() reply mode bits */
/*\{*/
#define  PL_BASIC           0x0001
#define  PL_TESTING         0x0002
#define  PL_PART_NUMBER     0x0004
#define  PL_ACCESSORIES     0x0008
#define  PL_MARKER_TYPE     0x0010
/*\}*/

/* plGetPSTATAccessories() return value bits */
/*\{*/
#define  PL_TOOL_IN_PORT_SWITCH   0x01  /* tool has tool-in-port switch */
#define  PL_SWITCH_1              0x02  /* tool has button #1 */
#define  PL_SWITCH_2              0x04  /* tool has button #2 */
#define  PL_SWITCH_3              0x08  /* tool has button #3 */
#define  PL_TOOL_TRACKING_LED     0x10  /* tool has tracking LED */
#define  PL_LED_1                 0x20  /* tool has LED #1 */
#define  PL_LED_2                 0x40  /* tool has LED #2 */
#define  PL_LED_3                 0x80  /* tool has LED #3 */
/*\}*/
  
/* plGetPSTATMarkerType() return value, the first 3 bits */
/*\{*/
#define PL_950NM            0x00
#define PL_850NM            0x01
/*\}*/

/* plGetPSTATMarkerType() return value, the last 5 bits */
/*\{*/
#define PL_NDI_ACTIVE       0x08
#define PL_NDI_CERAMIC      0x10
#define PL_PASSIVE_ANY      0x20
#define PL_PASSIVE_SPHERE   0x28
#define PL_PASSIVE_DISC     0x30
/*\}*/

/* plSSTAT() reply format bits */
/*\{*/
#define PL_CONTROL         0x0001  /* control processor information */
#define PL_SENSORS         0x0002  /* sensor processors */
#define PL_TIU             0x0004  /* TIU processor */
/*\}*/

/* plGetSSTATControl() return value bits */
/*\{*/
#define PL_EPROM_CODE_CHECKSUM     0x01
#define PL_EPROM_SYSTEM_CHECKSUM   0x02
/*\}*/

/* plGetSSTATSensor() return value bits */
/*\{*/
#define PL_LEFT_ROM_CHECKSUM   0x01
#define PL_LEFT_SYNC_TYPE_1    0x02
#define PL_LEFT_SYNC_TYPE_2    0x04
#define PL_RIGHT_ROM_CHECKSUM  0x10
#define PL_RIGHT_SYNC_TYPE_1   0x20
#define PL_RIGHT_SYNC_TYPE_2   0x40
/*\}*/

/* plGetSSTATTIU() return value bits */
/*\{*/
#define PL_ROM_CHECKSUM        0x01
#define PL_OPERATING_VOLTAGES  0x02
#define PL_MARKER_SEQUENCING   0x04
#define PL_SYNC                0x08
#define PL_COOLING_FAN         0x10
#define PL_INTERNAL_ERROR      0x20
/*\}*/

/* plIRCHK() reply mode bits */
/*\{*/
#define  PL_DETECTED   0x0001   /* simple yes/no whether IR detected */
#define  PL_SOURCES    0x0002   /* locations of up to 20 sources per camera */
/*\}*/

/* plGetIRCHKNumberOfSources(), plGetIRCHKSourceXY() sensor arguments */
/*\{*/
#define  PL_LEFT   0            /* left sensor */
#define  PL_RIGHT  1            /* right sensor */
/*\}*/

#ifdef __cplusplus
}
#endif

#endif /* POLARIS_H */

