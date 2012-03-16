/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkUsSimulatorAlgo_h
#define __vtkUsSimulatorAlgo_h
#include "vtkImageAlgorithm.h"

class  vtkUsSimulatorAlgo : public vtkImageAlgorithm
{

  public:
     void PrintSelf(ostream& os, vtkIndent indent);
	
  
  protected:
	 virtual int FillInputPortInformation(int port, vtkInformation* info);
	 virtual int FillOutputPortInformation(int port, vtkInformation* info);
	  int m_Dummy ; 
	 vtkUsSimulatorAlgo();
	~vtkUsSimulatorAlgo(); 
  
};

#endif // __vtkUsSimulatorAlgo_h