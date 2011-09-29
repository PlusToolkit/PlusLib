#ifndef __vtkCenterOfRotationCalibAlgo_h
#define __vtkCenterOfRotationCalibAlgo_h

#include "PlusConfigure.h"
#include "vtkObject.h"
#include "vtkTrackedFrameList.h"
#include "vtkTable.h"

class vtkHTMLGenerator; 
class vtkGnuplotExecuter; 

class VTK_EXPORT vtkCenterOfRotationCalibAlgo : public vtkObject
{
public:

  static vtkCenterOfRotationCalibAlgo *New();
  vtkTypeRevisionMacro(vtkCenterOfRotationCalibAlgo , vtkObject);
  virtual void PrintSelf(ostream& os, vtkIndent indent); 

  // Description:
  // Add generated html report from the selected data type (probe or template) translation axis calibration to the existing html report
  // htmlReport and plotter arguments has to be defined by the caller function
  virtual PlusStatus GenerateReport( vtkHTMLGenerator* htmlReport, vtkGnuplotExecuter* plotter, const char* gnuplotScriptsFolder); 

  // Description:
  // Set inputs: 
  // - TrackedFrameList with segmentation results 
  // - Image spacing (mm/px)
  virtual void SetInputs( vtkTrackedFrameList* trackedFrameList, std::vector<int> &indices, double spacing[2]); 

  // Description:
  // Set tracked frame indices used for calibration 
  virtual void SetTrackedFrameListIndices( std::vector<int> &indices ); 

  //! Description: 
  // Set/get tracked frame list
  vtkSetObjectMacro(TrackedFrameList, vtkTrackedFrameList); 
  vtkGetObjectMacro(TrackedFrameList, vtkTrackedFrameList); 

  //! Description: 
  // Set/Get the image spacing.
  // (x: lateral axis, y: axial axis)
  vtkSetVector2Macro(Spacing, double); 
  vtkGetVector2Macro(Spacing, double); 

  //! Description: 
  // Report table used for storing algorithm results 
  vtkGetObjectMacro(ReportTable, vtkTable); 

  // Description:
  // Get the rotation axis orientation 
  virtual PlusStatus GetCenterOfRotationPx(double centerOfRotationPx[2] ); 

  // Description:
  // Get the rotation axis calibration error 
  virtual PlusStatus GetError(double &mean, double &stdev); 

protected:
  vtkCenterOfRotationCalibAlgo();
  virtual ~vtkCenterOfRotationCalibAlgo(); 

  // Description:
  // Bring this algorithm's outputs up-to-date.
  virtual PlusStatus Update();

  //! Description: 
  // Construct linear equation for center of rotation calibration
  virtual PlusStatus ConstructLinearEquationForCalibration(std::vector<vnl_vector<double>> &aMatrix, std::vector<double> &bVector);

  //! Description: 
  // Add new column to the report table 
  PlusStatus AddNewColumnToReportTable( const char* columnName ); 

  //! Description: 
  // Update rotation axis calibration error report table
  virtual PlusStatus UpdateReportTable(); 

  //! Description: 
  // Report table used for storing algorithm results 
  vtkSetObjectMacro(ReportTable, vtkTable); 

  //! Description: 
	// Set the rotation center in pixels.
	vtkSetVector2Macro(CenterOfRotationPx, int); 

  // Stores the center of rotation in px space
	int CenterOfRotationPx[2]; 

  // Image scaling factors 
  // (x: lateral axis, y: axial axis)
  double Spacing[2];

  vtkTrackedFrameList* TrackedFrameList; 
  std::vector<int> TrackedFrameListIndices; 

  // Table used for storing algo results 
  vtkTable* ReportTable; 

  double ErrorMean; 
  double ErrorStdev; 

  // When was this data last updated
  vtkTimeStamp UpdateTime;  

}; 

#endif