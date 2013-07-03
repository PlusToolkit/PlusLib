/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#include "Reference.h"

//-----------------------------------------------------------------------------

Reference::Reference()
{
  this->Type = "Reference";
}

//-----------------------------------------------------------------------------

Reference::Reference( std::vector<double> newBasePoint )
{
  this->Type = "Reference";
  this->BasePoint = newBasePoint;
}

//-----------------------------------------------------------------------------

Reference::~Reference()
{
}

//-----------------------------------------------------------------------------

std::vector<double> Reference::ProjectVector( std::vector<double> vector )
{
  return this->BasePoint;
}

//-----------------------------------------------------------------------------

void Reference::Translate( std::vector<double> vector )
{
  for ( unsigned int i = 0; i < vector.size(); i++ )
  {
    this->BasePoint.at(i) = this->BasePoint.at(i) + vector.at(i);
  }
}

//-----------------------------------------------------------------------------

std::string Reference::ToXMLString() const
{
  std::stringstream xmlstring;

  xmlstring << "  <Reference";
  xmlstring << " Name=\"" << this->Name << "\"";
  xmlstring << " BasePoint=\"" << VectorToString( this->BasePoint ) << "\"";
  xmlstring << " />" << std::endl;

  return xmlstring.str();
}

//-----------------------------------------------------------------------------

void Reference::FromXMLElement( vtkXMLDataElement* element )
{
  if ( strcmp( element->GetName(), "Reference" ) != 0 )
  {
    return;  // If it's not a "log" or is the wrong tool jump to the next.
  }

  this->Name = std::string( element->GetAttribute( "Name" ) );
  this->BasePoint = StringToVector( std::string( element->GetAttribute( "BasePoint" ) ), 3 );

}