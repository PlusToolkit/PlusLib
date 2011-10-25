/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __vtkSpacingCalibAlgo_h
#define __vtkSpacingCalibAlgo_h

#include "PlusConfigure.h"
#include "vtkObject.h"
#include "vtkTrackedFrameList.h"
#include "vtkTable.h"
#include "FidPatternRecognitionCommon.h"

class vtkHTMLGenerator; 
class vtkGnuplotExecuter; 

/*!
  \class vtkSpacingCalibAlgo 
  \brief Calculates ultrasound image spacing from phantom definition file 
  \ingroup PlusLibCalibrationAlgorithm
*/ 
class VTK_EXPORT vtkSpacingCalibAlgo : public vtkObject
{
public:

  static vtkSpacingCalibAlgo *New();
  vtkTypeRevisionMacro(vtkSpacingCalibAlgo , vtkObject);
  virtual void PrintSelf(ostream& os, vtkIndent indent); 

  /*!
    Add generated html report from spacing calibration to the existing html report
    htmlReport and plotter arguments has to be defined by the caller function 
  */
  virtual PlusStatus GenerateReport( vtkHTMLGenerator* htmlReport, vtkGnuplotExecuter* plotter, const char* gnuplotScriptsFolder); 

  /*!
    Set inputs: 
    \param trackedFrameList tracked frames with segmentation results 
    \param nWires phantom definition structure 
    \sa NWire(), vtkTrackedFrameList()
  */
  virtual void SetInputs( vtkTrackedFrameList* trackedFrameList, const std::vector<NWire>& nWires ); 

  /*! Report table used for storing algorithm results */
  vtkGetObjectMacro(ReportTable, vtkTable); 

  /*! Get the image spacing (mm/pixel; spacing[0]: lateral axis, spacing[1]: axial axis) */
  virtual PlusStatus GetSpacing( double spacing[2] ); 

  /*! Get the spacing calibration error */
  virtual PlusStatus GetError(double &mean, double &stdev); 

protected:
  vtkSpacingCalibAlgo();
  virtual ~vtkSpacingCalibAlgo(); 

  /*! Bring this algorithm's outputs up-to-date. */
  virtual PlusStatus Update();

  /*! Construct linear equation for spacing calibration */
  virtual PlusStatus ConstructLinearEquationForCalibration(std::vector<vnl_vector<double>> &aMatrix, std::vector<double> &bVector);

  /*! Add new column to the report table */
  PlusStatus AddNewColumnToReportTable( const char* columnName ); 

  /*! Update spacing calibration error report table */
  virtual PlusStatus UpdateReportTable(
    const std::vector<vnl_vector<double>> &aMatrix, 
    const std::vector<double> &bVector, 
    const vnl_vector<double> &resultVector); 

  /*! Set tracked frame list */
  vtkSetObjectMacro(TrackedFrameList, vtkTrackedFrameList); 
  /*! Get tracked frame list */
  vtkGetObjectMacro(TrackedFrameList, vtkTrackedFrameList); 

  /*! Report table used for storing algorithm results */
  vtkSetObjectMacro(ReportTable, vtkTable); 

  /*! Set image spacing (mm/pixel)*/
  vtkSetVector2Macro(Spacing, double); 

  /*! Image image spacing (mm/pixel; spacing[0]: lateral axis, spacing[1]: axial axis) */
  double Spacing[2];

  /*! Tracked frame list with segmentation results */
  vtkTrackedFrameList* TrackedFrameList; 

  /*! Phantom definition structure */
  std::vector<NWire> NWires; 

  /*! Table used for storing algo results */
  vtkTable* ReportTable; 

  /*! Calibration error mean */
  double ErrorMean; 
  
  /*! Calibration error standard deviation */
  double ErrorStdev; 

  /*! When the results were computed. The result is recomputed only if the inputs changed more recently than UpdateTime. */
  vtkTimeStamp UpdateTime;  

}; 

#endif