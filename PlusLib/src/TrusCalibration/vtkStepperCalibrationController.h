#ifndef __VTKSTEPPERCALIBRATIONCONTROLLER_H
#define __VTKSTEPPERCALIBRATIONCONTROLLER_H
#include "vtkCalibrationController.h"
#include "vtkXMLDataElement.h"

class vtkHTMLGenerator; 
class vtkGnuplotExecuter; 
class vtkTable; 

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

class vtkStepperCalibrationController : public vtkCalibrationController
{
public:

  struct CalibStatistics
  {
    double Mean; 
    double Stdev; 
    double Min; 
    double Max; 
  }; 

	static vtkStepperCalibrationController *New();
	vtkTypeRevisionMacro(vtkStepperCalibrationController , vtkCalibrationController);
	virtual void PrintSelf(ostream& os, vtkIndent indent); 

	//! Description 
	// Initialize the calibration controller interface
	virtual PlusStatus Initialize(); 

	//! Description 
	// Read XML based configuration of the calibration controller
	virtual PlusStatus ReadConfiguration( vtkXMLDataElement* configData ); 

  virtual PlusStatus WriteConfiguration( vtkXMLDataElement* configData ); 

	//! Description 
	// Add new tracked data for segmentation and save the segmentation result to the SegmentedFrameContainer
	// The class has to be initialized before the segmentation process. 
	virtual PlusStatus AddVtkImageData( vtkImageData* frame, const double probePosition, const double probeRotation, const double templatePosition, IMAGE_DATA_TYPE dataType ); 

	//! Description 
	// Add new tracked data for segmentation and save the segmentation result to the SegmentedFrameContainer
	// The class has to be initialized before the segmentation process. 
	virtual PlusStatus AddItkImageData( const ImageType::Pointer& frame, const double probePosition, const double probeRotation, const double templatePosition, IMAGE_DATA_TYPE dataType );

	//! Description: 
	// Get stepper encoder values from the tracked frame info
	// Return true if we found the encoder values, otherwise false
	static PlusStatus GetStepperEncoderValues( TrackedFrame* trackedFrame, double &probePosition, double &probeRotation, double &templatePosition); 

	//! Description 
	// Run the probe rotation axis calibration algorithm 
	// Returns true on success otherwise false
	virtual PlusStatus CalibrateProbeRotationAxis(); 

	//! Description 
	// Run the probe translation axis calibration algorithm 
	virtual PlusStatus CalibrateProbeTranslationAxis(); 

	//! Description 
	// Run the template translation axis calibration algorithm 
	// Returns true on success otherwise false
	virtual PlusStatus CalibrateTemplateTranslationAxis(); 

	//! Description 
	// Run the probe rotation axis calibration algorithm in offline mode
	virtual PlusStatus OfflineProbeRotationAxisCalibration(); 

	//! Description 
	// Run the probe translation axis calibration algorithm in offline mode
	virtual PlusStatus OfflineProbeTranslationAxisCalibration(); 

	//! Description 
	// Run the template rotation axis calibration algorithm in offline mode
	virtual PlusStatus OfflineTemplateTranslationAxisCalibration(); 

	//! Description 
	// Read the stepper calibration configurations from xml data element
	virtual PlusStatus ReadStepperCalibrationConfiguration(vtkXMLDataElement* rootElement); 

	// Description:
	// Add generated html report from probe rotation axis calibration to the existing html report
	// htmlReport and plotter arguments has to be defined by the caller function
	virtual PlusStatus GenerateProbeRotationAxisCalibrationReport( vtkHTMLGenerator* htmlReport, vtkGnuplotExecuter* plotter, const char* gnuplotScriptsFolder); 

	// Description:
	// Add generated html report from probe translation axis calibration to the existing html report
	// htmlReport and plotter arguments has to be defined by the caller function
	virtual void GenerateProbeTranslationAxisCalibrationReport( vtkHTMLGenerator* htmlReport, vtkGnuplotExecuter* plotter, const char* gnuplotScriptsFolder); 

	// Description:
	// Add generated html report from template translation axis calibration to the existing html report
	// htmlReport and plotter arguments has to be defined by the caller function
	virtual void GenerateTemplateTranslationAxisCalibrationReport( vtkHTMLGenerator* htmlReport, vtkGnuplotExecuter* plotter, const char* gnuplotScriptsFolder); 

	// Description:
	// Add generated html report from probe rotation encoder calibration to the existing html report
	// htmlReport and plotter arguments has to be defined by the caller function
	virtual PlusStatus GenerateProbeRotationEncoderCalibrationReport( vtkHTMLGenerator* htmlReport, vtkGnuplotExecuter* plotter, const char* gnuplotScriptsFolder); 

	// Description:
	// Add generated html report from spacing calculation to the existing html report
	// htmlReport and plotter arguments has to be defined by the caller function
	virtual PlusStatus GenerateSpacingCalculationReport( vtkHTMLGenerator* htmlReport, vtkGnuplotExecuter* plotter, const char* gnuplotScriptsFolder); 
	
	// Description:
	// Add generated html report from center of rotation calculation to the existing html report
	// htmlReport and plotter arguments has to be defined by the caller function
	virtual PlusStatus GenerateCenterOfRotationReport( vtkHTMLGenerator* htmlReport, vtkGnuplotExecuter* plotter, const char* gnuplotScriptsFolder); 

	// Description:
	// Set/get outlier detection threshold
	vtkSetMacro(OutlierDetectionThreshold, double); 
	vtkGetMacro(OutlierDetectionThreshold, double); 

	//! Description: 
	// Set/Get the image spacing.
	// (x: lateral axis, y: axial axis)
	vtkSetVector2Macro(Spacing, double); 
	vtkGetVector2Macro(Spacing, double); 

	//! Description: 
	// Set/get probe translation axis orientation [Tx, Ty, 1]
	vtkSetVector3Macro(ProbeTranslationAxisOrientation, double); 
	vtkGetVector3Macro(ProbeTranslationAxisOrientation, double); 

	//! Description: 
	// Set/get template translation axis orientation [Tx, Ty, 1]
	vtkSetVector3Macro(TemplateTranslationAxisOrientation, double); 
	vtkGetVector3Macro(TemplateTranslationAxisOrientation, double); 

	//! Description: 
	// Rotation axis orientation [Rx, Ry, 1]
	vtkSetVector3Macro(ProbeRotationAxisOrientation, double); 
	vtkGetVector3Macro(ProbeRotationAxisOrientation, double); 

	//! Description: 
	// Set/Get the rotation center in pixels.
	// Origin: Left-upper corner (the original image frame)
	// Positive X: to the right;
	// Positive Y: to the bottom;
	vtkSetVector2Macro(CenterOfRotationPx, int); 
	vtkGetVector2Macro(CenterOfRotationPx, int);

	//! Description: 
	// Set/Get the minimum number of clusters for rotation axis calibration
	vtkSetMacro(MinNumberOfRotationClusters, int); 
	vtkGetMacro(MinNumberOfRotationClusters, int);
	
	//! Description: 
	// Set/Get the distance between the phantom and probe
	// Horizontal [0] and vertical [1] distance in mm
	vtkSetVector2Macro(PhantomToProbeDistanceInMm, double); 
	vtkGetVector2Macro(PhantomToProbeDistanceInMm, double); 

	// Description:
	// Set/get probe rotation encoder offset
	vtkSetMacro(ProbeRotationEncoderOffset, double); 
	vtkGetMacro(ProbeRotationEncoderOffset, double); 

	// Description:
	// Set/get probe rotation encoder scale
	vtkSetMacro(ProbeRotationEncoderScale, double); 
	vtkGetMacro(ProbeRotationEncoderScale, double); 

	//! Description: 
	// Set/get probe rotation axis calibration finished flag
	vtkSetMacro(ProbeRotationAxisCalibrated, bool); 
	vtkGetMacro(ProbeRotationAxisCalibrated, bool); 
	vtkBooleanMacro(ProbeRotationAxisCalibrated, bool); 

	//! Description: 
	// Set/get probe rotation encoder calibration finished flag
	vtkSetMacro(ProbeRotationEncoderCalibrated, bool); 
	vtkGetMacro(ProbeRotationEncoderCalibrated, bool); 
	vtkBooleanMacro(ProbeRotationEncoderCalibrated, bool); 

	//! Description: 
	// Set/get probe translation axis calibration finished flag
	vtkSetMacro(ProbeTranslationAxisCalibrated, bool); 
	vtkGetMacro(ProbeTranslationAxisCalibrated, bool); 
	vtkBooleanMacro(ProbeTranslationAxisCalibrated, bool); 
	
	//! Description: 
	// Set/get template translation axis calibration finished flag
	vtkSetMacro(TemplateTranslationAxisCalibrated, bool); 
	vtkGetMacro(TemplateTranslationAxisCalibrated, bool); 
	vtkBooleanMacro(TemplateTranslationAxisCalibrated, bool); 

	//! Description: 
	// Set/get spacing calculated finished flag
	vtkSetMacro(SpacingCalculated, bool); 
	vtkGetMacro(SpacingCalculated, bool); 
	vtkBooleanMacro(SpacingCalculated, bool); 

	//! Description: 
	// Set/get center of rotation finished flag
	vtkSetMacro(CenterOfRotationCalculated, bool); 
	vtkGetMacro(CenterOfRotationCalculated, bool); 
	vtkBooleanMacro(CenterOfRotationCalculated, bool); 

  //! Description: 
	// Set/get phantom to probe distance calculated finished flag
	vtkSetMacro(PhantomToProbeDistanceCalculated, bool); 
	vtkGetMacro(PhantomToProbeDistanceCalculated, bool); 
	vtkBooleanMacro(PhantomToProbeDistanceCalculated, bool); 

  //! Description: 
	// Set/get algorithm version in string 
	vtkSetStringMacro(AlgorithmVersion); 
	vtkGetStringMacro(AlgorithmVersion); 
  
	//! Description: 
	// Set/get calibration start time in string 
	vtkSetStringMacro(CalibrationStartTime); 
	vtkGetStringMacro(CalibrationStartTime); 

	//! Description: 
	// Set/get probe rotation axis calibration error report file path
	vtkSetStringMacro(ProbeRotationAxisCalibrationErrorReportFilePath); 
	vtkGetStringMacro(ProbeRotationAxisCalibrationErrorReportFilePath); 

	//! Description: 
	// Set/get probe translation axis calibration error report file path
	vtkSetStringMacro(ProbeTranslationAxisCalibrationErrorReportFilePath); 
	vtkGetStringMacro(ProbeTranslationAxisCalibrationErrorReportFilePath); 

	//! Description: 
	// Set/get template translation axis calibration error report file name
	vtkSetStringMacro(TemplateTranslationAxisCalibrationErrorReportFilePath); 
	vtkGetStringMacro(TemplateTranslationAxisCalibrationErrorReportFilePath); 
	
	//! Description: 
	// Set/get probe rotation encoder calibration error report file path
	vtkSetStringMacro(ProbeRotationEncoderCalibrationErrorReportFilePath); 
	vtkGetStringMacro(ProbeRotationEncoderCalibrationErrorReportFilePath); 

	//! Description: 
	// Set/get spacing calculation error report file path
	vtkSetStringMacro(SpacingCalculationErrorReportFilePath); 
	vtkGetStringMacro(SpacingCalculationErrorReportFilePath); 

	//! Description: 
	// Set/get center of rotation calculation error report file path
	vtkSetStringMacro(CenterOfRotationCalculationErrorReportFilePath); 
	vtkGetStringMacro(CenterOfRotationCalculationErrorReportFilePath); 
	
	
protected:
	vtkStepperCalibrationController ();
	virtual ~vtkStepperCalibrationController ();

	//! Description: 
	// Set the calibration start time
	virtual void SaveCalibrationStartTime(); 

	// Description:
	// Compute mean and stddev from dataset
	virtual PlusStatus ComputeStatistics(const std::vector< std::vector<double> > &diffVector, std::vector<CalibStatistics> &statistics); 

	//***************************************************************************
	//					Translation axis calibration
	//***************************************************************************

	//! Description: 
	// Do the translation axis calibration 
	// Returns true on success otherwise false
	virtual PlusStatus CalibrateTranslationAxis(IMAGE_DATA_TYPE dataType); 

	//! Description: 
	// Construct linear equation for translation axis calibration
	virtual void ConstrLinEqForTransAxisCalib( 
		std::vector<vnl_vector<double>> &aMatrix, 
		std::vector<double> &bVector, 
		IMAGE_DATA_TYPE dataType );

	//! Description: 
	// Remove outliers from translation axis calibration dataset
	virtual void RemoveOutliersFromTransAxisCalibData(std::vector<vnl_vector<double>> &aMatrix, std::vector<double> &bVector, const vnl_vector<double> &resultVector );
	
	//! Description: 
	// Calculate mean error and stdev of measured and computed wire positions for each wire
	virtual void GetTranslationAxisCalibrationError(
		const std::vector<vnl_vector<double>> &aMatrix, 
		const std::vector<double> &bVector, 
		const vnl_vector<double> &resultVector, 
    std::vector<CalibStatistics> &statistics); 

	//! Description: 
	// Save translation axis calibration error in gnuplot format 
	virtual void SaveTranslationAxisCalibrationError(
		const std::vector<vnl_vector<double>> &aMatrix, 
		const std::vector<double> &bVector, 
		const vnl_vector<double> &resultVector, 
		IMAGE_DATA_TYPE dataType ); 

	// Description:
	// Add generated html report from the selected data type (probe or template) translation axis calibration to the existing html report
	// htmlReport and plotter arguments has to be defined by the caller function
	virtual PlusStatus GenerateTranslationAxisCalibrationReport( IMAGE_DATA_TYPE dataType, vtkHTMLGenerator* htmlReport, vtkGnuplotExecuter* plotter, const char* gnuplotScriptsFolder); 


	//***************************************************************************
	//						Rotation axis calibration
	//***************************************************************************

	//! Description: 
	// Do the rotation axis calibration 
	// Returns true on success otherwise false
	virtual PlusStatus CalibrateRotationAxis(); 

	//! Description: 
	// Construct linear equation for rotation axis calibration
	virtual void ConstrLinEqForRotationAxisCalib( 
		const std::vector< std::pair<double, double> > &listOfCenterOfRotations, 
		const std::vector< double > &listOfClusterPositions, 
		std::vector<vnl_vector<double>> &aMatrix, 
		std::vector<double> &bVector); 


	//! Description: 
	// Calculate mean error and stdev of measured and computed rotation angles
	virtual void GetRotationAxisCalibrationError(
		const std::vector<vnl_vector<double>> &aMatrix, 
		const std::vector<double> &bVector, \
		const vnl_vector<double> &resultVector,
    std::vector<CalibStatistics> &statistics); 

	//! Description: 
	// Remove outliers from rotation axis calibration dataset
	virtual void RemoveOutliersFromRotAxisCalibData(
		std::vector<vnl_vector<double>> &aMatrix, 
		std::vector<double> &bVector, 
		vnl_vector<double> resultVector );

	//! Description: 
	// Save rotation axis calibration error in gnuplot format 
	virtual void SaveRotationAxisCalibrationError(
		const std::vector<vnl_vector<double>> &aMatrix, 
		const std::vector<double> &bVector, 
		const vnl_vector<double> &resultVector); 

	//***************************************************************************
	//						Rotation encoder calibration
	//***************************************************************************

	//! Description: 
	// Do the rotation encoder calibration 
	// Returns true on success otherwise false
	virtual PlusStatus CalibrateRotationEncoder(); 

	//! Description: 
	// Construct linear equation for rotation encoder calibration
	virtual void ConstrLinEqForRotEncCalc( 
		std::vector<vnl_vector<double>> &aMatrix, 
		std::vector<double> &bVector); 
	
	//! Description: 
	// Remove outliers from rotation encoder calibration dataset
	virtual void RemoveOutliersFromRotEncCalibData(
		std::vector<vnl_vector<double>> &aMatrix, 
		std::vector<double> &bVector, 
		vnl_vector<double> resultVector );

	//! Description: 
	// Calculate mean error and stdev of measured and computed rotation angles
	virtual void GetRotationEncoderCalibrationError(
		const std::vector<vnl_vector<double>> &aMatrix, 
		const std::vector<double> &bVector, 
		const vnl_vector<double> &resultVector, 
    CalibStatistics &statistics); 

	//! Description: 
	// Save rotation encoder calibration error in gnuplot format 
	virtual void SaveRotationEncoderCalibrationError(
		const std::vector<vnl_vector<double>> &aMatrix, 
		const std::vector<double> &bVector, 
		const vnl_vector<double> &resultVector ); 

	//***************************************************************************
	//						Center of rotation calculation
	//***************************************************************************

	//! Description: 
	// Compute rotation center using linear least squares
	// Returns true on success otherwise false
	virtual PlusStatus CalculateCenterOfRotation( SegmentedFrameList &frameListForCenterOfRotation, double centerOfRotationPx[2], vtkTable* centerOfRotationCalculationErrorTable );

	//! Description: 
	// Calculate mean error and stdev of measured and computed distances between rotation center and segmented wires
	virtual void GetCenterOfRotationCalculationError(
		const std::vector<vnl_vector<double>> &aMatrix, 
		const std::vector<double> &bVector, 
		const vnl_vector<double> &resultVector, 
		CalibStatistics &statistics); 

	//! Description: 
	// Remove outliers from center of rotation calculation dataset
	virtual void RemoveOutliersFromCenterOfRotCalcData(
		std::vector<vnl_vector<double>> &aMatrix, 
		std::vector<double> &bVector, 
		const vnl_vector<double> &resultVector ); 

	//! Description: 
	// Save center of rotation calibration error in gnuplot format 
	virtual void SaveCenterOfRotationCalculationError(
		SegmentedFrameList &frameListForCenterOfRotation, 
		const double centerOfRotationPx[2], 
		vtkTable* centerOfRotationCalculationErrorTable = NULL); 


	//***************************************************************************
	//							Spacing calculation
	//***************************************************************************

	//! Description: 
	// Compute spacing using linear least squares
	// This computation needs a set of point distances between two well known 
	// object on the image in X and Y direction (in px and mm as well) to define the spacing.
	// Returns true on success otherwise false
	virtual PlusStatus CalculateSpacing(); 

	//! Description: 
	// Remove outliers from spacing calculation dataset
	virtual void RemoveOutliersFromSpacingCalcData(
		std::vector<vnl_vector<double>> &aMatrix, 
		std::vector<double> &bVector, 
		vnl_vector<double> resultVector );

	//! Description: 
	// Calculate mean error and stdev of measured and computed spacing factors
	virtual void GetSpacingCalculationError(
		const std::vector<vnl_vector<double>> &aMatrix, 
		const std::vector<double> &bVector, 
		const vnl_vector<double> &resultVector, 
    std::vector<CalibStatistics> &statistics); 

	//! Description: 
	// Save spacing calculation error to file in gnuplot format 
	virtual void SaveSpacingCalculationError(const std::vector<vnl_vector<double>> &aMatrix, 
		const std::vector<double> &bVector, 
		const vnl_vector<double> &resultVector); 

	//! Description: 
	// Construct linear equation for spacing calculation
	virtual void ConstrLinEqForSpacingCalc( std::vector<vnl_vector<double>> &aMatrix, std::vector<double> &bVector); 
	
	//***************************************************************************
	//					Phantom to probe distance calculation
	//***************************************************************************
	
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
	// Returns true on success otherwise false
	virtual PlusStatus CalculatePhantomToProbeDistance(); 

	
	//***************************************************************************
	//								Clustering
	//***************************************************************************

	//! Description:
	// Clustering segmented frames by Z position 
	virtual void ClusterSegmentedFrames(IMAGE_DATA_TYPE dataType,  std::vector<SegmentedFrameList> &clusterList); 

	//! Description:
	// Get Z position of the cluster (aka. the z position of the point cloud)
	virtual double GetClusterZPosition(const SegmentedFrameList &cluster); 

protected:

	bool SpacingCalculated; 
	bool CenterOfRotationCalculated; 
  bool PhantomToProbeDistanceCalculated; 
	bool ProbeRotationAxisCalibrated; 
	bool ProbeTranslationAxisCalibrated; 
	bool TemplateTranslationAxisCalibrated; 
	bool ProbeRotationEncoderCalibrated; 

	// Stores the center of rotation in px space
	// Origin: Left-upper corner (the original image frame)
	// Positive X: to the right;
	// Positive Y: to the bottom;
	int CenterOfRotationPx[2]; 

	// Image scaling factors 
	// (x: lateral axis, y: axial axis)
	double Spacing[2];

	// Probe translation axis orientation [Tx, Ty, 1]
	double ProbeTranslationAxisOrientation[3]; 

	// Template translation axis orientation [Tx, Ty, 1]
	double TemplateTranslationAxisOrientation[3]; 

	// Probe rotation axis orientation [Rx, Ry, 1]
	double ProbeRotationAxisOrientation[3]; 

	// Horizontal [0] and vertical [1] distance between the phantom (a line defined by two points) 
	// and probe in mm 
	double PhantomToProbeDistanceInMm[2]; 

	double ProbeRotationEncoderOffset; 
	double ProbeRotationEncoderScale; 

	double OutlierDetectionThreshold; 

	// Stores the calibration start time in string format
	char* CalibrationStartTime; 

	std::vector< std::vector<HomogenousVector4x1> > PointSetForPhantomToProbeDistanceCalculation;

	char* ProbeRotationAxisCalibrationErrorReportFilePath; 
	char* ProbeTranslationAxisCalibrationErrorReportFilePath; 
	char* TemplateTranslationAxisCalibrationErrorReportFilePath; 
	char* ProbeRotationEncoderCalibrationErrorReportFilePath; 
	char* SpacingCalculationErrorReportFilePath; 
	char* CenterOfRotationCalculationErrorReportFilePath; 

  char* AlgorithmVersion; 

	int MinNumberOfRotationClusters; 
	int MinNumOfFramesUsedForCenterOfRotCalc; 

private:
	vtkStepperCalibrationController (const vtkStepperCalibrationController &);
	void operator=(const vtkStepperCalibrationController &);
};

#endif //  __VTKSTEPPERCALIBRATIONCONTROLLER_H
