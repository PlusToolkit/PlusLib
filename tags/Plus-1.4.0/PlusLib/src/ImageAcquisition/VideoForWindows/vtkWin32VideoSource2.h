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

// .NAME vtkWin32VideoSource2 - Video-for-Windows video digitizer
// .SECTION Description
// vtkWin32VideoSource2 grabs frames or streaming video from a
// Video for Windows compatible device on the Win32 platform.
// vtkWin32VideoSource2 is an updated version of vtkWin32VideoSource and uses
// vtkPlusVideoSource instead of vtkVideoSource
// .SECTION Caveats
// With some capture cards, if this class is leaked and ReleaseSystemResources 
// is not called, you may have to reboot before you can capture again.
// vtkPlusVideoSource used to keep a global list and delete the video sources
// if your program leaked, due to exit crashes that was removed.
//
// .SECTION See Also
// vtkPlusVideoSource vtkMILVideoSource2 vtkWin32VideoSource

#ifndef __vtkWin32VideoSource2_h
#define __vtkWin32VideoSource2_h

#include "vtkPlusVideoSource.h"

class vtkWin32VideoSource2Internal;

/*!
  \class vtkSonixVideoSource 
  \brief VTK interface for video input from Video for Windows
  \ingroup PlusLibImageAcquisition
*/ 
class VTK_HYBRID_EXPORT vtkWin32VideoSource2 : public vtkPlusVideoSource
{
public:
  static vtkWin32VideoSource2 *New();
  vtkTypeRevisionMacro(vtkWin32VideoSource2,vtkPlusVideoSource);
  void PrintSelf(ostream& os, vtkIndent indent);   

  /*! Standard VCR functionality: Record incoming video. */
  void Record();

  /*! Standard VCR functionality: Stop recording. */
  void Stop();

  /*! Grab a single video frame. */
  void Grab();
 
  /*! Request a particular frame size (set the third value to 1). */
  void SetFrameSize(int x, int y, int z);
  /*! Request a particular frame size (set the third value to 1). */
  virtual void SetFrameSize(int dim[3]) { this->SetFrameSize(dim[0], dim[1], dim[2]); };
  
  /*! Request a particular frame rate (default 30 frames per second). */
  void SetFrameRate(float rate);

  /*! Request a particular output format (default: VTK_RGB). */
  void SetOutputFormat(int format);

  /*! Turn on/off the preview (overlay) window. */
  void SetPreview(int p);
  vtkBooleanMacro(Preview,int);
  /*! Get state of preview (overlay) window */
  vtkGetMacro(Preview,int);

  /*! Bring up a modal dialog box for video format selection. */
  void VideoFormatDialog();

  /*! Bring up a modal dialog box for video input selection. */
  void VideoSourceDialog();

  /*! Initialize the driver (this is called automatically when the first grab is done) */
  void Initialize();

  /*! Free the driver (this is called automatically inside the */
  // destructor).
  void ReleaseSystemResources();

  /*! Grab - for internal use only */
  void LocalInternalGrab(void*);

  /*! Callback function called on parent window destroyed */
  void OnParentWndDestroy();

protected:
  /*! Constructor */
  vtkWin32VideoSource2();
  /*! Destructor */
  ~vtkWin32VideoSource2();

  char WndClassName[16];
  int BitMapSize;
  int Preview;

  vtkWin32VideoSource2Internal *Internal;

  void CheckBuffer();
  void DoVFWFormatSetup();
  void DoVFWFormatCheck();

private:
  vtkWin32VideoSource2(const vtkWin32VideoSource2&);  // Not implemented.
  void operator=(const vtkWin32VideoSource2&);  // Not implemented.
};

#endif





