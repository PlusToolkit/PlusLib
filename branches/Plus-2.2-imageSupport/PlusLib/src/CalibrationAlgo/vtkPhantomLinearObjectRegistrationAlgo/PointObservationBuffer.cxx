/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#include "PointObservationBuffer.h"
#include "PlusCommon.h"

PointObservationBuffer::PointObservationBuffer()
{
}

//-----------------------------------------------------------------------------

PointObservationBuffer::~PointObservationBuffer()
{
  for ( int i = 0; i < this->Size(); i++ )
  {
    delete this->observations.at(i);
  }
  this->observations.clear();
}

//-----------------------------------------------------------------------------

int PointObservationBuffer::Size() const
{
  return this->observations.size();
}

//-----------------------------------------------------------------------------

PointObservation* PointObservationBuffer::GetObservation( int index ) const
{
  return this->observations.at(index);
}

//-----------------------------------------------------------------------------

void PointObservationBuffer::AddObservation( PointObservation* newObservation )
{
  this->observations.push_back( newObservation );
}

//-----------------------------------------------------------------------------

void PointObservationBuffer::Translate( std::vector<double> translation )
{
  for ( int i = 0; i < this->Size(); i++ )
  {
    this->GetObservation(i)->Translate( translation );
  }
}

//-----------------------------------------------------------------------------

vnl_matrix<double>* PointObservationBuffer::SphericalRegistration( PointObservationBuffer* fromPoints )
{
  // Assume that it is already mean zero
  const double CONDITION_THRESHOLD = 1e-3;

  // Let us construct the data matrix
  vnl_matrix<double>* DataMatrix = new vnl_matrix<double>( PointObservation::SIZE, PointObservation::SIZE, 0.0 );

  // Pick two dimensions, and find their data matrix entry
  for ( int d1 = 0; d1 < PointObservation::SIZE; d1++ )
  {
    for ( int d2 = 0; d2 < PointObservation::SIZE; d2++ )
    {
      // Iterate over all times
      for ( int i = 0; i < this->Size(); i++ )
      {
        DataMatrix->put( d1, d2, DataMatrix->get( d1, d2 ) + fromPoints->GetObservation(i)->Observation.at(d1) * this->GetObservation(i)->Observation.at(d2) );
      }
    }
  }

  // Now we can calculate its svd
  vnl_svd<double>* SVDMatrix = new vnl_svd<double>( *DataMatrix, 0.0 );
  if ( SVDMatrix->well_condition() < CONDITION_THRESHOLD ) // This is the inverse of the condition number
  {
    LOG_ERROR("Failed - spherical registration is ill-conditioned!");
  } // TODO: Error if ill-conditioned

  return new vnl_matrix<double>( SVDMatrix->V() * SVDMatrix->U().transpose() );
}

//-----------------------------------------------------------------------------

vnl_matrix<double>* PointObservationBuffer::TranslationalRegistration( std::vector<double> toCentroid, std::vector<double> fromCentroid, vnl_matrix<double>* rotation )
{
  // Make matrices out of the centroids
  vnl_matrix<double>* toMatrix = new vnl_matrix<double>( PointObservation::SIZE, 1, 0.0 );
  vnl_matrix<double>* fromMatrix = new vnl_matrix<double>( PointObservation::SIZE, 1, 0.0 );

  for ( int i = 0; i < PointObservation::SIZE; i++ )
  {
    toMatrix->put( i, 0, toCentroid.at(i) );
    fromMatrix->put( i, 0, fromCentroid.at(i) );
  }

  return new vnl_matrix<double>( (*toMatrix) - (*rotation) * (*fromMatrix) );
}

//-----------------------------------------------------------------------------

LinearObject* PointObservationBuffer::LeastSquaresLinearObject( int dof )
{
  std::vector<double> centroid = this->CalculateCentroid();
  vnl_matrix<double>* cov = this->CovarianceMatrix( centroid );

  //Calculate the eigenvectors of the covariance matrix
  vnl_matrix<double> eigenvectors( PointObservation::SIZE, PointObservation::SIZE, 0.0 );
  vnl_vector<double> eigenvalues( PointObservation::SIZE, 0.0 );
  vnl_symmetric_eigensystem_compute( *cov, eigenvectors, eigenvalues );
  // Note: eigenvectors are ordered in increasing eigenvalue ( 0 = smallest, end = biggest )

  // Grab only the most important eigenvectors
  std::vector<double> Eigenvector1( PointObservation::SIZE, 0.0 ); // Smallest
  std::vector<double> Eigenvector2( PointObservation::SIZE, 0.0 ); // Medium
  std::vector<double> Eigenvector3( PointObservation::SIZE, 0.0 ); // Largest

  Eigenvector1.at(0) = eigenvectors.get( 0, 0 );
  Eigenvector1.at(1) = eigenvectors.get( 1, 0 );
  Eigenvector1.at(2) = eigenvectors.get( 2, 0 );

  Eigenvector2.at(0) = eigenvectors.get( 0, 1 );
  Eigenvector2.at(1) = eigenvectors.get( 1, 1 );
  Eigenvector2.at(2) = eigenvectors.get( 2, 1 );

  Eigenvector3.at(0) = eigenvectors.get( 0, 2 );
  Eigenvector3.at(1) = eigenvectors.get( 1, 2 );
  Eigenvector3.at(2) = eigenvectors.get( 2, 2 );

  // The threshold noise is twice the extraction threshold
  if ( dof == 0 )
  {
    return new Point( centroid );
  }
  if ( dof == 1 )
  {
    return new Line( centroid, LinearObject::Add( centroid, Eigenvector3 ) ); 
  }
  if ( dof == 2 )
  {
    return new Plane( centroid, LinearObject::Add( centroid, Eigenvector2 ), LinearObject::Add( centroid, Eigenvector3 ) );
  }

  LinearObject* obj = NULL;
  return obj;

}

//-----------------------------------------------------------------------------

void PointObservationBuffer::Filter( LinearObject* object, int filterWidth )
{
  const int THRESHOLD = 1e-3; // Deal with the case of very little noise
  bool changed = true;

  while ( changed )
  {
    std::vector<double> distances( this->Size(), 0 );
    double meanDistance = 0;
    double stdev = 0;

    // Calculate the distance of each point to the linear object
    for ( int i = 0; i < this->Size(); i++ )
    {
      distances.at(i) = object->DistanceToVector( this->GetObservation(i)->Observation );
      meanDistance = meanDistance + distances.at(i);
      stdev = stdev + distances.at(i) * distances.at(i);
    }
    meanDistance = meanDistance / this->Size();
    stdev = stdev / this->Size();
    stdev = sqrt( stdev - meanDistance * meanDistance );

    // Keep only the points that are within certain number of standard deviations
    std::vector<PointObservation*> newObservations;
    for ( int i = 0; i < this->Size(); i++ )
    {
      if ( distances.at(i) < filterWidth * stdev || distances.at(i) < THRESHOLD )
      {
        newObservations.push_back( this->GetObservation(i) );
      }
    }

    if ( newObservations.size() < this->Size() )
    {
      changed = true;
    }
    else
    {
      changed = false;
    }

    this->observations = newObservations;

  }

}

//-----------------------------------------------------------------------------

std::string PointObservationBuffer::ToXMLString() const
{
  std::stringstream xmlstring;

  for ( int i = 0; i < this->Size(); i++ )
  {
    xmlstring << this->GetObservation(i)->ToXMLString();
  }

  return xmlstring.str();
}

//-----------------------------------------------------------------------------

void PointObservationBuffer::FromXMLElement( vtkXMLDataElement* element )
{
  PointObservation* blankObservation = new PointObservation();
  this->observations = std::vector<PointObservation*>( 0, blankObservation );

  int numElements = element->GetNumberOfNestedElements();

  for ( int i = 0; i < numElements; i++ )
  {
    vtkXMLDataElement* noteElement = element->GetNestedElement( i );

    PointObservation* newObservation = new PointObservation();
    newObservation->FromXMLElement( noteElement );
    this->AddObservation( newObservation );

  }

}

//-----------------------------------------------------------------------------

vnl_matrix<double>* PointObservationBuffer::CovarianceMatrix( std::vector<double> centroid )
{
  // Construct a buffer for the zero mean data; initialize covariance matrix
  PointObservationBuffer* zeroMeanBuffer = new PointObservationBuffer();
  vnl_matrix<double> *cov = new vnl_matrix<double>( PointObservation::SIZE, PointObservation::SIZE );
  cov->fill( 0.0 );

  // Subtract the mean from each observation
  for ( int i = 0; i < this->Size(); i++ )
  {
    PointObservation* newObservation = new PointObservation();
    for( int d = 0; d < PointObservation::SIZE; d++ )
    {
      newObservation->Observation.push_back( this->GetObservation(i)->Observation.at(d) - centroid.at(d) );
    }
    zeroMeanBuffer->AddObservation( newObservation );
  }

  // Pick two dimensions, and find their covariance
  for ( int d1 = 0; d1 < PointObservation::SIZE; d1++ )
  {
    for ( int d2 = 0; d2 < PointObservation::SIZE; d2++ )
    {
      // Iterate over all times
      for ( int i = 0; i < this->Size(); i++ )
      {
        cov->put( d1, d2, cov->get( d1, d2 ) + zeroMeanBuffer->GetObservation(i)->Observation.at(d1) * zeroMeanBuffer->GetObservation(i)->Observation.at(d2) );
      }
      // Divide by the number of records
      cov->put( d1, d2, cov->get( d1, d2 ) / this->Size() );
    }
  }

  return cov;

}

//-----------------------------------------------------------------------------

std::vector<double> PointObservationBuffer::CalculateCentroid()
{
  // Calculate the centroid
  std::vector<double> centroid( PointObservation::SIZE, 0.0 );
  for ( int i = 0; i < this->Size(); i++ )
  {
    for ( int d = 0; d < PointObservation::SIZE; d++ )
    {
      centroid.at(d) = centroid.at(d) + this->GetObservation(i)->Observation.at(d);
    }
  }
  for ( int d = 0; d < PointObservation::SIZE; d++ )
  {
    centroid.at(d) = centroid.at(d) / this->Size();
  }

  return centroid;
}

//-----------------------------------------------------------------------------

std::vector<PointObservationBuffer*> PointObservationBuffer::ExtractLinearObjects( int collectionFrames, double extractionThreshold, std::vector<int>* dof )
{

  // First, let us identify the segmentation points and the associated DOFs, then we can divide up the points
  int TEST_INTERVAL = 21;

  PointObservationBuffer* eigenBuffer = new PointObservationBuffer(); // Note: 1 < 2 < 3
  int currStartIndex, currEndIndex;
  bool collecting = false;

  std::vector<PointObservationBuffer*> linearObjects;

  // Note: i is the start of the interval over which we will exam for linearity
  for ( int i = 0; i < this->Size() - TEST_INTERVAL; i++ )
  {
    // Create a smaller point observation buffer to work with at each iteration with the points of interest
    PointObservationBuffer* tempBuffer = new PointObservationBuffer();
    for ( int j = i; j < i + TEST_INTERVAL; j++ )
    {
      tempBuffer->AddObservation( this->GetObservation(j) );
    }

    // Find the eigenvalues of covariance matrix
    std::vector<double> centroid = tempBuffer->CalculateCentroid();
    vnl_matrix<double>* cov = tempBuffer->CovarianceMatrix( centroid );

    //Calculate the eigenvectors of the covariance matrix
    vnl_matrix<double> eigenvectors( PointObservation::SIZE, PointObservation::SIZE, 0.0 );
    vnl_vector<double> eigenvalues( PointObservation::SIZE, 0.0 );
    vnl_symmetric_eigensystem_compute( *cov, eigenvectors, eigenvalues );
    // Note: eigenvectors are ordered in increasing eigenvalue ( 0 = smallest, end = biggest )

    std::vector<double> eigen( 3, 0.0 );
    eigen.at(0) = eigenvalues.get( 0 );
    eigen.at(1) = eigenvalues.get( 1 );
    eigen.at(2) = eigenvalues.get( 2 );
    eigenBuffer->AddObservation( new PointObservation( eigen ) );

    if ( !collecting )
    {
      currStartIndex = i;
    }

    if ( eigenvalues.get( 0 ) < extractionThreshold )
    {
      collecting = true;
      continue;
    }
    collecting = false;
    currEndIndex = i;

    // Suppose that we have reached the end of a collecting section
    // If its too short then skip
    if ( currEndIndex - currStartIndex < collectionFrames )
    {
      continue;
    }


    // Now search for the largest interval of points which has the fewest DOF and satisfies the minimum interval
    for ( int e = PointObservation::SIZE - 1; e >= 0; e-- )
    {
      // Find the intervals where the eigenvalue is less than the threshold
      std::vector<int> dofInterval;
      dofInterval.push_back( currStartIndex );
      for ( int j = currStartIndex; j < currEndIndex; j++ )
      {
        if ( eigenBuffer->GetObservation(j)->Observation.at(e) > extractionThreshold )
        {
          dofInterval.push_back(j);
        }
      }
      dofInterval.push_back( currEndIndex );

      // Find the longest such interval
      int maxIntervalLength = 0;
      int maxIntervalIndex = 0;
      for ( int j = 0; j < dofInterval.size() - 1; j++ )
      {
        if ( dofInterval.at(j+1) - dofInterval.at(j) > maxIntervalLength )
        {
          maxIntervalLength = dofInterval.at(j+1) - dofInterval.at(j);
          maxIntervalIndex = j;
        }
      }

      // If the longest interval is too short, then ignore
      if ( maxIntervalLength < collectionFrames )
      {
        continue; 
      }

      // Otherwise, this is a collected linear object
      PointObservationBuffer* foundBuffer = new PointObservationBuffer();
      for ( int j = dofInterval.at(maxIntervalIndex); j < dofInterval.at( maxIntervalIndex + 1 ); j++ )
      {
        foundBuffer->AddObservation( this->GetObservation( j + TEST_INTERVAL ) );
      }

      linearObjects.push_back( foundBuffer );
      dof->push_back( PointObservation::SIZE - 1 - e );
      break;
    }
  }

  return linearObjects;
}
