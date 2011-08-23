/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkWin32VideoSource2.h,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

  Author: Danielle Pace
          Robarts Research Institute and The University of Western Ontario

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

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

class VTK_HYBRID_EXPORT vtkWin32VideoSource2 : public vtkPlusVideoSource
{
public:
  static vtkWin32VideoSource2 *New();
  vtkTypeRevisionMacro(vtkWin32VideoSource2,vtkPlusVideoSource);
  void PrintSelf(ostream& os, vtkIndent indent);   

  // Description:
  // Standard VCR functionality: Record incoming video.
  void Record();

  // Description:
  // Standard VCR functionality: Stop recording.
  void Stop();

  // Description:
  // Grab a single video frame.
  void Grab();
 
  // Description:
  // Request a particular frame size (set the third value to 1).
  void SetFrameSize(int x, int y, int z);
  virtual void SetFrameSize(int dim[3]) { 
    this->SetFrameSize(dim[0], dim[1], dim[2]); };
  
  // Description:
  // Request a particular frame rate (default 30 frames per second).
  void SetFrameRate(float rate);

  // Description:
  // Request a particular output format (default: VTK_RGB).
  void SetOutputFormat(int format);

  // Description:
  // Turn on/off the preview (overlay) window.
  void SetPreview(int p);
  vtkBooleanMacro(Preview,int);
  vtkGetMacro(Preview,int);

  // Description:
  // Bring up a modal dialog box for video format selection.
  void VideoFormatDialog();

  // Description:
  // Bring up a modal dialog box for video input selection.
  void VideoSourceDialog();

  // Description:
  // Initialize the driver (this is called automatically when the
  // first grab is done).
  void Initialize();

  // Description:
  // Free the driver (this is called automatically inside the
  // destructor).
  void ReleaseSystemResources();

  // Description:
  // For internal use only
  void LocalInternalGrab(void*);
  void OnParentWndDestroy();

protected:
  vtkWin32VideoSource2();
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





