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

#include "PlusConfigure.h"

#include "vtkObjectFactory.h"
#include "vtkSmartPointer.h"
#include "vtkMultiThreader.h"
#include "vtkXMLUtilities.h"
#include "vtkXMLDataElement.h"
#include "vtkIndent.h"
#include "vtkMath.h"
#include "vtkTransform.h"

#include "vtkPasteSliceIntoVolume.h"
#include "vtkPasteSliceIntoVolumeHelperUnoptimized.h"
#include "vtkPasteSliceIntoVolumeHelperOptimized.h"

vtkCxxRevisionMacro(vtkPasteSliceIntoVolume, "$Revisions: 1.0 $");
vtkStandardNewMacro(vtkPasteSliceIntoVolume);

struct InsertSliceThreadFunctionInfoStruct
{
  vtkImageData* InputFrameImage;
  vtkMatrix4x4* TransformImageToReference;
  vtkImageData* OutputVolume;
  vtkImageData* Accumulator;
  vtkPasteSliceIntoVolume::OptimizationType Optimization;
  int Compounding;
  vtkPasteSliceIntoVolume::InterpolationType InterpolationMode;
  vtkPasteSliceIntoVolume::CalculationType CalculationMode;

  double ClipRectangleOrigin[2];
  double ClipRectangleSize[2];
  double FanAngles[2];
  double FanOrigin[2];
  double FanDepth;
  std::vector<unsigned int> AccumulationBufferSaturationErrors;
};

//----------------------------------------------------------------------------
vtkPasteSliceIntoVolume::vtkPasteSliceIntoVolume()
{
  this->ReconstructedVolume=vtkImageData::New();
  this->AccumulationBuffer=vtkImageData::New();
  this->Threader=vtkMultiThreader::New();

  this->OutputOrigin[0] = 0.0;
  this->OutputOrigin[1] = 0.0;
  this->OutputOrigin[2] = 0.0;

  this->OutputSpacing[0] = 1.0;
  this->OutputSpacing[1] = 1.0;
  this->OutputSpacing[2] = 1.0;

  // Set to invalid values by default
  // If the user doesn't set the correct values then inserting the slice will be refused
  this->OutputExtent[0] = 0;
  this->OutputExtent[1] = 0;
  this->OutputExtent[2] = 0;
  this->OutputExtent[3] = 0;
  this->OutputExtent[4] = 0;
  this->OutputExtent[5] = 0;

  this->ClipRectangleOrigin[0] = 0;
  this->ClipRectangleOrigin[1] = 0;
  this->ClipRectangleSize[0] = 0;
  this->ClipRectangleSize[1] = 0;

  this->FanAngles[0] = 0.0;
  this->FanAngles[1] = 0.0;
  this->FanOrigin[0] = 0.0;
  this->FanOrigin[1] = 0.0;
  this->FanDepth = 1.0;

  // scalar type for input and output
  this->OutputScalarMode = VTK_UNSIGNED_CHAR;

  // reconstruction options
  this->InterpolationMode = NEAREST_NEIGHBOR_INTERPOLATION;
  this->Optimization = FULL_OPTIMIZATION;
  this->CalculationMode = WEIGHTED_AVERAGE;
  this->Compounding = 0;

  this->NumberOfThreads=0; // 0 means not set, the default number of threads will be used

  this->EnableAccumulationBufferOverflowWarning = true;
}

//----------------------------------------------------------------------------
vtkPasteSliceIntoVolume::~vtkPasteSliceIntoVolume()
{  
  if (this->ReconstructedVolume)
  {
    this->ReconstructedVolume->Delete();
    this->ReconstructedVolume=NULL;
  }
  if (this->AccumulationBuffer)
  {
    this->AccumulationBuffer->Delete();
    this->AccumulationBuffer=NULL;
  }
  if (this->Threader)
  {
    this->Threader->Delete();
    this->Threader=NULL;
  }
}

//----------------------------------------------------------------------------
void vtkPasteSliceIntoVolume::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  if (this->ReconstructedVolume)
  {
    os << indent << "ReconstructedVolume:\n";
    this->ReconstructedVolume->PrintSelf(os,indent.GetNextIndent());
  }
  if (this->AccumulationBuffer)
  {
    os << indent << "AccumulationBuffer:\n";
    this->AccumulationBuffer->PrintSelf(os,indent.GetNextIndent());
  }
  os << indent << "OutputOrigin: " << this->OutputOrigin[0] << " " <<
    this->OutputOrigin[1] << " " << this->OutputOrigin[2] << "\n";
  os << indent << "OutputSpacing: " << this->OutputSpacing[0] << " " <<
    this->OutputSpacing[1] << " " << this->OutputSpacing[2] << "\n";
  os << indent << "OutputExtent: " << this->OutputExtent[0] << " " <<
    this->OutputExtent[1] << " " << this->OutputExtent[2] << " " <<
    this->OutputExtent[3] << " " << this->OutputExtent[4] << " " <<
    this->OutputExtent[5] << "\n";
  os << indent << "ClipRectangleOrigin: " << this->ClipRectangleOrigin[0] << " " <<
    this->ClipRectangleOrigin[1] << "\n";
  os << indent << "ClipRectangleSize: " << this->ClipRectangleSize[0] << " " <<
    this->ClipRectangleSize[1] << "\n";
  os << indent << "FanAngles: " << this->FanAngles[0] << " " <<
    this->FanAngles[1] << "\n";
  os << indent << "FanOrigin: " << this->FanOrigin[0] << " " <<
    this->FanOrigin[1] << "\n";
  os << indent << "FanDepth: " << this->FanDepth << "\n";
  os << indent << "InterpolationMode: " << this->GetInterpolationModeAsString(this->InterpolationMode) << "\n";
  os << indent << "CalculationMode: " << this->GetCalculationModeAsString(this->CalculationMode) << "\n";
  os << indent << "Optimization: " << this->GetOptimizationModeAsString(this->Optimization) << "\n";
  os << indent << "Compounding: " << (this->Compounding ? "On\n":"Off\n");
  os << indent << "NumberOfThreads: ";
  if (this->NumberOfThreads>0)
  {
    os << this->NumberOfThreads << "\n";
  }
  else
  {
    os << "default\n";
  }
}


//----------------------------------------------------------------------------
vtkImageData *vtkPasteSliceIntoVolume::GetReconstructedVolume()
{
  return this->ReconstructedVolume;
}

//----------------------------------------------------------------------------
vtkImageData *vtkPasteSliceIntoVolume::GetAccumulationBuffer()
{
  if (!this->Compounding)
  {
    return NULL;
  }
  return this->AccumulationBuffer;
}

//----------------------------------------------------------------------------
// Clear the output volume and the accumulation buffer
PlusStatus vtkPasteSliceIntoVolume::ResetOutput()
{   
  // Allocate memory for accumulation buffer and set all pixels to 0
  // Start with this buffer because if no compunding is needed then we release memory before allocating memory for the reconstructed image.

  vtkImageData* accData = this->GetAccumulationBuffer();
  if (this->Compounding)
  {
    if (accData==NULL)
    {
      LOG_ERROR("Accumulation buffer object is not created");
      return PLUS_FAIL;
    }
    int accExtent[6]={0}; // by default set the accumulation buffer to 0 size
    if (this->Compounding)
    {
      // we do compunding, so we need to have an accumulation buffer with the same size as the output image
      for (int i=0; i<6; i++)
      {
        accExtent[i]=this->OutputExtent[i];
      }
    }
    accData->SetExtent(accExtent);
    accData->SetOrigin(this->OutputOrigin);
    accData->SetSpacing(this->OutputSpacing);
    accData->SetScalarType(VTK_UNSIGNED_SHORT); // changed from unsigned int, since the hole filler assumes unsigned short
    accData->SetNumberOfScalarComponents(1);
    accData->AllocateScalars();
    void *accPtr = accData->GetScalarPointerForExtent(accExtent);
    if (accPtr==NULL)
    {
      LOG_ERROR("Cannot allocate memory for accumulation image extent: "<< accExtent[1]-accExtent[0] <<"x"<< accExtent[3]-accExtent[2] <<" x "<< accExtent[5]-accExtent[4]);
    }
    else
    {
      memset(accPtr,0,((accExtent[1]-accExtent[0]+1)*
        (accExtent[3]-accExtent[2]+1)*
        (accExtent[5]-accExtent[4]+1)*
        accData->GetScalarSize()*accData->GetNumberOfScalarComponents()));
    }
  }
  // Allocate memory for the reconstructed image and set all pixels to 0

  vtkImageData* outData = this->ReconstructedVolume;
  if (outData==NULL)
  {
    LOG_ERROR("Output image object is not created");
    return PLUS_FAIL;
  }

  int *outExtent=this->OutputExtent;
  outData->SetExtent(outExtent);
  outData->SetOrigin(this->OutputOrigin);
  outData->SetSpacing(this->OutputSpacing);
  outData->SetScalarType(this->OutputScalarMode);
  outData->SetNumberOfScalarComponents(2); // first component: image intensity; second component: if the pixel was set (0 = not set (hole), 1 = set)
  outData->AllocateScalars();
  void *outPtr = outData->GetScalarPointerForExtent(outExtent);
  if (outPtr==NULL)
  {
    LOG_ERROR("Cannot allocate memory for output image extent: "<< outExtent[1]-outExtent[0] <<"x"<< outExtent[3]-outExtent[2] <<" x "<< outExtent[5]-outExtent[4]);
    return PLUS_FAIL;
  }
  else
  {
    memset(outPtr,0,((outExtent[1]-outExtent[0]+1)*
      (outExtent[3]-outExtent[2]+1)*
      (outExtent[5]-outExtent[4]+1)*
      outData->GetScalarSize()*outData->GetNumberOfScalarComponents()));
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
// Set compounding setting
void vtkPasteSliceIntoVolume::SetCompounding(int compound)
{
  this->Compounding = compound;
  ResetOutput();
}

//****************************************************************************
// RECONSTRUCTION - OPTIMIZED
//****************************************************************************

//----------------------------------------------------------------------------
// Does the actual work of optimally inserting a slice, with optimization
// Basically, just calls Multithread()
PlusStatus vtkPasteSliceIntoVolume::InsertSlice(vtkImageData *image, vtkMatrix4x4* transformImageToReference)
{
  if (this->OutputExtent[0]>=this->OutputExtent[1]
  && this->OutputExtent[2]>=this->OutputExtent[3]
  && this->OutputExtent[4]>=this->OutputExtent[5])
  {
    LOG_ERROR("Invalid output volume extent ["<<this->OutputExtent[0]<<","<<this->OutputExtent[1]<<","
      <<this->OutputExtent[2]<<","<<this->OutputExtent[3]<<","<<this->OutputExtent[4]<<","<<this->OutputExtent[5]<<"]."
      <<" Cannot insert slice into the volume. Set the correct output volume origin, spacing, and extent before inserting slices.");
    return PLUS_FAIL;
  }

  InsertSliceThreadFunctionInfoStruct str;
  str.InputFrameImage = image;
  str.TransformImageToReference = transformImageToReference;
  str.OutputVolume = this->ReconstructedVolume;
  str.Accumulator = this->AccumulationBuffer;
  str.Compounding = this->Compounding;
  str.InterpolationMode = this->InterpolationMode;
  str.CalculationMode = this->CalculationMode;
  str.Optimization = this->Optimization;
  if (this->ClipRectangleSize[0]>0 && this->ClipRectangleSize[1]>0)
  {
    // ClipRectangle specified
    str.ClipRectangleOrigin[0]=this->ClipRectangleOrigin[0];
    str.ClipRectangleOrigin[1]=this->ClipRectangleOrigin[1];
    str.ClipRectangleSize[0]=this->ClipRectangleSize[0];
    str.ClipRectangleSize[1]=this->ClipRectangleSize[1];
  }
  else
  {
    // ClipRectangle not specified, use full image slice
    str.ClipRectangleOrigin[0]=image->GetExtent()[0];
    str.ClipRectangleOrigin[1]=image->GetExtent()[2];
    str.ClipRectangleSize[0]=image->GetExtent()[1];
    str.ClipRectangleSize[1]=image->GetExtent()[3];
  }
  str.FanAngles[0]=this->FanAngles[0];
  str.FanAngles[1]=this->FanAngles[1];
  str.FanOrigin[0]=this->FanOrigin[0];
  str.FanOrigin[1]=this->FanOrigin[1];
  str.FanDepth=this->FanDepth;

  if (this->NumberOfThreads>0)
  {
    this->Threader->SetNumberOfThreads(this->NumberOfThreads);
  }

  // initialize array that counts the number of insertion errors due to overflow in the accumulation buffer
  int numThreads(this->Threader->GetNumberOfThreads());
  str.AccumulationBufferSaturationErrors.resize(numThreads);
  str.AccumulationBufferSaturationErrors.clear();
  for (int i = 0; i < numThreads; i++) {
    str.AccumulationBufferSaturationErrors.push_back(0);
  }

  this->Threader->SetSingleMethod(InsertSliceThreadFunction, &str);
  this->Threader->SingleMethodExecute();

  // sum up str.AccumulationBufferSaturationErrors
  unsigned int sumAccOverflowErrors(0);
  for (int i = 0; i < numThreads; i++) {
    sumAccOverflowErrors += str.AccumulationBufferSaturationErrors[i];
  }
  if (sumAccOverflowErrors && !EnableAccumulationBufferOverflowWarning) {
    LOG_WARNING(sumAccOverflowErrors << " voxels have had too many pixels inserted. This can result in errors in the final volume. It is recommended that the output volume resolution be increased.");
  }

  this->Modified();

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
VTK_THREAD_RETURN_TYPE vtkPasteSliceIntoVolume::InsertSliceThreadFunction( void *arg )
{
  vtkMultiThreader::ThreadInfo* threadInfo = static_cast<vtkMultiThreader::ThreadInfo *>(arg);
  InsertSliceThreadFunctionInfoStruct *str = static_cast<InsertSliceThreadFunctionInfoStruct*>(threadInfo->UserData);

  // Compute what extent of the input image will be processed by this thread
  int threadId = threadInfo->ThreadID;
  int threadCount = threadInfo->NumberOfThreads;
  int inputFrameExtent[6];
  str->InputFrameImage->GetExtent(inputFrameExtent);
  int inputFrameExtentForCurrentThread[6];
  int totalUsedThreads = vtkPasteSliceIntoVolume::SplitSliceExtent(inputFrameExtentForCurrentThread, inputFrameExtent, threadId, threadCount);

  if (threadId >= totalUsedThreads)
  {
    //   don't use this thread. Sometimes the threads dont
    //   break up very well and it is just as efficient to leave a 
    //   few threads idle.
    return VTK_THREAD_RETURN_VALUE;
  }

  // this filter expects that input is the same type as output.
  if (str->InputFrameImage->GetScalarType() != str->OutputVolume->GetScalarType())
  {
    LOG_ERROR("OptimizedInsertSlice: input ScalarType (" << str->InputFrameImage->GetScalarType()<<") "
      << " must match out ScalarType ("<<str->OutputVolume->GetScalarType()<<")");
    return VTK_THREAD_RETURN_VALUE;
  }

  // Get input frame extent and pointer
  vtkImageData *inData=str->InputFrameImage;  
  void *inPtr = inData->GetScalarPointerForExtent(inputFrameExtentForCurrentThread);
  // Get output volume extent and pointer
  vtkImageData *outData=str->OutputVolume;
  int *outExt = outData->GetExtent();
  void *outPtr = outData->GetScalarPointerForExtent(outExt);

  void *accPtr = NULL; 
  if (str->Compounding)
  {
    accPtr = str->Accumulator->GetScalarPointerForExtent(outExt);
  }

  // count the number of accumulation buffer overflow instances in the memory address here:
  unsigned int* accumulationBufferSaturationErrorsThread = &(str->AccumulationBufferSaturationErrors[threadId]);

  // Transform chain:
  // ImagePixToVolumePix = 
  //  = VolumePixFromImagePix
  //  = VolumePixFromRef * RefFromImage * ImageFromImagePix 

  vtkSmartPointer<vtkTransform> tVolumePixFromRef = vtkSmartPointer<vtkTransform>::New();
  tVolumePixFromRef->Translate(str->OutputVolume->GetOrigin());
  tVolumePixFromRef->Scale(str->OutputVolume->GetSpacing());
  tVolumePixFromRef->Inverse();

  vtkSmartPointer<vtkTransform> tRefFromImage = vtkSmartPointer<vtkTransform>::New();
  tRefFromImage->SetMatrix(str->TransformImageToReference);

  vtkSmartPointer<vtkTransform> tImageFromImagePix = vtkSmartPointer<vtkTransform>::New();
  tImageFromImagePix->Scale(str->InputFrameImage->GetSpacing()); 

  vtkSmartPointer<vtkTransform> tImagePixToVolumePix = vtkSmartPointer<vtkTransform>::New();
  tImagePixToVolumePix->Concatenate(tVolumePixFromRef);
  tImagePixToVolumePix->Concatenate(tRefFromImage);
  tImagePixToVolumePix->Concatenate(tImageFromImagePix);

  vtkSmartPointer<vtkMatrix4x4> mImagePixToVolumePix = vtkSmartPointer<vtkMatrix4x4>::New();
  tImagePixToVolumePix->GetMatrix(mImagePixToVolumePix);

  if (str->Optimization == FULL_OPTIMIZATION)
  {
    // use fixed-point math
    // change transform matrix so that instead of taking 
    // input coords -> output coords it takes output indices -> input indices
    fixed newmatrix[4][4]; // fixed because optimization = 2
    for (int i = 0; i < 4; i++)
    {
      newmatrix[i][0] = mImagePixToVolumePix->GetElement(i,0);
      newmatrix[i][1] = mImagePixToVolumePix->GetElement(i,1);
      newmatrix[i][2] = mImagePixToVolumePix->GetElement(i,2);
      newmatrix[i][3] = mImagePixToVolumePix->GetElement(i,3);
    }

    switch (str->InputFrameImage->GetScalarType())
    {
    case VTK_SHORT:
      vtkOptimizedInsertSlice(outData, (short *)(outPtr), 
        (unsigned short *)(accPtr), 
        str->InputFrameImage, (short *)(inPtr), 
        inputFrameExtentForCurrentThread, newmatrix,
        str->ClipRectangleOrigin,str->ClipRectangleSize,
        str->FanAngles,str->FanOrigin,str->FanDepth, str->InterpolationMode, str->CalculationMode,
        accumulationBufferSaturationErrorsThread);
      break;
    case VTK_UNSIGNED_SHORT:
      vtkOptimizedInsertSlice(outData,(unsigned short *)(outPtr),
        (unsigned short *)(accPtr), 
        str->InputFrameImage, (unsigned short *)(inPtr), 
        inputFrameExtentForCurrentThread, newmatrix,
        str->ClipRectangleOrigin,str->ClipRectangleSize,
        str->FanAngles,str->FanOrigin,str->FanDepth, str->InterpolationMode, str->CalculationMode,
        accumulationBufferSaturationErrorsThread);
      break;
    case VTK_CHAR:
      vtkOptimizedInsertSlice(outData,(char *)(outPtr),
        (unsigned short *)(accPtr), 
        str->InputFrameImage, (char *)(inPtr), 
        inputFrameExtentForCurrentThread, newmatrix,
        str->ClipRectangleOrigin,str->ClipRectangleSize,
        str->FanAngles,str->FanOrigin,str->FanDepth, str->InterpolationMode, str->CalculationMode,
        accumulationBufferSaturationErrorsThread);
      break;
    case VTK_UNSIGNED_CHAR:
      vtkOptimizedInsertSlice(outData,(unsigned char *)(outPtr),
        (unsigned short *)(accPtr), 
        str->InputFrameImage, (unsigned char *)(inPtr), 
        inputFrameExtentForCurrentThread, newmatrix,
        str->ClipRectangleOrigin,str->ClipRectangleSize,
        str->FanAngles,str->FanOrigin,str->FanDepth, str->InterpolationMode, str->CalculationMode,
        accumulationBufferSaturationErrorsThread);
      break;
    case VTK_FLOAT:
      vtkOptimizedInsertSlice(outData,(float *)(outPtr),
        (unsigned short *)(accPtr), 
        str->InputFrameImage, (float *)(inPtr), 
        inputFrameExtentForCurrentThread, newmatrix,
        str->ClipRectangleOrigin,str->ClipRectangleSize,
        str->FanAngles,str->FanOrigin,str->FanDepth, str->InterpolationMode, str->CalculationMode,
        accumulationBufferSaturationErrorsThread);
      break;
    case VTK_DOUBLE:
      vtkOptimizedInsertSlice(outData,(double *)(outPtr),
        (unsigned short *)(accPtr), 
        str->InputFrameImage, (double *)(inPtr), 
        inputFrameExtentForCurrentThread, newmatrix,
        str->ClipRectangleOrigin,str->ClipRectangleSize,
        str->FanAngles,str->FanOrigin,str->FanDepth, str->InterpolationMode, str->CalculationMode,
        accumulationBufferSaturationErrorsThread);
      break;
    case VTK_INT:
      vtkOptimizedInsertSlice(outData,(int *)(outPtr),
        (unsigned short *)(accPtr), 
        str->InputFrameImage, (int *)(inPtr), 
        inputFrameExtentForCurrentThread, newmatrix,
        str->ClipRectangleOrigin,str->ClipRectangleSize,
        str->FanAngles,str->FanOrigin,str->FanDepth, str->InterpolationMode, str->CalculationMode,
        accumulationBufferSaturationErrorsThread);
      break;
    case VTK_UNSIGNED_INT:
      vtkOptimizedInsertSlice(outData,(unsigned int *)(outPtr),
        (unsigned short *)(accPtr), 
        str->InputFrameImage, (unsigned int *)(inPtr), 
        inputFrameExtentForCurrentThread, newmatrix,
        str->ClipRectangleOrigin,str->ClipRectangleSize,
        str->FanAngles,str->FanOrigin,str->FanDepth, str->InterpolationMode, str->CalculationMode,
        accumulationBufferSaturationErrorsThread);
      break;
    case VTK_LONG:
      vtkOptimizedInsertSlice(outData,(long *)(outPtr),
        (unsigned short *)(accPtr), 
        str->InputFrameImage, (long *)(inPtr), 
        inputFrameExtentForCurrentThread, newmatrix,
        str->ClipRectangleOrigin,str->ClipRectangleSize,
        str->FanAngles,str->FanOrigin,str->FanDepth, str->InterpolationMode, str->CalculationMode,
        accumulationBufferSaturationErrorsThread);
      break;
    case VTK_UNSIGNED_LONG:
      vtkOptimizedInsertSlice(outData,(unsigned long *)(outPtr),
        (unsigned short *)(accPtr), 
        str->InputFrameImage, (unsigned long *)(inPtr), 
        inputFrameExtentForCurrentThread, newmatrix,
        str->ClipRectangleOrigin,str->ClipRectangleSize,
        str->FanAngles,str->FanOrigin,str->FanDepth, str->InterpolationMode, str->CalculationMode,
        accumulationBufferSaturationErrorsThread);
      break;
    default:
      LOG_ERROR("OptimizedInsertSlice: Unknown input ScalarType");
      return VTK_THREAD_RETURN_VALUE;
    }
  }
  else
  {
    // if we are not using fixed point math for optimization = 2, we are either:
    // doing no optimization (0) OR
    // breaking into x, y, z components with no bounds checking for nearest neighbor (1)

    // change transform matrix so that instead of taking 
    // input coords -> output coords it takes output indices -> input indices
    double newmatrix[4][4];
    for (int i = 0; i < 4; i++)
    {
      newmatrix[i][0] = mImagePixToVolumePix->GetElement(i,0);
      newmatrix[i][1] = mImagePixToVolumePix->GetElement(i,1);
      newmatrix[i][2] = mImagePixToVolumePix->GetElement(i,2);
      newmatrix[i][3] = mImagePixToVolumePix->GetElement(i,3);
    }


    if (str->Optimization==PARTIAL_OPTIMIZATION)
    {
      switch (inData->GetScalarType())
      {
      case VTK_SHORT:
        vtkOptimizedInsertSlice(outData, (short *)(outPtr), 
          (unsigned short *)(accPtr), 
          inData, (short *)(inPtr), 
          inputFrameExtentForCurrentThread, newmatrix,
          str->ClipRectangleOrigin,str->ClipRectangleSize,
          str->FanAngles,str->FanOrigin,str->FanDepth, str->InterpolationMode, str->CalculationMode,
          accumulationBufferSaturationErrorsThread);
        break;
      case VTK_UNSIGNED_SHORT:
        vtkOptimizedInsertSlice(outData,(unsigned short *)(outPtr),
          (unsigned short *)(accPtr), 
          inData, (unsigned short *)(inPtr), 
          inputFrameExtentForCurrentThread, newmatrix,
          str->ClipRectangleOrigin,str->ClipRectangleSize,
          str->FanAngles,str->FanOrigin,str->FanDepth, str->InterpolationMode, str->CalculationMode,
          accumulationBufferSaturationErrorsThread);
        break;
      case VTK_UNSIGNED_CHAR:
        vtkOptimizedInsertSlice(outData,(unsigned char *)(outPtr),
          (unsigned short *)(accPtr), 
          inData, (unsigned char *)(inPtr), 
          inputFrameExtentForCurrentThread, newmatrix,
          str->ClipRectangleOrigin,str->ClipRectangleSize,
          str->FanAngles,str->FanOrigin,str->FanDepth, str->InterpolationMode, str->CalculationMode,
          accumulationBufferSaturationErrorsThread);
        break;
      case VTK_CHAR:
        vtkOptimizedInsertSlice(outData,(char *)(outPtr),
          (unsigned short *)(accPtr), 
          inData, (char *)(inPtr), 
          inputFrameExtentForCurrentThread, newmatrix,
          str->ClipRectangleOrigin,str->ClipRectangleSize,
          str->FanAngles,str->FanOrigin,str->FanDepth, str->InterpolationMode, str->CalculationMode,
          accumulationBufferSaturationErrorsThread);
        break;
      case VTK_FLOAT:
        vtkOptimizedInsertSlice(outData,(float *)(outPtr),
          (unsigned short *)(accPtr), 
          inData, (float *)(inPtr), 
          inputFrameExtentForCurrentThread, newmatrix,
          str->ClipRectangleOrigin,str->ClipRectangleSize,
          str->FanAngles,str->FanOrigin,str->FanDepth, str->InterpolationMode, str->CalculationMode,
          accumulationBufferSaturationErrorsThread);
        break;
      case VTK_DOUBLE:
        vtkOptimizedInsertSlice(outData,(double *)(outPtr),
          (unsigned short *)(accPtr), 
          inData, (double *)(inPtr), 
          inputFrameExtentForCurrentThread, newmatrix,
          str->ClipRectangleOrigin,str->ClipRectangleSize,
          str->FanAngles,str->FanOrigin,str->FanDepth, str->InterpolationMode, str->CalculationMode,
          accumulationBufferSaturationErrorsThread);
        break;
      case VTK_INT:
        vtkOptimizedInsertSlice(outData,(int *)(outPtr),
          (unsigned short *)(accPtr), 
          inData, (int *)(inPtr), 
          inputFrameExtentForCurrentThread, newmatrix,
          str->ClipRectangleOrigin,str->ClipRectangleSize,
          str->FanAngles,str->FanOrigin,str->FanDepth, str->InterpolationMode, str->CalculationMode,
          accumulationBufferSaturationErrorsThread);
        break;
      case VTK_UNSIGNED_INT:
        vtkOptimizedInsertSlice(outData,(unsigned int *)(outPtr),
          (unsigned short *)(accPtr), 
          inData, (unsigned int *)(inPtr), 
          inputFrameExtentForCurrentThread, newmatrix,
          str->ClipRectangleOrigin,str->ClipRectangleSize,
          str->FanAngles,str->FanOrigin,str->FanDepth, str->InterpolationMode, str->CalculationMode,
          accumulationBufferSaturationErrorsThread);
        break;
      case VTK_LONG:
        vtkOptimizedInsertSlice(outData,(long *)(outPtr),
          (unsigned short *)(accPtr), 
          inData, (long *)(inPtr), 
          inputFrameExtentForCurrentThread, newmatrix,
          str->ClipRectangleOrigin,str->ClipRectangleSize,
          str->FanAngles,str->FanOrigin,str->FanDepth, str->InterpolationMode, str->CalculationMode,
          accumulationBufferSaturationErrorsThread);
        break;
      case VTK_UNSIGNED_LONG:
        vtkOptimizedInsertSlice(outData,(unsigned long *)(outPtr),
          (unsigned short *)(accPtr), 
          inData, (unsigned long *)(inPtr), 
          inputFrameExtentForCurrentThread, newmatrix,
          str->ClipRectangleOrigin,str->ClipRectangleSize,
          str->FanAngles,str->FanOrigin,str->FanDepth, str->InterpolationMode, str->CalculationMode,
          accumulationBufferSaturationErrorsThread);
        break;
      default:
        LOG_ERROR("OptimizedInsertSlice: Unknown input ScalarType");
      }
    }
    else
    {
      // no optimization
      switch (inData->GetScalarType())
      {
      case VTK_SHORT:
        vtkUnoptimizedInsertSlice(outData, (short *)(outPtr), 
          (unsigned short *)(accPtr), 
          inData, (short *)(inPtr), 
          inputFrameExtentForCurrentThread, mImagePixToVolumePix,
          str->ClipRectangleOrigin,str->ClipRectangleSize,
          str->FanAngles,str->FanOrigin,str->FanDepth, str->InterpolationMode, str->CalculationMode,
          accumulationBufferSaturationErrorsThread);
        break;
      case VTK_UNSIGNED_SHORT:
        vtkUnoptimizedInsertSlice(outData,(unsigned short *)(outPtr),
          (unsigned short *)(accPtr), 
          inData, (unsigned short *)(inPtr), 
          inputFrameExtentForCurrentThread, mImagePixToVolumePix,
          str->ClipRectangleOrigin,str->ClipRectangleSize,
          str->FanAngles,str->FanOrigin,str->FanDepth, str->InterpolationMode, str->CalculationMode,
          accumulationBufferSaturationErrorsThread);
        break;
      case VTK_UNSIGNED_CHAR:
        vtkUnoptimizedInsertSlice(outData,(unsigned char *)(outPtr),
          (unsigned short *)(accPtr), 
          inData, (unsigned char *)(inPtr), 
          inputFrameExtentForCurrentThread, mImagePixToVolumePix,
          str->ClipRectangleOrigin,str->ClipRectangleSize,
          str->FanAngles,str->FanOrigin,str->FanDepth, str->InterpolationMode, str->CalculationMode,
          accumulationBufferSaturationErrorsThread);
        break;
      case VTK_CHAR:
        vtkUnoptimizedInsertSlice(outData,(char *)(outPtr),
          (unsigned short *)(accPtr), 
          inData, (char *)(inPtr), 
          inputFrameExtentForCurrentThread, mImagePixToVolumePix,
          str->ClipRectangleOrigin,str->ClipRectangleSize,
          str->FanAngles,str->FanOrigin,str->FanDepth, str->InterpolationMode, str->CalculationMode,
          accumulationBufferSaturationErrorsThread);
        break;
      case VTK_FLOAT:
        vtkUnoptimizedInsertSlice(outData, (float *)(outPtr), 
          (unsigned short *)(accPtr), 
          inData, (float *)(inPtr), 
          inputFrameExtentForCurrentThread, mImagePixToVolumePix,
          str->ClipRectangleOrigin,str->ClipRectangleSize,
          str->FanAngles,str->FanOrigin,str->FanDepth, str->InterpolationMode, str->CalculationMode,
          accumulationBufferSaturationErrorsThread);
        break;
      case VTK_DOUBLE:
        vtkUnoptimizedInsertSlice(outData, (double *)(outPtr), 
          (unsigned short *)(accPtr), 
          inData, (double *)(inPtr), 
          inputFrameExtentForCurrentThread, mImagePixToVolumePix,
          str->ClipRectangleOrigin,str->ClipRectangleSize,
          str->FanAngles,str->FanOrigin,str->FanDepth, str->InterpolationMode, str->CalculationMode,
          accumulationBufferSaturationErrorsThread);
        break;
      case VTK_INT:
        vtkUnoptimizedInsertSlice(outData, (int *)(outPtr), 
          (unsigned short *)(accPtr), 
          inData, (int *)(inPtr), 
          inputFrameExtentForCurrentThread, mImagePixToVolumePix,
          str->ClipRectangleOrigin,str->ClipRectangleSize,
          str->FanAngles,str->FanOrigin,str->FanDepth, str->InterpolationMode, str->CalculationMode,
          accumulationBufferSaturationErrorsThread);
        break;
      case VTK_UNSIGNED_INT:
        vtkUnoptimizedInsertSlice(outData, (unsigned int *)(outPtr), 
          (unsigned short *)(accPtr), 
          inData, (unsigned int *)(inPtr), 
          inputFrameExtentForCurrentThread, mImagePixToVolumePix,
          str->ClipRectangleOrigin,str->ClipRectangleSize,
          str->FanAngles,str->FanOrigin,str->FanDepth, str->InterpolationMode, str->CalculationMode,
          accumulationBufferSaturationErrorsThread);
        break;
      case VTK_LONG:
        vtkUnoptimizedInsertSlice(outData, (long *)(outPtr), 
          (unsigned short *)(accPtr), 
          inData, (long *)(inPtr), 
          inputFrameExtentForCurrentThread, mImagePixToVolumePix,
          str->ClipRectangleOrigin,str->ClipRectangleSize,
          str->FanAngles,str->FanOrigin,str->FanDepth, str->InterpolationMode, str->CalculationMode,
          accumulationBufferSaturationErrorsThread);
        break;
      case VTK_UNSIGNED_LONG:
        vtkUnoptimizedInsertSlice(outData, (unsigned long *)(outPtr), 
          (unsigned short *)(accPtr), 
          inData, (unsigned long *)(inPtr), 
          inputFrameExtentForCurrentThread, mImagePixToVolumePix,
          str->ClipRectangleOrigin,str->ClipRectangleSize,
          str->FanAngles,str->FanOrigin,str->FanDepth, str->InterpolationMode, str->CalculationMode,
          accumulationBufferSaturationErrorsThread);
        break;
      default:
        LOG_ERROR("UnoptimizedInsertSlice: Unknown input ScalarType");
      }
    }
  }

  return VTK_THREAD_RETURN_VALUE;
}

//----------------------------------------------------------------------------
// For streaming and threads.  Splits output update extent into num pieces.
// This method needs to be called num times.  Results must not overlap for
// consistent starting extent.  Subclass can override this method.
// This method returns the number of pieces resulting from a successful split.
// This can be from 1 to "requestedNumberOfThreads".  
// If 1 is returned, the extent cannot be split.
int vtkPasteSliceIntoVolume::SplitSliceExtent(int splitExt[6], int fullExt[6], int threadId, int requestedNumberOfThreads)
{

  int min, max; // the min and max indices of the axis of interest

  LOG_TRACE("SplitSliceExtent: ( " << fullExt[0] << ", " 
    << fullExt[1] << ", "
    << fullExt[2] << ", " << fullExt[3] << ", "
    << fullExt[4] << ", " << fullExt[5] << "), " 
    << threadId << " of " << requestedNumberOfThreads);

  // start with same extent
  memcpy(splitExt, fullExt, 6 * sizeof(int));

  // determine which axis we should split along - preference is z, then y, then x
  // as long as we can possibly split along that axis (i.e. as long as z0 != z1)
  int splitAxis = 2; // the axis we should split along, try with z first
  min = fullExt[4];
  max = fullExt[5];
  while (min == max)
  {
    --splitAxis;
    // we cannot split if the input extent is something like [50, 50, 100, 100, 0, 0]!
    if (splitAxis < 0)
    { 
      LOG_DEBUG("Cannot split the extent to multiple threads");
      return 1;
    }
    min = fullExt[splitAxis*2];
    max = fullExt[splitAxis*2+1];
  }

  // determine the actual number of pieces that will be generated
  int range = max - min + 1;
  // split the range over the maximum number of threads
  int valuesPerThread = (int)ceil(range/(double)requestedNumberOfThreads);
  // figure out the largest thread id used
  int maxThreadIdUsed = (int)ceil(range/(double)valuesPerThread) - 1;
  // if we are in a thread that will work on part of the extent, then figure
  // out the range that this thread should work on
  if (threadId < maxThreadIdUsed)
  {
    splitExt[splitAxis*2] = splitExt[splitAxis*2] + threadId*valuesPerThread;
    splitExt[splitAxis*2+1] = splitExt[splitAxis*2] + valuesPerThread - 1;
  }
  if (threadId == maxThreadIdUsed)
  {
    splitExt[splitAxis*2] = splitExt[splitAxis*2] + threadId*valuesPerThread;
  }

  // return the number of threads used
  return maxThreadIdUsed + 1;
}

//****************************************************************************

char* vtkPasteSliceIntoVolume::GetInterpolationModeAsString(InterpolationType type)
{
  switch (type)
  {
  case NEAREST_NEIGHBOR_INTERPOLATION: return "NEAREST_NEIGHBOR";
  case LINEAR_INTERPOLATION: return "LINEAR";
  default:
    LOG_ERROR("Unknown interpolation option: "<<type);
    return "unknown";
  }
}

char* vtkPasteSliceIntoVolume::GetOutputScalarModeAsString(int type)
{
  switch (type)
  {
  // list obtained from: http://www.vtk.org/doc/nightly/html/vtkType_8h_source.html
  //case VTK_VOID:              return "VTK_VOID";
  //case VTK_BIT:               return "VTK_BIT";
  case VTK_CHAR:              return "VTK_CHAR";
  case VTK_UNSIGNED_CHAR:     return "VTK_UNSIGNED_CHAR";
  case VTK_SHORT:             return "VTK_SHORT";
  case VTK_UNSIGNED_SHORT:    return "VTK_UNSIGNED_SHORT";
  case VTK_INT:               return "VTK_INT";
  case VTK_UNSIGNED_INT:      return "VTK_UNSIGNED_INT";
  case VTK_LONG:              return "VTK_LONG";
  case VTK_UNSIGNED_LONG:     return "VTK_UNSIGNED_LONG";
  case VTK_FLOAT:             return "VTK_FLOAT";
  case VTK_DOUBLE:            return "VTK_DOUBLE";
  //case VTK_SIGNED_CHAR:       return "VTK_SIGNED_CHAR";
  //case VTK_ID_TYPE:           return "VTK_ID_TYPE";
  default:
    LOG_ERROR("Unknown output scalar mode option: "<<type);
    return "unknown";
  }
}

char* vtkPasteSliceIntoVolume::GetCalculationModeAsString(CalculationType type)
{
  switch (type)
  {
  case WEIGHTED_AVERAGE: return "WEIGHTED_AVERAGE";
  case MAXIMUM: return "MAXIMUM";
  default:
    LOG_ERROR("Unknown calculation option: "<<type);
    return "unknown";
  }
}

char* vtkPasteSliceIntoVolume::GetOptimizationModeAsString(OptimizationType type)
{
  switch (type)
  {
  case FULL_OPTIMIZATION: return "FULL";
  case PARTIAL_OPTIMIZATION: return "PARTIAL";
  case NO_OPTIMIZATION: return "NONE";
  default:
    LOG_ERROR("Unknown optimization option: "<<type);
    return "unknown";
  }
}

//----------------------------------------------------------------------------
bool vtkPasteSliceIntoVolume::FanClippingApplied()
{
  return this->FanAngles[0] != 0.0 || this->FanAngles[1] != 0.0;
}
