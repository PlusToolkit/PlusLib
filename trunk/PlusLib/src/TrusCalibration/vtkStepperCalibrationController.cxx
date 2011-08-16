#include "PlusConfigure.h"
#include "PlusMath.h"
#include "vtkStepperCalibrationController.h"
#include "vtkObjectFactory.h"
#include "vtkTransform.h"
#include "vtkMath.h"
#include <numeric>
#include <time.h>
#include "vtksys/SystemTools.hxx"
#include "vtkGnuplotExecuter.h"
#include "vtkHTMLGenerator.h"
#include "vtkMeanShiftClustering.h"
#include "vtkTable.h"
#include "vtkDoubleArray.h"
#include "vtkVariantArray.h"

#include "vnl/vnl_sparse_matrix.h"   
#include "vnl/vnl_sparse_matrix_linear_system.h"  
#include "vnl/algo/vnl_lsqr.h"  
#include "vnl/vnl_cross.h"  

//----------------------------------------------------------------------------
vtkCxxRevisionMacro(vtkStepperCalibrationController, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkStepperCalibrationController); 

//----------------------------------------------------------------------------
vtkStepperCalibrationController::vtkStepperCalibrationController()
{
  this->SetSpacing(0,0); 
  this->SetCenterOfRotationPx(0,0); 
  this->SetProbeTranslationAxisOrientation(0,0,1); 
  this->SetTemplateTranslationAxisOrientation(0,0,1);
  this->SetProbeRotationAxisOrientation(0,0,1); 
  this->SetProbeRotationEncoderOffset(0.0); 
  this->SetProbeRotationEncoderScale(1.0); 
  this->SetMinNumberOfRotationClusters(4); 
  this->SpacingCalculatedOff(); 
  this->CenterOfRotationCalculatedOff(); 
  this->PhantomToProbeDistanceCalculatedOff(); 
  this->ProbeRotationAxisCalibratedOff(); 
  this->ProbeTranslationAxisCalibratedOff(); 
  this->ProbeRotationEncoderCalibratedOff(); 
  this->TemplateTranslationAxisCalibratedOff(); 
  this->SetOutlierDetectionThreshold(3.0); 
  this->MinNumOfFramesUsedForCenterOfRotCalc = 25; 

  this->AlgorithmVersion = NULL; 
  this->CalibrationStartTime = NULL; 
  this->ProbeRotationAxisCalibrationErrorReportFilePath = NULL; 
  this->ProbeTranslationAxisCalibrationErrorReportFilePath = NULL; 
  this->TemplateTranslationAxisCalibrationErrorReportFilePath = NULL; 
  this->ProbeRotationEncoderCalibrationErrorReportFilePath = NULL; 
  this->SpacingCalculationErrorReportFilePath = NULL; 
  this->CenterOfRotationCalculationErrorReportFilePath = NULL; 

  this->SaveCalibrationStartTime(); 

  this->SetAlgorithmVersion("1.0.0"); 
}

//----------------------------------------------------------------------------
vtkStepperCalibrationController::~vtkStepperCalibrationController()
{
  this->SetCalibrationStartTime(NULL); 
  this->SetProbeRotationAxisCalibrationErrorReportFilePath(NULL); 
  this->SetProbeTranslationAxisCalibrationErrorReportFilePath(NULL); 
  this->SetTemplateTranslationAxisCalibrationErrorReportFilePath(NULL); 
  this->SetProbeRotationEncoderCalibrationErrorReportFilePath(NULL); 
  this->SetSpacingCalculationErrorReportFilePath(NULL); 
  this->SetCenterOfRotationCalculationErrorReportFilePath(NULL); 
}

//----------------------------------------------------------------------------
void vtkStepperCalibrationController::PrintSelf(std::ostream &os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}


//----------------------------------------------------------------------------
PlusStatus vtkStepperCalibrationController::Initialize()
{
  LOG_TRACE("vtkStepperCalibrationController::Initialize"); 

  if ( this->GetSegParameters() == NULL )
  {
    LOG_DEBUG("SegParameters is NULL, create one..."); 
    this->SegParameters = new SegmentationParameters(); 
  }

  // Initialize the segmenation component
  // ====================================
  this->mptrAutomatedSegmentation = new KPhantomSeg( 
	  this->GetSegParameters()->GetFrameSize(),this->GetSegParameters()->GetRegionOfInterest(), this->GetEnableSegmentationAnalysis(), "frame.jpg");

  this->InitializedOn(); 

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkStepperCalibrationController::AddVtkImageData( vtkImageData* frame, const double probePosition, const double probeRotation, const double templatePosition, IMAGE_DATA_TYPE dataType )
{
  LOG_TRACE("vtkCalibrationController::AddData - vtkImage with stepper encoder values"); 
  ImageType::Pointer exportedFrame = ImageType::New();
  if ( UsImageConverterCommon::ConvertVtkImageToItkImage(frame, exportedFrame) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to convert vtk image to itk image!"); 
    return PLUS_FAIL; 
  }

  return this->AddItkImageData(exportedFrame, probePosition, probeRotation, templatePosition, dataType); 
}

//----------------------------------------------------------------------------
PlusStatus vtkStepperCalibrationController::AddItkImageData( const ImageType::Pointer& frame, const double probePosition, const double probeRotation, const double templatePosition, IMAGE_DATA_TYPE dataType )
{
  LOG_TRACE("vtkCalibrationController::AddData - itkImage with stepper encoder values"); 
  TrackedFrame trackedFrame; 
  this->CreateTrackedFrame(frame, probePosition, probeRotation, templatePosition, dataType, trackedFrame); 
  return this->AddTrackedFrameData(&trackedFrame, dataType); 
}

//----------------------------------------------------------------------------
PlusStatus vtkStepperCalibrationController::ComputeStatistics(const std::vector< std::vector<double> > &diffVector, std::vector<CalibStatistics> &statistics)
{
  // copy differences to vnl_vectors 
  double min(0), max(0);
  statistics.clear();
  std::vector< vnl_vector<double> > posDifferences; 
  for ( unsigned int i = 0; i < diffVector.size(); ++i )
  {
    CalibStatistics stat;
    vnl_vector<double> diff(diffVector[i].size()); 
    for ( unsigned int j = 0; j < diffVector[i].size(); ++j )
    {
      diff[j] = diffVector[i][j]; 
      if ( min > diff[j] )
      {
        min = diff[j]; 
      }

      if ( max < diff[j] ) 
      {
        max = diff[j]; 
      }
    }
    stat.Max = max; 
    stat.Min = min; 
    statistics.push_back(stat); 
    posDifferences.push_back(diff); 
  }

  // calculate mean of difference between the computed and measured position for each wire 
  std::vector< vnl_vector<double> > diffFromMean; 
  for ( unsigned int i = 0; i < posDifferences.size(); i++ )
  {
    const double meanValue = posDifferences[i].mean(); 
    statistics[i].Mean = meanValue; 
    vnl_vector<double> diff = posDifferences[i] - meanValue; 
    diffFromMean.push_back(diff); 
  }

  // calculate standard deviation for each axis 
  for (unsigned int i = 0; i < diffFromMean.size(); i++ )
  {
    const double std = sqrt( diffFromMean[i].squared_magnitude() / (1.0 * diffFromMean[i].size()) ); 
    statistics[i].Stdev = std; 
  }

  // print results
  for ( unsigned int i = 0; i < statistics.size(); i++ )
  {
    LOG_DEBUG("Mean=" << std::fixed << statistics[i].Mean << " Std=" << statistics[i].Stdev); 
  }

  return PLUS_SUCCESS;
}

//***************************************************************************
//						Rotation axis calibration
//***************************************************************************

//----------------------------------------------------------------------------
PlusStatus vtkStepperCalibrationController::CalibrateProbeRotationAxis()
{
  LOG_TRACE("vtkStepperCalibrationController::CalibrateProbeRotationAxis"); 
  if ( ! this->GetInitialized() ) 
  {
    this->Initialize(); 
  }

  if ( !this->CalculateSpacing() )
  {
    LOG_ERROR("Failed to calibrate probe rotation axis without spacing information!"); 
    return PLUS_FAIL;
  }

  LOG_INFO( "----------------------------------------------------"); 
  LOG_INFO( ">>>>>>>>>>>> Rotation axis calibration ..."); 
  this->ProbeRotationAxisCalibratedOff(); 
  if ( this->CalibrateRotationAxis() )
  {
    if ( this->GetCenterOfRotationCalculated() )
    {
      LOG_INFO("CenterOfRotation (px): " << this->GetCenterOfRotationPx()[0] << "  " << this->GetCenterOfRotationPx()[1]); 
      LOG_INFO("CenterOfRotation (mm): " << this->GetCenterOfRotationPx()[0]*this->GetSpacing()[0] << "  " << this->GetCenterOfRotationPx()[1]*this->GetSpacing()[1]); 
    }
    if ( this->GetProbeRotationAxisCalibrated() )
    {
      LOG_INFO("Probe rotation axis orientation: Rx=" << std::fixed << this->GetProbeRotationAxisOrientation()[0] << "  Ry=" << this->GetProbeRotationAxisOrientation()[1]); 
    }
  }
  else
  {
    LOG_ERROR("Failed to calibrate probe rotation axis!"); 
    return PLUS_FAIL; 
  }


  LOG_INFO( "----------------------------------------------------"); 
  LOG_INFO( ">>>>>>>>>>>> Rotation encoder calibration ..."); 
  this->ProbeRotationEncoderCalibratedOff(); 
  if ( this->CalibrateRotationEncoder() )
  {
    if ( this->GetProbeRotationEncoderCalibrated() )
    {
      LOG_INFO("ProbeRotationEncoderScale = " << ProbeRotationEncoderScale); 
      LOG_INFO("ProbeRotationEncoderOffset = " << ProbeRotationEncoderOffset); 
    }
  }
  else
  {
    LOG_ERROR("Failed to calibrate probe rotation encoder!"); 
    return PLUS_FAIL; 
  }

  LOG_INFO( "----------------------------------------------------"); 
  LOG_INFO( ">>>>>>>>>>>> Phantom to probe distance calculation ..."); 
  this->PhantomToProbeDistanceCalculatedOff(); 
  if ( this->CalculatePhantomToProbeDistance() )
  {
    if ( this->GetPhantomToProbeDistanceCalculated() )
    {
      LOG_INFO("Phantom to probe distance: " << this->GetPhantomToProbeDistanceInMm()[0] << "  " << this->GetPhantomToProbeDistanceInMm()[1]); 
    }
  }
  else
  {
    LOG_ERROR("Failed to calculate phantom to probe distance!"); 
  }

  // save the input images to meta image
  if ( this->GetEnableTrackedSequenceDataSaving() )
  {
    LOG_INFO( "----------------------------------------------------"); 
    LOG_INFO(">>>>>>>> Save probe rotation data to sequence metafile..."); 
    // Save calibration dataset 
    std::ostringstream probeRotationDataFileName; 
    probeRotationDataFileName << this->CalibrationStartTime << this->GetImageDataInfo(PROBE_ROTATION).OutputSequenceMetaFileSuffix; 
    if ( this->SaveTrackedFrameListToMetafile( PROBE_ROTATION, this->GetOutputPath(), probeRotationDataFileName.str().c_str(), false ) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to save tracked frames to sequence metafile!"); 
    }
  }

  this->ClearSegmentedFrameContainer(PROBE_ROTATION); 

  // Set calibration date
  this->SetCalibrationDate(vtksys::SystemTools::GetCurrentDateTime("%Y.%m.%d %X").c_str()); 

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus vtkStepperCalibrationController::CalibrateRotationAxis()
{
  LOG_TRACE("vtkStepperCalibrationController::CalibrateRotationAxis"); 

  std::vector<SegmentedFrameList> clusteredFrames; 
  this->ClusterSegmentedFrames(PROBE_ROTATION, clusteredFrames); 

  vtkSmartPointer<vtkTable> centerOfRotationCalculationErrorTable = vtkSmartPointer<vtkTable>::New(); 
  std::ostringstream centerOfRotReportFileName; 
  centerOfRotReportFileName << this->OutputPath << "/" << this->CalibrationStartTime  << ".CenterOfRotationCalculationError.txt"; 
  this->SetCenterOfRotationCalculationErrorReportFilePath(centerOfRotReportFileName.str().c_str()); 

  if ( clusteredFrames.size() < this->MinNumberOfRotationClusters)
  {
    LOG_WARNING("Unable to calibrate rotation axis reliably: Number of rotation clusters are less than the minimum requirements (" << clusteredFrames.size() << " of " << this->MinNumberOfRotationClusters << ")." ); 
  }

  if ( clusteredFrames.size()==0 )
  {
      LOG_ERROR("Failed to calibrate rotation axis: Unable to find any rotation clusters!" ); 
      return PLUS_FAIL; 
  }

  if ( clusteredFrames.size()==1 )
  {
    // Not enough clusters to use LSQR fitting, so just use the one single cluster result
    double centerOfRotationPx[2] = {0, 0}; 
    this->CalculateCenterOfRotation(clusteredFrames[0], centerOfRotationPx, centerOfRotationCalculationErrorTable); 
    if ( centerOfRotationCalculationErrorTable )
    {
      vtkGnuplotExecuter::DumpTableToFileInGnuplotFormat(centerOfRotationCalculationErrorTable, this->GetCenterOfRotationCalculationErrorReportFilePath()); 
    }
    this->SetCenterOfRotationPx( centerOfRotationPx[0], centerOfRotationPx[1]); 
    this->CenterOfRotationCalculatedOn(); 

    if ( centerOfRotationCalculationErrorTable )
    {
      vtkGnuplotExecuter::DumpTableToFileInGnuplotFormat(centerOfRotationCalculationErrorTable, this->GetCenterOfRotationCalculationErrorReportFilePath()); 
    }

    return PLUS_SUCCESS; 
  }

  std::vector< std::pair<double, double> > listOfCenterOfRotations; 
  std::vector< double > listOfClusterPositions; 

  for ( unsigned int cluster = 0; cluster < clusteredFrames.size(); ++cluster )
  {
    if ( clusteredFrames[cluster].size() > this->MinNumOfFramesUsedForCenterOfRotCalc )
    {
      double clusterPosition = this->GetClusterZPosition(clusteredFrames[cluster]); 
      double centerOfRotationPx[2] = {0, 0}; 
      if ( this->CalculateCenterOfRotation(clusteredFrames[cluster], centerOfRotationPx, centerOfRotationCalculationErrorTable) )
      {
        LOG_INFO("Center of rotation in pixels for cluster #" << cluster << " at " << std::fixed << clusterPosition << " mm: " << centerOfRotationPx[0] << "   " << centerOfRotationPx[1]); 
        std::pair<double, double> cor (centerOfRotationPx[0], centerOfRotationPx[1]); 
        listOfCenterOfRotations.push_back( cor ); 
        listOfClusterPositions.push_back(clusterPosition); 
      }
      else
      {
        LOG_WARNING("Failed to compute center of rotation for cluster #" << cluster << " at " << std::fixed << clusterPosition << " mm"); 
      }
    }
  }

  if ( centerOfRotationCalculationErrorTable )
  {
    vtkGnuplotExecuter::DumpTableToFileInGnuplotFormat(centerOfRotationCalculationErrorTable, this->GetCenterOfRotationCalculationErrorReportFilePath()); 
  }

  // Construct linear equations Ax = b, where A is a matrix with m rows and 
  // n columns, b is an m-vector. 
  std::vector<vnl_vector<double>> aMatrix;
  std::vector<double> bVector; 

  // Construct linear equation for rotation axis calibration
  this->ConstrLinEqForRotationAxisCalib(listOfCenterOfRotations, listOfClusterPositions, aMatrix, bVector); 

  // [rx, ry, rx0, ry0 ]
  vnl_vector<double> rotationAxisCalibResult(4, 0);

  int numberOfEquations = bVector.size(); 
  int numberOfVariables = 0;
  if (aMatrix.size() > 0) {
    numberOfVariables = aMatrix[0].size();
  }

  if ((numberOfVariables == 0) || (numberOfEquations < numberOfVariables))
  {
    LOG_ERROR("There are more variables (" << numberOfVariables << ") than equations (" << numberOfEquations << "), least squares cannot be started!");
    return PLUS_FAIL;
  }

  do 
  {
    if ( PlusMath::LSQRMinimize(aMatrix, bVector, rotationAxisCalibResult) == PLUS_SUCCESS )
    {
      this->RemoveOutliersFromRotAxisCalibData(aMatrix, bVector, rotationAxisCalibResult); 
    }
    else
    {
      LOG_WARNING("Failed to run LSQRMinimize!"); 
    }
  } 
  while (numberOfEquations != bVector.size()); 

  if ( rotationAxisCalibResult.empty() )
  {
    LOG_ERROR("Unable to calibrate rotation axis! Minimizer returned empty result."); 
    return PLUS_FAIL; 
  }

  // Calculate mean error and stdev of measured and computed wire positions for each wire
  std::vector<CalibStatistics> statistics;
  this->GetRotationAxisCalibrationError(aMatrix, bVector, rotationAxisCalibResult, statistics); 
  this->SaveRotationAxisCalibrationError(aMatrix, bVector, rotationAxisCalibResult); 

  LOG_INFO("RotationAxisCalibResult: " << std::fixed << rotationAxisCalibResult[0] << "  " << rotationAxisCalibResult[1] << "  " << rotationAxisCalibResult[2] << "  " << rotationAxisCalibResult[3] ); 

  this->SetCenterOfRotationPx( rotationAxisCalibResult[2] / this->GetSpacing()[0], rotationAxisCalibResult[3] / this->GetSpacing()[1]); 
  this->CenterOfRotationCalculatedOn(); 
  
  // Set rotation axis orientation 
  // NOTE: If the probe goes down the wires goes down on the MF oriented image 
  // => we need to change the sign of the axis to compensate it
  this->SetProbeRotationAxisOrientation(-rotationAxisCalibResult[0], rotationAxisCalibResult[1], 1); 

  this->ProbeRotationAxisCalibratedOn(); 

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
void vtkStepperCalibrationController::ConstrLinEqForRotationAxisCalib( const std::vector< std::pair<double, double> > &listOfCenterOfRotations, 
                                                                      const std::vector< double > &listOfClusterPositions, 
                                                                      std::vector<vnl_vector<double>> &aMatrix, 
                                                                      std::vector<double> &bVector)
{
  LOG_TRACE("vtkStepperCalibrationController::ConstrLinEqForRotationAxisCalib"); 
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

    LOG_DEBUG("ConstrLinEqForRotationAxisCalib (rotX, rotY, probeZ): " << b1 << "  " << b2 << "  " << z ); 
  }
}

//----------------------------------------------------------------------------
void vtkStepperCalibrationController::GetRotationAxisCalibrationError(const std::vector<vnl_vector<double>> &aMatrix, 
                                                                      const std::vector<double> &bVector, 
                                                                      const vnl_vector<double> &resultVector, 
                                                                      std::vector<CalibStatistics> &statistics )
{
  LOG_TRACE("vtkStepperCalibrationController::GetRotationAxisCalibrationError"); 
  // The coefficient matrix aMatrix should be m-by-n and the column vector bVector must have length m.
  const int n = aMatrix.begin()->size(); 
  const int m = bVector.size();
  const int r = resultVector.size(); 

  const int numberOfAxes(2); 
  const double rx = resultVector[0]; 
  const double ry = resultVector[1]; 
  const double rx0 = resultVector[2]; 
  const double ry0 = resultVector[3]; 


  // calculate difference between the computed and measured position for each wire 
  std::vector< std::vector<double> > diffVector(numberOfAxes); 
  for( int row = 0; row < m; row = row + numberOfAxes)
  {
    diffVector[0].push_back( bVector[row    ] - rx0 - aMatrix[row    ].get(0) * rx ); 
    diffVector[1].push_back( bVector[row + 1] - ry0 - aMatrix[row + 1].get(1) * ry ); 
  }

  this->ComputeStatistics(diffVector, statistics);
}

//----------------------------------------------------------------------------
void vtkStepperCalibrationController::RemoveOutliersFromRotAxisCalibData(std::vector<vnl_vector<double>> &aMatrix, std::vector<double> &bVector, vnl_vector<double> resultVector )
{
  LOG_TRACE("vtkStepperCalibrationController::RemoveOutliersFromRotAxisCalibData"); 
  // Calculate mean error and stdev of measured and computed rotation angles
  std::vector<CalibStatistics> statistics;
  this->GetRotationAxisCalibrationError(aMatrix, bVector, resultVector, statistics); 

  const int n = aMatrix.begin()->size(); 
  const int m = bVector.size();
  const int r = resultVector.size(); 

  const int numberOfAxes(2); 
  const double rx = resultVector[0]; 
  const double ry = resultVector[1]; 
  const double rx0 = resultVector[2]; 
  const double ry0 = resultVector[3]; 

  // remove outliers
  for( int row = m - numberOfAxes; row >= 0; row = row - numberOfAxes)
  {
    if (abs ( bVector[row     ] - rx0 - aMatrix[row    ].get(0) * rx - statistics[0].Mean ) >  this->OutlierDetectionThreshold * statistics[0].Stdev 
    ||
      abs ( bVector[row + 1] - ry0 - aMatrix[row + 1].get(1) * ry - statistics[1].Mean ) >  this->OutlierDetectionThreshold * statistics[1].Stdev 
    )

    {
      LOG_DEBUG("Outlier found at row " << row ); 
      aMatrix.erase(aMatrix.begin() + row, aMatrix.begin() + row + numberOfAxes); 
      bVector.erase(bVector.begin() + row, bVector.begin() + row + numberOfAxes); 
    }
  }

}

//----------------------------------------------------------------------------
PlusStatus vtkStepperCalibrationController::GenerateProbeRotationAxisCalibrationReport( vtkHTMLGenerator* htmlReport, vtkGnuplotExecuter* plotter, const char* gnuplotScriptsFolder)
{
  if ( htmlReport == NULL || plotter == NULL )
  {
    LOG_ERROR("Caller should define HTML report generator and gnuplot plotter before report generation!"); 
    return PLUS_FAIL; 
  }

  std::string plotProbeRotationAxisCalibrationErrorScript = gnuplotScriptsFolder + std::string("/PlotProbeRotationAxisCalibrationError.gnu"); 
  if ( !vtksys::SystemTools::FileExists( plotProbeRotationAxisCalibrationErrorScript.c_str(), true) )
  {
    LOG_ERROR("Unable to find gnuplot script at: " << plotProbeRotationAxisCalibrationErrorScript); 
    return PLUS_FAIL; 
  }

  if ( this->GetProbeRotationAxisCalibrated() )
  {
    const char* reportFile = this->GetProbeRotationAxisCalibrationErrorReportFilePath(); 
    if ( reportFile == NULL )
    {
      LOG_ERROR("Failed to generate probe rotation axis calibration report - report file name is NULL!"); 
      return PLUS_FAIL; 
    }

    if ( !vtksys::SystemTools::FileExists( reportFile, true) )
    {
      LOG_ERROR("Unable to find rotation axis calibration report file at: " << reportFile); 
      return PLUS_FAIL; 
    }

    std::string title; 
    std::string scriptOutputFilePrefixHistogram, scriptOutputFilePrefix; 
    title = "Probe Rotation Axis Calibration Analysis"; 
    scriptOutputFilePrefix = "PlotProbeRotationAxisCalibrationError"; 

    htmlReport->AddText(title.c_str(), vtkHTMLGenerator::H1); 

    std::ostringstream report; 
    report << "Probe rotation axis orientation: " << this->GetProbeRotationAxisOrientation()[0] << "     " << this->GetProbeRotationAxisOrientation()[1] << "     " << this->GetProbeRotationAxisOrientation()[2] << "</br>" ; 
    htmlReport->AddParagraph(report.str().c_str()); 

    htmlReport->AddText("Error Plot", vtkHTMLGenerator::H2); 
    plotter->ClearArguments(); 
    plotter->AddArgument("-e");
    std::ostringstream rotAxisError; 
    rotAxisError << "f='" << reportFile << "'; o='" << scriptOutputFilePrefix << "';" << std::ends; 
    plotter->AddArgument(rotAxisError.str().c_str()); 
    plotter->AddArgument(plotProbeRotationAxisCalibrationErrorScript.c_str());  
    if ( plotter->Execute() != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to run gnuplot executer!"); 
      return PLUS_FAIL; 
    }
    plotter->ClearArguments(); 

    std::ostringstream imageSourceX, imageAltX, imageSourceY, imageAltY; 
    imageSourceX << "x_" << scriptOutputFilePrefix << ".jpg" << std::ends; 
    imageAltX << "Probe rotation axis calibration error - X axis" << std::ends; 
    imageSourceY << "y_" << scriptOutputFilePrefix << ".jpg" << std::ends; 
    imageAltY << "Probe rotation axis calibration error - Y axis" << std::ends; 

    htmlReport->AddImage(imageSourceX.str().c_str(), imageAltX.str().c_str()); 
    htmlReport->AddImage(imageSourceY.str().c_str(), imageAltY.str().c_str()); 

    htmlReport->AddHorizontalLine(); 
  }

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
void vtkStepperCalibrationController::SaveRotationAxisCalibrationError(
  const std::vector<vnl_vector<double>> &aMatrix, 
  const std::vector<double> &bVector, 
  const vnl_vector<double> &resultVector)
{

  LOG_TRACE("vtkStepperCalibrationController::SaveRotationAxisCalibrationError"); 
  const int numberOfAxes(2); 
  std::ostringstream filename; 
  std::ostringstream path; 

  std::ofstream rotationAxisCalibrationError;
  path << this->OutputPath << "/" << this->CalibrationStartTime  << ".ProbeRotationAxisCalibrationError.txt"; 
  this->SetProbeRotationAxisCalibrationErrorReportFilePath(path.str().c_str()); 
  rotationAxisCalibrationError.open (path.str().c_str(), ios::out);
  rotationAxisCalibrationError << "# Probe rotation axis calibration error report" << std::endl; 

  rotationAxisCalibrationError << "ProbePosition\t"
    << "MeasuredCenterOfRotationXInImageMm\tMeasuredCenterOfRotationYInImageMm\t" 
    << "ComputedCenterOfRotationXInImageMm\tComputedCenterOfRotationYInImageMm\t" 
    << std::endl; 

  for( int row = 0; row < bVector.size(); row = row + numberOfAxes)
  {
    rotationAxisCalibrationError << aMatrix[row    ].get(0) << "\t"
      << bVector[row] << "\t" << bVector[row+1] << "\t" 
      << resultVector[2] + aMatrix[row    ].get(0) * resultVector[0] << "\t" 
      << resultVector[3] + aMatrix[row + 1].get(1) * resultVector[1] << "\t"
      << std::endl; 
  }

  rotationAxisCalibrationError.close(); 
}

//***************************************************************************
//						Rotation encoder calibration
//***************************************************************************


//----------------------------------------------------------------------------
PlusStatus vtkStepperCalibrationController::CalibrateRotationEncoder()
{
  LOG_TRACE("vtkStepperCalibrationController::CalibrateRotationEncoder"); 
  if ( !this->CalculateSpacing() )
  {
    LOG_ERROR("Unable to calibrate rotation encoder without spacing information!"); 
    return PLUS_FAIL; 
  }

  // Construct linear equations Ax = b, where A is a matrix with m rows and 
  // n columns, b is an m-vector. 
  std::vector<vnl_vector<double>> aMatrix;
  std::vector<double> bVector;

  // Construct linear equation for rotation encoder calibration
  this->ConstrLinEqForRotEncCalc(aMatrix, bVector); 

  if ( aMatrix.size() == 0 || bVector.size() == 0 )
  {
    LOG_WARNING("Rotation encoder calibration failed, no data found!"); 
    return PLUS_FAIL; 
  }

  vnl_vector<double> rotationEncoderCalibrationResult(2,0);
  int numberOfEquations(0); 
  do
  {
    numberOfEquations = bVector.size(); 
    if ( PlusMath::LSQRMinimize(aMatrix, bVector, rotationEncoderCalibrationResult) == PLUS_SUCCESS )
    {
      this->RemoveOutliersFromRotEncCalibData(aMatrix, bVector, rotationEncoderCalibrationResult); 
    }
    else
    {
      LOG_WARNING("Failed to run LSQRMinimize!"); 
    }
  }
  while ( numberOfEquations != bVector.size() ); 

  if ( rotationEncoderCalibrationResult.empty() )
  {
    LOG_ERROR("Unable to calibrate rotation encoder! Minimizer returned empty result."); 
    return PLUS_FAIL; 
  }

  // Calculate mean error and stdev of measured and computed wire positions for each wire
  CalibStatistics statistics; 
  this->GetRotationEncoderCalibrationError(aMatrix, bVector, rotationEncoderCalibrationResult, statistics); 
  this->SaveRotationEncoderCalibrationError(aMatrix, bVector, rotationEncoderCalibrationResult); 

  this->SetProbeRotationEncoderScale(rotationEncoderCalibrationResult.get(0)); 
  this->SetProbeRotationEncoderOffset(rotationEncoderCalibrationResult.get(1)); 

  this->ProbeRotationEncoderCalibratedOn(); 

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
void vtkStepperCalibrationController::ConstrLinEqForRotEncCalc( std::vector<vnl_vector<double>> &aMatrix, std::vector<double> &bVector)
{
  LOG_TRACE("vtkStepperCalibrationController::ConstrLinEqForRotEncCalc"); 
  aMatrix.clear(); 
  bVector.clear(); 

  for ( int frame = 0; frame < this->SegmentedFrameContainer.size(); frame++ )
  {
    if ( this->SegmentedFrameContainer[frame].DataType == PROBE_ROTATION )
    {
      double probePos(0), probeRot(0), templatePos(0); 
      if ( !this->GetStepperEncoderValues(this->SegmentedFrameContainer[frame].TrackedFrameInfo, probePos, probeRot, templatePos) )
      {
        LOG_WARNING("Probe rotation axis calibration: Unable to get probe rotation from tracked frame info for frame #" << frame); 
        continue; 
      }

      // Wire #1 coordinate in mm 
      double w1xmm = this->SegmentedFrameContainer[frame].SegResults.GetFoundDotsCoordinateValue()[WIRE1][0] * this->GetSpacing()[0]; 
      double w1ymm = this->SegmentedFrameContainer[frame].SegResults.GetFoundDotsCoordinateValue()[WIRE1][1] * this->GetSpacing()[1]; 

      // Wire #3 coordinate in mm 
      double w3xmm = this->SegmentedFrameContainer[frame].SegResults.GetFoundDotsCoordinateValue()[WIRE3][0] * this->GetSpacing()[0]; 
      double w3ymm = this->SegmentedFrameContainer[frame].SegResults.GetFoundDotsCoordinateValue()[WIRE3][1] * this->GetSpacing()[1]; 

      // Wire #4 coordinate in mm 
      double w4xmm = this->SegmentedFrameContainer[frame].SegResults.GetFoundDotsCoordinateValue()[WIRE4][0] * this->GetSpacing()[0]; 
      double w4ymm = this->SegmentedFrameContainer[frame].SegResults.GetFoundDotsCoordinateValue()[WIRE4][1] * this->GetSpacing()[1]; 

      // Wire #6 coordinate in mm 
      double w6xmm = this->SegmentedFrameContainer[frame].SegResults.GetFoundDotsCoordinateValue()[WIRE6][0] * this->GetSpacing()[0]; 
      double w6ymm = this->SegmentedFrameContainer[frame].SegResults.GetFoundDotsCoordinateValue()[WIRE6][1] * this->GetSpacing()[1]; 

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

    }
  }

}

//----------------------------------------------------------------------------
void vtkStepperCalibrationController::GetRotationEncoderCalibrationError(const std::vector<vnl_vector<double>> &aMatrix, 
                                                                         const std::vector<double> &bVector, 
                                                                         const vnl_vector<double> &resultVector, 
                                                                         CalibStatistics &statistics ) 
{
  LOG_TRACE("vtkStepperCalibrationController::GetRotationEncoderCalibrationError"); 
  // The coefficient matrix aMatrix should be m-by-n and the column vector bVector must have length m.
  const int n = aMatrix.begin()->size(); 
  const int m = bVector.size();
  const int r = resultVector.size(); 

  const double scale = resultVector[0]; 
  const double offset = resultVector[1]; 

  // calculate difference between the computed and measured angles
  std::vector< std::vector<double> > diffVector;
  std::vector<double> diff; 
  for( int row = 0; row < m; row++ )
  {
    diff.push_back( bVector[row] - offset - aMatrix[row].get(0) * scale ); 
  }
  diffVector.push_back(diff); 

  std::vector<CalibStatistics> stat; 
  this->ComputeStatistics(diffVector, stat); 

  // calculate mean of difference 
  if ( !stat.empty() )
  {
    statistics = stat[0]; 
  }
}

//----------------------------------------------------------------------------
void vtkStepperCalibrationController::RemoveOutliersFromRotEncCalibData(std::vector<vnl_vector<double>> &aMatrix, std::vector<double> &bVector, vnl_vector<double> resultVector )
{
  LOG_TRACE("vtkStepperCalibrationController::RemoveOutliersFromRotEncCalibData"); 
  // Calculate mean error and stdev of measured and computed rotation angles
  CalibStatistics statistics; 
  this->GetRotationEncoderCalibrationError(aMatrix, bVector, resultVector, statistics); 

  const int n = aMatrix.begin()->size(); 
  const int m = bVector.size();
  const int r = resultVector.size(); 

  const double scale = resultVector[0]; 
  const double offset = resultVector[1]; 

  // remove outliers
  for( int row = m - 1; row >= 0; row--)
  {
    if ( abs ( bVector[row] - offset - aMatrix[row].get(0) * scale - statistics.Mean ) >  this->OutlierDetectionThreshold * statistics.Stdev) 
    {
      LOG_DEBUG("Outlier found at row " << row ); 
      aMatrix.erase(aMatrix.begin() + row); 
      bVector.erase(bVector.begin() + row); 
    }
  }

}

//----------------------------------------------------------------------------
void vtkStepperCalibrationController::SaveRotationEncoderCalibrationError(const std::vector<vnl_vector<double>> &aMatrix, 
                                                                          const std::vector<double> &bVector, 
                                                                          const vnl_vector<double> &resultVector)
{
  LOG_TRACE("vtkStepperCalibrationController::SaveRotationEncoderCalibrationError"); 
  std::ofstream rotationEncoderCalibrationError;
  std::ostringstream filename; 
  filename << this->OutputPath << "/" << this->CalibrationStartTime  << ".ProbeRotationEncoderCalibrationError.txt"; 

  this->SetProbeRotationEncoderCalibrationErrorReportFilePath(filename.str().c_str()); 

  rotationEncoderCalibrationError.open (filename.str().c_str() , ios::out);
  rotationEncoderCalibrationError << "# Probe rotation encoder calibration error report" << std::endl; 

  rotationEncoderCalibrationError << "ProbeRotationAngle\t" << "ComputedAngle\t" << "CompensatedAngle\t" << std::endl; 

  const double scale = resultVector[0]; 
  const double offset = resultVector[1]; 

  for( int row = 0; row < bVector.size(); row ++)
  {
    rotationEncoderCalibrationError << aMatrix[row].get(0) << "\t" << bVector[row]  << "\t" << offset + aMatrix[row].get(0) * scale << "\t" << std::endl; 
  }

  rotationEncoderCalibrationError.close(); 

}


//----------------------------------------------------------------------------
PlusStatus vtkStepperCalibrationController::GenerateProbeRotationEncoderCalibrationReport( vtkHTMLGenerator* htmlReport, vtkGnuplotExecuter* plotter, const char* gnuplotScriptsFolder)
{
  if ( htmlReport == NULL || plotter == NULL )
  {
    LOG_ERROR("Caller should define HTML report generator and gnuplot plotter before report generation!"); 
    return PLUS_FAIL; 
  }

  std::string plotProbeRotationEncoderCalibrationErrorScript = gnuplotScriptsFolder + std::string("/PlotProbeRotationEncoderCalibrationError.gnu"); 
  if ( !vtksys::SystemTools::FileExists( plotProbeRotationEncoderCalibrationErrorScript.c_str(), true) )
  {
    LOG_ERROR("Unable to find gnuplot script at: " << plotProbeRotationEncoderCalibrationErrorScript); 
    return PLUS_FAIL; 
  }

  if ( this->GetProbeRotationEncoderCalibrated() )
  {
    const char * reportFile = this->GetProbeRotationEncoderCalibrationErrorReportFilePath(); 
    if ( reportFile == NULL )
    {
      LOG_ERROR("Failed to generate probe rotation encoder calibration report - report file name is NULL!"); 
      return PLUS_FAIL; 
    }

    if ( !vtksys::SystemTools::FileExists( reportFile, true) )
    {
      LOG_ERROR("Unable to find rotation encoder calibration report file at: " << reportFile); 
      return PLUS_FAIL; 
    }

    std::string title; 
    std::string scriptOutputFilePrefixHistogram, scriptOutputFilePrefix; 
    title = "Probe Rotation Encoder Calibration Analysis"; 
    scriptOutputFilePrefix = "PlotProbeRotationEncoderCalibrationError"; 

    htmlReport->AddText(title.c_str(), vtkHTMLGenerator::H1); 

    std::ostringstream report; 
    report << "Probe rotation encoder scale: " << this->GetProbeRotationEncoderScale() << "</br>" ; 
    report << "Probe rotation encoder offset: " << this->GetProbeRotationEncoderOffset() << "</br>" ; 
    htmlReport->AddParagraph(report.str().c_str()); 

    plotter->ClearArguments(); 
    plotter->AddArgument("-e");
    std::ostringstream rotEncoderError; 
    rotEncoderError << "f='" << reportFile << "'; o='" << scriptOutputFilePrefix << "';" << std::ends; 
    plotter->AddArgument(rotEncoderError.str().c_str()); 
    plotter->AddArgument(plotProbeRotationEncoderCalibrationErrorScript.c_str());  
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
  }

  return PLUS_SUCCESS; 
}


//***************************************************************************
//					Translation axis calibration
//***************************************************************************


//----------------------------------------------------------------------------
PlusStatus vtkStepperCalibrationController::CalibrateProbeTranslationAxis()
{
  LOG_TRACE("vtkStepperCalibrationController::CalibrateProbeTranslationAxis"); 
  if ( ! this->GetInitialized() ) 
  {
    this->Initialize(); 
  }

  this->ProbeTranslationAxisCalibratedOff(); 
  if ( !this->CalculateSpacing() )
  {
    LOG_ERROR("Failed to calibrate probe translation axis without spacing information!"); 
    return PLUS_FAIL; 
  }

  // Probe translation axis calibration 
  LOG_INFO( "----------------------------------------------------"); 
  LOG_INFO( ">>>>>>>>>>>> Probe translation axis calibration ..."); 
  if ( this->CalibrateTranslationAxis(PROBE_TRANSLATION) )
  {
    if ( this->GetProbeTranslationAxisCalibrated() )
    {
      LOG_INFO("Probe translation axis orientation: Tx=" << std::fixed << this->GetProbeTranslationAxisOrientation()[0] << "  Ty=" << this->GetProbeTranslationAxisOrientation()[1]); 
    }
  }
  else
  {
    LOG_ERROR("Failed to calibrate probe translation axis!"); 
  }

  // save the input images to meta image
  if ( this->GetEnableTrackedSequenceDataSaving() )
  {
    LOG_INFO( "----------------------------------------------------"); 
    LOG_INFO(">>>>>>>> Save probe translation data to sequence metafile..."); 
    // Save calibration dataset 
    std::ostringstream probeTranslationDataFileName; 
    probeTranslationDataFileName << this->CalibrationStartTime << this->GetImageDataInfo(PROBE_TRANSLATION).OutputSequenceMetaFileSuffix; 
    if ( this->SaveTrackedFrameListToMetafile( PROBE_TRANSLATION, this->GetOutputPath(), probeTranslationDataFileName.str().c_str(), false ) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to save tracked frames to sequence metafile!"); 
    }
  }

  this->ClearSegmentedFrameContainer(PROBE_TRANSLATION); 

  // Set calibration date
  this->SetCalibrationDate(vtksys::SystemTools::GetCurrentDateTime("%Y.%m.%d %X").c_str()); 

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkStepperCalibrationController::CalibrateTemplateTranslationAxis()
{
  LOG_TRACE("vtkStepperCalibrationController::CalibrateTemplateTranslationAxis"); 
  if ( ! this->GetInitialized() ) 
  {
    this->Initialize(); 
  }

  if ( !this->CalculateSpacing() )
  {
    LOG_ERROR("Failed to calibrate template translation axis without spacing information!"); 
    return PLUS_FAIL; 
  }

  // Template translation axis calibration 
  LOG_INFO( "----------------------------------------------------"); 
  LOG_INFO( ">>>>>>>>>>>> Template translation axis calibration ..."); 
  this->TemplateTranslationAxisCalibratedOff(); 
  if ( this->CalibrateTranslationAxis(TEMPLATE_TRANSLATION) )
  {
    if ( this->GetTemplateTranslationAxisCalibrated() )
    {
      LOG_INFO("Template translation axis orientation: Tx=" << std::fixed << this->GetTemplateTranslationAxisOrientation()[0] << "  Ty=" << this->GetTemplateTranslationAxisOrientation()[1]); 
    }
  }
  else
  {
    LOG_ERROR("Failed to calibrate template translation axis!"); 
    this->TemplateTranslationAxisCalibratedOff(); 
  }

  // save the input images to meta image
  if ( this->GetEnableTrackedSequenceDataSaving() )
  {
    LOG_INFO( "----------------------------------------------------"); 
    LOG_INFO(">>>>>>>> Save template translation data to sequence metafile..."); 
    // Save calibration dataset 
    std::ostringstream templateTranslationDataFileName; 
    templateTranslationDataFileName << this->CalibrationStartTime << this->GetImageDataInfo(TEMPLATE_TRANSLATION).OutputSequenceMetaFileSuffix; 
    if ( this->SaveTrackedFrameListToMetafile( TEMPLATE_TRANSLATION, this->GetOutputPath(), templateTranslationDataFileName.str().c_str(), false ) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to save tracked frames to sequence metafile!");
    }
  }

  this->ClearSegmentedFrameContainer(TEMPLATE_TRANSLATION); 

  if (!this->GetTemplateTranslationAxisCalibrated())
  {
    return PLUS_FAIL;
  }

  // Set calibration date
  this->SetCalibrationDate(vtksys::SystemTools::GetCurrentDateTime("%Y.%m.%d %X").c_str()); 

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkStepperCalibrationController::CalibrateTranslationAxis( IMAGE_DATA_TYPE dataType )
{
  LOG_TRACE("vtkStepperCalibrationController::CalibrateTranslationAxis"); 
  // Construct linear equations Ax = b, where A is a matrix with m rows and 
  // n columns, b is an m-vector. 
  std::vector<vnl_vector<double>> aMatrix;
  std::vector<double> bVector; 

  // Construct linear equation for translation axis calibration
  this->ConstrLinEqForTransAxisCalib(aMatrix, bVector, dataType); 

  if ( aMatrix.size() == 0 || bVector.size() == 0 )
  {
    LOG_WARNING("Translation axis calibration failed, no data found!"); 
    return PLUS_FAIL; 
  }

  // [tx, ty, w1x0, w1y0, w3x0, w3y0, w4x0, w4y0, w6x0, w6y0 ]
  vnl_vector<double> translationAxisCalibResult(10, 0);

  int numberOfEquations(0);
  do 
  {
    numberOfEquations = bVector.size(); 
    if ( PlusMath::LSQRMinimize(aMatrix, bVector, translationAxisCalibResult) == PLUS_SUCCESS)
    {
      this->RemoveOutliersFromTransAxisCalibData(aMatrix, bVector, translationAxisCalibResult); 
    }
    else
    {
      LOG_WARNING("Failed to run LSQRMinimize!"); 
    }
  }
  while ( numberOfEquations != bVector.size() ); 

  if ( translationAxisCalibResult.empty() )
  {
    LOG_ERROR("Unable to calibrate translation axis! Minimizer returned empty result."); 
    return PLUS_FAIL; 
  }

  // Calculate mean error and stdev of measured and computed wire positions for each wire
  std::vector<CalibStatistics> statistics; 
  this->GetTranslationAxisCalibrationError(aMatrix, bVector, translationAxisCalibResult, statistics); 

  this->SaveTranslationAxisCalibrationError(aMatrix, bVector, translationAxisCalibResult, dataType); 

  // Set translation axis orientation 
  // NOTE: If the probe goes down the wires goes down on the MF oriented image 
  // => we need to change the sign of the axis to compensate it
  if ( dataType == PROBE_TRANSLATION )
  {
    this->SetProbeTranslationAxisOrientation(-translationAxisCalibResult[0], translationAxisCalibResult[1], 1); 
    this->ProbeTranslationAxisCalibratedOn(); 
  }
  else if ( dataType == TEMPLATE_TRANSLATION )
  {
    this->SetTemplateTranslationAxisOrientation(-translationAxisCalibResult[0], translationAxisCalibResult[1], 1); 
    this->TemplateTranslationAxisCalibratedOn(); 
  }

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
void vtkStepperCalibrationController::ConstrLinEqForTransAxisCalib( std::vector<vnl_vector<double>> &aMatrix, std::vector<double> &bVector, IMAGE_DATA_TYPE dataType)
{
  LOG_TRACE("vtkStepperCalibrationController::ConstrLinEqForTransAxisCalib"); 
  aMatrix.clear(); 
  bVector.clear(); 

  for ( int frame = 0; frame < this->SegmentedFrameContainer.size(); frame++ )
  {
    if ( this->SegmentedFrameContainer[frame].DataType == dataType )
    {
      double z(0); 
      switch (dataType)
      {
      case PROBE_TRANSLATION: 
        {
          double probePos(0), probeRot(0), templatePos(0); 
          if ( !this->GetStepperEncoderValues(this->SegmentedFrameContainer[frame].TrackedFrameInfo, probePos, probeRot, templatePos) )
          {
            LOG_WARNING("Probe translation axis calibration: Unable to get probe position from tracked frame info for frame #" << frame); 
            continue; 
          }

          z = probePos; 
        }
        break; 
      case TEMPLATE_TRANSLATION: 
        {
          double probePos(0), probeRot(0), templatePos(0); 
          if ( !this->GetStepperEncoderValues(this->SegmentedFrameContainer[frame].TrackedFrameInfo, probePos, probeRot, templatePos) )
          {
            LOG_WARNING("Template translation axis calibration: Unable to get template position from tracked frame info for frame #" << frame); 
            continue; 
          }

          z = templatePos; 
        }

        break; 
      }

      // Wire #1 X coordinate in mm 
      double b1 = this->SegmentedFrameContainer[frame].SegResults.GetFoundDotsCoordinateValue()[WIRE1][0] * this->GetSpacing()[0]; 
      vnl_vector<double> a1(10,0); 
      a1.put(0, z); 
      a1.put(2, 1); 

      bVector.push_back(b1); 
      aMatrix.push_back(a1); 

      // Wire #1 Y coordinate in mm 
      double b2 = this->SegmentedFrameContainer[frame].SegResults.GetFoundDotsCoordinateValue()[WIRE1][1] * this->GetSpacing()[1]; 
      vnl_vector<double> a2(10,0); 
      a2.put(1, z); 
      a2.put(3, 1); 

      bVector.push_back(b2); 
      aMatrix.push_back(a2); 

      // Wire #3 X coordinate in mm 
      double b3 = this->SegmentedFrameContainer[frame].SegResults.GetFoundDotsCoordinateValue()[WIRE3][0] * this->GetSpacing()[0]; 
      vnl_vector<double> a3(10,0); 
      a3.put(0, z); 
      a3.put(4, 1); 

      bVector.push_back(b3); 
      aMatrix.push_back(a3); 

      // Wire #3 Y coordinate in mm 
      double b4 = this->SegmentedFrameContainer[frame].SegResults.GetFoundDotsCoordinateValue()[WIRE3][1] * this->GetSpacing()[1]; 
      vnl_vector<double> a4(10,0); 
      a4.put(1, z); 
      a4.put(5, 1); 

      bVector.push_back(b4); 
      aMatrix.push_back(a4); 

      // Wire #4 X coordinate in mm 
      double b5 = this->SegmentedFrameContainer[frame].SegResults.GetFoundDotsCoordinateValue()[WIRE4][0] * this->GetSpacing()[0]; 
      vnl_vector<double> a5(10,0); 
      a5.put(0, z); 
      a5.put(6, 1); 

      bVector.push_back(b5); 
      aMatrix.push_back(a5); 

      // Wire #4 Y coordinate in mm 
      double b6 = this->SegmentedFrameContainer[frame].SegResults.GetFoundDotsCoordinateValue()[WIRE4][1] * this->GetSpacing()[1]; 
      vnl_vector<double> a6(10,0); 
      a6.put(1, z); 
      a6.put(7, 1); 

      bVector.push_back(b6); 
      aMatrix.push_back(a6); 

      // Wire #6 X coordinate in mm 
      double b7 = this->SegmentedFrameContainer[frame].SegResults.GetFoundDotsCoordinateValue()[WIRE6][0] * this->GetSpacing()[0]; 
      vnl_vector<double> a7(10,0); 
      a7.put(0, z); 
      a7.put(8, 1); 

      bVector.push_back(b7); 
      aMatrix.push_back(a7); 

      // Wire #6 Y coordinate in mm 
      double b8 = this->SegmentedFrameContainer[frame].SegResults.GetFoundDotsCoordinateValue()[WIRE6][1] * this->GetSpacing()[1]; 
      vnl_vector<double> a8(10,0); 
      a8.put(1, z); 
      a8.put(9, 1); 

      bVector.push_back(b8); 
      aMatrix.push_back(a8); 
    }
  }

}

//----------------------------------------------------------------------------
void vtkStepperCalibrationController::GetTranslationAxisCalibrationError(const std::vector<vnl_vector<double>> &aMatrix, 
                                                                         const std::vector<double> &bVector, 
                                                                         const vnl_vector<double> &resultVector, 
                                                                         std::vector<CalibStatistics> &statistics)
{
  LOG_TRACE("vtkStepperCalibrationController::GetTranslationAxisCalibrationError"); 
  // The coefficient matrix aMatrix should be m-by-n and the column vector bVector must have length m.
  const int n = aMatrix.begin()->size(); 
  const int m = bVector.size();
  const int r = resultVector.size(); 

  const int numOfSegmentedPoints(8); 
  const double tx = resultVector[0]; 
  const double ty = resultVector[1]; 
  const double w1x = resultVector[2]; 
  const double w1y = resultVector[3]; 
  const double w3x = resultVector[4]; 
  const double w3y = resultVector[5]; 
  const double w4x = resultVector[6]; 
  const double w4y = resultVector[7]; 
  const double w6x = resultVector[8]; 
  const double w6y = resultVector[9]; 


  // calculate difference between the computed and measured position for each wire 
  std::vector< std::vector<double> > diffVector(numOfSegmentedPoints); 
  for( int row = 0; row < m; row = row + numOfSegmentedPoints)
  {
    diffVector[0].push_back( bVector[row    ] - w1x - aMatrix[row    ].get(0) * tx ); 
    diffVector[1].push_back( bVector[row + 1] - w1y - aMatrix[row + 1].get(1) * ty ); 

    diffVector[2].push_back( bVector[row + 2] - w3x - aMatrix[row + 2].get(0) * tx ); 
    diffVector[3].push_back( bVector[row + 3] - w3y - aMatrix[row + 3].get(1) * ty ); 

    diffVector[4].push_back( bVector[row + 4] - w4x - aMatrix[row + 4].get(0) * tx ); 
    diffVector[5].push_back( bVector[row + 5] - w4y - aMatrix[row + 5].get(1) * ty ); 

    diffVector[6].push_back( bVector[row + 6] - w6x - aMatrix[row + 6].get(0) * tx ); 
    diffVector[7].push_back( bVector[row + 7] - w6y - aMatrix[row + 7].get(1) * ty ); 
  }

  this->ComputeStatistics(diffVector, statistics); 
}

//----------------------------------------------------------------------------
void vtkStepperCalibrationController::SaveTranslationAxisCalibrationError(const std::vector<vnl_vector<double>> &aMatrix, 
                                                                          const std::vector<double> &bVector, 
                                                                          const vnl_vector<double> &resultVector, 
                                                                          IMAGE_DATA_TYPE dataType)
{
  LOG_TRACE("vtkStepperCalibrationController::SaveTranslationAxisCalibrationError"); 
  const int numOfSegmentedPoints(8); 
  std::ostringstream filename; 
  std::ostringstream path; 

  std::ofstream translationAxisCalibrationError;
  if ( dataType == PROBE_TRANSLATION )
  {
    path << this->OutputPath << "/" << this->CalibrationStartTime  << ".ProbeTranslationAxisCalibrationError.txt"; 
    this->SetProbeTranslationAxisCalibrationErrorReportFilePath(path.str().c_str()); 

    translationAxisCalibrationError.open (path.str().c_str(), ios::out);
    translationAxisCalibrationError << "# Probe translation axis calibration error report" << std::endl; 
  }
  else if ( dataType == TEMPLATE_TRANSLATION )
  {
    path << this->OutputPath << "/" << this->CalibrationStartTime  << ".TemplateTranslationAxisCalibrationError.txt"; 
    this->SetTemplateTranslationAxisCalibrationErrorReportFilePath(path.str().c_str()); 
    translationAxisCalibrationError.open (path.str().c_str(), ios::out);
    translationAxisCalibrationError << "# Template translation axis calibration error report" << std::endl; 
  }

  translationAxisCalibrationError << "ProbePosition\t"
    << "MeasuredWire1xInImageMm\tMeasuredWire1yInImageMm\tMeasuredWire3xInImage\tMeasuredWire3yInImageMm\tMeasuredWire4xInImageMm\tMeasuredWire4yInImageMm\tMeasuredWire6xInImageMm\tMeasuredWire6yInImageMm\t" 
    << "ComputedWire1xInImageMm\tComputedWire1yInImageMm\tComputedWire3xInImage\tComputedWire3yInImageMm\tComputedWire4xInImageMm\tComputedWire4yInImageMm\tComputedWire6xInImageMm\tComputedWire6yInImageMm\t" 
    << std::endl; 


  for( int row = 0; row < bVector.size(); row = row + numOfSegmentedPoints)
  {
    translationAxisCalibrationError << aMatrix[row    ].get(0) << "\t"
      << bVector[row] << "\t" << bVector[row+1] << "\t" << bVector[row+2] << "\t" << bVector[row+3] << "\t" 
      << bVector[row+4] << "\t" << bVector[row+5] << "\t" << bVector[row+6] << "\t" << bVector[row+7] << "\t" 
      << resultVector[2] + aMatrix[row    ].get(0) * resultVector[0] << "\t" 
      << resultVector[3] + aMatrix[row + 1].get(1) * resultVector[1] << "\t"
      << resultVector[4] + aMatrix[row + 2].get(0) * resultVector[0] << "\t"
      << resultVector[5] + aMatrix[row + 3].get(1) * resultVector[1] << "\t"
      << resultVector[6] + aMatrix[row + 4].get(0) * resultVector[0] << "\t"
      << resultVector[7] + aMatrix[row + 5].get(1) * resultVector[1] << "\t"
      << resultVector[8] + aMatrix[row + 6].get(0) * resultVector[0] << "\t"
      << resultVector[9] + aMatrix[row + 7].get(1) * resultVector[1] << "\t"
      << std::endl; 
  }

  translationAxisCalibrationError.close(); 
}

//----------------------------------------------------------------------------
void vtkStepperCalibrationController::RemoveOutliersFromTransAxisCalibData(std::vector<vnl_vector<double>> &aMatrix, std::vector<double> &bVector, const vnl_vector<double> &resultVector )
{
  LOG_TRACE("vtkStepperCalibrationController::RemoveOutliersFromTransAxisCalibData"); 
  // Calculate mean error and stdev of measured and computed wire positions for each wire
  std::vector<CalibStatistics> statistics; 
  this->GetTranslationAxisCalibrationError(aMatrix, bVector, resultVector, statistics); 

  const int n = aMatrix.begin()->size(); 
  const int m = bVector.size();
  const int r = resultVector.size(); 

  const int numOfSegmentedPoints(8); 
  const double tx = resultVector[0]; 
  const double ty = resultVector[1]; 
  const double w1x = resultVector[2]; 
  const double w1y = resultVector[3]; 
  const double w3x = resultVector[4]; 
  const double w3y = resultVector[5]; 
  const double w4x = resultVector[6]; 
  const double w4y = resultVector[7]; 
  const double w6x = resultVector[8]; 
  const double w6y = resultVector[9]; 

  // remove outliers
  for( int row = m - numOfSegmentedPoints; row > 0; row = row - numOfSegmentedPoints )
  {
    if ( abs ( bVector[row    ] - w1x - aMatrix[row    ].get(0) * tx - statistics[0].Mean ) >  this->OutlierDetectionThreshold * statistics[0].Stdev 
    ||
      abs ( bVector[row + 1] - w1y - aMatrix[row + 1].get(1) * ty - statistics[1].Mean ) >  this->OutlierDetectionThreshold * statistics[1].Stdev  
    ||
      abs ( bVector[row + 2] - w3x - aMatrix[row + 2].get(0) * tx - statistics[2].Mean ) >  this->OutlierDetectionThreshold * statistics[2].Stdev  
    ||
      abs ( bVector[row + 3] - w3y - aMatrix[row + 3].get(1) * ty - statistics[3].Mean ) >  this->OutlierDetectionThreshold * statistics[3].Stdev  
    ||
      abs ( bVector[row + 4] - w4x - aMatrix[row + 4].get(0) * tx - statistics[4].Mean ) >  this->OutlierDetectionThreshold * statistics[4].Stdev  
    ||
      abs ( bVector[row + 5] - w4y - aMatrix[row + 5].get(1) * ty - statistics[5].Mean ) >  this->OutlierDetectionThreshold * statistics[5].Stdev  
    ||
      abs ( bVector[row + 6] - w6x - aMatrix[row + 6].get(0) * tx - statistics[6].Mean ) >  this->OutlierDetectionThreshold * statistics[6].Stdev  
    ||
      abs ( bVector[row + 7] - w6y - aMatrix[row + 7].get(1) * ty - statistics[7].Mean ) >  this->OutlierDetectionThreshold * statistics[7].Stdev  
    )
    {
      LOG_DEBUG("Outlier found at row " << row ); 
      aMatrix.erase(aMatrix.begin() + row, aMatrix.begin() + row + numOfSegmentedPoints); 
      bVector.erase(bVector.begin() + row, bVector.begin() + row + numOfSegmentedPoints); 
    }
  }

}

//----------------------------------------------------------------------------
void vtkStepperCalibrationController::GenerateProbeTranslationAxisCalibrationReport( vtkHTMLGenerator* htmlReport, vtkGnuplotExecuter* plotter, const char* gnuplotScriptsFolder)
{
  LOG_TRACE("vtkStepperCalibrationController::GenerateProbeTranslationAxisCalibrationReport"); 
  this->GenerateTranslationAxisCalibrationReport(PROBE_TRANSLATION, htmlReport, plotter, gnuplotScriptsFolder); 
}

//----------------------------------------------------------------------------
void vtkStepperCalibrationController::GenerateTemplateTranslationAxisCalibrationReport( vtkHTMLGenerator* htmlReport, vtkGnuplotExecuter* plotter, const char* gnuplotScriptsFolder)
{
  LOG_TRACE("vtkStepperCalibrationController::GenerateTemplateTranslationAxisCalibrationReport"); 
  this->GenerateTranslationAxisCalibrationReport(TEMPLATE_TRANSLATION, htmlReport, plotter, gnuplotScriptsFolder); 
}

//----------------------------------------------------------------------------
PlusStatus vtkStepperCalibrationController::GenerateTranslationAxisCalibrationReport( IMAGE_DATA_TYPE dataType, vtkHTMLGenerator* htmlReport, vtkGnuplotExecuter* plotter, const char* gnuplotScriptsFolder)
{
  if ( htmlReport == NULL || plotter == NULL )
  {
    LOG_ERROR("Caller should define HTML report generator and gnuplot plotter before report generation!"); 
    return PLUS_FAIL; 
  }

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

  bool calibrated(false); 
  switch ( dataType )
  {
  case PROBE_TRANSLATION: 
    calibrated = this->GetProbeTranslationAxisCalibrated(); 
    break; 
  case TEMPLATE_TRANSLATION: 
    calibrated = this->GetTemplateTranslationAxisCalibrated(); 
    break; 
  }

  if ( calibrated )
  {
    std::string reportFile("");
    switch ( dataType )
    {
    case PROBE_TRANSLATION: 
      {
        const char * report = this->GetProbeTranslationAxisCalibrationErrorReportFilePath(); 
        if ( report == NULL )
        {
          LOG_ERROR("Failed to generate probe translation axis calibration report - report file name is NULL!"); 
          return PLUS_FAIL; 
        }
        reportFile = report; 
      }
      break; 
    case TEMPLATE_TRANSLATION: 
      {
        const char * report = this->GetTemplateTranslationAxisCalibrationErrorReportFilePath(); 
        if ( report == NULL )
        {
          LOG_ERROR("Failed to generate template translation axis calibration report - report file name is NULL!"); 
          return PLUS_FAIL; 
        }
        reportFile = report; 
      }
      break; 
    }

    

    if ( !vtksys::SystemTools::FileExists( reportFile.c_str(), true) )
    {
      LOG_ERROR("Unable to find translation axis calibration report file at: " << reportFile); 
      return PLUS_FAIL; 
    }

    std::ostringstream report; 
    std::string title; 
    std::string scriptOutputFilePrefixHistogram, scriptOutputFilePrefix; 
    switch ( dataType )
    {
    case PROBE_TRANSLATION: 
      title = "Probe Translation Axis Calibration Analysis"; 
      scriptOutputFilePrefixHistogram = "ProbeTranslationAxisCalibrationErrorHistogram"; 
      scriptOutputFilePrefix = "ProbeTranslationAxisCalibrationError";
      report << "Probe translation axis orientation: " << this->GetProbeTranslationAxisOrientation()[0] << "     " 
        << this->GetProbeTranslationAxisOrientation()[1] << "     " << this->GetProbeTranslationAxisOrientation()[2] << "</br>" ; 
      break; 
    case TEMPLATE_TRANSLATION: 
      title = "Template Translation Axis Calibration Analysis";
      scriptOutputFilePrefixHistogram = "TemplateTranslationAxisCalibrationErrorHistogram"; 
      scriptOutputFilePrefix = "TemplateTranslationAxisCalibrationError"; 
      report << "Probe translation axis orientation: " << this->GetTemplateTranslationAxisOrientation()[0] << "     " 
        << this->GetTemplateTranslationAxisOrientation()[1] << "     " << this->GetTemplateTranslationAxisOrientation()[2] << "</br>" ; 
      break; 
    }

    htmlReport->AddText(title.c_str(), vtkHTMLGenerator::H1); 
    
    htmlReport->AddParagraph(report.str().c_str()); 

    htmlReport->AddText("Error Histogram", vtkHTMLGenerator::H2); 

    const int wires[4] = {1, 3, 4, 6}; 

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

      switch ( dataType )
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


    /////////////////////////////////////////////////////////////////////////////////////////////

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

      switch ( dataType )
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
  }

  return PLUS_SUCCESS; 
}

//***************************************************************************
//							Spacing calculation
//***************************************************************************

//----------------------------------------------------------------------------
PlusStatus vtkStepperCalibrationController::CalculateSpacing()
{
  LOG_TRACE("vtkStepperCalibrationController::CalculateSpacing"); 
  if ( this->GetSpacingCalculated() )
  {
    // we already calculated it, no need to recalculate
    return PLUS_SUCCESS; 
  }

  LOG_INFO( ">>>>>>>>>>>> Image spacing calculation ..."); 

  // Construct linear equations Ax = b, where A is a matrix with m rows and 
  // n columns, b is an m-vector. 
  std::vector<vnl_vector<double>> aMatrix;
  std::vector<double> bVector;

  // Construct linear equation for spacing calculation
  this->ConstrLinEqForSpacingCalc(aMatrix, bVector); 

  if ( aMatrix.size() == 0 || bVector.size() == 0 )
  {
    LOG_WARNING("Spacing calculation failed, no data found!"); 
    return PLUS_FAIL; 
  }

  // The TRUS Scale factors
  // - Sx: lateral axis;
  // - Sy: axial axis;
  // - Units in mm/pixel.
  vnl_vector<double> TRUSSquaredScaleFactorsInMMperPixel2x1(2,0);

  int numberOfEquations(0); 
  do 
  {
    numberOfEquations = bVector.size(); 
    if ( PlusMath::LSQRMinimize(aMatrix, bVector, TRUSSquaredScaleFactorsInMMperPixel2x1) == PLUS_SUCCESS )
    {
      this->RemoveOutliersFromSpacingCalcData(aMatrix, bVector, TRUSSquaredScaleFactorsInMMperPixel2x1); 
    }
    else
    {
      LOG_WARNING("Failed to run LSQRMinimize!"); 
    }
  } 
  while (numberOfEquations != bVector.size()); 

  if ( TRUSSquaredScaleFactorsInMMperPixel2x1.empty() )
  {
    LOG_ERROR("Unable to calculate spacing! Minimizer returned empty result."); 
    return PLUS_FAIL; 
  }

  this->SaveSpacingCalculationError(aMatrix, bVector, TRUSSquaredScaleFactorsInMMperPixel2x1); 
  this->SetSpacing( sqrt(TRUSSquaredScaleFactorsInMMperPixel2x1.get(0)), sqrt(TRUSSquaredScaleFactorsInMMperPixel2x1.get(1)) );

  this->SpacingCalculatedOn(); 

  LOG_INFO("Spacing: " << this->GetSpacing()[0] << "  " << this->GetSpacing()[1]); 

  return PLUS_SUCCESS; 
}


//----------------------------------------------------------------------------
void vtkStepperCalibrationController::ConstrLinEqForSpacingCalc( std::vector<vnl_vector<double>> &aMatrix, std::vector<double> &bVector)
{
  LOG_TRACE("vtkStepperCalibrationController::ConstrLinEqForSpacingCalc"); 
  aMatrix.clear(); 
  bVector.clear(); 

  for ( int frame = 0; frame < this->SegmentedFrameContainer.size(); frame++ )
  {
    // Compute distance between line #1 and #3 for scaling computation 
    // Constant Distance Measurements from iCAL phantom design in mm
    const double distanceN1ToN3inMm(40); // TODO: read it from pahantom design
    double xDistanceN1ToN3Px = this->SegmentedFrameContainer[frame].SegResults.GetFoundDotsCoordinateValue()[WIRE1][0] - this->SegmentedFrameContainer[frame].SegResults.GetFoundDotsCoordinateValue()[WIRE3][0]; 
    double yDistanceN1ToN3Px = this->SegmentedFrameContainer[frame].SegResults.GetFoundDotsCoordinateValue()[WIRE1][1] - this->SegmentedFrameContainer[frame].SegResults.GetFoundDotsCoordinateValue()[WIRE3][1]; 

    // Populate the sparse matrix with squared distances in pixel 
    vnl_vector<double> scaleFactorN1ToN3(2,0); 
    scaleFactorN1ToN3.put(0, pow(xDistanceN1ToN3Px, 2));
    scaleFactorN1ToN3.put(1, pow(yDistanceN1ToN3Px, 2));
    aMatrix.push_back(scaleFactorN1ToN3); 

    // Populate the vector with squared distances in mm 
    bVector.push_back(pow(distanceN1ToN3inMm, 2));

    // Compute distance between line #3 and #6 for scaling computation 
    // Constant Distance Measurements from iCAL phantom design in mm
    const double distanceN3ToN6inMm(20); // TODO: read it from pahantom design
    double xDistanceN3ToN6Px = this->SegmentedFrameContainer[frame].SegResults.GetFoundDotsCoordinateValue()[WIRE3][0] - this->SegmentedFrameContainer[frame].SegResults.GetFoundDotsCoordinateValue()[WIRE6][0]; 
    double yDistanceN3ToN6Px = this->SegmentedFrameContainer[frame].SegResults.GetFoundDotsCoordinateValue()[WIRE3][1] - this->SegmentedFrameContainer[frame].SegResults.GetFoundDotsCoordinateValue()[WIRE6][1]; 

    // Populate the sparse matrix with squared distances in pixel 
    vnl_vector<double> scaleFactorN3ToN6(2,0); 
    scaleFactorN3ToN6.put(0, pow(xDistanceN3ToN6Px, 2));
    scaleFactorN3ToN6.put(1, pow(yDistanceN3ToN6Px, 2));
    aMatrix.push_back(scaleFactorN3ToN6); 

    // Populate the vector with squared distances in mm 
    bVector.push_back(pow(distanceN3ToN6inMm, 2));
  }
}

//----------------------------------------------------------------------------
void vtkStepperCalibrationController::GetSpacingCalculationError(
  const std::vector<vnl_vector<double>> &aMatrix, 
  const std::vector<double> &bVector, 
  const vnl_vector<double> &resultVector, 
  std::vector<CalibStatistics> &statistics)
{

  LOG_TRACE("vtkStepperCalibrationController::GetRotationAxisCalibrationError"); 
  // The coefficient matrix aMatrix should be m-by-n and the column vector bVector must have length m.
  const int n = aMatrix.begin()->size(); 
  const int m = bVector.size();
  const int r = resultVector.size(); 

  const int numberOfAxes(2); 
  const double sX = resultVector[0]; 
  const double sY = resultVector[1]; 

  // calculate difference between the computed and measured position for each pair
  std::vector< std::vector<double> > diffVector(numberOfAxes); 
  for( int row = 0; row < m; row = row + numberOfAxes)
  {
    diffVector[0].push_back( bVector[row    ] - aMatrix[row    ].get(0) * sX ); 
    diffVector[1].push_back( bVector[row + 1] - aMatrix[row + 1].get(1) * sY ); 
  }

  this->ComputeStatistics(diffVector, statistics); 
}

//----------------------------------------------------------------------------
void vtkStepperCalibrationController::RemoveOutliersFromSpacingCalcData(
  std::vector<vnl_vector<double>> &aMatrix, 
  std::vector<double> &bVector, 
  vnl_vector<double> resultVector )
{

  LOG_TRACE("vtkStepperCalibrationController::RemoveOutliersFromSpacingCalcData"); 
  // Calculate mean error and stdev of measured and computed wire distances
  std::vector<CalibStatistics> statistics; 
  this->GetSpacingCalculationError(aMatrix, bVector, resultVector, statistics); 

  const int n = aMatrix.begin()->size(); 
  const int m = bVector.size();
  const int r = resultVector.size(); 

  const int numberOfAxes(2); 
  const double sX = resultVector[0]; 
  const double sY = resultVector[1]; 

  // remove outliers
  for( int row = m - numberOfAxes; row >= 0; row = row - numberOfAxes)
  {
    if (abs ( bVector[row    ] - aMatrix[row    ].get(0) * sX - statistics[0].Mean ) >  this->OutlierDetectionThreshold * statistics[0].Stdev 
    ||
      abs ( bVector[row + 1] - aMatrix[row + 1].get(1) * sY - statistics[1].Mean ) >  this->OutlierDetectionThreshold * statistics[1].Stdev )
    {
      LOG_DEBUG("Outlier found at row " << row ); 
      aMatrix.erase(aMatrix.begin() + row, aMatrix.begin() + row + numberOfAxes); 
      bVector.erase(bVector.begin() + row, bVector.begin() + row + numberOfAxes); 
    }
  }
}

//----------------------------------------------------------------------------
void vtkStepperCalibrationController::SaveSpacingCalculationError(const std::vector<vnl_vector<double>> &aMatrix, 
                                                                  const std::vector<double> &bVector, 
                                                                  const vnl_vector<double> &resultVector)
{
  LOG_TRACE("vtkStepperCalibrationController::SaveSpacingCalculationError"); 
  std::ofstream spacingCalculationError;
  std::ostringstream filename; 
  filename << this->OutputPath << "/" << this->CalibrationStartTime  << ".SpacingCalculationError.txt"; 

  this->SetSpacingCalculationErrorReportFilePath(filename.str().c_str()); 

  spacingCalculationError.open (filename.str().c_str() , ios::out);
  spacingCalculationError << "# Spacing calculation error report" << std::endl; 

  spacingCalculationError << "Computed Distance - X (mm)\t" << "Measured Distance - X (mm)\t" << "Computed Distance - Y (mm)\t" << "Measured Distance - Y (mm)\t" << std::endl; 

  const int numberOfAxes(2); 
  const int m = bVector.size();
  const double sX = resultVector[0]; 
  const double sY = resultVector[1]; 

  for( int row = 0; row < m; row = row + numberOfAxes)
  {
    spacingCalculationError << sqrt( aMatrix[row].get(0) * sX + aMatrix[row].get(1) * sY ) << "\t" << sqrt(bVector[row])  << "\t" 
      << sqrt( aMatrix[row + 1].get(0) * sX + aMatrix[row + 1].get(1) * sY  )<< "\t" << sqrt(bVector[row + 1])  << "\t" << "\t" << std::endl; 
  }

  spacingCalculationError.close(); 

}


//----------------------------------------------------------------------------
PlusStatus vtkStepperCalibrationController::GenerateSpacingCalculationReport( vtkHTMLGenerator* htmlReport, vtkGnuplotExecuter* plotter, const char* gnuplotScriptsFolder)
{
  if ( htmlReport == NULL || plotter == NULL )
  {
    LOG_ERROR("Caller should define HTML report generator and gnuplot plotter before report generation!"); 
    return PLUS_FAIL; 
  }

  std::string plotSpacingCalculationErrorScript = gnuplotScriptsFolder + std::string("/PlotSpacingCalculationErrorHistogram.gnu"); 
  if ( !vtksys::SystemTools::FileExists( plotSpacingCalculationErrorScript.c_str(), true) )
  {
    LOG_ERROR("Unable to find gnuplot script at: " << plotSpacingCalculationErrorScript); 
    return PLUS_FAIL; 
  }

  if ( this->GetSpacingCalculated() )
  {
    const char * reportFile = this->GetSpacingCalculationErrorReportFilePath(); 
    if ( reportFile == NULL )
    {
      LOG_ERROR("Failed to generate spacing calculation report - report file name is NULL!"); 
      return PLUS_FAIL; 
    }

    if ( !vtksys::SystemTools::FileExists( reportFile, true) )
    {
      LOG_ERROR("Unable to find spacing calculation report file at: " << reportFile); 
      return PLUS_FAIL; 
    }

    std::string title; 
    std::string scriptOutputFilePrefixHistogram, scriptOutputFilePrefix; 
    title = "Spacing Calculation Analysis"; 
    scriptOutputFilePrefix = "PlotSpacingCalculationErrorHistogram"; 

    htmlReport->AddText(title.c_str(), vtkHTMLGenerator::H1); 

    std::ostringstream report; 
    report << "Image spacing (mm/px): " << this->GetSpacing()[0] << "     " << this->GetSpacing()[1] << "</br>" ; 
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
  }

  return PLUS_SUCCESS; 
}

//***************************************************************************
//						Center of rotation calculation
//***************************************************************************

//----------------------------------------------------------------------------
PlusStatus vtkStepperCalibrationController::CalculateCenterOfRotation( SegmentedFrameList &frameListForCenterOfRotation, double centerOfRotationPx[2], vtkTable* centerOfRotationCalculationErrorTable )
{
  LOG_TRACE("vtkStepperCalibrationController::CalculateCenterOfRotation"); 
  // ====================================================================
  // Compute the TRUS rotation center using linear least squares
  // ====================================================================
  // Note: the number of the distant pairs is a combination (C2)
  // of the number of the segmented N-Fiducial_1 positions in TRUS.
  // N in LaTeX = {NumDataPoints}^C_{2}
  // E.g.: for 200 positions as used for thresholding here, N = 19,900.
  // ====================================================================

  std::vector< std::vector<HomogenousVector4x1> > pointSetForCenterOfRotationCalculation;

  for ( int frame = 0; frame < frameListForCenterOfRotation.size(); frame++ )
  {
    std::vector<HomogenousVector4x1> vectorOfWirePoints; 

    // Add Line #1 pixel coordinates to center of rotation point set 
    vectorOfWirePoints.push_back( HomogenousVector4x1( frameListForCenterOfRotation[frame].SegResults.GetFoundDotsCoordinateValue()[WIRE1][0], frameListForCenterOfRotation[frame].SegResults.GetFoundDotsCoordinateValue()[WIRE1][1], 0 ) ); 

    // Add Line #3 pixel coordinates to center of rotation point set 
    vectorOfWirePoints.push_back( HomogenousVector4x1( frameListForCenterOfRotation[frame].SegResults.GetFoundDotsCoordinateValue()[WIRE3][0], frameListForCenterOfRotation[frame].SegResults.GetFoundDotsCoordinateValue()[WIRE3][1], 0 ) ); 

    // Add Line #4 pixel coordinates to center of rotation point set 
    vectorOfWirePoints.push_back( HomogenousVector4x1( frameListForCenterOfRotation[frame].SegResults.GetFoundDotsCoordinateValue()[WIRE4][0], frameListForCenterOfRotation[frame].SegResults.GetFoundDotsCoordinateValue()[WIRE4][1], 0 ) ); 

    // Add Line #6 pixel coordinates to center of rotation point set 
    vectorOfWirePoints.push_back( HomogenousVector4x1( frameListForCenterOfRotation[frame].SegResults.GetFoundDotsCoordinateValue()[WIRE6][0], frameListForCenterOfRotation[frame].SegResults.GetFoundDotsCoordinateValue()[WIRE6][1], 0 ) ); 

    pointSetForCenterOfRotationCalculation.push_back(vectorOfWirePoints); 
  }

  if ( pointSetForCenterOfRotationCalculation.size() < 30 )
  {
    LOG_WARNING("Center of rotation calculation failed - there is not enough data (" << pointSetForCenterOfRotationCalculation.size() << " out of at least 30)!"); 
    return PLUS_FAIL; 
  }

  // Data containers
  std::vector<vnl_vector<double>> aMatrix;
  std::vector<double> bVector;

  for( unsigned int i = 0; i <= pointSetForCenterOfRotationCalculation.size() - 2; i++ )
  {
    for( unsigned int j = i + 1; j <= pointSetForCenterOfRotationCalculation.size() - 1; j++ )
    {
      for ( int point = 0; point < pointSetForCenterOfRotationCalculation[i].size(); point++ )
      {
        // coordiates of the i-th element
        double Xi = pointSetForCenterOfRotationCalculation[i][point].GetX() * this->GetSpacing()[0]; 
        double Yi = pointSetForCenterOfRotationCalculation[i][point].GetY() * this->GetSpacing()[1]; 

        // coordiates of the j-th element
        double Xj = pointSetForCenterOfRotationCalculation[j][point].GetX() * this->GetSpacing()[0]; 
        double Yj = pointSetForCenterOfRotationCalculation[j][point].GetY() * this->GetSpacing()[1]; 

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

  if ( aMatrix.size() == 0 || bVector.size() == 0 )
  {
    LOG_WARNING("Center of rotation calculation failed, no data found!"); 
    return PLUS_FAIL; 
  }


  // The TRUS rotation center in original image frame
  // The same as the segmentation coordinating system
  // - Origin: Left-upper corner;
  // - Positive X: to the right;
  // - Positive Y: to the bottom;
  // - Units in pixels.
  vnl_vector<double> TRUSRotationCenterInOriginalImageFrameInMm2x1(2,0);
  int numberOfEquations(0); 
  do 
  {
    numberOfEquations = bVector.size();     
    if ( PlusMath::LSQRMinimize(aMatrix, bVector, TRUSRotationCenterInOriginalImageFrameInMm2x1) == PLUS_SUCCESS )
    {
      if ( !TRUSRotationCenterInOriginalImageFrameInMm2x1.empty() )
      {
        LOG_DEBUG("Center of rotation in original image frame (outlier removal in progress): " << TRUSRotationCenterInOriginalImageFrameInMm2x1.get(0) <<", "<<TRUSRotationCenterInOriginalImageFrameInMm2x1.get(1)<< " mm");
      }
      this->RemoveOutliersFromCenterOfRotCalcData(aMatrix, bVector, TRUSRotationCenterInOriginalImageFrameInMm2x1);       
    }
    else
    {
      LOG_WARNING("Failed to run LSQRMinimize!"); 
    }
  } 
  while (numberOfEquations != bVector.size()); 

  if ( TRUSRotationCenterInOriginalImageFrameInMm2x1.empty() )
  {
    LOG_ERROR("Unable to calculate center of rotation! Minimizer returned empty result."); 
    return PLUS_FAIL; 
  }

  // Calculate mean error and stdev of measured and computed distances
  CalibStatistics statistics; 
  this->GetCenterOfRotationCalculationError(aMatrix, bVector, TRUSRotationCenterInOriginalImageFrameInMm2x1, statistics); 

  centerOfRotationPx[0] = TRUSRotationCenterInOriginalImageFrameInMm2x1.get(0) / this->GetSpacing()[0]; 
  centerOfRotationPx[1] = TRUSRotationCenterInOriginalImageFrameInMm2x1.get(1) / this->GetSpacing()[1]; 

  this->SaveCenterOfRotationCalculationError(frameListForCenterOfRotation, centerOfRotationPx, centerOfRotationCalculationErrorTable); 

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
void vtkStepperCalibrationController::GetCenterOfRotationCalculationError(const std::vector<vnl_vector<double>> &aMatrix, 
                                                                          const std::vector<double> &bVector, 
                                                                          const vnl_vector<double> &resultVector, 
                                                                          CalibStatistics &statistics ) 
{
  LOG_TRACE("vtkStepperCalibrationController::GetCenterOfRotationCalculationError"); 
  // The coefficient matrix aMatrix should be m-by-n and the column vector bVector must have length m.
  const int n = aMatrix.begin()->size(); 
  const int m = bVector.size();
  const int r = resultVector.size(); 

  const double centerOfRotationX = resultVector[0]; 
  const double centerOfRotationY = resultVector[1]; 

  // calculate difference between the computed and measured angles
  std::vector< std::vector<double> > diffVector;
  std::vector<double> diff; 
  for( int row = 0; row < m; row++ )
  {
    diff.push_back( bVector[row] - aMatrix[row].get(0) * centerOfRotationX - aMatrix[row].get(1) * centerOfRotationY ); 
  }
  diffVector.push_back(diff); 

  std::vector<CalibStatistics> stat; 
  this->ComputeStatistics(diffVector, stat); 

  // calculate mean of difference 
  if ( !stat.empty() )
  {
    statistics = stat[0]; 
  }
}

//----------------------------------------------------------------------------
void vtkStepperCalibrationController::RemoveOutliersFromCenterOfRotCalcData(std::vector<vnl_vector<double>> &aMatrix, 
                                                                            std::vector<double> &bVector, 
                                                                            const vnl_vector<double> &resultVector )
{
  LOG_TRACE("vtkStepperCalibrationController::RemoveOutliersFromCenterOfRotCalcData"); 
  // Calculate mean error and stdev of measured and computed rotation angles
  CalibStatistics statistics; 
  this->GetCenterOfRotationCalculationError(aMatrix, bVector, resultVector, statistics); 

  const int n = aMatrix.begin()->size(); 
  const int m = bVector.size();
  const int r = resultVector.size(); 

  const double centerOfRotationX = resultVector[0]; 
  const double centerOfRotationY = resultVector[1];  

  // remove outliers
  for( int row = m - 1; row >= 0; row--)
  {
    if ( abs (bVector[row] - aMatrix[row].get(0) * centerOfRotationX - aMatrix[row].get(1) * centerOfRotationY - statistics.Mean ) >  this->OutlierDetectionThreshold * statistics.Stdev ) 
    {
      LOG_TRACE("Outlier found at row " << row ); 
      aMatrix.erase(aMatrix.begin() + row); 
      bVector.erase(bVector.begin() + row); 
    }
  }
}

//----------------------------------------------------------------------------
void vtkStepperCalibrationController::SaveCenterOfRotationCalculationError(SegmentedFrameList &frameListForCenterOfRotation, 
                                                                           const double centerOfRotationPx[2], 
                                                                           vtkTable* centerOfRotationCalculationErrorTable /*=NULL*/ )
{
  LOG_TRACE("vtkStepperCalibrationController::SaveCenterOfRotationCalculationError"); 

  if ( centerOfRotationCalculationErrorTable == NULL ) 
  {
    LOG_DEBUG("No need to generate center of rotation calculation error table!"); 
    return; 
  }

  if ( centerOfRotationCalculationErrorTable->GetNumberOfColumns() == 0 )
  {
    const char* colProbePositionName = "ProbePosition"; 
    vtkSmartPointer<vtkDoubleArray> colProbePosition = vtkSmartPointer<vtkDoubleArray>::New(); 
    colProbePosition->SetName(colProbePositionName); 
    centerOfRotationCalculationErrorTable->AddColumn(colProbePosition); 

    const char* colProbeRotationName = "ProbeRotation"; 
    vtkSmartPointer<vtkDoubleArray> colProbeRotation = vtkSmartPointer<vtkDoubleArray>::New();
    colProbeRotation->SetName(colProbeRotationName); 
    centerOfRotationCalculationErrorTable->AddColumn(colProbeRotation); 

    const char* colTemplatePositionName = "TemplatePosition"; 
    vtkSmartPointer<vtkDoubleArray> colTemplatePosition = vtkSmartPointer<vtkDoubleArray>::New(); 
    colTemplatePosition->SetName(colTemplatePositionName); 
    centerOfRotationCalculationErrorTable->AddColumn(colTemplatePosition); 

    const char* colW1RadiusName = "Wire#1Radius"; 
    vtkSmartPointer<vtkDoubleArray> colW1Radius = vtkSmartPointer<vtkDoubleArray>::New(); 
    colW1Radius->SetName(colW1RadiusName); 
    centerOfRotationCalculationErrorTable->AddColumn(colW1Radius); 

    const char* colW3RadiusName = "Wire#3Radius"; 
    vtkSmartPointer<vtkDoubleArray> colW3Radius = vtkSmartPointer<vtkDoubleArray>::New(); 
    colW3Radius->SetName(colW3RadiusName); 
    centerOfRotationCalculationErrorTable->AddColumn(colW3Radius); 

    const char* colW4RadiusName = "Wire#4Radius"; 
    vtkSmartPointer<vtkDoubleArray> colW4Radius = vtkSmartPointer<vtkDoubleArray>::New(); 
    colW4Radius->SetName(colW4RadiusName); 
    centerOfRotationCalculationErrorTable->AddColumn(colW4Radius); 

    const char* colW6RadiusName = "Wire#6Radius"; 
    vtkSmartPointer<vtkDoubleArray> colW6Radius = vtkSmartPointer<vtkDoubleArray>::New(); 
    colW6Radius->SetName(colW6RadiusName); 
    centerOfRotationCalculationErrorTable->AddColumn(colW6Radius); 

  }

  const double sX = this->GetSpacing()[0]; 
  const double sY = this->GetSpacing()[1]; 

  for ( unsigned int i = 0; i < frameListForCenterOfRotation.size(); i++ )
  {
    vtkSmartPointer<vtkVariantArray> tableRow = vtkSmartPointer<vtkVariantArray>::New(); 

    double probePos(0), probeRot(0), templatePos(0); 
    if ( !this->GetStepperEncoderValues(frameListForCenterOfRotation[i].TrackedFrameInfo, probePos, probeRot, templatePos) )
    {
      LOG_WARNING("SaveCenterOfRotationCalculationError: Unable to get probe position from tracked frame info for frame #" << i); 
      continue; 
    }

    tableRow->InsertNextValue(probePos); 
    tableRow->InsertNextValue(probeRot); 
    tableRow->InsertNextValue(templatePos); 

    // Compute radius from Wire #1, #3, #4, #6
    double w1x = frameListForCenterOfRotation[i].SegResults.GetFoundDotsCoordinateValue()[WIRE1][0]; 
    double w1y = frameListForCenterOfRotation[i].SegResults.GetFoundDotsCoordinateValue()[WIRE1][1]; 

    double w3x = frameListForCenterOfRotation[i].SegResults.GetFoundDotsCoordinateValue()[WIRE3][0]; 
    double w3y = frameListForCenterOfRotation[i].SegResults.GetFoundDotsCoordinateValue()[WIRE3][1]; 

    double w4x = frameListForCenterOfRotation[i].SegResults.GetFoundDotsCoordinateValue()[WIRE4][0]; 
    double w4y = frameListForCenterOfRotation[i].SegResults.GetFoundDotsCoordinateValue()[WIRE4][1]; 

    double w6x = frameListForCenterOfRotation[i].SegResults.GetFoundDotsCoordinateValue()[WIRE6][0]; 
    double w6y = frameListForCenterOfRotation[i].SegResults.GetFoundDotsCoordinateValue()[WIRE6][1]; 

    tableRow->InsertNextValue( sqrt( pow( (w1x - centerOfRotationPx[0])*sX, 2) + pow((w1y - centerOfRotationPx[1])*sY, 2) ) ); 
    tableRow->InsertNextValue( sqrt( pow( (w3x - centerOfRotationPx[0])*sX, 2) + pow((w3y - centerOfRotationPx[1])*sY, 2) ) ); 
    tableRow->InsertNextValue( sqrt( pow( (w4x - centerOfRotationPx[0])*sX, 2) + pow((w4y - centerOfRotationPx[1])*sY, 2) ) ); 
    tableRow->InsertNextValue( sqrt( pow( (w6x - centerOfRotationPx[0])*sX, 2) + pow((w6y - centerOfRotationPx[1])*sY, 2) ) ); 

    if ( tableRow->GetNumberOfTuples() == centerOfRotationCalculationErrorTable->GetNumberOfColumns() )
    {
      centerOfRotationCalculationErrorTable->InsertNextRow(tableRow); 
    }
    else
    {
      LOG_WARNING("Unable to insert new row to center of rotation error table, number of columns are different (" << tableRow->GetNumberOfTuples() << " vs. " << centerOfRotationCalculationErrorTable->GetNumberOfColumns() << ")."); 
    }

  }
}


//----------------------------------------------------------------------------
PlusStatus vtkStepperCalibrationController::GenerateCenterOfRotationReport( vtkHTMLGenerator* htmlReport, vtkGnuplotExecuter* plotter, const char* gnuplotScriptsFolder)
{
  if ( htmlReport == NULL || plotter == NULL )
  {
    LOG_ERROR("Caller should define HTML report generator and gnuplot plotter before report generation!"); 
    return PLUS_FAIL; 
  }

  std::string plotCenterOfRotCalcErrorScript = gnuplotScriptsFolder + std::string("/PlotCenterOfRotationCalculationError.gnu"); 
  if ( !vtksys::SystemTools::FileExists( plotCenterOfRotCalcErrorScript.c_str(), true) )
  {
    LOG_ERROR("Unable to find gnuplot script at: " << plotCenterOfRotCalcErrorScript); 
    return PLUS_FAIL;  
  }

  if ( this->GetCenterOfRotationCalculated() )
  {
    const char * reportFile = this->GetCenterOfRotationCalculationErrorReportFilePath();

    if ( reportFile == NULL )
    {
      LOG_ERROR("Failed to generate center of rotation report - report file name is NULL!"); 
      return PLUS_FAIL; 
    }

    if ( !vtksys::SystemTools::FileExists( reportFile, true) )
    {
      LOG_ERROR("Unable to find center of rotation calculation report file at: " << reportFile); 
      return PLUS_FAIL; 
    }

    std::string title; 
    std::string scriptOutputFilePrefixHistogram, scriptOutputFilePrefix; 
    title = "Center of Rotation Calculation Analysis"; 
    scriptOutputFilePrefix = "CenterOfRotationCalculationError"; 

    htmlReport->AddText(title.c_str(), vtkHTMLGenerator::H1); 

    std::ostringstream report; 
    report << "Center of rotation (px): " << this->GetCenterOfRotationPx()[0] << "     " << this->GetCenterOfRotationPx()[1] << "</br>" ; 
    report << "Phantom to probe distance (mm): " << this->GetPhantomToProbeDistanceInMm()[0] << "     " << this->GetPhantomToProbeDistanceInMm()[1] << "</br>"; 
    //report << "Mean: " << this->CenterOfRotationCalculationStat.Mean << "     Stdev: " << this->CenterOfRotationCalculationStat.Stdev 
    //  << "     Min: " << this->CenterOfRotationCalculationStat.Min << "     Max: " << this->CenterOfRotationCalculationStat.Max;  
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
  }
  else
  {
    LOG_WARNING("Generate center of rotation report failed - center of rotation not yet calulated!"); 
  }
  
  return PLUS_SUCCESS;
}


//***************************************************************************
//					Phantom to probe distance calculation
//***************************************************************************


//----------------------------------------------------------------------------
void vtkStepperCalibrationController::AddPointsForPhantomToProbeDistanceCalculation(HomogenousVector4x1 pointA, HomogenousVector4x1 pointB, HomogenousVector4x1 pointC)
{
  LOG_TRACE("vtkStepperCalibrationController::AddPointsForPhantomToProbeDistanceCalculation - HomogenousVector4x1"); 
  std::vector<HomogenousVector4x1> vectorOfPointCoordinates; 
  vectorOfPointCoordinates.push_back(pointA); 
  vectorOfPointCoordinates.push_back(pointB); 
  vectorOfPointCoordinates.push_back(pointC); 
  this->PointSetForPhantomToProbeDistanceCalculation.push_back(vectorOfPointCoordinates); 
}

//----------------------------------------------------------------------------
void vtkStepperCalibrationController::AddPointsForPhantomToProbeDistanceCalculation(double xPointA, double yPointA, double zPointA, 
                                                                                    double xPointB, double yPointB, double zPointB, 
                                                                                    double xPointC, double yPointC, double zPointC)
{
  LOG_TRACE("vtkStepperCalibrationController::AddPointsForPhantomToProbeDistanceCalculation"); 
  this->AddPointsForPhantomToProbeDistanceCalculation( 
    HomogenousVector4x1(xPointA, yPointA, zPointA), 
    HomogenousVector4x1(xPointB, yPointB, zPointB), 
    HomogenousVector4x1(xPointC, yPointC, zPointC) 
    ); 
}

//----------------------------------------------------------------------------
PlusStatus vtkStepperCalibrationController::CalculatePhantomToProbeDistance()
{
  LOG_TRACE("vtkStepperCalibrationController::CalculatePhantomToProbeDistance"); 
  // ==================================================================================
  // Compute the distance from the probe to phantom 
  // ==================================================================================
  // 1. This point-to-line distance holds the key to relate the position of the TRUS 
  //    rotation center to the precisely designed iCAL phantom geometry in Solid Edge CAD.
  // 2. Here we employ a straight-forward method based on vector theory as one of the 
  //    simplest and most efficient way to compute a point-line distance.
  //    FORMULA: D_O2AB = norm( cross(OA,OB) ) / norm(A-B)
  // ==================================================================================

  if ( !this->GetSpacingCalculated() )
  {
    LOG_WARNING("Unable to calculate phantom to probe distance without spacing calculated!"); 
    return PLUS_FAIL; 
  }

  // Clear container before we start
  this->PointSetForPhantomToProbeDistanceCalculation.clear(); 

  for ( int frame = 0; frame < this->SegmentedFrameContainer.size(); frame++ )
  {
    if ( this->SegmentedFrameContainer[frame].DataType == PROBE_ROTATION )
    {

      // Add Line #1 (point A) Line #3 (point B) and Line #6 (point C) pixel coordinates to phantom to probe distance point set 
      this->AddPointsForPhantomToProbeDistanceCalculation(
        this->SegmentedFrameContainer[frame].SegResults.GetFoundDotsCoordinateValue()[WIRE1][0], this->SegmentedFrameContainer[frame].SegResults.GetFoundDotsCoordinateValue()[WIRE1][1], 0, 
        this->SegmentedFrameContainer[frame].SegResults.GetFoundDotsCoordinateValue()[WIRE3][0], this->SegmentedFrameContainer[frame].SegResults.GetFoundDotsCoordinateValue()[WIRE3][1], 0, 
        this->SegmentedFrameContainer[frame].SegResults.GetFoundDotsCoordinateValue()[WIRE6][0], this->SegmentedFrameContainer[frame].SegResults.GetFoundDotsCoordinateValue()[WIRE6][1], 0 
        ); 

    }
  }

  vnl_vector<double> rotationCenter3x1InMm(3,0);
  rotationCenter3x1InMm.put(0, this->GetCenterOfRotationPx()[0] * this->GetSpacing()[0]); 
  rotationCenter3x1InMm.put(1, this->GetCenterOfRotationPx()[1] * this->GetSpacing()[1]);
  rotationCenter3x1InMm.put(2, 0);

  // Total number images used for this computation
  const int totalNumberOfImages2ComputePtLnDist = this->PointSetForPhantomToProbeDistanceCalculation.size();

  if ( totalNumberOfImages2ComputePtLnDist == 0 )
  {
    LOG_ERROR("Failed to calculate phantom to probe distance. Probe distance calculation data is empty!"); 
    return PLUS_FAIL; 
  }

  // This will keep a trace on all the calculated distance
  vnl_vector<double> listOfPhantomToProbeVerticalDistanceInMm(totalNumberOfImages2ComputePtLnDist, 0 );
  vnl_vector<double> listOfPhantomToProbeHorizontalDistanceInMm(totalNumberOfImages2ComputePtLnDist, 0 );

  for (int i = 0; i < totalNumberOfImages2ComputePtLnDist; i++)
  {
    // Extract point A
    vnl_vector<double> pointAInMm(3,0);
    pointAInMm.put( 0, this->PointSetForPhantomToProbeDistanceCalculation[i][0].GetX() * this->GetSpacing()[0] );
    pointAInMm.put( 1, this->PointSetForPhantomToProbeDistanceCalculation[i][0].GetY() * this->GetSpacing()[1] );
    pointAInMm.put( 2, 0 ); 

    // Extract point B
    vnl_vector<double> pointBInMm(3,0);
    pointBInMm.put( 0, this->PointSetForPhantomToProbeDistanceCalculation[i][1].GetX() * this->GetSpacing()[0] );
    pointBInMm.put( 1, this->PointSetForPhantomToProbeDistanceCalculation[i][1].GetY() * this->GetSpacing()[1] );
    pointBInMm.put( 2, 0 ); 

    // Extract point C
    vnl_vector<double> pointCInMm(3,0);
    pointCInMm.put( 0, this->PointSetForPhantomToProbeDistanceCalculation[i][2].GetX() * this->GetSpacing()[0] );
    pointCInMm.put( 1, this->PointSetForPhantomToProbeDistanceCalculation[i][2].GetY() * this->GetSpacing()[1] );
    pointCInMm.put( 2, 0 ); 

    // Construct vectors among rotation center, point A, and point B.
    const vnl_vector<double> vectorRotationCenterToPointAInMm = pointAInMm - rotationCenter3x1InMm;
    const vnl_vector<double> vectorRotationCenterToPointBInMm = pointBInMm - rotationCenter3x1InMm;
    const vnl_vector<double> vectorRotationCenterToPointCInMm = pointCInMm - rotationCenter3x1InMm;
    const vnl_vector<double> vectorPointAToPointBInMm = pointBInMm - pointAInMm;
    const vnl_vector<double> vectorPointBToPointCInMm = pointCInMm - pointBInMm;

    // Compute the point-line distance from probe to the line passing through A and B points, based on the
    // standard vector theory. FORMULA: D_O2AB = norm( cross(OA,OB) ) / norm(A-B)
    const double thisPhantomToProbeVerticalDistanceInMm = vnl_cross_3d( vectorRotationCenterToPointAInMm, vectorRotationCenterToPointBInMm ).magnitude() / vectorPointAToPointBInMm.magnitude();

    // Compute the point-line distance from probe to the line passing through B and C points, based on the
    // standard vector theory. FORMULA: D_O2AB = norm( cross(OA,OB) ) / norm(A-B)
    const double thisPhantomToProbeHorizontalDistanceInMm = vnl_cross_3d( vectorRotationCenterToPointBInMm, vectorRotationCenterToPointCInMm ).magnitude() / vectorPointBToPointCInMm.magnitude();

    // Populate the data container
    listOfPhantomToProbeVerticalDistanceInMm.put(i, thisPhantomToProbeVerticalDistanceInMm );
    listOfPhantomToProbeHorizontalDistanceInMm.put(i, thisPhantomToProbeHorizontalDistanceInMm );
  }

  this->SetPhantomToProbeDistanceInMm( listOfPhantomToProbeHorizontalDistanceInMm.mean(), listOfPhantomToProbeVerticalDistanceInMm.mean() ); 

  this->PhantomToProbeDistanceCalculatedOn(); 

  return PLUS_SUCCESS; 
}


//***************************************************************************
//								Clustering
//***************************************************************************


//----------------------------------------------------------------------------
double vtkStepperCalibrationController::GetClusterZPosition(const SegmentedFrameList &cluster)
{
  double meanZPosition(0); 
  const int numOfFrames = cluster.size(); 
  for ( int frame = 0; frame < numOfFrames; ++frame )
  {
    double probePos(0), probeRot(0), templatePos(0); 
    if ( !this->GetStepperEncoderValues(cluster[frame].TrackedFrameInfo, probePos, probeRot, templatePos) )
    {
      LOG_WARNING("GetClusterZPosition: Unable to get probe position from tracked frame info for frame #" << frame); 
      continue; 
    }
    meanZPosition += probePos / (1.0*numOfFrames); 
  }

  return meanZPosition; 
}

//----------------------------------------------------------------------------
void vtkStepperCalibrationController::ClusterSegmentedFrames(IMAGE_DATA_TYPE dataType,  std::vector<SegmentedFrameList> &clusterList)
{
  LOG_TRACE("vtkStepperCalibrationController::ClusterSegmentedFrames"); 

  SegmentedFrameList clusterData; 

  vtkSmartPointer<vtkPoints> clusterPoints = vtkSmartPointer<vtkPoints>::New(); 
  for ( int frame = 0; frame < this->SegmentedFrameContainer.size(); frame++ )
  {

    if ( this->SegmentedFrameContainer[frame].DataType == dataType )
    {
      double probePos(0), probeRot(0), templatePos(0); 
      if ( !this->GetStepperEncoderValues(this->SegmentedFrameContainer[frame].TrackedFrameInfo, probePos, probeRot, templatePos) )
      {
        LOG_WARNING("Clustering: Unable to get probe position from tracked frame info for frame #" << frame); 
        continue; 
      }

      LOG_DEBUG("Insert point to cluster list at probe position: " << probePos); 
      clusterData.push_back(this->SegmentedFrameContainer[frame]); 
      clusterPoints->InsertNextPoint(probePos, 0, 0); 
    }
  }

  vtkSmartPointer<vtkPolyData> data = vtkSmartPointer<vtkPolyData>::New();
  data->SetPoints(clusterPoints); 

  vtkSmartPointer<vtkMeanShiftClustering> meanShiftFilter = vtkSmartPointer<vtkMeanShiftClustering>::New();
  meanShiftFilter->SetInputConnection(data->GetProducerPort());
  meanShiftFilter->SetWindowRadius(5.0); //radius should be bigger than expected clusters
  meanShiftFilter->SetGaussianVariance(1.0);
  meanShiftFilter->Update();


  for(unsigned int r = 0; r < clusterData.size(); r++)
  {
    int clusterID = meanShiftFilter->GetPointAssociations(r); 
    if ( clusterID >= 0 )
    {
      while ( clusterList.size() < clusterID + 1 )
      {
        LOG_DEBUG("Create new segmented frame cluster for clusterID: " << clusterID); 
        SegmentedFrameList newCluster; 
        clusterList.push_back(newCluster); 
      }

      clusterList[clusterID].push_back(clusterData[r]); 
    }
  }

  LOG_DEBUG("Number of clusters: " << clusterList.size()); 
  for ( unsigned int i = 0; i < clusterList.size(); ++i)
  {
    LOG_DEBUG("Number of elements in cluster #" << i << ": " << clusterList[i].size()); 
  }

}

//----------------------------------------------------------------------------
PlusStatus vtkStepperCalibrationController::GetStepperEncoderValues( TrackedFrame* trackedFrame, double &probePosition, double &probeRotation, double &templatePosition)
{
  // Get the probe position from tracked frame info
  const char* cProbePos = trackedFrame->GetCustomFrameField("ProbePosition"); 
  if ( cProbePos != NULL )
  {
    probePosition = atof(cProbePos); 
  }
  else
  {
    double transform[16]; 
    if ( trackedFrame->GetDefaultFrameTransform(transform) )
    {
      // Get probe position from matrix (0,3) element
      probePosition = transform[3]; 
    }
    else
    {
      LOG_ERROR("Unable to get probe position from tracked frame info."); 
      return PLUS_FAIL; 
    }
  }

  // Get the probe rotation from tracked frame info
  const char* cProbeRot = trackedFrame->GetCustomFrameField("ProbeRotation"); 
  if ( cProbeRot != NULL )
  {
    probeRotation = atof(cProbeRot); 
  }
  else
  {
    double transform[16]; 
    if ( trackedFrame->GetDefaultFrameTransform(transform) )
    {
      // Get probe rotation from matrix (1,3) element
      probeRotation = transform[7]; 
    }
    else
    {
      LOG_ERROR("Unable to get probe rotation from tracked frame info."); 
      return PLUS_FAIL; 
    }
  }

  // Get the template position from tracked frame info
  const char* cTemplatePos = trackedFrame->GetCustomFrameField("TemplatePosition"); 
  if ( cTemplatePos != NULL )
  {
    templatePosition = atof(cTemplatePos); 
  }
  else
  {
    double transform[16]; 
    if ( trackedFrame->GetDefaultFrameTransform(transform) )
    {
      // Get template position from matrix (2,3) element
      templatePosition = transform[11]; 
    }
    else
    {
      LOG_ERROR("Unable to get template position from tracked frame info."); 
      return PLUS_FAIL; 
    }
  }

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus vtkStepperCalibrationController::OfflineProbeRotationAxisCalibration()
{
  LOG_TRACE("vtkStepperCalibrationController::OfflineProbeRotationAxisCalibration"); 
  if ( ! this->GetInitialized() ) 
  {
    this->Initialize(); 
  }

  vtkSmartPointer<vtkTrackedFrameList> trackedFrameList = vtkSmartPointer<vtkTrackedFrameList>::New();
  if ( !this->GetImageDataInfo(PROBE_ROTATION).InputSequenceMetaFileName.empty() )
  {
    trackedFrameList->ReadFromSequenceMetafile(this->GetImageDataInfo(PROBE_ROTATION).InputSequenceMetaFileName.c_str()); 
  }
  else
  {
    LOG_ERROR("Unable to start OfflineProbeRotationAxisCalibration with probe rotation data: SequenceMetaFileName is empty!"); 
    return PLUS_FAIL; 
  }

  // Reset the counter before we start
  ImageDataInfo dataInfo = this->GetImageDataInfo(PROBE_ROTATION); 
  dataInfo.NumberOfSegmentedImages = 0; 
  if ( dataInfo.NumberOfImagesToAcquire > trackedFrameList->GetNumberOfTrackedFrames() )
  {
    dataInfo.NumberOfImagesToAcquire = trackedFrameList->GetNumberOfTrackedFrames(); 
  }
  this->SetImageDataInfo(PROBE_ROTATION, dataInfo); 

  int frameCounter(0); 
  int imgNumber(0); 
  for( imgNumber = 0; frameCounter < this->GetImageDataInfo(PROBE_ROTATION).NumberOfImagesToAcquire; imgNumber++ )
  {
    if ( imgNumber >= trackedFrameList->GetNumberOfTrackedFrames() )
    {
      break; 
    }

    if ( this->AddTrackedFrameData(trackedFrameList->GetTrackedFrame(imgNumber), PROBE_ROTATION) )
    {
      // The segmentation was successful 
      frameCounter++; 
    }

    if ( this->SegmentationProgressCallbackFunction != NULL )
    {
      int numberOfSegmentedImages = this->GetImageDataInfo(PROBE_ROTATION).NumberOfSegmentedImages; 
      int percent = 100* numberOfSegmentedImages / this->GetImageDataInfo(PROBE_ROTATION).NumberOfImagesToAcquire; 
      (*SegmentationProgressCallbackFunction)(percent); 
    }

    this->AddFrameToRenderer(trackedFrameList->GetTrackedFrame(imgNumber)->ImageData); 
  }

  LOG_INFO ( "A total of " << frameCounter << " images (" << 100*frameCounter/imgNumber << "%) have been successfully added for probe rotation axis calibration.");
  trackedFrameList->Clear(); 

  if ( this->CalibrateProbeRotationAxis() != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to calibrate probe rotation axis!"); 
    return PLUS_FAIL; 
  }

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus vtkStepperCalibrationController::OfflineProbeTranslationAxisCalibration()
{
  LOG_TRACE("vtkStepperCalibrationController::OfflineProbeTranslationAxisCalibration"); 
  if ( ! this->GetInitialized() ) 
  {
    this->Initialize(); 
  }

  vtkSmartPointer<vtkTrackedFrameList> trackedFrameList = vtkSmartPointer<vtkTrackedFrameList>::New();
  if ( !this->GetImageDataInfo(PROBE_TRANSLATION).InputSequenceMetaFileName.empty() )
  {
    trackedFrameList->ReadFromSequenceMetafile(this->GetImageDataInfo(PROBE_TRANSLATION).InputSequenceMetaFileName.c_str()); 
  }
  else
  {
    LOG_ERROR("Unable to start OfflineProbeTranslationAxisCalibration with probe translation data: SequenceMetaFileName is empty!"); 
    return PLUS_FAIL; 
  }

  // Reset the counter before we start
  ImageDataInfo dataInfo = this->GetImageDataInfo(PROBE_TRANSLATION); 
  dataInfo.NumberOfSegmentedImages = 0; 
  if ( dataInfo.NumberOfImagesToAcquire > trackedFrameList->GetNumberOfTrackedFrames() )
  {
    dataInfo.NumberOfImagesToAcquire = trackedFrameList->GetNumberOfTrackedFrames(); 
  }
  this->SetImageDataInfo(PROBE_TRANSLATION, dataInfo); 

  int frameCounter(0); 
  int imgNumber(0); 
  for( imgNumber = 0; frameCounter < this->GetImageDataInfo(PROBE_TRANSLATION).NumberOfImagesToAcquire; imgNumber++ )
  {
    if ( imgNumber >= trackedFrameList->GetNumberOfTrackedFrames() )
    {
      break; 
    }

    if ( this->AddTrackedFrameData(trackedFrameList->GetTrackedFrame(imgNumber), PROBE_TRANSLATION) )
    {
      // The segmentation was successful 
      frameCounter++; 
    }

    if ( this->SegmentationProgressCallbackFunction != NULL )
    {
      int numberOfSegmentedImages = this->GetImageDataInfo(PROBE_TRANSLATION).NumberOfSegmentedImages; 
      int percent = 100* numberOfSegmentedImages / this->GetImageDataInfo(PROBE_TRANSLATION).NumberOfImagesToAcquire; 
      (*SegmentationProgressCallbackFunction)(percent); 
    }

    this->AddFrameToRenderer(trackedFrameList->GetTrackedFrame(imgNumber)->ImageData); 
  }

  LOG_INFO ( "A total of " << frameCounter << " images (" << 100*frameCounter/imgNumber << "%) have been successfully added for probe translation axis calibration.");

  trackedFrameList->Clear(); 
  if ( this->CalibrateProbeTranslationAxis() != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to calibrate probe translation axis!"); 
    return PLUS_FAIL; 
  }

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus vtkStepperCalibrationController::OfflineTemplateTranslationAxisCalibration()
{
  LOG_TRACE("vtkStepperCalibrationController::OfflineTemplateTranslationAxisCalibration"); 
  if ( ! this->GetInitialized() ) 
  {
    this->Initialize(); 
  }

  vtkSmartPointer<vtkTrackedFrameList> trackedFrameList = vtkSmartPointer<vtkTrackedFrameList>::New();
  if ( !this->GetImageDataInfo(TEMPLATE_TRANSLATION).InputSequenceMetaFileName.empty() )
  {
    trackedFrameList->ReadFromSequenceMetafile(this->GetImageDataInfo(TEMPLATE_TRANSLATION).InputSequenceMetaFileName.c_str()); 
  }
  else
  {
    LOG_ERROR("Unable to start OfflineTemplateTranslationAxisCalibration with template translation data: SequenceMetaFileName is empty!"); 
    return PLUS_FAIL; 
  }

  // Reset the counter before we start
  ImageDataInfo dataInfo = this->GetImageDataInfo(TEMPLATE_TRANSLATION); 
  dataInfo.NumberOfSegmentedImages = 0; 
  if ( dataInfo.NumberOfImagesToAcquire > trackedFrameList->GetNumberOfTrackedFrames() )
  {
    dataInfo.NumberOfImagesToAcquire = trackedFrameList->GetNumberOfTrackedFrames(); 
  }
  this->SetImageDataInfo(TEMPLATE_TRANSLATION, dataInfo); 

  int frameCounter(0); 
  int imgNumber(0); 
  for( imgNumber = 0; frameCounter < this->GetImageDataInfo(TEMPLATE_TRANSLATION).NumberOfImagesToAcquire; imgNumber++ )
  {
    if ( imgNumber >= trackedFrameList->GetNumberOfTrackedFrames() )
    {
      break; 
    }

    if ( this->AddTrackedFrameData(trackedFrameList->GetTrackedFrame(imgNumber), TEMPLATE_TRANSLATION) )
    {
      // The segmentation was successful 
      frameCounter++; 
    }

    if ( this->SegmentationProgressCallbackFunction != NULL )
    {
      int numberOfSegmentedImages = this->GetImageDataInfo(TEMPLATE_TRANSLATION).NumberOfSegmentedImages; 
      int percent = 100* numberOfSegmentedImages / this->GetImageDataInfo(TEMPLATE_TRANSLATION).NumberOfImagesToAcquire; 
      (*SegmentationProgressCallbackFunction)(percent); 
    }

    this->AddFrameToRenderer(trackedFrameList->GetTrackedFrame(imgNumber)->ImageData); 
  }

  LOG_INFO ( "A total of " << frameCounter << " images (" << 100*frameCounter/imgNumber << "%) have been successfully added for template translation axis calibration.");

  trackedFrameList->Clear(); 
  if ( this->CalibrateTemplateTranslationAxis() != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to calibrate template translation axis!"); 
    return PLUS_FAIL; 
  }

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
void vtkStepperCalibrationController::SaveCalibrationStartTime()
{

  // Record the system timestamp
  char dateStr[9];
  char timeStr[9];
  _strdate( dateStr );
  _strtime( timeStr );
  std::string DateInString(dateStr);
  DateInString.erase(5,1);	// remve '/'
  DateInString.erase(2,1);	// remve '/'
  std::string TimeInString(timeStr);
  TimeInString.erase(5,1);	// remve ':'
  TimeInString.erase(2,1);	// remve ':'
  // DATE/TIME IN STRING: [MMDDYY_HHMMSS]
  std::string starttime = DateInString + "_" + TimeInString; 
  this->SetCalibrationStartTime(starttime.c_str()); 
  LOG_TRACE("vtkStepperCalibrationController::SaveCalibrationStartTime: " << starttime); 
}

//----------------------------------------------------------------------------
PlusStatus vtkStepperCalibrationController::ReadConfiguration( const char* configFileNameWithPath )
{
  LOG_TRACE("vtkStepperCalibrationController::ReadConfiguration - " << configFileNameWithPath); 
  this->SetConfigurationFileName(configFileNameWithPath); 

  vtkSmartPointer<vtkXMLDataElement> calibrationController = vtkXMLUtilities::ReadElementFromFile(this->GetConfigurationFileName()); 
  if (calibrationController==NULL)
  {
    LOG_ERROR("Failed to read configuration from " << configFileNameWithPath);
    return PLUS_FAIL;
  }
  return this->ReadConfiguration(calibrationController);
}

//----------------------------------------------------------------------------
PlusStatus vtkStepperCalibrationController::ReadConfiguration( vtkXMLDataElement* configData )
{
  LOG_TRACE("vtkStepperCalibrationController::ReadConfiguration"); 
  if ( configData == NULL )
  {
    LOG_ERROR("Unable to read configuration"); 
    return PLUS_FAIL;
  }

  Superclass::ReadConfiguration(configData); 

	vtkSmartPointer<vtkXMLDataElement> usCalibration = configData->FindNestedElementWithName("USCalibration");
	if (usCalibration == NULL) {
    LOG_ERROR("Cannot find USCalibration element in XML tree!");
    return PLUS_FAIL;
	}

  // Calibration controller specifications
  //********************************************************************
  vtkSmartPointer<vtkXMLDataElement> calibrationController = usCalibration->FindNestedElementWithName("CalibrationController"); 
  if (this->ReadCalibrationControllerConfiguration(calibrationController)!=PLUS_SUCCESS)
  {
    LOG_ERROR("Cannot find calibrationController element");
    return PLUS_FAIL;
  }

  // StepperCalibration specifications
  //*********************************
  vtkSmartPointer<vtkXMLDataElement> stepperCalibration = calibrationController->FindNestedElementWithName("StepperCalibration"); 
  if (this->ReadStepperCalibrationConfiguration(stepperCalibration)!=PLUS_SUCCESS)
  {
    LOG_ERROR("Cannot find stepperCalibration element");
    return PLUS_FAIL;
  }

	// Phantom definition
	//*********************************
	this->ReadPhantomDefinition(configData);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkStepperCalibrationController::ReadStepperCalibrationConfiguration(vtkXMLDataElement* stepperCalibration)
{
  LOG_TRACE("vtkStepperCalibrationController::ReadStepperCalibrationConfiguration"); 
  if ( stepperCalibration == NULL) 
  {	
    LOG_ERROR("Unable to read StepperCalibration XML data element!"); 
    return PLUS_FAIL; 
  } 

  int minNumberOfRotationClusters = 0;
  if ( stepperCalibration->GetScalarAttribute("MinNumberOfRotationClusters", minNumberOfRotationClusters) ) 
  {
    this->MinNumberOfRotationClusters = minNumberOfRotationClusters; 
  }

  	// TemplateTranslationData data set specifications
	//********************************************************************
	vtkSmartPointer<vtkXMLDataElement> templateTranslationData = stepperCalibration->FindNestedElementWithName("TemplateTranslationData"); 
	if ( templateTranslationData != NULL) 
	{
		vtkCalibrationController::ImageDataInfo imageDataInfo = this->GetImageDataInfo(TEMPLATE_TRANSLATION); 
		int numberOfImagesToUse = -1;
		if ( templateTranslationData->GetScalarAttribute("NumberOfImagesToAcquire", numberOfImagesToUse) ) 
		{
			imageDataInfo.NumberOfImagesToAcquire = numberOfImagesToUse; 
		}

		const char* sequenceMetaFile = templateTranslationData->GetAttribute("OutputSequenceMetaFileSuffix"); 
		if ( sequenceMetaFile != NULL) 
		{
			imageDataInfo.OutputSequenceMetaFileSuffix.assign(sequenceMetaFile); 
		}

		this->SetImageDataInfo(TEMPLATE_TRANSLATION, imageDataInfo); 
	}
	else
	{
		LOG_DEBUG("Unable to find TemplateTranslationData XML data element, default 100 is used"); 
		vtkCalibrationController::ImageDataInfo imageDataInfo = this->GetImageDataInfo(TEMPLATE_TRANSLATION); 
		imageDataInfo.NumberOfImagesToAcquire = 100;
    this->SetImageDataInfo(TEMPLATE_TRANSLATION, imageDataInfo); 
	}

	// ProbeTranslationData data set specifications
	//********************************************************************
	vtkSmartPointer<vtkXMLDataElement> probeTranslationData = stepperCalibration->FindNestedElementWithName("ProbeTranslationData"); 
	if ( probeTranslationData != NULL) 
	{
		vtkCalibrationController::ImageDataInfo imageDataInfo = this->GetImageDataInfo(PROBE_TRANSLATION); 
		int numberOfImagesToUse = -1;
		if ( probeTranslationData->GetScalarAttribute("NumberOfImagesToAcquire", numberOfImagesToUse) ) 
		{
			imageDataInfo.NumberOfImagesToAcquire = numberOfImagesToUse; 
		}

		const char* sequenceMetaFile = probeTranslationData->GetAttribute("OutputSequenceMetaFileSuffix"); 
		if ( sequenceMetaFile != NULL) 
		{
			imageDataInfo.OutputSequenceMetaFileSuffix.assign(sequenceMetaFile); 
		}

    this->SetImageDataInfo(PROBE_TRANSLATION, imageDataInfo); 
	}
	else
	{
		LOG_DEBUG("Unable to find ProbeTranslationData XML data element, default 200 is used"); 
		vtkCalibrationController::ImageDataInfo imageDataInfo = this->GetImageDataInfo(PROBE_TRANSLATION); 
		imageDataInfo.NumberOfImagesToAcquire = 200;
    this->SetImageDataInfo(PROBE_TRANSLATION, imageDataInfo); 
	}
  

	// ProbeRotationData data set specifications
	//********************************************************************
	vtkSmartPointer<vtkXMLDataElement> probeRotationData = stepperCalibration->FindNestedElementWithName("ProbeRotationData"); 
	if ( probeRotationData != NULL) 
	{
		vtkCalibrationController::ImageDataInfo imageDataInfo = this->GetImageDataInfo(PROBE_ROTATION); 
		int numberOfImagesToUse = -1;
		if ( probeRotationData->GetScalarAttribute("NumberOfImagesToAcquire", numberOfImagesToUse) ) 
		{
			imageDataInfo.NumberOfImagesToAcquire = numberOfImagesToUse; 
		}

		const char* sequenceMetaFile = probeRotationData->GetAttribute("OutputSequenceMetaFileSuffix"); 
		if ( sequenceMetaFile != NULL) 
		{
			imageDataInfo.OutputSequenceMetaFileSuffix.assign(sequenceMetaFile); 
		}

		this->SetImageDataInfo(PROBE_ROTATION, imageDataInfo); 
	}
	else
	{
		LOG_DEBUG("Unable to find ProbeRotationData XML data element, default 500 is used"); 
		vtkCalibrationController::ImageDataInfo imageDataInfo = this->GetImageDataInfo(PROBE_ROTATION); 
		imageDataInfo.NumberOfImagesToAcquire = 500;
    this->SetImageDataInfo(PROBE_ROTATION, imageDataInfo); 
	}

  // Calibration result specifications
	//********************************************************************
  vtkSmartPointer<vtkXMLDataElement> calibrationResult = stepperCalibration->FindNestedElementWithName("CalibrationResult"); 

  if ( calibrationResult != NULL )
  {
    const char* calibrationDate = calibrationResult->GetAttribute("Date"); 
    if ( calibrationDate != NULL )
    {
      this->SetCalibrationDate(calibrationDate); 
    }

    const char* algorithmVersion = calibrationResult->GetAttribute("AlgorithmVersion"); 
    if ( algorithmVersion != NULL )
    {
      this->SetAlgorithmVersion(algorithmVersion); 
    }

    int centerOfRotationPx[2]={0}; 
    if ( calibrationResult->GetVectorAttribute("CenterOfRotationPx", 2, centerOfRotationPx ) )
    {
      this->SetCenterOfRotationPx(centerOfRotationPx); 
      this->CenterOfRotationCalculatedOn(); 
    }

    double spacing[2]={0}; 
    if ( calibrationResult->GetVectorAttribute("Spacing", 2, spacing ) )
    {
      this->SetSpacing(spacing); 
      this->SpacingCalculatedOn(); 
    }

    double phantomToProbeDistanceInMm[2]={0}; 
    if ( calibrationResult->GetVectorAttribute("PhantomToProbeDistanceInMm", 2, phantomToProbeDistanceInMm) )
    {
      this->SetPhantomToProbeDistanceInMm(phantomToProbeDistanceInMm); 
      this->PhantomToProbeDistanceCalculatedOn(); 
    }
  
    double probeTranslationAxisOrientation[3]={0}; 
    if ( calibrationResult->GetVectorAttribute("ProbeTranslationAxisOrientation", 3, probeTranslationAxisOrientation) )
    {
      this->SetProbeTranslationAxisOrientation(probeTranslationAxisOrientation); 
      this->ProbeTranslationAxisCalibratedOn(); 
    }

    double templateTranslationAxisOrientation[3]={0}; 
    if ( calibrationResult->GetVectorAttribute("TemplateTranslationAxisOrientation", 3, templateTranslationAxisOrientation) )
    {
      this->SetTemplateTranslationAxisOrientation(templateTranslationAxisOrientation); 
      this->TemplateTranslationAxisCalibratedOn(); 
    }

    double probeRotationAxisOrientation[3]={0}; 
    if ( calibrationResult->GetVectorAttribute("ProbeRotationAxisOrientation", 3, probeRotationAxisOrientation) )
    {
      this->SetProbeRotationAxisOrientation(probeRotationAxisOrientation); 
      this->ProbeRotationAxisCalibratedOn(); 
    }

    double probeRotationEncoderOffset=0; 
    if ( calibrationResult->GetScalarAttribute("ProbeRotationEncoderOffset", probeRotationEncoderOffset) )
    {
      this->SetProbeRotationEncoderOffset(probeRotationEncoderOffset); 
    }

    double probeRotationEncoderScale=0;
    if ( calibrationResult->GetScalarAttribute("ProbeRotationEncoderScale", probeRotationEncoderScale ) )
    {
      this->SetProbeRotationEncoderScale(probeRotationEncoderScale); 
      this->ProbeRotationEncoderCalibratedOn(); 
    }

    this->CalibrationDoneOn(); 
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkStepperCalibrationController::WriteConfiguration( vtkXMLDataElement* configData )
{
	LOG_TRACE("vtkStepperCalibrationController::WriteConfiguration"); 
	if ( configData == NULL )
	{
		LOG_ERROR("Unable to write configuration - input xml data is NULL"); 
		return PLUS_FAIL;
	}

  vtkSmartPointer<vtkXMLDataElement> calibrationController = configData->LookupElementWithName("CalibrationController");

  if ( calibrationController == NULL )
  {
    LOG_ERROR("Unable to find CalibrationController XML data element!"); 
    return PLUS_FAIL; 
  }

  vtkSmartPointer<vtkXMLDataElement> stepperCalibration = calibrationController->LookupElementWithName("StepperCalibration");
  if ( stepperCalibration == NULL )
  {
    LOG_ERROR("Failed to write results to ProbeCalibration XML data element - element not found!"); 
    return PLUS_FAIL; 
  }

  vtkSmartPointer<vtkXMLDataElement> calibrationResult = stepperCalibration->FindNestedElementWithName("CalibrationResult");

  if ( calibrationResult == NULL )
  {
    calibrationResult = vtkSmartPointer<vtkXMLDataElement>::New(); 
    calibrationResult->SetName("CalibrationResult"); 
    calibrationResult->SetParent(stepperCalibration); 
    stepperCalibration->AddNestedElement(calibrationResult); 
  }

  calibrationResult->SetAttribute("Date", this->GetCalibrationDate()); 
  calibrationResult->SetAttribute("AlgorithmVersion", this->GetAlgorithmVersion()); 
  
  if ( this->GetCenterOfRotationCalculated() )
  {
    calibrationResult->SetVectorAttribute("CenterOfRotationPx", 2, this->GetCenterOfRotationPx()); 
  }

  if ( this->GetSpacingCalculated() )
  {
    calibrationResult->SetVectorAttribute("Spacing", 2, this->GetSpacing()); 
  }

  if ( this->GetPhantomToProbeDistanceCalculated() )
  {
    calibrationResult->SetVectorAttribute("PhantomToProbeDistanceInMm", 2, this->GetPhantomToProbeDistanceInMm()); 
  }
  
  if ( this->GetProbeTranslationAxisCalibrated() )
  {
    calibrationResult->SetVectorAttribute("ProbeTranslationAxisOrientation", 3, this->GetProbeTranslationAxisOrientation()); 
  }

  if ( this->GetTemplateTranslationAxisCalibrated() )
  {
    calibrationResult->SetVectorAttribute("TemplateTranslationAxisOrientation", 3, this->GetTemplateTranslationAxisOrientation()); 
  }

  if ( this->GetProbeRotationAxisCalibrated() )
  {
    calibrationResult->SetVectorAttribute("ProbeRotationAxisOrientation", 3, this->GetProbeRotationAxisOrientation()); 
  }

  if ( this->GetProbeRotationEncoderCalibrated() )
  {
    calibrationResult->SetDoubleAttribute("ProbeRotationEncoderOffset", this->GetProbeRotationEncoderOffset()); 
    calibrationResult->SetDoubleAttribute("ProbeRotationEncoderScale", this->GetProbeRotationEncoderScale()); 
  }
  
  return PLUS_SUCCESS; 
}

