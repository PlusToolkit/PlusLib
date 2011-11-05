/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#include "PlusMath.h"
#include "vtkSpacingCalibAlgo.h"
#include "vtkObjectFactory.h"
#include "vtkTrackedFrameList.h"
#include "vtkPoints.h"
#include "vtksys/SystemTools.hxx"
#include "vtkGnuplotExecuter.h"
#include "vtkHTMLGenerator.h"
#include "vtkDoubleArray.h"
#include "vtkVariantArray.h"

//----------------------------------------------------------------------------
vtkCxxRevisionMacro(vtkSpacingCalibAlgo, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkSpacingCalibAlgo); 

//----------------------------------------------------------------------------
vtkSpacingCalibAlgo::vtkSpacingCalibAlgo()
{
  this->TrackedFrameList = NULL; 
  this->SetSpacing(0,0); 
  this->ReportTable = NULL; 
  this->ErrorMean = 0.0; 
  this->ErrorStdev = 0.0; 
}

//----------------------------------------------------------------------------
vtkSpacingCalibAlgo::~vtkSpacingCalibAlgo()
{
  this->SetTrackedFrameList(NULL); 
  this->SetReportTable(NULL); 
}

//----------------------------------------------------------------------------
void vtkSpacingCalibAlgo::PrintSelf(ostream& os, vtkIndent indent)
{
  os << std::endl;
  this->Superclass::PrintSelf(os,indent);
  os << indent << "Update time: " << UpdateTime.GetMTime() << std::endl; 
  os << indent << "Spacing: " << this->Spacing[0] << "  " << this->Spacing[1] << std::endl;
  os << indent << "Calibration error: mean=" << this->ErrorMean << "  stdev=" << this->ErrorStdev << std::endl;

  if ( this->TrackedFrameList != NULL )
  {
    os << indent << "TrackedFrameList:" << std::endl;
    this->TrackedFrameList->PrintSelf(os, indent); 
  }

  if ( this->ReportTable != NULL )
  {
    os << indent << "ReportTable:" << std::endl;
    this->ReportTable->PrintSelf(os, indent); 
  }
}


//----------------------------------------------------------------------------
void vtkSpacingCalibAlgo::SetInputs(vtkTrackedFrameList* trackedFrameList, const std::vector<NWire>& nWires)
{
  LOG_TRACE("vtkSpacingCalibAlgo::SetInput"); 
  this->SetTrackedFrameList(trackedFrameList); 
  this->NWires = nWires; 
  this->Modified(); 
}


//----------------------------------------------------------------------------
PlusStatus vtkSpacingCalibAlgo::GetSpacing( double spacing[2] )
{
  LOG_TRACE("vtkSpacingCalibAlgo::GetSpacing"); 
  // Update calibration result 
  PlusStatus status = this->Update(); 

  spacing[0] = this->Spacing[0]; 
  spacing[1] = this->Spacing[1]; 

  return status; 
}

//----------------------------------------------------------------------------
PlusStatus vtkSpacingCalibAlgo::GetError(double &mean, double &stdev)
{
  LOG_TRACE("vtkSpacingCalibAlgo::GetError"); 
  // Update calibration result 
  PlusStatus status = this->Update(); 

  mean = this->ErrorMean; 
  stdev = this->ErrorStdev; 

  return status; 
}

//----------------------------------------------------------------------------
PlusStatus vtkSpacingCalibAlgo::Update()
{
  LOG_TRACE("vtkSpacingCalibAlgo::Update"); 

  if ( this->GetMTime() < this->UpdateTime.GetMTime() )
  {
    LOG_DEBUG("Spacing calibration result is up-to-date!"); 
    return PLUS_SUCCESS; 
  }

  // Construct linear equations Ax = b, where A is a matrix with m rows and 
  // n columns, b is an m-vector. 
  std::vector<vnl_vector<double>> aMatrix;
  std::vector<double> bVector; 

  // Construct linear equation for spacing calibration
  if ( this->ConstructLinearEquationForCalibration(aMatrix, bVector) != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to contruct linear equations for spacing calibration!"); 
    return PLUS_FAIL; 
  }

  if ( aMatrix.size() == 0 || bVector.size() == 0)
  {
    LOG_ERROR("Spacing calibration failed, no data found!"); 
    return PLUS_FAIL; 
  }

  vnl_vector<double> scalingCalibResult(2, 0); // [sx, sy]
  if ( PlusMath::LSQRMinimize(aMatrix, bVector, scalingCalibResult, &this->ErrorMean, &this->ErrorStdev) != PLUS_SUCCESS)
  {
    LOG_WARNING("Failed to run LSQRMinimize!"); 
    return PLUS_FAIL; 
  }

  if ( scalingCalibResult.empty() || scalingCalibResult.size() < 2 )
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
PlusStatus vtkSpacingCalibAlgo::ConstructLinearEquationForCalibration( std::vector<vnl_vector<double>> &aMatrix, std::vector<double> &bVector)
{
  LOG_TRACE("vtkSpacingCalibAlgo::ConstructLinearEquationForCalibration"); 
  aMatrix.clear(); 
  bVector.clear(); 

  if ( this->TrackedFrameList == NULL )
  {
    LOG_ERROR("Failed to construct linear equation for spacing calibration - tracked frame list is NULL!"); 
    return PLUS_FAIL; 
  }

  // Check the number of 
  if ( this->NWires.size() < 2 )
  {
    LOG_ERROR("Failed to construct linear equation for spacing calibration - phantom wire definition is not correct!"); 
    return PLUS_FAIL; 
  }

  // Compute wire distance from phantom definition 
  const double distanceW1ToW3inMm = fabs(this->NWires[0].Wires[0].EndPointFront[0] - this->NWires[0].Wires[2].EndPointFront[0]); // horizontal distance
  const double distanceW3ToW6inMm = fabs(this->NWires[0].Wires[2].EndPointFront[1] - this->NWires[1].Wires[2].EndPointFront[1]); // vertical  distance 
  
  for ( int frame = 0; frame < this->GetTrackedFrameList()->GetNumberOfTrackedFrames(); ++frame )
  {
    TrackedFrame* trackedFrame = this->GetTrackedFrameList()->GetTrackedFrame(frame); 
    if ( trackedFrame == NULL ) 
    {
      LOG_ERROR("Unable to get tracked frame from the list - tracked frame is NULL (position in the list: " << frame << ")!"); 
      continue; 
    }

    vtkPoints* fiduacialPointsCoordinatePx = trackedFrame->GetFiducialPointsCoordinatePx(); 
    if ( fiduacialPointsCoordinatePx == NULL )
    {
      LOG_ERROR("Unable to get segmented fiducial points from tracked frame - FiducialPointsCoordinatePx is NULL, frame is not yet segmented (position in the list: " << frame << ")!" ); 
      continue; 
    }

    if ( fiduacialPointsCoordinatePx->GetNumberOfPoints() == 0 )
    {
      LOG_DEBUG("Unable to get segmented fiducial points from tracked frame - couldn't segment image (position in the list: " << frame << ")!" ); 
      continue; 
    }

    double w1px[3] = {0}; 
    fiduacialPointsCoordinatePx->GetPoint(0, w1px); 

    double w3px[3] = {0}; 
    fiduacialPointsCoordinatePx->GetPoint(2, w3px); 

    double w6px[3] = {0}; 
    fiduacialPointsCoordinatePx->GetPoint(5, w6px); 

    // Compute distance between wire #1 and #3 
    double xDistanceW1ToW3Px = fabs(w1px[0] - w3px[0]); 
    double yDistanceW1ToW3Px = fabs(w1px[1] - w3px[1]); 

    // Populate the sparse matrix with squared distances in pixel 
    vnl_vector<double> scaleFactorW1ToW3(2,0); 
    scaleFactorW1ToW3.put(0, pow(xDistanceW1ToW3Px, 2));
    scaleFactorW1ToW3.put(1, pow(yDistanceW1ToW3Px, 2));
    aMatrix.push_back(scaleFactorW1ToW3); 

    // Add the squared distance in mm 
    bVector.push_back(pow(distanceW1ToW3inMm, 2));

    // Compute distance between wire #3 and #6 
    double xDistanceW3ToW6Px = fabs(w3px[0] - w6px[0]); 
    double yDistanceW3ToW6Px = fabs(w3px[1] - w6px[1]); 

    // Populate the sparse matrix with squared distances in pixel 
    vnl_vector<double> scaleFactorW3ToW6(2,0); 
    scaleFactorW3ToW6.put(0, pow(xDistanceW3ToW6Px, 2));
    scaleFactorW3ToW6.put(1, pow(yDistanceW3ToW6Px, 2));
    aMatrix.push_back(scaleFactorW3ToW6); 

    // Add the squared distance in mm 
    bVector.push_back(pow(distanceW3ToW6inMm, 2));

  } // end of frames 

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus vtkSpacingCalibAlgo::UpdateReportTable(const std::vector<vnl_vector<double>> &aMatrix, 
                                                  const std::vector<double> &bVector, 
                                                  const vnl_vector<double> &resultVector)
{
  LOG_TRACE("vtkSpacingCalibAlgo::UpdateReportTable"); 

  // Clear table before update
  this->SetReportTable(NULL); 

  if ( this->ReportTable == NULL )
  {
    this->AddNewColumnToReportTable("Computed Distance - X (mm)"); 
    this->AddNewColumnToReportTable("Measured Distance - X (mm)"); 
    this->AddNewColumnToReportTable("Computed Distance - Y (mm)"); 
    this->AddNewColumnToReportTable("Measured Distance - Y (mm)"); 
  }

  const double sX = resultVector[0]; 
  const double sY = resultVector[1]; 

  const int numberOfAxes(2); 

  for( int row = 0; row < bVector.size(); row = row + numberOfAxes)
  {
    vtkSmartPointer<vtkVariantArray> tableRow = vtkSmartPointer<vtkVariantArray>::New(); 

    tableRow->InsertNextValue(sqrt( aMatrix[row].get(0) * sX + aMatrix[row].get(1) * sY ));  // Computed Distance - X (mm)
    tableRow->InsertNextValue(sqrt( bVector[row]));  // Measured Distance - X (mm)
    tableRow->InsertNextValue(sqrt( aMatrix[row + 1].get(0) * sX + aMatrix[row + 1].get(1) * sY ));  // Computed Distance - Y (mm)
    tableRow->InsertNextValue(sqrt( bVector[row + 1]));  // Measured Distance - Y (mm)

    if ( tableRow->GetNumberOfTuples() == this->ReportTable->GetNumberOfColumns() )
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
PlusStatus vtkSpacingCalibAlgo::AddNewColumnToReportTable( const char* columnName )
{
  if ( this->ReportTable == NULL )
  {
    vtkSmartPointer<vtkTable> table = vtkSmartPointer<vtkTable>::New(); 
    this->SetReportTable(table); 
  }

  if ( columnName == NULL )
  {
    LOG_ERROR("Failed to add new column to table - column name is NULL!"); 
    return PLUS_FAIL; 
  }

  if ( this->ReportTable->GetColumnByName(columnName) != NULL )
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
PlusStatus vtkSpacingCalibAlgo::GenerateReport( vtkHTMLGenerator* htmlReport, vtkGnuplotExecuter* plotter)
{
  LOG_TRACE("vtkSpacingCalibAlgo::GenerateReport"); 

  // Update result before report generation 
  if ( this->Update() != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to generate report - spacing calibration failed!"); 
    return PLUS_FAIL;
  }

  if ( htmlReport == NULL || plotter == NULL )
  {
    LOG_ERROR("Caller should define HTML report generator and gnuplot plotter before report generation!"); 
    return PLUS_FAIL; 
  }

  const char* scriptsFolder = vtkPlusConfig::GetInstance()->GetScriptsDirectory();

  // Check gnuplot scripts 
  std::string plotSpacingCalculationErrorScript = scriptsFolder + std::string("/gnuplot/PlotSpacingCalculationErrorHistogram.gnu"); 
  if ( !vtksys::SystemTools::FileExists( plotSpacingCalculationErrorScript.c_str(), true) )
  {
    LOG_ERROR("Unable to find gnuplot script at: " << plotSpacingCalculationErrorScript); 
    return PLUS_FAIL; 
  }

  // Generate report files from table 
  std::string reportFile = std::string(vtkPlusConfig::GetInstance()->GetOutputDirectory()) + std::string("/SpacingCalibrationReport.txt");
  if ( vtkGnuplotExecuter::DumpTableToFileInGnuplotFormat( this->ReportTable, reportFile.c_str()) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to dump spacing calibration report table to " << reportFile );
    return PLUS_FAIL; 
  }

  // Make sure the report file is there
  if ( !vtksys::SystemTools::FileExists( reportFile.c_str(), true) )
  {
    LOG_ERROR("Unable to find spacing calibration report file at: " << reportFile); 
    return PLUS_FAIL; 
  }

  std::string title = "Spacing Calculation Analysis"; 
  std::string scriptOutputFilePrefix = "PlotSpacingCalculationErrorHistogram"; 

  htmlReport->AddText(title.c_str(), vtkHTMLGenerator::H1); 

  std::ostringstream report; 
  report << "Image spacing (mm/px): " << this->Spacing[0] << "     " << this->Spacing[1] << "</br>" ; 
  htmlReport->AddParagraph(report.str().c_str()); 

  plotter->ClearArguments(); 
  plotter->AddArgument("-e");
  std::ostringstream spacingCalculationError; 
  spacingCalculationError << "f='" << reportFile << "'; o='" << scriptOutputFilePrefix << "';" << std::ends; 
  plotter->AddArgument(spacingCalculationError.str().c_str()); 
  plotter->AddArgument(plotSpacingCalculationErrorScript.c_str());  
  if ( plotter->Execute() != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to run gnuplot executer!"); 
    return PLUS_FAIL; 
  }
  plotter->ClearArguments(); 

  std::ostringstream imageSource, imageAlt; 
  imageSource << scriptOutputFilePrefix << ".jpg" << std::ends; 
  imageAlt << "Spacing calculation error histogram" << std::ends; 

  htmlReport->AddImage(imageSource.str().c_str(), imageAlt.str().c_str()); 

  htmlReport->AddHorizontalLine(); 

  return PLUS_SUCCESS; 
}
