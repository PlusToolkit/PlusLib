/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkUsSimulatorAlgo_h
#define __vtkUsSimulatorAlgo_h
#include "vtkImageAlgorithm.h"
#include "vtkSmartPointer.h"

class VTK_EXPORT vtkUsSimulatorAlgo : public vtkImageAlgorithm
{

  public:
    void PrintSelf(ostream& os, vtkIndent indent);
    
    vtkTypeRevisionMacro(vtkUsSimulatorAlgo,vtkImageAlgorithm);	
	  static vtkUsSimulatorAlgo *New();
  
  protected:
	 virtual int FillInputPortInformation(int port, vtkInformation* info);
	 virtual int FillOutputPortInformation(int port, vtkInformation* info);
	 virtual int RequestData(vtkInformation *request,
                          vtkInformationVector** inputVector,
                          vtkInformationVector* outputVector);
 
	 
	 
	 int m_Dummy ; 
	 vtkUsSimulatorAlgo();
	~vtkUsSimulatorAlgo(); 

 private:
  double whiteImageOrigin[3]; 
  double volumeSpacing[3]; 
  double modelBounds[6];


};

#endif // __vtkUsSimulatorAlgo_h