/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef POINTOBSERVATIONBUFFER_H
#define POINTOBSERVATIONBUFFER_H

#include "PointObservation.h"
#include "LinearObject.h"
#include "Point.h"
#include "Line.h"
#include "Plane.h"
#include "Reference.h"

#include <string>
#include <sstream>
#include <vector>
#include <cmath>

#include "vnl/vnl_matrix.h"
#include "vnl/algo/vnl_symmetric_eigensystem.h"
#include "vnl/algo/vnl_svd.h"


class PointObservationBuffer
{
private:
  std::vector<PointObservation*> observations;

public:

  PointObservationBuffer();
  ~PointObservationBuffer();

  int Size() const;
  PointObservation* GetObservation( int index ) const;

  void AddObservation( PointObservation* newObservation );
  void Clear();

  void Translate( std::vector<double> translation );

  LinearObject* LeastSquaresLinearObject( int dof );
  void Filter( LinearObject* object, int filterWidth );

  vnl_matrix<double>* SphericalRegistration( PointObservationBuffer* fromPoints );
  vnl_matrix<double>* TranslationalRegistration( std::vector<double> toCentroid, std::vector<double> fromCentroid, vnl_matrix<double>* rotation );

  std::vector<PointObservationBuffer*> ExtractLinearObjects( int collectionFrames, double extractionThreshold, std::vector<int>* dof );

  std::string ToXMLString() const;
  void FromXMLElement( vtkXMLDataElement* element );

private:
  std::vector<double> CalculateCentroid();
  vnl_matrix<double>* CovarianceMatrix( std::vector<double> centroid );

};

#endif

//ETX