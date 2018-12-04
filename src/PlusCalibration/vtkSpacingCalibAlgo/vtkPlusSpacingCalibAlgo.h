/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusSpacingCalibAlgo_h
#define __vtkPlusSpacingCalibAlgo_h

#include "PlusConfigure.h"
#include "vtkPlusCalibrationExport.h"

#include "vtkObject.h"
#include "vtkIGSIOTrackedFrameList.h"
#include "vtkTable.h"
#include "PlusFidPatternRecognitionCommon.h"

class vtkPlusHTMLGenerator;

/*!
  \class vtkPlusSpacingCalibAlgo
  \brief Calculates ultrasound image spacing from phantom definition file
  \ingroup PlusLibCalibrationAlgorithm
*/
class vtkPlusCalibrationExport vtkPlusSpacingCalibAlgo : public vtkObject
{
public:
  static vtkPlusSpacingCalibAlgo* New();
  vtkTypeMacro(vtkPlusSpacingCalibAlgo , vtkObject);
  virtual void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

  /*!
    Add generated html report from spacing calibration to the existing html report
    htmlReport and plotter arguments has to be defined by the caller function
  */
  virtual PlusStatus GenerateReport(vtkPlusHTMLGenerator* htmlReport);

  /*!
    Set inputs:
    \param trackedFrameList tracked frames with segmentation results
    \param nWires phantom definition structure
    \sa NWire(), vtkIGSIOTrackedFrameList()
  */
  virtual void SetInputs(vtkIGSIOTrackedFrameList* trackedFrameList, const std::vector<PlusNWire>& nWires);

  /*! Report table used for storing algorithm results */
  vtkGetObjectMacro(ReportTable, vtkTable);

  /*! Get the image spacing (mm/pixel; spacing[0]: lateral axis, spacing[1]: axial axis) */
  virtual PlusStatus GetSpacing(double spacing[2]);

  /*! Get the spacing calibration error */
  virtual PlusStatus GetError(double& mean, double& stdev);

protected:
  vtkPlusSpacingCalibAlgo();
  virtual ~vtkPlusSpacingCalibAlgo();

  /*! Bring this algorithm's outputs up-to-date. */
  virtual PlusStatus Update();

  /*! Construct linear equation for spacing calibration */
  virtual PlusStatus ConstructLinearEquationForCalibration(std::vector<vnl_vector<double> >& aMatrix, std::vector<double>& bVector);

  /*! Add new column to the report table */
  PlusStatus AddNewColumnToReportTable(const char* columnName);

  /*! Update spacing calibration error report table */
  virtual PlusStatus UpdateReportTable(
    const std::vector<vnl_vector<double> >& aMatrix,
    const std::vector<double>& bVector,
    const vnl_vector<double>& resultVector);

  /*! Set tracked frame list */
  vtkSetObjectMacro(TrackedFrameList, vtkIGSIOTrackedFrameList);
  /*! Get tracked frame list */
  vtkGetObjectMacro(TrackedFrameList, vtkIGSIOTrackedFrameList);

  /*! Report table used for storing algorithm results */
  vtkSetObjectMacro(ReportTable, vtkTable);

  /*! Set image spacing (mm/pixel)*/
  vtkSetVector2Macro(Spacing, double);

  /*! Image image spacing (mm/pixel; spacing[0]: lateral axis, spacing[1]: axial axis) */
  double Spacing[2];

  /*! Tracked frame list with segmentation results */
  vtkIGSIOTrackedFrameList* TrackedFrameList;

  /*! Phantom definition structure */
  std::vector<PlusNWire> NWires;

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