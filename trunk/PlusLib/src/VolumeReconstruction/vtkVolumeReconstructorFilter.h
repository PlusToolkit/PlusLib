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
// .NAME vtkVolumeReconstructorFilter - real-time freehand ultrasound reconstruction
// .SECTION Description
// This filter incrementally compound ultrasound images into a
// reconstruction volume, given a transform which specifies the location of
// each ultrasound slice.  An alpha component is appended to the output to
// specify the coverage of each pixel in the output volume (i.e. whether or
// not a voxel has been touched by the reconstruction)
// .SECTION see also
// 

#ifndef __vtkVolumeReconstructorFilter_h
#define __vtkVolumeReconstructorFilter_h

#include "vtkFreehandUltrasound2.h"

class TrackedFrame;

class VTK_EXPORT vtkVolumeReconstructorFilter : public vtkObject
{

public:
  enum InterpolationType
  {
    NEAREST_NEIGHBOR_INTERPOLATION,
    LINEAR_INTERPOLATION
  };

  enum OptimizationType
  {
    NO_OPTIMIZATION,
    PARTIAL_OPTIMIZATION,
    FULL_OPTIMIZATION // fixed point computation
  };

  static vtkVolumeReconstructorFilter *New();
  vtkTypeRevisionMacro(vtkVolumeReconstructorFilter, vtkObject);
  virtual void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Get the output reconstructed 3D ultrasound volume
  // (the output is the reconstruction volume, the second component
  // is the alpha component that stores whether or not a voxel has
  // been touched by the reconstruction)
  virtual vtkImageData *GetReconstructedVolume();

  // Description:
  // Get the accumulation buffer
  // accumulation buffer is for compounding, there is a voxel in
  // the accumulation buffer for each voxel in the output
  virtual vtkImageData *GetAccumulationBuffer();

  // Description:
  // Fill holes in the output by using the weighted average of the
  // surrounding voxels.  If Compounding is off, then all hit voxels
  // are weighted equally. 
  virtual void FillHolesInOutput();

  // Description:
  // Cause the slice to be inserted into the first reconstruction volume
  virtual PlusStatus InsertSlice(vtkImageData *image, vtkMatrix4x4* mImageToReference);

  // Description:
  // Creates the and clears all necessary image buffers.
  virtual PlusStatus ResetOutput();

  // Description:
  // Set the clip rectangle origin to apply to the image in pixel coordinates. 
  // The origin of the rectangle is the corner that is closest to the image origin
  // (has the smallest x and y coordinate values).
  vtkSetVector2Macro(ClipRectangleOrigin,double);
  vtkGetVector2Macro(ClipRectangleOrigin,double);

  // Description:
  // Set the clip rectangle size in pixels. 
  vtkSetVector2Macro(ClipRectangleSize,double);
  vtkGetVector2Macro(ClipRectangleSize,double);

  // Description:
  // If the ultrasound probe collects a fan of data, specify the position and
  // dimensions of the fan in mm.
  vtkSetVector2Macro(FanAngles,double);
  vtkGetVector2Macro(FanAngles,double);
  vtkSetVector2Macro(FanOrigin,double);
  vtkGetVector2Macro(FanOrigin,double);
  vtkSetMacro(FanDepth,double);
  vtkGetMacro(FanDepth,double);

  // Description:
  // Turn on and off optimizations (default on, turn them off only if
  // they are not stable on your architecture).
  //   0 means no optimization (almost never used)
  //   1 means break transformation into x, y and z components, and
  //      don't do bounds checking for nearest-neighbor interpolation
  //   2 means used fixed-point (i.e. integer) math instead of float math
  vtkSetMacro(Optimization,OptimizationType);
  vtkGetMacro(Optimization,OptimizationType);
  char* GetOptimizationModeAsString(OptimizationType type);

  // Description:
  // Set/Get the interpolation mode, default is nearest neighbor. 
  vtkSetMacro(InterpolationMode,InterpolationType);
  vtkGetMacro(InterpolationMode,InterpolationType);
  char *GetInterpolationModeAsString(InterpolationType interpEnum);

  // Description:
  // Turn on or off the compounding (default on, which means
  // that scans will be compounded where they overlap instead of the
  vtkGetMacro(Compounding,int);
  virtual void SetCompounding(int c);

  // Description:
  // Spacing, origin, and extent of output data
  // You MUST set this information.
  vtkSetVector3Macro(OutputSpacing, vtkFloatingPointType);
  vtkGetVector3Macro(OutputSpacing, vtkFloatingPointType);
  vtkSetVector3Macro(OutputOrigin, vtkFloatingPointType);
  vtkGetVector3Macro(OutputOrigin, vtkFloatingPointType);
  vtkSetVector6Macro(OutputExtent, int);
  vtkGetVector6Macro(OutputExtent, int);

  // Description:
  // Configure the freehand ultrasound reconstruction according to summary XML data
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* aConfig);

  // Description:
  // Returns an XMLDataElement describing the freehand object
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement *elem);

  //////////////////////////////////////////////////////////////////////////////////

protected:
  vtkVolumeReconstructorFilter();
  ~vtkVolumeReconstructorFilter();

  bool FanParametersDefined();

  static VTK_THREAD_RETURN_TYPE InsertSliceThreadFunction( void *arg );
  static VTK_THREAD_RETURN_TYPE FillHoleThreadFunction( void *arg );

  // Description:
  // To split the extent over many threads
  // Input: the full extent (fullExt), current thread index (threadId), total number of threads (requestedNumberOfThreads)
  // Output: the split extent for the selected thread, number of threads to be used (return value)
  static int SplitSliceExtent(int splitExt[6], int fullExt[6], int threadId, int requestedNumberOfThreads);

  vtkImageData *ReconstructedVolume;
  vtkImageData *AccumulationBuffer;

  // parameters for fan/image
  vtkFloatingPointType OutputOrigin[3];
  vtkFloatingPointType OutputSpacing[3];
  int OutputExtent[6];
  
  double ClipRectangleOrigin[2];
  double ClipRectangleSize[2];
  double FanAngles[2];
  double FanOrigin[2];
  double FanDepth;

  // reconstruction options
  InterpolationType InterpolationMode;
  OptimizationType Optimization;
  int Compounding;

  // This can be used to limit the number of threads used for the reconstruction
  int NumberOfThreads;

  vtkMultiThreader *Threader;
  
private:
  vtkVolumeReconstructorFilter(const vtkVolumeReconstructorFilter&);
  void operator=(const vtkVolumeReconstructorFilter&);
};

#endif
