// 1. This the parent class of all current or future calibration phantoms.
// 2. This class defines some generic features and behaviors that one would
//    typically expect from an ultrasound probe calibration phantom.  By 
//    inheriting (or extending) this class, the child class would only need
//    to define some specific features of its own (e.g., the unique phantom 
//    geometries that differ from phantom to phantom).
// 3. Uses the open source VXL-VNL (Numerical and Algebra C++ Library)
// 4. Uses C++ Standard Library and Standard Template Library

#include "PlusConfigure.h"

#include "Phantom.h"

// C++ Standard Includes
#include <iostream>
#include <fstream>	// for file I/O process

#include "LinearLeastSquares.h"

double Phantom::mOutlierDetectionThreshold = 3.0; 

//-----------------------------------------------------------------------------

Phantom::Phantom()
{
	// Initialize flags
	mHasPhantomBeenRegistered = false;
	mIsPhantomGeometryLoaded = false;
	mIsUSBeamwidthAndWeightFactorsTableReady = false;
	        	
  this->resetDataContainers(); 

  mTransformOrigImageFrame2TRUSImageFrameMatrix4x4 = NULL;
  mMethodToIncorporateBeamWidth = 0;
  mUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xM.set_size(0,0);
	mOrigUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xN.set_size(0,0);
	mMinimumUSElevationBeamwidthAndFocalZoneInUSImageFrame.set_size(2);
	mUS3DBeamwidthAtNearestAxialDepth.set_size(3);
	mUS3DBeamwidthAtFarestAxialDepth.set_size(3);
	mNumOfTotalBeamWidthData = 0;
  mValidationDataConfidenceLevel = 0.95;
}

//-----------------------------------------------------------------------------

void Phantom::resetDataContainers()
{
  // Initialize flags
  mAreDataPositionsReady = false;
	mAreValidationPositionsReady = false;
	mArePRE3DsForValidationPositionsReady = false;
	mAreIndependentPointLineReconErrorsReady = false;
  mHasBeenCalibrated = false;
  mAreValidationDataMatricesConstructed = false;
	mAreOutliersRemoved = false; 

	// Initialize data containers
	mTransformOrigImageFrame2TRUSImageFrameMatrix4x4.set_size(4,4);
	mTransformMatrixPhantom2DRB4x4.set_size(4,4);

  mDataPositionsInPhantomFrame.resize(0);
	mDataPositionsInUSProbeFrame.resize(0);
	mDataPositionsInUSImageFrame.resize(0);
	mOutlierDataPositions.resize(0); 
	
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

	mAbsPRE3DAnalysis4ValidationPositionsInUSProbeFrame.resize(0);
	mPLDEAnalysis4ValidationPositionsInUSProbeFrame.resize(0);
	mRawPRE3DsforValidationPositionsInUSProbeFrameMatrix4xN.set_size(0,0);
	mSortedRawPRE3DsInAscendingOrderInUSProbeFrameMatrix4xN.set_size(0,0);
	mPLDEsforValidationPositionsInUSProbeFrame.set_size(0);
	mSortedPLDEsAscendingforValidationInUSProbeFrame.set_size(0);
}

//-----------------------------------------------------------------------------

Phantom::~Phantom()
{
}

//-----------------------------------------------------------------------------

void Phantom::setTransformOrigImageToTRUSImageFrame4x4(
	const vnl_matrix<double> TransformOrigImageFrame2TRUSImageFrameMatrix4x4)
{
	mTransformOrigImageFrame2TRUSImageFrameMatrix4x4 = 
		TransformOrigImageFrame2TRUSImageFrameMatrix4x4;
}

//-----------------------------------------------------------------------------

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
	LOG_DEBUG(" mMethodToIncorporateBeamWidth = " << mMethodToIncorporateBeamWidth 
		<< " (1 - BeamwidthVariance | 2 - BeamwidthRatio | 3 - BeamwidthVarianceAndThresholding)");
	LOG_DEBUG(" mNumOfTotalBeamWidthData = " 
		<< mNumOfTotalBeamWidthData);
	LOG_DEBUG(" mMinimumUSElevationBeamwidthAndFocalZoneInUSImageFrame [Focal Zone (US Image Frame), Elevation Beamwidth] = " 
		<< mMinimumUSElevationBeamwidthAndFocalZoneInUSImageFrame);
	LOG_DEBUG(" mOrigUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xN = " 
		<< mOrigUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xN);
	LOG_DEBUG(" mUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xM (interpolated) = " 
		<< mUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xM);
	LOG_DEBUG(" mTheNearestAxialDepthInUSBeamwidthAndWeightTable = " 
		<< mTheNearestAxialDepthInUSBeamwidthAndWeightTable);				
	LOG_DEBUG(" mTheFarestAxialDepthInUSBeamwidthAndWeightTable = " 
		<< mTheFarestAxialDepthInUSBeamwidthAndWeightTable);
}

//-----------------------------------------------------------------------------

void Phantom::setValidationDataConfidenceLevel( 
	const double ValidationDataConfidenceLevel )
{
	if( ValidationDataConfidenceLevel <= 1 && ValidationDataConfidenceLevel > 0 )
	{
		mValidationDataConfidenceLevel = ValidationDataConfidenceLevel;
	}
}

//-----------------------------------------------------------------------------

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

    // Set the calibration flag to true
	mHasPhantomBeenRegistered = true;

	LOG_DEBUG(" Setting Phantom to Reference matrix");
	LOG_DEBUG(" mTransformMatrixPhantom2DRB4x4 = " 
		<< mTransformMatrixPhantom2DRB4x4);				
}

//-----------------------------------------------------------------------------

void Phantom::setOutlierFlags()
{
	const double numOfElements = this->mDataPositionsInUSImageFrame.size(); 
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

//-----------------------------------------------------------------------------

void Phantom::resetOutlierFlags()
{
	this->mOutlierDataPositions.clear(); 

	this->mAreOutliersRemoved = false; 
}

//-----------------------------------------------------------------------------

PlusStatus Phantom::calibrate()
{
	if( mAreDataPositionsReady != true )
	{
		LOG_ERROR("The data positions are not ready yet for calibration!");
    return PLUS_FAIL;
	}

	if( true == mHasBeenCalibrated )
	{
		mHasBeenCalibrated = false;
	}

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
			LOG_ERROR("The number of weights and beamwidth data does NOT match the number of data for calibration!");
			return PLUS_FAIL;
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

  try {
	  LeastSquares.doOptimization();
  } catch (...) {
    LOG_ERROR("Optimiaztion failed!");
    return PLUS_FAIL;
  }

	// Output to the resulting matrix
	mTransformUSImageFrame2USProbeFrameMatrix4x4 = LeastSquares.getTransform();

	if ( mAreOutliersRemoved == true )
	{
    return PLUS_SUCCESS; 
	}

	// eliminate outliers from calibration 
	this->setOutlierFlags(); 

	// do a re-calibration without outliers
	this->calibrate(); 

	// reset the outlier flags 
	this->resetOutlierFlags(); 

	// Make sure the last row in homogeneous transform is [0 0 0 1]
	vnl_vector<double> lastRow(4,0);
	lastRow.put(3, 1);
	mTransformUSImageFrame2USProbeFrameMatrix4x4.set_row(3, lastRow);

	// Reset the calibration flag
	mHasBeenCalibrated = true;

	// Log the data pipeline if requested.
	LOG_DEBUG(" PERFORM CALIBRATION");
	LOG_DEBUG(" mTransformOrigImageFrame2TRUSImageFrameMatrix4x4 = \n" << mTransformOrigImageFrame2TRUSImageFrameMatrix4x4);
	LOG_DEBUG(" DataPositionsInUSImageFrameMatrix4xN = \n" << DataPositionsInUSImageFrameMatrix4xN);
	LOG_DEBUG(" DataPositionsInUSProbeFrameMatrix4xN = \n" << DataPositionsInUSProbeFrameMatrix4xN);
	if( true == mIsUSBeamwidthAndWeightFactorsTableReady )
	{
		LOG_DEBUG(" USBeamWidthEuclideanMagAtDataPositionsInVNLvectors = \n" << USBeamWidthEuclideanMagAtDataPositionsInVNLvectors);
		LOG_DEBUG(" WeightsForDataPositionsInVNLvectors = \n" << WeightsForDataPositionsInVNLvectors);
	}
	LOG_DEBUG(" mTransformUSImageFrame2USProbeFrameMatrix4x4 = \n" << mTransformUSImageFrame2USProbeFrameMatrix4x4);

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

vnl_matrix<double> Phantom::getCalibrationResultsInMatrix() const
{
	if( mHasBeenCalibrated != true )
	{
		LOG_ERROR("This operation is not possible since the calibration is not yet finished!");
	}

	return mTransformUSImageFrame2USProbeFrameMatrix4x4;
}

//-----------------------------------------------------------------------------

std::vector<double> Phantom::getPRE3DAnalysis4ValidationDataSet() const
{
	if( 0 == mAbsPRE3DAnalysis4ValidationPositionsInUSProbeFrame.size() )
	{
		LOG_ERROR("This operation is not possible since the PRE3D is not ready!");
	}

	return mAbsPRE3DAnalysis4ValidationPositionsInUSProbeFrame;
}

//-----------------------------------------------------------------------------

std::vector<double> Phantom::getPLDEAnalysis4ValidationDataSet() const
{
	if( 0 == mPLDEAnalysis4ValidationPositionsInUSProbeFrame.size() )
	{
		LOG_ERROR("This operation is not possible since the PLDE is not ready!");
	}

	return mPLDEAnalysis4ValidationPositionsInUSProbeFrame;
}

//-----------------------------------------------------------------------------

vnl_matrix <double> Phantom::getPRE3DsRAWMatrixforValidationDataSet4xN() const
{
	if( mArePRE3DsForValidationPositionsReady != true )
	{
		LOG_ERROR("This operation is not possible since the PRE3Ds' matrix is not ready!");
	}

	return mRawPRE3DsforValidationPositionsInUSProbeFrameMatrix4xN;
}

//-----------------------------------------------------------------------------

vnl_vector<double> Phantom::getOrigPLDEsforValidationDataSet() const
{
	if( mAreIndependentPointLineReconErrorsReady != true )
	{
		LOG_ERROR("This operation is not possible since the PLDEs' vector is not ready");
	}

	return mPLDEsforValidationPositionsInUSProbeFrame;
}

//-----------------------------------------------------------------------------

vnl_vector<double> Phantom::getSortedPLDEsforValidationDataSet() const
{
	if( mAreIndependentPointLineReconErrorsReady != true )
	{
		LOG_ERROR("This operation is not possible since the PLDEs' vector is not ready");
	}

	return mSortedPLDEsAscendingforValidationInUSProbeFrame;
}

//-----------------------------------------------------------------------------

vnl_matrix <double> Phantom::getLREOrigMatrix4xNForNWire1() const
{
	if( mAreIndependentPointLineReconErrorsReady != true )
	{
		LOG_ERROR("This operation is not possible since the data matrix is not ready");
	}

	return mNWire1LREOrigInUSProbeFrameMatrix4xN;
}

//-----------------------------------------------------------------------------

vnl_matrix <double> Phantom::getLREOrigMatrix4xNForNWire3() const
{
	if( mAreIndependentPointLineReconErrorsReady != true )
	{
		LOG_ERROR("This operation is not possible since the data matrix is not ready");
	}

	return mNWire3LREOrigInUSProbeFrameMatrix4xN;
}

//-----------------------------------------------------------------------------

vnl_matrix <double> Phantom::getLREOrigMatrix4xNForNWire4() const
{
	if( mAreIndependentPointLineReconErrorsReady != true )
	{
		LOG_ERROR("This operation is not possible since the data matrix is not ready!");
	}

	return mNWire4LREOrigInUSProbeFrameMatrix4xN;
}

//-----------------------------------------------------------------------------

vnl_matrix <double> Phantom::getLREOrigMatrix4xNForNWire6() const
{
	if( mAreIndependentPointLineReconErrorsReady != true )
	{
		LOG_ERROR("This operation is not possible since the data matrix is not ready!");
	}

	return mNWire6LREOrigInUSProbeFrameMatrix4xN;
}

//-----------------------------------------------------------------------------

std::vector<double> Phantom::getLREAbsAnalysisForNWire1() const
{
	if( mAreIndependentPointLineReconErrorsReady != true )
	{
		LOG_ERROR("This operation is not possible since the data matrix is not ready!");
	}

	return mNWire1AbsLREAnalysisInUSProbeFrame;
}

//-----------------------------------------------------------------------------

std::vector<double> Phantom::getLREAbsAnalysisForNWire3() const
{
	if( mAreIndependentPointLineReconErrorsReady != true )
	{
		LOG_ERROR("This operation is not possible since the data matrix is not ready!");
	}

	return mNWire3AbsLREAnalysisInUSProbeFrame;
}

//-----------------------------------------------------------------------------

std::vector<double> Phantom::getLREAbsAnalysisForNWire4() const
{
	if( mAreIndependentPointLineReconErrorsReady != true )
	{
		LOG_ERROR("This operation is not possible since the data matrix is not ready!");
	}

	return mNWire4AbsLREAnalysisInUSProbeFrame;
}

//-----------------------------------------------------------------------------

std::vector<double> Phantom::getLREAbsAnalysisForNWire6() const
{
	if( mAreIndependentPointLineReconErrorsReady != true )
	{
		LOG_ERROR("This operation is not possible since the data matrix is not ready!");
	}

	return mNWire6AbsLREAnalysisInUSProbeFrame;
}

//-----------------------------------------------------------------------------

PlusStatus Phantom::compute3DPointReconstructionError()
{
	if ( mAreValidationPositionsReady != true )
	{
		LOG_ERROR("This operation is not possible since the validation data set is not ready!");
    return PLUS_FAIL;
	}

	// Reset the flag
	if( true == mArePRE3DsForValidationPositionsReady )
	{
		mArePRE3DsForValidationPositionsReady = false;
	}

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
    if (constructValidationDataMatrices() != PLUS_SUCCESS) {
      LOG_ERROR("Failed to construct validation martices!");
      return PLUS_FAIL;
    }
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
	LOG_DEBUG(" COMPUTE 3D Point Reconstruction Error (PRE3D)");
	LOG_DEBUG("mAbsPRE3DAnalysis4ValidationPositionsInUSProbeFrame = ");
	LOG_DEBUG("[ vector 0 - 2:  PRE3D_X_mean, PRE3D_X_rms, PRE3D_X_std ]");
	LOG_DEBUG("[ vector 3 - 5:  PRE3D_Y_mean, PRE3D_Y_rms, PRE3D_Y_std ]");
	LOG_DEBUG("[ vector 6 - 8:  PRE3D_Z_mean, PRE3D_Z_rms, PRE3D_Z_std ]");
	LOG_DEBUG("[ vector 9    :  Validation data confidence level ]");
	LOG_DEBUG(mAbsPRE3DAnalysis4ValidationPositionsInUSProbeFrame[0] << ", " 
		<< mAbsPRE3DAnalysis4ValidationPositionsInUSProbeFrame[1] << ", "
		<< mAbsPRE3DAnalysis4ValidationPositionsInUSProbeFrame[2] << "\n"
		<< mAbsPRE3DAnalysis4ValidationPositionsInUSProbeFrame[3] << ", " 
		<< mAbsPRE3DAnalysis4ValidationPositionsInUSProbeFrame[4] << ", "
		<< mAbsPRE3DAnalysis4ValidationPositionsInUSProbeFrame[5] << "\n"
		<< mAbsPRE3DAnalysis4ValidationPositionsInUSProbeFrame[6] << ", " 
		<< mAbsPRE3DAnalysis4ValidationPositionsInUSProbeFrame[7] << ", "
		<< mAbsPRE3DAnalysis4ValidationPositionsInUSProbeFrame[8]);
	LOG_DEBUG("Validation Data Confidence Level = " << 
		mAbsPRE3DAnalysis4ValidationPositionsInUSProbeFrame[9] << ", or in other words");
	LOG_DEBUG(NumberOfTopRankedCalibrationData << " top-ranked validation data were used out of the total" 
		<< NumberOfValidationPositions << " validation data set for the above statistical analysis.");

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------

PlusStatus Phantom::computeIndependentPointLineReconstructionError()
{
	if ( mAreValidationPositionsReady != true )
	{
		LOG_ERROR("This operation is not possible since the validation data set is not ready!");
    return PLUS_FAIL;
	}

	// Reset the flag
	if( true == mAreIndependentPointLineReconErrorsReady )
	{
		mAreIndependentPointLineReconErrorsReady = false;
	}

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
    if (constructValidationDataMatrices() != PLUS_SUCCESS) {
      LOG_ERROR("Failed to construct validation matrices!");
      return PLUS_FAIL;
    }
	}

	// The total number of the validation data set.
	const int NumberOfValidationPositions( mValidationPositionsInUSImageFrame.size() );

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
		mNWire1LREOrigInUSProbeFrameMatrix4xN.put(
			3, i, 
			mNWire1LREOrigInUSProbeFrameMatrix4xN.get_column(i).magnitude() );

		mNWire3LREOrigInUSProbeFrameMatrix4xN.put(
			3, i, 
			mNWire3LREOrigInUSProbeFrameMatrix4xN.get_column(i).magnitude() );

		mNWire4LREOrigInUSProbeFrameMatrix4xN.put(
			3, i, 
			mNWire4LREOrigInUSProbeFrameMatrix4xN.get_column(i).magnitude() );

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


	// STEP-2. Calculate Point-Line Distance Error (PLDE) in the US Probe Frame

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
	const double PLDE_mean = AscendingTopRankingPLDEsinUSProbeFrame.mean();
	const double PLDE_rms = AscendingTopRankingPLDEsinUSProbeFrame.rms();
	const double PLDE_std = sqrt( 
		1/(double)( NumberOfTopRankedCalibrationData - 1 ) *	
		( AscendingTopRankingPLDEsinUSProbeFrame.squared_magnitude() - 
		NumberOfTopRankedCalibrationData*pow(PLDE_mean,2) ) );

	// Consolidate the returning data container

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
	LOG_DEBUG(" COMPUTE FINAL 3D Point Reconstruction Error (PRE3D)");
	LOG_DEBUG(" mRawPRE3DsforValidationPositionsInUSProbeFrameMatrix4xN = \n" << mRawPRE3DsforValidationPositionsInUSProbeFrameMatrix4xN);
	LOG_DEBUG(" mSortedRawPRE3DsInAscendingOrderInUSProbeFrameMatrix4xN = \n" << mSortedRawPRE3DsInAscendingOrderInUSProbeFrameMatrix4xN);
	LOG_DEBUG("mAbsPRE3DAnalysis4ValidationPositionsInUSProbeFrame = ");
	LOG_DEBUG("[ vector 0 - 2:  PRE3D_X_mean, PRE3D_X_rms, PRE3D_X_std ]");
	LOG_DEBUG("[ vector 3 - 5:  PRE3D_Y_mean, PRE3D_Y_rms, PRE3D_Y_std ]");
	LOG_DEBUG("[ vector 6 - 8:  PRE3D_Z_mean, PRE3D_Z_rms, PRE3D_Z_std ]");
	LOG_DEBUG("[ vector 9    :  Validation data confidence level ]");
	LOG_DEBUG(mAbsPRE3DAnalysis4ValidationPositionsInUSProbeFrame[0] << ", " 
		<< mAbsPRE3DAnalysis4ValidationPositionsInUSProbeFrame[1] << ", "
		<< mAbsPRE3DAnalysis4ValidationPositionsInUSProbeFrame[2] << "\n"
		<< mAbsPRE3DAnalysis4ValidationPositionsInUSProbeFrame[3] << ", " 
		<< mAbsPRE3DAnalysis4ValidationPositionsInUSProbeFrame[4] << ", "
		<< mAbsPRE3DAnalysis4ValidationPositionsInUSProbeFrame[5] << "\n"
		<< mAbsPRE3DAnalysis4ValidationPositionsInUSProbeFrame[6] << ", " 
		<< mAbsPRE3DAnalysis4ValidationPositionsInUSProbeFrame[7] << ", "
		<< mAbsPRE3DAnalysis4ValidationPositionsInUSProbeFrame[8]);
	LOG_DEBUG("Validation Data Confidence Level = " << 
		mAbsPRE3DAnalysis4ValidationPositionsInUSProbeFrame[9] << ".");

	LOG_DEBUG(" COMPUTE Independent NWire Line Reconstruction Error (LRE)");
	LOG_DEBUG(" NWire1ProjectedPositionsInUSProbeFrameMatrix4xN = \n" << NWire1ProjectedPositionsInUSProbeFrameMatrix4xN);
	LOG_DEBUG(" NWire3ProjectedPositionsInUSProbeFrameMatrix4xN = \n" << NWire3ProjectedPositionsInUSProbeFrameMatrix4xN);
	LOG_DEBUG(" NWire4ProjectedPositionsInUSProbeFrameMatrix4xN = \n" << NWire4ProjectedPositionsInUSProbeFrameMatrix4xN);
	LOG_DEBUG(" NWire6ProjectedPositionsInUSProbeFrameMatrix4xN = \n" << NWire6ProjectedPositionsInUSProbeFrameMatrix4xN);
	LOG_DEBUG(" mNWire1LREOrigInUSProbeFrameMatrix4xN = \n" << mNWire1LREOrigInUSProbeFrameMatrix4xN);
	LOG_DEBUG(" mNWire3LREOrigInUSProbeFrameMatrix4xN = \n" << mNWire3LREOrigInUSProbeFrameMatrix4xN);
	LOG_DEBUG(" mNWire4LREOrigInUSProbeFrameMatrix4xN = \n" << mNWire4LREOrigInUSProbeFrameMatrix4xN);
	LOG_DEBUG(" mNWire6LREOrigInUSProbeFrameMatrix4xN = \n" << mNWire6LREOrigInUSProbeFrameMatrix4xN);
	LOG_DEBUG(" mNWire1LRESortedAscendingInUSProbeFrameMatrix4xN = \n" << mNWire1LRESortedAscendingInUSProbeFrameMatrix4xN);
	LOG_DEBUG(" mNWire3LRESortedAscendingInUSProbeFrameMatrix4xN = \n" << mNWire3LRESortedAscendingInUSProbeFrameMatrix4xN);
	LOG_DEBUG(" mNWire4LRESortedAscendingInUSProbeFrameMatrix4xN = \n" << mNWire4LRESortedAscendingInUSProbeFrameMatrix4xN);
	LOG_DEBUG(" mNWire6LRESortedAscendingInUSProbeFrameMatrix4xN = \n" << mNWire6LRESortedAscendingInUSProbeFrameMatrix4xN);
	LOG_DEBUG(" NWire1AbsLRETopRankedInUSProbeFrameMatrix4xN = \n" << NWire1AbsLRETopRankedInUSProbeFrameMatrix4xN);
	LOG_DEBUG(" NWire3AbsLRETopRankedInUSProbeFrameMatrix4xN = \n" << NWire3AbsLRETopRankedInUSProbeFrameMatrix4xN);
	LOG_DEBUG(" NWire4AbsLRETopRankedInUSProbeFrameMatrix4xN = \n" << NWire4AbsLRETopRankedInUSProbeFrameMatrix4xN);
	LOG_DEBUG(" NWire6AbsLRETopRankedInUSProbeFrameMatrix4xN = \n" << NWire6AbsLRETopRankedInUSProbeFrameMatrix4xN);
	LOG_DEBUG("[ vector 0 - 1:  LRE_X_mean,   LRE_X_std   ]");
	LOG_DEBUG("[ vector 2 - 3:  LRE_Y_mean,   LRE_Y_std   ]");
	LOG_DEBUG("[ vector 4 - 5:  LRE_EUC_mean, LRE_EUC_std ]");
	LOG_DEBUG("[ vector 6    :  Validation data confidence level ]");
	LOG_DEBUG("mNWire1AbsLREAnalysisInUSProbeFrame = ");
	LOG_DEBUG(mNWire1AbsLREAnalysisInUSProbeFrame[0] << ", " 
		<< mNWire1AbsLREAnalysisInUSProbeFrame[1] << "\n"
		<< mNWire1AbsLREAnalysisInUSProbeFrame[2] << ", " 
		<< mNWire1AbsLREAnalysisInUSProbeFrame[3] << "\n"
		<< mNWire1AbsLREAnalysisInUSProbeFrame[4] << ", "
		<< mNWire1AbsLREAnalysisInUSProbeFrame[5] << "\n" 
		<< mNWire1AbsLREAnalysisInUSProbeFrame[6]);
	LOG_DEBUG("mNWire3AbsLREAnalysisInUSProbeFrame = ");
	LOG_DEBUG(mNWire3AbsLREAnalysisInUSProbeFrame[0] << ", " 
		<< mNWire3AbsLREAnalysisInUSProbeFrame[1] << "\n"
		<< mNWire3AbsLREAnalysisInUSProbeFrame[2] << ", " 
		<< mNWire3AbsLREAnalysisInUSProbeFrame[3] << "\n"
		<< mNWire3AbsLREAnalysisInUSProbeFrame[4] << ", "
		<< mNWire3AbsLREAnalysisInUSProbeFrame[5] << "\n" 
		<< mNWire3AbsLREAnalysisInUSProbeFrame[6]);
	LOG_DEBUG("mNWire4AbsLREAnalysisInUSProbeFrame = ");
	LOG_DEBUG(mNWire4AbsLREAnalysisInUSProbeFrame[0] << ", " 
		<< mNWire4AbsLREAnalysisInUSProbeFrame[1] << "\n"
		<< mNWire4AbsLREAnalysisInUSProbeFrame[2] << ", " 
		<< mNWire4AbsLREAnalysisInUSProbeFrame[3] << "\n"
		<< mNWire4AbsLREAnalysisInUSProbeFrame[4] << ", "
		<< mNWire4AbsLREAnalysisInUSProbeFrame[5] << "\n" 
		<< mNWire4AbsLREAnalysisInUSProbeFrame[6]);
	LOG_DEBUG("mNWire6AbsLREAnalysisInUSProbeFrame = ");
	LOG_DEBUG(mNWire6AbsLREAnalysisInUSProbeFrame[0] << ", " 
		<< mNWire6AbsLREAnalysisInUSProbeFrame[1] << "\n"
		<< mNWire6AbsLREAnalysisInUSProbeFrame[2] << ", " 
		<< mNWire6AbsLREAnalysisInUSProbeFrame[3] << "\n"
		<< mNWire6AbsLREAnalysisInUSProbeFrame[4] << ", "
		<< mNWire6AbsLREAnalysisInUSProbeFrame[5] << "\n" 
		<< mNWire6AbsLREAnalysisInUSProbeFrame[6]);		

	LOG_DEBUG(" COMPUTE Point-Line Distance Error");
	LOG_DEBUG(" mPLDEsforValidationPositionsInUSProbeFrame = \n" << mPLDEsforValidationPositionsInUSProbeFrame);
	LOG_DEBUG(" mSortedPLDEsAscendingforValidationInUSProbeFrame = \n" << mSortedPLDEsAscendingforValidationInUSProbeFrame);
	LOG_DEBUG("mPLDEAnalysis4ValidationPositionsInUSProbeFrame = ");
	LOG_DEBUG("[ vector 0 - 2:  PLDE_mean, PLDE_rms, PLDE_std ]");
	LOG_DEBUG("[ vector 3    :  Validation data confidence level ]");
	LOG_DEBUG(mPLDEAnalysis4ValidationPositionsInUSProbeFrame[0] << ", " 
		<< mPLDEAnalysis4ValidationPositionsInUSProbeFrame[1] << ", "
		<< mPLDEAnalysis4ValidationPositionsInUSProbeFrame[2]);
	LOG_DEBUG("Validation Data Confidence Level = " << 
		mPLDEAnalysis4ValidationPositionsInUSProbeFrame[3] << ".");

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus Phantom::constructValidationDataMatrices()
{
	if ( mAreValidationPositionsReady != true )
	{
		LOG_ERROR("This operation is not possible since the validation data set is not ready!");
    return PLUS_FAIL;
	}

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
	vnl_vector<double> USBeamWidthEuclideanMagAtValidationPositionsInVNLvectors(0);
	if( true == mIsUSBeamwidthAndWeightFactorsTableReady )
	{
		if( mUSBeamWidthEuclideanMagAtValidationPositions.size() != NumberOfValidationPositions )
		{
			LOG_ERROR("The number of beamwidth data does NOT match the number of data for validation!");
      return PLUS_FAIL;
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
	LOG_DEBUG(" CONSTRUCT THE VALIDATION DATASET");
	LOG_DEBUG(" mValidationPositionsInUSImageFrameMatrix4xN = \n" << mValidationPositionsInUSImageFrameMatrix4xN);
	LOG_DEBUG(" mValidationPositionsInUSProbeFrameMatrix4xN = \n" << mValidationPositionsInUSProbeFrameMatrix4xN);
	LOG_DEBUG(" mValidationPositionsNWireStartInUSProbeFrame3xN = \n" << mValidationPositionsNWireStartInUSProbeFrame3xN);
	LOG_DEBUG(" mValidationPositionsNWireEndInUSProbeFrame3xN = \n" << mValidationPositionsNWireEndInUSProbeFrame3xN);
	LOG_DEBUG(" mValidationPositionsNWire1InUSImageFrame4xN = \n" << mValidationPositionsNWire1InUSImageFrame4xN);
	LOG_DEBUG(" mValidationPositionsNWire3InUSImageFrame4xN = \n" << mValidationPositionsNWire3InUSImageFrame4xN);
	LOG_DEBUG(" mValidationPositionsNWire4InUSImageFrame4xN = \n" << mValidationPositionsNWire4InUSImageFrame4xN);
	LOG_DEBUG(" mValidationPositionsNWire6InUSImageFrame4xN = \n" << mValidationPositionsNWire6InUSImageFrame4xN);		
	LOG_DEBUG(" mValidationPositionsNWire1InUSProbeFrame4xN = \n" << mValidationPositionsNWire1InUSProbeFrame4xN);
	LOG_DEBUG(" mValidationPositionsNWire3InUSProbeFrame4xN = \n" << mValidationPositionsNWire3InUSProbeFrame4xN);
	LOG_DEBUG(" mValidationPositionsNWire4InUSProbeFrame4xN = \n" << mValidationPositionsNWire4InUSProbeFrame4xN);
	LOG_DEBUG(" mValidationPositionsNWire6InUSProbeFrame4xN = \n" << mValidationPositionsNWire6InUSProbeFrame4xN);			

	if( true == mIsUSBeamwidthAndWeightFactorsTableReady )
	{
		LOG_DEBUG(" USBeamWidthEuclideanMagAtValidationPositionsInVNLvectors = \n" 
			<< USBeamWidthEuclideanMagAtValidationPositionsInVNLvectors);
	}

  return PLUS_SUCCESS;
}
