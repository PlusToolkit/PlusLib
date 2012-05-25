/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkEpiphanVideoSource_h
#define __vtkEpiphanVideoSource_h

#include "vtkPlusVideoSource.h"
#include "epiphan/frmgrab.h"

/* exit codes */
#define V2U_GRABFRAME_STATUS_OK       0  /* successful completion */
#define V2U_GRABFRAME_STATUS_NODEV    1  /* VGA2USB device not found */
#define V2U_GRABFRAME_STATUS_VMERR    2  /* Video mode detection failure */
#define V2U_GRABFRAME_STATUS_NOSIGNAL 3  /* No signal detected */
#define V2U_GRABFRAME_STATUS_GRABERR  4  /* Capture error */
#define V2U_GRABFRAME_STATUS_IOERR    5  /* File save error */
#define V2U_GRABFRAME_STATUS_CMDLINE  6  /* Command line syntax error */


class VTK_EXPORT vtkEpiphanVideoSource;

/*!
  \class vtkEpiphanVideoSource 
  \brief Class for providing video input interfaces between VTK and Epiphan frame grabber device
  \ingroup PlusLibImageAcquisition
*/
class VTK_EXPORT vtkEpiphanVideoSource : public vtkPlusVideoSource
{
public:
	static vtkEpiphanVideoSource *New();
	vtkTypeRevisionMacro(vtkEpiphanVideoSource,vtkPlusVideoSource);
	void PrintSelf(ostream& os, vtkIndent indent);   

  /*! Read configuration from xml data */	
    virtual PlusStatus ReadConfiguration(vtkXMLDataElement* config); 
  /*! Write configuration to xml data */
	virtual PlusStatus WriteConfiguration(vtkXMLDataElement* config);
  /*! Set the Epiphan device video format (e.g. "Y8" ) */
	vtkSetMacro(VideoFormat,V2U_UINT32); 
  /*! Get the Epiphan device video format (e.g. "Y8" ) */
	vtkGetMacro(VideoFormat,V2U_UINT32);  

  /*! Set the Epiphan device serial */
	vtkSetStringMacro(SerialNumber); 
  /*! Get the Epiphan device serial */
	vtkGetStringMacro(SerialNumber); 

  /*! Set the crop rectangle of frame grabber */
//	vtkSetMacro(CropRect, V2URect);
  /*! Get the crop rectangle of frame grabber */
//	vtkGetMacro(CropRect, V2URect);

protected:
  /*! Constructor */
	vtkEpiphanVideoSource();
  /*! Destructor */
	~vtkEpiphanVideoSource();

  /*! Device-specific connect */
	virtual PlusStatus InternalConnect();

  /*! Device-specific disconnect */
	virtual PlusStatus InternalDisconnect();

  /*! Device-specific recording start */
	virtual PlusStatus InternalStartRecording();

  /*! Device-specific recording stop */
	virtual PlusStatus InternalStopRecording();

  /*! The internal function which actually does the grab.  */
	PlusStatus InternalGrab();

  /*! Video format (e.g. Y8) */
	V2U_UINT32 VideoFormat;

  /*! Crop rectangle for the grabber */
	V2URect * CropRect;

  /*! Serial number of the frame grabber */
	char* SerialNumber;

  /*! Epiphan Pointer to the grabber */
	FrmGrabber* fg;

  /*! Frame size of the captured image */
	int FrameSize[2];

private:
	vtkEpiphanVideoSource(const vtkEpiphanVideoSource&);  // Not implemented.
	void operator=(const vtkEpiphanVideoSource&);  // Not implemented.
};

#endif





