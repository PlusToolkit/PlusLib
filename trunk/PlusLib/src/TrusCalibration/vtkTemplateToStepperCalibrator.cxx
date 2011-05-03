#include "PlusConfigure.h"
#include "vtkTemplateToStepperCalibrator.h" 

// VTK includes
#include "vtkObjectFactory.h"
#include "vtkSmartPointer.h"
#include "vtkTransform.h"

// VNL Includes
#include "vnl/algo/vnl_matrix_inverse.h"
#include "vnl/vnl_sparse_matrix.h"   // for least squares
#include "vnl/vnl_sparse_matrix_linear_system.h"  // for least squares
#include "vnl/algo/vnl_lsqr.h"  // for least squares
#include "vnl/vnl_cross.h"  // for vnl_cross_3d


//----------------------------------------------------------------------------

vtkCxxRevisionMacro(vtkTemplateToStepperCalibrator, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkTemplateToStepperCalibrator); 


//----------------------------------------------------------------------------
vtkTemplateToStepperCalibrator::vtkTemplateToStepperCalibrator()
{
	this->SetCenterOfRotation(0,0); 
	this->SetSpacing(0,0); 
	this->SetPhantomToProbeDistanceInMm(0,0); 
	this->TemplateToStepperCalibrationDoneOff();
	
	this->Calibrator = NULL; 

	this->SetMinNumberOfPointsForCalibration(200); 

	// Set the TemplateToStepper matrix to identity
	this->TemplateToStepperTransformMatrix = NULL;
	vtkSmartPointer<vtkMatrix4x4> template2Stepper = vtkSmartPointer<vtkMatrix4x4>::New();
	this->SetTemplateToStepperTransformMatrix(template2Stepper);

	// Set the StepperToTemplate matrix to identity
	this->StepperToTemplateTransformMatrix = NULL;
	vtkSmartPointer<vtkMatrix4x4> stepper2Template = vtkSmartPointer<vtkMatrix4x4>::New();
	this->SetStepperToTemplateTransformMatrix(stepper2Template);

	// Set the TemplateHolderToTemplateTransformMatrix matrix to identity
	this->TemplateHolderToTemplateTransformMatrix = NULL;
	vtkSmartPointer<vtkMatrix4x4> templateHolder2Template = vtkSmartPointer<vtkMatrix4x4>::New();
	this->SetTemplateHolderToTemplateTransformMatrix(templateHolder2Template);

	// Set the StepperToTemplateHolder matrix to identity
	this->StepperToTemplateHolderTransformMatrix = NULL;
	vtkSmartPointer<vtkMatrix4x4> stepper2TemplateHolder = vtkSmartPointer<vtkMatrix4x4>::New();
	this->SetStepperToTemplateHolderTransformMatrix(stepper2TemplateHolder); 
}


//----------------------------------------------------------------------------
vtkTemplateToStepperCalibrator::~vtkTemplateToStepperCalibrator()
{
	this->SetTemplateToStepperTransformMatrix(NULL);
	this->SetStepperToTemplateTransformMatrix(NULL); 
	this->SetTemplateHolderToTemplateTransformMatrix(NULL); 
	this->SetStepperToTemplateHolderTransformMatrix(NULL); 
	this->SetCalibrator(NULL); 
}

//----------------------------------------------------------------------------
void vtkTemplateToStepperCalibrator::PrintSelf(std::ostream &os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
void vtkTemplateToStepperCalibrator::AddPointForCenterOfRotationCalculation( std::vector<HomogenousVector4x1> vector )
{
	this->PointSetForCenterOfRotationCalculation.push_back(vector); 
}


//----------------------------------------------------------------------------
void vtkTemplateToStepperCalibrator::CalculateCenterOfRotation()
{
	// ====================================================================
	// Compute the TRUS rotation center using linear least squares
	// ====================================================================
	// Note: the number of the distant pairs is a combination (C2)
	// of the number of the segmented N-Fiducial_1 positions in TRUS.
	// N in LaTeX = {NumDataPoints}^C_{2}
	// E.g.: for 200 positions as used for thresholding here, N = 19,900.
	// ====================================================================

	if ( this->PointSetForCenterOfRotationCalculation.size() < MinNumberOfPointsForCalibration )
	{
		vtkWarningMacro("Center of rotation calculation: There is not enough data points for Template to Stepper Calibration!"); 
		return; 
	}

	// Data containers
	std::vector<vnl_vector_double> ListOfDistanceNx2;
	ListOfDistanceNx2.resize(0);
	std::vector<double> SquaredDistanceVectorNx1;
	SquaredDistanceVectorNx1.resize(0);

	for( unsigned int i = 0; i <= this->PointSetForCenterOfRotationCalculation.size() - 2; i++ )
	{
		for( unsigned int j = i + 1; j <= this->PointSetForCenterOfRotationCalculation.size() - 1; j++ )
		{
			for ( int point = 0; point < PointSetForCenterOfRotationCalculation[i].size(); point++ )
			{
				// coordiates of the i-th element
				double Xi = PointSetForCenterOfRotationCalculation[i][point].GetX() * this->GetSpacing()[0]; 
				double Yi = PointSetForCenterOfRotationCalculation[i][point].GetY() * this->GetSpacing()[1]; 

				// coordiates of the j-th element
				double Xj = PointSetForCenterOfRotationCalculation[j][point].GetX() * this->GetSpacing()[0]; 
				double Yj = PointSetForCenterOfRotationCalculation[j][point].GetY() * this->GetSpacing()[1]; 

				// Populate the list of distance
				vnl_vector<double> RowOfDistance(2,0);
				RowOfDistance.put(0, Xi - Xj);
				RowOfDistance.put(1, Yi - Yj);
				ListOfDistanceNx2.push_back( RowOfDistance );

				// Populate the squared distance vector
				SquaredDistanceVectorNx1.push_back( 0.5*( Xi*Xi + Yi*Yi - Xj*Xj - Yj*Yj ));
			}
		}
	}

	// Construct the sparse matrix (left side of the equation) for least-squares optimizer
	// Note: vnl_sparse_matrix is required by VNL LSQR class
	const int numberOfInputData( SquaredDistanceVectorNx1.size() );
	vnl_sparse_matrix<double> SparseMatrixLeftSide2Equation(numberOfInputData, 2);
	// Construct the vector (right side of the equation) for least-squares optimizer
	vnl_vector<double> VectorRightSide2Equation(numberOfInputData, 0);

	for(int i = 0; i < numberOfInputData; i++)
	{
		// Populate the sparse matrix
		SparseMatrixLeftSide2Equation(i,0) = ListOfDistanceNx2.at(i).get(0);
		SparseMatrixLeftSide2Equation(i,1) = ListOfDistanceNx2.at(i).get(1);

		// Populate the vector
		VectorRightSide2Equation.put(i, SquaredDistanceVectorNx1.at(i));
	}

	// Feed the data to the least-squares optimizer

	// Construct linear system defined in VNL
	vnl_sparse_matrix_linear_system<double> LinearSystem(
		SparseMatrixLeftSide2Equation, VectorRightSide2Equation );
	// Find solution using LSQR optimization
	vnl_lsqr lsqr( LinearSystem );
	// The TRUS rotation center in original image frame
	// The same as the segmentation coordinating system
	// - Origin: Left-upper corner;
	// - Positive X: to the right;
	// - Positive Y: to the bottom;
	// - Units in pixels.
	vnl_vector<double> TRUSRotationCenterInOriginalImageFrameInMm2x1(2,0);
	lsqr.minimize( TRUSRotationCenterInOriginalImageFrameInMm2x1 );

	this->SetCenterOfRotation(TRUSRotationCenterInOriginalImageFrameInMm2x1.get(0) / this->GetSpacing()[0], TRUSRotationCenterInOriginalImageFrameInMm2x1.get(1) / this->GetSpacing()[1]); 
}


//----------------------------------------------------------------------------
void vtkTemplateToStepperCalibrator::AddDistancesForSpacingCalculation(Distance distAB)
{
	this->PointSetForSpacingCalculation.push_back(distAB);
}


//----------------------------------------------------------------------------
void vtkTemplateToStepperCalibrator::AddDistancesForSpacingCalculation(double xDistInPx, double yDistInPx, double distInMm)
{
	this->AddDistancesForSpacingCalculation(Distance(xDistInPx, yDistInPx, distInMm)); 
}


//----------------------------------------------------------------------------
void vtkTemplateToStepperCalibrator::CalculateSpacing()
{
	// =============================================================
	// Compute the scale factors using linear least squares
	// =============================================================
	// NOTE: in theory, one can use just two points to calculate the
	// scale factors in X (lateral) and Y (axial), however, the result
	// will not accurate as the data contains noise and errors in it.
	// So we built up an over-determined system to use all possible
	// input TRUS images to solve for the scale factors using a linear
	// least-squares optimizer, which would yield the most accurate 
	// possible given sufficient number of input data.
	// =============================================================

	if ( this->PointSetForSpacingCalculation.size() < MinNumberOfPointsForCalibration )
	{
		vtkWarningMacro("Spacing calculation: There is not enough data points for Template to Stepper Calibration!"); 
		return; 
	}

	// Construct the sparse matrix (left side of the equation) for least-squares optimizer
	// Note: vnl_sparse_matrix is required by VNL LSQR class
	const int numberOfInputData4ScaleFactors( this->PointSetForSpacingCalculation.size() );

	vnl_sparse_matrix<double> sparseMatrixLeftSide2Equation4ScaleFactors(numberOfInputData4ScaleFactors, 2);

	// Construct the vector (right side of the equation) for least-squares optimizer
	vnl_vector<double> vectorRightSide2Equation4ScaleFactors(numberOfInputData4ScaleFactors, 0);

	for(int i = 0; i < numberOfInputData4ScaleFactors; i++)
	{
		// Populate the sparse matrix with squared distances in pixel 
		sparseMatrixLeftSide2Equation4ScaleFactors(i,0) = pow(this->PointSetForSpacingCalculation[i].GetXDistanceInPx(), 2);
		sparseMatrixLeftSide2Equation4ScaleFactors(i,1) = pow(this->PointSetForSpacingCalculation[i].GetYDistanceInPx(), 2);

		// Populate the vector with squared distances in mm 
		vectorRightSide2Equation4ScaleFactors.put(i, pow(this->PointSetForSpacingCalculation[i].GetDistanceInMm(), 2));
	}

	// Feed the data to the least-squares optimizer

	// Construct linear system defined in VNL
	vnl_sparse_matrix_linear_system<double> LinearSystem4ScaleFactors(
		sparseMatrixLeftSide2Equation4ScaleFactors, vectorRightSide2Equation4ScaleFactors );
	// Find solution using LSQR optimization
	vnl_lsqr lsqr4ScaleFactor( LinearSystem4ScaleFactors );
	// The TRUS Scale factors
	// - Sx: lateral axis;
	// - Sy: axial axis;
	// - Units in mm/pixel.
	vnl_vector<double> TRUSSquaredScaleFactorsInMMperPixel2x1(2,0);
	lsqr4ScaleFactor.minimize( TRUSSquaredScaleFactorsInMMperPixel2x1 );

	this->SetSpacing( sqrt(TRUSSquaredScaleFactorsInMMperPixel2x1.get(0)), sqrt(TRUSSquaredScaleFactorsInMMperPixel2x1.get(1)) ); 

}

//----------------------------------------------------------------------------
void vtkTemplateToStepperCalibrator::AddPointsForPhantomToProbeDistanceCalculation(HomogenousVector4x1 pointA, HomogenousVector4x1 pointB, HomogenousVector4x1 pointC)
{
	std::vector<HomogenousVector4x1> vectorOfPointCoordinates; 
	vectorOfPointCoordinates.push_back(pointA); 
	vectorOfPointCoordinates.push_back(pointB); 
	vectorOfPointCoordinates.push_back(pointC); 
	this->PointSetForPhantomToProbeDistanceCalculation.push_back(vectorOfPointCoordinates); 
}


//----------------------------------------------------------------------------
void vtkTemplateToStepperCalibrator::AddPointsForPhantomToProbeDistanceCalculation(double xPointA, double yPointA, double zPointA, 
																				   double xPointB, double yPointB, double zPointB, 
																				   double xPointC, double yPointC, double zPointC)
{
	this->AddPointsForPhantomToProbeDistanceCalculation( 
		HomogenousVector4x1(xPointA, yPointA, zPointA), 
		HomogenousVector4x1(xPointB, yPointB, zPointB), 
		HomogenousVector4x1(xPointC, yPointC, zPointC) 
		); 
}

//----------------------------------------------------------------------------
void vtkTemplateToStepperCalibrator::CalculatePhantomToProbeDistance()
{
	// ==================================================================================
	// Compute the distance from the probe to phantom 
	// ==================================================================================
	// 1. This point-to-line distance holds the key to relate the position of the TRUS 
	//    rotation center to the precisely designed iCAL phantom geometry in Solid Edge CAD.
	// 2. Here we employ a straight-forward method based on vector theory as one of the 
	//    simplest and most efficient way to compute a point-line distance.
	//    FORMULA: D_O2AB = norm( cross(OA,OB) ) / norm(A-B)
	// ==================================================================================

	if ( this->PointSetForPhantomToProbeDistanceCalculation.size() < MinNumberOfPointsForCalibration )
	{
		// The template to stepper calibration was NOT successful 
		this->TemplateToStepperCalibrationDoneOff();
		vtkWarningMacro("Phantom to probe distance calculation: There is not enough data points for Template to Stepper Calibration!"); 
		return; 
	}

	vnl_vector<double> rotationCenter3x1InMm(3,0);
	rotationCenter3x1InMm.put(0, this->GetCenterOfRotation()[0] * this->GetSpacing()[0]); 
	rotationCenter3x1InMm.put(1, this->GetCenterOfRotation()[1] * this->GetSpacing()[1]);
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

	// The template to stepper calibration was successful 
	this->TemplateToStepperCalibrationDoneOn();

}


//----------------------------------------------------------------------------
void vtkTemplateToStepperCalibrator::CalculateTemplateToStepperTransformation(double templateOriginX /*= -1*/, double templateOriginY /*= -1*/, double templateOriginZ /*= -1*/)
{
	if ( this->Calibrator == NULL ) 
	{
		vtkErrorMacro("Unable to calculate template to stepper transformation. Set calibrator first!"); 
		return; 
	}

	if ( templateOriginX == -1 && templateOriginY == -1 && templateOriginZ == -1) 
	{
		// use default distances from iCAL phantom CAD model template 
		templateOriginX = this->Calibrator->GetPhantomPoints().TemplateHolderPosition.x; 
		templateOriginY = this->Calibrator->GetPhantomPoints().TemplateHolderPosition.y; 
		templateOriginZ = this->Calibrator->GetPhantomPoints().TemplateHolderPosition.z; 
	}

	vtkSmartPointer<vtkTransform> templateHolderToTemplateTransform = vtkSmartPointer<vtkTransform>::New();
	templateHolderToTemplateTransform->Translate( templateOriginX, templateOriginY, templateOriginZ);

	this->CalculateTemplateToStepperTransformation(templateHolderToTemplateTransform); 
}

//----------------------------------------------------------------------------
void vtkTemplateToStepperCalibrator::CalculateTemplateToStepperTransformation(vtkTransform* templateHolderToTemplateTransform)
{
	if ( this->Calibrator == NULL ) 
	{
		vtkErrorMacro("Unable to calculate template to stepper transformation. Set calibrator first!"); 
		return; 
	}

	// Vertical distance from the template mounter hole center
	// to the TRUS Rotation Center
	double verticalDistanceTemplateMounterHoleToTRUSRotationCenterInMM = 
			this->Calibrator->GetPhantomPoints().WirePositionFrontWall[0].y
			+ this->GetPhantomToProbeDistanceInMm()[1]
			- this->Calibrator->GetPhantomPoints().TemplateHolderPosition.y; 

	// Horizontal distance from the template mounter hole center
	// to the TRUS Rotation Center
	double horizontalDistanceTemplateMounterHoleToTRUSRotationCenterInMM = 
			this->Calibrator->GetPhantomPoints().WirePositionFrontWall[2].x
			+ this->GetPhantomToProbeDistanceInMm()[0]
			- this->Calibrator->GetPhantomPoints().TemplateHolderPosition.x; 

	double templateOriginX = templateHolderToTemplateTransform->GetPosition()[0]; 
	double templateOriginY = templateHolderToTemplateTransform->GetPosition()[1]; 

	double horizontalTemplateToStepper = horizontalDistanceTemplateMounterHoleToTRUSRotationCenterInMM +  templateOriginX;
	double verticalTemplateToStepper = verticalDistanceTemplateMounterHoleToTRUSRotationCenterInMM +  templateOriginY;

	vtkSmartPointer<vtkTransform> templateToStepperTransform = vtkSmartPointer<vtkTransform>::New();
	templateToStepperTransform->Translate( horizontalTemplateToStepper, verticalTemplateToStepper, 0);

	vtkSmartPointer<vtkTransform> stepperToTemplateTransform = vtkSmartPointer<vtkTransform>::New();
	stepperToTemplateTransform->Translate( -horizontalTemplateToStepper, -verticalTemplateToStepper, 0);

	vtkSmartPointer<vtkTransform> stepperToTemplateHolderTransform = vtkSmartPointer<vtkTransform>::New();
	stepperToTemplateHolderTransform->Translate( horizontalDistanceTemplateMounterHoleToTRUSRotationCenterInMM, verticalDistanceTemplateMounterHoleToTRUSRotationCenterInMM, 0);
	
	this->SetTemplateToStepperTransformMatrix( templateToStepperTransform->GetMatrix() );
	this->SetStepperToTemplateTransformMatrix( stepperToTemplateTransform->GetMatrix() ); 
	this->SetTemplateHolderToTemplateTransformMatrix( templateHolderToTemplateTransform->GetMatrix() ); 
	this->SetStepperToTemplateHolderTransformMatrix( stepperToTemplateHolderTransform->GetMatrix() ); 
}