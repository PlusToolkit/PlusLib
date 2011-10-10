#include "PlusConfigure.h"
#include "PlusMath.h"
#include "vtkStepperCalibrationController.h"

#include "vtkTranslAxisCalibAlgo.h"
#include "vtkRotationAxisCalibAlgo.h"
#include "vtkSpacingCalibAlgo.h"
#include "vtkRotationEncoderCalibAlgo.h"

#include "vtkObjectFactory.h"
#include "vtkTransform.h"
#include "vtkMath.h"
#include <numeric>
#include <time.h>
#include "vtksys/SystemTools.hxx"
#include "vtkGnuplotExecuter.h"
#include "vtkHTMLGenerator.h"
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
  this->ProbeRotationAxisCalibratedOff(); 
  this->ProbeTranslationAxisCalibratedOff(); 
  this->ProbeRotationEncoderCalibratedOff(); 
  this->TemplateTranslationAxisCalibratedOff(); 

  this->AlgorithmVersion = NULL; 
  this->CalibrationStartTime = NULL; 

  this->SaveCalibrationStartTime(); 

  this->SetAlgorithmVersion("1.0.0"); 
}

//----------------------------------------------------------------------------
vtkStepperCalibrationController::~vtkStepperCalibrationController()
{
  this->SetCalibrationStartTime(NULL); 
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
  rotationAxisCalibAlgo->SetInputs(this->TrackedFrameListContainer[PROBE_ROTATION], this->Spacing); 
  rotationAxisCalibAlgo->SetMinNumberOfRotationClusters(this->MinNumberOfRotationClusters); 

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
  
  vtkSmartPointer<vtkRotationEncoderCalibAlgo> rotationEncoderCalibAlgo = vtkSmartPointer<vtkRotationEncoderCalibAlgo>::New(); 
  rotationEncoderCalibAlgo->SetInputs(this->TrackedFrameListContainer[PROBE_ROTATION], this->Spacing); 

  if ( rotationEncoderCalibAlgo->GetRotationEncoderOffset(this->ProbeRotationEncoderOffset) != PLUS_SUCCESS ) 
  {
    LOG_ERROR("Rotation encoder calibration failed!"); 
    return PLUS_FAIL; 
  }

  if ( rotationEncoderCalibAlgo->GetRotationEncoderScale(this->ProbeRotationEncoderScale) != PLUS_SUCCESS ) 
  {
    LOG_ERROR("Rotation encoder calibration failed!"); 
    return PLUS_FAIL; 
  }

  this->ProbeRotationEncoderCalibratedOn(); 

  // TODO: Need to generate report
  //rotationEncoderCalibAlgo->GenerateReport(htmlGenerator, gnuplotExecuter, inputGnuplotScriptsFolder.c_str()); 

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus vtkStepperCalibrationController::GenerateProbeRotationEncoderCalibrationReport( vtkHTMLGenerator* htmlReport, vtkGnuplotExecuter* plotter, const char* gnuplotScriptsFolder)
{
  LOG_ERROR("TODO: call report generation from vtkRotationEncoderCalibAlgo!"); 
  return PLUS_FAIL; 
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

    this->SetOfflineImageData(trackedFrameList->GetTrackedFrame(imgNumber)->GetImageData()->GetDisplayableImage());

    if ( this->SegmentationProgressCallbackFunction != NULL )
    {
      int numberOfSegmentedImages = this->GetImageDataInfo(PROBE_ROTATION).NumberOfSegmentedImages; 
      int percent = 100* numberOfSegmentedImages / this->GetImageDataInfo(PROBE_ROTATION).NumberOfImagesToAcquire; 
      (*SegmentationProgressCallbackFunction)(percent); 
    }
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

    this->SetOfflineImageData(trackedFrameList->GetTrackedFrame(imgNumber)->GetImageData()->GetDisplayableImage());

    if ( this->SegmentationProgressCallbackFunction != NULL )
    {
      int numberOfSegmentedImages = this->GetImageDataInfo(PROBE_TRANSLATION).NumberOfSegmentedImages; 
      int percent = 100* numberOfSegmentedImages / this->GetImageDataInfo(PROBE_TRANSLATION).NumberOfImagesToAcquire; 
      (*SegmentationProgressCallbackFunction)(percent); 
    }
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

    this->SetOfflineImageData(trackedFrameList->GetTrackedFrame(imgNumber)->GetImageData()->GetDisplayableImage()); 

    if ( this->SegmentationProgressCallbackFunction != NULL )
    {
      int numberOfSegmentedImages = this->GetImageDataInfo(TEMPLATE_TRANSLATION).NumberOfSegmentedImages; 
      int percent = 100* numberOfSegmentedImages / this->GetImageDataInfo(TEMPLATE_TRANSLATION).NumberOfImagesToAcquire; 
      (*SegmentationProgressCallbackFunction)(percent); 
    }
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

