#include "PlusMath.h"
#include "vtkRotationAxisCalibAlgo.h"
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
#include "vtkCenterOfRotationCalibAlgo.h"



//----------------------------------------------------------------------------
vtkCxxRevisionMacro(vtkRotationAxisCalibAlgo, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkRotationAxisCalibAlgo); 

//----------------------------------------------------------------------------
vtkRotationAxisCalibAlgo::vtkRotationAxisCalibAlgo()
{
  this->TrackedFrameList = NULL; 
  this->SetRotationAxisOrientation(0,0,1);
  this->SetCenterOfRotationPx(0,0); 
  this->SetSpacing(0,0); 
  this->ReportTable = NULL; 
  this->SetMinNumberOfRotationClusters(4); 
  this->ErrorMean = 0.0; 
  this->ErrorStdev = 0.0; 
}

//----------------------------------------------------------------------------
vtkRotationAxisCalibAlgo::~vtkRotationAxisCalibAlgo()
{
  this->SetTrackedFrameList(NULL); 
  this->SetReportTable(NULL); 
}

//----------------------------------------------------------------------------
void vtkRotationAxisCalibAlgo::PrintSelf(ostream& os, vtkIndent indent)
{
  os << std::endl;
  this->Superclass::PrintSelf(os,indent);
  os << indent << "Update time: " << UpdateTime.GetMTime() << std::endl; 
  os << indent << "Spacing: " << this->Spacing[0] << "  " << this->Spacing[1] << std::endl;
  os << indent << "MinNumberOfRotationClusters: " << this->MinNumberOfRotationClusters << std::endl;
  os << indent << "RotationAxisOrientation: " << this->RotationAxisOrientation[0] << "  " 
    << this->RotationAxisOrientation[1] << "  " << this->RotationAxisOrientation[2] << std::endl;
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
void vtkRotationAxisCalibAlgo::SetInputs(vtkTrackedFrameList* trackedFrameList, double spacing[2])
{
  LOG_TRACE("vtkRotationAxisCalibAlgo::SetInput"); 
  this->SetTrackedFrameList(trackedFrameList); 
  this->SetSpacing(spacing);
}


//----------------------------------------------------------------------------
PlusStatus vtkRotationAxisCalibAlgo::GetRotationAxisOrientation(double rotationAxisOrientation[3])
{
  LOG_TRACE("vtkRotationAxisCalibAlgo::GetOutput"); 
  // Update calibration result 
  PlusStatus status = this->Update(); 

  rotationAxisOrientation[0] = this->RotationAxisOrientation[0]; 
  rotationAxisOrientation[1] = this->RotationAxisOrientation[1]; 
  rotationAxisOrientation[2] = this->RotationAxisOrientation[2]; 

  return status; 
}

//----------------------------------------------------------------------------
PlusStatus vtkRotationAxisCalibAlgo::GetError(double &mean, double &stdev)
{
  LOG_TRACE("vtkRotationAxisCalibAlgo::GetError"); 
  // Update calibration result 
  PlusStatus status = this->Update(); 

  mean = this->ErrorMean; 
  stdev = this->ErrorStdev; 

  return status; 
}

//----------------------------------------------------------------------------
PlusStatus vtkRotationAxisCalibAlgo::Update()
{
  LOG_TRACE("vtkRotationAxisCalibAlgo::Update"); 

  if ( this->GetMTime() < this->UpdateTime.GetMTime() )
  {
    LOG_DEBUG("Rotation axis calibration result is up-to-date!"); 
    return PLUS_SUCCESS; 
  }

  // Cluster input tracked frames by Z positions 
  std::vector< std::vector<int> > clusterOfIndices; 
  this->ClusterTrackedFrames(clusterOfIndices); 

  if ( clusterOfIndices.size() < this->MinNumberOfRotationClusters)
  {
    LOG_WARNING("Unable to calibrate rotation axis reliably: Number of rotation clusters are less than the minimum requirements (" << clusterOfIndices.size() << " of " << this->MinNumberOfRotationClusters << ")." ); 
  }

  if ( clusterOfIndices.size()==0 )
  {
    LOG_ERROR("Failed to calibrate rotation axis: Unable to find any rotation clusters!" ); 
    return PLUS_FAIL; 
  }

  if ( clusterOfIndices.size()==1 )
  {
    // Not enough clusters to use LSQR fitting, so just use the one single cluster result
    double centerOfRotationPx[2] = {0, 0}; 
    vtkSmartPointer<vtkCenterOfRotationCalibAlgo> centerOfRotationCalibAlgo = vtkSmartPointer<vtkCenterOfRotationCalibAlgo>::New(); 
    centerOfRotationCalibAlgo->SetInputs(this->TrackedFrameList, clusterOfIndices[0], this->Spacing); 
    if ( centerOfRotationCalibAlgo->GetCenterOfRotationPx(centerOfRotationPx) != PLUS_SUCCESS )
    {
      LOG_WARNING("Failed to compute center of rotation!"); 
      return PLUS_FAIL; 
    }
    // don't use set macro, it changes the modification time of the algorithm 
    this->CenterOfRotationPx[0] = centerOfRotationPx[0]; 
    this->CenterOfRotationPx[1] = centerOfRotationPx[1]; 

    this->UpdateTime.Modified(); 

    return PLUS_SUCCESS; 
  }

  std::vector< std::pair<double, double> > listOfCenterOfRotations; 
  std::vector< double > listOfClusterPositions; 

  for ( unsigned int cluster = 0; cluster < clusterOfIndices.size(); ++cluster )
  {
    double clusterPosition(0); 
    if ( this->GetClusterZPosition(clusterOfIndices[cluster], clusterPosition) != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to get cluster Z position for rotation axis calibration!"); 
      continue; 
    }

    double centerOfRotationPx[2] = {0, 0}; 
    vtkSmartPointer<vtkCenterOfRotationCalibAlgo> centerOfRotationCalibAlgo = vtkSmartPointer<vtkCenterOfRotationCalibAlgo>::New(); 
    centerOfRotationCalibAlgo->SetInputs(this->TrackedFrameList, clusterOfIndices[cluster], this->Spacing ); 
    if ( centerOfRotationCalibAlgo->GetCenterOfRotationPx(centerOfRotationPx) != PLUS_SUCCESS )
    {
      LOG_WARNING("Failed to compute center of rotation for cluster #" << cluster << " at " << std::fixed << clusterPosition << " mm"); 
    }
    else
    {
      LOG_INFO("Center of rotation in pixels for cluster #" << cluster << " at " << std::fixed << clusterPosition << " mm: " << centerOfRotationPx[0] << "   " << centerOfRotationPx[1]); 
      std::pair<double, double> cor (centerOfRotationPx[0], centerOfRotationPx[1]); 
      listOfCenterOfRotations.push_back( cor ); 
      listOfClusterPositions.push_back(clusterPosition); 
    }
  }

  // Construct linear equations Ax = b, where A is a matrix with m rows and 
  // n columns, b is an m-vector. 
  std::vector<vnl_vector<double>> aMatrix;
  std::vector<double> bVector; 

  // Construct linear equation for rotation axis calibration
  this->ConstructLinearEquationForCalibration(listOfCenterOfRotations, listOfClusterPositions, aMatrix, bVector); 

  // [rx, ry, rx0, ry0 ]
  vnl_vector<double> rotationAxisCalibResult(4, 0);
  if ( PlusMath::LSQRMinimize(aMatrix, bVector, rotationAxisCalibResult, &this->ErrorMean, &this->ErrorStdev) != PLUS_SUCCESS )
  {
    LOG_WARNING("Failed to run LSQRMinimize!"); 
    return PLUS_FAIL; 
  }

  if ( rotationAxisCalibResult.empty() || rotationAxisCalibResult.size() < 4 )
  {
    LOG_ERROR("Unable to calibrate rotation axis! Minimizer returned empty result."); 
    return PLUS_FAIL; 
  }

  // Set the center of rotation in px 
  // don't use set macro, it changes the modification time of the algorithm 
  this->CenterOfRotationPx[0] = rotationAxisCalibResult[2] / this->GetSpacing()[0]; 
  this->CenterOfRotationPx[1] = rotationAxisCalibResult[3] / this->GetSpacing()[1]; 

  // Set rotation axis orientation 
  // NOTE: If the probe goes down the wires goes down on the MF oriented image 
  // => we need to change the sign of the axis to compensate it
  // don't use set macro, it changes the modification time of the algorithm 
  this->RotationAxisOrientation[0] = -rotationAxisCalibResult[0]; 
  this->RotationAxisOrientation[1] = rotationAxisCalibResult[1]; 
  this->RotationAxisOrientation[2] = 1; 

  this->UpdateReportTable(aMatrix, bVector, rotationAxisCalibResult); 
  this->UpdateTime.Modified(); 

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus vtkRotationAxisCalibAlgo::ConstructLinearEquationForCalibration( const std::vector< std::pair<double, double> > &listOfCenterOfRotations, 
                                                                           const std::vector< double > &listOfClusterPositions, 
                                                                           std::vector<vnl_vector<double>> &aMatrix, 
                                                                           std::vector<double> &bVector)
{
  LOG_TRACE("vtkRotationAxisCalibAlgo::ConstructLinearEquationForCalibration"); 
  aMatrix.clear(); 
  bVector.clear(); 

  for ( unsigned int i = 0; i < listOfCenterOfRotations.size(); ++i )
  {
    double z = listOfClusterPositions[i]; 

    // Rotation x coordinate
    double b1 = listOfCenterOfRotations[i].first * this->GetSpacing()[0]; 
    vnl_vector<double> a1(4,0); 
    a1.put(0, z); 
    a1.put(2, 1); 

    bVector.push_back(b1); 
    aMatrix.push_back(a1); 

    // Rotation y coordinate
    double b2 = listOfCenterOfRotations[i].second * this->GetSpacing()[1]; 
    vnl_vector<double> a2(4,0); 
    a2.put(1, z); 
    a2.put(3, 1); 

    bVector.push_back(b2); 
    aMatrix.push_back(a2); 

    LOG_DEBUG("ConstructLinearEquationForCalibration (rotX, rotY, probeZ): " << b1 << "  " << b2 << "  " << z ); 
  }

  return PLUS_SUCCESS; 
}


//***************************************************************************
//								Clustering
//***************************************************************************

//----------------------------------------------------------------------------
PlusStatus vtkRotationAxisCalibAlgo::GetClusterZPosition(std::vector<int> &cluster, double &clusterPosition)
{
  double meanZPosition(0); 

  const unsigned int numOfFrames = cluster.size(); 
  for ( unsigned int index = 0; index < numOfFrames; ++index )
  {
    double probePos(0), probeRot(0), templatePos(0); 
    if ( !vtkBrachyTracker::GetStepperEncoderValues(this->TrackedFrameList->GetTrackedFrame( cluster[index] ), probePos, probeRot, templatePos) )
    {
      LOG_WARNING("GetClusterZPosition: Unable to get probe position from tracked frame info for frame #" << cluster[index]); 
      continue; 
    }
    meanZPosition += probePos / (1.0*numOfFrames); 
  }

  clusterPosition = meanZPosition; 

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus vtkRotationAxisCalibAlgo::ClusterTrackedFrames(std::vector< std::vector<int> > &clusterOfIndices)
{
  LOG_TRACE("vtkRotationAxisCalibAlgo::ClusterSegmentedFrames"); 

  vtkSmartPointer<vtkPoints> clusterPoints = vtkSmartPointer<vtkPoints>::New(); 
  for ( int frame = 0; frame < this->TrackedFrameList->GetNumberOfTrackedFrames(); ++frame )
  {
    TrackedFrame* trackedFrame = this->GetTrackedFrameList()->GetTrackedFrame(frame); 
    double probePos(0), probeRot(0), templatePos(0); 
    if ( !vtkBrachyTracker::GetStepperEncoderValues(trackedFrame, probePos, probeRot, templatePos) )
    {
      LOG_WARNING("Clustering: Unable to get probe position from tracked frame info for frame #" << frame); 
      continue; 
    }

    LOG_DEBUG("Insert point to cluster list at probe position: " << probePos); 
    clusterPoints->InsertNextPoint(probePos, 0, 0); 
  }


  vtkSmartPointer<vtkPolyData> data = vtkSmartPointer<vtkPolyData>::New();
  data->SetPoints(clusterPoints); 

  vtkSmartPointer<vtkMeanShiftClustering> meanShiftFilter = vtkSmartPointer<vtkMeanShiftClustering>::New();
  meanShiftFilter->SetInputConnection(data->GetProducerPort());
  meanShiftFilter->SetWindowRadius(5.0); //radius should be bigger than expected clusters
  meanShiftFilter->SetGaussianVariance(1.0);
  meanShiftFilter->Update();

  // Clear list before clustering 
  clusterOfIndices.clear(); 

  for(unsigned int r = 0; r < this->TrackedFrameList->GetNumberOfTrackedFrames(); ++r)
  {
    int clusterID = meanShiftFilter->GetPointAssociations(r); 
    if ( clusterID >= 0 )
    {
      while ( clusterOfIndices.size() < clusterID + 1 )
      {
        LOG_DEBUG("Create new segmented frame cluster for clusterID: " << clusterID); 
        std::vector<int> indices; 
        clusterOfIndices.push_back(indices); 
      }

      clusterOfIndices[clusterID].push_back(r); 
    }
  }

  LOG_DEBUG("Number of clusters: " << clusterOfIndices.size()); 
  for ( unsigned int i = 0; i < clusterOfIndices.size(); ++i)
  {
    LOG_DEBUG("Number of elements in cluster #" << i << ": " << clusterOfIndices[i].size()); 
  }

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus vtkRotationAxisCalibAlgo::UpdateReportTable(const std::vector<vnl_vector<double>> &aMatrix, 
                                                       const std::vector<double> &bVector, 
                                                       const vnl_vector<double> &resultVector)
{
  LOG_TRACE("vtkRotationAxisCalibAlgo::UpdateReportTable"); 

  // Clear report table 
  this->SetReportTable(NULL); 

  if ( this->ReportTable == NULL )
  {
    this->AddNewColumnToReportTable("ProbePosition"); 
    this->AddNewColumnToReportTable("MeasuredCenterOfRotationXInImageMm"); 
    this->AddNewColumnToReportTable("MeasuredCenterOfRotationYInImageMm"); 
    this->AddNewColumnToReportTable("ComputedCenterOfRotationXInImageMm"); 
    this->AddNewColumnToReportTable("ComputedCenterOfRotationYInImageMm"); 
  }

  const int numberOfAxes(2); // TODO: it works only with double N phantom 

  for( int row = 0; row < bVector.size(); row = row + numberOfAxes)
  {
    vtkSmartPointer<vtkVariantArray> tableRow = vtkSmartPointer<vtkVariantArray>::New(); 

    tableRow->InsertNextValue(aMatrix[row].get(0));  // ProbePosition

    tableRow->InsertNextValue(bVector[row]);   // MeasuredCenterOfRotationXInImageMm
    tableRow->InsertNextValue(bVector[row+1]); // MeasuredCenterOfRotationYInImageMm

    tableRow->InsertNextValue(resultVector[2] + aMatrix[row    ].get(0) * resultVector[0]);   // ComputedCenterOfRotationXInImageMm
    tableRow->InsertNextValue(resultVector[3] + aMatrix[row + 1].get(1) * resultVector[1]);   // ComputedCenterOfRotationYInImageMm

    if ( tableRow->GetNumberOfTuples() == this->ReportTable->GetNumberOfColumns() )
    {
      this->ReportTable->InsertNextRow(tableRow); 
    }
    else
    {
      LOG_WARNING("Unable to insert new row to rotation axis calibration report table, number of columns are different (" 
        << tableRow->GetNumberOfTuples() << " vs. " << this->ReportTable->GetNumberOfColumns() << ")."); 
    }
  }

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus vtkRotationAxisCalibAlgo::AddNewColumnToReportTable( const char* columnName )
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
PlusStatus vtkRotationAxisCalibAlgo::GenerateReport( vtkHTMLGenerator* htmlReport, vtkGnuplotExecuter* plotter, const char* gnuplotScriptsFolder)
{
  /* LOG_TRACE("vtkRotationAxisCalibAlgo::GenerateReport"); 

  // Update result before report generation 
  if ( this->Update() != PLUS_SUCCESS )
  {
  LOG_ERROR("Unable to generate report - translation axix calibration failed!"); 
  return PLUS_FAIL;
  }

  if ( htmlReport == NULL || plotter == NULL )
  {
  LOG_ERROR("Caller should define HTML report generator and gnuplot plotter before report generation!"); 
  return PLUS_FAIL; 
  }

  // Check gnuplot scripts 
  std::string plotStepperCalibrationErrorHistogramScript = gnuplotScriptsFolder + std::string("/PlotStepperCalibrationErrorHistogram.gnu"); 
  if ( !vtksys::SystemTools::FileExists( plotStepperCalibrationErrorHistogramScript.c_str(), true) )
  {
  LOG_ERROR("Unable to find gnuplot script at: " << plotStepperCalibrationErrorHistogramScript); 
  return PLUS_FAIL; 
  }

  std::string plotStepperCalibrationErrorScript = gnuplotScriptsFolder + std::string("/PlotStepperCalibrationError.gnu"); 
  if ( !vtksys::SystemTools::FileExists( plotStepperCalibrationErrorScript.c_str(), true) )
  {
  LOG_ERROR("Unable to find gnuplot script at: " << plotStepperCalibrationErrorScript); 
  return PLUS_FAIL; 
  }

  // Generate report files from table 
  std::string reportFile = std::string(vtkPlusConfig::GetInstance()->GetOutputDirectory()) + std::string("/TranslationAxisCalibrationReport.txt");
  if ( vtkGnuplotExecuter::DumpTableToFileInGnuplotFormat( this->ReportTable, reportFile.c_str()) != PLUS_SUCCESS )
  {
  LOG_ERROR("Failed to dump translation axis calibration report table to " << reportFile );
  return PLUS_FAIL; 
  }

  // Make sure the report file is there
  if ( !vtksys::SystemTools::FileExists( reportFile.c_str(), true) )
  {
  LOG_ERROR("Unable to find translation axis calibration report file at: " << reportFile); 
  return PLUS_FAIL; 
  }

  std::ostringstream report; 
  std::string title; 
  std::string scriptOutputFilePrefixHistogram, scriptOutputFilePrefix; 
  switch ( this->DataType )
  {
  case PROBE_TRANSLATION: 
  title = "Probe Translation Axis Calibration Analysis"; 
  scriptOutputFilePrefixHistogram = "ProbeTranslationAxisCalibrationErrorHistogram"; 
  scriptOutputFilePrefix = "ProbeTranslationAxisCalibrationError";
  report << "Probe translation axis orientation: " << this->RotationAxisOrientation[0] << "     " 
  << this->RotationAxisOrientation[1] << "     " << this->RotationAxisOrientation[2] << "</br>" ; 
  break; 
  case TEMPLATE_TRANSLATION: 
  title = "Template Translation Axis Calibration Analysis";
  scriptOutputFilePrefixHistogram = "TemplateTranslationAxisCalibrationErrorHistogram"; 
  scriptOutputFilePrefix = "TemplateTranslationAxisCalibrationError"; 
  report << "Probe translation axis orientation: " << this->RotationAxisOrientation[0] << "     " 
  << this->RotationAxisOrientation[1] << "     " << this->RotationAxisOrientation[2] << "</br>" ; 
  break; 
  default:
  LOG_ERROR("Unable to generate translation axis calibration report - current data type is not supported: " << this->DataType); 
  return PLUS_FAIL; 
  }

  htmlReport->AddText(title.c_str(), vtkHTMLGenerator::H1); 

  htmlReport->AddParagraph(report.str().c_str()); 

  htmlReport->AddText("Error Histogram", vtkHTMLGenerator::H2); 

  const int wires[4] = {1, 3, 4, 6}; // TODO: it just handle 2 N-wires

  //**************************** Error histogram ****************************

  for ( int i = 0; i < 4; i++ )
  {
  std::ostringstream wireName; 
  wireName << "Wire #" << wires[i] << std::ends; 
  htmlReport->AddText(wireName.str().c_str(), vtkHTMLGenerator::H3); 
  plotter->ClearArguments(); 
  plotter->AddArgument("-e");
  std::ostringstream transAxisError; 
  transAxisError << "f='" << reportFile << "'; o='" << scriptOutputFilePrefixHistogram << "'; w=" << wires[i] << std::ends; 
  plotter->AddArgument(transAxisError.str().c_str()); 
  plotter->AddArgument(plotStepperCalibrationErrorHistogramScript.c_str());  
  if ( plotter->Execute() != PLUS_SUCCESS )
  {
  LOG_ERROR("Failed to run gnuplot executer!"); 
  return PLUS_FAIL; 
  }
  plotter->ClearArguments(); 

  std::ostringstream imageSource; 
  std::ostringstream imageAlt; 

  switch ( this->DataType )
  {
  case PROBE_TRANSLATION: 
  imageSource << "w" << wires[i] << "_ProbeTranslationAxisCalibrationErrorHistogram.jpg" << std::ends; 
  imageAlt << "Probe translation axis calibration error histogram - wire #" << wires[i] << std::ends; 
  break; 
  case TEMPLATE_TRANSLATION: 
  imageSource << "w" << wires[i] << "_TemplateTranslationAxisCalibrationErrorHistogram.jpg" << std::ends; 
  imageAlt << "Template translation axis calibration error histogram - wire #" << wires[i] << std::ends; 
  break; 
  }

  htmlReport->AddImage(imageSource.str().c_str(), imageAlt.str().c_str()); 
  }

  //**************************** Error plot ****************************

  htmlReport->AddText("Error Plot", vtkHTMLGenerator::H2); 

  for ( int i = 0; i < 4; i++ )
  {
  std::ostringstream wireName; 
  wireName << "Wire #" << wires[i] << std::ends; 
  htmlReport->AddText(wireName.str().c_str(), vtkHTMLGenerator::H3); 
  plotter->ClearArguments(); 
  plotter->AddArgument("-e");
  std::ostringstream transAxisError; 
  transAxisError << "f='" << reportFile << "'; o='" << scriptOutputFilePrefix << "'; w=" << wires[i] << std::ends; 
  plotter->AddArgument(transAxisError.str().c_str()); 
  plotter->AddArgument(plotStepperCalibrationErrorScript.c_str());  
  if ( plotter->Execute() != PLUS_SUCCESS )
  {
  LOG_ERROR("Failed to run gnuplot executer!"); 
  return PLUS_FAIL; 
  }
  plotter->ClearArguments(); 

  std::ostringstream imageSourceX, imageAltX, imageSourceY, imageAltY; 

  switch ( this->DataType )
  {
  case PROBE_TRANSLATION: 
  imageSourceX << "w" << wires[i] << "x_ProbeTranslationAxisCalibrationError.jpg" << std::ends; 
  imageAltX << "Probe translation axis calibration error - wire #" << wires[i] << " X Axis" << std::ends; 
  imageSourceY << "w" << wires[i] << "y_ProbeTranslationAxisCalibrationError.jpg" << std::ends; 
  imageAltY << "Probe translation axis calibration error - wire #" << wires[i] << " Y Axis" << std::ends; 
  break; 
  case TEMPLATE_TRANSLATION: 
  imageSourceX << "w" << wires[i] << "x_TemplateTranslationAxisCalibrationError.jpg" << std::ends; 
  imageAltX << "Template translation axis calibration error - wire #" << wires[i] << " X Axis" << std::ends; 
  imageSourceY << "w" << wires[i] << "y_TemplateTranslationAxisCalibrationError.jpg" << std::ends; 
  imageAltY << "Template translation axis calibration error - wire #" << wires[i] << " Y Axis" << std::ends; 
  break; 
  }

  htmlReport->AddImage(imageSourceX.str().c_str(), imageAltX.str().c_str()); 
  htmlReport->AddImage(imageSourceY.str().c_str(), imageAltY.str().c_str()); 
  }

  htmlReport->AddHorizontalLine(); 
  */
  return PLUS_SUCCESS; 
}