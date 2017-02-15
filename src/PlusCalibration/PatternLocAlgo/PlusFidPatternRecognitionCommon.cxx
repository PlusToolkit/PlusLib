/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"
#include "PlusFidPatternRecognitionCommon.h"
#include "vtkMath.h"

//-----------------------------------------------------------------------------
PlusPatternRecognitionResult::PlusPatternRecognitionResult()
  : DotsFound(false)
  , Intensity(-1)
  , NumDots(-1)
{
}

//-----------------------------------------------------------------------------
double PlusFidLine::ComputeAngleRad(const PlusFidLine& line)
{
  //atan2 return the angle between the line and the x-axis from -Pi to Pi
  double angleRad = atan2(line.GetDirectionVector()[1], line.GetDirectionVector()[0]);
  return angleRad;
}

//-----------------------------------------------------------------------------
double PlusFidLine::ComputeAngleRad(const PlusFidLine& line1, const PlusFidLine& line2)
{
  // a * b = |a| * |b| * cos(alpha)
  const double* a = line1.GetDirectionVector();
  const double* b = line2.GetDirectionVector();
  double angleBetweenLines = acos(a[0] * b[0] + a[1] * b[1] / sqrt(a[0] * a[0] + a[1] * a[1]) / sqrt(b[0] * b[0] + b[1] * b[1]));
  // Normalize between -pi/2 .. +pi/2
  if (angleBetweenLines > vtkMath::Pi() / 2)
  {
    angleBetweenLines -= vtkMath::Pi();
  }
  else if (angleBetweenLines < -vtkMath::Pi() / 2)
  {
    angleBetweenLines += vtkMath::Pi();
  }
  // Return the absolute value (0..+pi/2)
  return fabs(angleBetweenLines);
}


//----------------------------------------------------------------------------
void PlusFidLine::SetPoint(int aIndex, int aValue)
{
  Points[aIndex] = aValue;
}

//----------------------------------------------------------------------------
int PlusFidLine::GetPoint(int aIndex) const
{
  return Points[aIndex];
}

//----------------------------------------------------------------------------
unsigned int PlusFidLine::GetNumberOfPoints() const
{
  return Points.size();
}

//----------------------------------------------------------------------------
void PlusFidLine::SetIntensity(double value)
{
  Intensity = value;
}

//----------------------------------------------------------------------------
double PlusFidLine::GetIntensity() const
{
  return Intensity;
}

//----------------------------------------------------------------------------
void PlusFidLine::SetLength(double value)
{
  Length = value;
}

//----------------------------------------------------------------------------
void PlusFidLine::SetDirectionVector(int aIndex, double aValue)
{
  DirectionVector[aIndex] = aValue;
}

//----------------------------------------------------------------------------
const double* PlusFidLine::GetDirectionVector() const
{
  return DirectionVector;
}

//----------------------------------------------------------------------------
void PlusFidLine::SetStartPointIndex(int index)
{
  StartPointIndex = index;
}

//----------------------------------------------------------------------------
int PlusFidLine::GetStartPointIndex() const
{
  return StartPointIndex;
}

//----------------------------------------------------------------------------
void PlusFidLine::SetEndPointIndex(int index)
{
  EndPointIndex = index;
}

//----------------------------------------------------------------------------
int PlusFidLine::GetEndPointIndex() const
{
  return EndPointIndex;
}

//----------------------------------------------------------------------------
void PlusFidLine::ResizePoints(int aNewSize)
{
  Points.resize(aNewSize);
}

//----------------------------------------------------------------------------
void PlusFidLine::AddPoint(int aPoint)
{
  Points.push_back(aPoint);
}

//-----------------------------------------------------------------------------
bool PlusFidDot::IntensityLessThan(const PlusFidDot& dot1, const PlusFidDot& dot2)
{
  /* Use > to get descending. */
  return dot1.GetDotIntensity() > dot2.GetDotIntensity();
}

//-----------------------------------------------------------------------------
bool PlusFidDot::PositionLessThan(std::vector<PlusFidDot>::iterator b1, std::vector<PlusFidDot>::iterator b2)
{
  /* Use > to get descending. */
  return b1->GetX() > b2->GetX();
}

//----------------------------------------------------------------------------
void PlusFidDot::SetX(double value)
{
  m_X = value;
}

//----------------------------------------------------------------------------
double PlusFidDot::GetX() const
{
  return m_X;
}

//----------------------------------------------------------------------------
void PlusFidDot::SetY(double value)
{
  m_Y = value;
}

//----------------------------------------------------------------------------
double PlusFidDot::GetY() const
{
  return m_Y;
}

//----------------------------------------------------------------------------
void PlusFidDot::SetDotIntensity(double value)
{
  m_DotIntensity = value;
}

//----------------------------------------------------------------------------
double PlusFidDot::GetDotIntensity() const
{
  return m_DotIntensity;
}

//-----------------------------------------------------------------------------
double  PlusFidDot::GetDistanceFrom(PlusFidDot& d)
{
  return sqrt((m_X - d.m_X) * (m_X - d.m_X) + (m_Y - d.m_Y) * (m_Y - d.m_Y));
}

//----------------------------------------------------------------------------
bool PlusFidDot::operator==(const PlusFidDot& data) const
{
  return (m_X == data.m_X && m_Y == data.m_Y) ;
}

//-----------------------------------------------------------------------------
bool PlusFidLine::lessThan(const PlusFidLine& line1, const PlusFidLine& line2)
{
  /* Use > to get descending. */
  return line1.GetIntensity() > line2.GetIntensity();
}

//-----------------------------------------------------------------------------
bool PlusFidLine::compareLines(const PlusFidLine& line1, const PlusFidLine& line2)
{
  //make sure the lines are not the same, dot-wise
  for (unsigned int i = 0; i < line1.GetNumberOfPoints(); i++)
  {
    if (line1.GetPoint(i) < line2.GetPoint(i))
    {
      return true;
    }
    else if (line1.GetPoint(i) > line2.GetPoint(i))
    {
      return false;
    }
  }
  return false;
}

//-----------------------------------------------------------------------------
void PlusPatternRecognitionResult::Clear()
{
  DotsFound = false;
  Intensity = -1;
  FoundDotsCoordinateValue.clear();
  NumDots = 1;
  CandidateFidValues.clear();
}

//----------------------------------------------------------------------------
void PlusPatternRecognitionResult::SetDotsFound(bool value)
{
  DotsFound = value;
}

//----------------------------------------------------------------------------
bool PlusPatternRecognitionResult::GetDotsFound()
{
  return DotsFound;
}

//----------------------------------------------------------------------------
void PlusPatternRecognitionResult::SetFoundDotsCoordinateValue(std::vector< std::vector<double> > value)
{
  FoundDotsCoordinateValue = value;
}

//----------------------------------------------------------------------------
std::vector< std::vector<double> >& PlusPatternRecognitionResult::GetFoundDotsCoordinateValue()
{
  return FoundDotsCoordinateValue;
}

//----------------------------------------------------------------------------
void PlusPatternRecognitionResult::SetCandidateFidValues(std::vector<PlusFidDot> value)
{
  CandidateFidValues = value;
}

//----------------------------------------------------------------------------
const std::vector<PlusFidDot>& PlusPatternRecognitionResult::GetCandidateFidValues() const
{
  return CandidateFidValues;
}

//----------------------------------------------------------------------------
PlusFidPattern::~PlusFidPattern()
{

}

//----------------------------------------------------------------------------
const std::vector<PlusFidWire>& PlusFidPattern::GetWires() const
{
  return Wires;
}

//----------------------------------------------------------------------------
const std::vector<double>& PlusFidPattern::GetDistanceToOriginMm() const
{
  return DistanceToOriginMm;
}

//----------------------------------------------------------------------------
const std::vector<double>& PlusFidPattern::GetDistanceToOriginToleranceMm() const
{
  return DistanceToOriginToleranceMm;
}

//----------------------------------------------------------------------------
void PlusFidPattern::AddWire(const PlusFidWire& wire)
{
  this->Wires.push_back(wire);
}

//----------------------------------------------------------------------------
void PlusFidPattern::AddDistanceToOriginElementMm(double aElement)
{
  this->DistanceToOriginMm.push_back(aElement);
}

//----------------------------------------------------------------------------
void PlusFidPattern::SetDistanceToOriginElementMm(int index, double aElement)
{
  this->DistanceToOriginMm[index] = aElement;
}

//----------------------------------------------------------------------------
void PlusFidPattern::AddDistanceToOriginToleranceElementMm(double aElement)
{
  this->DistanceToOriginToleranceMm.push_back(aElement);
}

//----------------------------------------------------------------------------
void PlusFidPattern::SetDistanceToOriginToleranceElementMm(int index, double aElement)
{
  this->DistanceToOriginMm[index] = aElement;
}

//----------------------------------------------------------------------------
void PlusFidWire::SetName(const std::string& aName)
{
  this->Name = aName;
}

//----------------------------------------------------------------------------
std::string PlusFidWire::GetName() const
{
  return Name;
}

//----------------------------------------------------------------------------
PlusNWire::~PlusNWire()
{

}

//----------------------------------------------------------------------------
PlusCoplanarParallelWires::~PlusCoplanarParallelWires()
{

}
