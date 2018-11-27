/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __PLUSMATH_H
#define __PLUSMATH_H

#include "PlusConfigure.h"
#include "vtkPlusCommonExport.h"

#include <vector>

#include "vnl/vnl_matrix_fixed.h"
#include "vnl/vnl_vector.h"
#include "vnl/vnl_sparse_matrix.h"
#include "vtkMath.h"

#include <igsioMath.h>

class vtkMatrix4x4;
class vtkTransform;

#if _MSC_VER == 1600 // VS 2010
namespace std
{
  double vtkPlusCommonExport round(double arg);
}
#endif

/*!
  \class PlusMath
  \brief A utility class that contains static functions for various useful commonly used computations
  \ingroup PlusLibCommon
*/
class vtkPlusCommonExport PlusMath
{
public:

  /*!
    Solve Ax = b sparse linear equations with robust linear least squares method (vnl_lsqr and outlier removal)
    \param aMatrix The coefficient matrix of size m-by-n.
    \param bVector Column vector of length m.
    \param mean Pointer to get the resulting mean of the the LSQR fit error
    \param stdev Pointer to get the resulting standard deviation of the the LSQR fit error
    \param resultVector to store the results
  \param notOutlierIndices Row that were not removed during the outliers rejection process
  */
  static PlusStatus LSQRMinimize(const std::vector< std::vector<double> > &aMatrix, const std::vector<double> &bVector, vnl_vector<double> &resultVector, double* mean = NULL, double* stdev = NULL, vnl_vector<unsigned int>* notOutliersIndices=NULL); 
  /*!
    Solve Ax = b sparse linear equations with robust linear least squares method (vnl_lsqr and outlier removal)
    \param aMatrix The coefficient matrix of size m-by-n.
    \param bVector Column vector of length m.
    \param mean Pointer to get the resulting mean of the the LSQR fit error
    \param stdev Pointer to get the resulting standard deviation of the the LSQR fit error
    \param resultVector to store the results
  \param notOutlierIndices Row that were not removed during the outliers rejection process
  */
  static PlusStatus LSQRMinimize(const std::vector<vnl_vector<double> > &aMatrix, const std::vector<double> &bVector, vnl_vector<double> &resultVector, double* mean = NULL, double* stdev = NULL , vnl_vector<unsigned int>* notOutliersIndices=NULL); 
  /*!
    Solve Ax = b sparse linear equations with robust linear least squares method (vnl_lsqr and outlier removal)
    \param sparseMatrixLeftSide The coefficient matrix of size m-by-n. (aMatrix)
    \param vectorRightSide Column vector of length m. (bVector)
    \param mean Pointer to get the resulting mean of the the LSQR fit error
    \param stdev Pointer to get the resulting standard deviation of the the LSQR fit error
    \param resultVector to store the results
  */
  static PlusStatus LSQRMinimize(const vnl_sparse_matrix<double> &sparseMatrixLeftSide, const vnl_vector<double> &vectorRightSide, vnl_vector<double> &resultVector, double* mean = NULL, double* stdev = NULL, vnl_vector<unsigned int>* notOutliersIndices=NULL); 

  /*! Convert matrix between VTK and VNL */
  static void ConvertVnlMatrixToVtkMatrix(const vnl_matrix_fixed<double,4,4>& inVnlMatrix, vtkMatrix4x4* outVtkMatrix); 
  static void ConvertVtkMatrixToVnlMatrix(const vtkMatrix4x4* inVtkMatrix, vnl_matrix_fixed<double,4,4>& outVnlMatrix );


  /*! Print matrix into STL stream */
  static void PrintMatrix(vnl_matrix_fixed<double,4,4> matrix, std::ostringstream &stream, int precision = 3);

  /*! Print matrix into log as info */
  static void LogMatrix(const vnl_matrix_fixed<double,4,4>& matrix, int precision = 3);

protected:
  PlusMath(); 
  ~PlusMath();

  /*! Remove outliers from Ax = b sparse linear equations after linear least squares method (vnl_lsqr) */
  static PlusStatus RemoveOutliersFromLSQR(
    vnl_sparse_matrix<double> &sparseMatrixLeftSide, 
    vnl_vector<double> &vectorRightSide, 
    vnl_vector<double> &resultVector, 
    bool &outlierFound, 
    double thresholdMultiplier = 3.0, 
    double* mean = NULL, 
    double* stdev = NULL,
    vnl_vector<unsigned int>* nonOutlierIndices = NULL
  ); 

private: 
  PlusMath(PlusMath const&);
  PlusMath& operator=(PlusMath const&);
};

#endif 