#include "PlusMath.h"
#include "vtkMath.h"
#include "vtkRotationEncoderCalibAlgo.h"
#include "vtkBrachyTracker.h"
#include "vtkObjectFactory.h"
#include "vtkTrackedFrameList.h"
#include "vtkPoints.h"
#include "vtksys/SystemTools.hxx"
#include "vtkGnuplotExecuter.h"
#include "vtkHTMLGenerator.h"
#include "vtkDoubleArray.h"
#include "vtkVariantArray.h"

//----------------------------------------------------------------------------
vtkCxxRevisionMacro(vtkRotationEncoderCalibAlgo, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkRotationEncoderCalibAlgo); 

//----------------------------------------------------------------------------
vtkRotationEncoderCalibAlgo::vtkRotationEncoderCalibAlgo()
{
  this->TrackedFrameList = NULL; 
  this->SetSpacing(0,0); 
  this->ReportTable = NULL; 
  this->ErrorMean = 0.0; 
  this->ErrorStdev = 0.0; 
  this->RotationEncoderScale = 0.0; 
  this->RotationEncoderOffset = 0.0; 
}

//----------------------------------------------------------------------------
vtkRotationEncoderCalibAlgo::~vtkRotationEncoderCalibAlgo()
{
  this->SetTrackedFrameList(NULL); 
  this->SetReportTable(NULL); 
}

//----------------------------------------------------------------------------
void vtkRotationEncoderCalibAlgo::PrintSelf(ostream& os, vtkIndent indent)
{
  os << std::endl;
  this->Superclass::PrintSelf(os,indent);
  os << indent << "Update time: " << UpdateTime.GetMTime() << std::endl; 
  os << indent << "Rotation encoder scale: " << this->RotationEncoderScale << std::endl;
  os << indent << "Rotation encoder offset: " << this->RotationEncoderOffset << std::endl;
  os << indent << "Calibration error: mean=" << this->ErrorMean << "  stdev=" << this->ErrorStdev << std::endl;
  os << indent << "Spacing: " << this->Spacing[0] << "  " << this->Spacing[1] << std::endl;

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
void vtkRotationEncoderCalibAlgo::SetInputs(vtkTrackedFrameList* trackedFrameList, double spacing[2])
{
  LOG_TRACE("vtkRotationEncoderCalibAlgo::SetInput"); 
  this->SetTrackedFrameList(trackedFrameList); 
  this->SetSpacing(spacing); 
}


//----------------------------------------------------------------------------
PlusStatus vtkRotationEncoderCalibAlgo::GetRotationEncoderScale( double &rotationEncoderScale )
{
  LOG_TRACE("vtkRotationEncoderCalibAlgo::GetRotationEncoderScale"); 
  // Update calibration result 
  PlusStatus status = this->Update(); 

  rotationEncoderScale = this->RotationEncoderScale; 
  return status; 
}

//----------------------------------------------------------------------------
PlusStatus vtkRotationEncoderCalibAlgo::GetRotationEncoderOffset( double &rotationEncoderOffset )
{
  LOG_TRACE("vtkRotationEncoderCalibAlgo::rotationEncoderOffset"); 
  // Update calibration result 
  PlusStatus status = this->Update(); 

  rotationEncoderOffset = this->RotationEncoderOffset; 
  return status; 
}


//----------------------------------------------------------------------------
PlusStatus vtkRotationEncoderCalibAlgo::GetError(double &mean, double &stdev)
{
  LOG_TRACE("vtkRotationEncoderCalibAlgo::GetError"); 
  // Update calibration result 
  PlusStatus status = this->Update(); 

  mean = this->ErrorMean; 
  stdev = this->ErrorStdev; 

  return status; 
}

//----------------------------------------------------------------------------
PlusStatus vtkRotationEncoderCalibAlgo::Update()
{
  LOG_TRACE("vtkRotationEncoderCalibAlgo::Update"); 

  if ( this->GetMTime() < this->UpdateTime.GetMTime() )
  {
    LOG_DEBUG("Rotation encoder calibration result is up-to-date!"); 
    return PLUS_SUCCESS; 
  }

  // Construct linear equations Ax = b, where A is a matrix with m rows and 
  // n columns, b is an m-vector. 
  std::vector<vnl_vector<double>> aMatrix;
  std::vector<double> bVector; 

  // Construct linear equation for rotation encoder calibration
  if ( this->ConstructLinearEquationForCalibration(aMatrix, bVector) != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to contruct linear equations for rotation encoder calibration!"); 
    return PLUS_FAIL; 
  }

  if ( aMatrix.size() == 0 || bVector.size() == 0)
  {
    LOG_ERROR("Rotation encoder calibration failed, no data found!"); 
    return PLUS_FAIL; 
  }

  vnl_vector<double> rotationEncoderCalibResult(2, 0); 
  if ( PlusMath::LSQRMinimize(aMatrix, bVector, rotationEncoderCalibResult, &this->ErrorMean, &this->ErrorStdev) != PLUS_SUCCESS)
  {
    LOG_WARNING("Failed to run LSQRMinimize!"); 
    return PLUS_FAIL; 
  }

  if ( rotationEncoderCalibResult.empty() || rotationEncoderCalibResult.size() < 2 )
  {
    LOG_ERROR("Unable to calibrate rotation encoder! Minimizer returned empty result."); 
    return PLUS_FAIL; 
  }

  // TODO: remove sign from scale  => we need to revise the equation instead 
  // Set rotation encoder scale and offset
  this->RotationEncoderScale = -rotationEncoderCalibResult[0];
  this->RotationEncoderOffset = rotationEncoderCalibResult[1]; 
   
  this->UpdateReportTable(aMatrix, bVector, rotationEncoderCalibResult); 

  this->UpdateTime.Modified(); 

  return PLUS_SUCCESS; 
}


//----------------------------------------------------------------------------
PlusStatus vtkRotationEncoderCalibAlgo::ConstructLinearEquationForCalibration( std::vector<vnl_vector<double>> &aMatrix, std::vector<double> &bVector)
{
  LOG_TRACE("vtkRotationEncoderCalibAlgo::ConstructLinearEquationForCalibration"); 
  aMatrix.clear(); 
  bVector.clear(); 

  if ( this->TrackedFrameList == NULL )
  {
    LOG_ERROR("Failed to construct linear equation for rotation encoder calibration - tracked frame list is NULL!"); 
    return PLUS_FAIL; 
  }

  std::string defaultFrameTransformName = this->TrackedFrameList->GetDefaultFrameTransformName();

  for ( int frame = 0; frame < this->TrackedFrameList->GetNumberOfTrackedFrames(); ++frame )
  {
    TrackedFrame* trackedFrame = this->TrackedFrameList->GetTrackedFrame(frame); 
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

    double probePos(0), probeRot(0), templatePos(0); 
    if ( vtkBrachyTracker::GetStepperEncoderValues( trackedFrame, probePos, probeRot, templatePos, defaultFrameTransformName.c_str() ) != PLUS_SUCCESS )
    {
      LOG_WARNING("Probe rotation axis calibration: Unable to get probe rotation from tracked frame info for frame #" << frame); 
      continue; 
    }

    // Wire #1 coordinate in mm 
    double w1xmm = fiduacialPointsCoordinatePx->GetPoint(0)[0] * this->Spacing[0]; 
    double w1ymm = fiduacialPointsCoordinatePx->GetPoint(0)[1] * this->Spacing[1]; 

    // Wire #3 coordinate in mm 
    double w3xmm = fiduacialPointsCoordinatePx->GetPoint(2)[0] * this->Spacing[0]; 
    double w3ymm = fiduacialPointsCoordinatePx->GetPoint(2)[1] * this->Spacing[1]; 

    // Wire #4 coordinate in mm 
    double w4xmm = fiduacialPointsCoordinatePx->GetPoint(3)[0] * this->Spacing[0]; 
    double w4ymm = fiduacialPointsCoordinatePx->GetPoint(3)[1] * this->Spacing[1]; 

    // Wire #6 coordinate in mm 
    double w6xmm = fiduacialPointsCoordinatePx->GetPoint(5)[0] * this->Spacing[0]; 
    double w6ymm = fiduacialPointsCoordinatePx->GetPoint(5)[1] * this->Spacing[1]; 

    double b1 = vtkMath::DegreesFromRadians(atan2( (w3ymm - w1ymm), (w1xmm - w3xmm) )); 
    bVector.push_back(b1); 

    double b2 = vtkMath::DegreesFromRadians(atan2( (w6ymm - w4ymm), (w4xmm - w6xmm) )); 
    bVector.push_back(b2); 

    vnl_vector<double> a1(2,1);
    a1.put(0,probeRot); 
    aMatrix.push_back(a1); 

    vnl_vector<double> a2(2,1);
    a2.put(0,probeRot); 
    aMatrix.push_back(a2); 
  } // end of frames 

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus vtkRotationEncoderCalibAlgo::UpdateReportTable(const std::vector<vnl_vector<double>> &aMatrix, 
                                                          const std::vector<double> &bVector, 
                                                          const vnl_vector<double> &resultVector)
{
  LOG_TRACE("vtkRotationEncoderCalibAlgo::UpdateReportTable"); 

  // Clear table before update
  this->SetReportTable(NULL); 

  if ( this->ReportTable == NULL )
  {
    this->AddNewColumnToReportTable("ProbeRotationAngle"); 
    this->AddNewColumnToReportTable("ComputedAngle"); 
    this->AddNewColumnToReportTable("CompensatedAngle"); 
  }

  const double scale = resultVector[0]; 
  const double offset = resultVector[1]; 

  const int numberOfAxes(2); 

  for( int row = 0; row < bVector.size(); ++row )
  {
    vtkSmartPointer<vtkVariantArray> tableRow = vtkSmartPointer<vtkVariantArray>::New(); 

    tableRow->InsertNextValue(aMatrix[row].get(0));  // ProbeRotationAngle
    tableRow->InsertNextValue(bVector[row]);  // ComputedAngle
    tableRow->InsertNextValue(offset + aMatrix[row].get(0) * scale);  // CompensatedAngle

    if ( tableRow->GetNumberOfTuples() == this->ReportTable->GetNumberOfColumns() )
    {
      this->ReportTable->InsertNextRow(tableRow); 
    }
    else
    {
      LOG_WARNING("Unable to insert new row to rotation encoder calibration report table, number of columns are different (" 
        << tableRow->GetNumberOfTuples() << " vs. " << this->ReportTable->GetNumberOfColumns() << ")."); 
    }
  }

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus vtkRotationEncoderCalibAlgo::AddNewColumnToReportTable( const char* columnName )
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
PlusStatus vtkRotationEncoderCalibAlgo::GenerateReport( vtkHTMLGenerator* htmlReport, vtkGnuplotExecuter* plotter, const char* gnuplotScriptsFolder)
{
  LOG_TRACE("vtkRotationEncoderCalibAlgo::GenerateReport"); 

  // Update result before report generation 
  if ( this->Update() != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to generate report - rotation encoder calibration failed!"); 
    return PLUS_FAIL;
  }

  if ( htmlReport == NULL || plotter == NULL )
  {
    LOG_ERROR("Caller should define HTML report generator and gnuplot plotter before report generation!"); 
    return PLUS_FAIL; 
  }

  // Check gnuplot scripts 
  std::string plotRotationEncoderCalculationErrorScript = gnuplotScriptsFolder + std::string("/PlotProbeRotationEncoderCalibrationError.gnu"); 
  if ( !vtksys::SystemTools::FileExists( plotRotationEncoderCalculationErrorScript.c_str(), true) )
  {
    LOG_ERROR("Unable to find gnuplot script at: " << plotRotationEncoderCalculationErrorScript); 
    return PLUS_FAIL; 
  }

  // Generate report files from table 
  std::string reportFile = std::string(vtkPlusConfig::GetInstance()->GetOutputDirectory()) + std::string("/RotationEncoderCalibrationReport.txt");
  if ( vtkGnuplotExecuter::DumpTableToFileInGnuplotFormat( this->ReportTable, reportFile.c_str()) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to dump rotation encoder calibration report table to " << reportFile );
    return PLUS_FAIL; 
  }

  // Make sure the report file is there
  if ( !vtksys::SystemTools::FileExists( reportFile.c_str(), true) )
  {
    LOG_ERROR("Unable to find rotation encoder calibration report file at: " << reportFile); 
    return PLUS_FAIL; 
  }

  std::string title = "Rotation Encoder Calibration Analysis"; 
  std::string scriptOutputFilePrefix = "PlotProbeRotationEncoderCalibrationError"; 

  htmlReport->AddText(title.c_str(), vtkHTMLGenerator::H1); 

  std::ostringstream report; 
  report << "Probe rotation encoder scale: " << this->RotationEncoderScale << "</br>" ; 
  report << "Probe rotation encoder offset: " << this->RotationEncoderOffset << "</br>" ; 
  htmlReport->AddParagraph(report.str().c_str()); 

  plotter->ClearArguments(); 
  plotter->AddArgument("-e");
  std::ostringstream rotationEncoderCalibError; 
  rotationEncoderCalibError << "f='" << reportFile << "'; o='" << scriptOutputFilePrefix << "';" << std::ends; 
  plotter->AddArgument(rotationEncoderCalibError.str().c_str()); 
  plotter->AddArgument(plotRotationEncoderCalculationErrorScript.c_str());  
  if ( plotter->Execute() != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to run gnuplot executer!"); 
    return PLUS_FAIL; 
  }
  plotter->ClearArguments(); 

  std::ostringstream imageSource, imageAlt; 
  imageSource << scriptOutputFilePrefix << ".jpg" << std::ends; 
  imageAlt << "Probe rotation encoder calibration error" << std::ends; 

  htmlReport->AddImage(imageSource.str().c_str(), imageAlt.str().c_str()); 

  htmlReport->AddHorizontalLine(); 

  return PLUS_SUCCESS; 
}
