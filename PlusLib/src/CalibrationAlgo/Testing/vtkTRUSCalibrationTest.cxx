/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

/*!
  \file vtkUSCalibrationControllerTest1.cxx 
  \brief This test runs a TRUS probe calibration on a recorded data set and 
  compares the results to a baseline
*/ 

#include "PlusConfigure.h"
#include "PlusMath.h"
#include "vtkTrackedFrameList.h"
#include "vtkSpacingCalibAlgo.h"
#include "vtkCenterOfRotationCalibAlgo.h"
#include "vtkBrachyStepperPhantomRegistrationAlgo.h"
#include "vtkProbeCalibrationAlgo.h"
#include "FidPatternRecognition.h"

#include "vtkXMLDataElement.h"
#include "vtkXMLUtilities.h"
#include "vtksys/CommandLineArguments.hxx" 
#include "vtksys/SystemTools.hxx"
#include "vtkMatrix4x4.h"
#include "vtkTransformRepository.h"
#include "vtkMath.h"
#include "vtkPlusConfig.h"

///////////////////////////////////////////////////////////////////
const double ERROR_THRESHOLD = 0.05; // error threshold is 5% 

int CompareCalibrationResultsWithBaseline(const char* baselineFileName, const char* currentResultFileName, int translationErrorThreshold, int rotationErrorThreshold); 

int main (int argc, char* argv[])
{ 
  int numberOfFailures(0); 
	std::string inputCalibrationSeqMetafile;
	std::string inputValidationSeqMetafile;
	std::string inputProbeRotationSeqMetafile;

	std::string inputConfigFileName;
	std::string inputBaselineFileName;
  std::string inputProbeToReferenceTransformName("ProbeToReference"); 
	double inputTranslationErrorThreshold(0); 
	double inputRotationErrorThreshold(0); 

	int verboseLevel=vtkPlusLogger::LOG_LEVEL_UNDEFINED;

	vtksys::CommandLineArguments cmdargs;
	cmdargs.Initialize(argc, argv);

	cmdargs.AddArgument("--calibration-seq-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputCalibrationSeqMetafile, "Sequence metafile name of input random stepper motion calibration dataset.");
	cmdargs.AddArgument("--validation-seq-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputValidationSeqMetafile, "Sequence metafile name of input random stepper motion validation dataset.");
	cmdargs.AddArgument("--probe-rotation-seq-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputProbeRotationSeqMetafile, "Sequence metafile name of input probe rotation dataset.");
	
  cmdargs.AddArgument("--config-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputConfigFileName, "Configuration file name");
	
	cmdargs.AddArgument("--baseline-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputBaselineFileName, "Name of file storing baseline calibration results");
	cmdargs.AddArgument("--translation-error-threshold", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputTranslationErrorThreshold, "Translation error threshold in mm.");	
	cmdargs.AddArgument("--rotation-error-threshold", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputRotationErrorThreshold, "Rotation error threshold in degrees.");	
	cmdargs.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug, 5=trace)");	

	if ( !cmdargs.Parse() )
	{
		std::cerr << "Problem parsing arguments" << std::endl;
		std::cout << "Help: " << cmdargs.GetHelp() << std::endl;
		exit(EXIT_FAILURE);
	}
  
  vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);

  // Read configuration
  vtkSmartPointer<vtkXMLDataElement> configRootElement = vtkSmartPointer<vtkXMLDataElement>::Take(
    vtkXMLUtilities::ReadElementFromFile(inputConfigFileName.c_str()));
  if (configRootElement == NULL)
  {	
    LOG_ERROR("Unable to read configuration from file " << inputConfigFileName.c_str()); 
		exit(EXIT_FAILURE);
  }
  vtkPlusConfig::GetInstance()->SetDeviceSetConfigurationData(configRootElement);

  FidPatternRecognition patternRecognition;
  PatternRecognitionError error;
	patternRecognition.ReadConfiguration(configRootElement);

  LOG_INFO("Reading probe rotation data from sequence metafile..."); 
  vtkSmartPointer<vtkTrackedFrameList> probeRotationTrackedFrameList = vtkSmartPointer<vtkTrackedFrameList>::New(); 
  if ( probeRotationTrackedFrameList->ReadFromSequenceMetafile(inputProbeRotationSeqMetafile.c_str()) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to read sequence metafile: " << inputProbeRotationSeqMetafile); 
    return EXIT_FAILURE;
  }

  LOG_INFO("Segmenting probe rotation data...");
  if (patternRecognition.RecognizePattern(probeRotationTrackedFrameList, error) != PLUS_SUCCESS)
  {
		LOG_ERROR("Error occured during segmentation of calibration images!"); 
		return EXIT_FAILURE;
  }

  LOG_INFO("Starting spacing calibration...");
  vtkSmartPointer<vtkSpacingCalibAlgo> spacingCalibAlgo = vtkSmartPointer<vtkSpacingCalibAlgo>::New(); 
  spacingCalibAlgo->SetInputs(probeRotationTrackedFrameList, patternRecognition.GetFidLineFinder()->GetNWires()); 

  double spacing[2]={0};
  if ( spacingCalibAlgo->GetSpacing(spacing) != PLUS_SUCCESS )
  {
    LOG_ERROR("Spacing calibration failed!"); 
    numberOfFailures++; 
  }
  else
  {
    LOG_INFO("Spacing: " << std::fixed << spacing[0] << "  " << spacing[1] << " mm/px"); 
  }

  LOG_INFO("Create rotation data indices vector..."); 
  std::vector<int> trackedFrameIndices(probeRotationTrackedFrameList->GetNumberOfTrackedFrames(), 0); 
  for ( unsigned int i = 0; i < probeRotationTrackedFrameList->GetNumberOfTrackedFrames(); ++i )
  {
    trackedFrameIndices[i]=i; 
  }

  LOG_INFO("Starting center of rotation calibration...");
  vtkSmartPointer<vtkCenterOfRotationCalibAlgo> centerOfRotationCalibAlgo = vtkSmartPointer<vtkCenterOfRotationCalibAlgo>::New(); 
  centerOfRotationCalibAlgo->SetInputs(probeRotationTrackedFrameList, trackedFrameIndices, spacing); 
  
  // Get center of rotation calibration output 
  double centerOfRotationPx[2] = {0}; 
  if ( centerOfRotationCalibAlgo->GetCenterOfRotationPx(centerOfRotationPx) != PLUS_SUCCESS )
  {
    LOG_ERROR("Center of rotation calibration failed!"); 
    numberOfFailures++; 
  }
  else
  {
    LOG_INFO("Center of rotation (px): " << std::fixed << centerOfRotationPx[0] << "  " << centerOfRotationPx[1]); 
  }

  // Read coordinate definitions
  vtkSmartPointer<vtkTransformRepository> transformRepository = vtkSmartPointer<vtkTransformRepository>::New();
  if ( transformRepository->ReadConfiguration(configRootElement) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to read CoordinateDefinitions!"); 
    return EXIT_FAILURE; 
  }

  // Phantom registration
  vtkSmartPointer<vtkBrachyStepperPhantomRegistrationAlgo> phantomRegistrationAlgo = vtkSmartPointer<vtkBrachyStepperPhantomRegistrationAlgo>::New(); 
  if (phantomRegistrationAlgo->ReadConfiguration(configRootElement) != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to read phantom definition!");
    return EXIT_FAILURE; 
  }
  phantomRegistrationAlgo->SetInputs(probeRotationTrackedFrameList, spacing, centerOfRotationPx, transformRepository, patternRecognition.GetFidLineFinder()->GetNWires()); 

  vtkSmartPointer<vtkMatrix4x4> tPhantomToReferenceMatrix = vtkSmartPointer<vtkMatrix4x4>::New(); 
  if ( phantomRegistrationAlgo->GetPhantomToReferenceTransformMatrix( tPhantomToReferenceMatrix ) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to register phantom frame to reference frame!"); 
    return EXIT_FAILURE; 
  }

  // Load and segment validation tracked frame list
  vtkSmartPointer<vtkTrackedFrameList> validationTrackedFrameList = vtkSmartPointer<vtkTrackedFrameList>::New();
  if ( validationTrackedFrameList->ReadFromSequenceMetafile(inputValidationSeqMetafile.c_str()) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to read tracked frames from sequence metafile from: " << inputValidationSeqMetafile ); 
    return EXIT_FAILURE; 
  }

  int numberOfSuccessfullySegmentedValidationImages = 0;
  if (patternRecognition.RecognizePattern(validationTrackedFrameList, error, &numberOfSuccessfullySegmentedValidationImages) != PLUS_SUCCESS)
  {
		LOG_ERROR("Error occured during segmentation of validation images!"); 
		return EXIT_FAILURE;
  }

  // Load and segment calibration tracked frame list
  vtkSmartPointer<vtkTrackedFrameList> calibrationTrackedFrameList = vtkSmartPointer<vtkTrackedFrameList>::New();
  if ( calibrationTrackedFrameList->ReadFromSequenceMetafile(inputCalibrationSeqMetafile.c_str()) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to read tracked frames from sequence metafile from: " << inputCalibrationSeqMetafile ); 
    return EXIT_FAILURE; 
  }

  int numberOfSuccessfullySegmentedCalibrationImages = 0;
  if (patternRecognition.RecognizePattern(calibrationTrackedFrameList, error, &numberOfSuccessfullySegmentedCalibrationImages) != PLUS_SUCCESS)
  {
		LOG_ERROR("Error occured during segmentation of calibration images!"); 
		return EXIT_FAILURE;
  }

  LOG_INFO("Segmentation success rate of validation images: " << numberOfSuccessfullySegmentedValidationImages << " out of " << validationTrackedFrameList->GetNumberOfTrackedFrames());

  // Initialize the probe calibration algo 
	vtkSmartPointer<vtkProbeCalibrationAlgo> probeCal = vtkSmartPointer<vtkProbeCalibrationAlgo>::New(); 
	probeCal->ReadConfiguration(configRootElement); 

  // Calibrate
  if (probeCal->Calibrate( validationTrackedFrameList, calibrationTrackedFrameList, transformRepository, patternRecognition.GetFidLineFinder()->GetNWires()) != PLUS_SUCCESS)
  {
    LOG_ERROR("Calibration failed!");
		return EXIT_FAILURE;
  }

  // Compare results
	std::string currentConfigFileName = vtkPlusConfig::GetInstance()->GetOutputPath( 
    vtkPlusConfig::GetInstance()->GetApplicationStartTimestamp() + ".Calibration.results.xml" );
	if ( CompareCalibrationResultsWithBaseline( inputBaselineFileName.c_str(), currentConfigFileName.c_str(), inputTranslationErrorThreshold, inputRotationErrorThreshold ) !=0 )
	{
    numberOfFailures++; 
		LOG_ERROR("Comparison of calibration data to baseline failed");
	}

  if ( numberOfFailures > 0 )
  {
    std::cout << "Test exited with failures!!!" << std::endl; 
	  return EXIT_FAILURE;
  }

	std::cout << "Exit success!!!" << std::endl; 
	return EXIT_SUCCESS; 
}

//----------------------------------------------------------------------------

// return the number of differences
int CompareCalibrationResultsWithBaseline(const char* baselineFileName, const char* currentResultFileName, int translationErrorThreshold, int rotationErrorThreshold)
{
  int numberOfFailures=0;

  vtkSmartPointer<vtkXMLDataElement> baselineRootElem = vtkSmartPointer<vtkXMLDataElement>::Take(
    vtkXMLUtilities::ReadElementFromFile(baselineFileName));
  vtkSmartPointer<vtkXMLDataElement> currentRootElem = vtkSmartPointer<vtkXMLDataElement>::Take(
    vtkXMLUtilities::ReadElementFromFile(currentResultFileName));

  // check to make sure we have the right element
  if (baselineRootElem == NULL )
  {
    LOG_ERROR("Reading baseline data file failed: " << baselineFileName);
    return ++numberOfFailures;
  }
  if (currentRootElem == NULL )
  {
    LOG_ERROR("Reading newly generated data file failed: " << currentResultFileName);
    return ++numberOfFailures;
  }

  {	//<CalibrationResults>
    vtkXMLDataElement* calibrationResultsBaseline = baselineRootElem->FindNestedElementWithName("CalibrationResults"); 
    vtkXMLDataElement* calibrationResults = currentRootElem->FindNestedElementWithName("CalibrationResults"); 

    if ( calibrationResultsBaseline == NULL) 
    {
      LOG_ERROR("Reading baseline CalibrationResults tag failed: " << baselineFileName);
      return ++numberOfFailures;
    }

    if ( calibrationResults == NULL) 
    {
      LOG_ERROR("Reading current CalibrationResults tag failed: " << currentResultFileName);
      return ++numberOfFailures;
    }

    {	// <Transform>
      vtkXMLDataElement* transformBaseline = calibrationResultsBaseline->FindNestedElementWithName("Transform"); 
      vtkXMLDataElement* transform = calibrationResults->FindNestedElementWithName("Transform");

      if ( transformBaseline == NULL) 
      {
        LOG_ERROR("Reading baseline Transform tag failed: " << baselineFileName);
        return ++numberOfFailures;
      }

      if ( transform == NULL) 
      {
        LOG_ERROR("Reading current Transform tag failed: " << currentResultFileName);
        return ++numberOfFailures;
      }

      // ImageToProbe
      double blTransformImageToProbe[16]; 
      double cTransformImageToProbe[16]; 
      const char* blFrom = transformBaseline->GetAttribute("From");
      const char* cFrom = transform->GetAttribute("From");
      const char* blTo = transformBaseline->GetAttribute("To");
      const char* cTo = transform->GetAttribute("To");

      if (STRCASECMP(blFrom, "Image") != 0 || STRCASECMP(blTo, "Probe"))
      {
        LOG_ERROR("Baseline From and To tags are invalid!");
        numberOfFailures++;			
      }
      else if (STRCASECMP(cFrom, "Image") != 0 || STRCASECMP(cTo, "Probe"))
      {
        LOG_ERROR("Current From and To tags are invalid!");
        numberOfFailures++;			
      }
      else if (!transformBaseline->GetVectorAttribute("Matrix", 16, blTransformImageToProbe))
      {
        LOG_ERROR("Baseline Matrix tag is missing");
        numberOfFailures++;			
      }
      else if (!transform->GetVectorAttribute("Matrix", 16, cTransformImageToProbe))
      {
        LOG_ERROR("Current Matrix tag is missing");
        numberOfFailures++;			
      }
      else
      { 
        vtkSmartPointer<vtkMatrix4x4> baseTransMatrix = vtkSmartPointer<vtkMatrix4x4>::New(); 
        vtkSmartPointer<vtkMatrix4x4> currentTransMatrix = vtkSmartPointer<vtkMatrix4x4>::New(); 
        for ( int i = 0; i < 4; i++) 
        {
          for ( int j = 0; j < 4; j++)
          {
            baseTransMatrix->SetElement(i,j, blTransformImageToProbe[4*i + j]); 
            currentTransMatrix->SetElement(i,j, cTransformImageToProbe[4*i + j]); 
          }
        }

        double translationError = PlusMath::GetPositionDifference(baseTransMatrix, currentTransMatrix); 
        if ( translationError > translationErrorThreshold )
        {
          LOG_ERROR("TransformImageToProbe translation error is higher than expected: " << translationError << " mm (threshold: " << translationErrorThreshold << " mm). " );
          numberOfFailures++;
        }

        double rotationError = PlusMath::GetOrientationDifference(baseTransMatrix, currentTransMatrix); 
        if ( rotationError > rotationErrorThreshold )
        {
          LOG_ERROR("TransformImageToProbe rotation error is higher than expected: " << rotationError << " degree (threshold: " << rotationErrorThreshold << " degree). " );
          numberOfFailures++;
        }
      }
    } // </Transforms>
  } // </CalibrationResults>


  {	// <ErrorReport>
    vtkXMLDataElement* errorReportBaseline = baselineRootElem->FindNestedElementWithName("ErrorReport"); 
    vtkXMLDataElement* errorReport = currentRootElem->FindNestedElementWithName("ErrorReport");

    if ( errorReportBaseline == NULL) 
    {
      LOG_ERROR("Reading baseline ErrorReports tag failed: " << baselineFileName);
      return ++numberOfFailures;
    }

    if ( errorReport == NULL) 
    {
      LOG_ERROR("Reading current ErrorReports tag failed: " << currentResultFileName);
      return ++numberOfFailures;
    }

    { // <ReprojectionError3DStatistics>
      vtkXMLDataElement* reprojectionError3DStatisticsBaseline = errorReportBaseline->FindNestedElementWithName("ReprojectionError3DStatistics"); 
      vtkXMLDataElement* reprojectionError3DStatistics = errorReport->FindNestedElementWithName("ReprojectionError3DStatistics");

      if ( reprojectionError3DStatisticsBaseline == NULL || reprojectionError3DStatistics == NULL ) 
      {
        LOG_ERROR("Reading ReprojectionError3DStatistics tag failed");
        return ++numberOfFailures;
      }

      double blReprojectionError3DValidationMeanMm = 0.0;
      double blReprojectionError3DValidationStdDevMm = 0.0;
	    if ( ! reprojectionError3DStatisticsBaseline->GetScalarAttribute("ValidationMeanMm", blReprojectionError3DValidationMeanMm)
        || ! reprojectionError3DStatisticsBaseline->GetScalarAttribute("ValidationStdDevMm", blReprojectionError3DValidationStdDevMm) )
      {
        LOG_ERROR("Reading baseline validation ReprojectionError3DStatistics statistics failed: " << baselineFileName);
        return ++numberOfFailures;
      }

      double cReprojectionError3DValidationMeanMm = 0.0;
      double cReprojectionError3DValidationStdDevMm = 0.0;
	    if ( ! reprojectionError3DStatistics->GetScalarAttribute("ValidationMeanMm", cReprojectionError3DValidationMeanMm)
        || ! reprojectionError3DStatistics->GetScalarAttribute("ValidationStdDevMm", cReprojectionError3DValidationStdDevMm) )
      {
        LOG_ERROR("Reading current validation ReprojectionError3DStatistics statistics failed: " << currentResultFileName);
        return ++numberOfFailures;
      }

      double ratioValidationMean = 1.0 * blReprojectionError3DValidationMeanMm / cReprojectionError3DValidationMeanMm; 
      if ( ratioValidationMean > 1 + ERROR_THRESHOLD || ratioValidationMean < 1 - ERROR_THRESHOLD )
      {
        LOG_ERROR("ReprojectionError3DStatistics/ValidationMeanMm mismatch: current=" << cReprojectionError3DValidationMeanMm << ", baseline=" << blReprojectionError3DValidationMeanMm);
        return ++numberOfFailures;
      }
      double ratioValidationStdDev = 1.0 * blReprojectionError3DValidationStdDevMm / cReprojectionError3DValidationStdDevMm; 
      if ( ratioValidationStdDev > 1 + ERROR_THRESHOLD || ratioValidationStdDev < 1 - ERROR_THRESHOLD )
      {
        LOG_ERROR("ReprojectionError3DStatistics/ValidationStdDevMm mismatch: current=" << cReprojectionError3DValidationStdDevMm << ", baseline=" << blReprojectionError3DValidationStdDevMm);
        return ++numberOfFailures;
      }

      double blReprojectionError3DCalibrationMeanMm = 0.0;
      double blReprojectionError3DCalibrationStdDevMm = 0.0;
	    if ( ! reprojectionError3DStatisticsBaseline->GetScalarAttribute("CalibrationMeanMm", blReprojectionError3DCalibrationMeanMm)
        || ! reprojectionError3DStatisticsBaseline->GetScalarAttribute("CalibrationStdDevMm", blReprojectionError3DCalibrationStdDevMm) )
      {
        LOG_ERROR("Reading baseline calibration ReprojectionError3DStatistics statistics failed: " << baselineFileName);
        return ++numberOfFailures;
      }

      double cReprojectionError3DCalibrationMeanMm = 0.0;
      double cReprojectionError3DCalibrationStdDevMm = 0.0;
	    if ( ! reprojectionError3DStatistics->GetScalarAttribute("CalibrationMeanMm", cReprojectionError3DCalibrationMeanMm)
        || ! reprojectionError3DStatistics->GetScalarAttribute("CalibrationStdDevMm", cReprojectionError3DCalibrationStdDevMm) )
      {
        LOG_ERROR("Reading current calibration ReprojectionError3DStatistics statistics failed: " << currentResultFileName);
        return ++numberOfFailures;
      }

      double ratioCalibrationMean = 1.0 * blReprojectionError3DCalibrationMeanMm / cReprojectionError3DCalibrationMeanMm; 
      if ( ratioCalibrationMean > 1 + ERROR_THRESHOLD || ratioCalibrationMean < 1 - ERROR_THRESHOLD )
      {
        LOG_ERROR("ReprojectionError3DStatistics/CalibrationMeanMm mismatch: current=" << cReprojectionError3DCalibrationMeanMm << ", baseline=" << blReprojectionError3DCalibrationMeanMm);
        ++numberOfFailures;
      }
      double ratioCalibrationStdDev = 1.0 * blReprojectionError3DCalibrationStdDevMm / cReprojectionError3DCalibrationStdDevMm; 
      if ( ratioCalibrationStdDev > 1 + ERROR_THRESHOLD || ratioCalibrationStdDev < 1 - ERROR_THRESHOLD )
      {
        LOG_ERROR("ReprojectionError3DStatistics/CalibrationStdDevMm mismatch: current=" << cReprojectionError3DCalibrationStdDevMm << ", baseline=" << blReprojectionError3DCalibrationStdDevMm);
        ++numberOfFailures;
      }
    } // </ReprojectionError3DStatistics>

    { // <ReprojectionError2DStatistics>
      vtkXMLDataElement* reprojectionError2DStatisticsBaseline = errorReportBaseline->FindNestedElementWithName("ReprojectionError2DStatistics"); 
      vtkXMLDataElement* reprojectionError2DStatistics = errorReport->FindNestedElementWithName("ReprojectionError2DStatistics");

      if ( reprojectionError2DStatisticsBaseline == NULL || reprojectionError2DStatistics == NULL ) 
      {
        LOG_ERROR("Reading ReprojectionError2DStatistics tag failed");
        return ++numberOfFailures;
      }

      // <Wire>
      for ( int wireIndex = 0; wireIndex < reprojectionError2DStatisticsBaseline->GetNumberOfNestedElements(); ++wireIndex )
      {
        vtkXMLDataElement* wireBaseline = reprojectionError2DStatisticsBaseline->GetNestedElement(wireIndex); 
        vtkXMLDataElement* wire = reprojectionError2DStatistics->GetNestedElement(wireIndex); 
        if ( !wireBaseline || !wire || STRCASECMP( wireBaseline->GetName(), "Wire" ) != 0 || STRCASECMP( wire->GetName(), "Wire" ) != 0 )
        {
          LOG_ERROR("Invalid Wire element in ReprojectionError2DStatistics");
          ++numberOfFailures;
          continue;
        }

        if ( STRCASECMP( wireBaseline->GetAttribute("Name"), wire->GetAttribute("Name") ) != 0 )
        {
          LOG_ERROR("Wire name mismatch: " << wireBaseline->GetAttribute("Name") << " <> " << wire->GetAttribute("Name"));
          ++numberOfFailures;
        }

        double blValidationMeanPx[2];
        double blValidationStdDevPx[2];
	      if ( ! wireBaseline->GetVectorAttribute("ValidationMeanPx", 2, blValidationMeanPx)
          || ! wireBaseline->GetVectorAttribute("ValidationStdDevPx", 2, blValidationStdDevPx) )
        {
          LOG_ERROR("Reading baseline validation ReprojectionError2DStatistics failed for wire " << wireIndex);
          ++numberOfFailures;
          continue;
        }

        double cValidationMeanPx[2];
        double cValidationStdDevPx[2];
	      if ( ! wire->GetVectorAttribute("ValidationMeanPx", 2, cValidationMeanPx)
          || ! wire->GetVectorAttribute("ValidationStdDevPx", 2, cValidationStdDevPx) )
        {
          LOG_ERROR("Reading current validation ReprojectionError2DStatistics failed for wire " << wireIndex);
          ++numberOfFailures;
          continue;
        }

        for ( int i = 0; i < 2; i++) 
        {
          double ratioMean = 1.0 * blValidationMeanPx[i] / cValidationMeanPx[i]; 
          if ( ratioMean > 1 + ERROR_THRESHOLD || ratioMean < 1 - ERROR_THRESHOLD )
          {
            LOG_ERROR("ValidationMeanPx mismatch for wire " << wireIndex << ": current=" << cValidationMeanPx[i] << ", baseline=" << blValidationMeanPx[i]);
            ++numberOfFailures;
          }
          double ratioStdDev = 1.0 * blValidationStdDevPx[i] / cValidationStdDevPx[i]; 
          if ( ratioStdDev > 1 + ERROR_THRESHOLD || ratioStdDev < 1 - ERROR_THRESHOLD )
          {
            LOG_ERROR("ValidationStdDevPx mismatch for wire " << wireIndex << ": current=" << cValidationStdDevPx[i] << ", baseline=" << blValidationStdDevPx[i]);
            ++numberOfFailures;
          }
        }

        double blCalibrationMeanPx[2];
        double blCalibrationStdDevPx[2];
	      if ( ! wireBaseline->GetVectorAttribute("CalibrationMeanPx", 2, blCalibrationMeanPx)
          || ! wireBaseline->GetVectorAttribute("CalibrationStdDevPx", 2, blCalibrationStdDevPx) )
        {
          LOG_ERROR("Reading baseline calibration ReprojectionError2DStatistics failed for wire " << wireIndex);
          ++numberOfFailures;
          continue;
        }

        double cCalibrationMeanPx[2];
        double cCalibrationStdDevPx[2];
	      if ( ! wire->GetVectorAttribute("CalibrationMeanPx", 2, cCalibrationMeanPx)
          || ! wire->GetVectorAttribute("CalibrationStdDevPx", 2, cCalibrationStdDevPx) )
        {
          LOG_ERROR("Reading current calibration ReprojectionError2DStatistics failed for wire " << wireIndex);
          ++numberOfFailures;
          continue;
        }

        for ( int i = 0; i < 2; i++) 
        {
          double ratioMean = 1.0 * blCalibrationMeanPx[i] / cCalibrationMeanPx[i]; 
          if ( ratioMean > 1 + ERROR_THRESHOLD || ratioMean < 1 - ERROR_THRESHOLD )
          {
            LOG_ERROR("CalibrationMeanPx mismatch for wire " << wireIndex << ": current=" << cCalibrationMeanPx[i] << ", baseline=" << blCalibrationMeanPx[i]);
            ++numberOfFailures;
          }
          double ratioStdDev = 1.0 * blCalibrationStdDevPx[i] / cCalibrationStdDevPx[i]; 
          if ( ratioStdDev > 1 + ERROR_THRESHOLD || ratioStdDev < 1 - ERROR_THRESHOLD )
          {
            LOG_ERROR("CalibrationStdDevPx mismatch for wire " << wireIndex << ": current=" << cCalibrationStdDevPx[i] << ", baseline=" << blCalibrationStdDevPx[i]);
            ++numberOfFailures;
          }
        }
      } // </Wire>
    } // </ReprojectionError2DStatistics>

    {	// <ValidationData>
      vtkXMLDataElement* validationDataBaseline = errorReportBaseline->FindNestedElementWithName("ValidationData"); 
      vtkXMLDataElement* validationData = errorReport->FindNestedElementWithName("ValidationData");

      if ( validationDataBaseline == NULL || validationData == NULL ) 
      {
        LOG_ERROR("Reading ValidationData tag failed");
        return ++numberOfFailures;
      }

      for ( int frameIndex = 0; frameIndex < validationDataBaseline->GetNumberOfNestedElements(); ++frameIndex ) // <Frame>
      {
        vtkXMLDataElement* frameBaseline = validationDataBaseline->GetNestedElement(frameIndex); 
        vtkXMLDataElement* frame = validationData->GetNestedElement(frameIndex); 
        if ( !frameBaseline || !frame || STRCASECMP( frameBaseline->GetName(), "Frame" ) != 0 || STRCASECMP( frame->GetName(), "Frame" ) != 0 )
        {
          LOG_ERROR("Invalid Frame element #" << frameIndex);
          ++numberOfFailures;
          continue;
        }

        const char* segmentationStatusBaseline = frameBaseline->GetAttribute("SegmentationStatus"); 
        const char* segmentationStatus = frame->GetAttribute("SegmentationStatus"); 

        if ( STRCASECMP( segmentationStatusBaseline, segmentationStatus ) != 0 )
        {
          LOG_ERROR("SegmentationStatus mismatch in Frame #" << frameIndex << ": current=" << segmentationStatus << ", baseline=" << segmentationStatusBaseline);
          ++numberOfFailures;
        }

        if ( STRCASECMP( segmentationStatusBaseline, "OK" ) == 0 )
        {
          { // <SegmentedPoints>
            vtkXMLDataElement* segmentedPointsBaseline = frameBaseline->FindNestedElementWithName("SegmentedPoints"); 
            vtkXMLDataElement* segmentedPoints = frame->FindNestedElementWithName("SegmentedPoints");

            if ( segmentedPointsBaseline == NULL || segmentedPoints == NULL ) 
            {
              LOG_ERROR("Reading SegmentedPoints tag in Frame #" << frameIndex << "failed");
              ++numberOfFailures;
              continue;
            }

            // <Point>
            for ( int pointIndex = 0; pointIndex < segmentedPointsBaseline->GetNumberOfNestedElements(); ++pointIndex )
            {
              vtkXMLDataElement* pointBaseline = segmentedPointsBaseline->GetNestedElement(pointIndex); 
              vtkXMLDataElement* point = segmentedPoints->GetNestedElement(pointIndex); 
              if ( !pointBaseline || !point || STRCASECMP( pointBaseline->GetName(), "Point" ) != 0 || STRCASECMP( point->GetName(), "Point" ) != 0 )
              {
                LOG_ERROR("Invalid Point element in Frame #" << frameIndex);
                ++numberOfFailures;
              }

              if ( STRCASECMP( pointBaseline->GetAttribute("WireName"), point->GetAttribute("WireName") ) != 0 )
              {
                LOG_ERROR("Wire name mismatch: " << pointBaseline->GetAttribute("Name") << " <> " << point->GetAttribute("Name"));
                ++numberOfFailures;
              }

              double blPosition[3];
              double cPosition[3];
              if ( ! pointBaseline->GetVectorAttribute("Position", 3, blPosition)
                || ! point->GetVectorAttribute("Position", 3, cPosition) )
              {
                LOG_ERROR("Reading Position of Point #" << pointIndex << " in Frame #" << frameIndex << "failed!");
                ++numberOfFailures;
                continue;
              }

              for ( int i = 0; i < 3; i++) 
              {
                double ratio = 1.0 * blPosition[i] / cPosition[i]; 
                if ( ratio > 1 + ERROR_THRESHOLD || ratio < 1 - ERROR_THRESHOLD )
                {
                  LOG_ERROR("Position component " << i << " mismatch: current=" << cPosition[i] << ", baseline=" << blPosition[i]);
                  ++numberOfFailures;
                }
              }
            }
          } // </SegmentedPoints>

          { // <ReprojectionError3DList>
            vtkXMLDataElement* reprojectionError3DListBaseline = frameBaseline->FindNestedElementWithName("ReprojectionError3DList"); 
            vtkXMLDataElement* reprojectionError3DList = frame->FindNestedElementWithName("ReprojectionError3DList");

            if ( reprojectionError3DListBaseline == NULL || reprojectionError3DList == NULL ) 
            {
              LOG_ERROR("Reading ReprojectionError3DList tag in Frame #" << frameIndex << " failed");
              ++numberOfFailures;
              continue;
            }

            // <ReprojectionError3D>
            for ( int reprojectionError3DIndex = 0; reprojectionError3DIndex < reprojectionError3DListBaseline->GetNumberOfNestedElements(); ++reprojectionError3DIndex )
            {
              vtkXMLDataElement* reprojectionError3DBaseline = reprojectionError3DListBaseline->GetNestedElement(reprojectionError3DIndex); 
              vtkXMLDataElement* reprojectionError3D = reprojectionError3DList->GetNestedElement(reprojectionError3DIndex); 
              if ( !reprojectionError3DBaseline || !reprojectionError3D || STRCASECMP( reprojectionError3DBaseline->GetName(), "ReprojectionError3D" ) != 0 || STRCASECMP( reprojectionError3D->GetName(), "ReprojectionError3D" ) != 0 )
              {
                LOG_ERROR("Invalid ReprojectionError3D element in Frame #" << frameIndex);
                ++numberOfFailures;
                continue;
              }

              if ( STRCASECMP( reprojectionError3DBaseline->GetAttribute("WireName"), reprojectionError3D->GetAttribute("WireName") ) != 0 )
              {
                LOG_ERROR("Wire name mismatch: " << reprojectionError3DBaseline->GetAttribute("Name") << " <> " << reprojectionError3D->GetAttribute("Name"));
                ++numberOfFailures;
              }

              double blErrorMm = 0.0;
              double cErrorMm = 0.0;
	            if ( ! reprojectionError3DBaseline->GetScalarAttribute("ErrorMm", blErrorMm)
                || ! reprojectionError3D->GetScalarAttribute("ErrorMm", cErrorMm) )
              {
                LOG_ERROR("Reading ErrorMm in ReprojectionError3D #" << reprojectionError3DIndex << " in Frame #" << frameIndex << "failed!");
                ++numberOfFailures;
                continue;
              }

              double ratio = 1.0 * blErrorMm / cErrorMm; 
              if ( ratio > 1 + ERROR_THRESHOLD || ratio < 1 - ERROR_THRESHOLD )
              {
                LOG_ERROR("ErrorMm mismatch: current=" << cErrorMm << ", baseline=" << blErrorMm);
                ++numberOfFailures;
              }
            }
          } // </ReprojectionError3DList>

          { // <ReprojectionError2DList>
            vtkXMLDataElement* reprojectionError2DListBaseline = frameBaseline->FindNestedElementWithName("ReprojectionError2DList"); 
            vtkXMLDataElement* reprojectionError2DList = frame->FindNestedElementWithName("ReprojectionError2DList");

            if ( reprojectionError2DListBaseline == NULL || reprojectionError2DList == NULL ) 
            {
              LOG_ERROR("Reading ReprojectionError2DList tag in Frame #" << frameIndex << "failed");
              ++numberOfFailures;
              continue;
            }

            // <ReprojectionError2D>
            for ( int reprojectionError2DIndex = 0; reprojectionError2DIndex < reprojectionError2DListBaseline->GetNumberOfNestedElements(); ++reprojectionError2DIndex )
            {
              vtkXMLDataElement* reprojectionError2DBaseline = reprojectionError2DListBaseline->GetNestedElement(reprojectionError2DIndex); 
              vtkXMLDataElement* reprojectionError2D = reprojectionError2DList->GetNestedElement(reprojectionError2DIndex); 
              if ( !reprojectionError2DBaseline || !reprojectionError2D || STRCASECMP( reprojectionError2DBaseline->GetName(), "ReprojectionError2D" ) != 0 || STRCASECMP( reprojectionError2D->GetName(), "ReprojectionError2D" ) != 0 )
              {
                LOG_ERROR("Invalid ReprojectionError2D element in Frame #" << frameIndex);
                ++numberOfFailures;
                continue;
              }

              if ( STRCASECMP( reprojectionError2DBaseline->GetAttribute("WireName"), reprojectionError2D->GetAttribute("WireName") ) != 0 )
              {
                LOG_ERROR("Wire name mismatch: " << reprojectionError2DBaseline->GetAttribute("Name") << " <> " << reprojectionError2D->GetAttribute("Name"));
                ++numberOfFailures;
              }

              double blErrorPx[2];
              double cErrorPx[2];
              if ( ! reprojectionError2DBaseline->GetVectorAttribute("ErrorPx", 2, blErrorPx)
                || ! reprojectionError2D->GetVectorAttribute("ErrorPx", 2, cErrorPx) )
              {
                LOG_ERROR("Reading ErrorPx of reprojectionError2D #" << reprojectionError2DIndex << " in Frame #" << frameIndex << "failed!");
                ++numberOfFailures;
                continue;
              }

              for ( int i = 0; i < 2; i++) 
              {
                double ratio = 1.0 * blErrorPx[i] / cErrorPx[i]; 
                if ( ratio > 1 + ERROR_THRESHOLD || ratio < 1 - ERROR_THRESHOLD )
                {
                  LOG_ERROR("ErrorPx component " << i << " mismatch: current=" << cErrorPx[i] << ", baseline=" << blErrorPx[i]);
                  ++numberOfFailures;
                }
              }
            }
          } // </ReprojectionError2DList>
        } // If SegmentationStatus is OK
      } // </Frame>
    } // </ValidationData>

    {	// <CalibrationData>
      vtkXMLDataElement* calibrationDataBaseline = errorReportBaseline->FindNestedElementWithName("CalibrationData"); 
      vtkXMLDataElement* calibrationData = errorReport->FindNestedElementWithName("CalibrationData");

      if ( calibrationDataBaseline == NULL || calibrationData == NULL ) 
      {
        LOG_ERROR("Reading CalibrationData tag failed");
        return ++numberOfFailures;
      }

      for ( int frameIndex = 0; frameIndex < calibrationDataBaseline->GetNumberOfNestedElements(); ++frameIndex ) // <Frame>
      {
        vtkXMLDataElement* frameBaseline = calibrationDataBaseline->GetNestedElement(frameIndex); 
        vtkXMLDataElement* frame = calibrationData->GetNestedElement(frameIndex); 
        if ( !frameBaseline || !frame || STRCASECMP( frameBaseline->GetName(), "Frame" ) != 0 || STRCASECMP( frame->GetName(), "Frame" ) != 0 )
        {
          LOG_ERROR("Invalid Frame element #" << frameIndex);
          ++numberOfFailures;
          continue;
        }

        const char* segmentationStatusBaseline = frameBaseline->GetAttribute("SegmentationStatus"); 
        const char* segmentationStatus = frame->GetAttribute("SegmentationStatus"); 

        if ( STRCASECMP( segmentationStatusBaseline, segmentationStatus ) != 0 )
        {
          LOG_ERROR("SegmentationStatus mismatch in Frame #" << frameIndex << ": current=" << segmentationStatus << ", baseline=" << segmentationStatusBaseline);
          ++numberOfFailures;
        }

        if ( STRCASECMP( segmentationStatusBaseline, "OK" ) == 0 )
        {
          { // <SegmentedPoints>
            vtkXMLDataElement* segmentedPointsBaseline = frameBaseline->FindNestedElementWithName("SegmentedPoints"); 
            vtkXMLDataElement* segmentedPoints = frame->FindNestedElementWithName("SegmentedPoints");

            if ( segmentedPointsBaseline == NULL || segmentedPoints == NULL ) 
            {
              LOG_ERROR("Reading SegmentedPoints tag in Frame #" << frameIndex << "failed");
              ++numberOfFailures;
              continue;
            }

            // <Point>
            for ( int pointIndex = 0; pointIndex < segmentedPointsBaseline->GetNumberOfNestedElements(); ++pointIndex )
            {
              vtkXMLDataElement* pointBaseline = segmentedPointsBaseline->GetNestedElement(pointIndex); 
              vtkXMLDataElement* point = segmentedPoints->GetNestedElement(pointIndex); 
              if ( !pointBaseline || !point || STRCASECMP( pointBaseline->GetName(), "Point" ) != 0 || STRCASECMP( point->GetName(), "Point" ) != 0 )
              {
                LOG_ERROR("Invalid Point element in Frame #" << frameIndex);
                ++numberOfFailures;
                continue;
              }

              if ( STRCASECMP( pointBaseline->GetAttribute("WireName"), point->GetAttribute("WireName") ) != 0 )
              {
                LOG_ERROR("Wire name mismatch: " << pointBaseline->GetAttribute("Name") << " <> " << point->GetAttribute("Name"));
                ++numberOfFailures;
              }

              double blPosition[3];
              double cPosition[3];
              if ( ! pointBaseline->GetVectorAttribute("Position", 3, blPosition)
                || ! point->GetVectorAttribute("Position", 3, cPosition) )
              {
                LOG_ERROR("Reading Position of Point #" << pointIndex << " in Frame #" << frameIndex << "failed!");
                ++numberOfFailures;
                continue;
              }

              for ( int i = 0; i < 3; i++) 
              {
                double ratio = 1.0 * blPosition[i] / cPosition[i]; 
                if ( ratio > 1 + ERROR_THRESHOLD || ratio < 1 - ERROR_THRESHOLD )
                {
                  LOG_ERROR("Position component " << i << " mismatch: current=" << cPosition[i] << ", baseline=" << blPosition[i]);
                  ++numberOfFailures;
                }
              }
            }
          } // </SegmentedPoints>

          { // <MiddleWires>
            vtkXMLDataElement* middleWiresBaseline = frameBaseline->FindNestedElementWithName("MiddleWires"); 
            vtkXMLDataElement* middleWires = frame->FindNestedElementWithName("MiddleWires");

            if ( middleWiresBaseline == NULL || middleWires == NULL ) 
            {
              LOG_ERROR("Reading MiddleWires tag in Frame #" << frameIndex << "failed");
              ++numberOfFailures;
              continue;
            }

            // <MiddleWire>
            for ( int middleWireIndex = 0; middleWireIndex < middleWiresBaseline->GetNumberOfNestedElements(); ++middleWireIndex )
            {
              vtkXMLDataElement* middleWireBaseline = middleWiresBaseline->GetNestedElement(middleWireIndex); 
              vtkXMLDataElement* middleWire = middleWires->GetNestedElement(middleWireIndex); 
              if ( !middleWireBaseline || !middleWire || STRCASECMP( middleWireBaseline->GetName(), "MiddleWire" ) != 0 || STRCASECMP( middleWire->GetName(), "MiddleWire" ) != 0 )
              {
                LOG_ERROR("Invalid MiddleWire element in Frame #" << frameIndex);
                ++numberOfFailures;
                continue;
              }

              double blPositionInImageFrame[3];
              double cPositionInImageFrame[3];
              if ( ! middleWireBaseline->GetVectorAttribute("PositionInImageFrame", 3, blPositionInImageFrame)
                || ! middleWire->GetVectorAttribute("PositionInImageFrame", 3, cPositionInImageFrame) )
              {
                LOG_ERROR("Reading PositionInImageFrame of MiddleWire #" << middleWireIndex << " in Frame #" << frameIndex << "failed!");
                ++numberOfFailures;
                continue;
              }

              for ( int i = 0; i < 3; i++) 
              {
                double ratio = 1.0 * blPositionInImageFrame[i] / cPositionInImageFrame[i]; 
                if ( ratio > 1 + ERROR_THRESHOLD || ratio < 1 - ERROR_THRESHOLD )
                {
                  LOG_ERROR("PositionInImageFrame component " << i << " mismatch (MiddleWire #" << middleWireIndex << " in Frame #" << frameIndex << "): current=" << cPositionInImageFrame[i] << ", baseline=" << blPositionInImageFrame[i]);
                  ++numberOfFailures;
                }
              }

              double blPositionInProbeFrame[3];
              double cPositionInProbeFrame[3];
              if ( ! middleWireBaseline->GetVectorAttribute("PositionInProbeFrame", 3, blPositionInProbeFrame)
                || ! middleWire->GetVectorAttribute("PositionInProbeFrame", 3, cPositionInProbeFrame) )
              {
                LOG_ERROR("Reading PositionInProbeFrame of MiddleWire #" << middleWireIndex << " in Frame #" << frameIndex << "failed!");
                ++numberOfFailures;
                continue;
              }

              for ( int i = 0; i < 3; i++) 
              {
                double ratio = 1.0 * blPositionInProbeFrame[i] / cPositionInProbeFrame[i]; 
                if ( ratio > 1 + ERROR_THRESHOLD || ratio < 1 - ERROR_THRESHOLD )
                {
                  LOG_ERROR("PositionInProbeFrame component " << i << " mismatch (MiddleWire #" << middleWireIndex << " in Frame #" << frameIndex << "): current=" << cPositionInProbeFrame[i] << ", baseline=" << blPositionInProbeFrame[i]);
                  ++numberOfFailures;
                }
              }
            }
          } // </MiddleWires>

          { // <ReprojectionError3DList>
            vtkXMLDataElement* reprojectionError3DListBaseline = frameBaseline->FindNestedElementWithName("ReprojectionError3DList"); 
            vtkXMLDataElement* reprojectionError3DList = frame->FindNestedElementWithName("ReprojectionError3DList");

            if ( reprojectionError3DListBaseline == NULL || reprojectionError3DList == NULL ) 
            {
              LOG_ERROR("Reading ReprojectionError3DList tag in Frame #" << frameIndex << " failed");
              ++numberOfFailures;
              continue;
            }

            // <ReprojectionError3D>
            for ( int reprojectionError3DIndex = 0; reprojectionError3DIndex < reprojectionError3DListBaseline->GetNumberOfNestedElements(); ++reprojectionError3DIndex )
            {
              vtkXMLDataElement* reprojectionError3DBaseline = reprojectionError3DListBaseline->GetNestedElement(reprojectionError3DIndex); 
              vtkXMLDataElement* reprojectionError3D = reprojectionError3DList->GetNestedElement(reprojectionError3DIndex); 
              if ( !reprojectionError3DBaseline || !reprojectionError3D || STRCASECMP( reprojectionError3DBaseline->GetName(), "ReprojectionError3D" ) != 0 || STRCASECMP( reprojectionError3D->GetName(), "ReprojectionError3D" ) != 0 )
              {
                LOG_ERROR("Invalid ReprojectionError3D element in Frame #" << frameIndex);
                ++numberOfFailures;
                continue;
              }

              if ( STRCASECMP( reprojectionError3DBaseline->GetAttribute("WireName"), reprojectionError3D->GetAttribute("WireName") ) != 0 )
              {
                LOG_ERROR("Wire name mismatch: " << reprojectionError3DBaseline->GetAttribute("Name") << " <> " << reprojectionError3D->GetAttribute("Name"));
                ++numberOfFailures;
              }

              double blErrorMm = 0.0;
              double cErrorMm = 0.0;
	            if ( ! reprojectionError3DBaseline->GetScalarAttribute("ErrorMm", blErrorMm)
                || ! reprojectionError3D->GetScalarAttribute("ErrorMm", cErrorMm) )
              {
                LOG_ERROR("Reading ErrorMm in ReprojectionError3D #" << reprojectionError3DIndex << " in Frame #" << frameIndex << "failed!");
                ++numberOfFailures;
                continue;
              }

              double ratio = 1.0 * blErrorMm / cErrorMm; 
              if ( ratio > 1 + ERROR_THRESHOLD || ratio < 1 - ERROR_THRESHOLD )
              {
                LOG_ERROR("ReprojectionError3D ErrorMm mismatch (Frame #" << frameIndex <<"): current=" << cErrorMm << ", baseline=" << blErrorMm);
                ++numberOfFailures;
              }
            }
          } // </ReprojectionError3DList>

          { // <ReprojectionError2DList>
            vtkXMLDataElement* reprojectionError2DListBaseline = frameBaseline->FindNestedElementWithName("ReprojectionError2DList"); 
            vtkXMLDataElement* reprojectionError2DList = frame->FindNestedElementWithName("ReprojectionError2DList");

            if ( reprojectionError2DListBaseline == NULL || reprojectionError2DList == NULL ) 
            {
              LOG_ERROR("Reading ReprojectionError2DList tag in Frame #" << frameIndex << "failed");
              ++numberOfFailures;
              continue;
            }

            // <ReprojectionError2D>
            for ( int reprojectionError2DIndex = 0; reprojectionError2DIndex < reprojectionError2DListBaseline->GetNumberOfNestedElements(); ++reprojectionError2DIndex )
            {
              vtkXMLDataElement* reprojectionError2DBaseline = reprojectionError2DListBaseline->GetNestedElement(reprojectionError2DIndex); 
              vtkXMLDataElement* reprojectionError2D = reprojectionError2DList->GetNestedElement(reprojectionError2DIndex); 
              if ( !reprojectionError2DBaseline || !reprojectionError2D || STRCASECMP( reprojectionError2DBaseline->GetName(), "ReprojectionError2D" ) != 0 || STRCASECMP( reprojectionError2D->GetName(), "ReprojectionError2D" ) != 0 )
              {
                LOG_ERROR("Invalid ReprojectionError2D element in Frame #" << frameIndex);
                ++numberOfFailures;
                continue;
              }

              if ( STRCASECMP( reprojectionError2DBaseline->GetAttribute("WireName"), reprojectionError2D->GetAttribute("WireName") ) != 0 )
              {
                LOG_ERROR("Wire name mismatch: " << reprojectionError2DBaseline->GetAttribute("Name") << " <> " << reprojectionError2D->GetAttribute("Name"));
                ++numberOfFailures;
              }

              double blErrorPx[2];
              double cErrorPx[2];
              if ( ! reprojectionError2DBaseline->GetVectorAttribute("ErrorPx", 2, blErrorPx)
                || ! reprojectionError2D->GetVectorAttribute("ErrorPx", 2, cErrorPx) )
              {
                LOG_ERROR("Reading ErrorPx of reprojectionError2D #" << reprojectionError2DIndex << " in Frame #" << frameIndex << "failed!");
                ++numberOfFailures;
                continue;
              }

              for ( int i = 0; i < 2; i++) 
              {
                double ratio = 1.0 * blErrorPx[i] / cErrorPx[i]; 
                if ( ratio > 1 + ERROR_THRESHOLD || ratio < 1 - ERROR_THRESHOLD )
                {
                  LOG_ERROR("ReprojectionError2D ErrorPx component " << i << " mismatch (Frame #" << frameIndex << "): current=" << cErrorPx[i] << ", baseline=" << blErrorPx[i]);
                  ++numberOfFailures;
                }
              }
            }
          } // </ReprojectionError2DList>
        } // If SegmentationStatus is OK
      } // </Frame>
    } // </CalibrationData>
  } // </ErrorReport>

  return numberOfFailures;
}
