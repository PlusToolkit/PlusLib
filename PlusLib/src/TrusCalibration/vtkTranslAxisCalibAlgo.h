#ifndef __vtkTranslAxisCalibAlgo_h
#define __vtkTranslAxisCalibAlgo_h

#include "PlusConfigure.h"
#include "vtkObject.h"
#include "vtkStepperCalibrationController.h"
#include "vtkTrackedFrameList.h"
#include "vtkTable.h"

class vtkHTMLGenerator; 
class vtkGnuplotExecuter; 

class VTK_EXPORT vtkTranslAxisCalibAlgo : public vtkObject
{
public:

  static vtkTranslAxisCalibAlgo *New();
	vtkTypeRevisionMacro(vtkTranslAxisCalibAlgo , vtkObject);
	virtual void PrintSelf(ostream& os, vtkIndent indent); 

  // Description:
	// Add generated html report from the selected data type (probe or template) translation axis calibration to the existing html report
	// htmlReport and plotter arguments has to be defined by the caller function
	virtual PlusStatus GenerateReport( vtkHTMLGenerator* htmlReport, vtkGnuplotExecuter* plotter, const char* gnuplotScriptsFolder); 

  // Description:
  // Set input TrackedFrameList with segmentation results 
  virtual void SetInput( vtkTrackedFrameList* trackedFrameList ); 

  // Description:
  // Set/get image data type used for calibration
  // Supported data types: TEMPLATE_TRANSLATION, PROBE_TRANSLATION
  vtkSetMacro(DataType, IMAGE_DATA_TYPE); 
  vtkGetMacro(DataType, IMAGE_DATA_TYPE); 

  //! Description: 
	// Set/Get the image spacing.
	// (x: lateral axis, y: axial axis)
	vtkSetVector2Macro(Spacing, double); 
	vtkGetVector2Macro(Spacing, double); 

  //! Description: 
	// Report table used for storing algorithm results 
  vtkGetObjectMacro(ReportTable, vtkTable); 

  // Description:
  // Get the translation axis orientation 
  virtual PlusStatus GetOutput(double translationAxisOrientation[3] ); 

  // Description:
  // Get the translation axis calibration error 
  virtual PlusStatus GetError(double &mean, double &stdev); 

protected:
  vtkTranslAxisCalibAlgo();
  virtual ~vtkTranslAxisCalibAlgo(); 

  // Description:
  // Bring this algorithm's outputs up-to-date.
  virtual PlusStatus Update();

  //! Description: 
	// Construct linear equation for translation axis calibration
	virtual PlusStatus ConstrLinEqForTransAxisCalib(std::vector<vnl_vector<double>> &aMatrix, std::vector<double> &bVector, int& numberOfUnknowns);

	//! Description: 
	// Save translation axis calibration error in gnuplot format 
	virtual PlusStatus UpdateReportTable(
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

  //! Description: 
	// Report table used for storing algorithm results 
  vtkSetObjectMacro(ReportTable, vtkTable); 

  // Data type used for calibration (TEMPLATE_TRANSLATION or PROBE_TRANSLATION)
  IMAGE_DATA_TYPE DataType; 

  // Image scaling factors 
	// (x: lateral axis, y: axial axis)
	double Spacing[2];

  // Translation axis orientation [Tx, Ty, 1]
	double TranslationAxisOrientation[3]; 

  vtkTrackedFrameList* TrackedFrameList; 

  // Table used for storing algo results 
  vtkTable* ReportTable; 

  double ErrorMean; 
  double ErrorStdev; 

  // When was this data last updated
  vtkTimeStamp UpdateTime;  

}; 

#endif