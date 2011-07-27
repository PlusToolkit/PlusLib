#ifndef LINEARLEASTSQUARES_H
#define LINEARLEASTSQUARES_H
// ===========================================================================
// Program Modification Record
// ===========================================================================
// Component:	CommonFramework
// ===========================================================================
// Class:		LinearLeastSquares
// ===========================================================================
// File Name:	LinearLeastSquares.h
// ===========================================================================
// Author:		Thomas K. Chen <chent@cs.queensu.ca>
//				School of Computing, Queen's University at Kingston, Canada
// ===========================================================================
// Purposes: 
// 1. This is the mathemtatic class for linear least squares optimization.
// 2. Uses the open source VXL-VNL (Numerical and Algebra C++ Library)
// 3. Uses C++ Standard Library and Standard Template Library
// ===========================================================================
// Change History:
// Author			Time						Release	Changes
// Thomas K. Chen	Sat Aug 23 09:28 EDT 2003	1.0		Creation
// Thomas K. Chen	Tue May 26 11:18 EDT 2009	2.0		Added Weights to the
//														standard LeastSquares
//
// ===========================================================================


// C++ Standard Includes
#include <string>
#include <vector>

// VXL/VNL Includes
#include "vnl/vnl_vector.h"
#include "vnl/vnl_matrix.h"
#include "vnl/vnl_diag_matrix.h"


// Linear Least Squares Standard Form
// A * dataObserver1 = dataObserver2
//======================================================================
// class LinearLeastSquares
// 1. Entity that contains individual execption thrown out.
// 2. An exception could have multiple layers of errors during throwing.
//    Therefore, it's the owner of one or more Error objects.
// 3. Typically, the error that originates the problem is the on the
//    lowest layer of the structure (thus the most detailed).  A higher
//    lever error message is normally generated when the same exception
//    gets thrown all way up to the control component, and is generally
//    more abstract and lack of details than the lowest error.
// 4. a std::list object of a chain of errors from bottom up is used to
//    illustrate the error information, from general to specific.
// 5. The class is a direct derived-class of std::exception
//
//    Example
//    . . .
//
//    const string MyImagerType::kstrScope("MyImagerType");
//    const string MyImagerType::kstrUnknownFrameGrabberType("Unknown frame grabber type: '%1'.");
//
//      . . .
//
//	    if (strFrameGrabberType == "SomeFrameGrabber")
//	    {
//		    m_pFrameGrabber = new SomeFrameGrabber();
//	    }
//	    else
//	    {
//		    throw Exception(__FILE__, __LINE__, kstrScope, kstrUnknownFrameGrabberType, strFrameGrabberType);
//	    }
//
//      . . .
//
//====================================================================== 
class LinearLeastSquares
{
	public:

		//! Messages
		static const std::string mstrScope;
		static const std::string mstrDataSizeNotMatch;
		static const std::string mstrDataSizeInvalid;
		static const std::string mstrCannotCreateRowsOfObserver2;
		static const std::string mstrCannotCreateTransformMatrix;
		static const std::string mstrErrorInLeastSquaresOtimization;
		static const std::string mstrCannotSetDataWeights;

		//! Constructor
		LinearLeastSquares(
			vnl_matrix<double> const & DataObserver1, 
			vnl_matrix<double> const & DataObserver2);
		
		~LinearLeastSquares(); 

		//! VNL vector in double format
		typedef vnl_vector<double> vnl_vector_double;

		//! Operation (optional): set weights to the least-squares optimization
		// 1. Here a public interfance is provided to set weights of the data 
		//    observers if desired.
		// 2. Bear in mind that data oberver 1 and 2 are actually two representations
		//    (i.e., in two different spaces) of the same data set, hence the
		//    need of least squares to bridge them together with a rigid transform.
		// 3. The introduction of weights aims to weaken the impact on the least-
		//    squares estimation from those unreliable data, e.g., ultrasound data
		//    that is collected by the region of the sound beam which has a large 
		//    beamwidth (for example, the large dominant elevation beam width).
        void setDataWeights ( vnl_vector_double const & Weights4Observers );

		//! Operation: perform linear least squares optimization via VNL
		void doOptimization ();

		//! Operation: get final transform matrix after optimization
	     vnl_matrix<double> getTransform () const;

	protected:
	 	
	private:

		// Priate Behaviours

		//! Operation: initialization on data members
		void initialize ();

		//! Operation: decompose DataObserver2 to row vectors
		void createRowsOfObserver2 ();

		//! Operation: initialize the final transform matrix at given sizes
		void createTransformMatrix ();

		// Data Members for Class Attributes

		//! Attribute: number of points in data set
		int mDataSetSize;

		//! Attribute: dimensions of the data space
		int mDataDimension;

		//! Attribute: storage of DataObserver1 matrix
		vnl_matrix<double> mDataObserver1;

		//! Attribute: storage of DataObserver2 matrix
		vnl_matrix<double> mDataObserver2;
        
		// Attribute: the rows of dataObserver2
		std::vector<vnl_vector_double> mRowsOfObserver2;

		// Attribute: the weights applied to the data observers.
		// Default: identity matrix (unity weights).
		vnl_diag_matrix<double> mDataWeights;

		// Attribute: the final transform matrix after optimization
		vnl_matrix<double> mTransform;

};


#endif // LINEARLEASTSQUARES_H
