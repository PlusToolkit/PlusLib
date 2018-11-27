/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"

#include "PlusMath.h"
#include "PlusPlotter.h"
#include "vtkPlusSpacingCalibAlgo.h"
#include "vtkObjectFactory.h"
#include "vtkIGSIOTrackedFrameList.h"
#include "igsioTrackedFrame.h"
#include "vtkPoints.h"
#include "vtksys/SystemTools.hxx"
#include "vtkPlusHTMLGenerator.h"
#include "vtkDoubleArray.h"
#include "vtkVariantArray.h"

#include "vtkOrderStatistics.h"

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkPlusSpacingCalibAlgo);

//----------------------------------------------------------------------------
vtkPlusSpacingCalibAlgo::vtkPlusSpacingCalibAlgo()
{
  this->TrackedFrameList = NULL;
  this->SetSpacing(0, 0);
  this->ReportTable = NULL;
  this->ErrorMean = 0.0;
  this->ErrorStdev = 0.0;
}

//----------------------------------------------------------------------------
vtkPlusSpacingCalibAlgo::~vtkPlusSpacingCalibAlgo()
{
  this->SetTrackedFrameList(NULL);
  this->SetReportTable(NULL);
}

//----------------------------------------------------------------------------
void vtkPlusSpacingCalibAlgo::PrintSelf(ostream& os, vtkIndent indent)
{
  os << std::endl;
  this->Superclass::PrintSelf(os, indent);
  os << indent << "Update time: " << UpdateTime.GetMTime() << std::endl;
  os << indent << "Spacing: " << this->Spacing[0] << "  " << this->Spacing[1] << std::endl;
  os << indent << "Calibration error: mean=" << this->ErrorMean << "  stdev=" << this->ErrorStdev << std::endl;

  if (this->TrackedFrameList != NULL)
  {
    os << indent << "TrackedFrameList:" << std::endl;
    this->TrackedFrameList->PrintSelf(os, indent);
  }

  if (this->ReportTable != NULL)
  {
    os << indent << "ReportTable:" << std::endl;
    this->ReportTable->PrintSelf(os, indent);
  }
}


//----------------------------------------------------------------------------
void vtkPlusSpacingCalibAlgo::SetInputs(vtkIGSIOTrackedFrameList* trackedFrameList, const std::vector<PlusNWire>& nWires)
{
  LOG_TRACE("vtkPlusSpacingCalibAlgo::SetInputs");
  this->SetTrackedFrameList(trackedFrameList);
  this->NWires = nWires;
  this->Modified();
}


//----------------------------------------------------------------------------
PlusStatus vtkPlusSpacingCalibAlgo::GetSpacing(double spacing[2])
{
  LOG_TRACE("vtkPlusSpacingCalibAlgo::GetSpacing");
  // Update calibration result
  PlusStatus status = this->Update();

  spacing[0] = this->Spacing[0];
  spacing[1] = this->Spacing[1];

  return status;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusSpacingCalibAlgo::GetError(double& mean, double& stdev)
{
  LOG_TRACE("vtkPlusSpacingCalibAlgo::GetError");
  // Update calibration result
  PlusStatus status = this->Update();

  mean = this->ErrorMean;
  stdev = this->ErrorStdev;

  return status;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusSpacingCalibAlgo::Update()
{
  LOG_TRACE("vtkPlusSpacingCalibAlgo::Update");

  if (this->GetMTime() < this->UpdateTime.GetMTime())
  {
    LOG_DEBUG("Spacing calibration result is up-to-date!");
    return PLUS_SUCCESS;
  }

  // Check if TrackedFrameList is MF oriented BRIGHTNESS image
  if (vtkIGSIOTrackedFrameList::VerifyProperties(this->TrackedFrameList, US_IMG_ORIENT_MF, US_IMG_BRIGHTNESS) != PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to perform calibration - tracked frame list is invalid");
    return PLUS_FAIL;
  }

  // Construct linear equations Ax = b, where A is a matrix with m rows and
  // n columns, b is an m-vector.
  std::vector<vnl_vector<double> > aMatrix;
  std::vector<double> bVector;

  // Construct linear equation for spacing calibration
  if (this->ConstructLinearEquationForCalibration(aMatrix, bVector) != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to construct linear equations for spacing calibration!");
    return PLUS_FAIL;
  }

  if (aMatrix.size() == 0 || bVector.size() == 0)
  {
    LOG_ERROR("Spacing calibration failed, no data found!");
    return PLUS_FAIL;
  }

  vnl_vector<double> scalingCalibResult(2, 0);   // [sx, sy]
  if (PlusMath::LSQRMinimize(aMatrix, bVector, scalingCalibResult, &this->ErrorMean, &this->ErrorStdev) != PLUS_SUCCESS)
  {
    LOG_WARNING("Failed to run LSQRMinimize!");
    return PLUS_FAIL;
  }

  if (scalingCalibResult.empty() || scalingCalibResult.size() < 2)
  {
    LOG_ERROR("Unable to calibrate spacing! Minimizer returned empty result.");
    return PLUS_FAIL;
  }

  // Set spacing
  // don't use set macro, it changes the modification time of the algorithm
  this->Spacing[0] = sqrt(scalingCalibResult[0]);
  this->Spacing[1] = sqrt(scalingCalibResult[1]);

  this->UpdateReportTable(aMatrix, bVector, scalingCalibResult);

  this->UpdateTime.Modified();

  return PLUS_SUCCESS;
}


//----------------------------------------------------------------------------
PlusStatus vtkPlusSpacingCalibAlgo::ConstructLinearEquationForCalibration(std::vector<vnl_vector<double> >& aMatrix, std::vector<double>& bVector)
{
  LOG_TRACE("vtkPlusSpacingCalibAlgo::ConstructLinearEquationForCalibration");
  aMatrix.clear();
  bVector.clear();

  if (this->TrackedFrameList == NULL)
  {
    LOG_ERROR("Failed to construct linear equation for spacing calibration - tracked frame list is NULL!");
    return PLUS_FAIL;
  }

  // Check the number of
  if (this->NWires.size() < 2)
  {
    LOG_ERROR("Failed to construct linear equation for spacing calibration - phantom wire definition is not correct!");
    return PLUS_FAIL;
  }

  std::vector<double> verticalDistanceMm;
  std::vector<double> horizontalDistanceMm;
  for (unsigned int i = 0; i < this->NWires.size() - 1; ++i)
  {
    // Distance between the two parallel stem of the N fiducial
    double hd = fabs(this->NWires[i].GetWires()[0].EndPointFront[0] - this->NWires[i].GetWires()[2].EndPointFront[0]);   // horizontal distance
    horizontalDistanceMm.push_back(hd);

    // Distance between the neighboring N wire patterns
    double vd = fabs(this->NWires[i].GetWires()[2].EndPointFront[1] - this->NWires[i + 1].GetWires()[2].EndPointFront[1]);   // vertical  distance
    verticalDistanceMm.push_back(vd);
  }

  for (unsigned int frame = 0; frame < this->TrackedFrameList->GetNumberOfTrackedFrames(); ++frame)
  {
    igsioTrackedFrame* trackedFrame = this->TrackedFrameList->GetTrackedFrame(frame);
    if (trackedFrame == NULL)
    {
      LOG_ERROR("Unable to get tracked frame from the list - tracked frame is NULL (position in the list: " << frame << ")!");
      continue;
    }

    vtkPoints* fiduacialPointsCoordinatePx = trackedFrame->GetFiducialPointsCoordinatePx();
    if (fiduacialPointsCoordinatePx == NULL)
    {
      LOG_ERROR("Unable to get segmented fiducial points from tracked frame - FiducialPointsCoordinatePx is NULL, frame is not yet segmented (position in the list: " << frame << ")!");
      continue;
    }

    if (fiduacialPointsCoordinatePx->GetNumberOfPoints() == 0)
    {
      LOG_DEBUG("Unable to get segmented fiducial points from tracked frame - couldn't segment image (position in the list: " << frame << ")!");
      continue;
    }

    for (unsigned int w = 0; w < this->NWires.size() - 1; ++w)
    {
      double wRightPx[3] = {0};
      fiduacialPointsCoordinatePx->GetPoint(w * 3, wRightPx);

      double wLeftPx[3] = {0};
      fiduacialPointsCoordinatePx->GetPoint(w * 3 + 2, wLeftPx);

      double wTopPx[3] = {0};
      fiduacialPointsCoordinatePx->GetPoint((w + 1) * 3 + 2, wTopPx);

      double wBottomPx[3] = {0};
      fiduacialPointsCoordinatePx->GetPoint(w * 3 + 2, wBottomPx);

      // Compute horizontal distance
      double xHorizontalDistance = fabs(wRightPx[0] - wLeftPx[0]);
      double yHorizontalDistance = fabs(wRightPx[1] - wLeftPx[1]);

      // Populate the sparse matrix with squared distances in pixel
      vnl_vector<double> scaleFactorHorizontal(2, 0);
      scaleFactorHorizontal.put(0, pow(xHorizontalDistance, 2));
      scaleFactorHorizontal.put(1, pow(yHorizontalDistance, 2));
      aMatrix.push_back(scaleFactorHorizontal);

      // Add the squared distance in mm
      bVector.push_back(pow(horizontalDistanceMm[w], 2));

      // Compute vertical distance
      double xVerticalDistance = fabs(wBottomPx[0] - wTopPx[0]);
      double yVerticalDistance = fabs(wBottomPx[1] - wTopPx[1]);

      // Populate the sparse matrix with squared distances in pixel
      vnl_vector<double> scaleFactorVertical(2, 0);
      scaleFactorVertical.put(0, pow(xVerticalDistance, 2));
      scaleFactorVertical.put(1, pow(yVerticalDistance, 2));
      aMatrix.push_back(scaleFactorVertical);

      // Add the squared distance in mm
      bVector.push_back(pow(verticalDistanceMm[w], 2));

    }
  } // end of frames

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusSpacingCalibAlgo::UpdateReportTable(const std::vector<vnl_vector<double> >& aMatrix,
    const std::vector<double>& bVector,
    const vnl_vector<double>& resultVector)
{
  LOG_TRACE("vtkPlusSpacingCalibAlgo::UpdateReportTable");

  // Clear table before update
  this->SetReportTable(NULL);

  if (this->ReportTable == NULL)
  {
    this->AddNewColumnToReportTable("Computed-Measured Distance - X (mm)");
    this->AddNewColumnToReportTable("Measured Distance - X (mm)");
    this->AddNewColumnToReportTable("Computed-Measured Distance - Y (mm)");
    this->AddNewColumnToReportTable("Measured Distance - Y (mm)");
  }

  const double sX = resultVector[0];
  const double sY = resultVector[1];

  const int numberOfAxes(2);

  for (unsigned int row = 0; row < bVector.size(); row = row + numberOfAxes)
  {
    vtkSmartPointer<vtkVariantArray> tableRow = vtkSmartPointer<vtkVariantArray>::New();

    tableRow->InsertNextValue(sqrt(aMatrix[row].get(0) * sX + aMatrix[row].get(1) * sY) - sqrt(bVector[row]));           // Computed-Measured Distance - X (mm)
    tableRow->InsertNextValue(sqrt(bVector[row]));     // Measured Distance - X (mm)
    tableRow->InsertNextValue(sqrt(aMatrix[row + 1].get(0) * sX + aMatrix[row + 1].get(1) * sY) - sqrt(bVector[row + 1]));           // Computed-Measured Distance - Y (mm)
    tableRow->InsertNextValue(sqrt(bVector[row + 1]));     // Measured Distance - Y (mm)

    if (tableRow->GetNumberOfTuples() == this->ReportTable->GetNumberOfColumns())
    {
      this->ReportTable->InsertNextRow(tableRow);
    }
    else
    {
      LOG_WARNING("Unable to insert new row to translation axis calibration report table, number of columns are different ("
                  << tableRow->GetNumberOfTuples() << " vs. " << this->ReportTable->GetNumberOfColumns() << ").");
    }
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusSpacingCalibAlgo::AddNewColumnToReportTable(const char* columnName)
{
  if (this->ReportTable == NULL)
  {
    vtkSmartPointer<vtkTable> table = vtkSmartPointer<vtkTable>::New();
    this->SetReportTable(table);
  }

  if (columnName == NULL)
  {
    LOG_ERROR("Failed to add new column to table - column name is NULL!");
    return PLUS_FAIL;
  }

  if (this->ReportTable->GetColumnByName(columnName) != NULL)
  {
    LOG_WARNING("Column name " << columnName << " already exist!");
    return PLUS_FAIL;
  }

  // Create table column
  vtkSmartPointer<vtkDoubleArray> col = vtkSmartPointer<vtkDoubleArray>::New();
  col->SetName(columnName);
  this->ReportTable->AddColumn(col);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusSpacingCalibAlgo::GenerateReport(vtkPlusHTMLGenerator* htmlReport)
{
  LOG_TRACE("vtkPlusSpacingCalibAlgo::GenerateReport");

  if (htmlReport == NULL)
  {
    LOG_ERROR("vtkPlusSpacingCalibAlgo::GenerateReport failed: HTML report generator is invalid");
    return PLUS_FAIL;
  }

  // Update result before report generation
  if (this->Update() != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to generate report - spacing calibration failed!");
    return PLUS_FAIL;
  }

  htmlReport->AddText("Spacing Calculation Analysis", vtkPlusHTMLGenerator::H1);

  std::ostringstream report;
  report << "Image spacing (mm/px): " << this->Spacing[0] << "     " << this->Spacing[1] << "</br>" ;
  report << "Mean error (mm): " << this->ErrorMean << "</br>" ;
  report << "Standard deviation (mm): " << this->ErrorStdev << "</br>" ;
  htmlReport->AddParagraph(report.str().c_str());

  double valueRangeMin = -2.5;
  double valueRangeMax = 2.5;
  int numberOfBins = 41;
  int imageSize[2] = {800, 400};

  std::string outputImageFilename = htmlReport->AddImageAutoFilename("ErrorHistogramX.png", "X spacing calculation error histogram");
  PlusPlotter::WriteHistogramChartToFile("X spacing error histogram", *this->ReportTable, 0 /* "Computed-Measured Distance - X (mm)" */, valueRangeMin, valueRangeMax, numberOfBins, imageSize, outputImageFilename.c_str());

  htmlReport->AddParagraph("<p>");

  outputImageFilename = htmlReport->AddImageAutoFilename("ErrorHistogramY.png", "Y spacing calculation error histogram");
  PlusPlotter::WriteHistogramChartToFile("Y spacing error histogram", *this->ReportTable, 2 /* "Computed-Measured Distance - Y (mm)" */, valueRangeMin, valueRangeMax, numberOfBins, imageSize, outputImageFilename.c_str());

  htmlReport->AddHorizontalLine();

  return PLUS_SUCCESS;
}
