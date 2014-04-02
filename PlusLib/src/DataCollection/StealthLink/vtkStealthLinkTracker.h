/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkStealthLinkTracker_h
#define __vtkStealthLinkTracker_h

#include "vtkPlusDevice.h"

//class StealthServer;

class VTK_EXPORT vtkStealthLinkTracker : public vtkPlusDevice
{
public:

  static vtkStealthLinkTracker *New();
  vtkTypeMacro(vtkStealthLinkTracker,vtkPlusDevice);
  void PrintSelf( ostream& os, vtkIndent indent );

  /*! Hardware device SDK version. */
  virtual std::string GetSdkVersion(); 
 
  /*! Get image from the camera into VTK images. If an input arguments is NULL then that image is not retrieved. !*/
  PlusStatus vtkStealthLinkTracker::GetDicomImage(vtkImageData*);

  virtual bool IsTracker() const { return true; }


protected:
	/*! Constructor !*/
	vtkStealthLinkTracker();
	/*! Destructor !*/
	~vtkStealthLinkTracker();

	/*! Connect to the tracker hardware */
	virtual PlusStatus InternalConnect();
	/*! Disconnect from the tracker hardware */
    virtual PlusStatus InternalDisconnect();

	/*! Start recording !*/
	virtual PlusStatus InternalStartRecording();
	/*! Stop the tracking system and bring it back to its initial state. */
	virtual PlusStatus InternalStopRecording();
	/*! The internal function that does the grab !*/
	PlusStatus InternalUpdate(); 

	/*! Acquire the current exam from the server !*/
	PlusStatus GetCurrentExam();
	/*! Acquire the current Instrument and Frame from the server !*/
	PlusStatus GetCurrentInstrumentAndFrame();
	// For internal storage of additional variables (to minimize the number of included headers)
	class vtkInternal;
	vtkInternal* Internal; 

	bool IsStealthServerInitialized;
	bool TrackerTimeToSystemTimeComputed;
	double TrackerTimeToSystemTimeSec;
	unsigned long stealthFrameNumber;


	/*! Is Tracker Connected? !*/
	PlusStatus IsLocalizerConencted();

	static void ModifyPatientName(std::string& patientName);

  /*!
    Probe to see if the tracking system is present.
  */
 // PlusStatus Probe();

  /*!
    Get an update from the tracking system and push the new transforms
    to the tools.  This should only be used within vtkTracker.cxx.
  */

  
  /*! Get the status of the MicronTracker (Tracking or not) */
 // vtkGetMacro(IsStealthLinkInitialized, int);
 
  /*! Read MicronTracker configuration and update the tracker settings accordingly */
  virtual PlusStatus ReadConfiguration( vtkXMLDataElement* config );

  /*! Write current MicronTracker configuration settings to XML */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* rootConfigElement);

 

 // StealthServer* GetStealthLinkServer() { return this->SLS; };

 // static void LogMessageCallback(int level, const char *message, void *userdata);




  
  /*!
    Start the tracking system.  The tracking system is brought from
    its ground state into full tracking mode.  The POLARIS will
    only be reset if communication cannot be established without
    a reset.
  */
 

  /*! Refresh the loaded markers by loading them from the Markers directory */
  //PlusStatus RefreshMarkerTemplates();

  /*! Returns the transformation matrix of the index_th marker */
 

  /*! Pointer to the StealthLinkServer class instance */

  /*! Non-zero if the tracker has been initialized */
  /*! Index of the last frame number. This is used for providing a frame number when the tracker doesn't return any transform */
  //double LastFrameNumber;
  
  //unsigned int FrameNumber;
  //std::string TemplateDirectory;
  //std::string IniFile;


//private:
 // vtkStealthLinkTracker(const vtkStealthLinkTracker&);
  //void operator=(const vtkStealthLinkTracker&);  
};

#endif
