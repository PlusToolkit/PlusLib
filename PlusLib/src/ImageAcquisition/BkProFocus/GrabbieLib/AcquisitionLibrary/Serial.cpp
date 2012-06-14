
#include  "Serial.h"
#include <stdio.h>


BOOL Serial::OpenSerial(int nPort, int nBaud) {
	BOOL			bPortReady;
	char			sComPort[256];
	DCB				dcb;
	COMMTIMEOUTS	CommTimeouts;

   
	//sprintf(sComPort,"\\\\.\\X64-CL_Express_1_Serial_0");
	sprintf_s(sComPort,"\\\\.\\COM%d",nPort);
	
	hComm = CreateFile(sComPort,
					  GENERIC_READ | GENERIC_WRITE,
					  0, //exclusive access
					  NULL, // no security
					  OPEN_EXISTING,
					  0, // no overlapped I/O
					  NULL); // null template

   // check for invalid handle value
	if (hComm == NULL) return (FALSE);

	// set buffer sizes
	bPortReady = SetupComm(hComm, 32768, 32768);

	// initialize port settings
	// get default values
	bPortReady = GetCommState(hComm, &dcb);
	// set new values
	dcb.BaudRate = nBaud;
	dcb.ByteSize = 8;
	dcb.Parity = NOPARITY; 
	dcb.StopBits = ONESTOPBIT;
	dcb.fAbortOnError = TRUE;
	bPortReady = SetCommState(hComm, &dcb);
	// check for invalid port setting
	if (!bPortReady) return (FALSE);

	// set communication timeouts
	// get default values
	bPortReady = GetCommTimeouts(hComm, &CommTimeouts);
	// set new values
	CommTimeouts.ReadIntervalTimeout = 0xFFFFFFFF;
	CommTimeouts.ReadTotalTimeoutConstant = 0;
	CommTimeouts.ReadTotalTimeoutMultiplier = 0;
	CommTimeouts.WriteTotalTimeoutConstant = 5000;
	CommTimeouts.WriteTotalTimeoutMultiplier = 0;
	bPortReady = SetCommTimeouts(hComm, &CommTimeouts);


	bOpened = TRUE;

	return bOpened;

}


BOOL Serial::OpenSerial(const char* pszPortName, int nBaud) {
	BOOL			bPortReady;
	DCB				dcb;
	COMMTIMEOUTS	CommTimeouts;

   
	hComm = CreateFile(pszPortName,
					  GENERIC_READ | GENERIC_WRITE,
					  0, //exclusive access
					  NULL, // no security
					  OPEN_EXISTING,
					  0, // no overlapped I/O
					  NULL); // null template

   // check for invalid handle value
	if (hComm == NULL) return (FALSE);

	// set buffer sizes
	bPortReady = SetupComm(hComm, 32768, 32768);

	// initialize port settings
	// get default values
	bPortReady = GetCommState(hComm, &dcb);
	// set new values
	dcb.BaudRate = nBaud;
	dcb.ByteSize = 8;
	dcb.Parity = NOPARITY; 
	dcb.StopBits = ONESTOPBIT;
	dcb.fAbortOnError = TRUE;
	bPortReady = SetCommState(hComm, &dcb);
	// check for invalid port setting
	if (!bPortReady) return (FALSE);

	// set communication timeouts
	// get default values
	bPortReady = GetCommTimeouts(hComm, &CommTimeouts);
	// set new values
	CommTimeouts.ReadIntervalTimeout = 0xFFFFFFFF;
	CommTimeouts.ReadTotalTimeoutConstant = 0;
	CommTimeouts.ReadTotalTimeoutMultiplier = 0;
	CommTimeouts.WriteTotalTimeoutConstant = 5000;
	CommTimeouts.WriteTotalTimeoutMultiplier = 0;
	bPortReady = SetCommTimeouts(hComm, &CommTimeouts);


	bOpened = TRUE;

	return bOpened;

}




BOOL Serial::CloseSerial(void) {
	if ( !bOpened || hComm==NULL ) return (TRUE);
	CloseHandle(hComm);
	bOpened = FALSE;
	hComm = NULL;

	return (TRUE);
}

BOOL Serial::PutSerial(unsigned char ucByte) {
	BOOL			bWriteStat;
	DWORD			iBytesWritten;
	
	if ( !bOpened || hComm==NULL ) return (FALSE);
	bWriteStat = WriteFile(hComm, (LPSTR) &ucByte, 1, &iBytesWritten, NULL);
	// check for no bytes writen or IO error
	if ( !bWriteStat || GetLastError()== ERROR_IO_PENDING ) return (FALSE);

	return (TRUE);
}

BOOL Serial::GetSerial(unsigned char *ucByte) {
	BOOL			bReadStat;
	DWORD			iBytesRead;

	*ucByte = 0;
	
	if ( !bOpened || hComm==NULL ) return (FALSE);
	bReadStat = ReadFile(hComm, ucByte, 1, &iBytesRead, NULL);
	// check for no bytes read or IO error
	if ( !bReadStat || GetLastError()== ERROR_IO_PENDING ){
		*ucByte = 0;
		return (FALSE);
	}

	return (iBytesRead);
}



