/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "vtkUsSimulatorAlgo.h"
#include "vtkImageAlgorithm.h"
#include "vtkInformation.h"


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
void vtkUsSimulatorAlgo::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os,indent);
	os << indent << "Dummy value : "<< m_Dummy <<"\n";
	

}

vtkUsSimulatorAlgo::~vtkUsSimulatorAlgo()
{
	m_Dummy = 0; 
};