/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) John SH Baxter, Robarts Research Institute. All rights reserved.
See License.txt for details.
=========================================================================*/

#include "PlusConfigure.h"

#include "vtkObjectFactory.h"
#include "vtkPolyData.h"
#include "vtkPlusPolydataForce.h"
#include "vtkMatrix4x4.h"

//----------------------------------------------------------------------------

vtkStandardNewMacro( vtkPlusPolydataForce )

//----------------------------------------------------------------------------
vtkPlusPolydataForce::vtkPlusPolydataForce()
{
  // Constant for sigmoid function
  this->gammaSigmoid = 2;
  this->scaleForce = 20.0;
  lastPos[0] = 0;
  lastPos[1] = 0;
  lastPos[2] = 0;
}

//----------------------------------------------------------------------------
void vtkPlusPolydataForce::PrintSelf( ostream& os, vtkIndent indent )
{
  this->Superclass::PrintSelf( os, indent.GetNextIndent() );
  os << indent.GetNextIndent() << "Gamma Sigmoid: " << this->gammaSigmoid << endl;
}

//----------------------------------------------------------------------------
vtkPlusPolydataForce::~vtkPlusPolydataForce()
{

}

//----------------------------------------------------------------------------
void vtkPlusPolydataForce::SetInput( vtkPolyData* poly )
{
  this->poly = poly;
}

//----------------------------------------------------------------------------
int vtkPlusPolydataForce::GenerateForce( vtkMatrix4x4* transformMatrix, double force[3] )
{
  double distance;
  distance = CalculateDistance( transformMatrix->GetElement( 0, 3 ), transformMatrix->GetElement( 1, 3 ), transformMatrix->GetElement( 2, 3 ) );

  if ( distance <= 5 )
  {
    CalculateForce( transformMatrix->GetElement( 0, 3 ), transformMatrix->GetElement( 1, 3 ), transformMatrix->GetElement( 2, 3 ), force );
  }
  else
  {
    force[0] = ( 0 );
    force[1] = ( 0 );
    force[2] = ( 0 );
  }
  cout <<  " FORCE: " << force[0] << ",  " << force[1] << ",  " << force[2] << endl;
  return 1;
}

//----------------------------------------------------------------------------
double vtkPlusPolydataForce::CalculateDistance( double x, double y, double z )
{
  int pointID;
  double distance;
  double position[3];
  position[0] = x;
  position[1] = y;
  position[2] = z;
  pointID = this->poly->FindPoint(position);
  this->poly->GetPoint( pointID, this->lastPos );

  double tmp = pow( ( x - lastPos[0] ), 2 ) + pow( ( y - lastPos[1] ), 2 ) + pow( ( z - lastPos[2] ), 2 );
  distance = sqrt( tmp );
  return distance;
}

//----------------------------------------------------------------------------
int vtkPlusPolydataForce::SetGamma( double gamma )
{
  gammaSigmoid = gamma;

  return 0;
}

//----------------------------------------------------------------------------
void vtkPlusPolydataForce::CalculateForce( double x, double y, double z, double force[3] )
{
  double vector[3];

  vector[0] = fabs( x - this->lastPos[0] );
  vector[1] = fabs( y - this->lastPos[1] );
  vector[2] = fabs( z - this->lastPos[2] );

  cout << "vector[0]: " << vector[0] << endl;
  cout << "vector[1]: " << vector[1] << endl;
  cout << "vector[2]: " << vector[2] << endl;

  for ( int i = 0; i < 3; i++ )
  {
    if ( vector[i] > 0 )
    {
      force[i] = ( 0.1 / ( vector[i] * vector[i] ) ) * .6;
    }
  }
  cout << "X: " << force[0] << endl;
  cout << "Y: " << force[1] << endl;
  cout << "Z: " << force[2] << endl;

  if ( force[0] > 1 )
  {
    force[0] = .6;
  }
  if ( force[1] > 1 )
  {
    force[1] = .6;
  }
  if ( force[2] > 1 )
  {
    force[2] = .6;
  }
}
