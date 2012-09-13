/*=======================================================================

  Program:   NDI Combined API C Interface Library
  Module:    $RCSfile: ndicapi_serial.c,v $
  Creator:   David Gobbi <dgobbi@atamai.com>
  Language:  C
  Author:    $Author: dgobbi $
  Date:      $Date: 2005/10/31 02:15:36 $
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

/*---------------------------------------------------------------------
  This file contains the platform-dependent portions of the source code
  that talk to the serial port.  All these methods
  are of the form ndiSerialXX().
---------------------------------------------------------------------*/

/* =========== standard includes */
#include <errno.h>
#include <time.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* =========== windows includes */
#if defined(WIN32) || defined(_WIN32)
#include <windows.h>
#include <winbase.h>
#include <sys/timeb.h>

/* =========== unix includes */
#elif defined(unix) || defined(__unix__) || defined(__APPLE__)
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <termios.h>

/* =========== mac includes */
#elif defined(macintosh)
#include <Devices.h>
#include <Files.h>
#include <Serial.h>
#include <Types.h>
#endif /* macintosh */

#include "ndicapi_serial.h"



/* USB versions of NDI tracking can communicate at baud rate 921600 but is not defined in WinBase.h */
#if defined(WIN32) || defined(_WIN32)
  #ifndef  CBR_921600
    #define  CBR_921600 921600
  #endif
#endif

/* TODO: add similar fixes to the higher baud rate for Linux, Apple, etc. */


/* time out period in milliseconds */
#define TIMEOUT_PERIOD 5000

/*---------------------------------------------------------------------*/
/* Some static variables to keep track of which ports are open, so that
   we can restore the comm parameters (baud rate etc) when they are closed.
   Restoring the comm parameters is just part of being a good neighbor. */

#if defined(WIN32) || defined(_WIN32)

#define NDI_MAX_SAVE_STATE 4
static HANDLE ndi_open_handles[4] = { INVALID_HANDLE_VALUE,
             INVALID_HANDLE_VALUE,
             INVALID_HANDLE_VALUE,
             INVALID_HANDLE_VALUE };

static COMMTIMEOUTS ndi_save_timeouts[4];
static DCB ndi_save_dcb[4];

#elif defined(unix) || defined(__unix__) || defined(__APPLE__)

#define NDI_MAX_SAVE_STATE 4
static int ndi_open_handles[4] = { -1, -1, -1, -1 };

static struct termios ndi_save_termios[4];

#elif defined(macintosh)

#define NDI_MAX_SAVE_STATE 4
static long ndi_open_handles[4] = { -1, -1, -1, -1 };

#endif

/*---------------------------------------------------------------------*/
#if defined(WIN32) || defined(_WIN32)

HANDLE ndiSerialOpen(const char *device)
{
  static COMMTIMEOUTS default_ctmo = { MAXDWORD, MAXDWORD,
               TIMEOUT_PERIOD, 
               2, 
               TIMEOUT_PERIOD };
  HANDLE serial_port;
  DCB comm_settings;
  int i;

  serial_port = CreateFile(device,
                           GENERIC_READ|GENERIC_WRITE,
                           0,  /* not allowed to share ports */
                           0,  /* child-processes don't inherit handle */
                           OPEN_EXISTING, 
                           FILE_ATTRIBUTE_NORMAL,
                           NULL); /* no template file */

  if (serial_port == INVALID_HANDLE_VALUE) {
    return INVALID_HANDLE_VALUE;
  }

  /* save the serial port state so that it can be restored when
     the serial port is closed in ndiSerialClose() */
  for (i = 0; i < NDI_MAX_SAVE_STATE; i++) {
    if (ndi_open_handles[i] == serial_port ||
  ndi_open_handles[i] == INVALID_HANDLE_VALUE) {
      ndi_open_handles[i] = serial_port;
      GetCommTimeouts(serial_port,&ndi_save_timeouts[i]);
      GetCommState(serial_port,&ndi_save_dcb[i]);
      break;
    }
  }

  if (SetupComm(serial_port,1600,1600) == FALSE) { /* set buffer size */
    if (i < NDI_MAX_SAVE_STATE) { /* if we saved the state, forget the state */
      ndi_open_handles[i] = INVALID_HANDLE_VALUE;
    }
    CloseHandle(serial_port);
    return INVALID_HANDLE_VALUE;
  }

  if (GetCommState(serial_port,&comm_settings) == FALSE) {
    if (i < NDI_MAX_SAVE_STATE) { /* if we saved the state, forget the state */
      ndi_open_handles[i] = INVALID_HANDLE_VALUE;
    }
    CloseHandle(serial_port);
    return INVALID_HANDLE_VALUE;
  }

  comm_settings.fOutX = FALSE;             /* no S/W handshake */
  comm_settings.fInX = FALSE;
  comm_settings.fAbortOnError = FALSE;     /* don't need to clear errors */
  comm_settings.fOutxDsrFlow = FALSE;      /* no modem-style flow stuff*/
  comm_settings.fDtrControl = DTR_CONTROL_ENABLE;  

  if (SetCommState(serial_port,&comm_settings) == FALSE) {
    if (i < NDI_MAX_SAVE_STATE) { /* if we saved the state, forget the state */
      ndi_open_handles[i] = INVALID_HANDLE_VALUE;
    }
    CloseHandle(serial_port);
    return INVALID_HANDLE_VALUE;
  }
  
  if (SetCommTimeouts(serial_port,&default_ctmo) == FALSE) {
    SetCommState(serial_port,&comm_settings);
    if (i < NDI_MAX_SAVE_STATE) { /* if we saved the state, forget the state */
      SetCommState(serial_port,&ndi_save_dcb[i]); 
      ndi_open_handles[i] = INVALID_HANDLE_VALUE;
    }
    CloseHandle(serial_port);
    return INVALID_HANDLE_VALUE;
  }

  return serial_port;
}

#elif defined(unix) || defined(__unix__) || defined(__APPLE__)

int ndiSerialOpen(const char *device)
{
  static struct flock fl = { F_WRLCK, 0, 0, 0 }; /* for file locking */
  static struct flock fu = { F_UNLCK, 0, 0, 0 }; /* for file unlocking */
  int serial_port;
  struct termios t;
  int i;

  /* port is readable/writable and is (for now) non-blocking */
  serial_port = open(device,O_RDWR|O_NOCTTY|O_NDELAY);

  if (serial_port == -1) {
    return -1;             /* bail out on error */
  }

  /* restore blocking now that the port is open (we just didn't want */
  /* the port to block while we were trying to open it) */
  fcntl(serial_port, F_SETFL, 0);

#ifndef __APPLE__
  /* get exclusive lock on the serial port */
  /* on many unices, this has no effect for device files */
  if (fcntl(serial_port, F_SETLK, &fl)) {
    close(serial_port);
    return -1;
  }
#endif /* __APPLE__ */

  /* get I/O information */
  if (tcgetattr(serial_port,&t) == -1) {
    fcntl(serial_port, F_SETLK, &fu);
    close(serial_port);
    return -1;
  }

  /* save the serial port state so that it can be restored when
     the serial port is closed in ndiSerialClose() */
  for (i = 0; i < NDI_MAX_SAVE_STATE; i++) {
    if (ndi_open_handles[i] == serial_port || ndi_open_handles[i] == -1) {
      ndi_open_handles[i] = serial_port;
      tcgetattr(serial_port,&ndi_save_termios[i]);
      break;
    }
  }

  /* clear everything specific to terminals */
  t.c_lflag = 0;
  t.c_iflag = 0;
  t.c_oflag = 0;

  t.c_cc[VMIN] = 0;                    /* use constant, not interval timout */
  t.c_cc[VTIME] = TIMEOUT_PERIOD/100;  /* wait for 5 secs max */

  if (tcsetattr(serial_port,TCSANOW,&t) == -1) { /* set I/O information */
    if (i < NDI_MAX_SAVE_STATE) { /* if we saved the state, forget the state */
      ndi_open_handles[i] = -1;
    }
    fcntl(serial_port, F_SETLK, &fu);
    close(serial_port);
    return -1;
  }

  tcflush(serial_port,TCIOFLUSH);         /* flush the buffers for good luck */

  return serial_port;
}

#elif defined(macintosh)

long ndiSerialOpen(const char *device)
{
  short serial_out, serial_in; /* macintosh device handles */
  long serial_port; /* hold them both, packed into 32 bits */
  char namein[64], nameout[64];
  int i;

  sprintf(namein,"%sIn",device);
  sprintf(nameout,"%sOut",device);

  if (OpenDriver(namein, &serial_in) != noErr) {
    return -1;
  }
  if (OpenDriver(nameout,&serial_out) != noErr) {
    CloseDriver(serial_in);
    return -1;
  }

  /* combine the two shorts into one long */
  serial_port = ((serial_in << 0) | (serial_out << 16));
  
  /* save the serial port state so that it can be restored when
     the serial port is closed in ndiSerialClose() */
  for (i = 0; i < NDI_MAX_SAVE_STATE; i++) {
    if (ndi_open_handles[i] == serial_port || ndi_open_handles[i] == -1) {
      ndi_open_handles[i] = serial_port;
      break;
    }
  }

  return serial_port;
}
#endif /* macintosh */

/*---------------------------------------------------------------------*/
#if defined(WIN32) || defined(_WIN32)

void ndiSerialClose(HANDLE serial_port)
{
  int i;

  /* restore the comm port state to from before it was opened */
  for (i = 0; i < NDI_MAX_SAVE_STATE; i++) {
    if (ndi_open_handles[i] == serial_port && 
  ndi_open_handles[i] != INVALID_HANDLE_VALUE) {
      SetCommTimeouts(serial_port,&ndi_save_timeouts[i]);
      SetCommState(serial_port,&ndi_save_dcb[i]);
      ndi_open_handles[i] = INVALID_HANDLE_VALUE;      
      break;
    }
  }

  CloseHandle(serial_port);
}

#elif defined(unix) || defined(__unix__) || defined(__APPLE__)

void ndiSerialClose(int serial_port)
{
  static struct flock fu = { F_UNLCK, 0, 0, 0 }; /* for file unlocking */
  int i;

  /* restore the comm port state to from before it was opened */
  for (i = 0; i < NDI_MAX_SAVE_STATE; i++) {
    if (ndi_open_handles[i] == serial_port && ndi_open_handles[i] != -1) {
      tcsetattr(serial_port,TCSANOW,&ndi_save_termios[i]);
      ndi_open_handles[i] = -1;
      break;
    }
  }

  /* release our lock on the serial port */
  fcntl(serial_port, F_SETLK, &fu);

  close(serial_port);
}

#elif defined(macintosh)

void ndiSerialClose(long serial_port)
{
  int i;

  /* restore the comm port state to from before it was opened */
  for (i = 0; i < NDI_MAX_SAVE_STATE; i++) {
    if (ndi_open_handles[i] == serial_port && ndi_open_handles[i] != -1) {
      ndi_open_handles[i] = -1;
      break;
    }
  }

  CloseDriver(input_file(serial_port));
  CloseDriver(output_file(serial_port));
}

#endif /* macintosh */

/*---------------------------------------------------------------------*/
#if defined(WIN32) || defined(_WIN32)

int ndiSerialCheckDSR(HANDLE serial_port)
{
  DWORD bits;
  /* get the bits to see if DSR is set (i.e. if device is connected) */
  GetCommModemStatus(serial_port, &bits);
  return ((bits & MS_DSR_ON) != 0);
}

#elif defined(unix) || defined(__unix__) || defined(__APPLE__)

int ndiSerialCheckDSR(int serial_port)
{
#if defined(linux) || defined(__linux__)
  int bits;
  /* get the bits to see if DSR is set (i.e. if device is connected) */
  if (ioctl(serial_port, TIOCMGET, &bits) >= 0) {
    return ((bits & TIOCM_DSR) != 0);
  }
#endif
  /* if called failed for any reason, return success for robustness */
  return 1;
}

#elif defined(macintosh)

int ndiSerialCheckDSR(long serial_port)
{
  /* not implemented... */
  return 1;
}

#endif /* macintosh */

/*---------------------------------------------------------------------*/
#if defined(WIN32) || defined(_WIN32)

int ndiSerialBreak(HANDLE serial_port)
{
  DWORD dumb;

  ClearCommError(serial_port,&dumb,NULL);       /* clear error */
  PurgeComm(serial_port,PURGE_TXCLEAR|PURGE_RXCLEAR); /* clear buffers */

  SetCommBreak(serial_port);
  Sleep(300);                            /* hold break for 0.3 seconds */ 
  ClearCommBreak(serial_port);

  return 0;
}

#elif defined(unix) || defined(__unix__) || defined(__APPLE__)

int ndiSerialBreak(int serial_port)
{
  tcflush(serial_port,TCIOFLUSH);     /* clear input/output buffers */
  tcsendbreak(serial_port,0);         /* send the break */

  return 0;
}

#elif defined(macintosh)

int ndiSerialBreak(long serial_port)
{
  SerSetBrk(output_file(serial_port));
  oldmicrosecs = clock();
  for (;;) {
    microsecs = clock();
    if (microsecs - oldmicrosecs > 3*CLOCKS_PER_SEC/10)/*break for .3 secs*/
      break;
  }
  SerClrBrk(output_file(serial_port));

  return 0;
}

#endif /* macintosh */

/*---------------------------------------------------------------------*/
#if defined(WIN32) || defined(_WIN32)

int ndiSerialFlush(HANDLE serial_port, int buffers)
{
  DWORD dumb;
  DWORD flushtype = PURGE_TXCLEAR | PURGE_RXCLEAR;

  if (buffers == NDI_IFLUSH) {
    flushtype = PURGE_RXCLEAR;
  }
  else if (buffers == NDI_OFLUSH) {
    flushtype = PURGE_TXCLEAR;
  }    

  ClearCommError(serial_port,&dumb,NULL);       /* clear error */
  PurgeComm(serial_port,flushtype);             /* clear buffers */

  return 0;
}

#elif defined(unix) || defined(__unix__) || defined(__APPLE__)

int ndiSerialFlush(int serial_port, int buffers)
{
  int flushtype = TCIOFLUSH;

  if (buffers == NDI_IFLUSH) {
    flushtype = TCIFLUSH;
  }
  else if (buffers == NDI_OFLUSH) {
    flushtype = TCOFLUSH;
  }    

  tcflush(serial_port,flushtype);     /* clear input/output buffers */

  return 0;
}

#elif defined(macintosh)

int ndiSerialFlush(long serial_port, int buffers)
{
  /* no flush on the mac for now */

  return 0;
}

#endif /* macintosh */

/*---------------------------------------------------------------------*/
#if defined(WIN32) || defined(_WIN32)

int ndiSerialComm(HANDLE serial_port, int baud, const char *mode,
                 int handshake)
{
  DCB comm_settings;
  int newbaud;

  switch (baud)
    {
    case 9600:   newbaud = CBR_9600;   break;
    case 14400:  newbaud = CBR_14400;  break;
    case 19200:  newbaud = CBR_19200;  break;
    case 38400:  newbaud = CBR_38400;  break;
    case 57600:  newbaud = CBR_57600;  break;
    case 115200: newbaud = CBR_115200; break;
    case 921600: newbaud = CBR_921600; break;
    default:     return -1;
    }

  GetCommState(serial_port,&comm_settings);

  comm_settings.BaudRate = newbaud;     /* speed */

  if (handshake) {                      /* set handshaking */
    comm_settings.fOutxCtsFlow = TRUE;       /* on */
    comm_settings.fRtsControl = RTS_CONTROL_HANDSHAKE;
  }
  else {
    comm_settings.fOutxCtsFlow = FALSE;       /* off */
    comm_settings.fRtsControl = RTS_CONTROL_DISABLE;
  }    

  if (mode[0] == '8') {                 /* data bits */
    comm_settings.ByteSize = 8;
  }
  else if (mode[0] == '7') {
    comm_settings.ByteSize = 7;
  }
  else {
    return -1;
  }

  if (mode[1] == 'N') {                 /* set parity */
    comm_settings.Parity = NOPARITY;
  }
  else if (mode[1] == 'O') {
    comm_settings.Parity = ODDPARITY;
  }
  else if (mode[1] == 'E') {
    comm_settings.Parity = EVENPARITY;
  }
  else {
    return -1;
  }

  if (mode[2] == '1') {                  /* set stop bits */
    comm_settings.StopBits = ONESTOPBIT;
  }
  else if (mode[2] == '2') {
    comm_settings.StopBits = TWOSTOPBITS;
  }
  else {
    return -1;
  }

  SetCommState(serial_port,&comm_settings);

  return 0;
}

#elif defined(unix) || defined(__unix__) || defined(__APPLE__)

int ndiSerialComm(int serial_port, int baud, const char mode[4], int handshake)
{
  struct termios t;
  int newbaud;

#if defined(linux) || defined(__linux__)
  switch (baud)
    {
    case 9600:   newbaud = B9600;   break;
    case 14400:  return -1;
    case 19200:  newbaud = B19200;  break;
    case 38400:  newbaud = B38400;  break;
    case 57600:  newbaud = B57600;  break;
    case 115200: newbaud = B115200; break;
    default:     return -1;
    }
#elif defined(__APPLE__)
  switch (baud)
    {
    case 9600:    newbaud = B9600;   break;
    case 14400:   return -1;
    case 19200:   newbaud = B19200;  break;
    case 38400:   newbaud = B38400;  break;
    case 57600:   newbaud = B57600;  break;
    case 115200:  newbaud = B115200; break;
    default:      return -1;
    }
#elif defined(sgi) && defined(__NEW_MAX_BAUD)
  switch (baud)
    {
    case 9600:    newbaud = 9600;   break;
    case 14400:   return -1;
    case 19200:   newbaud = 19200;  break;
    case 38400:   newbaud = 38400;  break;
    case 57600:   newbaud = 57600;  break;
    case 115200:  newbaud = 115200; break;
    default:      return -1;
    }
#else
  switch (baud)
    {
    case 9600:   newbaud = B9600;  break;
    case 14400:  return -1;
    case 19200:  newbaud = B19200;  break;
    case 38400:  newbaud = B38400;  break;
    case 57600:  return -1;
    case 115200: return -1;
    default:     return -1;
    }
#endif

  tcgetattr(serial_port,&t);          /* get I/O information */
  t.c_cflag &= ~CSIZE;                /* clear flags */

#if defined(linux) || defined(__linux__)
  t.c_cflag &= ~CBAUD;
  t.c_cflag |= newbaud;                /* set baud rate */
#elif defined(__APPLE__)
  cfsetispeed(&t, newbaud);
  cfsetospeed(&t, newbaud);
#elif defined(sgi) && defined(__NEW_MAX_BAUD)
  t.c_ospeed = newbaud;
#else
  t.c_cflag &= ~CBAUD;
  t.c_cflag |= newbaud;                /* set baud rate */
#endif

  if (mode[0] == '8') {                 /* set data bits */
    t.c_cflag |= CS8; 
  }
  else if (mode[0] == '7') {
    t.c_cflag |= CS7;
  }
  else {
    return -1;
  }

  if (mode[1] == 'N') {                 /* set parity */
    t.c_cflag &= ~PARENB;
    t.c_cflag &= ~PARODD;
  }
  else if (mode[1] == 'O') {
    t.c_cflag |= PARENB;
    t.c_cflag |= PARODD;
  }
  else if (mode[1] == 'E') {
    t.c_cflag |= PARENB;
    t.c_cflag &= ~PARODD;
  }
  else {
    return -1;
  }

  if (mode[2] == '1') {                  /* set stop bits */
    t.c_cflag &= ~CSTOPB; 
  }
  else if (mode[2] == '2') {
    t.c_cflag |= CSTOPB; 
  }
  else {
    return -1;
  }

  if (handshake) {
#ifdef sgi
    t.c_cflag |= CNEW_RTSCTS;       /* enable hardware handshake */
#else
    t.c_cflag |= CRTSCTS;           
#endif
  }
  else {
#ifdef sgi
    t.c_cflag &= ~CNEW_RTSCTS;          /* turn off hardware handshake */
#else
    t.c_cflag &= ~CRTSCTS;
#endif     
  } 

  tcsetattr(serial_port,TCSADRAIN,&t);  /* set I/O information */

  return 0;
}

#elif defined(macintosh)

int ndiSerialComm(long serial_port, int baud, const char mode[4], int handshake)
{
  SerShk sconf;
  short config = 0;

  int newbaud;

  switch (baud)
    {
    case 9600:   newbaud = baud9600;   break;
    case 14400:  newbaud = baud14400;  break;
    case 19200:  newbaud = baud19200;  break;
    case 38400:  newbaud = baud38400;  break;
    case 57600:  newbaud = baud57600;  break;
    case 115200: return -1;
    default:     return -1;
    }

  sconf.fXOn = 0;  /* disable software handshaking */
  sconf.fInX = 0;
  sconf.errs = 0;
  sconf.evts = 0;
  sconf.fCTS = (handshake != 0);  /* set hardware handshaking */
  sconf.fDTR = (handshake != 0);

  config = newbaud;             /* set speed */

  if (mode[0] == '8') {                 /* set data bits */
    config |= data8
  }
  else if (mode[0] == '7') {
    config |= data7
  }
  else {
    return -1;
  }

  if (mode[1] == 'N') {                 /* set parity */
    config |= noParity;
  }
  else if (mode[1] == 'O') {
    config |= oddParity;
  }
  else if (mode[1] == 'E') {
    config |= evenParity;
  }
  else {
    return -1;
  }

  if (mode[2] == '1') {                  /* set stop bits */
    config |= stop10;
  }
  else if (mode[2] == '2') {
    config |= stop20;
  }
  else {
    return -1;
  }

  SerHShake(input_file(serial_port), &sconf);
  SerHShake(output_file(serial_port), &sconf);
  SerReset(input_file(serial_port), newbaud);
  SerReset(output_file(serial_port), newbaud);

  return 0;
}

#endif /* macintosh */

/*---------------------------------------------------------------------*/
#if defined(WIN32) || defined(_WIN32)

int ndiSerialTimeout(HANDLE serial_port, int milliseconds)
{
  COMMTIMEOUTS ctmo;
  
  if (GetCommTimeouts(serial_port,&ctmo) == FALSE) {
    return -1;
  }
  
  ctmo.ReadIntervalTimeout = MAXDWORD;
  ctmo.ReadTotalTimeoutMultiplier = MAXDWORD;
  ctmo.ReadTotalTimeoutConstant = milliseconds;
  ctmo.WriteTotalTimeoutConstant = milliseconds;

  if (SetCommTimeouts(serial_port,&ctmo) == FALSE) {
    return -1;
  }

  return 0;
}

#elif defined(unix) || defined(__unix__) || defined(__APPLE__)

int ndiSerialTimeout(int serial_port, int milliseconds)
{
  struct termios t;

  if (tcgetattr(serial_port,&t) == -1) {
    return -1;
  }

  t.c_cc[VMIN] = 0;                  /* use constant, not interval timout */
  t.c_cc[VTIME] = milliseconds/100;  /* wait time is in 10ths of a second */

  if (tcsetattr(serial_port,TCSANOW,&t) == -1) {
    return -1;
  }

  return 0;
}

#elif defined(macintosh)

int ndiSerialTimeout(long serial_port, int milliseconds)
{
  /* not implemented for the mac */
  return 0;
}

#endif /* macintosh */

/*---------------------------------------------------------------------*/
#if defined(WIN32) || defined(_WIN32)

int ndiSerialWrite(HANDLE serial_port, const char *text, int n)
{
  DWORD m, dumb;
  int i = 0;

  while (n > 0) {
    if (WriteFile(serial_port,&text[i],n,&m,NULL) == FALSE) {
      if (GetLastError() == ERROR_OPERATION_ABORTED) {/* system cancelled us */
  ClearCommError(serial_port,&dumb,NULL); /* so clear error and retry */
      }
      else {
  return -1;  /* IO error occurred */
      }
    }
    else if (m == 0) { /* no characters written, must have timed out */
      return i;
    }

    n -= m;  /* n is number of chars left to write */
    i += m;  /* i is the number of chars written */
  }

  return i;  /* return the number of characters written */
}

#elif defined(unix) || defined(__unix__) || defined(__APPLE__)

int ndiSerialWrite(int serial_port, const char *text, int n)
{
  int i = 0;
  int m;

  while (n > 0) { 
    if ((m = write(serial_port,&text[i],n)) == -1) {
      if (errno == EAGAIN) { /* system cancelled us, retry */
  m = 0;
      }
      else {
  return -1;  /* IO error occurred */
      }
    }

    n -= m;  /* n is number of chars left to write */
    i += m;  /* i is the number of chars written */
  }

  return i;  /* return the number of characters written */
}

#elif defined(macintosh)

int ndiSerialWrite(long serial_port, const char *text, int n)
{
  if (FSWrite(output_file(serial_port),&n,text) != noErr) {
    return -1;
  }
  
  return n;
}

#endif /* macintosh */

/*---------------------------------------------------------------------*/
#if defined(WIN32) || defined(_WIN32)

int ndiSerialRead(HANDLE serial_port, char *reply, int n)
{
  int i = 0;
  DWORD m,dumb;
  
  while (n > 0) {
    if (ReadFile(serial_port,&reply[i],n,&m,NULL) == FALSE) { 
      if (GetLastError() == ERROR_OPERATION_ABORTED) {/* cancelled */
        ClearCommError(serial_port,&dumb,NULL); /* so clear error and retry */
      }
      else {
  return -1;  /* IO error occurred */
      }
    }
    else if (m == 0) { /* no characters read, must have timed out */
      return 0;
    }
    n -= m;  /* n is number of chars left to read */
    i += m;  /* i is the number of chars read */
    if (reply[i-1] == '\r') {  /* done when carriage return received */
      break;
    }
  }

  return i;
}

#elif defined(unix) || defined(__unix__) || defined(__APPLE__)

int ndiSerialRead(int serial_port, char *reply, int n)
{
  int i = 0;
  int m;

  while (n > 0) {                        /* read reply until <CR> */
    if ((m = read(serial_port,&reply[i],n)) == -1) {
      if (errno == EAGAIN) {      /* cancelled, so retry */
  m = 0;
      }
      else {
  return -1;  /* IO error occurred */
      }
    }
    else if (m == 0) { /* no characters read, must have timed out */
      return 0;
    }
    n -= m;  /* n is number of chars left to read */
    i += m;  /* i is the number of chars read */
    if (reply[i-1] == '\r') {  /* done when carriage return received */
      break;
    }
  }

  return i;
}

#elif defined(macintosh)

int ndiSerialRead(long serial_port, char *reply, int n)
{
  clock_t microsecs, oldmicrosecs;
  int i = 0;
  int m;

  oldmicrosecs = clock();                /* get current processor time */
  while (n > 0) {                        /* read reply until <CR> */
    m = n;
    if (SerGetBuf(serial_in,&m) != noErr) {
      return -1;
    }
    if (m == 0) {
      microsecs = clock();
      if (microsecs - oldmicrosecs > 5*CLOCKS_PER_SEC) {
        return i;                        /* time out after 5 secs */
      }
    }
    else {
      if (FSRead(serial_in,&m,&reply[i]) != noErr) {
        return -1;
      }
      n -= m;  /* n is number of chars left to read */
      i += m;  /* i is the number of chars read */
      if (reply[i-1] == '\r') {  /* done when carriage return received */
  break;
      }
    }
  }

  return i;
}

#endif /* macintosh */

/*---------------------------------------------------------------------*/
#if defined(WIN32) || defined(_WIN32)

int ndiSerialSleep(HANDLE serial_port, int milliseconds)
{
  Sleep(milliseconds);

  return 0;
}

#elif defined(unix) || defined(__unix__) || defined(__APPLE__)

int ndiSerialSleep(int serial_port, int milliseconds)
{
#ifdef USE_NANOSLEEP
  struct timespec sleep_time, dummy;
  sleep_time.tv_sec = milliseconds/1000;
  sleep_time.tv_nsec = (milliseconds - sleep_time.tv_sec*1000)*1000000;
  nanosleep(&sleep_time,&dummy);
#else /* use usleep instead */ 
  /* some unices like IRIX can't usleep for more than 1 second,
     so break usleep into 500 millisecond chunks */
  while (milliseconds > 500) {
    usleep(500000);
    milliseconds -= 500;
  }
  usleep(milliseconds*1000);
#endif

  return 0;
}

#elif defined(macintosh)

int ndiSerialSleep(long serial_port, int milliseconds)
{
  /* no sleep on the mac for now */

  return 0;
}

#endif /* macintosh */

/*---------------------------------------------------------------------*/
