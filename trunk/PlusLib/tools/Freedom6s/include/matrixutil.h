/*
$Workfile: MatrixUtil.h $
$Revision: 1.1 $
$Author: cwedlake $
============================================================================
                    Copyright (c) 2001 MPB Technologies Inc.
                              All Rights Reserved

      This is unpublished proprietary source code of MPB Technologies Inc.
            No part of this software may be disclosed or published 
          without the expressed written consent of MPB Technologies Inc.
============================================================================*/

// Some extra bonus functions for doing special things with Matrices

#include <math.h>
#include "matrix.h"
#include "vector.h"

class MatrixUtil
{
public:

/*******************************************************************************
|
| FUNCTION NAME
|   GetTransPart, SetTransPart
|   
| DESCRIPTION
|   Extracts or sets the translational part of the E3 matrix, respectively
|
| INPUT
|  mat.................an E3 matrix containing valid translational information
|
| OUTPUT
|  vec.................a Vector3 containing valid translational information
|   
| RETURN VALUE
|   A 3-vector with the translational part of the E3 matrix
|
*******************************************************************************/

  static Vector3 GetTransPart( Matrix4x4& mat )
    { return Vector3(mat[0][3], mat[1][3], mat[2][3]); }

  static void SetTransPart( Matrix4x4& mat, Vector3& vec )
  {
    mat[0][3] = vec[0];
    mat[1][3] = vec[1];
    mat[2][3] = vec[2];
  }

/*******************************************************************************
|
| FUNCTION NAME
|   SetRotTrans
|   
| DESCRIPTION
|   Gets or sets the rotational and translational parts of an E3 matrix, respectively.
|
| INPUT
|  E3..................an E3 matrix, with 3x3 rot part, 1x3 trans part
|   rotation............3x3 rot part holds rotational information
|   translation.........1x3 trans part holds translational information
|
| OUTPUT
|   rotation............a Matrix3x3 to hold rotational information
|   translation.........a Vector3 to hold translational information
|   
| RETURN VALUE
|   None
|
*******************************************************************************/

  static void SetE3RotTrans( Matrix4x4& E3, Matrix3x3& rotation, Vector3& translation )
  {
    for (int i=0; i<3; i++)
      for (int j=0; j<3; j++)
        E3[i][j] = rotation[i][j];

    E3[0][3] = translation[0];
    E3[1][3] = translation[1];
    E3[2][3] = translation[2];
  }

  static void GetE3RotTrans( Matrix4x4& E3, Matrix3x3& rotation, Vector3& translation )
  {
    for (int i=0; i<3; i++)
      for (int j=0; j<3; j++)
        rotation[i][j] = E3[i][j];

    translation[0] = E3[0][3];
    translation[1] = E3[1][3];
    translation[2] = E3[2][3];
  }

/*******************************************************************************
|
| FUNCTION NAME
|   MakeRotMatX, MakeRotMatY, MakeRotMatZ
|   
| DESCRIPTION
|   Creates a 3x3 rotation matrix for rotating theta degrees around X, Y, or Z axis.
|
| INPUT
|  mat.........................................a 3x3 matrix
|   theta.......................................the degree of rotation
|
| OUTPUT
|  None
|   
| RETURN VALUE
|   None
|
*******************************************************************************/

  static void MakeRotMatX( Matrix3x3& mat, double theta )
  {
    mat[0][0] = 1.0;  mat[0][1] = 0.0;      mat[0][2] = 0.0;
    mat[1][0] = 0.0;  mat[1][1] = cos(theta);    mat[1][2] = -sin(theta);
    mat[2][0] = 0.0;  mat[2][1] = sin(theta);    mat[2][2] = cos(theta);
  }

  static void MakeRotMatY( Matrix3x3& mat, double theta )
  {
    mat[0][0] = cos(theta);    mat[0][1] = 0.0;  mat[0][2] = sin(theta);
    mat[1][0] = 0.0;      mat[1][1] = 1.0;  mat[1][2] = 0.0;
    mat[2][0] = -sin(theta);  mat[2][1] = 0.0;  mat[2][2] = cos(theta);
  }

  static void MakeRotMatZ( Matrix3x3& mat, double theta )
  {
    mat[0][0] = cos(theta);    mat[0][1] = -sin(theta);  mat[0][2] = 0.0;
    mat[1][0] = sin(theta);    mat[1][1] = cos(theta);    mat[1][2] = 0.0;
    mat[2][0] = 0.0;      mat[2][1] = 0.0;      mat[2][2] = 1.0;
  }

/*******************************************************************************
|
| FUNCTION NAME
|   MakeAntiSymmetric
|   
| DESCRIPTION
|   Create an antisymmetric matrix based on a 3-vector.
|
| INPUT
|  mat.........................................a 3x3 matrix
|   vec.........................................a 3-vector
|
| OUTPUT
|  None
|   
| RETURN VALUE
|   None
|
*******************************************************************************/

  static void MakeAntiSymmetric( Matrix3x3& mat, Vector3& vec )
  {
    mat[0][0] = mat[1][1] = mat[2][2] = 0.0;
    mat[0][1] = -vec[2];
    mat[1][0] = -mat[0][1];
    mat[0][2] = vec[1];
    mat[2][0] = -mat[0][2];
    mat[1][2] = -vec[0];
    mat[2][1] = -mat[1][2];
  }

/*******************************************************************************
|
| FUNCTION NAME
|   SetNullPart
|   
| DESCRIPTION
|   Set the null part of an E3 matrix to the correct values
|
| INPUT
|  mat.........................................a 4x4 E3 matrix
|
| OUTPUT
|  None
|   
| RETURN VALUE
|   None
|
*******************************************************************************/

  static void SetNullPart( Matrix4x4& mat )
  {
    mat[3][0] = mat[3][1] = mat[3][2] = 0.0;
    mat[3][3] = 1.0;
  }

};
