/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkFreehandUltrasound2.h,v $
  Language:  C++
  Date:      $Date: 2009/07/20 18:48:07 $
  Version:   $Revision: 1.40 $
  Thanks:    Thanks to David G. Gobbi who developed this class. 
  Thanks:    Thanks to Danielle Pace who developed this class.

==========================================================================

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
// .NAME vtkFreehandUltrasound2 - real-time freehand ultrasound reconstruction
// .SECTION Description
// vtkFreehandUltrasound2 will incrementally compound ultrasound images into a
// reconstruction volume, given a transform which specifies the location of
// each ultrasound slice.  An alpha component is appended to the output to
// specify the coverage of each pixel in the output volume (i.e. whether or
// not a voxel has been touched by the reconstruction)
// .SECTION see also
// vtkVideoSource2, vtkTracker, vtkTrackerTool


#ifndef __vtkFreehandUltrasound2_h
#define __vtkFreehandUltrasound2_h

#include "PlusConfigure.h"
#include "vtkImageAlgorithm.h"
#include "vtkTimestampedCircularBuffer.h"

class vtkLinearTransform;
class vtkMatrix4x4;
class vtkMultiThreader;
class vtkVideoSource2;
class vtkTrackerTool;
class vtkTrackerBuffer;
class vtkCriticalSection;
class vtkImageData;
class vtkImageThreshold;
class vtkImageClip;
class vtkTransform;
class vtkXMLDataElement;

#define VTK_FREEHAND_NEAREST 0
#define VTK_FREEHAND_LINEAR 1

class VTK_EXPORT vtkFreehandUltrasound2 : public vtkImageAlgorithm
{
public:
  
  static vtkFreehandUltrasound2 *New();
  vtkTypeRevisionMacro(vtkFreehandUltrasound2, vtkImageAlgorithm);
  virtual void PrintSelf(ostream& os, vtkIndent indent);

  // Description: 
  // Set/Get the 2D image slice to insert into the reconstruction volume
  // the slice is the vtkImageData 'slice' (kind of like an input)
  // that is inserted into the reconstructed 3D volume (the output)
  virtual void SetSlice(vtkImageData *);
  virtual vtkImageData* GetSlice();

  // Description:
  // Get the accumulation buffer
  // accumulation buffer is for compounding, there is a voxel in
  // the accumulation buffer for each voxel in the output
  virtual vtkImageData *GetAccumulationBuffer();

  // Description:
  // Get the output reconstructed 3D ultrasound volume
  // (the output is the reconstruction volume, the second component
  // is the alpha component that stores whether or not a voxel has
  // been touched by the reconstruction)
  virtual vtkImageData *GetOutput();

  // Description:
  // Set/Get the video source to input the slices from
  virtual void SetVideoSource(vtkVideoSource2 *);
  vtkGetObjectMacro(VideoSource,vtkVideoSource2);

  // Description:
  // Set/Get the tracker tool to input transforms from
  virtual void SetTrackerTool(vtkTrackerTool *);
  vtkGetObjectMacro(TrackerTool,vtkTrackerTool);

  // Description:
  // Start doing a reconstruction from the video frames stored
  // in the VideoSource buffer.  You should first use 'Seek'
  // on the VideoSource to rewind first.  Then the reconstruction
  // will advance through n frames one by one until the
  // reconstruction is complete.  The reconstruction
  // is performed in the background.
  // TODO NOT currently implemented for dynamic(gated) reconstruction
  // and not tested for non-gated reconstruction: use
  // StartRealTimeReconstruction instead
  virtual PlusStatus StartReconstruction(int n);
  
  // Description:
  // Stop the reconstruction.  The number of frames remaining to
  // be reconstructed is returned.
  virtual int StopReconstruction();

  // Description:
  // Start doing real-time reconstruction from the video source.
  // This will spawn a thread that does the reconstruction in the
  // background.
  virtual void StartRealTimeReconstruction();
  
  // Description:
  // Stop the real-time reconstruction.
  virtual void StopRealTimeReconstruction();

  // Description:
  // Get the reconstruction rate.
  double GetReconstructionRate() { return this->ReconstructionRate; };

  // Description:
  // Fill holes in the output by using the weighted average of the
  // surrounding voxels.  If Compounding is off, then all hit voxels
  // are weighted equally. 
  virtual void FillHolesInOutput();

  // Description:
  // Print out the summary file for the scan
  virtual void SaveSummaryFile(const char *directory);

  // Description:
  // Configure the freehand ultrasound reconstruction according to a summary file
  virtual PlusStatus ReadSummaryFile(const char *filename);

  // Description:
  // Set the time by which the video lags behind the tracking information,
  // in seconds.  This value may be negative.  Default: 0.
  vtkSetMacro(VideoLag,double);
  vtkGetMacro(VideoLag,double);

  // Description:
  // Cause the slice to be inserted into the first reconstruction volume
  virtual void InsertSlice(vtkImageData* outData, vtkImageData* accData);

  // Description:
  // Insert the slice into the ith volume, with optimization
  virtual void OptimizedInsertSlice(vtkImageData* outData, vtkImageData* accData);

  // Description:
  // Clear the data volume.
  virtual void ClearOutput();

  // Description:
  // Set the clip rectangle (x0,y0,x1,y1) to apply to the image. 
  // Specify the rectange in millimeter coords, not pixel indices.
  // TODO not currently implemented
  vtkSetVector4Macro(ClipRectangle,double);
  vtkGetVector4Macro(ClipRectangle,double);

  // Description:
  // Get the clip rectangle as an extent, given a specific origin
  // spacing, and max possible extent.
  void GetClipExtent(int clipExtent[6],
         vtkFloatingPointType origin[3],
         vtkFloatingPointType spacing[3],
         const int extent[6]);

  // Description:
  // If the ultrasound probe collects a fan of data, specify the position and
  // dimensions of the fan.
  vtkSetVector2Macro(FanAngles,double);
  vtkGetVector2Macro(FanAngles,double);
  vtkSetVector2Macro(FanOrigin,double);
  vtkGetVector2Macro(FanOrigin,double);
  vtkSetMacro(FanDepth,double);
  vtkGetMacro(FanDepth,double);

  // Description:
  // Set the axes of the slice to insert into the reconstruction volume,
  // relative the (x,y,z) axes of the reconstruction volume itself.
  // The axes are extracted from the 4x4 matrix:  The x-axis is the 
  // first column, the y-axis is the second column, the z-axis is the 
  // third column, and the origin is the final column.  The bottom
  // row of the matrix should always be (0,0,0,1).
  // If you don't set the axes, the axes will default to 
  // (1,0,0), (0,1,0), (0,0,1) and their origin will be (0,0,0)
  virtual void SetSliceAxes(vtkMatrix4x4 *);
  vtkGetObjectMacro(SliceAxes,vtkMatrix4x4);

  // Description:
  // Set a transform to be applied to the SliceAxes.
  // If you don't set this, it will be treated as the identity transform.
  // If you are using a multiplanar probe, then the rotation will be
  // incorporated here - so a non-identity SliceTransform will probably
  // not work with rotating probes, and has never been tested
  // The slice axes matrix and slice transform together give the
  // coordinate transformation from the local coordinate system
  // of the Slice to the coordinate system of the Output.
  virtual void SetSliceTransform(vtkLinearTransform *);
  vtkGetObjectMacro(SliceTransform,vtkLinearTransform);

  // Description:
  // Decide whether or not to use a rotating probe (default off)
  void SetRotating(int probe);
  vtkGetMacro(Rotating,int);
  void RotatingOn() {this->SetRotating(1);};
  void RotatingOff() {this->SetRotating(0);};

  // Description:
  // Get the fan rotation (rotating probe)
  void SetFanRotation(int rot);
  vtkGetMacro(FanRotation,int);

  // Description:
  // Keeps track of the old fan rotation (rotating probe), so that it can be reversed when
  // we get the new rotation
  void SetPreviousFanRotation(int rot);
  vtkGetMacro(PreviousFanRotation,int);

  // Description:
  // Sets the maximum rotation change allowed between slices (try to omit unlikely
  // rotation changes, in case the rotation detection is not perfect) (default 20)
  vtkSetMacro(MaximumRotationChange, int);
  vtkGetMacro(MaximumRotationChange, int);

  // Description:
  // Threshold the image to pick out the screen pixels showing the
  // rotation (min intensity is RotatingThreshold1, max
  // intensity is RotatingThreshold2)
  void SetRotatingThreshold1(int thresh);
  vtkGetMacro(RotatingThreshold1,int);
  void SetRotatingThreshold2(int thresh);
  vtkGetMacro(RotatingThreshold2,int);
  vtkGetObjectMacro(RotationThresholder, vtkImageThreshold);

  // Description:
  // Parameters for isolating the region on the screen holding the rotation
  // value
  vtkSetMacro(RotatingShiftX,int);
  vtkGetMacro(RotatingShiftX,int);
  vtkSetMacro(RotatingShiftY,int);
  vtkGetMacro(RotatingShiftY,int);

  // Description:
  // Clipper to isolate the part of the image containing the rotation data
  vtkGetObjectMacro(RotationClipper, vtkImageClip);

  // Description:
  // Code to deal with whether the image is "flipped" (image is flipped if
  // the point of the fan is at the top of screen)
  vtkSetMacro(ImageFlipped,int);
  vtkGetMacro(ImageFlipped,int);
  vtkBooleanMacro(ImageFlipped, int);

  // Description:
  // Get the current rotation of the fan (rotating probe)
  int CalculateFanRotationValue();

  // Description:
  // Turn on and off optimizations (default on, turn them off only if
  // they are not stable on your architecture).
  //   0 means no optimization (almost never used)
  //   1 means break transformation into x, y and z components, and
  //      don't do bounds checking for nearest-neighbor interpolation
  //   2 means used fixed-point (i.e. integer) math instead of float math
  vtkSetMacro(Optimization,int);
  vtkGetMacro(Optimization,int);

  // Description:
  // Set/Get the interpolation mode, default is nearest neighbor. 
  vtkSetMacro(InterpolationMode,int);
  vtkGetMacro(InterpolationMode,int);
  void SetInterpolationModeToNearestNeighbor()
    { this->SetInterpolationMode(VTK_FREEHAND_NEAREST); };
  void SetInterpolationModeToLinear()
    { this->SetInterpolationMode(VTK_FREEHAND_LINEAR); };
  char *GetInterpolationModeAsString();

  // Description:
  // Turn on or off the compounding (default on, which means
  // that scans will be compounded where they overlap instead of the
  vtkGetMacro(Compounding,int);
  virtual void SetCompounding(int c);
  void CompoundingOn() {this->SetCompounding(1);};
  void CompoundingOff() {this->SetCompounding(0);}

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
  // When determining the modified time of the source. 
  unsigned long int GetMTime();

  // Description:
  // Get the thread ID for the reconstruction thread
  int GetReconstructionThreadId(){ return this->ReconstructionThreadId; };

  //////////////////////////////////////////////////////////////////////////////////
  // Not protected to be available from the reconstruction thread
  // These are internal functions, don't use them!!!
//BTX
  // Description:
  // Execute the reconstruction thread
  virtual void ThreadedSliceExecute(vtkImageData *inData, vtkImageData *outData,
        int extent[6], int threadId, vtkImageData *accData, vtkMatrix4x4* indexMatrix);
  
  // Description:
  // To split the extent over many threads
  int SplitSliceExtent(int splitExt[6], int startExt[6], int num, int total);

  // Description:
  // For filling holes
  void ThreadedFillExecute(vtkImageData *outData,  
         int outExt[6], int threadId, vtkImageData *accData);

  // Description:
  // Not protected to be accessible from reconstruction thread - do not use this function
  virtual void ReconstructSlice(double timestamp, vtkImageData* inData);

  // Description:
  // Do any old inserts before inserting the current slice (not used in base class, but
  // used in derived classes), for the reconstruction thread
  virtual void ReconstructOldSlice(double timestamp, vtkImageData* inData);

  // Description:
  // Get the time to sleep, for the reconstruction thread
  virtual double GetSleepTime();

  // Description:
  // Get the timestamp for the video slice, for the reconstruction thread
  virtual double CalculateCurrentVideoTime(vtkImageData* inData);

  // Description:
  // Update the input slice, for the reconstruction thread
  // Returns whether the slice has to be inserted now, even if it is not a
  // new slice
  virtual int UpdateSlice(vtkImageData* inData);

  // Description:
  // Update the accumulation buffer, for the reconstruction thread
  virtual void UpdateAccumulationBuffers();

  double ReconstructionRate;
  int RealTimeReconstruction; // # real-time or buffered
  int ReconstructionFrameCount; // # of frames to reconstruct
  // used for non-realtime reconstruction, and to store the tracker buffer's
  // information after we stop tracking
  vtkTrackerBuffer *TrackerBuffer; // used for non-realtime reconstruction,
//ETX
  //////////////////////////////////////////////////////////////////////////////////

  int PixelCount[4];

  // Description:
  // Set/Get the number of pixels inserted (by a particular thread)
  int GetPixelCount();
  void SetPixelCount(int threadId, int val);
  void IncrementPixelCount(int threadId, int increment);

protected:
  vtkFreehandUltrasound2();
  ~vtkFreehandUltrasound2();

  // basics
  vtkImageData *Slice;
  vtkImageData **AccumulationBuffers;
  vtkVideoSource2 *VideoSource;
  vtkTrackerTool *TrackerTool;
  double VideoLag;
  int NeedsClear;
  BufferItemUidType VideoBufferUid; 

  // parameters for fan/image
  vtkFloatingPointType OutputOrigin[3];
  vtkFloatingPointType OutputSpacing[3];
  int OutputExtent[6];
  double ClipRectangle[4];
  double FanAngles[2];
  double FanOrigin[2];
  double FanDepth;

  // reconstruction options
  int InterpolationMode;
  int Optimization;
  int Compounding;

  // reconstruction transformations
  vtkMatrix4x4 *SliceAxes;
  vtkLinearTransform *SliceTransform;
  vtkMatrix4x4 *IndexMatrix;
  //vtkMatrix4x4 *LastIndexMatrix; // never fully implemented, so comment it out

  // fan rotation
  int Rotating;
  int FanRotation;
  int PreviousFanRotation;
  int MaximumRotationChange;
  int RotatingThreshold1;
  int RotatingThreshold2;
  int RotatingShiftX;
  int RotatingShiftY;
  vtkImageClip *RotationClipper;
  vtkImageThreshold* RotationThresholder;
  int ImageFlipped; // 0 means no (good pizza), 1 means yes (bad pizza)

  // threading
  vtkCriticalSection *ActiveFlagLock;
  vtkMultiThreader *Threader;
  int NumberOfThreads;
  int ReconstructionThreadId;

  // Description:
  // Setup anything that needs to be done before real-time reconstruction
  // (override in derived classes)
  virtual int InitializeRealTimeReconstruction();

  // Description:
  // Setup anything that needs to be done before non-real-time reconstruction
  // (override in derived classes)
  virtual int InitializeReconstruction();

  // Description:
  // Clean up any loose ends after real-time reconstruction
  // (overridden in derived classes)
  virtual void UninitializeRealTimeReconstruction();

  // Description:
  // Clean up any loose ends after non-real-tme reconstruction
  // (override in derived classes)
  virtual void UninitializeReconstruction();

  // Description:
  // Allocates the output volumes
  virtual void SetupOutputVolumes();

  // Description:
  // Allocates the accumulation buffer if we are compounding
  virtual void SetupAccumulationBuffers();

  // Description:
  // Deletes the accumulation buffer
  virtual void DeleteAccumulationBuffers();

  // Description:
  // Actually clear the output volume(s), by calling InternalClearOutputHelperForOutput() and
  // InternalClearOutputHelperForAccumulation()
  virtual void InternalClearOutput();

  // Description:
  // Actually clear an output volume and accumulation buffer
  virtual void InternalInternalClearOutput(vtkImageData* outData, vtkImageData* accData);

  // Description:
  // Actually clear a single output volume
  virtual void InternalClearOutputHelperForOutput(vtkImageData* outData);

  // Description:
  // Actually clear a single accumulation buffer
  virtual void InternalClearOutputHelperForAccumulation(vtkImageData* accData);

  // Description:
  // Setup the threader and execute
  void MultiThread(vtkImageData *inData, vtkImageData *outData, vtkImageData *accData, vtkMatrix4x4 *indexMatrix);

   // Description:
  // Cause the slice to be inserted into the first reconstruction volume
  void InsertSliceHelper(vtkImageData *outData, vtkImageData* accData, vtkImageData* inData, vtkMatrix4x4* indexMatrix);

  // Description:
  // Insert the slice into the ith volume, with optimization
  void OptimizedInsertSliceHelper(vtkImageData* outData, vtkImageData* accData, vtkImageData* inData, vtkMatrix4x4* indexMatrix);

  // Description:
  // The IndexMatrix gives the coordinate transformation from (i,j,k)
  // voxel indices in the slice to (i,j,k) voxel indices in the
  // output.
  virtual vtkMatrix4x4 *GetIndexMatrix();
  vtkMatrix4x4 *GetIndexMatrixHelper(vtkMatrix4x4* sliceAxes, vtkLinearTransform* sliceTransform);

  // Description:
  // Get a coded representation of the pixels showing the
  // current rotation of the fan (rotating probe)
  int GetFanRepresentation (int x, int y);
  int GetFanRepresentationHelper(int x, int y);

  // Description:
  // Setup the threader and execute
  void MultiThreadFill(vtkImageData *outData, vtkImageData *accData);

  // Description:
  // Returns an XMLDataElement describing the freehand object
  virtual vtkXMLDataElement* MakeXMLElement();

  // Description:
  // Similar to RequestInformation(), but applied to the output
  virtual void InternalExecuteInformation();
  void InternalExecuteInformationHelper(vtkImageData* output, vtkImageData* accBuffer);

  // Description:
  // VTK 5 pipeline
  virtual int FillInputPortInformation(int port, vtkInformation* info);
  virtual int FillOutputPortInformation(int port, vtkInformation* info);
  virtual int ProcessRequest(vtkInformation*,
                             vtkInformationVector**,
                             vtkInformationVector*);
  virtual int RequestInformation(vtkInformation* request,
                                 vtkInformationVector** inputVector,
                                 vtkInformationVector* outputVector);
  virtual int RequestUpdateExtent(vtkInformation*,
                                 vtkInformationVector**,
                                 vtkInformationVector*);
  virtual int RequestData(vtkInformation *request,
                          vtkInformationVector** inputVector,
                          vtkInformationVector* outputVector);
  virtual int ComputePipelineMTime(vtkInformation *request,
           vtkInformationVector **inInfoVec,
           vtkInformationVector *outInfoVec,
           int requestFromOutputPort,
           unsigned long* mtime);
  void RequestInformationHelper(vtkInformation *outInfo, vtkImageData* output, vtkImageData *accBuffer);

private:
  vtkFreehandUltrasound2(const vtkFreehandUltrasound2&);
  void operator=(const vtkFreehandUltrasound2&);
};

//----------------------------------------------------------------------------
inline char *vtkFreehandUltrasound2::GetInterpolationModeAsString()
{
  switch (this->InterpolationMode)
    {
    case VTK_FREEHAND_NEAREST:
      return "NearestNeighbor";
    case VTK_FREEHAND_LINEAR:
      return "Linear";
    default:
      return "";
    }
}

#endif
