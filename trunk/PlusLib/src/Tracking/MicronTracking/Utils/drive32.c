/***************************************************
 * - - - - - - -   IMMERSION CORP.   - - - - - - - *
 *                                                 *
 *        IBM PC/Compatibles software series       *
 *                Copyright (c) 1993               *
 ***************************************************
 * DRIVE32.C   |   SDK1-2a   |   January 1996
 *
 * Immersion Corp. Software Developer's Kit
 *      Windows for serial communications functions
 *      for the Immersion Corp. MicroScribe-3D
 *      Not for use with the Probe or Personal Digitizer
 *      Requires HCI firmware version MSCR1-1C or later
 */

#include <stdio.h>
#include <windows.h>
#include <string.h>

#include "drive.h"

#define NUM_PORTS       4

/* Number of biostime ticks in one timeout period */
static long     timeout_ticks[NUM_PORTS+1];
static long     stop_tick[NUM_PORTS+1];


/*------------------*/
/* Timing Functions */
/*------------------*/


/* host_pause() pauses for the given number of seconds
 */
void host_pause(float delay_sec)
{
  long int start_tick = (long int)GetTickCount();
  long int stop_tick = start_tick + (long int) (delay_sec * 1000.0);

  if (stop_tick == start_tick) stop_tick++;

  while((long int)GetTickCount() < stop_tick);
}


/* host_get_timeout() gets the timeout period of the given port in seconds
 */
float host_get_timeout(int port)
{
  return 0.001f*timeout_ticks[port];
}


/* host_set_timeout() sets the length of all future timeout periods
 *   to the given # of seconds
 */
void host_set_timeout(int port, float timeout_sec)
{
  timeout_ticks[port] = (long) (timeout_sec * 1000.0);
  if (timeout_ticks == 0L)
    timeout_ticks[port] = 1L;
}


/* host_start_timeout() starts a timer for the specified port.
 *   Call timed_out_yet() to find out whether time is up.
 */
void host_start_timeout(int port)
{
  stop_tick[port] = GetTickCount() + timeout_ticks[port];
}


/* host_timed_out() returns True if the previously-started timeout
 *   period is over.  Returns False if not.
 */
int host_timed_out(int port)
{
  return (stop_tick[port] < (long int)GetTickCount());
}



/*----------------------*/
/* Serial i/o Functions */
/*----------------------*/

  /* the structure to set the baud rates */
typedef struct {
  long rate;
  unsigned int value;
} baud_list_st;

  /* Constants for PC/compatible serial COM */
#if 0
static const baud_list_st baud_list[] = {
  { 38400L, CBR_38400 },
  { 19200L, CBR_19200 },
  { 14400L, CBR_14400 },
  { 9600L, CBR_9600 },
  { 0L, 0 }                                       /* end of list indicator */
};
#endif /* 0 */

static const char *port_names[] = { "COM1", "COM2", "COM3", "COM4" };

  /* I/O buffers and supporting variables */
typedef struct {
  HANDLE handle;         /* the port id from OpenComm */
  long baud;      /* the actual baud rate */
   DCB dcb;             /* the device control block */
} com_port;

/* com port data structure */
com_port com_ports[4];


int host_get_id(int port)
{
  switch (port) {
    case 1 : return (int) com_ports[0].handle;
    case 2 : return (int) com_ports[1].handle;
    case 3 : return (int) com_ports[2].handle;
    case 4 : return (int) com_ports[3].handle;
    default : return (int) NULL;
  }
}


/*--------------------------------*/
/* Fixing up baud rate parameters */
/*--------------------------------*/


/* host_fix_baud() finds nearest valid baud rate to the one given.
 *    Takes small arguments as shorthand:
 *      115 --> 115200, 38 or 384 --> 38400, 96 --> 9600 etc.
 */
void host_fix_baud(long int *baud)
{
  switch(*baud)
  {
    case 115200L:
    case 1152L:
    case 115L:
      *baud = 115200L;
      break;
    case 57600L:
    case 576L:
    case 58L:
    case 57L:
      *baud = 57600L;
      break;
    case 38400L:
    case 384L:
    case 38L:
      *baud = 38400L;
      break;
    case 19200L:
    case 192L:
    case 19L:
      *baud = 19200L;
      break;
    case 14400L:
    case 144L:
    case 14L:
      *baud = 14400L;
      break;
    case 9600L:
    case 96L:
      *baud = 9600L;
      break;
    default:
      if (*baud < 1000L) *baud *= 1000;
      if (*baud > 86400L) *baud = 115200L;
      else if (*baud > 48000L) *baud = 57600L;
      else if (*baud > 28800L) *baud = 38400L;
      else if (*baud > 16800L) *baud = 19200L;
      else if (*baud > 12000L) *baud = 14400L;
      else *baud = 9600L;
      break;
  }
}



/*--------------------------*/
/* Configuring Serial Ports */
/*--------------------------*/


/* host_open_serial() opens the given serial port with specified baud rate
 *    Always uses 8 data bits, 1 stop bit, no parity.
 *    Returns False (zero) if called with zero baud rate.
 */
int host_open_serial(int port, long int baud)
{
  com_port *cport;
  int index;
  int     result = 0;
  char buffer[60];
  COMMTIMEOUTS timeouts;

  /* Calculate baud rate divisor, watch out for div by zero */
  if (baud != 0L) {
      /* Get parameters for the required port */
    switch (port) {
      case 1 :        index = 0;      break;
      case 2 :        index = 1;      break;
      case 3 :        index = 2;      break;
      case 4 :        index = 3;      break;
      default : return result;
    }
    cport = &(com_ports[index]);

      /* open the port */
      /* note -> this is NOT an overlapped file */
    if ((cport->handle = CreateFile(port_names[index],
        GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING,
        0 ,NULL)) == INVALID_HANDLE_VALUE)
      return result;

    if (SetupComm(cport->handle,2048,2048) != TRUE)
      return result;

         /* set some default timeouts */
    GetCommTimeouts(cport->handle,&timeouts);
    timeouts.ReadTotalTimeoutConstant = 10;
    timeouts.ReadTotalTimeoutMultiplier = 5;
    SetCommTimeouts(cport->handle,&timeouts);

      /* build the settings string and DCB */
      /* this is different than the 3.1 BuildCommDCB */
    wsprintf(buffer,"baud=%ld parity=N data=8 stop=1",baud);
    cport->dcb.DCBlength = sizeof(DCB);
    if (BuildCommDCB(buffer,&(cport->dcb)) != TRUE)
      return result;

      /* finish the setup */
    if (SetCommState(cport->handle,&(cport->dcb)) != TRUE)
      return result;

    result = 1;
  }

  return result;
}


/* host_close_serial() closes the given serial port.
 *    NEVER call this without first calling host_open_serial() on the
 *       same port.
 */
void host_close_serial(int port)
{
  com_port *cport;

  switch (port) {
    case 1 : cport = &(com_ports[0]); break;
    case 2 : cport = &(com_ports[1]); break;
    case 3 : cport = &(com_ports[2]); break;
    case 4 : cport = &(com_ports[3]); break;
    default : cport = NULL;
  }

  if (cport != NULL) {
      /* clear the queues */
    PurgeComm(cport->handle,PURGE_TXCLEAR | PURGE_RXCLEAR);

      /* close the port */
    CloseHandle(cport->handle);
  }
}


/* host_flush_serial() flushes and resets the serial i/o buffers
 */
void host_flush_serial(int port)
{
  com_port *cport;

  switch (port) {
    case 1 : cport = &(com_ports[0]); break;
    case 2 : cport = &(com_ports[1]); break;
    case 3 : cport = &(com_ports[2]); break;
    case 4 : cport = &(com_ports[3]); break;
    default : cport = NULL;
  }

  if (cport != NULL) {
    PurgeComm(cport->handle,PURGE_TXCLEAR | PURGE_RXCLEAR);
  }
}



/*------------------*/
/* Input and Output */
/*------------------*/



/* host_read_char() reads one character from the serial input buffer.
 *    returns -1 if input buffer is empty
 */
int host_read_char(int port)
{
  com_port *cport;
  DWORD statusCode, read;
  COMSTAT cs;

   unsigned char ch;

  switch (port) {
    case 1 : cport = &(com_ports[0]); break;
    case 2 : cport = &(com_ports[1]); break;
    case 3 : cport = &(com_ports[2]); break;
    case 4 : cport = &(com_ports[3]); break;
    default : return -1;
  }

  ClearCommError(cport->handle,&statusCode,&cs);
  if (cs.cbInQue > 0 && ReadFile(cport->handle, &ch, 1, &read, NULL) == TRUE && (read > 0))
    return (int) ch;
  else
    return -1;
}


/* host_read_bytes() will try to read a specified number of bytes
 * until the timeout period of time expires.  It returns the number
 * of bytes it actually read.
 */
int host_read_bytes(int port, char *buf, int count, float timeout)
{
  com_port *cport;
  DWORD read;
  COMMTIMEOUTS octo, cto;

  switch (port) {
    case 1 : cport = &(com_ports[0]); break;
    case 2 : cport = &(com_ports[1]); break;
    case 3 : cport = &(com_ports[2]); break;
    case 4 : cport = &(com_ports[3]); break;
    default : return 0;
  }

    /* change the timeouts */
  GetCommTimeouts(cport->handle,&octo);
  cto.ReadTotalTimeoutConstant = (int) (timeout * 1000);
  cto.ReadTotalTimeoutMultiplier = 0;
  SetCommTimeouts(cport->handle,&cto);

  ReadFile(cport->handle,buf,count,&read,NULL);

    /* restore the timeouts */
  SetCommTimeouts(cport->handle,&octo);

  return (int) read;
}


/* host_write_char() writes one character to the serial output buffer
 *    Returns False (zero) if buffer is full
 *    Returns True (non-zero) if successful
 */
int host_write_char(int port, int byt)
{
  com_port *cport;
  unsigned char ch;
  DWORD written;

  switch (port) {
    case 1 : cport = &(com_ports[0]); break;
    case 2 : cport = &(com_ports[1]); break;
    case 3 : cport = &(com_ports[2]); break;
    case 4 : cport = &(com_ports[3]); break;
    default : return 0;
  }

  ch = (unsigned char) byt;
  if (WriteFile(cport->handle,&ch,1,&written,NULL) == TRUE)
    return 1;
  else
    return 0;
}


/* host_write_string() writes a null-terminated string to the output buffer
 *    Returns False (zero) if not enough rooom
 *    Returns True (non-zero) if successful
 */
int host_write_string(int port, char *str)
{
  com_port *cport;
  COMSTAT cs;
  DWORD statusCode, written;

  switch (port) {
    case 1 : cport = &(com_ports[0]); break;
    case 2 : cport = &(com_ports[1]); break;
    case 3 : cport = &(com_ports[2]); break;
    case 4 : cport = &(com_ports[3]); break;
    default : return 0;
  }

  ClearCommError(cport->handle,&statusCode,&cs);
  if (strlen(str) >= (2048 - cs.cbOutQue))
    return 0;
  else if (WriteFile(cport->handle,str,strlen(str),&written,NULL) != TRUE)
    return 0;
  else
    return 1;
}



/*----------------------------*/
/* Getting Serial Port Status */
/*----------------------------*/


/* host_port_valid() returns True if the specified port number is valid
 */
int host_port_valid(int port)
{
  return (port > 0) && (port <= NUM_PORTS);
}


/* host_input_count() returns the number of chars waiting in the input queue
 */
int host_input_count(int port)
{
  COMSTAT cs;
   DWORD status;

  switch (port) {
    case 1 : ClearCommError(com_ports[0].handle,&status,&cs);     break;
    case 2 : ClearCommError(com_ports[1].handle,&status,&cs);     break;
    case 3 : ClearCommError(com_ports[2].handle,&status,&cs);     break;
    case 4 : ClearCommError(com_ports[3].handle,&status,&cs);     break;
    default : return 0;
  }

  return cs.cbInQue;
}


/* host_input_full() tells whether or not the serial input queue is full
 */
int host_input_full(int port)
{
  return ((2048 - host_input_count(port)) == 0);
}
