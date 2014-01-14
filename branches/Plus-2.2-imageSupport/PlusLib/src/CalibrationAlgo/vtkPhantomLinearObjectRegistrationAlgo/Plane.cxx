/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#include "Plane.h"

Plane::Plane()
{
  this->Type = "Plane";
}

//-----------------------------------------------------------------------------

Plane::Plane( std::vector<double> newBasePoint, std::vector<double> newEndPoint1, std::vector<double> newEndPoint2 )
{
  this->Type = "Plane";
  this->BasePoint = newBasePoint;
  this->EndPoint1 = newEndPoint1;
  this->EndPoint2 = newEndPoint2;
}

//-----------------------------------------------------------------------------

Plane::~Plane()
{
  this->EndPoint1.clear();
  this->EndPoint2.clear();
}

//-----------------------------------------------------------------------------

std::vector<double> Plane::GetNormal()
{
  std::vector<double> vector = Cross( Subtract( this->EndPoint1, this->BasePoint ), Subtract( this->EndPoint2, this->BasePoint ) );
  vector = Multiply( 1 / Norm( vector ), vector );
  return vector;
}

//-----------------------------------------------------------------------------

std::vector<double> Plane::ProjectVector( std::vector<double> vector )
{
  std::vector<double> outVec = Subtract( vector, this->BasePoint );
  return Subtract( vector, Multiply( Dot( this->GetNormal(), outVec ), this->GetNormal() ) );
}

//-----------------------------------------------------------------------------

void Plane::Translate( std::vector<double> vector )
{
  for ( unsigned int i = 0; i < vector.size(); i++ )
  {
    this->BasePoint.at(i) = this->BasePoint.at(i) + vector.at(i);
    this->EndPoint1.at(i) = this->EndPoint1.at(i) + vector.at(i);
    this->EndPoint2.at(i) = this->EndPoint2.at(i) + vector.at(i);
  }
}

//-----------------------------------------------------------------------------

std::string Plane::ToXMLString() const
{
  std::stringstream xmlstring;

  xmlstring << "  <Plane";
  xmlstring << " Name=\"" << this->Name << "\"";
  xmlstring << " BasePoint=\"" << VectorToString( this->BasePoint ) << "\"";
  xmlstring << " EndPoint1=\"" << VectorToString( this->EndPoint1 ) << "\"";
  xmlstring << " EndPoint2=\"" << VectorToString( this->EndPoint2 ) << "\"";
  xmlstring << " />" << std::endl;

  return xmlstring.str();
}

//-----------------------------------------------------------------------------

void Plane::FromXMLElement( vtkXMLDataElement* element )
{

  if ( strcmp( element->GetName(), "Plane" ) != 0 )
  {
    return;  // If it's not a "log" or is the wrong tool jump to the next.
  }

  this->Name = std::string( element->GetAttribute( "Name" ) );
  this->BasePoint = StringToVector( std::string( element->GetAttribute( "BasePoint" ) ), 3 );
  this->EndPoint1 = StringToVector( std::string( element->GetAttribute( "EndPoint1" ) ), 3 );
  this->EndPoint2 = StringToVector( std::string( element->GetAttribute( "EndPoint2" ) ), 3 );

}
