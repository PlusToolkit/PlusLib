// ===========================================================================
// Program Modification Record
// ===========================================================================
// Component:	Utility
// ===========================================================================
// Class:		Driection
// ===========================================================================
// File Name:	Driection.cpp
// ===========================================================================
// Author:		Thomas K. Chen <chent@cs.queensu.ca>
//				School of Computing, Queen's University at Kingston, Canada
// ===========================================================================
// Purposes: 
// 1. Expand the reusability of class by generalization.
// 2. Elimate class dependency on Qt framework
// 3. Elimate class dependency on IGO framework
// 4. Change the dependency to C++ Standard Library
// ===========================================================================
// Change History:
// Author				Time						Release	Changes
// Thomas Kuiran Chen	Mon Jun 26 09:22 EDT 2003	1.0		Generalization
// Thomas Kuiran Chen	Sun Feb 26 11:18 EDT 2006	2.0		Upgrade to VTK 5.0
//
// ===========================================================================


#ifndef DIRECTION_H
#include "Direction.h"
#endif

//================================================================== 
// Author: Thomas K. Chen
// Modifications Made:
// - Added includes of C++ Standard string class: std::string
//==================================================================
// C++ standard includes 
#include <string>
#include <strstream>

//================================================================== 
// Author: Thomas K. Chen
// Modifications Made:
// - StringUtil class was modified to elimate the trace of Qt frame-
//   work, in particular, the QString and QObject classes. 
// - The include file was renamed to be the same as the class name.
//==================================================================
// local includes
#include "StringUtils.h"

// vtk includes
#include "vtkMath.h"

// standard includes
#include <cmath>

/*!
Constructor. Unit vector along w.
*/
Direction::Direction()
{
	m_pfDirection[0] = 0.0;
	m_pfDirection[1] = 0.0;
	m_pfDirection[2] = 1.0;
	m_pfDirection[3] = 0.0;
}
	
/*!
Constructor. Automatic normalization.

\param fU	The u (x) component.
\param fV	The v (y) component.
\param fW	The w (z) component.
*/
Direction::Direction(double fU, double fV, double fW)
{
	m_pfDirection[0] = fU;
	m_pfDirection[1] = fV;
	m_pfDirection[2] = fW;
	m_pfDirection[3] = 0.0;
	
	normalize();
}
	
/*!
Constructor. Automatic normalization.

\param pfDirection	The u, v and w components as in an array of floats.
*/
Direction::Direction(const double* pfDirection)
{
	m_pfDirection[0] = pfDirection[0];
	m_pfDirection[1] = pfDirection[1];
	m_pfDirection[2] = pfDirection[2];
	m_pfDirection[3] = 0.0;

	normalize();
}
	
/*!
Constructor. Create a direction from a string.

\param str	The string.
*/
//================================================================== 
// Author: Thomas K. Chen
// Modifications Made:
// - Changed the Qstring from Qt framework to C++ std::string class.
//==================================================================
Direction::Direction(const std::string& str)
{
    //================================================================== 
    // Author: Thomas K. Chen
    // Modifications Made:
    // - Replace QTextStream with std::strstream.
    //==================================================================
	std::strstream ss;
    ss << (const_cast<std::string*>(&str));
	ss >> *this;
}

/*!
Test for equality

\return	true if the directions are equal, otherwise false.
*/
bool Direction::operator==(const Direction& direction) const
{
	return	(m_pfDirection[0] == direction.m_pfDirection[0]) &&
			(m_pfDirection[1] == direction.m_pfDirection[1]) &&
			(m_pfDirection[2] == direction.m_pfDirection[2]) &&
			(m_pfDirection[3] == direction.m_pfDirection[3]);
}


/*!
Average two direction vectors.

If you want to calculate an average direction from more than two vectors
you have to convert them to Positions first, sum them up, and then convert
back to a Direction.

\param dir	Direction to average with this one.

\return	Average of this direction and the one passed in
*/
Direction Direction::average( const Direction& dir ) const
{
	return Direction(	m_pfDirection[0] + dir.m_pfDirection[0],
						m_pfDirection[1] + dir.m_pfDirection[1],
						m_pfDirection[2] + dir.m_pfDirection[2]	);
}
	
/*!
Normalizes the Direction.
*/
void Direction::normalize()
{
	double fNorm = 0.0F;
	for (int nIndex = 0; nIndex < 3; nIndex++)
	{
		fNorm += m_pfDirection[nIndex] * m_pfDirection[nIndex];
	}
	
	if (fNorm == 0.0F)
	{
		// force zero-vectors along the z-axis
		m_pfDirection[2] = 1.0F;
	}
	else
	{
		// otherwise normalize
		fNorm = sqrt(fNorm);
		for (int nIndex = 0; nIndex < 3; nIndex++)
		{
			m_pfDirection[nIndex] /= fNorm;
		}
	}
}

/*!
Convert Direction to a string.

\return	The string.
*/
//================================================================== 
// Author: Thomas K. Chen
// Modifications Made:
// - Changed the Qstring from Qt framework to C++ std::string class.
//================================================================== 
Direction::operator std::string() const
{
    //================================================================== 
    // Author: Thomas K. Chen
    // Modifications Made:
    // - Replace QTextStream with std::strstream.
    //==================================================================
	std::strstream ss;
    ss << *this;
    ss << std::ends;

	return ss.str();
}

/*!
Prints the text for the Direction to the stream. Space delimited.

\relates Direction
\param output	The output stream.
\param input	The direction.

\return the output stream reference
*/
//================================================================== 
// Author: Thomas K. Chen
// Modifications Made:
// - Replace QTextStream with std::strstream.
//==================================================================
std::strstream& operator<< (std::strstream& output, const Direction& input)
{
	output	<< input.getU() << " "
			<< input.getV() << " "
			<< input.getW();
	
	return output;
}

/*!
Read the text for the Direction from the stream. White space delimited.

\relates Direction
\param input	The input stream.
\param output	The direction.

\return	The input stream reference.
*/
//================================================================== 
// Author: Thomas K. Chen
// Modifications Made:
// - Replace QTextStream with std::strstream.
//==================================================================
std::strstream& operator>> (std::strstream& input, Direction& output)
{
	double fU;
	double fV;
	double fW;

	input >> fU >> fV >> fW;
	output = Direction(fU, fV, fW);

	return input;
}

/*!
Check the orthogonality of three directions.

\param dirA			The first direction vector.
\param dirB			The second direction vector.
\param dirC			The third direction vector.
\param fTolerance	The tolerance in degrees

\return true if the directions are orthogonal, otherwise false
*/
bool Direction::checkOrthogonality(	const Direction& dirA,
									const Direction& dirB,
									const Direction& dirC,
									double fTolerance	)
{

	bool bOrthogonal = false;

	double fAngle;
  fAngle = vtkMath::DegreesFromRadians(acos(vtkMath::Dot(const_cast<Direction&>(dirA), const_cast<Direction&>(dirB))));
	if (fabs(90.0F - fAngle) < fTolerance)
	{
		fAngle = vtkMath::DegreesFromRadians(acos(vtkMath::Dot(const_cast<Direction&>(dirB), const_cast<Direction&>(dirC))));
		if (fabs(90.0F - fAngle) < fTolerance)
		{
			fAngle = vtkMath::DegreesFromRadians(acos(vtkMath::Dot(const_cast<Direction&>(dirC), const_cast<Direction&>(dirA))));
			if (fabs(90.0F - fAngle) < fTolerance)
			{
				bOrthogonal = true;
			}
		}
	}

	return bOrthogonal;
}

/*!
Check if the directions form a right handed coordinate system.

\param dirX			The first direction vector.
\param dirY			The second direction vector.
\param dirZ			The third direction vector.
\param fTolerance	The tolerance in degrees

\return true if the directions form a right handed coordinate system, otherwise false
*/
bool Direction::checkRHSystem(	const Direction& dirX,
								const Direction& dirY,
								const Direction& dirZ,
								double fTolerance	)
{
	bool bRHSystem = false;

	double fAngle;
	double pfCross[3];
	
	vtkMath::Cross(const_cast<Direction&>(dirX), const_cast<Direction&>(dirY), pfCross);
	fAngle = vtkMath::DegreesFromRadians(asin(vtkMath::Norm(pfCross)));
	if (fabs(90.0F - fAngle) < fTolerance)
	{
		vtkMath::Cross(const_cast<Direction&>(dirY), const_cast<Direction&>(dirZ), pfCross);
		fAngle = vtkMath::DegreesFromRadians(asin(vtkMath::Norm(pfCross)));
		if (fabs(90.0F - fAngle) < fTolerance)
		{
			vtkMath::Cross(const_cast<Direction&>(dirZ), const_cast<Direction&>(dirX), pfCross);
			fAngle = vtkMath::DegreesFromRadians(asin(vtkMath::Norm(pfCross)));
			if (fabs(90.0F - fAngle) < fTolerance)
			{
				bRHSystem = true;
			}
		}
	}

	return bRHSystem;
}
