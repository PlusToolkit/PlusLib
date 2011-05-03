/*=======================================================================

  Program:   Flock of Birds C Interface Library
  Module:    $RCSfile: flock.h,v $
  Creator:   David Gobbi <dgobbi@atamai.com>
  Language:  C
  Author:    $Author: dgobbi $
  Date:      $Date: 2002/11/04 02:09:39 $
  Version:   $Revision: 1.1 $

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

/** \file flock.h */

#ifndef FLOCK_H
#define FLOCK_H 1

#ifdef __cplusplus
extern "C" {
#endif

/** \name Bird Commands

Every bird command consists of a single byte.  The commands can be
sent using either fbSendCommand(), fbSendCommandBytes(), or 
fbSendCommandWords() depending on whether there is any data
associated with the command and whether the data is formatted as
8-bit bytes or 16-bit words.
*/
/*\{*/
#define FB_POINT               'B'
#define FB_STREAM              '@'
#define FB_REPORT_RATE_FULL    'Q'
#define FB_REPORT_RATE_DIV2    'R'
#define FB_REPORT_RATE_DIV8    'S'
#define FB_REPORT_RATE_DIV32   'T'

#define FB_POSITION            'V'
#define FB_ANGLES              'W'
#define FB_MATRIX              'X'
#define FB_POSITION_ANGLES     'Y'
#define FB_POSITION_MATRIX     'Z'
#define FB_QUATERNION          '\\'
#define FB_POSITION_QUATERNION ']'

#define FB_BUTTON_MODE         'M'
#define FB_BUTTON_READ         'N'

#define FB_ANGLE_ALIGN1        'J'
#define FB_REFERENCE_FRAME1    'H'
#define FB_ANGLE_ALIGN2        'q'
#define FB_REFERENCE_FRAME2    'r'

#define FB_FBB_RESET           '/'
#define FB_HEMISPHERE          'L'
#define FB_NEXT_TRANSMITTER    '0'
#define FB_RUN                 'F'
#define FB_SLEEP               'G'
#define FB_SYNC                'A'
#define FB_XOFF                '\x13'
#define FB_XON                 '\x11'
 
#define FB_RS232_TO_FBB        '\xF0'
#define FB_RS232_TO_FBB_E      '\xE0'
#define FB_RS232_TO_FBB_SE     '\xA0'

#define FB_EXAMINE_VALUE       'O'
#define FB_CHANGE_VALUE        'P'
/*\}*/

/** \name Bird Status Bits

The bird status parameter FB_STATUS can be examined using the
fbExamineValue() function.  The status consists of 16 bits as
defined below:
*/
/*\{*/
#define FB_STATUS_STREAMING   0x0001
#define FB_STATUS_FORMAT      0x001E
#define FB_STATUS_SLEEPING    0x0020
#define FB_STATUS_XOFF        0x0040
#define FB_STATUS_FACTORY     0x0080
#define FB_STATUS_NO_SYNC     0x0100
#define FB_STATUS_CRT_SYNC    0x0200
#define FB_STATUS_EXPANDED    0x0400  /* expanded addressing mode */
#define FB_STATUS_HOST_SYNC   0x0800
#define FB_STATUS_RUNNING     0x1000
#define FB_STATUS_ERROR       0x2000
#define FB_STATUS_INITIALIZED 0x4000
#define FB_STATUS_MASTER      0x8000
/*\}*/

/** \name Flock Status Bits

The FB_FBB_STATUS parameter can be examined using fbExamineValueBytes(),
which will provide one status byte for each bird supported by
the current addressing mode: 14 in FB_NORMAL addressing mode, 
30 in FB_EXPANDED addressing mode, and 126 in FB_SUPER_EXPANDED
addressing mode.  The bits for each bird are defined below.
*/
/*\{*/
#define FB_FBB_STATUS_ERT0        0x01
#define FB_FBB_STATUS_ERT1        0x02
#define FB_FBB_STATUS_ERT2        0x04
#define FB_FBB_STATUS_ERT3        0x08
#define FB_FBB_STATUS_ERT         0x10
#define FB_FBB_STATUS_SENSOR      0x20
#define FB_FBB_STATUS_RUNNING     0x40
#define FB_FBB_STATUS_ACCESSIBLE  0x80
/*\}*/

/** \name Error Codes

The following error codes are returned by fbGetError():
*/
/*\{*/
#define FB_OPEN_ERROR     1     /* error opening serial port */
#define FB_COM_ERROR      2     /* error setting COM port parameters */
#define FB_IO_ERROR       3     /* some sort of I/O error */
#define FB_TIMEOUT_ERROR  4     /* communications timeout error */
#define FB_PARM_ERROR     5     /* bad examine/change parameter */
#define FB_COMMAND_ERROR  6     /* unrecognized bird command */
#define FB_ILLEGAL_ERROR  7     /* action is illegal in this state */ 
#define FB_PHASE_ERROR    8     /* phase error: comm port is dropping bytes */
#define FB_RESOURCE_ERROR 9     /* out of system resources */
#define FB_MODE_ERROR     10    /* specified bad mode when opening flock */
/*\}*/

/** \name Addressing Modes

The FB_FBB_ADDRESS_MODE parameter can be examined with fbExamineValue().
It is set via the dip switches on the bird units.
The FB_NORMAL mode supports 14 birds, FB_EXPANDED supports 30 birds, and
FB_SUPER_EXPANDED supports 126 birds.
*/
/*\{*/
#define FB_NORMAL           0
#define FB_EXPANDED         1
#define FB_SUPER_EXPANDED   3
/*\}*/

/** \name Hemispheres

The FB_P_HEMISPHERE parameter can be examined with fbExamineValue() and
changed with fbChangeValue().  Alternatively, it can be changed by
sending the FB_HEMISPHERE command to the flock.
*/
/*\{*/
#define FB_FORWARD 0x0000
#define FB_AFT     0x0100
#define FB_UPPER   0x010C
#define FB_LOWER   0x000C
#define FB_LEFT    0x0106
#define FB_RIGHT   0x0006
/*\}*/

/** \name Position Scaling

The FB_POSITION_SCALING parameter is specified here in millimeters
rather than inches.  The extended range transmitters are not yet
supported, so only FB_STANDARD and FB_EXTENDED modes are available.
The mode can be examined with fbExamineValue() or changed with
fbChangeValue().
*/
/*\{*/
#define FB_914MM   0
#define FB_1828MM  1
#define FB_STANDARD FB_914MM
#define FB_EXTENDED FB_1928MM
/*\}*/

/* miscellaneous */
#define PI             3.14159265359
#define TIMEOUT_PERIOD 5000     /* timeout period in milliseconds */

/** \name Change/Examine Parameters

The parameters can be examined using fbExamineValue(), 
fbExamineValueBytes(), or fbExamineValueWords().  Certain
parameters can be changed with fbChangeValue(),
fbChangeValueBytes(), or fbChangeValueWords().
*/
/*\{*/
#define FB_STATUS             0    /* Bird status */
#define FB_REVISION           1    /* Software revision number */
#define FB_SPEED              2    /* Bird computer crystal speed */
#define FB_POSITION_SCALING   3    /* Position scaling */

#define FB_FILTER             4    /* Filter on/off status */
#define FB_ALPHA_MIN          5    /* DC Filter constant table ALPHA_MIN */
#define FB_RATE_COUNT         6    /* Bird measurement rate count */
#define FB_RATE               7    /* Bird measurement rate */

#define FB_DATA_READY         8    /* Disable/Enable data ready output */
#define FB_DATA_READY_CHAR    9    /* Changes data ready character */
#define FB_ERROR_CODE        10    /* Bird outputs an error code */
#define FB_ERROR_BEHAVIOR    11    /* On error: stop or don't stop Bird  */

#define FB_VM                12    /* DC filter constant table Vm */
#define FB_ALPHA_MAX         13    /* DC filter constant table ALPHA_MAX */
#define FB_ELIMINATION       14    /* Sudden output change elimination */
#define FB_IDENTIFICATION    15    /* System Model Identification */

#define FB_ERROR_CODE_EXPANDED 16  /* Expanded Error Code */
#define FB_REFERENCE_FRAME   17    /* XYZ Reference Frame */
#define FB_TRANSMITTER_MODE  18    /* Transmitter Operation Mode */
#define FB_FBB_ADDRESS_MODE  19    /* FBB addressing mode */

#define FB_LINE_FILTER       20    /* Filter line frequency */
#define FB_FBB_ADDRESS       21    /* FBB address */
#define FB_P_HEMISPHERE      22    /* Change / Examine Hemisphere */
#define FB_P_ANGLE_ALIGN2    23    /* Change / Examine Angle Align2 */

#define FB_P_REFERENCE_FRAME2 24   /* Change / Examine Reference Frame2 */
#define FB_SERIAL_NUMBER     25    /* Bird Serial Number */
#define FB_SENSOR_SERIAL_NUMBER 26 /* Sensor Serial Number */
#define FB_XMTR_SERIAL_NUMBER 27   /* Xmtr Serial Number */

#define FB_FBB_DELAY    32         /* FBB Host Delay */
#define FB_GROUP_MODE   35         /* Group Mode */

#define FB_FBB_STATUS   36         /* Flock System Status */

#define FB_FBB_AUTOCONFIG    50    /* FBB Auto-Configuration, 1 xmtr/N snsrs */
/*\}*/

/** \name Baud Rates

The following baud rates are supported by fbOpen():
*/
/*\{*/
#define FB_2400   0          /* 2400 baud, etc, etc */
#define FB_4800   1
#define FB_9600   2
#define FB_19200  3
#define FB_38400  4
#define FB_57600  5
#define FB_115200 6
/*\}*/

/** \name Synchronization Modes

The following synchronization modes are supported by fbOpen().
*/
/*\{*/
#define FB_NOTHREAD 0  /* synchronous mode, no multithreading */ 
#define FB_THREAD   1  /* spawn a streaming thread, improves performance */
#define FB_NONBLOCK 2  /* never block, i.e. allow duplicate readings */
/*\}*/

/* the flock structure: NEVER modify any parameters directly */

struct fbird;
typedef struct fbird fbird;
                                                                      
/* get a serial port device name, given an integer starting at zero */

char *fbDeviceName(int i);

/* allocation/destruction of the flock structure */

fbird *fbNew();            /* create & initialize flock structure */
void fbDelete(fbird *fb);  /* delete flock */

/* open/close communication with the flock */

int fbOpen(fbird *fb, const char *device, int baud, int mode);
void fbClose(fbird *fb);

/* do a hardware reset of the flock */ 

void fbReset(fbird *fb);

/* hemisphere control -- hemisphere is FB_AFT etc. as defined above*/

void fbSetHemisphere(fbird *fb, int hemisphere);

/* data format control */

void fbSetFormat(fbird *fb, int format); /* format is FB_POSITION, etc */
void fbSetButtonMode(fbird *fb, int mode); /* set buttons on/off  */

/* standard data request methods */

void fbPoint(fbird *fb);              /* request only one data record */
void fbStream(fbird *fb);             /* start streaming acquisition */
void fbEndStream(fbird *fb);          /* end streaming acquisition */

/* standard data acquisition methods, use in and out of streaming mode */
/* fbGetTime() returns 'unix' time, i.e. seconds since new year's 1970 */ 

int fbUpdate(fbird *fb);             /* get next data record */
void fbGetPosition(fbird *fb, float xyz[3]);
void fbGetAngles(fbird *fb, float zyx[3]);
void fbGetMatrix(fbird *fb, float a[9]);
void fbGetQuaternion(fbird *fb, float q[4]);
int fbGetButton(fbird *fb);           /* true if button pressed */
int fbGetBird(fbird *fb);             /* which bird data is from */
double fbGetTime(fbird *fb);          /* when the data was acquired */

/* data morph methods: convert data from one form to another */

void fbMatrixFromAngles(float a[9], const float zyx[3]);
void fbAnglesFromMatrix(float zyx[3], const float a[9]);

/* commands for dealing with multiple birds */

void fbFBBReset(fbird *fb);
void fbFBBAutoConfig(fbird *fb, int num);
void fbRS232ToFBB(fbird *fb, int bird);

/* standard command interface */

void fbSendCommand(fbird *fb, int command);
void fbSendCommandWords(fbird *fb, int command, const short *data);
void fbSendCommandBytes(fbird *fb, int command, const char *data);

/* examine or change internal bird info, see bird manual for more info */

int fbExamineValue(fbird *fb, int parm);
int fbExamineValueWords(fbird *fb, int parm, short *data);
int fbExamineValueBytes(fbird *fb, int parm, char *data);
void fbChangeValue(fbird *fb, int parm, int data);
void fbChangeValueWords(fbird *fb, int parm, const short *data);
void fbChangeValueBytes(fbird *fb, int parm, const char *data);

/* error reporting */
/* if the error is FB_TIMEOUT_ERROR, the bird itself probably has an error
   which you can retrieve with fbExamineValue(fb,FB_ERROR_CODE) */

int fbGetError(fbird *fb);          /* get error code (zero if no error) */
char *fbGetErrorMessage(fbird *fb); /* get text error message */
void fbSetErrorCallback(fbird *fb, void (*callback)(void *data), 
                        void *data);

/* one-shot data acquisition -- use fbSetButtonMode instead */

void fbButtonRead(fbird *fb, int *val);  /* return button state */ 

/* utility functions */

int fbUnpack(char **cpp); /* unpack two phase-packed chars into a short */
int fbGetShort(const char *cp); /* unpack two chars into a short */
void fbPutShort(char *cp, int val); /* convert a short into two chars */

/* ultra low-level interface, for use only for diagnostics  */

void fbReceiveRaw(fbird *fb, char *reply, int len, int thread);
void fbSendRaw(fbird *fb, const char *text, int len);


#ifdef __cplusplus
}
#endif

#endif /* FLOCK_H */
