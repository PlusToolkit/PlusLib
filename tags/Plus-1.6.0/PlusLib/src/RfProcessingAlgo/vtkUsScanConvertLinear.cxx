/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#include "PlusCommon.h"

#include "vtkUsScanConvertLinear.h"

#include "vtkObjectFactory.h"
#include "vtkXMLDataElement.h"
#include "vtkImageReslice.h"
#include "vtkImageData.h"
#include "vtkAlgorithmOutput.h"

vtkStandardNewMacro(vtkUsScanConvertLinear);

//----------------------------------------------------------------------------
vtkUsScanConvertLinear::vtkUsScanConvertLinear()
{
  this->OutputImageExtent[0]=0;
  this->OutputImageExtent[1]=599;
  this->OutputImageExtent[2]=0;
  this->OutputImageExtent[3]=799;
  this->OutputImageExtent[4]=0; // not used
  this->OutputImageExtent[5]=1; // not used
  this->OutputImageSpacing[0]=0.2;
  this->OutputImageSpacing[1]=0.2;
  this->OutputImageSpacing[2]=1.0; // not used

  this->ImagingDepthMm=50.0;
  this->TransducerWidthMm=38.0;

  this->ImageReslice=vtkImageReslice::New();  
}

//----------------------------------------------------------------------------
vtkUsScanConvertLinear::~vtkUsScanConvertLinear()
{
  this->ImageReslice->Delete();
  this->ImageReslice=NULL;  
}

void vtkUsScanConvertLinear::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "OutputImageExtent: ("
    << this->OutputImageExtent[0] <<", "<< this->OutputImageExtent[1] <<", "
    << this->OutputImageExtent[2] <<", "<< this->OutputImageExtent[3] <<")\n";
  os << indent << "OutputImageSpacing: ("<< this->OutputImageSpacing[0] <<", "<< this->OutputImageSpacing[1] <<")\n";
  os << indent << "OutputImageStartDepthMm: "<< this->OutputImageStartDepthMm << "\n";  
  os << indent << "ImagingDepthMm: "<< this->ImagingDepthMm << "\n";
  os << indent << "TransducerWidthMm: "<< this->TransducerWidthMm << "\n";
}

//-----------------------------------------------------------------------------
void vtkUsScanConvertLinear::SetInputConnection(vtkAlgorithmOutput* input)
{
  this->ImageReslice->SetInputConnection(input);
}

//-----------------------------------------------------------------------------
void vtkUsScanConvertLinear::Update()
{
  this->ImageReslice->SetOutputExtent(this->OutputImageExtent);
  // In Plus the convention is that the image coordinate system has always unit spacing and zero origin
  this->ImageReslice->SetOutputSpacing(1.0, 1.0, 1.0);

  // The direction cosines give the x, y, and z axes for the output volume.
  // Updating whole extent is needed when the requested extent is smaller than the producer's whole extent
  this->ImageReslice->GetInputConnection(0,0)->GetProducer()->UpdateWholeExtent();
  vtkImageData* inputImage=this->ImageReslice->GetImageDataInput(0);  
  if (inputImage==NULL)
  {
    LOG_ERROR("vtkUsScanConvertLinear::Update failed: no input image is specified");
    return;
  }
  int scanLineLength=inputImage->GetExtent()[1]-inputImage->GetExtent()[0]+1;
  int numberOfScanLines=inputImage->GetExtent()[3]-inputImage->GetExtent()[2]+1;

  // xVec: controls the width of the output image, if larger then image becomes narrower
  double inputWidthSpacing=this->TransducerWidthMm/static_cast<double>(numberOfScanLines);
  double xVec[3]={0, (this->OutputImageSpacing[0]/inputWidthSpacing), 0};
  
  // yVec: controls the height of the output image, if larger then image becomes shorter
  double inputDepthSpacing=this->ImagingDepthMm/static_cast<double>(scanLineLength);
  double yVec[3]={this->OutputImageSpacing[1]/inputDepthSpacing, 0, 0};

  double zVec[3]={0,0,1.0};
  this->ImageReslice->SetResliceAxesDirectionCosines(xVec, yVec, zVec);

  this->ImageReslice->SetOutputOrigin(0,0,0);

  this->ImageReslice->Update();
}

//-----------------------------------------------------------------------------
vtkImageData* vtkUsScanConvertLinear::GetOutput()
{
  return this->ImageReslice->GetOutput();
}

//-----------------------------------------------------------------------------
PlusStatus vtkUsScanConvertLinear::ReadConfiguration(vtkXMLDataElement* scanConversionElement)
{
  LOG_TRACE("vtkUsScanConvertLinear::ReadConfiguration"); 
  if ( scanConversionElement == NULL )
  {
    LOG_DEBUG("Unable to configure vtkUsScanConvertLinear! (XML data element is NULL)"); 
    return PLUS_FAIL; 
  }
  if (STRCASECMP(scanConversionElement->GetName(), "ScanConversion")!=NULL)
  {
    LOG_ERROR("Cannot read vtkUsScanConvertLinear configuration: ScanConversion element is expected"); 
    return PLUS_FAIL;
  }

  int OutputImageStartDepthMm=0;
  if ( scanConversionElement->GetScalarAttribute("OutputImageStartDepthMm", OutputImageStartDepthMm)) 
  {
    this->OutputImageStartDepthMm=OutputImageStartDepthMm; 
  }
  
  double outputImageSpacing[2]={0};
  if ( scanConversionElement->GetVectorAttribute("OutputImageSpacingMmPerPixel", 2, outputImageSpacing)) 
  {
    this->OutputImageSpacing[0]=outputImageSpacing[0]; 
    this->OutputImageSpacing[1]=outputImageSpacing[1]; 
    this->OutputImageSpacing[2]=1; 
  }

  double outputImageSize[2]={0};
  if ( scanConversionElement->GetVectorAttribute("OutputImageSizePixel", 2, outputImageSize)) 
  {
    this->OutputImageExtent[0]=0;
    this->OutputImageExtent[1]=outputImageSize[0]-1;
    this->OutputImageExtent[2]=0;
    this->OutputImageExtent[3]=outputImageSize[1]-1;
    this->OutputImageExtent[4]=0;
    this->OutputImageExtent[5]=1;
  }

  double imagingDepthMm=0;
  if ( scanConversionElement->GetScalarAttribute("ImagingDepthMm", imagingDepthMm)) 
  {
    this->ImagingDepthMm=imagingDepthMm; 
  }

  double transducerWidthMm=0;
  if ( scanConversionElement->GetScalarAttribute("TransducerWidthMm", transducerWidthMm)) 
  {
    this->TransducerWidthMm=transducerWidthMm; 
  }
 
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkUsScanConvertLinear::WriteConfiguration(vtkXMLDataElement* scanConversionElement)
{
  LOG_TRACE("vtkUsScanConvertLinear::WriteConfiguration"); 
  if ( scanConversionElement == NULL )
  {
    LOG_DEBUG("Unable to write vtkUsScanConvertLinear: XML data element is NULL"); 
    return PLUS_FAIL; 
  }
  if (STRCASECMP(scanConversionElement->GetName(), "ScanConversion")!=NULL)
  {
    LOG_ERROR("Cannot write vtkUsScanConvertLinear configuration: ScanConversion element is expected"); 
    return PLUS_FAIL;
  }  
  
  scanConversionElement->SetDoubleAttribute("OutputImageStartDepthMm", this->OutputImageStartDepthMm);

  scanConversionElement->SetVectorAttribute("OutputImageSpacingMmPerPixel", 2, this->OutputImageSpacing);

  double outputImageSize[2]=
  {
    this->OutputImageExtent[1]-this->OutputImageExtent[0]+1,
    this->OutputImageExtent[3]-this->OutputImageExtent[2]+1
  };
  scanConversionElement->SetVectorAttribute("OutputImageSizePixel", 2, outputImageSize);

  scanConversionElement->SetDoubleAttribute("ImagingDepthMm", this->ImagingDepthMm);
  scanConversionElement->SetDoubleAttribute("TransducerWidthMm", this->TransducerWidthMm);

  return PLUS_SUCCESS;
}

