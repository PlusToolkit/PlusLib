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


//-----------------------------------------------------------------------------

vtkUsSimulatorAlgo::vtkUsSimulatorAlgo()
{
  this->SetNumberOfInputPorts(2); 
  this->SetNumberOfOutputPorts(1); 
  for ( int i=0; i<3; i++)
  {
    this->volumeSpacing[i] = 0; 
    this->modelBounds[i] = 0; 
    this->whiteImageOrigin[i] = 0; 
  }

};

int vtkUsSimulatorAlgo::FillInputPortInformation(int portNum, vtkInformation * info)
{
 if(portNum==0)
 {
  info->Set(vtkImageAlgorithm:: INPUT_REQUIRED_DATA_TYPE(), "vtkTransform");
  return 1;
 }
 else if(portNum==1) 
 {
  info->Set(vtkImageAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPolyData"); 
  return 1; 
 }
 return 0; 
 
}

int vtkUsSimulatorAlgo::FillOutputPortInformation(int portNum, vtkInformation * info)
{
	info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkImageData"); 
	return 1; 
}


int vtkUsSimulatorAlgo::RequestData(vtkInformation* request,vtkInformationVector** inputVector,vtkInformationVector* outputVector)
{
  // Try catch blocks? Remember to add some sort of error checking... what if input/output vector is empty

  vtkInformation* inInfoPort0 = inputVector[0]->GetInformationObject(0);
  vtkInformation* inInfoPort1 = inputVector[1]->GetInformationObject(0);
  vtkInformation* outInfo = outputVector->GetInformationObject(0); 

  vtkSmartPointer<vtkTransform> modelToImageTransform = vtkTransform::SafeDownCast(inInfoPort0->Get(vtkDataObject::DATA_OBJECT()));
  vtkSmartPointer<vtkPolyData> model = vtkPolyData::SafeDownCast(inInfoPort1->Get(vtkDataObject::DATA_OBJECT()));
  vtkSmartPointer<vtkImageData> simulatedUsImage = vtkImageData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT())); 

  // create white image

  vtkSmartPointer<vtkImageData> whiteImage = vtkSmartPointer<vtkImageData>::New(); 
  
  //TODO: Get and Set functions for image origin, bounds, and spacin g
  //TODO: report error if such values are empty, also report error if transform and model are empty 

  whiteImage->SetSpacing(volumeSpacing); 
  whiteImage->SetOrigin(whiteImageOrigin); 
  whiteImage->SetScalarTypeToUnsignedChar();
  whiteImage->AllocateScalars();

  unsigned char inval = 255;
  unsigned char outval = 0;
  vtkIdType count = whiteImage->GetNumberOfPoints();
  for (vtkIdType i = 0; i < count; ++i)
  {
   whiteImage->GetPointData()->GetScalars()->SetTuple1(i, inval);
  }

   vtkSmartPointer<vtkPolyDataToImageStencil> modelCutout = 
   vtkSmartPointer<vtkPolyDataToImageStencil>::New();

   modelCutout->SetInput(model); 
   modelCutout->SetOutputSpacing(volumeSpacing); 
   modelCutout->SetOutputOrigin(whiteImageOrigin); //think about this later
   modelCutout->SetOutputWholeExtent(whiteImage->GetExtent()); 
   modelCutout->Update(); 

   vtkSmartPointer<vtkImageStencil> stencilCombineCutoutandWhiteImage = 
   vtkSmartPointer<vtkImageStencil>::New();

  stencilCombineCutoutandWhiteImage->SetInput(whiteImage);
  stencilCombineCutoutandWhiteImage->SetStencil(modelCutout->GetOutput());

  stencilCombineCutoutandWhiteImage->ReverseStencilOff();
  stencilCombineCutoutandWhiteImage->SetBackgroundValue(outval);
  stencilCombineCutoutandWhiteImage->Update();

  simulatedUsImage = stencilCombineCutoutandWhiteImage->GetOutput(); 
   

 

  /*
  vtkPolydataToImageStencil stencilCreator; 
  stencilCreator SetInput(model); 
  // other parameters to be set, such as spacing, origin extent. 
  stencilCreator->Update(); 

  simulatedUsImage
  */  

  return 1; 
}



void vtkUsSimulatorAlgo::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os,indent);
	os << indent << "Dummy value : "<< m_Dummy <<"\n";
	

}

vtkUsSimulatorAlgo::~vtkUsSimulatorAlgo()
{
	m_Dummy = 0; 
};