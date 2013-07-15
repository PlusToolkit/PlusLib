//BTX

#ifndef POINTOBSERVATION_H
#define POINTOBSERVATION_H

#include "LinearObject.h"

#include <string>
#include <sstream>
#include <vector>
#include <cmath>

#include "vtkXMLDataElement.h"
#include "vnl/vnl_matrix.h"


// This class stores a vector of values only - we do not care about time
class PointObservation
{
public:
  std::vector<double> Observation;
  static const int SIZE = 3;

public:
  PointObservation();
  PointObservation( std::vector<double> newObservation );
  ~PointObservation();

  void Translate ( std::vector<double> translation );
  void Rotate( vnl_matrix<double>* rotation );

  std::string ToXMLString();
  void FromXMLElement( vtkXMLDataElement* element );
  bool FromXMLElement( vtkXMLDataElement* currElement, vtkXMLDataElement* prevElement );

};

#endif

//ETX