/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#include "LinearObjectBuffer.h"
#include "PlusCommon.h"

//-----------------------------------------------------------------------------

LinearObjectBuffer::LinearObjectBuffer()
{
}

//-----------------------------------------------------------------------------

LinearObjectBuffer::~LinearObjectBuffer()
{
  for ( int i = 0; i < this->Size(); i++ )
  {
    delete this->objects.at(i);
  }
  this->objects.clear();
}

//-----------------------------------------------------------------------------

int LinearObjectBuffer::Size() const
{
  return this->objects.size();
}

//-----------------------------------------------------------------------------

LinearObject* LinearObjectBuffer::GetLinearObject( int index ) const
{
  return this->objects.at(index);
}

//-----------------------------------------------------------------------------

LinearObject* LinearObjectBuffer::GetLinearObject( std::string name ) const
{
  for ( int i = 0; i < this->Size(); i++ )
  {
    if ( STRCASECMP( this->GetLinearObject(i)->Name.c_str(), name.c_str() ) == 0 )
    {
      return this->GetLinearObject(i);
    }
  }

  LinearObject* obj = NULL;
  return obj;
}

//-----------------------------------------------------------------------------

void LinearObjectBuffer::AddLinearObject( LinearObject* newObject )
{
  this->objects.push_back( newObject );
}

//-----------------------------------------------------------------------------

void LinearObjectBuffer::Concatenate( LinearObjectBuffer* catBuffer )
{
  for ( int i = 0; i < catBuffer->Size(); i++ )
  {
    this->AddLinearObject( catBuffer->GetLinearObject(i) );
  }
}

//-----------------------------------------------------------------------------

void LinearObjectBuffer::Translate( std::vector<double> vector )
{
  for ( int i = 0; i < this->Size(); i++ )
  {
    this->GetLinearObject(i)->Translate( vector );
  }
}

//-----------------------------------------------------------------------------

void LinearObjectBuffer::CalculateSignature( LinearObjectBuffer* refBuffer )
{
  // Calculate the signature of everything in this, assume the inputted object is a buffer of references
  for ( int i = 0; i < this->Size(); i++ )
  {
    std::vector<double> sig( refBuffer->Size(), 0.0 );
    for ( int j = 0; j < refBuffer->Size(); j++ )
    {
      sig.at(j) = this->GetLinearObject(i)->DistanceToVector( refBuffer->GetLinearObject(j)->BasePoint );
    }
    this->GetLinearObject(i)->Signature = sig;
  }
}

//-----------------------------------------------------------------------------

LinearObjectBuffer* LinearObjectBuffer::GetMatches( LinearObjectBuffer* candidates, double matchingThreshold )
{
  // For each object in this, find the object in candidates that has the closest signature
  LinearObjectBuffer* matchedCandidates = new LinearObjectBuffer();
  std::vector<LinearObject*> matchedObjects;
  if ( this->Size() == 0 || candidates->Size() == 0 )
  {
    this->objects = matchedObjects;
    return matchedCandidates;
  }

  for ( int i = 0; i < this->Size(); i++ )
  {

    LinearObject* closestObject = candidates->GetLinearObject(0);
    double closestDistance = LinearObject::Norm( LinearObject::Subtract( this->GetLinearObject(i)->Signature, closestObject->Signature ) );

    for ( int j = 0; j < candidates->Size(); j++ )
    {
      if ( LinearObject::Norm( LinearObject::Subtract( this->GetLinearObject(i)->Signature, candidates->GetLinearObject(j)->Signature ) ) < closestDistance )
      {
        closestObject = candidates->GetLinearObject(j);
        closestDistance = LinearObject::Norm( LinearObject::Subtract( this->GetLinearObject(i)->Signature, candidates->GetLinearObject(j)->Signature ) );
      }
    }

    // Only accept the matching if it is sufficiently good (this throws away potentially wrongly identified collected objects)
    if ( closestDistance < matchingThreshold )
    {
      matchedObjects.push_back( this->GetLinearObject(i) );
      matchedCandidates->AddLinearObject( closestObject );
    }

  }

  this->objects = matchedObjects;

  return matchedCandidates;
}

//-----------------------------------------------------------------------------

std::vector<double> LinearObjectBuffer::CalculateCentroid()
{
  const double CONDITION_THRESHOLD = 1e-3;

  // Assume each will take 3 rows. If it doesn't leaving them blank won't affect the result
  vnl_matrix<double>* A = new vnl_matrix<double>( LinearObject::DIMENSION * this->Size(), LinearObject::DIMENSION, 0.0 );
  vnl_matrix<double>* B = new vnl_matrix<double>( LinearObject::DIMENSION * this->Size(), 1, 0.0 );

  // We wish to solve the system A * X = B
  for ( int i = 0; i < this->Size(); i++ )
  {
    int row = LinearObject::DIMENSION * i; 
    // A = I for point, B = coordinates
    if ( strcmp( this->GetLinearObject(i)->Type.c_str(), "Point" ) == 0 )
    {
      Point* PointObject = (Point*) this->GetLinearObject(i);
      A->put( row + 0, 0, 1.0 ); 
      A->put( row + 1, 1, 1.0 );
      A->put( row + 2, 2, 1.0 );
      B->put( row + 0, 0, PointObject->BasePoint.at(0) );
      B->put( row + 1, 0, PointObject->BasePoint.at(1) );
      B->put( row + 2, 0, PointObject->BasePoint.at(2) );
    }

    // A = Normal 1, Normal 2, B = Dot( Normal 1, BasePoint ), Dot( Normal 2, BasePoint )
    if ( strcmp( this->GetLinearObject(i)->Type.c_str(), "Line" ) == 0 )
    {
      Line* LineObject = (Line*) this->GetLinearObject(i);
      A->put( row + 0, 0, LineObject->GetOrthogonalNormal1().at(0) );
      A->put( row + 0, 1, LineObject->GetOrthogonalNormal1().at(1) );
      A->put( row + 0, 2, LineObject->GetOrthogonalNormal1().at(2) );
      A->put( row + 1, 0, LineObject->GetOrthogonalNormal2().at(0) );
      A->put( row + 1, 1, LineObject->GetOrthogonalNormal2().at(1) );
      A->put( row + 1, 2, LineObject->GetOrthogonalNormal2().at(2) );
      B->put( row + 0, 0, LinearObject::Dot( LineObject->GetOrthogonalNormal1(), LineObject->BasePoint ) );
      B->put( row + 1, 0, LinearObject::Dot( LineObject->GetOrthogonalNormal2(), LineObject->BasePoint ) );
    }

    // A = Normal, B = Dot( Normal, BasePoint )
    if ( strcmp( this->GetLinearObject(i)->Type.c_str(), "Plane" ) == 0 )
    {
      Plane* PlaneObject = (Plane*) this->GetLinearObject(i);
      A->put( row + 0, 0, PlaneObject->GetNormal().at(0) );
      A->put( row + 0, 1, PlaneObject->GetNormal().at(1) );
      A->put( row + 0, 2, PlaneObject->GetNormal().at(2) );
      B->put( row + 0, 0, LinearObject::Dot( PlaneObject->GetNormal(), PlaneObject->BasePoint ) );
    }

  }

  // Now, calculate X
  vnl_matrix_inverse<double>* X = new vnl_matrix_inverse<double>( A->transpose() * (*A) );
  if ( X->well_condition() < CONDITION_THRESHOLD ) // This is the inverse of the condition number
  {
    throw std::logic_error("Failed - centroid calculation is ill-conditioned!");
  } // TODO: Error if ill-conditioned
  vnl_matrix<double>* Y = new vnl_matrix<double>( X->inverse() * A->transpose() * (*B) );

  std::vector<double> centroid( LinearObject::DIMENSION, 0.0 );
  centroid.at(0) = Y->get( 0, 0 );
  centroid.at(1) = Y->get( 1, 0 );
  centroid.at(2) = Y->get( 2, 0 );

  return centroid;
}

//-----------------------------------------------------------------------------

std::string LinearObjectBuffer::ToXMLString() const
{
  std::stringstream xmlstring;

  xmlstring << "<Geometry>" << std::endl;
  for ( int i = 0; i < this->Size(); i++ )
  {
    xmlstring << this->GetLinearObject(i)->ToXMLString();
  }
  xmlstring << "</Geometry>";

  return xmlstring.str();
}

//-----------------------------------------------------------------------------

void LinearObjectBuffer::FromXMLElement( vtkXMLDataElement* element )
{
  LinearObject* blankObject = NULL;
  this->objects = std::vector<LinearObject*>( 0, blankObject );

  int numElements = element->GetNumberOfNestedElements();

  for ( int i = 0; i < numElements; i++ )
  {

    vtkXMLDataElement* noteElement = element->GetNestedElement( i );

    if ( STRCASECMP( noteElement->GetName(), "Reference" ) == 0 )
    {
      Reference* newObject = new Reference();
      newObject->FromXMLElement( noteElement );
      this->AddLinearObject( newObject );
    }
    if ( STRCASECMP( noteElement->GetName(), "Point" ) == 0 )
    {
      Point* newObject = new Point();
      newObject->FromXMLElement( noteElement );
      this->AddLinearObject( newObject );
    }
    if ( STRCASECMP( noteElement->GetName(), "Line" ) == 0 )
    {
      Line* newObject = new Line();
      newObject->FromXMLElement( noteElement );
      this->AddLinearObject( newObject );
    }
    if ( STRCASECMP( noteElement->GetName(), "Plane" ) == 0 )
    {
      Plane* newObject = new Plane();
      newObject->FromXMLElement( noteElement );
      this->AddLinearObject( newObject );
    }
  }
}
