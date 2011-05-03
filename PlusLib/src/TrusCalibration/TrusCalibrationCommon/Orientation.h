#ifndef ORIENTATION_H
#define ORIENTATION_H
// ===========================================================================
// Program Modification Record
// ===========================================================================
// Component:	Utility
// ===========================================================================
// Class:		Orientation
// ===========================================================================
// File Name:	Orientation.h
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


//================================================================== 
// Author: Thomas K. Chen
// Modifications Made:
// - Added includes of C++ Standard string class: std::string
//==================================================================
// C++ standard includes 
#include <string>
#include <strstream>

// local forward declarations
class Direction;

//! Utility class for managing orientations
/*!
The angle is specified in degrees, all other components are unitless. Rotation
is right-handed about the direction vector, that is: counter-clockwise. Think
of this as a pseudo-quaternion.
*/
class Orientation
{
    public:
	    //! Constructor
	    Orientation();
	    
	    //! Constructor
	    Orientation(double fAngle, const Direction& dir);
	    
	    //! Constructor
	    Orientation(double fAngle, double fQX, double fQY, double fQZ);
	    
	    //! Constructor
	    Orientation(const double* pfOrientation);

        //================================================================== 
        // Author: Thomas K. Chen
        // Modifications Made:
        // - Changed the Qstring from Qt framework to C++ std::string class.
        //==================================================================	
	    //! Constructor. Create an orientation from a string.
	    Orientation(const std::string& str);

	    //! Copy constructor
	    Orientation(const Orientation& orientation);
	    
	    // Copy assignment
	    Orientation& operator=(const Orientation& orientation);
	    
	    //! Destructor
	    ~Orientation() {;}

	    //! Test for equality
	    bool operator==(const Orientation& orientation) const;
	    
	    //! Utility method for passing quaternions to vtk methods
	    operator double*() {return m_pfOrientation;}

	    //! Normalizes the direction component of the orientation
	    void normalize();

	    //! Get the angle
	    double getAngle() const {return m_pfOrientation[0];}
	    
	    //! Set the angle
	    void setAngle(double fAngle) {m_pfOrientation[0] = fAngle;}

	    //! Get the x component
	    double getQX() const {return m_pfOrientation[1];}
	    
	    //! Set the x component
	    void setQX(double fQX) {m_pfOrientation[1] = fQX;}

	    //! Get the y component
	    double getQY() const {return m_pfOrientation[2];}
	    
	    //! Set the y component
	    void setQY(double fQY) {m_pfOrientation[2] = fQY;}

	    //! Get the z component
	    double getQZ() const {return m_pfOrientation[3];}
	    
	    //! Set the z component
	    void setQZ(double fQZ) {m_pfOrientation[3] = fQZ;}
		    
	    //! Create an orientation that is the interpolation between two orientations.
	    Orientation interpolate(const Orientation& prevPosition, double fRatio) const;

        //================================================================== 
        // Author: Thomas K. Chen
        // Modifications Made:
        // - Changed the Qstring from Qt framework to C++ std::string class.
        //==================================================================	
	    //! Convert position to a string
	    operator std::string() const;
	    
    protected:

    private:
	    //! The data
	    double m_pfOrientation[4];
};

//================================================================== 
// Author: Thomas K. Chen
// Modifications Made:
// - Replace QTextStream with std::strstream.
//==================================================================
//! Allows orientations to be written to streams.
std::strstream& operator<< (std::strstream& output, const Orientation& input);

//================================================================== 
// Author: Thomas K. Chen
// Modifications Made:
// - Replace QTextStream with std::strstream.
//==================================================================
//! Allows orientations to be read from streams.
std::strstream& operator>> (std::strstream& input, Orientation& output);

#endif
