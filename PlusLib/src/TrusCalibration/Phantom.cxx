// ===========================================================================
// Program Modification Record
// ===========================================================================
// Component:	Component_AutoTRUSCalibration
// ===========================================================================
// Class:		Phantom
// ===========================================================================
// File Name:	Phantom.h
// ===========================================================================
// Author:		Thomas Kuiran Chen <chent@cs.queensu.ca>
//				School of Computing, Queen's University at Kingston, Canada
// ===========================================================================
// Purposes: 
// 1. This the parent class of all current or future calibration phantoms.
// 2. This class defines some generic features and behaviors that one would
//    typically expect from an ultrasound probe calibration phantom.  By 
//    inheriting (or extending) this class, the child class would only need
//    to define some specific features of its own (e.g., the unique phantom 
//    geometries that differ from phantom to phantom).
// 3. Uses the open source VXL-VNL (Numerical and Algebra C++ Library)
// 4. Uses C++ Standard Library and Standard Template Library
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

#include "Phantom.h"

// C++ Standard Includes
#include <iostream>
#include <fstream>	// for file I/O process
#include <ctime>	// for system time: _strdate, _strtime

// CommonFramework includes
// Least-Squares class include
#ifndef LINEARLEASTSQUARES_H
#include "LinearLeastSquares.h"
#endif

// VNL Includes
#include "vnl/algo/vnl_matrix_inverse.h"
#include "vcl_istream.h"

// Strings
const std::string Phantom::mstrScope = "Phantom";
const std::string Phantom::mstrCalibrationLogFileNameSuffix = ".Calibration.log";

// Constants
const double Phantom::mPI = 3.141592653589;

double Phantom::mOutlierDetectionThreshold = 3.0; 


Phantom::Phantom( const bool IsSystemLogOn )
{
	try
	{
		// Initialize flags
		mHasPhantomBeenRegistered = false;
		mAreDataPositionsReady = false;
		mAreValidationPositionsReady = false;
		mArePRE3DsForValidationPositionsReady = false;
		mAreIndependentPointLineReconErrorsReady = false;
		mIsPhantomGeometryLoaded = false;
		mIsUSBeamwidthAndWeightFactorsTableReady = false;
		mHasBeenCalibrated = false;
		mIsSystemLogOn = false;
		mHasUSImageFrameOriginBeenSet = false;
		mAreValidationDataMatricesConstructed = false;
		mAreOutliersRemoved = false; 
        	
		// Initialize data containers
		mSystemLogFileNameWithTimeStamp = "";
		mUSImageFrameOriginInPixels.set_size(4);
		mTransformOrigImageFrame2TRUSImageFrameMatrix4x4.set_size(4,4);
		mTransformMatrixPhantom2DRB4x4.set_size(4,4);
		mTransformMatrixDRB2Phantom4x4.set_size(4,4);
		mPhantomSpecificReferencePoints.resize(0);
		mNamesOfPhantomSpecificReferencePoints.resize(0);
		mDataPositionsInPhantomFrame.resize(0);
		mDataPositionsInUSProbeFrame.resize(0);
		mDataPositionsInUSImageFrame.resize(0);
		mOutlierDataPositions.resize(0); 
		mUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xM.set_size(0,0);
		mOrigUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xN.set_size(0,0);
		mMinimumUSElevationBeamwidthAndFocalZoneInUSImageFrame.set_size(2);
		mUS3DBeamwidthAtNearestAxialDepth.set_size(3);
		mUS3DBeamwidthAtFarestAxialDepth.set_size(3);
		mMethodToIncorporateBeamWidth = 0;
		mNumOfTotalBeamWidthData = 0;
		mWeightsForDataPositions.resize(0);
		mUSBeamWidthEuclideanMagAtDataPositions.resize(0);
		mUSBeamWidthEuclideanMagAtValidationPositions.resize(0);
		mValidationPositionsInPhantomFrame.resize(0);
		mValidationPositionsInUSProbeFrame.resize(0);
		mValidationPositionsNWireStartInUSProbeFrame.resize(0);
		mValidationPositionsNWireEndInUSProbeFrame.resize(0);

		mValidationPositionsNWire1InUSImageFrame.resize(0);
		mValidationPositionsNWire3InUSImageFrame.resize(0);
		mValidationPositionsNWire4InUSImageFrame.resize(0);
		mValidationPositionsNWire6InUSImageFrame.resize(0);

		mValidationPositionsNWire1InUSProbeFrame.resize(0);
		mValidationPositionsNWire3InUSProbeFrame.resize(0);
		mValidationPositionsNWire4InUSProbeFrame.resize(0);
		mValidationPositionsNWire6InUSProbeFrame.resize(0);

		mValidationPositionsInUSImageFrame.resize(0);
		mWeightsForValidationPositions.resize(0);
		mValidationPositionsInUSImageFrameMatrix4xN.set_size(0,0);

		mValidationPositionsNWire1InUSImageFrame4xN.set_size(0,0);
		mValidationPositionsNWire3InUSImageFrame4xN.set_size(0,0);
		mValidationPositionsNWire4InUSImageFrame4xN.set_size(0,0);
		mValidationPositionsNWire6InUSImageFrame4xN.set_size(0,0);

		mValidationPositionsNWire1InUSProbeFrame4xN.set_size(0,0);
		mValidationPositionsNWire3InUSProbeFrame4xN.set_size(0,0);
		mValidationPositionsNWire4InUSProbeFrame4xN.set_size(0,0);
		mValidationPositionsNWire6InUSProbeFrame4xN.set_size(0,0);

		mNWire1LREOrigInUSProbeFrameMatrix4xN.set_size(0,0);
		mNWire3LREOrigInUSProbeFrameMatrix4xN.set_size(0,0);
		mNWire4LREOrigInUSProbeFrameMatrix4xN.set_size(0,0);
		mNWire6LREOrigInUSProbeFrameMatrix4xN.set_size(0,0);

		mNWire1LRESortedAscendingInUSProbeFrameMatrix4xN.set_size(0,0);
		mNWire3LRESortedAscendingInUSProbeFrameMatrix4xN.set_size(0,0);
		mNWire4LRESortedAscendingInUSProbeFrameMatrix4xN.set_size(0,0);
		mNWire6LRESortedAscendingInUSProbeFrameMatrix4xN.set_size(0,0);

		mTransformUSImageFrame2USProbeFrameParameters.resize(0);
		mAbsPRE3DAnalysis4ValidationPositionsInUSProbeFrame.resize(0);
		mPLDEAnalysis4ValidationPositionsInUSProbeFrame.resize(0);
		mRawPRE3DsforValidationPositionsInUSProbeFrameMatrix4xN.set_size(0,0);
		mSortedRawPRE3DsInAscendingOrderInUSProbeFrameMatrix4xN.set_size(0,0);
		mPLDEsforValidationPositionsInUSProbeFrame.set_size(0);
		mSortedPLDEsAscendingforValidationInUSProbeFrame.set_size(0);
		mValidationDataConfidenceLevel = 0.95;

		// Record the system timestamp
		char dateStr[9];
		char timeStr[9];
		_strdate( dateStr );
		_strtime( timeStr );
		std::string DateInString(dateStr);
		DateInString.erase(5,1);	// remve '/'
		DateInString.erase(2,1);	// remve '/'
		std::string TimeInString(timeStr);
		TimeInString.erase(5,1);	// remve ':'
		TimeInString.erase(2,1);	// remve ':'
		// DATE/TIME IN STRING: [MMDDYY_HHMMSS]
		mSystemTimestampInString = DateInString + "_" + TimeInString;
		// DATE/TIME WITH FORMAT: [MM/DD/YY HH:MM:SS]
		mSystemTimestampWithFormat.append( dateStr ); 
		mSystemTimestampWithFormat.append( " " ); 
		mSystemTimestampWithFormat.append( timeStr );

		// Turn on the system logging service if instructed.
		// 1. The system log file with its name associated with a system 
		//    timestamp, when turned on, is used to record all the data pipeline 
		//    processing during the calibration.
		// 2. The system log should be turned on before any calibration related 
		//    activities (e.g., add data positions, validating, calibrating, etc.).
		// [DEFAULT: OFF (FALSE)]
		if( true == IsSystemLogOn )
		{
			// Turn on the system logging service
			mIsSystemLogOn = true;

			// construct the system log filename with timestamp
			mSystemLogFileNameWithTimeStamp = 
				mSystemTimestampInString + mstrCalibrationLogFileNameSuffix;

			// Set the log file header
			std::ofstream SystemLogFile(
				mSystemLogFileNameWithTimeStamp.c_str(), std::ios::out);
			SystemLogFile << "# ======================================= #\n";
			SystemLogFile << "# ULTRASOUND CALIBRATION PROCESS LOG FILE #\n";
			SystemLogFile << "# ======================================= #\n";
			SystemLogFile << "# THIS FILE IS AUTOMATICALLY CREATED BY   #\n";
			SystemLogFile << "# THE PROGRAM.  DO NOT EDIT THE FILE!!!   #\n";
			SystemLogFile << "# ======================================= #\n\n";
			//SystemLogFile << " SYSTEM TIMESTAMP:\t" << dateStr << " " << timeStr << "\n" ;
			SystemLogFile << " SYSTEM TIMESTAMP:\t" << mSystemTimestampWithFormat << "\n" ;
			SystemLogFile << " LOG FILE NAME:\t\t\t" << mSystemLogFileNameWithTimeStamp << "\n\n";
			SystemLogFile.close();
		}

	}
	catch(...)
	{
		std::cerr << "\n\n" << __FILE__ << "," << __LINE__ << "\n"
			<< ">>>>>>>> In " << mstrScope << "::Construction failed!!!  Throw up ...\n";

		throw;
	}
}

Phantom::~Phantom()
{
	try
	{
		if( true == mIsSystemLogOn )
		{	
			std::ofstream SystemLogFile(
				mSystemLogFileNameWithTimeStamp.c_str(), std::ios::app);

			// Set end of file.
			SystemLogFile << "\n\n# ============================== #\n";
			SystemLogFile << "# END OF THE LOG FILE:           #\n";
			SystemLogFile << "# " << mSystemLogFileNameWithTimeStamp << "  #\n";
			SystemLogFile << "# ============================== #\n";
			SystemLogFile.close();
		}
	}
	catch(...)
	{

	}
}

void Phantom::setUltrasoundImageFrameOriginInPixels(
	const double USImageFrameOriginXInPixels,
	const double USImageFrameOriginYInPixels )
{
	double USImageFrameOriginInPixels[] = 
		{ USImageFrameOriginXInPixels, USImageFrameOriginYInPixels, 0, 1 };

	mUSImageFrameOriginInPixels.set( USImageFrameOriginInPixels );
	mHasUSImageFrameOriginBeenSet = true;
}

void Phantom::setTransformOrigImageToTRUSImageFrame4x4(
	const vnl_matrix<double> TransformOrigImageFrame2TRUSImageFrameMatrix4x4)
{
	mTransformOrigImageFrame2TRUSImageFrameMatrix4x4 = 
		TransformOrigImageFrame2TRUSImageFrameMatrix4x4;

	mHasUSImageFrameOriginBeenSet = true;
}

void Phantom::setUltrasoundBeamwidthAndWeightFactorsTable(
	const int FlagOnHowToIncorporateBeamWidth,
	vnl_matrix<double> const & US3DBeamwidthAndWeightFactorsInUSImageFrameTable, 
	vnl_matrix<double> const & OrigUS3DBeamwidthAndWeightFactorsInUSImageFrameTable,
	vnl_vector<double> const & MinimumUSElevationBeamwidthAndFocalZoneInUSImageFrame )
{
	// Set the way to incorporate the ultrasound beamwidth
	mMethodToIncorporateBeamWidth = FlagOnHowToIncorporateBeamWidth;

	// Set the minimum US beamwidth in axial, lateral, and elevation axes.
	mMinimumUSElevationBeamwidthAndFocalZoneInUSImageFrame = MinimumUSElevationBeamwidthAndFocalZoneInUSImageFrame;

	// Populate the beam-width data accordingly
	mNumOfTotalBeamWidthData = US3DBeamwidthAndWeightFactorsInUSImageFrameTable.columns();
	mUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xM = 
		US3DBeamwidthAndWeightFactorsInUSImageFrameTable;
	mOrigUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xN = 
		OrigUS3DBeamwidthAndWeightFactorsInUSImageFrameTable;

	// Fetch the data at the nearest and farest axial depth for fast access 
	mTheNearestAxialDepthInUSBeamwidthAndWeightTable = 
		ROUND(mUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xM.get(0,0));
	mTheFarestAxialDepthInUSBeamwidthAndWeightTable = 
		ROUND(mUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xM.get(0, mNumOfTotalBeamWidthData-1));

	// Populate the 3D beamwidth elements at the nearest and farest axial depth
	mUS3DBeamwidthAtNearestAxialDepth.put(0,
		mUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xM.get(1,0));
	mUS3DBeamwidthAtNearestAxialDepth.put(1,
		mUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xM.get(2,0));
	mUS3DBeamwidthAtNearestAxialDepth.put(2,
		mUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xM.get(3,0));

	mUS3DBeamwidthAtFarestAxialDepth.put(0,
		mUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xM.get(1,mNumOfTotalBeamWidthData-1));
	mUS3DBeamwidthAtFarestAxialDepth.put(1,
		mUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xM.get(2,mNumOfTotalBeamWidthData-1));
	mUS3DBeamwidthAtFarestAxialDepth.put(2,
		mUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xM.get(3,mNumOfTotalBeamWidthData-1));

	mIsUSBeamwidthAndWeightFactorsTableReady = true;

	// Log the data pipeline if requested.
	if( true == mIsSystemLogOn )
	{
		std::ofstream SystemLogFile(
			mSystemLogFileNameWithTimeStamp.c_str(), std::ios::app);
		SystemLogFile << " ==========================================================================\n";
		SystemLogFile << " (OPTIONAL) US 3D BEAM-WIDTH AND WEIGHT FACTORS TABLE WAS SET >>>>>>>>>>>\n\n";
		SystemLogFile << " mMethodToIncorporateBeamWidth = " << mMethodToIncorporateBeamWidth 
			<< " (1 - BeamwidthVariance | 2 - BeamwidthRatio | 3 - BeamwidthVarianceAndThresholding)\n";
		SystemLogFile << " -----------------------------------------------------------------------------------\n";
		SystemLogFile << " mNumOfTotalBeamWidthData = " 
			<< mNumOfTotalBeamWidthData << "\n";
		SystemLogFile << " -----------------------------------------------------------------------------------\n";
		SystemLogFile << " mMinimumUSElevationBeamwidthAndFocalZoneInUSImageFrame [Focal Zone (US Image Frame), Elevation Beamwidth] = \n" 
			<< mMinimumUSElevationBeamwidthAndFocalZoneInUSImageFrame << "\n";
		SystemLogFile << " -----------------------------------------------------------------------------------\n";
		SystemLogFile << " mOrigUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xN = \n" 
			<< mOrigUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xN << "\n";
		SystemLogFile << " -----------------------------------------------------------------------------------\n";
		SystemLogFile << " mUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xM (interpolated) = \n" 
			<< mUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xM << "\n";
		SystemLogFile << " -----------------------------------------------------------------------------------\n";
		SystemLogFile << " mTheNearestAxialDepthInUSBeamwidthAndWeightTable = " 
			<< mTheNearestAxialDepthInUSBeamwidthAndWeightTable << "\n";				
		SystemLogFile << " -----------------------------------------------------------------------------------\n";
		SystemLogFile << " mTheFarestAxialDepthInUSBeamwidthAndWeightTable = " 
			<< mTheFarestAxialDepthInUSBeamwidthAndWeightTable << "\n\n";
		SystemLogFile.close();
	}
}

void Phantom::setValidationDataConfidenceLevel( 
	const double ValidationDataConfidenceLevel )
{
	if( ValidationDataConfidenceLevel <= 1 && ValidationDataConfidenceLevel > 0 )
	{
		mValidationDataConfidenceLevel = ValidationDataConfidenceLevel;
	}
}

std::vector<std::string> Phantom::getNamesOfPhantomSpecificReferencePoints ()
{
	return mNamesOfPhantomSpecificReferencePoints;
}

void Phantom::registerPhantomGeometry( 
			const std::vector<vnl_vector_double> PhantomSpecificReferencePointsInDRBFrame )
{
	const int NumberOfPhantomSpecificReferencePoints( mPhantomSpecificReferencePoints.size() );
	if( PhantomSpecificReferencePointsInDRBFrame.size() 
		!= NumberOfPhantomSpecificReferencePoints )
	{
		std::cerr << "\n\n" << __FILE__ << "," << __LINE__ << "\n"
		<< ">>>>>>>> In " << mstrScope 
		<< "::Number of Phantom-Specific Reference Points Does NOT MATCH!!!  Throw up ...\n";
	
         throw;
	}

	if( mHasPhantomBeenRegistered == true )
	{
		mHasPhantomBeenRegistered = false;
	}

	// Use the least-squares algorithm to register the phantom to DRB
	// ===============================================================

	// Construct the 4xN matrix of positions in the phantom frame
	// with number of columns N being the number of points
	vnl_matrix<double> PhantomRefPositionsMatrixInPhantomFrame4xN(4, NumberOfPhantomSpecificReferencePoints);
	for( int i = 0; i < NumberOfPhantomSpecificReferencePoints; i++ )
	{
		PhantomRefPositionsMatrixInPhantomFrame4xN.set_column( i, mPhantomSpecificReferencePoints[i] );
	}

	// Construct the 4xN matrix of positions in the DRB frame
	// with number of columns N being the number of points
	vnl_matrix<double> PhantomRefPositionsMatrixInDRBFrame4xN(4, NumberOfPhantomSpecificReferencePoints);
	for( int i = 0; i < NumberOfPhantomSpecificReferencePoints; i++ )
	{
		PhantomRefPositionsMatrixInDRBFrame4xN.set_column( i, PhantomSpecificReferencePointsInDRBFrame.at(i) );
	}

	// Feed the data to the least-squares class (from CommonFramework component)
	// to find the homogeneous matrix that transfers a point in the
	// phantom frame to the DRB frame that is mounted with the phantom.
	// T_Phantom2DRB * XinPhantomFrame = XinDRBFrame -> to solve T
	// The transform obtained is a 4x4 homogemeous matrix.
	try
	{
		LinearLeastSquares LeastSquares(
			PhantomRefPositionsMatrixInPhantomFrame4xN,		// dataObserver1
			PhantomRefPositionsMatrixInDRBFrame4xN);		// dataobserver2
		LeastSquares.doOptimization();
		mTransformMatrixPhantom2DRB4x4 = LeastSquares.getTransform();
	}
	catch(...)
	{
		std::cerr << "\n\n" << __FILE__ << "," << __LINE__ << "\n"
		<< ">>>>>>>> In " << mstrScope 
		<< "::Least-squares calculation failed!!!  Throw up ...\n";
	
         throw;
	}
		
	// Make sure the last row in homogeneous transform is [0 0 0 1]
	vnl_vector<double> lastRow(4,0);
	lastRow.put(3, 1);
	mTransformMatrixPhantom2DRB4x4.set_row(3, lastRow);

	// Obtain the inverse to of the transform from DRB to Phantom frame
	// This matrix is not required for calibration, but it could be
	// userful for debugging or other validation analysis.
	vnl_matrix_inverse<double> inverseMatrix( mTransformMatrixPhantom2DRB4x4 );
	mTransformMatrixDRB2Phantom4x4 = inverseMatrix.inverse();
	// Make sure the last row in homogeneous transform is [0 0 0 1]
	mTransformMatrixDRB2Phantom4x4.set_row(3, lastRow);

    // Set the calibration flag to true
	mHasPhantomBeenRegistered = true;

	// Log the data pipeline if requested.
	if( true == mIsSystemLogOn )
	{
		std::vector<double> TransformPhantom2DRBFrameInParameters = 
			convertHomogeneousMatrixToParameters( mTransformMatrixPhantom2DRB4x4 );

		std::ofstream SystemLogFile(
			mSystemLogFileNameWithTimeStamp.c_str(), std::ios::app);
		SystemLogFile << " ==========================================================================\n";
		SystemLogFile << " REGISTER THE PHANTOM TO THE DRB FRAME >>>>>>>>>>>\n\n";
		SystemLogFile << " PhantomRefPositionsMatrixInPhantomFrame4xN = \n" 
			<< PhantomRefPositionsMatrixInPhantomFrame4xN << "\n";
		SystemLogFile << " PhantomRefPositionsMatrixInDRBFrame4xN = \n" 
			<< PhantomRefPositionsMatrixInDRBFrame4xN << "\n";
		SystemLogFile << " -----------------------------------------------------------------------------------\n";
		SystemLogFile << " mTransformMatrixPhantom2DRB4x4 = \n" 
			<< mTransformMatrixPhantom2DRB4x4 << "\n";				
		SystemLogFile << " -----------------------------------------------------------------------------------\n";
		SystemLogFile << " mTransformMatrixDRB2Phantom4x4 = \n" 
			<< mTransformMatrixDRB2Phantom4x4 << "\n";				
		SystemLogFile << " -----------------------------------------------------------------------------------\n";
		SystemLogFile << "Convert to 8-parameter transform (from Phantom to DRB frame) = \n";
		SystemLogFile << "FORMAT: [Alpha, Beta, Gamma (in radians); Sx, Sy; Tx, Ty, Tz]\n";
		SystemLogFile << TransformPhantom2DRBFrameInParameters[0] << ", "
			<< TransformPhantom2DRBFrameInParameters[1] << ", "
			<< TransformPhantom2DRBFrameInParameters[2] << ";  "
			<< TransformPhantom2DRBFrameInParameters[3] << ", "
			<< TransformPhantom2DRBFrameInParameters[4] << ";  "
			<< TransformPhantom2DRBFrameInParameters[5] << ", "
			<< TransformPhantom2DRBFrameInParameters[6] << ", "
			<< TransformPhantom2DRBFrameInParameters[7] << ".\n\n";
		SystemLogFile.close();
	}
}

void Phantom::registerPhantomGeometryInEmulatorMode( 
    const vnl_matrix<double> TransformMatrixPhantom2DRB4x4 )
{
	if( mHasPhantomBeenRegistered == true )
	{
		mHasPhantomBeenRegistered = false;
	}

	// Obtain the transform from Phantom to DRB frame
	// This matrix would be used in the calibration process to
	// bring the N-fiducials from phantom frame to DRB frame.
	mTransformMatrixPhantom2DRB4x4 = TransformMatrixPhantom2DRB4x4;

	// Obtain the inverse to of the transform from DRB to Phantom frame
	// This matrix is not required for calibration, but it could be
	// userful for debugging or other validation analysis.
	vnl_matrix_inverse<double> inverseMatrix( mTransformMatrixPhantom2DRB4x4 );
	mTransformMatrixDRB2Phantom4x4 = inverseMatrix.inverse();
	// Make sure the last row in homogeneous transform is [0 0 0 1]
	vnl_vector<double> lastRow(4,0);
	lastRow.put(3, 1);
	mTransformMatrixDRB2Phantom4x4.set_row(3, lastRow);

    // Set the calibration flag to true
	mHasPhantomBeenRegistered = true;

	// Log the data pipeline if requested.
	if( true == mIsSystemLogOn )
	{
		std::vector<double> TransformPhantom2DRBFrameInParameters = 
			convertHomogeneousMatrixToParameters( mTransformMatrixPhantom2DRB4x4 );

		std::ofstream SystemLogFile(
			mSystemLogFileNameWithTimeStamp.c_str(), std::ios::app);
		SystemLogFile << " ==========================================================================\n";
		SystemLogFile << " REGISTER THE PHANTOM TO THE DRB FRAME IN EMULATOR MODE >>>>>>>>>>>\n\n";
		SystemLogFile << " mTransformMatrixPhantom2DRB4x4 = \n" 
			<< mTransformMatrixPhantom2DRB4x4 << "\n";				
		SystemLogFile << " -----------------------------------------------------------------------------------\n";
		SystemLogFile << " mTransformMatrixDRB2Phantom4x4 = \n" 
			<< mTransformMatrixDRB2Phantom4x4 << "\n";				
		SystemLogFile << " -----------------------------------------------------------------------------------\n";
		SystemLogFile << "Convert to 8-parameter transform (from Phantom to DRB frame) = \n";
		SystemLogFile << "FORMAT: [Alpha, Beta, Gamma (in radians); Sx, Sy; Tx, Ty, Tz]\n";
		SystemLogFile << TransformPhantom2DRBFrameInParameters[0] << ", "
			<< TransformPhantom2DRBFrameInParameters[1] << ", "
			<< TransformPhantom2DRBFrameInParameters[2] << ";  "
			<< TransformPhantom2DRBFrameInParameters[3] << ", "
			<< TransformPhantom2DRBFrameInParameters[4] << ";  "
			<< TransformPhantom2DRBFrameInParameters[5] << ", "
			<< TransformPhantom2DRBFrameInParameters[6] << ", "
			<< TransformPhantom2DRBFrameInParameters[7] << ".\n\n";
		SystemLogFile.close();
	}
}

int Phantom::getNumberOfDataForCalibration ()
{
	return mDataPositionsInUSImageFrame.size();
}

int Phantom::getNumberOfDataForValidation()
{
	return mValidationPositionsInUSImageFrame.size();
}

bool Phantom::areDataPositionsReadyForCalibration()
{
	return mAreDataPositionsReady;
}

void Phantom::setOutlierFlags()
{
	const double numOfElements = this->getNumberOfDataForCalibration(); 
	vnl_vector<double> LREx(numOfElements); 
	vnl_vector<double> LREy(numOfElements); 

	// Get LRE for each calibration data 
	for ( int i = 0; i < numOfElements; i++ )
	{
		vnl_vector<double> LRExy = this->getPointLineReconstructionError(
			this->getDataPositionsInUSImageFrame()[i], 
			this->getDataPositionsInUSProbeFrame()[i] ); 

		LREx.put(i, LRExy.get(0)); 
		LREy.put(i, LRExy.get(1)); 
	}

	// Compute LRE mean
	double LRExMean = LREx.mean(); 
	double LREyMean = LREy.mean(); 

	// Compute LRE std
	vnl_vector<double> diffFromMeanX = LREx - LRExMean; 
	double LRExStd = sqrt(	diffFromMeanX.squared_magnitude() / numOfElements );

	vnl_vector<double> diffFromMeanY = LREy - LREyMean; 
	double LREyStd = sqrt(	diffFromMeanY.squared_magnitude() / numOfElements );

	// find outliers
	for ( int i = 0; i < numOfElements; i++ )
	{
		if ( abs(LREx.get(i) - LRExMean) > mOutlierDetectionThreshold * LRExStd 
			|| 
			abs(LREy.get(i) - LREyMean) > mOutlierDetectionThreshold * LREyStd )
		{
			// Add position to the outlier list
			this->mOutlierDataPositions.push_back(i); 
		}
	}

	this->mAreOutliersRemoved = true;
}

void Phantom::resetOutlierFlags()
{
	this->mOutlierDataPositions.clear(); 

	this->mAreOutliersRemoved = false; 
}

void Phantom::calibrate ()
{
	if( mAreDataPositionsReady != true )
	{
		std::cerr << "\n\n" << __FILE__ << "," << __LINE__ << "\n"
			<< ">>>>>>>> In " << mstrScope << "::The data positions are not ready yet for calibration!!!  Throw up ...\n";

		throw;
	}

	if( true == mHasBeenCalibrated )
	{
		mHasBeenCalibrated = false;
	}

	try
	{
		// STEP-0 Copy original data positions and remove outliers 
		std::vector< vnl_vector<double> > DataPositionsInUSImageFrame = this->getDataPositionsInUSImageFrame(); 
		std::vector< vnl_vector<double> > DataPositionsInUSProbeFrame = this->getDataPositionsInUSProbeFrame();

		for ( std::vector<int>::reverse_iterator rit = mOutlierDataPositions.rbegin();  rit != mOutlierDataPositions.rend(); ++rit )
		{
			DataPositionsInUSImageFrame.erase( DataPositionsInUSImageFrame.begin() + *rit );
			DataPositionsInUSProbeFrame.erase( DataPositionsInUSProbeFrame.begin() + *rit );
		}

		// STEP-1. Populate the data positions into matrices

		const int TotalNumberOfDataPositionsForCalibration( DataPositionsInUSImageFrame.size() );
		vnl_matrix<double> DataPositionsInUSImageFrameMatrix4xN(4, TotalNumberOfDataPositionsForCalibration );
		vnl_matrix<double> DataPositionsInUSProbeFrameMatrix4xN(4, TotalNumberOfDataPositionsForCalibration );

		for( int i = 0; i < TotalNumberOfDataPositionsForCalibration; i++ )
		{
			// Populate the data positions in the US image frame
			DataPositionsInUSImageFrameMatrix4xN.set_column( i, DataPositionsInUSImageFrame.at(i) );
			
			// Populate the data positions in the US probe frame
			DataPositionsInUSProbeFrameMatrix4xN.set_column( i, DataPositionsInUSProbeFrame.at(i) );
		}

		// STEP-2. Call the LeastSquare Interface from CommonFramework Component
		// T_USImage2Probe * XinUSImageFrame = XinProbeFrame -> solve T
		// NOTE: The transform obtained is carrying the scaling information
		// for the ultrasound image already.

		LinearLeastSquares LeastSquares(
			DataPositionsInUSImageFrameMatrix4xN,		// dataObserver1
			DataPositionsInUSProbeFrameMatrix4xN);		// dataobserver2

		// If the weights for the data positions are available, apply the weights to the optimization
		vnl_vector<double> WeightsForDataPositionsInVNLvectors(0);
		vnl_vector<double> USBeamWidthEuclideanMagAtDataPositionsInVNLvectors(0);
		if( true == mIsUSBeamwidthAndWeightFactorsTableReady )
		{
			// Copy original data and remove outliers 
			std::vector<double> weightsForDataPositions = mWeightsForDataPositions; 
			std::vector<double> usBeamWidthEuclideanMagAtDataPositions = mUSBeamWidthEuclideanMagAtDataPositions;

			for ( std::vector<int>::reverse_iterator rit = mOutlierDataPositions.rbegin();  rit != mOutlierDataPositions.rend(); ++rit )
			{
				weightsForDataPositions.erase( weightsForDataPositions.begin() + *rit );
				usBeamWidthEuclideanMagAtDataPositions.erase( usBeamWidthEuclideanMagAtDataPositions.begin() + *rit );
			}

			if( weightsForDataPositions.size() != TotalNumberOfDataPositionsForCalibration ||
				usBeamWidthEuclideanMagAtDataPositions.size() != TotalNumberOfDataPositionsForCalibration )
			{
				std::cerr << "\n\n" << __FILE__ << "," << __LINE__ << "\n"
					<< ">>>>>>>> In " << mstrScope << "::The number of weights and beamwidth data does NOT match the number of data for calibration!!!  Throw up ...\n";

				throw;
			}

			// Populate the weights to vnl_vector format
			WeightsForDataPositionsInVNLvectors.set_size( TotalNumberOfDataPositionsForCalibration );
			USBeamWidthEuclideanMagAtDataPositionsInVNLvectors.set_size( TotalNumberOfDataPositionsForCalibration );
			for( int i = 0; i < TotalNumberOfDataPositionsForCalibration; i++ )
			{
				WeightsForDataPositionsInVNLvectors.put(i, weightsForDataPositions.at(i));
				USBeamWidthEuclideanMagAtDataPositionsInVNLvectors.put(i, 
					usBeamWidthEuclideanMagAtDataPositions.at(i));
			}

			LeastSquares.setDataWeights( WeightsForDataPositionsInVNLvectors );
		}

		LeastSquares.doOptimization();

		// Output to the resulting matrix
		mTransformUSImageFrame2USProbeFrameMatrix4x4 = LeastSquares.getTransform();

		if ( mAreOutliersRemoved == true )
		{
			return; 
		}

		// eliminate outliers from calibration 
		this->setOutlierFlags(); 

		// do a re-calibration without outliers
		this->calibrate(); 

		// reset the outlier flags 
		this->resetOutlierFlags(); 

		/*
// Thomas Debug:
vnl_vector<double> Row1(4,0);
Row1.put(0, 1.80664e-006 );
Row1.put(1, 0.000106626 );
Row1.put(2, 0 );
Row1.put(3, 0.200802 );
vnl_vector<double> Row2(4,0);
Row1.put(0, -0.000108566 );
Row1.put(1, 7.4053e-007 );
Row1.put(2, 0 );
Row1.put(3, 0.036419 );
vnl_vector<double> Row3(4,0);
Row1.put(0, 8.24461e-006 );
Row1.put(1,  -8.66195e-006 );
Row1.put(2, 0 );
Row1.put(3, -0.0240589 );
vnl_vector<double> Row4(4,0);
Row1.put(0, 0 );
Row1.put(1, 0 );
Row1.put(2, 0 );
Row1.put(3, 1 );
mTransformUSImageFrame2USProbeFrameMatrix4x4.set_size(4,4);
mTransformUSImageFrame2USProbeFrameMatrix4x4.set_row( 0, Row1 );
mTransformUSImageFrame2USProbeFrameMatrix4x4.set_row( 1, Row2 );
mTransformUSImageFrame2USProbeFrameMatrix4x4.set_row( 2, Row3 );
mTransformUSImageFrame2USProbeFrameMatrix4x4.set_row( 3, Row4 );*/

		// Make sure the last row in homogeneous transform is [0 0 0 1]
		vnl_vector<double> lastRow(4,0);
		lastRow.put(3, 1);
		mTransformUSImageFrame2USProbeFrameMatrix4x4.set_row(3, lastRow);

		// Reset the calibration flag
		mHasBeenCalibrated = true;

		// Convert the calibration matrix to parameters
		convertCalibrationResultsToParameters();

		// Log the data pipeline if requested.
		if( true == mIsSystemLogOn )
		{
			std::ofstream SystemLogFile(
				mSystemLogFileNameWithTimeStamp.c_str(), std::ios::app);
			SystemLogFile << " ==========================================================================\n";
			SystemLogFile << " PERFORM CALIBRATION >>>>>>>>>>>>>>>>>>>>>\n\n";
			SystemLogFile << " mTransformOrigImageFrame2TRUSImageFrameMatrix4x4 = \n" << mTransformOrigImageFrame2TRUSImageFrameMatrix4x4 << "\n\n";
			SystemLogFile << " DataPositionsInUSImageFrameMatrix4xN = \n" << DataPositionsInUSImageFrameMatrix4xN << "\n";
			SystemLogFile << " DataPositionsInUSProbeFrameMatrix4xN = \n" << DataPositionsInUSProbeFrameMatrix4xN << "\n";
			SystemLogFile << " -----------------------------------------------------------------------------------\n";
			if( true == mIsUSBeamwidthAndWeightFactorsTableReady )
			{
				SystemLogFile << " USBeamWidthEuclideanMagAtDataPositionsInVNLvectors = \n" << USBeamWidthEuclideanMagAtDataPositionsInVNLvectors << "\n\n";
				SystemLogFile << " WeightsForDataPositionsInVNLvectors = \n" << WeightsForDataPositionsInVNLvectors << "\n\n";
				SystemLogFile << " -----------------------------------------------------------------------------------\n";
			}
			SystemLogFile << " mTransformUSImageFrame2USProbeFrameMatrix4x4 = \n" << mTransformUSImageFrame2USProbeFrameMatrix4x4 << "\n";
			SystemLogFile << " -----------------------------------------------------------------------------------\n";
			SystemLogFile << "Convert to mTransformUSImageFrame2USProbeFrameParameters = \n";
			SystemLogFile << "FORMAT: [Alpha, Beta, Gamma (in radians); Sx, Sy; Tx, Ty, Tz]\n";
			SystemLogFile << mTransformUSImageFrame2USProbeFrameParameters[0] << ", "
				<< mTransformUSImageFrame2USProbeFrameParameters[1] << ", "
				<< mTransformUSImageFrame2USProbeFrameParameters[2] << ";  "
				<< mTransformUSImageFrame2USProbeFrameParameters[3] << ", "
				<< mTransformUSImageFrame2USProbeFrameParameters[4] << ";  "
				<< mTransformUSImageFrame2USProbeFrameParameters[5] << ", "
				<< mTransformUSImageFrame2USProbeFrameParameters[6] << ", "
				<< mTransformUSImageFrame2USProbeFrameParameters[7] << ".\n\n";
			SystemLogFile.close();
		}
	}
	catch(...)
	{
		std::cerr << "\n\n" << __FILE__ << "," << __LINE__ << "\n"
			<< ">>>>>>>> In " << mstrScope << "::Failed to finish the calibration task!!!  Throw up ...\n";

		throw;
	}
}

vnl_matrix<double> Phantom::getCalibrationResultsInMatrix() const
{
	if( mHasBeenCalibrated != true )
	{
		std::cerr << "\n\n" << __FILE__ << "," << __LINE__ << "\n"
			<< ">>>>>>>> In " << mstrScope << "::This operation is not possible since the calibration is not yet finished!!!  Throw up ...\n";

		throw;
	}

	return mTransformUSImageFrame2USProbeFrameMatrix4x4;
}

std::string Phantom::getCalibrationTimeStampInString()
{
	return mSystemTimestampInString;
}

std::string Phantom::getCalibrationTimeStampWithFormat()
{
	return mSystemTimestampWithFormat;
}

std::vector<double> Phantom::getCalibrationResultsInParameters()
{
	if( mHasBeenCalibrated != true )
	{
		std::cerr << "\n\n" << __FILE__ << "," << __LINE__ << "\n"
			<< ">>>>>>>> In " << mstrScope 
			<< "::This operation is not possible since the calibration is not yet finished!!!  Throw up ...\n";

		throw;
	}

	return mTransformUSImageFrame2USProbeFrameParameters;
}

void Phantom::convertCalibrationResultsToParameters ()
{
	if( mHasBeenCalibrated != true )
	{
		std::cerr << "\n\n" << __FILE__ << "," << __LINE__ << "\n"
			<< ">>>>>>>> In " << mstrScope 
			<< "::This operation is not possible since the calibration is not yet finished!!!  Throw up ...\n";

		throw;
	}

	mTransformUSImageFrame2USProbeFrameParameters = 
		convertHomogeneousMatrixToParameters( mTransformUSImageFrame2USProbeFrameMatrix4x4 );
}

std::vector<double> Phantom::convertHomogeneousMatrixToParameters(
	const vnl_matrix<double> HomogeneousTransformMatrix4x4 )
{
	try
	{
		// Calculate the Rotation-Translation-Scaling (8 parameters)
		
		// 1. EulerZYX Angles (all angles are in radians)
		// ==============================================
		// Alpha, Beta, Gamma: Rotation in EulerZYX angles are performed in
		// the following order w.r.t. the current frame (frmae 0-1-2):
		// - 1st, rotation about current Z0 axis of angle Alpha;
		// - 2nd, rotation about current Y1 axis of angle Beta;
		// - 3rd, rotation about current X2 axis of angle Gamma;
		// - These are equvalent to rotate about the original fixed frame, 
		//   X0-Y0-Z0, but in a reversed order, by Gamma-Beta-Alpha.

		// First, we will normalize the column vectors in the rotation part (3x3)
		// of the input matrix to elimate the influnce from the scaling factor. 
		// NOTE: 
		// - The translation part (the 4th column) remains untouched.
		// - Because 3rd column of the calibration matrix is all zero, we will
		//   generate a 3rd vector from cross-product of the 1st and 2nd column.
		vnl_vector<double> Column1st(3,0);
		vnl_vector<double> Column2nd(3,0);
		Column1st.put(0, HomogeneousTransformMatrix4x4.get_column(0).get(0));
		Column1st.put(1, HomogeneousTransformMatrix4x4.get_column(0).get(1));
		Column1st.put(2, HomogeneousTransformMatrix4x4.get_column(0).get(2));
		Column2nd.put(0, HomogeneousTransformMatrix4x4.get_column(1).get(0));
		Column2nd.put(1, HomogeneousTransformMatrix4x4.get_column(1).get(1));
		Column2nd.put(2, HomogeneousTransformMatrix4x4.get_column(1).get(2));
		vnl_vector<double> Column3rdNormalized(4,0);
		Column3rdNormalized.put(0, vnl_cross_3d(Column1st, Column2nd).normalize().get(0));
		Column3rdNormalized.put(1, vnl_cross_3d(Column1st, Column2nd).normalize().get(1));
		Column3rdNormalized.put(2, vnl_cross_3d(Column1st, Column2nd).normalize().get(2));
		Column3rdNormalized.put(3, 0);

		vnl_matrix <double> TransformMatrixRotationNormalized(4,4);
		TransformMatrixRotationNormalized.set_column(0, HomogeneousTransformMatrix4x4.get_column(0).normalize());
		TransformMatrixRotationNormalized.set_column(1, HomogeneousTransformMatrix4x4.get_column(1).normalize());
		TransformMatrixRotationNormalized.set_column(2, Column3rdNormalized);
		TransformMatrixRotationNormalized.set_column(3, HomogeneousTransformMatrix4x4.get_column(3));

		// Alpha: cos(Beta) > 0
		double Alpha = atan2( 
			TransformMatrixRotationNormalized.get(1,0), 
			TransformMatrixRotationNormalized.get(0,0) );
		// Beta: in the region of (-pi/2, pi/2) or cos(Beta) > 0
		double Beta = atan2( 
			-TransformMatrixRotationNormalized.get(2,0), 
			sqrt( pow(TransformMatrixRotationNormalized.get(0,0),2) + 
			pow(TransformMatrixRotationNormalized.get(1,0),2) ) );
		// Gamma: 
		// We found using the 2nd column alone to solve Gamma is not reliable
		// and inconsistant in results.  Therefore as in most literature
		// documented, we use r21 and r22 to solve Gamma. 
		double Gamma = atan2( 
			TransformMatrixRotationNormalized.get(2,1), 
			TransformMatrixRotationNormalized.get(2,2) );

		// 2. Scaling Factors (in meters/pixel)
		// ====================================
		double Sx = HomogeneousTransformMatrix4x4.get(2,0)/( -sin( Beta ) );
		double Sy = HomogeneousTransformMatrix4x4.get(2,1)/( cos( Beta )*sin( Gamma ) );

		// 3. Translation of Origin (in meters)
		// ====================================
		double Tx = HomogeneousTransformMatrix4x4.get(0,3);
		double Ty = HomogeneousTransformMatrix4x4.get(1,3);
		double Tz = HomogeneousTransformMatrix4x4.get(2,3);

		// Finalizing
		// ==========
		// 0-Alpha, 1-Beta, 2-Gamma, 3-Sx, 4-Sy, 5-Tx, 6-Ty, 7-Tz 
		std::vector<double> HomogeneousTransformInParameters;

		HomogeneousTransformInParameters.push_back(Alpha);
		HomogeneousTransformInParameters.push_back(Beta);
		HomogeneousTransformInParameters.push_back(Gamma);
		HomogeneousTransformInParameters.push_back(Sx);
		HomogeneousTransformInParameters.push_back(Sy);
		HomogeneousTransformInParameters.push_back(Tx);
		HomogeneousTransformInParameters.push_back(Ty);
		HomogeneousTransformInParameters.push_back(Tz);

		return HomogeneousTransformInParameters;
	}
	catch(...)
	{
		std::cerr << "\n\n" << __FILE__ << "," << __LINE__ << "\n"
			<< ">>>>>>>> In " << mstrScope 
			<< "::Failed to convert the homogeneous transform matrix to parametric format!!!  Throw up ...\n";

		throw;
	}
}


std::vector<double> Phantom::getPRE3DAnalysis4ValidationDataSet () const
{
	if( 0 == mAbsPRE3DAnalysis4ValidationPositionsInUSProbeFrame.size() )
	{
		std::cerr << "\n\n" << __FILE__ << "," << __LINE__ << "\n"
			<< ">>>>>>>> In " << mstrScope << "::This operation is not possible since the PRE3D is not ready!!!  Throw up ...\n";

		throw;
	}

	return mAbsPRE3DAnalysis4ValidationPositionsInUSProbeFrame;
}

std::vector<double> Phantom::getPLDEAnalysis4ValidationDataSet () const
{
	if( 0 == mPLDEAnalysis4ValidationPositionsInUSProbeFrame.size() )
	{
		std::cerr << "\n\n" << __FILE__ << "," << __LINE__ << "\n"
			<< ">>>>>>>> In " << mstrScope << "::This operation is not possible since the PLDE is not ready!!!  Throw up ...\n";

		throw;
	}

	return mPLDEAnalysis4ValidationPositionsInUSProbeFrame;
}

vnl_matrix <double> Phantom::getPRE3DsRAWMatrixforValidationDataSet4xN () const
{
	if( mArePRE3DsForValidationPositionsReady != true )
	{
		std::cerr << "\n\n" << __FILE__ << "," << __LINE__ << "\n"
			<< ">>>>>>>> In " << mstrScope << "::This operation is not possible since the PRE3Ds' matrix is not ready!!!  Throw up ...\n";

		throw;
	}

	return mRawPRE3DsforValidationPositionsInUSProbeFrameMatrix4xN;
}

vnl_matrix <double> Phantom::getSortedPRE3DsRAWMatrixforValidationDataSet4xN () const
{
	if( mArePRE3DsForValidationPositionsReady != true )
	{
		std::cerr << "\n\n" << __FILE__ << "," << __LINE__ << "\n"
			<< ">>>>>>>> In " << mstrScope << "::This operation is not possible since the PRE3Ds' matrix is not ready!!!  Throw up ...\n";

		throw;
	}

	return mSortedRawPRE3DsInAscendingOrderInUSProbeFrameMatrix4xN;
}

vnl_vector<double> Phantom::getOrigPLDEsforValidationDataSet () const
{
	if( mAreIndependentPointLineReconErrorsReady != true )
	{
		std::cerr << "\n\n" << __FILE__ << "," << __LINE__ << "\n"
			<< ">>>>>>>> In " << mstrScope << "::This operation is not possible since the PLDEs' vector is not ready!!!  Throw up ...\n";

		throw;
	}

	return mPLDEsforValidationPositionsInUSProbeFrame;
}

vnl_vector<double> Phantom::getSortedPLDEsforValidationDataSet () const
{
	if( mAreIndependentPointLineReconErrorsReady != true )
	{
		std::cerr << "\n\n" << __FILE__ << "," << __LINE__ << "\n"
			<< ">>>>>>>> In " << mstrScope << "::This operation is not possible since the PLDEs' vector is not ready!!!  Throw up ...\n";

		throw;
	}

	return mSortedPLDEsAscendingforValidationInUSProbeFrame;
}

std::vector<Phantom::vnl_vector_double> Phantom::getValidationDataSetInUSImageFrame () const
{
	if ( mAreValidationPositionsReady != true )
	{
		std::cerr << "\n\n" << __FILE__ << "," << __LINE__ << "\n"
			<< ">>>>>>>> In " << mstrScope << "::This operation is not possible since the validation data set is not ready!!!  Throw up ...\n";

		throw;
	}

	return mValidationPositionsInUSImageFrame;
}

vnl_matrix <double> Phantom::getLREOrigMatrix4xNForNWire1 () const
{
	if( mAreIndependentPointLineReconErrorsReady != true )
	{
		std::cerr << "\n\n" << __FILE__ << "," << __LINE__ << "\n"
			<< ">>>>>>>> In " << mstrScope << "::This operation is not possible since the data matrix is not ready!!!  Throw up ...\n";

		throw;
	}

	return mNWire1LREOrigInUSProbeFrameMatrix4xN;
}

vnl_matrix <double> Phantom::getLREOrigMatrix4xNForNWire3 () const
{
	if( mAreIndependentPointLineReconErrorsReady != true )
	{
		std::cerr << "\n\n" << __FILE__ << "," << __LINE__ << "\n"
			<< ">>>>>>>> In " << mstrScope << "::This operation is not possible since the data matrix is not ready!!!  Throw up ...\n";

		throw;
	}

	return mNWire3LREOrigInUSProbeFrameMatrix4xN;
}

vnl_matrix <double> Phantom::getLREOrigMatrix4xNForNWire4 () const
{
	if( mAreIndependentPointLineReconErrorsReady != true )
	{
		std::cerr << "\n\n" << __FILE__ << "," << __LINE__ << "\n"
			<< ">>>>>>>> In " << mstrScope << "::This operation is not possible since the data matrix is not ready!!!  Throw up ...\n";

		throw;
	}

	return mNWire4LREOrigInUSProbeFrameMatrix4xN;
}

vnl_matrix <double> Phantom::getLREOrigMatrix4xNForNWire6 () const
{
	if( mAreIndependentPointLineReconErrorsReady != true )
	{
		std::cerr << "\n\n" << __FILE__ << "," << __LINE__ << "\n"
			<< ">>>>>>>> In " << mstrScope << "::This operation is not possible since the data matrix is not ready!!!  Throw up ...\n";

		throw;
	}

	return mNWire6LREOrigInUSProbeFrameMatrix4xN;
}

vnl_matrix <double> Phantom::getLRESortedAscendingMatrix4xNForNWire1 () const
{
	if( mAreIndependentPointLineReconErrorsReady != true )
	{
		std::cerr << "\n\n" << __FILE__ << "," << __LINE__ << "\n"
			<< ">>>>>>>> In " << mstrScope << "::This operation is not possible since the data matrix is not ready!!!  Throw up ...\n";

		throw;
	}

	return mNWire1LRESortedAscendingInUSProbeFrameMatrix4xN;
}

vnl_matrix <double> Phantom::getLRESortedAscendingMatrix4xNForNWire3 () const
{
	if( mAreIndependentPointLineReconErrorsReady != true )
	{
		std::cerr << "\n\n" << __FILE__ << "," << __LINE__ << "\n"
			<< ">>>>>>>> In " << mstrScope << "::This operation is not possible since the data matrix is not ready!!!  Throw up ...\n";

		throw;
	}

	return mNWire3LRESortedAscendingInUSProbeFrameMatrix4xN;
}

vnl_matrix <double> Phantom::getLRESortedAscendingMatrix4xNForNWire4 () const
{
	if( mAreIndependentPointLineReconErrorsReady != true )
	{
		std::cerr << "\n\n" << __FILE__ << "," << __LINE__ << "\n"
			<< ">>>>>>>> In " << mstrScope << "::This operation is not possible since the data matrix is not ready!!!  Throw up ...\n";

		throw;
	}

	return mNWire4LRESortedAscendingInUSProbeFrameMatrix4xN;
}

vnl_matrix <double> Phantom::getLRESortedAscendingMatrix4xNForNWire6 () const
{
	if( mAreIndependentPointLineReconErrorsReady != true )
	{
		std::cerr << "\n\n" << __FILE__ << "," << __LINE__ << "\n"
			<< ">>>>>>>> In " << mstrScope << "::This operation is not possible since the data matrix is not ready!!!  Throw up ...\n";

		throw;
	}

	return mNWire6LRESortedAscendingInUSProbeFrameMatrix4xN;
}

std::vector<double> Phantom::getLREAbsAnalysisForNWire1 () const
{
	if( mAreIndependentPointLineReconErrorsReady != true )
	{
		std::cerr << "\n\n" << __FILE__ << "," << __LINE__ << "\n"
			<< ">>>>>>>> In " << mstrScope << "::This operation is not possible since the data matrix is not ready!!!  Throw up ...\n";

		throw;
	}

	return mNWire1AbsLREAnalysisInUSProbeFrame;
}

std::vector<double> Phantom::getLREAbsAnalysisForNWire3 () const
{
	if( mAreIndependentPointLineReconErrorsReady != true )
	{
		std::cerr << "\n\n" << __FILE__ << "," << __LINE__ << "\n"
			<< ">>>>>>>> In " << mstrScope << "::This operation is not possible since the data matrix is not ready!!!  Throw up ...\n";

		throw;
	}

	return mNWire3AbsLREAnalysisInUSProbeFrame;
}

std::vector<double> Phantom::getLREAbsAnalysisForNWire4 () const
{
	if( mAreIndependentPointLineReconErrorsReady != true )
	{
		std::cerr << "\n\n" << __FILE__ << "," << __LINE__ << "\n"
			<< ">>>>>>>> In " << mstrScope << "::This operation is not possible since the data matrix is not ready!!!  Throw up ...\n";

		throw;
	}

	return mNWire4AbsLREAnalysisInUSProbeFrame;
}

std::vector<double> Phantom::getLREAbsAnalysisForNWire6 () const
{
	if( mAreIndependentPointLineReconErrorsReady != true )
	{
		std::cerr << "\n\n" << __FILE__ << "," << __LINE__ << "\n"
			<< ">>>>>>>> In " << mstrScope << "::This operation is not possible since the data matrix is not ready!!!  Throw up ...\n";

		throw;
	}

	return mNWire6AbsLREAnalysisInUSProbeFrame;
}

void Phantom::compute3DPointReconstructionError ()
{
	if ( mAreValidationPositionsReady != true )
	{
		std::cerr << "\n\n" << __FILE__ << "," << __LINE__ << "\n"
			<< ">>>>>>>> In " << mstrScope << "::This operation is not possible since the validation data set is not ready!!!  Throw up ...\n";

		throw;
	}

	// Reset the flag
	if( true == mArePRE3DsForValidationPositionsReady )
	{
		mArePRE3DsForValidationPositionsReady = false;
	}

	try
	{
		// Populate the validation data set matrices if it's not yet been done
		// ===================================================================
		// NOTE: since the validation data set is separated from the calibration 
		//       data and acquired before the calibration, the construction of
		//       the validation data matrices needs to be done once and only 
		//       once (e.g., before the 1st validation task is performed) with
		//       the flag set to true.  This would save the system runtime
		//       during the iterative calibration/validation process.
		if( false == mAreValidationDataMatricesConstructed )
		{
			constructValidationDataMatrices ();
		}

		// The total number of the validation data set.
		const int NumberOfValidationPositions( mValidationPositionsInUSImageFrame.size() );

		// Calculate 3D point reconstruction error (PRE3D) in the US Probe Frame
		// ======================================================================

		// Using the calibration matrix, we can obtain the projected positions
		// in the US probe frame after applying the calibration matrix to
		// the validation positions in the US image frame.
		vnl_matrix<double> ProjectedPositionsInUSProbeFrameMatrix4xN =
			mTransformUSImageFrame2USProbeFrameMatrix4x4 * 
			mValidationPositionsInUSImageFrameMatrix4xN;

		// Obtain the PRE3D matrix:
		// PRE3D_matrix = ( Projected - True ) positions in US probe frame
		mRawPRE3DsforValidationPositionsInUSProbeFrameMatrix4xN = 
			ProjectedPositionsInUSProbeFrameMatrix4xN - 
			mValidationPositionsInUSProbeFrameMatrix4xN;

		// Make sure the last row (4th) in the PRE3D matrix is all zeros
		const vnl_vector<double> AllZerosVector( NumberOfValidationPositions, 0 );
		mRawPRE3DsforValidationPositionsInUSProbeFrameMatrix4xN.set_row(3, AllZerosVector);

		// Sort the PRE3D in an ascending order with respect to the euclidean PRE3D distance
		// =============================================================================

		mSortedRawPRE3DsInAscendingOrderInUSProbeFrameMatrix4xN =
			mRawPRE3DsforValidationPositionsInUSProbeFrameMatrix4xN;

		// First calculate the euclidean PRE3D distance from PRE3D
		// vector and store them in the fourth row of the matrix.
		for( int i = 0; i < NumberOfValidationPositions; i++ )
		{
			mSortedRawPRE3DsInAscendingOrderInUSProbeFrameMatrix4xN.put(
				3, i, 
				mRawPRE3DsforValidationPositionsInUSProbeFrameMatrix4xN.get_column(i).magnitude() );
		}

		// Populate the last row of the raw PRE3D matrix with the euclidean distance as well.
		mRawPRE3DsforValidationPositionsInUSProbeFrameMatrix4xN.set_row( 3, 
			mSortedRawPRE3DsInAscendingOrderInUSProbeFrameMatrix4xN.get_row(3) );

		// Sorting the PRE3D matrix w.r.t the  euclidean PRE3D distance (the 4th row)
		// Sorting algorithm employed: Insertion Sort
		for( int i = 0; i < NumberOfValidationPositions; i++ )
			for( int j = i; j > 0 && 
				mSortedRawPRE3DsInAscendingOrderInUSProbeFrameMatrix4xN.get(3, j-1) > 
				mSortedRawPRE3DsInAscendingOrderInUSProbeFrameMatrix4xN.get(3, j); j-- )
			{
				// Swap the positions of j-th and j-1-th elements
				const vnl_vector<double> SwapPRE3DColumnVector = 
					mSortedRawPRE3DsInAscendingOrderInUSProbeFrameMatrix4xN.get_column(j-1);
				
				mSortedRawPRE3DsInAscendingOrderInUSProbeFrameMatrix4xN.set_column( j-1, 
					mSortedRawPRE3DsInAscendingOrderInUSProbeFrameMatrix4xN.get_column(j) );

				mSortedRawPRE3DsInAscendingOrderInUSProbeFrameMatrix4xN.set_column( j, 
					SwapPRE3DColumnVector ); 
			}

		// We are only interested at the top-ranked ascending PRE3D values
		// presented in the sorted validation result matrix.
		// Default percentage: 95%
		const int NumberOfTopRankedCalibrationData = 
			ROUND( (double)NumberOfValidationPositions * mValidationDataConfidenceLevel );

		// We only need absolute values of PRE3D to average
		vnl_matrix<double> AbsoluteTopRankedPRE3DInUSProbeFrameMatrix4xN = 
			mSortedRawPRE3DsInAscendingOrderInUSProbeFrameMatrix4xN.get_n_columns( 
			0, NumberOfTopRankedCalibrationData ).apply( fabs );

		// Obtain PRE3Ds in X-, Y-, and Z-axis
		vnl_vector<double> AbsPRE3DsinX =  
			AbsoluteTopRankedPRE3DInUSProbeFrameMatrix4xN.get_row( 0 );
		vnl_vector<double> AbsPRE3DsinY =  
			AbsoluteTopRankedPRE3DInUSProbeFrameMatrix4xN.get_row( 1 );
		vnl_vector<double> AbsPRE3DsinZ =  
			AbsoluteTopRankedPRE3DInUSProbeFrameMatrix4xN.get_row( 2 );

		// Perform statistical analysis
		// =============================

		// X-axis PRE3D statistics
		double PRE3D_X_mean = AbsPRE3DsinX.mean();
		double PRE3D_X_rms = AbsPRE3DsinX.rms();
		double PRE3D_X_std = sqrt( 
			1/(double)( NumberOfTopRankedCalibrationData - 1 ) *	
			( AbsPRE3DsinX.squared_magnitude() - NumberOfTopRankedCalibrationData*pow(PRE3D_X_mean,2) )
			);

		// Y-axis PRE3D statistics
		double PRE3D_Y_mean = AbsPRE3DsinY.mean();
		double PRE3D_Y_rms = AbsPRE3DsinY.rms();
		double PRE3D_Y_std = sqrt( 
			1/(double)( NumberOfTopRankedCalibrationData - 1 ) *	
			( AbsPRE3DsinY.squared_magnitude() - NumberOfTopRankedCalibrationData*pow(PRE3D_Y_mean,2) )
			);

		// Z-axis PRE3D statistics
		double PRE3D_Z_mean = AbsPRE3DsinZ.mean();
		double PRE3D_Z_rms = AbsPRE3DsinZ.rms();
		double PRE3D_Z_std = sqrt( 
			1/(double)( NumberOfTopRankedCalibrationData - 1 ) *	
			( AbsPRE3DsinZ.squared_magnitude() - NumberOfTopRankedCalibrationData*pow(PRE3D_Z_mean,2) )
			);
		
		// Consolidate the returning data container
		// =========================================

		// FORMAT: (all positions are in the US probe frame)
		// [ vector 0 - 2:  PRE3D_X_mean, PRE3D_X_rms, PRE3D_X_std ]
		// [ vector 3 - 5:  PRE3D_Y_mean, PRE3D_Y_rms, PRE3D_Y_std ]
		// [ vector 6 - 8:  PRE3D_Z_mean, PRE3D_Z_rms, PRE3D_Z_std ]
		// [ vector 9	 :	Validation data confidence level ]
		// where: 
		// - mean: linearly averaged;
		// - rms : root mean square;
		// - std : standard deviation.
		mAbsPRE3DAnalysis4ValidationPositionsInUSProbeFrame.resize(0);
		mAbsPRE3DAnalysis4ValidationPositionsInUSProbeFrame.push_back( PRE3D_X_mean );
		mAbsPRE3DAnalysis4ValidationPositionsInUSProbeFrame.push_back( PRE3D_X_rms );
		mAbsPRE3DAnalysis4ValidationPositionsInUSProbeFrame.push_back( PRE3D_X_std );

		mAbsPRE3DAnalysis4ValidationPositionsInUSProbeFrame.push_back( PRE3D_Y_mean );
		mAbsPRE3DAnalysis4ValidationPositionsInUSProbeFrame.push_back( PRE3D_Y_rms );
		mAbsPRE3DAnalysis4ValidationPositionsInUSProbeFrame.push_back( PRE3D_Y_std );

		mAbsPRE3DAnalysis4ValidationPositionsInUSProbeFrame.push_back( PRE3D_Z_mean );
		mAbsPRE3DAnalysis4ValidationPositionsInUSProbeFrame.push_back( PRE3D_Z_rms );
		mAbsPRE3DAnalysis4ValidationPositionsInUSProbeFrame.push_back( PRE3D_Z_std );

		mAbsPRE3DAnalysis4ValidationPositionsInUSProbeFrame.push_back( mValidationDataConfidenceLevel );

		// Reset the flag
		mArePRE3DsForValidationPositionsReady = true;

		// Log the data pipeline if requested.
		if( true == mIsSystemLogOn )
		{
			std::ofstream SystemLogFile(
				mSystemLogFileNameWithTimeStamp.c_str(), std::ios::app);
			SystemLogFile << " ==========================================================================\n";
			SystemLogFile << " COMPUTE 3D Point Reconstruction Error (PRE3D) >>>>>>>>>>>>>>>>>>>>>\n\n";
			//SystemLogFile << " ProjectedPositionsInUSProbeFrameMatrix4xN = \n" << ProjectedPositionsInUSProbeFrameMatrix4xN << "\n";
			//SystemLogFile << " -----------------------------------------------------------------------------------\n";
			//SystemLogFile << " mRawPRE3DsforValidationPositionsInUSProbeFrameMatrix4xN = \n" << mRawPRE3DsforValidationPositionsInUSProbeFrameMatrix4xN << "\n";
			//SystemLogFile << " -----------------------------------------------------------------------------------\n";
			//SystemLogFile << " mSortedRawPRE3DsInAscendingOrderInUSProbeFrameMatrix4xN = \n" << mSortedRawPRE3DsInAscendingOrderInUSProbeFrameMatrix4xN << "\n";
			//SystemLogFile << " -----------------------------------------------------------------------------------\n";
			SystemLogFile << "mAbsPRE3DAnalysis4ValidationPositionsInUSProbeFrame = \n";
			SystemLogFile << "[ vector 0 - 2:  PRE3D_X_mean, PRE3D_X_rms, PRE3D_X_std ]\n";
			SystemLogFile << "[ vector 3 - 5:  PRE3D_Y_mean, PRE3D_Y_rms, PRE3D_Y_std ]\n";
			SystemLogFile << "[ vector 6 - 8:  PRE3D_Z_mean, PRE3D_Z_rms, PRE3D_Z_std ]\n";
			SystemLogFile << "[ vector 9    :  Validation data confidence level ]\n";
			SystemLogFile << mAbsPRE3DAnalysis4ValidationPositionsInUSProbeFrame[0] << ", " 
				<< mAbsPRE3DAnalysis4ValidationPositionsInUSProbeFrame[1] << ", "
				<< mAbsPRE3DAnalysis4ValidationPositionsInUSProbeFrame[2] << "\n"
				<< mAbsPRE3DAnalysis4ValidationPositionsInUSProbeFrame[3] << ", " 
				<< mAbsPRE3DAnalysis4ValidationPositionsInUSProbeFrame[4] << ", "
				<< mAbsPRE3DAnalysis4ValidationPositionsInUSProbeFrame[5] << "\n"
				<< mAbsPRE3DAnalysis4ValidationPositionsInUSProbeFrame[6] << ", " 
				<< mAbsPRE3DAnalysis4ValidationPositionsInUSProbeFrame[7] << ", "
				<< mAbsPRE3DAnalysis4ValidationPositionsInUSProbeFrame[8] << "\n\n";
			SystemLogFile << "Validation Data Confidence Level = " << 
				mAbsPRE3DAnalysis4ValidationPositionsInUSProbeFrame[9] << ", or in other words\n";
			SystemLogFile << NumberOfTopRankedCalibrationData << " top-ranked validation data were used out of the total\n" 
				<< NumberOfValidationPositions << " validation data set for the above statistical analysis.\n\n";
			SystemLogFile.close();
		}
	}
	catch(...)
	{
		std::cerr << "\n\n" << __FILE__ << "," << __LINE__ << "\n"
			<< ">>>>>>>> In " << mstrScope << "::Failed to calculate PRE3Ds for the validation data set!!!  Throw up ...\n";

		throw;	
	}
}

vnl_vector<double> Phantom::getPointLineReconstructionError(vnl_vector<double> NWirePositionInUSImageFrame, 
			vnl_vector<double> NWirePositionInUSProbeFrame)
{

	vnl_vector<double> NWireProjectedPositionsInUSProbeFrame =
			mTransformUSImageFrame2USProbeFrameMatrix4x4 * NWirePositionInUSImageFrame;

	vnl_vector<double> NWireLREOrigInUSProbeFrame = 
			NWirePositionInUSProbeFrame - NWireProjectedPositionsInUSProbeFrame;

	NWireLREOrigInUSProbeFrame[3] = 1; 

	return NWireLREOrigInUSProbeFrame; 

}

void Phantom::computeIndependentPointLineReconstructionError ()
{
	if ( mAreValidationPositionsReady != true )
	{
		std::cerr << "\n\n" << __FILE__ << "," << __LINE__ << "\n"
			<< ">>>>>>>> In " << mstrScope << "::This operation is not possible since the validation data set is not ready!!!  Throw up ...\n";

		throw;
	}

	// Reset the flag
	if( true == mAreIndependentPointLineReconErrorsReady )
	{
		mAreIndependentPointLineReconErrorsReady = false;
	}

	try
	{
		// Populate the validation data set matrices if it's not yet been done
		// ===================================================================
		// NOTE: since the validation data set is separated from the calibration 
		//       data and acquired before the calibration, the construction of
		//       the validation data matrices needs to be done once and only 
		//       once (e.g., before the 1st validation task is performed) with
		//       the flag set to true.  This would save the system runtime
		//       during the iterative calibration/validation process.
		if( false == mAreValidationDataMatricesConstructed )
		{
			constructValidationDataMatrices ();
		}

		// The total number of the validation data set.
		const int NumberOfValidationPositions( mValidationPositionsInUSImageFrame.size() );

		// ============================================================================
		// STEP-1. Calculate Parallel-Line Reconstruction Errors in the US Probe Frame
		// NOTES:
		// 1. The parallel lines in use are: Wire N1, N3, N4, and N6;
		// 2. Only X and Y reconstruction errors are computed, since we do not have
		//    ground-truth information along the Z-axis when imaging a line object.
		// ============================================================================

		// Using the calibration matrix, we can obtain the projected positions
		// in the US probe frame after applying the calibration matrix to
		// the validation positions in the US image frame.
		
		// NWire-1 (note only x, y coordinates are of interests to us)
		const vnl_matrix<double> NWire1ProjectedPositionsInUSProbeFrameMatrix4xN =
			mTransformUSImageFrame2USProbeFrameMatrix4x4 * 
			mValidationPositionsNWire1InUSImageFrame4xN;

		// NWire-3 (note only x, y coordinates are of interests to us)
		const vnl_matrix<double> NWire3ProjectedPositionsInUSProbeFrameMatrix4xN =
			mTransformUSImageFrame2USProbeFrameMatrix4x4 * 
			mValidationPositionsNWire3InUSImageFrame4xN;

		// NWire-4 (note only x, y coordinates are of interests to us)
		const vnl_matrix<double> NWire4ProjectedPositionsInUSProbeFrameMatrix4xN =
			mTransformUSImageFrame2USProbeFrameMatrix4x4 * 
			mValidationPositionsNWire4InUSImageFrame4xN;

		// NWire-6 (note only x, y coordinates are of interests to us)
		const vnl_matrix<double> NWire6ProjectedPositionsInUSProbeFrameMatrix4xN =
			mTransformUSImageFrame2USProbeFrameMatrix4x4 * 
			mValidationPositionsNWire6InUSImageFrame4xN;

		// Now we calculate the independent line reconstrution errors (in X and Y axes)
		// for NWires N1, N3, N4, N6.
		// Make sure the 3rd row (z-axis) in the LRE matrix is all zeros
		const vnl_vector<double> AllZerosVector( NumberOfValidationPositions, 0 );

		// NWire-1 LREs
		mNWire1LREOrigInUSProbeFrameMatrix4xN = 
			mValidationPositionsNWire1InUSProbeFrame4xN - 
			NWire1ProjectedPositionsInUSProbeFrameMatrix4xN;
		mNWire1LREOrigInUSProbeFrameMatrix4xN.set_row(2, AllZerosVector);

		// NWire-3 LREs
		mNWire3LREOrigInUSProbeFrameMatrix4xN = 
			mValidationPositionsNWire3InUSProbeFrame4xN - 
			NWire3ProjectedPositionsInUSProbeFrameMatrix4xN;
		mNWire3LREOrigInUSProbeFrameMatrix4xN.set_row(2, AllZerosVector);
		
		// NWire-4 LREs
		mNWire4LREOrigInUSProbeFrameMatrix4xN = 
			mValidationPositionsNWire4InUSProbeFrame4xN - 
			NWire4ProjectedPositionsInUSProbeFrameMatrix4xN;
		mNWire4LREOrigInUSProbeFrameMatrix4xN.set_row(2, AllZerosVector);

		// NWire-6 LREs
		mNWire6LREOrigInUSProbeFrameMatrix4xN = 
			mValidationPositionsNWire6InUSProbeFrame4xN - 
			NWire6ProjectedPositionsInUSProbeFrameMatrix4xN;
		mNWire6LREOrigInUSProbeFrameMatrix4xN.set_row(2, AllZerosVector);

		// Now we calculate the euclidean LRE distance from LRE
		// vector and store them in the fourth row of the matrix.
		for( int i = 0; i < NumberOfValidationPositions; i++ )
		{
			// NWire-1
			mNWire1LREOrigInUSProbeFrameMatrix4xN.put(
				3, i, 
				mNWire1LREOrigInUSProbeFrameMatrix4xN.get_column(i).magnitude() );

			// NWire-3
			mNWire3LREOrigInUSProbeFrameMatrix4xN.put(
				3, i, 
				mNWire3LREOrigInUSProbeFrameMatrix4xN.get_column(i).magnitude() );

			// NWire-4
			mNWire4LREOrigInUSProbeFrameMatrix4xN.put(
				3, i, 
				mNWire4LREOrigInUSProbeFrameMatrix4xN.get_column(i).magnitude() );

			// NWire-6
			mNWire6LREOrigInUSProbeFrameMatrix4xN.put(
				3, i, 
				mNWire6LREOrigInUSProbeFrameMatrix4xN.get_column(i).magnitude() );
		}

		// Sorting the LRE matrice w.r.t the  euclidean distance (the 4th row)
		// Sorting algorithm employed: Insertion Sort

		mNWire1LRESortedAscendingInUSProbeFrameMatrix4xN = 
			mNWire1LREOrigInUSProbeFrameMatrix4xN;
		mNWire3LRESortedAscendingInUSProbeFrameMatrix4xN = 
			mNWire3LREOrigInUSProbeFrameMatrix4xN;
		mNWire4LRESortedAscendingInUSProbeFrameMatrix4xN = 
			mNWire4LREOrigInUSProbeFrameMatrix4xN;
		mNWire6LRESortedAscendingInUSProbeFrameMatrix4xN = 
			mNWire6LREOrigInUSProbeFrameMatrix4xN;

		// NWire-1
		for( int i = 0; i < NumberOfValidationPositions; i++ )
			for( int j = i; j > 0 && 
				mNWire1LRESortedAscendingInUSProbeFrameMatrix4xN.get(3, j-1) > 
				mNWire1LRESortedAscendingInUSProbeFrameMatrix4xN.get(3, j); j-- )
			{
				// Swap the positions of j-th and j-1-th elements
				const vnl_vector<double> Swap3DColumnVector = 
					mNWire1LRESortedAscendingInUSProbeFrameMatrix4xN.get_column(j-1);
				
				mNWire1LRESortedAscendingInUSProbeFrameMatrix4xN.set_column( j-1, 
					mNWire1LRESortedAscendingInUSProbeFrameMatrix4xN.get_column(j) );

				mNWire1LRESortedAscendingInUSProbeFrameMatrix4xN.set_column( j, 
					Swap3DColumnVector ); 
			}
			
		// NWire-3
		for( int i = 0; i < NumberOfValidationPositions; i++ )
			for( int j = i; j > 0 && 
				mNWire3LRESortedAscendingInUSProbeFrameMatrix4xN.get(3, j-1) > 
				mNWire3LRESortedAscendingInUSProbeFrameMatrix4xN.get(3, j); j-- )
			{
				// Swap the positions of j-th and j-1-th elements
				const vnl_vector<double> Swap3DColumnVector = 
					mNWire3LRESortedAscendingInUSProbeFrameMatrix4xN.get_column(j-1);
				
				mNWire3LRESortedAscendingInUSProbeFrameMatrix4xN.set_column( j-1, 
					mNWire3LRESortedAscendingInUSProbeFrameMatrix4xN.get_column(j) );

				mNWire3LRESortedAscendingInUSProbeFrameMatrix4xN.set_column( j, 
					Swap3DColumnVector ); 
			}

		// NWire-4
		for( int i = 0; i < NumberOfValidationPositions; i++ )
			for( int j = i; j > 0 && 
				mNWire4LRESortedAscendingInUSProbeFrameMatrix4xN.get(3, j-1) > 
				mNWire4LRESortedAscendingInUSProbeFrameMatrix4xN.get(3, j); j-- )
			{
				// Swap the positions of j-th and j-1-th elements
				const vnl_vector<double> Swap3DColumnVector = 
					mNWire4LRESortedAscendingInUSProbeFrameMatrix4xN.get_column(j-1);
				
				mNWire4LRESortedAscendingInUSProbeFrameMatrix4xN.set_column( j-1, 
					mNWire4LRESortedAscendingInUSProbeFrameMatrix4xN.get_column(j) );

				mNWire4LRESortedAscendingInUSProbeFrameMatrix4xN.set_column( j, 
					Swap3DColumnVector ); 
			}

		// NWire-6
		for( int i = 0; i < NumberOfValidationPositions; i++ )
			for( int j = i; j > 0 && 
				mNWire6LRESortedAscendingInUSProbeFrameMatrix4xN.get(3, j-1) > 
				mNWire6LRESortedAscendingInUSProbeFrameMatrix4xN.get(3, j); j-- )
			{
				// Swap the positions of j-th and j-1-th elements
				const vnl_vector<double> Swap3DColumnVector = 
					mNWire6LRESortedAscendingInUSProbeFrameMatrix4xN.get_column(j-1);
				
				mNWire6LRESortedAscendingInUSProbeFrameMatrix4xN.set_column( j-1, 
					mNWire6LRESortedAscendingInUSProbeFrameMatrix4xN.get_column(j) );

				mNWire6LRESortedAscendingInUSProbeFrameMatrix4xN.set_column( j, 
					Swap3DColumnVector ); 
			}			

		// NOTES: 
		// 1. We are only interested in the absolute value of the errors.
		// 2. We will perform analysis on all and the top-ranked ascending LRE values
		//    presented in the sorted validation result matrix.  This will effectively 
		//    get rid of outliers in the validation data set (e.g., caused by erroneous 
		//    segmentation results in the validation data).
		// Default percentage: 95%

		const int NumberOfTopRankedData = 
			floor( (double)NumberOfValidationPositions * mValidationDataConfidenceLevel + 0.5 );

		const vnl_matrix<double> NWire1AbsLRETopRankedInUSProbeFrameMatrix4xN = 
			mNWire1LRESortedAscendingInUSProbeFrameMatrix4xN.get_n_columns( 
			0, NumberOfTopRankedData ).apply( fabs );
		const vnl_matrix<double> NWire3AbsLRETopRankedInUSProbeFrameMatrix4xN = 
			mNWire3LRESortedAscendingInUSProbeFrameMatrix4xN.get_n_columns( 
			0, NumberOfTopRankedData ).apply( fabs );
		const vnl_matrix<double> NWire4AbsLRETopRankedInUSProbeFrameMatrix4xN = 
			mNWire4LRESortedAscendingInUSProbeFrameMatrix4xN.get_n_columns( 
			0, NumberOfTopRankedData ).apply( fabs );
		const vnl_matrix<double> NWire6AbsLRETopRankedInUSProbeFrameMatrix4xN = 
			mNWire6LRESortedAscendingInUSProbeFrameMatrix4xN.get_n_columns( 
			0, NumberOfTopRankedData ).apply( fabs );

		// Perform statistical analysis

		// NWire-1: LRE in X, Y and Euclidean (norm)
		// =========================================
		const vnl_vector<double> NWire1AbsLREDistributionInX = 
			NWire1AbsLRETopRankedInUSProbeFrameMatrix4xN.get_row( 0 );
		const vnl_vector<double> NWire1AbsLREDistributionInY = 
			NWire1AbsLRETopRankedInUSProbeFrameMatrix4xN.get_row( 1 );
		const vnl_vector<double> NWire1AbsLREDistributionInEUC = 
			NWire1AbsLRETopRankedInUSProbeFrameMatrix4xN.get_row( 3 );

		const double NWire1LRE_X_mean = NWire1AbsLREDistributionInX.mean();
		const double NWire1LRE_X_std = sqrt( 
			1/(double)( NumberOfTopRankedData - 1 ) *	
			( NWire1AbsLREDistributionInX.squared_magnitude() - 
			NumberOfTopRankedData*pow( NWire1LRE_X_mean,2 )) );

		const double NWire1LRE_Y_mean = NWire1AbsLREDistributionInY.mean();
		const double NWire1LRE_Y_std = sqrt( 
			1/(double)( NumberOfTopRankedData - 1 ) *	
			( NWire1AbsLREDistributionInY.squared_magnitude() - 
			NumberOfTopRankedData*pow( NWire1LRE_Y_mean,2 )) );

		const double NWire1LRE_EUC_mean = NWire1AbsLREDistributionInEUC.mean();
		const double NWire1LRE_EUC_std = sqrt( 
			1/(double)( NumberOfTopRankedData - 1 ) *	
			( NWire1AbsLREDistributionInEUC.squared_magnitude() - 
			NumberOfTopRankedData*pow( NWire1LRE_EUC_mean,2 )) );

		// NWire-3: LRE in X, Y and Euclidean (norm)
		// =========================================
		const vnl_vector<double> NWire3AbsLREDistributionInX = 
			NWire3AbsLRETopRankedInUSProbeFrameMatrix4xN.get_row( 0 );
		const vnl_vector<double> NWire3AbsLREDistributionInY = 
			NWire3AbsLRETopRankedInUSProbeFrameMatrix4xN.get_row( 1 );
		const vnl_vector<double> NWire3AbsLREDistributionInEUC = 
			NWire3AbsLRETopRankedInUSProbeFrameMatrix4xN.get_row( 3 );

		const double NWire3LRE_X_mean = NWire3AbsLREDistributionInX.mean();
		const double NWire3LRE_X_std = sqrt( 
			1/(double)( NumberOfTopRankedData - 1 ) *	
			( NWire3AbsLREDistributionInX.squared_magnitude() - 
			NumberOfTopRankedData*pow( NWire3LRE_X_mean,2 )) );

		const double NWire3LRE_Y_mean = NWire3AbsLREDistributionInY.mean();
		const double NWire3LRE_Y_std = sqrt( 
			1/(double)( NumberOfTopRankedData - 1 ) *	
			( NWire3AbsLREDistributionInY.squared_magnitude() - 
			NumberOfTopRankedData*pow( NWire3LRE_Y_mean,2 )) );

		const double NWire3LRE_EUC_mean = NWire3AbsLREDistributionInEUC.mean();
		const double NWire3LRE_EUC_std = sqrt( 
			1/(double)( NumberOfTopRankedData - 1 ) *	
			( NWire3AbsLREDistributionInEUC.squared_magnitude() - 
			NumberOfTopRankedData*pow( NWire3LRE_EUC_mean,2 )) );						
			
		// NWire-4: LRE in X, Y and Euclidean (norm)
		// =========================================
		const vnl_vector<double> NWire4AbsLREDistributionInX = 
			NWire4AbsLRETopRankedInUSProbeFrameMatrix4xN.get_row( 0 );
		const vnl_vector<double> NWire4AbsLREDistributionInY = 
			NWire4AbsLRETopRankedInUSProbeFrameMatrix4xN.get_row( 1 );
		const vnl_vector<double> NWire4AbsLREDistributionInEUC = 
			NWire4AbsLRETopRankedInUSProbeFrameMatrix4xN.get_row( 3 );

		const double NWire4LRE_X_mean = NWire4AbsLREDistributionInX.mean();
		const double NWire4LRE_X_std = sqrt( 
			1/(double)( NumberOfTopRankedData - 1 ) *	
			( NWire4AbsLREDistributionInX.squared_magnitude() - 
			NumberOfTopRankedData*pow( NWire4LRE_X_mean,2 )) );

		const double NWire4LRE_Y_mean = NWire4AbsLREDistributionInY.mean();
		const double NWire4LRE_Y_std = sqrt( 
			1/(double)( NumberOfTopRankedData - 1 ) *	
			( NWire4AbsLREDistributionInY.squared_magnitude() - 
			NumberOfTopRankedData*pow( NWire4LRE_Y_mean,2 )) );

		const double NWire4LRE_EUC_mean = NWire4AbsLREDistributionInEUC.mean();
		const double NWire4LRE_EUC_std = sqrt( 
			1/(double)( NumberOfTopRankedData - 1 ) *	
			( NWire4AbsLREDistributionInEUC.squared_magnitude() - 
			NumberOfTopRankedData*pow( NWire4LRE_EUC_mean,2 )) );						

		// NWire-6: LRE in X, Y and Euclidean (norm)
		// =========================================
		const vnl_vector<double> NWire6AbsLREDistributionInX = 
			NWire6AbsLRETopRankedInUSProbeFrameMatrix4xN.get_row( 0 );
		const vnl_vector<double> NWire6AbsLREDistributionInY = 
			NWire6AbsLRETopRankedInUSProbeFrameMatrix4xN.get_row( 1 );
		const vnl_vector<double> NWire6AbsLREDistributionInEUC = 
			NWire6AbsLRETopRankedInUSProbeFrameMatrix4xN.get_row( 3 );

		const double NWire6LRE_X_mean = NWire6AbsLREDistributionInX.mean();
		const double NWire6LRE_X_std = sqrt( 
			1/(double)( NumberOfTopRankedData - 1 ) *	
			( NWire6AbsLREDistributionInX.squared_magnitude() - 
			NumberOfTopRankedData*pow( NWire6LRE_X_mean,2 )) );

		const double NWire6LRE_Y_mean = NWire6AbsLREDistributionInY.mean();
		const double NWire6LRE_Y_std = sqrt( 
			1/(double)( NumberOfTopRankedData - 1 ) *	
			( NWire6AbsLREDistributionInY.squared_magnitude() - 
			NumberOfTopRankedData*pow( NWire6LRE_Y_mean,2 )) );

		const double NWire6LRE_EUC_mean = NWire6AbsLREDistributionInEUC.mean();
		const double NWire6LRE_EUC_std = sqrt( 
			1/(double)( NumberOfTopRankedData - 1 ) *	
			( NWire6AbsLREDistributionInEUC.squared_magnitude() - 
			NumberOfTopRankedData*pow( NWire6LRE_EUC_mean,2 )) );									

		// Consolidate the returning data containers
		// =========================================
		// FORMAT: (all positions are in the US probe frame)
		// For NWire N1, N3, N4, N6:
		// [ vector 0 - 1:  LRE_X_mean,   LRE_X_std   ]
		// [ vector 2 - 3:  LRE_Y_mean,   LRE_Y_std   ]
		// [ vector 4 - 5:  LRE_EUC_mean, LRE_EUC_std ]
		// [ vector 6	 :	Validation data confidence level ]
		// where: 
		// - mean: linearly averaged;
		// - std : standard deviation;
		// - EUC: Euclidean (norm) measurement.

		// NWire-1 LRE Analysis
		mNWire1AbsLREAnalysisInUSProbeFrame.resize(0);
		mNWire1AbsLREAnalysisInUSProbeFrame.push_back( NWire1LRE_X_mean );
		mNWire1AbsLREAnalysisInUSProbeFrame.push_back( NWire1LRE_X_std );
		mNWire1AbsLREAnalysisInUSProbeFrame.push_back( NWire1LRE_Y_mean );
		mNWire1AbsLREAnalysisInUSProbeFrame.push_back( NWire1LRE_Y_std );
		mNWire1AbsLREAnalysisInUSProbeFrame.push_back( NWire1LRE_EUC_mean );
		mNWire1AbsLREAnalysisInUSProbeFrame.push_back( NWire1LRE_EUC_std );
		mNWire1AbsLREAnalysisInUSProbeFrame.push_back( mValidationDataConfidenceLevel );

		// NWire-3 LRE Analysis
		mNWire3AbsLREAnalysisInUSProbeFrame.resize(0);
		mNWire3AbsLREAnalysisInUSProbeFrame.push_back( NWire3LRE_X_mean );
		mNWire3AbsLREAnalysisInUSProbeFrame.push_back( NWire3LRE_X_std );
		mNWire3AbsLREAnalysisInUSProbeFrame.push_back( NWire3LRE_Y_mean );
		mNWire3AbsLREAnalysisInUSProbeFrame.push_back( NWire3LRE_Y_std );
		mNWire3AbsLREAnalysisInUSProbeFrame.push_back( NWire3LRE_EUC_mean );
		mNWire3AbsLREAnalysisInUSProbeFrame.push_back( NWire3LRE_EUC_std );
		mNWire3AbsLREAnalysisInUSProbeFrame.push_back( mValidationDataConfidenceLevel );

		// NWire-4 LRE Analysis
		mNWire4AbsLREAnalysisInUSProbeFrame.resize(0);
		mNWire4AbsLREAnalysisInUSProbeFrame.push_back( NWire4LRE_X_mean );
		mNWire4AbsLREAnalysisInUSProbeFrame.push_back( NWire4LRE_X_std );
		mNWire4AbsLREAnalysisInUSProbeFrame.push_back( NWire4LRE_Y_mean );
		mNWire4AbsLREAnalysisInUSProbeFrame.push_back( NWire4LRE_Y_std );
		mNWire4AbsLREAnalysisInUSProbeFrame.push_back( NWire4LRE_EUC_mean );
		mNWire4AbsLREAnalysisInUSProbeFrame.push_back( NWire4LRE_EUC_std );
		mNWire4AbsLREAnalysisInUSProbeFrame.push_back( mValidationDataConfidenceLevel );

		// NWire-6 LRE Analysis
		mNWire6AbsLREAnalysisInUSProbeFrame.resize(0);
		mNWire6AbsLREAnalysisInUSProbeFrame.push_back( NWire6LRE_X_mean );
		mNWire6AbsLREAnalysisInUSProbeFrame.push_back( NWire6LRE_X_std );
		mNWire6AbsLREAnalysisInUSProbeFrame.push_back( NWire6LRE_Y_mean );
		mNWire6AbsLREAnalysisInUSProbeFrame.push_back( NWire6LRE_Y_std );
		mNWire6AbsLREAnalysisInUSProbeFrame.push_back( NWire6LRE_EUC_mean );
		mNWire6AbsLREAnalysisInUSProbeFrame.push_back( NWire6LRE_EUC_std );
		mNWire6AbsLREAnalysisInUSProbeFrame.push_back( mValidationDataConfidenceLevel );		


		// =========================================================================
		// STEP-2. Calculate Point-Line Distance Error (PLDE) in the US Probe Frame
		// =========================================================================

		// Using the calibration matrix, we can obtain the projected positions
		// in the US probe frame after applying the calibration matrix to
		// the validation positions in the US image frame.
		const vnl_matrix<double> ProjectedPositionsInUSProbeFrameMatrix4xN =
			mTransformUSImageFrame2USProbeFrameMatrix4x4 * 
			mValidationPositionsInUSImageFrameMatrix4xN;
		// We need the 3D coordinates (the first 3 elements) for cross product
		const vnl_matrix<double> ProjectedPositionsInUSProbeFrameMatrix3xN =
			ProjectedPositionsInUSProbeFrameMatrix4xN.get_n_rows(0,3);

		// Now we calculate the Point-Line Distance Error (PLDE).  		
		// The PLDE was defined as the absolute point-line distance from the projected
		// positions to the N-Wire (the ground truth), both in the US probe frame.  
		// If there was no error, the PLDE would be zero and the projected postions
		// would reside right on the N-Wire.  The physical position of the N-Wire
		// was measured based on the phantom geometry and converted into the US
		// US probe frame by the optical tracking device affixed on the phantom.
		// NOTE: this data may be used for statistical analysis if desired.
		// FORMAT: vector 1xN (with N being the total number of validation positions)
		mPLDEsforValidationPositionsInUSProbeFrame.set_size( NumberOfValidationPositions );

		for( int i = 0; i < NumberOfValidationPositions; i++ )
		{
			// Fetch the respective points
			const vnl_vector<double> ThisProjectedPosition3D = 
				ProjectedPositionsInUSProbeFrameMatrix3xN.get_column(i);
			const vnl_vector<double> NWireStartPoint3D = 
				mValidationPositionsNWireStartInUSProbeFrame3xN.get_column(i);
			const vnl_vector<double> NWireEndPoint3D = 
				mValidationPositionsNWireEndInUSProbeFrame3xN.get_column(i);

			// Construct the vectors in 3D space
			const vnl_vector<double> NWireVector3D = 
				NWireEndPoint3D - NWireStartPoint3D;
			const vnl_vector<double> ProjectedPosition2NWireStartVector3D =
				ThisProjectedPosition3D - NWireStartPoint3D;

			// Calculate the point-line distance (using triangle-area principle)
			// Formula: x denotes the cross product
			//		|NWireVector3D x ProjectedPosition2NWireStartVector3D|
			// D =	------------------------------------------------------
			//							|NWireVector3D|
			// 
			const double ProjectedPosition2NWireDistance = 
				vnl_cross_3d(NWireVector3D,ProjectedPosition2NWireStartVector3D).magnitude()/
				NWireVector3D.magnitude();

			mPLDEsforValidationPositionsInUSProbeFrame.put(i,
				ProjectedPosition2NWireDistance );

		}

		// Sort the PLDE in an ascending order
		// ===================================

		mSortedPLDEsAscendingforValidationInUSProbeFrame = 
			mPLDEsforValidationPositionsInUSProbeFrame;

		// Sorting algorithm employed: Insertion Sort
		for( int i = 0; i < NumberOfValidationPositions; i++ )
			for( int j = i; j > 0 && 
				mSortedPLDEsAscendingforValidationInUSProbeFrame.get(j-1) > 
				mSortedPLDEsAscendingforValidationInUSProbeFrame.get(j); j-- )
			{
				// Swap the positions of j-th and j-1-th elements
				const double SwapPLDEValue = 
					mSortedPLDEsAscendingforValidationInUSProbeFrame.get(j-1);
				
				mSortedPLDEsAscendingforValidationInUSProbeFrame.put( j-1, 
					mSortedPLDEsAscendingforValidationInUSProbeFrame.get(j) );

				mSortedPLDEsAscendingforValidationInUSProbeFrame.put( j, 
					SwapPLDEValue ); 

			}

		// We are only interested at the top-ranked ascending PLDE values
		// presented in the sorted PLDE result vector.
		// Default percentage: 95%
		const int NumberOfTopRankedCalibrationData = 
			ROUND( (double)NumberOfValidationPositions * mValidationDataConfidenceLevel );
		const vnl_vector<double> AscendingTopRankingPLDEsinUSProbeFrame = 
			mSortedPLDEsAscendingforValidationInUSProbeFrame.extract(
				NumberOfTopRankedCalibrationData );

		// Perform statistical analysis
		// =============================
		const double PLDE_mean = AscendingTopRankingPLDEsinUSProbeFrame.mean();
		const double PLDE_rms = AscendingTopRankingPLDEsinUSProbeFrame.rms();
		const double PLDE_std = sqrt( 
			1/(double)( NumberOfTopRankedCalibrationData - 1 ) *	
			( AscendingTopRankingPLDEsinUSProbeFrame.squared_magnitude() - 
			NumberOfTopRankedCalibrationData*pow(PLDE_mean,2) ) );

		// Consolidate the returning data container
		// =========================================

		// FORMAT: (all positions are in the US probe frame)
		// [ vector 0 - 2:  PLDE_mean, PLDE_rms, PLDE_std ]
		// [ vector 3	 :	Validation data confidence level ]
		// where: 
		// - mean: linearly averaged;
		// - rms : root mean square;
		// - std : standard deviation.
		// - validation data confidence level: this is a percentage of 
		//   the independent validation data used to produce the final
		//   validation results.  It serves as an effective way to get 
		//   rid of corrupted data (or outliers) in the validation 
		//   dataset.  Default value: 0.95 (or 95%), meaning the top 
		//   ranked 95% of the ascendingly-ordered results from the 
		//   validation data would be accepted as the valid error values.
		mPLDEAnalysis4ValidationPositionsInUSProbeFrame.resize(0);
		mPLDEAnalysis4ValidationPositionsInUSProbeFrame.push_back( PLDE_mean );
		mPLDEAnalysis4ValidationPositionsInUSProbeFrame.push_back( PLDE_rms );
		mPLDEAnalysis4ValidationPositionsInUSProbeFrame.push_back( PLDE_std );
		mPLDEAnalysis4ValidationPositionsInUSProbeFrame.push_back( mValidationDataConfidenceLevel );

		// Reset the flag
		mAreIndependentPointLineReconErrorsReady = true;

		// Log the data pipeline if requested.
		if( true == mIsSystemLogOn )
		{
			std::ofstream SystemLogFile(
				mSystemLogFileNameWithTimeStamp.c_str(), std::ios::app);

			SystemLogFile << " ==========================================================================\n";
			SystemLogFile << " COMPUTE FINAL 3D Point Reconstruction Error (PRE3D) >>>>>>>>>>>>>>>>>>>>>\n\n";
			SystemLogFile << " mRawPRE3DsforValidationPositionsInUSProbeFrameMatrix4xN = \n" << mRawPRE3DsforValidationPositionsInUSProbeFrameMatrix4xN << "\n";
			SystemLogFile << " -----------------------------------------------------------------------------------\n";
			SystemLogFile << " mSortedRawPRE3DsInAscendingOrderInUSProbeFrameMatrix4xN = \n" << mSortedRawPRE3DsInAscendingOrderInUSProbeFrameMatrix4xN << "\n";
			SystemLogFile << " -----------------------------------------------------------------------------------\n";
			SystemLogFile << "mAbsPRE3DAnalysis4ValidationPositionsInUSProbeFrame = \n";
			SystemLogFile << "[ vector 0 - 2:  PRE3D_X_mean, PRE3D_X_rms, PRE3D_X_std ]\n";
			SystemLogFile << "[ vector 3 - 5:  PRE3D_Y_mean, PRE3D_Y_rms, PRE3D_Y_std ]\n";
			SystemLogFile << "[ vector 6 - 8:  PRE3D_Z_mean, PRE3D_Z_rms, PRE3D_Z_std ]\n";
			SystemLogFile << "[ vector 9    :  Validation data confidence level ]\n";
			SystemLogFile << mAbsPRE3DAnalysis4ValidationPositionsInUSProbeFrame[0] << ", " 
				<< mAbsPRE3DAnalysis4ValidationPositionsInUSProbeFrame[1] << ", "
				<< mAbsPRE3DAnalysis4ValidationPositionsInUSProbeFrame[2] << "\n"
				<< mAbsPRE3DAnalysis4ValidationPositionsInUSProbeFrame[3] << ", " 
				<< mAbsPRE3DAnalysis4ValidationPositionsInUSProbeFrame[4] << ", "
				<< mAbsPRE3DAnalysis4ValidationPositionsInUSProbeFrame[5] << "\n"
				<< mAbsPRE3DAnalysis4ValidationPositionsInUSProbeFrame[6] << ", " 
				<< mAbsPRE3DAnalysis4ValidationPositionsInUSProbeFrame[7] << ", "
				<< mAbsPRE3DAnalysis4ValidationPositionsInUSProbeFrame[8] << "\n\n";
			SystemLogFile << "Validation Data Confidence Level = " << 
				mAbsPRE3DAnalysis4ValidationPositionsInUSProbeFrame[9] << ".\n\n";

			SystemLogFile << " ==========================================================================\n";
			SystemLogFile << " COMPUTE Independent NWire Line Reconstruction Error (LRE) >>>>>>>>>>>>>>>>>>>>>\n\n";
			SystemLogFile << " NWire1ProjectedPositionsInUSProbeFrameMatrix4xN = \n" << NWire1ProjectedPositionsInUSProbeFrameMatrix4xN << "\n";
			SystemLogFile << " NWire3ProjectedPositionsInUSProbeFrameMatrix4xN = \n" << NWire3ProjectedPositionsInUSProbeFrameMatrix4xN << "\n";
			SystemLogFile << " NWire4ProjectedPositionsInUSProbeFrameMatrix4xN = \n" << NWire4ProjectedPositionsInUSProbeFrameMatrix4xN << "\n";
			SystemLogFile << " NWire6ProjectedPositionsInUSProbeFrameMatrix4xN = \n" << NWire6ProjectedPositionsInUSProbeFrameMatrix4xN << "\n";
			SystemLogFile << " -----------------------------------------------------------------------------------\n";
			SystemLogFile << " mNWire1LREOrigInUSProbeFrameMatrix4xN = \n" << mNWire1LREOrigInUSProbeFrameMatrix4xN << "\n";
			SystemLogFile << " mNWire3LREOrigInUSProbeFrameMatrix4xN = \n" << mNWire3LREOrigInUSProbeFrameMatrix4xN << "\n";
			SystemLogFile << " mNWire4LREOrigInUSProbeFrameMatrix4xN = \n" << mNWire4LREOrigInUSProbeFrameMatrix4xN << "\n";
			SystemLogFile << " mNWire6LREOrigInUSProbeFrameMatrix4xN = \n" << mNWire6LREOrigInUSProbeFrameMatrix4xN << "\n";
			SystemLogFile << " -----------------------------------------------------------------------------------\n";
			SystemLogFile << " mNWire1LRESortedAscendingInUSProbeFrameMatrix4xN = \n" << mNWire1LRESortedAscendingInUSProbeFrameMatrix4xN << "\n";
			SystemLogFile << " mNWire3LRESortedAscendingInUSProbeFrameMatrix4xN = \n" << mNWire3LRESortedAscendingInUSProbeFrameMatrix4xN << "\n";
			SystemLogFile << " mNWire4LRESortedAscendingInUSProbeFrameMatrix4xN = \n" << mNWire4LRESortedAscendingInUSProbeFrameMatrix4xN << "\n";
			SystemLogFile << " mNWire6LRESortedAscendingInUSProbeFrameMatrix4xN = \n" << mNWire6LRESortedAscendingInUSProbeFrameMatrix4xN << "\n";
			SystemLogFile << " -----------------------------------------------------------------------------------\n";
			SystemLogFile << " NWire1AbsLRETopRankedInUSProbeFrameMatrix4xN = \n" << NWire1AbsLRETopRankedInUSProbeFrameMatrix4xN << "\n";
			SystemLogFile << " NWire3AbsLRETopRankedInUSProbeFrameMatrix4xN = \n" << NWire3AbsLRETopRankedInUSProbeFrameMatrix4xN << "\n";
			SystemLogFile << " NWire4AbsLRETopRankedInUSProbeFrameMatrix4xN = \n" << NWire4AbsLRETopRankedInUSProbeFrameMatrix4xN << "\n";
			SystemLogFile << " NWire6AbsLRETopRankedInUSProbeFrameMatrix4xN = \n" << NWire6AbsLRETopRankedInUSProbeFrameMatrix4xN << "\n";
			SystemLogFile << " -----------------------------------------------------------------------------------\n";
			SystemLogFile << "[ vector 0 - 1:  LRE_X_mean,   LRE_X_std   ]\n";
			SystemLogFile << "[ vector 2 - 3:  LRE_Y_mean,   LRE_Y_std   ]\n";
			SystemLogFile << "[ vector 4 - 5:  LRE_EUC_mean, LRE_EUC_std ]\n";
			SystemLogFile << "[ vector 6    :  Validation data confidence level ]\n";			
			SystemLogFile << "mNWire1AbsLREAnalysisInUSProbeFrame = \n";
			SystemLogFile << mNWire1AbsLREAnalysisInUSProbeFrame[0] << ", " 
				<< mNWire1AbsLREAnalysisInUSProbeFrame[1] << "\n"
				<< mNWire1AbsLREAnalysisInUSProbeFrame[2] << ", " 
				<< mNWire1AbsLREAnalysisInUSProbeFrame[3] << "\n"
				<< mNWire1AbsLREAnalysisInUSProbeFrame[4] << ", "
				<< mNWire1AbsLREAnalysisInUSProbeFrame[5] << "\n" 
				<< mNWire1AbsLREAnalysisInUSProbeFrame[6] << "\n\n";
			SystemLogFile << "mNWire3AbsLREAnalysisInUSProbeFrame = \n";
			SystemLogFile << mNWire3AbsLREAnalysisInUSProbeFrame[0] << ", " 
				<< mNWire3AbsLREAnalysisInUSProbeFrame[1] << "\n"
				<< mNWire3AbsLREAnalysisInUSProbeFrame[2] << ", " 
				<< mNWire3AbsLREAnalysisInUSProbeFrame[3] << "\n"
				<< mNWire3AbsLREAnalysisInUSProbeFrame[4] << ", "
				<< mNWire3AbsLREAnalysisInUSProbeFrame[5] << "\n" 
				<< mNWire3AbsLREAnalysisInUSProbeFrame[6] << "\n\n";
			SystemLogFile << "mNWire4AbsLREAnalysisInUSProbeFrame = \n";
			SystemLogFile << mNWire4AbsLREAnalysisInUSProbeFrame[0] << ", " 
				<< mNWire4AbsLREAnalysisInUSProbeFrame[1] << "\n"
				<< mNWire4AbsLREAnalysisInUSProbeFrame[2] << ", " 
				<< mNWire4AbsLREAnalysisInUSProbeFrame[3] << "\n"
				<< mNWire4AbsLREAnalysisInUSProbeFrame[4] << ", "
				<< mNWire4AbsLREAnalysisInUSProbeFrame[5] << "\n" 
				<< mNWire4AbsLREAnalysisInUSProbeFrame[6] << "\n\n";
			SystemLogFile << "mNWire6AbsLREAnalysisInUSProbeFrame = \n";
			SystemLogFile << mNWire6AbsLREAnalysisInUSProbeFrame[0] << ", " 
				<< mNWire6AbsLREAnalysisInUSProbeFrame[1] << "\n"
				<< mNWire6AbsLREAnalysisInUSProbeFrame[2] << ", " 
				<< mNWire6AbsLREAnalysisInUSProbeFrame[3] << "\n"
				<< mNWire6AbsLREAnalysisInUSProbeFrame[4] << ", "
				<< mNWire6AbsLREAnalysisInUSProbeFrame[5] << "\n" 
				<< mNWire6AbsLREAnalysisInUSProbeFrame[6] << "\n\n";		

			SystemLogFile << " ==========================================================================\n";
			SystemLogFile << " COMPUTE Point-Line Distance Error >>>>>>>>>>>>>>>>>>>>>\n\n";
			SystemLogFile << " mPLDEsforValidationPositionsInUSProbeFrame = \n" << mPLDEsforValidationPositionsInUSProbeFrame << "\n";
			SystemLogFile << " -----------------------------------------------------------------------------------\n";
			SystemLogFile << " mSortedPLDEsAscendingforValidationInUSProbeFrame = \n" << mSortedPLDEsAscendingforValidationInUSProbeFrame << "\n";
			SystemLogFile << " -----------------------------------------------------------------------------------\n";
			SystemLogFile << "mPLDEAnalysis4ValidationPositionsInUSProbeFrame = \n";
			SystemLogFile << "[ vector 0 - 2:  PLDE_mean, PLDE_rms, PLDE_std ]\n";
			SystemLogFile << "[ vector 3    :  Validation data confidence level ]\n";
			SystemLogFile << mPLDEAnalysis4ValidationPositionsInUSProbeFrame[0] << ", " 
				<< mPLDEAnalysis4ValidationPositionsInUSProbeFrame[1] << ", "
				<< mPLDEAnalysis4ValidationPositionsInUSProbeFrame[2] << "\n\n";
			SystemLogFile << "Validation Data Confidence Level = " << 
				mPLDEAnalysis4ValidationPositionsInUSProbeFrame[3] << ".\n\n";
			SystemLogFile.close();
		}
	}
	catch(...)
	{
		std::cerr << "\n\n" << __FILE__ << "," << __LINE__ << "\n"
			<< ">>>>>>>> In " << mstrScope << "::Failed to calculate PLDEs for the validation data set!!!  Throw up ...\n";

		throw;	
	}
}


void Phantom::constructValidationDataMatrices ()
{
	if ( mAreValidationPositionsReady != true )
	{
		std::cerr << "\n\n" << __FILE__ << "," << __LINE__ << "\n"
			<< ">>>>>>>> In " << mstrScope << "::This operation is not possible since the validation data set is not ready!!!  Throw up ...\n";

		throw;
	}

	try
	{
		// The total number of the validation data set.
		const int NumberOfValidationPositions( mValidationPositionsInUSImageFrame.size() );
		
		// Populate the validation positions into to matrices for processing
		mValidationPositionsInUSImageFrameMatrix4xN.set_size(4, NumberOfValidationPositions);
		mValidationPositionsInUSProbeFrameMatrix4xN.set_size(4, NumberOfValidationPositions);
		
		mValidationPositionsNWire1InUSImageFrame4xN.set_size(4, NumberOfValidationPositions);
		mValidationPositionsNWire3InUSImageFrame4xN.set_size(4, NumberOfValidationPositions);
		mValidationPositionsNWire4InUSImageFrame4xN.set_size(4, NumberOfValidationPositions);
		mValidationPositionsNWire6InUSImageFrame4xN.set_size(4, NumberOfValidationPositions);

		mValidationPositionsNWire1InUSProbeFrame4xN.set_size(4, NumberOfValidationPositions);
		mValidationPositionsNWire3InUSProbeFrame4xN.set_size(4, NumberOfValidationPositions);
		mValidationPositionsNWire4InUSProbeFrame4xN.set_size(4, NumberOfValidationPositions);
		mValidationPositionsNWire6InUSProbeFrame4xN.set_size(4, NumberOfValidationPositions);

		mValidationPositionsNWireStartInUSProbeFrame3xN.set_size(3, NumberOfValidationPositions);
		mValidationPositionsNWireEndInUSProbeFrame3xN.set_size(3, NumberOfValidationPositions);
		vnl_matrix<double> ValidationPositionsNWireStartInUSProbeFrame4xN(4, NumberOfValidationPositions);
		vnl_matrix<double> ValidationPositionsNWireEndInUSProbeFrame4xN(4, NumberOfValidationPositions);

		for( int i = 0; i < NumberOfValidationPositions; i++ )
		{
			// Validation positions in the US image frame
			mValidationPositionsInUSImageFrameMatrix4xN.set_column(
				i, mValidationPositionsInUSImageFrame.at(i) );

			// Validation positions in the US probe frame
			mValidationPositionsInUSProbeFrameMatrix4xN.set_column(
				i, mValidationPositionsInUSProbeFrame.at(i) );

			// Validation points' NWire Start/End positions in US probe frame
			ValidationPositionsNWireStartInUSProbeFrame4xN.set_column(
				i, mValidationPositionsNWireStartInUSProbeFrame.at(i) );
			ValidationPositionsNWireEndInUSProbeFrame4xN.set_column(
				i, mValidationPositionsNWireEndInUSProbeFrame.at(i) );

			// Validation positions of the parallel wires in the US image frame
			// Wire N1
			mValidationPositionsNWire1InUSImageFrame4xN.set_column(
				i, mValidationPositionsNWire1InUSImageFrame.at(i) );
			// Wire N3
			mValidationPositionsNWire3InUSImageFrame4xN.set_column(
				i, mValidationPositionsNWire3InUSImageFrame.at(i) );
			// Wire N4
			mValidationPositionsNWire4InUSImageFrame4xN.set_column(
				i, mValidationPositionsNWire4InUSImageFrame.at(i) );
			// Wire N6
			mValidationPositionsNWire6InUSImageFrame4xN.set_column(
				i, mValidationPositionsNWire6InUSImageFrame.at(i) );

			// Validation positions of the parallel wires in the US probe frame
			// Wire N1
			mValidationPositionsNWire1InUSProbeFrame4xN.set_column(
				i, mValidationPositionsNWire1InUSProbeFrame.at(i) );
			// Wire N3
			mValidationPositionsNWire3InUSProbeFrame4xN.set_column(
				i, mValidationPositionsNWire3InUSProbeFrame.at(i) );
			// Wire N4
			mValidationPositionsNWire4InUSProbeFrame4xN.set_column(
				i, mValidationPositionsNWire4InUSProbeFrame.at(i) );
			// Wire N6
			mValidationPositionsNWire6InUSProbeFrame4xN.set_column(
				i, mValidationPositionsNWire6InUSProbeFrame.at(i) );
		}
		
		// For PLDE calcuation, we need the first 3 elements for cross products.
		mValidationPositionsNWireStartInUSProbeFrame3xN =
			ValidationPositionsNWireStartInUSProbeFrame4xN.get_n_rows(0, 3);
		mValidationPositionsNWireEndInUSProbeFrame3xN =
			ValidationPositionsNWireEndInUSProbeFrame4xN.get_n_rows(0, 3);

		// OPTION: populate the beamwidth data if selected (Option-1,2 or 3)
		// ==================================================================

		vnl_vector<double> USBeamWidthEuclideanMagAtValidationPositionsInVNLvectors(0);
		if( true == mIsUSBeamwidthAndWeightFactorsTableReady )
		{
			if( mUSBeamWidthEuclideanMagAtValidationPositions.size() != NumberOfValidationPositions )
			{
				std::cerr << "\n\n" << __FILE__ << "," << __LINE__ << "\n"
					<< ">>>>>>>> In " << mstrScope 
					<< "::The number of beamwidth data does NOT match the number of data for validation!!!  Throw up ...\n";

				throw;
			}		
			
			// Populate the data to vnl_vector format
			USBeamWidthEuclideanMagAtValidationPositionsInVNLvectors.set_size( NumberOfValidationPositions );
			for( int i = 0; i < NumberOfValidationPositions; i++ )
			{
				USBeamWidthEuclideanMagAtValidationPositionsInVNLvectors.put(i, 
					mUSBeamWidthEuclideanMagAtValidationPositions.at(i));
			}
		}

		mAreValidationDataMatricesConstructed = true;

		// Log the data pipeline if requested.
		if( true == mIsSystemLogOn )
		{
			std::ofstream SystemLogFile(
				mSystemLogFileNameWithTimeStamp.c_str(), std::ios::app);
			SystemLogFile << " ==========================================================================\n";
			SystemLogFile << " CONSTRUCT THE VALIDATION DATASET >>>>>>>>>>>>>>>>>>>>>\n\n";
			SystemLogFile << " mValidationPositionsInUSImageFrameMatrix4xN = \n" << mValidationPositionsInUSImageFrameMatrix4xN << "\n";
			SystemLogFile << " mValidationPositionsInUSProbeFrameMatrix4xN = \n" << mValidationPositionsInUSProbeFrameMatrix4xN << "\n";
			SystemLogFile << " -----------------------------------------------------------------------------------\n";
			SystemLogFile << " mValidationPositionsNWireStartInUSProbeFrame3xN = \n" << mValidationPositionsNWireStartInUSProbeFrame3xN << "\n";
			SystemLogFile << " mValidationPositionsNWireEndInUSProbeFrame3xN = \n" << mValidationPositionsNWireEndInUSProbeFrame3xN << "\n";
			SystemLogFile << " -----------------------------------------------------------------------------------\n";
			SystemLogFile << " mValidationPositionsNWire1InUSImageFrame4xN = \n" << mValidationPositionsNWire1InUSImageFrame4xN << "\n";
			SystemLogFile << " mValidationPositionsNWire3InUSImageFrame4xN = \n" << mValidationPositionsNWire3InUSImageFrame4xN << "\n";
			SystemLogFile << " mValidationPositionsNWire4InUSImageFrame4xN = \n" << mValidationPositionsNWire4InUSImageFrame4xN << "\n";
			SystemLogFile << " mValidationPositionsNWire6InUSImageFrame4xN = \n" << mValidationPositionsNWire6InUSImageFrame4xN << "\n";		
			SystemLogFile << " -----------------------------------------------------------------------------------\n";
			SystemLogFile << " mValidationPositionsNWire1InUSProbeFrame4xN = \n" << mValidationPositionsNWire1InUSProbeFrame4xN << "\n";
			SystemLogFile << " mValidationPositionsNWire3InUSProbeFrame4xN = \n" << mValidationPositionsNWire3InUSProbeFrame4xN << "\n";
			SystemLogFile << " mValidationPositionsNWire4InUSProbeFrame4xN = \n" << mValidationPositionsNWire4InUSProbeFrame4xN << "\n";
			SystemLogFile << " mValidationPositionsNWire6InUSProbeFrame4xN = \n" << mValidationPositionsNWire6InUSProbeFrame4xN << "\n";			
		
			if( true == mIsUSBeamwidthAndWeightFactorsTableReady )
			{
				SystemLogFile << " -----------------------------------------------------------------------------------\n";
				SystemLogFile << " USBeamWidthEuclideanMagAtValidationPositionsInVNLvectors = \n" 
					<< USBeamWidthEuclideanMagAtValidationPositionsInVNLvectors << "\n\n";
			}
			SystemLogFile.close();
		}
	}
	catch(...)
	{
		std::cerr << "\n\n" << __FILE__ << "," << __LINE__ << "\n"
			<< ">>>>>>>> In " << mstrScope << "::Failed to construct the validation data matrices!!!  Throw up ...\n";

		throw;	
	}
}

vnl_matrix<double> Phantom::convertTransform2HomoMatrix(
	const std::vector<double> &TransformInVectorArray)
{
	try
	{
		// IMPORTANT: Please follow the interface closely.
		// 1. The input transform is in a vector array format:
		//		- [0]     = Q0			(in degrees - the rotation angle)
		//		- [1 - 3] = Qx, Qy, Qz	(the unit rotation axis)
		//		- [4 - 6] = Tx, Ty, Tz	(translation); 
		// 2. The output is a 4x4 homogeneous transform matrix.

		// Extract elements from the Transform vector array

		// Angle of rotation in degrees (Q0)
		double RotationAngleInDegrees = TransformInVectorArray.at(0);	
		double RotationAngleInRadians = RotationAngleInDegrees * ( mPI/180 );
		// Unit rotation axis (Qx, Qy, Qz)
		double Qx = TransformInVectorArray.at(1);			
		double Qy = TransformInVectorArray.at(2);			
        double Qz = TransformInVectorArray.at(3);			

		// Translations (Tx, Ty, Tz)
		double Tx = TransformInVectorArray.at(4);			
		double Ty = TransformInVectorArray.at(5);			
		double Tz = TransformInVectorArray.at(6);			

		// Construct the Unit Quaterions [Qa Qb Qc Qd]'
        // NOTE: Qa*Qa + Qb*Qb + Qc*Qc + Qd*Qd = 1, or Unit Quaterion
		double Qa = cos( RotationAngleInRadians * 0.5 );
		double Qb = Qx * sin( RotationAngleInRadians * 0.5 );
		double Qc = Qy * sin( RotationAngleInRadians * 0.5 );
		double Qd = Qz * sin( RotationAngleInRadians * 0.5 );
		
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
		std::cerr << "\n\n" << __FILE__ << "," << __LINE__ << "\n"
			<< ">>>>>>>> In " << mstrScope 
			<< "::Failed to convert the transform to 4x4 homogeneous matrix!!!  Throw up ...\n";

		throw;	
	}
}



// END OF FILE