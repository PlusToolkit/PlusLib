/**************************************************************
*
*     Micron Tracker: Example C++ wrapper and Multi-platform demo
*
*     Written by:
*      Shahram Izadyar, Robarts Research Institute - London- Ontario , www.robarts.ca
*      Claudio Gatti, Claron Technology - Toronto -Ontario, www.clarontech.com
*
*   Revised by:
*     Gregory Bootsma, Princess Margaret Hospital - Toronto - Ontario
*
*     Copyright Claron Technology 2000-2013
*
***************************************************************/

#include <MTC.h>

#include "Xform3D.h"

//----------------------------------------------------------------------------
Xform3D::Xform3D(mtHandle h)
{
  if (h != 0)
  {
    this->Handle = h;
  }
  else
  {
    this->Handle = Xform3D_New();
  }

  this->OwnedByMe = true;
}

//----------------------------------------------------------------------------
Xform3D::~Xform3D()
{
  if (this->Handle != 0 && this->OwnedByMe == true)
  {
    Xform3D_Free(this->Handle);
    this->Handle = NULL;
  }
}

//----------------------------------------------------------------------------
mtHandle Xform3D::getHandle()
{
  return this->Handle;
}

//----------------------------------------------------------------------------
Xform3D* Xform3D::concatenate(Xform3D* subsequentXform)
{
  Xform3D* concatXf = new Xform3D;
  Xform3D_Concatenate(this->Handle, subsequentXform->getHandle(), concatXf->getHandle());
  return concatXf;
}

//----------------------------------------------------------------------------
/* Will generate a divide by 0 error if the xform is not invertible. */
Xform3D* Xform3D::inverse()
{
  Xform3D* inv = new Xform3D();
  Xform3D_Inverse(this->Handle, inv->getHandle());
  return inv;
}

//----------------------------------------------------------------------------
Xform3D* Xform3D::inBetween(Xform3D* secondXf, double secondFract0To1)
{
  Xform3D* newXf = new Xform3D();
  Xform3D_InBetween(this->Handle, secondXf->getHandle(), secondFract0To1, newXf->getHandle());
  return newXf;
}

//----------------------------------------------------------------------------
double Xform3D::getQuaternion(int index)
{
  double q[4] = {0};
  Xform3D_RotQuaternionsGet(this->Handle, q);
  return q[index];
}

//----------------------------------------------------------------------------
int Xform3D::getQuaternionVector(double* q)
{
  return Xform3D_RotQuaternionsGet(this->Handle, q);
}

//----------------------------------------------------------------------------
int Xform3D::getHazardState()
{
  mtMeasurementHazardCode code = mtNone;
  Xform3D_HazardCodeGet(this->Handle, &code);
  return code;
}

//----------------------------------------------------------------------------
double Xform3D::getShift(int index)
{
  double s[3] = {0};
  Xform3D_ShiftGet(this->Handle, s);
  return s[index];
}

//----------------------------------------------------------------------------
int Xform3D::getShiftVector(double* s)
{
  double ss[3] = {0};
  int result = Xform3D_ShiftGet(this->Handle, ss);
  for (int i = 0; i < 3; i++)
  {
    s[i] = ss[i];
  }
  return result;
}

//----------------------------------------------------------------------------
int Xform3D::getRotateVector(double* resultVector, double* inVector, bool scaleIt)
{
  double v[3] = {0};
  int result = Xform3D_RotateLocation(this->Handle, inVector, v, scaleIt);
  for (int i = 0; i < 3; i++)
  {
    resultVector[i] = v[i];
  }
  return result;
}
