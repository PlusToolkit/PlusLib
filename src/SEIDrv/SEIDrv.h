#pragma once

/*********** SEI functions ***********/

// function: Initializes the SEI bus
// comm: CommPort of SEI devices
//  -If comm is zero, INITIALIZESEI will search all available comm ports
// mode: 
//  -Modes may be combined together
//  -If SKIPDEVICEINFO is set, AUTOASSIGN is ignored
//  -If SKIPDEVICESEARCH is set, AUTOASSIGN is ignored
// return: 0 if no error
long InitializeSEI(long comm, long mode);

const long AUTOASSIGN = 0x0001;	// autoassign addresses if they conflict
const long SKIPDEVICESEARCH = 0x0002;	// don't check if devices exist on chosen comm port
const long SKIPDEVICEINFO = 0x0004;	// don't retrieve devices info
const long NORESET = 0x0008;	// don't generate reset to SEI devices
const long NOCHECKSUM = 0x0010;	// don't check checksum of commands
const long REINITIALIZE = 0x0020;	// reinitialize if already initialized
const long NOA2ERRORCLEAR = 0x0040;	// don't clear A2 errors on powerup

// function: returns status of initialization
bool IsInitialized();

// function: returns number of devices found on the SEI bus
// return: number of devices 0-15, any number other than 0-15 is an error code
long GetNumberOfDevices();

// function: Gets some information about a device
// devnum: 0 To GetNumberOfDevices - 1
// serialnum: devnum's serial#
// addr: devnum's address
// model: a pointer to allocated char* buffer, not filled with data if nullptr
// firmwareVersion: a pointer to allocated char* buffer, not filled with data if nullptr
// return: 0 if no error
long GetDeviceInfo(long devnum, long& serialnum, long& addr, char* model, char* firmwareVersion);

// function: closes the SEI bus
void CloseSEI();


/*********** A2 functions ***********/

// function: if the A2 is in strobe mode, it will take a position reading after receiving this command.
// return: 0 if no error
long A2SetStrobe();

// function: makes all A2's on the SEI bus go to sleep, the current consumption then drops below 0.6 mA / device
// return: 0 if no error
long A2SetSleep();

// function: wakes up all A2's on the SEI bus, wait at least 5mSec before sending the next command
// return: 0 if no error
long A2SetWakeup();

// function: Sets the absolute zero to the current position, in single-turn mode the new position is stored in EEPROM
// address: SEI address 0-15
// return: 0 if no error
long A2SetOrigin(long address);

// function: Gets the mode of an A2
// address: SEI address 0-14
// mode: A2's mode, see A2 Communications Protocol Datasheet
// return: 0 if no error
long A2GetMode(long address, long *mode);

// function: Sets the mode of an A2
// address: SEI address 0-15
// mode: A2's mode, see A2 Communications Protocol Datasheet
// return: 0 if no error
long A2SetMode(long address, long mode);

// function: Gets the resolution of an A2
// address: SEI address 0-14
// res: A2's resolution
// return: 0 if no error
long A2GetResolution(long address, long *res);

// function: Sets the resolution of an A2
// address: SEI address 0-15
// res: A2's resolution
// return: 0 if no error
long A2SetResolution(long address, long res);

// function: Gets the position of an A2
// address: SEI address 0-14
// pos: A2's position
// return: 0 if no error
long A2GetPosition(long address, long *pos);

// function: Sets the position of an A2
// address: SEI address 0-15
// pos: A2's position
// return: 0 if no error
long A2SetPosition(long address, long pos);
