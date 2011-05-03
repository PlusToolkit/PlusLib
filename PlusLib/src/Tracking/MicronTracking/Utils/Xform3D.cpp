/**************************************************************
*
*     Micron Tracker: Example C++ wrapper and Multi-platform demo
*   
*     Written by: 
*      Shahram Izadyar, Robarts Research Institute - London- Ontario , www.robarts.ca
*      Claudio Gatti, Claron Technology - Toronto -Ontario, www.clarontech.com
*
*     Copyright Claron Technology 2000-2003
*
***************************************************************/
#include "Xform3D.h"

Xform3D::Xform3D(int h)
{
  if (h != 0)
    this->m_handle = h;
  else
    this->m_handle = Xform3D_New();

  this->ownedByMe = TRUE;
}


/****************************/
/** */
Xform3D::~Xform3D()
{
  if (this->m_handle != 0 && this->ownedByMe == TRUE)
    Xform3D_Free(this->m_handle);
}

/****************************/
/** */
Xform3D* Xform3D::concatenate(Xform3D* subsequentXform)
{
  Xform3D* concatXf = new Xform3D;
  Xform3D_Concatenate(this->m_handle, subsequentXform->getHandle(), concatXf->getHandle());
  return concatXf;
}

/****************************/
/** Returns an inverted transform (ie, from target space back to source space)
Will generate a divide by 0 error if the xform is not inversible. */
Xform3D* Xform3D::inverse()
{
  Xform3D* inv = new Xform3D();
  Xform3D_Inverse(this->m_handle, inv->getHandle());
  return inv;
}

/****************************/
/** */
Xform3D* Xform3D::inBetween(Xform3D* secondXf, double secondFract0To1)
{
  Xform3D* newXf = new Xform3D();
  Xform3D_InBetween( this->m_handle, secondXf->getHandle(), secondFract0To1, newXf->getHandle() );
  return newXf;
}  

/****************************/
/** */
double Xform3D::getShift(int index)
{
  double s[3] ={0};
  Xform3D_ShiftGet(this->m_handle, s );
  return s[index];
}
/****************************/
/** */
void Xform3D::getShiftVector(double* s)
{
  double ss[3] = {0};
  Xform3D_ShiftGet(this->m_handle, ss);
  for(int i=0; i<3; i++)
    s[i] = ss[i];
}

/****************************/
/** */
void Xform3D::getRotateVector(double* resultVector, double* inVector, bool scaleIt)
{
  double v[3];
  Xform3D_RotateLocation(this->m_handle, inVector, v, scaleIt);
  for(int i=0; i<3; i++)
    resultVector[i] = v[i];
}


int Xform3D::getRotationMatrix(double *resultMatrix_3x3)
{
    int r = Xform3D_RotMatGet (this->m_handle, resultMatrix_3x3);
    return r;
}
