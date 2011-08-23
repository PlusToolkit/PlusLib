// ===========================================================================
// Program Modification Record
// ===========================================================================
// Component:	CommonFramework
// ===========================================================================
// Class:		LinearLeastSquares
// ===========================================================================
// File Name:	LinearLeastSquares.cpp
// ===========================================================================
// Author:		Thomas K. Chen <chent@cs.queensu.ca>
//				School of Computing, Queen's University at Kingston, Canada
// ===========================================================================
// Purposes: 
// 1. This is the mathemtatic class for linear least squares optimization.
// 2. It encaptulates the open source VXL-VNL (Numerical C++ Library)
// 3. Uses C++ Standard Library and Standard Template Library
// ===========================================================================
// Change History:
// Author			Time						Release	Changes
// Thomas K. Chen	Sat Aug 23 09:28 EDT 2003	1.0		Creation
// Thomas K. Chen	Tue May 26 11:18 EDT 2009	2.0		Added Weights to the
//														standard LeastSquares
//
// ===========================================================================

#ifndef LINEARLEASTSQUARES_H
#include "LinearLeastSquares.h"
#endif

// C++ Standard Includes
#include <iostream>

// Exception handler include
#include "Exception.h"

// VNL Includes
#include "vnl/vnl_sparse_matrix.h"
#include "vnl/vnl_sparse_matrix_linear_system.h"
#include "vnl/algo/vnl_lsqr.h"

// Messages
const std::string LinearLeastSquares::mstrScope = "LinearLeastSquares";
const std::string LinearLeastSquares::mstrDataSizeNotMatch = "Data Observer 1 doesn't match Data Observer 2 in size!";
const std::string LinearLeastSquares::mstrDataSizeInvalid = "The size of data set is invalid (apparently <=0)!";
const std::string LinearLeastSquares::mstrCannotCreateRowsOfObserver2 = "Error occurs in breaking data observer 2 to row vectors!";
const std::string LinearLeastSquares::mstrCannotCreateTransformMatrix = "Cannot resize the transform matrix to the desired sizes!";
const std::string LinearLeastSquares::mstrErrorInLeastSquaresOtimization = "Error occurs when perform VNL least squares optimization!";
const std::string LinearLeastSquares::mstrCannotSetDataWeights = "Cannot set the required data weights in a diagnal matrix!";

LinearLeastSquares::LinearLeastSquares(
	vnl_matrix<double> const & DataObserver1, 
	vnl_matrix<double> const & DataObserver2)
	:
	mDataObserver1(DataObserver1),
	mDataObserver2(DataObserver2),
	mDataSetSize(0),
	mDataDimension(0)
{
	initialize();
}

LinearLeastSquares::~LinearLeastSquares()
{
	try
	{

	}
	catch(...)
	{

	}
}

vnl_matrix<double> LinearLeastSquares::getTransform () const
{
	return mTransform;
}

void LinearLeastSquares::initialize()
{
	try
	{
		// Check the size of input data observer 1 and 2
		const int NumOfRowsObserver1 = mDataObserver1.rows();
		const int NumOfColsObserver1 = mDataObserver1.columns();
		const int NumOfRowsObserver2 = mDataObserver2.rows();
		const int NumOfColsObserver2 = mDataObserver2.columns();

		if( NumOfRowsObserver1 <= 0 || 
			NumOfColsObserver1 <= 0 || 
			NumOfRowsObserver2 <= 0 || 
			NumOfColsObserver2 <= 0 )
		{
			throw Exception(__FILE__, __LINE__, mstrScope, mstrDataSizeInvalid);
		}
		
		if( NumOfRowsObserver1 != NumOfRowsObserver2 || 
			NumOfColsObserver1 != NumOfColsObserver2 )
		{
			throw Exception(__FILE__, __LINE__, mstrScope, mstrDataSizeNotMatch);
		}
		
		// Set the size of data set
		mDataSetSize = NumOfColsObserver1;
 
		// Set the dimensionality of data (1D, 2D, ...)
		mDataDimension = NumOfRowsObserver1;

		// Initialize the data weights to identity matrix (unit)
		mDataWeights.set_size( mDataSetSize );
		mDataWeights.fill_diagonal(1);

		// Break DataObserver2 matrix to row vectors
		createRowsOfObserver2();

		// Initialize the transform matrix given the sizes
        createTransformMatrix();

	}
	catch(...)
	{
		throw;
	}
}

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
void LinearLeastSquares::setDataWeights ( vnl_vector_double const & Weights4Observers )
{
	try
	{
		if( Weights4Observers.size() != mDataSetSize ) 
		{
			throw Exception(__FILE__, __LINE__, mstrScope, mstrDataSizeNotMatch);
		}	

		// Set the diagnal elements of the weight matrix
		mDataWeights.set( Weights4Observers );

	}
	catch(...)
	{
		throw Exception(__FILE__, __LINE__, mstrScope, mstrCannotSetDataWeights);
	}
}

void LinearLeastSquares::createRowsOfObserver2()
{
	try
	{
		if(!mRowsOfObserver2.empty())
		{
			mRowsOfObserver2.clear();
		}

		for(int i = 0; i < mDataDimension; i++)
		{
			mRowsOfObserver2.push_back(mDataObserver2.get_row(i));
		}
	}
	catch(...)
	{
		throw Exception(__FILE__, __LINE__, mstrScope, mstrCannotCreateRowsOfObserver2);
	}
}

void LinearLeastSquares::createTransformMatrix()
{
	if( mTransform.set_size(mDataDimension, mDataDimension) )
	{
		mTransform.fill(0);
	}
	else
	{
		throw Exception(__FILE__, __LINE__, mstrScope, mstrCannotCreateTransformMatrix);
	}
}

void LinearLeastSquares::doOptimization()
{
	// We'll have to use vnl_sparse_matrix in the least squares solution.
	// But we are going to limit the use of the sparse matrix just within 
	// this member function scope and use vnl_matrix through rest of the class.
	try
	{
		// ----------------------------------------------------------------------------
		// NOTES:
		// Here we implemented a weighted Least-Squares method where individual weights
		// could be applied to individual data observers respectively, in order to play
		// down the impact from possibly unreliable data (e.g., data acquired using the
		// portion of the ultrasound field that has a relatively large beam width).
		// ----------------------------------------------------------------------------
		// Transfer DataObserver1 to vnl_sparse_matrix format as required by LSQR
		vnl_sparse_matrix<double> SparseObserver1TransposeWeighted(mDataSetSize, mDataDimension);
		const vnl_matrix<double> OriginalObserver1TransposeWeighted( mDataWeights.asMatrix() * mDataObserver1.transpose() );
		for(int i = 0; i < mDataSetSize; i++)
			for(int j = 0; j < mDataDimension; j++)
			{
				// We have to do the transpose manually like this as
				// vnl_sparse_matrix doesn't provide transpose operation
				SparseObserver1TransposeWeighted(i,j) = OriginalObserver1TransposeWeighted(i,j);
			}

		// Construct and solve individual row vectors of transform
		for(int row = 0; row < mRowsOfObserver2.size(); row++)
		{
			// Add weights to DataObserver2 
			const vnl_vector_double WeightedRowOfObserver2 =
				mDataWeights.asMatrix() * mRowsOfObserver2.at(row);

			// Construct linear system defined in VNL
			vnl_sparse_matrix_linear_system<double> LinearSystem(
				SparseObserver1TransposeWeighted, WeightedRowOfObserver2 );

			// Find solution using LSQR optimization
			vnl_lsqr lsqr(LinearSystem);
			vnl_vector<double> RowOfTransform(mDataDimension);
			lsqr.minimize(RowOfTransform);

			// Assign the row into rows of transform
			mTransform.set_row(row, RowOfTransform);
		}

	}
	catch(...)
	{
		throw Exception(__FILE__, __LINE__, mstrScope, mstrErrorInLeastSquaresOtimization);
	}
}
