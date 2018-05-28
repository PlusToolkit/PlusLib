/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef _INTUITIVE_DAVINCI_H_
#define _INTUITIVE_DAVINCI_H_

#ifdef WIN32
  #define _CRT_SECURE_NO_WARNINGS
  #include <windows.h>
#endif

// OS includes
#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

// STL includes
#include <iostream>
#include <sstream>
#include <vector>

// Intuitive includes
#include <isi_api_types.h>
#include <isi_api.h>

class IntuitiveDaVinci
{
public:
  // Constructor
  IntuitiveDaVinci();

  // Destructor
  ~IntuitiveDaVinci();

  // Start streaming from the da Vinci
  bool start();

  // Stop streaming from the da Vinci
  void stop();

  // Make a request to connect to the da Vinci
  ISI_STATUS connect();

  // Make a request to disconnect from the da Vinci
  ISI_STATUS disconnect();

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
  void printTransform(const ISI_TRANSFORM* T);

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

protected:
  // Moved. Connect with hardcoded arguments for our custom config.
  ISI_STATUS connectWithArgs();

  void copyTransform(ISI_TRANSFORM* in, ISI_TRANSFORM* out);

  void printStreamState(ISI_MANIP_INDEX manipIndex);

  void printVersion();

protected:
  ISI_BOOLEAN       mPrintStream;
  ISI_STATUS        mStatus;
  ISI_BOOLEAN       mQuit;
  ISI_MANIP_INDEX   mManipIndex;

  bool              mConnected;

  unsigned int      mRateHz;
  std::string       mIpAddr;
  unsigned int      mPort;
  std::string       mPassword;
};

#endif
