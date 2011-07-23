#include "PlusMath.h"

#include "vnl/vnl_sparse_matrix_linear_system.h"  
#include "vnl/algo/vnl_lsqr.h"  
#include "vnl/vnl_cross.h"  

#include "vtkMath.h"
#include "vtkTransform.h"

//----------------------------------------------------------------------------
PlusMath::PlusMath()
{

}

//----------------------------------------------------------------------------
PlusMath::~PlusMath()
{

}

//----------------------------------------------------------------------------
PlusStatus PlusMath::LSQRMinimize(const std::vector< std::vector<double> > &aMatrix, const std::vector<double> &bVector, vnl_vector<double> &resultVector)
{
  LOG_TRACE("PlusMath::LSQRMinimize"); 

  if (aMatrix.size()==0)
  {
    LOG_ERROR("LSQRMinimize: A matrix is empty");
    resultVector.clear();
    return PLUS_FAIL;
  }
  if (bVector.size()==0)
  {
    LOG_ERROR("LSQRMinimize: b vector is empty");
    resultVector.clear();
    return PLUS_FAIL;
  }

  // The coefficient matrix aMatrix should be m-by-n and the column vector bVector must have length m.
  const int n = aMatrix.begin()->size(); 
  const int m = bVector.size();

  std::vector<vnl_vector<double>> aMatrixVnl(m); 
  vnl_vector<double> row(n); 
  for ( unsigned int i = 0; i < aMatrix.size(); ++i )
  {
    for ( unsigned int r = 0; r < aMatrix[i].size(); ++r )
    {
      row[r] = aMatrix[i][r]; 
    }
    aMatrixVnl.push_back(row); 
  }

  return PlusMath::LSQRMinimize(aMatrixVnl, bVector, resultVector); 
}

//----------------------------------------------------------------------------
PlusStatus PlusMath::LSQRMinimize(const std::vector<vnl_vector<double>> &aMatrix, const std::vector<double> &bVector, vnl_vector<double> &resultVector)
{
  LOG_TRACE("PlusMath::LSQRMinimize"); 

  if (aMatrix.size()==0)
  {
    LOG_ERROR("LSQRMinimize: A matrix is empty");
    resultVector.clear();
    return PLUS_FAIL;
  }
  if (bVector.size()==0)
  {
    LOG_ERROR("LSQRMinimize: b vector is empty");
    resultVector.clear();
    return PLUS_FAIL;
  }

  // The coefficient matrix aMatrix should be m-by-n and the column vector bVector must have length m.
  const int n = aMatrix.begin()->size(); 
  const int m = bVector.size();

  vnl_sparse_matrix<double> sparseMatrixLeftSide(m, n);
  vnl_vector<double> vectorRightSide(m);

  for(int row = 0; row < m; row++)
  {
    // Populate the sparse matrix
    for ( int i = 0; i < n; i++)
    {
      sparseMatrixLeftSide(row,i) = aMatrix[row].get(i);
    }

    // Populate the vector
    vectorRightSide.put(row, bVector[row]);
  }

  return PlusMath::LSQRMinimize(sparseMatrixLeftSide, vectorRightSide, resultVector); 
}


//----------------------------------------------------------------------------
PlusStatus PlusMath::LSQRMinimize(const vnl_sparse_matrix<double> &sparseMatrixLeftSide, const vnl_vector<double> &vectorRightSide, vnl_vector<double> &resultVector)
{
  LOG_TRACE("PlusMath::LSQRMinimize"); 

  // Construct linear system defined in VNL
  vnl_sparse_matrix_linear_system<double> linearSystem( sparseMatrixLeftSide, vectorRightSide );

  // Instantiate the LSQR solver
  vnl_lsqr lsqr( linearSystem );

  // call minimize on the solver
  lsqr.minimize( resultVector );

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
double PlusMath::GetPositionDifference(vtkMatrix4x4* aMatrix, vtkMatrix4x4* bMatrix)
{
  LOG_TRACE("PlusMath::GetPositionDifference"); 
  vtkSmartPointer<vtkTransform> aTransform = vtkSmartPointer<vtkTransform>::New(); 
  aTransform->SetMatrix(aMatrix); 

  vtkSmartPointer<vtkTransform> bTransform = vtkSmartPointer<vtkTransform>::New(); 
  bTransform->SetMatrix(bMatrix); 

  double ax = aTransform->GetPosition()[0]; 
  double ay = aTransform->GetPosition()[1]; 
  double az = aTransform->GetPosition()[2]; 

  double bx = bTransform->GetPosition()[0]; 
  double by = bTransform->GetPosition()[1]; 
  double bz = bTransform->GetPosition()[2]; 

  // Euclidean distance
  double distance = sqrt( pow(ax-bx,2) + pow(ay-by,2) + pow(az-bz,2) ); 

  return distance; 
}

//----------------------------------------------------------------------------
double PlusMath::GetOrientationDifference(vtkMatrix4x4* aMatrix, vtkMatrix4x4* bMatrix)
{
  LOG_TRACE("PlusMath::GetOrientationDifference"); 

  vtkSmartPointer<vtkMatrix4x4> diffMatrix = vtkSmartPointer<vtkMatrix4x4>::New(); 
  vtkSmartPointer<vtkMatrix4x4> invBmatrix = vtkSmartPointer<vtkMatrix4x4>::New(); 

  vtkMatrix4x4::Invert(bMatrix, invBmatrix);  

  vtkMatrix4x4::Multiply4x4(aMatrix, invBmatrix, diffMatrix); 

  vtkSmartPointer<vtkTransform> diffTransform = vtkSmartPointer<vtkTransform>::New(); 
  diffTransform->SetMatrix(diffMatrix); 

  double angleDiff_rad= vtkMath::RadiansFromDegrees(diffTransform->GetOrientationWXYZ()[0]);

  double normalizedAngleDiff_rad = atan2( sin(angleDiff_rad), cos(angleDiff_rad) ); // normalize angle to domain -pi, pi 

  return vtkMath::DegreesFromRadians(normalizedAngleDiff_rad);
}

