#ifndef SEGMENTATION_H
#define SEGMENTATION_H
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
// 2. The class calls the original segmentation library written in C by 
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


// C++ Standard Includes
#include <string>
#include <vector>
#include <cmath>		

// VXL/VNL Includes
#include "vnl/vnl_matrix.h"
#include "vnl/vnl_vector.h"

class SegmentationResults
{
public:
	SegmentationResults::SegmentationResults() :
		mIsSegmentationSuccessful( false ),
		mAngleScore(-1),
		mIntensityScore(-1)
	{
	}

	//! VNL vector in double format
	typedef vnl_vector<double> vnl_vector_double;	

	//! Attribute: Flag to set if all required positions are segmented. 
	bool mIsSegmentationSuccessful;

	//!Attribute: allows for producing an image of potential fiducials for all frame
	//! Attribute: the segmented positions of the N-fiducials
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
	std::vector<vnl_vector_double> mSegmentedNFiducialsInFixCorrespondence;

	//! Attribubutes: Quality measures of segmentation results.
	// 1. The degree to which the lines are parallel and the dots linear.
	// On the range [0-1], with 
	// - 0: very good, 
	// - 1: the threshold of acceptability.
	float mAngleScore;
	// 2. The combined intensity of the segmented image positions. 
	// This is the sum of the pixel intensity after the morphological operations,
	// with the pixel values on the range [0-1].  
	// - Over 100: A good intensity score;
	// - Below 25: Bad but still valid.
	float mIntensityScore;	
	double fidCandidates; 
};

class SegmentationParameters
{
public:
	SegmentationParameters::SegmentationParameters() :
		mThreshold( 10.0 )
	{
	}
		
	double mThreshold;  // segmentation threshold (in percentage, minimum is 0, maximum is 100
};


//======================================================================
// class Segmentation
//====================================================================== 
class Segmentation
{
	public:
		
		//! Strings
		static const std::string mstrScope;

		// Constructor
		// Initialize the segmentation object with parameters.  
		// IMPORTANT:
		// 1. Input image data should be given in a one-dimension array with
		//    with each element representing the image intensity at pixel
		//    location (X, Y), on the gray-scale range of [0 - 255].
		// 2. The origin of the image is defined to be the top left corner 
		//    of the image: increasing X moves to the right and increasng Y 
		//    moves down. 
		// 3. Given the above definition, the 1-D data array stores the
		//    pixel intensity of one particular position (X, Y) in a row-
		//    major order, using the following formula:
		//    ArrayIndex = X + Y * ImageWidthInPixels
		//
		// DebugOutput: if it is set to true, then intermediate results 
		// are written into files.
		Segmentation(
			unsigned char *ImageDataIn1DArray, 
			const int ImageWidthInPixels, const int ImageHeightInPixels, 
			const int SearchStartAtX, const int SearchStartAtY, 
			const int SearchDimensionX, const int SearchDimensionY, bool DebugOutput=false, std::string  possibleFiducialsImageFilename=0 );

		// Destructor
		~Segmentation ();

		bool isSuccessful ();

		void GetSegmentationResults(SegmentationResults &segResults) { segResults=mSegResults; }

		//std::vector<vnl_vector_double> getAllFiducialCandidates ();
		
	private:

		//! Operation: Perform the segmentation  
		void segment ();

		//! Operation: sort the segmented N-fiducials in X-Ascendent order.
		// This will sort the segmented but unsorted 3 N-fiducials in one set of 
		// N-wires into X-ascendent order.  The operation is essential to
		// establish the fixed correspondence between N-fiducials (in the US
		// image) and the N-wires (in the calibration phantom).
		// Return: one set of sorted N-fiducials [0 - 2] in X-Ascendent Order
		std::vector<SegmentationResults::vnl_vector_double> sortXAscendentOrderForOneNWiresSet( 
			const double *PositionsIn1DArray );

		//! Attribute: A pointer to the input image data
		unsigned char* mptrImageDataIn1DArray;

		//! Attributes: The input image dimensions (in pixels)
		int mImageWidthInPixels;
		int mImageHeightInPixels;

		//! Attributes: Search origin and size 
		// These define the region within the image where we should search for 
		// the dots. Outside of this region the image data must be null. Around 
		// all sides of this region there must be at least 8 pixels of this null 
		// space (searchOrigin >= 8 and searchSize <= imageSize-16). 
		int mSearchStartAtX;
		int mSearchStartAtY;
		int mSearchDimensionX;
		int mSearchDimensionY;
	
		//! Attribute: Flag to test if intermediate transformations should be saved to image files
		bool mDebugOutput;
		std::string mpossibleFiducialsImageFilename; 	

		SegmentationResults mSegResults;

};


#endif // SEGMENTATION_H
