// ===========================================================================
// Program Modification Record
// ===========================================================================
// Component:	Utility
// ===========================================================================
// Class:		Position
// ===========================================================================
// File Name:	Position.cpp
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
// Thomas Kuiran Chen	Mon Jun 28 09:22 EDT 2003	1.0		Generalization
// Thomas Kuiran Chen	Sun Feb 26 11:18 EDT 2006	2.0		Upgrade to VTK 5.0
//
// ===========================================================================


#ifndef POSITION_H
#include "Position.h"
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

// standard includes
#include <cassert>
#include <cmath>

/*!
Constructor. Creates a position at the origin.
*/
Position::Position()
{
	m_pfPosition[0] = 0;
	m_pfPosition[1] = 0;
	m_pfPosition[2] = 0;
	m_pfPosition[3] = 1;
}
	
/*!
Constructor

\param fX	The x coordinate.
\param fY	The y coordinate.
\param fZ	The z coordinate.
\param fW	The w component for homogeneous coordinates
*/
Position::Position(double fX, double fY, double fZ, double fW)
{
	assert(fW != 0);
	
	m_pfPosition[0] = fX / fW;
	m_pfPosition[1] = fY / fW;
	m_pfPosition[2] = fZ / fW;
	m_pfPosition[3] = 1;
}
	
/*!
Constructor

\param pfPosition	The x, y and z coordinates as in an array of floats
\param fW			The w component for homogeneous coordinates
*/
Position::Position(const double* pfPosition, double fW)
{
	assert(fW != 0);
	
	m_pfPosition[0] = pfPosition[0] / fW;
	m_pfPosition[1] = pfPosition[1] / fW;
	m_pfPosition[2] = pfPosition[2] / fW;
	m_pfPosition[3] = 1;
}
	
/*!
Copy constructor.

\param position	The position to copy
*/
Position::Position(const Position& position)
{
	m_pfPosition[0] = position.m_pfPosition[0];
	m_pfPosition[1] = position.m_pfPosition[1];
	m_pfPosition[2] = position.m_pfPosition[2];
	m_pfPosition[3] = position.m_pfPosition[3];
}

/*!
Constructor. Create a position from a string.

\param str	The string.
*/
//================================================================== 
// Author: Thomas K. Chen
// Modifications Made:
// - Changed the Qstring from Qt framework to C++ std::string class.
//==================================================================
Position::Position(const std::string& str)
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
Copy assignment.

\param position	The position to copy
*/
Position& Position::operator=(const Position& position)
{
	// check for self assignment
	if (this != &position)
	{
		m_pfPosition[0] = position.m_pfPosition[0];
		m_pfPosition[1] = position.m_pfPosition[1];
		m_pfPosition[2] = position.m_pfPosition[2];
		m_pfPosition[3] = position.m_pfPosition[3];
	}
	
	return *this;
}
	
/*!
Test for equality

\return	true if the positions are equal, otherwise false.
*/
bool Position::operator==(const Position& position) const
{
	return	(m_pfPosition[0] == position.m_pfPosition[0]) &&
			(m_pfPosition[1] == position.m_pfPosition[1]) &&
			(m_pfPosition[2] == position.m_pfPosition[2]) &&
			(m_pfPosition[3] == position.m_pfPosition[3]);
}

/*!
Vector subtraction on position vectors.

\param rhs	Right-hand side of subtraction.

\return	*this minus right-hand side.
*/
Position Position::operator-( const Position& rhs ) const
{
	return Position(
		m_pfPosition[0] - rhs.m_pfPosition[0],
		m_pfPosition[1] - rhs.m_pfPosition[1],
		m_pfPosition[2] - rhs.m_pfPosition[2],
		1.0
		);
}

/*!
Vector addition on position vectors.

\param rhs	Right-hand side of addition.

\return	*this plus right-hand side.
*/
Position Position::operator+( const Position& rhs ) const
{
	return Position(
		m_pfPosition[0] + rhs.m_pfPosition[0],
		m_pfPosition[1] + rhs.m_pfPosition[1],
		m_pfPosition[2] + rhs.m_pfPosition[2],
		1.0
		);
}
	
/*!
Calculate the distance between two points.

\param	position	The other position.
*/
double Position::distance(const Position& position) const
{
	double fDistX = getX() - position.getX();
	double fDistY = getY() - position.getY();
	double fDistZ = getZ() - position.getZ();

	return sqrt(fDistX * fDistX + fDistY * fDistY + fDistZ * fDistZ);
}

/*!
Calculate the squared distance between two points.  Does not call
distance() for presumed efficiency reasons.

\param	position	The other position.
*/
double Position::distanceSquared(const Position& position) const
{
	double fDistX = getX() - position.getX();
	double fDistY = getY() - position.getY();
	double fDistZ = getZ() - position.getZ();

	return (fDistX * fDistX + fDistY * fDistY + fDistZ * fDistZ);
}

/*!
Create a position that is the linear interpolation between two positions.

\param prevPosition	The previous position
\param fRatio		The ratio between positions. For example, 0.25 is a position a quarter of the distance from prevPosition to this.

\return	The interpolated position.
*/
Position Position::interpolate(const Position& prevPosition, double fRatio) const
{
	// standard linear interpolation:
	//   v_result = v_start + t * ( v_final - v_start )
	return Position(
		prevPosition.getX() + fRatio * (getX() - prevPosition.getX()),
		prevPosition.getY() + fRatio * (getY() - prevPosition.getY()),
		prevPosition.getZ() + fRatio * (getZ() - prevPosition.getZ()) );
}

/*!
Convert position to a string.

\return	The string.
*/
//================================================================== 
// Author: Thomas K. Chen
// Modifications Made:
// - Changed the Qstring from Qt framework to C++ std::string class.
//================================================================== 
Position::operator std::string() const
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
Prints the text for the position to the stream.

\param output	The output stream.
\param input	The position.

\return the output stream reference
*/
//================================================================== 
// Author: Thomas K. Chen
// Modifications Made:
// - Replace QTextStream with std::strstream.
//==================================================================
std::strstream& operator<< (std::strstream& output, const Position& input)
{
	output <<	input.getX() << " " <<
				input.getY() << " " <<
				input.getZ();
	
	return output;
}

/*!
Read the text for the position from the stream.

\param input	The input stream.
\param output	The position.

\return	The input stream reference.
*/
//================================================================== 
// Author: Thomas K. Chen
// Modifications Made:
// - Replace QTextStream with std::strstream.
//==================================================================
std::strstream& operator>> (std::strstream& input, Position& output)
{
	double fX;
	double fY;
	double fZ;

	input >> fX >> fY >> fZ;
	output = Position(fX, fY, fZ);

	return input;
}
