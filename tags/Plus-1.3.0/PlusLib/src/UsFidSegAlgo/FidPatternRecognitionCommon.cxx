/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#include "FidPatternRecognitionCommon.h"
#include "vtkMath.h"

//-----------------------------------------------------------------------------

PatternRecognitionResult::PatternRecognitionResult()
{
  this->m_DotsFound = false;
	this->m_Angles = -1;
	this->m_Intensity = -1;
	this->m_NumDots = -1;
}

//-----------------------------------------------------------------------------

float Line::ComputeAngle(Line &line)
{
  float x = line.GetDirectionVector(0);
  float y = line.GetDirectionVector(1);

  //float angle = acos((-y)/( sqrt(x*x+y*y)));
 
  float angle = atan2(y,x);

  /*if(angle > vtkMath::Pi()/2)
    angle -= vtkMath::Pi();
  else if(angle < -vtkMath::Pi()/2)
    angle += vtkMath::Pi();*/

  return angle;
}

//-----------------------------------------------------------------------------

float Line::ComputeHalfSpaceAngle(Line &line)
{
  float x = line.GetDirectionVector(0);
  float y = line.GetDirectionVector(1);

  //float angle = acos((-y)/( sqrt(x*x+y*y)));
 
  float angle = atan2(y,x);

  if(angle > vtkMath::Pi()/2)
    angle -= vtkMath::Pi();
  else if(angle < -vtkMath::Pi()/2)
    angle += vtkMath::Pi();

  return angle;
}

bool Dot::lessThan( Dot &dot1, Dot &dot2 )
{
	/* Use > to get descending. */
	return dot1.GetDotIntensity() > dot2.GetDotIntensity();
}

//-----------------------------------------------------------------------------

bool Position::lessThan( std::vector<Dot>::iterator b1, std::vector<Dot>::iterator b2 )
{
	/* Use > to get descending. */
	return b1->GetX() > b2->GetX();
}

//-----------------------------------------------------------------------------

bool LinePair::lessThan( LinePair &pair1, LinePair &pair2 )
{
	/* Use > to get descending. */
	return pair1.GetLinePairIntensity() > pair2.GetLinePairIntensity();
}

//-----------------------------------------------------------------------------

bool Line::lessThan( Line &line1, Line &line2 )
{
	/* Use > to get descending. */
	return line1.GetIntensity() > line2.GetIntensity();
}

//-----------------------------------------------------------------------------

bool Line::compareLines(Line line1, Line line2 )
{
  for (unsigned int i=0; i<line1.GetPoints()->size(); i++)
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

bool SortedAngle::lessThan( SortedAngle &pt1, SortedAngle &pt2 )
{
	/* Use > to get descending. */
	return pt1.GetAngle() > pt2.GetAngle();
}

//-----------------------------------------------------------------------------

void PatternRecognitionResult::Clear()
{
	m_DotsFound = false;
	m_Angles = -1;
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