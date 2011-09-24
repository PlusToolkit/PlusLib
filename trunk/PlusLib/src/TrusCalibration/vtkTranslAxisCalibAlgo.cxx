#include "PlusMath.h"
#include "vtkTranslAxisCalibAlgo.h"
#include "vtkObjectFactory.h"
#include "vtkTrackedFrameList.h"
#include "vtkPoints.h"
#include "vtksys/SystemTools.hxx"
#include "vtkGnuplotExecuter.h"
#include "vtkHTMLGenerator.h"
#include "vtkDoubleArray.h"
#include "vtkVariantArray.h"
#include "vtkBrachyTracker.h"

//----------------------------------------------------------------------------
vtkCxxRevisionMacro(vtkTranslAxisCalibAlgo, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkTranslAxisCalibAlgo); 

//----------------------------------------------------------------------------
vtkTranslAxisCalibAlgo::vtkTranslAxisCalibAlgo()
{
  this->TrackedFrameList = NULL; 
  this->SetTranslationAxisOrientation(0,0,1);
  this->SetSpacing(0,0); 
  this->SetDataType(UNKNOWN_DATA); 
  this->ReportTable = NULL; 
}

//----------------------------------------------------------------------------
vtkTranslAxisCalibAlgo::~vtkTranslAxisCalibAlgo()
{
  this->SetTrackedFrameList(NULL); 
  this->SetReportTable(NULL); 
}

//----------------------------------------------------------------------------
void vtkTranslAxisCalibAlgo::PrintSelf(ostream& os, vtkIndent indent)
{
  os << std::endl;
  this->Superclass::PrintSelf(os,indent);
  os << indent << "Update time: " << UpdateTime.GetMTime() << std::endl; 
  os << indent << "Spacing: " << this->Spacing[0] << "  " << this->Spacing[1] << std::endl;
  os << indent << "TranslationAxisOrientation: " << this->TranslationAxisOrientation[0] << "  " 
    << this->TranslationAxisOrientation[1] << "  " << this->TranslationAxisOrientation[2] << std::endl;
  os << indent << "Calibration error: mean=" << this->ErrorMean << "  stdev=" << this->ErrorStdev << std::endl;
 
  std::string strDataType("UNKNOWN_DATA"); 
  if ( this->DataType == PROBE_TRANSLATION ) strDataType = "PROBE_TRANSLATION"; 
  if ( this->DataType == TEMPLATE_TRANSLATION ) strDataType = "TEMPLATE_TRANSLATION"; 
  os << indent << "DataType: " << strDataType << std::endl;

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
void vtkTranslAxisCalibAlgo::SetInput(vtkTrackedFrameList* trackedFrameList)
{
  LOG_TRACE("vtkTranslAxisCalibAlgo::SetInput"); 
  this->SetTrackedFrameList(trackedFrameList); 
}


//----------------------------------------------------------------------------
PlusStatus vtkTranslAxisCalibAlgo::GetOutput(double translationAxisOrientation[3])
{
  LOG_TRACE("vtkTranslAxisCalibAlgo::GetOutput"); 
  // Update calibration result 
  PlusStatus status = this->Update(); 

  translationAxisOrientation[0] = this->TranslationAxisOrientation[0]; 
  translationAxisOrientation[1] = this->TranslationAxisOrientation[1]; 
  translationAxisOrientation[2] = this->TranslationAxisOrientation[2]; 

  return status; 
}

//----------------------------------------------------------------------------
PlusStatus vtkTranslAxisCalibAlgo::GetError(double &mean, double &stdev)
{
  LOG_TRACE("vtkTranslAxisCalibAlgo::GetError"); 
  // Update calibration result 
  PlusStatus status = this->Update(); 

  mean = this->ErrorMean; 
  stdev = this->ErrorStdev; 

  return status; 
}

//----------------------------------------------------------------------------
PlusStatus vtkTranslAxisCalibAlgo::Update()
{
  LOG_TRACE("vtkTranslAxisCalibAlgo::Update"); 

  if ( this->GetMTime() < this->UpdateTime.GetMTime() )
  {
    LOG_DEBUG("Translation axis calibration result is up-to-date!"); 
    return PLUS_SUCCESS; 
  }

  // Construct linear equations Ax = b, where A is a matrix with m rows and 
  // n columns, b is an m-vector. 
  std::vector<vnl_vector<double>> aMatrix;
  std::vector<double> bVector; 
  int numberOfUnknowns = 0; 

  // Construct linear equation for translation axis calibration
  if ( this->ConstrLinEqForTransAxisCalib(aMatrix, bVector, numberOfUnknowns) != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to contruct linear equations for translation axis calibration!"); 
    return PLUS_FAIL; 
  }

  if ( aMatrix.size() == 0 || bVector.size() == 0 || numberOfUnknowns == 0)
  {
    LOG_ERROR("Translation axis calibration failed, no data found!"); 
    return PLUS_FAIL; 
  }

  // [tx, ty, w1x0, w1y0, w3x0, w3y0, w4x0, w4y0, w6x0, w6y0... ]
  vnl_vector<double> translationAxisCalibResult(numberOfUnknowns, 0);
  if ( PlusMath::LSQRMinimize(aMatrix, bVector, translationAxisCalibResult, &this->ErrorMean, &this->ErrorStdev) != PLUS_SUCCESS)
  {
    LOG_WARNING("Failed to run LSQRMinimize!"); 
    return PLUS_FAIL; 
  }

  if ( translationAxisCalibResult.empty() || translationAxisCalibResult.size() < 2 )
  {
    LOG_ERROR("Unable to calibrate translation axis! Minimizer returned empty result."); 
    return PLUS_FAIL; 
  }

  // Set translation axis orientation 
  // NOTE: If the probe goes down the wires goes down on the MF oriented image 
  // => we need to change the sign of the axis to compensate it
  this->SetTranslationAxisOrientation(-translationAxisCalibResult[0], translationAxisCalibResult[1], 1); 

  this->UpdateReportTable(aMatrix, bVector, translationAxisCalibResult); 

  this->UpdateTime.Modified(); 

  return PLUS_SUCCESS; 
}


//----------------------------------------------------------------------------
PlusStatus vtkTranslAxisCalibAlgo::ConstrLinEqForTransAxisCalib( std::vector<vnl_vector<double>> &aMatrix, std::vector<double> &bVector, int& numberOfUnknowns)
{
  LOG_TRACE("vtkTranslAxisCalibAlgo::ConstrLinEqForTransAxisCalib"); 
  aMatrix.clear(); 
  bVector.clear(); 
  numberOfUnknowns = 0; 

  if ( this->GetTrackedFrameList() == NULL )
  {
    LOG_ERROR("Failed to construct linear equation for translation axis calibration - tracked frame list is NULL!"); 
    return PLUS_FAIL; 
  }

  for ( int frame = 0; frame < this->GetTrackedFrameList()->GetNumberOfTrackedFrames(); ++frame )
  {
    TrackedFrame* trackedFrame = this->GetTrackedFrameList()->GetTrackedFrame(frame); 
    if ( trackedFrame == NULL ) 
    {
      LOG_ERROR("Unable to get tracked frame from the list - tracked frame is NULL (position in the list: " << frame << ")!"); 
      continue; 
    }

    // Get the encoder value in mm 
    double probePos(0), probeRot(0), templatePos(0); 
    if ( !vtkBrachyTracker::GetStepperEncoderValues(trackedFrame, probePos, probeRot, templatePos) )
    {
      LOG_WARNING("Translation axis calibration: Unable to get stepper encoder values from tracked frame info for frame #" << frame); 
      continue; 
    }

    double z(0); 
    if ( this->GetDataType() == PROBE_TRANSLATION ) z = probePos; 
    else if ( this->GetDataType() == TEMPLATE_TRANSLATION ) z = templatePos; 
    else
    {
      LOG_ERROR("Unable to construct linear equations for translation axis calibration - this data type is not supported: " << this->GetDataType()); 
      return PLUS_FAIL;
    }

    if ( trackedFrame->GetFiducialPointsCoordinatePx() == NULL )
    {
      LOG_ERROR("Unable to get segmented fiducial points from tracked frame - FiducialPointsCoordinatePx is NULL, frame is not yet segmented (position in the list: " << frame << ")!" ); 
      continue; 
    }

    if ( trackedFrame->GetFiducialPointsCoordinatePx()->GetNumberOfPoints() == 0 )
    {
      LOG_DEBUG("Unable to get segmented fiducial points from tracked frame - couldn't segment image (position in the list: " << frame << ")!" ); 
      continue; 
    }

    // Every N fiducial has 3 points on the image: numberOfNFiducials = NumberOfPoints / 3 
    const int numberOfNFiduacials = trackedFrame->GetFiducialPointsCoordinatePx()->GetNumberOfPoints() / 3; 

    // We're using 2 out of 3 wires per each N-wire => ( 2 * numberOfNFiduacials )
    // We need the X and Y coordinates of each wires => need to multiply by 2 
    // We have two more unknown variables (tx, ty) the translation axis orientation for each axes
    // numberOfUnknowns = ( 2 * numberOfNFiduacials ) * 2 + 2 
    numberOfUnknowns = ( 2 * numberOfNFiduacials ) * 2 + 2;

    // Matrix column indecies used for value 1 in the matrix
    // e.g. for w4x = w4x0 + z * tx => [ z 0 0 0 0 0 1 0 0 0 ] => movingIndex = 6, the index position of value 1 
    int movingIndex = numberOfUnknowns; 

    // Construct the A matrix and b vector for each side fiducials  
    for ( int i = 0; i < trackedFrame->GetFiducialPointsCoordinatePx()->GetNumberOfPoints(); ++i)
    {
      if ( ( (i+1) % 3 ) != 2 ) // wire #1,#3,#4,#6... => non moving points of the N-wire 
      {
        // Wire X coordinate in mm (#1,#3,#4,#6...)
        double bX = trackedFrame->GetFiducialPointsCoordinatePx()->GetPoint(i)[0] * this->GetSpacing()[0]; 
        bVector.push_back(bX); 

        // Wire Y coordinate in mm (#1,#3,#4,#6...)
        double bY = trackedFrame->GetFiducialPointsCoordinatePx()->GetPoint(i)[1] * this->GetSpacing()[1]; 
        bVector.push_back(bY); 

        if ( movingIndex >= numberOfUnknowns - 1 )
        {
          movingIndex = 1; // start from matrix column index 2 
        }

        vnl_vector<double> a1(numberOfUnknowns,0); 
        a1.put(0, z);  // raw encoder value in mm
        a1.put(++movingIndex, 1);  // the index position of matrix column for value 1 

        vnl_vector<double> a2(numberOfUnknowns,0); 
        a2.put(1, z); // raw encoder value in mm
        a2.put(++movingIndex, 1);  // the index position of matrix column for value 1 

        aMatrix.push_back(a1); 
        aMatrix.push_back(a2); 
      }
    } // end of fiduacials 

  } // end of frames 

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus vtkTranslAxisCalibAlgo::UpdateReportTable(const std::vector<vnl_vector<double>> &aMatrix, 
                                                     const std::vector<double> &bVector, 
                                                     const vnl_vector<double> &resultVector)
{
  LOG_TRACE("vtkTranslAxisCalibAlgo::UpdateReportTable"); 

  const int numOfSegmentedPoints(8); 

  if ( this->ReportTable == NULL )
  {
    vtkSmartPointer<vtkTable> table = vtkSmartPointer<vtkTable>::New(); 
    this->SetReportTable(table); 

    // Create table columns 
    vtkSmartPointer<vtkDoubleArray> colProbePosition = vtkSmartPointer<vtkDoubleArray>::New(); 
    colProbePosition->SetName("ProbePosition"); 
    this->ReportTable->AddColumn(colProbePosition); 

    // Measured positions 
    vtkSmartPointer<vtkDoubleArray> colMW1X = vtkSmartPointer<vtkDoubleArray>::New(); 
    colMW1X->SetName("MeasuredWire1xInImageMm"); 
    this->ReportTable->AddColumn(colMW1X); 

    vtkSmartPointer<vtkDoubleArray> colMW1Y = vtkSmartPointer<vtkDoubleArray>::New(); 
    colMW1Y->SetName("MeasuredWire1yInImageMm"); 
    this->ReportTable->AddColumn(colMW1Y); 

    vtkSmartPointer<vtkDoubleArray> colMW3X = vtkSmartPointer<vtkDoubleArray>::New(); 
    colMW3X->SetName("MeasuredWire3xInImageMm"); 
    this->ReportTable->AddColumn(colMW3X); 

    vtkSmartPointer<vtkDoubleArray> colMW3Y = vtkSmartPointer<vtkDoubleArray>::New(); 
    colMW3Y->SetName("MeasuredWire3yInImageMm"); 
    this->ReportTable->AddColumn(colMW3Y); 

    vtkSmartPointer<vtkDoubleArray> colMW4X = vtkSmartPointer<vtkDoubleArray>::New(); 
    colMW4X->SetName("MeasuredWire4xInImageMm"); 
    this->ReportTable->AddColumn(colMW4X); 

    vtkSmartPointer<vtkDoubleArray> colMW4Y = vtkSmartPointer<vtkDoubleArray>::New(); 
    colMW4Y->SetName("MeasuredWire4yInImageMm"); 
    this->ReportTable->AddColumn(colMW4Y); 

    vtkSmartPointer<vtkDoubleArray> colMW6X = vtkSmartPointer<vtkDoubleArray>::New(); 
    colMW6X->SetName("MeasuredWire6xInImageMm"); 
    this->ReportTable->AddColumn(colMW6X); 

    vtkSmartPointer<vtkDoubleArray> colMW6Y = vtkSmartPointer<vtkDoubleArray>::New(); 
    colMW6Y->SetName("MeasuredWire6yInImageMm"); 
    this->ReportTable->AddColumn(colMW6Y); 

    // Computed position 
     vtkSmartPointer<vtkDoubleArray> colCW1X = vtkSmartPointer<vtkDoubleArray>::New(); 
    colCW1X->SetName("ComputedWire1xInImageMm"); 
    this->ReportTable->AddColumn(colCW1X); 

    vtkSmartPointer<vtkDoubleArray> colCW1Y = vtkSmartPointer<vtkDoubleArray>::New(); 
    colCW1Y->SetName("ComputedWire1yInImageMm"); 
    this->ReportTable->AddColumn(colCW1Y); 

    vtkSmartPointer<vtkDoubleArray> colCW3X = vtkSmartPointer<vtkDoubleArray>::New(); 
    colCW3X->SetName("ComputedWire3xInImageMm"); 
    this->ReportTable->AddColumn(colCW3X); 

    vtkSmartPointer<vtkDoubleArray> colCW3Y = vtkSmartPointer<vtkDoubleArray>::New(); 
    colCW3Y->SetName("ComputedWire3yInImageMm"); 
    this->ReportTable->AddColumn(colCW3Y); 

    vtkSmartPointer<vtkDoubleArray> colCW4X = vtkSmartPointer<vtkDoubleArray>::New(); 
    colCW4X->SetName("ComputedWire4xInImageMm"); 
    this->ReportTable->AddColumn(colCW4X); 

    vtkSmartPointer<vtkDoubleArray> colCW4Y = vtkSmartPointer<vtkDoubleArray>::New(); 
    colCW4Y->SetName("ComputedWire4yInImageMm"); 
    this->ReportTable->AddColumn(colCW4Y); 

    vtkSmartPointer<vtkDoubleArray> colCW6X = vtkSmartPointer<vtkDoubleArray>::New(); 
    colCW6X->SetName("ComputedWire6xInImageMm"); 
    this->ReportTable->AddColumn(colCW6X); 

    vtkSmartPointer<vtkDoubleArray> colCW6Y = vtkSmartPointer<vtkDoubleArray>::New(); 
    colCW6Y->SetName("ComputedWire6yInImageMm"); 
    this->ReportTable->AddColumn(colCW6Y); 

  }

  for( int row = 0; row < bVector.size(); row = row + numOfSegmentedPoints)
  {
    vtkSmartPointer<vtkVariantArray> tableRow = vtkSmartPointer<vtkVariantArray>::New(); 

    tableRow->InsertNextValue(aMatrix[row].get(0));  // ProbePosition

    tableRow->InsertNextValue(bVector[row]);   // MeasuredWire1xInImageMm
    tableRow->InsertNextValue(bVector[row+1]); // MeasuredWire1yInImageMm
    tableRow->InsertNextValue(bVector[row+2]); // MeasuredWire3xInImageMm
    tableRow->InsertNextValue(bVector[row+3]); // MeasuredWire3yInImageMm
    tableRow->InsertNextValue(bVector[row+4]); // MeasuredWire4xInImageMm
    tableRow->InsertNextValue(bVector[row+5]); // MeasuredWire4yInImageMm
    tableRow->InsertNextValue(bVector[row+6]); // MeasuredWire6xInImageMm
    tableRow->InsertNextValue(bVector[row+7]); // MeasuredWire6yInImageMm

    tableRow->InsertNextValue(resultVector[2] + aMatrix[row    ].get(0) * resultVector[0]);   // ComputedWire1xInImageMm
    tableRow->InsertNextValue(resultVector[3] + aMatrix[row + 1].get(1) * resultVector[1]);   // ComputedWire1yInImageMm
    tableRow->InsertNextValue(resultVector[4] + aMatrix[row + 2].get(0) * resultVector[0]);   // ComputedWire3xInImageMm
    tableRow->InsertNextValue(resultVector[5] + aMatrix[row + 3].get(1) * resultVector[1]);   // ComputedWire3yInImageMm
    tableRow->InsertNextValue(resultVector[6] + aMatrix[row + 4].get(0) * resultVector[0]);   // ComputedWire4xInImageMm
    tableRow->InsertNextValue(resultVector[7] + aMatrix[row + 5].get(1) * resultVector[1]);   // ComputedWire4yInImageMm
    tableRow->InsertNextValue(resultVector[8] + aMatrix[row + 6].get(0) * resultVector[0]);   // ComputedWire6xInImageMm
    tableRow->InsertNextValue(resultVector[9] + aMatrix[row + 7].get(1) * resultVector[1]);   // ComputedWire6yInImageMm

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
PlusStatus vtkTranslAxisCalibAlgo::GenerateReport( vtkHTMLGenerator* htmlReport, vtkGnuplotExecuter* plotter, const char* gnuplotScriptsFolder)
{
  LOG_TRACE("vtkTranslAxisCalibAlgo::GenerateReport"); 

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
    report << "Probe translation axis orientation: " << this->TranslationAxisOrientation[0] << "     " 
      << this->TranslationAxisOrientation[1] << "     " << this->TranslationAxisOrientation[2] << "</br>" ; 
    break; 
  case TEMPLATE_TRANSLATION: 
    title = "Template Translation Axis Calibration Analysis";
    scriptOutputFilePrefixHistogram = "TemplateTranslationAxisCalibrationErrorHistogram"; 
    scriptOutputFilePrefix = "TemplateTranslationAxisCalibrationError"; 
    report << "Probe translation axis orientation: " << this->TranslationAxisOrientation[0] << "     " 
      << this->TranslationAxisOrientation[1] << "     " << this->TranslationAxisOrientation[2] << "</br>" ; 
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

  return PLUS_SUCCESS; 
}