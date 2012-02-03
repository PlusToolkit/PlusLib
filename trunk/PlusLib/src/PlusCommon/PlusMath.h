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

/*!
  \class PlusMath
  \brief A utility class that contains static functions for various useful commonly used computations
  \ingroup PlusLibCommon
*/
class VTK_EXPORT PlusMath
{
public:

  /*!
    Solve Ax = b sparse linear equations with robust linear least squares method (vnl_lsqr and outlier removal)
    \param aMatrix The coefficient matrix of size m-by-n.
    \param bVector Column vector of length m.
    \param mean Pointer to get the resulting mean of the the LSQR fit error
    \param stdev Pointer to get the resulting standard deviation of the the LSQR fit error
    \param resultVector to store the results
  */
  static PlusStatus LSQRMinimize(const std::vector< std::vector<double> > &aMatrix, const std::vector<double> &bVector, vnl_vector<double> &resultVector, double* mean = NULL, double* stdev = NULL); 
  /*!
    Solve Ax = b sparse linear equations with robust linear least squares method (vnl_lsqr and outlier removal)
    \param aMatrix The coefficient matrix of size m-by-n.
    \param bVector Column vector of length m.
    \param mean Pointer to get the resulting mean of the the LSQR fit error
    \param stdev Pointer to get the resulting standard deviation of the the LSQR fit error
    \param resultVector to store the results
  */
  static PlusStatus LSQRMinimize(const std::vector<vnl_vector<double>> &aMatrix, const std::vector<double> &bVector, vnl_vector<double> &resultVector, double* mean = NULL, double* stdev = NULL); 
  /*!
    Solve Ax = b sparse linear equations with robust linear least squares method (vnl_lsqr and outlier removal)
    \param sparseMatrixLeftSide The coefficient matrix of size m-by-n. (aMatrix)
    \param vectorRightSide Column vector of length m. (bVector)
    \param mean Pointer to get the resulting mean of the the LSQR fit error
    \param stdev Pointer to get the resulting standard deviation of the the LSQR fit error
    \param resultVector to store the results
  */
  static PlusStatus LSQRMinimize(const vnl_sparse_matrix<double> &sparseMatrixLeftSide, const vnl_vector<double> &vectorRightSide, vnl_vector<double> &resultVector, double* mean = NULL, double* stdev = NULL); 

  /*! Returns the Euclidean distance between two 4x4 homogeneous transformation matrix */
  static double GetPositionDifference(vtkMatrix4x4* aMatrix, vtkMatrix4x4* bMatrix); 

  /*! Returns the orientation difference in degrees between two 4x4 homogeneous transformation matrix */
  static double GetOrientationDifference(vtkMatrix4x4* aMatrix, vtkMatrix4x4* bMatrix); 

  /*! Returns the distance between a line, defined by two point (x and y) and a point (z) */
  static double PlusMath::ComputeDistanceLinePoint(const double x[3], const double y[3], const double z[3]);

  /*!
    Spherical linear interpolation between two rotation quaternions.
    Precondition: no aliasing problems to worry about ("result" can be "from" or "to" param).
    \param result Interpolated quaternion
    \param from Input quaternion
    \param to Input quaternion
    \param t Value between 0 and 1 that interpolates between from and to (t=0 means the results is the same as "from").    
    \param adjustSign If true, then slerp will operate by adjusting the sign of the slerp to take shortest path
    References: From Adv Anim and Rendering Tech. Pg 364
  */
  static void Slerp(double *result, double t, double *from, double *to, bool adjustSign = true); 

  /*! Returns a string containing the parameters (rotation, translation, scaling) from a transformation */
  static std::string GetTransformParametersString(vtkTransform* transform);

  /*! Returns a string containing the parameters (rotation, translation, scaling) from a vtkMatrix */
  static std::string GetTransformParametersString(vtkMatrix4x4* matrix); 

  /*! Convert matrix between VTK and VNL */
	static void ConvertVnlMatrixToVtkMatrix(vnl_matrix<double>& inVnlMatrix, vtkMatrix4x4* outVtkMatrix); 
	static void ConvertVtkMatrixToVnlMatrix(vtkMatrix4x4* inVtkMatrix, vnl_matrix<double>& outVnlMatrix ); 

  /*! Print VTK matrix into STL stream */
  static void PrintVtkMatrix(vtkMatrix4x4* matrix, std::ostringstream &stream, int precision = 3);

  /*! Print VTK matrix into log as info */
  static void LogVtkMatrix(vtkMatrix4x4* matrix, int precision = 3);

  /*! 
  The 'floor' function on x86 and mips is 4-10 times slower than these
  and is used a lot in this code, optimize for different CPU architectures.
  The output type is int, as we don't want to round if the result is a float.
  */
  static inline int Floor(double x)
  {
#if defined mips || defined sparc || defined __ppc__
    x += 2147483648.0;
    unsigned int i = (unsigned int)(x);
    return (int)(i - 2147483648U);
#elif defined i386 || defined _M_IX86
    union { double d; unsigned short s[4]; unsigned int i[2]; } dual;
    // use 52-bit precision of IEEE double to round (x - 0.25) to 
    // the nearest multiple of 0.5, according to prevailing rounding
    // mode which is IEEE round-to-nearest,even
    dual.d = x + 103079215104.0;  // (2**(52-16))*1.5
    // extract mantissa, use shift to divide by 2 and hence get rid
    // of the bit that gets messed up because the FPU uses
    // round-to-nearest,even mode instead of round-to-nearest,+infinity
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

  /*! Ceiling operation optimized for speed */
  static inline int Ceil(double x)
  {
    return -Floor(-x - 1.0) - 1;
  }

  /*! Round operation optimized for speed  */
  static inline int Round(double x)
  {
    return Floor(x + 0.5);
  }

  /*! 
  Convert a float into an integer plus a fraction, optimized for speed 
  \param x Input floating point value
  \param f Output fraction part
  \return Output integer part
  */
  template <class F>
  static inline int Floor(F x, F &f)
  {
    int ix = Floor(x);
    f = x - ix;
    return ix;
  }

  /*! 
  Round operation that copies the result to a specified memory address, optimized for speed  
  \param val Input floating point value
  \param rnd Output rounded value
  */
  template <class F, class T>
  static inline void Round(F val, T& rnd)
  {
    rnd = Round(val);
  }

protected:
  PlusMath(); 
  ~PlusMath();

  /*! Remove outliers from Ax = b sparse linear equations after linear least squares method (vnl_lsqr) */
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


#endif 
