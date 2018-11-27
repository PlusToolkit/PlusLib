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

#ifndef __vtkPlusWin32VideoSource2_h
#define __vtkPlusWin32VideoSource2_h

#include "vtkPlusDataCollectionExport.h"

#include "vtkPlusDevice.h"
//#include "igsioVideoFrame.h"

class vtkPlusWin32VideoSource2Internal;

/*!
  \class vtkPlusWin32VideoSource2
  \brief Video-for-Windows video digitizer

  vtkPlusWin32VideoSource2 grabs frames or streaming video from a
  Video for Windows compatible device on the Win32 platform.
  vtkPlusWin32VideoSource2 is an updated version of vtkWin32VideoSource and uses
  vtkPlusDevice instead of vtkVideoSource.

  Caveats:
  With some capture cards, if this class is leaked and ReleaseSystemResources
  is not called, you may have to reboot before you can capture again.
  vtkPlusDevice used to keep a global list and delete the video sources
  if your program leaked, due to exit crashes that was removed.

  \sa vtkPlusDevice vtkMILVideoSource2 vtkWin32VideoSource
  \ingroup PlusLibDataCollection
*/
class vtkPlusDataCollectionExport vtkPlusWin32VideoSource2 : public vtkPlusDevice
{
public:
  static vtkPlusWin32VideoSource2* New();
  vtkTypeMacro(vtkPlusWin32VideoSource2, vtkPlusDevice);
  virtual void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

  /*! Request a particular frame size */
  virtual PlusStatus SetFrameSize(const FrameSizeType& frameSize);

  /*! Request a particular frame rate (default 30 frames per second). */
  virtual PlusStatus SetAcquisitionRate(double rate);

  /*! Request a particular output format (default: VTK_RGB). */
  virtual PlusStatus SetOutputFormat(int format);

  /*! Turn on/off the preview (overlay) window. */
  void SetPreview(int showPreview);
  vtkBooleanMacro(Preview, int);
  /*! Get state of preview (overlay) window */
  vtkGetMacro(Preview, int);

  /*! Bring up a modal dialog box for video format selection. */
  PlusStatus VideoFormatDialog();

  /*! Bring up a modal dialog box for video input selection. */
  PlusStatus VideoSourceDialog();

  /*! Callback function called on parent window destroyed. Public to allow calling from static function. */
  void OnParentWndDestroy();

  /*! Adds a frame to the frame buffer. Called whenever the driver notified a new frame acquisition. Public to allow calling from static function. */
  PlusStatus AddFrameToBuffer(void* lpVideoHeader);

  /*! Verify the device is correctly configured */
  virtual PlusStatus NotifyConfigured();

  virtual bool IsTracker() const { return false; }

protected:

  /*! Constructor */
  vtkPlusWin32VideoSource2();
  /*! Destructor */
  ~vtkPlusWin32VideoSource2();

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
  virtual PlusStatus InternalUpdate();

  /*! Set the capture window class name */
  vtkSetStringMacro(WndClassName);

  char* WndClassName;
  int Preview;

  int FrameIndex;

  vtkPlusWin32VideoSource2Internal* Internal;

  /*! Update the buffer format to match the capture settings */
  PlusStatus UpdateFrameBuffer();

  void ReleaseSystemResources();

  igsioVideoFrame UncompressedVideoFrame;

private:
  vtkPlusWin32VideoSource2(const vtkPlusWin32VideoSource2&);  // Not implemented.
  void operator=(const vtkPlusWin32VideoSource2&);  // Not implemented.
};

#endif