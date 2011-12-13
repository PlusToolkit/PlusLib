/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __vtkMeanShiftClustering_h
#define __vtkMeanShiftClustering_h

#include "vtkPolyDataAlgorithm.h" //superclass
#include "vtkSmartPointer.h"
#include "vtkVector.h"

#include <vector>

/*!
  \class vtkMeanShiftClustering 
  \brief Cluster points using the mean shift method
  \ingroup PlusLibCalibrationAlgorithm
*/ 
class vtkMeanShiftClustering : public vtkPolyDataAlgorithm
{
public:
  static vtkMeanShiftClustering *New();
  vtkTypeMacro(vtkMeanShiftClustering, vtkPolyDataAlgorithm);
  void PrintSelf(ostream &os, vtkIndent indent);

  /*! Print point cluster associations to stream */
  void DisplayPointAssociations(ostream &os);

  /*! Get point association (cluster number) from point index */
  int GetPointAssociations(unsigned int pointIndex); 

  /*! Set window radius */
  vtkSetMacro(WindowRadius, double);
  /*! Get window radius */
  vtkGetMacro(WindowRadius, double);

  /*! Set Gaussian variance */
  vtkSetMacro(GaussianVariance, double);
  /*! Get Gaussian variance */
  vtkGetMacro(GaussianVariance, double);

  /*! Kernel types */
  enum KernelEnum {UNIFORM, GAUSSIAN};

  /*! Set kernel type to Gaussian */
  void SetKernelToGaussian() {this->Kernel = GAUSSIAN;} 
  /*! Set kernel type to Uniform */
  void SetKernelToUniform() {this->Kernel = UNIFORM;}

protected:
  vtkMeanShiftClustering();
  ~vtkMeanShiftClustering(){}
  int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);

  int Kernel;

  void ComputeUniformCenter(vtkPoints* points, double* center);
  void ComputeGaussianCenter(vtkPoints* points, double computedCenter[3], double inputCenter[3]);
  void AssignBtoA(double* a, double* b);

private:
  /*! which cluster each point is associated with */
  std::vector<int> ClusterId; 
  std::vector<vtkVector3d> ClusterCenters;

  double WindowRadius;
  double ConvergenceThreshold;
  unsigned int MaxIterations;
  double MinDistanceBetweenClusters;
  double GaussianVariance;

  double Distance(vtkVector3d avec, vtkVector3d bvec);
};

#endif
