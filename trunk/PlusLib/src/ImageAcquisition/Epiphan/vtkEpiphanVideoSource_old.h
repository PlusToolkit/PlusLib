/*=========================================================================

  File:		 vtkEpiphanVideoSource.h
  Creator:   Chris Wedlake <cwedlake@robarts.ca>

  ==========================================================================

  Copyright (c) Chris Wedlake, cwedlake@robarts.ca

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
// .NAME vtkEpiphanVideoSource - Epiphan Video Source for VTK (http://www.epiphan.com/products/dvi-frame-grabbers/dvi2usb-solo/)
// .SECTION Description provides an interface to Epiphan USB capture devices
//	through their API which is provided.  In order to use this class, you must link with frmgrab.lib.
//
//  Layouts can be added and removed from the screen as required but typically once they
//  are added they are left in the application.
//
// .SECTION Caveats
//  Have only tested with the DVI2USB-SOLO but should work with the other units as well.  some assumptions are made about the image size.
//	I am positive this will not compile under linux and only works/tested on windows.  The device should work with linux/mac however.  
// .SECTION see also
//  vtkVideoSource
//

#ifndef __vtkEpiphanVideoSource_h
#define __vtkEpiphanVideoSource_h

#include <windows.h>

#include "vtkVideoSource.h"  


#include "vtkMultiThreader.h"

#include "epiphan/frmgrab.h"

/* exit codes */
#define V2U_GRABFRAME_STATUS_OK       0  /* successful completion */
#define V2U_GRABFRAME_STATUS_NODEV    1  /* VGA2USB device not found */
#define V2U_GRABFRAME_STATUS_VMERR    2  /* Video mode detection failure */
#define V2U_GRABFRAME_STATUS_NOSIGNAL 3  /* No signal detected */
#define V2U_GRABFRAME_STATUS_GRABERR  4  /* Capture error */
#define V2U_GRABFRAME_STATUS_IOERR    5  /* File save error */
#define V2U_GRABFRAME_STATUS_CMDLINE  6  /* Command line syntax error */


class VTK_EXPORT vtkEpiphanVideoSource : public vtkPlusVideoSource
{
public:

  vtkTypeMacro(vtkEpiphanVideoSource,vtkVideoSource);
  static vtkEpiphanVideoSource *New();
  void PrintSelf(ostream& os, vtkIndent indent);   
  
  // Description:
  // Initialize the driver (this is called automatically when the
  // first grab is done).
  void Initialize();
  
  // Description:
  // Free the driver (this is called automatically inside the
  // destructor).
  void ReleaseSystemResources();

  // Shouldn't be but python wrapping was complaining and didn't feel like fixing 
  // for the time being as not needed from python.  Only needed if mulitple devices anyways
  //BTX
  void SetSerialNumber(char * serial);
  //ETX

  // Description:
  // Standard VCR functionality: Record incoming video.
  void Record();

  // Description:
  // Standard VCR functionality: Play recorded video.
  void Play();

  // Description:
  // Standard VCR functionality: Stop recording or playing.
  void Stop();

  // Description:
  // Request a particular frame rate (default 25 frames per second).
  // Reset to 25 fps at init but I don't recall why I was doing this.  I think
  // there was an issue with the computer trying to keep up to 60 fps and would keep
  // dropping frames but I could be wrong.
  void SetFrameRate(float rate);

  // Description:
  // Set the output format.  This must be appropriate for device,
  // usually only VTK_LUMINANCE, VTK_RGB, and VTK_RGBA are supported.
  void SetOutputFormat(int format);

  // Description:
  // Set the clip rectangle for the frames.  The video will be clipped 
  // before it is copied into the framebuffer.  Changing the ClipRegion
  // will destroy the current contents of the framebuffer.
  // *** Clip region is modified to multiples of 4! ***
  // The default ClipRegion is (0,1648,0,1450,0,1).
  void SetClipRegion(int x0, int x1, int y0, int y1, int z0, int z1);

  // Description:
  // experimental... not used yet.
  void Pause();
  void UnPause();

  // Description:
  // For internal use only
  void InternalGrab();
  
protected:
  vtkEpiphanVideoSource();
  ~vtkEpiphanVideoSource();

  // status of the frame grabber
  int status;

  // pointer to the grabber
  FrmGrabber* fg;

  // crop rectangle for the grabber
  V2URect * cropRect;

  // serial number of the frame grabber
  char serialNumber[15];

  // pause the recording feed ... experimental and not used.
  int pauseFeed;
};

#endif
