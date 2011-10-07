#ifndef __VTKSTEPPERCALIBRATIONCONTROLLER_H
#define __VTKSTEPPERCALIBRATIONCONTROLLER_H
#include "vtkCalibrationController.h"
#include "vtkXMLDataElement.h"

class vtkHTMLGenerator; 
class vtkGnuplotExecuter; 
class vtkTable; 

enum CalibrationMode
{
	REALTIME, 
	OFFLINE
};  

class vtkStepperCalibrationController : public vtkCalibrationController
{
public:

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
	vtkSetVector2Macro(CenterOfRotationPx, double); 
	vtkGetVector2Macro(CenterOfRotationPx, double);

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
	// Set/get algorithm version in string 
	vtkSetStringMacro(AlgorithmVersion); 
	vtkGetStringMacro(AlgorithmVersion); 
  
	//! Description: 
	// Set/get calibration start time in string 
	vtkSetStringMacro(CalibrationStartTime); 
	vtkGetStringMacro(CalibrationStartTime); 
	
	//! Description: 
	// Set/get calibration mode
	vtkSetMacro(CalibrationMode, CalibrationMode); 
	vtkGetMacro(CalibrationMode, CalibrationMode); 


protected:
	vtkStepperCalibrationController ();
	virtual ~vtkStepperCalibrationController ();

	//! Description: 
	// Set the calibration start time
	virtual void SaveCalibrationStartTime(); 

	//***************************************************************************
	//					Translation axis calibration
	//***************************************************************************

	//! Description: 
	// Do the translation axis calibration 
	// Returns true on success otherwise false
	virtual PlusStatus CalibrateTranslationAxis(IMAGE_DATA_TYPE dataType); 

	//***************************************************************************
	//						Rotation axis calibration
	//***************************************************************************

	//! Description: 
	// Do the rotation axis calibration 
	// Returns true on success otherwise false
	virtual PlusStatus CalibrateRotationAxis(); 

	//***************************************************************************
	//						Rotation encoder calibration
	//***************************************************************************

	//! Description: 
	// Do the rotation encoder calibration 
	// Returns true on success otherwise false
	virtual PlusStatus CalibrateRotationEncoder(); 

	//***************************************************************************
	//							Spacing calculation
	//***************************************************************************

	//! Description: 
	// Compute spacing using linear least squares
	// This computation needs a set of point distances between two well known 
	// object on the image in X and Y direction (in px and mm as well) to define the spacing.
	// Returns true on success otherwise false
	virtual PlusStatus CalculateSpacing(); 

protected:

	bool SpacingCalculated; 
	bool CenterOfRotationCalculated; 
	bool ProbeRotationAxisCalibrated; 
	bool ProbeTranslationAxisCalibrated; 
	bool TemplateTranslationAxisCalibrated; 
	bool ProbeRotationEncoderCalibrated; 

	// Stores the center of rotation in px space
	// Origin: Left-upper corner (the original image frame)
	// Positive X: to the right;
	// Positive Y: to the bottom;
	double CenterOfRotationPx[2]; 

	// Image scaling factors 
	// (x: lateral axis, y: axial axis)
	double Spacing[2];

	// Probe translation axis orientation [Tx, Ty, 1]
	double ProbeTranslationAxisOrientation[3]; 

	// Template translation axis orientation [Tx, Ty, 1]
	double TemplateTranslationAxisOrientation[3]; 

	// Probe rotation axis orientation [Rx, Ry, 1]
	double ProbeRotationAxisOrientation[3]; 

	double ProbeRotationEncoderOffset; 
	double ProbeRotationEncoderScale; 

	// Stores the calibration start time in string format
	char* CalibrationStartTime; 

  char* AlgorithmVersion; 

	int MinNumberOfRotationClusters; 

	//! calibration mode (see CALIBRATION_MODE)
	CalibrationMode CalibrationMode;

private:
	vtkStepperCalibrationController (const vtkStepperCalibrationController &);
	void operator=(const vtkStepperCalibrationController &);
};

#endif //  __VTKSTEPPERCALIBRATIONCONTROLLER_H
