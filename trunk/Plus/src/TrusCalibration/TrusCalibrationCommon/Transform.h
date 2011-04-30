#ifndef TRANSFORM_H
#define TRANSFORM_H
// ===========================================================================
// Program Modification Record
// ===========================================================================
// Component:	Utility
// ===========================================================================
// Class:		Transform
// ===========================================================================
// File Name:	Transform.h
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
// Thomas Kuiran Chen	Mon Jun 29 08:32 EDT 2003	1.0		Generalization
// Thomas Kuiran Chen	Sun Feb 26 11:18 EDT 2006	2.0		Upgrade to VTK 5.0
//
// ===========================================================================


// vtk includes
#include "vtkMath.h"
#include "vtkTransform.h"
#include "vtkPoints.h"

//================================================================== 
// Author: Thomas K. Chen
// Modifications Made:
// - Added includes of C++ Standard string class: std::string
//==================================================================
// C++ standard includes 
#include <string>
#include <strstream>

// C++ standard includes
#include <list>

//================================================================== 
// Author: Thomas K. Chen
// Modifications Made:
// - Added includes of CommonFramework
//==================================================================
// CommonFramework includes
#include "Orientation.h"
#include "Position.h"
#include "Direction.h"

// vtk forward declarations
class vtkMatrix4x4;
class vtkTransform;

//! Utility class for managing transformations.
/*!
This class manages transformations, positions and orientations in 3D space.
It is a thin wrapper around the vtkTransform class.
*/
class Transform
{
    public:
	    //! List of transforms
	    typedef std::list<Transform> TransformList;

	    //! Default constructor
	    Transform();
	    
	    //! Constructor
	    Transform(const Position& position, double fScale = 1.0f);
	    
	    //! Constructor
	    Transform(const Position& position, const Orientation& orientation);

	    //! Constructor. Create a transform from a matrix.
	    Transform(vtkMatrix4x4* pMatrix);
	    
        //================================================================== 
        // Author: Thomas K. Chen
        // Modifications Made:
        // - Changed the Qstring from Qt framework to C++ std::string class.
        //==================================================================	    
	    //! Constructor. Create a transform from a string.
	    Transform(const std::string& str);

	    //! Copy constructor
	    Transform(const Transform& transform);
	    
	    //! Copy assignment
	    Transform& operator=(const Transform& transform);
	    
	    //! Destructor
	    ~Transform();
	    
	    //! Change transform to identity
	    void identity();
	    
	    //! Rotate about XYZ axes in that order
	    void rotateXYZ(double fXAngle, double fYAngle, double fZAngle);
	    
	    //! Set the pose (position and orientation) for this transform
	    void setPose(const Position& position, const Orientation& orientation);
	    
	    //! Add to the current transform position
	    void translate(const Position& position);
	    
	    //! Add to the current transform orientation
	    void rotate(const Orientation& orientation);

	    //! Get the current scale
	    void getScale(double* fXScale, double* fYScale, double* fZScale) const;

	    //! Scale the current transform
	    void scale(double fScale);

	    //! Scale the transform by the given factors.
	    void scale(double fXScale, double fYScale, double fZScale);

	    //! Invert the transform
	    void invert() {m_pVTKTransform->Inverse();}
	    
	    //! Get the position represented by this transform
	    Position getPosition() const;
	    
	    //! Get the orientation represented by this transform as a quaternion
	    Orientation getOrientation() const;

	    //! Get the component angles represented by this transform
	    void getOrientation(double* fXAngle, double* fYAngle, double* fZAngle);

	    //! Apply a given relative transform to this one.
	    void applyRelative(Transform transform);
	    
	    //! Make this transform relative to a given transform.
	    void makeRelative(Transform transform);

	    //! Apply this transform to a point.
	    Position transformPosition(const Position& position) const;
	    
	    //! Apply this transform to a direction.
	    Direction transformDirection(const Direction& direction) const;

	    //! Get the underlying vtk transform (ownership retained)
	    vtkTransform* getVTKTransform() {return m_pVTKTransform;}
	    
	    //! Get a pointer to the underlying matrix.
	    vtkMatrix4x4* getMatrix() const;
	    
	    //! Create a transform that is the interpolation between two transforms.
	    Transform* interpolate(const Transform& prevTransform, double fRatio) const;

        //================================================================== 
        // Author: Thomas K. Chen
        // Modifications Made:
        // - Changed the Qstring from Qt framework to C++ std::string class.
        //==================================================================	    
	    //! Convert transform to a string.
	    operator std::string() const;

	    //! Calculate the transform that takes one set of landmarks to another
	    static Transform* buildTransform(	vtkPoints* pSourceLandmarks,
										    vtkPoints* pTargetLandmarks,
										    bool bRigidBody = false	);

	    //! decompose a transform into rotatation about a point and pure translation
	    void decomposeTransform(	Position& point,  Position& translation, 
								    double* fRx, double* fRy, double* fRz	);
						    
	    //! compose a transform from a rotation about a point and a pure translation
	    static Transform composeTransform(	Position& point, const Position& translation,
										    double fRx, double fRy, double fRz	);
    protected:

    private:
	    //! The vtk transform
	    vtkTransform* m_pVTKTransform;
};

//================================================================== 
// Author: Thomas K. Chen
// Modifications Made:
// - Replace QTextStream with std::strstream.
//==================================================================
/*!
Allows transforms to be piped to streams.
\relates Transform
*/
std::strstream& operator<< (std::strstream& output, const Transform& input);

//================================================================== 
// Author: Thomas K. Chen
// Modifications Made:
// - Replace QTextStream with std::strstream.
//==================================================================
/*!
Allows transforms to be read from streams.
\relates Transform
*/
std::strstream& operator>> (std::strstream& input, Transform& output);

/*!
Calculate a vector average for the positions in the Transform container.

\param	begin	Iterator to beginning of transforms
\param	end		Iterator to end of transforms

\return	The average position
*/
template <class T>
Position calculatePositionMean(const T begin, const T end)
{
	// accumulate as doubles to maintain precision
	double fX = 0.0f;
	double fY = 0.0f;
	double fZ = 0.0f;

	int nCount = 0;
	T it = begin;
	for (; it != end; ++it)
	{
		Position pos = it->getPosition();

		fX += pos.getX();
		fY += pos.getY();
		fZ += pos.getZ();

		++nCount;
	}
	
	assert(0 < nCount);
	return Position(fX / nCount, fY / nCount, fZ / nCount);
}

/*!
Calculate an average orientation from a list of transforms.

\param	begin	Iterator to beginning of transforms
\param	end		Iterator to end of transforms

\return	The mean orientation.
*/
template <class T>
Orientation calculateOrientationMean(const T begin, const T end)
{
	// accumulators for the "average" calculation
	double posSumX[3] = {0.0, 0.0, 0.0};
	double posSumY[3] = {0.0, 0.0, 0.0};

	// for each measured target position/orientation
	T it = begin;
	for (; it != end; ++it)
	{
		// A coordinate system to rotate.
		// Note, Z is implicitly defined by X cross Y.
		Position posX(1.0, 0.0, 0.0);
		Position posY(0.0, 1.0, 0.0);

		// get the rotation only transform, don't want translation
		Transform transform;
		transform.rotate(it->getOrientation());

		// rotate the coordinate system
		posX = transform.transformPosition(posX);
		posY = transform.transformPosition(posY);

		// Sum the result as a first step toward an "average".
		// Note, the resulting sums will continue to be orthogonal.
		posSumX[0] += posX.getX();
		posSumX[1] += posX.getY();
		posSumX[2] += posX.getZ();

		posSumY[0] += posY.getX();
		posSumY[1] += posY.getY();
		posSumY[2] += posY.getZ();
	}

	// make Z explicit
	//Position posSumZ;
	double posSumZ[3];
	vtkMath::Cross(posSumX, posSumY, posSumZ);

	// normalize by converting to Directions
	Direction dirX(static_cast<double>(posSumX[0]), static_cast<double>(posSumX[1]), static_cast<double>(posSumX[2]));
	Direction dirY(static_cast<double>(posSumY[0]), static_cast<double>(posSumY[1]), static_cast<double>(posSumY[2]));
	Direction dirZ(static_cast<double>(posSumZ[0]), static_cast<double>(posSumZ[1]), static_cast<double>(posSumZ[2]));

	// Build a transform to the rotated coordinate system.
	// Use points to represent the basis vectors and then do a rigid body
	// transform.
	vtkPoints* pPoints1 = vtkPoints::New();
	pPoints1->InsertNextPoint(Position(1.0, 0.0, 0.0));
	pPoints1->InsertNextPoint(Position(0.0, 1.0, 0.0));
	pPoints1->InsertNextPoint(Position(0.0, 0.0, 1.0));
	vtkPoints* pPoints2 = vtkPoints::New();
	pPoints2->InsertNextPoint(dirX);
	pPoints2->InsertNextPoint(dirY);
	pPoints2->InsertNextPoint(dirZ);
	std::auto_ptr<Transform> pTrans(Transform::buildTransform(pPoints1, pPoints2, true));

	// pull out the pseudo-quaternion
	return pTrans->getOrientation();
}

/*!
Calculate a statistic representative of the variance of the orientations

This is the "target shooting" algorithm described in the "Tracker
Comparison Test" document. Basically, we pick a unit vector, rotate
it by the "mean" orientation, and call it, "d_bar". Then, for each
orientation in the sample, we rotate the vector again and call it, "d".
The dot product of d and d_bar is the amount of agreement between
them (0 is no agreement, 1 is total agreement). To get the disagreement
(error) we use the identity sin^2 = 1 - cos^2, recognising that the
dot product of unit vectors is a cosine. Finally, we sum the sin^2
terms, divide by N - 1, and take the square root in typical RMS
fashion.

\param vecTransforms List of transforms to compare to the specified mean orientation.
\param orientMean The orientation which is a "vector average" of the orientations contained in vecTransforms.
\return The standard deviation of the error angle.
*/
template <class T>
double calculateOrientationStdDev(const T begin, const T end, const Orientation& orientMean)
{
	/* The code below does a few tricks with the approach described
	above. First, the unit vector is chosen to be the quaternion vector,
	this removes the need for a few transformations. Second, the
	standard deviation calculation is factored to avoid a few
	subtractions.
	*/
	double fOrientationSum = 0.0;
	Direction d_bar( orientMean.getQX(), orientMean.getQY(), orientMean.getQZ() );

	int nCount = 0;
	T it = begin;
	for (; it != end; ++it)
	{
		Orientation orientation = it->getOrientation();
		Position pos;
		Transform transform( pos, orientation );
		Direction d = transform.transformDirection(d_bar);
		double fDot = vtkMath::Dot(d, d_bar);
		fOrientationSum += fDot * fDot;

		++nCount;
	}

	assert(1 < nCount);
	return sqrt((nCount - fOrientationSum) / (nCount - 1));
}

/*!
Calculate a standard deviation of a list of vectors from a given average
vector.

\param vecTransforms Container of transforms with position information.
\param posMean Previously calculated average vector to find deviation from.
\return The standard deviation.
*/
template <class T>
double calculatePositionStdDev(const T begin, const T end, const Position& posMean)
{
	double fSum = 0.0f;
	int nCount = 0;
	T it = begin;
	for (; it != end; ++it)
	{
		double fDistance = posMean.distance(it->getPosition());
		fSum += fDistance * fDistance;

		++nCount;
	}

	assert(1 < nCount);
	return sqrt(fSum / (nCount - 1));
}

//! Utility Operation: Convert the Transform object to 4x4 homogeneous matrix 
// The matrix returned will be in standard vnl_matrix format.  Eventually, 
// this function should be moved to a Transform/matrix utility class.
// Author: Thomas Kuiran Chen
// Date of Creation: Sat Jan 10 14:02 EDT 2004
/*
vnl_matrix<double> convertTransform2HomoMatrix(const Transform &TransformObject)
{
	const std::string strScope("Transform");
	const std::string strConvertTransform2HomoMatrixFailed = "Failed to convert the Transform to Homogeneous Matrix!";

	try
	{
		const double PI(3.1415927);

		// Extract elements from the Transform object 
		double Theta = TransformObject.getOrientation().getAngle();	// Angle of rotation in degrees
		double ThetaInRadians = Theta * ( PI/180 );

		double Qx = TransformObject.getOrientation().getQX();			// rotation axis x
		double Qy = TransformObject.getOrientation().getQY();			// rotation axis y 
        double Qz = TransformObject.getOrientation().getQZ();			// rotation axis z
		
		double Tx = TransformObject.getPosition().getX();				// translation x
		double Ty = TransformObject.getPosition().getY();				// translation y
		double Tz = TransformObject.getPosition().getZ();				// translation z

		// Form Quaterions
		double Qa = cos(ThetaInRadians * 0.5);
		double Qb = Qx*sin(ThetaInRadians * 0.5);
		double Qc = Qy*sin(ThetaInRadians * 0.5);
		double Qd = Qz*sin(ThetaInRadians * 0.5);

		// Finally we compose the 4x4 homogeneous transform matrix 
		// using the information collected
		vnl_matrix<double> HomoTransformMatrix(4,4);
		vnl_vector<double> Row( 4, 0 );

		// 1st row
		Row.put( 0, pow(Qa, 2) + pow(Qb, 2) - pow(Qc, 2) - pow(Qd, 2) );
		Row.put( 1, 2 * ( Qb * Qc - Qa * Qd) );
		Row.put( 2, 2 * ( Qb * Qd + Qa * Qc) );
		Row.put( 3, Tx );
		HomoTransformMatrix.set_row(0, Row);

		// 2nd row
		Row.put( 0, 2 * ( Qb * Qc + Qa * Qd ) );
		Row.put( 1, pow(Qa, 2) + pow(Qc, 2) - pow(Qb, 2) - pow(Qd, 2) );
		Row.put( 2, 2 * ( Qc * Qd - Qa * Qb ) );
		Row.put( 3, Ty );
		HomoTransformMatrix.set_row(1, Row);

		// 3rd row
		Row.put( 0, 2 * ( Qb * Qd - Qa * Qc ) );
		Row.put( 1, 2 * ( Qc * Qd + Qa * Qb ) );
		Row.put( 2, pow(Qa, 2) + pow(Qd, 2) - pow(Qb, 2) - pow(Qc, 2) );
		Row.put( 3, Tz );
		HomoTransformMatrix.set_row(2, Row);

		// 4th row as standard (0,0,0,1)
		Row.fill(0);
		Row.put(3, 1);
		HomoTransformMatrix.set_row(3, Row);

		return HomoTransformMatrix;

	}
	catch(...)
	{
		throw Exception(__FILE__, __LINE__, strScope, strConvertTransform2HomoMatrixFailed);
	}
}
*/

#endif
