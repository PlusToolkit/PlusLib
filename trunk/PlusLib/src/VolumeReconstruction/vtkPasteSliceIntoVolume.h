/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

/*=========================================================================
The following copyright notice is applicable to parts of this file:

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

#ifndef __vtkPasteSliceIntoVolume_h
#define __vtkPasteSliceIntoVolume_h

class TrackedFrame;
class vtkImageData;
class vtkMatrix4x4;
class vtkXMLDataElement;
class vtkMultiThreader;

/*!
  \class vtkPasteSliceIntoVolume
  \brief Paste a single slice into a volume to reconstruct a single image volume from multiple image slices

  This filter incrementally compound ultrasound images into a
  reconstruction volume, given a transform which specifies the location of
  each ultrasound slice. An alpha component is appended to the output to
  specify the coverage of each pixel in the output volume (i.e. whether or
  not a voxel has been touched by the reconstruction)

  The output reconstructed volume may contain holes (empty voxels between images slices).
  The vtkFillHolesInVolume filter can be used for post-processing the data to fill holes with
  values similar to nearby voxels.

  \sa vtkFillHolesInVolume
  \ingroup PlusLibVolumeReconstruction
*/
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

  enum CalculationType
  {
    WEIGHTED_AVERAGE,
    MAXIMUM
  };

  static vtkPasteSliceIntoVolume *New();
  vtkTypeRevisionMacro(vtkPasteSliceIntoVolume, vtkObject);
  virtual void PrintSelf(ostream& os, vtkIndent indent);

  /*!
    Set spacing of the output data in Reference coordinate system.
    This is required to be set, otherwise the reconstructed volume will be empty.
  */
  vtkSetVector3Macro(OutputSpacing, vtkFloatingPointType);
  /*! Get spacing of the output data in Reference coordinate system  */
  vtkGetVector3Macro(OutputSpacing, vtkFloatingPointType);

  /*!
    Set origin of the output data in Reference coordinate system.
    This is required to be set, otherwise the reconstructed volume will be empty.
  */
  vtkSetVector3Macro(OutputOrigin, vtkFloatingPointType);
  /*! Get origin of the output data in Reference coordinate system  */
  vtkGetVector3Macro(OutputOrigin, vtkFloatingPointType);

  /*!
    Set extent of the output data in Reference coordinate system.
    This is required to be set, otherwise the reconstructed volume will be empty.
  */
  vtkSetVector6Macro(OutputExtent, int);
  /*! Get extentof the output data in Reference coordinate system  */
  vtkGetVector6Macro(OutputExtent, int);  

  /*!
    Insert the slice into the reconstructed volume
    The origin of the image is at the first pixel stored in the memory.
    The extent, origin, and spacing of the output must be defined before calling this method.
  */
  virtual PlusStatus InsertSlice(vtkImageData *image, vtkMatrix4x4* mImageToReference);

  /*!
    Get the output reconstructed 3D ultrasound volume
    (the output is the reconstruction volume, the second component
    is the alpha component that stores whether or not a voxel has
    been touched by the reconstruction)
  */
  virtual vtkImageData *GetReconstructedVolume();

  /*!
    Get the accumulation buffer
    accumulation buffer is for compounding, there is a voxel in
    the accumulation buffer for each voxel in the output.
    Will be NULL if we are not compounding.
  */
  virtual vtkImageData *GetAccumulationBuffer();

  /*! Creates the and clears all necessary image buffers */
  virtual PlusStatus ResetOutput();

  /*!
    Set the clip rectangle origin to apply to the image in pixel coordinates.
    Pixels outside the clip rectangle will not be pasted into the volume.
    The origin of the rectangle is at its corner that is closest to the image origin.
  */
  vtkSetVector2Macro(ClipRectangleOrigin,int);
  /*!
    Get the clip rectangle origin to apply to the image in pixel coordinates.
  */
  vtkGetVector2Macro(ClipRectangleOrigin,int);

  /*! Set the clip rectangle size in pixels */
  vtkSetVector2Macro(ClipRectangleSize,int);
  /*! Get the clip rectangle size in pixels */
  vtkGetVector2Macro(ClipRectangleSize,int);

  /*!
    Set fan-shaped clipping region for curvilinear probes.
    The origin of the fan is defined in the image coordinate system, in mm.
  */
  vtkSetVector2Macro(FanOrigin,double);
  /*! Get fan-shaped clipping region for curvilinear probes */
  vtkGetVector2Macro(FanOrigin,double);

  /*!
    Set the fan-shaped clipping region for curvilinear probes.
    Fan angles is a vector containing the angles of the two straight edge of the fan, in degrees.
    If both angles are 0 then no fan-shaped clipping is performed.
  */
  vtkSetVector2Macro(FanAngles,double);
  /*! Set the fan-shaped clipping region for curvilinear probes */
  vtkGetVector2Macro(FanAngles,double);
  
  /*!
    Set the fan-shaped clipping region for curvilinear probes.
    Fan depth is the radius of the fan, in mm.
  */
  vtkSetMacro(FanDepth,double);
  /*! Get the fan-shaped clipping region for curvilinear probes */
  vtkGetMacro(FanDepth,double);

  /*! 
    Returns true if fan-shaped clipping is applied (true, if any of the
    fan angles are non-zero).
  */
  bool FanClippingApplied();

  /*!
    Set optimization method (turn off optimization only if it is not stable
    on your architecture).
    NO_OPTIMIZATION: means no optimization (almost never used) 
    PARTIAL_OPTIMIZATION: break transformation into x, y and z components, and
      don't do bounds checking for nearest-neighbor interpolation
    FULL_OPTIMIZATION: fixed-point (i.e. integer) math is used instead of float math,
      it is only useful with NEAREST_NEIGHBOR interpolation
      (when used with LINEAR interpolation then it is slower than NO_OPTIMIZATION)
  */
  vtkSetMacro(Optimization,OptimizationType);
  /*! Get the current optimization method */
  vtkGetMacro(Optimization,OptimizationType);
  /*! Get the name of an optimization method from a type id */
  char* GetOptimizationModeAsString(OptimizationType type);

  /*!
    Set the interpolation mode
    LINEAR: better image quality, slower
    NEAREST_NEIGHBOR: lower image quality, faster (default)
  */
  vtkSetMacro(InterpolationMode,InterpolationType);
  /*! Get the interpolation mode */
  vtkGetMacro(InterpolationMode,InterpolationType);
  /*! Get the name of an interpolation mode from a type id */
  char *GetInterpolationModeAsString(InterpolationType interpEnum);

  /*!
    Set the result mode
    WEIGHTED_AVERAGE: Used on single sweeps when slices are not expected to intersect
    MAXIMUM: used when multiple slices are expected to intersect
  */
  vtkSetMacro(CalculationMode,CalculationType);
  /*! Get the result mode */
  vtkGetMacro(CalculationMode,CalculationType);
  /*! Get the name of a result mode from a type id */
  char *GetCalculationModeAsString(CalculationType calculationEnum);

  /*!
    Turn on or off the compounding (default on, which means
    that scans will be averaged where they overlap instead of just considering
    the last acquired slice).
  */
  virtual void SetCompounding(int c);
  /*! Get current compounding setting */
  vtkGetMacro(Compounding,int);
  
  /*!
    Set number of threads used for processing the data.
    The reconstruction result is slightly different if more than one thread is used
    because due to interpolation and rounding errors is influenced by the order the pixels
    are processed.
    Choose 0 (this is the default) for maximum speed, in this case the default number of
    used threads equals the number of processors. Choose 1 for reproducible results.
  */
  vtkSetMacro(NumberOfThreads,int);
  /*! Get number of threads used for processing the data */
  vtkGetMacro(NumberOfThreads,int);

protected:
  vtkPasteSliceIntoVolume();
  ~vtkPasteSliceIntoVolume();

  /*! Thread function that actually performs the pasting of frame pixels into the volume */
  static VTK_THREAD_RETURN_TYPE InsertSliceThreadFunction( void *arg );
  
  /*!
    To split the extent over many threads
    Input: the full extent (fullExt), current thread index (threadId), total number of threads (requestedNumberOfThreads)
    Output: the split extent for the selected thread, number of threads to be used (return value)
  */
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
  CalculationType CalculationMode;
  int Compounding;

  // Multithreading
  vtkMultiThreader *Threader;
  int NumberOfThreads;
  
private:
  vtkPasteSliceIntoVolume(const vtkPasteSliceIntoVolume&);
  void operator=(const vtkPasteSliceIntoVolume&);
};

#endif
