
#include "PointObservation.h"

PointObservation
::PointObservation()
{
}


PointObservation
::PointObservation( std::vector<double> newObervation )
{
  this->Observation = newObervation;
}


PointObservation
::~PointObservation()
{
  this->Observation.clear();
}


void PointObservation
::Translate( std::vector<double> translation )
{
  this->Observation = LinearObject::Add( this->Observation, translation );
}


void PointObservation
::Rotate( vnl_matrix<double>* rotation )
{
  vnl_matrix<double>* currPoint = new vnl_matrix<double>( PointObservation::SIZE, 1, 0.0 );
  currPoint->put( 0, 0, this->Observation.at(0) );
  currPoint->put( 1, 0, this->Observation.at(1) );
  currPoint->put( 2, 0, this->Observation.at(2) );

  vnl_matrix<double>* rotPoint = new vnl_matrix<double>( ( *rotation ) * ( *currPoint ) );
  this->Observation.at(0) = rotPoint->get( 0, 0 );
  this->Observation.at(1) = rotPoint->get( 1, 0 );
  this->Observation.at(2) = rotPoint->get( 2, 0 );
}


std::string PointObservation
::ToXMLString()
{
  std::stringstream xmlstring;
  std::stringstream matrixstring;
  matrixstring << "0 0 0 " << this->Observation.at(0) << " ";
  matrixstring << "0 0 0 " << this->Observation.at(1) << " ";
  matrixstring << "0 0 0 " << this->Observation.at(2) << " ";
  matrixstring << "0 0 0 1";

  xmlstring << "  <log";
  xmlstring << " TimeStampSec=\"" << 0 << "\"";
  xmlstring << " TimeStampNSec=\"" << 0 << "\"";
  xmlstring << " type=\"transform\"";
  xmlstring << " DeviceName=\"" << "Pointer" << "\"";
  xmlstring << " transform=\"" << matrixstring << "\"";
  xmlstring << " />" << std::endl;

  return xmlstring.str();
}


void PointObservation
::FromXMLElement( vtkXMLDataElement* element )
{

  if ( strcmp( element->GetName(), "log" ) != 0 || strcmp( element->GetAttribute( "type" ), "transform" ) != 0 )
  {
    return;  // If it's not a "log" or is the wrong tool jump to the next.
  }

  this->Observation = std::vector<double>( SIZE, 0.0 );

  std::stringstream matrixstring( std::string( element->GetAttribute( "transform" ) ) );
  double value;

  for ( int i = 0; i < 16; i++ )
  {
    matrixstring >> value;
	if ( i == 3 )
	{
	  this->Observation.at(0) = value;
	}
	if ( i == 7 )
	{
	  this->Observation.at(1) = value;
	}
	if ( i == 11 )
	{
	  this->Observation.at(2) = value;
	}
  }

}


bool PointObservation
::FromXMLElement( vtkXMLDataElement* currElement, vtkXMLDataElement* prevElement )
{
  const double ROTATION_THRESHOLD = 0.005;
  const double TRANSLATION_THRESHOLD = 0.5;

  // Check to ensure that the transformation has changed since the previously collected element
  if ( strcmp( currElement->GetName(), "log" ) != 0 || strcmp( currElement->GetAttribute( "type" ), "transform" ) != 0 )
  {
    return false;  // If it's not a "log" or is the wrong tool jump to the next.
  }
  if ( strcmp( prevElement->GetName(), "log" ) != 0 || strcmp( prevElement->GetAttribute( "type" ), "transform" ) != 0 )
  {
    return false;  // If it's not a "log" or is the wrong tool jump to the next.
  }

  std::stringstream currmatrixstring( std::string( currElement->GetAttribute( "transform" ) ) );
  std::stringstream prevmatrixstring( std::string( prevElement->GetAttribute( "transform" ) ) );
  double currValue, prevValue;

  std::vector<double> currRotation, prevRotation;
  std::vector<double> currTranslation, prevTranslation;

  for ( int i = 0; i < 16; i++ )
  {
    currmatrixstring >> currValue;
	prevmatrixstring >> prevValue;
	if ( i == 0 || i == 1 || i == 2 || i == 4 || i == 5 || i == 6 || i == 8 || i == 9 || i == 10 )
	{
	  currRotation.push_back( currValue );
	  prevRotation.push_back( prevValue );
	}
	if ( i == 3 || i == 7 || i == 11 )
	{
	  currTranslation.push_back( currValue );
	  prevTranslation.push_back( prevValue );
	}
  }

  if ( LinearObject::Distance( currRotation, prevRotation ) > ROTATION_THRESHOLD || LinearObject::Distance( currTranslation, prevTranslation ) > TRANSLATION_THRESHOLD )
  {
    this->FromXMLElement( currElement );
	return true;
  }

  return false;
}