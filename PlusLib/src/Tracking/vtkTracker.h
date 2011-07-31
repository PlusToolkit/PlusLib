/*=========================================================================

Program:   AtamaiTracking for VTK
Module:    $RCSfile: vtkTracker.h,v $
Creator:   David Gobbi <dgobbi@atamai.com>
Language:  C++
Author:    $Author: dgobbi $
Date:      $Date: 2008/04/10 17:26:35 $
Version:   $Revision: 1.7 $

==========================================================================

Copyright (c) 2000-2005 Atamai, Inc.

Use, modification and redistribution of the software, in source or
binary forms, are permitted provided that the following terms and
conditions are met:

1) Redistribution of the source code, in verbatim or modified
form, must retain the above copyright notice, this license,
the following disclaimer, and any notices that refer to this
license and/or the following disclaimer.  

2) Redistribution in binary form must include the above copyright
notice, a copy of this license and the following disclaimer
in the documentation or with other materials provided with the
distribution.

3) Modified copies of the source code must be clearly marked as such,
and must not be misrepresented as verbatim copies of the source code.

THE COPYRIGHT HOLDERS AND/OR OTHER PARTIES PROVIDE THE SOFTWARE "AS IS"
WITHOUT EXPRESSED OR IMPLIED WARRANTY INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE.  IN NO EVENT SHALL ANY COPYRIGHT HOLDER OR OTHER PARTY WHO MAY
MODIFY AND/OR REDISTRIBUTE THE SOFTWARE UNDER THE TERMS OF THIS LICENSE
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, LOSS OF DATA OR DATA BECOMING INACCURATE
OR LOSS OF PROFIT OR BUSINESS INTERRUPTION) ARISING IN ANY WAY OUT OF
THE USE OR INABILITY TO USE THE SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGES.

=========================================================================*/
// .NAME vtkTracker - interfaces VTK with real-time 3D tracking systems
// .SECTION Description
// The vtkTracker is a generic VTK interface to real-time tracking
// systems.  Subclasses to this class implement this interface for
// the POLARIS (Northern Digital Inc., Waterloo, Canada), the
// Flock of Birds (Ascension Technology Corporation), and a few
// other systems.
// Derived classes should override the Probe(), InternalUpdate(),
// InternalStartTracking(), and InternalStopTracking() methods.
// The InternalUpdate() method is called from within a separate
// thread, therefore its contents must be thread safe.  Use the
// vtkPOLARISTracker as a framework for developing subclasses
// for new tracking systems.
// .SECTION see also
// vtkTrackerTool vtkPOLARISTracker vtkFlockTracker

#ifndef __vtkTracker_h
#define __vtkTracker_h

#include "PlusConfigure.h"
#include "vtkTrackerBuffer.h"
#include "vtkObject.h"
#include "vtkCriticalSection.h"
#include "vtkXMLDataElement.h"
#include <string>
#include <vector>
#include <map>

class vtkMatrix4x4;
class vtkMultiThreader;
class vtkTrackerTool;
class vtkSocketCommunicator;
class vtkCharArray;
class vtkDataArray;
class vtkDoubleArray;
class vtkHTMLGenerator; 
class vtkGnuplotExecuter;

//// several flags which give added info about a transform
//enum {
//	TR_OK			   = 0x0000,  // Tool OK
//	TR_MISSING       = 0x0001,  // tool or tool port is not available
//	TR_OUT_OF_VIEW   = 0x0002,  // cannot obtain transform for tool
//	TR_OUT_OF_VOLUME = 0x0004,  // tool is not within the sweet spot of system
//	TR_SWITCH1_IS_ON = 0x0010,  // various buttons/switches on tool
//	TR_SWITCH2_IS_ON = 0x0020,
//	TR_SWITCH3_IS_ON = 0x0040, 
//	TR_REQ_TIMEOUT   = 0x0100   // Request timeout
//
//};

// Flags for tool LEDs (specifically for the POLARIS)
enum {
	TR_LED_OFF   = 0,
	TR_LED_ON    = 1,
	TR_LED_FLASH = 2
};

// Tracker tool types
enum TRACKER_TOOL_TYPE
{
  TRACKER_TOOL_NONE=0, 
  TRACKER_TOOL_REFERENCE,
  TRACKER_TOOL_PROBE,
  TRACKER_TOOL_STYLUS, 
  TRACKER_TOOL_NEEDLE,
  TRACKER_TOOL_GENERAL
}; 

class VTK_EXPORT vtkTracker : public vtkObject
{
public:
	static vtkTracker *New();
	vtkTypeMacro(vtkTracker,vtkObject);
	void PrintSelf(ostream& os, vtkIndent indent);

	// Description:
	// Probe to see to see if the tracking system is connected to the
	// computer.  Returns 1 if the tracking system was found and is working.
	// Do not call this method while the system is Tracking.  This method
	// should be overridden in subclasses. 
	virtual PlusStatus Probe();

	// Description:
	// Start the tracking system.  The tracking system is brought from
	// its ground state (i.e. on but not necessarily initialized) into
	// full tracking mode.  This method calls InternalStartTracking()
	// after doing a bit of housekeeping.
	virtual PlusStatus StartTracking();

	// Description:
	// Stop the tracking system and bring it back to its ground state.
	// This method calls InternalStopTracking().
	virtual PlusStatus StopTracking();

	// Description:
	// Test whether or not the system is tracking.
	virtual int IsTracking() { return this->Tracking; };

  // Description:
  // Set/get recording start time for each tool
  virtual void SetStartTime( double startTime ); 
  virtual double GetStartTime(); 

	// Description:
	// This method will call Update() on each of the tools.  Note that
	// this method does not call the InternalUpdate() method, which
	// is called by a separate thread.
	//virtual PlusStatus Update();

	// Description:
	// Read/write main configuration from/to xml data
	virtual PlusStatus ReadConfiguration(vtkXMLDataElement* config); 
	virtual PlusStatus WriteConfiguration(vtkXMLDataElement* config); 

	// Description:
	// Convert tracker status to string 
	// TODO the retudn valus should be PLusStatus and the result should be got using parameter by reference
	static std::string ConvertTrackerStatusToString(TrackerStatus status); 

	// Description:
	// Get tool type enum from string and vice versa
	static PlusStatus ConvertStringToToolType(const char* typeString, TRACKER_TOOL_TYPE &type);
	static PlusStatus ConvertToolTypeToString(const TRACKER_TOOL_TYPE type, char* &typeString);

	// Description:
	// Get the buffer element values of each tool in a string list by timestamp. 
	virtual PlusStatus GetTrackerToolBufferStringList(double timestamp, 
		std::map<std::string, std::string> &toolsBufferMatrices, 
		std::map<std::string, std::string> &toolsCalibrationMatrices, 
		std::map<std::string, std::string> &toolsStatuses,
		bool calibratedTransform = false); 

	// Description:
	// Add generated html report from tracking data acquisition to the existing html report
	// htmlReport and plotter arguments has to be defined by the caller function
	virtual PlusStatus GenerateTrackingDataAcquisitionReport( vtkHTMLGenerator* htmlReport, vtkGnuplotExecuter* plotter, const char* gnuplotScriptsFolder); 

	// Description:
	// Get the internal update rate for this tracking system.  This is
	// the number of transformations sent by the tracking system per
	// second per tool.
	double GetInternalUpdateRate() { return this->InternalUpdateRate; };

	// Description:
	// Get the tool object for the specified port.  The first tool is
	// retrieved by GetTool(0).  See vtkTrackerTool for more information.
	vtkTrackerTool *GetTool(int port);

	// Description:
	// Get the number of available tool ports.  This is the maxiumum that a
	// particular tracking system can support, not the number of tools
	// that are actually connected to the system.  In order to determine
	// how many tools are connected, you must call Update() and then
	// check IsMissing() for each tool between 0 and NumberOfTools-1.
	vtkGetMacro(NumberOfTools, int);

	// Description:
	// Get the timestamp for the last time that Update() was called, in
	// seconds since 1970 (i.e. the UNIX epoch).  This method is not a
	// good method of getting timestamps for tracking information,
	// you should use the vtkTrackerTool GetTimeStamp() method to get
	// the timestamp associated with each transform.  This method is
	// only valuable for determining e.g. how old the transforms were
	// before the Update method was called.
	vtkGetMacro(UpdateTimeStamp,double);

	// Description:
	// Get tool port by name 
	int GetToolPortByName( const char* toolName); 

	// Description:
	// Get tool ports by type
	PlusStatus GetToolPortNumbersByType(TRACKER_TOOL_TYPE type, std::vector<int> &toolNumbersVector);
	int GetFirstPortNumberByType(TRACKER_TOOL_TYPE type);

	// Description:
	// Get port number of reference tool
	int GetReferenceToolNumber();

	// Description:
	// Get port number of the first active tool tool
	PlusStatus GetFirstActiveTool(int &tool);

	// Description:
	// Set/get the acquisition frequency
	vtkSetMacro(Frequency, double);
	vtkGetMacro(Frequency, double);

	// Description:
	// Set/get the acquisition frequency
	vtkSetMacro(TrackerCalibrated, bool);
	vtkGetMacro(TrackerCalibrated, bool);
	vtkBooleanMacro(TrackerCalibrated, bool); 

	// Description:
	// Set the transformation matrix between tracking-system coordinates
	// and the desired world coordinate system.  You can use 
	// vtkLandmarkTransform to create this matrix from a set of 
	// registration points.  Warning: the matrix is copied,
	// not referenced.
  vtkSetObjectMacro(WorldCalibrationMatrix, vtkMatrix4x4); 
  vtkGetObjectMacro(WorldCalibrationMatrix, vtkMatrix4x4); 

  // Description:
	// Make the unit emit a string of audible beeps.  This is
	// supported by the POLARIS.
	void Beep(int n);

	// Description:
	// Turn one of the LEDs on the specified tool on or off.  This
	// is supported by the POLARIS.
	void SetToolLED(int tool, int led, int state);

	// Description:
	// The subclass will do all the hardware-specific update stuff
	// in this function.   It should call ToolUpdate() for each tool.
	// Note that vtkTracker.cxx starts up a separate thread after
	// InternalStartTracking() is called, and that InternalUpdate() is
	// called repeatedly from within that thread.  Therefore, any code
	// within InternalUpdate() must be thread safe.  You can temporarily
	// pause the thread by locking this->UpdateMutex->Lock() e.g. if you
	// need to communicate with the device from outside of InternalUpdate().
	// A call to this->UpdateMutex->Unlock() will resume the thread.
	virtual PlusStatus InternalUpdate() { return PLUS_SUCCESS; };

	//BTX
	// These are used by static functions in vtkTracker.cxx, and since
	// VTK doesn't generally use 'friend' functions they are public
	// instead of protected.  Do not use them anywhere except inside
	// vtkTracker.cxx.
	vtkCriticalSection *UpdateMutex;
	vtkCriticalSection *RequestUpdateMutex;
	vtkTimeStamp UpdateTime;
	double InternalUpdateRate;  
	//ETX

	// Description:
	// The ServerTracker should call this function. This creates
	// a thread that allows it to wait till a client connects. 
	virtual PlusStatus Connect();
	virtual PlusStatus Disconnect();

	// Description:
	// Make this tracker into a copy of another tracker.
	void DeepCopy(vtkTracker *tracker);

protected:
	vtkTracker();
	~vtkTracker();

	// Description:
	// This function is called by InternalUpdate() so that the subclasses
	// can communicate information back to the vtkTracker base class, which
	// will in turn relay the information to the appropriate vtkTrackerTool.
	PlusStatus ToolTimeStampedUpdate(int tool, vtkMatrix4x4 *matrix, TrackerStatus status, unsigned long frameNumber, 
		double unfilteredtimestamp);

	// Description:
	// Set the number of tools for the tracker -- this method is
	// only called once within the constructor for derived classes.
	void SetNumberOfTools(int num);

	// Description:
	// Set the tool name
	void SetToolName(int tool, const char* name);

	// Description:
	// These methods should be overridden in derived classes: 
	// InternalStartTracking() should initialize the tracking device, and
	// InternalStopTracking() should free all resources associated with
	// the device.  These methods should return 1 if they are successful,
	// or 0 if they are not.
	virtual PlusStatus InternalStartTracking() { return PLUS_SUCCESS; };
	virtual PlusStatus InternalStopTracking() { return PLUS_SUCCESS; };
	virtual void InternalInterpretCommand( char * c) { };
	// Description:
	// This method should be overridden in derived classes that can make
	// an audible beep.  The return value should be zero if an error
	// occurred while the request was being processed.
	virtual PlusStatus InternalBeep(int n) { return PLUS_SUCCESS; };

	// Description:
	// This method should be overridden for devices that have one or more LEDs
	// on the tracked tools. The return value should be zero if an error
	// occurred while the request was being processed.
	virtual PlusStatus InternalSetToolLED(int tool, int led, int state) { return PLUS_SUCCESS; };

	vtkMatrix4x4 *WorldCalibrationMatrix;
	int NumberOfTools;
	vtkTrackerTool **Tools;
	int Tracking;

	double UpdateTimeStamp;
	unsigned long LastUpdateTime;

	vtkMultiThreader *Threader;
	int ThreadId;

	double Frequency; 

	bool TrackerCalibrated; 

private:
	vtkTracker(const vtkTracker&);
	void operator=(const vtkTracker&);  
};

#endif

