/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

/*!
  \file vtkFreehandCalibrationTest.cxx 
  \brief This test runs a freehand calibration on a recorded data set and 
  compares the results to a baseline
*/ 

#include "PlusConfigure.h"
#include "vtkPlusConfig.h"
#include "PlusMath.h"
#include "vtkProbeCalibrationAlgo.h"
#include "vtkTransformRepository.h"
#include "FidPatternRecognition.h"

#include "vtkSmartPointer.h"
#include "vtkCommand.h"
#include "vtkCallbackCommand.h"
#include "vtkXMLDataElement.h"
#include "vtkXMLUtilities.h"
#include "vtksys/CommandLineArguments.hxx" 
#include "vtksys/SystemTools.hxx"
#include "vtkMatrix4x4.h"
#include "vtkTransform.h"
#include "vtkTransformRepository.h"
#include "vtkMath.h"
#include "vtkTrackedFrameList.h"

#include <stdlib.h>
#include <iostream>

///////////////////////////////////////////////////////////////////
const double ERROR_THRESHOLD = 0.05; // error threshold is 5% 

int CompareCalibrationResultsWithBaseline(const char* baselineFileName, const char* currentResultFileName, int translationErrorThreshold, int rotationErrorThreshold); 

int main (int argc, char* argv[])
{
  std::string inputCalibrationSeqMetafile;
  std::string inputValidationSeqMetafile;

  std::string inputConfigFileName;
  std::string inputBaselineFileName;
  std::string resultConfigFileName = "";

  double inputTranslationErrorThreshold(0);
  double inputRotationErrorThreshold(0);

  int verboseLevel=vtkPlusLogger::LOG_LEVEL_DEFAULT;

  vtksys::CommandLineArguments cmdargs;
  cmdargs.Initialize(argc, argv);

  cmdargs.AddArgument("--input-calibration-sequence-metafile", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputCalibrationSeqMetafile, "Sequence metafile name of input calibration dataset.");
  cmdargs.AddArgument("--input-validation-sequence-metafile", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputValidationSeqMetafile, "Sequence metafile name of input validation dataset.");

  cmdargs.AddArgument("--input-config-file-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputConfigFileName, "Configuration file name)");
  cmdargs.AddArgument("--input-baseline-file-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputBaselineFileName, "Name of file storing baseline calibration results");

  cmdargs.AddArgument("--translation-error-threshold", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputTranslationErrorThreshold, "Translation error threshold in mm.");	
  cmdargs.AddArgument("--rotation-error-threshold", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputRotationErrorThreshold, "Rotation error threshold in degrees.");	

  cmdargs.AddArgument("--save-result-configuration", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &resultConfigFileName, "Result configuration file name");	

  cmdargs.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug, 5=trace)");	

  if ( !cmdargs.Parse() )
  {
    std::cerr << "Problem parsing arguments" << std::endl;
    std::cout << "Help: " << cmdargs.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }

  LOG_INFO("Initialize"); 

  // Read configuration
  vtkSmartPointer<vtkXMLDataElement> configRootElement = vtkSmartPointer<vtkXMLDataElement>::Take(vtkXMLUtilities::ReadElementFromFile(inputConfigFileName.c_str()));
  if (configRootElement == NULL)
  {	
    LOG_ERROR("Unable to read configuration from file " << inputConfigFileName.c_str()); 
    return EXIT_FAILURE;
  }
  vtkPlusConfig::GetInstance()->SetDeviceSetConfigurationData(configRootElement);

  vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);

  // Read coordinate definitions
  vtkSmartPointer<vtkTransformRepository> transformRepository = vtkSmartPointer<vtkTransformRepository>::New();
  if ( transformRepository->ReadConfiguration(configRootElement) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to read CoordinateDefinitions!"); 
    return EXIT_FAILURE;
  }

  vtkSmartPointer<vtkProbeCalibrationAlgo> freehandCalibration = vtkSmartPointer<vtkProbeCalibrationAlgo>::New();
  freehandCalibration->ReadConfiguration(configRootElement);

  FidPatternRecognition patternRecognition;
  patternRecognition.ReadConfiguration(configRootElement);

  // Load and segment calibration image
  vtkSmartPointer<vtkTrackedFrameList> calibrationTrackedFrameList = vtkSmartPointer<vtkTrackedFrameList>::New();
  if (calibrationTrackedFrameList->ReadFromSequenceMetafile(inputCalibrationSeqMetafile.c_str()) != PLUS_SUCCESS)
  {
    LOG_ERROR("Reading calibration images from '" << inputCalibrationSeqMetafile << "' failed!"); 
    return EXIT_FAILURE;
  }

  int numberOfSuccessfullySegmentedCalibrationImages = 0;
  if (patternRecognition.RecognizePattern(calibrationTrackedFrameList, &numberOfSuccessfullySegmentedCalibrationImages) != PLUS_SUCCESS)
  {
    LOG_ERROR("Error occured during segmentation of calibration images!"); 
    return EXIT_FAILURE;
  }

  LOG_INFO("Segmentation success rate of calibration images: " << numberOfSuccessfullySegmentedCalibrationImages << " out of " << calibrationTrackedFrameList->GetNumberOfTrackedFrames());

  // Load and segment validation image
  vtkSmartPointer<vtkTrackedFrameList> validationTrackedFrameList = vtkSmartPointer<vtkTrackedFrameList>::New();
  if (validationTrackedFrameList->ReadFromSequenceMetafile(inputValidationSeqMetafile.c_str()) != PLUS_SUCCESS)
  {
    LOG_ERROR("Reading validation images from '" << inputValidationSeqMetafile << "' failed!"); 
    return EXIT_FAILURE;
  }

  int numberOfSuccessfullySegmentedValidationImages = 0;
  if (patternRecognition.RecognizePattern(validationTrackedFrameList, &numberOfSuccessfullySegmentedValidationImages) != PLUS_SUCCESS)
  {
    LOG_ERROR("Error occured during segmentation of validation images!"); 
    return EXIT_FAILURE;
  }

  LOG_INFO("Segmentation success rate of validation images: " << numberOfSuccessfullySegmentedValidationImages << " out of " << validationTrackedFrameList->GetNumberOfTrackedFrames());

  // Calibrate
  if (freehandCalibration->Calibrate( validationTrackedFrameList, calibrationTrackedFrameList, transformRepository, patternRecognition.GetFidLineFinder()->GetNWires()) != PLUS_SUCCESS)
  {
    LOG_ERROR("Calibration failed!");
    return EXIT_FAILURE;
  }

  // Save result to configuration file
  if (resultConfigFileName.size() > 0)
  {
    if ( transformRepository->WriteConfiguration( vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData() ) != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to save freehand calibration result in configuration XML tree!");
      return EXIT_FAILURE;
    }

    vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData()->PrintXML(resultConfigFileName.c_str());
  }

  // Compare results
	std::string currentConfigFileName = vtkPlusConfig::GetInstance()->GetOutputDirectory() + std::string("/") + std::string(vtkPlusConfig::GetInstance()->GetApplicationStartTimestamp()) + ".Calibration.results.xml";
  if ( CompareCalibrationResultsWithBaseline( inputBaselineFileName.c_str(), currentConfigFileName.c_str(), inputTranslationErrorThreshold, inputRotationErrorThreshold ) !=0 )
  {
    LOG_ERROR("Comparison of calibration data to baseline failed");
    std::cout << "Exit failure!!!" << std::endl; 

    return EXIT_FAILURE;
  }

  std::cout << "Exit success!!!" << std::endl; 
  return EXIT_SUCCESS;
}

//-------------------------------------------------------------------------------------------------

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

      //********************************* Transform ImageToProbe *************************************
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

      double blReprojectionError3DMeanMm = 0.0;
      double blReprojectionError3DStdDevMm = 0.0;
	    if ( ! reprojectionError3DStatisticsBaseline->GetScalarAttribute("MeanMm", blReprojectionError3DMeanMm)
        || ! reprojectionError3DStatisticsBaseline->GetScalarAttribute("StdDevMm", blReprojectionError3DStdDevMm) )
      {
        LOG_ERROR("Reading baseline ReprojectionError3DStatistics statistics failed: " << baselineFileName);
        return ++numberOfFailures;
      }

      double cReprojectionError3DMeanMm = 0.0;
      double cReprojectionError3DStdDevMm = 0.0;
	    if ( ! reprojectionError3DStatistics->GetScalarAttribute("MeanMm", cReprojectionError3DMeanMm)
        || ! reprojectionError3DStatistics->GetScalarAttribute("StdDevMm", cReprojectionError3DStdDevMm) )
      {
        LOG_ERROR("Reading current ReprojectionError3DStatistics statistics failed: " << currentResultFileName);
        return ++numberOfFailures;
      }

      double ratioMean = 1.0 * blReprojectionError3DMeanMm / cReprojectionError3DMeanMm; 
      if ( ratioMean > 1 + ERROR_THRESHOLD || ratioMean < 1 - ERROR_THRESHOLD )
      {
        LOG_ERROR("ReprojectionError3DStatistics/MeanMm mismatch: current=" << cReprojectionError3DMeanMm << ", baseline=" << blReprojectionError3DMeanMm);
        return ++numberOfFailures;
      }
      double ratioStdDev = 1.0 * blReprojectionError3DStdDevMm / cReprojectionError3DStdDevMm; 
      if ( ratioStdDev > 1 + ERROR_THRESHOLD || ratioStdDev < 1 - ERROR_THRESHOLD )
      {
        LOG_ERROR("ReprojectionError3DStatistics/StdDevMm mismatch: current=" << cReprojectionError3DStdDevMm << ", baseline=" << blReprojectionError3DStdDevMm);
        return ++numberOfFailures;
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
          continue;
        }

        double blMeanPx[2];
        double blStdDevPx[2];
	      if ( ! wireBaseline->GetVectorAttribute("MeanPx", 2, blMeanPx)
          || ! wireBaseline->GetVectorAttribute("StdDevPx", 2, blStdDevPx) )
        {
          LOG_ERROR("Reading baseline ReprojectionError2DStatistics failed for wire " << wireIndex);
          ++numberOfFailures;
          continue;
        }

        double cMeanPx[2];
        double cStdDevPx[2];
	      if ( ! wire->GetVectorAttribute("MeanPx", 2, cMeanPx)
          || ! wire->GetVectorAttribute("StdDevPx", 2, cStdDevPx) )
        {
          LOG_ERROR("Reading current ReprojectionError2DStatistics failed for wire " << wireIndex);
          ++numberOfFailures;
          continue;
        }

        for ( int i = 0; i < 2; i++) 
        {
          double ratioMean = 1.0 * blMeanPx[i] / cMeanPx[i]; 
          if ( ratioMean > 1 + ERROR_THRESHOLD || ratioMean < 1 - ERROR_THRESHOLD )
          {
            LOG_ERROR("MeanPx mismatch for wire " << wireIndex << ": current=" << cMeanPx[i] << ", baseline=" << blMeanPx[i]);
            return ++numberOfFailures;
          }
          double ratioStdDev = 1.0 * blStdDevPx[i] / cStdDevPx[i]; 
          if ( ratioStdDev > 1 + ERROR_THRESHOLD || ratioStdDev < 1 - ERROR_THRESHOLD )
          {
            LOG_ERROR("StdDevPx mismatch for wire " << wireIndex << ": current=" << cStdDevPx[i] << ", baseline=" << blStdDevPx[i]);
            return ++numberOfFailures;
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
          continue;
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
                continue;
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
                  continue;
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
                continue;
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
                continue;
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
                continue;
              }

              double blErrorPx[3];
              double cErrorPx[3];
              if ( ! reprojectionError2DBaseline->GetVectorAttribute("ErrorPx", 3, blErrorPx)
                || ! reprojectionError2D->GetVectorAttribute("ErrorPx", 3, cErrorPx) )
              {
                LOG_ERROR("Reading ErrorPx of reprojectionError2D #" << reprojectionError2DIndex << " in Frame #" << frameIndex << "failed!");
                ++numberOfFailures;
                continue;
              }

              for ( int i = 0; i < 3; i++) 
              {
                double ratio = 1.0 * blErrorPx[i] / cErrorPx[i]; 
                if ( ratio > 1 + ERROR_THRESHOLD || ratio < 1 - ERROR_THRESHOLD )
                {
                  LOG_ERROR("ErrorPx component " << i << " mismatch: current=" << cErrorPx[i] << ", baseline=" << blErrorPx[i]);
                  ++numberOfFailures;
                  continue;
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
          continue;
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
                continue;
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
                  continue;
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
                  LOG_ERROR("PositionInImageFrame component " << i << " mismatch: current=" << cPositionInImageFrame[i] << ", baseline=" << blPositionInImageFrame[i]);
                  ++numberOfFailures;
                  continue;
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
                  LOG_ERROR("PositionInProbeFrame component " << i << " mismatch: current=" << cPositionInProbeFrame[i] << ", baseline=" << blPositionInProbeFrame[i]);
                  ++numberOfFailures;
                  continue;
                }
              }
            }
          } // </MiddleWires>
        } // If SegmentationStatus is OK
      } // </Frame>
    } // </CalibrationData>
  } // </ErrorReport>

  return numberOfFailures;
}
