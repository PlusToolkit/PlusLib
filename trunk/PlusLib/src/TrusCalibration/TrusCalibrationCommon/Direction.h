#ifndef DIRECTION_H
#define DIRECTION_H
// ===========================================================================
// Program Modification Record
// ===========================================================================
// Component:	Utility
// ===========================================================================
// Class:		Driection
// ===========================================================================
// File Name:	Driection.h
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

// C++ standard includes
#include <assert.h>

//! Utility class for managing directions.
/*!
Directions are unit-less and auto-normalized.

\remarks
This class could be templated on the float type if the need ever arose for
example to represent Directions as doubles. It would go something like this:
\code
template < class T >
class DirectionImpl
{
	// body changed to template version
};

typedef DirectionImpl<float> Direction;
typedef DirectionImpl<double> Direction_d;
*/
class Direction
{
    public:
	    //! Constructor.
	    Direction();
	    
	    //! Constructor.
	    Direction(double fU, double fV, double fW);
	    
	    //! Constructor.
	    explicit Direction(const double* pfDirection);

        //================================================================== 
        // Author: Thomas K. Chen
        // Modifications Made:
        // - Changed the Qstring from Qt framework to C++ std::string class.
        //==================================================================	    
        //! Constructor. Create a Direction from a string.
	    Direction(const std::string& str);

	    //! Test for equality.
	    bool operator==(const Direction& direction) const;

	    //! Test for inequality.
	    bool operator!=(const Direction& ref) const { return !operator==(ref);}
	    
	    //! Return an element.
	    double operator[](const int nIndex) const
	    {
		    assert(0 <= nIndex && 3 >= nIndex);
		    return m_pfDirection[nIndex];
	    }
	    
	    /*!
	    Utility method for passing Directions <strong>to</strong> VTK methods.

	    You should not use this mechanism to read from VTK because the object
	    will no longer be able to guarantee the normalised invariant.

	    Couldn't write this as operator const double*() because VTK does no use
	    const in all the cases that it should. We are trading safety for
	    usability here.
	    */
	    operator double*() {return m_pfDirection;}

	    //! Average two direction vectors
	    Direction average( const Direction& ) const;

        //================================================================== 
        // Author: Thomas K. Chen
        // Modifications Made:
        // - Changed the Qstring from Qt framework to C++ std::string class.
        //==================================================================	    
        //! Convert Direction to a string.
	    operator std::string() const;

	    //! Get the u component.
	    double getU() const {return m_pfDirection[0];}
	    
	    //! Get the v component.
	    double getV() const {return m_pfDirection[1];}
	    
	    //! Get the w component.
	    double getW() const {return m_pfDirection[2];}
	    
	    //! Check the orthogonality of three directions.
	    static bool checkOrthogonality(	const Direction& dirA,
									    const Direction& dirB,
									    const Direction& dirC,
									    double fTolerance	);

	    //! Check if the directions form a right handed coordinate system.
	    static bool checkRHSystem(	const Direction& dirX,
								    const Direction& dirY,
								    const Direction& dirZ,
								    double fTolerance	);

    protected:

	    //! Normalizes the Direction.
	    void normalize();
	    
    private:
	    //! The data
	    double m_pfDirection[4];
};

//================================================================== 
// Author: Thomas K. Chen
// Modifications Made:
// - Replace QTextStream with std::strstream.
//==================================================================
//! Allows Directions to be written to streams.
std::strstream& operator<< (std::strstream& input, const Direction& output);

//================================================================== 
// Author: Thomas K. Chen
// Modifications Made:
// - Replace QTextStream with std::strstream.
//==================================================================
//! Allows Directions to be read from streams.
std::strstream& operator>> (std::strstream& input, Direction& output);

#endif
