#include "PlusConfigure.h"
#include "PlusMath.h"
#include "vtkStepperCalibrationController.h"

#include "vtkTranslAxisCalibAlgo.h"
#include "vtkRotationAxisCalibAlgo.h"
#include "vtkSpacingCalibAlgo.h"

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
#include "vtkBrachyTracker.h"

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
  this->ProbeRotationEncoderCalibrationErrorReportFilePath = NULL; 

  this->SaveCalibrationStartTime(); 

  this->SetAlgorithmVersion("1.0.0"); 
}

//----------------------------------------------------------------------------
vtkStepperCalibrationController::~vtkStepperCalibrationController()
{
  this->SetCalibrationStartTime(NULL); 
  this->SetProbeRotationEncoderCalibrationErrorReportFilePath(NULL); 
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

  this->InitializedOn(); 

  return PLUS_SUCCESS;
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
      LOG_INFO("CenterOfRotation (px): " << this->CenterOfRotationPx[0] << "  " << this->CenterOfRotationPx[1]); 
      LOG_INFO("CenterOfRotation (mm): " << this->CenterOfRotationPx[0]*this->Spacing[0] << "  " << this->CenterOfRotationPx[1]*this->Spacing[1]); 
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
    if ( this->SaveTrackedFrameListToMetafile( PROBE_ROTATION, vtkPlusConfig::GetInstance()->GetOutputDirectory(), probeRotationDataFileName.str().c_str(), false ) != PLUS_SUCCESS )
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

  vtkSmartPointer<vtkRotationAxisCalibAlgo> rotationAxisCalibAlgo = vtkSmartPointer<vtkRotationAxisCalibAlgo>::New(); 
  rotationAxisCalibAlgo->SetInputs(this->TrackedFrameListContainer[PROBE_ROTATION], this->GetSpacing()); 

  // Get rotation axis calibration output 
  double rotationAxisOrientation[3] = {0}; 
  if ( rotationAxisCalibAlgo->GetRotationAxisOrientation(rotationAxisOrientation) != PLUS_SUCCESS )
  {
    LOG_ERROR("Rotation axis calibration failed!"); 
    return PLUS_FAIL; 
  }
 
  // Set rotation axis orientation 
  this->SetProbeRotationAxisOrientation(rotationAxisOrientation[0], rotationAxisOrientation[1], rotationAxisOrientation[2]); 
  this->ProbeRotationAxisCalibratedOn(); 

  // Get center of rotation 
  double *centerOfRotationPx = rotationAxisCalibAlgo->GetCenterOfRotationPx(); 
  this->SetCenterOfRotationPx( centerOfRotationPx[0], centerOfRotationPx[1]); 
  this->CenterOfRotationCalculatedOn(); 

  // TODO: Need to generate report
  //rotAxisCalibAlgo->GenerateReport(htmlGenerator, gnuplotExecuter, inputGnuplotScriptsFolder.c_str()); 

  return PLUS_SUCCESS; 
}


//----------------------------------------------------------------------------
PlusStatus vtkStepperCalibrationController::GenerateProbeRotationAxisCalibrationReport( vtkHTMLGenerator* htmlReport, vtkGnuplotExecuter* plotter, const char* gnuplotScriptsFolder)
{
  LOG_ERROR("TODO: call report generation from vtkRotationAxisCalibAlgo!"); 
  return PLUS_FAIL; 
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
  if ( PlusMath::LSQRMinimize(aMatrix, bVector, rotationEncoderCalibrationResult) != PLUS_SUCCESS )
  {
    LOG_WARNING("Failed to run LSQRMinimize!"); 
  }

  if ( rotationEncoderCalibrationResult.empty() )
  {
    LOG_ERROR("Unable to calibrate rotation encoder! Minimizer returned empty result."); 
    return PLUS_FAIL; 
  }

  // Calculate mean error and stdev of measured and computed wire positions for each wire
  CalibStatistics statistics; 
  this->GetRotationEncoderCalibrationError(aMatrix, bVector, rotationEncoderCalibrationResult, statistics); 
  this->SaveRotationEncoderCalibrationError(aMatrix, bVector, rotationEncoderCalibrationResult); 

  // TODO: remove sign from scale  => we need to revise the equation instead 
  this->SetProbeRotationEncoderScale(-rotationEncoderCalibrationResult.get(0)); 
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
      if ( !vtkBrachyTracker::GetStepperEncoderValues(this->SegmentedFrameContainer[frame].TrackedFrameInfo, probePos, probeRot, templatePos, this->SegmentedFrameDefaultTransformName.c_str()) )
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
  filename << vtkPlusConfig::GetInstance()->GetOutputDirectory() << "/" << this->CalibrationStartTime  << ".ProbeRotationEncoderCalibrationError.txt"; 

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
    if ( this->SaveTrackedFrameListToMetafile( PROBE_TRANSLATION, vtkPlusConfig::GetInstance()->GetOutputDirectory(), probeTranslationDataFileName.str().c_str(), false ) != PLUS_SUCCESS )
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
    if ( this->SaveTrackedFrameListToMetafile( TEMPLATE_TRANSLATION, vtkPlusConfig::GetInstance()->GetOutputDirectory(), templateTranslationDataFileName.str().c_str(), false ) != PLUS_SUCCESS )
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

  vtkSmartPointer<vtkTranslAxisCalibAlgo> translationAxisCalibAlgo = vtkSmartPointer<vtkTranslAxisCalibAlgo>::New(); 
  translationAxisCalibAlgo->SetInputs(this->TrackedFrameListContainer[dataType], this->GetSpacing(), dataType); 
  
  // Get translation axis calibration output 
  double translationAxisOrientation[3] = {0}; 
  if ( translationAxisCalibAlgo->GetTranslationAxisOrientation(translationAxisOrientation) != PLUS_SUCCESS )
  {
    LOG_ERROR("Translation axis calibration failed!"); 
    return PLUS_FAIL; 
  }

  // Set translation axis orientation 
  if ( dataType == PROBE_TRANSLATION )
  {
    this->SetProbeTranslationAxisOrientation(translationAxisOrientation); 
    this->ProbeTranslationAxisCalibratedOn(); 
  }
  else if ( dataType == TEMPLATE_TRANSLATION )
  {
    this->SetTemplateTranslationAxisOrientation(translationAxisOrientation); 
    this->TemplateTranslationAxisCalibratedOn(); 
  }

  // TODO: Need to generate report
  //translAxisCalibAlgo->GenerateReport(htmlGenerator, gnuplotExecuter, inputGnuplotScriptsFolder.c_str()); 

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
void vtkStepperCalibrationController::GenerateProbeTranslationAxisCalibrationReport( vtkHTMLGenerator* htmlReport, vtkGnuplotExecuter* plotter, const char* gnuplotScriptsFolder)
{
  LOG_TRACE("vtkStepperCalibrationController::GenerateProbeTranslationAxisCalibrationReport"); 
  LOG_ERROR("TODO: call report generation from vtkTranslAxisCalibAlgo!"); 
}

//----------------------------------------------------------------------------
void vtkStepperCalibrationController::GenerateTemplateTranslationAxisCalibrationReport( vtkHTMLGenerator* htmlReport, vtkGnuplotExecuter* plotter, const char* gnuplotScriptsFolder)
{
  LOG_TRACE("vtkStepperCalibrationController::GenerateTemplateTranslationAxisCalibrationReport"); 
  LOG_ERROR("TODO: call report generation from vtkTranslAxisCalibAlgo!"); 
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

  vtkSmartPointer<vtkTrackedFrameList> trackedFrameList = vtkSmartPointer<vtkTrackedFrameList>::New(); 
  trackedFrameList->AddTrackedFrameList( this->TrackedFrameListContainer[PROBE_ROTATION] ); 
  trackedFrameList->AddTrackedFrameList( this->TrackedFrameListContainer[PROBE_TRANSLATION] ); 
  trackedFrameList->AddTrackedFrameList( this->TrackedFrameListContainer[TEMPLATE_TRANSLATION] ); 

  vtkSmartPointer<vtkSpacingCalibAlgo> spacingCalibAlgo = vtkSmartPointer<vtkSpacingCalibAlgo>::New(); 
  spacingCalibAlgo->SetInputs(trackedFrameList, this->PatternRecognition.GetFidLabeling()->GetNWires()); 
  
  double spacing[2]={0};
  if ( spacingCalibAlgo->GetSpacing(spacing) != PLUS_SUCCESS )
  {
    LOG_ERROR("Spacing calibration failed!"); 
    return PLUS_FAIL;  
  }
 
  this->SetSpacing( spacing[0], spacing[1]);

  this->SpacingCalculatedOn(); 

  LOG_INFO("Spacing: " << this->Spacing[0] << "  " << this->Spacing[1]); 

  // TODO: Need to generate report
  //spacingCalibAlgo->GenerateReport(htmlGenerator, gnuplotExecuter, inputGnuplotScriptsFolder.c_str()); 

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus vtkStepperCalibrationController::GenerateSpacingCalculationReport( vtkHTMLGenerator* htmlReport, vtkGnuplotExecuter* plotter, const char* gnuplotScriptsFolder)
{
  LOG_ERROR("TODO: call report generation from vtkSpacingCalibAlgo!"); 
  return PLUS_FAIL; 
}


//----------------------------------------------------------------------------
PlusStatus vtkStepperCalibrationController::GenerateCenterOfRotationReport( vtkHTMLGenerator* htmlReport, vtkGnuplotExecuter* plotter, const char* gnuplotScriptsFolder)
{
  LOG_ERROR("TODO: call report generation from vtkRotationAxisCalibAlgo!"); 
  return PLUS_FAIL;
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
  std::string defaultFrameTransformName=trackedFrameList->GetDefaultFrameTransformName();
  for( imgNumber = 0; frameCounter < this->GetImageDataInfo(PROBE_ROTATION).NumberOfImagesToAcquire; imgNumber++ )
  {
    if ( imgNumber >= trackedFrameList->GetNumberOfTrackedFrames() )
    {
      break; 
    }

    if ( this->AddTrackedFrameData(trackedFrameList->GetTrackedFrame(imgNumber), PROBE_ROTATION, defaultFrameTransformName.c_str()) )
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

    this->SetOfflineImageData(trackedFrameList->GetTrackedFrame(imgNumber)->GetImageData()->GetDisplayableImage()); 
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
  std::string defaultFrameTransformName=trackedFrameList->GetDefaultFrameTransformName();
  for( imgNumber = 0; frameCounter < this->GetImageDataInfo(PROBE_TRANSLATION).NumberOfImagesToAcquire; imgNumber++ )
  {
    if ( imgNumber >= trackedFrameList->GetNumberOfTrackedFrames() )
    {
      break; 
    }

    if ( this->AddTrackedFrameData(trackedFrameList->GetTrackedFrame(imgNumber), PROBE_TRANSLATION, defaultFrameTransformName.c_str()) )
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

    this->SetOfflineImageData(trackedFrameList->GetTrackedFrame(imgNumber)->GetImageData()->GetDisplayableImage());
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
  std::string defaultFrameTransformName=trackedFrameList->GetDefaultFrameTransformName();
  for( imgNumber = 0; frameCounter < this->GetImageDataInfo(TEMPLATE_TRANSLATION).NumberOfImagesToAcquire; imgNumber++ )
  {
    if ( imgNumber >= trackedFrameList->GetNumberOfTrackedFrames() )
    {
      break; 
    }

    if ( this->AddTrackedFrameData(trackedFrameList->GetTrackedFrame(imgNumber), TEMPLATE_TRANSLATION, defaultFrameTransformName.c_str()) )
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

    this->SetOfflineImageData(trackedFrameList->GetTrackedFrame(imgNumber)->GetImageData()->GetDisplayableImage()); 
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
PlusStatus vtkStepperCalibrationController::ReadConfiguration( vtkXMLDataElement* configData )
{
  LOG_TRACE("vtkStepperCalibrationController::ReadConfiguration"); 
  if ( configData == NULL )
  {
    LOG_ERROR("Unable to read configuration"); 
    return PLUS_FAIL;
  }

  Superclass::ReadConfiguration(configData); 

  // Calibration controller specifications
  //********************************************************************
  if (this->ReadCalibrationControllerConfiguration(configData)!=PLUS_SUCCESS)
  {
    LOG_ERROR("Cannot find calibrationController element");
    return PLUS_FAIL;
  }

  // StepperCalibration specifications
  //*********************************
  if (this->ReadStepperCalibrationConfiguration(configData)!=PLUS_SUCCESS)
  {
    LOG_ERROR("Cannot find stepperCalibration element");
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkStepperCalibrationController::ReadStepperCalibrationConfiguration(vtkXMLDataElement* rootElement)
{
  LOG_TRACE("vtkStepperCalibrationController::ReadStepperCalibrationConfiguration"); 
  if (rootElement == NULL) 
  {	
    LOG_ERROR("Unable to read StepperCalibration XML data element!"); 
    return PLUS_FAIL; 
  } 

	vtkSmartPointer<vtkXMLDataElement> usCalibration = rootElement->FindNestedElementWithName("USCalibration");
	if (usCalibration == NULL) {
    LOG_ERROR("Cannot find USCalibration element in XML tree!");
    return PLUS_FAIL;
	}

  vtkSmartPointer<vtkXMLDataElement> calibrationController = usCalibration->FindNestedElementWithName("CalibrationController"); 
  if (calibrationController == NULL)
  {
    LOG_ERROR("Cannot find calibrationController element");
    return PLUS_FAIL;
  }

  vtkSmartPointer<vtkXMLDataElement> stepperCalibration = calibrationController->FindNestedElementWithName("StepperCalibration"); 
  if (stepperCalibration == NULL)
  {
    LOG_ERROR("Cannot find stepperCalibration element");
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

    double centerOfRotationPx[2]={0}; 
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

	vtkSmartPointer<vtkXMLDataElement> usCalibration = configData->FindNestedElementWithName("USCalibration");
	if (usCalibration == NULL) {
		LOG_ERROR("No calibration configuration is found in the XML tree!");
		return PLUS_FAIL;
	}

	vtkSmartPointer<vtkXMLDataElement> calibrationController = usCalibration->FindNestedElementWithName("CalibrationController"); 
	if (calibrationController == NULL) {
		LOG_ERROR("Unable to read configuration");
		return PLUS_FAIL;
	}

  vtkSmartPointer<vtkXMLDataElement> stepperCalibration = calibrationController->FindNestedElementWithName("StepperCalibration");
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

