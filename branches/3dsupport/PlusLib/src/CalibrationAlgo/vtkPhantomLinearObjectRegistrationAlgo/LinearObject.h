/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef LINEAROBJECT_H
#define LINEAROBJECT_H

#include "vtkXMLDataElement.h"
#include <cmath>
#include <sstream>
#include <string>
#include <vector>

// This class stores a vector of values and a string label
class LinearObject
{
public:
  std::string Name;
  std::string Type;
  std::vector<double> Signature;
  std::vector<double> BasePoint;

  static const int DIMENSION = 3;

public:

  LinearObject();
  ~LinearObject();

  double DistanceToVector( std::vector<double> vector );

  // TODO: This should be abstract, but I can't get this to work right now
  // We can just avoid using this class directly for now
  virtual std::vector<double> ProjectVector( std::vector<double> vector ) = 0;
  virtual void Translate( std::vector<double> vector ) = 0;

  virtual std::string ToXMLString() const = 0;
  virtual void FromXMLElement( vtkXMLDataElement* element ) = 0;

public:
  static double Distance( std::vector<double> v1, std::vector<double> v2 );
  static double Norm( std::vector<double> vector );
  static double Dot( std::vector<double> v1, std::vector<double> v2 );
  static std::vector<double> Cross( std::vector<double> v1, std::vector<double> v2 );

  static std::vector<double> Add( std::vector<double> v1, std::vector<double> v2 );
  static std::vector<double> Subtract( std::vector<double> v1, std::vector<double> v2 );
  static std::vector<double> Multiply( double c, std::vector<double> vector ); 

  static std::string VectorToString( std::vector<double> vector );
  static std::vector<double> StringToVector( std::string s, int size );

};

#endif

//ETX