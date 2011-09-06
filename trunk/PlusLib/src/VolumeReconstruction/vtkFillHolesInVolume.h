/*=========================================================================
Copyright (c) 2000-2007 Atamai, Inc.
Copyright (c) 2008-2009 Danielle Pace

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

// .NAME vtkFillHolesInVolume - Fill holes in a volume reconstructed from slices.
// .SECTION Description
// This class has not been tested extensively and it may not work properly!
// Fill holes in the output by using the weighted average of the
// surrounding voxels.  If Compounding is off, then all hit voxels
// are weighted equally. 
// These papers could be useful to study when reviewing/updating the algorithm:
//  http://www.irma-international.org/viewtitle/46058/
//  http://www.3dmed.net/paper/daiyakang_TITB.pdf
// .SECTION See Also
// vtkPasteSliceIntoVolume

#ifndef __vtkFillHolesInVolume_h
#define __vtkFillHolesInVolume_h

#include "vtkThreadedImageAlgorithm.h"

class VTK_EXPORT vtkFillHolesInVolume : public vtkThreadedImageAlgorithm
{
public:
  static vtkFillHolesInVolume *New();
  vtkTypeMacro(vtkFillHolesInVolume,vtkThreadedImageAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);
    
  // Description:
  // Turn on or off the compounding (default on, which means
  // that scans will be compounded where they overlap instead of just considering
  // the last acquired slice.
  vtkGetMacro(Compounding,int);
  vtkSetMacro(Compounding,int);

  void SetReconstructedVolume(vtkImageData *reconstructedVolume);

  void SetAccumulationBuffer(vtkImageData *accumulationBuffer);

protected:
  vtkFillHolesInVolume();
  ~vtkFillHolesInVolume() {};

  virtual int RequestInformation (vtkInformation*,
                                  vtkInformationVector**,
                                  vtkInformationVector*);
  virtual int RequestUpdateExtent(vtkInformation*,
                                  vtkInformationVector**,
                                  vtkInformationVector*);

  virtual void ThreadedRequestData(vtkInformation *request, 
    vtkInformationVector **inputVector, 
    vtkInformationVector *outputVector,
    vtkImageData ***inData, 
    vtkImageData **outData,
    int extent[6], int threadId);

  static VTK_THREAD_RETURN_TYPE FillHoleThreadFunction( void *arg );

  int Compounding;

private:
  vtkFillHolesInVolume(const vtkFillHolesInVolume&);  // Not implemented.
  void operator=(const vtkFillHolesInVolume&);  // Not implemented.
};

#endif



