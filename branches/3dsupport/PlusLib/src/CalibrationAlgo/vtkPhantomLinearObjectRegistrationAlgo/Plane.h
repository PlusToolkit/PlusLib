/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef PLANE_H
#define PLANE_H


#include "LinearObject.h"
#include <cmath>
#include <sstream>
#include <string>
#include <vector>

// This class stores a vector of values and a string label
class Plane : public LinearObject
{
public:
  Plane();
  Plane( std::vector<double> newBasePoint, std::vector<double> newEndPoint1, std::vector<double> newEndPoint2 );
  ~Plane();

  std::vector<double> GetNormal();
  std::vector<double> ProjectVector( std::vector<double> vector );
  void Translate( std::vector<double> vector );

  virtual std::string ToXMLString() const;
  virtual void FromXMLElement( vtkXMLDataElement* element );

protected:
  std::vector<double> EndPoint1;
  std::vector<double> EndPoint2;
};

#endif

//ETX