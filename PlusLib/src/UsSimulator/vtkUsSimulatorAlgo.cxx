/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkUsSimulatorAlgo.h"
#include "vtkImageAlgorithm.h"
#include "vtkInformation.h"
#include "vtkTransform.h"
#include "vtkPolyData.h"
#include "vtkPolyDataToImageStencil.h"
#include <vtkSmartPointer.h>
#include <vtkImageStencil.h>
#include <vtkPolyDataNormals.h>
#include <vtkTriangleFilter.h>
#include <vtkStripper.h>

#include "vtkInformationVector.h"
#include "vtkCellData.h"
#include "vtkDataArray.h"
#include "vtkImageData.h"
#include "vtkMultiThreader.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkImageStencilData.h"
//-----------------------------------------------------------------------------

const unsigned char vtkUsSimulatorAlgo::OUTVALSTENCILFOREGROUND = 155;

vtkCxxRevisionMacro(vtkUsSimulatorAlgo, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkUsSimulatorAlgo);

vtkUsSimulatorAlgo::vtkUsSimulatorAlgo()
{
  SetNumberOfInputPorts(1); 
  SetNumberOfOutputPorts(1); 

  this->StencilBackgroundImage = NULL; 
  this->ModelToImageMatrix= NULL; 

  this->OutputImage=vtkImageData::New();
}

int vtkUsSimulatorAlgo::FillInputPortInformation(int, vtkInformation * info)
{

  info->Set(vtkImageAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPolyData"); 
  return 1; 


}

int vtkUsSimulatorAlgo::FillOutputPortInformation(int, vtkInformation * info)
{
  info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkImageData"); 
  return 1; 
}

/*
void vtkUsSimulatorAlgo::SetStencilBackgroundImage (vtkImageData* stencilBackgroundImage)
{
this->StencilBackgroundImage = stencilBackgroundImage; 

}*/ 


int vtkUsSimulatorAlgo::RequestData(vtkInformation* request,vtkInformationVector** inputVector,vtkInformationVector* outputVector)
{
  if( this->ModelToImageMatrix== NULL)
  {
    LOG_ERROR(" No Model to US image transform specified " ); 
    return 1; 
  }
  if( this->StencilBackgroundImage== NULL)
  {
    LOG_ERROR(" background image necessary for stencil creation not specified " ); 
    return 1; 
  } 

  //Get input
  vtkInformation* inInfoPort = inputVector[0]->GetInformationObject(0);
  vtkInformation* outInfo = outputVector->GetInformationObject(0); 

  vtkPolyData* model_MODEL = vtkPolyData::SafeDownCast(inInfoPort->Get(vtkDataObject::DATA_OBJECT()));
  if(model_MODEL == NULL)
  {
    LOG_ERROR("Model specified is empty");
    return 1; 
  }

  vtkImageData* simulatedUsImage = vtkImageData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT())); 
  if(simulatedUsImage == NULL)
  {
    LOG_ERROR("vtkUsSimulatorAlgo output type is invalid");
    return 1; 
  }
  
  // Get the model points in the image coordinate system
  vtkSmartPointer<vtkPolyData> model_IMAGE;
  {
    vtkSmartPointer<vtkPolyDataNormals> normalFilter=vtkSmartPointer<vtkPolyDataNormals>::New();
    normalFilter->SetInput(model_MODEL);
    normalFilter->ConsistencyOn();

    // Make sure that we have a clean triangle polydata
    vtkSmartPointer<vtkTriangleFilter> triangle=vtkSmartPointer<vtkTriangleFilter>::New();
    triangle->SetInputConnection(normalFilter->GetOutputPort());

    // Convert to triangle strip
    vtkSmartPointer<vtkStripper> stripper=vtkSmartPointer<vtkStripper>::New();
    stripper->SetInputConnection(triangle->GetOutputPort());

    // Transform model points from the MODEL coordinate system to image coordinate system
    vtkSmartPointer<vtkTransform> modelToImageTransform = vtkSmartPointer<vtkTransform>::New(); 
    modelToImageTransform->SetMatrix(this->ModelToImageMatrix);   
    vtkSmartPointer<vtkTransformPolyDataFilter> transformModelFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
    transformModelFilter->SetInputConnection(stripper->GetOutputPort());
    transformModelFilter->SetTransform(modelToImageTransform);
    transformModelFilter->Update();
    model_IMAGE = transformModelFilter->GetOutput();
  }

  // Create PolyData to Image stencil
  vtkSmartPointer<vtkPolyDataToImageStencil> modelStencil = vtkSmartPointer<vtkPolyDataToImageStencil>::New();
  modelStencil->SetInput(model_IMAGE); 
  modelStencil->SetOutputSpacing(this->StencilBackgroundImage->GetSpacing()); 
  modelStencil->SetOutputOrigin(this->StencilBackgroundImage->GetOrigin());
  modelStencil->SetOutputWholeExtent(this->StencilBackgroundImage->GetExtent()); 
  modelStencil->Update(); 
  vtkImageStencilData *modelStencilOutput=modelStencil->GetOutput();

  // Create Image stencil
  vtkSmartPointer<vtkImageStencil> combineModelwithBackgroundStencil = vtkSmartPointer<vtkImageStencil>::New();
  combineModelwithBackgroundStencil->SetInput(this->StencilBackgroundImage);
  combineModelwithBackgroundStencil->SetStencil(modelStencilOutput);
  combineModelwithBackgroundStencil->ReverseStencilOff();
  combineModelwithBackgroundStencil->SetBackgroundValue(OUTVALSTENCILFOREGROUND);
  combineModelwithBackgroundStencil->Update();
  vtkImageData *combinedStencilOutput=combineModelwithBackgroundStencil->GetOutput();

  simulatedUsImage->DeepCopy(combinedStencilOutput); 
  this->OutputImage->DeepCopy(combinedStencilOutput); 

  return 1; 
}



void vtkUsSimulatorAlgo::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  os << indent << "Dummy value : "<< m_Dummy <<"\n";


}

vtkUsSimulatorAlgo::~vtkUsSimulatorAlgo()
{
  SetModelToImageMatrix(NULL); 
  SetStencilBackgroundImage(NULL);
  this->OutputImage->Delete();
  this->OutputImage=NULL;
}
