#ifndef __vtkTranslAxisCalibAlgo_h
#define __vtkTranslAxisCalibAlgo_h

#include "PlusConfigure.h"
#include "vtkAlgorithm.h"
#include "vtkStepperCalibrationController.h"
#include "vtkTrackedFrameList.h"

class vtkHTMLGenerator; 
class vtkGnuplotExecuter; 
class vtkTable; 

class VTK_EXPORT vtkTranslAxisCalibAlgo : public vtkAlgorithm
{
public:

  static vtkTranslAxisCalibAlgo *New();
	vtkTypeRevisionMacro(vtkTranslAxisCalibAlgo , vtkAlgorithm);
	virtual void PrintSelf(ostream& os, vtkIndent indent); 

  // Description:
	// Add generated html report from the selected data type (probe or template) translation axis calibration to the existing html report
	// htmlReport and plotter arguments has to be defined by the caller function
	virtual PlusStatus GenerateReport( vtkHTMLGenerator* htmlReport, vtkGnuplotExecuter* plotter, const char* gnuplotScriptsFolder); 

  // Description:
  // Bring this algorithm's outputs up-to-date.
  virtual void Update();

  // Description:
  // 
  virtual void SetInput( vtkTrackedFrameList* trackedFrameList ); 

  //! Description: 
	// Set/Get the image spacing.
	// (x: lateral axis, y: axial axis)
	vtkSetVector2Macro(Spacing, double); 
	vtkGetVector2Macro(Spacing, double); 

  // Description:
  // Get the translation axis orientation 
  virtual double * GetOutput(); 

protected:
  vtkTranslAxisCalibAlgo();
  ~vtkTranslAxisCalibAlgo(); 

  //! Description: 
	// Construct linear equation for translation axis calibration
	virtual PlusStatus ConstrLinEqForTransAxisCalib(std::vector<vnl_vector<double>> &aMatrix, std::vector<double> &bVector);

  //! Description: 
	// Remove outliers from translation axis calibration dataset
	virtual void RemoveOutliersFromTransAxisCalibData(std::vector<vnl_vector<double>> &aMatrix, std::vector<double> &bVector, const vnl_vector<double> &resultVector );
	
	//! Description: 
	// Calculate mean error and stdev of measured and computed wire positions for each wire
	virtual void GetTranslationAxisCalibrationError(
		const std::vector<vnl_vector<double>> &aMatrix, 
		const std::vector<double> &bVector, 
		const vnl_vector<double> &resultVector, 
    std::vector<vtkStepperCalibrationController::CalibStatistics> &statistics); 

	//! Description: 
	// Save translation axis calibration error in gnuplot format 
	virtual void SaveTranslationAxisCalibrationError(
		const std::vector<vnl_vector<double>> &aMatrix, 
		const std::vector<double> &bVector, 
		const vnl_vector<double> &resultVector); 

  //! Description: 
	// Set/get tracked frame list
  vtkSetObjectMacro(TrackedFrameList, vtkTrackedFrameList); 
  vtkGetObjectMacro(TrackedFrameList, vtkTrackedFrameList); 

  //! Description: 
	// Set/get translation axis orientation [Tx, Ty, 1]
	vtkSetVector3Macro(TranslationAxisOrientation, double); 
	vtkGetVector3Macro(TranslationAxisOrientation, double); 

  // Image scaling factors 
	// (x: lateral axis, y: axial axis)
	double Spacing[2];

  // Translation axis orientation [Tx, Ty, 1]
	double TranslationAxisOrientation[3]; 

  vtkTrackedFrameList* TrackedFrameList; 
}; 

#endif