/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#include "FidPatternRecognitionCommon.h"
#include "PlusConfigure.h"
#include "vtkMath.h"

//-----------------------------------------------------------------------------
PatternRecognitionResult::PatternRecognitionResult()
  : DotsFound(false)
  , Intensity(-1)
  , NumDots(-1)
{
}

//-----------------------------------------------------------------------------
double FidLine::ComputeAngleRad(const FidLine& line)
{
  //atan2 return the angle between the line and the x-axis from -Pi to Pi
  double angleRad = atan2(line.GetDirectionVector()[1],line.GetDirectionVector()[0]);
  return angleRad;
}

//-----------------------------------------------------------------------------
double FidLine::ComputeAngleRad(const FidLine& line1, const FidLine& line2)
{
  // a * b = |a| * |b| * cos(alpha)
  const double* a=line1.GetDirectionVector();
  const double* b=line2.GetDirectionVector();
  double angleBetweenLines=acos(a[0]*b[0]+a[1]*b[1]/sqrt(a[0]*a[0]+a[1]*a[1])/sqrt(b[0]*b[0]+b[1]*b[1]));
  // Normalize between -pi/2 .. +pi/2
  if (angleBetweenLines>vtkMath::Pi()/2)
  {
    angleBetweenLines -= vtkMath::Pi();
  }
  else if (angleBetweenLines<-vtkMath::Pi()/2)
  {
    angleBetweenLines += vtkMath::Pi();
  }
  // Return the absolute value (0..+pi/2)
  return fabs(angleBetweenLines);
}


//----------------------------------------------------------------------------
void FidLine::SetPoint(int aIndex, int aValue)
{
  Points[aIndex] = aValue;
}

//----------------------------------------------------------------------------
int FidLine::GetPoint(int aIndex) const
{
  return Points[aIndex];
}

//----------------------------------------------------------------------------
int FidLine::GetNumberOfPoints() const
{
  return Points.size();
}

//----------------------------------------------------------------------------
void FidLine::SetIntensity(double value)
{
  Intensity = value;
}

//----------------------------------------------------------------------------
double FidLine::GetIntensity() const
{
  return Intensity;
}

//----------------------------------------------------------------------------
void FidLine::SetLength(double value)
{
  Length = value;
}

//----------------------------------------------------------------------------
void FidLine::SetDirectionVector(int aIndex, double aValue)
{
  DirectionVector[aIndex] = aValue;
}

//----------------------------------------------------------------------------
const double* FidLine::GetDirectionVector() const
{
  return DirectionVector;
}

//----------------------------------------------------------------------------
void FidLine::SetStartPointIndex(int index)
{
  StartPointIndex = index;
}

//----------------------------------------------------------------------------
int FidLine::GetStartPointIndex() const
{
  return StartPointIndex;
}

//----------------------------------------------------------------------------
void FidLine::SetEndPointIndex(int index)
{
  EndPointIndex = index;
}

//----------------------------------------------------------------------------
int FidLine::GetEndPointIndex() const
{
  return EndPointIndex;
}

//----------------------------------------------------------------------------
void FidLine::ResizePoints(int aNewSize)
{
  Points.resize(aNewSize);
}

//----------------------------------------------------------------------------
void FidLine::AddPoint(int aPoint)
{
  Points.push_back(aPoint);
}

//-----------------------------------------------------------------------------
bool FidDot::IntensityLessThan( const FidDot &dot1, const FidDot &dot2 )
{
  /* Use > to get descending. */
  return dot1.GetDotIntensity() > dot2.GetDotIntensity();
}

//-----------------------------------------------------------------------------
bool FidDot::PositionLessThan( std::vector<FidDot>::iterator b1, std::vector<FidDot>::iterator b2 )
{
  /* Use > to get descending. */
  return b1->GetX() > b2->GetX();
}

//----------------------------------------------------------------------------
void FidDot::SetX(double value)
{
  m_X = value;
}

//----------------------------------------------------------------------------
double FidDot::GetX() const
{
  return m_X;
}

//----------------------------------------------------------------------------
void FidDot::SetY(double value)
{
  m_Y = value;
}

//----------------------------------------------------------------------------
double FidDot::GetY() const
{
  return m_Y;
}

//----------------------------------------------------------------------------
void FidDot::SetDotIntensity(double value)
{
  m_DotIntensity = value;
}

//----------------------------------------------------------------------------
double FidDot::GetDotIntensity() const
{
  return m_DotIntensity;
}

//-----------------------------------------------------------------------------
double  FidDot::GetDistanceFrom(FidDot &d) 
{ 
  return sqrt((m_X-d.m_X)*(m_X-d.m_X)+(m_Y-d.m_Y)*(m_Y-d.m_Y)); 
}

//----------------------------------------------------------------------------
bool FidDot::operator==(const FidDot& data) const
{
  return (m_X == data.m_X && m_Y == data.m_Y) ;
}

//-----------------------------------------------------------------------------
bool FidLine::lessThan(const FidLine& line1, const FidLine& line2)
{
  /* Use > to get descending. */
  return line1.GetIntensity() > line2.GetIntensity();
}

//-----------------------------------------------------------------------------
bool FidLine::compareLines(const FidLine& line1, const FidLine& line2)
{
  //make sure the lines are not the same, dot-wise
  for (unsigned int i=0; i<line1.GetNumberOfPoints(); i++)
  {
    if ( line1.GetPoint(i) < line2.GetPoint(i) )
    {
      return true;
    }
    else if ( line1.GetPoint(i) > line2.GetPoint(i) )
    {
      return false;
    }
  }
  return false;
}

//-----------------------------------------------------------------------------
void PatternRecognitionResult::Clear()
{
  DotsFound = false;
  Intensity = -1;
  FoundDotsCoordinateValue.clear();
  NumDots = 1;
  CandidateFidValues.clear(); 
}

//----------------------------------------------------------------------------
void PatternRecognitionResult::SetDotsFound(bool value)
{
  DotsFound = value;
}

//----------------------------------------------------------------------------
bool PatternRecognitionResult::GetDotsFound()
{
  return DotsFound;
}

//----------------------------------------------------------------------------
void PatternRecognitionResult::SetFoundDotsCoordinateValue(std::vector< std::vector<double> > value)
{
  FoundDotsCoordinateValue = value;
}

//----------------------------------------------------------------------------
std::vector< std::vector<double> >& PatternRecognitionResult::GetFoundDotsCoordinateValue()
{
  return FoundDotsCoordinateValue;
}

//----------------------------------------------------------------------------
void PatternRecognitionResult::SetCandidateFidValues(std::vector<FidDot> value)
{
  CandidateFidValues = value;
}

//----------------------------------------------------------------------------
const std::vector<FidDot>& PatternRecognitionResult::GetCandidateFidValues() const
{
  return CandidateFidValues;
}

//----------------------------------------------------------------------------
FidPattern::~FidPattern()
{

}

//----------------------------------------------------------------------------
const std::vector<FidWire>& FidPattern::GetWires() const
{
  return Wires;
}

//----------------------------------------------------------------------------
const std::vector<double>& FidPattern::GetDistanceToOriginMm() const
{
  return DistanceToOriginMm;
}

//----------------------------------------------------------------------------
const std::vector<double>& FidPattern::GetDistanceToOriginToleranceMm() const
{
  return DistanceToOriginToleranceMm;
}

//----------------------------------------------------------------------------
void FidPattern::AddWire(const FidWire& wire)
{
  this->Wires.push_back(wire);
}

//----------------------------------------------------------------------------
void FidPattern::AddDistanceToOriginElementMm(double aElement)
{
  this->DistanceToOriginMm.push_back(aElement);
}

//----------------------------------------------------------------------------
void FidPattern::AddDistanceToOriginToleranceElementMm(double aElement)
{
  this->DistanceToOriginToleranceMm.push_back(aElement);
}

//----------------------------------------------------------------------------
void FidWire::SetName(const std::string& aName)
{
  this->Name = aName;
}

//----------------------------------------------------------------------------
const std::string& FidWire::GetName() const
{
  return Name;
}

//----------------------------------------------------------------------------
NWire::~NWire()
{

}

//----------------------------------------------------------------------------
CoplanarParallelWires::~CoplanarParallelWires()
{

}
