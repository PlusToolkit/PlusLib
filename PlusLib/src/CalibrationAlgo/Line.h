/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef LINE_H
#define LINE_H

#include "LinearObject.h"
#include <cmath>
#include <sstream>
#include <string>
#include <vector>

// This class stores a vector of values and a string label
class Line : public LinearObject
{
public:
  std::vector<double> EndPoint;

  Line();
  Line( std::vector<double> newBasePoint, std::vector<double> newEndPoint );
  ~Line();

  std::vector<double> GetDirection();
  std::vector<double> ProjectVector( std::vector<double> vector );
  void Translate( std::vector<double> vector );

  std::vector<double> GetOrthogonalNormal1();
  std::vector<double> GetOrthogonalNormal2();

  virtual std::string ToXMLString() const;
  virtual void FromXMLElement( vtkXMLDataElement* element );
};

#endif