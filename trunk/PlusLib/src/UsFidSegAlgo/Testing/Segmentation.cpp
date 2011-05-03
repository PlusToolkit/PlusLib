// ===========================================================================
// Program Modification Record
// ===========================================================================
// Component:	Component_AutoUSCalibration
// ===========================================================================
// Class:		Segmentation
// ===========================================================================
// File Name:	Segmentation.h
// ===========================================================================
// Author:		Thomas Kuiran Chen <chent@cs.queensu.ca>
//				School of Computing, Queen's University at Kingston, Canada
// ===========================================================================
// Purposes: 
// 1. This the entity class that handles the automated segmentation of the
//    US images (refer to [1, 2] for details).
// 2. The class was built upon the original segmentation program written by 
//    Adrian Thurston (email: thurston@cs.queensu.ca) who is also one of our 
//    co-authors of the publication in [2].
// 3. Uses C++ Standard Library and Standard Template Library.
// ===========================================================================
// References:
// [1] Chen, T.K., Abolmaesumi, P., Ellis, R.E.: An easy-to-use phantom with 
//     reduced complexity for automated freehand ultrasound calibration in the 
//     operating room and large-scale cost-effective manufacture. In Langlotz, 
//     F., Davies, B.L., Ellis, R.E., eds.: Proceedings of 6th Annual Meeting 
//     of the International Society for Computer Assisted Orthopaedic Surgery, 
//     Montr¡äeal, Canada (2006) 89¨C92.
// [2] T. K. Chen, P. Abolmaesumi, A. Thurston, R.E. Ellis, "Automated 3D 
//     Freehand Ultrasound Calibration with Real-Time Accuracy Control," 
//     Accepted to Medical Image Computing and Computer Assisted Intervention 
//     - MICCAI 2006, Denmark.
// ===========================================================================
// Change History:
// Author				Time						Release	Changes
// Thomas Kuiran Chen	Tue Aug 18 08:19 EDT 2006	1.0		Creation
//
// ===========================================================================
//					  Copyright @ Thomas Kuiran Chen, 2006
// ===========================================================================

/*
#ifndef SEGMENTATION_H
#include "Segmentation.h"
#endif

// C++ Standard Includes
#include <iostream>
#include <fstream>	// for file I/O process
#include <ctime>	// for system time: _strdate, _strtime

// VNL Includes
#include "vnl/algo/vnl_matrix_inverse.h"
#include "vcl_istream.h"

// Include of the Segmentation C library written by
// Adrian Thurston (email: thurston@cs.queensu.ca).
#include "kphantseg.h"


// Strings
const std::string Segmentation::mstrScope = "Segmentation";


Segmentation::Segmentation( 
	unsigned char* ImageDataIn1DArray,
	const int ImageWidthInPixels, const int ImageHeightInPixels, 
	const int SearchStartAtX, const int SearchStartAtY, 
	const int SearchDimensionX, const int SearchDimensionY, bool DebugOutput /*=false*//*, std::string possibleFiducialsImageFilename )
	:
	mptrImageDataIn1DArray( ImageDataIn1DArray ),
	mImageWidthInPixels( ImageWidthInPixels ),
	mImageHeightInPixels( ImageHeightInPixels ),
	mSearchStartAtX( SearchStartAtX ),
	mSearchStartAtY( SearchStartAtY ),
	mSearchDimensionX( SearchDimensionX ),
	mSearchDimensionY( SearchDimensionY ),	
	mDebugOutput(DebugOutput),
	mpossibleFiducialsImageFilename(possibleFiducialsImageFilename) 
{
	try
	{
		// perform the segmentation
		segment ();

	}
	catch(...)
	{
		std::cerr << "\n\n" << __FILE__ << "," << __LINE__ << "\n"
			<< ">>>>>>>> In " << mstrScope << "::Construction failed!!!  Throw up ...\n";

		throw;
	}
}

Segmentation::~Segmentation()
{

}

void Segmentation::segment ()
{
	// Reset the segmentation flag
	mSegResults.mIsSegmentationSuccessful = false;

	// Initialize the KPhantomSeg structure with required inputs
	KPhantomSeg TheSegmentationStruct( 
		mImageWidthInPixels, mImageHeightInPixels, 
		mSearchStartAtX, mSearchStartAtY, 
		mSearchDimensionX, mSearchDimensionY,
		mDebugOutput, mpossibleFiducialsImageFilename );
	
	try
	{
		// Initiate the segmentation process
		TheSegmentationStruct.segment( mptrImageDataIn1DArray ); // add paramter to kphantseg


		// Populate the segmentation result
		mSegResults.mSegmentedNFiducialsInFixCorrespondence.resize(0);
		
		
		if( true == TheSegmentationStruct.m_DotsFound )
		{
			mSegResults.mAngleScore = TheSegmentationStruct.m_Angles;
			mSegResults.mIntensityScore = TheSegmentationStruct.m_Intensity;
			mSegResults.fidCandidates = TheSegmentationStruct.m_NumDots; 

			// IMPORTANT:
			// This is a very critical point that would easily cause confusions
			// or mistakes.  There is one and only one fixed correspondence between 
			// the 6 segmented image positions (N-fiducials) and the N-wires (look
			// at 3D view of the N-wire phantom (/KingstonPhantom/
			// VisualizationOfPhantomGeometry/KingstonPhantomGeometryVisualization.png
			// for a visual understanding).
			// NOTE: N-fiducials are those positions where the US image plane 
			// intersects the N-wires therefore appear to be a bright spot (fiducial) 
			// in the US images.
			// - Segmented N-fiducial-0 is on N-wire 1;
			// - Segmented N-fiducial-1 is on N-wire 2;
			// - Segmented N-fiducial-2 is on N-wire 3;
			// - Segmented N-fiducial-3 is on N-wire 4;
			// - Segmented N-fiducial-4 is on N-wire 5;
			// - Segmented N-fiducial-5 is on N-wire 6;
			/*
			double N1[] = {TheSegmentationStruct.x1, TheSegmentationStruct.y1, 0, 1};
			double N2[] = {TheSegmentationStruct.x2, TheSegmentationStruct.y2, 0, 1};
			double N3[] = {TheSegmentationStruct.x3, TheSegmentationStruct.y3, 0, 1};
			double N4[] = {TheSegmentationStruct.x4, TheSegmentationStruct.y4, 0, 1};
			double N5[] = {TheSegmentationStruct.x5, TheSegmentationStruct.y5, 0, 1};
			double N6[] = {TheSegmentationStruct.x6, TheSegmentationStruct.y6, 0, 1};
			vnl_vector<double> NFiducial_1(4,0);
			vnl_vector<double> NFiducial_2(4,0);
			vnl_vector<double> NFiducial_3(4,0);
			vnl_vector<double> NFiducial_4(4,0);
			vnl_vector<double> NFiducial_5(4,0);
			vnl_vector<double> NFiducial_6(4,0);
			NFiducial_1.set( N1 );
			NFiducial_2.set( N2 );
			NFiducial_3.set( N3 );
			NFiducial_4.set( N4 );
			NFiducial_5.set( N5 );
			NFiducial_6.set( N6 );
			*/ 
			
			// NOTE: Adrian's current segmentation library (v2.x) has corrected
			//		 the previous mirrored correspondence as in the following:
			//       (the correspondence of the segmented N-fiducails)
			// Top layer:		1, 2, 3 (from right to left);
			// Bottom Layer:	4, 5, 6 (from right to left);
			// which is just a mirror to our fixed correspondence.
			
			//mSegResults.mSegmentedNFiducialsInFixCorrespondence = m_FoundDotsCoordinateValue;
			/*
			mSegResults.mSegmentedNFiducialsInFixCorrespondence.push_back(NFiducial_2);
			mSegResults.mSegmentedNFiducialsInFixCorrespondence.push_back(NFiducial_3);
			mSegResults.mSegmentedNFiducialsInFixCorrespondence.push_back(NFiducial_4);
			mSegResults.mSegmentedNFiducialsInFixCorrespondence.push_back(NFiducial_5);
			mSegResults.mSegmentedNFiducialsInFixCorrespondence.push_back(NFiducial_6);
			*/

			// NOTE: Adrian's current segmentation library (v1.5-2.0) has the following 
			//       correspondence of the segmented N-fiducails:
			// Top layer:		1, 2, 3 (from left to right);
			// Bottom Layer:	4, 5, 6 (from left to right);
			// which is just a mirror to our fixed correspondence.
			/*
			mSegmentedNFiducialsInFixCorrespondence.push_back(NFiducial_3);
			mSegmentedNFiducialsInFixCorrespondence.push_back(NFiducial_2);
			mSegmentedNFiducialsInFixCorrespondence.push_back(NFiducial_1);
			mSegmentedNFiducialsInFixCorrespondence.push_back(NFiducial_6);
			mSegmentedNFiducialsInFixCorrespondence.push_back(NFiducial_5);
			mSegmentedNFiducialsInFixCorrespondence.push_back(NFiducial_4);*/


		/* 
		   Thanks to Adrian who has taken his effects to update the 
		   segmentation library release to v1.5 where now the output 
		   positions (x1,y1) to (x6,y6) are sorted w.r.t our fixed
		   pre-defined correspondence as in the above, the following
		   sorting codes are not necessary any more.  Just comment
		   out for a future reference. - Thomas Kuiran Chen
		   ==========================================================
		
			// Sort the segmented positions such that aforementioned 
			// wiring correspondence would be strictly established
			// NOTE: 
			// 1. The only guarantee from the Segmentation DLL program
			//    regarding the relationship between points 1 .. 6 is 
			//    that NO.1-3 come from one line and NO.4-6 come from the 
			//    other. Otherwise the data remains unsorted. 
			// 2. Sorting on X values should give us a left-to-right 
			//    ordering, while sorting on average Y values should tell 
			//    us which line is on top/bottom.
	        
			

			// IMPORTANT:
			// This is a very critical point that would easily cause confusions
			// or mistakes.  There is one and only one fixed correspondence between 
			// the 6 segmented image positions (N-fiducials) and the N-wires (look
			// at 3D view of the N-wire phantom (/KingstonPhantom/
			// VisualizationOfPhantomGeometry/KingstonPhantomGeometryVisualization.png
			// for a visual understanding).
			// NOTE: N-fiducials are those positions where the US image plane 
			// intersects the N-wires therefore appear to be a bright spot (fiducial) 
			// in the US images.
			// - Segmented N-fiducial-0 is on N-wire 1;
			// - Segmented N-fiducial-1 is on N-wire 2;
			// - Segmented N-fiducial-2 is on N-wire 3;
			// - Segmented N-fiducial-3 is on N-wire 4;
			// - Segmented N-fiducial-4 is on N-wire 5;
			// - Segmented N-fiducial-5 is on N-wire 6;

			// NOTE: Remember the sorted segmented positions are 
			//       all in X-ascendent order, so in order to output 
			//       the correct N-fiducials in the fixed predefined
			//       correspondence with the N-wires, we will have 
			//       to adjust the ordering as the following.
			if( MeanOfYIn123 < MeanOfYIn456 )
			{	
				// Segmented Positions 1-2-3 is in top layer	
				mSegmentedNFiducialsInFixCorrespondence.push_back(
					SortedNFiducials123XAscendent.at(2) );
				mSegmentedNFiducialsInFixCorrespondence.push_back(
					SortedNFiducials123XAscendent.at(1) );
				mSegmentedNFiducialsInFixCorrespondence.push_back(
					SortedNFiducials123XAscendent.at(0) );

				// Segmented Positions 4-5-6 is in bottom layer
				mSegmentedNFiducialsInFixCorrespondence.push_back(
					SortedNFiducials456XAscendent.at(2) );
				mSegmentedNFiducialsInFixCorrespondence.push_back(
					SortedNFiducials456XAscendent.at(1) );
				mSegmentedNFiducialsInFixCorrespondence.push_back(
					SortedNFiducials456XAscendent.at(0) );
			}
			else
			{
				// Segmented Positions 4-5-6 is in top layer
				mSegmentedNFiducialsInFixCorrespondence.push_back(
					SortedNFiducials456XAscendent.at(2) );
				mSegmentedNFiducialsInFixCorrespondence.push_back(
					SortedNFiducials456XAscendent.at(1) );
				mSegmentedNFiducialsInFixCorrespondence.push_back(
					SortedNFiducials456XAscendent.at(0) );

				// Segmented Positions 1-2-3 is in bottom layer	
				mSegmentedNFiducialsInFixCorrespondence.push_back(
					SortedNFiducials123XAscendent.at(2) );
				mSegmentedNFiducialsInFixCorrespondence.push_back(
					SortedNFiducials123XAscendent.at(1) );
				mSegmentedNFiducialsInFixCorrespondence.push_back(
					SortedNFiducials123XAscendent.at(0) );
			}
		*/ // Sorting codes are not in need and commented out.

		/*} // Segmentation is successful

		mSegResults.mIsSegmentationSuccessful = true;

	}
	catch(...)
	{
		std::cerr << "\n\n" << __FILE__ << "," << __LINE__ << "\n"
			<< ">>>>>>>> In " << mstrScope << "::Unable to complete the segmentation process due to unexpected exceptions thrown!!!\n";

		mSegResults.mIsSegmentationSuccessful = false;
		mSegResults.mSegmentedNFiducialsInFixCorrespondence.resize(0);

		return;
	}	
}

std::vector<SegmentationResults::vnl_vector_double> Segmentation::sortXAscendentOrderForOneNWiresSet( 
	const double *PositionsIn1DArray )
{
	// Extract the unsorted positions for the particular 
	// N-wires set that is composed of three N-fiducials.
	const double X1 = PositionsIn1DArray[0];
	const double Y1 = PositionsIn1DArray[1];
	const double X2 = PositionsIn1DArray[2];
	const double Y2 = PositionsIn1DArray[3];
	const double X3 = PositionsIn1DArray[4];
	const double Y3 = PositionsIn1DArray[5];
	
	// One set of sorted N-fiducials [0 - 2] in X-Ascendent Order
	std::vector<SegmentationResults::vnl_vector_double> SortedNFiducialsInXAscendent;
	vnl_vector<double> NFiducial_1(4,0);
	vnl_vector<double> NFiducial_2(4,0);
	vnl_vector<double> NFiducial_3(4,0);

	if( X1 < X2 )
	{
		if( X3 > X2 )
		{
			// pattern: X1 < X2 < X3
			double N1[] = { X1, Y1, 0, 1};
			double N2[] = { X2, Y2, 0, 1};
			double N3[] = { X3, Y3, 0, 1};
			NFiducial_1.set( N1 );
			NFiducial_2.set( N2 );
			NFiducial_3.set( N3 );
		}
		else if ( X3 < X1 )
		{
			// pattern: X3 < X1 < X2
			double N1[] = { X3, Y3, 0, 1};
			double N2[] = { X1, Y1, 0, 1};
			double N3[] = { X2, Y2, 0, 1};
			NFiducial_1.set( N1 );
			NFiducial_2.set( N2 );
			NFiducial_3.set( N3 );
		}
		else
		{
			// pattern: X1 < X3 < X2
			double N1[] = { X1, Y1, 0, 1};
			double N2[] = { X3, Y3, 0, 1};
			double N3[] = { X2, Y2, 0, 1};
			NFiducial_1.set( N1 );
			NFiducial_2.set( N2 );
			NFiducial_3.set( N3 );
		}

		SortedNFiducialsInXAscendent.push_back( NFiducial_1 );
		SortedNFiducialsInXAscendent.push_back( NFiducial_2 );
		SortedNFiducialsInXAscendent.push_back( NFiducial_3 );
	}
	else	// X1 >= X2
	{
		if( X3 < X2 )
		{
			// pattern: X3 < X2 < X1
			double N1[] = { X3, Y3, 0, 1};
			double N2[] = { X2, Y2, 0, 1};
			double N3[] = { X1, Y1, 0, 1};
			NFiducial_1.set( N1 );
			NFiducial_2.set( N2 );
			NFiducial_3.set( N3 );
		}
		else if ( X3 > X1 )
		{
			// pattern: X2 < X1 < X3
			double N1[] = { X2, Y2, 0, 1};
			double N2[] = { X1, Y1, 0, 1};
			double N3[] = { X3, Y3, 0, 1};
			NFiducial_1.set( N1 );
			NFiducial_2.set( N2 );
			NFiducial_3.set( N3 );
		}
		else
		{
			// pattern: X2 < X3 < X1
			double N1[] = { X2, Y2, 0, 1};
			double N2[] = { X3, Y3, 0, 1};
			double N3[] = { X1, Y1, 0, 1};
			NFiducial_1.set( N1 );
			NFiducial_2.set( N2 );
			NFiducial_3.set( N3 );
		}

		SortedNFiducialsInXAscendent.push_back( NFiducial_1 );
		SortedNFiducialsInXAscendent.push_back( NFiducial_2 );
		SortedNFiducialsInXAscendent.push_back( NFiducial_3 );
	}

	return SortedNFiducialsInXAscendent;

}

// END OF FILE */  