/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef PLANES_H
#define PLANES_H

#include "Plane.h"
#include <vector>

class Planes
{
public:
  Planes();
  ~Planes();

  void Reset();
  void InsertPlane(const Plane& newPlane);
  void RemovePlane(int index);
  const Plane& GetPlane(int index);
  int GetNumberOfPlanes();

  std::vector<Plane>::iterator PlanesBegin();
  std::vector<Plane>::iterator PlanesEnd();

protected:
  std::vector<Plane> m_PlaneList;
};

#endif