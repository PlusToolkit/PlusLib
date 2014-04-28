/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#include "PlusMath.h"
#include "vtkCenterOfRotationCalibAlgo.h"
#include "vtkObjectFactory.h"
#include "vtkTrackedFrameList.h"
#include "TrackedFrame.h"
#include "vtkPoints.h"
#include "vtksys/SystemTools.hxx"
#include "vtkGnuplotExecuter.h"
#include "vtkHTMLGenerator.h"
#include "vtkDoubleArray.h"
#include "vtkVariantArray.h"

#ifdef PLUS_USE_BRACHY_TRACKER
  #include "vtkBrachyTracker.h"
#endif 

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
  LOG_TRACE("vtkCenterOfRotationCalibAlgo::SetInputs"); 
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

  // Check if TrackedFrameList is MF oriented BRIGHTNESS image
  if (vtkTrackedFrameList::VerifyProperties(this->TrackedFrameList, US_IMG_ORIENT_MF, US_IMG_BRIGHTNESS)!=PLUS_SUCCESS)
  {
    LOG_ERROR("Failed to perform calibration - tracked frame list is invalid"); 
    return PLUS_FAIL; 
  }
  
  // Data containers
  std::vector<vnl_vector<double> > aMatrix;
  std::vector<double> bVector;

  if ( this->ConstructLinearEquationForCalibration(aMatrix, bVector) != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to construct linear equation for center of rotation calibration algorithm!"); 
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
int vtkCenterOfRotationCalibAlgo::GetNumberOfNWirePatterns()
{
  int numberOfNWirePatterns = 0; 

  if ( this->TrackedFrameList == NULL )
  {
    LOG_WARNING("Unable to compute number of N wire patterns - tracked frame list is NULL!"); 
    return numberOfNWirePatterns; 
  }
  
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
      //LOG_DEBUG("Unable to get segmented fiducial points from tracked frame - couldn't segment image (position in the list: " << frame << ")!" ); 
      continue; 
    }

    // Make sure we have 3 points for each N wire pattern 
    if ( fiduacialPointsCoordinatePx->GetNumberOfPoints() % 3 != 0 )
    {
      LOG_WARNING("Unrecognized wire patterns for frame #" << frame); 
      continue; 
    }

    // Every N fiducial has 3 points on the image: numberOfNFiducials = NumberOfPoints / 3 
    int patterns = fiduacialPointsCoordinatePx->GetNumberOfPoints() / 3; 
    
    // Make sure all the frames has the same number of patterns
    if ( numberOfNWirePatterns > 0 && patterns != numberOfNWirePatterns )
    {
      LOG_ERROR( "Number of N-wire patterns different between frames!"); // this shouldn't happen
      return 0; 
    }

    numberOfNWirePatterns = patterns; 
  }

  return numberOfNWirePatterns; 
}

//----------------------------------------------------------------------------
PlusStatus vtkCenterOfRotationCalibAlgo::ConstructLinearEquationForCalibration( std::vector<vnl_vector<double> > &aMatrix, std::vector<double> &bVector)
{
  LOG_TRACE("vtkCenterOfRotationCalibAlgo::ConstructLinearEquationForCalibration"); 
  aMatrix.clear(); 
  bVector.clear(); 

  if ( this->TrackedFrameList == NULL )
  {
    LOG_ERROR("Failed to construct linear equation for center of rotation calibration - tracked frame list is NULL!"); 
    return PLUS_FAIL; 
  }

  const int numberOfNFiduacials = this->GetNumberOfNWirePatterns(); 

  //******************* Count the number of frames used for calibration *****************
  int numberOfFrames = 0; 
  for ( unsigned int index = 0; index < this->TrackedFrameListIndices.size(); ++index )
  {
    // Get the next frame index used for calibration 
    int frameNumber = this->TrackedFrameListIndices[index]; 
    
    // Get tracked frame from list 
    TrackedFrame* trackedFrame = this->TrackedFrameList->GetTrackedFrame(frameNumber); 

    // Skip frame if it was not segmented
    if ( trackedFrame->GetFiducialPointsCoordinatePx() == NULL )
    {
      continue; 
    }

    // Skip frame if the segmentation was not successful 
    if ( trackedFrame->GetFiducialPointsCoordinatePx()->GetNumberOfPoints() == 0 )
    {
      continue;
    }

    numberOfFrames++; 
  }

  if ( numberOfFrames < 30 )
  {
    LOG_WARNING("Center of rotation calculation failed - there is not enough data (" << numberOfFrames << " out of at least 30)!"); 
    return PLUS_FAIL; 
  }

  // Reserve memory for wire points 
  std::vector<vtkSmartPointer<vtkPoints> > vectorOfWirePoints; 
  vectorOfWirePoints.reserve(numberOfFrames); 

  for ( int i = 0; i < this->TrackedFrameListIndices.size(); ++i )
  {
    // Get the next frame index used for calibration 
    int frameNumber = this->TrackedFrameListIndices[i]; 
    
    // Get tracked frame from list 
    TrackedFrame* trackedFrame = this->TrackedFrameList->GetTrackedFrame(frameNumber); 

    if ( trackedFrame->GetFiducialPointsCoordinatePx() == NULL )
    {
      LOG_ERROR("Unable to get segmented fiducial points from tracked frame - FiducialPointsCoordinatePx is NULL, frame is not yet segmented (position in the list: " << frameNumber << ")!" ); 
      continue; 
    }

    if ( trackedFrame->GetFiducialPointsCoordinatePx()->GetNumberOfPoints() == 0 )
    {
      continue; 
    }

    vtkSmartPointer<vtkPoints> point = vtkSmartPointer<vtkPoints>::New(); 
    point->SetDataTypeToDouble(); 
    point->SetNumberOfPoints(numberOfNFiduacials*2); // Use only the two non-moving points of the N fiducial 
    
    int vectorID(0); // ID used for point position in vtkPoints for faster execution 
    for ( int p = 0; p < trackedFrame->GetFiducialPointsCoordinatePx()->GetNumberOfPoints(); ++p )
    {
      if ( ( (p+1) % 3 ) != 2 ) // wire #1,#3,#4,#6... => use only non moving points of the N-wire 
      {
        double* wireCoordinatePx = trackedFrame->GetFiducialPointsCoordinatePx()->GetPoint(p); 
        point->SetPoint(vectorID++, wireCoordinatePx[0], wireCoordinatePx[1], wireCoordinatePx[2]); 
      }
    }

    vectorOfWirePoints.push_back(point); 
  }

  for ( unsigned int i = 0; i <= vectorOfWirePoints.size() - 2; i++ )
  {
    for( unsigned int j = i + 1; j <= vectorOfWirePoints.size() - 1; j = j + 2 )
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
#ifdef PLUS_USE_BRACHY_TRACKER
    this->AddNewColumnToReportTable("ProbePosition"); 
    this->AddNewColumnToReportTable("ProbeRotation"); 
    this->AddNewColumnToReportTable("TemplatePosition"); 
#endif
    for ( int i = 0; i < this->GetNumberOfNWirePatterns(); ++i )
    {
      std::ostringstream columnNameRightRadius; 
      columnNameRightRadius << "Wire#" << (i*3 + 1 )<< "Radius"; 
      this->AddNewColumnToReportTable(columnNameRightRadius.str().c_str());

      std::ostringstream columnNameRightRadiusDistanceFromMean; 
      columnNameRightRadiusDistanceFromMean << "Wire#" << (i*3 + 1 )<< "RadiusDistanceFromMean"; 
      this->AddNewColumnToReportTable(columnNameRightRadiusDistanceFromMean.str().c_str()); 

       std::ostringstream columnNameRightWireX; 
      columnNameRightWireX << "w" << (i*3 + 1 )<< "xPx"; 
      this->AddNewColumnToReportTable(columnNameRightWireX.str().c_str()); 

      std::ostringstream columnNameRightWireY; 
      columnNameRightWireY << "w" << (i*3 + 1 )<< "yPx"; 
      this->AddNewColumnToReportTable(columnNameRightWireY.str().c_str()); 

      std::ostringstream columnNameLeftRadius; 
      columnNameLeftRadius << "Wire#" << (i*3 + 3 )<< "Radius"; 
      this->AddNewColumnToReportTable(columnNameLeftRadius.str().c_str());

      std::ostringstream columnNameLeftRadiusDistanceFromMean; 
      columnNameLeftRadiusDistanceFromMean << "Wire#" << (i*3 + 3 )<< "RadiusDistanceFromMean"; 
      this->AddNewColumnToReportTable(columnNameLeftRadiusDistanceFromMean.str().c_str());

      std::ostringstream columnNameLeftWireX; 
      columnNameLeftWireX << "w" << (i*3 + 3 )<< "xPx"; 
      this->AddNewColumnToReportTable(columnNameLeftWireX.str().c_str()); 

      std::ostringstream columnNameLeftWireY; 
      columnNameLeftWireY << "w" << (i*3 + 3 )<< "yPx"; 
      this->AddNewColumnToReportTable(columnNameLeftWireY.str().c_str()); 
    }
  }

  const double sX = this->Spacing[0]; 
  const double sY = this->Spacing[1]; 
  std::vector<std::vector<double> > wireRadiusVector(this->GetNumberOfNWirePatterns()*2); // each wire has two points 
  std::vector<std::vector<double> > wirePositions(this->GetNumberOfNWirePatterns()*4); // each wire has 2 point and each point has 2 coordinates 

#ifdef PLUS_USE_BRACHY_TRACKER
  std::vector<double> probePosVector; 
  std::vector<double> probeRotVector; 
  std::vector<double> templatePosVector; 
#endif


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
#ifdef PLUS_USE_BRACHY_TRACKER
    double probePos(0), probeRot(0), templatePos(0); 
    if ( !vtkBrachyTracker::GetStepperEncoderValues(frame, probePos, probeRot, templatePos) )
    {
      LOG_WARNING("Unable to get probe position from tracked frame info for frame #" << frameNumber); 
      continue; 
    }
    probePosVector.push_back(probePos); 
    probeRotVector.push_back(probeRot); 
    templatePosVector.push_back(templatePos); 
#endif
  
    // Compute radius 
    for ( int w = 0; w < this->GetNumberOfNWirePatterns(); ++w )
    {
      double w1x = frame->GetFiducialPointsCoordinatePx()->GetPoint(w*3)[0]; 
      double w1y = frame->GetFiducialPointsCoordinatePx()->GetPoint(w*3)[1]; 
      double w1Radius = sqrt( pow( (w1x - this->CenterOfRotationPx[0])*sX, 2) + pow((w1y - this->CenterOfRotationPx[1])*sY, 2) ); 

      wirePositions[4*w].push_back(w1x); 
      wirePositions[4*w+1].push_back(w1y);
      wireRadiusVector[2*w].push_back(w1Radius); 


      double w3x = frame->GetFiducialPointsCoordinatePx()->GetPoint(w*3+2)[0]; 
      double w3y = frame->GetFiducialPointsCoordinatePx()->GetPoint(w*3+2)[1]; 
      double w3Radius = sqrt( pow( (w3x - this->CenterOfRotationPx[0])*sX, 2) + pow((w3y - this->CenterOfRotationPx[1])*sY, 2) ); 

      wirePositions[4*w+2].push_back(w3x); 
      wirePositions[4*w+3].push_back(w3y);
      wireRadiusVector[2*w+1].push_back(w3Radius);
    }
  }

  const int numberOfElements = wireRadiusVector[0].size(); 
  std::vector<double> wireRadiusMean(this->GetNumberOfNWirePatterns()*2, 0); // each wire has two points 
  
  for ( int i = 0; i < numberOfElements; ++i )
  {
    for ( int w = 0; w < this->GetNumberOfNWirePatterns()*2; ++w)
    {
      wireRadiusMean[w] += wireRadiusVector[w][i] / (1.0 * numberOfElements); 
    }
  }

  std::vector<std::vector<double> > wireDistancesFromMeanRadius(this->GetNumberOfNWirePatterns()*2); // each wire has two points 
  for ( int i = 0; i < numberOfElements; ++i ) 
  {
    for ( int w = 0; w < this->GetNumberOfNWirePatterns()*2; ++w)
    {
      wireDistancesFromMeanRadius[w].push_back( wireRadiusMean[w] - wireRadiusVector[w][i] ); 
    }
  }

  for ( int row = 0; row < numberOfElements; ++row )
  {
    vtkSmartPointer<vtkVariantArray> tableRow = vtkSmartPointer<vtkVariantArray>::New(); 

#ifdef PLUS_USE_BRACHY_TRACKER
    tableRow->InsertNextValue(probePosVector[row]); //ProbePosition
    tableRow->InsertNextValue(probeRotVector[row]); //ProbeRotation
    tableRow->InsertNextValue(templatePosVector[row]); //TemplatePosition
#endif
    for ( int w = 0; w < this->GetNumberOfNWirePatterns()*2; ++w)
    {
      tableRow->InsertNextValue( wireRadiusVector[w][row] ); //Wire Radius
      tableRow->InsertNextValue( wireDistancesFromMeanRadius[w][row] ); //Wire RadiusDistanceFromMean
      tableRow->InsertNextValue(wirePositions[2*w][row]); //wire pixel coordinate x
      tableRow->InsertNextValue(wirePositions[2*w+1][row]); //wire pixel coordinate y
    }

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
PlusStatus vtkCenterOfRotationCalibAlgo::GenerateReport( vtkHTMLGenerator* htmlReport, vtkGnuplotExecuter* plotter)
{
  LOG_TRACE("vtkCenterOfRotationCalibAlgo::GenerateReport"); 

  // Update result before report generation 
  if ( this->Update() != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to generate report - center of rotation axis calibration failed!"); 
    return PLUS_FAIL;
  }

  return vtkCenterOfRotationCalibAlgo::GenerateCenterOfRotationReport(this->GetNumberOfNWirePatterns(), htmlReport, plotter, this->ReportTable, this->CenterOfRotationPx); 
}

//----------------------------------------------------------------------------
//static
PlusStatus vtkCenterOfRotationCalibAlgo::GenerateCenterOfRotationReport(int numberOfNWirePatterns, 
                                                                        vtkHTMLGenerator* htmlReport, 
                                                                        vtkGnuplotExecuter* plotter, 
                                                                        vtkTable* reportTable,
                                                                        double centerOfRotationPx[2])
{
  LOG_TRACE("vtkCenterOfRotationCalibAlgo::GenerateCenterOfRotationReport"); 

#ifndef PLUS_USE_BRACHY_TRACKER
  LOG_INFO("Unable to generate center of rotation report without PLUS_USE_BRACHY_TRACKER enabled!"); 
#endif

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

  // Check gnuplot scripts 
  std::string plotCenterOfRotCalcErrorScript = vtkPlusConfig::GetInstance()->GetScriptPath("gnuplot/PlotCenterOfRotationCalculationError.gnu"); 
  if ( !vtksys::SystemTools::FileExists( plotCenterOfRotCalcErrorScript.c_str(), true) )
  {
    LOG_ERROR("Unable to find gnuplot script at: " << plotCenterOfRotCalcErrorScript); 
    return PLUS_FAIL; 
  }

  std::string plotCenterOfRotCalcErrorHistogramScript = vtkPlusConfig::GetInstance()->GetScriptPath("gnuplot/PlotCenterOfRotationCalculationErrorHistogram.gnu"); 
  if ( !vtksys::SystemTools::FileExists( plotCenterOfRotCalcErrorHistogramScript.c_str(), true) )
  {
    LOG_ERROR("Unable to find gnuplot script at: " << plotCenterOfRotCalcErrorHistogramScript); 
    return PLUS_FAIL; 
  }

  // Generate report files from table 
  std::string reportFileName=vtkPlusConfig::GetInstance()->GetApplicationStartTimestamp()+".CenterOfRotationCalculationError.txt";
  std::string reportFile = vtkPlusConfig::GetInstance()->GetOutputPath(reportFileName);
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
  std::string scriptOutputFilePrefixHistogram = "CenterOfRotationCalculationErrorHistogram"; 

  htmlReport->AddText(title.c_str(), vtkHTMLGenerator::H1); 

  std::ostringstream report; 
  report << "Center of rotation (px): " << centerOfRotationPx[0] << "     " << centerOfRotationPx[1] << "</br>" ; 
  htmlReport->AddParagraph(report.str().c_str()); 

#ifdef PLUS_USE_BRACHY_TRACKER
 
  // Every N wire has 2 plots, one for w#1 and w#3 
  const int numOfPlots = numberOfNWirePatterns * 2; 

  // Create wire labels 
  int *wires = new int[numOfPlots]; // = {1, 3, 4, 6, ... }; 
  for ( int i = 0; i < numberOfNWirePatterns; ++i )
  {
    wires[2*i] = i*3 + 1; 
    wires[2*i+1] = i*3 + 3; 
  }

  for ( int i = 0; i < numOfPlots; i++ )
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

    // Generate histogram 
    plotter->ClearArguments(); 
    plotter->AddArgument("-e");
    std::ostringstream centerOfRotCalcErrorHistogram; 
    centerOfRotCalcErrorHistogram << "f='" << reportFile << "'; o='" << scriptOutputFilePrefixHistogram << "'; w=" << wires[i] << std::ends; 
    plotter->AddArgument(centerOfRotCalcErrorHistogram.str().c_str()); 
    plotter->AddArgument(plotCenterOfRotCalcErrorHistogramScript.c_str());  
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

    std::ostringstream imageSourceHistogram; 
    std::ostringstream imageAltHistogram; 
    imageSourceHistogram << "w" << wires[i] << "_CenterOfRotationCalculationErrorHistogram.jpg" << std::ends; 
    imageAltHistogram << "Center of rotation calculation error histogram - wire #" << wires[i] << std::ends; 
    htmlReport->AddImage(imageSourceHistogram.str().c_str(), imageAltHistogram.str().c_str()); 
  }

  delete[] wires; 
#endif

  htmlReport->AddHorizontalLine(); 

  return PLUS_SUCCESS; 
}