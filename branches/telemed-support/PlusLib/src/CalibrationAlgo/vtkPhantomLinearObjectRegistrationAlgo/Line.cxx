/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#include "Line.h"

Line::Line()
{
  this->Type = "Line";
}

//-----------------------------------------------------------------------------

Line::Line( std::vector<double> newBasePoint, std::vector<double> newEndPoint )
{
  this->Type = "Line";
  this->BasePoint = newBasePoint;
  this->EndPoint = newEndPoint;
}

//-----------------------------------------------------------------------------

Line::~Line()
{
  this->EndPoint.clear();
}

//-----------------------------------------------------------------------------

std::vector<double> Line::GetDirection()
{
  std::vector<double> vector = Subtract( this->EndPoint, this->BasePoint );
  return Multiply( 1 / Norm( vector ), vector );
}

//-----------------------------------------------------------------------------

std::vector<double> Line::ProjectVector( std::vector<double> vector )
{
  std::vector<double> outVec = Subtract( vector, this->BasePoint );
  return Add( Multiply( Dot( this->GetDirection(), outVec ), this->GetDirection() ), this->BasePoint );
}

//-----------------------------------------------------------------------------

void Line::Translate( std::vector<double> vector )
{
  for ( unsigned int i = 0; i < vector.size(); i++ )
  {
    this->BasePoint.at(i) = this->BasePoint.at(i) + vector.at(i);
  this->EndPoint.at(i) = this->EndPoint.at(i) + vector.at(i);
  }
}

//-----------------------------------------------------------------------------

std::vector<double> Line::GetOrthogonalNormal1()
{
  // Find the two axis unit vectors least parallel with the direction vector
  std::vector<double> e1( 3, 0.0 );
  std::vector<double> e2( 3, 0.0 );
  if ( abs( this->GetDirection().at(1) ) <= abs( this->GetDirection().at(0) ) && abs( this->GetDirection().at(2) ) <= abs( this->GetDirection().at(0) ) )
  {
    e1.at(0) = 0; e1.at(1) = 1; e1.at(2) = 0;
  e2.at(0) = 0; e2.at(1) = 0; e2.at(2) = 1;
  }
  if ( abs( this->GetDirection().at(0) ) <= abs( this->GetDirection().at(1) ) && abs( this->GetDirection().at(2) ) <= abs( this->GetDirection().at(1) ) )
  {
    e1.at(0) = 1; e1.at(1) = 0; e1.at(2) = 0;
  e2.at(0) = 0; e2.at(1) = 0; e2.at(2) = 1;
  }
  if ( abs( this->GetDirection().at(0) ) <= abs( this->GetDirection().at(2) ) && abs( this->GetDirection().at(1) ) <= abs( this->GetDirection().at(2) ) )
  {
    e1.at(0) = 1; e1.at(1) = 0; e1.at(2) = 0;
  e2.at(0) = 0; e2.at(1) = 1; e2.at(2) = 0;
  }

  std::vector<double> Normal1 = Subtract( e1, Multiply( Dot( e1, this->GetDirection() ), this->GetDirection() ) );
  Normal1 = Multiply( 1 / Norm( Normal1 ), Normal1 );

  std::vector<double> Normal2 = Subtract( e2, Add( Multiply( Dot( e2, this->GetDirection() ), this->GetDirection() ), Multiply( Dot( e2, Normal1 ), Normal1 ) ) );
  Normal2 = Multiply( 1 / Norm( Normal2 ), Normal2 );

  return Normal1;
}

//-----------------------------------------------------------------------------

std::vector<double> Line::GetOrthogonalNormal2()
{
  // Find the two axis unit vectors least parallel with the direction vector
  std::vector<double> e1( 3, 0.0 );
  std::vector<double> e2( 3, 0.0 );
  if ( abs( this->GetDirection().at(1) ) <= abs( this->GetDirection().at(0) ) && abs( this->GetDirection().at(2) ) <= abs( this->GetDirection().at(0) ) )
  {
    e1.at(0) = 0; e1.at(1) = 1; e1.at(2) = 0;
  e2.at(0) = 0; e2.at(1) = 0; e2.at(2) = 1;
  }
  if ( abs( this->GetDirection().at(0) ) <= abs( this->GetDirection().at(1) ) && abs( this->GetDirection().at(2) ) <= abs( this->GetDirection().at(1) ) )
  {
    e1.at(0) = 1; e1.at(1) = 0; e1.at(2) = 0;
  e2.at(0) = 0; e2.at(1) = 0; e2.at(2) = 1;
  }
  if ( abs( this->GetDirection().at(0) ) <= abs( this->GetDirection().at(2) ) && abs( this->GetDirection().at(1) ) <= abs( this->GetDirection().at(2) ) )
  {
    e1.at(0) = 1; e1.at(1) = 0; e1.at(2) = 0;
  e2.at(0) = 0; e2.at(1) = 1; e2.at(2) = 0;
  }

  std::vector<double> Normal1 = Subtract( e1, Multiply( Dot( e1, this->GetDirection() ), this->GetDirection() ) );
  Normal1 = Multiply( 1 / Norm( Normal1 ), Normal1 );

  std::vector<double> Normal2 = Subtract( e2, Add( Multiply( Dot( e2, this->GetDirection() ), this->GetDirection() ), Multiply( Dot( e2, Normal1 ), Normal1 ) ) );
  Normal2 = Multiply( 1 / Norm( Normal2 ), Normal2 );

  return Normal2;
}

//-----------------------------------------------------------------------------

std::string Line::ToXMLString() const
{
  std::stringstream xmlstring;

  xmlstring << "  <Line";
  xmlstring << " Name=\"" << this->Name << "\"";
  xmlstring << " BasePoint=\"" << VectorToString( this->BasePoint ) << "\"";
  xmlstring << " EndPoint=\"" << VectorToString( this->EndPoint ) << "\"";
  xmlstring << " />" << std::endl;

  return xmlstring.str();
}

//-----------------------------------------------------------------------------

void Line::FromXMLElement( vtkXMLDataElement* element )
{

  if ( strcmp( element->GetName(), "Line" ) != 0 )
  {
    return;  // If it's not a "log" or is the wrong tool jump to the next.
  }

  this->Name = std::string( element->GetAttribute( "Name" ) );
  this->BasePoint = StringToVector( std::string( element->GetAttribute( "BasePoint" ) ), 3 );
  this->EndPoint = StringToVector( std::string( element->GetAttribute( "EndPoint" ) ), 3 );

}