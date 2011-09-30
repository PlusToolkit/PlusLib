// Center of rotation calibration 
// Asumptions
//    Distance between a non-moving wire position (denoted as S) and the rotation center 
//    of the optical encoder (denoted as O) remains constant during rotation:
//        || Si - O || = || Sj - O ||
// Model:
//        pow(Sxi - Ox, 2) + pow(Syi - Oy, 2) = pow(Sxj - Ox, 2) + pow(Syj - Oy, 2) where i != j
// Unknowns:
//    Ox, Oy = center of rotation in pixel 
// Linear system:
//    Ax = B
//
// [ (Sxi - Sxj) (Syi - Syj) ] * [ Ox ] = [ (0.5*( Sxi*Sxi + Syi*Syi - Sxj*Sxj - Syj*Syj )) ]
//                               [ Oy ]

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
  // Set inputs: 
  // - TrackedFrameList with segmentation results 
  // - Frame indices that the algoritm should use during calibration
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
  // Set/Get the image spacing (mm/px)
  vtkSetVector2Macro(Spacing, double); 
  vtkGetVector2Macro(Spacing, double); 

  //! Description: 
  // Report table used for storing algorithm results 
  vtkGetObjectMacro(ReportTable, vtkTable); 

  // Description:
  // Get the center of rotation in px 
  virtual PlusStatus GetCenterOfRotationPx( double centerOfRotationPx[2] ); 

  // Description:
  // Get the center of rotation calibration error 
  virtual PlusStatus GetError(double &mean, double &stdev); 

  // Description:
  // Add generated html report from center of rotation calibration to the existing html report
  // htmlReport and plotter arguments has to be defined by the caller function
  virtual PlusStatus GenerateReport( vtkHTMLGenerator* htmlReport, vtkGnuplotExecuter* plotter, const char* gnuplotScriptsFolder); 
  static PlusStatus GenerateCenterOfRotationReport( vtkHTMLGenerator* htmlReport, 
    vtkGnuplotExecuter* plotter, 
    const char* gnuplotScriptsFolder, 
    vtkTable* reportTable,
    double centerOfRotationPx[2]); 

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
  // Update center of rotation calibration error report table
  virtual PlusStatus UpdateReportTable(); 

  //! Description: 
  // Report table used for storing algorithm results 
  vtkSetObjectMacro(ReportTable, vtkTable); 

  //! Description: 
	// Set the rotation center in pixels.
	vtkSetVector2Macro(CenterOfRotationPx, int); 

  // Stores the center of rotation in px space
	double CenterOfRotationPx[2]; 

  // Image scaling factors 
  double Spacing[2];

  // Tracked frames used for calibration 
  vtkTrackedFrameList* TrackedFrameList; 

  // Tracked frame indices used for calibration 
  std::vector<int> TrackedFrameListIndices; 

  // Table used for storing algo results 
  vtkTable* ReportTable; 

  double ErrorMean; 
  double ErrorStdev; 

  // When was this data last updated
  vtkTimeStamp UpdateTime;  

}; 

#endif