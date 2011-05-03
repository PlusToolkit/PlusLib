/*=======================================================================

  Program:   NDI Combined API C Interface Library
  Module:    $RCSfile: ndicapi.c,v $
  Creator:   David Gobbi <dgobbi@atamai.com>
  Language:  C
  Author:    $Author: dgobbi $
  Date:      $Date: 2005/07/01 22:52:05 $
  Version:   $Revision: 1.10 $

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

#if defined(_MSC_VER)
#pragma warning ( disable : 4996 )
#endif

#include "ndicapi.h"
#include "ndicapi_thread.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#if defined(__APPLE__)
#include <dirent.h>
#endif /* __APPLE__ */

/*---------------------------------------------------------------------
  Structure for holding ndicapi data.

  This structure is defined in this C file because its members are not
  directly accessed anywhere except for this C file. 
*/

/* max tools is 12 active plus 9 passive, so 24 is a safe number  */
/* (note that we are only counting the number of handles that can */
/* be simultaneously occupied)                                    */
#define NDI_MAX_HANDLES 24

struct ndicapi {

  /* low-level communication information */

  NDIFileHandle serial_device;            /* file handle for ndicapi */
  char *serial_device_name;               /* device name for ndicapi */
  char *serial_command;                   /* text sent to the ndicapi */
  char *serial_reply;                     /* reply from the ndicapi */

  /* this is set to 1 during tracking mode */

  int tracking;                           /* 'is tracking' flag */

  /* low-level threading information */

  int thread_mode;                        /* flag for threading mode */
  NDIThread thread;                       /* the thread handle */
  NDIMutex thread_mutex;                  /* for blocking the thread */
  NDIMutex thread_buffer_mutex;           /* lock the reply buffer */
  NDIEvent thread_buffer_event;           /* for when buffer is updated */
  char *thread_command;                   /* last command sent from thread */
  char *thread_reply;                     /* reply from the ndicapi */
  char *thread_buffer;                    /* buffer for previous reply */
  int thread_error;                       /* error code to go with buffer */

  /* command reply -- this is the return value from plCommand() */

  char *command_reply;                    /* reply without CRC and <CR> */

  /* error handling information */

  int error_code;                         /* error code (zero if no error) */
                                          /* error callback */
  void (*error_callback)(int code, char *description, void *data);
  void *error_callback_data;              /* user data for callback */

  /* GX command reply data */

  char gx_transforms[3][52];              /* 3 active tool transforms */
  char gx_status[8];                      /* tool and system status */
  char gx_information[3][12];             /* extra transform information */
  char gx_single_stray[3][24];            /* one stray marker per tool */
  char gx_frame[3][8];                    /* frame number for each tool */

  char gx_passive_transforms[9][52];      /* 9 passive tool transforms */
  char gx_passive_status[24];             /* tool and system status */
  char gx_passive_information[9][12];     /* extra transform information */
  char gx_passive_frame[9][8];            /* frame number for each tool */

  char gx_passive_stray[424];             /* all passive stray markers */

  /* PSTAT command reply data */

  char pstat_basic[3][32];                /* basic pstat info */
  char pstat_testing[3][8];               /* testing results */
  char pstat_part_number[3][20];          /* part number */
  char pstat_accessories[3][2];           /* accessory information */
  char pstat_marker_type[3][2];           /* marker information */

  char pstat_passive_basic[9][32];        /* basic passive pstat info */
  char pstat_passive_testing[9][8];       /* meaningless info */
  char pstat_passive_part_number[9][20];  /* virtual srom part number */
  char pstat_passive_accessories[9][2];   /* virtual srom accessories */
  char pstat_passive_marker_type[9][2];   /* meaningless for passive */

  /* SSTAT command reply data */

  char sstat_control[2];                  /* control processor status */
  char sstat_sensor[2];                   /* sensor processors status */
  char sstat_tiu[2];                      /* tiu processor status */

  /* IRCHK command reply data */

  int irchk_detected;                     /* irchk detected infrared */
  char irchk_sources[128];                /* coordinates of sources */

  /* PHRQ comand reply data */

  char phrq_reply[2];

  /* PHSR comand reply data */

  char phsr_reply[1284];

  /* PHINF command reply data */

  int phinf_unoccupied;
  char phinf_basic[34];
  char phinf_testing[8];
  char phinf_part_number[20];
  char phinf_accessories[2];
  char phinf_marker_type[2];
  char phinf_port_location[14];
  char phinf_gpio_status[2];

  /* TX command reply data */

  int tx_nhandles;
  unsigned char tx_handles[NDI_MAX_HANDLES];
  char tx_transforms[NDI_MAX_HANDLES][52];
  char tx_status[NDI_MAX_HANDLES][8];
  char tx_frame[NDI_MAX_HANDLES][8];
  char tx_information[NDI_MAX_HANDLES][12];
  char tx_single_stray[NDI_MAX_HANDLES][24];
  char tx_system_status[4];

  int tx_npassive_stray;
  char tx_passive_stray_oov[14];
  char tx_passive_stray[1052];
};

/*---------------------------------------------------------------------
  Prototype for the error helper function, the definition is at the
  end of this file.  A call to this function will both set ndicapi
  error indicator, and will also call the error callback function if
  there is one.  The return value is equal to errnum.
*/
static int ndi_set_error(ndicapi *pol, int errnum);

/*---------------------------------------------------------------------*/
void ndiSetErrorCallback(ndicapi *pol, NDIErrorCallback callback,
                         void *userdata)
{
  pol->error_callback = callback;
  pol->error_callback_data = userdata;
}

/*---------------------------------------------------------------------*/
NDIErrorCallback ndiGetErrorCallback(ndicapi *pol)
{
  return pol->error_callback;
}

/*---------------------------------------------------------------------*/
void *ndiGetErrorCallbackData(ndicapi *pol)
{
  return pol->error_callback_data;
}

/*---------------------------------------------------------------------*/
unsigned long ndiHexToUnsignedLong(const char *cp, int n)
{
  int i;
  unsigned long result = 0;
  int c;

  for (i = 0; i < n; i++) {
    c = cp[i];
    if (c >= 'a' && c <= 'f') {
      result = (result << 4) | (c + (10 - 'a'));
    }
    else if (c >= 'A' && c <= 'F') {
      result = (result << 4) | (c + (10 - 'A'));
    }
    else if (c >= '0' && c <= '9') {
      result = (result << 4) | (c - '0');
    }
    else {
      break;
    }
  }

  return result;
}

/*---------------------------------------------------------------------*/
long ndiSignedToLong(const char *cp, int n)
{
  int i;
  int c;
  long result = 0;
  int sign = 1;

  c = cp[0];

  if (c == '+') {
    sign = 1;
  }
  else if (c == '-') {
    sign = -1;
  }
  else {
    return 0;
  }

  for (i = 1; i < n; i++) {
    c = cp[i];
    if (c >= '0' && c <= '9') {
      result = (result * 10) + (c - '0');
    }
    else {
      break;
    }
  }

  return sign*result;
}

/*---------------------------------------------------------------------*/
char *ndiHexEncode(char *cp, const void *data, int n)
{
  const unsigned char *bdata;
  int i, c1, c2;
  unsigned int d;
  char *tcp;

  bdata = (const unsigned char *)data;
  tcp = cp;

  for (i = 0; i < n; i++) {
    d = bdata[i];
    c1 = (d & 0xf0) >> 4;
    c2 = (d & 0x0f);
    c1 += '0';
    c2 += '0';
    if (c1 > '9') {
      c1 += ('A' - '0' - 10);
    }
    if (c2 > '9') {
      c2 += ('A' - '0' - 10);
    }
    *tcp++ = c1;
    *tcp++ = c2;
  }

  return cp;
}

/*---------------------------------------------------------------------*/
void *ndiHexDecode(void *data, const char *cp, int n)
{
  unsigned char *bdata;
  int i, c1, c2;
  unsigned int d;

  bdata = (unsigned char *)data;

  for (i = 0; i < n; i++) {
    d = 0;
    c1 = *cp++;
    if (c1 >= 'a' && c1 <= 'f') {
      d = (c1 + (10 - 'a'));
    }
    else if (c1 >= 'A' && c1 <= 'F') {
      d = (c1 + (10 - 'A'));
    }
    else if (c1 >= '0' && c1 <= '9') {
      d = (c1 - '0');
    }
    c2 = *cp++;
    d <<= 4;
    if (c2 >= 'a' && c2 <= 'f') {
      d |= (c2 + (10 - 'a'));
    }
    else if (c2 >= 'A' && c2 <= 'F') {
      d |= (c2 + (10 - 'A'));
    }
    else if (c2 >= '0' && c2 <= '9') {
      d |= (c2 - '0');
    }
    
    bdata[i] = d;
  }

  return data;
}

/*---------------------------------------------------------------------*/
int ndiPVWRFromFile(ndicapi *pol, int port, char *filename)
{
  unsigned char buffer[1024];
  char hexdata[128];
  FILE *file;
  int addr;

  pol->error_code = 0;

  file = fopen(filename,"rb");
  if (file == NULL) {
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
    ndiPVWR(pol, port, addr, ndiHexEncode(hexdata, &buffer[addr], 64));
    if (ndiGetError(pol) != NDI_OKAY) {
      return -1;
    }
  }
 
  return 0;
}

/*---------------------------------------------------------------------*/
int ndiGetError(ndicapi *pol)
{
  return pol->error_code;
}

/*---------------------------------------------------------------------*/
char *ndiErrorString(int errnum)
{
  static char *textarray_low[] = /* values from 0x01 to 0x21 */ 
  {
    "No error",
    "Invalid command",
    "Command too long",
    "Command too short",
    "Invalid CRC calculated for command",
    "Time-out on command execution",
    "Unable to set up new communication parameters",
    "Incorrect number of command parameters",
    "Invalid port handle selected",
    "Invalid tracking priority selected (must be S, D or B)",
    "Invalid LED selected",
    "Invalid LED state selected (must be B, F or S)",
    "Command is invalid while in the current mode",
    "No tool assigned to the selected port handle",
    "Selected port handle not initialized",
    "Selected port handle not enabled",
    "System not initialized",
    "Unable to stop tracking",
    "Unable to start tracking",
    "Unable to initialize Tool-in-Port",
    "Invalid Position Sensor or Field Generator characterization parameters",
    "Unable to initialize the Measurement System",
    "Unable to start diagnostic mode",
    "Unable to stop diagnostic mode",
    "Unable to determine environmental infrared or magnetic interference",
    "Unable to read device's firmware version information",
    "Internal Measurement System error",
    "Unable to initialize for environmental infrared diagnostics",
    "Unable to set marker firing signature",
    "Unable to search for SROM IDs",
    "Unable to read SROM data",
    "Unable to write SROM data",
    "Unable to select SROM",
    "Unable to perform tool current test",
    "Unable to find camera parameters from the selected volume for the"
    " wavelength of a tool enabled for tracking",
    "Command parameter out of range",
    "Unable to select parameters by volume",
    "Unable to determine Measurement System supported features list",
    "Reserved - Unrecognized Error 0x26",
    "Reserved - Unrecognized Error 0x27",
    "SCU hardware has changed state; a card has been removed or added",
    "Main processor firmware corrupt",
    "No memory available for dynamic allocation (heap is full)",
    "Requested handle has not been allocated",
    "Requested handle has become unoccupied",
    "All handles have been allocated",
    "Invalid port description",
    "Requested port already assigned to a port handle",
    "Invalid input or output state",    
  };

  static char *textarray_high[] = /* values from 0xf6 to 0xf4 */
  {
    "Too much environmental infrared",
    "Unrecognized error code",
    "Unrecognized error code",
    "Unable to read Flash EPROM",
    "Unable to write Flash EPROM",
    "Unable to erase Flash EPROM"
  };

  static char *textarray_api[] = /* values specific to the API */
  {
    "Bad CRC on reply from Measurement System",
    "Error opening serial connection",
    "Host not capable of given communications parameters",
    "Device->host communication timeout",
    "Serial port write error",
    "Serial port read error",
    "Measurement System failed to reset on break",
    "Measurement System not found on specified port"
  };

  if (errnum >= 0x00 && errnum <= 0x31) {
    return textarray_low[errnum];
  }
  else if (errnum <= 0xf6 && errnum >= 0xf1) {
    return textarray_high[errnum-0xf1];
  }
  else if (errnum >= 0x100 && errnum <= 0x700) {
    return textarray_api[(errnum >> 8)-1];
  }

  return "Unrecognized error code";
}

/*---------------------------------------------------------------------*/
char *ndiDeviceName(int i)
{
#if defined(__APPLE__)
  static char devicenames[4][255+6];
  DIR *dirp;
  struct dirent *ep;
  int j = 0;
  
  dirp = opendir("/dev/");
  if (dirp == NULL) {
    return NULL;
  }

  while ((ep = readdir(dirp)) != NULL && j < 4) {
    if (ep->d_name[0] == 'c' && ep->d_name[1] == 'u' &&
        ep->d_name[2] == '.')
    {
      if (j == i) {
        strncpy(devicenames[j],"/dev/",5);
        strncpy(devicenames[j]+5,ep->d_name,255);
        devicenames[j][255+5] == '\0';
        closedir(dirp);
        return devicenames[j];
      }
      j++;
    }
  }
  closedir(dirp);
  return NULL;  
#endif /* __APPLE__ */

#ifdef NDI_DEVICE0
  if (i == 0) { return NDI_DEVICE0; }
#endif
#ifdef NDI_DEVICE1
  if (i == 1) { return NDI_DEVICE1; }
#endif
#ifdef NDI_DEVICE2
  if (i == 2) { return NDI_DEVICE2; }
#endif
#ifdef NDI_DEVICE3
  if (i == 3) { return NDI_DEVICE3; }
#endif
#ifdef NDI_DEVICE4
  if (i == 4) { return NDI_DEVICE4; }
#endif
#ifdef NDI_DEVICE5
  if (i == 5) { return NDI_DEVICE5; }
#endif
#ifdef NDI_DEVICE6
  if (i == 6) { return NDI_DEVICE6; }
#endif
#ifdef NDI_DEVICE7
  if (i == 7) { return NDI_DEVICE7; }
#endif

  return NULL;
}

/*---------------------------------------------------------------------*/
int ndiProbe(const char *device)
{
  char reply[1024]; 
  char init_reply[16];
  NDIFileHandle serial_port;
  int n;

  serial_port = ndiSerialOpen(device);
  if (serial_port == NDI_INVALID_HANDLE) {
    return NDI_OPEN_ERROR;
  }

  /* check DSR line to see whether any device is connected */
  if (!ndiSerialCheckDSR(serial_port)) {
    ndiSerialClose(serial_port);
    return NDI_PROBE_FAIL;
  }

  /* set comm parameters to default, but decrease timeout to 0.1s */
  if (ndiSerialComm(serial_port, 9600, "8N1", 0) < 0 ||
      ndiSerialTimeout(serial_port, 100) < 0) {
    ndiSerialClose(serial_port);
    return NDI_BAD_COMM;
  }

  /* flush the buffers (which are unlikely to contain anything) */
  ndiSerialFlush(serial_port, NDI_IOFLUSH);

  /* try to initialize ndicapi */
  if (ndiSerialWrite(serial_port, "INIT:E3A5\r", 10) < 10 ||
      ndiSerialSleep(serial_port, 100) < 0 ||
      ndiSerialRead(serial_port, init_reply, 16) <= 0 ||
      strncmp(init_reply, "OKAYA896\r", 9) != 0) {

    /* increase timeout to 5 seconds for reset */
    ndiSerialTimeout(serial_port, 5000);

    /* init failed: flush, reset, and try again */
    ndiSerialFlush(serial_port, NDI_IOFLUSH);
    if (ndiSerialFlush(serial_port, NDI_IOFLUSH) < 0 ||
        ndiSerialBreak(serial_port)) {
      ndiSerialClose(serial_port);
      return NDI_BAD_COMM;
    }

    n = ndiSerialRead(serial_port, init_reply, 16);
    if (n < 0) {
      ndiSerialClose(serial_port);
      return NDI_READ_ERROR;
    }
    else if (n == 0) {
      ndiSerialClose(serial_port);
      return NDI_TIMEOUT;
    }

    /* check reply from reset */
    if (strncmp(init_reply, "RESETBE6F\r", 10) != 0)  {
      ndiSerialClose(serial_port);
      return NDI_PROBE_FAIL;
    }
    /* try to initialize a second time */
    ndiSerialSleep(serial_port, 100);
    n = ndiSerialWrite(serial_port, "INIT:E3A5\r", 10);
    if (n < 0) {
      ndiSerialClose(serial_port);
      return NDI_WRITE_ERROR;
    }
    else if (n < 10) {
      ndiSerialClose(serial_port);
      return NDI_TIMEOUT;
    }
      
    ndiSerialSleep(serial_port, 100);
    n = ndiSerialRead(serial_port, init_reply, 16);
    if (n < 0) {
      ndiSerialClose(serial_port);
      return NDI_READ_ERROR;
    }
    else if (n == 0) {
      ndiSerialClose(serial_port);
      return NDI_TIMEOUT;
    }

    if (strncmp(init_reply, "OKAYA896\r", 9) != 0) {
      ndiSerialClose(serial_port);
      return NDI_PROBE_FAIL;
    }
  }

  /* use VER command to verify that this is a NDI device */
  ndiSerialSleep(serial_port, 100);
  if (ndiSerialWrite(serial_port, "VER:065EE\r", 10) < 10 ||
      (n = ndiSerialRead(serial_port, reply, 1023)) < 7) {
    ndiSerialClose(serial_port);
    return NDI_PROBE_FAIL;
  }

  /* restore things back to the way they were */
  ndiSerialClose(serial_port);

  return NDI_OKAY;
}
   
/*---------------------------------------------------------------------*/
ndicapi *ndiOpen(const char *device)
{
  NDIFileHandle serial_port;
  ndicapi *pol;

  serial_port = ndiSerialOpen(device);

  if (serial_port == NDI_INVALID_HANDLE) {
    return NULL;
  }

  if (ndiSerialComm(serial_port, 9600, "8N1", 0) < 0) {
    ndiSerialClose(serial_port);
    return NULL;
  }

  if (ndiSerialFlush(serial_port, NDI_IOFLUSH) < 0) {
    ndiSerialClose(serial_port);
    return NULL;
  }

  pol = (ndicapi *)malloc(sizeof(ndicapi));

  if (pol == 0) {
    ndiSerialClose(serial_port);
    return NULL;
  }
  
  memset(pol, 0, sizeof(ndicapi));
  pol->serial_device = serial_port;
  
  /* allocate the buffers */
  pol->serial_device_name = (char *)malloc(strlen(device)+1);
  pol->serial_command = (char *)malloc(2048);
  pol->serial_reply = (char *)malloc(2048);
  pol->command_reply = (char *)malloc(2048);

  if (pol->serial_device_name == 0 ||
      pol->serial_command == 0 ||
      pol->serial_reply == 0 ||
      pol->command_reply == 0) {

    if (pol->serial_device_name) {
      free(pol->serial_device_name);
    }
    if (pol->serial_command) {
      free(pol->serial_command);
    }
    if (pol->serial_reply) {
      free(pol->serial_reply);
    }
    if (pol->command_reply) {
      free(pol->command_reply);
    }

    ndiSerialClose(serial_port);
    return NULL;
  }

  /* initialize the allocated memory */
  strcpy(pol->serial_device_name, device);
  memset(pol->serial_command, 0, 2048);
  memset(pol->serial_reply, 0, 2048);
  memset(pol->command_reply, 0, 2048);

  return pol;
}

/*---------------------------------------------------------------------*/
char *ndiGetDeviceName(ndicapi *pol)
{
  return pol->serial_device_name;
}

/*---------------------------------------------------------------------*/
NDIFileHandle ndiGetDeviceHandle(ndicapi *pol)
{
  return pol->serial_device;
}

/*---------------------------------------------------------------------*/
void ndiClose(ndicapi *pol)
{
  /* end the thracking thread if it is running */
  ndiSetThreadMode(pol, 0);

  /* close the serial port */
  ndiSerialClose(pol->serial_device);

  /* free the buffers */
  free(pol->serial_device_name);
  free(pol->serial_command);
  free(pol->serial_reply);
  free(pol->command_reply);

  free(pol);
}

/*---------------------------------------------------------------------*/
/* the CalcCRC16 function is taken from the NDI ndicapi documentation  */
/*****************************************************************
Name:                   CalcCRC16

Input Values:
    int
        data        :Data value to add to running CRC16.
    unsigned int
        *puCRC16    :Ptr. to running CRC16.

Output Values:
    None.

Returned Value:
    None.

Description:
    This routine calculates a running CRC16 using the polynomial
    X^16 + X^15 + X^2 + 1.

*****************************************************************/
static const int oddparity[16] =    { 0, 1, 1, 0, 1, 0, 0, 1,
                                      1, 0, 0, 1, 0, 1, 1, 0 };

#define CalcCRC16(nextchar, puCRC16) \
{ \
    int data; \
    data = nextchar; \
    data = (data ^ (*(puCRC16) & 0xff)) & 0xff; \
    *puCRC16 >>= 8; \
    if ( oddparity[data & 0x0f] ^ oddparity[data >> 4] ) { \
      *(puCRC16) ^= 0xc001; \
    } /* if */ \
    data <<= 6; \
    *puCRC16 ^= data; \
    data <<= 1; \
    *puCRC16 ^= data; \
} /* CalcCRC16 */ 

/*---------------------------------------------------------------------
  Prototypes for helper functions for certain commands.  These functions
  are called whenever the corresponding command sent the Measurement System
  unless an error was generated.

  cp  -> the command string that was sent to the NDICAPI
  crp -> the reply from the Measurement System, but with the CRC hacked off 
*/

static void ndi_COMM_helper(ndicapi *pol, const char *cp, const char *crp);
static void ndi_PHINF_helper(ndicapi *pol, const char *cp, const char *crp);
static void ndi_PHSR_helper(ndicapi *pol, const char *cp, const char *crp);
static void ndi_TX_helper(ndicapi *pol, const char *cp, const char *crp);
static void ndi_GX_helper(ndicapi *pol, const char *cp, const char *crp);
static void ndi_INIT_helper(ndicapi *pol, const char *cp, const char *crp);
static void ndi_IRCHK_helper(ndicapi *pol, const char *cp, const char *crp);
static void ndi_PSTAT_helper(ndicapi *pol, const char *cp, const char *crp);
static void ndi_SSTAT_helper(ndicapi *pol, const char *cp, const char *crp);
static void ndi_PHRQ_helper(ndicapi *pol, const char *cp, const char *crp);

/*---------------------------------------------------------------------*/
char *ndiCommand(ndicapi *pol, const char *format, ...)
{
  char *reply;
  va_list ap;            /* see stdarg.h */
  va_start(ap,format);

  reply = ndiCommandVA(pol, format, ap);

  va_end(ap);

  return reply;
}

/*---------------------------------------------------------------------*/
char *ndiCommandVA(ndicapi *pol, const char *format, va_list ap)
{
  int i, m, nc;
  unsigned int CRC16 = 0;
  int use_crc = 0;
  int in_command = 1;
  char *cp, *rp, *crp;

  cp = pol->serial_command;      /* text sent to ndicapi */
  rp = pol->serial_reply;        /* text received from ndicapi */
  crp = pol->command_reply;      /* received text, with CRC hacked off */
  nc = 0;                        /* length of 'command' part of command */

  pol->error_code = 0;           /* clear error */
  cp[0] = '\0';
  rp[0] = '\0';
  crp[0] = '\0';

  /* verify that the serial device was opened */
  if (pol->serial_device == NDI_INVALID_HANDLE) {
    ndi_set_error(pol, NDI_OPEN_ERROR);
    return crp;
  }

  /* if the command is NULL, send a break to reset the Measurement System */
  if (format == NULL) {

    if (pol->thread_mode && pol->tracking) {
      /* block the tracking thread */
      ndiMutexLock(pol->thread_mutex);
    }
    pol->tracking = 0;

    ndiSerialComm(pol->serial_device, 9600, "8N1", 0);
    ndiSerialFlush(pol->serial_device, NDI_IOFLUSH);
    ndiSerialBreak(pol->serial_device);
    m = ndiSerialRead(pol->serial_device, rp, 2047);

    /* check for correct reply */
    if (strncmp(rp, "RESETBE6F\r", 8) != 0)  { 
      ndi_set_error(pol, NDI_RESET_FAIL);
      return crp;
    }

    /* terminate the reply string */
    rp[m] = '\0';
    m -= 5;
    strncpy(crp, rp, m);
    crp[m] = '\0';
    
    /* return the reply string, minus the CRC */
    return crp;
  }

  vsprintf(cp, format, ap);                   /* format parameters */

  CRC16 = 0;                                  /* calculate CRC */
  for (i = 0; cp[i] != '\0'; i++) {
    CalcCRC16(cp[i], &CRC16);
    if (in_command && cp[i] == ':') {         /* only use CRC if a ':' */
      use_crc = 1;                            /*  follows the command  */
    }
    if (in_command &&
        !((cp[i] >= 'A' && cp[i] <= 'Z') || 
          (cp[i] >= '0' && cp[i] <= '9'))) {
      in_command = 0;                         /* 'command' part has ended */
      nc = i;                                 /* command length */
    }
  }

  if (use_crc) {
    sprintf(&cp[i], "%04X", CRC16);           /* tack on the CRC */
    i += 4;
  }

  cp[i] = '\0';

  cp[i++] = '\r';                             /* tack on carriage return */
  cp[i] = '\0';                               /* terminate for good luck */

  /* if the command is GX and thread_mode is on, we copy the reply from
     the thread rather than getting it directly from the Measurement System */
  if (pol->thread_mode && pol->tracking && 
      nc == 2 && (cp[0] == 'G' && cp[1] == 'X' ||
                  cp[0] == 'T' && cp[1] == 'X' ||
                  cp[0] == 'B' && cp[1] == 'X')) {
    int errcode = 0;

    /* check that the thread is sending the GX command that we want */
    if (strcmp(cp, pol->thread_command) != 0) {
      /* tell thread to start using the new GX command */
      ndiMutexLock(pol->thread_mutex);
      strcpy(pol->thread_command, cp);
      ndiMutexUnlock(pol->thread_mutex);
      /* wait for the next data record to arrive (we have to throw it away) */
      if (ndiEventWait(pol->thread_buffer_event, 5000)) {
        ndi_set_error(pol, NDI_TIMEOUT);
        return crp;
      }
    }
    /* there is usually no wait, because usually new data is ready */
    if (ndiEventWait(pol->thread_buffer_event, 5000)) {
      ndi_set_error(pol, NDI_TIMEOUT);
      return crp;
    }
    /* copy the thread's reply buffer into the main reply buffer */
    ndiMutexLock(pol->thread_buffer_mutex);
    for (m = 0; pol->thread_buffer[m] != '\0'; m++) {
      rp[m] = pol->thread_buffer[m];
    }
    rp[m] = '\0';   /* terminate string */
    errcode = pol->thread_error;
    ndiMutexUnlock(pol->thread_buffer_mutex);

    if (errcode != 0) {
      ndi_set_error(pol, errcode);
      return crp;
    }
  }
  /* if the command is not a GX or thread_mode is not on, then
     send the command directly to the Measurement System and get a reply */
  else {
    int errcode = 0;
    int thread_mode;

    /* guard against pol->thread_mode changing while mutex is locked */
    thread_mode = pol->thread_mode;

    if (thread_mode && pol->tracking) {
      /* block the tracking thread while we slip this command through */
      ndiMutexLock(pol->thread_mutex);
    }

    /* change  pol->tracking  if either TSTOP or TSTART is sent  */ 
    if ((nc == 5 && strncmp(cp, "TSTOP", nc) == 0) ||
        (nc == 4 && strncmp(cp, "INIT", nc) == 0)) {
      pol->tracking = 0;
    }
    else if (nc == 6 && strncmp(cp, "TSTART", nc) == 0) {
      pol->tracking = 1;
      if (thread_mode) {
        /* this will force the thread to wait until the application
           sends the first GX command */
        pol->thread_command[0] = '\0';
      }
    }

    /* flush the input buffer, because anything that we haven't read
       yet is garbage left over by a previously failed command */
    ndiSerialFlush(pol->serial_device, NDI_IFLUSH);

    /* send the command to the Measurement System */
    if (errcode == 0) {
      m = ndiSerialWrite(pol->serial_device, cp, i);
      if (m < 0) {
        errcode = NDI_WRITE_ERROR;
      }
      else if (m < i) {
        errcode = NDI_TIMEOUT;
      }
    }

    /* read the reply from the Measurement System */
    m = 0;
    if (errcode == 0) {
      m = ndiSerialRead(pol->serial_device, rp, 2047);
      if (m < 0) {
        errcode = NDI_WRITE_ERROR;
        m = 0;
      }
      else if (m == 0) {
        errcode = NDI_TIMEOUT;
      }
      rp[m] = '\0';   /* terminate string */
    }

    if (thread_mode & pol->tracking) {
      /* unblock the tracking thread */
      /* fprintf(stderr,"unlock\n"); */
      ndiMutexUnlock(pol->thread_mutex);
      /* fprintf(stderr,"unlocked\n"); */
    }

    if (errcode != 0) {
      ndi_set_error(pol, errcode);
      return crp;
    }
  }

  /* back up to before the CRC */
  m -= 5;
  if (m < 0) {
    ndi_set_error(pol, NDI_BAD_CRC);
    return crp;
  }

  /* calculate the CRC and copy serial_reply to command_reply */
  CRC16 = 0;
  for (i = 0; i < m; i++) {
    CalcCRC16(rp[i], &CRC16);
    crp[i] = rp[i];
  }

  /* terminate command_reply before the CRC */
  crp[i] = '\0';           

  /* read and check the CRC value of the reply */
  if (CRC16 != ndiHexToUnsignedLong(&rp[m], 4)) {
    ndi_set_error(pol, NDI_BAD_CRC);
    return crp;
  }

  /* check for error code */
  if (crp[0] == 'E' && strncmp(crp, "ERROR", 5) == 0)  {
    ndi_set_error(pol, ndiHexToUnsignedLong(&crp[5], 2));
    return crp;
  }

  /*----------------------------------------*/
  /* special behavior for specific commands */

  if (cp[0] == 'T' && cp[1] == 'X' && nc == 2) { /* the TX command */
    ndi_TX_helper(pol, cp, crp);
  }
  else if (cp[0] == 'G' && cp[1] == 'X' && nc == 2) { /* the GX command */
    ndi_GX_helper(pol, cp, crp);
  }
  else if (cp[0] == 'C' && nc == 4 && strncmp(cp, "COMM", nc) == 0) {
    ndi_COMM_helper(pol, cp, crp);
  }
  else if (cp[0] == 'I' && nc == 4 && strncmp(cp, "INIT", nc) == 0) {
    ndi_INIT_helper(pol, cp, crp);
  }
  else if (cp[0] == 'I' && nc == 5 && strncmp(cp, "IRCHK", nc) == 0) {
    ndi_IRCHK_helper(pol, cp, crp);
  }
  else if (cp[0] == 'P' && nc == 5 && strncmp(cp, "PHINF", nc) == 0) {
    ndi_PHINF_helper(pol, cp, crp);
  }
  else if (cp[0] == 'P' && nc == 4 && strncmp(cp, "PHRQ", nc) == 0) {
    ndi_PHRQ_helper(pol, cp, crp);
  }
  else if (cp[0] == 'P' && nc == 4 && strncmp(cp, "PHSR", nc) == 0) {
    ndi_PHSR_helper(pol, cp, crp);
  }
  else if (cp[0] == 'P' && nc == 5 && strncmp(cp, "PSTAT", nc) == 0) {
    ndi_PSTAT_helper(pol, cp, crp);
  }
  else if (cp[0] == 'S' && nc == 5 && strncmp(cp, "SSTAT", nc) == 0) {
    ndi_SSTAT_helper(pol, cp, crp);
  }

  /* return the Measurement System reply, but with the CRC hacked off */
  return crp;
}

/*---------------------------------------------------------------------*/
int ndiGetPHINFPortStatus(ndicapi *pol)
{
  char *dp;

  dp = &pol->phinf_basic[31];

  return (int)ndiHexToUnsignedLong(dp, 2);  
}

/*---------------------------------------------------------------------*/
int ndiGetPHINFToolInfo(ndicapi *pol, char information[31])
{
  char *dp;
  int i;
  
  dp = pol->phinf_basic;

  for (i = 0; i < 31; i++) {
    information[i] = *dp++;
  }

  return pol->phinf_unoccupied;
}

/*---------------------------------------------------------------------*/
unsigned long ndiGetPHINFCurrentTest(ndicapi *pol)
{
  char *dp;

  dp = pol->phinf_testing;

  return (int)ndiHexToUnsignedLong(dp, 8);  
}

/*---------------------------------------------------------------------*/
int ndiGetPHINFPartNumber(ndicapi *pol, char part[20])
{
  char *dp;
  int i;
  
  dp = pol->phinf_part_number;

  for (i = 0; i < 20; i++) {
    part[i] = *dp++;
  }

  return pol->phinf_unoccupied;
}

/*---------------------------------------------------------------------*/
int ndiGetPHINFAccessories(ndicapi *pol)
{
  char *dp;

  dp = pol->phinf_accessories;

  return (int)ndiHexToUnsignedLong(dp, 2);  
}

/*---------------------------------------------------------------------*/
int ndiGetPHINFMarkerType(ndicapi *pol)
{
  char *dp;

  dp = pol->phinf_marker_type;

  return (int)ndiHexToUnsignedLong(dp, 2);  
}

/*---------------------------------------------------------------------*/
int ndiGetPHINFPortLocation(ndicapi *pol, char location[14])
{
  char *dp;
  int i;
  
  dp = pol->phinf_port_location;

  for (i = 0; i < 14; i++) {
    location[i] = *dp++;
  }

  return pol->phinf_unoccupied;
}

/*---------------------------------------------------------------------*/
int ndiGetPHINFGPIOStatus(ndicapi *pol)
{
  char *dp;

  dp = pol->phinf_gpio_status;

  return (int)ndiHexToUnsignedLong(dp, 2);  
}

/*---------------------------------------------------------------------*/
int ndiGetPHRQHandle(ndicapi *pol)
{
  char *dp;

  dp = pol->phrq_reply;
  return (int)ndiHexToUnsignedLong(dp, 2);
}

/*---------------------------------------------------------------------*/
int ndiGetPHSRNumberOfHandles(ndicapi *pol)
{
  char *dp;

  dp = pol->phsr_reply;

  return (int)ndiHexToUnsignedLong(dp, 2);  
}

/*---------------------------------------------------------------------*/
int ndiGetPHSRHandle(ndicapi *pol, int i)
{
  char *dp;
  int n;

  dp = pol->phsr_reply;
  n = (int)ndiHexToUnsignedLong(dp, 2);
  dp += 2;

  if (i < 0 || i > n)
    {
    return 0;
    }
  dp += 5*i;

  return (int)ndiHexToUnsignedLong(dp, 2);  
}

/*---------------------------------------------------------------------*/
int ndiGetPHSRInformation(ndicapi *pol, int i)
{
  char *dp;
  int n;

  dp = pol->phsr_reply;
  n = (int)ndiHexToUnsignedLong(dp, 2);
  dp += 2;

  if (i < 0 || i > n)
    {
    return 0;
    }
  dp += 5*i + 2;

  return (int)ndiHexToUnsignedLong(dp, 3);  
}

/*---------------------------------------------------------------------*/
int ndiGetTXTransform(ndicapi *pol, int ph, double transform[8])
{
  char *dp;
  int i, n;
  
  n = pol->tx_nhandles;
  for (i = 0; i < n; i++) {
    if (pol->tx_handles[i] == ph) {
      break;
    }
  }
  if (i == n) {
    return NDI_DISABLED;
  }

  dp = pol->tx_transforms[i];
  if (*dp == 'D' || *dp == '\0') {
    return NDI_DISABLED;
  }
  else if (*dp == 'M') {
    return NDI_MISSING;
  }
  
  transform[0] = ndiSignedToLong(&dp[0],  6)*0.0001;
  transform[1] = ndiSignedToLong(&dp[6],  6)*0.0001;
  transform[2] = ndiSignedToLong(&dp[12], 6)*0.0001;
  transform[3] = ndiSignedToLong(&dp[18], 6)*0.0001;
  transform[4] = ndiSignedToLong(&dp[24], 7)*0.01;
  transform[5] = ndiSignedToLong(&dp[31], 7)*0.01;
  transform[6] = ndiSignedToLong(&dp[38], 7)*0.01;
  transform[7] = ndiSignedToLong(&dp[45], 6)*0.0001;

  return NDI_OKAY;
}

/*---------------------------------------------------------------------*/
int ndiGetTXPortStatus(ndicapi *pol, int ph)
{
  char *dp;
  int i, n;
  
  n = pol->tx_nhandles;
  for (i = 0; i < n; i++) {
    if (pol->tx_handles[i] == ph) {
      break;
    }
  }
  if (i == n) {
    return 0;
  }

  dp = pol->tx_status[i];

  return (int)ndiHexToUnsignedLong(dp, 8);
}

/*---------------------------------------------------------------------*/
unsigned long ndiGetTXFrame(ndicapi *pol, int ph)
{
  char *dp;
  int i, n;

  n = pol->tx_nhandles;
  for (i = 0; i < n; i++) {
    if (pol->tx_handles[i] == ph) {
      break;
    }
  }
  if (i == n) {
    return 0;
  }

  dp = pol->tx_frame[i];

  return (unsigned long)ndiHexToUnsignedLong(dp, 8);
}

/*---------------------------------------------------------------------*/
int ndiGetTXToolInfo(ndicapi *pol, int ph)
{
  char *dp;
  int i, n;
  
  n = pol->tx_nhandles;
  for (i = 0; i < n; i++) {
    if (pol->tx_handles[i] == ph) {
      break;
    }
  }
  if (i == n) {
    return 0;
  }

  dp = pol->tx_information[i];
  return (int)ndiHexToUnsignedLong(dp, 2);
}

/*---------------------------------------------------------------------*/
int ndiGetTXMarkerInfo(ndicapi *pol, int ph, int marker)
{
  char *dp;
  int i, n;
  
  n = pol->tx_nhandles;
  for (i = 0; i < n; i++) {
    if (pol->tx_handles[i] == ph) {
      break;
    }
  }
  if (i == n || marker < 0 || marker >= 20) {
    return NDI_DISABLED;
  }

  dp = pol->tx_information[2+marker];
  return (int)ndiHexToUnsignedLong(dp, 1);
}

/*---------------------------------------------------------------------*/
int ndiGetTXSingleStray(ndicapi *pol, int ph, double coord[3])
{
  char *dp;
  int i, n;
  
  n = pol->tx_nhandles;
  for (i = 0; i < n; i++) {
    if (pol->tx_handles[i] == ph) {
      break;
    }
  }
  if (i == n) {
    return NDI_DISABLED;
  }

  dp = pol->tx_single_stray[i];
  if (*dp == 'D' || *dp == '\0') {
    return NDI_DISABLED;
  }
  else if (*dp == 'M') {
    return NDI_MISSING;
  }

  coord[0] = ndiSignedToLong(&dp[0],  7)*0.01;
  coord[1] = ndiSignedToLong(&dp[7],  7)*0.01;
  coord[2] = ndiSignedToLong(&dp[14], 7)*0.01;

  return NDI_OKAY;
}

/*---------------------------------------------------------------------*/
int ndiGetTXNumberOfPassiveStrays(ndicapi *pol)
{
  return pol->tx_npassive_stray; 
}

/*---------------------------------------------------------------------*/
int ndiGetTXPassiveStray(ndicapi *pol, int i, double coord[3])
{
  const char *dp;
  int n;

  dp = pol->tx_passive_stray;

  if (*dp == '\0') {
    return NDI_DISABLED;
  }

  n = pol->tx_npassive_stray;
  dp += 3;
  if (n < 0) {
    return NDI_MISSING;
  }
  if (n > 20) {
    n = 20;
  }

  if (i < 0 || i >= n) {
    return NDI_MISSING;
  }

  dp += 7*3*i;
  coord[0] = ndiSignedToLong(&dp[0],  7)*0.01;
  coord[1] = ndiSignedToLong(&dp[7],  7)*0.01;
  coord[2] = ndiSignedToLong(&dp[14], 7)*0.01;

  return NDI_OKAY;
}

/*---------------------------------------------------------------------*/
int ndiGetTXSystemStatus(ndicapi *pol)
{
  char *dp;

  dp = pol->tx_system_status;

  return (int)ndiHexToUnsignedLong(dp, 4);
}

/*---------------------------------------------------------------------*/
int ndiGetGXTransform(ndicapi *pol, int port, double transform[8])
{
  char *dp;
  
  if (port >= '1' && port <= '3') {
    dp = pol->gx_transforms[port - '1'];
  }
  else if (port >= 'A' && port <= 'I') {
    dp = pol->gx_passive_transforms[port - 'A'];
  }
  else {
    return NDI_DISABLED;
  }

  if (*dp == 'D' || *dp == '\0') {
    return NDI_DISABLED;
  }
  else if (*dp == 'M') {
    return NDI_MISSING;
  }

  transform[0] = ndiSignedToLong(&dp[0],  6)*0.0001;
  transform[1] = ndiSignedToLong(&dp[6],  6)*0.0001;
  transform[2] = ndiSignedToLong(&dp[12], 6)*0.0001;
  transform[3] = ndiSignedToLong(&dp[18], 6)*0.0001;
  transform[4] = ndiSignedToLong(&dp[24], 7)*0.01;
  transform[5] = ndiSignedToLong(&dp[31], 7)*0.01;
  transform[6] = ndiSignedToLong(&dp[38], 7)*0.01;
  transform[7] = ndiSignedToLong(&dp[45], 6)*0.0001;

  return NDI_OKAY;
}

/*---------------------------------------------------------------------*/
int ndiGetGXPortStatus(ndicapi *pol, int port)
{
  char *dp;

  if (port >= '1' && port <= '3') {
    dp = &pol->gx_status[6 - 2*(port - '1')];
  }
  else if (port >= 'A' && port <= 'C') {
    dp = &pol->gx_passive_status[6 - 2*(port - 'A')];
  }
  else if (port >= 'D' && port <= 'F') {
    dp = &pol->gx_passive_status[14 - 2*(port - 'D')];
  }
  else if (port >= 'G' && port <= 'I') {
    dp = &pol->gx_passive_status[22 - 2*(port - 'G')];
  }
  else {
    return 0;
  }

  return (int)ndiHexToUnsignedLong(dp, 2);
}

/*---------------------------------------------------------------------*/
int ndiGetGXSystemStatus(ndicapi *pol)
{
  char *dp;

  dp = pol->gx_status;

  if (*dp == '\0') {
    dp = pol->gx_passive_status;
  }

  return (int)ndiHexToUnsignedLong(dp, 2);
}

/*---------------------------------------------------------------------*/
int ndiGetGXToolInfo(ndicapi *pol, int port)
{
  char *dp;

  if (port >= '1' && port <= '3') {
    dp = pol->gx_information[port - '1'];
  }
  else if (port >= 'A' && port <= 'I') {
    dp = pol->gx_passive_information[port - 'A'];
  }
  else {
    return 0;
  }

  return (int)ndiHexToUnsignedLong(dp, 2);
}

/*---------------------------------------------------------------------*/
int ndiGetGXMarkerInfo(ndicapi *pol, int port, int marker)
{
  char *dp;

  if (marker < 'A' || marker > 'T') {
    return 0;
  }

  if (port >= '1' && port <= '3') {
    dp = pol->gx_information[port - '1'];
  }
  else if (port >= 'A' && port <= 'I') {
    dp = pol->gx_passive_information[port - 'A'];
  }
  else {
    return 0;
  }
  dp += 11 - (marker - 'A');

  return (int)ndiHexToUnsignedLong(dp, 1);
}

/*---------------------------------------------------------------------*/
int ndiGetGXSingleStray(ndicapi *pol, int port, double coord[3])
{
  char *dp;
  
  if (port >= '1' && port <= '3') {
    dp = pol->gx_single_stray[port - '1'];
  }
  else {
    return NDI_DISABLED;
  }

  if (*dp == 'D' || *dp == '\0') {
    return NDI_DISABLED;
  }
  else if (*dp == 'M') {
    return NDI_MISSING;
  }

  coord[0] = ndiSignedToLong(&dp[0],  7)*0.01;
  coord[1] = ndiSignedToLong(&dp[7],  7)*0.01;
  coord[2] = ndiSignedToLong(&dp[14], 7)*0.01;

  return NDI_OKAY;
}

/*---------------------------------------------------------------------*/
unsigned long ndiGetGXFrame(ndicapi *pol, int port)
{
  char *dp;

  if (port >= '1' && port <= '3') {
    dp = pol->gx_frame[port - '1'];
  }
  else if (port >= 'A' && port <= 'I') {
    dp = pol->gx_passive_frame[port - 'A'];
  }
  else {
    return 0;
  }

  return (unsigned long)ndiHexToUnsignedLong(dp, 8);
}

/*---------------------------------------------------------------------*/
int ndiGetGXNumberOfPassiveStrays(ndicapi *pol)
{
  const char *dp;
  int n;

  dp = pol->gx_passive_stray;

  if (*dp == '\0') {
    return 0;
  }
  
  n = (int)ndiSignedToLong(dp, 3);
  if (n < 0) {
    return 0;
  }
  if (n > 20) {
    return 20;
  }

  return n; 
}

/*---------------------------------------------------------------------*/
int ndiGetGXPassiveStray(ndicapi *pol, int i, double coord[3])
{
  const char *dp;
  int n;

  dp = pol->gx_passive_stray;

  if (*dp == '\0') {
    return NDI_DISABLED;
  }

  n = (int)ndiSignedToLong(dp, 3);
  dp += 3;
  if (n < 0) {
    return NDI_MISSING;
  }
  if (n > 20) {
    n = 20;
  }

  if (i < 0 || i >= n) {
    return NDI_MISSING;
  }

  dp += 7*3*i;
  coord[0] = ndiSignedToLong(&dp[0],  7)*0.01;
  coord[1] = ndiSignedToLong(&dp[7],  7)*0.01;
  coord[2] = ndiSignedToLong(&dp[14], 7)*0.01;

  return NDI_OKAY;
}

/*---------------------------------------------------------------------*/
int ndiGetPSTATPortStatus(ndicapi *pol, int port)
{
  char *dp;

  if (port >= '1' && port <= '3') {
    dp = pol->pstat_basic[port - '1'];
  }
  else if (port >= 'A' && port <= 'I') {
    dp = pol->pstat_passive_basic[port - 'A'];
  }
  else {
    return 0;
  }

  /* the 'U' is for UNOCCUPIED */
  if (*dp == 'U' || *dp == '\0') {
    return 0;
  }

  /* skip to the last two characters */
  dp += 30;

  return (int)ndiHexToUnsignedLong(dp, 2);
}

/*---------------------------------------------------------------------*/
int ndiGetPSTATToolInfo(ndicapi *pol, int port, char information[30])
{
  char *dp;

  if (port >= '1' && port <= '3') {
    dp = pol->pstat_basic[port - '1'];
  }
  else if (port >= 'A' && port <= 'I') {
    dp = pol->pstat_passive_basic[port - 'A'];
  }
  else {
    return NDI_UNOCCUPIED;
  }

  /* the 'U' is for UNOCCUPIED */
  if (*dp == 'U' || *dp == '\0') {
    return NDI_UNOCCUPIED;
  }

  strncpy(information, dp, 30);

  return NDI_OKAY;
}

/*---------------------------------------------------------------------*/
unsigned long ndiGetPSTATCurrentTest(ndicapi *pol, int port)
{
  char *dp;

  if (port >= '1' && port <= '3') {
    dp = pol->pstat_testing[port - '1'];
  }
  else if (port >= 'A' && port <= 'I') {
    dp = pol->pstat_passive_testing[port - 'A'];
  }
  else {
    return 0;
  }

  if (*dp == '\0') {
    return 0;
  }

  return (unsigned long)ndiHexToUnsignedLong(dp, 8);
}

/*---------------------------------------------------------------------*/
int ndiGetPSTATPartNumber(ndicapi *pol, int port, char part[20])
{
  char *dp;

  if (port >= '1' && port <= '3') {
    dp = pol->pstat_part_number[port - '1'];
  }
  else if (port >= 'A' && port <= 'I') {
    dp = pol->pstat_passive_part_number[port - 'A'];
  }
  else {
    return NDI_UNOCCUPIED;
  }

  if (*dp == '\0') {
    return NDI_UNOCCUPIED;
  }

  strncpy(part, dp, 20);

  return NDI_OKAY;
}

/*---------------------------------------------------------------------*/
int ndiGetPSTATAccessories(ndicapi *pol, int port)
{
  char *dp;

  if (port >= '1' && port <= '3') {
    dp = pol->pstat_accessories[port - '1'];
  }
  else if (port >= 'A' && port <= 'I') {
    dp = pol->pstat_passive_accessories[port - 'A'];
  }
  else {
    return 0;
  }

  if (*dp == '\0') {
    return 0;
  }

  return (int)ndiHexToUnsignedLong(dp, 2);
}

/*---------------------------------------------------------------------*/
int ndiGetPSTATMarkerType(ndicapi *pol, int port)
{
  char *dp;

  if (port >= '1' && port <= '3') {
    dp = pol->pstat_marker_type[port - '1'];
  }
  else if (port >= 'A' && port <= 'I') {
    dp = pol->pstat_passive_marker_type[port - 'A'];
  }
  else {
    return 0;
  }

  if (*dp == '\0') {
    return 0;
  }

  return (int)ndiHexToUnsignedLong(dp, 2);
}

/*---------------------------------------------------------------------*/
int ndiGetSSTATControl(ndicapi *pol)
{
  char *dp;

  dp = pol->sstat_control;

  if (*dp == '\0') {
    return 0;
  }

  return (int)ndiHexToUnsignedLong(dp, 2);
}

/*---------------------------------------------------------------------*/
int ndiGetSSTATSensors(ndicapi *pol)
{
  char *dp;

  dp = pol->sstat_sensor;

  if (*dp == '\0') {
    return 0;
  }

  return (int)ndiHexToUnsignedLong(dp, 2);
}

/*---------------------------------------------------------------------*/
int ndiGetSSTATTIU(ndicapi *pol)
{
  char *dp;

  dp = pol->sstat_tiu;

  if (*dp == '\0') {
    return 0;
  }

  return (int)ndiHexToUnsignedLong(dp, 2);
}

/*---------------------------------------------------------------------*/
int ndiGetIRCHKDetected(ndicapi *pol)
{
  if (pol->irchk_detected == '1') {
    return 1;
  }
  return 0;
}

/*---------------------------------------------------------------------*/
int ndiGetIRCHKNumberOfSources(ndicapi *pol, int side)
{
  const char *dp;
  int n, m;

  dp = pol->irchk_sources;

  if (*dp == '\0') {
    return 0;
  }

  n = (int)ndiSignedToLong(dp, 3);
  if (n < 0 || n > 20) {
    return 0;
  }
  m = (int)ndiSignedToLong(dp + 3 + 2*3*n, 3);
  if (m < 0 || m > 20) {
    return 0;
  }

  if (side == NDI_LEFT) {
    return n;
  }
  else if (side == NDI_RIGHT) {
    return m;
  }

  return 0;
}
 
/*---------------------------------------------------------------------*/
int ndiGetIRCHKSourceXY(ndicapi *pol, int side, int i, double xy[2])
{
  const char *dp;
  int n, m;

  dp = pol->irchk_sources;

  if (dp == NULL || *dp == '\0') {
    return NDI_MISSING;
  }

  n = (int)ndiSignedToLong(dp, 3);
  if (n < 0 || n > 20) {
    return NDI_MISSING;
  }
  m = (int)ndiSignedToLong(dp + 3 + 2*3*n, 3);
  if (m < 0 || m > 20) {
    return NDI_MISSING;
  }

  if (side == NDI_LEFT) {
    if (i < 0 || i >= n) {
      return NDI_MISSING;
    }
    dp += 3 + 2*3*i;
  }
  else if (side == NDI_RIGHT) {
    if (i < 0 || i >= m) {
      return NDI_MISSING;
    }
    dp += 3 + 2*3*n + 3 + 2*3*i;
  }
  else if (side == NDI_RIGHT) {
    return NDI_MISSING;
  }

  xy[0] = ndiSignedToLong(&dp[0], 3)*0.01;
  xy[1] = ndiSignedToLong(&dp[3], 3)*0.01;

  return NDI_OKAY;
}

/*---------------------------------------------------------------------
  Copy all the PHINF reply information into the ndicapi structure, according
  to the PHINF reply mode that was requested.

  This function is called every time a PHINF command is sent to the
  Measurement System.

  This information can be later extracted through one of the ndiGetPHINFxx()
  functions.
*/
static void ndi_PHINF_helper(ndicapi *pol, const char *cp, const char *crp)
{
  unsigned long mode = 0x0001; /* the default reply mode */
  char *dp;
  int j;
  int unoccupied = NDI_OKAY;

  /* if the PHINF command had a reply mode, read it */
  if ((cp[5] == ':' && cp[10] != '\r') || (cp[5] == ' ' && cp[6] != '\r')) { 
    mode = ndiHexToUnsignedLong(&cp[8], 4);
  }

  /* check for unoccupied */
  if (*crp == 'U') {
    unoccupied = NDI_UNOCCUPIED;
  }
  pol->phinf_unoccupied = unoccupied;

  /* fprintf(stderr, "mode = %04lx\n", mode); */

  if (mode & NDI_BASIC) {
    dp = pol->phinf_basic;
    if (!unoccupied) {
      for (j = 0; j < 33 && *crp >= ' '; j++) {
        /* fprintf(stderr,"%c",*crp); */
        *dp++ = *crp++;
      }
      /* fprintf(stderr,"\n"); */
    }
    else {  /* default "00000000            0000000000000" */
      for (j = 0; j < 8; j++) {
        *dp++ = '0';
      }
      for (j = 0; j < 12; j++) {
        *dp++ = ' ';
      }
      for (j = 0; j < 13; j++) {
        *dp++ = '0';
      }
    }
  }

  if (mode & NDI_TESTING) {
    dp = pol->phinf_testing;
    if (!unoccupied) {
      for (j = 0; j < 8 && *crp >= ' '; j++) {
        *dp++ = *crp++;
      }
    }
    else { /* default "00000000" */
      for (j = 0; j < 8; j++) {
        *dp++ = '0';
      }
    }
  }

  if (mode & NDI_PART_NUMBER) {
    dp = pol->phinf_part_number;
    if (!unoccupied) {
      for (j = 0; j < 20 && *crp >= ' '; j++) {
        *dp++ = *crp++;
      }
    }
    else { /* default "                    " */
      for (j = 0; j < 20; j++) {
        *dp++ = ' ';
      }
    }
  }

  if (mode & NDI_ACCESSORIES) {
    dp = pol->phinf_accessories;
    if (!unoccupied) {
      for (j = 0; j < 2 && *crp >= ' '; j++) {
        *dp++ = *crp++;
      }
    }
    else { /* default "00" */
      for (j = 0; j < 2; j++) {
        *dp++ = '0';
      }
    }
  }

  if (mode & NDI_MARKER_TYPE) {
    dp = pol->phinf_marker_type;
    if (!unoccupied) {
      for (j = 0; j < 2 && *crp >= ' '; j++) {
        *dp++ = *crp++;
      }
    }
    else { /* default "00" */
      for (j = 0; j < 2; j++) {
        *dp++ = '0';
      }
    }
  }

  if (mode & NDI_PORT_LOCATION) {
    dp = pol->phinf_port_location;
    if (!unoccupied) {
      for (j = 0; j < 14 && *crp >= ' '; j++) {
        *dp++ = *crp++;
      }
    }
    else { /* default "00000000000000" */
      for (j = 0; j < 14; j++) {
        *dp++ = '0';
      }
    }
  }

  if (mode & NDI_GPIO_STATUS) {
    dp = pol->phinf_gpio_status;
    if (!unoccupied) {
      for (j = 0; j < 2 && *crp >= ' '; j++) {
        *dp++ = *crp++;
      }
    }
    else { /* default "00" */
      for (j = 0; j < 2; j++) {
        *dp++ = '0';
      }
    }
  }
}

/*---------------------------------------------------------------------
  Copy all the PHRQ reply information into the ndicapi structure.

  This function is called every time a PHRQ command is sent to the
  Measurement System.

  This information can be later extracted through one of the ndiGetPHRQHandle()
  functions.
*/
static void ndi_PHRQ_helper(ndicapi *pol, const char *cp, const char *crp)
{
  char *dp;
  int j;

  dp = pol->phrq_reply;
  for (j = 0; j < 2; j++) {
    *dp++ = *crp++;
  }
}

/*---------------------------------------------------------------------
  Copy all the PHSR reply information into the ndicapi structure.

  This function is called every time a PHSR command is sent to the
  Measurement System.

  This information can be later extracted through one of the ndiGetPHSRxx()
  functions.
*/
static void ndi_PHSR_helper(ndicapi *pol, const char *cp, const char *crp)
{
  char *dp;
  int j;

  dp = pol->phsr_reply;
  for (j = 0; j < 1282 && *crp >= ' '; j++) {
    *dp++ = *crp++;
  }
  *dp++ = '\0';
}

/*---------------------------------------------------------------------
  Copy all the TX reply information into the ndicapi structure, according
  to the TX reply mode that was requested.

  This function is called every time a TX command is sent to the
  Measurement System.

  This information can be later extracted through one of the ndiGetTXxx()
  functions.
*/
static void ndi_TX_helper(ndicapi *pol, const char *cp, const char *crp)
{
  unsigned long mode = 0x0001; /* the default reply mode */
  char *dp;
  int i, j, n;
  int ph, nhandles, nstray;

  /* if the TX command had a reply mode, read it */
  if ((cp[2] == ':' && cp[7] != '\r') || (cp[2] == ' ' && cp[3] != '\r')) { 
    mode = ndiHexToUnsignedLong(&cp[3], 4);
  }

  /* get the number of handles */
  nhandles = (int)ndiHexToUnsignedLong(crp, 2);
  for (j = 0; j < 2 && *crp >= ' '; j++) {
    crp++;
  }

  /* go through the information for each handle */
  for (i = 0; i < nhandles; i++) {
    /* get the handle itself (two chars) */
    ph = (int)ndiHexToUnsignedLong(crp,2);
    for (j = 0; j < 2 && *crp >= ' '; j++) {
      crp++;
    }

    /* check for "UNOCCUPIED" */
    if (*crp == 'U') {
      for (j = 0; j < 10 && *crp >= ' '; j++) {
        crp++;
      }
      /* back up and continue (don't store information for unoccupied ports) */
      i--;
      nhandles--;
      continue;
    }

    /* save the port handle in the list */
    pol->tx_handles[i] = ph;

    if (mode & NDI_XFORMS_AND_STATUS) {
      /* get the transform, MISSING, or DISABLED */
      dp = pol->tx_transforms[i];

      if (*crp == 'M') {
        /* check for "MISSING" */
        for (j = 0; j < 7 && *crp >= ' '; j++) {
          *dp++ = *crp++;
        }
      }
      else if (*crp == 'D') {
        /* check for "DISABLED" */
        for (j = 0; j < 8 && *crp >= ' '; j++) {
          *dp++ = *crp++;
        }
      }
      else {
        /* read the transform */
        for (j = 0; j < 51 && *crp >= ' '; j++) {
          *dp++ = *crp++;
        }
      }
      *dp = '\0';

      /* get the status */
      dp = pol->tx_status[i];
      for (j = 0; j < 8 && *crp >= ' '; j++) {
        *dp++ = *crp++;
      }

      /* get the frame number */
      dp = pol->tx_frame[i];
      for (j = 0; j < 8 && *crp >= ' '; j++) {
        *dp++ = *crp++;
      }
    }

    /* grab additonal information */
    if (mode & NDI_ADDITIONAL_INFO) {
      dp = pol->tx_information[i];
      for (j = 0; j < 20 && *crp >= ' '; j++) {
        *dp++ = *crp++;
      }
    }

    /* grab the single marker info */ 
    if (mode & NDI_SINGLE_STRAY) {
      dp = pol->tx_single_stray[i];
      if (*crp == 'M') {
        /* check for "MISSING" */
        for (j = 0; j < 7 && *crp >= ' '; j++) {
          *dp++ = *crp++;
        }
      }
      else if (*crp == 'D') {
        /* check for "DISABLED" */
        for (j = 0; j < 8 && *crp >= ' '; j++) {
          *dp++ = *crp++;
        }
      }
      else {
        /* read the single stray position */
        for (j = 0; j < 21 && *crp >= ' '; j++) {
          *dp++ = *crp++;
        }
      }
      *dp = '\0';
    }
      
    /* skip over any unsupported information */
    while (*crp >= ' ') {
      crp++;
    }

    /* eat the trailing newline */
    if (*crp == '\n') {
      crp++;
    }
  }

  /* save the number of handles (minus the unoccupied handles) */
  pol->tx_nhandles = nhandles;

  /* get all the passive stray information */
  /* this will be a maximum of 3 + 13 + 50*3*7 = 1066 bytes */
  if (mode & NDI_PASSIVE_STRAY) {
    /* get the number of strays */
    nstray = (int)ndiSignedToLong(crp, 3);
    for (j = 0; j < 2 && *crp >= ' '; j++) {
      crp++;
    }
    if (nstray > 50) {
      nstray = 50;
    }
    pol->tx_npassive_stray = nstray;
    /* get the out-of-volume bits */
    dp = pol->tx_passive_stray_oov;
    n = (nstray + 3)/4;
    for (j = 0; j < n && *crp >= ' '; j++) {
      *dp++ = *crp++;
    }
    /* get the coordinates */
    dp = pol->tx_passive_stray;
    n = nstray*21;
    for (j = 0; j < n && *crp >= ' '; j++) {
      *dp++ = *crp++;
    }
    *dp = '\0';
  }

  /* get the system status */
  dp = pol->tx_system_status;
  for (j = 0; j < 4 && *crp >= ' '; j++) {
    *dp++ = *crp++;
  }
}

/*---------------------------------------------------------------------
  Copy all the GX reply information into the ndicapi structure, according
  to the GX reply mode that was requested.

  This function is called every time a GX command is sent to the
  Measurement System.

  This information can be later extracted through one of the ndiGetGXxx()
  functions.
*/
static void ndi_GX_helper(ndicapi *pol, const char *cp, const char *crp)
{
  unsigned long mode = 0x0001; /* the default reply mode */
  char *dp;
  int i, j, k;
  int npassive, nactive;

  /* if the GX command had a reply mode, read it */
  if ((cp[2] == ':' && cp[7] != '\r') || (cp[2] == ' ' && cp[3] != '\r')) { 
    mode = ndiHexToUnsignedLong(&cp[3], 4);
  }

  /* always three active ports */
  nactive = 3;

  if (mode & NDI_XFORMS_AND_STATUS) {
    for (k = 0; k < nactive; k += 3) {
      /* grab the three transforms */
      for (i = 0; i < 3; i++) {
        dp = pol->gx_transforms[i];
        for (j = 0; j < 51 && *crp >= ' '; j++) {
          *dp++ = *crp++;
        }
        *dp = '\0';
        /* fprintf(stderr, "xf %.51s\n", pol->gx_transforms[i]); */
        /* eat the trailing newline */
        if (*crp == '\n') {
          crp++;
        }
      }
      /* grab the status flags */
      dp = pol->gx_status + k/3*8;
      for (j = 0; j < 8 && *crp >= ' '; j++) {
        *dp++ = *crp++;
      }
      /* fprintf(stderr, "st %.8s\n", pol->gx_status); */
    }
    /* eat the trailing newline */
    if (*crp == '\n') {
      crp++;
    }
  }

  if (mode & NDI_ADDITIONAL_INFO) {
    /* grab information for each port */
    for (i = 0; i < nactive; i++) {
      dp = pol->gx_information[i];
      for (j = 0; j < 12 && *crp >= ' '; j++) {
        *dp++ = *crp++;
      }
      /* fprintf(stderr, "ai %.12s\n", pol->gx_information[i]); */
    }
    /* eat the trailing newline */
    if (*crp == '\n') {
      crp++;
    }
  }

  if (mode & NDI_SINGLE_STRAY) {
    /* grab stray marker for each port */
    for (i = 0; i < nactive; i++) {
      dp = pol->gx_single_stray[i];
      for (j = 0; j < 21 && *crp >= ' '; j++) {
        *dp++ = *crp++;
      }
      *dp = '\0';
      /* fprintf(stderr, "ss %.21s\n", pol->gx_single_stray[i]); */      
      /* eat the trailing newline */
      if (*crp == '\n') {
        crp++;
      }
    }
  }

  if (mode & NDI_FRAME_NUMBER) {
    /* get frame number for each port */
    for (i = 0; i < nactive; i++) {
      dp = pol->gx_frame[i];
      for (j = 0; j < 8 && *crp >= ' '; j++) {
        *dp++ = *crp++;
      }
      /* fprintf(stderr, "fn %.8s\n", pol->gx_frame[i]); */
    }
    /* eat the trailing newline */
    if (*crp == '\n') {
      crp++;
    }
  }

  /* if there is no passive information, stop here */
  if (!(mode & NDI_PASSIVE)) {
    return;
  }

  /* in case there are 9 passive tools instead of just 3 */
  npassive = 3;
  if (mode & NDI_PASSIVE_EXTRA) {
    npassive = 9;
  }

  if ((mode & NDI_XFORMS_AND_STATUS) || (mode == NDI_PASSIVE)) {
    /* the information is grouped in threes */
    for (k = 0; k < npassive; k += 3) {
      /* grab the three transforms */
      for (i = 0; i < 3; i++) {
        dp = pol->gx_passive_transforms[k+i];
        for (j = 0; j < 51 && *crp >= ' '; j++) {
          *dp++ = *crp++;
        }
        *dp = '\0';
        /* fprintf(stderr, "pxf %.31s\n", pol->gx_passive_transforms[k+i]); */
        /* eat the trailing newline */
        if (*crp == '\n') {
          crp++;
        }
      }
      /* grab the status flags */
      dp = pol->gx_passive_status + k/3*8;
      for (j = 0; j < 8 && *crp >= ' '; j++) {
        *dp++ = *crp++;
      }
      /* fprintf(stderr, "pst %.8s\n", pol->gx_passive_status + k/3*8); */
      /* skip the newline */
      if (*crp == '\n') {
        crp++;
      }
      else { /* no newline: no more passive transforms */
        npassive = k + 3;
      }
    }
    /* eat the trailing newline */
    if (*crp == '\n') {
      crp++;
    }
  }

  if (mode & NDI_ADDITIONAL_INFO) {
    /* grab information for each port */
    for (i = 0; i < npassive; i++) {
      dp = pol->gx_passive_information[i];
      for (j = 0; j < 12 && *crp >= ' '; j++) {
        *dp++ = *crp++;
      }
      /* fprintf(stderr, "pai %.12s\n", pol->gx_passive_information[i]); */
    }
    /* eat the trailing newline */
    if (*crp == '\n') {
      crp++;
    }
  }

  if (mode & NDI_FRAME_NUMBER) {
    /* get frame number for each port */
    for (i = 0; i < npassive; i++) {
      dp = pol->gx_passive_frame[i];
      for (j = 0; j < 8 && *crp >= ' '; j++) {
        *dp++ = *crp++;
      }
      /* fprintf(stderr, "pfn %.8s\n", pol->gx_passive_frame[i]); */      
    }
    /* eat the trailing newline */
    if (*crp == '\n') {
      crp++;
    }
  }

  if (mode & NDI_PASSIVE_STRAY) {
    /* get all the passive stray information */
    /* this will be a maximum of 3 + 20*3*7 = 423 bytes */
    dp = pol->gx_passive_stray;
    for (j = 0; j < 423 && *crp >= ' '; j++) {
      *dp++ = *crp++;
    }
    /* fprintf(stderr, "psm %s\n", pol->gx_passive_stray); */
  }
}

/*---------------------------------------------------------------------
  Copy all the PSTAT reply information into the ndicapi structure.
*/
static void ndi_PSTAT_helper(ndicapi *pol, const char *cp, const char *crp)
{
  unsigned long mode = 0x0001; /* the default reply mode */
  char *dp;
  int i, j;
  int npassive, nactive;

  /* if the PSTAT command had a reply mode, read it */
  if ((cp[5] == ':' && cp[10] != '\r') || (cp[5] == ' ' && cp[6] != '\r')) { 
    mode = ndiHexToUnsignedLong(&cp[6], 4);
  }

  /* always three active ports */
  nactive = 3;

  /* information for each port is separated by a newline */
  for (i = 0; i < nactive; i++) {

    /* basic tool information and port status */
    if (mode & NDI_BASIC) {
      dp = pol->pstat_basic[i];
      for (j = 0; j < 32 && *crp >= ' '; j++) {
        *dp++ = *crp++;
      }
      /* terminate if UNOCCUPIED */
      if (j < 32) {
        *dp = '\0';
      }
      /* fprintf(stderr, "ba %.32s\n", pol->pstat_basic[i]); */
    }

    /* current testing */
    if (mode & NDI_TESTING) {
      dp = pol->pstat_testing[i];
      *dp = '\0';
      for (j = 0; j < 8 && *crp >= ' '; j++) {
        *dp++ = *crp++;
      }
      /* fprintf(stderr, "ai %.8s\n", pol->pstat_testing[i]); */
    }

    /* part number */
    if (mode & NDI_PART_NUMBER) {
      dp = pol->pstat_part_number[i];
      *dp = '\0';
      for (j = 0; j < 20 && *crp >= ' '; j++) {
        *dp++ = *crp++;
      }
      /* fprintf(stderr, "pn %.20s\n", pol->pstat_part_number[i]); */
    }
    
    /* accessories */
    if (mode & NDI_ACCESSORIES) {
      dp = pol->pstat_accessories[i];
      *dp = '\0';
      for (j = 0; j < 2 && *crp >= ' '; j++) {
        *dp++ = *crp++;
      }
      /* fprintf(stderr, "ac %.2s\n", pol->pstat_accessories[i]); */
    }

    /* marker type */
    if (mode & NDI_MARKER_TYPE) {
      dp = pol->pstat_marker_type[i];
      *dp = '\0';
      for (j = 0; j < 2 && *crp >= ' '; j++) {
        *dp++ = *crp++;
      }
      /* fprintf(stderr, "mt %.2s\n", pol->pstat_marker_type[i]); */
    }

    /* skip any other information that might be present */
    while (*crp >= ' ') {
      crp++;
    }

    /* eat the trailing newline */
    if (*crp == '\n') {
      crp++;
    }
  }

  /* if there is no passive information, stop here */
  if (!(mode & NDI_PASSIVE)) {
    return;
  }

  /* in case there are 9 passive tools instead of just 3 */
  npassive = 3;
  if (mode & NDI_PASSIVE_EXTRA) {
    npassive = 9;
  }

  /* information for each port is separated by a newline */
  for (i = 0; i < npassive; i++) {

    /* basic tool information and port status */
    if (mode & NDI_BASIC) {
      dp = pol->pstat_passive_basic[i];
      *dp = '\0';
      for (j = 0; j < 32 && *crp >= ' '; j++) {
        *dp++ = *crp++;
      }
      /* terminate if UNOCCUPIED */
      if (j < 32) {
        *dp = '\0';
      }
      /* fprintf(stderr, "pba %.32s\n", pol->pstat_passive_basic[i]); */
    }

    /* current testing */
    if (mode & NDI_TESTING) {
      dp = pol->pstat_passive_testing[i];
      *dp = '\0';
      for (j = 0; j < 8 && *crp >= ' '; j++) {
        *dp++ = *crp++;
      }
      /* fprintf(stderr, "pai %.8s\n", pol->pstat_passive_testing[i]); */
    }

    /* part number */
    if (mode & NDI_PART_NUMBER) {
      dp = pol->pstat_passive_part_number[i];
      *dp = '\0';
      for (j = 0; j < 20 && *crp >= ' '; j++) {
        *dp++ = *crp++;
      }
      /* fprintf(stderr, "ppn %.20s\n", pol->pstat_passive_part_number[i]); */
    }
    
    /* accessories */
    if (mode & NDI_ACCESSORIES) {
      dp = pol->pstat_passive_accessories[i];
      *dp = '\0';
      for (j = 0; j < 2 && *crp >= ' '; j++) {
        *dp++ = *crp++;
      }
      /* fprintf(stderr, "pac %.2s\n", pol->pstat_passive_accessories[i]); */
    }

    /* marker type */
    if (mode & NDI_MARKER_TYPE) {
      dp = pol->pstat_passive_marker_type[i];
      *dp = '\0';
      for (j = 0; j < 2 && *crp >= ' '; j++) {
        *dp++ = *crp++;
      }
      /* fprintf(stderr, "pmt %.2s\n", pol->pstat_passive_marker_type[i]); */
    }

    /* skip any other information that might be present */
    while (*crp >= ' ') {
      crp++;
    }

    /* eat the trailing newline */
    if (*crp == '\n') {
      crp++;
    }
  }
}

/*---------------------------------------------------------------------
  Copy all the SSTAT reply information into the ndicapi structure.
*/
static void ndi_SSTAT_helper(ndicapi *pol, const char *cp, const char *crp)
{
  unsigned long mode;
  char *dp;

  /* read the SSTAT command reply mode */
  mode = ndiHexToUnsignedLong(&cp[6], 4);

  if (mode & NDI_CONTROL) {
    dp = pol->sstat_control;
    *dp++ = *crp++; 
    *dp++ = *crp++; 
  }

  if (mode & NDI_SENSORS) {
    dp = pol->sstat_sensor;
    *dp++ = *crp++; 
    *dp++ = *crp++; 
  }

  if (mode & NDI_TIU) {
    dp = pol->sstat_tiu;
    *dp++ = *crp++; 
    *dp++ = *crp++; 
  }

}

/*---------------------------------------------------------------------
  Copy all the IRCHK reply information into the ndicapi structure.
*/
static void ndi_IRCHK_helper(ndicapi *pol, const char *cp, const char *crp)
{
  unsigned long mode = 0x0001; /* the default reply mode */
  int j;

  /* if the IRCHK command had a reply mode, read it */
  if ((cp[5] == ':' && cp[10] != '\r') || (cp[5] == ' ' && cp[6] != '\r')) { 
    mode = ndiHexToUnsignedLong(&cp[6], 4);
  }

  /* a single character, '0' or '1' */
  if (mode & NDI_DETECTED) {
    pol->irchk_detected = *crp++;
  }

  /* maximum string length for 20 sources is 2*(3 + 20*3) = 126 */
  /* copy until a control char (less than 0x20) is found */
  if (mode & NDI_SOURCES) {
    for (j = 0; j < 126 && *crp >= ' '; j++) {
      pol->irchk_sources[j] = *crp++;
    }
  }
}
 
/*---------------------------------------------------------------------
  Adjust the host to match a COMM command.
*/
static void ndi_COMM_helper(ndicapi *pol, const char *cp, const char *crp)
{
  static int convert_baud[6] = { 9600, 14400, 19200, 38400, 57600, 115200 };
  char newdps[4] = "8N1";
  int newspeed = 9600;
  int newhand = 0;

  if (cp[5] >= '0' && cp[5] <= '5') {
    newspeed = convert_baud[cp[5]-'0'];
  }
  if (cp[6] == '1') {
    newdps[0] = '7';
  }
  if (cp[7] == '1') {
    newdps[1] = 'O';
  }
  else if (cp[7] == '2') {
    newdps[1] = 'E';
  }
  if (cp[8] == '1') {
    newdps[2] = '2';
  }
  if (cp[9] == '1') {
    newhand = 1;
  } 

  ndiSerialSleep(pol->serial_device, 100);  /* let the device adjust itself */
  if (ndiSerialComm(pol->serial_device, newspeed, newdps, newhand) != 0) {
    ndi_set_error(pol, NDI_BAD_COMM);
  }
}

/*---------------------------------------------------------------------
  Sleep for 100 milliseconds after an INIT command.
*/
static void ndi_INIT_helper(ndicapi *pol, const char *cp, const char *crp)
{
  ndiSerialSleep(pol->serial_device, 100);
}

/*---------------------------------------------------------------------
  This function is called any time the error marker is set.
  It sets the 'error_code' member of the ndicapi structure,
  and calls the error_callback if there is one.
*/
static int ndi_set_error(ndicapi *pol, int errnum)
{
  pol->error_code = errnum;
  
  /* call the user-supplied callback function */
  if (pol->error_callback) {
    pol->error_callback(errnum, ndiErrorString(errnum),
                        pol->error_callback_data);
  }

  return errnum;
}

/*---------------------------------------------------------------------
  The tracking thread.

  This thread continually sends the most recent GX command to the
  NDICAPI until it is told to quit or until an error occurs.

  The thread is blocked unless the Measurement System is in tracking mode.
*/
static void *ndi_thread(void *userdata)
{
  int i, m;
  int errcode = 0;
  char *cp, *rp;
  ndicapi *pol;

  pol = (ndicapi *)userdata;
  cp = pol->thread_command;
  rp = pol->thread_reply;

  while (errcode == 0) {

    /* if the application is blocking us, we sit here and wait */
    ndiMutexLock(pol->thread_mutex);

    /* quit if threading has been turned off */
    if (pol->thread_mode == 0) {
      ndiMutexUnlock(pol->thread_mutex);
      return NULL;
    }

    /* check whether we have a GX command ready to send */
    if (cp[0] == '\0') {
      ndiSerialSleep(pol->serial_device, 20);
      ndiMutexUnlock(pol->thread_mutex);
      continue;
    }

    /* flush the input buffer, because anything that we haven't read
       yet is garbage left over by a previously failed command */
    ndiSerialFlush(pol->serial_device, NDI_IFLUSH);

    /* send the command to the Measurement System */
    i = strlen(cp);
    if (errcode == 0) {
      m = ndiSerialWrite(pol->serial_device, cp, i);
      if (m < 0) {
        errcode = NDI_WRITE_ERROR;
      }
      else if (m < i) {
        errcode = NDI_TIMEOUT;
      }
    }
    
    /* read the reply from the Measurement System */
    if (errcode == 0) {
      m = ndiSerialRead(pol->serial_device, rp, 2047);
      if (m < 0) {
        errcode = NDI_READ_ERROR;
        m = 0;
      }
      else if (m == 0) {
        errcode = NDI_TIMEOUT;
      }
      /* terminate the string */
      rp[m] = '\0';
    }
    
    /* lock the buffer */
    ndiMutexLock(pol->thread_buffer_mutex);
    /* copy the reply into the buffer, also copy the error code */
    strcpy(pol->thread_buffer, rp);
    pol->thread_error = errcode;
    /* signal the main thread that a new data record is ready */
    ndiEventSignal(pol->thread_buffer_event);
    /* unlock the buffer */
    ndiMutexUnlock(pol->thread_buffer_mutex);

    /* release the lock to give the application a chance to block us */
    ndiMutexUnlock(pol->thread_mutex);
  }

  return NULL;
}

/*---------------------------------------------------------------------
  Allocate all the objects needed for threading and then start the thread. 
*/
static void ndi_spawn_thread(ndicapi *pol)
{
  pol->thread_command = (char *)malloc(2048);
  pol->thread_command[0] = '\0';
  pol->thread_reply = (char *)malloc(2048);
  pol->thread_reply[0] = '\0';
  pol->thread_buffer = (char *)malloc(2048);
  pol->thread_buffer[0] = '\0';
  pol->thread_error = 0;

  pol->thread_buffer_mutex = ndiMutexCreate();
  pol->thread_buffer_event = ndiEventCreate();
  pol->thread_mutex = ndiMutexCreate();
  if (!pol->tracking) {
    /* if not tracking, then block the thread */
    ndiMutexLock(pol->thread_mutex);
  }
  pol->thread = ndiThreadSplit(&ndi_thread, pol);
}

/*---------------------------------------------------------------------
  Wait for the tracking thread to end, and then do the clean-up.
*/
static void ndi_join_thread(ndicapi *pol)
{
  if (!pol->tracking) {
    /* if not tracking, unblock the thread or it can't stop */
    ndiMutexUnlock(pol->thread_mutex);
  }
  ndiThreadJoin(pol->thread);
  ndiEventDestroy(pol->thread_buffer_event);
  ndiMutexDestroy(pol->thread_buffer_mutex);
  ndiMutexDestroy(pol->thread_mutex);
  
  free(pol->thread_buffer);
  pol->thread_buffer = 0;
  free(pol->thread_reply);
  pol->thread_reply = 0;
  free(pol->thread_command);
  pol->thread_command = 0;
}

/*---------------------------------------------------------------------
  For starting and stopping the tracking thread.

  The mode is either 0 (no thread) or 1 (thread).  Other modes
  might be added in the future.
*/
void ndiSetThreadMode(ndicapi *pol, int mode)
{
  if ((pol->thread_mode && mode) || (!pol->thread_mode && !mode)) {
    return;
  }

  pol->thread_mode = mode;
  
  if (mode) {
    ndi_spawn_thread(pol);
  }
  else {
    ndi_join_thread(pol);
  } 
}

/*---------------------------------------------------------------------*/
int ndiGetThreadMode(ndicapi *pol)
{
  return pol->thread_mode;
}




