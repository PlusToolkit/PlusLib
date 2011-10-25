/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __PLUSMATH_H
#define __PLUSMATH_H

#include "PlusConfigure.h"
#include <vector>

#include "vnl/vnl_matrix.h"
#include "vnl/vnl_vector.h"
#include "vnl/vnl_sparse_matrix.h"   
#include "vtkMatrix4x4.h"
#include "vtkTransform.h"

class VTK_EXPORT PlusMath
{
public:

  //! Solve Ax = b sparse linear equations with robust linear least squares method (vnl_lsqr + outlier removal)
	// The coefficient matrix aMatrix should be m-by-n and the column vector bVector must have length m. 
	// resultVector size need to be fixed by constructor time
  static PlusStatus LSQRMinimize(const std::vector< std::vector<double> > &aMatrix, const std::vector<double> &bVector, vnl_vector<double> &resultVector, double* mean = NULL, double* stdev = NULL); 
  static PlusStatus LSQRMinimize(const std::vector<vnl_vector<double>> &aMatrix, const std::vector<double> &bVector, vnl_vector<double> &resultVector, double* mean = NULL, double* stdev = NULL); 
  static PlusStatus LSQRMinimize(const vnl_sparse_matrix<double> &sparseMatrixLeftSide, const vnl_vector<double> &vectorRightSide, vnl_vector<double> &resultVector, double* mean = NULL, double* stdev = NULL); 

  //! Returns the Euclidean distance between two 4x4 homogeneous transformation matrix
  static double GetPositionDifference(vtkMatrix4x4* aMatrix, vtkMatrix4x4* bMatrix); 

  //! Returns the orientation difference in degrees between two 4x4 homogeneous transformation matrix
  static double GetOrientationDifference(vtkMatrix4x4* aMatrix, vtkMatrix4x4* bMatrix); 

  //! Spherical linear interpolation between two rotation quaternions.
  // t is a value between 0 and 1 that interpolates between from and to (t=0 means the results is the same as "from").
  // Precondition: no aliasing problems to worry about ("result" can be "from" or "to" param).
  // Parameters: adjustSign - If true, then slerp will operate by adjusting the sign of the slerp to take shortest path
  // References: From Adv Anim and Rendering Tech. Pg 364
  static void Slerp(double *result, double t, double *from, double *to, bool adjustSign = true); 

  //! Returns a string containing the parameters (rotation, translation, scaling) from a transformation
  static std::string GetTransformParametersString(vtkTransform* transform);

  //! Convert matrix between VTK and VNL
	static void ConvertVnlMatrixToVtkMatrix(vnl_matrix<double>& inVnlMatrix, vtkMatrix4x4* outVtkMatrix); 
	static void ConvertVtkMatrixToVnlMatrix(vtkMatrix4x4* inVtkMatrix, vnl_matrix<double>& outVnlMatrix ); 

  //! Print VTK matrix into STL stream
  static void PrintVtkMatrix(vtkMatrix4x4* matrix, std::ostringstream &stream);

  //! Print VTK matrix into log as info
  static void LogVtkMatrix(vtkMatrix4x4* matrix);

protected:
  PlusMath(); 
  ~PlusMath();

  //! Remove outliers from Ax = b sparse linear equations after linear least squares method (vnl_lsqr)
  static PlusStatus RemoveOutliersFromLSRQ(
    vnl_sparse_matrix<double> &sparseMatrixLeftSide, 
    vnl_vector<double> &vectorRightSide, 
    vnl_vector<double> &resultVector, 
    bool &outlierFound, 
    double thresholdMultiplier = 3.0, 
    double* mean = NULL, 
    double* stdev = NULL ); 

private: 
  PlusMath(PlusMath const&);
  PlusMath& operator=(PlusMath const&);
};

//****************************************************************************
// ROUNDING CODE
//****************************************************************************

//----------------------------------------------------------------------------
// rounding functions, split and optimized for each type
// (because we don't want to round if the result is a float!)

// in the case of a tie between integers, the larger integer wins.

// The 'floor' function on x86 and mips is many times slower than these
// and is used a lot in this code, optimize for different CPU architectures
// static inline int vtkUltraFloor(double x)
// {
// #if defined mips || defined sparc
//   return (int)((unsigned int)(x + 2147483648.0) - 2147483648U);
// #elif defined i386
//   double tempval = (x - 0.25) + 3377699720527872.0; // (2**51)*1.5
//   return ((int*)&tempval)[0] >> 1;
// #else
//   return int(floor(x));
// #endif
// }

static inline int vtkUltraFloor(double x)
{
#if defined mips || defined sparc || defined __ppc__
  x += 2147483648.0;
  unsigned int i = (unsigned int)(x);
  return (int)(i - 2147483648U);
#elif defined i386 || defined _M_IX86
  union { double d; unsigned short s[4]; unsigned int i[2]; } dual;
  dual.d = x + 103079215104.0;  // (2**(52-16))*1.5
  return (int)((dual.i[1]<<16)|((dual.i[0])>>16));
#elif defined ia64 || defined __ia64__ || defined IA64
  x += 103079215104.0;
  long long i = (long long)(x);
  return (int)(i - 103079215104LL);
#else
  double y = floor(x);
  return (int)(y);
#endif
}

static inline int vtkUltraCeil(double x)
{
  return -vtkUltraFloor(-x - 1.0) - 1;
}

static inline int vtkUltraRound(double x)
{
  return vtkUltraFloor(x + 0.5);
}

static inline int vtkUltraFloor(float x)
{
  return vtkUltraFloor((double)x);
}

static inline int vtkUltraCeil(float x)
{
  return vtkUltraCeil((double)x);
}

static inline int vtkUltraRound(float x)
{
  return vtkUltraRound((double)x);
}

// convert a float into an integer plus a fraction
template <class F>
static inline int vtkUltraFloor(F x, F &f)
{
  int ix = vtkUltraFloor(x);
  f = x - ix;
  return ix;
}

template <class F, class T>
static inline void vtkUltraRound(F val, T& rnd)
{
  rnd = vtkUltraRound(val);
}


#endif 
