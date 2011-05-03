/***************************************************
 * - - - - - - -   IMMERSION CORP.   - - - - - - - *
 *                                                 *
 *        IBM PC/Compatibles software series       *
 *                Copyright (c) 1993               *
 ***************************************************
 * DRIVE.H   |   SDK1-2   |   November 1995
 *
 * Immersion Corp. Software Developer's Kit
 *      Definitions and prototypes for serial communications functions
 *      for the Immersion Corp. MicroScribe-3D
 *      Not for use with the Probe or Personal Digitizer
 *      Requires HCI firmware version MSCR1-1C or later
 */

#ifndef drive_h
#define drive_h 1


/* Public constants */
#define MIN_TIMEOUT     0.1

/* this was put in for windows only */
int host_get_id(int port);


/* Timing functions */
void    host_pause(float delay_sec);
float   host_get_timeout(int port);
void    host_set_timeout(int port, float timeout_sec);
void    host_start_timeout(int port);
int     host_timed_out(int port);


/* Configuring serial ports */
void    host_fix_baud(long int *baud);
int     host_open_serial(int port, long int baud);
void    host_close_serial(int port);
void    host_flush_serial(int port);


/* Reading/writing serial data */
int     host_read_char(int port);
int     host_read_bytes(int port, char *buf, int count, float timeout);
int     host_write_char(int port, int ch);
int     host_write_string(int port, char *str);
int     host_port_valid(int port);
int     host_input_count(int port);
int     host_input_full(int port);


#endif /* drive_h */

