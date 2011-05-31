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

#include <float.h>

// VNL Includes
#include "vnl/algo/vnl_matrix_inverse.h"
#include "vcl_istream.h"

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

BrachyTRUSCalibrator::BrachyTRUSCalibrator( const bool IsSystemLogOn )
	: Phantom( IsSystemLogOn )	// Call the parent's constructor
{
	try
	{
		for( int i = 0; i < 5; i++ )
			for( int j = 0; j < 5; j++ )
		{
			mPhantomGeometryOnFrontInnerWall[i][j].set_size(4);
			mPhantomGeometryOnBackInnerWall[i][j].set_size(4);
		}

		mNWireJointTopLayerBackWall.set_size(4);
		mNWireJointTopLayerFrontWall.set_size(4);
		mNWireJointBottomLayerBackWall.set_size(4);
		mNWireJointBottomLayerFrontWall.set_size(4);

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

		// Load the phantom-specfic geometry
		loadGeometry ();
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

void BrachyTRUSCalibrator::loadGeometry()
{
	try
	{
		if( true == mIsPhantomGeometryLoaded )
		{
			mIsPhantomGeometryLoaded = false;
		}
		
		// ========================================================
		// General Terminologies used in the phantom design
		// ========================================================
		// Visit the Solid Edge CAD model of the phantom for details.
		//
		// - Top:		the upper, sealed side of the phantom box, marked
		//				by the 3D-printed triangle pointing up;
		// - Bottom:	the lower, open side of the phantom box;
		// - Front:		the face that has the brachy template;
		// - Back:		the opposite side to the front face.
		// ========================================================

		// ========================================================
		// PART-1 The phantom geometry of The BrachyTRUSCalibrator
		// ========================================================
		// 1. This geometry is all kept w.r.t the brachy template frame;
		// 2. The definition of the BrachyTRUSCalibrator frame (see SolidEdge design
		//    CAD specs: DoubleN-EG-Phantom-SolidEdge_AssembledAllParts_v1.0.asm).
		//    The template is the BMSI design. The coordinate system is defined as:  
		//    - Left/Right is x;
		//    - Up/Down is y; 
		//    - z is toward the patient.
		//    - Origin: the top left hole, on the front face (labeled as A7).
		// 3. There are a total of 50 drilled holes in the calibrator (25 on front plate
		//    and 25 on back plate).
		// 4. All the geometry is defined for the inner-walls of the wire mount because
		//    these are what matters to us to calculate the N-wire joints.
		// 5. The holes in the front/back inner walls are marked as: Fij/Bij, where i,j
		//    are the indices of rows and columns in a top-down, left-right matter.
		// 6. The precise location of each holes on the inner walls can be calculated as:
		//    - Front Wall Index Hole: the top-left hole on the front inner wall;
		//    - Back Wall Index Hole: the top-left hole on the back inner wall.
		//    - A constant step size on both X and Y direction;
		//    - A constant depth for front and back inner wall (Z-axis).
		// 7. The hole positions are used to calculate their joints of the N-wires.
		// 8. All units are in meters; all positions are in 4x1 homogeneous coordinates.
		// ========================================================

		// The precise location of the front/back wall index holes.
		// NOTE: there is a distance of 80mm between the front and back inner walls.
		double const FrontIndexHole[] = {9.86*0.001, 14.385*0.001, 62.05*0.001, 1};
		vnl_vector<double> FrontIndexHoleVector(4);
		FrontIndexHoleVector.set( FrontIndexHole );

		double const BackIndexHole[] = {9.86*0.001, 14.385*0.001, 142.05*0.001, 1};
		vnl_vector<double> BackIndexHoleVector(4);
		BackIndexHoleVector.set( BackIndexHole );
        
		// The precise distance/step between the holes.
		double const HoleXStep[] = {10*0.001, 0, 0, 0}; 
		vnl_vector<double> HoleXStepVector(4);
		HoleXStepVector.set( HoleXStep );
		double const HoleYStep[] = {0, 10*0.001, 0, 0};
		vnl_vector<double> HoleYStepVector(4);
		HoleYStepVector.set( HoleYStep );

		// N-wire's Start and End Positions in the Front and Back Inner Walls
		// Populate the computed locations of the hole matrices
		// based on the index holes and steps.
		// There are 5x5 matrix of holes on both front and back inner walls
		// [FORMAT: Front/Back [i][j], i & j are rows/columns of the hole matrix]
		for( int i = 0; i < 5; i++ ) // i: row of matrix from top to down
			for( int j = 0; j < 5; j++ ) // j: column of matrix from left to right
			{
				mPhantomGeometryOnFrontInnerWall[i][j] = 
					FrontIndexHoleVector + HoleYStepVector*i + HoleXStepVector*j;

				mPhantomGeometryOnBackInnerWall[i][j] = 
					BackIndexHoleVector + HoleYStepVector*i + HoleXStepVector*j;
			}
		
		// =================================
		// PART-2 The Joints of the N-wires
		// =================================
		// 1. This section calculates the joints between two wires in 
		//    the N-wire geometry, i.e., NWireJointAB
		// 2. The wire joints are in turn used for computation in the
		//    similar triangles of each N-wire geometry
		// 3. Since there are only two layers of the N-wires are 
		//    deployed, only those joints related are computed here.
		// 4. NWireJointAB and NwireJointBA should be theoretically the 
		//    same, except NWireJointAB is calculated using NWire-A while 
		//    NWireJointBA is calculated using NWire-B.
		//
		// Important Note:
		// - These N-wire joints should be exactly corresponding to 
		//   the wire locations in the phantom (one to one fixation).
		// ========================================================

		// Current Setup (Wed Mar 10 11:06 EST 2010):
		// - We used the 3rd (ROW[2]) and 5th rows (ROW[4]) in Part-1 for wiring.
		// - Check the phantom to see where the wire joints are.
		
		// Joint of N-wires at 
		// [Top Layer] toward [Front Wall]
		double alphaTopLayerFrontWall =  
			(mPhantomGeometryOnFrontInnerWall[2][0] - mPhantomGeometryOnFrontInnerWall[2][1]).magnitude()/
			(mPhantomGeometryOnBackInnerWall[2][0] - mPhantomGeometryOnBackInnerWall[2][3]).magnitude();
		mNWireJointTopLayerFrontWall = 
			(1/(1-alphaTopLayerFrontWall))*mPhantomGeometryOnFrontInnerWall[2][0] -
			(alphaTopLayerFrontWall/(1-alphaTopLayerFrontWall))* mPhantomGeometryOnBackInnerWall[2][0];

		// Joint of N-wires at 
		// [Top Layer] toward [Back Wall]
		double alphaTopLayerBackWall =  
			(mPhantomGeometryOnBackInnerWall[2][3] - mPhantomGeometryOnBackInnerWall[2][4]).magnitude()/
			(mPhantomGeometryOnFrontInnerWall[2][1] - mPhantomGeometryOnFrontInnerWall[2][4]).magnitude();
		mNWireJointTopLayerBackWall = 
			(1/(1-alphaTopLayerBackWall))*mPhantomGeometryOnBackInnerWall[2][4] -
			(alphaTopLayerBackWall/(1-alphaTopLayerBackWall))* mPhantomGeometryOnFrontInnerWall[2][4];

		// Joint of N-wires at
		// [Bottom Layer] toward [Front Wall]
		double alphaBottomLayerFrontWall =  
			(mPhantomGeometryOnFrontInnerWall[4][3] - mPhantomGeometryOnFrontInnerWall[4][4]).magnitude()/
			(mPhantomGeometryOnBackInnerWall[4][1] - mPhantomGeometryOnBackInnerWall[4][4]).magnitude();
		mNWireJointBottomLayerFrontWall = 
			(1/(1-alphaBottomLayerFrontWall))*mPhantomGeometryOnFrontInnerWall[4][4] -
			(alphaBottomLayerFrontWall/(1-alphaBottomLayerFrontWall))* mPhantomGeometryOnBackInnerWall[4][4];

		// Joint of N-wires at
		// [Bottom Layer] toward [Back Wall]
		double alphaBottomLayerBackWall =  
			(mPhantomGeometryOnBackInnerWall[4][0] - mPhantomGeometryOnBackInnerWall[4][1]).magnitude()/
			(mPhantomGeometryOnFrontInnerWall[4][0] - mPhantomGeometryOnFrontInnerWall[4][3]).magnitude();
		mNWireJointBottomLayerBackWall = 
			(1/(1-alphaBottomLayerBackWall))*mPhantomGeometryOnBackInnerWall[4][0] -
			(alphaBottomLayerBackWall/(1-alphaBottomLayerBackWall))* mPhantomGeometryOnFrontInnerWall[4][0];


		// PART-2. The phantom-specific reference points on BrachyTRUSCalibrator
		// ======================================================================

		// NOTE: this is NOT in use currently.  Rather, it serves as an backup
		// plan in the event if we need to use an optical tracking system (e.g.,
		// NDI Certus) to locate the phantom's geometry with a stylus probe.
		// At the moment, the 3D print of the calibration phantom design is very
		// accurate, so no need for stylus probing at all.  - TKC, Mar-10-2010.

		// 1. These are fixed physical positions measurable using a Stylus probe.
		// 2. All positions are all kept w.r.t the phantom frame as defined above.
		// 3. They are used to register the phantom geomtry from the phantom 
		//	  frame to the DRB reference frame to be mounted on the calibrator.
		// 4. There are totally 8 reference points (4 on each plate).  See the marking
		//    on the physical calibrator surfaces for their IDs. 
		// 5. All units are in meters.

		// fill in the reference position indices (for stylus)
		double ref0[] = {-0.020, -0.02475, -0.024, 1};
		double ref1[] = { 0.020, -0.02475, -0.024, 1};
		double ref2[] = {-0.020, -0.02475, -0.090, 1};
		double ref3[] = { 0.020, -0.02475, -0.080, 1};
		double ref4[] = {-0.020,  0.02475, -0.024, 1};
		double ref5[] = { 0.020,  0.02475, -0.024, 1};
		double ref6[] = {-0.020,  0.02475, -0.090, 1};
		double ref7[] = { 0.020,  0.02475, -0.080, 1};
		mPhantomSpecificReferencePoints[0].set( ref0 );
		mPhantomSpecificReferencePoints[1].set( ref1 );
		mPhantomSpecificReferencePoints[2].set( ref2 );
		mPhantomSpecificReferencePoints[3].set( ref3 );
		mPhantomSpecificReferencePoints[4].set( ref4 );
		mPhantomSpecificReferencePoints[5].set( ref5 );
		mPhantomSpecificReferencePoints[6].set( ref6 );
		mPhantomSpecificReferencePoints[7].set( ref7 );

		// PART-3. The name list of the phantom-specific reference points
		// ===============================================================
		
		// There are totally 8 reference points (4 on each plate). These are the
		// IDs given to each of the points (see the marking on the phantom).
		std::string MarksName[8] = { "R1", "R2", "R3", "R4", "R5", "R6", "R7", "R8"};
		
		mNamesOfPhantomSpecificReferencePoints.resize(0);
		for( int i = 0; i < 8; i++ )
		{
			mNamesOfPhantomSpecificReferencePoints.push_back( MarksName[i] );
		}

		// Set the flag
		mIsPhantomGeometryLoaded = true;

		// Log the data pipeline if requested.
		if( true == mIsSystemLogOn )
		{
			std::ofstream SystemLogFile(
				mSystemLogFileNameWithTimeStamp.c_str(), std::ios::app);
			SystemLogFile << " ==========================================================================\n";
			SystemLogFile << " PHANTOM GEOMETRY >>>>>>>>>>>>>>>>>>>>>\n\n";
			SystemLogFile << "\n NWires Start/End Positions in the DRB frame [0-5] = \n";
			for(int i = 0; i < 5; i++ )
				for(int j = 0; j < 5; j++ )
			{
				SystemLogFile << "\tFrontWallGeometry [" << i <<"]["<< j <<"]= "<< mPhantomGeometryOnFrontInnerWall[i][j] <<"\n";
				SystemLogFile << "\tBackWallGeometry  [" << i <<"]["<< j <<"]= "<< mPhantomGeometryOnBackInnerWall[i][j] <<"\n";
			}
			SystemLogFile << " -----------------------------------------------------------------------------------\n";
			SystemLogFile << "\n Alpha ratio between NWires = \n\n";
			SystemLogFile << "\t alphaTopLayerBackWall \t= " << alphaTopLayerBackWall << "\n";
			SystemLogFile << "\t alphaTopLayerFrontWall \t= " << alphaTopLayerFrontWall << "\n";
			SystemLogFile << "\t alphaBottomLayerBackWall \t= " << alphaBottomLayerBackWall << "\n";
			SystemLogFile << "\t alphaBottomLayerFrontWall \t= " << alphaBottomLayerFrontWall << "\n";

			SystemLogFile << " -----------------------------------------------------------------------------------\n";
			SystemLogFile << "\n Joints of N-wires in the Phantom frame = \n\n";
			SystemLogFile << "\t mNWireJointTopLayerFrontWall(used) \t= " << mNWireJointTopLayerFrontWall << "\n\n";
			SystemLogFile << "\t mNWireJointTopLayerBackWall(used) \t= " << mNWireJointTopLayerBackWall << "\n\n";
			SystemLogFile << "\t mNWireJointBottomLayerFrontWall(used) \t= " << mNWireJointBottomLayerFrontWall << "\n\n";
			SystemLogFile << "\t mNWireJointBottomLayerBackWall(used) \t= " << mNWireJointBottomLayerBackWall << "\n\n";
			SystemLogFile << " -----------------------------------------------------------------------------------\n";
			SystemLogFile << "\n Phantom Specified Reference Points (x, y, z, 1) = \n";
			SystemLogFile << " (Used to register the Phantom to the DRB frame)\n\n";
			for( int i = 0; i < 8; i++ )
			{
				SystemLogFile << "\t" << mNamesOfPhantomSpecificReferencePoints[i] 
					<< " = " << mPhantomSpecificReferencePoints[i] << "\n";
			}
			SystemLogFile << "\n";
			SystemLogFile.close();
		}
	}
	catch(...)
	{
		std::cerr << "\n\n" << __FILE__ << "," << __LINE__ << "\n"
			<< ">>>>>>>> In " << mstrScope << "::Unable to load the phantom geometry!!!  Throw up ...\n";

		throw;
	}
}

//-----------------------------------------------------------------------------

bool BrachyTRUSCalibrator::loadGeometry(SegmentationParameters* aSegmentationParameters)
{
	if (mIsPhantomGeometryLoaded == true) {
		mIsPhantomGeometryLoaded = false;
	}

	if( true == mIsSystemLogOn )
	{
		std::ofstream SystemLogFile(
			mSystemLogFileNameWithTimeStamp.c_str(), std::ios::app);
		SystemLogFile << " ==========================================================================\n";
		SystemLogFile << " PHANTOM GEOMETRY >>>>>>>>>>>>>>>>>>>>>\n\n";
		SystemLogFile.close();
	}

	// Read input NWires and convert them to vnl vectors to easier processing
	std::vector<NWire> nWires = aSegmentationParameters->mNWires;
	std::vector<std::vector<vnl_vector<double>>> vnl_NWires;

	if (mIsSystemLogOn == true) {
		std::ofstream SystemLogFile(
			mSystemLogFileNameWithTimeStamp.c_str(), std::ios::app);
		SystemLogFile << " -----------------------------------------------------------------------------------\n";
		SystemLogFile << "\n Endpoints of wires = \n\n";
	}

	int layer = -1;
	std::vector<NWire>::iterator it;
	for (layer = 0, it = nWires.begin(); it != nWires.end(); ++it, ++layer) {
		std::vector<vnl_vector<double>> vnl_NWire;

		for (int i=0; i<3; ++i) {
			vnl_vector<double> endPointFront(4);
			vnl_vector<double> endPointBack(4);
		
			for (int j=0; j<3; ++j) {
				endPointFront[j] = it->wires[i].endPointFront[j] / 1000.0; //TODO this is meter
				endPointBack[j] = it->wires[i].endPointBack[j] / 1000.0;
			}
			// Insert front first then back (so vector will be like {front0, back0, front1, back1, front2, back2}
			endPointFront[3] = 1.0;
			endPointBack[3] = 1.0;

			vnl_NWire.push_back(endPointFront);
			vnl_NWire.push_back(endPointBack);

			if (mIsSystemLogOn == true) {
				std::ofstream SystemLogFile(
					mSystemLogFileNameWithTimeStamp.c_str(), std::ios::app);
				SystemLogFile << "\t Front endpoint of wire " << i << " on layer " << layer << " = " << endPointFront << "\n";
				SystemLogFile << "\t Back endpoint of wire " << i << " on layer " << layer << " = " << endPointBack << "\n";
			}
		}

		vnl_NWires.push_back(vnl_NWire);
	}

	if (mIsSystemLogOn == true) {
		std::ofstream SystemLogFile(
			mSystemLogFileNameWithTimeStamp.c_str(), std::ios::app);
		SystemLogFile.close();
	}


	double alphaTopLayerFrontWall = -1.0;
	double alphaTopLayerBackWall = -1.0;
	double alphaBottomLayerFrontWall = -1.0;
	double alphaBottomLayerBackWall = -1.0;

	// Calculate wire joints
	std::vector<std::vector<vnl_vector<double>>>::iterator itNWire;
	for (layer = 0, itNWire = vnl_NWires.begin(); itNWire != vnl_NWires.end(); ++itNWire, ++layer) {
		// Find the diagonal wire for the NWire
		int diagonal = -1;

		// Convert 4D vectors to 3D to be able to compute cross product (for determining parallelism)
		std::vector<vnl_vector<double>> endPoints3D;
		for (int i=0; i<6; ++i) {
			vnl_vector<double> endPoint(3);
			for (int j=0; j<3; ++j) {
				endPoint[j] = itNWire->at(i)[j];
			}

			endPoints3D.push_back(endPoint);
		}

		if (vnl_cross_3d(endPoints3D[0] - endPoints3D[1], endPoints3D[2] - endPoints3D[3]).is_zero()) {
			diagonal = 2;
		} else if (vnl_cross_3d(endPoints3D[0] - endPoints3D[1], endPoints3D[4] - endPoints3D[5]).is_zero()) {
			diagonal = 1;
		} else if (vnl_cross_3d(endPoints3D[2] - endPoints3D[3], endPoints3D[4] - endPoints3D[5]).is_zero()) {
			diagonal = 0;
		} else {
			return false; // no diagonal found (there were no parallel wires in an NWire)! //TODO proper error message
		}

		// Determine wire that has a joint with the diagonal wire in front and the wire that has with the back
		int jointFront = -1;
		int jointBack = -1;
		double minHoleDistanceFront = FLT_MAX;

		for (int i=0; i<3; ++i) {
			if (i == diagonal) {
				continue;
			}

			if ((itNWire->at(diagonal*2) - itNWire->at(i*2)).magnitude() < minHoleDistanceFront) {
				minHoleDistanceFront = (itNWire->at(diagonal*2) - itNWire->at(i*2)).magnitude();
				jointFront = i;
			}
		}

		// Determine wire that has a joint with the diagonal wire in back
		jointBack = 5 - (diagonal+1) - (jointFront+1);

//TODO Tomitol megkerdezni h melyik a felso es melyik az also!!!
		if (layer == 1) { // top
			alphaTopLayerFrontWall =
				(itNWire->at(jointFront*2) - itNWire->at(diagonal*2)).magnitude() / 
				(itNWire->at(jointFront*2+1) - itNWire->at(diagonal*2+1)).magnitude();
			mNWireJointTopLayerFrontWall = 
				(1/(1-alphaTopLayerFrontWall)) * itNWire->at(jointFront*2) -
				(alphaTopLayerFrontWall/(1-alphaTopLayerFrontWall)) * itNWire->at(jointFront*2+1);

			alphaTopLayerBackWall =
				(itNWire->at(jointBack*2+1) - itNWire->at(diagonal*2+1)).magnitude() / 
				(itNWire->at(jointBack*2) - itNWire->at(diagonal*2)).magnitude();
			mNWireJointTopLayerBackWall = 
				(1/(1-alphaTopLayerBackWall)) * itNWire->at(jointBack*2+1) -
				(alphaTopLayerBackWall/(1-alphaTopLayerBackWall)) * itNWire->at(jointBack*2);

		} else if (layer == 0) { // bottom
			alphaBottomLayerFrontWall =
				(itNWire->at(jointFront*2) - itNWire->at(diagonal*2)).magnitude() / 
				(itNWire->at(jointFront*2+1) - itNWire->at(diagonal*2+1)).magnitude();
			mNWireJointBottomLayerFrontWall = 
				(1/(1-alphaBottomLayerFrontWall)) * itNWire->at(jointFront*2) -
				(alphaBottomLayerFrontWall/(1-alphaBottomLayerFrontWall)) * itNWire->at(jointFront*2+1);

			alphaBottomLayerBackWall =
				(itNWire->at(jointBack*2+1) - itNWire->at(diagonal*2+1)).magnitude() / 
				(itNWire->at(jointBack*2) - itNWire->at(diagonal*2)).magnitude();
			mNWireJointBottomLayerBackWall = 
				(1/(1-alphaBottomLayerBackWall)) * itNWire->at(jointBack*2+1) -
				(alphaBottomLayerBackWall/(1-alphaBottomLayerBackWall)) * itNWire->at(jointBack*2);

		} else {
			return false; //only 2 layers of wires are supported! //TODO proper error message
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
		SystemLogFile << "\n Alpha ratio between NWires = \n\n";
		SystemLogFile << "\t alphaTopLayerBackWall \t= " << alphaTopLayerBackWall << "\n";
		SystemLogFile << "\t alphaTopLayerFrontWall \t= " << alphaTopLayerFrontWall << "\n";
		SystemLogFile << "\t alphaBottomLayerBackWall \t= " << alphaBottomLayerBackWall << "\n";
		SystemLogFile << "\t alphaBottomLayerFrontWall \t= " << alphaBottomLayerFrontWall << "\n";
		SystemLogFile << " -----------------------------------------------------------------------------------\n";
		SystemLogFile << "\n Joints of N-wires in the Phantom frame = \n\n";
		SystemLogFile << "\t mNWireJointTopLayerFrontWall(used) \t= " << mNWireJointTopLayerFrontWall << "\n\n";
		SystemLogFile << "\t mNWireJointTopLayerBackWall(used) \t= " << mNWireJointTopLayerBackWall << "\n\n";
		SystemLogFile << "\t mNWireJointBottomLayerFrontWall(used) \t= " << mNWireJointBottomLayerFrontWall << "\n\n";
		SystemLogFile << "\t mNWireJointBottomLayerBackWall(used) \t= " << mNWireJointBottomLayerBackWall << "\n\n";
		SystemLogFile.close();
	}
}

//-----------------------------------------------------------------------------

void BrachyTRUSCalibrator::addDataPositionsPerImage( 
	std::vector<vnl_vector_double> SegmentedDataPositionListPerImage, 
	const vnl_matrix<double> TransformMatrixUSProbe2Stepper4x4 )
{
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

			vnl_vector<double> PositionInTemplateFrame; 
			
			if( 0 == Layer )
			{
				PositionInTemplateFrame = 
					mNWireJointTopLayerBackWall + 
					alpha*( mNWireJointTopLayerFrontWall - mNWireJointTopLayerBackWall );

				// Log the data pipeline if requested.
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
					SystemLogFile << " PositionInTemplateFrame = " << PositionInTemplateFrame << "\n";
					SystemLogFile << " TransformMatrixUSProbe2Stepper4x4 = \n" << TransformMatrixUSProbe2Stepper4x4 << "\n";
					SystemLogFile << " TransformMatrixStepper2USProbe4x4 = \n" << TransformMatrixStepper2USProbe4x4 << "\n";
					SystemLogFile.close();
				}
			}
			else // Layer == 1
			{
				PositionInTemplateFrame = 
					mNWireJointBottomLayerFrontWall + 
					alpha*( mNWireJointBottomLayerBackWall - mNWireJointBottomLayerFrontWall );
				
				// Log the data pipeline if requested.
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
					SystemLogFile << " PositionInTemplateFrame = " << PositionInTemplateFrame << "\n";
					SystemLogFile << " TransformMatrixUSProbe2Stepper4x4 = \n" << TransformMatrixUSProbe2Stepper4x4 << "\n";
					SystemLogFile << " TransformMatrixStepper2USProbe4x4 = \n" << TransformMatrixStepper2USProbe4x4 << "\n";
					SystemLogFile.close();
				}
			}

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
				SystemLogFile << " ----------------------------------------------------------------\n";
				SystemLogFile << " PositionInUSProbeFrame = \n" << PositionInUSProbeFrame << "\n";
				SystemLogFile << " TransformMatrixPhantom2DRB4x4 = \n" << mTransformMatrixPhantom2DRB4x4 << "\n";
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

			vnl_vector<double> PositionInPhantomFrame;

			if( 0 == Layer )
			{
				PositionInPhantomFrame = 
					mNWireJointTopLayerBackWall + 
					alpha*( mNWireJointTopLayerFrontWall - mNWireJointTopLayerBackWall );
		
				// Log the data pipeline if requested.
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
					SystemLogFile.close();
				}

			}
			else // Layer == 1
			{
				PositionInPhantomFrame = 
					mNWireJointBottomLayerFrontWall + 
					alpha*( mNWireJointBottomLayerBackWall - mNWireJointBottomLayerFrontWall );

				// Log the data pipeline if requested.
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
					SystemLogFile.close();
				}
			}

			// Finally, calculate the position in the US probe frame
			// X(US Probe) =  T(Tracker->US Probe)*T(DRB->Tracker)*T(Phantom->DRB)*X(Phantom)
			vnl_vector<double> PositionInUSProbeFrame =  
				TransformMatrixDRB2USProbe4x4 * 
				mTransformMatrixPhantom2DRB4x4 * 
				PositionInPhantomFrame;

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
            
			vnl_vector<double> PositionInTemplateFrame;

			// NWire joints that need to be saved to compute the PLDE (Point-Line Distance Error)
			// in addition to the real-time PRE3D.
			vnl_vector<double> NWireStartInTemplateFrame;
			vnl_vector<double> NWireEndInTemplateFrame;
			
			if( 0 == Layer )
			{
				PositionInTemplateFrame = 
					mNWireJointTopLayerBackWall + 
					alpha*( mNWireJointTopLayerFrontWall - mNWireJointTopLayerBackWall );
				
				NWireStartInTemplateFrame = mNWireJointTopLayerBackWall;
				NWireEndInTemplateFrame = mNWireJointTopLayerFrontWall;

				// Log the data pipeline if requested.
				if( true == mIsSystemLogOn )
				{
					std::ofstream SystemLogFile(
						mSystemLogFileNameWithTimeStamp.c_str(), std::ios::app);
					SystemLogFile << " ==========================================================================\n";
					SystemLogFile << " ADD DATA FOR VALIDATION >>>>>>>>>>>>>>>>>>>>>\n\n";
					SystemLogFile << " SegmentedNFiducial-" << Layer*3 << " = " << SegmentedDataPositionListPerImage.at( Layer*3 ) << "\n"; 
					SystemLogFile << " SegmentedNFiducial-" << Layer*3+1 << " = " << SegmentedDataPositionListPerImage.at( Layer*3+1 ) << "\n";  
					SystemLogFile << " SegmentedNFiducial-" << Layer*3+2 << " = " << SegmentedDataPositionListPerImage.at( Layer*3+2 ) << "\n";  
					SystemLogFile << " ----------------------------------------------------------------\n";
					SystemLogFile << " N1SegmentedPositionInUSImageFrame = " << N1SegmentedPositionInUSImageFrame << "\n";
					SystemLogFile << " N3SegmentedPositionInUSImageFrame = " << N3SegmentedPositionInUSImageFrame << "\n";
					SystemLogFile << " ----------------------------------------------------------------\n";
					SystemLogFile << " SegmentedPositionInOriginalImageFrame = " << SegmentedPositionInOriginalImageFrame << "\n";
					SystemLogFile << " SegmentedPositionInUSImageFrame = " << SegmentedPositionInUSImageFrame << "\n";
					SystemLogFile << " alpha = " << alpha << "\n";
					SystemLogFile << " PositionInTemplateFrame = " << PositionInTemplateFrame << "\n";
					SystemLogFile << " TransformMatrixUSProbe2Stepper4x4 = \n" << TransformMatrixUSProbe2Stepper4x4 << "\n";					
					SystemLogFile << " TransformMatrixStepper2USProbe4x4 = \n" << TransformMatrixStepper2USProbe4x4 << "\n";
					SystemLogFile.close();
				}
			}
			else // Layer == 1
			{
				PositionInTemplateFrame = 
					mNWireJointBottomLayerFrontWall + 
					alpha*( mNWireJointBottomLayerBackWall - mNWireJointBottomLayerFrontWall );
				
				NWireStartInTemplateFrame = mNWireJointBottomLayerBackWall;
				NWireEndInTemplateFrame = mNWireJointBottomLayerFrontWall;

				// Log the data pipeline if requested.
				if( true == mIsSystemLogOn )
				{
					std::ofstream SystemLogFile(
						mSystemLogFileNameWithTimeStamp.c_str(), std::ios::app);
					SystemLogFile << " ==========================================================================\n";
					SystemLogFile << " ADD DATA FOR VALIDATION >>>>>>>>>>>>>>>>>>>>>\n\n";
					SystemLogFile << " SegmentedNFiducial-" << Layer*3 << " = " << SegmentedDataPositionListPerImage.at( Layer*3 ) << "\n"; 
					SystemLogFile << " SegmentedNFiducial-" << Layer*3+1 << " = " << SegmentedDataPositionListPerImage.at( Layer*3+1 ) << "\n";  
					SystemLogFile << " SegmentedNFiducial-" << Layer*3+2 << " = " << SegmentedDataPositionListPerImage.at( Layer*3+2 ) << "\n";  
					SystemLogFile << " ----------------------------------------------------------------\n";
					SystemLogFile << " N4SegmentedPositionInUSImageFrame = " << N4SegmentedPositionInUSImageFrame << "\n";
					SystemLogFile << " N6SegmentedPositionInUSImageFrame = " << N6SegmentedPositionInUSImageFrame << "\n";
					SystemLogFile << " ----------------------------------------------------------------\n";
					SystemLogFile << " SegmentedPositionInOriginalImageFrame = " << SegmentedPositionInOriginalImageFrame << "\n";
					SystemLogFile << " SegmentedPositionInUSImageFrame = " << SegmentedPositionInUSImageFrame << "\n";
					SystemLogFile << " alpha = " << alpha << "\n";
					SystemLogFile << " PositionInTemplateFrame = " << PositionInTemplateFrame << "\n";
					SystemLogFile << " TransformMatrixUSProbe2Stepper4x4 = \n" << TransformMatrixUSProbe2Stepper4x4 << "\n";
					SystemLogFile << " TransformMatrixStepper2USProbe4x4 = \n" << TransformMatrixStepper2USProbe4x4 << "\n";
					SystemLogFile.close();
				}
			}

			// Finally, calculate the position in the US probe frame
			// X_USProbe = T_Stepper->USProbe * T_Template->Stepper * X_Template
			// NOTE: T_Template->Stepper = mTransformMatrixPhantom2DRB4x4 
			vnl_vector<double> PositionInUSProbeFrame =  
				TransformMatrixStepper2USProbe4x4 * 
				mTransformMatrixPhantom2DRB4x4 *
				PositionInTemplateFrame;

			vnl_vector<double> NWireStartinUSProbeFrame =
				TransformMatrixStepper2USProbe4x4 * 
				mTransformMatrixPhantom2DRB4x4 *
				NWireStartInTemplateFrame;

			vnl_vector<double> NWireEndinUSProbeFrame =
				TransformMatrixStepper2USProbe4x4 * 
				mTransformMatrixPhantom2DRB4x4 *
				NWireEndInTemplateFrame;

			// The parallel wires position in US Probe frame 
			// Note: 
			// 1. Parallel wires share the same X, Y coordinates as the N-wire joints
			//    in the phantom (template) frame.
			// 2. The Z-axis of the N-wire joints is not used in the computing.

 			// Wire N1 corresponds to mNWireJointTopLayerBackWall 
			vnl_vector<double> NWireJointTopLayerBackWallForN1InUSProbeFrame =
				TransformMatrixStepper2USProbe4x4 * 
				mTransformMatrixPhantom2DRB4x4 *
				mNWireJointTopLayerBackWall;

			// Wire N3 corresponds to mNWireJointTopLayerFrontWall
			vnl_vector<double> NWireJointTopLayerBackWallForN3InUSProbeFrame =
				TransformMatrixStepper2USProbe4x4 * 
				mTransformMatrixPhantom2DRB4x4 *
				mNWireJointTopLayerFrontWall;

			// Wire N4 corresponds to mNWireJointBottomLayerFrontWall
			vnl_vector<double> NWireJointTopLayerBackWallForN4InUSProbeFrame =
				TransformMatrixStepper2USProbe4x4 * 
				mTransformMatrixPhantom2DRB4x4 *
				mNWireJointBottomLayerFrontWall;

			// Wire N6 corresponds to mNWireJointBottomLayerBackWall
			vnl_vector<double> NWireJointTopLayerBackWallForN6InUSProbeFrame =
				TransformMatrixStepper2USProbe4x4 * 
				mTransformMatrixPhantom2DRB4x4 *
				mNWireJointBottomLayerBackWall;

			// Store into the list of positions in the US image frame
			mValidationPositionsInUSImageFrame.push_back( SegmentedPositionInUSImageFrame );
			mValidationPositionsNWire1InUSImageFrame.push_back( N1SegmentedPositionInUSImageFrame );
			mValidationPositionsNWire3InUSImageFrame.push_back( N3SegmentedPositionInUSImageFrame );
			mValidationPositionsNWire4InUSImageFrame.push_back( N4SegmentedPositionInUSImageFrame );
			mValidationPositionsNWire6InUSImageFrame.push_back( N6SegmentedPositionInUSImageFrame );


			// Store into the list of positions in the US probe frame
			mValidationPositionsInUSProbeFrame.push_back( PositionInUSProbeFrame );
			mValidationPositionsNWireStartInUSProbeFrame.push_back( NWireStartinUSProbeFrame );
			mValidationPositionsNWireEndInUSProbeFrame.push_back( NWireEndinUSProbeFrame );
			mValidationPositionsNWire1InUSProbeFrame.push_back( NWireJointTopLayerBackWallForN1InUSProbeFrame );
			mValidationPositionsNWire3InUSProbeFrame.push_back( NWireJointTopLayerBackWallForN3InUSProbeFrame );
			mValidationPositionsNWire4InUSProbeFrame.push_back( NWireJointTopLayerBackWallForN4InUSProbeFrame );
			mValidationPositionsNWire6InUSProbeFrame.push_back( NWireJointTopLayerBackWallForN6InUSProbeFrame );


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

			vnl_vector<double> PositionInPhantomFrame;

			// NWire joints that need to be saved to compute the PLDE (Point-Line Distance Error) 
			// in addition to the real-time PRE3D.
			vnl_vector<double> NWireStartinPhantomFrame;
			vnl_vector<double> NWireEndinPhantomFrame;
			
			if( 0 == Layer )
			{
				PositionInPhantomFrame = 
					mNWireJointTopLayerBackWall + 
					alpha*( mNWireJointTopLayerFrontWall - mNWireJointTopLayerBackWall );
				
				NWireStartinPhantomFrame = mNWireJointTopLayerBackWall;
				NWireEndinPhantomFrame = mNWireJointTopLayerFrontWall;

				// Log the data pipeline if requested.
				if( true == mIsSystemLogOn )
				{
					std::ofstream SystemLogFile(
						mSystemLogFileNameWithTimeStamp.c_str(), std::ios::app);
					SystemLogFile << " ==========================================================================\n";
					SystemLogFile << " ADD DATA FOR VALIDATION >>>>>>>>>>>>>>>>>>>>>\n\n";
					SystemLogFile << " SegmentedNFiducial-" << Layer*3 << " = " << SegmentedDataPositionListPerImage.at( Layer*3 ) << "\n"; 
					SystemLogFile << " SegmentedNFiducial-" << Layer*3+1 << " = " << SegmentedDataPositionListPerImage.at( Layer*3+1 ) << "\n";  
					SystemLogFile << " SegmentedNFiducial-" << Layer*3+2 << " = " << SegmentedDataPositionListPerImage.at( Layer*3+2 ) << "\n";  
					SystemLogFile << " ----------------------------------------------------------------\n";
					SystemLogFile << " SegmentedPositionInOriginalImageFrame = " << SegmentedPositionInOriginalImageFrame << "\n";
					SystemLogFile << " SegmentedPositionInUSImageFrame = " << SegmentedPositionInUSImageFrame << "\n";
					SystemLogFile << " alpha = " << alpha << "\n";
					SystemLogFile << " PositionInPhantomFrame = " << PositionInPhantomFrame << "\n";
					SystemLogFile << " TransformMatrixDRB2USProbe4x4 = \n" << TransformMatrixDRB2USProbe4x4 << "\n";
					SystemLogFile.close();
				}
			}
			else // Layer == 1
			{
				PositionInPhantomFrame = 
					mNWireJointBottomLayerFrontWall + 
					alpha*( mNWireJointBottomLayerBackWall - mNWireJointBottomLayerFrontWall );
				
				NWireStartinPhantomFrame = mNWireJointBottomLayerBackWall;
				NWireEndinPhantomFrame = mNWireJointBottomLayerFrontWall;

				// Log the data pipeline if requested.
				if( true == mIsSystemLogOn )
				{
					std::ofstream SystemLogFile(
						mSystemLogFileNameWithTimeStamp.c_str(), std::ios::app);
					SystemLogFile << " ==========================================================================\n";
					SystemLogFile << " ADD DATA FOR VALIDATION >>>>>>>>>>>>>>>>>>>>>\n\n";
					SystemLogFile << " SegmentedNFiducial-" << Layer*3 << " = " << SegmentedDataPositionListPerImage.at( Layer*3 ) << "\n"; 
					SystemLogFile << " SegmentedNFiducial-" << Layer*3+1 << " = " << SegmentedDataPositionListPerImage.at( Layer*3+1 ) << "\n";  
					SystemLogFile << " SegmentedNFiducial-" << Layer*3+2 << " = " << SegmentedDataPositionListPerImage.at( Layer*3+2 ) << "\n";  
					SystemLogFile << " ----------------------------------------------------------------\n";
					SystemLogFile << " SegmentedPositionInOriginalImageFrame = " << SegmentedPositionInOriginalImageFrame << "\n";
					SystemLogFile << " SegmentedPositionInUSImageFrame = " << SegmentedPositionInUSImageFrame << "\n";
					SystemLogFile << " alpha = " << alpha << "\n";
					SystemLogFile << " PositionInPhantomFrame = " << PositionInPhantomFrame << "\n";
					SystemLogFile << " TransformMatrixDRB2USProbe4x4 = \n" << TransformMatrixDRB2USProbe4x4 << "\n";
					SystemLogFile.close();
				}
			}

			// Finally, calculate the position in the US probe frame
			// X(US Probe) =  T(Tracker->US Probe)*T(DRB->Tracker)*T(Phantom->DRB)*X(Phantom)
			vnl_vector<double> PositionInUSProbeFrame =  
				TransformMatrixDRB2USProbe4x4 * 
				mTransformMatrixPhantom2DRB4x4 * 
				PositionInPhantomFrame;

			vnl_vector<double> NWireStartinUSProbeFrame =
				TransformMatrixDRB2USProbe4x4 * 
				mTransformMatrixPhantom2DRB4x4 * 
				NWireStartinPhantomFrame;

			vnl_vector<double> NWireEndinUSProbeFrame =
				TransformMatrixDRB2USProbe4x4 * 
				mTransformMatrixPhantom2DRB4x4 * 
				NWireEndinPhantomFrame;

			// Store into the list of positions in the US image frame
			mValidationPositionsInUSImageFrame.push_back( SegmentedPositionInUSImageFrame );

			// Store into the list of positions in the US probe frame
			mValidationPositionsInUSProbeFrame.push_back( PositionInUSProbeFrame );
			mValidationPositionsNWireStartInUSProbeFrame.push_back( NWireStartinUSProbeFrame );
			mValidationPositionsNWireEndInUSProbeFrame.push_back( NWireEndinUSProbeFrame );

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

			vnl_vector<double> PositionInPhantomFrame;
			
			if( 0 == Layer )
			{
				PositionInPhantomFrame = 
					mNWireJointTopLayerBackWall + 
					alpha* (mNWireJointTopLayerFrontWall - mNWireJointTopLayerBackWall );

				// Log the data pipeline if requested.
				if( true == mIsSystemLogOn )
				{
					std::ofstream SystemLogFile(
						mSystemLogFileNameWithTimeStamp.c_str(), std::ios::app);
					SystemLogFile << " ==========================================================================\n";
					SystemLogFile << " REAL-TIME DATA FOR VALIDATION >>>>>>>>>>>>>>>>>>>>>\n\n";
					SystemLogFile << " SegmentedNFiducial-" << Layer*3 << " = " << SegmentedDataPositionListPerImage.at( Layer*3 ) << "\n"; 
					SystemLogFile << " SegmentedNFiducial-" << Layer*3+1 << " = " << SegmentedDataPositionListPerImage.at( Layer*3+1 ) << "\n";  
					SystemLogFile << " SegmentedNFiducial-" << Layer*3+2 << " = " << SegmentedDataPositionListPerImage.at( Layer*3+2 ) << "\n";  
					SystemLogFile << " ----------------------------------------------------------------\n";
					SystemLogFile << " SegmentedPositionInOriginalImageFrame = " << SegmentedPositionInOriginalImageFrame << "\n";
					SystemLogFile << " SegmentedPositionInUSImageFrame = " << SegmentedPositionInUSImageFrame << "\n";
					SystemLogFile << " alpha = " << alpha << "\n";
					SystemLogFile << " PositionInPhantomFrame = " << PositionInPhantomFrame << "\n";
					SystemLogFile << " TransformMatrixDRB2USProbe4x4 = \n" << TransformMatrixDRB2USProbe4x4 << "\n";
					SystemLogFile.close();
				}
			}
			else // Layer == 1
			{
				PositionInPhantomFrame = 
					mNWireJointBottomLayerFrontWall + 
					alpha*( mNWireJointBottomLayerBackWall - mNWireJointBottomLayerFrontWall );

				// Log the data pipeline if requested.
				if( true == mIsSystemLogOn )
				{
					std::ofstream SystemLogFile(
						mSystemLogFileNameWithTimeStamp.c_str(), std::ios::app);
					SystemLogFile << " ==========================================================================\n";
					SystemLogFile << " REAL-TIME DATA FOR VALIDATION >>>>>>>>>>>>>>>>>>>>>\n\n";
					SystemLogFile << " SegmentedNFiducial-" << Layer*3 << " = " << SegmentedDataPositionListPerImage.at( Layer*3 ) << "\n"; 
					SystemLogFile << " SegmentedNFiducial-" << Layer*3+1 << " = " << SegmentedDataPositionListPerImage.at( Layer*3+1 ) << "\n";  
					SystemLogFile << " SegmentedNFiducial-" << Layer*3+2 << " = " << SegmentedDataPositionListPerImage.at( Layer*3+2 ) << "\n";  
					SystemLogFile << " ----------------------------------------------------------------\n";
					SystemLogFile << " SegmentedPositionInOriginalImageFrame = " << SegmentedPositionInOriginalImageFrame << "\n";
					SystemLogFile << " SegmentedPositionInUSImageFrame = " << SegmentedPositionInUSImageFrame << "\n";
					SystemLogFile << " alpha = " << alpha << "\n";
					SystemLogFile << " PositionInPhantomFrame = " << PositionInPhantomFrame << "\n";
					SystemLogFile << " TransformMatrixDRB2USProbe4x4 = \n" << TransformMatrixDRB2USProbe4x4 << "\n";
					SystemLogFile.close();
				}
			}

			// Finally, calculate the position in the US probe frame
			// X(US Probe) =  T(Tracker->US Probe)*T(DRB->Tracker)*T(Phantom->DRB)*X(Phantom)
			vnl_vector<double> PositionInUSProbeFrame =  
				TransformMatrixDRB2USProbe4x4 * 
				mTransformMatrixPhantom2DRB4x4 * 
				PositionInPhantomFrame;

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
