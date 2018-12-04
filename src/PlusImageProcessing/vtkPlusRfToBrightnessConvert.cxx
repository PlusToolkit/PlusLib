/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"

#include "vtkPlusRfToBrightnessConvert.h"

#include "vtkImageData.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkMath.h"

#include <math.h>

vtkStandardNewMacro(vtkPlusRfToBrightnessConvert);

const double MIN_BRIGHTNESS_VALUE=0.0;
const double MAX_BRIGHTNESS_VALUE=255.0;

//----------------------------------------------------------------------------
vtkPlusRfToBrightnessConvert::vtkPlusRfToBrightnessConvert()
{
  this->ImageType=US_IMG_TYPE_XX;
  this->BrightnessScale=10.0;
  this->NumberOfHilbertFilterCoeffs=64;
}

//----------------------------------------------------------------------------
vtkPlusRfToBrightnessConvert::~vtkPlusRfToBrightnessConvert()
{
}

//----------------------------------------------------------------------------
int vtkPlusRfToBrightnessConvert::RequestInformation(vtkInformation*,
                                    vtkInformationVector** inputVector,
                                    vtkInformationVector* outputVector)
{
  // get the info objects
  vtkInformation* outInfo = outputVector->GetInformationObject(0);
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
      
  int inExt[6]={0};
  inInfo->Get(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(), inExt);
  
  // Set the output extent to be the same as the input extent by default
  int outExt[6]={0};
  inInfo->Get(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(), outExt);
  
  // Update the output image extent depending on the RF encoding type
  switch (this->ImageType)
  {
  case US_IMG_BRIGHTNESS:  
    {
      // output is the same as the input
    }
    break;
  case US_IMG_RF_I_LINE_Q_LINE:  
    {
      // RF data: IIIIII..., QQQQQQ....
      // B-mode data: BBBBBB
      // => number of rows in the output image is half of the rows in the input image
      int numberOfBmodeRows=(inExt[3]-inExt[2]+1)/2;
      outExt[2] = inExt[2]/2;
      outExt[3] = outExt[2] + numberOfBmodeRows - 1;
    }
    break;
  case US_IMG_RF_REAL:
    {
      // RF data: III..., III...
      // B-mode data: BBB..., BBB...
      // => the output image size is the same as the input image size
      // keep the default extent
    }
    break;
  case US_IMG_RF_IQ_LINE:
    {
      // RF data: IQIQIQ....., IQIQIQ.....
      // B-mode data: BBB..., BBB...
      // => number of columns in the output image is half of the columns in the input image
      int numberOfBmodeColumns=(inExt[1]-inExt[0]+1)/2;
      outExt[0] = inExt[0]/2;
      outExt[1] = outExt[0] + numberOfBmodeColumns - 1;
    }
    break;
  default:
    vtkErrorMacro("Unknown RF image type: " << this->ImageType);
    return 0;
    }

  // Set the updated output image size
  outInfo->Set(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(),outExt,6);

  // Output is B-mode image, the pixel type is always unsigned 8-bit integer
  vtkDataObject::SetPointDataActiveScalarInfo(outInfo, VTK_UNSIGNED_CHAR, -1);

  return 1;
}

//----------------------------------------------------------------------------
void vtkPlusRfToBrightnessConvert::ThreadedRequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *vtkNotUsed(outputVector),
  vtkImageData ***inData,
  vtkImageData **outData,
  int outExt[6], int id)
{  
  if (outData[0]->GetScalarType() != VTK_UNSIGNED_CHAR)
  {
    vtkErrorMacro("Expecting VTK_UNSIGNED_CHAR output pixel type");
    return;
  }  

  // Check input and output parameters
  if (inData[0][0]->GetNumberOfScalarComponents() != 1)
  {
    vtkErrorMacro("Expecting 1 components not " << inData[0][0]->GetNumberOfScalarComponents());
    return;
  }
  if (this->ImageType!=US_IMG_BRIGHTNESS && inData[0][0]->GetScalarType() != VTK_SHORT && inData[0][0]->GetScalarType() != VTK_INT)
  {
    vtkErrorMacro("Expecting VTK_SHORT or VTK_INT as input pixel type, or US_IMG_BRIGHTNESS as image type");
    return;
  }

  int inExt[6]={outExt[0], outExt[1], outExt[2], outExt[3], outExt[4], outExt[5]};
  // Get the input extent for the output extent
  switch (this->ImageType)
  {
  case US_IMG_BRIGHTNESS:  
    // => the output image size is the same as the input image size
    // keep the default extent
    break;
  case US_IMG_RF_I_LINE_Q_LINE:  
    {
      // RF data: IIIIII..., QQQQQQ....
      // B-mode data: BBBBBB
      // => number of rows in the output image is half of the rows in the input image
      int numberOfBmodeRows=outExt[3]-outExt[2]+1;
      int numberOfRfRows=numberOfBmodeRows*2;
      inExt[2] = outExt[2]*2;
      inExt[3] = inExt[2] + numberOfRfRows - 1;
    }
    break;
  case US_IMG_RF_REAL:
    {
      // RF data: III..., III...
      // B-mode data: BBB..., BBB...
      // => the output image size is the same as the input image size
      // keep the default extent
    }
    break;
  case US_IMG_RF_IQ_LINE:
    {
      // RF data: IQIQIQ....., IQIQIQ.....
      // B-mode data: BBB..., BBB...
      // => number of columns in the output image is half of the columns in the input image
      int numberOfBmodeColumns=outExt[1]-outExt[0]+1;
      int numberOfRfColumns=numberOfBmodeColumns*2;
      inExt[0] = outExt[0]*2;
      inExt[1] = inExt[0] + numberOfRfColumns - 1;
    }
    break;
  default:
    vtkErrorMacro("Unknown RF image type: " << this->ImageType);
    }

  if (inData[0][0]->GetScalarType() == VTK_SHORT)
  {
    ThreadedLineByLineHilbertTransform<short>(inExt, outExt, inData, outData, id);
  }
  else if (inData[0][0]->GetScalarType() == VTK_INT)
  {
    ThreadedLineByLineHilbertTransform<int>(inExt, outExt, inData, outData, id);
  }
  else if (inData[0][0]->GetScalarType() == VTK_UNSIGNED_CHAR)
  {
    ThreadedLineByLineHilbertTransform<unsigned char>(inExt, outExt, inData, outData, id);
  }
  else
  {
    vtkErrorMacro("Internal error: unexpected branching");
  }
}

template<typename ScalarType>
void vtkPlusRfToBrightnessConvert::ThreadedLineByLineHilbertTransform(int inExt[6], int outExt[6], vtkImageData ***inData, vtkImageData **outData, int threadId)
{
  vtkIdType inInc0=0;
  vtkIdType inInc1=0;
  vtkIdType inInc2=0;
  inData[0][0]->GetContinuousIncrements(inExt, inInc0, inInc1, inInc2);
  ScalarType *inPtr = static_cast<ScalarType*>(inData[0][0]->GetScalarPointerForExtent(inExt));

  vtkIdType outInc0 = 0;
  vtkIdType outInc1 = 0;
  vtkIdType outInc2 = 0;
  outData[0]->GetContinuousIncrements(outExt, outInc0, outInc1, outInc2);
  unsigned char *outPtr = static_cast<unsigned char*>(outData[0]->GetScalarPointerForExtent(outExt));

  unsigned long target = static_cast<unsigned long>((outExt[5]-outExt[4]+1)*(outExt[3]-outExt[2]+1)/50.0);
  target++;

  // Temporary buffer to hold Hilbert transform results
  int numberOfRfSamplesInScanline=inExt[1]-inExt[0]+1;
  int numberOfBmodeSamplesInScanline=outExt[1]-outExt[0]+1;

  bool imageTypeValid=true;
  unsigned long count = 0;
  // loop over all the pixels (keeping track of normalized distance to origin.
  if (this->ImageType==US_IMG_BRIGHTNESS)
  {
    // TODO: we copy the pixels, probably we could return the input image on the output in a more efficient manner
    unsigned char* inPtrByte = static_cast<unsigned char*>(inData[0][0]->GetScalarPointerForExtent(inExt));
    for (int idx2 = outExt[4]; idx2 <= outExt[5]; ++idx2)
    {
      for (int idx1 = outExt[2]; !this->AbortExecute && idx1 <= outExt[3]; ++idx1)    
      {
        if (threadId==0)
        {
          // it is the first thread, report progress
          if (!(count%target))
          {
            this->UpdateProgress(count/(50.0*target));
          }
          count++;
        }
        memcpy(outPtr,inPtrByte,numberOfRfSamplesInScanline+inInc1);
        inPtrByte += numberOfRfSamplesInScanline+inInc1;
        outPtr += numberOfRfSamplesInScanline+outInc1;
      }
      inPtrByte += inInc2;
      outPtr += outInc2;    
    }
    return;
  }

  ScalarType* hilbertTransformBuffer = new ScalarType[numberOfRfSamplesInScanline + 1];
  for (int idx2 = outExt[4]; idx2 <= outExt[5]; ++idx2)
  {
    for (int idx1 = outExt[2]; !this->AbortExecute && idx1 <= outExt[3]; ++idx1)    
    {
      if (threadId==0)
      {
        // it is the first thread, report progress
        if (!(count%target))
        {
          this->UpdateProgress(count/(50.0*target));
        }
        count++;
      }

      switch (this->ImageType)
      {
      case US_IMG_RF_I_LINE_Q_LINE:
        {
          // e.g., Ultrasonix
          // RF data: IIIIIII..., QQQQQQ...., IIIIIII..., QQQQQQ....
          ScalarType *originalSignal=inPtr;
          inPtr += numberOfRfSamplesInScanline+inInc1;
          ScalarType *phaseShiftedSignal=inPtr;
          inPtr += numberOfRfSamplesInScanline+inInc1;
          ComputeAmplitudeILineQLine(outPtr, originalSignal, phaseShiftedSignal, numberOfRfSamplesInScanline);
          outPtr += numberOfBmodeSamplesInScanline+outInc1;          
        }
        break;
      case US_IMG_RF_REAL:
        {
          // e.g., Ultrasonix
          // RF data: IIIII..., IIIII...
          ComputeHilbertTransform(hilbertTransformBuffer, inPtr, numberOfRfSamplesInScanline);
          ComputeAmplitudeILineQLine(outPtr, inPtr, hilbertTransformBuffer, numberOfRfSamplesInScanline);
          inPtr += numberOfRfSamplesInScanline+inInc1;
          outPtr += numberOfBmodeSamplesInScanline+outInc1;
        }
        break;
      case US_IMG_RF_IQ_LINE:
        {
          // RF data: IQIQIQ....., IQIQIQIQ.....
          ComputeAmplitudeIqLine(outPtr, inPtr, numberOfRfSamplesInScanline);
          inPtr += numberOfRfSamplesInScanline+inInc1;
          outPtr += numberOfBmodeSamplesInScanline+outInc1;
        }
        break;
      default:
        imageTypeValid=false;
      }
    }
    inPtr += inInc2;
    outPtr += outInc2;    
  }
  if (!imageTypeValid)
  {
    LOG_ERROR("Unsupported image type for brightness conversion: "<<igsioVideoFrame::GetStringFromUsImageType(this->ImageType));
  }

  delete[] hilbertTransformBuffer;
  hilbertTransformBuffer=NULL;
}

void vtkPlusRfToBrightnessConvert::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusRfToBrightnessConvert::ReadConfiguration(vtkXMLDataElement* rfToBrightnessElement)
{
  LOG_TRACE("vtkPlusRfToBrightnessConvert::ReadConfiguration"); 
  XML_VERIFY_ELEMENT(rfToBrightnessElement, "RfToBrightnessConversion");
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(int, NumberOfHilbertFilterCoeffs, rfToBrightnessElement);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(double, BrightnessScale, rfToBrightnessElement);
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkPlusRfToBrightnessConvert::WriteConfiguration(vtkXMLDataElement* rfToBrightnessElement)
{
  LOG_TRACE("vtkPlusRfToBrightnessConvert::WriteConfiguration"); 

  XML_VERIFY_ELEMENT(rfToBrightnessElement, "RfToBrightnessConversion");

  rfToBrightnessElement->SetDoubleAttribute("NumberOfHilbertFilterCoeffs", this->NumberOfHilbertFilterCoeffs);
  rfToBrightnessElement->SetDoubleAttribute("BrightnessScale", this->BrightnessScale);

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
void vtkPlusRfToBrightnessConvert::ComputeHilbertTransformCoeffs()
{
  if ((int)(this->HilbertTransformCoeffs.size())==this->NumberOfHilbertFilterCoeffs+1)
  {
    // already computed the requested number of Hilbert transform coefficients
    return;
  }

  this->HilbertTransformCoeffs.resize(this->NumberOfHilbertFilterCoeffs+1);
  for (int i=1; i<=this->NumberOfHilbertFilterCoeffs; i++)
  {
    // From http://www.vbforums.com/archive/index.php/t-639223.html
    this->HilbertTransformCoeffs[i]=1/((i-this->NumberOfHilbertFilterCoeffs/2)-0.5)/vtkMath::Pi();
  }
  
  bool debugOutput=false; // print Hilbert transform coefficients in Matlab format
  if (debugOutput)
  {
    std::cout << "hc = [ ";
    for (int i=1; i<=this->NumberOfHilbertFilterCoeffs; i++)
    {
      std::cout << this->HilbertTransformCoeffs[i];
      if (i<this->NumberOfHilbertFilterCoeffs)
      {
        std::cout << ";";
      }
      if (i%6==0)
      {
        std::cout << "\n";
      }
    }
    std::cout << "]\n";
  }
}

template<typename ScalarType>
PlusStatus vtkPlusRfToBrightnessConvert::ComputeHilbertTransform(ScalarType *hilbertTransformOutput, ScalarType *input, int npt)
{
  ComputeHilbertTransformCoeffs(); // update the transform coefficients if needed

  if (npt < this->NumberOfHilbertFilterCoeffs)
  {
    LOG_ERROR("Insufficient data for performing Hilbert transform");
    return PLUS_FAIL;
  }

  // Compute Hilbert transform by convolution
  for (int l=1; l<=npt-this->NumberOfHilbertFilterCoeffs+1; l++) 
  {
    double yt = 0.0;
    for (int i=1; i<=this->NumberOfHilbertFilterCoeffs; i++) 
    {
      yt += input[l+i-1]*this->HilbertTransformCoeffs[this->NumberOfHilbertFilterCoeffs+1-i];
    }
    hilbertTransformOutput[l] = yt;
  }

  // Shift this->NumberOfHilbertFilterCoeffs/1+1/2 points
  for (int i=1; i<=npt-this->NumberOfHilbertFilterCoeffs; i++) 
  {
    hilbertTransformOutput[i] = 0.5*(hilbertTransformOutput[i]+hilbertTransformOutput[i+1]);
  }
  for (int i=npt-this->NumberOfHilbertFilterCoeffs; i>=1; i--)
  {
    hilbertTransformOutput[i+this->NumberOfHilbertFilterCoeffs/2]=hilbertTransformOutput[i];
  }

  // Pad by zeros 
  for (int i=1; i<=this->NumberOfHilbertFilterCoeffs/2; i++) 
  {
    hilbertTransformOutput[i] = 0.0;
    hilbertTransformOutput[npt+1-i] = 0.0;
  }

  return PLUS_SUCCESS;
}

template<typename ScalarType>
void vtkPlusRfToBrightnessConvert::ComputeAmplitudeILineQLine(unsigned char *ampl, ScalarType *inputSignal, ScalarType *inputSignalHilbertTransformed, int npt)
{
  for (int i=0; i<this->NumberOfHilbertFilterCoeffs/2+1; i++)
  {
    ampl[i]=0;
  }
  for (int i=this->NumberOfHilbertFilterCoeffs/2+1; i<=npt-this->NumberOfHilbertFilterCoeffs/2; i++) 
  {
    double xt = inputSignal[i];
    double xht = inputSignalHilbertTransformed[i];
    double brightnessValue = sqrt(sqrt(sqrt(xt*xt+xht*xht)))*this->BrightnessScale;
    if (brightnessValue>MAX_BRIGHTNESS_VALUE) brightnessValue=MAX_BRIGHTNESS_VALUE;
    if (brightnessValue<MIN_BRIGHTNESS_VALUE) brightnessValue=MIN_BRIGHTNESS_VALUE;
    ampl[i]=brightnessValue;
    /*
    If needed, the phase could be computed as follows:
    phase[i] = atan2(xht ,xt);
    omega[i] = phase[i]-phase[i-1];
    if (omega[i]<0)
    {
      omega[i]+=2*pi;
    }
    */
  }
  for (int i=npt-this->NumberOfHilbertFilterCoeffs/2+1; i<npt; i++)
  {
    ampl[i]=0;
  }
}

template<typename ScalarType>
void vtkPlusRfToBrightnessConvert::ComputeAmplitudeIqLine(unsigned char *ampl, ScalarType *inputSignal, const int npt)
{
  int inputIndex=0;
  int outputIndex=0;
  int numberOfIqPairs=floor(double(npt)/2);
  for (int i=0; i<numberOfIqPairs; i++) 
  {
    double xt = inputSignal[inputIndex++];
    double xht = inputSignal[inputIndex++];
    double outputValue=sqrt(sqrt(sqrt(xt*xt+xht*xht)))*this->BrightnessScale;
    if (outputValue>MAX_BRIGHTNESS_VALUE) outputValue=MAX_BRIGHTNESS_VALUE;
    if (outputValue<MIN_BRIGHTNESS_VALUE) outputValue=MIN_BRIGHTNESS_VALUE;
    ampl[outputIndex++] = outputValue;
  }
}
