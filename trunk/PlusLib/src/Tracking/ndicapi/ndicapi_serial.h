/*=======================================================================

  Program:   NDI Combined API C Interface Library
  Module:    $RCSfile: ndicapi_serial.h,v $
  Creator:   David Gobbi <dgobbi@atamai.com>
  Language:  C
  Author:    $Author: kcharbon $
  Date:      $Date: 2008/06/18 15:33:20 $
  Version:   $Revision: 1.6 $

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

/*! \file ndicapi_serial.h
  This file contains the platform-dependent portions of the
  NDICAPI C API that talk to the serial port.
*/

#ifndef NDICAPI_SERIAL_H
#define NDICAPI_SERIAL_H 1

#ifdef __cplusplus
extern "C" {
#endif

/*=====================================================================*/
/*! \defgroup NDISerial NDI Serial Methods
  These are low-level methods that provide a platform-independent
  interface to the serial port.
*/

/*! \ingroup NDISerial
  \typedef NDIFileHandle
  The serial port handle is a platform-specific type, for which we use
  the typedef NDIFileHandle.  On UNIX it is an int.
*/

#if defined(WIN32) || defined(_WIN32)

#include <windows.h>
typedef HANDLE NDIFileHandle;
#define NDI_INVALID_HANDLE INVALID_HANDLE_VALUE

#define NDI_NUMBER_OF_DEVICES 7
#define NDI_DEVICE0    "COM1:"
#define NDI_DEVICE1    "COM2:"
#define NDI_DEVICE2    "COM3:"
#define NDI_DEVICE3    "COM4:"
#define NDI_DEVICE4    "COM5:"
#define NDI_DEVICE5    "COM6:"
#define NDI_DEVICE6    "COM7:"
#define NDI_DEVICE7    "COM8:"

#elif defined(unix) || defined(__unix__) || defined(__APPLE__)

typedef int NDIFileHandle;
#define NDI_INVALID_HANDLE -1

#if defined(linux) || defined(__linux__)
#define NDI_NUMBER_OF_DEVICES 7
#define NDI_DEVICE0    "/dev/ttyS0"
#define NDI_DEVICE1    "/dev/ttyS1"
#define NDI_DEVICE2    "/dev/ttyUSB0"
#define NDI_DEVICE3    "/dev/ttyUSB1"
#define NDI_DEVICE4    "/dev/ttyUSB2"
#define NDI_DEVICE5    "/dev/ttyUSB3"
#define NDI_DEVICE6     "/dev/ttyUSB4"
#define NDI_DEVICE7    "/dev/ttyUSB5"

#elif defined(__APPLE__)
#define NDI_NUMBER_OF_DEVICES 0
#elif defined(sgi)
#define NDI_NUMBER_OF_DEVICES 4
#define NDI_DEVICE0    "/dev/ttyd1"
#define NDI_DEVICE1    "/dev/ttyd2"
#define NDI_DEVICE2    "/dev/ttyd3"
#define NDI_DEVICE3    "/dev/ttyd4"
#define NDI_DEVICE4    "/dev/ttyd5"
#define NDI_DEVICE5    "/dev/ttyd6"
#define NDI_DEVICE6    "/dev/ttyd7"
#define NDI_DEVICE7    "/dev/ttyd8"
#else
#define NDI_NUMBER_OF_DEVICES 2
#define NDI_DEVICE0    "/dev/ttya"
#define NDI_DEVICE1    "/dev/ttyb"
#endif

#elif defined(macintosh)

typedef long NDIFileHandle;
#define NDI_INVALID_HANDLE -1
/* macros to extract input and output file handles */
#define output_file(serial_port) ((short)((serial_port & 0xff00) >> 16));
#define input_file(serial_port) ((short)((serial_port & 0x00ff) >> 0));

#define NDI_NUMBER_OF_DEVICES 2
#define NDI_DEVICE0    "\p.A"
#define NDI_DEVICE1    "\p.B"

#endif

/*! \ingroup NDISerial
  Open the specified serial port device.
  A return value of NDI_INVALID_HANDLE means that an error occurred.
  The serial port will be set to 9600 baud, 8N1, no handshake.

  The macros NDI_DEVICE0 through NDI_DEVICE3 will expand to valid device
  names for the first four serial ports, e.g. "/dev/ttyS0" on linux.

  The type of the handle is platform-specific.
*/ 
NDIFileHandle ndiSerialOpen(const char *device);

/*! \ingroup NDISerial
  Close the serial port.  It is wise to send a "COMM 00000\r" command
  to the NDICAPI before you close the port, otherwise the next time the
  serial port is opened you will have to reset the NDICAPI before you can
  resume communication with it.
*/ 
void ndiSerialClose(NDIFileHandle serial_port);

/*! \ingroup NDISerial
  Check whether the DSR signel is set.  If the return value is zero,
  then the DSR is not set which means that either the device has been
  unplugged from the serial port or has been turned off.
*/
int ndiSerialCheckDSR(NDIFileHandle serial_port);

/*! \ingroup NDISerial
  Send a 300ms break to the serial port to reset the NDICAPI.
  After you call this function, you must also call
  ndiSerialComm(file, 9600, "8N1", 0) to reset the
  serial port back to the default NDICAPI communication parameters.
  The NDICAPI will beep and send the text "RESETBE6F\r", which
  must be read with ndiSerialRead().

  The return value of this function will be if the call was successful.
  A negative return value means that an IO error occurred.
*/
int ndiSerialBreak(NDIFileHandle serial_port);

/*! \ingroup NDISerial
  Flush out the serial I/O buffers. The following options are available:
  - NDI_IFLUSH:  discard the contents of the input buffer
  - NDI_OFLUSH:  discard the contents of the output buffer
  - NDI_IOFLUSH: discard the contents of both buffers.

  <p>The return value of this function will be if the call was successful.
  A negative return value means that an IO error occurred.
*/
int ndiSerialFlush(NDIFileHandle serial_port, int flushtype);

#define  NDI_IFLUSH  0x1
#define  NDI_OFLUSH  0x2
#define  NDI_IOFLUSH 0x3

/*! \ingroup NDISerial
  Change the baud rate and other comm parameters.

  The baud rate should be one of 9600, 14400, 19200, 38400, 57600, 115200.

  The mode string should be one of "8N1", "7O2" etc. The first character
  is the number of data bits (7 or 8), the second character is the parity
  (N, O or E for none, odd or even), and the the third character is the
  number of stop bits (1 or 2).

  The handshake value should be 0 or 1 depending on whether hardware
  handshaking is desired.

  The default setting for the NDICAPI (i.e. after a power-on or a reset)
  is (9600, "8N1", 0).  A commonly used setting is (115200, "8N1", 1).

  Before this function is called you should send a COMM command to the
  ndicapi and read the "OKAY" reply. 

  The return value will be 0 if the call was successful.
  A negative return value means that an IO error occurred, or that
  the specified parameters are not available for this serial port.
*/ 
int ndiSerialComm(NDIFileHandle serial_port, int baud, const char *mode,
                  int handshake);

/*! \ingroup NDISerial
  Change the timeout for the serial port in milliseconds.
  The default is 5 seconds, but this might be too long for certain
  applications.

  The return value will be 0 if the call was successful.
  A negative return value signals failure.
*/
int ndiSerialTimeout(NDIFileHandle serial_port, int milliseconds);

/*! \ingroup NDISerial
  Write a stream of 'n' characters from the string 'text' to the serial
  port.  The number of characters actually written is returned.
  The NDICAPI expects all commands to end with a carriage return.

  The serial port input buffers are flushed before the information is
  written, because leftover bytes in the input buffer are likely due
  to an error on a previous communication.

  If the return value is negative, then an IO error occurred.
  If the return value is less than 'n', then a timeout error occurred.
*/
int ndiSerialWrite(NDIFileHandle serial_port, const char *text, int n);

/*! \ingroup NDISerial
  Read characters from the serial port until a carriage return is
  received.  A maximum of 'n' characters will be read.  The number
  of characters actually read is returned.  The resulting string will
  not be null-terminated.

  If the return value is negative, then an IO error occurred.
  If the return value is zero, then a timeout error occurred.
  If the return value is equal to 'n' and the final character
  is not a carriage return (i.e. reply[n-1] != '\r'), then the
  read was incomplete and there are more characters waiting to
  be read.
*/
int ndiSerialRead(NDIFileHandle serial_port, char *reply, int n);

/*! \ingroup NDISerial
  Sleep for the specified number of milliseconds.  The actual sleep time
  is likely to last for 10ms longer than the specifed time due to 
  task scheduling overhead.  The return value is always zero.
*/
int ndiSerialSleep(NDIFileHandle serial_port, int milliseconds);

#ifdef __cplusplus
}
#endif

#endif /* NDICAPI_SERIAL_H */
