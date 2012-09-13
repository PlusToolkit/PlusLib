/************************ Functions/Constants Declarations ***********************/


/*********** Version ***********/
const char* versiontxt = "SEIDrv32.DLL version 5.22\n";

#ifdef __cplusplus 
extern "C"{ 
#endif 


/********** General functions ***********/
long __stdcall GetDLLVersionText(char* pstr);
	// function: gets the DLL version text.
	// pstr: pointer to the version text.
	// return: length of dll version text.

long __stdcall GetLastErrorText(char* pstr);
	// function: gets the error messages from the last error that occured
	// pstr: pointer to the messages
	// return: 0

/*********** SEI functions ***********/
extern long __stdcall InitializeSEI(long comm, long mode);
	// function: Initializes the SEI bus
	// comm: CommPort of SEI devices
	//  -If comm is zero, INITIALIZESEI will search all available comm ports
	// mode: 
	//  -Modes may be combined together
	//  -If SKIPDEVICEINFO is set, AUTOASSIGN is ignored
	//  -If SKIPDEVICESEARCH is set, AUTOASSIGN is ignored
	const long AUTOASSIGN =			0x0001;	// autoassign addresses if they conflict
	const long SKIPDEVICESEARCH =	0x0002;	// don't check if devices exist on chosen comm port
	const long SKIPDEVICEINFO =		0x0004;	// don't retrieve devices info
	const long NORESET =			0x0008;	// don't generate reset to SEI devices
	const long NOCHECKSUM =			0x0010;	// don't check checksum of commands
	const long REINITIALIZE =		0x0020;	// reinitialize if already initialized
	const long NOA2ERRORCLEAR =		0x0040;	// don't clear A2 errors on powerup
	// return: 0 if no error
extern long __stdcall IsInitialized();
	// function: returns status of initialization
	// return: 0 if not initialized, 1 if initialized
extern long __stdcall GetCommPort();
	// function: returns the initialized CommPort number
	// return: the CommPort number 1 to 8, -1 if CommPort is not initialized
extern long __stdcall ReopenCommPort();
	// function: re-opens the last opened CommPortreturns the initialized CommPort number
	// return: the CommPort number 1 to 8, -1 if CommPort is not initialized
extern long __stdcall IsAnalog();
	// function: returns true if the device is analog
	// return: 0 if not an analog device, 1 if analog device
extern long __stdcall GetNumberOfDevices();
	// function: returns number of devices found on the SEI bus
	// return: number of devices 0-15, any number other than 0-15 is an error code
extern long __stdcall GetDeviceInfo(long devnum, long *model, long *serialnum, long *version, long *addr);
	// function: Gets some information about a device
	// devnum: 0 To GetNumberOfDevices - 1
	// model: devnum's model#, M1=1, A2=2, ED2=3, AD5=5
	// serialnum: devnum's serial#
	// version: devnum's firmware version#
	// addr: devnum's address
	// return: 0 if no error
extern long __stdcall GetAllDeviceInfo(long devnum, long *model, long *serialnum, long *version, long *addr, long *month, long *day, long *year, long *configuration);
	// function: Gets all information about a device
	// devnum: 0 To GetNumberOfDevices - 1
	// model: devnum's model#, M1=1, A2=2, ED2=3, AD5=5
	// serialnum: devnum's serial#
	// version: devnum's firmware version#
	// addr: devnum's address
	// month: devnum's manufacturing month
	// day: devnum's manufacturing day
	// year: devnum's manufacturing year
	// configuration: devnum's configuration#
	// return: 0 if no error
extern long __stdcall SetDeviceAddress(long SN, long address);
	// function: changes the address of a device on the SEI bus
	// SN: Serial number of the device you want to change the address of
	// address: New address for the device
	// return: 0 if no error
extern long __stdcall SetBaudRate(long baudrate);
	// function: changes the baudrate of the SEI bus
	// baudrate: new baudrate, valid baudrates are: 1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200
	// return: 0 if no error
extern long __stdcall ResetSEI();
	// function: resets the SEI bus by sending a break condition
	// return: 0 if no error
extern long __stdcall GetBusyStatus();
	// function: returns the status of the busy line
	// return: 1 if busy is true, 0 if busy is false
extern long __stdcall CloseSEI();
	// function: closes the SEI bus
	// return: 0
extern long __stdcall Mcommand(char* bufout, long sizeout, char* bufin, long sizein);			// generic command
	// function: generic read/write command for the SEI bus
	// bufout: the array to be written on the SEI bus
	// sizeout: number of bytes to write
	// bufin: the array of bytes read back
	// sizein: number of bytes to read back
	// return: 0 if no error
extern long __stdcall McommandLong(long* bufout, long sizeout, long* bufin, long sizein);	// generic command w/long array
	// function: generic read/write command for the SEI bus using longs
	// bufout: the array to be written on the SEI bus
	// sizeout: number of longs to write
	// bufin: the array to be read back
	// sizein: number of longs to read back
	// return: 0 if no error


/*********** A2 functions ***********/
extern long __stdcall A2SetStrobe();
	// function: if the A2 is in strobe mode, it will take a position reading after receiving this command.
	// return: 0 if no error
extern long __stdcall A2SetSleep();
	// function: makes all A2's on the SEI bus go to sleep, the current consumption then drops below 0.6 mA / device
	// return: 0 if no error
extern long __stdcall A2SetWakeup();
	// function: wakes up all A2's on the SEI bus, wait at least 5mSec before sending the next command
	// return: 0 if no error
extern long __stdcall A2SetOrigin(long address);
	// function: Sets the absolute zero to the current position, in single-turn mode the new position is stored in EEPROM
	// address: SEI address 0-15
	// return: 0 if no error
extern long __stdcall A2GetMode(long address, long *mode);
	// function: Gets the mode of an A2
	// address: SEI address 0-14
	// mode: A2's mode, see A2 Communications Protocol Datasheet
	// return: 0 if no error
extern long __stdcall A2SetMode(long address, long mode);
	// function: Sets the mode of an A2
	// address: SEI address 0-15
	// mode: A2's mode, see A2 Communications Protocol Datasheet
	// return: 0 if no error
extern long __stdcall A2GetResolution(long address, long *res);
	// function: Gets the resolution of an A2
	// address: SEI address 0-14
	// res: A2's resolution
	// return: 0 if no error
extern long __stdcall A2SetResolution(long address, long res);
	// function: Sets the resolution of an A2
	// address: SEI address 0-15
	// res: A2's resolution
	// return: 0 if no error
extern long __stdcall A2GetPosition(long address, long *pos);
	// function: Gets the position of an A2
	// address: SEI address 0-14
	// pos: A2's position
	// return: 0 if no error
extern long __stdcall A2SetPosition(long address, long pos);
	// function: Sets the position of an A2
	// address: SEI address 0-15
	// pos: A2's position
	// return: 0 if no error
extern long __stdcall A2StopClocks(long lngMilliseconds);
	// function: stops the clocks of all A2's on bus
	// lngMilliseconds: the number of milliseconds to look for a
	//	confirmation wakeup character before going back to the
	//	the stopped clock state
	// return: 0 if no error (will return error if no A2's on bus)
extern long __stdcall A2RestartClocks(long lngTries);
	// function: restarts the clocks of all A2's after A2StopClocks()
	// lngTries: the number of wakeup characters to send before giving up
	// returns 0 if successful
extern long __stdcall A2ComputeCRC(long lngSEIAddress, long lngCRCStart, long lngCRCLength, long *plResult);
	// function: computes 16 bit CRC over the specified block of A2 memory
	// lngSEIAddress: SEI address, 0-14
	// lngCRCStart: starting point for CRC (0xE080 is code image start)
	// lngCRCLength: length of block over which to computer (0x1F80 is normal for code image)
	// lngplResult: pointer to long result of CRC computation
extern long __stdcall A2LockParameters(long lngSEIAddress);
	// function: disables ability to write to A2 parameter space;
	//			 (code space writes are always disabled)
	// lngSEIAddress: SEI address, 0-14
extern long __stdcall A2GetPositionTimeStatus(long address, long *ptr_pos, long *ptr_time, long *ptr_status);
	// function: returns position, time, and status of device to caller
	// lngSEIAddress: SEI address, 0-14
	// ptr_pos: pointer to long that will be updated with 12-bit position value
	// ptr_time: pointer to long that will be updated with 16-bit timer value from microprocessor timer
	// ptr_status: pointer to long that will be updated with 8 bit error status/checksum information
extern long __stdcall A2GetPositionLongTimeStatus(long address, long *ptr_pos, unsigned long *ptr_time, long *ptr_status);
	// function: returns position, time, and status of device to caller
	// lngSEIAddress: SEI address, 0-14
	// ptr_pos: pointer to long that will be updated with 12-bit position value
	// ptr_time: pointer to long that will be updated with 32-bit timer value from microprocessor timer
	// ptr_status: pointer to long that will be updated with 8 bit error status/checksum information

/*********** M1 functions ***********/
extern long __stdcall M1GetInput(long address, long *input);
	// function: Gets the input status of an M1
	// address: SEI address 0-14
	// input: input value of M1
	// return: 0 if no error
extern long __stdcall M1SetOutput(long address, long output);
	// function: Sets the output of an M1
	// address: SEI address 0-15
	// output: output value of an M1
	// return: 0 if no error
extern long __stdcall M1SetMotors(long address, long motors);
	// function: Sets the motors of an M1
	// address: SEI address 0-15
	// motors: control byte
	// -bit0: A1, 1 = Enable Driver Output; 0 = Disable Driver Output
	// -bit1: B1, 1 = Enable Driver Output; 0 = Disable Driver Output
	// -bit2: A2, 1 = Enable Driver Output; 0 = Disable Driver Output
	// -bit3: B2, 1 = Enable Driver Output; 0 = Disable Driver Output
	// -bit4: A1, 1 = Drive output High; 0 = Drive output Low
	// -bit5: B1, 1 = Drive output High; 0 = Drive output Low
	// -bit6: A2, 1 = Drive output High; 0 = Drive output Low
	// -bit7: B2, 1 = Drive output High; 0 = Drive output Low
	// return: 0 if no error
extern long __stdcall M1SetInitialOutput(long address, long output);
	// function: Same as M1SetOutput but the M1 stores this value in its EEPROM
	// address: SEI address 0-15
	// output: output value of an M1
	// return: 0 if no error
extern long __stdcall M1SetInitialMotors(long address, long motors);
	// function: Same as M1SetMotors but the M1 stores this value in its EEPROM
	// address: SEI address 0-15
	// motors: control byte
	// return: 0 if no error


/*********** ED2 functions ***********/
extern long __stdcall ED2SetOrigin(long address);
	// function: Resets both counters
	// address: SEI address 0-15
	// return: 0 if no error
extern long __stdcall ED2GetMode(long address, long *mode);
	// function: Gets the mode of an ED2
	// address: SEI address 0-14
	// mode: ED2's mode, see ED2 Communications Protocol Datasheet
	// return: 0 if no error
extern long __stdcall ED2SetMode(long address, long mode);
	// function: Sets the mode of an ED2
	// address: SEI address 0-15
	// mode: ED2's mode, see ED2 Communications Protocol Datasheet
	// return: 0 if no error
extern long __stdcall ED2GetResolution(long address, long *res);
	// function: Gets the resolution of an ED2
	// address: SEI address 0-14
	// res: ED2's resolution
	// return: 0 if no error
extern long __stdcall ED2SetResolution(long address, long res);
	// function: Sets the resolution of an ED2
	// address: SEI address 0-15
	// res: ED2's resolution
	// return: 0 if no error
extern long __stdcall ED2GetPosition1(long address, long *pos);
	// function: Gets the position of an ED2, counter 1
	// address: SEI address 0-14
	// pos: ED2's position
	// return: 0 if no error
extern long __stdcall ED2GetPosition2(long address, long *pos);
	// function: Gets the position of an ED2, counter 2
	// address: SEI address 0-14
	// pos: ED2's position
	// return: 0 if no error
extern long __stdcall ED2SetPosition1(long address, long pos);
	// function: Sets the position of an ED2, counter 1
	// address: SEI address 0-15
	// pos: ED2's position
	// return: 0 if no error
extern long __stdcall ED2SetPosition2(long address, long pos);
	// function: Sets the position of an ED2, counter 2
	// address: SEI address 0-15
	// pos: ED2's position
	// return: 0 if no error
extern long __stdcall ED2GetInput(long address, long *input);
	// function: Gets the status of an ED2's digital inputs
	// address: SEI address 0-14
	// input: input's status
	// -bit0: Push Button #1 State
	// -bit1: Push Button #2 State
	// -bit2: Push Button #3 State
	// -bit3: TTL Input #1 State
	// -bit4: TTL Input #2 State
	// -bit5: TTL Input #3 State
	// -bit6: TTL Input #4 State
	// -bit7: Back Switch State
	// return: 0 if no error
extern long __stdcall ED2SetOutput(long address, long output);
	// function: Sets the status of an ED2's digital outputs
	// address: SEI address 0-15
	// output: output's status
	// -bit0: Push Button #1 State
	// -bit1: Push Button #2 State
	// -bit2: Push Button #3 State
	// -bit3: TTL Input #1 State
	// -bit4: TTL Input #2 State
	// -bit5: TTL Input #3 State
	// -bit6: TTL Input #4 Enable, OR this value with TTL Input #4
	// -bit7: Or this value with the back switch's state
	// return: 0 if no error
extern long __stdcall ED2ComputeCRC(long lngSEIAddress, long lngCRCStart, long lngCRCLength, long *plResult);
	// function: computes 16 bit CRC over the specified block of A2 memory
	// lngSEIAddress: SEI address, 0-14
	// lngCRCStart: starting point for CRC (0xE080 is code image start)
	// lngCRCLength: length of block over which to computer (0x1F80 is normal for code image)
	// lngplResult: pointer to long result of CRC computation


/*********** AD5 functions ***********/
extern long __stdcall AD5Reset(long address, long port);
	// function: Resets a counter of an AD5
	// address: SEI address 0-15
	// port: 0-3
	// return: 0 if no error
extern long __stdcall AD5GetMode(long address, long *mode);
	// function: Gets the mode of an AD5
	// address: SEI address 0-14
	// mode: AD5's mode
	// -bit0: 1 = Port#1 is active; 0 = Port#1 is disabled
	// -bit1: 1 = Port#1's index is active; 0 = Port#1's index is disabled
	// -bit2: 1 = Port#2 is active; 0 = Port#2 is disabled
	// -bit3: 1 = Port#2's index is active; 0 = Port#2's index is disabled
	// -bit4: 1 = Port#3 is active; 0 = Port#3 is disabled
	// -bit5: 1 = Port#3's index is active; 0 = Port#3's index is disabled
	// -bit6: 1 = Port#4 is active; 0 = Port#4 is disabled
	// -bit7: 1 = Port#4's index is active; 0 = Port#4's index is disabled
	// return: 0 if no error
extern long __stdcall AD5SetMode(long address, long mode);
	// function: Sets the mode of an AD5, writes it to EEPROM
	// address: SEI address 0-15
	// mode: AD5's mode
	// return: 0 if no error
extern long __stdcall AD5GetResolution(long address, long port, long *res);
	// function: Gets the resolution of an AD5
	// address: SEI address 0-14
	// port: 0-3
	// res: resolution of an AD5
	// return: 0 if no error
extern long __stdcall AD5SetResolution(long address, long port, long res);
	// function: Sets the resolution of an AD5
	// address: SEI address 0-15
	// port: 0-3
	// res: resolution of an AD5
	// return: 0 if no error
extern long __stdcall AD5GetPosition(long address, long port, long *pos);
	// function: Gets the position of an AD5
	// address: SEI address 0-14
	// port: 0-3
	// pos: position of an AD5
	// return: 0 if no error
extern long __stdcall AD5SetPosition(long address, long port, long pos);
	// function: Sets the position of an AD5
	// address: SEI address 0-15
	// port: 0-3
	// pos: position of an AD5
	// return: 0 if no error
extern long __stdcall AD5GetAllPositions(long address, long *pos1, long *pos2, long *pos3, long *pos4);
	// function: Gets all the position of an AD5 at once
	// address: SEI address 0-14
	// pos1: position of an AD5 port 4
	// pos2: position of an AD5 port 3
	// pos3: position of an AD5 port 2
	// pos4: position of an AD5 port 1
	// return: 0 if no error
extern long __stdcall AD5SetCMR(long address, long port, long cmr);
	// function: Sets the CMR registry of the counter, see the LS7266R1 datasheet for more information on the counter
	// address: SEI address 0-15
	// port: 0-3
	// cmr: CMR registry
	// return: 0 if no error
extern long __stdcall AD5GetCMR(long address, long port, long *cmr);
	// function: Gets the CMR registry of the counter, see the LS7266R1 datasheet for more information on the counter
	// address: SEI address 0-15
	// port: 0-3
	// cmr: CMR registry
	// return: 0 if no error
extern long __stdcall AD5SetModeVolatile(long address, long mode);
	// function: Sets the mode of an AD5, not written to EEPROM
	// address: SEI address 0-15
	// mode: AD5's mode
	// return: 0 if no error
extern long __stdcall AD5GetAllPositionsOrdered(long address, long *pos1, long *pos2, long *pos3, long *pos4);
	// function: Gets all the position of an AD5 at once
	// address: SEI address 0-14
	// pos1: position of an AD5 port 1
	// pos2: position of an AD5 port 2
	// pos3: position of an AD5 port 3
	// pos4: position of an AD5 port 4
	// return: 0 if no error
extern long __stdcall AD5ComputeCRC(long lngSEIAddress, long lngCRCStart, long lngCRCLength, long *plResult);
	// function: computes 16 bit CRC over the specified block of A2 memory
	// lngSEIAddress: SEI address, 0-14
	// lngCRCStart: starting point for CRC (0xE080 is code image start)
	// lngCRCLength: length of block over which to computer (0x1F80 is normal for code image)
	// lngplResult: pointer to long result of CRC computation

#ifdef __cplusplus 
} 
#endif