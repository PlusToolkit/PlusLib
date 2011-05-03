// ===========================================================================
// Program Modification Record
// ===========================================================================
// Component:	Utility
// ===========================================================================
// Class:		Orientation
// ===========================================================================
// File Name:	Orientation.cpp
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

#ifndef ORIENTATION_H
#include "Orientation.h"
#endif

//================================================================== 
// Author: Thomas K. Chen
// Modifications Made:
// - Added includes of C++ Standard string class: std::string
//==================================================================
// C++ standard includes 
#include <string>

//================================================================== 
// Author: Thomas K. Chen
// Modifications Made:
// - StringUtil class was modified to elimate the trace of Qt frame-
//   work, in particular, the QString and QObject classes. 
// - The include file was renamed to be the same as the class name.
//==================================================================
// local includes
#include "StringUtils.h"

// local includes
#include "Direction.h"


// standard includes

/*!
Constructor
*/
Orientation::Orientation()
{
	m_pfOrientation[0] = 0;
	m_pfOrientation[1] = 0;
	m_pfOrientation[2] = 0;
	m_pfOrientation[3] = 1;
}
	
/*!
Constructor

\param fAngle	The angle in degrees.
\param dir		The direction about which to rotate.
*/
Orientation::Orientation(double fAngle, const Direction& dir)
{
	m_pfOrientation[0] = fAngle;
	m_pfOrientation[1] = dir.getU();
	m_pfOrientation[2] = dir.getV();
	m_pfOrientation[3] = dir.getW();
}
	
/*!
Constructor

\param fAngle	The angle in degrees.
\param fQX		The x coordinate.
\param fQY		The y coordinate.
\param fQZ		The z coordinate.
*/
Orientation::Orientation(double fAngle, double fQX, double fQY, double fQZ)
{
	m_pfOrientation[0] = fAngle;
	m_pfOrientation[1] = fQX;
	m_pfOrientation[2] = fQY;
	m_pfOrientation[3] = fQZ;

	normalize();
}
	
/*!
Constructor

\param pfOrientation	The orientation as an array of floats.
*/
Orientation::Orientation(const double* pfOrientation)
{
	m_pfOrientation[0] = pfOrientation[0];
	m_pfOrientation[1] = pfOrientation[1];
	m_pfOrientation[2] = pfOrientation[2];
	m_pfOrientation[3] = pfOrientation[3];
}

/*!
Constructor. Create an orientation from a string.

\param str	The string.
*/
//================================================================== 
// Author: Thomas K. Chen
// Modifications Made:
// - Changed the Qstring from Qt framework to C++ std::string class.
//==================================================================
Orientation::Orientation(const std::string& str)
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
Copy constructor.

\param orientation	The orientation to copy
*/
Orientation::Orientation(const Orientation& orientation)
{
	m_pfOrientation[0] = orientation.m_pfOrientation[0];
	m_pfOrientation[1] = orientation.m_pfOrientation[1];
	m_pfOrientation[2] = orientation.m_pfOrientation[2];
	m_pfOrientation[3] = orientation.m_pfOrientation[3];
}
	
/*!
Copy assignment.

\param orientation	The orientation to copy
*/
Orientation& Orientation::operator=(const Orientation& orientation)
{
	// check for self assignment
	if (this != &orientation)
	{
		m_pfOrientation[0] = orientation.m_pfOrientation[0];
		m_pfOrientation[1] = orientation.m_pfOrientation[1];
		m_pfOrientation[2] = orientation.m_pfOrientation[2];
		m_pfOrientation[3] = orientation.m_pfOrientation[3];
	}
	
	return *this;
}
	
/*!
Test for equality

\return	true if the orientations are equal, otherwise false.
*/
bool Orientation::operator==(const Orientation& orientation) const
{
	return	(m_pfOrientation[0] == orientation.m_pfOrientation[0]) &&
			(m_pfOrientation[1] == orientation.m_pfOrientation[1]) &&
			(m_pfOrientation[2] == orientation.m_pfOrientation[2]) &&
			(m_pfOrientation[3] == orientation.m_pfOrientation[3]);
}

/*!
Normalizes the direction component of the orientation
*/
void Orientation::normalize()
{
	Direction dir(getQX(), getQY(), getQZ());
	setQX(dir.getU());
	setQY(dir.getV());
	setQZ(dir.getW());
}
	
/*!
Create an orientation that is the linear interpolation between two orientations.

\param prevOrientation	The previous orientation
\param fRatio			The ratio between orientations.

\return	The interpolated orientation.
*/
Orientation Orientation::interpolate(const Orientation& prevOrient, double fRatio) const
{
	return Orientation(
		prevOrient.getAngle() + fRatio * (getAngle() - prevOrient.getAngle()),	
		prevOrient.getQX() + fRatio * (getQX() - prevOrient.getQX()),
		prevOrient.getQY() + fRatio * (getQY() - prevOrient.getQY()),
		prevOrient.getQZ() + fRatio * (getQZ() - prevOrient.getQZ())	);
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
Orientation::operator std::string() const
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

	/*
    return	"angle = " + toString(m_pfOrientation[0]) +
			", Qx = " + toString(m_pfOrientation[1]) +
			", Qy = " + toString(m_pfOrientation[2]) +
			", Qz = " + toString(m_pfOrientation[3]);
    */
}

/*!
Prints the text for the orientation to the stream.

\param output	The output stream.
\param input	The orientation.

\return the output stream reference
*/
//================================================================== 
// Author: Thomas K. Chen
// Modifications Made:
// - Replace QTextStream with std::strstream.
//==================================================================
std::strstream& operator<< (std::strstream& output, const Orientation& input)
{
	output <<	input.getAngle() << " " <<
				input.getQX() << " " <<
				input.getQY() << " " <<
				input.getQZ();
	
	return output;
}

/*!
Read the text for the orientation from the stream.

\param input	The input stream.
\param output	The orientation.

\return	The input stream reference.
*/
//================================================================== 
// Author: Thomas K. Chen
// Modifications Made:
// - Replace QTextStream with std::strstream.
//==================================================================
std::strstream& operator>> (std::strstream& input, Orientation& output)
{
	double fAngle;
	double fQX;
	double fQY;
	double fQZ;

	input >> fAngle >> fQX >> fQY >> fQZ;
	output = Orientation(fAngle, fQX, fQY, fQZ);

	return input;
}
