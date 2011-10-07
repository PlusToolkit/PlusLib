#ifndef __vtkRotationEncoderCalibAlgo_h
#define __vtkRotationEncoderCalibAlgo_h

#include "PlusConfigure.h"
#include "vtkObject.h"
#include "vtkTrackedFrameList.h"
#include "vtkTable.h"

class vtkHTMLGenerator; 
class vtkGnuplotExecuter; 

class VTK_EXPORT vtkRotationEncoderCalibAlgo : public vtkObject
{
public:

  static vtkRotationEncoderCalibAlgo *New();
  vtkTypeRevisionMacro(vtkRotationEncoderCalibAlgo , vtkObject);
  virtual void PrintSelf(ostream& os, vtkIndent indent); 

  //! Add generated html report from rotation encoder calibration to the existing html report
  // htmlReport and plotter arguments has to be defined by the caller function
  virtual PlusStatus GenerateReport( vtkHTMLGenerator* htmlReport, vtkGnuplotExecuter* plotter, const char* gnuplotScriptsFolder); 

  //! Set inputs: 
  // - TrackedFrameList with segmentation results 
  // - 
  virtual void SetInputs( vtkTrackedFrameList* trackedFrameList, double spacing[2] ); 

  //! Report table used for storing algorithm results 
  vtkGetObjectMacro(ReportTable, vtkTable); 

  //! Get rotation encoder scale 
  virtual PlusStatus GetRotationEncoderScale( double &rotationEncoderScale ); 

  //! Get rotation encoder offset 
  virtual PlusStatus GetRotationEncoderOffset( double &rotationEncoderOffset ); 

  //! Get the rotation encoder calibration error 
  virtual PlusStatus GetError(double &mean, double &stdev); 

protected:
  vtkRotationEncoderCalibAlgo();
  virtual ~vtkRotationEncoderCalibAlgo(); 

  //! Bring this algorithm's outputs up-to-date.
  virtual PlusStatus Update();

  //! Construct linear equation for rotation encoder calibration
  virtual PlusStatus ConstructLinearEquationForCalibration(std::vector<vnl_vector<double>> &aMatrix, std::vector<double> &bVector);

  //! Description: 
  // Add new column to the report table 
  PlusStatus AddNewColumnToReportTable( const char* columnName ); 

  //! Description: 
  // Update rotation encoder calibration error report table 
  virtual PlusStatus UpdateReportTable(
    const std::vector<vnl_vector<double>> &aMatrix, 
    const std::vector<double> &bVector, 
    const vnl_vector<double> &resultVector); 

  //! Set/get tracked frame list
  vtkSetObjectMacro(TrackedFrameList, vtkTrackedFrameList); 
  vtkGetObjectMacro(TrackedFrameList, vtkTrackedFrameList); 

  //! Report table used for storing algorithm results 
  vtkSetObjectMacro(ReportTable, vtkTable); 

  //! Set spacing 
  vtkSetVector2Macro(Spacing, double); 

  //! Image scaling factors 
  // (x: lateral axis, y: axial axis)
  double Spacing[2];

  double RotationEncoderScale; 
  double RotationEncoderOffset; 

  vtkTrackedFrameList* TrackedFrameList; 

  //! Table used for storing algo results 
  vtkTable* ReportTable; 

  double ErrorMean; 
  double ErrorStdev; 

  // When was this data last updated
  vtkTimeStamp UpdateTime;  

}; 

#endif