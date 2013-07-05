/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#include  "Planes.h"

//-----------------------------------------------------------------------------

Planes::Planes()
{
}

//-----------------------------------------------------------------------------

Planes::~Planes()
{
}

//-----------------------------------------------------------------------------

void  Planes::Reset()
{
  this->m_PlaneList.clear();
}

//-----------------------------------------------------------------------------

void Planes::InsertPlane(const Plane& newPlane)
{
  Plane copy = newPlane;
  this->m_PlaneList.push_back(copy);
}

//-----------------------------------------------------------------------------

void Planes::RemovePlane(int index)
{
  std::vector<Plane>::iterator it = this->m_PlaneList.begin();
  it += index;
  this->m_PlaneList.erase(it);
}

//-----------------------------------------------------------------------------

const Plane& Planes::GetPlane(int index)
{
  return this->m_PlaneList.at(index);
}

//-----------------------------------------------------------------------------

int Planes::GetNumberOfPlanes()
{
  return this->m_PlaneList.size();
}

//-----------------------------------------------------------------------------

std::vector<Plane>::const_iterator Planes::PlanesBegin() const
{
  return this->m_PlaneList.begin();
}

//-----------------------------------------------------------------------------

std::vector<Plane>::const_iterator Planes::PlanesEnd() const
{
  return this->m_PlaneList.end();
}

//-----------------------------------------------------------------------------

Planes& Planes::operator=(const Planes& rhs)
{
  if (this != &rhs)
  {
    this->m_PlaneList.clear();
    this->m_PlaneList.resize(rhs.m_PlaneList.size());
    std::copy(rhs.PlanesBegin(), rhs.PlanesEnd(), this->m_PlaneList.begin());
  }
  return *this;
}