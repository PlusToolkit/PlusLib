/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#include "LinearObject.h"

//-----------------------------------------------------------------------------

LinearObject::LinearObject()
{
  this->Name = "";
  this->Type = "LinearObject";
}

//-----------------------------------------------------------------------------

LinearObject::~LinearObject()
{
  this->BasePoint.clear();
  this->Signature.clear();
}

//-----------------------------------------------------------------------------

double LinearObject::DistanceToVector( std::vector<double> vector )
{
  return Distance( vector, this->ProjectVector( vector ) );
}

//-----------------------------------------------------------------------------

double LinearObject::Distance( std::vector<double> v1, std::vector<double> v2 )
{
  if ( v1.size() != v2.size() )
  {
    return -1;
  }

  double distance = 0.0;
  for ( unsigned int i = 0; i < v1.size(); i++ )
  {
    distance += ( v1.at(i) - v2.at(i) ) * ( v1.at(i) - v2.at(i) );
  }

  return sqrt( distance );
}

//-----------------------------------------------------------------------------

double LinearObject::Norm( std::vector<double> vector )
{
  double norm = 0.0;
  for ( unsigned int i = 0; i < vector.size(); i++ )
  {
    norm += vector.at(i) * vector.at(i);
  }

  return sqrt( norm );
}

//-----------------------------------------------------------------------------

double LinearObject::Dot( std::vector<double> v1, std::vector<double> v2 )
{
  if ( v1.size() != v2.size() )
  {
    return -1;
  }

  double dot = 0.0;
  for ( unsigned int i = 0; i < v1.size(); i++ )
  {
    dot += v1.at(i) * v2.at(i);
  }

  return dot;
}

//-----------------------------------------------------------------------------

std::vector<double> LinearObject::Cross( std::vector<double> v1, std::vector<double> v2 )
{
  if ( v1.size() != 3 || v2.size() != 3 )
  {
    return std::vector<double>( 0, 0.0 );
  }

  std::vector<double> result( 3, 0.0 );
  result.at(0) = v1.at(1) * v2.at(2) - v1.at(2) * v2.at(1);
  result.at(1) = v1.at(2) * v2.at(0) - v1.at(0) * v2.at(2);
  result.at(2) = v1.at(0) * v2.at(1) - v1.at(1) * v2.at(0);

  return result;
}

//-----------------------------------------------------------------------------

std::vector<double> LinearObject::Add( std::vector<double> v1, std::vector<double> v2 )
{
  if ( v1.size() != v2.size() )
  {
    return std::vector<double>( 0, 0.0 );
  }

  std::vector<double> result( v1.size(), 0.0 );
  for ( unsigned int i = 0; i < v1.size(); i++ )
  {
    result.at(i) = v1.at(i) + v2.at(i);
  }

  return result;
}

//-----------------------------------------------------------------------------

std::vector<double> LinearObject::Subtract( std::vector<double> v1, std::vector<double> v2 )
{
  if ( v1.size() != v2.size() )
  {
    return std::vector<double>( 0, 0.0 );
  }

  std::vector<double> result( v1.size(), 0.0 );
  for ( unsigned int i = 0; i < v1.size(); i++ )
  {
    result.at(i) = v1.at(i) - v2.at(i);
  }

  return result;
}

//-----------------------------------------------------------------------------

std::vector<double> LinearObject::Multiply( double c, std::vector<double> vector )
{
  std::vector<double> result( vector.size(), 0.0 );
  for ( unsigned int i = 0; i < vector.size(); i++ )
  {
    result.at(i) = c * vector.at(i);
  }

  return result;
}

//-----------------------------------------------------------------------------

std::string LinearObject::VectorToString( std::vector<double> vector )
{
  std::stringstream s;

  for ( unsigned int i = 0; i < vector.size(); i++ )
  {
    s << vector.at(i) << " ";
  }

  return s.str();
}

//-----------------------------------------------------------------------------

std::vector<double> LinearObject::StringToVector( std::string s, int size )
{
  std::stringstream ss( s );
  double value;
  std::vector<double> vector( size, 0.0 );

  for ( int i = 0; i < size; i++ )
  {
    ss >> value;
    vector.at(i) = value;
  }

  return vector;
}