// ===========================================================================
// Program Modification Record
// ===========================================================================
// Component:	Component_AutoTRUSCalibration
// ===========================================================================
// Class:		BrachyTRUSCalibrator
// ===========================================================================
// File Name:	BrachyTRUSCalibrator.cpp
// ===========================================================================
// Author:		Thomas Kuiran Chen <chent@cs.queensu.ca>
//				School of Computing, Queen's University at Kingston, Canada
// ===========================================================================
// Purposes: 
// 1. This is the class that defines our Brachy-TRUS Calibrator [1,2] for 
//    transrectal ultrasound (TRUS) probe calibration in brachytherapy.
// 2. The class is a child class of the parent Phantom class.
// 3. Uses the open source VXL-VNL (Numerical and Algebra C++ Library)
// 4. Uses C++ Standard Library and Standard Template Library
// ===========================================================================
// References:
// [1] Chen, T. K., Thurston, A. D., Ellis, R. E. & Abolmaesumi, P. A real-
//     time freehand ultrasound calibration system with automatic accuracy 
//     feedback and control. Ultrasound Med Biol, 2009, vol. 35, page 79-93.
// [2] Chen, T. K., Thurston, A. D., Moghari, M. H., Ellis, R. E. & 
//     Abolmaesumi, P. Miga, M. I. & Cleary, K. R. (ed.) A real-time 
//     ultrasound calibration system with automatic accuracy control and 
//     incorporation of ultrasound section thickness SPIE Medical Imaging 2008: 
//     Visualization, Image-guided Procedures, and Modeling, 2008,6918,69182A.
//     Best Student Paper Award - Second Place and Cum Laude Poster Award.
// ===========================================================================
// Change History:
//
// Author				Time						Release	Changes
// Thomas Kuiran Chen	Mon Mar 1 08:19 EST 2010	1.0		Creation
//
// ===========================================================================
//					  Copyright @ Thomas Kuiran Chen, 2010
// ===========================================================================

#include "PlusConfigure.h"

#include "BrachyTRUSCalibrator.h"

// C++ Standard Includes
#include <iostream>
#include <fstream>	// for file I/O process
#include <algorithm>

#include <set>
#include <float.h>

// VNL Includes
#include "vnl/algo/vnl_matrix_inverse.h"
#include "vcl_istream.h"

#include "vtkMath.h"
#include "vtkLine.h"

// CommonFramework includes
#include "LinearLeastSquares.h"

// Strings
const std::string BrachyTRUSCalibrator::mstrScope = "BrachyTRUSCalibrator";

//! Contants
// For the simple calibrator, there are only two layers of N-wire with one 
// N-shape in each layer, so there are only two reference points per image 
// to collect. Note: this is very good to automate the calibration process.
const int BrachyTRUSCalibrator::mNUMREFPOINTSPERIMAGE = 6;
// This is the threshold to filter out input data acquired at large beamwidth
// 1. We set the maximum tolerance to be the number of times of the current 
//    minimum magnitude of the US beamwidth typically measured at the 
//    elevation plane's focal zone. 
// 2. According to ultrasound physics, the beamwidth loses most of its
//    resolution after grows to two times that of at focal zone. So 10% 
//    above the twice of the minimum beamwidth may serve a good cutoff
//    point to quality control the imaging data for a reliable calibration.
const double BrachyTRUSCalibrator::mNUMOFTIMESOFMINBEAMWIDTH = 2.1;  

//-----------------------------------------------------------------------------

BrachyTRUSCalibrator::BrachyTRUSCalibrator( FidPatternRecognition * patternRecognitionObject, const bool IsSystemLogOn )
	: Phantom( IsSystemLogOn )	// Call the parent's constructor
{
	try
	{
/*
for( int i = 0; i < 5; i++ )
			for( int j = 0; j < 5; j++ )
		{
			mPhantomGeometryOnFrontInnerWall[i][j].set_size(4);
			mPhantomGeometryOnBackInnerWall[i][j].set_size(4);
		}
*/
		// The reference points on BrachyTRUSCalibrator
		// 1. All positions are all kept w.r.t the phantom frame.
		// 2. These are fixed physical positions measurable using a Stylus probe.
		// 3. They are used to register the phantom geomtry from the phantom 
		//	  frame to the DRB reference frame to be mounted on the calibrator.
		// 4. There are totally 8 reference points (4 on each plate).  See the marking
		//    on the physical calibrator surfaces for their IDs. 
		// 5. All units are in meters.
		mPhantomSpecificReferencePoints.resize(8);
		for( int i = 0; i < 8; i++ )
		{
			mPhantomSpecificReferencePoints[i].set_size(4);
		}
    
    mNWires = patternRecognitionObject->GetFidSegmentation()->GetNWires();

		// Load the phantom-specfic geometry
		loadGeometry();
	}
	catch(...)
	{
		std::cerr << "\n\n" << __FILE__ << "," << __LINE__ << "\n"
			<< ">>>>>>>> In " << mstrScope << "::Construction failed!!!  Throw up ...\n";

		throw;
	}
}

//-----------------------------------------------------------------------------

BrachyTRUSCalibrator::~BrachyTRUSCalibrator()
{
	try
	{

	}
	catch(...)
	{

	}
}

//-----------------------------------------------------------------------------

PlusStatus BrachyTRUSCalibrator::loadGeometry()
{
	if (mIsPhantomGeometryLoaded == true) {
		mIsPhantomGeometryLoaded = false;
	}

	double alphaTopLayerFrontWall = -1.0;
	double alphaTopLayerBackWall = -1.0;
	double alphaBottomLayerFrontWall = -1.0;
	double alphaBottomLayerBackWall = -1.0;

	if( true == mIsSystemLogOn )
	{
		std::ofstream SystemLogFile(
			mSystemLogFileNameWithTimeStamp.c_str(), std::ios::app);
		SystemLogFile << " ==========================================================================\n";
		SystemLogFile << " PHANTOM GEOMETRY >>>>>>>>>>>>>>>>>>>>>\n\n";
		SystemLogFile.close();
	}

	// Read input NWires and convert them to vnl vectors to easier processing
	if (mIsSystemLogOn == true) {
		std::ofstream SystemLogFile(
			mSystemLogFileNameWithTimeStamp.c_str(), std::ios::app);
		SystemLogFile << " -----------------------------------------------------------------------------------\n";
		SystemLogFile << "\n Endpoints of wires = \n\n";
	}

	// List endpoints, check wire ids and NWire geometry correctness (wire order and locations) and compute intersections
	for (std::vector<NWire>::iterator it = mNWires.begin(); it != mNWires.end(); ++it) {
		int layer = it->wires[0].id / 3;
		int sumLayer = 0;

		for (int i=0; i<3; ++i) {
			vnl_vector<double> endPointFront(3);
			vnl_vector<double> endPointBack(3);

			sumLayer += it->wires[i].id;
		
			for (int j=0; j<3; ++j) {
				endPointFront[j] = it->wires[i].endPointFront[j] / 1000.0; //TODO this is meter
				endPointBack[j] = it->wires[i].endPointBack[j] / 1000.0;
			}

			if (mIsSystemLogOn == true) {
				std::ofstream SystemLogFile(
					mSystemLogFileNameWithTimeStamp.c_str(), std::ios::app);
				SystemLogFile << "\t Front endpoint of wire " << i << " on layer " << layer << " = " << endPointFront << "\n";
				SystemLogFile << "\t Back endpoint of wire " << i << " on layer " << layer << " = " << endPointBack << "\n";
			}
		}

		if (sumLayer != layer * 9 + 6) {
			LOG_ERROR("Invalid NWire IDs (" << it->wires[0].id << ", " << it->wires[1].id << ", " << it->wires[2].id << ")!");
			return PLUS_FAIL;
		}

		// Check if the middle wire is the diagonal (the other two are parallel to each other and the first and the second, and the second and the third intersect)
		double wire1[3];
		double wire3[3];
		double cross[3];
		if ((it->GetWireById(1) == NULL) || (it->GetWireById(3) == NULL)) {
			LOG_ERROR("No first or third wire found!");
			return PLUS_FAIL;
		}
		vtkMath::Subtract(it->GetWireById(1)->endPointFront, it->GetWireById(1)->endPointBack, wire1);
		vtkMath::Subtract(it->GetWireById(3)->endPointFront, it->GetWireById(3)->endPointBack, wire3);
		vtkMath::Cross(wire1, wire3, cross);
		if (vtkMath::Norm(cross) != 0) {
			LOG_ERROR("The first and third wire of layer " << layer << " are not parallel!");
			return PLUS_FAIL;
		}
		double closestTemp[3];
		double parametricCoord1, parametricCoord2;
		if (vtkLine::DistanceBetweenLines(it->wires[0].endPointFront, it->wires[0].endPointBack, it->wires[1].endPointFront, it->wires[1].endPointBack, it->intersectPosW12, closestTemp, parametricCoord1, parametricCoord2) > 0.000001) {
			LOG_ERROR("The first and second wire of layer " << layer << " do not intersect each other!");
			return PLUS_FAIL;
		}
		if (vtkLine::DistanceBetweenLines(it->wires[2].endPointFront, it->wires[2].endPointBack, it->wires[1].endPointFront, it->wires[1].endPointBack, it->intersectPosW32, closestTemp, parametricCoord1, parametricCoord2) > 0.000001) {
			LOG_ERROR("The second and third wire of layer " << layer << " do not intersect each other!");
			return PLUS_FAIL;
		}
	}

	// Set the flag
	mIsPhantomGeometryLoaded = true;

	// Log the data pipeline if requested.
	if( true == mIsSystemLogOn )
	{
		std::ofstream SystemLogFile(
			mSystemLogFileNameWithTimeStamp.c_str(), std::ios::app);
		SystemLogFile << " -----------------------------------------------------------------------------------\n";
		int layer;
		std::vector<NWire>::iterator it;
		for (it = mNWires.begin(), layer = 0; it != mNWires.end(); ++it, ++layer) {
			SystemLogFile << "\t Intersection of wire 1 and 2 in layer " << layer << " \t= (" << it->intersectPosW12[0] << ", " << it->intersectPosW12[1] << ", " << it->intersectPosW12[2] << ")\n";
			SystemLogFile << "\t Intersection of wire 3 and 2 in layer " << layer << " \t= (" << it->intersectPosW32[0] << ", " << it->intersectPosW32[1] << ", " << it->intersectPosW32[2] << ")\n";
		}
		SystemLogFile.close();
	}

	return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

void BrachyTRUSCalibrator::addDataPositionsPerImage( 
	std::vector<vnl_vector_double> SegmentedDataPositionListPerImage, 
	const vnl_matrix<double> TransformMatrixUSProbe2Stepper4x4 )
{
  static int frameIndex=-1;
  frameIndex++;
	if( mHasPhantomBeenRegistered != true )
	{
		std::cerr << "\n\n" << __FILE__ << "," << __LINE__ << "\n"
			<< ">>>>>>>> In " << mstrScope << "::The phantom is not yet registered to the DRB frame!!!  Throw up ...\n";

		throw;
	}

	if( mIsPhantomGeometryLoaded != true )
	{
		std::cerr << "\n\n" << __FILE__ << "," << __LINE__ << "\n"
			<< ">>>>>>>> In " << mstrScope << "::The phantom geometry has not been loaded!!!  Throw up ...\n";

		throw;
	}

	if( SegmentedDataPositionListPerImage.size() != mNUMREFPOINTSPERIMAGE )
	{
		std::cerr << "\n\n" << __FILE__ << "," << __LINE__ << "\n"
			<< ">>>>>>>> In " << mstrScope << "::The number of N-wires is NOT "
			<< mNUMREFPOINTSPERIMAGE << " in one US image as required!!!  Throw up ...\n";

		throw;
	}

	if( mHasUSImageFrameOriginBeenSet != true )
	{
		std::cerr << "\n\n" << __FILE__ << "," << __LINE__ << "\n"
			<< ">>>>>>>> In " << mstrScope << "::The ultrasound image frame origin "
			<< "has not been defined yet!!!  Throw up ...\n";

		throw;
	}

	try
	{
		// Obtain the transform matrix from Template/Stepper Frame to the US probe Frame 
		vnl_matrix_inverse<double> inverseMatrix( TransformMatrixUSProbe2Stepper4x4 );
		vnl_matrix<double> TransformMatrixStepper2USProbe4x4 = inverseMatrix.inverse();
		// Make sure the last row in homogeneous transform is [0 0 0 1]
		vnl_vector<double> lastRow(4,0);
		lastRow.put(3, 1);
		TransformMatrixStepper2USProbe4x4.set_row(3, lastRow);

		// ========================================
		// Calculate then store the data positions 
		// ========================================
		// For BrachyTRUSCalibrator, row/col indices are of no interest
		// to us any more, because we only see two layers of N-wires with one
		// N-shape on each layer.  Therefore in each image only two phantom 
		// geometry indices are collected and the program knows their indices
		// in the pre-generated geometry.  This is a good start for automating 
		// the process since we are able to calculate alpha on-the-fly now
		// without requiring to specify the phantom indices one by one.
		//
		// IMPORTANT:
		// Indices defined in the input std::vector array.
		// This is the order that the segmentation algorithm gives the 
		// segmented positions in each image
		//
		// [ Array 0-2: Top N-wire Layer (Right-Middle-Left)]; 
		// [ Array 3-5: Bottom N-wire Layer (Right-Middle-Left)]
		// Each acquired data position is a 4x1 homogenous vector :
		// [ X, Y, 0, 1] all units in pixels
		// ==================================================================

		for( int Layer = 0; Layer < 2; Layer++ )
		{
			// ========================================================
			// The protocol is that the middle point collected in 
			// the set of three points of the N-wire is the data point.
			// ========================================================
			vnl_vector<double> SegmentedPositionInOriginalImageFrame( 
				SegmentedDataPositionListPerImage.at( Layer*3 + 1 ) );
			
			// Convert the segmented image positions from the original 
			// image to the predefined ultrasound image frame.
			vnl_vector<double> SegmentedPositionInUSImageFrame =  
				mTransformOrigImageFrame2TRUSImageFrameMatrix4x4 * 
				SegmentedPositionInOriginalImageFrame;
			
			// Add weights to the positions if required
			// =========================================
			// 1. 3D beam width samples are measured at various axial depth/distance away 
			//    from the transducer crystals surface, i.e., the starting position of 
			//    the sound propagation in an ultrasound image.
			// 2. We have three ways to incorporate the US beamidth to the calibration: Use the 
			//    variance of beamwidth (BWVar) to weight the calibration, use the beamwidth ratio
			//    (BWRatio) to weight the calibration, or use the beamwidth to threshold the input
			//    data (BWTHEVar) in order to eliminate potentially unreliable or error-prone data.
			//    This is determined by the choice of the input flag (Option-1, 2, or 3).
			//    [1] BWVar: This conforms to the standard way of applying weights to least squares, 
			//        where the weights should, ideally, be equal to the reciprocal of the variance of 
			//        the measurement of the data if they are uncorrelated.  Since we know the US beam
			//        width at a given axial depth, resonably assuming the data acquired by the sound 
			//        field is normally distributed, the standard deviation (Sigma) of the data can be
			//        roughly estimated as in the equation: Sigma = USBeamWidth/4 (for 95% of data).
			//	  [2] BWRatio: The fifth row of the matrix is the overall weight defined at that axial 
			//        depth. The weight factor was calculated using: CurrentBeamwidth/MinimumBeamwidth.
			//        The weight is inversely proportional to the weight factor, as obviously, the larger 
			//        the beamwidth, the less reliable the data is than those with the minimum beamwidth.  
			//        We found the weight factor to be a good indicator for how reliable the data is, 
			//        because the larger the beamwidth the larger the uncertainties and errors in data 
			//        acquired from that US field.  E.g., at the axial depth where the beamwidth is two 
			//        times that of minimum beamwidth, the uncertainties are doubled than the imaging 
			//        region that has the minimum beamwidth.
			//	  [3] BWTHEVar: This utilizes the beamwidth to quality control the input calibration 
			//        data, by filtering out those that has a larger beamwidth (e.g., larger than twice
			//        of the minimum beamwidth at the current imaging settings).  According to ultrasound
			//        physics, data acquired in the sound field that doubles the minimum beamwidth at
			//        the scanplane or elevation plane focal zone are typically much less reliable than
			//        those closer to the focal zone.  In addition, the filtered, remainng data would be
			//        weighted for calibration using their beamwidth (BWVar in [1]).
			// FORMAT: each column in the matrix has the following rows:
			// [0]:		Sorted in ascending axial depth in US Image Frame (in pixels);
			// [1-3]:	Beamwith in axial, lateral and elevational axes respectively (in mm);
			// [4]:		Weight Factor = CurrentBeamWidth/MininumBeamWidth (>=1).
			if( true == mIsUSBeamwidthAndWeightFactorsTableReady )
			{
				// Get the axial depth in the US Image Frame for the segmented data point
				// NOTE: the axial depth is in pixels and along the Y-axis of the frame.
				const double ThisAxialDepthInUSImageFrameOriginal = SegmentedPositionInUSImageFrame.get(1);
				// Round the axial depth to one-pixel level (integer)
				const int ThisAxialDepthInUSImageFrameRounded = 
					floor( ThisAxialDepthInUSImageFrameOriginal + 0.5 );

				// Set the weight according to the selected method of incorporation
				double Weight4ThisAxialDepth(-1);
				double USBeamWidthEuclideanMagAtThisAxialDepthInMM(-1);
                if( ThisAxialDepthInUSImageFrameRounded <= 
					mMinimumUSElevationBeamwidthAndFocalZoneInUSImageFrame.get(0) )
				{
					// #1. This is the ultrasound elevation near field which has the
					// the best imaging quality (before the elevation focal zone)
					// -------------------------------------------------------------

					// We will set the beamwidth at the near field to be the same
					// as that of the elevation focal zone.
					USBeamWidthEuclideanMagAtThisAxialDepthInMM = 
						mMinimumUSElevationBeamwidthAndFocalZoneInUSImageFrame.get(1);

					if( 1 == mMethodToIncorporateBeamWidth || 3 == mMethodToIncorporateBeamWidth )
					{	
						// Filtering is not necessary in the near field
						//if( 3 == mMethodToIncorporateBeamWidth && 
						//	USBeamWidthEuclideanMagAtThisAxialDepthInMM >=
                        //    (mMinimumUSElevationBeamwidthAndFocalZoneInUSImageFrame.get(1)*mNUMOFTIMESOFMINBEAMWIDTH) )
						//{
							// Option: BWTHEVar
						//	continue;  // Ignore this data, jump to the next iteration of for-loop
						//}

						// Option: BWVar or BWTHEVar
						// NOTE: The units of the standard deviation of beamwidth
						// needs to be converted to meters to comply to that of
						// the calibration data.
						Weight4ThisAxialDepth = 
							1/( USBeamWidthEuclideanMagAtThisAxialDepthInMM/4 * 0.001 );
					}
					else 
					{
						// Option = BWRatio
						Weight4ThisAxialDepth = 
							sqrt( 1/mUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xM.get(4,0) );
					}
				}
				else if( ThisAxialDepthInUSImageFrameRounded >= 
					mTheFarestAxialDepthInUSBeamwidthAndWeightTable )
				{
					// #2. Further deep in far field (close to the bottom of the image)
					// Ultrasound diverses quickly in this region and data quality deteriorates
					// ------------------------------------------------------------------------

					USBeamWidthEuclideanMagAtThisAxialDepthInMM =
						mUS3DBeamwidthAtFarestAxialDepth.magnitude();

					if( 1 == mMethodToIncorporateBeamWidth || 3 == mMethodToIncorporateBeamWidth )
					{	
						if( 3 == mMethodToIncorporateBeamWidth && 
							USBeamWidthEuclideanMagAtThisAxialDepthInMM >=
                            (mMinimumUSElevationBeamwidthAndFocalZoneInUSImageFrame.get(1)*mNUMOFTIMESOFMINBEAMWIDTH) )
						{
							// Option: BWTHEVar
							continue;  // Ignore this data, jump to the next iteration of for-loop
						}

						// Option: BWVar or BWTHEVar
						// NOTE: The units of the standard deviation of beamwidth
						// needs to be converted to meters to comply to that of
						// the calibration data.
						Weight4ThisAxialDepth = 
							1/( USBeamWidthEuclideanMagAtThisAxialDepthInMM/4 * 0.001 );
					}
					else
					{
						// Option = BWRatio
						Weight4ThisAxialDepth = 
							sqrt( 1/mUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xM.get(4,
								mNumOfTotalBeamWidthData-1) );
					}
				}
				else 
				{
					// #3. Ultrasound far field 
					// Here the sound starts to diverse with elevation beamwidth getting
					// larger and larger.  Data quality starts to deteriorate.
					// ------------------------------------------------------------------

					// Populate the beamwidth vector (axial, lateral and elevation elements)
					vnl_vector<double> US3DBeamwidthAtThisAxialDepth(3,0);
					US3DBeamwidthAtThisAxialDepth.put(0,
						mUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xM.get(1,
								ThisAxialDepthInUSImageFrameRounded -
								mTheNearestAxialDepthInUSBeamwidthAndWeightTable));
					US3DBeamwidthAtThisAxialDepth.put(1,
						mUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xM.get(2,
								ThisAxialDepthInUSImageFrameRounded -
								mTheNearestAxialDepthInUSBeamwidthAndWeightTable));
					US3DBeamwidthAtThisAxialDepth.put(2,
						mUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xM.get(3,
								ThisAxialDepthInUSImageFrameRounded -
								mTheNearestAxialDepthInUSBeamwidthAndWeightTable));
					USBeamWidthEuclideanMagAtThisAxialDepthInMM =
						US3DBeamwidthAtThisAxialDepth.magnitude();

					if( 1 == mMethodToIncorporateBeamWidth || 3 == mMethodToIncorporateBeamWidth )
					{	
						if( 3 == mMethodToIncorporateBeamWidth && 
							USBeamWidthEuclideanMagAtThisAxialDepthInMM >=
                            (mMinimumUSElevationBeamwidthAndFocalZoneInUSImageFrame.get(1)*mNUMOFTIMESOFMINBEAMWIDTH) )
						{
							// Option: BWTHEVar
							continue;  // Ignore this data, jump to the next iteration of for-loop
						}

						// Option: BWVar or BWTHEVar
						// NOTE: The units of the standard deviation of beamwidth
						// needs to be converted to meters to comply to that of
						// the calibration data.
						Weight4ThisAxialDepth = 
							1/( USBeamWidthEuclideanMagAtThisAxialDepthInMM/4 * 0.001 );
					}
					else
					{
						 // Option = BWRatio
						Weight4ThisAxialDepth = 
							sqrt( 1/mUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xM.get(4,
								ThisAxialDepthInUSImageFrameRounded -
								mTheNearestAxialDepthInUSBeamwidthAndWeightTable) );
					}
				}

				mWeightsForDataPositions.push_back( Weight4ThisAxialDepth );
				mUSBeamWidthEuclideanMagAtDataPositions.push_back( 
					USBeamWidthEuclideanMagAtThisAxialDepthInMM );
			}

			// Calcuate the alpha value
			// alpha = |CiXi|/|CiCi+1|
			vnl_vector<double> VectorCi2Xi = 
				SegmentedDataPositionListPerImage.at( Layer*3 + 1 ) -  
				SegmentedDataPositionListPerImage.at( Layer*3 );
			vnl_vector<double> VectorCi2Cii = 
				SegmentedDataPositionListPerImage.at( Layer*3 + 2 ) -  
				SegmentedDataPositionListPerImage.at( Layer*3 );
			double alpha = (double)VectorCi2Xi.magnitude()/VectorCi2Cii.magnitude();
            
			// ==============================================================
			// Apply alpha to Equation: Xi = Ai + alpha * (Bi - Ai)
			// where:
			// - Ai and Bi are the N-wire joints in either front or back walls.
			//
			// IMPORTANT:
			// - This is a very crucial point that would easily cause confusions
			//   or mistakes.  There is one and only one fixed correspondence between 
			//   the 6 segmented image positions (N-fiducials) and the wires.
			// - Closely examine the wiring design and set Ai and Bi accordingly.
            // ==============================================================

			vnl_vector<double> PositionInTemplateFrame(4);
			vnl_vector<double> IntersectPosW12(4);
			vnl_vector<double> IntersectPosW32(4);

			// NWire joints that need to be saved to compute the PLDE (Point-Line Distance Error) 
			// in addition to the real-time PRE3D.
			vnl_vector<double> NWireStartinPhantomFrame;
			vnl_vector<double> NWireEndinPhantomFrame;

			for (int i=0; i<3; ++i) {
				IntersectPosW12[i] = mNWires[Layer].intersectPosW12[i] / 1000.0;
				IntersectPosW32[i] = mNWires[Layer].intersectPosW32[i] / 1000.0;
			}
			IntersectPosW12[3] = 1.0;
			IntersectPosW32[3] = 1.0;

			PositionInTemplateFrame = IntersectPosW12 + alpha * ( IntersectPosW32 - IntersectPosW12 );
      PositionInTemplateFrame[3]=1.0;

			// Finally, calculate the position in the US probe frame
			// X_USProbe = T_Stepper->USProbe * T_Template->Stepper * X_Template
			// NOTE: T_Template->Stepper = mTransformMatrixPhantom2DRB4x4 
			vnl_vector<double> PositionInUSProbeFrame =  
				TransformMatrixStepper2USProbe4x4 * 
				mTransformMatrixPhantom2DRB4x4 *
				PositionInTemplateFrame;

			if( true == mIsSystemLogOn )
			{
				std::ofstream SystemLogFile(
					mSystemLogFileNameWithTimeStamp.c_str(), std::ios::app);
				SystemLogFile << " ==========================================================================\n";
				SystemLogFile << " ADD DATA FOR CALIBRATION ("<<frameIndex<<") >>>>>>>>>>>>>>>>>>>>>\n\n";
				SystemLogFile << " SegmentedNFiducial-" << Layer*3 << " = " << SegmentedDataPositionListPerImage.at( Layer*3 ) << "\n"; 
				SystemLogFile << " SegmentedNFiducial-" << Layer*3+1 << " = " << SegmentedDataPositionListPerImage.at( Layer*3+1 ) << "\n";  
				SystemLogFile << " SegmentedNFiducial-" << Layer*3+2 << " = " << SegmentedDataPositionListPerImage.at( Layer*3+2 ) << "\n";  
				SystemLogFile << " ----------------------------------------------------------------\n";
				SystemLogFile << " SegmentedPositionInOriginalImageFrame = " << SegmentedPositionInOriginalImageFrame << "\n";
				SystemLogFile << " SegmentedPositionInUSImageFrame = " << SegmentedPositionInUSImageFrame << "\n";
				SystemLogFile << " alpha = " << alpha << "\n";
				SystemLogFile << " PositionInTemplateFrame = " << PositionInTemplateFrame << "\n";
				SystemLogFile << " TransformMatrixStepper2USProbe4x4 = \n" << TransformMatrixStepper2USProbe4x4 << "\n";
        SystemLogFile << " mTransformMatrixPhantom2DRB4x4 = \n" << mTransformMatrixPhantom2DRB4x4 << "\n";
        SystemLogFile << " PositionInUSProbeFrame = " << PositionInUSProbeFrame << "\n";
				SystemLogFile.close();
			}

			// Store into the list of positions in the US image frame
			mDataPositionsInUSImageFrame.push_back( SegmentedPositionInUSImageFrame );

			// Store into the list of positions in the Phantom frame
			mDataPositionsInPhantomFrame.push_back( PositionInTemplateFrame );

			// Store into the list of positions in the US probe frame
			mDataPositionsInUSProbeFrame.push_back( PositionInUSProbeFrame );
		}

		// The mAreDataPositionsReady flag will be set when there is at least
		// four sets of reference positions have been successfully populated.
		// Note: for least-squares to converge, at least 4 sets of data positions
		// need to be acquired before the calibration can proceed.
		if( false == mAreDataPositionsReady && mDataPositionsInUSImageFrame.size() >= 4 )
		{
			mAreDataPositionsReady = true;
		}
	}
	catch(...)
	{
		std::cerr << "\n\n" << __FILE__ << "," << __LINE__ << "\n"
			<< ">>>>>>>> In " << mstrScope << "::Failed to add data positions!!!  Throw up ...\n";

		throw;	
	}
}

//-----------------------------------------------------------------------------

void BrachyTRUSCalibrator::addDataPositionsPerImage( 
	std::vector<vnl_vector_double> SegmentedDataPositionListPerImage, 
	const std::vector<double> TransformUSProbe2Tracker,
	const std::vector<double> TransformDRB2Tracker )
{
  static int frameIndex=-1;
  frameIndex++;

	if( mHasPhantomBeenRegistered != true )
	{
		std::cerr << "\n\n" << __FILE__ << "," << __LINE__ << "\n"
			<< ">>>>>>>> In " << mstrScope << "::The phantom is not yet registered to the DRB frame!!!  Throw up ...\n";

		throw;
	}

	if( mIsPhantomGeometryLoaded != true )
	{
		std::cerr << "\n\n" << __FILE__ << "," << __LINE__ << "\n"
			<< ">>>>>>>> In " << mstrScope << "::The phantom geometry has not been loaded!!!  Throw up ...\n";

		throw;
	}

	if( SegmentedDataPositionListPerImage.size() != mNUMREFPOINTSPERIMAGE )
	{
		std::cerr << "\n\n" << __FILE__ << "," << __LINE__ << "\n"
			<< ">>>>>>>> In " << mstrScope << "::The number of N-wires is NOT "
			<< mNUMREFPOINTSPERIMAGE << " in one US image as required!!!  Throw up ...\n";

		throw;
	}

	if( mHasUSImageFrameOriginBeenSet != true )
	{
		std::cerr << "\n\n" << __FILE__ << "," << __LINE__ << "\n"
			<< ">>>>>>>> In " << mstrScope << "::The ultrasound image frame origin "
			<< "has not been defined yet!!!  Throw up ...\n";

		throw;
	}

	try
	{
		// Obtain the transform matrix from DRB Frame to Tracker Frame 
		// when the ultrasound image is captured.
		vnl_matrix<double> TransformMatrixDRB2Tracker = 
			convertTransform2HomoMatrix( TransformDRB2Tracker );

		// Obtain the transform matrix from Tracker Frame to US Probe Frame
		// when the ultrasound image is captured.
		vnl_matrix_inverse<double> inverseMatrix( convertTransform2HomoMatrix( TransformUSProbe2Tracker ) );
		vnl_matrix<double> TransformMatrixTracker2USProbe = inverseMatrix.inverse();
		// Make sure the last row in homogeneous transform is [0 0 0 1]
		vnl_vector<double> lastRow(4,0);
		lastRow.put(3, 1);
		TransformMatrixTracker2USProbe.set_row(3, lastRow);

		// Finally, calculate the transform matrix from DRB Frame to the US probe Frame 
		vnl_matrix<double> TransformMatrixDRB2USProbe4x4 = 
			TransformMatrixTracker2USProbe * 
			TransformMatrixDRB2Tracker;

		// ========================================
		// Calculate then store the data positions 
		// ========================================
		// For BrachyTRUSCalibrator, row/col indices are of no interest
		// to us any more, because we only see two layers of N-wires with one
		// N-shape on each layer.  Therefore in each image only two phantom 
		// geometry indices are collected and the program knows their indices
		// in the pre-generated geometry.  This is a good start for automating 
		// the process since we are able to calculate alpha on-the-fly now
		// without requiring to specify the phantom indices one by one.
		//
		// IMPORTANT:
		// Indices defined in the input std::vector array.
		// This is the order that the segmentation algorithm gives the 
		// segmented positions in each image
		//
		// [ Array 0-2: Top N-wire Layer (Right-Middle-Left)]; 
		// [ Array 3-5: Bottom N-wire Layer (Right-Middle-Left)]
		// Each acquired data position is a 4x1 homogenous vector :
		// [ X, Y, 0, 1] all units in pixels
		// ==================================================================

		for( int Layer = 0; Layer < 2; Layer++ )
		{
			// ========================================================
			// The protocol is that the middle point collected in 
			// the set of three points of the N-wire is the data point.
			// ========================================================
			vnl_vector<double> SegmentedPositionInOriginalImageFrame =
				SegmentedDataPositionListPerImage.at( Layer*3 + 1 );

			// Convert the segmented image positions from the original 
			// image to the predefined ultrasound image frame.
			vnl_vector<double> SegmentedPositionInUSImageFrame =  
				mTransformOrigImageFrame2TRUSImageFrameMatrix4x4 * 
				SegmentedPositionInOriginalImageFrame;

			// Add weights to the positions if required
			// =========================================
			// 1. 3D beam width samples are measured at various axial depth/distance away 
			//    from the transducer crystals surface, i.e., the starting position of 
			//    the sound propagation in an ultrasound image.
			// 2. We have three ways to incorporate the US beamidth to the calibration: Use the 
			//    variance of beamwidth (BWVar) to weight the calibration, use the beamwidth ratio
			//    (BWRatio) to weight the calibration, or use the beamwidth to threshold the input
			//    data (BWTHEVar) in order to eliminate potentially unreliable or error-prone data.
			//    This is determined by the choice of the input flag (Option-1, 2, or 3).
			//    [1] BWVar: This conforms to the standard way of applying weights to least squares, 
			//        where the weights should, ideally, be equal to the reciprocal of the variance of 
			//        the measurement of the data if they are uncorrelated.  Since we know the US beam
			//        width at a given axial depth, resonably assuming the data acquired by the sound 
			//        field is normally distributed, the standard deviation (Sigma) of the data can be
			//        roughly estimated as in the equation: Sigma = USBeamWidth/4 (for 95% of data).
			//	  [2] BWRatio: The fifth row of the matrix is the overall weight defined at that axial 
			//        depth. The weight factor was calculated using: CurrentBeamwidth/MinimumBeamwidth.
			//        The weight is inversely proportional to the weight factor, as obviously, the larger 
			//        the beamwidth, the less reliable the data is than those with the minimum beamwidth.  
			//        We found the weight factor to be a good indicator for how reliable the data is, 
			//        because the larger the beamwidth the larger the uncertainties and errors in data 
			//        acquired from that US field.  E.g., at the axial depth where the beamwidth is two 
			//        times that of minimum beamwidth, the uncertainties are doubled than the imaging 
			//        region that has the minimum beamwidth.
			//	  [3] BWTHEVar: This utilizes the beamwidth to quality control the input calibration 
			//        data, by filtering out those that has a larger beamwidth (e.g., larger than twice
			//        of the minimum beamwidth at the current imaging settings).  According to ultrasound
			//        physics, data acquired in the sound field that doubles the minimum beamwidth at
			//        the scanplane or elevation plane focal zone are typically much less reliable than
			//        those closer to the focal zone.  In addition, the filtered, remainng data would be
			//        weighted for calibration using their beamwidth (BWVar in [1]).
			// FORMAT: each column in the matrix has the following rows:
			// [0]:		Sorted in ascending axial depth in US Image Frame (in pixels);
			// [1-3]:	Beamwith in axial, lateral and elevational axes respectively (in mm);
			// [4]:		Weight Factor = CurrentBeamWidth/MininumBeamWidth (>=1).
			if( true == mIsUSBeamwidthAndWeightFactorsTableReady )
			{
				// Get the axial depth in the US Image Frame for the segmented data point
				// NOTE: the axial depth is in pixels and along the Y-axis of the frame.
				const double ThisAxialDepthInUSImageFrameOriginal = SegmentedPositionInUSImageFrame.get(1);
				// Round the axial depth to one-pixel level (integer)
				const int ThisAxialDepthInUSImageFrameRounded = 
					floor( ThisAxialDepthInUSImageFrameOriginal + 0.5 );

				// Set the weight according to the selected method of incorporation
				double Weight4ThisAxialDepth(-1);
				double USBeamWidthEuclideanMagAtThisAxialDepthInMM(-1);
                if( ThisAxialDepthInUSImageFrameRounded <= 
					mMinimumUSElevationBeamwidthAndFocalZoneInUSImageFrame.get(0) )
				{
					// #1. This is the ultrasound elevation near field which has the
					// the best imaging quality (before the elevation focal zone)
					// -------------------------------------------------------------

					// We will set the beamwidth at the near field to be the same
					// as that of the elevation focal zone.
					USBeamWidthEuclideanMagAtThisAxialDepthInMM = 
						mMinimumUSElevationBeamwidthAndFocalZoneInUSImageFrame.get(1);

					if( 1 == mMethodToIncorporateBeamWidth || 3 == mMethodToIncorporateBeamWidth )
					{	
						// Filtering is not necessary in the near field
						//if( 3 == mMethodToIncorporateBeamWidth && 
						//	USBeamWidthEuclideanMagAtThisAxialDepthInMM >=
                        //    (mMinimumUSElevationBeamwidthAndFocalZoneInUSImageFrame.get(1)*mNUMOFTIMESOFMINBEAMWIDTH) )
						//{
							// Option: BWTHEVar
						//	continue;  // Ignore this data, jump to the next iteration of for-loop
						//}

						// Option: BWVar or BWTHEVar
						// NOTE: The units of the standard deviation of beamwidth
						// needs to be converted to meters to comply to that of
						// the calibration data.
						Weight4ThisAxialDepth = 
							1/( USBeamWidthEuclideanMagAtThisAxialDepthInMM/4 * 0.001 );
					}
					else 
					{
						// Option = BWRatio
						Weight4ThisAxialDepth = 
							sqrt( 1/mUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xM.get(4,0) );
					}
				}
				else if( ThisAxialDepthInUSImageFrameRounded >= 
					mTheFarestAxialDepthInUSBeamwidthAndWeightTable )
				{
					// #2. Further deep in far field (close to the bottom of the image)
					// Ultrasound diverses quickly in this region and data quality deteriorates
					// ------------------------------------------------------------------------

					USBeamWidthEuclideanMagAtThisAxialDepthInMM =
						mUS3DBeamwidthAtFarestAxialDepth.magnitude();

					if( 1 == mMethodToIncorporateBeamWidth || 3 == mMethodToIncorporateBeamWidth )
					{	
						if( 3 == mMethodToIncorporateBeamWidth && 
							USBeamWidthEuclideanMagAtThisAxialDepthInMM >=
                            (mMinimumUSElevationBeamwidthAndFocalZoneInUSImageFrame.get(1)*mNUMOFTIMESOFMINBEAMWIDTH) )
						{
							// Option: BWTHEVar
							continue;  // Ignore this data, jump to the next iteration of for-loop
						}

						// Option: BWVar or BWTHEVar
						// NOTE: The units of the standard deviation of beamwidth
						// needs to be converted to meters to comply to that of
						// the calibration data.
						Weight4ThisAxialDepth = 
							1/( USBeamWidthEuclideanMagAtThisAxialDepthInMM/4 * 0.001 );
					}
					else
					{
						// Option = BWRatio
						Weight4ThisAxialDepth = 
							sqrt( 1/mUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xM.get(4,
								mNumOfTotalBeamWidthData-1) );
					}
				}
				else 
				{
					// #3. Ultrasound far field 
					// Here the sound starts to diverse with elevation beamwidth getting
					// larger and larger.  Data quality starts to deteriorate.
					// ------------------------------------------------------------------

					// Populate the beamwidth vector (axial, lateral and elevation elements)
					vnl_vector<double> US3DBeamwidthAtThisAxialDepth(3,0);
					US3DBeamwidthAtThisAxialDepth.put(0,
						mUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xM.get(1,
								ThisAxialDepthInUSImageFrameRounded -
								mTheNearestAxialDepthInUSBeamwidthAndWeightTable));
					US3DBeamwidthAtThisAxialDepth.put(1,
						mUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xM.get(2,
								ThisAxialDepthInUSImageFrameRounded -
								mTheNearestAxialDepthInUSBeamwidthAndWeightTable));
					US3DBeamwidthAtThisAxialDepth.put(2,
						mUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xM.get(3,
								ThisAxialDepthInUSImageFrameRounded -
								mTheNearestAxialDepthInUSBeamwidthAndWeightTable));
					USBeamWidthEuclideanMagAtThisAxialDepthInMM =
						US3DBeamwidthAtThisAxialDepth.magnitude();

					if( 1 == mMethodToIncorporateBeamWidth || 3 == mMethodToIncorporateBeamWidth )
					{	
						if( 3 == mMethodToIncorporateBeamWidth && 
							USBeamWidthEuclideanMagAtThisAxialDepthInMM >=
                            (mMinimumUSElevationBeamwidthAndFocalZoneInUSImageFrame.get(1)*mNUMOFTIMESOFMINBEAMWIDTH) )
						{
							// Option: BWTHEVar
							continue;  // Ignore this data, jump to the next iteration of for-loop
						}

						// Option: BWVar or BWTHEVar
						// NOTE: The units of the standard deviation of beamwidth
						// needs to be converted to meters to comply to that of
						// the calibration data.
						Weight4ThisAxialDepth = 
							1/( USBeamWidthEuclideanMagAtThisAxialDepthInMM/4 * 0.001 );
					}
					else
					{
						 // Option = BWRatio
						Weight4ThisAxialDepth = 
							sqrt( 1/mUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xM.get(4,
								ThisAxialDepthInUSImageFrameRounded -
								mTheNearestAxialDepthInUSBeamwidthAndWeightTable) );
					}
				}

				mWeightsForDataPositions.push_back( Weight4ThisAxialDepth );
				mUSBeamWidthEuclideanMagAtDataPositions.push_back( 
					USBeamWidthEuclideanMagAtThisAxialDepthInMM );
			}

			// Calcuate the alpha value
			// alpha = |CiXi|/|CiCi+1|
			vnl_vector<double> VectorCi2Xi = 
				SegmentedDataPositionListPerImage.at( Layer*3 + 1 ) -  
				SegmentedDataPositionListPerImage.at( Layer*3 );
			vnl_vector<double> VectorCi2Cii = 
				SegmentedDataPositionListPerImage.at( Layer*3 + 2 ) -  
				SegmentedDataPositionListPerImage.at( Layer*3 );
			double alpha = (double)VectorCi2Xi.magnitude()/VectorCi2Cii.magnitude();
            
			// ==============================================================
			// Apply alpha to Equation: Xi = Ai + alpha * (Bi - Ai)
			// where:
			// - Ai and Bi are the N-wire joints in either front or back walls.
			//
			// IMPORTANT:
			// - This is a very crucial point that would easily cause confusions
			//   or mistakes.  There is one and only one fixed correspondence between 
			//   the 6 segmented image positions (N-fiducials) and the wires.
			// - Closely examine the wiring design and set Ai and Bi accordingly.
            // ==============================================================

			vnl_vector<double> PositionInPhantomFrame(3);
			vnl_vector<double> IntersectPosW12(3);
			vnl_vector<double> IntersectPosW32(3);

			for (int i=0; i<3; ++i) {
				IntersectPosW12[i] = mNWires[Layer].intersectPosW12[i] / 1000.0;
				IntersectPosW32[i] = mNWires[Layer].intersectPosW32[i] / 1000.0;
			}

			PositionInPhantomFrame = IntersectPosW12 + alpha * ( IntersectPosW32 - IntersectPosW12 );

      // Finally, calculate the position in the US probe frame
      // X(US Probe) =  T(Tracker->US Probe)*T(DRB->Tracker)*T(Phantom->DRB)*X(Phantom)
      vnl_vector<double> PositionInUSProbeFrame =  
        TransformMatrixDRB2USProbe4x4 * 
        mTransformMatrixPhantom2DRB4x4 * 
        PositionInPhantomFrame;

			if( true == mIsSystemLogOn )
			{
				std::ofstream SystemLogFile(
					mSystemLogFileNameWithTimeStamp.c_str(), std::ios::app);
				SystemLogFile << " ==========================================================================\n";
				SystemLogFile << " ADD DATA FOR CALIBRATION ("<<frameIndex<<") >>>>>>>>>>>>>>>>>>>>>\n\n";
				SystemLogFile << " SegmentedNFiducial-" << Layer*3 << " = " << SegmentedDataPositionListPerImage.at( Layer*3 ) << "\n"; 
				SystemLogFile << " SegmentedNFiducial-" << Layer*3+1 << " = " << SegmentedDataPositionListPerImage.at( Layer*3+1 ) << "\n";  
				SystemLogFile << " SegmentedNFiducial-" << Layer*3+2 << " = " << SegmentedDataPositionListPerImage.at( Layer*3+2 ) << "\n";  
				SystemLogFile << " ----------------------------------------------------------------\n";
				SystemLogFile << " SegmentedPositionInOriginalImageFrame = " << SegmentedPositionInOriginalImageFrame << "\n";
				SystemLogFile << " SegmentedPositionInUSImageFrame = " << SegmentedPositionInUSImageFrame << "\n";
				SystemLogFile << " alpha = " << alpha << "\n";
				SystemLogFile << " PositionInPhantomFrame = " << PositionInPhantomFrame << "\n";
				SystemLogFile << " TransformMatrixDRB2USProbe4x4 = \n" << TransformMatrixDRB2USProbe4x4 << "\n";
        SystemLogFile << " mTransformMatrixPhantom2DRB4x4 = \n" << mTransformMatrixPhantom2DRB4x4 << "\n";
        SystemLogFile << " PositionInUSProbeFrame = " << PositionInUSProbeFrame << "\n";
				SystemLogFile.close();
			}

			// Store into the list of positions in the US image frame
			mDataPositionsInUSImageFrame.push_back( SegmentedPositionInUSImageFrame );

			// Store into the list of positions in the Phantom frame
			mDataPositionsInPhantomFrame.push_back( PositionInPhantomFrame );

			// Store into the list of positions in the US probe frame
			mDataPositionsInUSProbeFrame.push_back( PositionInUSProbeFrame );
		}

		// The mAreDataPositionsReady flag will be set when there is at least
		// four sets of reference positions have been successfully populated.
		// Note: for least-squares to converge, at least 4 sets of data positions
		// need to be acquired before the calibration can proceed.
		if( false == mAreDataPositionsReady && mDataPositionsInUSImageFrame.size() >= 4 )
		{
			mAreDataPositionsReady = true;
		}

	}
	catch(...)
	{
		std::cerr << "\n\n" << __FILE__ << "," << __LINE__ << "\n"
			<< ">>>>>>>> In " << mstrScope << "::Failed to add data positions!!!  Throw up ...\n";

		throw;	
	}
}

//-----------------------------------------------------------------------------

void BrachyTRUSCalibrator::deleteLatestAddedDataPositionsPerImage ()
{
	// Delete the latest added data positions per image
	// NOTE: this operation will delete all the two layers' of N-wires 
	// data from the last used image from both data positions in the US 
	// image frame and in the US probe frame.
	for( int i = 0; i < 2; i++ )
	{
		mDataPositionsInUSImageFrame.pop_back();
		mDataPositionsInUSProbeFrame.pop_back();
	}
}

//-----------------------------------------------------------------------------

void BrachyTRUSCalibrator::deleteDataPositionsPerImage(int position)
{
	// Delete the specified data positions per image
	// NOTE: this operation will delete only one layer of N-wires 
	// data from both data positions in the US image frame, phantom frame and in the US probe frame.
	mDataPositionsInUSImageFrame.erase( mDataPositionsInUSImageFrame.begin() + position );
	mDataPositionsInPhantomFrame.erase( mDataPositionsInPhantomFrame.begin() + position );
	mDataPositionsInUSProbeFrame.erase( mDataPositionsInUSProbeFrame.begin() + position );
}

//-----------------------------------------------------------------------------

void BrachyTRUSCalibrator::addValidationPositionsPerImage( 
	std::vector<vnl_vector_double> SegmentedDataPositionListPerImage, 
	const vnl_matrix<double> TransformMatrixUSProbe2Stepper4x4 )
{
  static int frameIndex=-1;
  frameIndex++;

	if( mHasPhantomBeenRegistered != true )
	{
		std::cerr << "\n\n" << __FILE__ << "," << __LINE__ << "\n"
			<< ">>>>>>>> In " << mstrScope << "::The phantom is not yet registered to the DRB frame!!!  Throw up ...\n";

		throw;
	}

	if( mIsPhantomGeometryLoaded != true )
	{
		std::cerr << "\n\n" << __FILE__ << "," << __LINE__ << "\n"
			<< ">>>>>>>> In " << mstrScope << "::The phantom geometry has not been loaded!!!  Throw up ...\n";

		throw;
	}

	if( SegmentedDataPositionListPerImage.size() != mNUMREFPOINTSPERIMAGE )
	{
		std::cerr << "\n\n" << __FILE__ << "," << __LINE__ << "\n"
			<< ">>>>>>>> In " << mstrScope << "::The number of N-wires is NOT "
			<< mNUMREFPOINTSPERIMAGE << " in one US image as required!!!  Throw up ...\n";

		throw;
	}

	if( mHasUSImageFrameOriginBeenSet != true )
	{
		std::cerr << "\n\n" << __FILE__ << "," << __LINE__ << "\n"
			<< ">>>>>>>> In " << mstrScope << "::The ultrasound image frame origin "
			<< "has not been defined yet!!!  Throw up ...\n";

		throw;
	}

	try
	{
		// Obtain the transform matrix from DRB Frame to the US probe Frame 
		vnl_matrix_inverse<double> inverseMatrix( TransformMatrixUSProbe2Stepper4x4 );
		vnl_matrix<double> TransformMatrixStepper2USProbe4x4 = inverseMatrix.inverse();
		// Make sure the last row in homogeneous transform is [0 0 0 1]
		vnl_vector<double> lastRow(4,0);
		lastRow.put(3, 1);
		TransformMatrixStepper2USProbe4x4.set_row(3, lastRow);

		// ========================================
		// Calculate then store the data positions 
		// ========================================
		// For BrachyTRUSCalibrator, row/col indices are of no interest
		// to us any more, because we only see two layers of N-wires with one
		// N-shape on each layer.  Therefore in each image only two phantom 
		// geometry indices are collected and the program knows their indices
		// in the pre-generated geometry.  This is a good start for automating 
		// the process since we are able to calculate alpha on-the-fly now
		// without requiring to specify the phantom indices one by one.
		//
		// IMPORTANT:
		// Indices defined in the input std::vector array.
		// This is the order that the segmentation algorithm gives the 
		// segmented positions in each image
		//
		// [ Array 0-2: Top N-wire Layer (Right-Middle-Left)]; 
		// [ Array 3-5: Bottom N-wire Layer (Right-Middle-Left)]
		// Each acquired data position is a 4x1 homogenous vector :
		// [ X, Y, 0, 1] all units in pixels
		// ==================================================================

		// ========================================================
		// Collect the wire locations (the two parallel wires of 
		// each of the N-shape) for independent Line-Reconstruction 
		// Error (LRE) validation.
		// Note: N1, N3, N4, and N6 are the parallel wires here.
		// ========================================================
            
		vnl_vector<double> N1SegmentedPositionInOriginalImageFrame( 
			SegmentedDataPositionListPerImage.at(0) );
		vnl_vector<double> N3SegmentedPositionInOriginalImageFrame( 
			SegmentedDataPositionListPerImage.at(2) );
		vnl_vector<double> N4SegmentedPositionInOriginalImageFrame( 
			SegmentedDataPositionListPerImage.at(3) );
		vnl_vector<double> N6SegmentedPositionInOriginalImageFrame( 
			SegmentedDataPositionListPerImage.at(5) );

		// Convert the segmented image positions from the original 
		// image to the predefined ultrasound image frame.
		vnl_vector<double> N1SegmentedPositionInUSImageFrame =  
			mTransformOrigImageFrame2TRUSImageFrameMatrix4x4 * 
			N1SegmentedPositionInOriginalImageFrame;
		vnl_vector<double> N3SegmentedPositionInUSImageFrame =  
			mTransformOrigImageFrame2TRUSImageFrameMatrix4x4 * 
			N3SegmentedPositionInOriginalImageFrame;
		vnl_vector<double> N4SegmentedPositionInUSImageFrame =  
			mTransformOrigImageFrame2TRUSImageFrameMatrix4x4 * 
			N4SegmentedPositionInOriginalImageFrame;
		vnl_vector<double> N6SegmentedPositionInUSImageFrame =  
			mTransformOrigImageFrame2TRUSImageFrameMatrix4x4 * 
			N6SegmentedPositionInOriginalImageFrame;

		for( int Layer = 0; Layer < 2; Layer++ )
		{
			// ========================================================
			// The protocol is that the middle point collected in 
			// the set of three points of the N-wire is the data point.
			// ========================================================
            vnl_vector<double> SegmentedPositionInOriginalImageFrame( 
				SegmentedDataPositionListPerImage.at( Layer*3 + 1 ) );

			// Convert the segmented image positions from the original 
			// image to the predefined ultrasound image frame.
			vnl_vector<double> SegmentedPositionInUSImageFrame =  
				mTransformOrigImageFrame2TRUSImageFrameMatrix4x4 * 
				SegmentedPositionInOriginalImageFrame;

			// OPTION: Apply the ultrasound 3D beamwidth to the validation dataset
			// ====================================================================
			// If Option 3 is selected, we will use the ultrasound 3D
			// beamwidth to filter out the validation data that has larger
			// beamwidth which are potentially unreliable than those dataset
			// with a smaller beamwidth.  If Option 1 or 2 is selected, the
			// beamwidth of the validation data will be tracked and recorded 
			// for analysis only (no filtering is performed).
			if( true == mIsUSBeamwidthAndWeightFactorsTableReady )
			{
				// Get the axial depth in the US Image Frame for the segmented data point
				// NOTE: the axial depth is in pixels and along the Y-axis of the frame.
				const double ThisAxialDepthInUSImageFrameOriginal = 
					SegmentedPositionInUSImageFrame.get(1);
				// Round the axial depth to one-pixel level (integer)
				const int ThisAxialDepthInUSImageFrameRounded = 
					floor( ThisAxialDepthInUSImageFrameOriginal + 0.5 );

				double USBeamWidthEuclideanMagAtThisAxialDepthInMM(-1);
				if( ThisAxialDepthInUSImageFrameRounded <= 
					mMinimumUSElevationBeamwidthAndFocalZoneInUSImageFrame.get(0) )
				{
					// #1. This is the ultrasound elevation near field which has the
					// the best imaging quality (before the elevation focal zone)
					// -------------------------------------------------------------

					// We will set the beamwidth to be the same as the elevation
					// focal zone.
					USBeamWidthEuclideanMagAtThisAxialDepthInMM = 
						mMinimumUSElevationBeamwidthAndFocalZoneInUSImageFrame.get(1);
				}
				else if( ThisAxialDepthInUSImageFrameRounded >= 
					mTheFarestAxialDepthInUSBeamwidthAndWeightTable )
				{
					// #2. Further deep in far field (close to the bottom of the image)
					// Ultrasound diverses quickly in this region and data quality deteriorates
					// ------------------------------------------------------------------------
					USBeamWidthEuclideanMagAtThisAxialDepthInMM =
						mUS3DBeamwidthAtFarestAxialDepth.magnitude();
				}
				else 
				{
					// #3. Ultrasound far field 
					// Here the sound starts to diverse with elevation beamwidth getting
					// larger and larger.  Data quality starts to deteriorate.
					// ------------------------------------------------------------------

					// Populate the beamwidth vector (axial, lateral and elevation elements)
					vnl_vector<double> US3DBeamwidthAtThisAxialDepth(3,0);
					US3DBeamwidthAtThisAxialDepth.put(0,
						mUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xM.get(1,
								ThisAxialDepthInUSImageFrameRounded -
								mTheNearestAxialDepthInUSBeamwidthAndWeightTable));
					US3DBeamwidthAtThisAxialDepth.put(1,
						mUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xM.get(2,
								ThisAxialDepthInUSImageFrameRounded -
								mTheNearestAxialDepthInUSBeamwidthAndWeightTable));
					US3DBeamwidthAtThisAxialDepth.put(2,
						mUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xM.get(3,
								ThisAxialDepthInUSImageFrameRounded -
								mTheNearestAxialDepthInUSBeamwidthAndWeightTable));

					USBeamWidthEuclideanMagAtThisAxialDepthInMM =
						US3DBeamwidthAtThisAxialDepth.magnitude();
				}
					
				if( 3 == mMethodToIncorporateBeamWidth && USBeamWidthEuclideanMagAtThisAxialDepthInMM >= 
					(mMinimumUSElevationBeamwidthAndFocalZoneInUSImageFrame.get(1)*mNUMOFTIMESOFMINBEAMWIDTH) )
				{
					continue;  // Ignore this data, jump to the next iteration of the for-loop
				}
				
				mUSBeamWidthEuclideanMagAtValidationPositions.push_back(
					USBeamWidthEuclideanMagAtThisAxialDepthInMM );
			}

			// Calcuate the alpha value
			// alpha = |CiXi|/|CiCi+1|
			vnl_vector<double> VectorCi2Xi = 
				SegmentedDataPositionListPerImage.at( Layer*3 + 1 ) -  
				SegmentedDataPositionListPerImage.at( Layer*3 );
			vnl_vector<double> VectorCi2Cii = 
				SegmentedDataPositionListPerImage.at( Layer*3 + 2 ) -  
				SegmentedDataPositionListPerImage.at( Layer*3 );
			double alpha = (double)VectorCi2Xi.magnitude()/VectorCi2Cii.magnitude();
            
			// ==============================================================
			// Apply alpha to Equation: Xi = Ai + alpha * (Bi - Ai)
			// where:
			// - Ai and Bi are the N-wire joints in either front or back walls.
			//
			// IMPORTANT:
			// - This is a very crucial point that would easily cause confusions
			//   or mistakes.  There is one and only one fixed correspondence between 
			//   the 6 segmented image positions (N-fiducials) and the wires.
			// - Closely examine the wiring design and set Ai and Bi accordingly.
            // ==============================================================

			vnl_vector<double> PositionInTemplateFrame(4);
			vnl_vector<double> IntersectPosW12(4);
			vnl_vector<double> IntersectPosW32(4);

			// NWire joints that need to be saved to compute the PLDE (Point-Line Distance Error) 
			// in addition to the real-time PRE3D.
			vnl_vector<double> NWireStartinPhantomFrame;
			vnl_vector<double> NWireEndinPhantomFrame;

			for (int i=0; i<3; ++i) {
				IntersectPosW12[i] = mNWires[Layer].intersectPosW12[i] / 1000.0;
				IntersectPosW32[i] = mNWires[Layer].intersectPosW32[i] / 1000.0;
			}
			IntersectPosW12[3] = 1.0;
			IntersectPosW32[3] = 1.0;

			PositionInTemplateFrame = IntersectPosW12 + alpha * ( IntersectPosW32 - IntersectPosW12 );
      PositionInTemplateFrame[3]=1.0;

			// Finally, calculate the position in the US probe frame
			// X_USProbe = T_Stepper->USProbe * T_Template->Stepper * X_Template
			// NOTE: T_Template->Stepper = mTransformMatrixPhantom2DRB4x4 
			vnl_vector<double> PositionInUSProbeFrame =  
				TransformMatrixStepper2USProbe4x4 * 
				mTransformMatrixPhantom2DRB4x4 *
				PositionInTemplateFrame;

			if( true == mIsSystemLogOn )
			{
				std::ofstream SystemLogFile(
					mSystemLogFileNameWithTimeStamp.c_str(), std::ios::app);
				SystemLogFile << " ==========================================================================\n";
        SystemLogFile << " ADD DATA FOR VALIDATION ("<<frameIndex<<") >>>>>>>>>>>>>>>>>>>>>\n\n";
				SystemLogFile << " SegmentedNFiducial-" << Layer*3 << " = " << SegmentedDataPositionListPerImage.at( Layer*3 ) << "\n"; 
				SystemLogFile << " SegmentedNFiducial-" << Layer*3+1 << " = " << SegmentedDataPositionListPerImage.at( Layer*3+1 ) << "\n";  
				SystemLogFile << " SegmentedNFiducial-" << Layer*3+2 << " = " << SegmentedDataPositionListPerImage.at( Layer*3+2 ) << "\n";  
				SystemLogFile << " ----------------------------------------------------------------\n";
				SystemLogFile << " SegmentedPositionInOriginalImageFrame = " << SegmentedPositionInOriginalImageFrame << "\n";
				SystemLogFile << " SegmentedPositionInUSImageFrame = " << SegmentedPositionInUSImageFrame << "\n";
				SystemLogFile << " alpha = " << alpha << "\n";
				SystemLogFile << " PositionInTemplateFrame = " << PositionInTemplateFrame << "\n";					
				SystemLogFile << " TransformMatrixStepper2USProbe4x4 = \n" << TransformMatrixStepper2USProbe4x4 << "\n";
        SystemLogFile << " mTransformMatrixPhantom2DRB4x4 = \n" << mTransformMatrixPhantom2DRB4x4 << "\n";
        SystemLogFile << " PositionInUSProbeFrame = " << PositionInUSProbeFrame << "\n";					
				SystemLogFile.close();
			}

			vnl_vector<double> NWireStartinUSProbeFrame =
				TransformMatrixStepper2USProbe4x4 * 
				mTransformMatrixPhantom2DRB4x4 *
				IntersectPosW12;

			vnl_vector<double> NWireEndinUSProbeFrame =
				TransformMatrixStepper2USProbe4x4 * 
				mTransformMatrixPhantom2DRB4x4 *
				IntersectPosW32;

			// The parallel wires position in US Probe frame 
			// Note: 
			// 1. Parallel wires share the same X, Y coordinates as the N-wire joints
			//    in the phantom (template) frame.
			// 2. The Z-axis of the N-wire joints is not used in the computing.

 			// Wire N1 corresponds to mNWireJointTopLayerBackWall 
			vnl_vector<double> NWireJointForN1InUSProbeFrame =
				TransformMatrixStepper2USProbe4x4 * 
				mTransformMatrixPhantom2DRB4x4 *
				IntersectPosW12; //any point of wire 1 of this layer

			// Wire N3 corresponds to mNWireJointTopLayerFrontWall
			vnl_vector<double> NWireJointForN3InUSProbeFrame =
				TransformMatrixStepper2USProbe4x4 * 
				mTransformMatrixPhantom2DRB4x4 *
				IntersectPosW32; //any point of wire 3 of this layer

			// Store into the list of positions in the US image frame
			mValidationPositionsInUSImageFrame.push_back( SegmentedPositionInUSImageFrame );

			// Store into the list of positions in the US probe frame
			mValidationPositionsInUSProbeFrame.push_back( PositionInUSProbeFrame );
			mValidationPositionsNWireStartInUSProbeFrame.push_back( NWireStartinUSProbeFrame );
			mValidationPositionsNWireEndInUSProbeFrame.push_back( NWireEndinUSProbeFrame );

      for (int i=0; i<2; i++)
      {
        // all the matrices are expected to have the same length, so we need to add each value 
        // as many times as many layer we have - this really have to be cleaned up
        if (Layer==0)
        {
          mValidationPositionsNWire1InUSImageFrame.push_back( N1SegmentedPositionInUSImageFrame );
          mValidationPositionsNWire3InUSImageFrame.push_back( N3SegmentedPositionInUSImageFrame );
          mValidationPositionsNWire1InUSProbeFrame.push_back( NWireJointForN1InUSProbeFrame );
          mValidationPositionsNWire3InUSProbeFrame.push_back( NWireJointForN3InUSProbeFrame );
        }
        else
        {
          mValidationPositionsNWire4InUSImageFrame.push_back( N4SegmentedPositionInUSImageFrame );
          mValidationPositionsNWire6InUSImageFrame.push_back( N6SegmentedPositionInUSImageFrame );
          mValidationPositionsNWire4InUSProbeFrame.push_back( NWireJointForN1InUSProbeFrame );
          mValidationPositionsNWire6InUSProbeFrame.push_back( NWireJointForN3InUSProbeFrame );
        }
      }

			// Store into the list of positions in the Phantom frame
			mValidationPositionsInPhantomFrame.push_back( PositionInTemplateFrame );
		}

		// The flag will be set when there is at least one set of validation positions
		// have been populated successfully.
		if( false == mAreValidationPositionsReady )
		{
			mAreValidationPositionsReady = true;
		}


	}
	catch(...)
	{
		std::cerr << "\n\n" << __FILE__ << "," << __LINE__ << "\n"
			<< ">>>>>>>> In " << mstrScope << "::Failed to add validation positions!!!  Throw up ...\n";

		throw;	
	}
}

//-----------------------------------------------------------------------------

void BrachyTRUSCalibrator::addValidationPositionsPerImage( 
	std::vector<vnl_vector_double> SegmentedDataPositionListPerImage, 
	const std::vector<double> TransformUSProbe2Tracker,
	const std::vector<double> TransformDRB2Tracker )
{
  static int frameIndex=-1;
  frameIndex++;

	if( mHasPhantomBeenRegistered != true )
	{
		std::cerr << "\n\n" << __FILE__ << "," << __LINE__ << "\n"
			<< ">>>>>>>> In " << mstrScope << "::The phantom is not yet registered to the DRB frame!!!  Throw up ...\n";

		throw;
	}

	if( mIsPhantomGeometryLoaded != true )
	{
		std::cerr << "\n\n" << __FILE__ << "," << __LINE__ << "\n"
			<< ">>>>>>>> In " << mstrScope << "::The phantom geometry has not been loaded!!!  Throw up ...\n";

		throw;
	}

	if( SegmentedDataPositionListPerImage.size() != mNUMREFPOINTSPERIMAGE )
	{
		std::cerr << "\n\n" << __FILE__ << "," << __LINE__ << "\n"
			<< ">>>>>>>> In " << mstrScope << "::The number of N-wires is NOT "
			<< mNUMREFPOINTSPERIMAGE << " in one US image as required!!!  Throw up ...\n";

		throw;
	}

	if( mHasUSImageFrameOriginBeenSet != true )
	{
		std::cerr << "\n\n" << __FILE__ << "," << __LINE__ << "\n"
			<< ">>>>>>>> In " << mstrScope << "::The ultrasound image frame origin "
			<< "has not been defined yet!!!  Throw up ...\n";

		throw;
	}

	try
	{
		// Obtain the transform matrix from DRB Frame to Tracker Frame 
		// when the ultrasound image is captured.
		vnl_matrix<double> TransformMatrixDRB2Tracker = 
			convertTransform2HomoMatrix( TransformDRB2Tracker );

		// Obtain the transform matrix from Tracker Frame to US Probe Frame
		// when the ultrasound image is captured.
		vnl_matrix_inverse<double> inverseMatrix( convertTransform2HomoMatrix( TransformUSProbe2Tracker ) );
		vnl_matrix<double> TransformMatrixTracker2USProbe = inverseMatrix.inverse();
		// Make sure the last row in homogeneous transform is [0 0 0 1]
		vnl_vector<double> lastRow(4,0);
		lastRow.put(3, 1);
		TransformMatrixTracker2USProbe.set_row(3, lastRow);

		// Finally, calculate the transform matrix from DRB Frame to the US probe Frame 
		vnl_matrix<double> TransformMatrixDRB2USProbe4x4 = 
			TransformMatrixTracker2USProbe * 
			TransformMatrixDRB2Tracker;

		// ========================================
		// Calculate then store the data positions 
		// ========================================
		// For BrachyTRUSCalibrator, row/col indices are of no interest
		// to us any more, because we only see two layers of N-wires with one
		// N-shape on each layer.  Therefore in each image only two phantom 
		// geometry indices are collected and the program knows their indices
		// in the pre-generated geometry.  This is a good start for automating 
		// the process since we are able to calculate alpha on-the-fly now
		// without requiring to specify the phantom indices one by one.
		//
		// IMPORTANT:
		// Indices defined in the input std::vector array.
		// This is the order that the segmentation algorithm gives the 
		// segmented positions in each image
		//
		// [ Array 0-2: Top N-wire Layer (Right-Middle-Left)]; 
		// [ Array 3-5: Bottom N-wire Layer (Right-Middle-Left)]
		// Each acquired data position is a 4x1 homogenous vector :
		// [ X, Y, 0, 1] all units in pixels
		// ==================================================================

		for( int Layer = 0; Layer < 2; Layer++ )
		{
			// ========================================================
			// The protocol is that the middle point collected in 
			// the set of three points of the N-wire is the data point.
			// ========================================================
            vnl_vector<double> SegmentedPositionInOriginalImageFrame =
				SegmentedDataPositionListPerImage.at( Layer*3 + 1 );

			// Convert the segmented image positions from the original 
			// image to the predefined ultrasound image frame.
			vnl_vector<double> SegmentedPositionInUSImageFrame =  
				mTransformOrigImageFrame2TRUSImageFrameMatrix4x4 * 
				SegmentedPositionInOriginalImageFrame;

			// OPTION: Apply the ultrasound 3D beamwidth to the validation dataset
			// ====================================================================
			// If Option 3 is selected, we will use the ultrasound 3D
			// beamwidth to filter out the validation data that has larger
			// beamwidth which are potentially unreliable than those dataset
			// with a smaller beamwidth.  If Option 1 or 2 is selected, the
			// beamwidth of the validation data will be tracked and recorded 
			// for analysis only (no filtering is performed).
			if( true == mIsUSBeamwidthAndWeightFactorsTableReady )
			{
				// Get the axial depth in the US Image Frame for the segmented data point
				// NOTE: the axial depth is in pixels and along the Y-axis of the frame.
				const double ThisAxialDepthInUSImageFrameOriginal = 
					SegmentedPositionInUSImageFrame.get(1);
				// Round the axial depth to one-pixel level (integer)
				const int ThisAxialDepthInUSImageFrameRounded = 
					floor( ThisAxialDepthInUSImageFrameOriginal + 0.5 );

				double USBeamWidthEuclideanMagAtThisAxialDepthInMM(-1);
				if( ThisAxialDepthInUSImageFrameRounded <= 
					mMinimumUSElevationBeamwidthAndFocalZoneInUSImageFrame.get(0) )
				{
					// #1. This is the ultrasound elevation near field which has the
					// the best imaging quality (before the elevation focal zone)
					// -------------------------------------------------------------

					// We will set the beamwidth to be the same as the elevation
					// focal zone.
					USBeamWidthEuclideanMagAtThisAxialDepthInMM = 
						mMinimumUSElevationBeamwidthAndFocalZoneInUSImageFrame.get(1);
				}
				else if( ThisAxialDepthInUSImageFrameRounded >= 
					mTheFarestAxialDepthInUSBeamwidthAndWeightTable )
				{
					// #2. Further deep in far field (close to the bottom of the image)
					// Ultrasound diverses quickly in this region and data quality deteriorates
					// ------------------------------------------------------------------------
					USBeamWidthEuclideanMagAtThisAxialDepthInMM =
						mUS3DBeamwidthAtFarestAxialDepth.magnitude();
				}
				else 
				{
					// #3. Ultrasound far field 
					// Here the sound starts to diverse with elevation beamwidth getting
					// larger and larger.  Data quality starts to deteriorate.
					// ------------------------------------------------------------------

					// Populate the beamwidth vector (axial, lateral and elevation elements)
					vnl_vector<double> US3DBeamwidthAtThisAxialDepth(3,0);
					US3DBeamwidthAtThisAxialDepth.put(0,
						mUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xM.get(1,
								ThisAxialDepthInUSImageFrameRounded -
								mTheNearestAxialDepthInUSBeamwidthAndWeightTable));
					US3DBeamwidthAtThisAxialDepth.put(1,
						mUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xM.get(2,
								ThisAxialDepthInUSImageFrameRounded -
								mTheNearestAxialDepthInUSBeamwidthAndWeightTable));
					US3DBeamwidthAtThisAxialDepth.put(2,
						mUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xM.get(3,
								ThisAxialDepthInUSImageFrameRounded -
								mTheNearestAxialDepthInUSBeamwidthAndWeightTable));

					USBeamWidthEuclideanMagAtThisAxialDepthInMM =
						US3DBeamwidthAtThisAxialDepth.magnitude();
				}
					
				if( 3 == mMethodToIncorporateBeamWidth && USBeamWidthEuclideanMagAtThisAxialDepthInMM >= 
					(mMinimumUSElevationBeamwidthAndFocalZoneInUSImageFrame.get(1)*mNUMOFTIMESOFMINBEAMWIDTH) )
				{
					continue;  // Ignore this data, jump to the next iteration of the for-loop
				}
				
				mUSBeamWidthEuclideanMagAtValidationPositions.push_back(
					USBeamWidthEuclideanMagAtThisAxialDepthInMM );
			}

			// Calcuate the alpha value
			// alpha = |CiXi|/|CiCi+1|
			vnl_vector<double> VectorCi2Xi = 
				SegmentedDataPositionListPerImage.at( Layer*3 + 1 ) -  
				SegmentedDataPositionListPerImage.at( Layer*3 );
			vnl_vector<double> VectorCi2Cii = 
				SegmentedDataPositionListPerImage.at( Layer*3 + 2 ) -  
				SegmentedDataPositionListPerImage.at( Layer*3 );
			double alpha = (double)VectorCi2Xi.magnitude()/VectorCi2Cii.magnitude();
            
			// ==============================================================
			// Apply alpha to Equation: Xi = Ai + alpha * (Bi - Ai)
			// where:
			// - Ai and Bi are the N-wire joints in either front or back walls.
			//
			// IMPORTANT:
			// - This is a very crucial point that would easily cause confusions
			//   or mistakes.  There is one and only one fixed correspondence between 
			//   the 6 segmented image positions (N-fiducials) and the wires.
			// - Closely examine the wiring design and set Ai and Bi accordingly.
            // ==============================================================


			vnl_vector<double> PositionInPhantomFrame(4);
			vnl_vector<double> IntersectPosW12(4);
			vnl_vector<double> IntersectPosW32(4);

			for (int i=0; i<3; ++i) {
				IntersectPosW12[i] = mNWires[Layer].intersectPosW12[i] / 1000.0;
				IntersectPosW32[i] = mNWires[Layer].intersectPosW32[i] / 1000.0;
			}
			IntersectPosW12[3] = 1.0;
			IntersectPosW32[3] = 1.0;

			PositionInPhantomFrame = IntersectPosW12 + alpha * ( IntersectPosW32 - IntersectPosW12 );

			// Finally, calculate the position in the US probe frame
			// X(US Probe) =  T(Tracker->US Probe)*T(DRB->Tracker)*T(Phantom->DRB)*X(Phantom)
			vnl_vector<double> PositionInUSProbeFrame =  
				TransformMatrixDRB2USProbe4x4 * 
				mTransformMatrixPhantom2DRB4x4 * 
				PositionInPhantomFrame;

			if( true == mIsSystemLogOn )
			{
				std::ofstream SystemLogFile(
					mSystemLogFileNameWithTimeStamp.c_str(), std::ios::app);
				SystemLogFile << " ==========================================================================\n";
				SystemLogFile << " ADD DATA FOR VALIDATION ("<<frameIndex<<") >>>>>>>>>>>>>>>>>>>>>\n\n";
				SystemLogFile << " SegmentedNFiducial-" << Layer*3 << " = " << SegmentedDataPositionListPerImage.at( Layer*3 ) << "\n"; 
				SystemLogFile << " SegmentedNFiducial-" << Layer*3+1 << " = " << SegmentedDataPositionListPerImage.at( Layer*3+1 ) << "\n";  
				SystemLogFile << " SegmentedNFiducial-" << Layer*3+2 << " = " << SegmentedDataPositionListPerImage.at( Layer*3+2 ) << "\n";  
				SystemLogFile << " ----------------------------------------------------------------\n";
				SystemLogFile << " SegmentedPositionInOriginalImageFrame = " << SegmentedPositionInOriginalImageFrame << "\n";
				SystemLogFile << " SegmentedPositionInUSImageFrame = " << SegmentedPositionInUSImageFrame << "\n";
				SystemLogFile << " alpha = " << alpha << "\n";
				SystemLogFile << " PositionInPhantomFrame = " << PositionInPhantomFrame << "\n";
				SystemLogFile << " TransformMatrixDRB2USProbe4x4 = \n" << TransformMatrixDRB2USProbe4x4 << "\n";
        SystemLogFile << " mTransformMatrixPhantom2DRB4x4 = \n" << mTransformMatrixPhantom2DRB4x4 << "\n";
        SystemLogFile << " PositionInUSProbeFrame = " << PositionInUSProbeFrame << "\n";
				SystemLogFile.close();
			}

			vnl_vector<double> NWireStartinUSProbeFrame =
				TransformMatrixDRB2USProbe4x4 * 
				mTransformMatrixPhantom2DRB4x4 * 
				IntersectPosW12;

			vnl_vector<double> NWireEndinUSProbeFrame =
				TransformMatrixDRB2USProbe4x4 * 
				mTransformMatrixPhantom2DRB4x4 * 
				IntersectPosW32;

			// Store into the list of positions in the US image frame
			mValidationPositionsInUSImageFrame.push_back( SegmentedPositionInUSImageFrame );

			// Store into the list of positions in the US probe frame
			mValidationPositionsInUSProbeFrame.push_back( PositionInUSProbeFrame );
			mValidationPositionsNWireStartInUSProbeFrame.push_back( IntersectPosW12 );
			mValidationPositionsNWireEndInUSProbeFrame.push_back( IntersectPosW32 );

			// Store into the list of positions in the Phantom frame
			mValidationPositionsInPhantomFrame.push_back( PositionInPhantomFrame );
		}

		// The flag will be set when there is at least one set of validation positions
		// have been populated successfully.
		if( false == mAreValidationPositionsReady )
		{
			mAreValidationPositionsReady = true;
		}
	}
	catch(...)
	{
		std::cerr << "\n\n" << __FILE__ << "," << __LINE__ << "\n"
			<< ">>>>>>>> In " << mstrScope << "::Failed to add validation positions!!!  Throw up ...\n";

		throw;	
	}
}

//-----------------------------------------------------------------------------

std::vector<double> BrachyTRUSCalibrator::getPRE3DforRealtimeImage(
    std::vector<vnl_vector_double> SegmentedDataPositionListPerImage, 
	const vnl_matrix<double> TransformMatrixUSProbe2DRB4x4 ) const
{
	if( mHasPhantomBeenRegistered != true )
	{
		std::cerr << "\n\n" << __FILE__ << "," << __LINE__ << "\n"
		<< ">>>>>>>> In " << mstrScope << "::The phantom is not yet registered to the DRB frame!!!  Throw up ...\n";

		throw;
	}
	
	if( mHasBeenCalibrated != true )
	{
		std::cerr << "\n\n" << __FILE__ << "," << __LINE__ << "\n"
			<< ">>>>>>>> In " << mstrScope << "::This operation is not possible since the calibration is not yet finished!!!  Throw up ...\n";

		throw;
	}
	
	if( mHasUSImageFrameOriginBeenSet != true )
	{
		std::cerr << "\n\n" << __FILE__ << "," << __LINE__ << "\n"
			<< ">>>>>>>> In " << mstrScope << "::The ultrasound image frame origin "
			<< "has not been defined yet!!!  Throw up ...\n";

		throw;
	}

	try
	{
		// STEP-1. Populate the position data
		// ===================================

		// Data containers
		std::vector<vnl_vector_double> DataPositionsInUSImageFrame;
		std::vector<vnl_vector_double> DataPositionsInPhantomFrame;
		std::vector<vnl_vector_double> DataPositionsInUSProbeFrame;

		// Obtain the transform matrix from DRB Frame to the US probe Frame 
		vnl_matrix_inverse<double> inverseMatrix( TransformMatrixUSProbe2DRB4x4 );
		vnl_matrix<double> TransformMatrixDRB2USProbe4x4 = inverseMatrix.inverse();
		// Make sure the last row in homogeneous transform is [0 0 0 1]
		vnl_vector<double> lastRow(4,0);
		lastRow.put(3, 1);
		TransformMatrixDRB2USProbe4x4.set_row(3, lastRow);

		// ========================================
		// Calculate then store the data positions 
		// ========================================
		// For BrachyTRUSCalibrator, row/col indices are of no interest
		// to us any more, because we only see two layers of N-wires with one
		// N-shape on each layer.  Therefore in each image only two phantom 
		// geometry indices are collected and the program knows their indices
		// in the pre-generated geometry.  This is a good start for automating 
		// the process since we are able to calculate alpha on-the-fly now
		// without requiring to specify the phantom indices one by one.
		//
		// IMPORTANT:
		// Indices defined in the input std::vector array.
		// This is the order that the segmentation algorithm gives the 
		// segmented positions in each image
		//
		// [ Array 0-2: Top N-wire Layer (Right-Middle-Left)]; 
		// [ Array 3-5: Bottom N-wire Layer (Right-Middle-Left)]
		// Each acquired data position is a 4x1 homogenous vector :
		// [ X, Y, 0, 1] all units in pixels
		// ==================================================================

		for( int Layer = 0; Layer < 2; Layer++ )
		{
			// ========================================================
			// The protocol is that the middle point collected in 
			// the set of three points of the N-wire is the data point.
			// ========================================================
			vnl_vector<double> SegmentedPositionInOriginalImageFrame( 
				SegmentedDataPositionListPerImage.at( Layer*3 + 1 ) );

			// Convert the segmented image positions from the original 
			// image to the predefined ultrasound image frame.
			vnl_vector<double> SegmentedPositionInUSImageFrame =  
				mTransformOrigImageFrame2TRUSImageFrameMatrix4x4 * 
				SegmentedPositionInOriginalImageFrame;

			DataPositionsInUSImageFrame.push_back( SegmentedPositionInUSImageFrame );

			// Calcuate the alpha value
			// alpha = |CiXi|/|CiCi+1|
			vnl_vector<double> VectorCi2Xi = 
				SegmentedDataPositionListPerImage.at( Layer*3 + 1 ) -  
				SegmentedDataPositionListPerImage.at( Layer*3 );
			vnl_vector<double> VectorCi2Cii = 
				SegmentedDataPositionListPerImage.at( Layer*3 + 2 ) -  
				SegmentedDataPositionListPerImage.at( Layer*3 );
			double alpha = (double)VectorCi2Xi.magnitude()/VectorCi2Cii.magnitude();
            
			// ==============================================================
			// Apply alpha to Equation: Xi = Ai + alpha * (Bi - Ai)
			// where:
			// - Ai and Bi are the N-wire joints in either front or back walls.
			//
			// IMPORTANT:
			// - This is a very crucial point that would easily cause confusions
			//   or mistakes.  There is one and only one fixed correspondence between 
			//   the 6 segmented image positions (N-fiducials) and the wires.
			// - Closely examine the wiring design and set Ai and Bi accordingly.
            // ==============================================================


			vnl_vector<double> PositionInPhantomFrame(4);
			vnl_vector<double> IntersectPosW12(4);
			vnl_vector<double> IntersectPosW32(4);

			// NWire joints that need to be saved to compute the PLDE (Point-Line Distance Error) 
			// in addition to the real-time PRE3D.
			vnl_vector<double> NWireStartinPhantomFrame;
			vnl_vector<double> NWireEndinPhantomFrame;

			for (int i=0; i<3; ++i) {
				IntersectPosW12[i] = mNWires[Layer].intersectPosW12[i] / 1000.0;
				IntersectPosW32[i] = mNWires[Layer].intersectPosW32[i] / 1000.0;
			}
			IntersectPosW12[3] = 1.0;
			IntersectPosW32[3] = 1.0;

			PositionInPhantomFrame = IntersectPosW12 + alpha * ( IntersectPosW32 - IntersectPosW12 );

			// Finally, calculate the position in the US probe frame
			// X(US Probe) =  T(Tracker->US Probe)*T(DRB->Tracker)*T(Phantom->DRB)*X(Phantom)
			vnl_vector<double> PositionInUSProbeFrame =  
				TransformMatrixDRB2USProbe4x4 * 
				mTransformMatrixPhantom2DRB4x4 * 
				PositionInPhantomFrame;

			if( true == mIsSystemLogOn )
			{
				std::ofstream SystemLogFile(
					mSystemLogFileNameWithTimeStamp.c_str(), std::ios::app);
				SystemLogFile << " ==========================================================================\n";
				SystemLogFile << " ADD DATA FOR CALIBRATION >>>>>>>>>>>>>>>>>>>>>\n\n";
				SystemLogFile << " SegmentedNFiducial-" << Layer*3 << " = " << SegmentedDataPositionListPerImage.at( Layer*3 ) << "\n"; 
				SystemLogFile << " SegmentedNFiducial-" << Layer*3+1 << " = " << SegmentedDataPositionListPerImage.at( Layer*3+1 ) << "\n";  
				SystemLogFile << " SegmentedNFiducial-" << Layer*3+2 << " = " << SegmentedDataPositionListPerImage.at( Layer*3+2 ) << "\n";  
				SystemLogFile << " ----------------------------------------------------------------\n";
				SystemLogFile << " SegmentedPositionInOriginalImageFrame = " << SegmentedPositionInOriginalImageFrame << "\n";
				SystemLogFile << " SegmentedPositionInUSImageFrame = " << SegmentedPositionInUSImageFrame << "\n";
				SystemLogFile << " alpha = " << alpha << "\n";
				SystemLogFile << " PositionInPhantomFrame = " << PositionInPhantomFrame << "\n";
				SystemLogFile << " TransformMatrixDRB2USProbe4x4 = \n" << TransformMatrixDRB2USProbe4x4 << "\n";
        SystemLogFile << " mTransformMatrixPhantom2DRB4x4 = \n" << mTransformMatrixPhantom2DRB4x4 << "\n";
        SystemLogFile << " PositionInUSProbeFrame = " << PositionInUSProbeFrame << "\n";
				SystemLogFile.close();
			}

			// Store into the list of positions in the Phantom frame
			DataPositionsInPhantomFrame.push_back( PositionInPhantomFrame );

			// Store into the list of positions in the US probe frame
			DataPositionsInUSProbeFrame.push_back( PositionInUSProbeFrame );
		}

		// STEP-2.  Calculate 3D Point Reconstruction Error (PRE3D)
		// ========================================================

		std::vector<vnl_vector_double> PRE3DsInUSProbeFrameABS;
		std::vector<vnl_vector_double> ProjectedPositionsInUSImageFrame;

		// To get the projected positions using the calibration matrix in
		// the image frame, we need an inverse of the calibration matrix.
		// However, since the 3rd column of the calibration matrix is all 
		// zeros (the US image frame does not have a 3rd z-component) 
		// thus non-singular, we will generate a 3rd vector from cross-
		// product of the 1st and 2nd column to make the matrix singular 
		// just that we could make an inverse out of it.
		// NOTE:
		// - We will normalize the column vectors in the rotation part (3x3) 
		//   of the input matrix to elimate the impact of the scaling factor.  
		// - We don't really care about the scale factor in the 3rd column (or
		//   the Z-axis), since we do not have data to evaluate that. So we 
		//   will use 1 (or normalized) for the sake of calculation.
		// - The translation part (the 4th column) remains untouched.
		vnl_vector<double> Column1st(3,0);
		vnl_vector<double> Column2nd(3,0);
		Column1st.put(0, mTransformUSImageFrame2USProbeFrameMatrix4x4.get_column(0).get(0));
		Column1st.put(1, mTransformUSImageFrame2USProbeFrameMatrix4x4.get_column(0).get(1));
		Column1st.put(2, mTransformUSImageFrame2USProbeFrameMatrix4x4.get_column(0).get(2));
		Column2nd.put(0, mTransformUSImageFrame2USProbeFrameMatrix4x4.get_column(1).get(0));
		Column2nd.put(1, mTransformUSImageFrame2USProbeFrameMatrix4x4.get_column(1).get(1));
		Column2nd.put(2, mTransformUSImageFrame2USProbeFrameMatrix4x4.get_column(1).get(2));
		vnl_vector<double> Column3rdNormalized(4,0);
		Column3rdNormalized.put(0, vnl_cross_3d(Column1st, Column2nd).normalize().get(0));
		Column3rdNormalized.put(1, vnl_cross_3d(Column1st, Column2nd).normalize().get(1));
		Column3rdNormalized.put(2, vnl_cross_3d(Column1st, Column2nd).normalize().get(2));
		Column3rdNormalized.put(3, 0);
		vnl_matrix<double> CreatedTransformMatrixUSProbeFame2USImageFrame4x4 =
			mTransformUSImageFrame2USProbeFrameMatrix4x4;
		CreatedTransformMatrixUSProbeFame2USImageFrame4x4.set_column(2, Column3rdNormalized);
		
		// Apply the calibration result to the data positions
		for( int Layer = 0; Layer < 2; Layer++ )
		{
			// 1. The projected position in the US probe frame
			//    after applying the calibration matrix
			// ------------------------------------------------
			vnl_vector<double> ProjectedPositionInUSProbeFrame = 
				mTransformUSImageFrame2USProbeFrameMatrix4x4 * 
				DataPositionsInUSImageFrame.at(Layer);

			// PRE3D_USProbeFrame = (projected position - true position) in US probe frame.
			vnl_vector<double> PRE3DInUSProbeFrameProjected2True = 			
				ProjectedPositionInUSProbeFrame - DataPositionsInUSProbeFrame.at(Layer);
			// Take the absolute value of PRE3D in each axis
			vnl_vector<double> PRE3DInUSProbeFrameABS(4,0);
			PRE3DInUSProbeFrameABS.put(0, fabs( PRE3DInUSProbeFrameProjected2True.get(0) ));
			PRE3DInUSProbeFrameABS.put(1, fabs( PRE3DInUSProbeFrameProjected2True.get(1) ));
			PRE3DInUSProbeFrameABS.put(2, fabs( PRE3DInUSProbeFrameProjected2True.get(2) ));
			PRE3DInUSProbeFrameABS.put(3, 0 );

			PRE3DsInUSProbeFrameABS.push_back( PRE3DInUSProbeFrameABS );

			// 2. The projected position in the US image frame
			//    after applying the calibration matrix
			// ------------------------------------------------
			vnl_vector<double> ProjectedPositionInUSImageFrame = 
				CreatedTransformMatrixUSProbeFame2USImageFrame4x4 * 
				DataPositionsInUSProbeFrame.at( Layer );
			ProjectedPositionsInUSImageFrame.push_back( ProjectedPositionInUSImageFrame );
		}

		// STEP-3. Generate the returning data container
		// ==============================================
		// FORMAT: PRE3Ds are averaged for all the data positions in the image and 
		// given in the following format:
		// [vector 0-2: Averaged PRE3D (x, y, z) in the US probe frame in X,Y and Z axis ]
		// [vector 3-6: Back-projected positions in the US image frame, X1, Y1, X2, Y2 ]

		std::vector<double> ReturningDataContainer;

		// Averaged PRE3D in the US probe frame (x, y, z, 0)
		vnl_vector<double> AvgPRE3DinUSProbeFrameABS = 0.5 *
			( PRE3DsInUSProbeFrameABS.at(0) + PRE3DsInUSProbeFrameABS.at(1) );
		ReturningDataContainer.push_back( AvgPRE3DinUSProbeFrameABS.get(0) );
		ReturningDataContainer.push_back( AvgPRE3DinUSProbeFrameABS.get(1) );
		ReturningDataContainer.push_back( AvgPRE3DinUSProbeFrameABS.get(2) );

		// Back-projected positions in the US image frame (X1, Y1, X2, Y2)
		ReturningDataContainer.push_back( ProjectedPositionsInUSImageFrame.at(0).get(0) );
		ReturningDataContainer.push_back( ProjectedPositionsInUSImageFrame.at(0).get(1) );
		ReturningDataContainer.push_back( ProjectedPositionsInUSImageFrame.at(1).get(0) );
		ReturningDataContainer.push_back( ProjectedPositionsInUSImageFrame.at(1).get(1) );

		return ReturningDataContainer;

	}
	catch(...)
	{
		std::cerr << "\n\n" << __FILE__ << "," << __LINE__ << "\n"
			<< ">>>>>>>> In " << mstrScope << "::Failed to obtain PRE3Ds!!!  Throw up ...\n";

		throw;	
	}
}

// END OF FILE
