/*=======================================================================

  Program:   NDI Combined API C Interface Library
  Module:    $RCSfile: ndicapi_math.c,v $
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

#include <math.h>

/*---------------------------------------------------------
  Divide the transform 'trans' by the transform 'ref':
  trans = trans * ref^(-1)
  ---------------------------------------------------------*/

void ndiRelativeTransform(const double a[8], const double b[8], double c[8])
{
  double f,x,y,z,w1,x1,y1,z1,w2,x2,y2,z2;

  w1 = b[0];
  x1 = b[1];
  y1 = b[2];
  z1 = b[3];

  w2 = a[0];
  x2 = a[1];
  y2 = a[2];
  z2 = a[3];
  
  /* for rotation part of transformation:     
     q = q1\q2  (divide on the right to get new orientation) */
  c[0] = w1*w2 + x1*x2 + y1*y2 + z1*z2;
  c[1] = w1*x2 - x1*w2 - y1*z2 + z1*y2;
  c[2] = w1*y2 + x1*z2 - y1*w2 - z1*x2;
  c[3] = w1*z2 - x1*y2 + y1*x2 - z1*w2;           

  /* several steps required to calculate new translation: */  

  /* distance between tools */
  x = a[4] - b[4];
  y = a[5] - b[5];
  z = a[6] - b[6];

  /* q = q1\q*q1 (apply inverse of reference tranformation to distance) */

  /* first:  qtmp = q1\q */
  w2 = x1*x + y1*y + z1*z;
  x2 = w1*x - y1*z + z1*y;
  y2 = w1*y + x1*z - z1*x;
  z2 = w1*z - x1*y + y1*x;

  /* next:  q = qtmp*q1 */
  x = w2*x1 + x2*w1 + y2*z1 - z2*y1;
  y = w2*y1 - x2*z1 + y2*w1 + z2*x1;
  z = w2*z1 + x2*y1 - y2*x1 + z2*w1;
  
  /* find the normalization factor for q1 */
  f = 1.0f/(w1*w1 + x1*x1 + y1*y1 + z1*z1);
  c[4] = x*f;
  c[5] = y*f;
  c[6] = z*f;

  c[7] = 0.0;
}

/*---------------------------------------------------------
  converts the quaternion rotation + translation transform returned
  by the NDICAPI into a 4x4 base-zero row-major matrix, following
  the right-multiplication convention:
                     
                                /  m_11 m_12 m_11 m_12 \ 
  [ x' y' z' 1 ] = [ x y z 1 ]  |  m_21 m_22 m_21 m_22 |
                                |  m_31 m_32 m_31 m_32 |
                                \  m_41 m_42 m_41 m_42 /
  
  where m_11 == m[0][0],  m_12 = m[0][1],   etc.  This is the
  convention used by QuickDraw3D on the Macintosh and by
  DirectDraw3D on the PC.  

  This matrix can also be used in OpenGL without modification
  even though OpenGL follows the left-multiplication convention,
  because OpenGL uses a column-major storage model so references
  to the matrix are automagically transposed.
---------------------------------------------------------*/

void ndiTransformToMatrixf(const double trans[8], float matrix[16])
{
  double ww, xx, yy, zz, wx, wy, wz, xy, xz, yz, ss, rr, f;
  
  /* Determine some calculations done more than once. */
  ww = trans[0] * trans[0];
  xx = trans[1] * trans[1];
  yy = trans[2] * trans[2];
  zz = trans[3] * trans[3];
  wx = trans[0] * trans[1];
  wy = trans[0] * trans[2];
  wz = trans[0] * trans[3];
  xy = trans[1] * trans[2];
  xz = trans[1] * trans[3];
  yz = trans[2] * trans[3];

  rr = xx + yy + zz;
  ss = (ww - rr)*0.5f;
  /* Normalization factor */
  f = 2.0f/(ww + rr);
  
  /* Fill in the matrix. */
  matrix[0]  = (float)(( ss + xx)*f);
  matrix[1]  = (float)(( wz + xy)*f);
  matrix[2]  = (float)((-wy + xz)*f);
  matrix[3]  = 0;
  matrix[4]  = (float)((-wz + xy)*f);
  matrix[5]  = (float)(( ss + yy)*f);
  matrix[6]  = (float)(( wx + yz)*f);
  matrix[7]  = 0;
  matrix[8]  = (float)(( wy + xz)*f);
  matrix[9]  = (float)((-wx + yz)*f);
  matrix[10] = (float)(( ss + zz)*f);
  matrix[11] = 0;
  matrix[12] = (float)(trans[4]);
  matrix[13] = (float)(trans[5]);
  matrix[14] = (float)(trans[6]);
  matrix[15] = 1;
}

void ndiTransformToMatrixd(const double trans[8], double matrix[16])
{
  double ww, xx, yy, zz, wx, wy, wz, xy, xz, yz, ss, rr, f;
  
  /* Determine some calculations done more than once. */
  ww = trans[0] * trans[0];
  xx = trans[1] * trans[1];
  yy = trans[2] * trans[2];
  zz = trans[3] * trans[3];
  wx = trans[0] * trans[1];
  wy = trans[0] * trans[2];
  wz = trans[0] * trans[3];
  xy = trans[1] * trans[2];
  xz = trans[1] * trans[3];
  yz = trans[2] * trans[3];

  rr = xx + yy + zz;
  ss = (ww - rr)*0.5;
  /* Normalization factor */
  f = 2.0/(ww + rr);
  
  /* Fill in the matrix. */
  matrix[0]  = ( ss + xx)*f;
  matrix[1]  = ( wz + xy)*f;
  matrix[2]  = (-wy + xz)*f;
  matrix[3]  = 0;
  matrix[4]  = (-wz + xy)*f;
  matrix[5]  = ( ss + yy)*f;
  matrix[6]  = ( wx + yz)*f;
  matrix[7]  = 0;
  matrix[8]  = ( wy + xz)*f;
  matrix[9]  = (-wx + yz)*f;
  matrix[10] = ( ss + zz)*f;
  matrix[11] = 0;
  matrix[12] = trans[4];
  matrix[13] = trans[5];
  matrix[14] = trans[6];
  matrix[15] = 1;
}

/*---------------------------------------------------------
  extracts the roll,pitch,yaw rotation angles (in radians) from the 
  4x4 transform matrix (note that these are _not_ the same as Euler
  angles, which follow a different convention).  

  The rotations are described by the following three matrices, the
  product of which is the full rotation matrix.  Please note that
  the matrices follow the right-multiplication convention (see notes
  for ndiTransformToMatrix() ).

     roll around x axis       pitch around y axix       yaw around z axis

  / 1    0       0       \ / cos(pch) 0 -sin(pch) \ /  cos(rol) sin(rol)  0 \
  | 0  cos(yaw) sin(yaw) |*|    0     1     0     |*| -sin(rol) cos(rol)  0 |
  \ 0 -sin(yaw) cos(yaw) / \ sin(pch) 0  cos(pch) / \    0         0      1 / 
  
  Pay careful attention to the above: according to the right-multiplication
  convention, the order in which the rotations are applied are 1) roll 
  around x 2) pitch around y 3) yaw around z.

  This function was nabbed from the NDI ndicapi docs and heavily modified.
---------------------------------------------------------*/

void ndiAnglesFromMatrixf(float pdtAnglesRot[3], 
                          const float dtRotMatrix[16])
{
  double Yaw,CosYaw,SinYaw;
  
  Yaw    = atan2( dtRotMatrix[1], dtRotMatrix[0] );
  CosYaw = cos(Yaw);
  SinYaw = sin(Yaw);

  pdtAnglesRot[2] = (float)Yaw;
  pdtAnglesRot[1] = (float)atan2( -dtRotMatrix[2],
      (CosYaw * dtRotMatrix[0]) + (SinYaw * dtRotMatrix[1]) );
  pdtAnglesRot[0] = (float)atan2(
      (SinYaw * dtRotMatrix[8]) - (CosYaw * dtRotMatrix[9]),
      (-SinYaw * dtRotMatrix[4]) + (CosYaw * dtRotMatrix[5]) );
}

void ndiAnglesFromMatrixd(double pdtAnglesRot[3], 
                          const double dtRotMatrix[16])
{
  double Yaw,CosYaw,SinYaw;
  
  Yaw    = atan2( dtRotMatrix[1], dtRotMatrix[0] );
  CosYaw = cos(Yaw);
  SinYaw = sin(Yaw);

  pdtAnglesRot[2] = Yaw;
  pdtAnglesRot[1] = atan2( -dtRotMatrix[2],
      (CosYaw * dtRotMatrix[0]) + (SinYaw * dtRotMatrix[1]) );
  pdtAnglesRot[0] = atan2(
      (SinYaw * dtRotMatrix[8]) - (CosYaw * dtRotMatrix[9]),
      (-SinYaw * dtRotMatrix[4]) + (CosYaw * dtRotMatrix[5]) );
}

/*---------------------------------------------------------
  A very simple function to extract the translation portion of a
  transformation matrix.
---------------------------------------------------------*/

void ndiCoordsFromMatrixf(float coords[3], const float matrix[16])
{
  coords[0] = matrix[12];
  coords[1] = matrix[13];
  coords[2] = matrix[14];
}

void ndiCoordsFromMatrixd(double coords[3], const double matrix[16])
{
  coords[0] = matrix[12];
  coords[1] = matrix[13];
  coords[2] = matrix[14];
}

