/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkUsSimulatorAlgo_h
#define __vtkUsSimulatorAlgo_h
#include "vtkImageAlgorithm.h"
#include "vtkSmartPointer.h"
#include "vtkImageData.h"
#include "vtkMatrix4x4.h"

class VTK_EXPORT vtkUsSimulatorAlgo : public vtkImageAlgorithm
{

  public:
    void PrintSelf(ostream& os, vtkIndent indent);
    
    vtkTypeRevisionMacro(vtkUsSimulatorAlgo,vtkImageAlgorithm);	
	  static vtkUsSimulatorAlgo *New();
    
    //void SetStencilBackgroundImage (vtkImageData* stencilBackgroundImage); 

    vtkSetObjectMacro( ModelToImageMatrix, vtkMatrix4x4); 
    vtkGetObjectMacro( ModelToImageMatrix, vtkMatrix4x4); 

    vtkSetObjectMacro( StencilBackgroundImage, vtkImageData); 
    
    vtkGetMacro( OutputImage, vtkImageData*); 
    
  
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
   vtkImageData* StencilBackgroundImage; 
   vtkMatrix4x4* ModelToImageMatrix;  
   static const unsigned char OUTVALSTENCILFOREGROUND;
   vtkImageData* OutputImage; 
  

};

#endif // __vtkUsSimulatorAlgo_h