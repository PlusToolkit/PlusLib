/*
$Workfile: Vector.h $
$Revision: 1.1 $
$Author: cwedlake $
============================================================================
                    Copyright (c) 2001 MPB Technologies Inc.
                              All Rights Reserved

      This is unpublished proprietary source code of MPB Technologies Inc.
            No part of this software may be disclosed or published 
          without the expressed written consent of MPB Technologies Inc.
============================================================================*/

/* Vector.h */

#ifndef _VECTOR_H_
#define _VECTOR_H_

#include <math.h>
#include <assert.h>

#define VECTOR_EQ_OP(dowhat)    \
  { for (int i=0; i < DIM; i++)  \
      m_Data[i] dowhat;      \
    return *this; }

#define VECTOR_OP(dowhat)      \
  { Vector<TYPE, DIM> v;      \
    for (int i=0; i < DIM; i++)  \
      v.m_Data[i] dowhat;    \
    return v; }

template <class TYPE, int DIM>
class Vector
{
public:
  Vector() {};

  // contructor for 3-dimensional vector
  Vector( TYPE a0, TYPE a1, TYPE a2 )
    { m_Data[0] = a0; m_Data[1] = a1; m_Data[2] = a2; }

  // contructor for 4-dimensional vector
  Vector( TYPE a0, TYPE a1, TYPE a2, TYPE a3 )
    { m_Data[0] = a0; m_Data[1] = a1; m_Data[2] = a2; m_Data[3] = a3; }

  // contructor for 7-dimensional vector
  Vector( TYPE a0, TYPE a1, TYPE a2, TYPE a3, TYPE a4, TYPE a5, TYPE a6 )
    { m_Data[0] = a0; m_Data[1] = a1; m_Data[2] = a2; m_Data[3] = a3;
      m_Data[4] = a4; m_Data[5] = a5; m_Data[6] = a6; }

  // constructor for pointer to an array of values
  Vector( const TYPE pT[DIM] )
    { for (int i=0; i<DIM; i++) m_Data[i] = pT[i]; }

  // set all the values of this vector from an array of values
  void set( const TYPE *pT )
    { for (int i=0; i<DIM; i++) m_Data[i] = pT[i]; }

  // return the address of the data for this vector
  TYPE *getData()
    { return &(m_Data[0]); }
  const TYPE *getData() const
    { return &(m_Data[0]); }
  /*
  operator TYPE* () const
    { return &(m_Data[0]); }
  operator TYPE* ()
    { return &(m_Data[0]); }
  */

  // return the norm of this vector
  TYPE norm()
    { TYPE n=0;
      for (int i=0; i < DIM; i++)
        n += m_Data[i]*m_Data[i];
      return (TYPE)sqrt(n);
    }

  // normalize this vector
  void normalize()
  { TYPE n = norm();
    for (int i=0; i < DIM; i++)
      m_Data[i] /= n;
  }

  // return the dot product of two vectors
  TYPE operator*( const Vector<TYPE, DIM> vec ) const
    { TYPE d=0;
      for (int i=0; i < DIM; i++)
        d += m_Data[i]*vec.m_Data[i];
      return d;
    }

  // return the cross product of two vectors
  // (only works for 3-dimensional vectors)
  Vector<TYPE, DIM> cross( const Vector<TYPE, DIM> vec ) const
    { assert(DIM==3);
      Vector<TYPE, DIM> v;
      v.m_Data[0] = m_Data[1]*vec.m_Data[2] - m_Data[2]*vec.m_Data[1];
      v.m_Data[1] = m_Data[2]*vec.m_Data[0] - m_Data[0]*vec.m_Data[2];
      v.m_Data[2] = m_Data[0]*vec.m_Data[1] - m_Data[1]*vec.m_Data[0];
      return v;
    }

  // set the whole vector to zero
  void setZero()
    { for (int i=0; i<DIM; i++) m_Data[i] = 0; }
      
  // return the value of a specified dimension
  TYPE& operator[]( int dim )
    { return m_Data[dim]; }
  const TYPE operator[]( int dim ) const
    { return m_Data[dim]; }

  // copy a vector
  Vector<TYPE, DIM>& operator=( const Vector<TYPE, DIM> vec )
    VECTOR_EQ_OP(=vec.m_Data[i]);

  // add a vector to this one
  Vector<TYPE, DIM>& operator+=( const Vector<TYPE, DIM> vec )
    VECTOR_EQ_OP(+=vec.m_Data[i])

  // subtract a vector from this one
  Vector<TYPE, DIM>& operator-=( const Vector<TYPE, DIM> vec )
    VECTOR_EQ_OP(-=vec.m_Data[i])

  // multiply the vector by a variable of type TYPE
  Vector<TYPE, DIM>& operator*=( const TYPE d )
    VECTOR_EQ_OP(*=d)

  // divide the vector by a variable of type TYPE
  Vector<TYPE, DIM>& operator/=( const TYPE d )
    VECTOR_EQ_OP(/=d)

  // add two vectors together
  Vector<TYPE, DIM> operator+( const Vector<TYPE, DIM> vec ) const
    VECTOR_OP(=m_Data[i]+vec.m_Data[i])

  // subtract two vectors
  Vector<TYPE, DIM> operator-( const Vector<TYPE, DIM> vec ) const
    VECTOR_OP(=m_Data[i]-vec.m_Data[i])

  // multiply a vector and a variable of type TYPE
  Vector<TYPE, DIM> operator*( const TYPE d ) const
    VECTOR_OP(=m_Data[i]*d)

  // negate this vector
  Vector<TYPE, DIM> operator-( ) const
    VECTOR_OP(=-m_Data[i])

protected:
  TYPE m_Data[DIM];
};

// Some standard Vector classes
typedef Vector<double, 2> Vector2;
typedef Vector<double, 3> Vector3;
typedef Vector<double, 4> Vector4;
typedef Vector<double, 7> Vector7;

#undef VECTOR_EQ_OP
#undef VECTOR_OP

#endif // _VECTOR_H_
