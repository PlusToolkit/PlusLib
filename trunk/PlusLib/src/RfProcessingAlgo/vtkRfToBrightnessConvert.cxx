/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"

#include "vtkRfToBrightnessConvert.h"

#include "vtkImageData.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkMath.h"

#include <math.h>

vtkStandardNewMacro(vtkRfToBrightnessConvert);

//----------------------------------------------------------------------------
vtkRfToBrightnessConvert::vtkRfToBrightnessConvert()
{
  this->BrightnessScale=10.0;
  this->NumberOfHilberFilterCoeffs=64; // 128;
  PrepareHilbertTransform();
}

//----------------------------------------------------------------------------
void vtkRfToBrightnessConvert::ThreadedRequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *vtkNotUsed(outputVector),
  vtkImageData ***inData,
  vtkImageData **outData,
  int ext[6], int id)
{  
  // Check input and output parameters
  if (inData[0][0]->GetNumberOfScalarComponents() != 1)
  {
    vtkErrorMacro("Expecting 1 components not " << inData[0][0]->GetNumberOfScalarComponents());
    return;
  }
  if (inData[0][0]->GetScalarType() != VTK_SHORT || 
    outData[0]->GetScalarType() != VTK_SHORT)
  {
    vtkErrorMacro("Expecting input and output to be of type short");
    return;
  }

  int wholeExtent[6]={0};
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);  
  inInfo->Get(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(), wholeExtent);

  short *inPtr = static_cast<short*>(inData[0][0]->GetScalarPointerForExtent(ext));
  short *outPtr = static_cast<short*>(outData[0]->GetScalarPointerForExtent(ext));

  vtkIdType inInc0=0;
  vtkIdType inInc1=0;
  vtkIdType inInc2=0;
  inData[0][0]->GetContinuousIncrements(ext, inInc0, inInc1, inInc2);
  vtkIdType outInc0=0;
  vtkIdType outInc1=0;
  vtkIdType outInc2=0;
  outData[0]->GetContinuousIncrements(ext, outInc0, outInc1, outInc2);  

  unsigned long target = static_cast<unsigned long>((ext[5]-ext[4]+1)*(ext[3]-ext[2]+1)/50.0);
  target++;

  // Temporary buffer to hold Hilbert transform results
  int numberOfSamplesInScanline=ext[1]-ext[0]+1;
  short* hilbertTransformBuffer=new short[numberOfSamplesInScanline+1];
  /*
  std::vector<short> hilbertTransformBuffer;
  hilbertTransformBuffer.resize(numberOfSamplesInScanline);
  */

  unsigned long count = 0;
  // loop over all the pixels (keeping track of normalized distance to origin.
  for (int idx2 = ext[4]; idx2 <= ext[5]; ++idx2)
  {
    //for (int idx1 = ext[2]; !this->AbortExecute && idx1 <= ext[3]; ++idx1)
    for (int idx1 = ext[2]; !this->AbortExecute && idx1 <= ext[3]; idx1+=2)
    {
      if (id==0)
      {
        // it is the first thread, report progress
        if (!(count%target))
        {
          this->UpdateProgress(count/(50.0*target));
        }
        count++;
      }

      // Modes: IQIQIQ....., IQIQIQIQ..... / IIIIIII..., QQQQQQ.... / IIIII..., IIIII...
      ComputeHilbertTransform(hilbertTransformBuffer, inPtr, numberOfSamplesInScanline);
      //ComputeAmplitude(outPtr, inPtr, hilbertTransformBuffer, numberOfSamplesInScanline);
      //short *phaseShiftedSignal=inPtr+numberOfSamplesInScanline+inInc1;
      short *phaseShiftedSignal=hilbertTransformBuffer;
      ComputeAmplitude(outPtr, inPtr, phaseShiftedSignal, numberOfSamplesInScanline);
      outPtr += numberOfSamplesInScanline+outInc1;
      ComputeAmplitude(outPtr, inPtr, phaseShiftedSignal, numberOfSamplesInScanline);

      inPtr += 2*(numberOfSamplesInScanline+inInc1);
      outPtr += numberOfSamplesInScanline+outInc1;
    }
    inPtr += inInc2;
    outPtr += outInc2;    
  }

  delete[] hilbertTransformBuffer;
  hilbertTransformBuffer=NULL;
}

void vtkRfToBrightnessConvert::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//-----------------------------------------------------------------------------
PlusStatus vtkRfToBrightnessConvert::ReadConfiguration(vtkXMLDataElement* config)
{
  LOG_TRACE("vtkRfToBrightnessConvert::ReadConfiguration"); 
  if ( config == NULL )
  {
    LOG_ERROR("Unable to configure vtkRfToBrightnessConvert! (XML data element is NULL)"); 
    return PLUS_FAIL; 
  }
  vtkXMLDataElement* rfProcessingElement = config->FindNestedElementWithName("RfProcessing"); 
  if (rfProcessingElement == NULL)
  {
    LOG_ERROR("Unable to find RfProcessing element in XML tree!"); 
    return PLUS_FAIL;
  }
  vtkXMLDataElement* rfToBrightnessElement = rfProcessingElement->FindNestedElementWithName("RfToBrightnessConversion"); 
  if (rfToBrightnessElement == NULL)
  {
    LOG_ERROR("Unable to find RfProcessing/RfToBrightnessConversion element in XML tree!"); 
    return PLUS_FAIL;
  }  

  int numberOfHilberFilterCoeffs=0;
  if ( rfToBrightnessElement->GetScalarAttribute("NumberOfHilberFilterCoeffs", numberOfHilberFilterCoeffs)) 
  {
    this->NumberOfHilberFilterCoeffs=numberOfHilberFilterCoeffs; 
  }

  double brightnessScale=0;
  if ( rfToBrightnessElement->GetScalarAttribute("BrightnessScale", brightnessScale)) 
  {
    this->BrightnessScale=brightnessScale; 
  }

  return PLUS_SUCCESS;
}




// From http://www.vbforums.com/archive/index.php/t-639223.html

 
void vtkRfToBrightnessConvert::PrepareHilbertTransform()
{
  this->HilbertTransformCoeffs.resize(this->NumberOfHilberFilterCoeffs+1);
  std::cout << "hc = [ ";
  for (int i=1; i<=this->NumberOfHilberFilterCoeffs; i++)
  {
    this->HilbertTransformCoeffs[i]=1/((i-this->NumberOfHilberFilterCoeffs/2)-0.5)/vtkMath::Pi();

    std::cout << this->HilbertTransformCoeffs[i];
    if (i<this->NumberOfHilberFilterCoeffs)
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

  // time, x: input vectors
  // npt: number of input points
  // xh, ampl, phase, omega: output vectors
PlusStatus vtkRfToBrightnessConvert::ComputeHilbertTransform(short *hilbertTransformOutput, short *input, int npt)
{
  double pi=vtkMath::Pi();
  double pi2=2*pi;

  if (npt < this->NumberOfHilberFilterCoeffs)
  {
    LOG_ERROR("Insufficient data for performing Hilbert transform");
    return PLUS_FAIL;
  }

  // Compute Hilbert transform by convolution
  for (int l=1; l<=npt-this->NumberOfHilberFilterCoeffs+1; l++) 
  {
    double yt = 0.0;
    for (int i=1; i<=this->NumberOfHilberFilterCoeffs; i++) 
    {
      yt += input[l+i-1]*this->HilbertTransformCoeffs[this->NumberOfHilberFilterCoeffs+1-i];
    }
    hilbertTransformOutput[l] = yt;
  }

  // Shift this->NumberOfHilberFilterCoeffs/1+1/2 points
  for (int i=1; i<=npt-this->NumberOfHilberFilterCoeffs; i++) 
  {
    hilbertTransformOutput[i] = 0.5*(hilbertTransformOutput[i]+hilbertTransformOutput[i+1]);
  }
  for (int i=npt-this->NumberOfHilberFilterCoeffs; i>=1; i--)
  {
    hilbertTransformOutput[i+this->NumberOfHilberFilterCoeffs/2]=hilbertTransformOutput[i];
  }

  // Pad by zeros 
  for (int i=1; i<=this->NumberOfHilberFilterCoeffs/2; i++) 
  {
    hilbertTransformOutput[i] = 0.0;
    hilbertTransformOutput[npt+1-i] = 0.0;
  }

  return PLUS_SUCCESS;
}

void vtkRfToBrightnessConvert::ComputeAmplitude(short *ampl, short *inputSignal, short *inputSignalHilbertTransformed, int npt)
{
  for (int i=0; i<this->NumberOfHilberFilterCoeffs/2+1; i++)
  {
    ampl[i]=0;
  }
  for (int i=this->NumberOfHilberFilterCoeffs/2+1; i<=npt-this->NumberOfHilberFilterCoeffs/2; i++) 
  {
    double xt = inputSignal[i];
    double xht = inputSignalHilbertTransformed[i];
    ampl[i] = sqrt(sqrt(sqrt(xt*xt+xht*xht)))*this->BrightnessScale;
    if (ampl[i]>255.0) ampl[i]=255.0;
    if (ampl[i]<0.0) ampl[i]=0.0;

    /*
    phase[i] = atan2(xht ,xt);
    if (phase[i] < phase[i-1])
    {
      omega[i] = phase[i]-phase[i-1]+pi2;
    }
    else
    {
      omega[i] = phase[i]-phase[i-1];
    }
    */
  }
  for (int i=npt-this->NumberOfHilberFilterCoeffs/2+1; i<npt; i++)
  {
    ampl[i]=0;
  }
}

