// .NAME vtkMeanShiftClustering - Cluster points using the mean shift method.
// .SECTION Description

#ifndef __vtkMeanShiftClustering_h
#define __vtkMeanShiftClustering_h

#include "vtkPolyDataAlgorithm.h" //superclass
#include "vtkSmartPointer.h"
#include "vtkVector.h"

#include <vector>

class vtkMeanShiftClustering : public vtkPolyDataAlgorithm
{
  public:
    static vtkMeanShiftClustering *New();
    vtkTypeMacro(vtkMeanShiftClustering, vtkPolyDataAlgorithm);
    void PrintSelf(ostream &os, vtkIndent indent);
    void DisplayPointAssociations(ostream &os);

	int GetPointAssociations(unsigned int pointIndex); 
    
    vtkSetMacro(WindowRadius, double);
    vtkGetMacro(WindowRadius, double);
    
    vtkSetMacro(GaussianVariance, double);
    vtkGetMacro(GaussianVariance, double);
   
    enum KernelEnum {UNIFORM, GAUSSIAN};
    
    void SetKernelToGaussian(){this->Kernel = GAUSSIAN;}
    void SetKernelToUniform(){this->Kernel = UNIFORM;}
   
  protected:
    vtkMeanShiftClustering();
    ~vtkMeanShiftClustering(){}
    int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
    
    int Kernel;
    
    void ComputeUniformCenter(vtkPoints* points, double* center);
    void ComputeGaussianCenter(vtkPoints* points, double computedCenter[3], double inputCenter[3]);
    void AssignBtoA(double* a, double* b);
    
  private:
    std::vector<int> ClusterId; //which cluster each point is associated with
    std::vector<vtkVector3d> ClusterCenters;
    
    double WindowRadius;
    double ConvergenceThreshold;
    unsigned int MaxIterations;
    double MinDistanceBetweenClusters;
    double GaussianVariance;
    
    double Distance(vtkVector3d avec, vtkVector3d bvec);
};

#endif
