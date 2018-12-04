/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"

#include "PlusMath.h"

#include "vnl/vnl_sparse_matrix.h"
#include "vnl/vnl_sparse_matrix_linear_system.h"  
#include "vnl/algo/vnl_lsqr.h"  
#include "vnl/vnl_cross.h"  

#include "vtkMath.h"
#include "vtkTransform.h"

#define MINIMUM_NUMBER_OF_CALIBRATION_EQUATIONS 8

//----------------------------------------------------------------------------
PlusMath::PlusMath()
{

}

//----------------------------------------------------------------------------
PlusMath::~PlusMath()
{

}

//----------------------------------------------------------------------------
PlusStatus PlusMath::LSQRMinimize(const std::vector< std::vector<double> > &aMatrix, const std::vector<double> &bVector, vnl_vector<double> &resultVector, double* mean/*=NULL*/, double* stdev/*=NULL*/,vnl_vector<unsigned int> *notOutliersIndices/*=NULL*/)
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

  std::vector<vnl_vector<double> > aMatrixVnl(m); 
  vnl_vector<double> row(n); 
  for ( unsigned int i = 0; i < aMatrix.size(); ++i )
  {
    for ( unsigned int r = 0; r < aMatrix[i].size(); ++r )
    {
      row[r] = aMatrix[i][r]; 
    }
    aMatrixVnl.push_back(row); 
  }

  return PlusMath::LSQRMinimize(aMatrixVnl, bVector, resultVector, mean, stdev,notOutliersIndices); 
}

//----------------------------------------------------------------------------
PlusStatus PlusMath::LSQRMinimize(const std::vector< vnl_vector<double> > &aMatrix, const std::vector<double> &bVector, vnl_vector<double> &resultVector, double* mean/*=NULL*/, double* stdev/*=NULL*/, vnl_vector<unsigned int>* notOutliersIndices /*=NULL*/)
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

  return PlusMath::LSQRMinimize(sparseMatrixLeftSide, vectorRightSide, resultVector, mean, stdev, notOutliersIndices); 
}


//----------------------------------------------------------------------------
PlusStatus PlusMath::LSQRMinimize(const vnl_sparse_matrix<double> &sparseMatrixLeftSide, const vnl_vector<double> &vectorRightSide, vnl_vector<double> &resultVector, double* mean/*=NULL*/, double* stdev/*=NULL*/, vnl_vector<unsigned int>* notOutliersIndices/*NULL*/)
{
  LOG_TRACE("PlusMath::LSQRMinimize"); 

  PlusStatus returnStatus = PLUS_SUCCESS; 

  vnl_sparse_matrix<double> aMatrix(sparseMatrixLeftSide); 
  vnl_vector<double> bVector(vectorRightSide); 

  bool outlierFound(true); 

  while ( outlierFound && (bVector.size()>MINIMUM_NUMBER_OF_CALIBRATION_EQUATIONS) )
  {
    // Construct linear system defined in VNL
    vnl_sparse_matrix_linear_system<double> linearSystem( aMatrix, bVector );

    // Instantiate the LSQR solver
    vnl_lsqr lsqr( linearSystem );

    // call minimize on the solver
    int returnCode = lsqr.minimize( resultVector );

    switch (returnCode)
    {
    case 0: // x = 0  is the exact solution. No iterations were performed.
      returnStatus = PLUS_SUCCESS;
      break;
    case 1: // The equations A*x = b are probably compatible.  "
      // Norm(A*x - b) is sufficiently small, given the "
      // "values of ATOL and BTOL.",
      returnStatus = PLUS_SUCCESS;
      break;
    case 2: // "The system A*x = b is probably not compatible.  "
      // "A least-squares solution has been obtained that is "
      // "sufficiently accurate, given the value of ATOL.",
      returnStatus = PLUS_SUCCESS;
      break;
    case 3: // "An estimate of cond(Abar) has exceeded CONLIM.  "
      //"The system A*x = b appears to be ill-conditioned.  "
      // "Otherwise, there could be an error in subroutine APROD.",
      LOG_WARNING("LSQR fit may be inaccurate, CONLIM exceeded");
      returnStatus = PLUS_SUCCESS;
      break;
    case 4: // "The equations A*x = b are probably compatible.  "
      // "Norm(A*x - b) is as small as seems reasonable on this machine.",
      returnStatus = PLUS_SUCCESS;
      break;
    case 5: // "The system A*x = b is probably not compatible.  A least-squares "
      // "solution has been obtained that is as accurate as seems "
      // "reasonable on this machine.",
      returnStatus = PLUS_SUCCESS;
      break;
    case 6: // "Cond(Abar) seems to be so large that there is no point in doing further "
      // "iterations, given the precision of this machine. "
      // "There could be an error in subroutine APROD.",
      LOG_ERROR("LSQR fit may be inaccurate, ill-conditioned matrix");
      return PLUS_FAIL;
    case 7: // "The iteration limit ITNLIM was reached."
      LOG_WARNING("LSQR fit may be inaccurate, ITNLIM was reached");
      returnStatus = PLUS_SUCCESS;
      break;
    default:
      LOG_ERROR("Unkown LSQR return code "<<returnCode);
      return PLUS_FAIL;
    }

    const double thresholdMultiplier = 3.0; 

    if ( PlusMath::RemoveOutliersFromLSQR(aMatrix, bVector, resultVector, outlierFound, thresholdMultiplier, mean, stdev , notOutliersIndices) != PLUS_SUCCESS )
    {
      LOG_WARNING("Failed to remove outliers from linear equations!"); 
      return PLUS_FAIL; 
    }

    if (bVector.size()<= MINIMUM_NUMBER_OF_CALIBRATION_EQUATIONS)
    {
      LOG_ERROR("It was not possible calibrate! Not enough equations!"); 
      return PLUS_FAIL; 
    }
  }

  return returnStatus; 
}

//----------------------------------------------------------------------------
PlusStatus PlusMath::RemoveOutliersFromLSQR(vnl_sparse_matrix<double> &sparseMatrixLeftSide, 
                                            vnl_vector<double> &vectorRightSide, 
                                            vnl_vector<double> &resultVector, 
                                            bool &outlierFound, 
                                            double thresholdMultiplier/* = 3.0*/, 
                                            double* mean/*=NULL*/, 
                                            double* stdev/*=NULL*/,
                      vnl_vector<unsigned int>* nonOutlierIndices /*NULL*/)
{
  // Set outlierFound flag to false by default 
  outlierFound = false; 

  const unsigned int numberOfEquations = sparseMatrixLeftSide.rows(); 
  const unsigned int numberOfUnknowns = resultVector.size(); 

  if ( vectorRightSide.size() != numberOfEquations )
  {
    LOG_ERROR("Input A matrix and b vector dimensions were not met (number of equations were not the same)!"); 
    return PLUS_FAIL; 
  }

  if ( sparseMatrixLeftSide.cols() != numberOfUnknowns )
  {
    LOG_ERROR("Input A matrix dimension (columns) and number of unknowns are different (cols: " << sparseMatrixLeftSide.cols() << "  unknowns: " << numberOfUnknowns << ")" ); 
    return PLUS_FAIL; 
  }


  vnl_vector<double> differenceVector(numberOfEquations, 0); 
  // Compute the difference between the measured and computed data ( Ax - b )
  for ( unsigned int row = 0; row < numberOfEquations; ++row )
  {
    vnl_sparse_matrix<double>::row matrixRow = sparseMatrixLeftSide.get_row(row);
    double difference(0); 
    // Compute Ax - b for each row
    for ( unsigned int i = 0; i < numberOfUnknowns; ++i )
    {
      // difference = A1x = a11*x1 + a12*x2 + ...
      difference += (matrixRow[i].second * resultVector[i] ); 
    }
    // difference = A1x - b1 = a11*x1 + a12*x2 + ... - b1
    difference -= vectorRightSide[row]; 
    
    // Add the difference to the vector 
    differenceVector.put(row, difference); 
  }

  // Compute the mean difference 
  const double meanDifference = differenceVector.mean(); 

  // Compute the stdev of differences 
  vnl_vector<double> diffFromMean = differenceVector - meanDifference; 
  const double stdevDifference = sqrt( diffFromMean.squared_magnitude() / (1.0 * diffFromMean.size()) ); 

  LOG_DEBUG("Mean = " << std::fixed << meanDifference << "   Stdev = " << stdevDifference); 

  if ( mean != NULL )
  {
    *mean = meanDifference; 
  }

  if ( stdev != NULL )
  {
    *stdev = stdevDifference; 
  }

  // Temporary containers for input data 
  std::vector< vcl_vector<double> > matrixRowsData; 
  std::vector< vcl_vector<int> > matrixRowsIndecies; 
  std::vector<double> bVector;   
  std::vector<double> auxiliarNonOutlierIndicesVector;   
 
  vcl_vector<double> rowData(numberOfUnknowns, 0.0); 
  vcl_vector<int> rowIndecies(numberOfUnknowns, 0); 

  // Look for outliers in each equations 
  // If the difference from mean larger than thresholdMultiplier * stdev, remove it from equation 
  for ( unsigned int row = 0; row < numberOfEquations; ++row )
  {
    if ( fabs(differenceVector[row] - meanDifference) < thresholdMultiplier * stdevDifference )
    { // Not an outlier 

      // Copy data from matrix row in a format which is good for vnl_sparse_matrix<T>::set_row function 
      vnl_sparse_matrix<double>::row matrixRow = sparseMatrixLeftSide.get_row(row);
      for ( unsigned int i = 0; i < matrixRow.size(); ++i )
      {
        rowIndecies[i] = matrixRow[i].first; 
        rowData[i] = matrixRow[i].second; 
      }

      // Save matrix row data, indecies and result vector into std::vectors
      matrixRowsData.push_back(rowData); 
      matrixRowsIndecies.push_back(rowIndecies); 
      bVector.push_back(vectorRightSide[row]); 
      if (nonOutlierIndices != NULL)
      {
        auxiliarNonOutlierIndicesVector.push_back(nonOutlierIndices->get(row));
      }
    }
    else
    {
      // Outlier found
      outlierFound = true; 
      LOG_DEBUG("Outlier: " << std::fixed << differenceVector[row] << "(mean: " << meanDifference << "  stdev: " << stdevDifference << "  outlierTreshold: " << thresholdMultiplier * stdevDifference << ")" ); 
    }
  }

  // Resize matrices only if we found an outlier
  if ( outlierFound ) 
  {
    // Copy back the new aMatrix and bVector 
    vectorRightSide.clear(); 
    vectorRightSide.set_size(bVector.size()); 
    for ( unsigned int i = 0; i < bVector.size(); ++i )
    {
      vectorRightSide.put(i, bVector[i]); 
    }

  if (nonOutlierIndices != NULL)
  {
    (*nonOutlierIndices).clear();
      (*nonOutlierIndices).set_size(auxiliarNonOutlierIndicesVector.size());
    for ( unsigned int i = 0; i < auxiliarNonOutlierIndicesVector.size(); ++i )
      {
      (*nonOutlierIndices).put(i,auxiliarNonOutlierIndicesVector[i]);
      }
  }


    sparseMatrixLeftSide.resize(matrixRowsData.size(), numberOfUnknowns); 
    for ( unsigned int r = 0; r < matrixRowsData.size(); r++ )
    {
      sparseMatrixLeftSide.set_row(r, matrixRowsIndecies[r], matrixRowsData[r]); 
    }
  }
  else
  {
    LOG_DEBUG("*** Outlier removal was successful! No more outlier found!"); 
  }

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
void PlusMath::ConvertVtkMatrixToVnlMatrix(const vtkMatrix4x4* inVtkMatrix, vnl_matrix_fixed<double,4,4>& outVnlMatrix )
{
  LOG_TRACE("PlusMath::ConvertVtkMatrixToVnlMatrix"); 

  for (int row = 0; row < 4; row++)
  {
    for (int column = 0; column < 4; column++)
    {
      outVnlMatrix.put(row,column, inVtkMatrix->GetElement(row,column)); 
    }
  }
}

//----------------------------------------------------------------------------
void PlusMath::ConvertVnlMatrixToVtkMatrix(const vnl_matrix_fixed<double,4,4>& inVnlMatrix, vtkMatrix4x4* outVtkMatrix )
{
  LOG_TRACE("PlusMath::ConvertVnlMatrixToVtkMatrix");

  outVtkMatrix->Identity(); 

  for (int row = 0; row < 3; row++)
  {
    for (int column = 0; column < 4; column++)
    {
      outVtkMatrix->SetElement(row,column, inVnlMatrix.get(row, column) ); 
    }
  }
}

//----------------------------------------------------------------------------
void PlusMath::PrintMatrix(vnl_matrix_fixed<double,4,4> matrix, std::ostringstream &stream, int precision/* = 3*/)
{
  vtkSmartPointer<vtkMatrix4x4> matrixVtk=vtkSmartPointer<vtkMatrix4x4>::New();
  ConvertVnlMatrixToVtkMatrix(matrix,matrixVtk);
  igsioMath::PrintVtkMatrix(matrixVtk, stream, precision);
}

//----------------------------------------------------------------------------
void PlusMath::LogMatrix(const vnl_matrix_fixed<double,4,4>& matrix, int precision/* = 3*/)
{
  vtkSmartPointer<vtkMatrix4x4> matrixVtk=vtkSmartPointer<vtkMatrix4x4>::New();
  ConvertVnlMatrixToVtkMatrix(matrix,matrixVtk);
  igsioMath::LogVtkMatrix(matrixVtk, precision);
}