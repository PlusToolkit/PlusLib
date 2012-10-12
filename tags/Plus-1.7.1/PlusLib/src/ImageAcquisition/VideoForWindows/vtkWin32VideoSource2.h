/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

/*=========================================================================
The following copyright notice is applicable to parts of this file:
Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
All rights reserved.
See Copyright.txt or http://www.kitware.com/Copyright.htm for details.
Authors include: Danielle Pace
(Robarts Research Institute and The University of Western Ontario)
=========================================================================*/ 

#ifndef __vtkWin32VideoSource2_h
#define __vtkWin32VideoSource2_h

#include "vtkPlusVideoSource.h"
#include "PlusVideoFrame.h"

class vtkWin32VideoSource2Internal;

/*!
  \class vtkWin32VideoSource2 
  \brief Video-for-Windows video digitizer

  vtkWin32VideoSource2 grabs frames or streaming video from a
  Video for Windows compatible device on the Win32 platform.
  vtkWin32VideoSource2 is an updated version of vtkWin32VideoSource and uses
  vtkPlusVideoSource instead of vtkVideoSource.

  Caveats:
  With some capture cards, if this class is leaked and ReleaseSystemResources 
  is not called, you may have to reboot before you can capture again.
  vtkPlusVideoSource used to keep a global list and delete the video sources
  if your program leaked, due to exit crashes that was removed.

  \sa vtkPlusVideoSource vtkMILVideoSource2 vtkWin32VideoSource
  \ingroup PlusLibImageAcquisition
*/ 
class VTK_EXPORT vtkWin32VideoSource2 : public vtkPlusVideoSource
{
public:
  static vtkWin32VideoSource2 *New();
  vtkTypeRevisionMacro(vtkWin32VideoSource2,vtkPlusVideoSource);
  void PrintSelf(ostream& os, vtkIndent indent);   
 
  /*! Request a particular frame size */
  virtual PlusStatus SetFrameSize(int x, int y);
  
  /*! Request a particular frame rate (default 30 frames per second). */
  virtual PlusStatus SetAcquisitionRate(double rate);

  /*! Request a particular output format (default: VTK_RGB). */
  virtual PlusStatus SetOutputFormat(int format);

  /*! Turn on/off the preview (overlay) window. */
  void SetPreview(int showPreview);
  vtkBooleanMacro(Preview,int);
  /*! Get state of preview (overlay) window */
  vtkGetMacro(Preview,int);

  /*! Bring up a modal dialog box for video format selection. */
  PlusStatus VideoFormatDialog();

  /*! Bring up a modal dialog box for video input selection. */
  PlusStatus VideoSourceDialog();

  /*! Callback function called on parent window destroyed. Public to allow calling from static function. */
  void OnParentWndDestroy();

  /*! Adds a frame to the frame buffer. Called whenever the driver notified a new frame acquisition. Public to allow calling from static function. */
	PlusStatus AddFrameToBuffer(void *lpVideoHeader);

protected:

  /*! Constructor */
  vtkWin32VideoSource2();
  /*! Destructor */
  ~vtkWin32VideoSource2();

  /*! Device-specific connect */
	virtual PlusStatus InternalConnect();

  /*! Device-specific disconnect */
	virtual PlusStatus InternalDisconnect();

  /*! Device-specific recording start */
	virtual PlusStatus InternalStartRecording();

  /*! Device-specific recording stop */
	virtual PlusStatus InternalStopRecording();

  /*!
    The internal function which actually grabs one frame.
    It just requests a single frame from the hardware and the object
    will be notified when it is ready.
  */
  virtual PlusStatus InternalGrab();


  /*! Set the capture window class name */
  vtkSetStringMacro(WndClassName);

  char* WndClassName;
  int Preview;

  int FrameIndex;

  vtkWin32VideoSource2Internal *Internal;

  /*! Update the capture settings to match the buffer format */
  PlusStatus SetCaptureSettings(int widh, int height);

  /*! Update the buffer format to match the capture settings */
  PlusStatus UpdateFrameBuffer();

  void ReleaseSystemResources();

  PlusVideoFrame UncompressedVideoFrame;

private:
  vtkWin32VideoSource2(const vtkWin32VideoSource2&);  // Not implemented.
  void operator=(const vtkWin32VideoSource2&);  // Not implemented.
};

#endif
