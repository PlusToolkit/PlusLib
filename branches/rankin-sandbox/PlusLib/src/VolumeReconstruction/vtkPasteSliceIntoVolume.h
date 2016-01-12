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

#include "vtkVolumeReconstructionExport.h"

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
class vtkVolumeReconstructionExport vtkPasteSliceIntoVolume : public vtkObject
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

  enum CompoundingType
  {
    UNDEFINED_COMPOUNDING_MODE,
    LATEST_COMPOUNDING_MODE,
    MAXIMUM_COMPOUNDING_MODE,
    MEAN_COMPOUNDING_MODE
  };

  enum CalculationTypeDeprecated
  {
    UNDEFINED_CALCULATION,
    WEIGHTED_AVERAGE_CALCULATION,
    MAXIMUM_CALCULATION
  };

  static vtkPasteSliceIntoVolume *New();
  vtkTypeMacro(vtkPasteSliceIntoVolume, vtkObject);
  virtual void PrintSelf(ostream& os, vtkIndent indent);

  /*!
    Set spacing of the output data in Reference coordinate system.
    This is required to be set, otherwise the reconstructed volume will be empty.
  */
  vtkSetVector3Macro(OutputSpacing, double);
  /*! Get spacing of the output data in Reference coordinate system  */
  vtkGetVector3Macro(OutputSpacing, double);

  /*!
    Set origin of the output data in Reference coordinate system.
    This is required to be set, otherwise the reconstructed volume will be empty.
  */
  vtkSetVector3Macro(OutputOrigin, double);
  /*! Get origin of the output data in Reference coordinate system  */
  vtkGetVector3Macro(OutputOrigin, double);

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
    The origin of the fan is defined in the image coordinate system, in the input image physical coordinate system.
  */
  vtkSetVector2Macro(FanOrigin,double);
  /*! Get fan-shaped clipping region for curvilinear probes */
  vtkGetVector2Macro(FanOrigin,double);

  /*!
    Set the fan-shaped clipping region for curvilinear probes.
    Fan angles is a vector containing the angles of the two straight edge of the fan, in degrees.
    If both angles are 0 then no fan-shaped clipping is performed.
  */
  vtkSetVector2Macro(FanAnglesDeg,double);
  /*! Set the fan-shaped clipping region for curvilinear probes */
  vtkGetVector2Macro(FanAnglesDeg,double);
  
  /*!
    Set the fan-shaped clipping region for curvilinear probes.
    Fan radius start is the minimum depth, in the input image physical coordinate system.
  */
  vtkSetMacro(FanRadiusStart,double);
  /*!
    Get the fan-shaped clipping region for curvilinear probes.
    Fan radius start is the minimum depth, in the input image physical coordinate system.
  */
  vtkGetMacro(FanRadiusStart,double);

  /*!
    Set the fan-shaped clipping region for curvilinear probes.
    Fan radius stopt is the maximum depth, in the input image physical coordinate system.
  */
  vtkSetMacro(FanRadiusStop,double);
  /*!
    Get the fan-shaped clipping region for curvilinear probes.
    Fan radius start is the maximum depth, in the input image physical coordinate system.
  */
  vtkGetMacro(FanRadiusStop,double);

  /*! 
    Returns true if fan-shaped clipping is applied (true, if any of the
    fan angles are non-zero).
  */
  bool FanClippingApplied();

  /*! The output scalar type can be of any supported VTK_[TYPE]. These are:
    VTK_CHAR
    VTK_UNSIGNED_CHAR
    VTK_SHORT
    VTK_UNSIGNED_SHORT
    VTK_INT
    VTK_UNSIGNED_INT
    VTK_LONG
    VTK_UNSIGNED_LONG
    VTK_FLOAT
    VTK_DOUBLE*/
  vtkSetMacro(OutputScalarMode,int);
  /*! Get the output data type */
  vtkGetMacro(OutputScalarMode,int);
  /*! Get the output data type from an id */
  const char *GetOutputScalarModeAsString(int type);

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
  const char* GetOptimizationModeAsString(OptimizationType type);

  /*!
    Set the interpolation mode
    LINEAR:           Each pixel is distributed into the surrounding eight voxels using trilinear interpolation weights.
                      Resists noise, but is slower and may introduce blurring.
    NEAREST_NEIGHBOR: Each pixel is inserted only into the spatially nearest voxel.
                      Faster, but is susceptible to noise. (default)
  */
  vtkSetMacro(InterpolationMode,InterpolationType);
  /*! Get the interpolation mode */
  vtkGetMacro(InterpolationMode,InterpolationType);
  /*! Get the name of an interpolation mode from a type id */
  const char *GetInterpolationModeAsString(InterpolationType interpEnum);

  /*!
    Set the compounding mode
    MEAN:     For each voxel, use an average of all inserted pixel values. Used on single or multiple sweeps 
              from the same angle (regardless of intersection). Resistant to noise, but slower than other 
              compounding methods.
    LATEST:   For each voxel, use only the latest inserted pixel value. Used on single or multiple sweeps 
              from the same angle (regardless of intersection). Fast, but susceptible to noise.
    MAXIMUM:  For each voxel, use only the pixel value with the highest intensity. Used when multiple slices 
              from different angles are expected to intersect. Fast, but susceptible to noise.
  */
  vtkSetMacro(CompoundingMode,CompoundingType);
  /*! Get the result mode */
  vtkGetMacro(CompoundingMode,CompoundingType);
  /*! Get the name of a result mode from a type id */
  const char *GetCompoundingModeAsString(CompoundingType compoundingEnum);
  
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

  /*! DEPRECATED - use CompoundingMode instead! */
  vtkSetMacro(Compounding,int);
  /*! DEPRECATED - use CompoundingMode instead! */
  vtkGetMacro(Compounding,int);

  /*! DEPRECATED - use CompoundingMode instead! */
  vtkSetMacro(Calculation,CalculationTypeDeprecated);
  /*! DEPRECATED - use CompoundingMode instead! */
  vtkGetMacro(Calculation,CalculationTypeDeprecated);
  /*! DEPRECATED - use CompoundingMode instead! */
  const char *GetCalculationAsString(CalculationTypeDeprecated calcEnum);

  /*!
    Pixels that have lower brightness value than this threshold value will not be inserted into the volume.
    If value is NO_PIXEL_REJECTION then no pixels will be rejected based on intensity value (slightly faster
    than setting a value that is lower than all pixel values).
  */
  vtkSetMacro(PixelRejectionThreshold, double);
  vtkGetMacro(PixelRejectionThreshold, double);
  bool IsPixelRejectionEnabled();
  void SetPixelRejectionDisabled();
  
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
  double OutputOrigin[3];
  double OutputSpacing[3];
  int OutputExtent[6];
  
  // Clipping parameters
  int ClipRectangleOrigin[2];
  int ClipRectangleSize[2];
  double FanAnglesDeg[2];
  double FanOrigin[2]; // in the input image coordinate system (in physical coordinates; but Plus always uses 1.0 spacing, so these are effectively in pixels)
  double FanRadiusStart; // in the input image coordinate system (in physical coordinates; but Plus always uses 1.0 spacing, so the value is effectively in pixels)
  double FanRadiusStop; // in the input image coordinate system (in physical coordinates; but Plus always uses 1.0 spacing, so the value effectively in pixels)

  // Reconstruction options
  InterpolationType InterpolationMode;
  OptimizationType Optimization;
  CompoundingType CompoundingMode;
  int OutputScalarMode;
  // deprecated
  int Compounding;
  CalculationTypeDeprecated Calculation;

  // Multithreading
  vtkMultiThreader *Threader;
  int NumberOfThreads;
  
  double PixelRejectionThreshold;
  
private:
  vtkPasteSliceIntoVolume(const vtkPasteSliceIntoVolume&);
  void operator=(const vtkPasteSliceIntoVolume&);

  bool EnableAccumulationBufferOverflowWarning; // TODO: Remove this and all references once we've figured out how to handle this problem
};

#endif
