#include "PlusMath.h"
#include "vtkCenterOfRotationCalibAlgo.h"
#include "vtkObjectFactory.h"
#include "vtkTrackedFrameList.h"
#include "vtkPoints.h"
#include "vtksys/SystemTools.hxx"
#include "vtkGnuplotExecuter.h"
#include "vtkHTMLGenerator.h"
#include "vtkDoubleArray.h"
#include "vtkVariantArray.h"
#include "vtkBrachyTracker.h"

#include "vtkMeanShiftClustering.h"



//----------------------------------------------------------------------------
vtkCxxRevisionMacro(vtkCenterOfRotationCalibAlgo, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkCenterOfRotationCalibAlgo); 

//----------------------------------------------------------------------------
vtkCenterOfRotationCalibAlgo::vtkCenterOfRotationCalibAlgo()
{
  this->TrackedFrameList = NULL; 
  this->SetCenterOfRotationPx(0,0); 
  this->SetSpacing(0,0); 
  this->ReportTable = NULL; 
  this->ErrorMean = 0.0; 
  this->ErrorStdev = 0.0; 
}

//----------------------------------------------------------------------------
vtkCenterOfRotationCalibAlgo::~vtkCenterOfRotationCalibAlgo()
{
  this->SetTrackedFrameList(NULL); 
  this->SetReportTable(NULL); 
}

//----------------------------------------------------------------------------
void vtkCenterOfRotationCalibAlgo::PrintSelf(ostream& os, vtkIndent indent)
{
  os << std::endl;
  this->Superclass::PrintSelf(os,indent);
  os << indent << "Update time: " << UpdateTime.GetMTime() << std::endl; 
  os << indent << "Spacing: " << this->Spacing[0] << "  " << this->Spacing[1] << std::endl;
  os << indent << "Center of rotation (px): " << this->CenterOfRotationPx[0] << "  " << this->CenterOfRotationPx[1] << std::endl;
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
void vtkCenterOfRotationCalibAlgo::SetInputs(vtkTrackedFrameList* trackedFrameList, std::vector<int> &indices, double spacing[2])
{
  LOG_TRACE("vtkCenterOfRotationCalibAlgo::SetInput"); 
  this->SetTrackedFrameList(trackedFrameList); 
  this->SetSpacing(spacing);
  this->SetTrackedFrameListIndices(indices); 
}

//----------------------------------------------------------------------------
void vtkCenterOfRotationCalibAlgo::SetTrackedFrameListIndices( std::vector<int> &indices )
{
  this->TrackedFrameListIndices = indices; 
  this->Modified(); 
}


//----------------------------------------------------------------------------
PlusStatus vtkCenterOfRotationCalibAlgo::GetCenterOfRotationPx(double centerOfRotationPx[2])
{
  LOG_TRACE("vtkCenterOfRotationCalibAlgo::GetCenterOfRotationPx"); 
  // Update calibration result 
  PlusStatus status = this->Update(); 

  centerOfRotationPx[0] = this->CenterOfRotationPx[0]; 
  centerOfRotationPx[1] = this->CenterOfRotationPx[1]; 

  return status; 
}

//----------------------------------------------------------------------------
PlusStatus vtkCenterOfRotationCalibAlgo::GetError(double &mean, double &stdev)
{
  LOG_TRACE("vtkCenterOfRotationCalibAlgo::GetError"); 
  // Update calibration result 
  PlusStatus status = this->Update(); 

  mean = this->ErrorMean; 
  stdev = this->ErrorStdev; 

  return status; 
}

//----------------------------------------------------------------------------
PlusStatus vtkCenterOfRotationCalibAlgo::Update()
{
  LOG_TRACE("vtkCenterOfRotationCalibAlgo::Update"); 

  if ( this->GetMTime() < this->UpdateTime.GetMTime() )
  {
    LOG_DEBUG("Center of rotation calibration result is up-to-date!"); 
    return PLUS_SUCCESS; 
  }

  // Data containers
  std::vector<vnl_vector<double>> aMatrix;
  std::vector<double> bVector;

  if ( this->ConstructLinearEquationForCalibration(aMatrix, bVector) != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to contruct linear equation for center of rotation calibration algorithm!"); 
    return PLUS_FAIL; 
  }
  
  if ( aMatrix.size() == 0 || bVector.size() == 0 )
  {
    LOG_WARNING("Center of rotation calculation failed, no data found!"); 
    return PLUS_FAIL; 
  }

  // The rotation center in original image frame in px
  vnl_vector<double> centerOfRotationInPx(2,0);
  if ( PlusMath::LSQRMinimize(aMatrix, bVector, centerOfRotationInPx, &this->ErrorMean, &this->ErrorStdev) != PLUS_SUCCESS)
  {
    LOG_WARNING("Failed to run LSQRMinimize!"); 
    return PLUS_FAIL; 
  }

  if ( centerOfRotationInPx.empty() || centerOfRotationInPx.size() < 2 )
  {
    LOG_ERROR("Unable to calibrate center of rotation! Minimizer returned empty result."); 
    return PLUS_FAIL; 
  }

  // Set center of rotation - don't use set macro, it changes the modification time of the algorithm 
  this->CenterOfRotationPx[0] = centerOfRotationInPx[0] / this->Spacing[0]; 
  this->CenterOfRotationPx[1] = centerOfRotationInPx[1] / this->Spacing[1]; 

  this->UpdateReportTable(); 

  this->UpdateTime.Modified(); 

  return PLUS_SUCCESS; 

}


//----------------------------------------------------------------------------
PlusStatus vtkCenterOfRotationCalibAlgo::ConstructLinearEquationForCalibration( std::vector<vnl_vector<double>> &aMatrix, std::vector<double> &bVector)
{
  LOG_TRACE("vtkCenterOfRotationCalibAlgo::ConstructLinearEquationForCalibration"); 
  aMatrix.clear(); 
  bVector.clear(); 

  if ( this->TrackedFrameList == NULL )
  {
    LOG_ERROR("Failed to construct linear equation for center of rotation calibration - tracked frame list is NULL!"); 
    return PLUS_FAIL; 
  }

  const int numberOfFrames = this->TrackedFrameListIndices.size(); 

  if ( numberOfFrames < 30 )
  {
    LOG_WARNING("Center of rotation calculation failed - there is not enough data (" << numberOfFrames << " out of at least 30)!"); 
    return PLUS_FAIL; 
  }

  typedef std::vector<vtkSmartPointer<vtkPoints>> VectorPoints; 
  VectorPoints vectorOfWirePoints; 
  for ( int i = 0; i < numberOfFrames; ++i )
  {
    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New(); 
    points->SetDataTypeToDouble(); 
    vectorOfWirePoints.push_back(points); 
  }

  for ( int index = 0; index < numberOfFrames; ++index )
  {
    // Get the next frame index used for calibration 
    int frameNumber = this->TrackedFrameListIndices[index]; 
    
    // Get tracked frame from list 
    TrackedFrame* trackedFrame = this->TrackedFrameList->GetTrackedFrame(frameNumber); 

    if ( trackedFrame->GetFiducialPointsCoordinatePx() == NULL )
    {
      LOG_ERROR("Unable to get segmented fiducial points from tracked frame - FiducialPointsCoordinatePx is NULL, frame is not yet segmented (position in the list: " << frameNumber << ")!" ); 
      continue; 
    }

    // Every N fiducial has 3 points on the image: numberOfNFiducials = NumberOfPoints / 3 
    const int numberOfNFiduacials = trackedFrame->GetFiducialPointsCoordinatePx()->GetNumberOfPoints() / 3; 
    vectorOfWirePoints[index]->SetNumberOfPoints(numberOfNFiduacials*2); // Use only the two non-moving points of the N fiducial 
    int vectorID(0); // ID used for point position in vtkPoints for faster execution 
    for ( int i = 0; i < trackedFrame->GetFiducialPointsCoordinatePx()->GetNumberOfPoints(); ++i )
    {
      if ( ( (i+1) % 3 ) != 2 ) // wire #1,#3,#4,#6... => use only non moving points of the N-wire 
      {
        double* wireCoordinatePx = trackedFrame->GetFiducialPointsCoordinatePx()->GetPoint(i); 
        vectorOfWirePoints[index]->SetPoint(vectorID++, wireCoordinatePx[0], wireCoordinatePx[1], wireCoordinatePx[2]); 
      }
    }
  }

  for ( unsigned int i = 0; i <= vectorOfWirePoints.size() - 2; i++ )
  {
    for( unsigned int j = i + 1; j <= vectorOfWirePoints.size() - 1; j++ )
    {
      if ( vectorOfWirePoints[i]->GetNumberOfPoints() != vectorOfWirePoints[j]->GetNumberOfPoints() )
      {
        continue; 
      }

      for ( int point = 0; point < vectorOfWirePoints[i]->GetNumberOfPoints(); point++ )
      {
        // coordiates of the i-th element
        double Xi = vectorOfWirePoints[i]->GetPoint(point)[0] * this->Spacing[0]; 
        double Yi = vectorOfWirePoints[i]->GetPoint(point)[1] * this->Spacing[1]; 

        // coordiates of the j-th element
        double Xj = vectorOfWirePoints[j]->GetPoint(point)[0] * this->Spacing[0]; 
        double Yj = vectorOfWirePoints[j]->GetPoint(point)[1] * this->Spacing[1]; 

         // Populate the list of distance
        vnl_vector<double> rowOfDistance(2,0);
        rowOfDistance.put(0, Xi - Xj);
        rowOfDistance.put(1, Yi - Yj);
        aMatrix.push_back( rowOfDistance );

        // Populate the squared distance vector
        bVector.push_back( 0.5*( Xi*Xi + Yi*Yi - Xj*Xj - Yj*Yj ));
      }
    }
  }

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus vtkCenterOfRotationCalibAlgo::UpdateReportTable()
{
  LOG_TRACE("vtkCenterOfRotationCalibAlgo::UpdateReportTable"); 

  // Clear table before update
  this->SetReportTable(NULL); 

  if ( this->ReportTable == NULL )
  {
    this->AddNewColumnToReportTable("ProbePosition"); 
    this->AddNewColumnToReportTable("ProbeRotation"); 
    this->AddNewColumnToReportTable("TemplatePosition"); 
    this->AddNewColumnToReportTable("Wire#1Radius"); 
    this->AddNewColumnToReportTable("Wire#3Radius"); 
    this->AddNewColumnToReportTable("Wire#4Radius"); 
    this->AddNewColumnToReportTable("Wire#6Radius"); 
  }

  const double sX = this->Spacing[0]; 
  const double sY = this->Spacing[1]; 

  for ( unsigned int i = 0; i < this->TrackedFrameListIndices.size(); i++ )
  {
    const int frameNumber = this->TrackedFrameListIndices[i]; 
    TrackedFrame* frame = this->TrackedFrameList->GetTrackedFrame(frameNumber); 

    if ( frame->GetFiducialPointsCoordinatePx() == NULL
      || frame->GetFiducialPointsCoordinatePx()->GetNumberOfPoints() == 0 )
    {
      // This frame was not segmented
      continue; 
    }

    double probePos(0), probeRot(0), templatePos(0); 
    if ( !vtkBrachyTracker::GetStepperEncoderValues(frame, probePos, probeRot, templatePos) )
    {
      LOG_WARNING("Unable to get probe position from tracked frame info for frame #" << frameNumber); 
      continue; 
    }

    vtkSmartPointer<vtkVariantArray> tableRow = vtkSmartPointer<vtkVariantArray>::New(); 
    tableRow->InsertNextValue(probePos); 
    tableRow->InsertNextValue(probeRot); 
    tableRow->InsertNextValue(templatePos); 
    
    // TODO: it works only with double N phantom 
    // Compute radius from Wire #1, #3, #4, #6
    double w1x = frame->GetFiducialPointsCoordinatePx()->GetPoint(0)[0]; 
    double w1y = frame->GetFiducialPointsCoordinatePx()->GetPoint(0)[1]; 

    double w3x = frame->GetFiducialPointsCoordinatePx()->GetPoint(2)[0]; 
    double w3y = frame->GetFiducialPointsCoordinatePx()->GetPoint(2)[1]; 

    double w4x = frame->GetFiducialPointsCoordinatePx()->GetPoint(3)[0]; 
    double w4y = frame->GetFiducialPointsCoordinatePx()->GetPoint(3)[1]; 
    
    double w6x = frame->GetFiducialPointsCoordinatePx()->GetPoint(5)[0]; 
    double w6y = frame->GetFiducialPointsCoordinatePx()->GetPoint(5)[1]; 

    tableRow->InsertNextValue( sqrt( pow( (w1x - this->CenterOfRotationPx[0])*sX, 2) + pow((w1y - this->CenterOfRotationPx[1])*sY, 2) ) ); 
    tableRow->InsertNextValue( sqrt( pow( (w3x - this->CenterOfRotationPx[0])*sX, 2) + pow((w3y - this->CenterOfRotationPx[1])*sY, 2) ) ); 
    tableRow->InsertNextValue( sqrt( pow( (w4x - this->CenterOfRotationPx[0])*sX, 2) + pow((w4y - this->CenterOfRotationPx[1])*sY, 2) ) ); 
    tableRow->InsertNextValue( sqrt( pow( (w6x - this->CenterOfRotationPx[0])*sX, 2) + pow((w6y - this->CenterOfRotationPx[1])*sY, 2) ) ); 

    if ( tableRow->GetNumberOfTuples() == this->ReportTable->GetNumberOfColumns() )
    {
      this->ReportTable->InsertNextRow(tableRow); 
    }
    else
    {
      LOG_WARNING("Unable to insert new row to center of rotation error table, number of columns are different (" 
        << tableRow->GetNumberOfTuples() << " vs. " << this->ReportTable->GetNumberOfColumns() << ")."); 
    }

  }

   vtkGnuplotExecuter::DumpTableToFileInGnuplotFormat(ReportTable, "./RotationAxisCalibrationErrorReport.txt"); 

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus vtkCenterOfRotationCalibAlgo::AddNewColumnToReportTable( const char* columnName )
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
PlusStatus vtkCenterOfRotationCalibAlgo::GenerateReport( vtkHTMLGenerator* htmlReport, vtkGnuplotExecuter* plotter, const char* gnuplotScriptsFolder)
{
  LOG_TRACE("vtkCenterOfRotationCalibAlgo::GenerateReport"); 

  // Update result before report generation 
  if ( this->Update() != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to generate report - center of rotation axis calibration failed!"); 
    return PLUS_FAIL;
  }

  return vtkCenterOfRotationCalibAlgo::GenerateCenterOfRotationReport(htmlReport, plotter, gnuplotScriptsFolder, this->ReportTable, this->CenterOfRotationPx); 
}

//----------------------------------------------------------------------------
PlusStatus vtkCenterOfRotationCalibAlgo::GenerateCenterOfRotationReport( vtkHTMLGenerator* htmlReport, 
                                                                        vtkGnuplotExecuter* plotter, 
                                                                        const char* gnuplotScriptsFolder, 
                                                                        vtkTable* reportTable,
                                                                        double centerOfRotationPx[2])
{
  LOG_TRACE("vtkCenterOfRotationCalibAlgo::GenerateCenterOfRotationReport"); 

  if ( htmlReport == NULL || plotter == NULL )
  {
    LOG_ERROR("Caller should define HTML report generator and gnuplot plotter before report generation!"); 
    return PLUS_FAIL; 
  }

  if ( reportTable == NULL )
  {
    LOG_ERROR("Unable to generate report - input report table is NULL!"); 
    return PLUS_FAIL; 
  }

  if ( gnuplotScriptsFolder == NULL )
  {
    LOG_ERROR("Unable to generate report - gnuplot scripts folder is NULL!"); 
    return PLUS_FAIL; 
  }

  // Check gnuplot scripts 
  std::string plotCenterOfRotCalcErrorScript = gnuplotScriptsFolder + std::string("/PlotCenterOfRotationCalculationError.gnu"); 
  if ( !vtksys::SystemTools::FileExists( plotCenterOfRotCalcErrorScript.c_str(), true) )
  {
    LOG_ERROR("Unable to find gnuplot script at: " << plotCenterOfRotCalcErrorScript); 
    return PLUS_FAIL; 
  }

  // Generate report files from table 
  std::string reportFile = std::string(vtkPlusConfig::GetInstance()->GetOutputDirectory()) + std::string("/CenterOfRotationCalculationError.txt");
  if ( vtkGnuplotExecuter::DumpTableToFileInGnuplotFormat( reportTable, reportFile.c_str()) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to dump translation axis calibration report table to " << reportFile );
    return PLUS_FAIL; 
  }

  // Make sure the report file is there
  if ( !vtksys::SystemTools::FileExists( reportFile.c_str(), true) )
  {
    LOG_ERROR("Unable to find center of rotation calibration report file at: " << reportFile); 
    return PLUS_FAIL; 
  }

  std::string title = "Center of Rotation Calculation Analysis"; 
  std::string scriptOutputFilePrefix = "CenterOfRotationCalculationError"; 

  htmlReport->AddText(title.c_str(), vtkHTMLGenerator::H1); 

  std::ostringstream report; 
  report << "Center of rotation (px): " << centerOfRotationPx[0] << "     " << centerOfRotationPx[1] << "</br>" ; 
  htmlReport->AddParagraph(report.str().c_str()); 

  const int wires[4] = {1, 3, 4, 6}; 

  for ( int i = 0; i < 4; i++ )
  {
    std::ostringstream wireName; 
    wireName << "Wire #" << wires[i] << std::ends; 
    htmlReport->AddText(wireName.str().c_str(), vtkHTMLGenerator::H3); 
    plotter->ClearArguments(); 
    plotter->AddArgument("-e");
    std::ostringstream centerOfRotCalcError; 
    centerOfRotCalcError << "f='" << reportFile << "'; o='" << scriptOutputFilePrefix << "'; w=" << wires[i] << std::ends; 
    plotter->AddArgument(centerOfRotCalcError.str().c_str()); 
    plotter->AddArgument(plotCenterOfRotCalcErrorScript.c_str());  
    if ( plotter->Execute() != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to run gnuplot executer!"); 
      return PLUS_FAIL; 
    }
    plotter->ClearArguments(); 

    std::ostringstream imageSource; 
    std::ostringstream imageAlt; 
    imageSource << "w" << wires[i] << "_CenterOfRotationCalculationError.jpg" << std::ends; 
    imageAlt << "Center of rotation calculation error - wire #" << wires[i] << std::ends; 

    htmlReport->AddImage(imageSource.str().c_str(), imageAlt.str().c_str()); 
  }

  htmlReport->AddHorizontalLine(); 

  return PLUS_SUCCESS; 
}