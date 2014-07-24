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

#define ISI_API_RATE        60 // Hz

class daVinci
{

public: 
  // RS: Constructor
  daVinci();

  // RS: Destructor
  ~daVinci();

  // RS: Disconnect from the da Vinci
  void Stop();

  // RS: Make a request to connect to the da Vinci
  ISI_STATUS Connect();

  // RS: Subscribe to all events and fields, and set up the given callbacks.
  //     If the stream fails, disconnect from the da Vinci.
  ISI_STATUS Subscribe(ISI_EVENT_CALLBACK eCB, ISI_STREAM_CALLBACK sCB);

  // RS: Request all manipulators indices and names. Prompt the user for 
  //     a selection, and if its valid, set the manipIndex to the prompted value. 
  void get_manip_id(ISI_MANIP_INDEX *manipIndex);

  // RS: Added. Return a vector of the manipulator names.
  std::vector<std::string> get_manip_names_list();

  // RS: From the previously set manipIndex, get the TIP TRANSFORM.
  void daVinci::get_position(ISI_TRANSFORM* T);

  // RS: Print out the 6DOF from the given transform.
  void print_transform(const ISI_TRANSFORM *T);

  // RS: Print out help.
  void print_help();

  // RS: Added. Accessor for library version.
  std::string get_library_version();

  // RS: Added. Accessor for connected state.
  bool IsConnected();
  
private:
  
  // RS: Moved. Connect with hardcoded arguments for our custom config. 
  ISI_STATUS connect_with_args();

  void transform_copy(ISI_TRANSFORM* in, ISI_TRANSFORM* out);

  void print_stream_state(ISI_MANIP_INDEX manipIndex);

  void save_stream_state(ISI_MANIP_INDEX manipIndex);

  void print_version();

  //ISI_EVENT_CALLBACK eventCB;
  //ISI_STREAM_CALLBACK streamCB;

  ISI_BOOLEAN g_print_stream;
  ISI_STATUS status;
  ISI_BOOLEAN quit;
  ISI_MANIP_INDEX manipIndex;

  // RS: Moved this to be private.
  bool connected;

  const ISI_CHAR* mIpAddr = "10.0.0.5";
  const ISI_UINT mPort = 5002;
  const ISI_CHAR* mPassword = "";
};

