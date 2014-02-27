/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#include "Point.h"

Point::Point()
{
  this->Type = "Point";
}

//-----------------------------------------------------------------------------

Point::Point( std::vector<double> newBasePoint )
{
  this->Type = "Point";
  this->BasePoint = newBasePoint;
}

//-----------------------------------------------------------------------------

Point::~Point()
{
}

//-----------------------------------------------------------------------------

std::vector<double> Point::ProjectVector( std::vector<double> vector )
{
  return this->BasePoint;
}

//-----------------------------------------------------------------------------

void Point::Translate( std::vector<double> vector )
{
  for ( unsigned int i = 0; i < vector.size(); i++ )
  {
    this->BasePoint.at(i) = this->BasePoint.at(i) + vector.at(i);
  }
}

//-----------------------------------------------------------------------------

std::string Point::ToXMLString() const
{
  std::stringstream xmlstring;

  xmlstring << "  <Point";
  xmlstring << " Name=\"" << this->Name << "\"";
  xmlstring << " BasePoint=\"" << VectorToString( this->BasePoint ) << "\"";
  xmlstring << " />" << std::endl;

  return xmlstring.str();
}

//-----------------------------------------------------------------------------

void Point::FromXMLElement( vtkXMLDataElement* element )
{

  if ( strcmp( element->GetName(), "Point" ) != 0 )
  {
    return;  // If it's not a "log" or is the wrong tool jump to the next.
  }

  this->Name = std::string( element->GetAttribute( "Name" ) );
  this->BasePoint = StringToVector( std::string( element->GetAttribute( "BasePoint" ) ), 3 );

}
