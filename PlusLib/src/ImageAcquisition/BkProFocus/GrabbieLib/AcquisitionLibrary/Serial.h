#pragma once
#include <windows.h>


class Serial {
private:
	// Common variables for the com port
	HANDLE			hComm;
	BOOL			bOpened;

public:

	BOOL OpenSerial(int nPort, int nBaud); 
/*	Input ......: nPort - Comm. port number, 1,2, 3 or 4.
				  nBaud - Comm. port Baud rate, 110, 300, 1200, 2400, 4800, 9600,
												19200, 38400, 57600 or 115200.
	Process ....: Opens the serial port as a stream connected to a comm. port Handle.
	Output .....: Returns a Boolean TRUE if the port is successfully opened, otherwise
				  returns FALSE.
*/
	BOOL OpenSerial(const char* pszPortName, int nBaud);

	BOOL CloseSerial(void);
/*	Input ......: none
	Process ....: Closing the stream and releasing the serial port.
	Output .....: Returns a Boolean TRUE if the port is successfully closed, otherwise
				  returns FALSE.
*/

	BOOL PutSerial(unsigned char ucByte);
/*	Input ......: ucByte - Byte to be transmitted by the serial port.
	Process ....: Transmits a byte on the serial line.
	Output .....: Returns a Boolean TRUE if the byte is successfully transmitted,
				  otherwise returns FALSE.
*/

	BOOL GetSerial(unsigned char* ucByte);
/*	Input ......: Pointer(address) to recieved byte.
	Process ....: Recieves a byte from the serial line.
	Output .....: (ucByte) - the recieved byte is stored in address ucByte.
				  Returns a Boolean TRUE if a byte is successfully recieved(no IO fault),
				  otherwise returns FALSE.
*/
};