/*
$Workfile: Matrix.h $
$Revision: 1.1 $
$Author: cwedlake $
============================================================================
                    Copyright (c) 2001 MPB Technologies Inc.
                              All Rights Reserved

      This is unpublished proprietary source code of MPB Technologies Inc.
            No part of this software may be disclosed or published 
          without the expressed written consent of MPB Technologies Inc.
============================================================================*/

/* Matrix.h */
/* Class to represent a matrix. */

#ifndef _MATRIX_H_
#define _MATRIX_H_

#include <math.h>
#include <assert.h>

#include "vector.h"

#define MATRIX_EQ_OP(dowhat)            \
      { for (int row=0; row < ROWS; row++)    \
          for (int col=0; col < COLS; col++)  \
            m_Data[row][col] dowhat;      \
        return *this;                \
      }

#define MATRIX_OP(dowhat)              \
      { Matrix<TYPE, ROWS, COLS> result;      \
        for (int row=0; row < ROWS; row++)    \
          for (int col=0; col < COLS; col++)  \
            result.m_Data[row][col] dowhat;  \
        return result;              \
      }

template <class TYPE, int ROWS, int COLS>
class Matrix
{
public:
    Matrix() {};

    // constructor for a pointer to a two-dimensional array of type TYPE
    // (array must be arranged such that it can be accessed as array[rows][cols])
    Matrix( TYPE pT[ROWS][COLS] )
      { for (int row=0; row < ROWS; row++)
          for (int col=0; col < COLS; col++)
            m_Data[row][col] = pT[row][col];
      }
    Matrix( const TYPE pT[ROWS][COLS] )
      { for (int row=0; row < ROWS; row++)
          for (int col=0; col < COLS; col++)
            m_Data[row][col] = pT[row][col];
      }

    // access matrix elements
    TYPE* operator[](int row)
      { return m_Data[row]; }

    const TYPE* operator[](int row) const
      { return m_Data[row]; }

  // access data array
  TYPE (* getData())[COLS]
    { return m_Data; }

    // set the whole matrix to zero
    void setZero()
      { for (int row=0; row<ROWS; row++)
          for (int col=0; col<COLS; col++)
            m_Data[row][col] = 0;
      }

    // set the whole matrix to identity matrix
    void setIdentity()
      { int row;
    for (row=0; row<ROWS; row++)
          for (int col=0; col<COLS; col++)
            m_Data[row][col] = 0;
        for(row=0; row<ROWS; row++)
          m_Data[row][row] = 1.0;
      }

    // assignment
    Matrix<TYPE, ROWS, COLS> operator=( const Matrix<TYPE, ROWS, COLS>& m )
      MATRIX_EQ_OP(=m.m_Data[row][col])

    // assignment add
    Matrix<TYPE, ROWS, COLS> operator+=( const Matrix<TYPE, ROWS, COLS>& m )
      MATRIX_EQ_OP(+=m.m_Data[row][col])

    // assignment subtract
    Matrix<TYPE, ROWS, COLS> operator-=( const Matrix<TYPE, ROWS, COLS>& m )
      MATRIX_EQ_OP(-=m.m_Data[row][col])

    // add two matrices
    Matrix<TYPE, ROWS, COLS> operator+( const Matrix<TYPE, ROWS, COLS>& m ) const
      MATRIX_OP (=m_Data[row][col] + m.m_Data[row][col])

    // subtract two matrices
    Matrix<TYPE, ROWS, COLS> operator-( const Matrix<TYPE, ROWS, COLS>& m ) const
      MATRIX_OP (=m_Data[row][col] - m.m_Data[row][col])

    // Multiply a matrix with this one
    // note: this only works for square matrices
    // corrected October 2002 (Bob Stoughton - thanks)
    Matrix<TYPE, ROWS, COLS> operator*=( const Matrix<TYPE, ROWS, COLS>& m )
      { Matrix<TYPE, ROWS, COLS> result;
        Matrix<TYPE, ROWS, COLS> tmp;
        for (int row=0; row < ROWS; row++)
        {
          for (int col=0; col < COLS; col++) 
          {
            tmp[row][col] = 0;
            for (int i=0; i < COLS; i++)
              tmp[row][col] += m_Data[row][i] * m.m_Data[i][col];
          }
        }
        for (int col=0; col < COLS; col++) 
          for (int row=0; row < ROWS; row++)
            m_Data[row][col] = tmp[row][col];
        return *this;
      }

    // Multiply two matrices
    // note: this only works for square matrices
    Matrix<TYPE, ROWS, COLS> operator*( const Matrix<TYPE, ROWS, COLS>& m ) const
      { Matrix<TYPE, ROWS, COLS> result;
        for (int row=0; row < ROWS; row++)
          for (int col=0; col < COLS; col++) {
            result.m_Data[row][col] = 0;
            for (int i=0; i < COLS; i++)
              result.m_Data[row][col] += m_Data[row][i] * m.m_Data[i][col];
          }
        return result;
      }

    // Return a vector from a matrix-vector multiplication
    Vector<TYPE, COLS> operator*( const Vector<TYPE, COLS>& v ) const
      { Vector<TYPE, COLS> result;
        for (int row=0; row < ROWS; row++) {
          result[row] = 0;
          for (int col=0; col < COLS; col++)
            result[row] += m_Data[row][col] * v[col];
        }
        return result;
      }

    // multiply a matrix by a variable of type TYPE
    Matrix<TYPE, ROWS, COLS> operator*( const TYPE d ) const
      MATRIX_OP(=m_Data[row][col]*d)

    // negate this matrix
    Matrix<TYPE, ROWS, COLS> operator-( )
      MATRIX_OP(=-m_Data[row][col])

    // Return the transpose of this matrix
    Matrix<TYPE, ROWS, COLS> transpose() const
      { assert( ROWS == COLS );
        MATRIX_OP(=m_Data[col][row]);
      }

protected:
    TYPE m_Data[ROWS][COLS];
};

// Some standard Matrix classes
typedef Matrix<double, 3, 3> Matrix3x3;
typedef Matrix<double, 4, 4> Matrix4x4;
typedef Matrix<double, 6, 6> Matrix6x6;

#undef MATRIX_EQ_OP
#undef MATRIX_OP

#endif // _MATRIX_H_
