/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef LINEAROBJECTBUFFER_H
#define LINEAROBJECTBUFFER_H

#include "LinearObject.h"
#include "Reference.h"
#include "Point.h"
#include "Line.h"
#include "Plane.h"

#include <string>
#include <sstream>
#include <vector>
#include <cmath>

#include "vnl/vnl_matrix.h"
#include "vnl/algo/vnl_matrix_inverse.h"

// This class stores a vector of values and a string label
class LinearObjectBuffer
{
private:
	std::vector<LinearObject*> objects;

public:
  LinearObjectBuffer();
  ~LinearObjectBuffer();

  int Size() const;
  LinearObject* GetLinearObject( int index ) const;
  LinearObject* GetLinearObject( std::string name ) const;
  void AddLinearObject( LinearObject* newObject );
  void Concatenate( LinearObjectBuffer* catBuffer );

  void Translate( std::vector<double> vector );

  void CalculateSignature( LinearObjectBuffer* refBuffer );

  LinearObjectBuffer* GetMatches( LinearObjectBuffer* candidates, double matchingThreshold );

  std::vector<double> CalculateCentroid();

  std::string ToXMLString() const;
  void FromXMLElement( vtkXMLDataElement* element );

};

#endif
