#ifndef __vtkSpacingCalibAlgo_h
#define __vtkSpacingCalibAlgo_h

#include "PlusConfigure.h"
#include "vtkObject.h"
#include "vtkTrackedFrameList.h"
#include "vtkTable.h"
#include "FidPatternRecognitionCommon.h"

class vtkHTMLGenerator; 
class vtkGnuplotExecuter; 

class VTK_EXPORT vtkSpacingCalibAlgo : public vtkObject
{
public:

  static vtkSpacingCalibAlgo *New();
  vtkTypeRevisionMacro(vtkSpacingCalibAlgo , vtkObject);
  virtual void PrintSelf(ostream& os, vtkIndent indent); 

  //! Add generated html report from spacing calibration to the existing html report
  // htmlReport and plotter arguments has to be defined by the caller function
  virtual PlusStatus GenerateReport( vtkHTMLGenerator* htmlReport, vtkGnuplotExecuter* plotter, const char* gnuplotScriptsFolder); 

  //! Set inputs: 
  // - TrackedFrameList with segmentation results 
  // - 
  virtual void SetInputs( vtkTrackedFrameList* trackedFrameList, const std::vector<NWire>& nWires ); 

  //! Report table used for storing algorithm results 
  vtkGetObjectMacro(ReportTable, vtkTable); 

  //! Get the image spacing
  // (x: lateral axis, y: axial axis)
  virtual PlusStatus GetSpacing( double spacing[2] ); 

  //! Get the spacing calibration error 
  virtual PlusStatus GetError(double &mean, double &stdev); 

protected:
  vtkSpacingCalibAlgo();
  virtual ~vtkSpacingCalibAlgo(); 

  //! Bring this algorithm's outputs up-to-date.
  virtual PlusStatus Update();

  //! Construct linear equation for spacing calibration
  virtual PlusStatus ConstructLinearEquationForCalibration(std::vector<vnl_vector<double>> &aMatrix, std::vector<double> &bVector);

  //! Description: 
  // Add new column to the report table 
  PlusStatus AddNewColumnToReportTable( const char* columnName ); 

  //! Description: 
  // Update spacing calibration error report table 
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

  vtkTrackedFrameList* TrackedFrameList; 

  std::vector<NWire> NWires; 

  //! Table used for storing algo results 
  vtkTable* ReportTable; 

  double ErrorMean; 
  double ErrorStdev; 

  // When was this data last updated
  vtkTimeStamp UpdateTime;  

}; 

#endif