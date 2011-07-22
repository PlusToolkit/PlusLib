#ifndef __PLUSMATH_H
#define __PLUSMATH_H

#include "PlusConfigure.h"
#include <vector>

#include "vnl/vnl_matrix.h"
#include "vnl/vnl_vector.h"
#include "vnl/vnl_sparse_matrix.h"   

class VTK_EXPORT PlusMath
{
public:

  //! Description: 
	// Solve Ax = b sparse linear equations with linear least squares method (vnl_lsqr)
	// The coefficient matrix aMatrix should be m-by-n and the column vector bVector must have length m. 
	// resultVector size need to be fixed by constructor time
  static PlusStatus LSQRMinimize(const std::vector< std::vector<double> > &aMatrix, const std::vector<double> &bVector, vnl_vector<double> &resultVector); 
  static PlusStatus LSQRMinimize(const std::vector<vnl_vector<double>> &aMatrix, const std::vector<double> &bVector, vnl_vector<double> &resultVector); 
  static PlusStatus LSQRMinimize(const vnl_sparse_matrix<double> &sparseMatrixLeftSide, const vnl_vector<double> &vectorRightSide, vnl_vector<double> &resultVector); 

protected:
  PlusMath(); 
  ~PlusMath();

private: 
  PlusMath(PlusMath const&);
  PlusMath& operator=(PlusMath const&);
};

#endif 