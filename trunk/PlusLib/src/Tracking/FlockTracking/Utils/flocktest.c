#include "flock.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define RAD_TO_DEG 57.2957795132

/* this is called when an error occurs */
void callback(void *fb)
{
  struct fbird *bird = (struct fbird *)fb;
  fprintf(stderr,"error %d: %s\n",fbGetError(bird),fbGetErrorMessage(bird));
  abort();
}

/* this probes for the device on all serial ports and at all baud rates */
int probe(struct fbird *bird, int *devicenum_p, int *baud_p, int *mode_p)
{
  static int baudrates[7] = { 115200,
                              57600,
                              19200,
            9600,
                              4800,
                              2400,
                              38400 };
  int mode = FB_THREAD;
  int nbaud, baud, devicenum;
  int errnum;
  char *devicename;

  for (devicenum = 0; devicenum < 2; devicenum++) {
    devicename = fbDeviceName(devicenum);
    for (nbaud = 0; nbaud < 7; nbaud++) {
      baud = baudrates[nbaud];
      fbOpen(bird,devicename,baud,mode);
      errnum = fbGetError(bird);
      /* if FB_THREAD is not supported, switch mode to FB_NOTHREAD */
      if (errnum == FB_MODE_ERROR && mode != FB_NOTHREAD) {
        mode = FB_NOTHREAD;
        fbOpen(bird,devicename,baud,mode);
        errnum = fbGetError(bird);
      }
      /* if we can't open the serial port, go on to next serial port */
      if (errnum == FB_OPEN_ERROR) {
        break;
      }
      /* if no error, then we're done switching baud rates */
      if (!errnum) {
        break;
      }
    }
    /* if no error, then we're done switching ports */
    if (!errnum) {
      break;
    }
  }

  if (!errnum) {
    fbClose(bird);
    *devicenum_p = devicenum;
    *baud_p = baud;
    *mode_p = mode;
  }
 
  return !errnum;
}

int main(int argc, char *argv[])
{
  float pos[3],ang[3];
  struct fbird *bird;
  int i;
  char status[128];
  int devicenum = 0;
  int baud = 115200;
  int mode = FB_THREAD;
  char *devicename;
  double starttime;
  int count = 1000;

  if (argc > 1) {
    count = atoi(argv[1]);
  }

  /* return a bird object */
  bird = fbNew();

  /* probe for port/baud rate */
  if (!probe(bird,&devicenum,&baud,&mode)) {
    fprintf(stderr,"error %d: %s\n",fbGetError(bird),fbGetErrorMessage(bird));
    exit(0);
  }

  mode = 1;
  /* print out probed information */
  fprintf(stderr,"found bird on port %s at baud rate %d in mode %d\n",
          fbDeviceName(devicenum),baud,mode);

  /* you can set an error callback if you want */
  fbSetErrorCallback(bird,callback,bird); 

  /* get the serial port name, e.g. "COM1:" or "COM2:" */
  devicename = fbDeviceName(devicenum);

  /* args are bird, serial port, baud, flags */
  fbOpen(bird,devicename,baud,mode);

  fprintf(stderr,"autoconfig\n");
  /* configure for one bird -- you might want to change this*/
  fbFBBAutoConfig(bird,1);
  fbChangeValue(bird,FB_GROUP_MODE,1);

  fprintf(stderr,"system status\n");
  fbExamineValueBytes(bird,FB_FBB_STATUS,status);
  fprintf(stderr,"1: %2.2x 2: %2.2x",status[0],status[1]);
  fprintf(stderr,"\n");

/* this is not necessary, because it is the default
  fbRS232ToFBB(bird,1);
  fbSendCommand(bird,FB_POSITION_ANGLES);
  fbRS232ToFBB(bird,2);
  fbSendCommand(bird,FB_POSITION_ANGLES);
*/

  fprintf(stderr,"streaming\n");
  fbStream(bird);

  /* grab a single record */
  
  fbUpdate(bird);
  starttime = fbGetTime(bird);

  for (i = 0;i<count;i++) {
    fbUpdate(bird);
    fbGetPosition(bird,pos);
    fbGetAngles(bird,ang);
    fprintf(stderr,"bird %d time %10.3f pos %+6.1f %+6.1f %+6.1f ang %+6.1f %+6.1f %+6.1f\n",
      fbGetBird(bird),fbGetTime(bird)-starttime,
      pos[0],pos[1],pos[2],
      ang[0]*RAD_TO_DEG,ang[1]*RAD_TO_DEG,ang[2]*RAD_TO_DEG);
  }
  
  fbClose(bird);

  return 0;
}
