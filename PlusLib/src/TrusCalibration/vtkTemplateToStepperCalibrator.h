#ifndef __VTKTEMPLATETOSTEPPERCALIBRATOR_H
#define __VTKTEMPLATETOSTEPPERCALIBRATOR_H

#include "vtkAlgorithm.h"
#include <vtkstd/string>
#include <vtkstd/vector>
#include "vtkMatrix4x4.h"
#include "vtkTransform.h"
#include "BrachyTRUSCalibrator.h"

// VXL/VNL Includes
#include "vnl/vnl_matrix.h"
#include "vnl/vnl_vector.h"

class HomogenousVector4x1
{
public: 
	HomogenousVector4x1() { Vector[0] = Vector[1] = Vector[2] = 0; Vector[3] = 1; }; 
	HomogenousVector4x1( double x, double y, double z) 
	{ 
		Vector[0] = x; 
		Vector[1] = y;
		Vector[2] = z;
		Vector[3] = 1;
	
	}; 
	virtual double GetX() { return Vector[0]; }; 
	virtual double GetY() { return Vector[1]; }; 
	virtual double GetZ() { return Vector[2]; }; 

	virtual void SetX( double x ) { Vector[0] = x; }; 
	virtual void SetY( double y ) { Vector[1] = y; }; 
	virtual void SetZ( double z ) { Vector[2] = z; }; 

	virtual double* GetVector() { return Vector; }; 
	virtual void GetVector( double* outVector ) 
	{ 
		outVector[0] = Vector[0]; 
		outVector[1] = Vector[1]; 
		outVector[2] = Vector[2]; 
		outVector[3] = Vector[3]; 
	}; 

protected: 
	double Vector[4]; 
}; 

class Distance
{
public: 
	Distance() { XDistInPx = YDistInPx = DistInMm = 0; }; 
	Distance( double xDistInPx, double yDistInPx, double distInMm)
	{
		XDistInPx = xDistInPx; 
		YDistInPx = yDistInPx; 

		DistInMm = distInMm; 
	}

	virtual double GetXDistanceInPx() { return XDistInPx; }; 
	virtual double GetYDistanceInPx() { return YDistInPx; }; 
	virtual double GetDistanceInMm() { return DistInMm; }; 

protected: 
	double XDistInPx; 
	double YDistInPx; 
	double DistInMm; 
}; 

class vtkTemplateToStepperCalibrator : public vtkAlgorithm
{
public:
	//! VNL vector in double format
	typedef vnl_vector<double> vnl_vector_double;

	static vtkTemplateToStepperCalibrator *New();
	vtkTypeRevisionMacro(vtkTemplateToStepperCalibrator, vtkAlgorithm);
	virtual void PrintSelf(ostream& os, vtkIndent indent); 

	//! Description: 
	// Add vector of points to the point set for the rotation center computing
	virtual void AddPointForCenterOfRotationCalculation(std::vector<HomogenousVector4x1> vector); 

	//! Description: 
	// Compute rotation center using linear least squares
	// The algorithm uses the previously uploaded PointSetForCenterOfRotationCalculation
	// dataset with the different pixel positions of the moving object
	virtual void CalculateCenterOfRotation(); 

	//! Description: 
	// Add distances to the point set for the spacing calculation
	virtual void AddDistancesForSpacingCalculation(Distance distAB); 
	virtual void AddDistancesForSpacingCalculation(double xDistInPx, double yDistInPx, double distInMm);

	//! Description: 
	// Compute spacing using linear least squares
	// This computation needs a set of point distances between two well known 
	// object on the image in X and Y direction (in px and mm as well) to define the spacing. 
	virtual void CalculateSpacing(); 

	//! Description: 
	// Add points to the point set for calculating the distance between the 
	// phantom and TRUS probe
	// Add Line #1 (point A) Line #3 (point B) and Line #6 (point C) pixel coordinates
	virtual void AddPointsForPhantomToProbeDistanceCalculation(HomogenousVector4x1 pointA, HomogenousVector4x1 pointB, HomogenousVector4x1 pointC); 
	virtual void AddPointsForPhantomToProbeDistanceCalculation(
		double xPointA, double yPointA, double zPointA, 
		double xPointB, double yPointB, double zPointB, 
		double xPointC, double yPointC, double zPointC );

	//! Description:
	// Calculate the distance between the probe and phantom 
	virtual void CalculatePhantomToProbeDistance(); 

	//! Description:
	// Calculate the transformation between template origin and the stepper
	// This transformation depends on the template origin in mm relative to 
	// the stepper mounter hole center (left)
	virtual void CalculateTemplateToStepperTransformation(double templateOriginX = -1, double templateOriginY = -1, double templateOriginZ = -1); 
	virtual void CalculateTemplateToStepperTransformation(vtkTransform* templateHolderToTemplateTransform); 

	//! Description: 
	// Set brachy TRUS calibrator to get phantom known point in template coordinate system 
	void SetCalibrator( BrachyTRUSCalibrator* calibrator ) { this->Calibrator = calibrator; };

	//! Description: 
	// Set/Get the rotation center in pixels.
	// Origin: Left-upper corner (the original image frame)
	// Positive X: to the right;
	// Positive Y: to the bottom;
	vtkSetVector2Macro(CenterOfRotation, int); 
	vtkGetVector2Macro(CenterOfRotation, int); 

	//! Description: 
	// Set/Get the image spacing.
	// (x: lateral axis, y: axial axis)
	vtkSetVector2Macro(Spacing, double); 
	vtkGetVector2Macro(Spacing, double); 

	//! Description: 
	// Set/Get the minimum number of points needed for the center of rotation 
	// computing algorithm
	vtkSetMacro(MinNumberOfPointsForCalibration, int); 
	vtkGetMacro(MinNumberOfPointsForCalibration, int); 
	

	//! Description: 
	// Set/Get the distance between the phantom and probe
	// Horizontal [0] and vertical [1] distance in mm
	vtkSetVector2Macro(PhantomToProbeDistanceInMm, double); 
	vtkGetVector2Macro(PhantomToProbeDistanceInMm, double); 
	
	//! Description: 
	// Set/Get the flag of the template to stepper calibration success
	vtkGetMacro(TemplateToStepperCalibrationDone, bool);
	vtkSetMacro(TemplateToStepperCalibrationDone, bool);
	vtkBooleanMacro(TemplateToStepperCalibrationDone, bool);

	//! Description: 
	// Transfomration from the template origin to stepper
	vtkSetObjectMacro(TemplateToStepperTransformMatrix, vtkMatrix4x4);
	vtkGetObjectMacro(TemplateToStepperTransformMatrix, vtkMatrix4x4);

	//! Description: 
	// Transfomration from stepper to the template holder center hole (left)
	vtkSetObjectMacro(StepperToTemplateHolderTransformMatrix, vtkMatrix4x4);
	vtkGetObjectMacro(StepperToTemplateHolderTransformMatrix, vtkMatrix4x4);
	
	//! Description: 
	// Transfomration from stepper to the template origin
	vtkSetObjectMacro(StepperToTemplateTransformMatrix, vtkMatrix4x4);
	vtkGetObjectMacro(StepperToTemplateTransformMatrix, vtkMatrix4x4);

	//! Description: 
	// Transfomration from template holder to the template origin
	vtkSetObjectMacro(TemplateHolderToTemplateTransformMatrix, vtkMatrix4x4);
	vtkGetObjectMacro(TemplateHolderToTemplateTransformMatrix, vtkMatrix4x4);
	

protected:
	vtkTemplateToStepperCalibrator();
	virtual ~vtkTemplateToStepperCalibrator();
	
	// The minimum number of points needed for the center of rotation 
	// computing algorithm
	int MinNumberOfPointsForCalibration; 

	// Stores the center of rotation in px space
	// Origin: Left-upper corner (the original image frame)
	// Positive X: to the right;
	// Positive Y: to the bottom;
	int CenterOfRotation[2]; 

	// Image scaling factors 
	// - x: lateral axis;
	// - y: axial axis;
	double Spacing[2]; 

	// Horizontal [0] and vertical [1] distance between the phantom (a line defined by two points) 
	// and probe in mm 
	double PhantomToProbeDistanceInMm[2]; 

	// Flag of template to stepper calibration success 
	bool TemplateToStepperCalibrationDone;

	std::vector< std::vector<HomogenousVector4x1> > PointSetForCenterOfRotationCalculation;

	std::vector< std::vector<HomogenousVector4x1> > PointSetForPhantomToProbeDistanceCalculation;

	std::vector<Distance> PointSetForSpacingCalculation; 

	vtkMatrix4x4* TemplateToStepperTransformMatrix;
	vtkMatrix4x4* StepperToTemplateHolderTransformMatrix;
	vtkMatrix4x4* StepperToTemplateTransformMatrix;
	vtkMatrix4x4* TemplateHolderToTemplateTransformMatrix; 

	BrachyTRUSCalibrator* Calibrator; 

private:
	vtkTemplateToStepperCalibrator(const vtkTemplateToStepperCalibrator&);
	void operator=(const vtkTemplateToStepperCalibrator&);
};

#endif //  __VTKTEMPLATETOSTEPPERCALIBRATOR_H