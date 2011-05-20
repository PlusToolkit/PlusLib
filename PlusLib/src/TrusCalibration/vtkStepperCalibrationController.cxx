#include "PlusConfigure.h"
#include "vtkStepperCalibrationController.h"
#include "vtkObjectFactory.h"
#include "vtkTransform.h"
#include "vtkMath.h"
#include <numeric>
#include <time.h>
#include "vtksys/SystemTools.hxx"
#include "vtkGnuplotExecuter.h"
#include "vtkHTMLGenerator.h"
#include "vtkMeanShiftClustering.h"

#include "vnl/vnl_sparse_matrix.h"   
#include "vnl/vnl_sparse_matrix_linear_system.h"  
#include "vnl/algo/vnl_lsqr.h"  
#include "vnl/vnl_cross.h"  


//----------------------------------------------------------------------------
vtkCxxRevisionMacro(vtkStepperCalibrationController, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkStepperCalibrationController); 

//----------------------------------------------------------------------------
vtkStepperCalibrationController::vtkStepperCalibrationController()
{
	this->SetSpacing(0,0); 
	this->SetCenterOfRotationPx(0,0); 
	this->SetProbeTranslationAxisOrientation(0,0,1); 
	this->SetTemplateTranslationAxisOrientation(0,0,1);
	this->SetProbeRotationAxisOrientation(0,0,1); 
	this->SetProbeRotationEncoderOffset(0.0); 
	this->SetProbeRotationEncoderScale(1.0); 
	this->SetMinNumberOfRotationClusters(4); 
	this->SpacingCalculatedOff(); 
	this->ProbeRotationAxisCalibratedOff(); 
	this->ProbeTranslationAxisCalibratedOff(); 
	this->TemplateTranslationAxisCalibratedOff(); 
	this->SetOutlierDetectionThreshold(3.0); 
	this->MinNumOfFramesUsedForCenterOfRotCalc = 25; 

	this->CalibrationStartTime = NULL; 
	this->ProbeTranslationAxisCalibrationErrorReportFilePath = NULL; 
	this->TemplateTranslationAxisCalibrationErrorReportFilePath = NULL; 

	this->SaveCalibrationStartTime(); 
}

//----------------------------------------------------------------------------
vtkStepperCalibrationController::~vtkStepperCalibrationController()
{

}

//----------------------------------------------------------------------------
void vtkStepperCalibrationController::PrintSelf(std::ostream &os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os,indent);
}


//----------------------------------------------------------------------------
void vtkStepperCalibrationController::Initialize()
{
	LOG_TRACE("vtkStepperCalibrationController::Initialize"); 

	if ( this->GetSegParameters() == NULL )
	{
		LOG_DEBUG("SegParameters is NULL, create one..."); 
		this->SegParameters = new SegmentationParameters(); 
	}

	// Initialize the segmenation component
	// ====================================
	this->mptrAutomatedSegmentation = new KPhantomSeg( 
		this->GetImageWidthInPixels(), this->GetImageHeightInPixels(), 
		this->GetSearchStartAtX(), this->GetSearchStartAtY(), 
		this->GetSearchDimensionX(), this->GetSearchDimensionY(), this->GetEnableSegmentationAnalysis(), "frame.jpg");

	this->InitializedOn(); 
}

//----------------------------------------------------------------------------
bool vtkStepperCalibrationController::AddVtkImageData( vtkImageData* frame, const double probePosition, const double probeRotation, const double templatePosition, IMAGE_DATA_TYPE dataType )
{
	LOG_TRACE("vtkCalibrationController::AddData - vtkImage with stepper encoder values"); 
	ImageType::Pointer exportedFrame = ImageType::New();
	this->ExportVtkImageData(frame, exportedFrame); 

	return this->AddItkImageData(exportedFrame, probePosition, probeRotation, templatePosition, dataType); 
}

//----------------------------------------------------------------------------
bool vtkStepperCalibrationController::AddItkImageData( ImageType* frame, const double probePosition, const double probeRotation, const double templatePosition, IMAGE_DATA_TYPE dataType )
{
	LOG_TRACE("vtkCalibrationController::AddData - itkImage with stepper encoder values"); 
	TrackedFrame trackedFrame; 
	this->CreateTrackedFrame(frame, probePosition, probeRotation, templatePosition, dataType, trackedFrame); 
	return this->AddTrackedFrameData(&trackedFrame, dataType); 
}

//----------------------------------------------------------------------------
bool vtkStepperCalibrationController::LSQRMinimizer(const std::vector<vnl_vector<double>> &aMatrix, const std::vector<double> &bVector, vnl_vector<double> &resultVector)
{
	LOG_TRACE("vtkStepperCalibrationController::LSQRMinimizer"); 

	if (aMatrix.size()==0)
	{
		LOG_ERROR("LSQRMinimizer: A matrix is empty");
		resultVector.clear();
		return false;
	}
	if (bVector.size()==0)
	{
		LOG_ERROR("LSQRMinimizer: b vector is empty");
		resultVector.clear();
		return false;
	}

	// The coefficient matrix aMatrix should be m-by-n and the column vector bVector must have length m.
	const int n = aMatrix.begin()->size(); 
	const int m = bVector.size();

	vnl_sparse_matrix<double> sparseMatrixLeftSide(m, n);
	vnl_vector<double> vectorRightSide(m);

	for(int row = 0; row < m; row++)
	{
		// Populate the sparse matrix
		for ( int i = 0; i < n; i++)
		{
			sparseMatrixLeftSide(row,i) = aMatrix[row].get(i);
		}

		// Populate the vector
		vectorRightSide.put(row, bVector[row]);
	}

	// Construct linear system defined in VNL
	vnl_sparse_matrix_linear_system<double> linearSystem( sparseMatrixLeftSide, vectorRightSide );

	// Instantiate the LSQR solver
	vnl_lsqr lsqr( linearSystem );

	// call minimize on the solver
	lsqr.minimize( resultVector );

	return true; 
}


//----------------------------------------------------------------------------
void vtkStepperCalibrationController::ComputeStatistics(const std::vector< std::vector<double> > &diffVector, std::vector<double> &mean, std::vector<double> &stdev)
{
	// copy differences to vnl_vectors 
	std::vector< vnl_vector<double> > posDifferences; 
	for ( unsigned int i = 0; i < diffVector.size(); i++ )
	{
		vnl_vector<double> diff(diffVector[i].size()); 
		for ( int j = 0; j < diffVector[i].size(); j++ )
		{
			diff[j] = diffVector[i][j]; 
		}
		posDifferences.push_back(diff); 
	}

	// calculate mean of difference between the computed and measured position for each wire 
	mean.clear(); 
	std::vector< vnl_vector<double> > diffFromMean; 
	for ( unsigned int i = 0; i < posDifferences.size(); i++ )
	{
		mean.push_back(posDifferences[i].mean()); 
		vnl_vector<double> diff = posDifferences[i] - posDifferences[i].mean(); 
		diffFromMean.push_back(diff); 
	}

	// calculate standard deviation for each axis 
	stdev.clear(); 
	for (unsigned int i = 0; i < diffFromMean.size(); i++ )
	{
		double std = sqrt( diffFromMean[i].squared_magnitude() / (1.0 * diffFromMean[i].size()) ); 
		stdev.push_back(std); 
	}

	// print results
	for ( unsigned int i = 0; i < mean.size(); i++ )
	{
		LOG_DEBUG("Mean=" << std::fixed << mean[i] << " Std=" << stdev[i]); 
	}
}

//***************************************************************************
//						Rotation axis calibration
//***************************************************************************

//----------------------------------------------------------------------------
int vtkStepperCalibrationController::CalibrateProbeRotationAxis()
{
	LOG_TRACE("vtkStepperCalibrationController::CalibrateProbeRotationAxis"); 
	if ( ! this->GetInitialized() ) 
	{
		this->Initialize(); 
	}

	if ( !this->CalculateSpacing() )
	{
		LOG_ERROR("Failed to calibrate probe rotation axis without spacing information!"); 
		this->ProbeRotationAxisCalibratedOff(); 
		return 0;
	}

	LOG_INFO( "----------------------------------------------------"); 
	LOG_INFO( ">>>>>>>>>>>> Rotation axis calibration ..."); 
	if ( this->RotationAxisCalibration() )
	{
		LOG_INFO("CenterOfRotation (px): " << this->GetCenterOfRotationPx()[0] << "  " << this->GetCenterOfRotationPx()[1]); 
		LOG_INFO("CenterOfRotation (mm): " << this->GetCenterOfRotationPx()[0]*this->GetSpacing()[0] << "  " << this->GetCenterOfRotationPx()[1]*this->GetSpacing()[1]); 
		LOG_INFO("Probe rotation axis orientation: Rx=" << std::fixed << this->GetProbeRotationAxisOrientation()[0] << "  Ry=" << this->GetProbeRotationAxisOrientation()[1]); 
	}
	else
	{
		LOG_ERROR("Failed to calibrate probe rotation axis!"); 
		this->ProbeRotationAxisCalibratedOff(); 
		return 0; 
	}
	

	LOG_INFO( "----------------------------------------------------"); 
	LOG_INFO( ">>>>>>>>>>>> Rotation encoder calibration ..."); 
	if ( this->RotationEncoderCalibration() )
	{
		LOG_INFO("ProbeRotationEncoderScale = " << ProbeRotationEncoderScale); 
		LOG_INFO("ProbeRotationEncoderOffset = " << ProbeRotationEncoderOffset); 
	}
	else
	{
		LOG_ERROR("Failed to calibrate probe rotation encoder!"); 
		this->ProbeRotationAxisCalibratedOff(); 
		return 0; 
	}

	LOG_INFO( "----------------------------------------------------"); 
	LOG_INFO( ">>>>>>>>>>>> Phantom to probe distance calculation ..."); 
	if ( this->CalculatePhantomToProbeDistance() )
	{
		LOG_INFO("Phantom to probe distance: " << this->GetPhantomToProbeDistanceInMm()[0] << "  " << this->GetPhantomToProbeDistanceInMm()[1]); 
	}
	else
	{
		LOG_ERROR("Failed to calculate phantom to probe distance!"); 
		this->ProbeRotationAxisCalibratedOff(); 
		return 0; 
	}

	this->ProbeRotationAxisCalibratedOn(); 

	// save the input images to meta image
	if ( this->GetEnableTrackedSequenceDataSaving() )
	{
		LOG_INFO( "----------------------------------------------------"); 
		LOG_INFO(">>>>>>>> Save probe rotation data to sequence metafile..."); 
		// Save calibration dataset 
		std::ostringstream probeRotationDataFileName; 
		probeRotationDataFileName << this->CalibrationStartTime << this->GetRealtimeImageDataInfo(PROBE_ROTATION).OutputSequenceMetaFileSuffix; 
		this->SaveTrackedFrameListToMetafile( PROBE_ROTATION, this->GetOutputPath(), probeRotationDataFileName.str().c_str(), false ); 
	}

	return 1; 
}

//----------------------------------------------------------------------------
int vtkStepperCalibrationController::RotationAxisCalibration()
{
	LOG_TRACE("vtkStepperCalibrationController::RotationAxisCalibration"); 

	std::vector<SegmentedFrameList> clusteredFrames; 
	this->ClusterSegmentedFrames(PROBE_ROTATION, clusteredFrames); 

	if ( clusteredFrames.size() < this->MinNumberOfRotationClusters )
	{
		LOG_WARNING("Unable to calibrate rotation axis: Number of rotation clusters are less than the minimum requirements (" << clusteredFrames.size() << " of " << this->MinNumberOfRotationClusters << ")." ); 
		if ( clusteredFrames.size() > 0 )
		{
			double centerOfRotationPx[2] = {0, 0}; 
			this->CalculateCenterOfRotation(clusteredFrames[0], centerOfRotationPx); 
			this->SetCenterOfRotationPx( centerOfRotationPx[0], centerOfRotationPx[1]); 
			return 1; 
		}
		else
		{
			return 0; 
		}
	}

	std::vector< std::pair<double, double> > listOfCenterOfRotations; 
	std::vector< double > listOfClusterPositions; 

	for ( unsigned int cluster = 0; cluster < clusteredFrames.size(); ++cluster )
	{
		if ( clusteredFrames[cluster].size() > this->MinNumOfFramesUsedForCenterOfRotCalc )
		{
			double clusterPosition = this->GetClusterZPosition(clusteredFrames[cluster]); 
			double centerOfRotationPx[2] = {0, 0}; 
			if ( this->CalculateCenterOfRotation(clusteredFrames[cluster], centerOfRotationPx) )
			{
				LOG_INFO("Center of rotation in pixels for cluster #" << cluster << " at " << std::fixed << clusterPosition << " mm: " << centerOfRotationPx[0] << "   " << centerOfRotationPx[1]); 
				std::pair<double, double> cor (centerOfRotationPx[0], centerOfRotationPx[1]); 
				listOfCenterOfRotations.push_back( cor ); 
				listOfClusterPositions.push_back(clusterPosition); 
			}
			else
			{
				LOG_WARNING("Failed to compute center of rotation for cluster #" << cluster << " at " << std::fixed << clusterPosition << " mm"); 
			}
		}
	}

	// Construct linear equations Ax = b, where A is a matrix with m rows and 
	// n columns, b is an m-vector. 
	std::vector<vnl_vector<double>> aMatrix;
	std::vector<double> bVector; 

	// Construct linear equation for rotation axis calibration
	this->ConstrLinEqForRotationAxisCalib(listOfCenterOfRotations, listOfClusterPositions, aMatrix, bVector); 

	// [rx, ry, rx0, ry0 ]
	vnl_vector<double> rotationAxisCalibResult(4, 0);

	int numberOfEquations(0); 
	do 
	{
		numberOfEquations = bVector.size(); 
		if ( this->LSQRMinimizer(aMatrix, bVector, rotationAxisCalibResult) )
		{
			this->RemoveOutliersFromRotAxisCalibData(aMatrix, bVector, rotationAxisCalibResult); 
		}
	} 
	while (numberOfEquations != bVector.size()); 

	if ( rotationAxisCalibResult.empty() )
	{
		LOG_ERROR("Unable to calibrate rotation axis! Minimizer returned empty result."); 
		return 0; 
	}

	LOG_INFO("RotationAxisCalibResult: " << std::fixed << rotationAxisCalibResult[0] << "  " << rotationAxisCalibResult[1] << "  " << rotationAxisCalibResult[2] << "  " << rotationAxisCalibResult[3] ); 

	this->SetCenterOfRotationPx( rotationAxisCalibResult[2] / this->GetSpacing()[0], rotationAxisCalibResult[3] / this->GetSpacing()[1]); 

	// Set rotation axis orientation 
	// NOTE: If the probe goes down the wires goes up on the image 
	// => we need to change the sign of the axis to compensate it
	this->SetProbeRotationAxisOrientation(-rotationAxisCalibResult[0], -rotationAxisCalibResult[1], 1); 

	return 1; 
}

//----------------------------------------------------------------------------
void vtkStepperCalibrationController::ConstrLinEqForRotationAxisCalib( const std::vector< std::pair<double, double> > &listOfCenterOfRotations, 
																	  const std::vector< double > &listOfClusterPositions, 
																	  std::vector<vnl_vector<double>> &aMatrix, 
																	  std::vector<double> &bVector)
{
	LOG_TRACE("vtkStepperCalibrationController::ConstrLinEqForRotationAxisCalib"); 
	aMatrix.clear(); 
	bVector.clear(); 

	for ( unsigned int i = 0; i < listOfCenterOfRotations.size(); ++i )
	{
		double z = listOfClusterPositions[i]; 

		// Rotation x coordinate
		double b1 = listOfCenterOfRotations[i].first * this->GetSpacing()[0]; 
		vnl_vector<double> a1(4,0); 
		a1.put(0, z); 
		a1.put(2, 1); 

		bVector.push_back(b1); 
		aMatrix.push_back(a1); 

		// Rotation y coordinate
		double b2 = listOfCenterOfRotations[i].second * this->GetSpacing()[1]; 
		vnl_vector<double> a2(4,0); 
		a2.put(1, z); 
		a2.put(3, 1); 

		bVector.push_back(b2); 
		aMatrix.push_back(a2); 

		LOG_DEBUG("ConstrLinEqForRotationAxisCalib (rotX, rotY, probeZ): " << b1 << "  " << b2 << "  " << z ); 
	}
}

//----------------------------------------------------------------------------
void vtkStepperCalibrationController::GetRotationAxisCalibrationError(const std::vector<vnl_vector<double>> &aMatrix, 
																	  const std::vector<double> &bVector, 
																	  const vnl_vector<double> &resultVector, 
																	  std::vector<double> &mean, 
																	  std::vector<double> &stdev )
{
	LOG_TRACE("vtkStepperCalibrationController::GetRotationAxisCalibrationError"); 
	// The coefficient matrix aMatrix should be m-by-n and the column vector bVector must have length m.
	const int n = aMatrix.begin()->size(); 
	const int m = bVector.size();
	const int r = resultVector.size(); 

	const int numberOfAxes(2); 
	const double rx = resultVector[0]; 
	const double ry = resultVector[1]; 
	const double rx0 = resultVector[2]; 
	const double ry0 = resultVector[3]; 


	// calculate difference between the computed and measured position for each wire 
	std::vector< std::vector<double> > diffVector(numberOfAxes); 
	for( int row = 0; row < m; row = row + numberOfAxes)
	{
		diffVector[0].push_back( bVector[row    ] - rx0 - aMatrix[row    ].get(0) * rx ); 
		diffVector[1].push_back( bVector[row + 1] - ry0 - aMatrix[row + 1].get(1) * ry ); 
	}

	this->ComputeStatistics(diffVector, mean, stdev);
}

//----------------------------------------------------------------------------
void vtkStepperCalibrationController::RemoveOutliersFromRotAxisCalibData(std::vector<vnl_vector<double>> &aMatrix, std::vector<double> &bVector, vnl_vector<double> resultVector )
{
	LOG_TRACE("vtkStepperCalibrationController::RemoveOutliersFromRotAxisCalibData"); 
	// Calculate mean error and stdev of measured and computed rotation angles
	std::vector<double> mean, stdev;
	this->GetRotationAxisCalibrationError(aMatrix, bVector, resultVector, mean, stdev); 

	const int n = aMatrix.begin()->size(); 
	const int m = bVector.size();
	const int r = resultVector.size(); 

	const int numberOfAxes(2); 
	const double rx = resultVector[0]; 
	const double ry = resultVector[1]; 
	const double rx0 = resultVector[2]; 
	const double ry0 = resultVector[3]; 

	// remove outliers
	for( int row = m - numberOfAxes; row >= 0; row = row - numberOfAxes)
	{
		if (abs ( bVector[row     ] - rx0 - aMatrix[row    ].get(0) * rx - mean[0] ) >  this->OutlierDetectionThreshold * stdev[0] 
		||
			abs ( bVector[row + 1] - ry0 - aMatrix[row + 1].get(1) * ry - mean[1] ) >  this->OutlierDetectionThreshold * stdev[1] 
		)

		{
			LOG_DEBUG("Outlier found at row " << row ); 
			aMatrix.erase(aMatrix.begin() + row, aMatrix.begin() + row + numberOfAxes); 
			bVector.erase(bVector.begin() + row, bVector.begin() + row + numberOfAxes); 
		}
	}

}


//***************************************************************************
//						Rotation encoder calibration
//***************************************************************************


//----------------------------------------------------------------------------
int vtkStepperCalibrationController::RotationEncoderCalibration()
{
	LOG_TRACE("vtkStepperCalibrationController::RotationEncoderCalibration"); 
	if ( !this->CalculateSpacing() )
	{
		LOG_ERROR("Unable to calibrate rotation encoder without spacing information!"); 
		return 0; 
	}

	// Construct linear equations Ax = b, where A is a matrix with m rows and 
	// n columns, b is an m-vector. 
	std::vector<vnl_vector<double>> aMatrix;
	std::vector<double> bVector;

	// Construct linear equation for rotation encoder calibration
	this->ConstrLinEqForRotEncCalc(aMatrix, bVector); 

	if ( aMatrix.size() == 0 || bVector.size() == 0 )
	{
		LOG_WARNING("Rotation encoder calibration failed, no data found!"); 
		return 0; 
	}

	vnl_vector<double> rotationEncoderCalibrationResult(2,0);
	int numberOfEquations(0); 
	do
	{
		numberOfEquations = bVector.size(); 
		if ( this->LSQRMinimizer(aMatrix, bVector, rotationEncoderCalibrationResult) )
		{
			this->RemoveOutliersFromRotEncCalibData(aMatrix, bVector, rotationEncoderCalibrationResult); 
		}
	}
	while ( numberOfEquations != bVector.size() ); 

	if ( rotationEncoderCalibrationResult.empty() )
	{
		LOG_ERROR("Unable to calibrate rotation encoder! Minimizer returned empty result."); 
		return 0; 
	}

	// Calculate mean error and stdev of measured and computed wire positions for each wire
	double mean, stdev;
	this->GetRotationEncoderCalibrationError(aMatrix, bVector, rotationEncoderCalibrationResult, mean, stdev); 

	this->SaveRotationEncoderCalibrationError(aMatrix, bVector, rotationEncoderCalibrationResult); 

	this->SetProbeRotationEncoderScale(rotationEncoderCalibrationResult.get(0)); 
	this->SetProbeRotationEncoderOffset(rotationEncoderCalibrationResult.get(1)); 

	return 1; 
}

//----------------------------------------------------------------------------
void vtkStepperCalibrationController::ConstrLinEqForRotEncCalc( std::vector<vnl_vector<double>> &aMatrix, std::vector<double> &bVector)
{
	LOG_TRACE("vtkStepperCalibrationController::ConstrLinEqForRotEncCalc"); 
	aMatrix.clear(); 
	bVector.clear(); 

	for ( int frame = 0; frame < this->SegmentedFrameContainer.size(); frame++ )
	{
		if ( this->SegmentedFrameContainer[frame].DataType == PROBE_ROTATION )
		{
			double probePos(0), probeRot(0), templatePos(0); 
			if ( !this->GetStepperEncoderValues(this->SegmentedFrameContainer[frame].TrackedFrameInfo, probePos, probeRot, templatePos) )
			{
				LOG_WARNING("Probe rotation axis calibration: Unable to get probe rotation from tracked frame info for frame #" << frame); 
				continue; 
			}

			// Wire #1 coordinate in mm 
			double w1xmm = this->SegmentedFrameContainer[frame].SegResults.m_FoundDotsCoordinateValue[0][0] * this->GetSpacing()[0]; 
			double w1ymm = this->SegmentedFrameContainer[frame].SegResults.m_FoundDotsCoordinateValue[0][1] * this->GetSpacing()[1]; 

			// Wire #3 coordinate in mm 
			double w3xmm = this->SegmentedFrameContainer[frame].SegResults.m_FoundDotsCoordinateValue[2][0] * this->GetSpacing()[0]; 
			double w3ymm = this->SegmentedFrameContainer[frame].SegResults.m_FoundDotsCoordinateValue[2][1] * this->GetSpacing()[1]; 

			// Wire #4 coordinate in mm 
			double w4xmm = this->SegmentedFrameContainer[frame].SegResults.m_FoundDotsCoordinateValue[3][0] * this->GetSpacing()[0]; 
			double w4ymm = this->SegmentedFrameContainer[frame].SegResults.m_FoundDotsCoordinateValue[3][1] * this->GetSpacing()[1]; 

			// Wire #6 coordinate in mm 
			double w6xmm = this->SegmentedFrameContainer[frame].SegResults.m_FoundDotsCoordinateValue[5][0] * this->GetSpacing()[0]; 
			double w6ymm = this->SegmentedFrameContainer[frame].SegResults.m_FoundDotsCoordinateValue[5][1] * this->GetSpacing()[1]; 

			double b1 = vtkMath::DegreesFromRadians(atan2( (w3ymm - w1ymm), (w1xmm - w3xmm) )); 
			bVector.push_back(b1); 

			double b2 = vtkMath::DegreesFromRadians(atan2( (w6ymm - w4ymm), (w4xmm - w6xmm) )); 
			bVector.push_back(b2); 

			vnl_vector<double> a1(2,1);
			a1.put(0,probeRot); 
			aMatrix.push_back(a1); 

			vnl_vector<double> a2(2,1);
			a2.put(0,probeRot); 
			aMatrix.push_back(a2); 

		}
	}

}

//----------------------------------------------------------------------------
void vtkStepperCalibrationController::GetRotationEncoderCalibrationError(const std::vector<vnl_vector<double>> &aMatrix, 
																		 const std::vector<double> &bVector, 
																		 const vnl_vector<double> &resultVector, 
																		 double &mean, 
																		 double &stdev ) 
{
	LOG_TRACE("vtkStepperCalibrationController::GetRotationEncoderCalibrationError"); 
	// The coefficient matrix aMatrix should be m-by-n and the column vector bVector must have length m.
	const int n = aMatrix.begin()->size(); 
	const int m = bVector.size();
	const int r = resultVector.size(); 

	const double scale = resultVector[0]; 
	const double offset = resultVector[1]; 

	// calculate difference between the computed and measured angles
	std::vector< std::vector<double> > diffVector(1);
	std::vector<double> diff; 
	for( int row = 0; row < m; row++ )
	{
		 diff.push_back( bVector[row] - offset - aMatrix[row].get(0) * scale ); 
	}
	diffVector.push_back(diff); 

	std::vector<double> meanVector, stdevVector; 
	this->ComputeStatistics(diffVector, meanVector, stdevVector); 

	// calculate mean of difference 
	mean = 0; 
	if ( !meanVector.empty() )
	{
		mean = meanVector[0]; 
	}

	stdev = 0; 
	if ( !stdevVector.empty() )
	{
		stdev = stdevVector[0]; 
	}
}

//----------------------------------------------------------------------------
void vtkStepperCalibrationController::RemoveOutliersFromRotEncCalibData(std::vector<vnl_vector<double>> &aMatrix, std::vector<double> &bVector, vnl_vector<double> resultVector )
{
	LOG_TRACE("vtkStepperCalibrationController::RemoveOutliersFromRotEncCalibData"); 
	// Calculate mean error and stdev of measured and computed rotation angles
	double mean(0), stdev(0);
	this->GetRotationEncoderCalibrationError(aMatrix, bVector, resultVector, mean, stdev); 

	const int n = aMatrix.begin()->size(); 
	const int m = bVector.size();
	const int r = resultVector.size(); 

	const double scale = resultVector[0]; 
	const double offset = resultVector[1]; 

	// remove outliers
	for( int row = m - 1; row >= 0; row--)
	{
		if ( abs ( bVector[row] - offset - aMatrix[row].get(0) * scale - mean ) >  this->OutlierDetectionThreshold * stdev ) 
		{
			LOG_DEBUG("Outlier found at row " << row ); 
			aMatrix.erase(aMatrix.begin() + row); 
			bVector.erase(bVector.begin() + row); 
		}
	}

}

//----------------------------------------------------------------------------
void vtkStepperCalibrationController::SaveRotationEncoderCalibrationError(const std::vector<vnl_vector<double>> &aMatrix, 
																		  const std::vector<double> &bVector, 
																		  const vnl_vector<double> &resultVector)
{
	LOG_TRACE("vtkStepperCalibrationController::SaveRotationEncoderCalibrationError"); 
	std::ofstream rotationEncoderCalibrationError;
	std::ostringstream filename; 
	filename << this->OutputPath << "/" << this->CalibrationStartTime  << ".ProbeRotationEncoderCalibrationError.txt"; 

	rotationEncoderCalibrationError.open (filename.str().c_str() , ios::out);
	rotationEncoderCalibrationError << "# Probe rotation encoder calibration error report" << std::endl; 

	rotationEncoderCalibrationError << "ProbeRotationAngle\t" << "ComputedAngle\t" << "CompensatedAngle\t" << std::endl; 

	const double scale = resultVector[0]; 
	const double offset = resultVector[1]; 

	for( int row = 0; row < bVector.size(); row ++)
	{
		rotationEncoderCalibrationError << aMatrix[row].get(0) << "\t" << bVector[row] << offset + aMatrix[row].get(0) * scale << "\t" << std::endl; 
	}

	rotationEncoderCalibrationError.close(); 

}


//***************************************************************************
//					Translation axis calibration
//***************************************************************************


//----------------------------------------------------------------------------
int vtkStepperCalibrationController::CalibrateProbeTranslationAxis()
{
	int retValue(0); 

	LOG_TRACE("vtkStepperCalibrationController::CalibrateProbeTranslationAxis"); 
	if ( ! this->GetInitialized() ) 
	{
		this->Initialize(); 
	}

	if ( !this->CalculateSpacing() )
	{
		LOG_ERROR("Failed to calibrate probe translation axis without spacing information!"); 
		this->ProbeTranslationAxisCalibratedOff(); 
		return 0; 
	}

	// Probe translation axis calibration 
	LOG_INFO( "----------------------------------------------------"); 
	LOG_INFO( ">>>>>>>>>>>> Probe translation axis calibration ..."); 
	if ( this->TranslationAxisCalibration(PROBE_TRANSLATION) )
	{
		LOG_INFO("Probe translation axis orientation: Tx=" << std::fixed << this->GetProbeTranslationAxisOrientation()[0] << "  Ty=" << this->GetProbeTranslationAxisOrientation()[1]); 
		this->ProbeTranslationAxisCalibratedOn(); 
		retValue = 1; 
	}
	else
	{
		LOG_ERROR("Failed to calibrate probe translation axis!"); 
		this->ProbeTranslationAxisCalibratedOff(); 
		retValue = 0; 
	}

	// save the input images to meta image
	if ( this->GetEnableTrackedSequenceDataSaving() )
	{
		LOG_INFO( "----------------------------------------------------"); 
		LOG_INFO(">>>>>>>> Save probe translation data to sequence metafile..."); 
		// Save calibration dataset 
		std::ostringstream probeTranslationDataFileName; 
		probeTranslationDataFileName << this->CalibrationStartTime << this->GetRealtimeImageDataInfo(PROBE_TRANSLATION).OutputSequenceMetaFileSuffix; 
		this->SaveTrackedFrameListToMetafile( PROBE_TRANSLATION, this->GetOutputPath(), probeTranslationDataFileName.str().c_str(), false ); 
	}

	return retValue; 
}

//----------------------------------------------------------------------------
int vtkStepperCalibrationController::CalibrateTemplateTranslationAxis()
{
	int retValue(0); 
	LOG_TRACE("vtkStepperCalibrationController::CalibrateTemplateTranslationAxis"); 
	if ( ! this->GetInitialized() ) 
	{
		this->Initialize(); 
	}

	if ( !this->CalculateSpacing() )
	{
		LOG_ERROR("Failed to calibrate template translation axis without spacing information!"); 
		this->TemplateTranslationAxisCalibratedOff(); 
		return 0; 
	}

	// Template translation axis calibration 
	LOG_INFO( "----------------------------------------------------"); 
	LOG_INFO( ">>>>>>>>>>>> Template translation axis calibration ..."); 
	if ( this->TranslationAxisCalibration(TEMPLATE_TRANSLATION) )
	{
		LOG_INFO("Template translation axis orientation: Tx=" << std::fixed << this->GetTemplateTranslationAxisOrientation()[0] << "  Ty=" << this->GetTemplateTranslationAxisOrientation()[1]); 
		this->TemplateTranslationAxisCalibratedOn(); 
		retValue = 1; 
	}
	else
	{
		LOG_ERROR("Failed to calibrate template translation axis!"); 
		this->TemplateTranslationAxisCalibratedOff(); 
		retValue = 0; 
	}

	// save the input images to meta image
	if ( this->GetEnableTrackedSequenceDataSaving() )
	{
		LOG_INFO( "----------------------------------------------------"); 
		LOG_INFO(">>>>>>>> Save template translation data to sequence metafile..."); 
		// Save calibration dataset 
		std::ostringstream templateTranslationDataFileName; 
		templateTranslationDataFileName << this->CalibrationStartTime << this->GetRealtimeImageDataInfo(TEMPLATE_TRANSLATION).OutputSequenceMetaFileSuffix; 
		this->SaveTrackedFrameListToMetafile( TEMPLATE_TRANSLATION, this->GetOutputPath(), templateTranslationDataFileName.str().c_str(), false ); 
	}

	return retValue; 
}

//----------------------------------------------------------------------------
int vtkStepperCalibrationController::TranslationAxisCalibration( IMAGE_DATA_TYPE dataType )
{
	LOG_TRACE("vtkStepperCalibrationController::TranslationAxisCalibration"); 
	// Construct linear equations Ax = b, where A is a matrix with m rows and 
	// n columns, b is an m-vector. 
	std::vector<vnl_vector<double>> aMatrix;
	std::vector<double> bVector; 

	// Construct linear equation for translation axis calibration
	this->ConstrLinEqForTransAxisCalib(aMatrix, bVector, dataType); 

	if ( aMatrix.size() == 0 || bVector.size() == 0 )
	{
		LOG_WARNING("Translation axis calibration failed, no data found!"); 
		return 0; 
	}

	// [tx, ty, w1x0, w1y0, w3x0, w3y0, w4x0, w4y0, w6x0, w6y0 ]
	vnl_vector<double> translationAxisCalibResult(10, 0);

	int numberOfEquations(0);
	do 
	{
		numberOfEquations = bVector.size(); 
		if ( this->LSQRMinimizer(aMatrix, bVector, translationAxisCalibResult) )
		{
			this->RemoveOutliersFromTransAxisCalibData(aMatrix, bVector, translationAxisCalibResult); 
		}
	}
	while ( numberOfEquations != bVector.size() ); 

	if ( translationAxisCalibResult.empty() )
	{
		LOG_ERROR("Unable to calibrate translation axis! Minimizer returned empty result."); 
		return 0; 
	}

	// Calculate mean error and stdev of measured and computed wire positions for each wire
	std::vector<double> mean, stdev;
	this->GetTranslationAxisCalibrationError(aMatrix, bVector, translationAxisCalibResult, mean, stdev); 

	this->SaveTranslationAxisCalibrationError(aMatrix, bVector, translationAxisCalibResult, dataType); 

	// Set translation axis orientation 
	// NOTE: If the probe goes down the wires goes up on the image 
	// => we need to change the sign of the axis to compensate it
	if ( dataType == PROBE_TRANSLATION )
	{
		this->SetProbeTranslationAxisOrientation(-translationAxisCalibResult[0], -translationAxisCalibResult[1], 1); 
	}
	else if ( dataType == TEMPLATE_TRANSLATION )
	{
		this->SetTemplateTranslationAxisOrientation(-translationAxisCalibResult[0], -translationAxisCalibResult[1], 1); 
	}

	return 1; 
}

//----------------------------------------------------------------------------
void vtkStepperCalibrationController::ConstrLinEqForTransAxisCalib( std::vector<vnl_vector<double>> &aMatrix, std::vector<double> &bVector, IMAGE_DATA_TYPE dataType)
{
	LOG_TRACE("vtkStepperCalibrationController::ConstrLinEqForTransAxisCalib"); 
	aMatrix.clear(); 
	bVector.clear(); 

	for ( int frame = 0; frame < this->SegmentedFrameContainer.size(); frame++ )
	{
		if ( this->SegmentedFrameContainer[frame].DataType == dataType )
		{
			double z(0); 
			switch (dataType)
			{
			case PROBE_TRANSLATION: 
				{
					double probePos(0), probeRot(0), templatePos(0); 
					if ( !this->GetStepperEncoderValues(this->SegmentedFrameContainer[frame].TrackedFrameInfo, probePos, probeRot, templatePos) )
					{
						LOG_WARNING("Probe translation axis calibration: Unable to get probe position from tracked frame info for frame #" << frame); 
						continue; 
					}

					z = probePos; 
				}
				break; 
			case TEMPLATE_TRANSLATION: 
				{
					double probePos(0), probeRot(0), templatePos(0); 
					if ( !this->GetStepperEncoderValues(this->SegmentedFrameContainer[frame].TrackedFrameInfo, probePos, probeRot, templatePos) )
					{
						LOG_WARNING("Template translation axis calibration: Unable to get template position from tracked frame info for frame #" << frame); 
						continue; 
					}

					z = templatePos; 
				}

				break; 
			}

			// Wire #1 X coordinate in mm 
			double b1 = this->SegmentedFrameContainer[frame].SegResults.m_FoundDotsCoordinateValue[0][0] * this->GetSpacing()[0]; 
			vnl_vector<double> a1(10,0); 
			a1.put(0, z); 
			a1.put(2, 1); 

			bVector.push_back(b1); 
			aMatrix.push_back(a1); 

			// Wire #1 Y coordinate in mm 
			double b2 = this->SegmentedFrameContainer[frame].SegResults.m_FoundDotsCoordinateValue[0][1] * this->GetSpacing()[1]; 
			vnl_vector<double> a2(10,0); 
			a2.put(1, z); 
			a2.put(3, 1); 

			bVector.push_back(b2); 
			aMatrix.push_back(a2); 

			// Wire #3 X coordinate in mm 
			double b3 = this->SegmentedFrameContainer[frame].SegResults.m_FoundDotsCoordinateValue[2][0] * this->GetSpacing()[0]; 
			vnl_vector<double> a3(10,0); 
			a3.put(0, z); 
			a3.put(4, 1); 

			bVector.push_back(b3); 
			aMatrix.push_back(a3); 

			// Wire #3 Y coordinate in mm 
			double b4 = this->SegmentedFrameContainer[frame].SegResults.m_FoundDotsCoordinateValue[2][1] * this->GetSpacing()[1]; 
			vnl_vector<double> a4(10,0); 
			a4.put(1, z); 
			a4.put(5, 1); 

			bVector.push_back(b4); 
			aMatrix.push_back(a4); 

			// Wire #4 X coordinate in mm 
			double b5 = this->SegmentedFrameContainer[frame].SegResults.m_FoundDotsCoordinateValue[3][0] * this->GetSpacing()[0]; 
			vnl_vector<double> a5(10,0); 
			a5.put(0, z); 
			a5.put(6, 1); 

			bVector.push_back(b5); 
			aMatrix.push_back(a5); 

			// Wire #4 Y coordinate in mm 
			double b6 = this->SegmentedFrameContainer[frame].SegResults.m_FoundDotsCoordinateValue[3][1] * this->GetSpacing()[1]; 
			vnl_vector<double> a6(10,0); 
			a6.put(1, z); 
			a6.put(7, 1); 

			bVector.push_back(b6); 
			aMatrix.push_back(a6); 

			// Wire #6 X coordinate in mm 
			double b7 = this->SegmentedFrameContainer[frame].SegResults.m_FoundDotsCoordinateValue[5][0] * this->GetSpacing()[0]; 
			vnl_vector<double> a7(10,0); 
			a7.put(0, z); 
			a7.put(8, 1); 

			bVector.push_back(b7); 
			aMatrix.push_back(a7); 

			// Wire #6 Y coordinate in mm 
			double b8 = this->SegmentedFrameContainer[frame].SegResults.m_FoundDotsCoordinateValue[5][1] * this->GetSpacing()[1]; 
			vnl_vector<double> a8(10,0); 
			a8.put(1, z); 
			a8.put(9, 1); 

			bVector.push_back(b8); 
			aMatrix.push_back(a8); 
		}
	}

}

//----------------------------------------------------------------------------
void vtkStepperCalibrationController::GetTranslationAxisCalibrationError(const std::vector<vnl_vector<double>> &aMatrix, 
																		 const std::vector<double> &bVector, 
																		 const vnl_vector<double> &resultVector, 
																		 std::vector<double> &mean, 
																		 std::vector<double> &stdev )
{
	LOG_TRACE("vtkStepperCalibrationController::GetTranslationAxisCalibrationError"); 
	// The coefficient matrix aMatrix should be m-by-n and the column vector bVector must have length m.
	const int n = aMatrix.begin()->size(); 
	const int m = bVector.size();
	const int r = resultVector.size(); 

	const int numOfSegmentedPoints(8); 
	const double tx = resultVector[0]; 
	const double ty = resultVector[1]; 
	const double w1x = resultVector[2]; 
	const double w1y = resultVector[3]; 
	const double w3x = resultVector[4]; 
	const double w3y = resultVector[5]; 
	const double w4x = resultVector[6]; 
	const double w4y = resultVector[7]; 
	const double w6x = resultVector[8]; 
	const double w6y = resultVector[9]; 


	// calculate difference between the computed and measured position for each wire 
	std::vector< std::vector<double> > diffVector(numOfSegmentedPoints); 
	for( int row = 0; row < m; row = row + numOfSegmentedPoints)
	{
		diffVector[0].push_back( bVector[row    ] - w1x - aMatrix[row    ].get(0) * tx ); 
		diffVector[1].push_back( bVector[row + 1] - w1y - aMatrix[row + 1].get(1) * ty ); 

		diffVector[2].push_back( bVector[row + 2] - w3x - aMatrix[row + 2].get(0) * tx ); 
		diffVector[3].push_back( bVector[row + 3] - w3y - aMatrix[row + 3].get(1) * ty ); 

		diffVector[4].push_back( bVector[row + 4] - w4x - aMatrix[row + 4].get(0) * tx ); 
		diffVector[5].push_back( bVector[row + 5] - w4y - aMatrix[row + 5].get(1) * ty ); 

		diffVector[6].push_back( bVector[row + 6] - w6x - aMatrix[row + 6].get(0) * tx ); 
		diffVector[7].push_back( bVector[row + 7] - w6y - aMatrix[row + 7].get(1) * ty ); 
	}

	this->ComputeStatistics(diffVector, mean, stdev); 
}

//----------------------------------------------------------------------------
void vtkStepperCalibrationController::SaveTranslationAxisCalibrationError(const std::vector<vnl_vector<double>> &aMatrix, 
																		  const std::vector<double> &bVector, 
																		  const vnl_vector<double> &resultVector, 
																		  IMAGE_DATA_TYPE dataType)
{
	LOG_TRACE("vtkStepperCalibrationController::SaveTranslationAxisCalibrationError"); 
	const int numOfSegmentedPoints(8); 
	std::ostringstream filename; 
	std::ostringstream path; 

	std::ofstream translationAxisCalibrationError;
	if ( dataType == PROBE_TRANSLATION )
	{
		path << this->OutputPath << "/" << this->CalibrationStartTime  << ".ProbeTranslationAxisCalibrationError.txt"; 
		this->SetProbeTranslationAxisCalibrationErrorReportFilePath(path.str().c_str()); 

		translationAxisCalibrationError.open (path.str().c_str(), ios::out);
		translationAxisCalibrationError << "# Probe translation axis calibration error report" << std::endl; 
	}
	else if ( dataType == TEMPLATE_TRANSLATION )
	{
		path << this->OutputPath << "/" << this->CalibrationStartTime  << ".TemplateTranslationAxisCalibrationError.txt"; 
		this->SetTemplateTranslationAxisCalibrationErrorReportFilePath(path.str().c_str()); 
		translationAxisCalibrationError.open (path.str().c_str(), ios::out);
		translationAxisCalibrationError << "# Template translation axis calibration error report" << std::endl; 
	}

	translationAxisCalibrationError << "ProbePosition\t"
		<< "MeasuredWire1xInImageMm\tMeasuredWire1yInImageMm\tMeasuredWire3xInImage\tMeasuredWire3yInImageMm\tMeasuredWire4xInImageMm\tMeasuredWire4yInImageMm\tMeasuredWire6xInImageMm\tMeasuredWire6yInImageMm\t" 
		<< "ComputedWire1xInImageMm\tComputedWire1yInImageMm\tComputedWire3xInImage\tComputedWire3yInImageMm\tComputedWire4xInImageMm\tComputedWire4yInImageMm\tComputedWire6xInImageMm\tComputedWire6yInImageMm\t" 
		<< std::endl; 


	for( int row = 0; row < bVector.size(); row = row + numOfSegmentedPoints)
	{
		translationAxisCalibrationError << aMatrix[row    ].get(0) << "\t"
			<< bVector[row] << "\t" << bVector[row+1] << "\t" << bVector[row+2] << "\t" << bVector[row+3] << "\t" 
			<< bVector[row+4] << "\t" << bVector[row+5] << "\t" << bVector[row+6] << "\t" << bVector[row+7] << "\t" 
			<< resultVector[2] + aMatrix[row    ].get(0) * resultVector[0] << "\t" 
			<< resultVector[3] + aMatrix[row + 1].get(1) * resultVector[1] << "\t"
			<< resultVector[4] + aMatrix[row + 2].get(0) * resultVector[0] << "\t"
			<< resultVector[5] + aMatrix[row + 3].get(1) * resultVector[1] << "\t"
			<< resultVector[6] + aMatrix[row + 4].get(0) * resultVector[0] << "\t"
			<< resultVector[7] + aMatrix[row + 5].get(1) * resultVector[1] << "\t"
			<< resultVector[8] + aMatrix[row + 6].get(0) * resultVector[0] << "\t"
			<< resultVector[9] + aMatrix[row + 7].get(1) * resultVector[1] << "\t"
			<< std::endl; 
	}

	translationAxisCalibrationError.close(); 
}

//----------------------------------------------------------------------------
void vtkStepperCalibrationController::RemoveOutliersFromTransAxisCalibData(std::vector<vnl_vector<double>> &aMatrix, std::vector<double> &bVector, const vnl_vector<double> &resultVector )
{
	LOG_TRACE("vtkStepperCalibrationController::RemoveOutliersFromTransAxisCalibData"); 
	// Calculate mean error and stdev of measured and computed wire positions for each wire
	std::vector<double> mean, stdev;
	this->GetTranslationAxisCalibrationError(aMatrix, bVector, resultVector, mean, stdev); 

	const int n = aMatrix.begin()->size(); 
	const int m = bVector.size();
	const int r = resultVector.size(); 

	const int numOfSegmentedPoints(8); 
	const double tx = resultVector[0]; 
	const double ty = resultVector[1]; 
	const double w1x = resultVector[2]; 
	const double w1y = resultVector[3]; 
	const double w3x = resultVector[4]; 
	const double w3y = resultVector[5]; 
	const double w4x = resultVector[6]; 
	const double w4y = resultVector[7]; 
	const double w6x = resultVector[8]; 
	const double w6y = resultVector[9]; 

	// remove outliers
	for( int row = m - numOfSegmentedPoints; row > 0; row = row - numOfSegmentedPoints )
	{
		if ( abs ( bVector[row    ] - w1x - aMatrix[row    ].get(0) * tx - mean[0] ) >  this->OutlierDetectionThreshold * stdev[0] 
		||
			abs ( bVector[row + 1] - w1y - aMatrix[row + 1].get(1) * ty - mean[1] ) >  this->OutlierDetectionThreshold * stdev[1] 
		||
			abs ( bVector[row + 2] - w3x - aMatrix[row + 2].get(0) * tx - mean[2] ) >  this->OutlierDetectionThreshold * stdev[2] 
		||
			abs ( bVector[row + 3] - w3y - aMatrix[row + 3].get(1) * ty - mean[3] ) >  this->OutlierDetectionThreshold * stdev[3] 
		||
			abs ( bVector[row + 4] - w4x - aMatrix[row + 4].get(0) * tx - mean[4] ) >  this->OutlierDetectionThreshold * stdev[4] 
		||
			abs ( bVector[row + 5] - w4y - aMatrix[row + 5].get(1) * ty - mean[5] ) >  this->OutlierDetectionThreshold * stdev[5] 
		||
			abs ( bVector[row + 6] - w6x - aMatrix[row + 6].get(0) * tx - mean[6] ) >  this->OutlierDetectionThreshold * stdev[6] 
		||
			abs ( bVector[row + 7] - w6y - aMatrix[row + 7].get(1) * ty - mean[7] ) >  this->OutlierDetectionThreshold * stdev[7] 
		)
		{
			LOG_DEBUG("Outlier found at row " << row ); 
			aMatrix.erase(aMatrix.begin() + row, aMatrix.begin() + row + numOfSegmentedPoints); 
			bVector.erase(bVector.begin() + row, bVector.begin() + row + numOfSegmentedPoints); 
		}
	}

}

//----------------------------------------------------------------------------
void vtkStepperCalibrationController::GenerateProbeTranslationAxisCalibrationReport( vtkHTMLGenerator* htmlReport, vtkGnuplotExecuter* plotter, const char* gnuplotScriptsFolder)
{
	LOG_TRACE("vtkStepperCalibrationController::GenerateProbeTranslationAxisCalibrationReport"); 
	this->GenerateTranslationAxisCalibrationReport(PROBE_TRANSLATION, htmlReport, plotter, gnuplotScriptsFolder); 
}

//----------------------------------------------------------------------------
void vtkStepperCalibrationController::GenerateTemplateTranslationAxisCalibrationReport( vtkHTMLGenerator* htmlReport, vtkGnuplotExecuter* plotter, const char* gnuplotScriptsFolder)
{
	LOG_TRACE("vtkStepperCalibrationController::GenerateTemplateTranslationAxisCalibrationReport"); 
	this->GenerateTranslationAxisCalibrationReport(TEMPLATE_TRANSLATION, htmlReport, plotter, gnuplotScriptsFolder); 
}

//----------------------------------------------------------------------------
void vtkStepperCalibrationController::GenerateTranslationAxisCalibrationReport( IMAGE_DATA_TYPE dataType, vtkHTMLGenerator* htmlReport, vtkGnuplotExecuter* plotter, const char* gnuplotScriptsFolder)
{
	if ( htmlReport == NULL || plotter == NULL )
	{
		LOG_ERROR("Caller should define HTML report generator and gnuplot plotter before report generation!"); 
		return; 
	}

	std::string plotStepperCalibrationErrorHistogramScript = gnuplotScriptsFolder + std::string("/PlotStepperCalibrationErrorHistogram.gnu"); 
	if ( !vtksys::SystemTools::FileExists( plotStepperCalibrationErrorHistogramScript.c_str(), true) )
	{
		LOG_ERROR("Unable to find gnuplot script at: " << plotStepperCalibrationErrorHistogramScript); 
		return; 
	}

	std::string plotStepperCalibrationErrorScript = gnuplotScriptsFolder + std::string("/PlotStepperCalibrationError.gnu"); 
	if ( !vtksys::SystemTools::FileExists( plotStepperCalibrationErrorScript.c_str(), true) )
	{
		LOG_ERROR("Unable to find gnuplot script at: " << plotStepperCalibrationErrorScript); 
		return; 
	}

	bool calibrated(false); 
	switch ( dataType )
	{
	case PROBE_TRANSLATION: 
		calibrated = this->GetProbeTranslationAxisCalibrated(); 
		break; 
	case TEMPLATE_TRANSLATION: 
		calibrated = this->GetTemplateTranslationAxisCalibrated(); 
		break; 
	}

	if ( calibrated )
	{
		std::string reportFile;
		switch ( dataType )
		{
		case PROBE_TRANSLATION: 
			reportFile = this->GetProbeTranslationAxisCalibrationErrorReportFilePath(); 
			break; 
		case TEMPLATE_TRANSLATION: 
			reportFile = this->GetTemplateTranslationAxisCalibrationErrorReportFilePath(); 
			break; 
		}

		if ( !vtksys::SystemTools::FileExists( reportFile.c_str(), true) )
		{
			LOG_ERROR("Unable to find translation axis calibration report file at: " << reportFile); 
			return; 
		}

		std::string title; 
		std::string scriptOutputFilePrefixHistogram, scriptOutputFilePrefix; 
		switch ( dataType )
		{
		case PROBE_TRANSLATION: 
			title = "Probe Translation Axis Calibration Analysis"; 
			scriptOutputFilePrefixHistogram = "ProbeTranslationAxisCalibrationErrorHistogram"; 
			scriptOutputFilePrefix = "ProbeTranslationAxisCalibrationError"; 
			break; 
		case TEMPLATE_TRANSLATION: 
			title = "Template Translation Axis Calibration Analysis";
			scriptOutputFilePrefixHistogram = "TemplateTranslationAxisCalibrationErrorHistogram"; 
			scriptOutputFilePrefix = "TemplateTranslationAxisCalibrationError"; 
			break; 
		}

		htmlReport->AddText(title.c_str(), vtkHTMLGenerator::H1); 

		htmlReport->AddText("Error Histogram", vtkHTMLGenerator::H2); 

		const int wires[4] = {1, 3, 4, 6}; 

		for ( int i = 0; i < 4; i++ )
		{
			std::ostringstream wireName; 
			wireName << "Wire #" << wires[i] << std::ends; 
			htmlReport->AddText(wireName.str().c_str(), vtkHTMLGenerator::H3); 
			plotter->ClearArguments(); 
			plotter->AddArgument("-e");
			std::ostringstream transAxisError; 
			transAxisError << "f='" << reportFile << "'; o='" << scriptOutputFilePrefixHistogram << "'; w=" << wires[i] << std::ends; 
			plotter->AddArgument(transAxisError.str().c_str()); 
			plotter->AddArgument(plotStepperCalibrationErrorHistogramScript.c_str());  
			plotter->Execute(); 
			plotter->ClearArguments(); 

			std::ostringstream imageSource; 
			std::ostringstream imageAlt; 

			switch ( dataType )
			{
			case PROBE_TRANSLATION: 
				imageSource << "w" << wires[i] << "_ProbeTranslationAxisCalibrationErrorHistogram.jpg" << std::ends; 
				imageAlt << "Probe translation axis calibration error histogram - wire #" << wires[i] << std::ends; 
				break; 
			case TEMPLATE_TRANSLATION: 
				imageSource << "w" << wires[i] << "_TemplateTranslationAxisCalibrationErrorHistogram.jpg" << std::ends; 
				imageAlt << "Template translation axis calibration error histogram - wire #" << wires[i] << std::ends; 
				break; 
			}

			htmlReport->AddImage(imageSource.str().c_str(), imageAlt.str().c_str()); 
		}


		/////////////////////////////////////////////////////////////////////////////////////////////

		htmlReport->AddText("Error Plot", vtkHTMLGenerator::H2); 

		for ( int i = 0; i < 4; i++ )
		{
			std::ostringstream wireName; 
			wireName << "Wire #" << wires[i] << std::ends; 
			htmlReport->AddText(wireName.str().c_str(), vtkHTMLGenerator::H3); 
			plotter->ClearArguments(); 
			plotter->AddArgument("-e");
			std::ostringstream transAxisError; 
			transAxisError << "f='" << reportFile << "'; o='" << scriptOutputFilePrefix << "'; w=" << wires[i] << std::ends; 
			plotter->AddArgument(transAxisError.str().c_str()); 
			plotter->AddArgument(plotStepperCalibrationErrorScript.c_str());  
			plotter->Execute(); 
			plotter->ClearArguments(); 

			std::ostringstream imageSourceX, imageAltX, imageSourceY, imageAltY; 

			switch ( dataType )
			{
			case PROBE_TRANSLATION: 
				imageSourceX << "w" << wires[i] << "x_ProbeTranslationAxisCalibrationError.jpg" << std::ends; 
				imageAltX << "Probe translation axis calibration error - wire #" << wires[i] << " X Axis" << std::ends; 
				imageSourceY << "w" << wires[i] << "y_ProbeTranslationAxisCalibrationError.jpg" << std::ends; 
				imageAltY << "Probe translation axis calibration error - wire #" << wires[i] << " Y Axis" << std::ends; 
				break; 
			case TEMPLATE_TRANSLATION: 
				imageSourceX << "w" << wires[i] << "x_TemplateTranslationAxisCalibrationError.jpg" << std::ends; 
				imageAltX << "Template translation axis calibration error - wire #" << wires[i] << " X Axis" << std::ends; 
				imageSourceY << "w" << wires[i] << "y_TemplateTranslationAxisCalibrationError.jpg" << std::ends; 
				imageAltY << "Template translation axis calibration error - wire #" << wires[i] << " Y Axis" << std::ends; 
				break; 
			}

			htmlReport->AddImage(imageSourceX.str().c_str(), imageAltX.str().c_str()); 
			htmlReport->AddImage(imageSourceY.str().c_str(), imageAltY.str().c_str()); 
		}

		htmlReport->AddHorizontalLine(); 
	}
}

//***************************************************************************
//							Spacing calculation
//***************************************************************************

//----------------------------------------------------------------------------
int vtkStepperCalibrationController::CalculateSpacing()
{
	LOG_TRACE("vtkStepperCalibrationController::CalculateSpacing"); 
	if ( this->GetSpacingCalculated() )
	{
		// we already calculated it, no need to recalculate
		return 1; 
	}

	LOG_INFO( ">>>>>>>>>>>> Image spacing calculation ..."); 

	// Construct linear equations Ax = b, where A is a matrix with m rows and 
	// n columns, b is an m-vector. 
	std::vector<vnl_vector<double>> aMatrix;
	std::vector<double> bVector;

	// Construct linear equation for spacing calculation
	this->ConstrLinEqForSpacingCalc(aMatrix, bVector); 

	if ( aMatrix.size() == 0 || bVector.size() == 0 )
	{
		LOG_WARNING("Spacing calculation failed, no data found!"); 
		return 0; 
	}

	// The TRUS Scale factors
	// - Sx: lateral axis;
	// - Sy: axial axis;
	// - Units in mm/pixel.
	vnl_vector<double> TRUSSquaredScaleFactorsInMMperPixel2x1(2,0);

	int numberOfEquations(0); 
	do 
	{
		numberOfEquations = bVector.size(); 
		if ( this->LSQRMinimizer(aMatrix, bVector, TRUSSquaredScaleFactorsInMMperPixel2x1) )
		{
			this->RemoveOutliersFromSpacingCalcData(aMatrix, bVector, TRUSSquaredScaleFactorsInMMperPixel2x1); 
		}
	} 
	while (numberOfEquations != bVector.size()); 

	if ( TRUSSquaredScaleFactorsInMMperPixel2x1.empty() )
	{
		LOG_ERROR("Unable to calculate spacing! Minimizer returned empty result."); 
		return 0; 
	}

	this->SetSpacing( sqrt(TRUSSquaredScaleFactorsInMMperPixel2x1.get(0)), sqrt(TRUSSquaredScaleFactorsInMMperPixel2x1.get(1)) );

	this->SpacingCalculatedOn(); 

	LOG_INFO("Spacing: " << this->GetSpacing()[0] << "  " << this->GetSpacing()[1]); 

	return 1; 
}


//----------------------------------------------------------------------------
void vtkStepperCalibrationController::ConstrLinEqForSpacingCalc( std::vector<vnl_vector<double>> &aMatrix, std::vector<double> &bVector)
{
	LOG_TRACE("vtkStepperCalibrationController::ConstrLinEqForSpacingCalc"); 
	aMatrix.clear(); 
	bVector.clear(); 

	for ( int frame = 0; frame < this->SegmentedFrameContainer.size(); frame++ )
	{
		// Compute distance between line #1 and #3 for scaling computation 
		// Constant Distance Measurements from iCAL phantom design in mm
		const double distanceN1ToN3inMm(40); // TODO: read it from pahantom design
		double xDistanceN1ToN3Px = this->SegmentedFrameContainer[frame].SegResults.m_FoundDotsCoordinateValue[0][0] - this->SegmentedFrameContainer[frame].SegResults.m_FoundDotsCoordinateValue[2][0]; 
		double yDistanceN1ToN3Px = this->SegmentedFrameContainer[frame].SegResults.m_FoundDotsCoordinateValue[0][1] - this->SegmentedFrameContainer[frame].SegResults.m_FoundDotsCoordinateValue[2][1]; 

		// Populate the sparse matrix with squared distances in pixel 
		vnl_vector<double> scaleFactorN1ToN3(2,0); 
		scaleFactorN1ToN3.put(0, pow(xDistanceN1ToN3Px, 2));
		scaleFactorN1ToN3.put(1, pow(yDistanceN1ToN3Px, 2));
		aMatrix.push_back(scaleFactorN1ToN3); 

		// Populate the vector with squared distances in mm 
		bVector.push_back(pow(distanceN1ToN3inMm, 2));

		// Compute distance between line #3 and #6 for scaling computation 
		// Constant Distance Measurements from iCAL phantom design in mm
		const double distanceN3ToN6inMm(20); // TODO: read it from pahantom design
		double xDistanceN3ToN6Px = this->SegmentedFrameContainer[frame].SegResults.m_FoundDotsCoordinateValue[2][0] - this->SegmentedFrameContainer[frame].SegResults.m_FoundDotsCoordinateValue[5][0]; 
		double yDistanceN3ToN6Px = this->SegmentedFrameContainer[frame].SegResults.m_FoundDotsCoordinateValue[2][1] - this->SegmentedFrameContainer[frame].SegResults.m_FoundDotsCoordinateValue[5][1]; 

		// Populate the sparse matrix with squared distances in pixel 
		vnl_vector<double> scaleFactorN3ToN6(2,0); 
		scaleFactorN3ToN6.put(0, pow(xDistanceN3ToN6Px, 2));
		scaleFactorN3ToN6.put(1, pow(yDistanceN3ToN6Px, 2));
		aMatrix.push_back(scaleFactorN3ToN6); 

		// Populate the vector with squared distances in mm 
		bVector.push_back(pow(distanceN3ToN6inMm, 2));
	}
}

//----------------------------------------------------------------------------
void vtkStepperCalibrationController::GetSpacingCalculationError(
		const std::vector<vnl_vector<double>> &aMatrix, 
		const std::vector<double> &bVector, 
		const vnl_vector<double> &resultVector, 
		std::vector<double> &mean, 
		std::vector<double> &stdev )
{

	LOG_TRACE("vtkStepperCalibrationController::GetRotationAxisCalibrationError"); 
	// The coefficient matrix aMatrix should be m-by-n and the column vector bVector must have length m.
	const int n = aMatrix.begin()->size(); 
	const int m = bVector.size();
	const int r = resultVector.size(); 

	const int numberOfAxes(2); 
	const double sX = resultVector[0]; 
	const double sY = resultVector[1]; 

	// calculate difference between the computed and measured position for each pair
	std::vector< std::vector<double> > diffVector(numberOfAxes); 
	for( int row = 0; row < m; row = row + numberOfAxes)
	{
		diffVector[0].push_back( bVector[row    ] - aMatrix[row    ].get(0) * sX ); 
		diffVector[1].push_back( bVector[row + 1] - aMatrix[row + 1].get(1) * sY ); 
	}

	this->ComputeStatistics(diffVector, mean, stdev); 
}

//----------------------------------------------------------------------------
void vtkStepperCalibrationController::RemoveOutliersFromSpacingCalcData(
		std::vector<vnl_vector<double>> &aMatrix, 
		std::vector<double> &bVector, 
		vnl_vector<double> resultVector )
{

	LOG_TRACE("vtkStepperCalibrationController::RemoveOutliersFromSpacingCalcData"); 
	// Calculate mean error and stdev of measured and computed wire distances
	std::vector<double> mean, stdev;
	this->GetSpacingCalculationError(aMatrix, bVector, resultVector, mean, stdev); 

	const int n = aMatrix.begin()->size(); 
	const int m = bVector.size();
	const int r = resultVector.size(); 

	const int numberOfAxes(2); 
	const double sX = resultVector[0]; 
	const double sY = resultVector[1]; 

	// remove outliers
	for( int row = m - numberOfAxes; row >= 0; row = row - numberOfAxes)
	{
		if (abs ( bVector[row    ] - aMatrix[row    ].get(0) * sX - mean[0] ) >  this->OutlierDetectionThreshold * stdev[0] 
			||
			abs ( bVector[row + 1] - aMatrix[row + 1].get(1) * sY - mean[1] ) >  this->OutlierDetectionThreshold * stdev[1] )
		{
			LOG_DEBUG("Outlier found at row " << row ); 
			aMatrix.erase(aMatrix.begin() + row, aMatrix.begin() + row + numberOfAxes); 
			bVector.erase(bVector.begin() + row, bVector.begin() + row + numberOfAxes); 
		}
	}
}

//***************************************************************************
//						Center of rotation calculation
//***************************************************************************

//----------------------------------------------------------------------------
int vtkStepperCalibrationController::CalculateCenterOfRotation( const SegmentedFrameList &frameListForCenterOfRotation, double centerOfRotationPx[2] )
{
	LOG_TRACE("vtkStepperCalibrationController::CalculateCenterOfRotation"); 
	// ====================================================================
	// Compute the TRUS rotation center using linear least squares
	// ====================================================================
	// Note: the number of the distant pairs is a combination (C2)
	// of the number of the segmented N-Fiducial_1 positions in TRUS.
	// N in LaTeX = {NumDataPoints}^C_{2}
	// E.g.: for 200 positions as used for thresholding here, N = 19,900.
	// ====================================================================

	std::vector< std::vector<HomogenousVector4x1> > pointSetForCenterOfRotationCalculation;

	for ( int frame = 0; frame < frameListForCenterOfRotation.size(); frame++ )
	{
		std::vector<HomogenousVector4x1> vectorOfWirePoints; 

		// Add Line #1 pixel coordinates to center of rotation point set 
		vectorOfWirePoints.push_back( HomogenousVector4x1( frameListForCenterOfRotation[frame].SegResults.m_FoundDotsCoordinateValue[0][0], frameListForCenterOfRotation[frame].SegResults.m_FoundDotsCoordinateValue[0][1], 0 ) ); 

		// Add Line #3 pixel coordinates to center of rotation point set 
		vectorOfWirePoints.push_back( HomogenousVector4x1( frameListForCenterOfRotation[frame].SegResults.m_FoundDotsCoordinateValue[2][0], frameListForCenterOfRotation[frame].SegResults.m_FoundDotsCoordinateValue[2][1], 0 ) ); 

		// Add Line #4 pixel coordinates to center of rotation point set 
		vectorOfWirePoints.push_back( HomogenousVector4x1( frameListForCenterOfRotation[frame].SegResults.m_FoundDotsCoordinateValue[3][0], frameListForCenterOfRotation[frame].SegResults.m_FoundDotsCoordinateValue[3][1], 0 ) ); 

		// Add Line #6 pixel coordinates to center of rotation point set 
		vectorOfWirePoints.push_back( HomogenousVector4x1( frameListForCenterOfRotation[frame].SegResults.m_FoundDotsCoordinateValue[5][0], frameListForCenterOfRotation[frame].SegResults.m_FoundDotsCoordinateValue[5][1], 0 ) ); 

		pointSetForCenterOfRotationCalculation.push_back(vectorOfWirePoints); 
	}

	// Data containers
	std::vector<vnl_vector<double>> aMatrix;
	std::vector<double> bVector;

	for( unsigned int i = 0; i <= pointSetForCenterOfRotationCalculation.size() - 2; i++ )
	{
		for( unsigned int j = i + 1; j <= pointSetForCenterOfRotationCalculation.size() - 1; j++ )
		{
			for ( int point = 0; point < pointSetForCenterOfRotationCalculation[i].size(); point++ )
			{
				// coordiates of the i-th element
				double Xi = pointSetForCenterOfRotationCalculation[i][point].GetX() * this->GetSpacing()[0]; 
				double Yi = pointSetForCenterOfRotationCalculation[i][point].GetY() * this->GetSpacing()[1]; 

				// coordiates of the j-th element
				double Xj = pointSetForCenterOfRotationCalculation[j][point].GetX() * this->GetSpacing()[0]; 
				double Yj = pointSetForCenterOfRotationCalculation[j][point].GetY() * this->GetSpacing()[1]; 

				// Populate the list of distance
				vnl_vector<double> rowOfDistance(2,0);
				rowOfDistance.put(0, Xi - Xj);
				rowOfDistance.put(1, Yi - Yj);
				aMatrix.push_back( rowOfDistance );

				// Populate the squared distance vector
				bVector.push_back( 0.5*( Xi*Xi + Yi*Yi - Xj*Xj - Yj*Yj ));
			}
		}
	}

	if ( aMatrix.size() == 0 || bVector.size() == 0 )
	{
		LOG_WARNING("Center of rotation calculation failed, no data found!"); 
		return 0; 
	}


	// The TRUS rotation center in original image frame
	// The same as the segmentation coordinating system
	// - Origin: Left-upper corner;
	// - Positive X: to the right;
	// - Positive Y: to the bottom;
	// - Units in pixels.
	vnl_vector<double> TRUSRotationCenterInOriginalImageFrameInMm2x1(2,0);
	int numberOfEquations(0); 
	do 
	{
		numberOfEquations = bVector.size(); 
		if ( this->LSQRMinimizer(aMatrix, bVector, TRUSRotationCenterInOriginalImageFrameInMm2x1) )
		{
			this->RemoveOutliersFromCenterOfRotCalcData(aMatrix, bVector, TRUSRotationCenterInOriginalImageFrameInMm2x1); 
		}
	} 
	while (numberOfEquations != bVector.size()); 

	if ( TRUSRotationCenterInOriginalImageFrameInMm2x1.empty() )
	{
		LOG_ERROR("Unable to calculate center of rotation! Minimizer returned empty result."); 
		return 0; 
	}

	// Calculate mean error and stdev of measured and computed distances
	double mean, stdev;
	this->GetCenterOfRotationCalculationError(aMatrix, bVector, TRUSRotationCenterInOriginalImageFrameInMm2x1, mean, stdev); 

	//this->SaveCenterOfRotationCalculationError(aMatrix, bVector, TRUSRotationCenterInOriginalImageFrameInMm2x1); 

	centerOfRotationPx[0] = TRUSRotationCenterInOriginalImageFrameInMm2x1.get(0) / this->GetSpacing()[0]; 
	centerOfRotationPx[1] = TRUSRotationCenterInOriginalImageFrameInMm2x1.get(1) / this->GetSpacing()[1]; 

	return 1; 
}

//----------------------------------------------------------------------------
void vtkStepperCalibrationController::GetCenterOfRotationCalculationError(const std::vector<vnl_vector<double>> &aMatrix, 
																		  const std::vector<double> &bVector, 
																		  const vnl_vector<double> &resultVector, 
																		  double &mean, 
																		  double &stdev ) 
{
	LOG_TRACE("vtkStepperCalibrationController::GetCenterOfRotationCalculationError"); 
	// The coefficient matrix aMatrix should be m-by-n and the column vector bVector must have length m.
	const int n = aMatrix.begin()->size(); 
	const int m = bVector.size();
	const int r = resultVector.size(); 

	const double centerOfRotationX = resultVector[0]; 
	const double centerOfRotationY = resultVector[1]; 

	// calculate difference between the computed and measured angles
	std::vector< std::vector<double> > diffVector(1);
	std::vector<double> diff; 
	for( int row = 0; row < m; row++ )
	{
		 diff.push_back( bVector[row] - aMatrix[row].get(0) * centerOfRotationX - aMatrix[row].get(1) * centerOfRotationY ); 
	}
	diffVector.push_back(diff); 

	std::vector<double> meanVector, stdevVector; 
	this->ComputeStatistics(diffVector, meanVector, stdevVector); 

	// calculate mean of difference 
	mean = 0; 
	if ( !meanVector.empty() )
	{
		mean = meanVector[0]; 
	}

	stdev = 0; 
	if ( !stdevVector.empty() )
	{
		stdev = stdevVector[0]; 
	}
}

//----------------------------------------------------------------------------
void vtkStepperCalibrationController::RemoveOutliersFromCenterOfRotCalcData(std::vector<vnl_vector<double>> &aMatrix, 
																			std::vector<double> &bVector, 
																			const vnl_vector<double> &resultVector )
{
	LOG_TRACE("vtkStepperCalibrationController::RemoveOutliersFromCenterOfRotCalcData"); 
	// Calculate mean error and stdev of measured and computed rotation angles
	double mean(0), stdev(0);
	this->GetCenterOfRotationCalculationError(aMatrix, bVector, resultVector, mean, stdev); 

	const int n = aMatrix.begin()->size(); 
	const int m = bVector.size();
	const int r = resultVector.size(); 

	const double centerOfRotationX = resultVector[0]; 
	const double centerOfRotationY = resultVector[1];  

	// remove outliers
	for( int row = m - 1; row >= 0; row--)
	{
		if ( abs (bVector[row] - aMatrix[row].get(0) * centerOfRotationX - aMatrix[row].get(1) * centerOfRotationY - mean ) >  this->OutlierDetectionThreshold * stdev ) 
		{
			LOG_DEBUG("Outlier found at row " << row ); 
			aMatrix.erase(aMatrix.begin() + row); 
			bVector.erase(bVector.begin() + row); 
		}
	}
}


//***************************************************************************
//					Phantom to probe distance calculation
//***************************************************************************


//----------------------------------------------------------------------------
void vtkStepperCalibrationController::AddPointsForPhantomToProbeDistanceCalculation(HomogenousVector4x1 pointA, HomogenousVector4x1 pointB, HomogenousVector4x1 pointC)
{
	LOG_TRACE("vtkStepperCalibrationController::AddPointsForPhantomToProbeDistanceCalculation - HomogenousVector4x1"); 
	std::vector<HomogenousVector4x1> vectorOfPointCoordinates; 
	vectorOfPointCoordinates.push_back(pointA); 
	vectorOfPointCoordinates.push_back(pointB); 
	vectorOfPointCoordinates.push_back(pointC); 
	this->PointSetForPhantomToProbeDistanceCalculation.push_back(vectorOfPointCoordinates); 
}

//----------------------------------------------------------------------------
void vtkStepperCalibrationController::AddPointsForPhantomToProbeDistanceCalculation(double xPointA, double yPointA, double zPointA, 
																					double xPointB, double yPointB, double zPointB, 
																					double xPointC, double yPointC, double zPointC)
{
	LOG_TRACE("vtkStepperCalibrationController::AddPointsForPhantomToProbeDistanceCalculation"); 
	this->AddPointsForPhantomToProbeDistanceCalculation( 
		HomogenousVector4x1(xPointA, yPointA, zPointA), 
		HomogenousVector4x1(xPointB, yPointB, zPointB), 
		HomogenousVector4x1(xPointC, yPointC, zPointC) 
		); 
}

//----------------------------------------------------------------------------
int vtkStepperCalibrationController::CalculatePhantomToProbeDistance()
{
	LOG_TRACE("vtkStepperCalibrationController::CalculatePhantomToProbeDistance"); 
	// ==================================================================================
	// Compute the distance from the probe to phantom 
	// ==================================================================================
	// 1. This point-to-line distance holds the key to relate the position of the TRUS 
	//    rotation center to the precisely designed iCAL phantom geometry in Solid Edge CAD.
	// 2. Here we employ a straight-forward method based on vector theory as one of the 
	//    simplest and most efficient way to compute a point-line distance.
	//    FORMULA: D_O2AB = norm( cross(OA,OB) ) / norm(A-B)
	// ==================================================================================

	if ( !this->GetSpacingCalculated() )
	{
		LOG_WARNING("Unable to calculate phantom to probe distance without spacing calculated!"); 
		return 0; 
	}

	for ( int frame = 0; frame < this->SegmentedFrameContainer.size(); frame++ )
	{
		if ( this->SegmentedFrameContainer[frame].DataType == PROBE_ROTATION )
		{

			// Add Line #1 (point A) Line #3 (point B) and Line #6 (point C) pixel coordinates to phantom to probe distance point set 
			this->AddPointsForPhantomToProbeDistanceCalculation(
				this->SegmentedFrameContainer[frame].SegResults.m_FoundDotsCoordinateValue[0][0], this->SegmentedFrameContainer[frame].SegResults.m_FoundDotsCoordinateValue[0][1], 0, 
				this->SegmentedFrameContainer[frame].SegResults.m_FoundDotsCoordinateValue[2][0], this->SegmentedFrameContainer[frame].SegResults.m_FoundDotsCoordinateValue[2][1], 0, 
				this->SegmentedFrameContainer[frame].SegResults.m_FoundDotsCoordinateValue[5][0], this->SegmentedFrameContainer[frame].SegResults.m_FoundDotsCoordinateValue[5][1], 0 
				); 

		}
	}

	vnl_vector<double> rotationCenter3x1InMm(3,0);
	rotationCenter3x1InMm.put(0, this->GetCenterOfRotationPx()[0] * this->GetSpacing()[0]); 
	rotationCenter3x1InMm.put(1, this->GetCenterOfRotationPx()[1] * this->GetSpacing()[1]);
	rotationCenter3x1InMm.put(2, 0);

	// Total number images used for this computation
	const int totalNumberOfImages2ComputePtLnDist = this->PointSetForPhantomToProbeDistanceCalculation.size();

	// This will keep a trace on all the calculated distance
	vnl_vector<double> listOfPhantomToProbeVerticalDistanceInMm(totalNumberOfImages2ComputePtLnDist, 0 );
	vnl_vector<double> listOfPhantomToProbeHorizontalDistanceInMm(totalNumberOfImages2ComputePtLnDist, 0 );

	for (int i = 0; i < totalNumberOfImages2ComputePtLnDist; i++)
	{
		// Extract point A
		vnl_vector<double> pointAInMm(3,0);
		pointAInMm.put( 0, this->PointSetForPhantomToProbeDistanceCalculation[i][0].GetX() * this->GetSpacing()[0] );
		pointAInMm.put( 1, this->PointSetForPhantomToProbeDistanceCalculation[i][0].GetY() * this->GetSpacing()[1] );
		pointAInMm.put( 2, 0 ); 

		// Extract point B
		vnl_vector<double> pointBInMm(3,0);
		pointBInMm.put( 0, this->PointSetForPhantomToProbeDistanceCalculation[i][1].GetX() * this->GetSpacing()[0] );
		pointBInMm.put( 1, this->PointSetForPhantomToProbeDistanceCalculation[i][1].GetY() * this->GetSpacing()[1] );
		pointBInMm.put( 2, 0 ); 

		// Extract point C
		vnl_vector<double> pointCInMm(3,0);
		pointCInMm.put( 0, this->PointSetForPhantomToProbeDistanceCalculation[i][2].GetX() * this->GetSpacing()[0] );
		pointCInMm.put( 1, this->PointSetForPhantomToProbeDistanceCalculation[i][2].GetY() * this->GetSpacing()[1] );
		pointCInMm.put( 2, 0 ); 

		// Construct vectors among rotation center, point A, and point B.
		const vnl_vector<double> vectorRotationCenterToPointAInMm = pointAInMm - rotationCenter3x1InMm;
		const vnl_vector<double> vectorRotationCenterToPointBInMm = pointBInMm - rotationCenter3x1InMm;
		const vnl_vector<double> vectorRotationCenterToPointCInMm = pointCInMm - rotationCenter3x1InMm;
		const vnl_vector<double> vectorPointAToPointBInMm = pointBInMm - pointAInMm;
		const vnl_vector<double> vectorPointBToPointCInMm = pointCInMm - pointBInMm;

		// Compute the point-line distance from probe to the line passing through A and B points, based on the
		// standard vector theory. FORMULA: D_O2AB = norm( cross(OA,OB) ) / norm(A-B)
		const double thisPhantomToProbeVerticalDistanceInMm = vnl_cross_3d( vectorRotationCenterToPointAInMm, vectorRotationCenterToPointBInMm ).magnitude() / vectorPointAToPointBInMm.magnitude();

		// Compute the point-line distance from probe to the line passing through B and C points, based on the
		// standard vector theory. FORMULA: D_O2AB = norm( cross(OA,OB) ) / norm(A-B)
		const double thisPhantomToProbeHorizontalDistanceInMm = vnl_cross_3d( vectorRotationCenterToPointBInMm, vectorRotationCenterToPointCInMm ).magnitude() / vectorPointBToPointCInMm.magnitude();

		// Populate the data container
		listOfPhantomToProbeVerticalDistanceInMm.put(i, thisPhantomToProbeVerticalDistanceInMm );
		listOfPhantomToProbeHorizontalDistanceInMm.put(i, thisPhantomToProbeHorizontalDistanceInMm );
	}

	this->SetPhantomToProbeDistanceInMm( listOfPhantomToProbeHorizontalDistanceInMm.mean(), listOfPhantomToProbeVerticalDistanceInMm.mean() ); 

	return 1; 
}


//***************************************************************************
//								Clustering
//***************************************************************************


//----------------------------------------------------------------------------
double vtkStepperCalibrationController::GetClusterZPosition(const SegmentedFrameList &cluster)
{
	double meanZPosition(0); 
	const int numOfFrames = cluster.size(); 
	for ( int frame = 0; frame < numOfFrames; ++frame )
	{
		double probePos(0), probeRot(0), templatePos(0); 
		if ( !this->GetStepperEncoderValues(cluster[frame].TrackedFrameInfo, probePos, probeRot, templatePos) )
		{
			LOG_WARNING("GetClusterZPosition: Unable to get probe position from tracked frame info for frame #" << frame); 
			continue; 
		}
		meanZPosition += probePos / (1.0*numOfFrames); 
	}

	return meanZPosition; 
}

//----------------------------------------------------------------------------
void vtkStepperCalibrationController::ClusterSegmentedFrames(IMAGE_DATA_TYPE dataType,  std::vector<SegmentedFrameList> &clusterList)
{
	LOG_TRACE("vtkStepperCalibrationController::ClusterSegmentedFrames"); 

	SegmentedFrameList clusterData; 

	vtkSmartPointer<vtkPoints> clusterPoints = vtkSmartPointer<vtkPoints>::New(); 
	for ( int frame = 0; frame < this->SegmentedFrameContainer.size(); frame++ )
	{

		if ( this->SegmentedFrameContainer[frame].DataType == dataType )
		{
			double probePos(0), probeRot(0), templatePos(0); 
			if ( !this->GetStepperEncoderValues(this->SegmentedFrameContainer[frame].TrackedFrameInfo, probePos, probeRot, templatePos) )
			{
				LOG_WARNING("Clustering: Unable to get probe position from tracked frame info for frame #" << frame); 
				continue; 
			}

			LOG_DEBUG("Insert point to cluster list at probe position: " << probePos); 
			clusterData.push_back(this->SegmentedFrameContainer[frame]); 
			clusterPoints->InsertNextPoint(probePos, 0, 0); 
		}
	}

	vtkSmartPointer<vtkPolyData> data = vtkSmartPointer<vtkPolyData>::New();
	data->SetPoints(clusterPoints); 

	vtkSmartPointer<vtkMeanShiftClustering> meanShiftFilter = vtkSmartPointer<vtkMeanShiftClustering>::New();
	meanShiftFilter->SetInputConnection(data->GetProducerPort());
	meanShiftFilter->SetWindowRadius(5.0); //radius should be bigger than expected clusters
	meanShiftFilter->SetGaussianVariance(1.0);
	meanShiftFilter->Update();


	for(unsigned int r = 0; r < clusterData.size(); r++)
	{
		int clusterID = meanShiftFilter->GetPointAssociations(r); 
		if ( clusterID >= 0 )
		{
			while ( clusterList.size() < clusterID + 1 )
			{
				LOG_DEBUG("Create new segmented frame cluster for clusterID: " << clusterID); 
				SegmentedFrameList newCluster; 
				clusterList.push_back(newCluster); 
			}

			clusterList[clusterID].push_back(clusterData[r]); 
		}
	}

	LOG_DEBUG("Number of clusters: " << clusterList.size()); 
	for ( unsigned int i = 0; i < clusterList.size(); ++i)
	{
		LOG_DEBUG("Number of elements in cluster #" << i << ": " << clusterList[i].size()); 
	}

}

//----------------------------------------------------------------------------
bool vtkStepperCalibrationController::GetStepperEncoderValues( TrackedFrame* trackedFrame, double &probePosition, double &probeRotation, double &templatePosition)
{
	// Get the probe position from tracked frame info
	const char* cProbePos = trackedFrame->GetCustomFrameField("ProbePosition"); 
	if ( cProbePos != NULL )
	{
		probePosition = atof(cProbePos); 
	}
	else
	{
		double transform[16]; 
		if ( trackedFrame->GetDefaultFrameTransform(transform) )
		{
			// Get probe position from matrix (0,3) element
			probePosition = transform[3]; 
		}
		else
		{
			LOG_DEBUG("Unable to get probe position from tracked frame info."); 
			return false; 
		}
	}

	// Get the probe rotation from tracked frame info
	const char* cProbeRot = trackedFrame->GetCustomFrameField("ProbeRotation"); 
	if ( cProbeRot != NULL )
	{
		probeRotation = atof(cProbeRot); 
	}
	else
	{
		double transform[16]; 
		if ( trackedFrame->GetDefaultFrameTransform(transform) )
		{
			// Get probe rotation from matrix (1,3) element
			probeRotation = transform[7]; 
		}
		else
		{
			LOG_DEBUG("Unable to get probe rotation from tracked frame info."); 
			return false; 
		}
	}

	// Get the template position from tracked frame info
	const char* cTemplatePos = trackedFrame->GetCustomFrameField("TemplatePosition"); 
	if ( cTemplatePos != NULL )
	{
		templatePosition = atof(cTemplatePos); 
	}
	else
	{
		double transform[16]; 
		if ( trackedFrame->GetDefaultFrameTransform(transform) )
		{
			// Get template position from matrix (2,3) element
			templatePosition = transform[11]; 
		}
		else
		{
			LOG_DEBUG("Unable to get template position from tracked frame info."); 
			return false; 
		}
	}

	return true; 
}

//----------------------------------------------------------------------------
void vtkStepperCalibrationController::OfflineProbeRotationAxisCalibration()
{
	LOG_TRACE("vtkStepperCalibrationController::OfflineProbeRotationAxisCalibration"); 
	if ( ! this->GetInitialized() ) 
	{
		this->Initialize(); 
	}

	vtkSmartPointer<vtkTrackedFrameList> trackedFrameList = vtkSmartPointer<vtkTrackedFrameList>::New();
	if ( !this->GetSavedImageDataInfo(PROBE_ROTATION).SequenceMetaFileName.empty() )
	{
		trackedFrameList->ReadFromSequenceMetafile(this->GetSavedImageDataInfo(PROBE_ROTATION).SequenceMetaFileName.c_str()); 
	}
	else
	{
		LOG_ERROR("Unable to start OfflineProbeRotationAxisCalibration with probe rotation data: SequenceMetaFileName is empty!"); 
		return; 
	}

	int frameCounter(0); 
	int imgNumber(0); 
	for( imgNumber = this->GetSavedImageDataInfo(PROBE_ROTATION).StartingIndex; frameCounter < this->GetSavedImageDataInfo(PROBE_ROTATION).NumberOfImagesToUse; imgNumber++ )
	{
		if ( imgNumber >= trackedFrameList->GetNumberOfTrackedFrames() )
		{
			break; 
		}

		if ( this->AddTrackedFrameData(trackedFrameList->GetTrackedFrame(imgNumber), PROBE_ROTATION) )
		{
			// The segmentation was successful 
			frameCounter++; 
		}

		this->AddFrameToRenderer(trackedFrameList->GetTrackedFrame(imgNumber)->ImageData); 
	}

	LOG_INFO ( "A total of " << frameCounter << " images (" << 100*frameCounter/imgNumber << "%) have been successfully added for probe rotation axis calibration.");
	trackedFrameList->Clear(); 

	if ( !this->CalibrateProbeRotationAxis() )
	{
		LOG_ERROR("Unable to calibrate probe rotation axis!"); 
	}
}

//----------------------------------------------------------------------------
void vtkStepperCalibrationController::OfflineProbeTranslationAxisCalibration()
{
	LOG_TRACE("vtkStepperCalibrationController::OfflineProbeTranslationAxisCalibration"); 
	if ( ! this->GetInitialized() ) 
	{
		this->Initialize(); 
	}

	vtkSmartPointer<vtkTrackedFrameList> trackedFrameList = vtkSmartPointer<vtkTrackedFrameList>::New();
	if ( !this->GetSavedImageDataInfo(PROBE_TRANSLATION).SequenceMetaFileName.empty() )
	{
		trackedFrameList->ReadFromSequenceMetafile(this->GetSavedImageDataInfo(PROBE_TRANSLATION).SequenceMetaFileName.c_str()); 
	}
	else
	{
		LOG_ERROR("Unable to start OfflineProbeTranslationAxisCalibration with probe translation data: SequenceMetaFileName is empty!"); 
		return; 
	}

	int frameCounter(0); 
	int imgNumber(0); 
	for( imgNumber = this->GetSavedImageDataInfo(PROBE_TRANSLATION).StartingIndex; frameCounter < this->GetSavedImageDataInfo(PROBE_TRANSLATION).NumberOfImagesToUse; imgNumber++ )
	{
		if ( imgNumber >= trackedFrameList->GetNumberOfTrackedFrames() )
		{
			break; 
		}

		if ( this->AddTrackedFrameData(trackedFrameList->GetTrackedFrame(imgNumber), PROBE_TRANSLATION) )
		{
			// The segmentation was successful 
			frameCounter++; 
		}

		this->AddFrameToRenderer(trackedFrameList->GetTrackedFrame(imgNumber)->ImageData); 
	}

	LOG_INFO ( "A total of " << frameCounter << " images (" << 100*frameCounter/imgNumber << "%) have been successfully added for probe translation axis calibration.");

	trackedFrameList->Clear(); 
	if ( !this->CalibrateProbeTranslationAxis() )
	{
		LOG_ERROR("Unable to calibrate probe translation axis!"); 
	}
}

//----------------------------------------------------------------------------
void vtkStepperCalibrationController::OfflineTemplateTranslationAxisCalibration()
{
	LOG_TRACE("vtkStepperCalibrationController::OfflineTemplateTranslationAxisCalibration"); 
	if ( ! this->GetInitialized() ) 
	{
		this->Initialize(); 
	}

	vtkSmartPointer<vtkTrackedFrameList> trackedFrameList = vtkSmartPointer<vtkTrackedFrameList>::New();
	if ( !this->GetSavedImageDataInfo(TEMPLATE_TRANSLATION).SequenceMetaFileName.empty() )
	{
		trackedFrameList->ReadFromSequenceMetafile(this->GetSavedImageDataInfo(TEMPLATE_TRANSLATION).SequenceMetaFileName.c_str()); 
	}
	else
	{
		LOG_ERROR("Unable to start OfflineTemplateTranslationAxisCalibration with template translation data: SequenceMetaFileName is empty!"); 
		return; 
	}

	int frameCounter(0); 
	int imgNumber(0); 
	for( imgNumber = this->GetSavedImageDataInfo(TEMPLATE_TRANSLATION).StartingIndex; frameCounter < this->GetSavedImageDataInfo(TEMPLATE_TRANSLATION).NumberOfImagesToUse; imgNumber++ )
	{
		if ( imgNumber >= trackedFrameList->GetNumberOfTrackedFrames() )
		{
			break; 
		}

		if ( this->AddTrackedFrameData(trackedFrameList->GetTrackedFrame(imgNumber), TEMPLATE_TRANSLATION) )
		{
			// The segmentation was successful 
			frameCounter++; 
		}

		this->AddFrameToRenderer(trackedFrameList->GetTrackedFrame(imgNumber)->ImageData); 
	}

	LOG_INFO ( "A total of " << frameCounter << " images (" << 100*frameCounter/imgNumber << "%) have been successfully added for template translation axis calibration.");

	trackedFrameList->Clear(); 
	if ( !this->CalibrateTemplateTranslationAxis() )
	{
		LOG_ERROR("Unable to calibrate template translation axis!"); 
	}
}

//----------------------------------------------------------------------------
void vtkStepperCalibrationController::SaveCalibrationStartTime()
{

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
	std::string starttime = DateInString + "_" + TimeInString; 
	this->SetCalibrationStartTime(starttime.c_str()); 
	LOG_TRACE("vtkStepperCalibrationController::SaveCalibrationStartTime: " << starttime); 
}

//----------------------------------------------------------------------------
void vtkStepperCalibrationController::ReadConfiguration( const char* configFileNameWithPath )
{
	LOG_TRACE("vtkStepperCalibrationController::ReadConfiguration - " << configFileNameWithPath); 
	this->SetConfigurationFileName(configFileNameWithPath); 

	vtkXMLDataElement *calibrationController = vtkXMLUtilities::ReadElementFromFile(this->GetConfigurationFileName()); 
	this->ReadConfiguration(calibrationController); 
	calibrationController->Delete(); 
}

//----------------------------------------------------------------------------
void vtkStepperCalibrationController::ReadConfiguration( vtkXMLDataElement* configData )
{
	LOG_TRACE("vtkStepperCalibrationController::ReadConfiguration"); 
	if ( configData == NULL )
	{
		LOG_ERROR("Unable to read the main configration file"); 
		exit(EXIT_FAILURE); 
	}

	// Calibration controller specifications
	//********************************************************************
	vtkSmartPointer<vtkXMLDataElement> calibrationController = configData->FindNestedElementWithName("CalibrationController"); 
	this->ReadCalibrationControllerConfiguration(calibrationController); 

	// StepperCalibration specifications
	//*********************************
	vtkSmartPointer<vtkXMLDataElement> stepperCalibration = calibrationController->FindNestedElementWithName("StepperCalibration"); 
	this->ReadStepperCalibrationConfiguration(stepperCalibration); 
}

//----------------------------------------------------------------------------
void vtkStepperCalibrationController::ReadStepperCalibrationConfiguration(vtkXMLDataElement* stepperCalibration)
{
	LOG_TRACE("vtkStepperCalibrationController::ReadStepperCalibrationConfiguration"); 
	if ( stepperCalibration == NULL) 
	{	
		LOG_WARNING("Unable to read StepperCalibration XML data element!"); 
		return; 
	} 

	int minNumberOfRotationClusters = 0;
	if ( stepperCalibration->GetScalarAttribute("MinNumberOfRotationClusters", minNumberOfRotationClusters) ) 
	{
		this->MinNumberOfRotationClusters = minNumberOfRotationClusters; 
	}

	// ProbeRotationData data set specifications
	//********************************************************************
	vtkSmartPointer<vtkXMLDataElement> probeRotationData = stepperCalibration->FindNestedElementWithName("ProbeRotationData"); 
	if ( probeRotationData != NULL) 
	{
		vtkCalibrationController::SavedImageDataInfo imageDataInfo; 
		int numberOfImagesToUse = -1;
		if ( probeRotationData->GetScalarAttribute("NumberOfImagesToUse", numberOfImagesToUse) ) 
		{
			imageDataInfo.NumberOfImagesToUse = numberOfImagesToUse; 
		}

		int startingIndex = 0;
		if ( probeRotationData->GetScalarAttribute("StartingIndex", startingIndex) ) 
		{
			imageDataInfo.StartingIndex = startingIndex; 
		}

		// Path to center of rotation input sequence metafile
		const char* sequenceMetaFile = probeRotationData->GetAttribute("SequenceMetaFile"); 
		if ( sequenceMetaFile != NULL) 
		{
			imageDataInfo.SequenceMetaFileName.assign(sequenceMetaFile); 
		}

		this->SetSavedImageDataInfo(PROBE_ROTATION, imageDataInfo); 
	}
	else
	{
		LOG_WARNING("Unable to find ProbeRotationData XML data element"); 
	}


	// TemplateTranslationData data set specifications
	//********************************************************************
	vtkSmartPointer<vtkXMLDataElement> templateTranslationData = stepperCalibration->FindNestedElementWithName("TemplateTranslationData"); 
	if ( templateTranslationData != NULL) 
	{
		vtkCalibrationController::SavedImageDataInfo imageDataInfo; 
		int numberOfImagesToUse = -1;
		if ( templateTranslationData->GetScalarAttribute("NumberOfImagesToUse", numberOfImagesToUse) ) 
		{
			imageDataInfo.NumberOfImagesToUse = numberOfImagesToUse; 
		}

		int startingIndex = 0;
		if ( templateTranslationData->GetScalarAttribute("StartingIndex", startingIndex) ) 
		{
			imageDataInfo.StartingIndex = startingIndex; 
		}

		// Path to center of rotation input sequence metafile
		const char* sequenceMetaFile = templateTranslationData->GetAttribute("SequenceMetaFile"); 
		if ( sequenceMetaFile != NULL) 
		{
			imageDataInfo.SequenceMetaFileName.assign(sequenceMetaFile); 
		}

		this->SetSavedImageDataInfo(TEMPLATE_TRANSLATION, imageDataInfo); 
	}
	else
	{
		LOG_WARNING("Unable to find TemplateTranslationData XML data element"); 
	}

	// ProbeTranslationData data set specifications
	//********************************************************************
	vtkSmartPointer<vtkXMLDataElement> probeTranslationData = stepperCalibration->FindNestedElementWithName("ProbeTranslationData"); 
	if ( probeTranslationData != NULL) 
	{
		vtkCalibrationController::SavedImageDataInfo imageDataInfo; 
		int numberOfImagesToUse = -1;
		if ( probeTranslationData->GetScalarAttribute("NumberOfImagesToUse", numberOfImagesToUse) ) 
		{
			imageDataInfo.NumberOfImagesToUse = numberOfImagesToUse; 
		}

		int startingIndex = 0;
		if ( probeTranslationData->GetScalarAttribute("StartingIndex", startingIndex) ) 
		{
			imageDataInfo.StartingIndex = startingIndex; 
		}

		// Path to center of rotation input sequence metafile
		const char* sequenceMetaFile = probeTranslationData->GetAttribute("SequenceMetaFile"); 
		if ( sequenceMetaFile != NULL) 
		{
			imageDataInfo.SequenceMetaFileName.assign(sequenceMetaFile); 
		}

		this->SetSavedImageDataInfo(PROBE_TRANSLATION, imageDataInfo); 
	}
	else
	{
		LOG_WARNING("Unable to find ProbeTranslationData XML data element"); 
	}
}

//----------------------------------------------------------------------------
void vtkStepperCalibrationController::WriteCalibrationResultToXml(vtkXMLDataElement* config)
{
	LOG_TRACE("vtkStepperCalibrationController::WriteCalibrationResultToXml"); 
	if ( config == NULL )
	{
		LOG_ERROR("Unable to write calibration result: xml data element is NULL!"); 
	}

	if ( !this->GetProbeRotationAxisCalibrated() )
	{
		LOG_WARNING("You have to calibrate probe rotation axis before saving calibration result!"); 
		return; 
	}

	if ( !this->GetProbeTranslationAxisCalibrated() )
	{
		LOG_WARNING("You have to calibrate probe translation axis before saving calibration result!"); 
		return; 
	}

	if ( !this->GetTemplateTranslationAxisCalibrated() )
	{
		LOG_WARNING("You have to calibrate template translation axis before saving calibration result!"); 
		return; 
	}

	vtkSmartPointer<vtkXMLDataElement> calibration = config->FindNestedElementWithName("Calibration"); 

	if ( calibration == NULL )
	{
		calibration = vtkSmartPointer<vtkXMLDataElement>::New(); 
		calibration->SetName("Calibration");
	}
	else
	{
		config->RemoveNestedElement(calibration); 
	}

	calibration->SetAttribute("AlgorithmVersion", "1.0.0"); 

	calibration->SetAttribute("Date", this->CalibrationStartTime); 

	calibration->SetVectorAttribute("ProbeTranslationAxisOrientation", 3, this->GetProbeTranslationAxisOrientation() ); 

	calibration->SetVectorAttribute("TemplateTranslationAxisOrientation", 3, this->GetTemplateTranslationAxisOrientation() ); 

	calibration->SetVectorAttribute("ProbeRotationAxisOrientation", 3, this->GetProbeRotationAxisOrientation() );		

	calibration->SetDoubleAttribute("ProbeRotationEncoderScale", this->GetProbeRotationEncoderScale() ); 

	config->AddNestedElement(calibration); 

}

