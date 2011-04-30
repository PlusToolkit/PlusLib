// ===========================================================================
// Program Modification Record
// ===========================================================================
// Component:	Utility
// ===========================================================================
// Class:		Transform
// ===========================================================================
// File Name:	Transform.cpp
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


#ifndef TRANSFORM_H
#include "Transform.h"
#endif

//================================================================== 
// Author: Thomas K. Chen
// Modifications Made:
// - Added includes of CommonFramework
//==================================================================
// CommonFramework includes
//#include "Orientation.h"
//#include "Position.h"
//#include "Direction.h"
#include "TraceUtils.h"

// vtk includes
#include "vtkActor.h"
#include "vtkVersion.h"

//================================================================== 
// Author: Thomas K. Chen
// Modifications Made:
// - Added includes of C++ Standard string class: std::string
//==================================================================
// C++ standard includes 
#include <string>
#include <strstream>

// C++ standard includes
#include <memory>

/*!
Default constructor. Creates a transform with no rotation, located at the
origin.
*/
Transform::Transform() :
	m_pVTKTransform( vtkTransform::New() )	// create the identity matrix
{
}

/*!
Constructor. Create a transform with the given scale, position and orientation.
Used for converting between units and origins.

\param position		The position.
\param fScale		The scale.
*/
Transform::Transform(const Position& position, double fScale) :
	m_pVTKTransform( vtkTransform::New() )	// create the identity matrix
{
	// apply the position
   	m_pVTKTransform->Translate( position.getX(),
   		position.getY(), position.getZ() );

	// apply the scaling
	if (1.0f != fScale)
	{
		m_pVTKTransform->Scale(fScale, fScale, fScale);
	}
}
	
/*!
Constructor. Create a transform with the given position and orientation.

\param position		The position
\param orientation	The orientation.
*/
Transform::Transform(const Position& position, const Orientation& orientation) :
	m_pVTKTransform( vtkTransform::New() )	// create the identity matrix
{
	setPose(position, orientation);
}

/*!
Constructor. Create a transform with the given matrix.

\param pMatrix	The matrix.
*/
Transform::Transform(vtkMatrix4x4* pMatrix) :
	m_pVTKTransform( vtkTransform::New() )	// create the identity matrix
{
	m_pVTKTransform->SetMatrix(pMatrix);
}

/*!
Constructor. Create a transform from a string.

\param str	The string.
*/
//================================================================== 
// Author: Thomas K. Chen
// Modifications Made:
// - Changed the Qstring from Qt framework to C++ std::string class.
//==================================================================
Transform::Transform(const std::string& str)
{
	// create the identity matrix
	m_pVTKTransform = vtkTransform::New();
	
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

\param transform	The transform to copy
*/
Transform::Transform(const Transform& transform)
: m_pVTKTransform( vtkTransform::New() )	// create the identity matrix
{
	// copy the matrix
	m_pVTKTransform->DeepCopy(transform.m_pVTKTransform);
}
	
/*!
Copy assignment.

\param transform	The transform to copy
*/
Transform& Transform::operator=(const Transform& transform)
{
	// check for self assignment
	if (this != &transform)
	{
		// copy the matrix
		m_pVTKTransform->DeepCopy(transform.m_pVTKTransform);
	}
	
	return *this;
}
	
/*!
Destructor.
*/
Transform::~Transform()
{
	m_pVTKTransform->Delete();
}
		
/*!
Reset the transform to the identity matrix
*/
void Transform::identity()
{
	m_pVTKTransform->Identity();
}
		
/*!
Apply rotations about the z, x and y axes in that order.  All angles 
are right-handed.  The ordering is determined so that getOrientation
will return the same angles as input.

\param fXAngle angle of rotation in degrees about x-axis
\param fYAngle angle of rotation in degrees about y-axis
\param fZAngle angle of rotation in degrees about z-axis
*/
void Transform::rotateXYZ(double fXAngle, double fYAngle, double fZAngle)
{
	m_pVTKTransform->RotateZ(fZAngle);
	m_pVTKTransform->RotateX(fXAngle);
	m_pVTKTransform->RotateY(fYAngle);
}
		
/*!
Set the pose (position and orientation) for this transform.

\param position		The position
\param orientation	The orientation.
*/
void Transform::setPose(const Position& position, const Orientation& orientation)
{
	// reset the transform
	m_pVTKTransform->Identity();
	
	// apply the position
   	m_pVTKTransform->Translate(	position.getX(),
   								position.getY(),
   								position.getZ()	);
   		
   	// apply the orientation
   	m_pVTKTransform->RotateWXYZ(	orientation.getAngle(),
   									orientation.getQX(),
   									orientation.getQY(),
   									orientation.getQZ()	);
}
	
/*!
Get the position represented by this transform. This is infact the position
you would get if you applied this transform to the origin.

\return The position
*/
Position Transform::getPosition() const
{
	return Position(m_pVTKTransform->GetPosition(), 1);
}

/*!
Get the orientation represented by this transform. This is the rotation that
will be applied by the transform.

\return The orientation
*/
Orientation Transform::getOrientation() const
{
	return Orientation(m_pVTKTransform->GetOrientationWXYZ());
}
	
/*!
Get the component angles represented by this transform

\param fXAngle	The angle about the x-axis (out)
\param fYAngle	The angle about the y-axis (out)
\param fZAngle	The angle about the z-axis (out)
*/
void Transform::getOrientation(double* fXAngle, double* fYAngle, double* fZAngle)
{
	// Thomas Kuiran Chen Modification
	// Change the interface to VTK 5.0
	//m_pVTKTransform->GetOrientation(fXAngle, fYAngle, fZAngle);
	double orientationArray[3] = {0};

	m_pVTKTransform->GetOrientation( orientationArray );
	*fXAngle = orientationArray[0];
	*fYAngle = orientationArray[1];
	*fZAngle = orientationArray[2];
}

/*!
Apply a given relative transform to this one. This method is used for applying
calibration to tool transformations. The relative transform should be expressed
in the tool's local coordinate system.

\param transform	The relative transform to apply.
*/
void Transform::applyRelative(Transform transform)
{
	// apply to the LHS
	m_pVTKTransform->PreMultiply();
 	m_pVTKTransform->Concatenate(transform.m_pVTKTransform->GetMatrix());
}
	
/*!
Make this transform relative to a given transform. This method is used for
calculating a tool transformation relative to the DRB or calibration block.

Both transforms should be relative to the same coordinate system.

\param transform	The origin transform.
*/
void Transform::makeRelative(Transform transform)
{
	// invert the transform
   	transform.m_pVTKTransform->Inverse();
   			
   	// apply to the RHS
   	m_pVTKTransform->PostMultiply();
   	m_pVTKTransform->Concatenate(transform.m_pVTKTransform->GetMatrix());
   	m_pVTKTransform->PreMultiply();
}

/*!
Apply this transform to a position. This method is used to calculate the
transformed position for a point specified in the local coordinate
system of the trackable object. An example is the position of the source for
the imager subsystem.

\param position	The position.
\return The new position.
*/
Position Transform::transformPosition(const Position& position) const
{
	Position newPosition;
	
	m_pVTKTransform->MultiplyPoint(const_cast<Position&>(position), newPosition);
	return newPosition;
}

/*!
Apply this transform to a direction. This method is used to calculate the
transformed direction for a vector specified in the local coordinate
system of the trackable object. An example is transforming the view-up
vector.

\param direction	The direction.
*/
Direction Transform::transformDirection(const Direction& direction) const
{
	Direction newDirection;
	
	m_pVTKTransform->MultiplyPoint(const_cast<Direction&>(direction), newDirection);
	return newDirection;
}

/*!
Get a pointer to the underlying matrix. This method may be used to get a
matrix suitable for vtkActor::SetUserMatrix()

\return A pointer to the matrix (ownership retained)
*/
vtkMatrix4x4* Transform::getMatrix() const
{
	return m_pVTKTransform->GetMatrix();
}

/*!
Treat the position as a displacement and add it to the current
transform.
\param position Translation vector.
*/
void Transform::translate(const Position& position)
{
	// apply the position
   	m_pVTKTransform->Translate(	position.getX(),
   								position.getY(),
								position.getZ()	);
}

/*!
Apply the orientation as a change to the current one.

\param orientation rotation specification.
*/
void Transform::rotate(const Orientation& orientation)
{
   	// apply the orientation
   	m_pVTKTransform->RotateWXYZ(	orientation.getAngle(),
   									orientation.getQX(),
									orientation.getQY(),
									orientation.getQZ()	);
}

/*!
Get the current scale

\param fXScale	x scaling (out)
\param fYScale	y scaling (out)
\param fZScale	z scaling (out)
*/
void Transform::getScale(double* fXScale, double* fYScale, double* fZScale) const
{
	// Thomas Kuiran Chen Modification
	// Change the interface to VTK 5.0
	//m_pVTKTransform->GetScale(fXScale, fYScale, fZScale);
	double scaleArray[3] = {0};

	m_pVTKTransform->GetScale( scaleArray );
	*fXScale = scaleArray[0];
	*fYScale = scaleArray[1];
	*fZScale = scaleArray[2];

}

/*!
Scale the current transform by the given factor.

\param fScale	The scaling factor
*/
void Transform::scale(double fScale)
{
	assert(0 < fScale);

	// scale evenly in all dimensions
	m_pVTKTransform->Scale(fScale, fScale, fScale);
}

/*!
Scale the transform by the given factors.

\param fXScale	The x scaling factor.
\param fYScale	The y scaling factor.
\param fZScale	The z scaling factor.
*/
void Transform::scale(double fXScale, double fYScale, double fZScale)
{
	assert(0 < fXScale);
	assert(0 < fYScale);
	assert(0 < fZScale);

	m_pVTKTransform->Scale(fXScale, fYScale, fZScale);
}

/*!
Create a transform that is the interpolation between two transforms.

\param prevTransform	The previous transform.
\param fRatio			The ratio

\return	The interpolated transform.
*/
Transform* Transform::interpolate(const Transform& prevTransform, double fRatio) const
{
	return new Transform(
		getPosition().interpolate(prevTransform.getPosition(), fRatio),
		getOrientation().interpolate(prevTransform.getOrientation(), fRatio)	);
}

/*!
Convert transform to a string.

\return	The string.
*/
//================================================================== 
// Author: Thomas K. Chen
// Modifications Made:
// - Changed the Qstring from Qt framework to C++ std::string class.
//==================================================================
Transform::operator std::string() const
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
Prints the text for the transform to the stream. This is not the full
transformation in matrix format, but rather the (x, y, z) position and
the (angle, qx, qy, qz) orientation.

\param output	The output stream.
\param input	The transform.

\return The output stream reference.
*/
//================================================================== 
// Author: Thomas K. Chen
// Modifications Made:
// - Replace QTextStream with std::strstream.
//==================================================================
std::strstream& operator<< (std::strstream& output, const Transform& input)
{
	output << input.getPosition() << " " << input.getOrientation();

	double fXScale;
	double fYScale;
	double fZScale;
	input.getScale(&fXScale, &fYScale, &fZScale);
	output << " " << fXScale << " " << fYScale << " " << fZScale;

	return output;
}

/*!
Read the text for the transform from the stream.

\param input	The input stream.
\param output	The transform.

\return	The input stream reference.
*/
//================================================================== 
// Author: Thomas K. Chen
// Modifications Made:
// - Replace QTextStream with std::strstream.
//==================================================================
std::strstream& operator>> (std::strstream& input, Transform& output)
{
	Position position;
	Orientation orientation;

	input >> position >> orientation;
	output.setPose(position, orientation);

	// earlier versions of transform didn't save scale
	if (!input.eof())
	{
		double fXScale;
		double fYScale;
		double fZScale;

		input >> fXScale >> fYScale >> fZScale;
		output.scale(fXScale, fYScale, fZScale);
	}

	return input;
}

/*!
Using two sets of points, compute the transform that matches the two sets.

This code is taken from vtkLandmarkTransform. It was copied here because
it was not part of the standard VTK distribution at the time. If that ever
changes then this code could be removed.

This method could also be used to map detected warped points to expected
(commissioned) warped points. This would potentially aid in accounting for
source position shift after commissioning.

\param pSourceLandmarks	The source points.
\param pTargetLandmarks	The target points.
\param bRigidBody If rigid body is true then points can not be scaled. The default is false.

\return The transform from source to target that matches the two sets (ownership transferred)
*/
Transform* Transform::buildTransform(
	vtkPoints* pSourceLandmarks,
	vtkPoints* pTargetLandmarks,
	bool bRigidBody	)
{
	Transform* pTransform = new Transform();
	vtkMatrix4x4& matrix = *(pTransform->m_pVTKTransform->GetMatrix());

	if (pSourceLandmarks == NULL || pTargetLandmarks == NULL)
	{
		return pTransform;
	}

	// --- compute the necessary transform to match the two sets of landmarks ---

	/*
	The solution is based on
	Berthold K. P. Horn (1987),
	"Closed-form solution of absolute orientation using unit quaternions,"
	Journal of the Optical Society of America A, 4:629-642
	*/

	// Original python implementation by David G. Gobbi

	const int nNumPoints = pSourceLandmarks->GetNumberOfPoints();
	if (nNumPoints != pTargetLandmarks->GetNumberOfPoints())
	{
		TRACE_ERROR("Source and Target Landmarks contain a different number of points");
		return pTransform;
	}

	// -- if no points, stop here
	if (0 == nNumPoints)
	{
		matrix.Identity();
		return pTransform;
	}

	int i;

	// -- find the centroid of each set --
	double source_centroid[3]={0,0,0};
	double target_centroid[3]={0,0,0};
	double *p;
	for (i=0; i < nNumPoints; ++i)
    {
		p = pSourceLandmarks->GetPoint(i);
		source_centroid[0] += p[0];
		source_centroid[1] += p[1];
		source_centroid[2] += p[2];
		p = pTargetLandmarks->GetPoint(i);
		target_centroid[0] += p[0];
		target_centroid[1] += p[1];
		target_centroid[2] += p[2];
    }

	source_centroid[0] /= nNumPoints;
	source_centroid[1] /= nNumPoints;
	source_centroid[2] /= nNumPoints;
	target_centroid[0] /= nNumPoints;
	target_centroid[1] /= nNumPoints;
	target_centroid[2] /= nNumPoints;

	// -- if only one point, stop right here

	if (1 == nNumPoints)
	{
		matrix.Identity();
		matrix.Element[0][3] = target_centroid[0] - source_centroid[0];
		matrix.Element[1][3] = target_centroid[1] - source_centroid[1];
		matrix.Element[2][3] = target_centroid[2] - source_centroid[2];
		return pTransform;
	}

	// -- build the 3x3 matrix M --

	double M[3][3];
	for(i=0; i < 3; ++i) 
	{
		M[i][0]=0.0F; // fill M with zeros
		M[i][1]=0.0F; 
		M[i][2]=0.0F; 
	}

	int pt;
	double a[3],b[3];
	double sa=0.0F,sb=0.0F;
	for(pt=0; pt < nNumPoints; ++pt)
	{
		// get the origin-centred point (a) in the source set
		pSourceLandmarks->GetPoint(pt,a);
		a[0] -= source_centroid[0];
		a[1] -= source_centroid[1];
		a[2] -= source_centroid[2];

		// get the origin-centred point (b) in the target set
		pTargetLandmarks->GetPoint(pt,b);
		b[0] -= target_centroid[0];
		b[1] -= target_centroid[1];
		b[2] -= target_centroid[2];

		// accumulate the products a*T(b) into the matrix M
		for(i=0; i < 3; ++i) 
		{
			M[i][0] += a[i]*b[0];
			M[i][1] += a[i]*b[1];
			M[i][2] += a[i]*b[2];
		}

		// accumulate scale factors (if desired)
		sa += a[0]*a[0]+a[1]*a[1]+a[2]*a[2];
		sb += b[0]*b[0]+b[1]*b[1]+b[2]*b[2];
    }

	// compute required scaling factor (if desired)
	double scale = (double)sqrt(sb/sa);

	// -- build the 4x4 matrix N --

	double Ndata[4][4];
	double *N[4];
	for(i=0;i<4;++i)
	{
		N[i] = Ndata[i];
		N[i][0]=0.0F; // fill N with zeros
		N[i][1]=0.0F;
		N[i][2]=0.0F;
		N[i][3]=0.0F;
	}

	// on-diagonal elements
	N[0][0] = M[0][0]+M[1][1]+M[2][2];
	N[1][1] = M[0][0]-M[1][1]-M[2][2];
	N[2][2] = -M[0][0]+M[1][1]-M[2][2];
	N[3][3] = -M[0][0]-M[1][1]+M[2][2];
	// off-diagonal elements
	N[0][1] = N[1][0] = M[1][2]-M[2][1];
	N[0][2] = N[2][0] = M[2][0]-M[0][2];
	N[0][3] = N[3][0] = M[0][1]-M[1][0];

	N[1][2] = N[2][1] = M[0][1]+M[1][0];
	N[1][3] = N[3][1] = M[2][0]+M[0][2];
	N[2][3] = N[3][2] = M[1][2]+M[2][1];

	// -- eigen-decompose N (is symmetric) --

	double eigenvectorData[4][4];
	double *eigenvectors[4],eigenvalues[4];

	eigenvectors[0] = eigenvectorData[0];
	eigenvectors[1] = eigenvectorData[1];
	eigenvectors[2] = eigenvectorData[2];
	eigenvectors[3] = eigenvectorData[3];

	vtkMath::JacobiN(N,4,eigenvalues,eigenvectors);

	// the eigenvector with the largest eigenvalue is the quaternion we want
	// (they are sorted in decreasing order for us by JacobiN)
	double w,x,y,z;

	// first: if points are collinear, choose the quaternion that 
	// results in the smallest rotation.
	if (eigenvalues[0] == eigenvalues[1] || nNumPoints == 2)
	{
		double s0[3],t0[3],s1[3],t1[3];

		pSourceLandmarks->GetPoint(0,s0);
		pTargetLandmarks->GetPoint(0,t0);
		pSourceLandmarks->GetPoint(1,s1);
		pTargetLandmarks->GetPoint(1,t1);

		double ds[3],dt[3];
		double rs = 0, rt = 0;
		for (i = 0; i < 3; ++i)
		{
			ds[i] = s1[i] - s0[i];      // vector between points
			rs += ds[i]*ds[i];
			dt[i] = t1[i] - t0[i];
			rt += dt[i]*dt[i];
		}

		// normalize the two vectors
		rs = sqrt(rs);
		ds[0] /= rs; ds[1] /= rs; ds[2] /= rs; 
		rt = sqrt(rt);
		dt[0] /= rt; dt[1] /= rt; dt[2] /= rt; 

		// take dot & cross product
		w = ds[0]*dt[0] + ds[1]*dt[1] + ds[2]*dt[2];
		x = ds[1]*dt[2] - ds[2]*dt[1];
		y = ds[2]*dt[0] - ds[0]*dt[2];
		z = ds[0]*dt[1] - ds[1]*dt[0];

		double r = sqrt(x*x + y*y + z*z);
		double theta = atan2(r,w);

		// construct quaternion
		w = cos(theta/2);
		if (r != 0)
		{
			r = sin(theta/2)/r;
			x = x*r;
			y = y*r;
			z = z*r;
		}
		else // rotation by 180 degrees: special case
		{
			// rotate around a vector perpendicular to ds
			vtkMath::Perpendiculars(ds,dt,0,0);
			r = sin(theta/2);
			x = dt[0]*r;
			y = dt[1]*r;
			z = dt[2]*r;
		}
    }
	else // points are not collinear
    {
		w = eigenvectors[0][0];
		x = eigenvectors[1][0];
		y = eigenvectors[2][0];
		z = eigenvectors[3][0];
    }

	// convert quaternion to a rotation matrix

	double ww = w*w;
	double wx = w*x;
	double wy = w*y;
	double wz = w*z;

	double xx = x*x;
	double yy = y*y;
	double zz = z*z;

	double xy = x*y;
	double xz = x*z;
	double yz = y*z;

	matrix.Element[0][0] = ww + xx - yy - zz; 
	matrix.Element[1][0] = 2.0*(wz + xy);
	matrix.Element[2][0] = 2.0*(-wy + xz);

	matrix.Element[0][1] = 2.0*(-wz + xy);  
	matrix.Element[1][1] = ww - xx + yy - zz;
	matrix.Element[2][1] = 2.0*(wx + yz);

	matrix.Element[0][2] = 2.0*(wy + xz);
	matrix.Element[1][2] = 2.0*(-wx + yz);
	matrix.Element[2][2] = ww - xx - yy + zz;

	if (!bRigidBody  /*this->Mode != VTK_LANDMARK_RIGIDBODY*/)
	{ // add in the scale factor (if desired)
		for(i=0;i<3;++i) 
		{
			matrix.Element[i][0] *= scale;
			matrix.Element[i][1] *= scale;
			matrix.Element[i][2] *= scale;
		}
	}

	// the translation is given by the difference in the transformed source
	// centroid and the target centroid
	double sx, sy, sz;

	sx = matrix.Element[0][0] * source_centroid[0] +
	   matrix.Element[0][1] * source_centroid[1] +
	   matrix.Element[0][2] * source_centroid[2];
	sy = matrix.Element[1][0] * source_centroid[0] +
	   matrix.Element[1][1] * source_centroid[1] +
	   matrix.Element[1][2] * source_centroid[2];
	sz = matrix.Element[2][0] * source_centroid[0] +
	   matrix.Element[2][1] * source_centroid[1] +
	   matrix.Element[2][2] * source_centroid[2];

	matrix.Element[0][3] = target_centroid[0] - sx;
	matrix.Element[1][3] = target_centroid[1] - sy;
	matrix.Element[2][3] = target_centroid[2] - sz;

	// fill the bottom row of the 4x4 matrix
	matrix.Element[3][0] = 0.0;
	matrix.Element[3][1] = 0.0;
	matrix.Element[3][2] = 0.0;
	matrix.Element[3][3] = 1.0;

	matrix.Modified();

	return pTransform;
}


/*!
Convert a transform and a point to a rotation about and translation
of that point.  This effectively acts to shift the origin of the
co-ordinate system in which the transform is described to the given
point.  The translation and rotation are returned separately for
ease of use.

The translation is the translation of the point, including the effects
of rotation about the original origin.  The rotation is just the rotation
of the original transform.  Combined, these two operations (rotation
followed by translation) will affect point exactly as the original
transform does.  To rebuild the original transformation, composeTransform
should be used.

\param point The position to re-center on
\param translation The translation of the point
\param fRx the rotation in degrees about the x-axis
\param fRy the rotation in degrees about the y-axis
\param fRz the rotation in degrees about the z-axis

*/
void Transform::decomposeTransform(Position& point, Position & translation, 
								   double* fRx, double* fRy, double* fRz)
{
	translation = transformPosition(point) - point;
	getOrientation(fRx,fRy,fRz);
}

/*!
Convert a rotation about a point, the point and a translation to
a transform.   This method is verbose in the interests of making
clear exactly what is going on.  Every transformation is built
explicitly and accumulated into a single resultant using a vtkActor
to get body-transform rather than co-ordinate transform semantics.

\param point The position about which rotations are made
\param translation The translation of the point
\param fRx the rotation in degrees about the x-axis
\param fRy the rotation in degrees about the y-axis
\param fRz the rotation in degrees about the z-axis

*/
Transform Transform::composeTransform(Position& point, 
									  const Position & translation,
									  double fRx, double fRy, double fRz)
{
	vtkActor* pProp = vtkActor::New();
	pProp->SetOrigin(point.getX(),point.getY(),point.getZ());

	// now rotate about the new origin
	pProp->RotateZ(fRz);
	pProp->RotateX(fRx);
	pProp->RotateY(fRy);

	// now do the user-requested translation
	pProp->AddPosition(translation.getX(),translation.getY(),translation.getZ());

	Transform transform = Transform(pProp->GetMatrix());
	pProp->Delete();
	return transform;
}
