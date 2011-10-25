/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#include "FidPatternRecognitionCommon.h"

//-----------------------------------------------------------------------------

PatternRecognitionResult::PatternRecognitionResult()
{
  this->m_DotsFound = false;
	this->m_Angles = -1;
	this->m_Intensity = -1;
	this->m_NumDots = -1;
}

//-----------------------------------------------------------------------------

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
	return line1.GetLineIntensity() > line2.GetLineIntensity();
}

//-----------------------------------------------------------------------------

bool Line::compareLines(const Line &line1, const Line &line2 )
{
	for (int i=0; i<3; i++)
	{
		if ( line1.GetLinePoint(i) < line2.GetLinePoint(i) )
		{
			return true;
		}
		else if ( line1.GetLinePoint(i) > line2.GetLinePoint(i) )
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