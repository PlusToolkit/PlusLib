/*=======================================================================

  Program:   NDI Combined API C Interface Library
  Module:    $RCSfile: ndicapi_math.h,v $
  Creator:   David Gobbi <dgobbi@atamai.com>
  Language:  C
  Author:    $Author: dgobbi $
  Date:      $Date: 2005/07/01 22:52:05 $
  Version:   $Revision: 1.2 $

==========================================================================

Copyright (c) 2000-2005 Atamai, Inc.

Use, modification and redistribution of the software, in source or
binary forms, are permitted provided that the following terms and
conditions are met:

1) Redistribution of the source code, in verbatim or modified
   form, must retain the above copyright notice, this license,
   the following disclaimer, and any notices that refer to this
   license and/or the following disclaimer.  

2) Redistribution in binary form must include the above copyright
   notice, a copy of this license and the following disclaimer
   in the documentation or with other materials provided with the
   distribution.

3) Modified copies of the source code must be clearly marked as such,
   and must not be misrepresented as verbatim copies of the source code.

THE COPYRIGHT HOLDERS AND/OR OTHER PARTIES PROVIDE THE SOFTWARE "AS IS"
WITHOUT EXPRESSED OR IMPLIED WARRANTY INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE.  IN NO EVENT SHALL ANY COPYRIGHT HOLDER OR OTHER PARTY WHO MAY
MODIFY AND/OR REDISTRIBUTE THE SOFTWARE UNDER THE TERMS OF THIS LICENSE
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, LOSS OF DATA OR DATA BECOMING INACCURATE
OR LOSS OF PROFIT OR BUSINESS INTERRUPTION) ARISING IN ANY WAY OUT OF
THE USE OR INABILITY TO USE THE SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGES.

=======================================================================*/

/*! \file ndicapi_math.h
  This file contains some math functions that are useful with the NDICAPI.
*/

#ifndef NDICAPI_MATH_H
#define NDICAPI_MATH_H 1

#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>

/*=====================================================================*/
/*! \defgroup NdicapiMath Mathematical Functions 

   These are some useful math functions.  Note that the matrices are
   stored using the OpenGL convention:
  \f[
  \left(
  \begin{array}{cccc}
  m[0] & m[4] & m[8] & m[12] \\
  m[1] & m[5] & m[9] & m[13] \\
  m[2] & m[6] & m[10] & m[14] \\
  m[3] & m[7] & m[11] & m[15] \\
  \end{array}
  \right)
  \f]
*/

/*! \ingroup NdicapiMath
  Find the position and orientation of a tool relative to a 
  reference tool.  This is done by quaternion division.

  \param a   the original tool transformation
  \param b   the reference tool transformation
  \param c   the resulting relative transformation

  The pointer \em c can be the same as pointer \em a if you want to do
  the division in-place.
*/
void ndiRelativeTransform(const double a[8], const double b[8], double c[8]);

/*! \ingroup NdicapiMath
  Convert a quaternion transformation into a 4x4 float matrix.
*/
void ndiTransformToMatrixf(const double trans[8], float matrix[16]);
/*! \ingroup NdicapiMath
  Convert a quaternion transformation into a 4x4 double matrix.
*/
void ndiTransformToMatrixd(const double trans[8], double matrix[16]);

/*! \ingroup NdicapiMath
  Extract rotation angles from a 4x4 float matrix.  The order of the
  rotations is: 
  -# roll around \em x axis
  -# pitch around \em y axis
  -# yaw around \em z axis
*/
void ndiAnglesFromMatrixf(float angles[3], const float matrix[16]);
/*! \ingroup NdicapiMath
  Extract rotation angles from a 4x4 double matrix.  The order of the
  rotations is: 
  -# roll around \em x axis
  -# pitch around \em y axis
  -# yaw around \em z axis
*/
void ndiAnglesFromMatrixd(double angles[3], const double matrix[16]);

/*! \ingroup NdicapiMath
  Extract position coordinates from a 4x4 float matrix.  These have
  the same value as the position coordinates in the quaternion
  transformation.
*/
void ndiCoordsFromMatrixf(float coords[3], const float matrix[16]);

/*! \ingroup NdicapiMath
  Extract position coordinates from a 4x4 double matrix.  These have
  the same value as the position coordinates in the quaternion
  transformation.
*/
void ndiCoordsFromMatrixd(double coords[3], const double matrix[16]);

#ifdef __cplusplus
}
#endif

#endif /* NDICAPI_MATH_H */

