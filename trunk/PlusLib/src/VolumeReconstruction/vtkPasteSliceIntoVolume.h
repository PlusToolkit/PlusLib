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
// .NAME vtkPasteSliceIntoVolume - paste image slice(s) into a volume
// .SECTION Description
// This filter incrementally compound ultrasound images into a
// reconstruction volume, given a transform which specifies the location of
// each ultrasound slice. An alpha component is appended to the output to
// specify the coverage of each pixel in the output volume (i.e. whether or
// not a voxel has been touched by the reconstruction)
// .SECTION see also
// vtkVolumeReconstructor

#ifndef __vtkPasteSliceIntoVolume_h
#define __vtkPasteSliceIntoVolume_h

class TrackedFrame;
class vtkImageData;
class vtkMatrix4x4;
class vtkXMLDataElement;
class vtkMultiThreader;

class VTK_EXPORT vtkPasteSliceIntoVolume : public vtkObject
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
    FULL_OPTIMIZATION
  };

  static vtkPasteSliceIntoVolume *New();
  vtkTypeRevisionMacro(vtkPasteSliceIntoVolume, vtkObject);
  virtual void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Spacing, origin, and extent of output data in Reference coordinate system.
  // This is required to be set, otherwise the reconstructed volume will be empty.
  vtkSetVector3Macro(OutputSpacing, vtkFloatingPointType);
  vtkGetVector3Macro(OutputSpacing, vtkFloatingPointType);
  vtkSetVector3Macro(OutputOrigin, vtkFloatingPointType);
  vtkGetVector3Macro(OutputOrigin, vtkFloatingPointType);
  vtkSetVector6Macro(OutputExtent, int);
  vtkGetVector6Macro(OutputExtent, int);  

  // Description:
  // Insert the slice into the reconstructed volume
  // The origin of the image is at the first pixel stored in the memory (follows the ITK convention).
  // Note that this is origin not the one that is used by VTK when displaying a 2D image on screen
  // (VTK normally shows the first pixel of the last image line at the origin).
  virtual PlusStatus InsertSlice(vtkImageData *image, vtkMatrix4x4* mImageToReference);

  // Description:
  // Get the output reconstructed 3D ultrasound volume
  // (the output is the reconstruction volume, the second component
  // is the alpha component that stores whether or not a voxel has
  // been touched by the reconstruction)
  virtual vtkImageData *GetReconstructedVolume();

  // Description:
  // Get the accumulation buffer
  // accumulation buffer is for compounding, there is a voxel in
  // the accumulation buffer for each voxel in the output.
  // Will be NULL if we are not compounding.
  virtual vtkImageData *GetAccumulationBuffer();

  // Description:
  // Creates the and clears all necessary image buffers.
  virtual PlusStatus ResetOutput();

  // Description:
  // Set the clip rectangle origin to apply to the image in pixel coordinates.
  // Pixels outside the clip rectangle will not be pasted into the volume.
  // The origin of the rectangle is at its corner that is closest to the image origin.
  vtkSetVector2Macro(ClipRectangleOrigin,int);
  vtkGetVector2Macro(ClipRectangleOrigin,int);

  // Description:
  // Set the clip rectangle size in pixels.
  vtkSetVector2Macro(ClipRectangleSize,int);
  vtkGetVector2Macro(ClipRectangleSize,int);

  // Description:
  // Set the fan-shaped clipping region for curvilinear probes.
  // The origin of the fan is defined in the image coordinate system, in mm.
  vtkSetVector2Macro(FanOrigin,double);
  vtkGetVector2Macro(FanOrigin,double);

  // Description:
  // Set the fan-shaped clipping region for curvilinear probes.
  // Fan angles is a vector containing the angles of the two straight edge of the fan, in degrees.
  // If both angles are 0 then no fan-shaped clipping is performed.
  vtkSetVector2Macro(FanAngles,double);
  vtkGetVector2Macro(FanAngles,double);
  
  // Description:
  // Set the fan-shaped clipping region for curvilinear probes.
  // Fan depth is the radius of the fan, in mm.
  vtkSetMacro(FanDepth,double);
  vtkGetMacro(FanDepth,double);

  // Description:
  // Returns true if fan-shaped clipping is applied (true, if any of the
  // fan angles are non-zero).
  bool FanClippingApplied();

  // Description:
  // Turn on and off optimizations (default on, turn them off only if
  // they are not stable on your architecture).
  // NO_OPTIMIZATION: means no optimization (almost never used) 
  // PARTIAL_OPTIMIZATION: break transformation into x, y and z components, and
  //   don't do bounds checking for nearest-neighbor interpolation
  // FULL_OPTIMIZATION: fixed-point (i.e. integer) math is used instead of float math,
  //   it is only useful with NEAREST_NEIGHBOR interpolation
  //  (when used with LINEAR interpolation then it is slower than NO_OPTIMIZATION)
  vtkSetMacro(Optimization,OptimizationType);
  vtkGetMacro(Optimization,OptimizationType);
  char* GetOptimizationModeAsString(OptimizationType type);

  // Description:
  // Set/Get the interpolation mode
  // LINEAR: better image quality, slower
  // NEAREST_NEIGHBOR: lower image quality, faster (default)
  vtkSetMacro(InterpolationMode,InterpolationType);
  vtkGetMacro(InterpolationMode,InterpolationType);
  char *GetInterpolationModeAsString(InterpolationType interpEnum);

  // Description:
  // Turn on or off the compounding (default on, which means
  // that scans will be averaged where they overlap instead of just considering
  // the last acquired slice).
  vtkGetMacro(Compounding,int);
  virtual void SetCompounding(int c);

  // Description:
  // Number of threads use for processing the data.
  // The reconstruction result is slightly different if more than one thread is used
  // because due to interpolation and rounding errors is influenced by the order the pixels
  // are processed.
  // Choose 0 (this is the default) for maximum speed, in this case the default number of
  // used threads equals the number of processors. Choose 1 for reproducible results.
  vtkGetMacro(NumberOfThreads,int);
  vtkSetMacro(NumberOfThreads,int);

protected:
  vtkPasteSliceIntoVolume();
  ~vtkPasteSliceIntoVolume();

  // Description:
  // Thread function that actually performs the pasting of frame pixels into the volume
  static VTK_THREAD_RETURN_TYPE InsertSliceThreadFunction( void *arg );
  
  // Description:
  // To split the extent over many threads
  // Input: the full extent (fullExt), current thread index (threadId), total number of threads (requestedNumberOfThreads)
  // Output: the split extent for the selected thread, number of threads to be used (return value)
  static int SplitSliceExtent(int splitExt[6], int fullExt[6], int threadId, int requestedNumberOfThreads);

  vtkImageData *ReconstructedVolume;
  vtkImageData *AccumulationBuffer;

  // Output image position and size
  vtkFloatingPointType OutputOrigin[3];
  vtkFloatingPointType OutputSpacing[3];
  int OutputExtent[6];
  
  // Clipping parameters
  int ClipRectangleOrigin[2];
  int ClipRectangleSize[2];
  double FanAngles[2];
  double FanOrigin[2];
  double FanDepth;

  // Reconstruction options
  InterpolationType InterpolationMode;
  OptimizationType Optimization;
  int Compounding;

  // Multithreading
  vtkMultiThreader *Threader;
  int NumberOfThreads;
  
private:
  vtkPasteSliceIntoVolume(const vtkPasteSliceIntoVolume&);
  void operator=(const vtkPasteSliceIntoVolume&);
};

#endif
