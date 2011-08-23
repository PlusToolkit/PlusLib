#ifndef POSITION_H
#define POSITION_H
// ===========================================================================
// Program Modification Record
// ===========================================================================
// Component:	Utility
// ===========================================================================
// Class:		Position
// ===========================================================================
// File Name:	Position.h
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


//================================================================== 
// Author: Thomas K. Chen
// Modifications Made:
// - Added includes of C++ Standard string class: std::string
//==================================================================
// C++ standard includes 
#include <string>
#include <strstream>

//! Utility class for managing positions
/*!
Positions are specified in metres.

If you want to represent unit vectors, use the Direction class instead.
*/
class Position
{
    public:
	    //! Constructor
	    Position();
	    
	    //! Constructor
	    Position(double fX, double fY, double fZ = 0.0f, double fW = 1.0f);
	    
	    //! Constructor
	    explicit Position(const double* pfPosition, double fW = 1.0f);

        //================================================================== 
        // Author: Thomas K. Chen
        // Modifications Made:
        // - Changed the Qstring from Qt framework to C++ std::string class.
        //==================================================================	    	    
        //! Constructor. Create a position from a string.
	    Position(const std::string& str);

	    //! Copy constructor
	    Position(const Position& position);
	    
	    // Copy assignment
	    Position& operator=(const Position& position);
	    
	    //! Destructor
	    ~Position() {;}

	    //! Test for equality
	    bool operator==(const Position& position) const;

	    //! Test for inequality
	    bool operator!=(const Position& position) const {return !operator==(position);}

	    //! Test for less-than (compares magnitudes, and is used for sorting)
	    bool operator<(const Position& position) const {return magnitude() < position.magnitude();}
	    
	    const double& operator[](int nIndex) const { return m_pfPosition[nIndex]; }
	    
	    //! Utility method for passing positions to vtk methods
	    operator double*() {return m_pfPosition;}

	    //! Vector subtraction.
	    Position operator-( const Position& ) const;

	    //! Vector addition.
	    Position operator+( const Position& ) const;

	    //! Get the x component
	    double getX() const {return m_pfPosition[0];}
	    
	    //! Set the x component
	    void setX(double fX) {m_pfPosition[0] = fX;}

	    //! Get the y component
	    double getY() const {return m_pfPosition[1];}
	    
	    //! Set the y component
	    void setY(double fY) {m_pfPosition[1] = fY;}

	    //! Get the z component
	    double getZ() const {return m_pfPosition[2];}
	    
	    //! Set the z component
	    void setZ(double fZ) {m_pfPosition[2] = fZ;}
	    
	    //! Calculate the distance between two points.
	    double distance(const Position& position) const;

	    //! Calculate the squared distance between two points.
	    double distanceSquared(const Position& position) const;

	    //! Create a position that is the interpolation between two positions.
	    Position interpolate(const Position& prevPosition, double fRatio) const;

	    //! Get the magnitude of the position if representing a vector
	    double magnitude() const {return distance(Position());}

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
	    double m_pfPosition[4];
};

//================================================================== 
// Author: Thomas K. Chen
// Modifications Made:
// - Replace QTextStream with std::strstream.
//==================================================================
//! Allows positions to be written to streams.
std::strstream& operator<< (std::strstream& output, const Position& input);

//================================================================== 
// Author: Thomas K. Chen
// Modifications Made:
// - Replace QTextStream with std::strstream.
//==================================================================
//! Allows positions to be read from streams.
std::strstream& operator>> (std::strstream& input, Position& output);

#endif
