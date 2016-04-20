#ifndef _INTUITIVE_DAVINCI_H_
#define _INTUITIVE_DAVINCI_H_

#ifdef WIN32
#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#endif

#include <stdio.h>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>

#include <vector>

#include <isi_api_types.h>
#include <isi_api.h>

#define ISI_API_RATE        60 // Hz

class IntuitiveDaVinci
{

public: 
	// Constructor
	IntuitiveDaVinci();

	// Destructor
	~IntuitiveDaVinci();

	// Disconnect from the da Vinci
	void stop();

	// Make a request to connect to the da Vinci
	ISI_STATUS connect();

	// Subscribe to all events and fields, and set up the given callbacks.
	//     If the stream fails, disconnect from the da Vinci.
	ISI_STATUS subscribe(ISI_EVENT_CALLBACK eCB, ISI_STREAM_CALLBACK sCB, void* eventUserData, void* streamUserData);

	//  Subscribe the given stream callback
	ISI_STATUS setStreamCallback(ISI_STREAM_CALLBACK sCB, void* userData);

	// Subscribe the given event callback
	ISI_STATUS setEventCallback(ISI_EVENT_CALLBACK eCB, void* userData);

	// Added. Return a vector of the manipulator names.
	std::vector<std::string> getManipulatorNames();

	// From the previously set manipIndex, get the TIP TRANSFORM.
	void getPosition(ISI_TRANSFORM* T);

	// Print out the 6DOF from the given transform.
	void printTransform(const ISI_TRANSFORM *T);

	// Print out help.
	void printHelp();

	// Added. Accessor for library version.
	std::string getLibraryVersion();

	// Added. Accessor for connected state.
	bool isConnected();

	// Added. Mutator for connection arguments.
	void setHostInfo(const std::string ip, const unsigned int port, const std::string pass);
	
	void setIpAddr(const std::string ip);
	
	void setPort(const unsigned int port);
	
	void setPassword(const std::string password);
	
private:
	
	// Moved. Connect with hardcoded arguments for our custom config. 
	ISI_STATUS connectWithArgs();

	void copyTransform(ISI_TRANSFORM* in, ISI_TRANSFORM* out);

	void printStreamState(ISI_MANIP_INDEX manipIndex);

	void printVersion();

	ISI_BOOLEAN mPrintStream;
	ISI_STATUS mStatus;
	ISI_BOOLEAN mQuit;
	ISI_MANIP_INDEX mManipIndex;

	// RS: Moved this to be private.
	bool mConnected;

	std::string mIpAddr;
	unsigned int  mPort;
	std::string mPassword;
	};

#endif
