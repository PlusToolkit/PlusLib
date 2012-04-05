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

#include "vtkInformationVector.h"
#include "vtkCellData.h"
#include "vtkDataArray.h"
#include "vtkImageData.h"
#include "vtkMultiThreader.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkImageActor.h"
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"
#include "vtkRenderWindowInteractor.h"

#include "vtkInteractorStyleImage.h"
//-----------------------------------------------------------------------------

const unsigned char vtkUsSimulatorAlgo::OUTVALSTENCILFOREGROUND = 0;

vtkCxxRevisionMacro(vtkUsSimulatorAlgo, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkUsSimulatorAlgo);

vtkUsSimulatorAlgo::vtkUsSimulatorAlgo()
{
  this->SetNumberOfInputPorts(1); 
  this->SetNumberOfOutputPorts(1); 

  this->StencilBackgroundImage = NULL; 
  this->ModelToImageMatrix= NULL; 

  //vtkSmartPointer<vtkMatrix4x4> modelToImageMatrix = vtkSmartPointer<vtkMatrix4x4>::New(); 
  //this->SetModelToImageMatrix(modelToImageMatrix); 

  //vtkSmartPointer<vtkImageData> stencilBackgroundImage = vtkSmartPointer<vtkImageData>::New(); 
  //this->SetStencilBackgroundImage(stencilBackgroundImage); 
 

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
  //Get input
  vtkInformation* inInfoPort = inputVector[0]->GetInformationObject(0);
  vtkInformation* outInfo = outputVector->GetInformationObject(0); 

  
  vtkSmartPointer<vtkPolyData> model = vtkPolyData::SafeDownCast(inInfoPort->Get(vtkDataObject::DATA_OBJECT()));
  vtkSmartPointer<vtkImageData> simulatedUsImage = vtkImageData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT())); 

  // align model with US image
  
  
 if( ModelToImageMatrix== NULL)
  {
    LOG_ERROR(" No Model to US image transform specified " ); 
    return 1; 
  }
 if(model == NULL)
 {
   LOG_ERROR("Model specified is empty");
   return 1; 
 }

 vtkSmartPointer<vtkTransform> modelToImageTransform = vtkSmartPointer<vtkTransform>::New(); 
 modelToImageTransform->SetMatrix(ModelToImageMatrix); 
  
  vtkSmartPointer<vtkPolyData> alignedModel = vtkSmartPointer<vtkPolyData>::New(); 

  vtkSmartPointer<vtkTransformPolyDataFilter> transformModelFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
  transformModelFilter->SetInput(model);
  transformModelFilter->SetTransform(modelToImageTransform);
  transformModelFilter->Update();

  alignedModel->DeepCopy(transformModelFilter->GetOutputDataObject(0)); 
  // 


 
  
  
  if( StencilBackgroundImage== NULL)
  {
    LOG_ERROR(" background image necessary for stencil creation not specified " ); 
    return 1; 
  } 

  //Create PolyData to Image stencil
  
  vtkSmartPointer<vtkPolyDataToImageStencil> modelStencil = 
  vtkSmartPointer<vtkPolyDataToImageStencil>::New();

  modelStencil->SetInput(model); 
  modelStencil->SetOutputSpacing(StencilBackgroundImage->GetSpacing()); 
  modelStencil->SetOutputOrigin(StencilBackgroundImage->GetOrigin()); //think about this later
  modelStencil->SetOutputWholeExtent(StencilBackgroundImage->GetExtent()); 
  modelStencil->Update(); 

  // Create Image stencil

  vtkSmartPointer<vtkImageStencil> combineModelwithBackgroundStencil = 
  vtkSmartPointer<vtkImageStencil>::New();

  combineModelwithBackgroundStencil->SetInput(StencilBackgroundImage);
  combineModelwithBackgroundStencil->SetStencil(modelStencil->GetOutput());

  combineModelwithBackgroundStencil->ReverseStencilOn();
  combineModelwithBackgroundStencil->SetBackgroundValue(OUTVALSTENCILFOREGROUND);
  combineModelwithBackgroundStencil->Update();

  simulatedUsImage->DeepCopy(combineModelwithBackgroundStencil->GetOutput()); 
 


  //display
   vtkSmartPointer<vtkImageActor> redImageActor =
    vtkSmartPointer<vtkImageActor>::New();

  redImageActor->SetInput(simulatedUsImage);

 
  // Visualize
  vtkSmartPointer<vtkRenderer> renderer =
    vtkSmartPointer<vtkRenderer>::New();
 
  // Red image is displayed
  renderer->AddActor(redImageActor);
  
 
  // White image is displayed
  //renderer->AddActor(redImageActor);
  //renderer->AddActor(whiteImageActor);
  renderer->ResetCamera();
 
  vtkSmartPointer<vtkRenderWindow> renderWindow =
    vtkSmartPointer<vtkRenderWindow>::New();
  renderWindow->AddRenderer(renderer);
 
  vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
    vtkSmartPointer<vtkRenderWindowInteractor>::New();
  vtkSmartPointer<vtkInteractorStyleImage> style =
    vtkSmartPointer<vtkInteractorStyleImage>::New();
 
  renderWindowInteractor->SetInteractorStyle(style);
 
  renderWindowInteractor->SetRenderWindow(renderWindow);
  renderWindowInteractor->Initialize();
  renderWindowInteractor->Start();
   

  return 1; 
}



void vtkUsSimulatorAlgo::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os,indent);
	os << indent << "Dummy value : "<< m_Dummy <<"\n";
	

}

vtkUsSimulatorAlgo::~vtkUsSimulatorAlgo()
{
  this->SetModelToImageMatrix(NULL); 
  this->SetStencilBackgroundImage(NULL);
}