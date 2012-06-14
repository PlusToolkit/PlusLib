#pragma once

/// \mainpage Overview
/// 
/// The Acquisition Library exposes functionality to use IQ data received from
/// the research interface. 
/// 
/// The data is pulled from the grabber card using the 
/// acquisition grabber. The acquisition settings load settings for the grab, e.g.
/// how many frames to grab or how to set up the research interface. When a frame is 
/// available, the frame data is made available to the acquisition injector. The 
/// acquisition injector maintains a list of receivers that must implement the 
/// IAcquisitionDataReceiver interface. These receivers are called in turn with 
/// the needed data. The BmodeViewDataReceiver
/// and SaperaViewDataReceiver are examples 
/// of how to implement the of how to implement IAcquisitionDataReceiver interface.
/// library. An example of usage can be found in the demo 
/// Demo_AcquisitionLibrary.
/// 
/// Currently, the Acquisition Library only support Sapera grabbers, and it is the 
/// responsibility of the user to have the Sapera libraries installed. However, 
/// the AcquisitionGrabberSapera wraps all Sapera dependencies. AcquisitionGrabberSapera
/// implements the IAcquisitionGrabber interface, so if another library is to be used,
/// it "just" needs to implement that interface.
/// 
/// \section sec1 Using the Acquisition Library
/// 
/// The code snippet below illustrates how to use and set up a grabber using Sapera. 
/// 
/// \code
/// // declare the acquisition objects
/// AcquisitionInjector injector;
/// AcquisitionSettings settings;
/// AcquisitionGrabberSapera sapera;
/// BmodeViewDataReceiver bmodeView;
/// 
/// // start by loading the ini and setting up the grab the number
/// // of samples per line and number of lines can be loaded 
/// // from the scanner, using the CommandAndControl class
/// char IniFileName[] = ".\\IniFile.ini";
/// settings.LoadIni(IniFileName);
/// settings.SetRFLineLength(numSamples);
/// settings.SetLinesPerFrame(numLines);
/// settings.SetFramesToGrab(numFrames);
/// 
/// // settings are loaded, so initialize the grabber
/// sapera.Init(settings);
/// 
/// // add data receiver to the injector
/// injector.AddDataReceiver(&bmodeView);
/// 
/// // start grabbing
/// sapera.StartGrabbing(&injector);
/// 
/// // the current thread will continue here, while a new grabber thread 
/// // handles grabbing and processing of grabbed data.
/// \endcode
/// 
/// Most of the functions above will return true if they are successful, and false
/// otherwise. It is recommended to inspect and handle the return value. 
/// 

class AcquisitionImpl;   // This will be a private implementation of CommandAndControl

/// <summary>	Intended for use with Command control. Currently unused. </summary>
struct AcquisitionSettings
{
	bool acquire;

};

/// <summary>	Intended for use with Command control. Currently unused. </summary>
class Acquisition
{
public:
	Acquisition();
	virtual ~Acquisition();
private:
	AcquisitionImpl * impl;
};