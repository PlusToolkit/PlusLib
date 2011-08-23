/*=======================================================================

  Program:   Flock of Birds C Interface Library
  Module:    $RCSfile: flock.c,v $
  Creator:   David Gobbi <dgobbi@atamai.com>
  Language:  C
  Author:    $Author: dgobbi $
  Date:      $Date: 2005/10/31 02:07:16 $
  Version:   $Revision: 1.5 $

==========================================================================
Copyright 2000 Atamai, Inc.

Redistribution of this source code and/or any binary applications created
using this source code is prohibited without the expressed, written
permission of the copyright holders.  

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
==========================================================================*/

#include <time.h>
#include <ctype.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#if defined(_WIN32) || defined(WIN32)
#include <windows.h>
#include <winbase.h>
#include <sys/timeb.h>
#ifdef _MT
#include <process.h>
#define FLOCK_USE_THREADS 1
#endif /* _MT */

#elif defined(__unix__) || defined(unix) || defined(__APPLE__)
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <termios.h>
#if defined(__APPLE__)
#include <dirent.h>
#endif
#include <errno.h>
#ifdef _POSIX_THREADS
#include <pthread.h>
#define FLOCK_USE_THREADS 1
#endif /* _POSIX_THREADS */
#endif /* unix */

#include "flock.h"

/* A short note on multithreading:

   Multithreading is enabled when FLOCK_USE_THREADS is defined, which
   happens when
   1) the _MT flag is set under Windows (i.e. when the /MT flag is given
      to CL.EXE or when multithreaded compilation is selected in
      Visual Studio)
   2) the _POSIX_THREADS flag is set under UNIX, i.e. if the operating
      system supports POSIX threads

   If threading is enabled and fbOpen() is called with FB_THREAD or
   FB_NONBLOCK, then a thread will be spawned that listens for data
   records sent from the Flock.

   This thread continually checks for new data records from the flock,
   which come at a rate of 100Hz in the Flock's "Stream" mode.  Without
   the use of this thread to catch the records, the serial port buffer
   might overflow during Stream mode.
   
   If multithreading is not available on a particular operating system,
   then "Point" mode should be used instead of "Stream" mode.
*/


/* the flock structure: never meant to be used anywhere but in flock.c */

struct fbird {
  /* stuff that needs to be set before the flock is opened */
  char device_name[256]; /* serial port device name */
  int baud_rate;         /* baud rate */
  int asynchronous;      /* asynchronous mode */

  /* architecture dependent stuff */
#if defined(_WIN32) || defined(WIN32)
  HANDLE file;           /* windows file handle */
#elif defined(__unix__) || defined(unix) || defined(__APPLE__)
  int file;              /* unix file handle */
#endif

  /* multithreading stuff */
#ifdef FLOCK_USE_THREADS
#if defined(_WIN32) || defined(WIN32)
  HANDLE file_mutex;        /* mutex lock on file handle */
  HANDLE data_event;        /* event is triggered when new data arrives */
  HANDLE data_mutex;        /* lock on data_buffer */
  HANDLE stream_thread;  /* tracking thread for asynchronous mode */
  HANDLE stream_mutex;   /* use to pause tracking thread */
#elif defined(__unix__) || defined(unix) || defined(__APPLE__)
#ifdef _POSIX_THREADS
  pthread_mutex_t file_mutex;      /* mutex lock on file handle */
  pthread_cond_t  fresh_data_cond; /* condition to wait on for new data */
  pthread_mutex_t fresh_data_mutex;/* mutex lock on condition           */
  pthread_mutex_t data_mutex;      /* lock on data_buffer */
  pthread_t stream_thread;      /* tracking thread for asynchronous mode */
  pthread_mutex_t stream_mutex; /* use to pause tracking thread */
#endif
#endif /* unix */
  int async_bird;           /* bird counter for thread */
  int async_error;          /* error in asyncronous thread */
  char async_buffer[128][256]; /* buffer for use by the extra thread  */
  long async_timestamp_secs;   /* timestamp -- secs since 1970 */
  long async_timestamp_msecs;  /* timestamp -- millisecs */  
  int async_data_rate;            /* data rate (Hz) in asynchronous mode */
#endif /* FLOCK_USE_THREADS */

  /* error reporting */
  int error;             /* stores last error */
  char error_text[256];  /* stores text for last error */
  void (*error_handler)(void *); /* error callback function */
  void *error_handler_data;   /* error callback data */

  /* miscellaneous state information */
  int revision;          /* revision number (quite important) */
  int max_parameter;     /* max examine/change parameter number for this rev */
  int address_mode;      /* FBB addressing mode */
  int group_mode;        /* birds are in group mode */
  int num_birds;         /* number of running birds */
  int stream;            /* true if currently stream */
  int point;             /* true if currently point */
  int RS232toFBB;        /* stores bird address after RS232_TO_FBB */
  int bird;              /* the bird which this data set is for */
  char position_scale[128]; /* position scale mode for each bird */
  char button_mode[128];    /* button mode for each bird */
  char format[128];         /* FB_POSITION, FB_ANGLES, etc for each bird */ 

  /* storage of data etc. */
  char data_buffer[256]; /* place where data is stored after being read */
  int phase_leftovers;   /* leftover chars after a phase error */
  int fresh_data;        /* is the data still fresh? */
  long timestamp_secs;   /* timestamp -- secs since 1970 */
  long timestamp_msecs;  /* timestamp -- millisecs */  
};

/* number of words in point/angle/etc data */
static int format_len_table[8] = { 3, 3, 9, 6, 12, 0, 4, 7 };
#define data_len(format) (format_len_table[(format) - FB_POSITION])

/* prototypes for static internal-use-only functions */

static int set_error(fbird *fb, int error_code, const char *text);
static int convert_baud_rate(int baud);
static int max_parameter(int revision);
static void set_timestamp(long *sec, long *msec);

#ifdef FLOCK_USE_THREADS
#if defined(_WIN32) || defined(WIN32)
static void stream_thread(void *user_data);
#elif defined(__unix__) || defined(unix) || defined(__APPLE__)
static void *stream_thread(void *user_data);
#endif
static int start_stream_thread(fbird *fb);
static void end_stream_thread(fbird *fb);
#endif 

/*---------------------------------------------------------------------*/
/** \defgroup SetupMethods Connecting to the Flock

The flock of birds must be connected to one of the serial ports on the
computer.  The device name for the serial ports are different for
different operating systems, the fbDeviceName() function can be used
to generate an appropriate device name for the host OS.

The \p fbird data structure stores all of the information that is needed
by the host computer in order to maintain communication with the flock.
This includes information about the flock status as well as copies of the
most recent data records from the flock.  The contents of this structure
cannot be accessed directly.
*/

/** \fn      char *fbDeviceName(int i)
    \ingroup SetupMethods

This function returns a serial port device name that is appropriate
for whatever operating system this code was compiled on.

\param  i    an integer between 0 and 3
\return      a serial port device name, or NULL if \em i is out of range
*/

char *fbDeviceName(int i)
{
#if defined(_WIN32) || defined(WIN32)
  static char *dev_names[] = { "COM1:", "COM2:", "COM3:", "COM4:", NULL };
#elif defined(__unix__) || defined(unix) || defined(__APPLE__)
#if defined(linux)
  static char *dev_names[] = { "/dev/ttyS0", "/dev/ttyS1",
                               "/dev/ttyUSB0", "/dev/ttyUSB1", NULL }; 
#elif defined(__APPLE__)
   static char *dev_names[] = { NULL, NULL, NULL, NULL, NULL };
#elif defined(sgi)
  static char *dev_names[] = { "/dev/ttyd1", "/dev/ttyd2",
                               "/dev/ttyd3", "/dev/ttyd4", NULL }; 
#else
  static char *dev_names[] = { "/dev/ttya", "/dev/ttyb", NULL, NULL, NULL }; 
#endif /* sgi */
#endif /* unix */

  int j;

#if defined(__APPLE__)
  static char devicenames[4][255+6];
  DIR *dirp;
  struct dirent *ep;
  
  dirp = opendir("/dev/");
  if (dirp == NULL) {
    return NULL;
  }

  j = 0;
  while ((ep = readdir(dirp)) != NULL && j < 4) {
    if (ep->d_name[0] == 'c' && ep->d_name[1] == 'u' &&
        ep->d_name[2] == '.')
    {
      strncpy(devicenames[j],"/dev/",5);
      strncpy(devicenames[j]+5,ep->d_name,255);
      devicenames[j][255+5] == '\0';
      dev_names[j] = devicenames[j];
      j++;
    }
  }

  while (j < 4) {
    dev_names[j] = NULL;
    j++;
  }

  closedir(dirp);
#endif /* __APPLE__ */

  /* guard against negative values */
  if (i < 0) {
    return NULL;
  }

  /* guard against values greater than the dev_names array length */
  for (j = 0; j < i; j++) {
    if (dev_names[j] == NULL) {
      return NULL;
    }
  }

  return dev_names[i];
}

/** \fn      fbird *fbNew() 
    \ingroup SetupMethods

Allocate an initialized fbird structure that can be used to
communicating with the flock.

\return   pointer to a new fbird structure
*/

fbird *fbNew() 
{
  fbird *fb;

  fb = (fbird *)malloc(sizeof(fbird));
  fb->device_name[0] = '\0';
  fb->baud_rate = FB_9600;
  fb->asynchronous = FB_NOTHREAD;
  fb->max_parameter = 32;
#if defined(_WIN32) || defined(WIN32)
  fb->file = INVALID_HANDLE_VALUE;
#elif defined(__unix__) || defined(unix) || defined(__APPLE__)
  fb->file = -1;
#endif
  fb->error = 0;
  fb->point = 0;
  fb->stream = 0;
  fb->error_handler = NULL;
  fb->error_text[0] = '\0';
  fb->group_mode = 0;
  fb->bird = 1;
  fb->num_birds = 1;
  fb->format[1] = FB_POSITION_ANGLES;
  fb->button_mode[1] = 0;
  fb->position_scale[1] = FB_STANDARD; 
  fb->RS232toFBB = 1;
  fb->phase_leftovers = 0;
 
#ifdef FLOCK_USE_THREADS
  fb->async_bird = 0;
  fb->async_error = 0;
  fb->async_data_rate = 0;
  fb->fresh_data = 0;
#endif

  return fb;
}

/** \fn      void fbDelete(fbird *fb)
    \ingroup SetupMethods

Release all computer resources associated with a flock structure.
This will terminate communication with the flock before the flock
structure is deallocated.

\param fb    pointer to an fbird structure created with fbNew()
*/

void fbDelete(fbird *fb)
{
#if defined(_WIN32) || defined(WIN32)
  if (fb->file != INVALID_HANDLE_VALUE) {
    fbClose(fb);
  }
#elif defined(__unix__) || defined(unix) || defined(__APPLE__)
  if (fb->file != -1) {
    fbClose(fb);
  }
#endif
  free(fb);
}

/*
int convert_baud_rate(fbird *fb, int rate)

Convert the baud rate into an operating-system specific form:
the baud rate must match the rate specified
by the bird's dip switches.

Valid values are FB_2400, FB_9600, FB_14400, FB_19200, FB_38400, 
FB_57600 and FB_115200. */

static int convert_baud_rate(int rate)
{
#if defined(_WIN32) || defined(WIN32)
  static int equiv[] = { CBR_2400,
                         CBR_4800,
                         CBR_9600,
                         CBR_19200,
                         CBR_38400,
                         CBR_57600,
                         CBR_115200 };
#elif defined(__unix__) || defined(unix) || defined(__APPLE__)
#if defined(sgi) && defined(__NEW_MAX_BAUD)
  static int equiv[] = { 2400,
                         4800,
                         9600,
                         19200,
                         38400,
                         57600,
                         115200 };
#elif defined(B115200)
  static int equiv[] = { B2400,
                         B4800,
                         B9600,
                         B19200,
                         B38400,
                         B57600,
                         B115200 };
#else
  static int equiv[] = { B2400,
                         B4800,
                         B9600, 
                         B19200, 
                         B38400,
                         -1,
                         -1 };
#endif /* sgi __NEW_MAX_BAUD */
#endif /* unix */

  switch (rate)
    {
    case 2400:
      rate = FB_2400;
      break;
    case 4800:
      rate = FB_4800;
      break;
    case 9600:
      rate = FB_9600;
      break;
    case 19200:
      rate = FB_19200;
      break;
    case 38400:
      rate = FB_38400;
      break;
    case 57600:
      rate = FB_57600;
      break;
    case 115200:
      rate = FB_115200;
      break;
    }
  if (rate < FB_2400 || rate > FB_115200) {
    return -1;
  }
  return equiv[rate];
}

/*------
return the maximum examine/change parameter for a given revision number,
not guaranteed to be perfectly accurate
*/

int max_parameter(int revision)
{
  if (revision >= (3 << 8) + 71) {
    return FB_XMTR_SERIAL_NUMBER;
  }
  if (revision >= (3 << 8) + 67) {
    return FB_SERIAL_NUMBER;
  }
  if (revision >= (3 << 8) + 66) {
    return FB_TRANSMITTER_MODE;
  }
  if (revision >= (3 << 8) + 45) {
    return FB_REFERENCE_FRAME;
  }
  return FB_ERROR_CODE_EXPANDED;
}    

/** \fn      int fbOpen(fbird *fb,
                        const char *device,
                        int baud,
                        int mode)
    \ingroup SetupMethods

Open communication between the computer and the flock.  This will
also probe the flock for information such as the addressing mode.
The serial port is set up to do XON/XOFF handshaking, and the RTS
line is set to low.

\param fb      pointer to an fbird structure
\param device  a valid serial port device name (see fbDeviceName())
\param baud    the baud rate for the flock, either as the actual
               desired baud rate or as one of the following constants:
               FB_2400, FB_4800, FB_9600, FB_19200, FB_38400,
               FB_57600, FB_115200
\param mode    the threading mode, which should be one of:
               - FB_NOTHREAD: disable multithreading
               - FB_THREAD: enable threading
               - FB_NONBLOCK: enable threading and allow duplicate records

\return        zero if fbOpen() was successful, or an error code otherwise
               (use fbGetErrorMessage() to get a text error description)

If it is unkown which serial port the flock is plugged into or what
baud rate the flock is set to, then fbOpen() can be called repeatedly
with different serial port names and baud rates in order to probe for
the flock.

The threading mode used is critical to the performance of communication
with the flock.  The flock is usually used in stream mode, which means
that it is continuously sending data record to the computer at a rate
of 100Hz.  The threading mode determines how the computer manages to
catch these data records and transfer the information to your application.

In FB_NOTHREAD mode, your application is responsibe for calling fbUpdate()
at least 100 times per second during stream mode.  If the application is
not able to do so, then the serial port buffer will fill up and data
records will become stale or might even be corrupted.  It is, however,
safe to use FB_NOTHREAD mode if only point mode data collection is used.

In FB_THREAD mode, a separate thread is spawned that listens to the
serial port while your application runs.  Each time your application calls
fbUpdate() the most recent data record received from the flock is returned.
If a new data record has not been recieved from the flock since the last
time fbUpdate() was called, then fbUpdate() will not return until a
fresh data record is transmitted from the flock.

The FB_NONBLOCK mode is almost identical to FB_THREAD mode, except that
a call to fbUpdate() will always return immediately even if no new
information has been recieved since the last call to fbUpdate().  This
means that a fbUpdate() call might return exactly the same information as
the previous fbUpdate().
*/

int fbOpen(fbird *fb, const char *device, int baud, int mode)
{
  int i;
#if defined(_WIN32) || defined(WIN32)   /* start of WIN32 portion of code -------- */
  static COMMTIMEOUTS default_ctmo = { 0, 2, /* return every char */
                                       TIMEOUT_PERIOD, 
                                       2, 
                                       TIMEOUT_PERIOD };
  DCB comm_settings;

  fb->asynchronous = mode;
#ifndef _MT
  if (mode != FB_NOTHREAD) {
    return set_error(fb,FB_MODE_ERROR,"bad mode: threading not supported");
  }
#endif

  baud = convert_baud_rate(baud);
  if (baud == -1) {
    return set_error(fb,FB_ILLEGAL_ERROR,"illegal baud rate");
  }

  if (device == NULL || strlen(device) > 255) {
    return set_error(fb,FB_ILLEGAL_ERROR,"illegal device name");
  }

  strcpy(fb->device_name,device);

  fb->file = CreateFile(fb->device_name,
                        GENERIC_READ|GENERIC_WRITE,
                        0,  /* not allowed to share ports */
                        0,  /* child-processes don't inherit handle */
                        OPEN_EXISTING, 
                        FILE_ATTRIBUTE_NORMAL,
                        NULL); /* no template file */

  if (fb->file == INVALID_HANDLE_VALUE) {
    return set_error(fb,FB_OPEN_ERROR,"couldn't open serial port");
  }

  if (SetupComm(fb->file,1024,1024) == FALSE ||
      SetCommTimeouts(fb->file,&default_ctmo) == FALSE || 
      GetCommState(fb->file,&comm_settings) == FALSE) {
    CloseHandle(fb->file);
    return set_error(fb,FB_COM_ERROR,"couldn't get serial port parameters");
  }

  comm_settings.fOutX = FALSE;             /* no S/W handshake on output */
  comm_settings.fInX = TRUE;               /* use XON/XOFF on input */
  comm_settings.XonChar = '\x11';
  comm_settings.XoffChar = '\x13';

  comm_settings.fAbortOnError = TRUE;      /* must clear errors */

  comm_settings.fOutxDsrFlow = TRUE;       /* allow modem-style handshaking*/
  comm_settings.fDtrControl = DTR_CONTROL_HANDSHAKE;  

  comm_settings.fOutxCtsFlow = FALSE;      /* no RTS/CTS handshake */
  comm_settings.fRtsControl = RTS_CONTROL_ENABLE; /* leave high until reset */

  comm_settings.ByteSize = 8;
  comm_settings.Parity = NOPARITY;
  comm_settings.StopBits = ONESTOPBIT;

  comm_settings.BaudRate = baud;  /* speed */

  if (SetCommState(fb->file,&comm_settings) == FALSE) {
    CloseHandle(fb->file);
    return set_error(fb,FB_COM_ERROR,"couldn't set serial port parameters");
  }

#elif defined(__unix__) || defined(unix) || defined(__APPLE__) /* start of UNIX portion of code -------------------*/

  static struct flock fl = { F_WRLCK, 0, 0, 0 }; /* for file locking */
  static struct flock fu = { F_UNLCK, 0, 0, 0 }; /* for file unlocking */
  struct termios t;

  fb->asynchronous = mode;
#ifndef _POSIX_THREADS
  if (mode != FB_NOTHREAD) {
    return set_error(fb,FB_MODE_ERROR,"bad mode: threading not supported");
  }
#endif
  baud = convert_baud_rate(baud);
  if (baud == -1) {
    return set_error(fb,FB_ILLEGAL_ERROR,"illegal baud rate");
  }

  if (device == NULL || strlen(device) > 255) {
    return set_error(fb,FB_ILLEGAL_ERROR,"illegal device name");
  }

  strcpy(fb->device_name,device);

  /* port is readable/writable and is blocking */
  fb->file = open(fb->device_name,O_RDWR|O_NOCTTY);

  if (fb->file == -1) {
    char text[80];
    sprintf(text,"couldn't open serial port %s",fb->device_name);
    return set_error(fb,FB_OPEN_ERROR,text);
  }

  /* get exclusive lock on the serial port */
  /* (on many unices, this has no effect for a device file but try anyway) */
#ifndef __APPLE__
  if (fcntl(fb->file, F_SETLK, &fl) == -1) {
    char text[80];
    sprintf(text,"serial port %s is in use",fb->device_name);
    close(fb->file);
    return set_error(fb,FB_OPEN_ERROR,text);
  }
#endif /* __APPLE__ */

  if (tcgetattr(fb->file,&t) == -1) { /* get I/O information */
#ifndef __APPLE__
    fcntl(fb->file, F_SETLK, &fu);
#endif /* __APPLE__ */
    close(fb->file);
    return set_error(fb,FB_COM_ERROR,"couldn't get serial port parameters");
  }

  /* 9600 baud, 8 data bits, no parity, 1 stop bit, enable read */
#if defined(sgi) && defined (__NEW_MAX_BAUD)
  t.c_cflag = CS8 | CREAD | CLOCAL;
  t.c_ospeed = baud;
#elif defined(__APPLE__)
  t.c_cflag = CS8 | CREAD | CLOCAL;
  cfsetispeed(&t, baud);
  cfsetospeed(&t, baud);
#else
  t.c_cflag = baud | CS8 | CREAD | CLOCAL;
#endif

  /* clear everything specific to terminals */
  t.c_lflag = 0;
  t.c_iflag = IXOFF; /* send XOFF when buffer is full */
  t.c_oflag = 0;

  t.c_cc[VMIN] = 0;                    /* use constant, not interval timout */
  t.c_cc[VTIME] = TIMEOUT_PERIOD/100;  /* wait for 1 secs max */

  if (tcsetattr(fb->file,TCSANOW,&t) == -1) { /* set I/O information */
#ifndef __APPLE__
    fcntl(fb->file, F_SETLK, &fu);
#endif /* __APPLE__ */
    close(fb->file);
    return set_error(fb,FB_COM_ERROR,"couldn't set serial port parameters");
  }
#endif /* unix */

  /* start of system-independent portion of code ---------------*/

  fbReset(fb);

  /* reset command will fail if there was no carrier detect */
  if (fb->error) {
    fbClose(fb);
    return set_error(fb,FB_COM_ERROR,"no device connected to serial port");
  }

#ifdef FLOCK_USE_THREADS
  if (fb->asynchronous != FB_NOTHREAD) {
    if (start_stream_thread(fb) == 0) {
      fb->asynchronous = FB_NOTHREAD;
      fbClose(fb);
      return set_error(fb,FB_RESOURCE_ERROR,"couldn't open streaming thread");
    }
  }
#endif

  i = fbExamineValue(fb,FB_REVISION);
  fb->revision = ((i >> 8) & 0x00ff) | ((i << 8) & 0xff00); 
  fb->max_parameter = max_parameter(fb->revision);
  if (fb->error) {
    fbClose(fb);
    return fb->error;
  }

  fb->position_scale[1] = fbExamineValue(fb,FB_POSITION_SCALING);
  if (fb->error) {
    fbClose(fb);
    return fb->error;
  }
  i = fbExamineValue(fb,FB_STATUS);
  fb->format[1] = ((i & FB_STATUS_FORMAT) >> 1) + (FB_POSITION-1);
  if (fb->error) {
    fbClose(fb);
    return fb->error;
  }

  fb->address_mode = FB_NORMAL;
  if (fb->revision > (3 << 8) + 33) {
    if ((i & FB_STATUS_EXPANDED) != 0) {
      fb->address_mode = FB_EXPANDED;
      if (fb->revision > (3 << 8) + 67) {
        fb->address_mode = fbExamineValue(fb,FB_FBB_ADDRESS_MODE);
      }
    }
  }

  if (fb->error) {
    fbClose(fb);
  }
  return fb->error;
}

/** \fn      void fbClose(fbird *fb)
    \ingroup SetupMethods

Shut down the flock and close communication.

\param fb      pointer to an fbird structure
*/

void fbClose(fbird *fb)
{
#if defined(_WIN32) || defined(WIN32)
  DCB comm_settings;
#elif defined(__unix__) || defined(unix) || defined(__APPLE__)
  static struct flock fu = { F_UNLCK, 0, 0, 0 }; /* for file unlocking */
  int term_bits;
#endif

  if (fb->stream) {
    fbEndStream(fb);
  }
#ifdef FLOCK_USE_THREADS
  if (fb->asynchronous != FB_NOTHREAD) {
    end_stream_thread(fb);
  }
#endif 

#if defined(_WIN32) || defined(WIN32)
  /* turn off the Flock by setting the RTS line */
  GetCommState(fb->file,&comm_settings);
  comm_settings.fRtsControl = RTS_CONTROL_ENABLE;
  SetCommState(fb->file,&comm_settings);

  CloseHandle(fb->file);
  fb->file = INVALID_HANDLE_VALUE;

#elif defined(__unix__) || defined(unix) || defined(__APPLE__)
  /* turn off the Flock by setting the RTS line */
  ioctl(fb->file, TIOCMGET, &term_bits);
  term_bits |= TIOCM_RTS;
  ioctl(fb->file, TIOCMSET, &term_bits);

  /* release our lock on the serial port */
#ifndef __APPLE__
  fcntl(fb->file, F_SETLK, &fu);
#endif /* __APPLE__ */
  close(fb->file);
  fb->file = -1;
#endif
}

/** \fn      void fbReset(fbird *fb)
    \ingroup SetupMethods

Reset the bird that is connected to the serial port by toggling the RTS line.

\param fb         pointer to an fbird structure

Note that this does not reset the whole flock.  After fbReset() is called,
fbFBBReset() can be called to reset the other birds.
*/

void fbReset(fbird *fb)
{
#if defined(_WIN32) || defined(WIN32)
  DCB comm_settings;
  DWORD comm_bits;
#elif defined(__unix__) || defined(unix) || defined(__APPLE__)
  int term_bits;
#endif

  if (fb->stream) {
    fbEndStream(fb);
  }

#if defined(_WIN32) || defined(WIN32)
  GetCommState(fb->file,&comm_settings);
  /* set the RTS line low: allows use of a normal serial cable */
  comm_settings.fRtsControl = RTS_CONTROL_ENABLE;  /* set high to reset */ 
  SetCommState(fb->file,&comm_settings);
  Sleep(1000);                                     /* hold for 1 sec */
  comm_settings.fRtsControl = RTS_CONTROL_DISABLE; /* clear */ 
  SetCommState(fb->file,&comm_settings);        
  Sleep(2000);                                     /* wait for bird to wake */

#elif defined(__unix__) || defined(unix) || defined(__APPLE__)
  /* set the RTS line low: allows use of a normal serial cable */
  ioctl(fb->file, TIOCMGET, &term_bits);
  term_bits |= TIOCM_RTS;
  ioctl(fb->file, TIOCMSET, &term_bits);
  usleep(500000); /* hold down 1 sec to reset */
  usleep(500000);   /* on IRIX the max sleep time is 1 second */
  term_bits &= ~TIOCM_RTS;
  ioctl(fb->file, TIOCMSET, &term_bits);
  usleep(500000); /* wait for 2 sec for bird to wake up */
  usleep(500000);
  usleep(500000);
  usleep(500000);
#endif /* unix */

  fb->RS232toFBB = 1;
  fb->stream = 0;
  fb->point = 0;
  fb->phase_leftovers = 0;
  fb->bird = 1;
  fb->error = 0;
  fb->error_text[0] = '\0';
  fb->group_mode = 0;
  fb->num_birds = 1;
  fb->format[1] = FB_POSITION_ANGLES;
  fb->button_mode[1] = 0;
  fb->position_scale[1] = FB_STANDARD; 
 
#ifdef FLOCK_USE_THREADS
  fb->async_bird = 0;
  fb->async_error = 0;
  fb->async_data_rate = 0;
  fb->fresh_data = 0;
#endif

#if defined(_WIN32) || defined(WIN32)
  GetCommModemStatus(fb->file,&comm_bits);
  if ((comm_bits & MS_RLSD_ON) == 0) {  /* check for carrier signal */
    set_error(fb,FB_COM_ERROR,"no carrier");
  }

  PurgeComm(fb->file,PURGE_TXCLEAR);
  PurgeComm(fb->file,PURGE_RXCLEAR);

#elif defined(__unix__) || defined(unix) || defined(__APPLE__)
  ioctl(fb->file, TIOCMGET, &term_bits);
  if ((term_bits & TIOCM_CAR) == 0) { /* check for carrier signal */
    set_error(fb,FB_COM_ERROR,"no carrier");
  }

  tcflush(fb->file,TCIOFLUSH);
#endif /* unix */
}  

/*---------------------------------------------------------------------*/
/** \defgroup FlockMethods The Flock of Birds Bus

If more than one bird is used, then the flock of birds bus or FBB must
be properly initialized through the use of fbFBBReset() followed by
fbFBBAutoConfig().  As well, for commands that must be directed towards
one specific bird the fbRS232ToFBB() function specifies what bird the
next command should be sent to.
*/

/** \fn      void fbFBBReset(fbird *fb)
    \ingroup FlockMethods

Reset the flock.  This should be called after fbReset() has
been used to reset the first bird in the flock.

\param fb         pointer to an fbird structure
*/

void fbFBBReset(fbird *fb)
{
  if (fb->stream) {
    fbEndStream(fb);
  }
  fbSendRaw(fb,"/",1);

#if defined(_WIN32) || defined(WIN32)
  Sleep(600);
#elif defined(__unix__) || defined(unix) || defined(__APPLE__)
  usleep(600000);
#endif
}

/** \fn     void fbFBBAutoConfig(fbird *fb,
                                 int num)
    \ingroup FlockMethods

Configure the flock for the specified number of birds.

\param fb         pointer to an fbird structure
\param num        the number of birds to configure for

The number of birds connected to the flock can be determined
by examining the FB_FBB_STATUS parameter via fbExamineValueBytes().
For each bird that is attached to the flock, the corresponding
byte will be nonzero.
*/

void fbFBBAutoConfig(fbird *fb, int num)
{
  int i;
  char text[3];
#if defined(_WIN32) || defined(WIN32)
  Sleep(600);
#elif defined(__unix__) || defined(unix) || defined(__APPLE__)
  usleep(600000);
#endif
  fb->num_birds = num;
  for (i = 1; i <= num; i++) {
    fb->format[i] = FB_POSITION_ANGLES;
    fb->button_mode[i] = 0;
    fb->position_scale[i] = FB_STANDARD; 
  }
  text[0] = FB_CHANGE_VALUE;
  text[1] = FB_FBB_AUTOCONFIG;
  text[2] = num;
  fbSendRaw(fb,text,3);
#if defined(_WIN32) || defined(WIN32)
  Sleep(600);
#elif defined(__unix__) || defined(unix) || defined(__APPLE__)
  usleep(600000);
#endif
}  

/** \fn      void fbRS232ToFBB(fbird *fb,
                               int bird)
    \ingroup FlockMethods

Inform the flock that the next command is to be sent
to a specific bird.  This is only applicable to certain
commands.

\param fb         pointer to an fbird structure
\param bird       the bird address (1 or greater)
*/

void fbRS232ToFBB(fbird *fb, int bird)
{
  char text[2];

  if (fb->address_mode == FB_SUPER_EXPANDED) {
    text[0] = FB_RS232_TO_FBB_SE;
    text[1] = bird;
    fbSendRaw(fb,text,2);
  }
  else {
    if (bird > 15) {
      text[0] = FB_RS232_TO_FBB_E - 0x10 + bird;
    }
    else {
      text[0] = FB_RS232_TO_FBB + bird;
    }
    fbSendRaw(fb,text,1);
  }

  fb->RS232toFBB = bird;
}

/*---------------------------------------------------------------------*/
/** \defgroup ConfigureMethods Configuring the Data Format

These methods are used to configure the data records that will be sent
from the flock.  It is necessary to call these methods before data is
requested from the flock, or the data format remains unspecified.
*/

/** \fn      void fbSetHemisphere(fbird *fb,
                                  int hemisphere)
    \ingroup ConfigureMethods

Set the tracking hemisphere for the flock.  The flock will only
correctly report the positions for birds within the specified
hemisphere.

\param fb         pointer to an fbird structure
\param hemisphere the hemisphere to use: one of FB_FORWARD,
                  FB_AFT, FB_UPPER, FB_LOWER, FB_LEFT, FB_RIGHT
*/

void fbSetHemisphere(fbird *fb, int hemisphere)
{
  short hemi_short = hemisphere;
  fbSendCommandWords(fb,FB_HEMISPHERE,&hemi_short);
}
  

/** \fn      void fbSetFormat(fbird *fb,
                              int format)
    \ingroup ConfigureMethods

Set the data format that will be used by the flock.  This must
be set before data records are requested from the flock.

\param fb         pointer to an fbird structure
\param format     one of the following: FB_POSITION, FB_ANGLES,
                  FB_MATRIX, FB_POSITION_ANGLES, FB_POSITION_MATRIX,  
                  FB_QUATERNION, FB_POSITION_QUATERNION

The most common data format is FB_POSITION_ANGLES, which is the most
compact format for the full six degrees of freedom.
*/

void fbSetFormat(fbird *fb, int format)
{
  char text[1];

  if (fb->stream) {
    fbEndStream(fb);
  }    
  fb->format[fb->RS232toFBB] = format;
  text[0] = format;

  fbSendRaw(fb,text,1);
}

/** \fn      void fbSetButtonMode(fbird *fb,
                                  int mode)
    \ingroup ConfigureMethods

Enable or disable the reporting of button information from the flock.

\param fb         pointer to an fbird structure
\param mode       0 or 1 depending on whether button information is desired
*/

void fbSetButtonMode(fbird *fb, int mode)
{
  char text[2];

  if (fb->stream) {
    fbEndStream(fb);
  }    
  fb->button_mode[fb->RS232toFBB] = mode;
  text[0] = FB_BUTTON_MODE;
  text[1] = mode;

  fbSendRaw(fb,text,2);
}

/*---------------------------------------------------------------------*/
/** \defgroup RequestMethods Requesting Data from the Flock

The flock has two primary methods for sending data records to the 
host computer: stream mode and point mode.

In stream mode, the flock sends data records for all birds in the flock
at a maximum rate of 100Hz.  Stream mode continues indefinitely until
it is interrupted.

In point mode, the flock will only send exactly one data record per bird
each time that data is requested from the flock.

By default, the flock will not be set to group mode and data records
will only be sent from one bird.  The flock can be put into group mode
by using fbChangeValue() to set the FB_GROUP_MODE parameter to 1.
*/

/** \fn      void fbStream(fbird *fb)
    \ingroup RequestMethods

Request the flock to begin streaming data records.
The default data rate is 100Hz.

\param fb         pointer to an fbird structure

Once the flock is in stream mode, fbUpdate() can be used
to prepare a data record for access via fbGetPosition() and
the other related functions.

Stream mode should only be used if fbOpen() was called with
either the FB_THREAD or FB_NONBLOCK mode.  In FB_NOTHREAD mode,
it is the responsibility of the application to call fbUpdate()
often enough to ensure that the serial port buffer does not
fill up.

Stream mode can be turned of by fbEndStream().  Note that stream
mode is automatically terminated by any of the following functions:
fbSetFormat(), fbSetButtonMode(), fbExamineValue(), fbClose(),
fbReset(), fbFBBReset(), fbPoint(), fbButtonRead().
*/

void fbStream(fbird *fb)
{
  if (fb->stream) {
    return;
  }
  fb->stream = 1;
  fb->bird = 1;

  fbSendRaw(fb,"@",1);

#ifdef FLOCK_USE_THREADS
  if (fb->asynchronous != FB_NOTHREAD) {
    int i;
    for (i = 1; i <= fb->num_birds; i++) {
       memset(fb->async_buffer[i],'\0',256);
       fb->async_buffer[i][2*data_len(fb->format[i]) \
                          + fb->button_mode[i]] = i;
    }
    /* wake-up the asynchronous stream thread */
#if defined(_WIN32) || defined(WIN32)
#ifdef _MT
    ReleaseMutex(fb->stream_mutex);
#endif
#elif defined(__unix__) || defined(unix) || defined(__APPLE__)
#ifdef _POSIX_THREADS
    pthread_mutex_unlock(&fb->stream_mutex);
#endif
#endif
  }
#endif
}

/** \fn      void fbEndStream(fbird *fb)
    \ingroup RequestMethods

Terminate streaming mode.

\param fb         pointer to an fbird structure
*/

void fbEndStream(fbird *fb)
{
  if (!fb->stream) {
    return;
  }

  fb->stream = 0;
  fbSendRaw(fb,"B",1);

#ifdef FLOCK_USE_THREADS
  if (fb->asynchronous != FB_NOTHREAD) { 
    /* put the asynchronous stream thread to sleep */ 
#if defined(_WIN32) || defined(WIN32)
#ifdef _MT
    WaitForSingleObject(fb->stream_mutex,INFINITE);
#endif
#elif defined(__unix__) || defined(unix) || defined(__APPLE__)
#ifdef _POSIX_THREADS
    pthread_mutex_lock(&fb->stream_mutex);
#endif
#endif
  }
#endif

#if defined(_WIN32) || defined(WIN32)
  PurgeComm(fb->file,PURGE_RXCLEAR);
#elif defined(__unix__) || defined(unix) || defined(__APPLE__)
  tcflush(fb->file,TCIFLUSH);
#endif
}

/** \fn      void fbPoint(fbird *fb)
    \ingroup RequestMethods

Request a single data record from the flock.

\param fb         pointer to an fbird structure

In group mode, this requests a single data record from each bird in
the flock.  The data records must then be retrieved by a call to
fbUpdate() for each one of the birds in the flock.

If there are multiple birds but the flock is not in group mode,
precede this command with fbRS232ToFBB() to get data from a single bird.

Using point mode to obtain information from the flock is not
as efficient as using stream mode.
*/

void fbPoint(fbird *fb)
{
  if (fb->stream) {
    fbEndStream(fb);
  }    

  if (!fb->group_mode) {
    fb->bird = fb->RS232toFBB;
  }
  else {
    fb->bird = 1;
  }

  fbSendRaw(fb,"B",1);

  if (!fb->error) {
    fb->point = 1;
  }
}   

/** \fn      void fbButtonRead(fbird *fb,
                               int *val)
    \ingroup RequestMethods

Read the button state.  This method should rarely be used.

\param fb         pointer to an fbird structure

\param val        space to store the button state

It is much better to use fbSetButtonMode() to force the
flock to return button information with each fbUpdate() call,
and to then use fbGetButton() to retrieve the button state.

In particular, the use of fbButtonRead() in stream mode will
cause stream mode to terminate.  The use of fbSetButtonMode()
prior to calling fbStream() will cause button information to
be streamed from the flock along with the position information.
*/

void fbButtonRead(fbird *fb, int *val)
{
  char data[1];

  if (fb->stream) {
    fbEndStream(fb);
  }    

  fbSendRaw(fb,"N",1);
  if (fb->error || val == 0) {
    return;
  }

  if (val != 0) {
    fbReceiveRaw(fb,data,1,0);
    *val = data[0] >> 4;
  }
}

/*---------------------------------------------------------------------*/
/** \defgroup DataMethods Decoding Flock Data

After a data record has been sent by the flock, the fbUpdate() method
can be used to retrieve it.  Data records can be requested from the
flock via either the fbStream() or fbPoint() methods.

After fbUpdate() has been called, the various fbGetXX() methods 
extract various pieces of information from the data record.  The
fbGetBird() method should always be used to check which bird the
data record is for, unless there is only a single bird.
*/

/** \fn      int fbUpdate(fbird *fb)
    \ingroup DataMethods

This is the central function in the flock interface: it retrieves
a single data record from the flock.

\param fb         pointer to an fbird structure

\return           only for FB_NONBLOCK mode: return value is 0 if no
                  new information has been received from the bird since
                  the last call (in FB_THREAD or FB_NOTHREAD mode the
                  return value is always 1)

If fbPoint() is used to request data records from the flock, then
every call to fbUpdate() must be preceeded by fbPoint().

If fbStream() is used to put the flock into stream mode, then
fbUpdate() is used to obtain the most recent data record that was
sent from the flock.

In particular, if fbOpen() was called with FB_THREAD set, then
fbUpdate() will wait for the next data record to be sent from the
flock or will return immediately if there is already a data record
waiting.  If fbOpen() was called with FB_NONBLOCK, then fbUpdate()
will always return immediately and the return value will be 0 if
a new data record has not yet arrived.
*/

int fbUpdate(fbird *fb)
{
  int len;

#ifdef FLOCK_USE_THREADS
  int fresh = 0;
  int async_error;

  if (fb->stream && fb->asynchronous != FB_NOTHREAD) {

#if defined(_WIN32) || defined(WIN32)
#ifdef _MT

    /* wait until a new data record arrives */
    if (fb->asynchronous != FB_NONBLOCK) {
      if (WaitForSingleObject(fb->data_event,TIMEOUT_PERIOD) == WAIT_TIMEOUT) {
        set_error(fb,FB_TIMEOUT_ERROR,
                  "timeout waiting for asynchronous data event");
        return fresh;
      }
    }

    /* get a lock on the data record */
    WaitForSingleObject(fb->data_mutex,INFINITE);
    len = 2*data_len(fb->format[fb->bird]) \
      + fb->button_mode[fb->bird] + fb->group_mode;
    memcpy(fb->data_buffer,fb->async_buffer[fb->bird],len);
    fb->timestamp_secs = fb->async_timestamp_secs;
    fb->timestamp_msecs = fb->async_timestamp_msecs;
    fresh = fb->fresh_data;
    fb->fresh_data = 0;
    async_error = fb->async_error;
    fb->async_error = 0;
    ReleaseMutex(fb->data_mutex);

#endif /* _MT */
#elif defined(__unix__) || defined(unix) || defined(__APPLE__)
#ifdef _POSIX_THREADS

    /* wait until a new data record arrives */
    if (fb->asynchronous != FB_NONBLOCK) {
      struct timeval tv;
#ifndef PTHREAD_COND_TIMEDWAIT_USES_TIMEVAL
      struct timespec ts;
#endif
      pthread_mutex_lock(&fb->fresh_data_mutex);
      if (!fb->fresh_data) {
        /* all the time stuff is used to check for timeouts */
        gettimeofday(&tv,0);
        tv.tv_sec += TIMEOUT_PERIOD/1000; /* msec to sec */ 
        tv.tv_usec += (TIMEOUT_PERIOD % 1000)*1000; /* msec to usec */
        if (tv.tv_usec >= 1000000) { /* if usec overflow */
          tv.tv_usec -= 1000000;
          tv.tv_sec += 1;
        }
#ifdef PTHREAD_COND_TIMEDWAIT_USES_TIMEVAL
        if (pthread_cond_timedwait(&fb->fresh_data_cond,
                                   &fb->fresh_data_mutex,&tv) 
            == ETIMEDOUT) {
#else   /* convert timeval to timespec */
        ts.tv_sec = tv.tv_sec;
        ts.tv_nsec = tv.tv_usec * 1000; 
        if (pthread_cond_timedwait(&fb->fresh_data_cond,
                                   &fb->fresh_data_mutex,&ts) 
            == ETIMEDOUT) {
#endif
          pthread_mutex_unlock(&fb->fresh_data_mutex);
          set_error(fb,FB_TIMEOUT_ERROR,
                    "timeout waiting for asynchronous data event");
          return fresh;
        }
      }
      pthread_mutex_unlock(&fb->fresh_data_mutex);
    }

    /* get a lock on the data record */
    pthread_mutex_lock(&fb->data_mutex);
    len = 2*data_len(fb->format[fb->bird]) \
      + fb->button_mode[fb->bird] + fb->group_mode;
    memcpy(fb->data_buffer,fb->async_buffer[fb->bird],len);
    fb->timestamp_secs = fb->async_timestamp_secs;
    fb->timestamp_msecs = fb->async_timestamp_msecs;
    fresh = fb->fresh_data;
    fb->fresh_data = 0;
    async_error = fb->async_error;
    fb->async_error = 0;
    pthread_mutex_unlock(&fb->data_mutex);

#endif /* _POSIX_THREADS */
#endif /* unix */

    /* check for error code set by stream thread */
    if (async_error) {
      if (async_error == FB_TIMEOUT_ERROR) {
        set_error(fb,FB_TIMEOUT_ERROR,"timeout while waiting for bird data");
      }
      else if (async_error == FB_IO_ERROR) {
        set_error(fb,FB_IO_ERROR,"I/O error on serial port read");
      } 
      else if (async_error == FB_PHASE_ERROR) {
        set_error(fb,FB_PHASE_ERROR,"received malformed data record");
      }
    }
    else if (fbGetBird(fb) != fb->bird) {
      if (fbGetBird(fb)) {
        fb->bird = fbGetBird(fb);
      }
      set_error(fb,FB_PHASE_ERROR,"received malformed data record");
    } 

    if (++fb->bird > fb->num_birds) {
      fb->bird = 1;
    }

    return fresh;
  }    
#endif /* FLOCK_USE_THREADS */

  len = 2*data_len(fb->format[fb->bird]) \
    + fb->button_mode[fb->bird] + fb->group_mode;
  fbReceiveRaw(fb,fb->data_buffer,len,0);
  set_timestamp(&fb->timestamp_secs,&fb->timestamp_msecs);

  if (fbGetBird(fb) != fb->bird) {
    if (fbGetBird(fb)) {
      fb->bird = fbGetBird(fb);
    }
    set_error(fb,FB_PHASE_ERROR,"received malformed data record");
  }

  if (++fb->bird > fb->num_birds) {
    fb->bird = 1;
    fb->point = 0;    /* finished point data */
  }

  return 1;
}

/** \fn      void fbGetPosition(fbird *fb,
                                float xyz[3])
    \ingroup DataMethods

Get the position returned in the last fbUpdate() data record.

\param fb         pointer to an fbird structure
\param xyz        storage space for the position to be returned in

The bird positions are only available if fbSetFormat() was called
with one of the following modes: FB_POSITION, FB_POSITION_ANGLES,
FB_POSITION_MATRIX, FB_POSITION_QUATERNION.
*/

void fbGetPosition(fbird *fb, float xyz[3])
{
  float range;
  char *cp;
  switch (fb->format[fb->bird])
    {
    case FB_POSITION:
    case FB_POSITION_ANGLES:
    case FB_POSITION_MATRIX:
    case FB_POSITION_QUATERNION:
      cp = &fb->data_buffer[0];
      break;
    default:
      return;
    }
  /* note: 914.4 mm == 36 inches */
  range = (fb->position_scale[fb->bird]+1)*914.4f;

  xyz[0] = (float)(fbUnpack(&cp)*range*0.000030517578125f);
  xyz[1] = (float)(fbUnpack(&cp)*range*0.000030517578125f);
  xyz[2] = (float)(fbUnpack(&cp)*range*0.000030517578125f);
}

/** \fn      void fbGetAngles(fbird *fb,
                              float zyx[3])
    \ingroup DataMethods

Get the euler angles returned in the last fbUpdate() data record.

\param fb         pointer to an fbird structure
\param zyx        storage space for the angles to be returned in

The bird angles are only available if fbSetFormat() was called
with one of the following modes: FB_ANGLES, FB_POSITION_ANGLES.
*/

void fbGetAngles(fbird *fb, float zyx[3])
{
  char *cp;
  switch (fb->format[fb->bird])
    {
    case FB_ANGLES:
      cp = &fb->data_buffer[0];
      break;
    case FB_POSITION_ANGLES:
      cp = &fb->data_buffer[6];
      break;
    default:
      return;
    }
  zyx[0] = (float)(fbUnpack(&cp)*PI*0.000030517578125f);
  zyx[1] = (float)(fbUnpack(&cp)*PI*0.000030517578125f);
  zyx[2] = (float)(fbUnpack(&cp)*PI*0.000030517578125f);
}

/** \fn      void fbGetMatrix(fbird *fb,
                              float a[9])
    \ingroup DataMethods

Get the matrix returned in the last fbUpdate() data record.

\param fb         pointer to an fbird structure
\param a          storage space for the nine matrix elements,
                  where the first three numbers are the first
                  column, the next three numbers are the middle
                  column, and the final three numbers are the
                  final matrix column

The bird matrix is only available if fbSetFormat() was called
with one of the following modes: FB_MATRIX, FB_POSITION_MATRIX.

It is almost always better to request angles from the flock
and convert the angles to a matrix using fbMatrixFromAngles()
because this reduces the amount of information that must be
transferred through the serial port.
*/

void fbGetMatrix(fbird *fb, float a[9])
{
  int i;
  char *cp;
  switch (fb->format[fb->bird])
    {
    case FB_MATRIX:
      cp = &fb->data_buffer[0];
      break;
    case FB_POSITION_MATRIX:
      cp = &fb->data_buffer[6];
      break;
    default:
      return;
    }
  for (i = 0; i < 9; i++) {
    a[i] = (float)(fbUnpack(&cp)*0.000030517578125f);
  }
}

/** \fn      void fbGetQuaternion(fbird *fb,
                                  float q[4])
    \ingroup DataMethods

Get the quaternion returned in the last fbUpdate() data record.

\param fb         pointer to an fbird structure
\param q          storage space for the quaternion to be returned in

The bird quaternion is only available if fbSetFormat() was called
with one of the following modes: FB_QUATERNION, FB_POSITION_QUATERNION.
*/

void fbGetQuaternion(fbird *fb, float q[4])
{
  int i;
  char *cp;
  switch (fb->format[fb->bird])
    {
    case FB_QUATERNION:
      cp = &fb->data_buffer[0];
      break;
    case FB_POSITION_QUATERNION:
      cp = &fb->data_buffer[6];
      break;
    default:
      return;
    }

  for (i = 0; i < 4; i++) {
    q[i] = (float)(fbUnpack(&cp)*0.000030517578125f);
  }
}

/** \fn      int fbGetButton(fbird *fb)
    \ingroup DataMethods

Get the button state returned in the last fbUpdate() data record.

\param fb         pointer to an fbird structure

\return           button state:
                  - 0x00 no button was pressed
                  - 0x10 left button was pressed
                  - 0x30 middle (or middle and left) was pressed
                  - 0x70 right (or right and any other) was pressed

The return value is always zero unless fbButtonMode() has been
used to turn on button reporting.
*/

int fbGetButton(fbird *fb)
{
  if (fb->button_mode[fb->bird]) {
    return fb->data_buffer[2*data_len(fb->format[fb->bird])] >> 4;
  }
  return 0;
}

/** \fn      int fbGetBird(fbird *fb)
    \ingroup DataMethods

Get the fbb address of the bird for the data record obtained through
the last fbUpdate().

\param fb         pointer to an fbird structure

\return           a value between 1 and \em n where \em n is the
                  number of birds in the flock, or 0 if an error
                  occurred

If the flock is not operating in group mode, then the return value
is always 1.  A return value of zero indicates that a phase error
or some other communication problem occurred with the flock.
*/

int fbGetBird(fbird *fb)
{
  int bird;
  if (fb->group_mode) {
    bird = fb->data_buffer[2*data_len(fb->format[fb->bird]) \
                          + fb->button_mode[fb->bird]];
    if (bird == 0) {
      bird = 1;
    }
  }
  else {
    bird = fb->bird;
  }
  /* if bird > num_birds, then then an error has occured: return 0
     to flag the error */
  if (bird > fb->num_birds || bird < 1) {
    bird = 0;
  }

  return bird;
}

/** \fn      double fbGetTime(fbird *fb)
    \ingroup DataMethods

Get the timestamp (in seconds since 1970) for the last fbUpdate().

\param fb         pointer to an fbird structure

\return           an absolute time value in seconds

The time is generated by the computer, not by the flock.  The time is
only accurate to within a few milliseconds.
*/

double fbGetTime(fbird *fb)
{
  return fb->timestamp_secs + 0.001*fb->timestamp_msecs;
}

/*---------------------------------------------------------------------*/
/** \defgroup ConversionMethods Data Format Conversion

These are helper functions that convert data from one format to
another in order to ease the decoding of data records sent by the flock.
*/

/** \fn      void fbMatrixFromAngles(float a[9],
                                     const float zyx[3])
    \ingroup ConversionMethods

Convert euler angles into a 3x3 matrix.

\param a        the nine matrix elements are stored here, column by column
\param zyx      the three angles
*/

void fbMatrixFromAngles(float a[9], const float zyx[3])
{
  double cx,sx,cy,sy,cz,sz;

  cz = cos((double)(zyx[0]));
  sz = sin((double)(zyx[0]));
  cy = cos((double)(zyx[1]));
  sy = sin((double)(zyx[1]));
  cx = cos((double)(zyx[2]));
  sx = sin((double)(zyx[2]));

  a[0] = (float)(cy*cz);
  a[1] = (float)(-cx*sz + sx*sy*cz);
  a[2] = (float)(sx*sz + cx*sy*cz);

  a[3] = (float)(cy*sz);
  a[4] = (float)(cx*cz + sx*sy*sz);
  a[5] = (float)(-sx*cz + cx*sy*sz);

  a[6] = (float)(-sy);
  a[7] = (float)(sx*cy);
  a[8] = (float)(cx*cy);
}

/** \fn      void fbAnglesFromMatrix(float zyx[3],
                                     const float a[9])
    \ingroup ConversionMethods

Does the opposite of fbMatrixFromAngles().

\param zyx        the three angles are stored here
\param a          the matrix
*/

void fbAnglesFromMatrix(float zyx[3], const float a[9])
{
  double r,cz,sz;

  r = sqrt((double)(a[0]*a[0] + a[3]*a[3]));
  cz = a[0]/r;
  sz = a[3]/r;

  zyx[0] = (float)atan2(sz, cz);
  zyx[1] = (float)atan2((double)(-a[6]), cz*a[0]+sz*a[3]);
  zyx[2] = (float)atan2(sz*a[2]-cz*a[5], -sz*a[1]+cz*a[4]);
}

/** \fn       int fbGetShort(const char *cp)
    \ingroup  ConversionMethods

A helper function that concatenates two characters into an unsigned short.

\param cp     pointer to the two characters

\return       the unsigned short value

*/

int fbGetShort(const char *cp)
{
  unsigned char lsb;
  short msb;

  lsb = *cp++;
  msb = *cp++;
  msb <<= 8;
  msb |= lsb;
  return msb; 
}

/*
  A simple function to silently fudge commands like FB_HEMISPHERE into
  their parameter values like FB_P_HEMISPHERE.

  This follows the the general programming rule that allowing a little
  sloppiness can be a good thing.
*/

static int fudge_parameter(int parameter)
{
  if (parameter == FB_HEMISPHERE) {
    parameter = FB_P_HEMISPHERE;
  }
  else if (parameter == FB_ANGLE_ALIGN2) {
    parameter = FB_P_ANGLE_ALIGN2;
  }
  else if (parameter == FB_REFERENCE_FRAME2) {
    parameter = FB_P_REFERENCE_FRAME2;
  }

  return parameter;
}

/*---------------------------------------------------------------------*/
/** \defgroup CommandMethods Sending Commands to the Flock

These are general-purpose methods for communicating with the flock.
The data sent to the flock and recived from the flock usually consists
of a single byte, multiple bytes, a single 2-byte word, or multiple
2-byte words.

Note that there are pre-defined functions to support
many of the flock commands, e.g. fbStream() for FB_STREAM
and fbSetFormat() for handling FB_POSITION_ANGLES and the other
data format mode commands.  As a result, there is rarely any need
to call fbSendCommand() directly.

The commands are listed in flock.h.  For a description of the
commands, see The Flock of Birds INSTALLATION AND OPERATION GUIDE
from Ascension Technology Corporation.
*/

/** \fn      int fbExamineValue(fbird *fb,
                                int parameter)
    \ingroup CommandMethods

Examine a flock parameter.

\param fb         pointer to an fbird structure
\param parameter  a constant that specifies what parameter to examine

\return           the integer value of the parameter

This function can only be used if the parameter value fits into a single
byte or into a single word, otherwise either fbExamineValueWords() or
fbExamineValueBytes() should be used instead.
*/

int fbExamineValue(fbird *fb, int parameter)
{
  char data[128];
  int len;

  parameter = fudge_parameter(parameter);

  len = fbExamineValueBytes(fb,parameter,data);
  
  if (len == 2) {
    return fbGetShort(data);
  }
  else if (len == 1) {
    return (unsigned char)data[0];
  }
  else {
    set_error(fb,FB_PARM_ERROR,"bad parameter for fbExamineValue");
    return 0;
  }
}  

/** \fn      int fbExamineValueWords(fbird *fb,
                                     int parameter,
                                     short *data)
    \ingroup CommandMethods

Examine a flock parameter that consists of 16-bit words.

\param fb         pointer to an fbird structure
\param parameter  a constant that specifies what parameter to examine
\param data       space to store the word values

\return           number of word values stored in the array
*/

int fbExamineValueWords(fbird *fb, int parameter, short *data)
{
  char text[128];
  int i,len;

  parameter = fudge_parameter(parameter);

  len = fbExamineValueBytes(fb,parameter,text);
  
  if (len % 2 != 0) {
    set_error(fb,FB_PARM_ERROR,"bad parameter for fbExamineValueWords");
    return 0;
  }

  for (i = 0; i < len/2; i++) {
    data[i] = fbGetShort(&text[2*i]);
  }

  return len/2;
}  

/* number of bytes in examine/change data */
static int examine_change_len_table[36] = { 2, 2, 2, 2,
                                            2,14, 2, 2, 
                                            1, 1, 1, 1, 
                                            14,14, 1,10, 
                                            2, 1, 1, 1,
                                            1, 1, 2, 6,
                                            6, 2, 2, 2,
                                            0, 0, 0, 0,
                                            2, 0, 0, 1 };

/** \fn      int fbExamineValueBytes(fbird *fb,
                                     int parameter,
                                     char *data)
    \ingroup CommandMethods

Examine a flock parameter that consists of bytes.

\param fb         pointer to an fbird structure
\param parameter  a constant that specifies what parameter to examine
\param data       space to store the byte values

\return           number of bytes stored in the array
*/
int fbExamineValueBytes(fbird *fb, int parameter, char *data)
{
  char text[2];
  int len;

  parameter = fudge_parameter(parameter);

  if (fb->stream) {
    fbEndStream(fb);
  }    

  text[0] = FB_EXAMINE_VALUE;
  text[1] = parameter;

  if (parameter <= fb->max_parameter) {
    len = examine_change_len_table[parameter];
  }
  else if (parameter == FB_FBB_DELAY) {
    len = 2;
  }
  else if (parameter == FB_GROUP_MODE && fb->revision >= (3 << 8) + 33) {
    len = 1;
  }
  else if (parameter == FB_FBB_STATUS) {
    len = 14;
    if (fb->address_mode == FB_EXPANDED) len = 30;
    if (fb->address_mode == FB_SUPER_EXPANDED) len = 126;
  }
  else if (parameter == FB_FBB_AUTOCONFIG) {
    len = 5;
    if (fb->address_mode == FB_EXPANDED) len = 7;
    if (fb->address_mode == FB_SUPER_EXPANDED) len = 19;
  }
  else {
    set_error(fb,FB_PARM_ERROR,"bad examine value parameter");
    return 0;
  }

  fbSendRaw(fb,text,2);
  if (fb->error || data == 0)
    return 0;
 
  fbReceiveRaw(fb,data,len,0);

  return len;
}

/** \fn       void fbPutShort(char *cp,
                              int val)
    \ingroup  ConversionMethods

Store the low 16 bits of an integer in two bytes.

\param cp      space to store the resulting two bytes
\param val     the integer to convert
*/

void fbPutShort(char *cp, int val)
{
  unsigned char lsb;
  short msb;

  lsb = val;
  msb = val;
  msb >>= 8;
  msb &= 0x00ff;

  *cp++ = lsb;
  *cp++ = (unsigned char)msb;
}

/** \fn      void fbChangeValue(fbird *fb,
                                int param,
                                int val)
    \ingroup CommandMethods

Modify an 8-bit or 16-bit flock parameter.

\param fb         pointer to an fbird structure
\param parameter  a constant that specifies what parameter to modify
\param val        the new parameter value
*/

void fbChangeValue(fbird *fb, int parameter, int val)
{
  char data[16];
  int output_data_len;
  parameter = fudge_parameter(parameter);
  
  output_data_len = 0;
  
  if (parameter <= 32) {
    output_data_len = examine_change_len_table[parameter];
  }
  else if (parameter == FB_GROUP_MODE) {
    output_data_len = 1;
  }
  else if (parameter == FB_FBB_AUTOCONFIG) {
    output_data_len = 1;
  }

  if (output_data_len == 1) {
    data[0] = val;
  }
  else if (output_data_len == 2) {
    fbPutShort(data,val);
  }
  else {
    set_error(fb,FB_PARM_ERROR,"bad parameter for fbChangeValue");
    return;
  }

  fbChangeValueBytes(fb,parameter,data);
}

/** \fn      void fbChangeValueWords(fbird *fb,
                                     int param,
                                     const short *data)
    \ingroup CommandMethods

Modify a flock parameter that consists of several words.

\param fb         pointer to an fbird structure
\param parameter  a constant that specifies what parameter to modify
\param data       the new parameter data
*/

void fbChangeValueWords(fbird *fb, int parameter, const short *data)
{
  char text[16];
  int i, output_data_len;
  parameter = fudge_parameter(parameter);
  
  output_data_len = 0;
  
  if (parameter <= 32) {
    output_data_len = examine_change_len_table[parameter];
  }
  else if (parameter == FB_GROUP_MODE) {
    output_data_len = 1;
  }
  else if (parameter == FB_FBB_AUTOCONFIG) {
    output_data_len = 1;
  }

  if (output_data_len % 2 != 0) {
    set_error(fb,FB_PARM_ERROR,"bad parameter for fbChangeValueWords");
    return;
  }

  for (i = 0; i < output_data_len/2; i++) {
    fbPutShort(&text[2*i],data[i]);
  }
  fbChangeValueBytes(fb,parameter,text);
}

/** \fn      void fbChangeValueBytes(fbird *fb,
                                     int param,
                                     const char *data)
    \ingroup CommandMethods

Modify a flock parameter that consists of several bytes.

\param fb         pointer to an fbird structure
\param parameter  a constant that specifies what parameter to modify
\param data       the new parameter data
*/

void fbChangeValueBytes(fbird *fb, int parameter, const char *data)
{
  char text[16];
  int output_data_len;
  parameter = fudge_parameter(parameter);

  text[0] = (char)FB_CHANGE_VALUE;
  text[1] = (char)parameter;

  if (parameter == FB_POSITION_SCALING) {
    fb->position_scale[fb->RS232toFBB] = fbGetShort(data);
  }

  if (parameter <= fb->max_parameter) {
    output_data_len = examine_change_len_table[parameter];
  }
  else if (parameter == FB_GROUP_MODE && fb->revision >= (3 << 8) + 33) {
    fb->group_mode = data[0];
    output_data_len = 1;
  }
  else if (parameter == FB_FBB_STATUS) {
    output_data_len = 0;
  }
  else if (parameter == FB_FBB_AUTOCONFIG) {
    fbFBBAutoConfig(fb,data[0]);
    return;
  }
  else {
    set_error(fb,FB_PARM_ERROR,"bad parameter for fbChangeValueBytes");
    return;
  }

  memcpy(&text[2],data,output_data_len);

  fbSendRaw(fb,text,output_data_len+2);    
}

/** \fn      void fbSendCommand(fbird *fb,
                                int command)
    \ingroup CommandMethods

Send a command to the flock with no arguments.

\param fb         pointer to an fbird structure
\param command    a constant that specifies what command to send

This function is appropriate for the following commands only:
FB_REPORT_RATE_FULL, FB_REPORT_RATE_DIV2, FB_REPORT_RATE_DIV8,
FB_REPORT_RATE_DIV32, FB_RUN, FB_SLEEP, FB_XOFF, FB_XON.
*/

void fbSendCommand(fbird *fb, int c)
{
  fbSendCommandBytes(fb,c,0);
}

/** \fn      void fbSendCommandWords(fbird *fb,
                                     int command,
                                     const short *data)
    \ingroup CommandMethods

Send a command to the flock along with the data words associated
with the command.

\param fb         pointer to an fbird structure
\param command    a constant that specifies what command to send
\param data       the data to send with the command

This function is only appropriate for the following commands:
FB_ANGLE_ALIGN1 (6 words), FB_REFERENCE_FRAME1 (6 words),
FB_ANGLE_ALIGN2 (3 words), FB_REFERENCE_FRAME2 (3 words),
FB_HEMISPHERE (1 word) or FB_SYNC (1 word).
*/

void fbSendCommandWords(fbird *fb, int c, const short *output_data)
{
  char text[16];
  int output_data_len,i;

  output_data_len = 0;

  switch (c)
    {
    case FB_ANGLE_ALIGN1:
    case FB_REFERENCE_FRAME1:
      output_data_len = 6;
      break;
    case FB_ANGLE_ALIGN2:
    case FB_REFERENCE_FRAME2:
      output_data_len = 3;
      break;
    case FB_HEMISPHERE:
    case FB_SYNC:
      output_data_len = 1;
      break;
    default:
      set_error(fb,FB_COMMAND_ERROR,"bad command for fbSendCommandWords");
      return;
    }

  for (i = 0; i < output_data_len; i++) {
    fbPutShort(&text[2*i],output_data[i]);
  }

  fbSendCommandBytes(fb,c,text);
}

/** \fn      void fbSendCommandBytes(fbird *fb,
                                     int command,
                                     const char *data)
    \ingroup CommandMethods

Send a command to the flock along with the data bytes associated
with the command.

\param fb         pointer to an fbird structure
\param command    a constant that specifies what command to send
\param data       the data to send with the command, or NULL if there
                  is no data associated with the command

This function can be used to send any command to the flock.
*/

void fbSendCommandBytes(fbird *fb, int c, const char *output_data)
{
  int output_data_len,i;
  char text[16];

  switch (c)
    {
    case FB_EXAMINE_VALUE:
      fbExamineValueBytes(fb,output_data[0],0);
      return;
    case FB_CHANGE_VALUE:
      fbChangeValueBytes(fb,output_data[0],&output_data[1]);
      return;
    case FB_POINT:
      fbPoint(fb);
      return;
    case FB_BUTTON_READ:
      fbButtonRead(fb,0);
      return;
    case FB_STREAM:
      fbStream(fb);
      return;
    case FB_POSITION:
    case FB_ANGLES:
    case FB_MATRIX:
    case FB_POSITION_ANGLES:
    case FB_POSITION_MATRIX:
    case FB_QUATERNION:
    case FB_POSITION_QUATERNION:
      fbSetFormat(fb,c);
      return;
    case FB_BUTTON_MODE:
      fbSetButtonMode(fb,output_data[0]);
      return;
    case FB_FBB_RESET:
      fbFBBReset(fb);
      return;
    case FB_REPORT_RATE_FULL:
    case FB_REPORT_RATE_DIV2:
    case FB_REPORT_RATE_DIV8:
    case FB_REPORT_RATE_DIV32:
    case FB_RUN:
    case FB_SLEEP:
    case FB_XOFF:
    case FB_XON:
      output_data_len = 0;
      break;
    case FB_ANGLE_ALIGN1:
    case FB_REFERENCE_FRAME1:
      output_data_len = 12;
      break;
    case FB_ANGLE_ALIGN2:
    case FB_REFERENCE_FRAME2:
      output_data_len = 6;
      break;
    case FB_HEMISPHERE:
    case FB_SYNC:
      output_data_len = 2;
      break;
    case FB_NEXT_TRANSMITTER:
      output_data_len = 1;
      break;
    case FB_RS232_TO_FBB_SE:
      fbRS232ToFBB(fb,output_data[0]);
      return;
    default:
      if (c >= FB_RS232_TO_FBB && c < FB_RS232_TO_FBB+16) {
        fbRS232ToFBB(fb,c-FB_RS232_TO_FBB);
        return;
      }
      if (c >= FB_RS232_TO_FBB_E && c < FB_RS232_TO_FBB) {
        fbRS232ToFBB(fb,c - FB_RS232_TO_FBB_E + 0x10);
        return;
      }
      set_error(fb,FB_COMMAND_ERROR,"unrecognized command sent to bird");
      return;
    }

  text[0] = c;
  if (output_data) {
    for (i = 0; i < output_data_len; i++)
      text[i+1] = output_data[i];
  }
  fbSendRaw(fb,text,output_data_len+1);
}

/*---------------------------------------------------------------------*/
/** \defgroup RawMethods Internal Methods

These methods send raw data to the flock and read raw data from the flock.
They should only be used by someone who is very familiar both with the
flock of birds and with the driver code.
*/

/** \fn      void fbSendRaw(fbird *fb,
                            const char *text,
                            int len)
    \ingroup RawMethods

This function is meant primarily for internal use.  It sends a
raw stream of bytes to the flock.

\param fb         pointer to an fbird structure
\param text       the bytes to send to the flock
\param len        the number of bytes to send

If a command is sent to the flock with this function that causes
the state of the flock to change, then communication with the flock
might be disrupted.  The fbSendCommandBytes() function should be
used instead of fbSendRaw() and fbReceiveRaw() in all circumstances.
*/

void fbSendRaw(fbird *fb, const char *text, int len)
{
  int error;

#if defined(_WIN32) || defined(WIN32)
  DWORD m,n,dumb;

#ifdef _MT
  int asynchronous;
  asynchronous = fb->asynchronous;

  if (asynchronous != FB_NOTHREAD) {
    WaitForSingleObject(fb->file_mutex,INFINITE);
  }
#endif /* _MT */

#elif defined(__unix__) || defined(unix) || defined(__APPLE__)
  int i,n;
  struct timeval start_time,tv;

#ifdef _POSIX_THREADS
  int asynchronous;
  asynchronous = fb->asynchronous;

  if (asynchronous != FB_NOTHREAD) {
    pthread_mutex_lock(&fb->file_mutex);
  }
#endif /* _POSIX_THREADS */
#endif /* unix */

  /* clear bird address from last RS232ToFBB command */
  fb->RS232toFBB = 1;

  n = len;
  error = 0;
  
  /* fprintf(stderr,"%s\n",text);   debug line - print output */

#if defined(_WIN32) || defined(WIN32)
  while (WriteFile(fb->file,text,n,&m,NULL) == FALSE) {
    if (GetLastError() == ERROR_OPERATION_ABORTED) {/* system cancelled us */
      ClearCommError(fb->file,&dumb,NULL); /* so clear error */
    }
    else {
      error = FB_IO_ERROR;
      break;
    }
  }
  if (!error && m != n) {  /* incomplete write: must have timed out */
    error = FB_TIMEOUT_ERROR;
  }

#ifdef _MT
  if (asynchronous != FB_NOTHREAD) {
    ReleaseMutex(fb->file_mutex);   /* release comm port */
  }
#endif

#elif defined(__unix__) || defined(unix) || defined(__APPLE__)
  gettimeofday(&start_time,0);
  while ((i = write(fb->file,text,n)) != n) { 
    if (i == -1 && errno != EAGAIN) {
      error = FB_IO_ERROR;
      break;
    }
    n -= i;
    gettimeofday(&tv,0);
    tv.tv_sec -= start_time.tv_sec;
    tv.tv_usec -= start_time.tv_usec;
    if (tv.tv_sec*1000 + tv.tv_usec/1000 > TIMEOUT_PERIOD) {
      error = FB_TIMEOUT_ERROR;
      break;
    }
  }
#ifdef _POSIX_THREADS
  if (asynchronous != FB_NOTHREAD) {
    pthread_mutex_unlock(&fb->file_mutex);   /* release comm port */
  }
#endif
#endif /* unix */

  if (error == FB_IO_ERROR) {
    set_error(fb,FB_IO_ERROR,"I/O error on serial port write");
  }
  else if (error == FB_TIMEOUT_ERROR) {
    set_error(fb,FB_TIMEOUT_ERROR,"timeout on serial port write");
  }
}

/** \fn      void fbReceiveRaw(fbird *fb,
                               char *reply,
                               int len,
                               int thread)
    \ingroup RawMethods

This function is meant primarily for internal use.  It reads a
raw stream of bytes from the flock.

\param fb         pointer to an fbird structure
\param reply      the bytes read from the flock
\param len        the number of bytes to read
\param thread     0 if the function was called from the application,
                  or 1 if the function was called from the streaming
                  thread
*/

void fbReceiveRaw(fbird *fb, char *reply, int len, int thread)
{
  int error;

  /* WIN32 code ------------------------*/
#if defined(_WIN32) || defined(WIN32)
  DWORD m,n,dumb;
  int i = 0;

#ifdef _MT
  int asynchronous;
  asynchronous = fb->asynchronous;

  if (asynchronous != FB_NOTHREAD) { /* request comm port */
    WaitForSingleObject(fb->file_mutex,INFINITE);
  }
#endif

  /* unix code ------------------------*/
#elif defined(__unix__) || defined(unix) || defined(__APPLE__)
  int m,n,i;

#ifdef _POSIX_THREADS
  int asynchronous;
  asynchronous = fb->asynchronous;

  if (asynchronous != FB_NOTHREAD) { /* request comm port */
    pthread_mutex_lock(&fb->file_mutex);
  }
#endif 
#endif /* unix */

  /* shared code ------------------------*/
  error = 0;
  n = len;
  i = 0;

  /* fprintf(stderr,"receiving: %d\n",n);  debug line - print output */

  if (fb->stream || fb->point) { /* correct for previous phase error */
    i = fb->phase_leftovers;
    n -= i;
  }
  fb->phase_leftovers = 0;

  /* WIN32 code ------------------------*/
#if defined(_WIN32) || defined(WIN32)
  while (ReadFile(fb->file,&reply[i],n,&m,NULL) == FALSE) {
    if (GetLastError() == ERROR_OPERATION_ABORTED) {/* cancelled */
      ClearCommError(fb->file,&dumb,NULL); /* so clear error */
      n -= m; /* number of chars read so far */
      i += m;
    }
    else {
      error = FB_IO_ERROR;
      break;
    }
  }
  if (!error && n != m) {
    error = FB_TIMEOUT_ERROR;
  }

#ifdef _MT
  if (asynchronous != FB_NOTHREAD) {
    ReleaseMutex(fb->file_mutex);   /* release comm port */
  }
#endif

  /* unix code ------------------------*/
#elif defined(__unix__) || defined(unix) || defined(__APPLE__)
  while (!error && (m = read(fb->file,&reply[i],n)) != n) {
    /* fprintf(stderr,"m = %d, n = %d, i = %d\n",m,n,i); */
    if (m == -1 && errno != EAGAIN) {    /* if problem is not 'temporary,' */ 
      error = FB_IO_ERROR;
    }
    else if (m == 0) {
      error = FB_TIMEOUT_ERROR;
    }
    else {
      i += m;
      n -= m;
    }
  }      
  /* fprintf(stderr,"m = %d, n = %d, i = %d\n",m,n,i); */
  
#ifdef _POSIX_THREADS
  if (asynchronous != FB_NOTHREAD) {
    pthread_mutex_unlock(&fb->file_mutex);   /* release comm port */
  }
#endif

#endif /* unix */

  /* shared code ------------------------*/
  if ((fb->stream || fb->point) && !error) {  /* check for phase errors */
    if (!(reply[0] & 0x80)) {
      error = FB_PHASE_ERROR;
    }
    for (i = 1; i < len; i++) {
      if (reply[i] & 0x80) {
        error = FB_PHASE_ERROR;
        memmove(reply,&reply[i],len-i);
        fb->phase_leftovers = len-i;
        break;
      }
    }
  }

  if (!thread) {
    if (error == FB_IO_ERROR) {
      set_error(fb,FB_IO_ERROR,"I/O error on serial port read");
    }
    else if (error == FB_TIMEOUT_ERROR) {
      set_error(fb,FB_TIMEOUT_ERROR,"timeout while waiting for bird data");
    }
    else if (error == FB_PHASE_ERROR) {
      set_error(fb,FB_PHASE_ERROR,"received malformed data record");
    }
  }
#ifdef FLOCK_USE_THREADS
  else if (fb->async_error == 0) {
    fb->async_error = error;
  }
#endif
}

/** \fn       int fbUnpack(char **cp)
    \ingroup  RawMethods

A helper function: unpack two characters sent from the flock into
a short integer, and advance the character pointer by two.  This
will convert the data from the flock's special 7-bit data record
encoding into conventional 8-bit data.

\param cp    pointer to a data string from the flock

\return      the unpacked data
*/

int fbUnpack(char **cp)
{
  unsigned char lsb;
  short msb;

  lsb = *(*cp)++;
  msb = *(*cp)++;
  lsb <<= 1;
  msb <<= 8;
  msb |= lsb;
  msb <<= 1;
  return msb;
}

/*---------------------------------------------------------------------*/
/** \defgroup ErrorMethods Error Checking

These methods are used to check whether an error occured as a result of
an attempt to communicate with the flock.
*/

/** \fn      int fbGetError(fbird *fb)
    \ingroup ErrorMethods

Return the last error code and clear the error indicator.

\param fb         pointer to an fbird structure

\return           integer error code, or zero if no error

Note that the error codes are generated by the host computer,
not by the flock.  To check the error code for the flock,
use fbExamineValue() to get the value of the FB_ERROR_CODE
parameter.

All of the flock functions can generate errors except for
the following:  fbGetButton(), fbGetPosition(), fbGetAngles(),
fbGetQuaternion(), fbGetMatrix().
*/

int fbGetError(fbird *fb)
{
  int error = fb->error;
  fb->error = 0;
  return error;
}

/** \fn      char *fbGetErrorMessage(fbird *fb)
    \ingroup ErrorMethods

Return some text that describes the last error.

\param fb         pointer to an fbird structure

\return           text for the last error
*/

char *fbGetErrorMessage(fbird *fb)
{
  return fb->error_text;
}

/** \fn      void fbSetErrorCallback(fbird *fb,
                                     void (*callback)(void *data),
                                     void *data)
    \ingroup ErrorMethods

Set a callback function for error handling.  This function will
be called whenever there is a communications error with the flock.
it is not necessary to set an error callback.

\param fb         pointer to an fbird structure
\param callback   pointer to callback function
\param data       pointer to data that will be sent to callback function
*/

void fbSetErrorCallback(fbird *fb, void (*callback)(void *data),
                        void *data)
{
  fb->error_handler_data = data;
  fb->error_handler = callback;
}

/*-------------------------------------------------------------
The following code is for internal use only
*/
      
static int set_error(fbird *fb, int error_code, const char *text)
{
  fb->error = error_code;
  strncpy(fb->error_text,text,255);
  fb->error_text[255] = '\0';
  if (fb->error_handler) {
    fb->error_handler(fb->error_handler_data);
    fb->error = 0;
  }
  return error_code;
}

static void set_timestamp(long *sec, long *msec)
{
#if defined(_WIN32) || defined(WIN32)
  /* The ftime() system time isn't precise enough, but the 
     QueryPerformanceCounter() function doesn't give the
     absolute time value that is desired.
     So we read them both once to get an offset to add to
     the QueryPerformanceCounter() value.
     (This is a quick-and-dirty type fix, it really should
     average the system time and it should correct for drift
     in the performance time) */
  static int perf_initialized = 0;
  static CRITICAL_SECTION perf_lock;
  static LARGE_INTEGER perf_freq;
  static LARGE_INTEGER perf_offset;
  static int has_perf = -1;
  LARGE_INTEGER perf_time;
  struct timeb curr_time;

  /* do this once only */
  if (!perf_initialized) {
    InitializeCriticalSection(&perf_lock);
    EnterCriticalSection(&perf_lock);
    /* re-check just in case another thread has already done it */
    if (!perf_initialized) {
      has_perf = QueryPerformanceFrequency(&perf_freq);
      if (has_perf) {
  QueryPerformanceCounter(&perf_offset);
  ftime(&curr_time);
  perf_offset.QuadPart = ((curr_time.time*perf_freq.QuadPart
         + curr_time.millitm*perf_freq.QuadPart/1000) 
        - perf_offset.QuadPart);
      }
      perf_initialized = 1;
    }
    LeaveCriticalSection(&perf_lock);
  }
  if (has_perf) {
    QueryPerformanceCounter(&perf_time);
    perf_time.QuadPart = perf_time.QuadPart + perf_offset.QuadPart;
      *sec = (time_t)(perf_time.QuadPart
          /perf_freq.QuadPart);
      *msec = (unsigned short)((perf_time.QuadPart
        %perf_freq.QuadPart)*1000
             /perf_freq.QuadPart);
  }
  else {
    ftime(&curr_time);
    *sec = curr_time.time;
    *msec = curr_time.millitm;
  }
#elif defined(__unix__) || defined(unix) || defined(__APPLE__)
  struct timeval curr_time;
  gettimeofday(&curr_time, 0);
  *sec = curr_time.tv_sec;
  *msec = curr_time.tv_usec/1000;
#endif /* unix */
}  

/*-------------------------------------------------------------
The following code is used only in asynchronous mode.  It is
non-portable and is only compiled if threading is available.
*/

/*-----
void stream_thread(void *user_data)

The stream thread starts up when fbOpen() is called and
is not terminated until fbClose() or fbDelete() are called.
The thread waits around in a state of suspended animation
until fbStream() is called.

This function contains the code that is run by the thread.
The thread ends when this function returns.  

The function consists of a loop that does the following:

1) Check fb->stream_mutex, which is unlocked only when the flock
   is put into stream mode by a '@' character e.g. by fbStream().
   This ensures that the thread is only running if the Flock is
   actually streaming data.
2) Check fb->asyncronous to see whether the program wants the
   thread to stop, i.e. to see whether end_stream_thread() has
   been called.
3) Get the data record from the flock via the serial port.
4) Copy the data record into the flock structure, after the flock 
   structure has been locked so that the application thread cannot
   look at the data until the copy is complete
5) Inform the application that a new data record has arrived
   (use an Event under Windows and a 'cond' under UNIX)


The start_stream_thread() initializes all of the data locking structures
and starts the stream thread.

The end_stream_thread() signals for the stream thread to stop and, once
the thread has stopped, frees up all of the data locking stuctures.
*/

#ifdef FLOCK_USE_THREADS

#if defined(_WIN32) || defined(WIN32)
#ifdef _MT

static void stream_thread(void *user_data)
{
  long curr_time_sec, curr_time_msec;
  long old_time_sec, old_time_msec;
  int count,oldcount,len,check_bird;
  char buffer[256];
  fbird *fb;
  fb = (fbird *)user_data;
  
  old_time_sec = 0;
  fb->async_bird = 0;

  /* the stream-recieve loop */
  for (count = 0;; count++) {

    /* the stream_mutex is used to wake up/put to sleep this thread */
    WaitForSingleObject(fb->stream_mutex,INFINITE);
    ReleaseMutex(fb->stream_mutex);

    if (fb->asynchronous == FB_NOTHREAD) {
      break; /* no longer in asynchronous mode: terminate thread */ 
    }

    fb->async_bird = (fb->async_bird % fb->num_birds) + 1;
    len = 2*data_len(fb->format[fb->async_bird]) \
      + fb->button_mode[fb->async_bird] + fb->group_mode;
    fbReceiveRaw(fb,buffer,len,1);
    set_timestamp(&curr_time_sec,&curr_time_msec);

    /* make sure we are receiving data from the bird we think we are */
    if (fb->group_mode) {
      check_bird = buffer[len-1];
      if (check_bird > 0 && check_bird <= fb->num_birds) {
        fb->async_bird = check_bird;
      }
      else if (check_bird == 0) {
        fb->async_bird = 1;
      }
    }

    WaitForSingleObject(fb->data_mutex,INFINITE);    
    memcpy(fb->async_buffer[fb->async_bird],buffer,len);
    fb->fresh_data = 1;
    fb->async_timestamp_secs = curr_time_sec;
    fb->async_timestamp_msecs = curr_time_msec;
    ReleaseMutex(fb->data_mutex);
    SetEvent(fb->data_event);

    /* calculate the refresh rate every second */
    if (curr_time_sec > old_time_sec 
        && curr_time_msec > old_time_msec) { 
      if (old_time_sec != 0) { /* calc hertz */
        fb->async_data_rate = count - oldcount;
      }
      old_time_sec = curr_time_sec;
      old_time_msec = curr_time_msec;
      oldcount = count;
    }
  }
  // thread automatically terminates on return
}

static int start_stream_thread(fbird *fb)
{
  fb->file_mutex = CreateMutex(0,FALSE,0);  /* lock on serial port */
  fb->data_mutex = CreateMutex(0,FALSE,0);  /* lock on fb->async_ elements */
  fb->data_event = CreateEvent(0,FALSE,FALSE,0); /* event for new data */
  fb->stream_mutex = CreateMutex(0,FALSE,0); /* only unlocked in stream mode */
  WaitForSingleObject(fb->stream_mutex,INFINITE);  
  fb->stream_thread = (HANDLE)_beginthread(&stream_thread,8*1024,fb);
  if (fb->stream_thread == INVALID_HANDLE_VALUE) {
    CloseHandle(fb->file_mutex);
    CloseHandle(fb->data_mutex);
    CloseHandle(fb->data_event);
    CloseHandle(fb->stream_mutex);
    return 0;
  }
  /* this thread spends most of its time just waiting on the serial port */
  SetThreadPriority(fb->stream_thread,THREAD_PRIORITY_TIME_CRITICAL);
  
  return 1;  /* success */ 
}

static void end_stream_thread(fbird *fb)
{
  int async = fb->asynchronous;
  fb->asynchronous = FB_NOTHREAD;  /* this signals thread to stop */
  if (!fb->stream) { 
    ReleaseMutex(fb->stream_mutex); /* this wakes the thread up */
  }
  WaitForSingleObject(fb->stream_thread,INFINITE);
  fb->asynchronous = async;
  CloseHandle(fb->file_mutex);
  CloseHandle(fb->data_mutex);
  CloseHandle(fb->data_event);
  CloseHandle(fb->stream_mutex);
}

#endif /* _MT */

#elif defined(__unix__) || defined(unix) || defined(__APPLE__)
#ifdef _POSIX_THREADS

static void *stream_thread(void *user_data)
{
  struct timeval curr_time;
  struct timeval old_time;
  int count,len,check_bird;
  int oldcount = 0;
  char buffer[256];
  fbird *fb;
  fb = (fbird *)user_data;
  
  old_time.tv_sec = 0;
  fb->async_bird = 0;

  /* the stream-recieve loop */
  for (count = 0;; count++) {
    /* the stream_mutex is used to wake up/put to sleep this thread */
    pthread_mutex_lock(&fb->stream_mutex);
    pthread_mutex_unlock(&fb->stream_mutex);

    if (fb->asynchronous == FB_NOTHREAD) {
      break; /* no longer in asynchronous mode: terminate thread */ 
    }

    fb->async_bird = (fb->async_bird % fb->num_birds) + 1;
    len = 2*data_len(fb->format[fb->async_bird]) \
      + fb->button_mode[fb->async_bird] + fb->group_mode;
    fbReceiveRaw(fb,buffer,len,1);
    gettimeofday(&curr_time,0);

    /* make sure we are receiving data from the bird we think we are */
    if (fb->group_mode) {
      check_bird = buffer[len-1];
      if (check_bird > 0 && check_bird <= fb->num_birds) {
        fb->async_bird = check_bird;
      }
      else if (check_bird == 0) {
        fb->async_bird = 1;
      }
    }

    /* lock the async data buffer and copy data to it */
    pthread_mutex_lock(&fb->data_mutex);    
    memcpy(fb->async_buffer[fb->async_bird],buffer,len);
    fb->async_timestamp_secs = curr_time.tv_sec;
    fb->async_timestamp_msecs = curr_time.tv_usec/1000;
    pthread_mutex_lock(&fb->fresh_data_mutex);
    fb->fresh_data = 1;
    pthread_mutex_unlock(&fb->data_mutex);
    pthread_cond_signal(&fb->fresh_data_cond);
    pthread_mutex_unlock(&fb->fresh_data_mutex);

    /* calculate the refresh rate every second */
    if (curr_time.tv_sec > old_time.tv_sec 
        && curr_time.tv_usec > old_time.tv_usec) { 
      if (old_time.tv_sec != 0) {  /* calc hertz */
        fb->async_data_rate = (count-oldcount);
        /* fprintf(stderr,"hertz %d\n",fb->async_data_rate); */
      }
      old_time.tv_sec = curr_time.tv_sec;
      old_time.tv_usec = curr_time.tv_usec;
      oldcount = count;
    }
  }

  return 0;
}

static int start_stream_thread(fbird *fb)
{
  pthread_mutex_init(&fb->file_mutex,0);   /* lock on serial port */
  pthread_mutex_init(&fb->data_mutex,0);   /* lock on fb->async_ elements */
  pthread_mutex_init(&fb->stream_mutex,0); /* only unlocked in stream mode */
  pthread_mutex_init(&fb->fresh_data_mutex,0); /* helper mutex for cond */
  pthread_cond_init(&fb->fresh_data_cond,0); /* signals arrival of new data */
  pthread_mutex_lock(&fb->stream_mutex);
  if (pthread_create(&fb->stream_thread,0,&stream_thread,fb)) {
    pthread_mutex_destroy(&fb->file_mutex);
    pthread_mutex_destroy(&fb->data_mutex);
    pthread_mutex_destroy(&fb->stream_mutex);
    pthread_mutex_destroy(&fb->fresh_data_mutex);
    pthread_cond_destroy(&fb->fresh_data_cond);
    return 0;
  }
  return 1;
} 

static void end_stream_thread(fbird *fb)
{
  int async = fb->asynchronous;
  fb->asynchronous = FB_NOTHREAD;  /* this signals thread to stop */
  if (!fb->stream) { 
    pthread_mutex_unlock(&fb->stream_mutex); /* this wakes the thread up */
  }
  pthread_join(fb->stream_thread,0);
  fb->asynchronous = async;
  pthread_mutex_destroy(&fb->file_mutex);
  pthread_mutex_destroy(&fb->data_mutex);
  pthread_mutex_destroy(&fb->stream_mutex);
  pthread_mutex_destroy(&fb->fresh_data_mutex);
  pthread_cond_destroy(&fb->fresh_data_cond);
}

#endif /* _POSIX_THREADS */
#endif /* unix */

#endif /* FLOCK_USE_THREADS */





