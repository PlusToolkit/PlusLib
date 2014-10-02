/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"

#include "FidPatternRecognitionCommon.h"
#include "vtkMath.h"

//-----------------------------------------------------------------------------

PatternRecognitionResult::PatternRecognitionResult()
{
  this->m_DotsFound = false;
  this->m_Intensity = -1;
  this->m_NumDots = -1;
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

//-----------------------------------------------------------------------------
double  FidDot::GetDistanceFrom(FidDot &d) 
{ 
  return sqrt((m_X-d.m_X)*(m_X-d.m_X)+(m_Y-d.m_Y)*(m_Y-d.m_Y)); 
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
  for (unsigned int i=0; i<line1.GetPoints().size(); i++)
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
  m_DotsFound = false;
  m_Intensity = -1;
  m_FoundDotsCoordinateValue.clear();
  m_NumDots = 1;
  m_CandidateFidValues.clear(); 
}

//-----------------------------------------------------------------------------

PatternRecognitionResult::~PatternRecognitionResult()
{

}

//-----------------------------------------------------------------------------