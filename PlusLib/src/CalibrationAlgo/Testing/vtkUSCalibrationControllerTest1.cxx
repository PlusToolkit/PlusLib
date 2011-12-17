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
#include "vtkTransform.h"
#include "vtkTransformRepository.h"
#include "vtkMath.h"
#include "vtkPlusConfig.h"

///////////////////////////////////////////////////////////////////
const double ERROR_THRESHOLD = 0.05; // error threshold is 5% 

int CompareCalibrationResultsWithBaseline(const char* baselineFileName, const char* currentResultFileName, int translationErrorThreshold, int rotationErrorThreshold); 

int main (int argc, char* argv[])
{ 
  int numberOfFailures(0); 
	std::string inputRandomStepperMotion1SeqMetafile;
	std::string inputRandomStepperMotion2SeqMetafile;
	std::string inputProbeRotationSeqMetafile;

	std::string inputConfigFileName;
	std::string inputBaselineFileName;
  std::string inputProbeToReferenceTransformName("ProbeToReference"); 
	double inputTranslationErrorThreshold(0); 
	double inputRotationErrorThreshold(0); 

	int verboseLevel=vtkPlusLogger::LOG_LEVEL_DEFAULT;

	vtksys::CommandLineArguments cmdargs;
	cmdargs.Initialize(argc, argv);

	cmdargs.AddArgument("--input-random-stepper-motion-1-sequence-metafile", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputRandomStepperMotion1SeqMetafile, "Sequence metafile name of saved random stepper motion 1 dataset.");
	cmdargs.AddArgument("--input-random-stepper-motion-2-sequence-metafile", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputRandomStepperMotion2SeqMetafile, "Sequence metafile name of saved random stepper motion 2 dataset.");
	cmdargs.AddArgument("--input-probe-rotation-sequence-metafile", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputProbeRotationSeqMetafile, "Sequence metafile name of saved probe rotation dataset.");
	
  cmdargs.AddArgument("--input-probe-to-reference-transform-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputProbeToReferenceTransformName, "Name of the probe to reference transform (Default: ProbeToTracker)");
	cmdargs.AddArgument("--input-config-file-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputConfigFileName, "Configuration file name");
	
	cmdargs.AddArgument("--input-baseline-file-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputBaselineFileName, "Name of file storing baseline calibration results");
	cmdargs.AddArgument("--translation-error-threshold", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputTranslationErrorThreshold, "Translation error threshold in mm.");	
	cmdargs.AddArgument("--rotation-error-threshold", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputRotationErrorThreshold, "Rotation error threshold in degrees.");	
	cmdargs.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug, 5=trace)");	

	if ( !cmdargs.Parse() )
	{
		std::cerr << "Problem parsing arguments" << std::endl;
		std::cout << "Help: " << cmdargs.GetHelp() << std::endl;
		exit(EXIT_FAILURE);
	}

  // Read configuration
  vtkSmartPointer<vtkXMLDataElement> configRootElement = vtkSmartPointer<vtkXMLDataElement>::Take(
    vtkXMLUtilities::ReadElementFromFile(inputConfigFileName.c_str()));
  if (configRootElement == NULL)
  {	
    LOG_ERROR("Unable to read configuration from file " << inputConfigFileName.c_str()); 
		exit(EXIT_FAILURE);
  }
  vtkPlusConfig::GetInstance()->SetDeviceSetConfigurationData(configRootElement);
 
	vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);

  FidPatternRecognition patternRecognition; 
	patternRecognition.ReadConfiguration(configRootElement);

  LOG_INFO("Reading probe rotation data from sequence metafile..."); 
  vtkSmartPointer<vtkTrackedFrameList> probeRotationTrackedFrameList = vtkSmartPointer<vtkTrackedFrameList>::New(); 
  if ( probeRotationTrackedFrameList->ReadFromSequenceMetafile(inputProbeRotationSeqMetafile.c_str()) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to read sequence metafile: " << inputProbeRotationSeqMetafile); 
    return EXIT_FAILURE;
  }

  LOG_INFO("Segmenting probe rotation data...");
  if (patternRecognition.RecognizePattern(probeRotationTrackedFrameList) != PLUS_SUCCESS)
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

	// Initialize the probe calibration controller 
	vtkSmartPointer<vtkProbeCalibrationAlgo> probeCal = vtkSmartPointer<vtkProbeCalibrationAlgo>::New(); 
	probeCal->ReadConfiguration(configRootElement); 
	probeCal->Initialize(); 

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

  vtkSmartPointer<vtkTransform> tPhantomToReference = vtkSmartPointer<vtkTransform>::New(); 
  if ( phantomRegistrationAlgo->GetPhantomToReferenceTransform( tPhantomToReference ) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to register phantom frame to reference frame!"); 
    return EXIT_FAILURE; 
  }

  //******************************************************************
  // TODO: remove these transforms from vtkProbeCalibrationAlgo
  PlusTransformName tnTemplateHolderToPhantom("TemplateHolder", "Phantom"); 
  vtkSmartPointer<vtkMatrix4x4> templateHolderToPhantomMatrix = vtkSmartPointer<vtkMatrix4x4>::New(); 
  transformRepository->GetTransform(tnTemplateHolderToPhantom, templateHolderToPhantomMatrix); 
  probeCal->GetTransformTemplateHolderToTemplate()->SetMatrix(templateHolderToPhantomMatrix); 
  probeCal->GetTransformReferenceToTemplateHolderHome()->SetMatrix( phantomRegistrationAlgo->GetTransformReferenceToTemplateHolder()->GetMatrix() ); 
  //******************************************************************

  // Load and segment validation tracked frame list
  vtkSmartPointer<vtkTrackedFrameList> validationTrackedFrameList = vtkSmartPointer<vtkTrackedFrameList>::New();
  if ( validationTrackedFrameList->ReadFromSequenceMetafile(inputRandomStepperMotion2SeqMetafile.c_str()) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to read tracked frames from sequence metafile from: " << inputRandomStepperMotion2SeqMetafile ); 
    return EXIT_FAILURE; 
  }

  int numberOfSuccessfullySegmentedValidationImages = 0;
  if (patternRecognition.RecognizePattern(validationTrackedFrameList, &numberOfSuccessfullySegmentedValidationImages) != PLUS_SUCCESS)
  {
		LOG_ERROR("Error occured during segmentation of validation images!"); 
		return EXIT_FAILURE;
  }

  // Load and segment calibration tracked frame list
  vtkSmartPointer<vtkTrackedFrameList> calibrationTrackedFrameList = vtkSmartPointer<vtkTrackedFrameList>::New();
  if ( calibrationTrackedFrameList->ReadFromSequenceMetafile(inputRandomStepperMotion1SeqMetafile.c_str()) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to read tracked frames from sequence metafile from: " << inputRandomStepperMotion1SeqMetafile ); 
    return EXIT_FAILURE; 
  }

  int numberOfSuccessfullySegmentedCalibrationImages = 0;
  if (patternRecognition.RecognizePattern(calibrationTrackedFrameList, &numberOfSuccessfullySegmentedCalibrationImages) != PLUS_SUCCESS)
  {
		LOG_ERROR("Error occured during segmentation of calibration images!"); 
		return EXIT_FAILURE;
  }

  LOG_INFO("Segmentation success rate of validation images: " << numberOfSuccessfullySegmentedValidationImages << " out of " << validationTrackedFrameList->GetNumberOfTrackedFrames());

  // Calibrate
  if (probeCal->Calibrate( validationTrackedFrameList, calibrationTrackedFrameList, transformRepository, patternRecognition.GetFidLineFinder()->GetNWires()) != PLUS_SUCCESS)
  {
    LOG_ERROR("Calibration failed!");
		return EXIT_FAILURE;
  }

  // Compare results
	std::string currentConfigFileName = vtkPlusConfig::GetInstance()->GetOutputDirectory() + std::string("/") + std::string(vtkPlusConfig::GetInstance()->GetApplicationStartTimestamp()) + ".Calibration.results.xml";
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
		numberOfFailures++;
		return numberOfFailures;
	}
	if (currentRootElem == NULL )
	{
		LOG_ERROR("Reading newly generated data file failed: " << currentResultFileName);
		numberOfFailures++;
		return numberOfFailures;
	}

	{	//<CalibrationResults>
		vtkXMLDataElement* calibrationResultsBaseline = baselineRootElem->FindNestedElementWithName("CalibrationResults"); 
		vtkXMLDataElement* calibrationResults = currentRootElem->FindNestedElementWithName("CalibrationResults"); 

		if ( calibrationResultsBaseline == NULL) 
		{
			LOG_ERROR("Reading baseline CalibrationResults tag failed: " << baselineFileName);
			numberOfFailures++;
			return numberOfFailures;
		}

		if ( calibrationResults == NULL) 
		{
			LOG_ERROR("Reading current CalibrationResults tag failed: " << currentResultFileName);
			numberOfFailures++;
			return numberOfFailures;
		}

		{	// <CalibrationTransform>
			vtkXMLDataElement* calibrationTransformBaseline = calibrationResultsBaseline->FindNestedElementWithName("CalibrationTransform"); 
			vtkXMLDataElement* calibrationTransform = calibrationResults->FindNestedElementWithName("CalibrationTransform");

			if ( calibrationTransformBaseline == NULL) 
			{
				LOG_ERROR("Reading baseline CalibrationTransform tag failed: " << baselineFileName);
				numberOfFailures++;
				return numberOfFailures;
			}

			if ( calibrationTransform == NULL) 
			{
				LOG_ERROR("Reading current CalibrationTransform tag failed: " << currentResultFileName);
				numberOfFailures++;
				return numberOfFailures;
			}
			
			//********************************* TransformImageToTransducerOriginPixel *************************************
			double blTransformImageToTransducerOriginPixel[16]; 
			double cTransformImageToTransducerOriginPixel[16]; 

			if (!calibrationTransformBaseline->GetVectorAttribute("TransformImageToTransducerOriginPixel", 16, blTransformImageToTransducerOriginPixel))
			{
				LOG_ERROR("Baseline TransformImageToTransducerOriginPixel tag is missing");
				numberOfFailures++;			
			}
			else if (!calibrationTransform->GetVectorAttribute("TransformImageToTransducerOriginPixel", 16, cTransformImageToTransducerOriginPixel))
			{
				LOG_ERROR("Current TransformImageToTransducerOriginPixel tag is missing");
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

						baseTransMatrix->SetElement(i,j, blTransformImageToTransducerOriginPixel[4*i + j]); 
						currentTransMatrix->SetElement(i,j, cTransformImageToTransducerOriginPixel[4*i + j]); 
					}

				}
					double translationError = PlusMath::GetPositionDifference(baseTransMatrix, currentTransMatrix); 
					if ( translationError > translationErrorThreshold )
					{
						LOG_ERROR("TransformImageToTransducerOriginPixel translation error is higher than expected: " << translationError << " mm (threshold: " << translationErrorThreshold << " mm). " );
						numberOfFailures++;
					}

					double rotationError = PlusMath::GetOrientationDifference(baseTransMatrix, currentTransMatrix); 
					if ( rotationError > rotationErrorThreshold )
					{
						LOG_ERROR("TransformImageToTransducerOriginPixel rotation error is higher than expected: " << rotationError << " degree (threshold: " << rotationErrorThreshold << " degree). " );
						numberOfFailures++;
					}
				
			}

			//********************************* TransformImageToProbe *************************************
			double blTransformImageToProbe[16]; 
			double cTransformImageToProbe[16]; 

			if (!calibrationTransformBaseline->GetVectorAttribute("TransformImageToProbe", 16, blTransformImageToProbe))
			{
				LOG_ERROR("Baseline TransformImageToProbe tag is missing");
				numberOfFailures++;			
			}
			else if (!calibrationTransform->GetVectorAttribute("TransformImageToProbe", 16, cTransformImageToProbe))
			{
				LOG_ERROR("Current TransformImageToProbe tag is missing");
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

			//********************************* TransformReferenceToTemplateHolderHome *************************************
			double blTransformReferenceToTemplateHolderHome[16]; 
			double cTransformReferenceToTemplateHolderHome[16]; 

			if (!calibrationTransformBaseline->GetVectorAttribute("TransformReferenceToTemplateHolderHome", 16, blTransformReferenceToTemplateHolderHome))
			{
				LOG_ERROR("Baseline TransformReferenceToTemplateHolderHome tag is missing");
				numberOfFailures++;			
			}
			else if (!calibrationTransform->GetVectorAttribute("TransformReferenceToTemplateHolderHome", 16, cTransformReferenceToTemplateHolderHome))
			{
				LOG_ERROR("Current TransformReferenceToTemplateHolderHome tag is missing");
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

						baseTransMatrix->SetElement(i,j, blTransformReferenceToTemplateHolderHome[4*i + j]); 
						currentTransMatrix->SetElement(i,j, cTransformReferenceToTemplateHolderHome[4*i + j]); 
					}

				}
					double translationError = PlusMath::GetPositionDifference(baseTransMatrix, currentTransMatrix); 
					if ( translationError > translationErrorThreshold )
					{
						LOG_ERROR("TransformReferenceToTemplateHolderHome translation error is higher than expected: " << translationError << " mm (threshold: " << translationErrorThreshold << " mm). " );
						numberOfFailures++;
					}

					double rotationError = PlusMath::GetOrientationDifference(baseTransMatrix, currentTransMatrix); 
					if ( rotationError > rotationErrorThreshold )
					{
						LOG_ERROR("TransformReferenceToTemplateHolderHome rotation error is higher than expected: " << rotationError << " degree (threshold: " << rotationErrorThreshold << " degree). " );
						numberOfFailures++;
					}
				
			}

			//********************************* TransformTemplateHolderToTemplate *************************************
			double blTransformTemplateHolderToTemplate[16]; 
			double cTransformTemplateHolderToTemplate[16]; 

			if (!calibrationTransformBaseline->GetVectorAttribute("TransformTemplateHolderToTemplate", 16, blTransformTemplateHolderToTemplate))
			{
				LOG_ERROR("Baseline TransformTemplateHolderToTemplate tag is missing");
				numberOfFailures++;			
			}
			else if (!calibrationTransform->GetVectorAttribute("TransformTemplateHolderToTemplate", 16, cTransformTemplateHolderToTemplate))
			{
				LOG_ERROR("Current TransformTemplateHolderToTemplate tag is missing");
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

						baseTransMatrix->SetElement(i,j, blTransformTemplateHolderToTemplate[4*i + j]); 
						currentTransMatrix->SetElement(i,j, cTransformTemplateHolderToTemplate[4*i + j]); 
					}

				}
					double translationError = PlusMath::GetPositionDifference(baseTransMatrix, currentTransMatrix); 
					if ( translationError > translationErrorThreshold )
					{
						LOG_ERROR("TransformTemplateHolderToTemplate translation error is higher than expected: " << translationError << " mm (threshold: " << translationErrorThreshold << " mm). " );
						numberOfFailures++;
					}

					double rotationError = PlusMath::GetOrientationDifference(baseTransMatrix, currentTransMatrix); 
					if ( rotationError > rotationErrorThreshold )
					{
						LOG_ERROR("TransformTemplateHolderToTemplate rotation error is higher than expected: " << rotationError << " degree (threshold: " << rotationErrorThreshold << " degree). " );
						numberOfFailures++;
					}
				
			}

			//********************************* TransformTemplateHomeToTemplate *************************************
			double blTransformTemplateHomeToTemplate[16]; 
			double cTransformTemplateHomeToTemplate[16]; 

			if (!calibrationTransformBaseline->GetVectorAttribute("TransformTemplateHomeToTemplate", 16, blTransformTemplateHomeToTemplate))
			{
				LOG_ERROR("Baseline TransformTemplateHomeToTemplate tag is missing");
				numberOfFailures++;			
			}
			else if (!calibrationTransform->GetVectorAttribute("TransformTemplateHomeToTemplate", 16, cTransformTemplateHomeToTemplate))
			{
				LOG_ERROR("Current TransformTemplateHomeToTemplate tag is missing");
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

						baseTransMatrix->SetElement(i,j, blTransformTemplateHomeToTemplate[4*i + j]); 
						currentTransMatrix->SetElement(i,j, cTransformTemplateHomeToTemplate[4*i + j]); 
					}

				}
					double translationError = PlusMath::GetPositionDifference(baseTransMatrix, currentTransMatrix); 
					if ( translationError > translationErrorThreshold )
					{
						LOG_ERROR("TransformTemplateHomeToTemplate translation error is higher than expected: " << translationError << " mm (threshold: " << translationErrorThreshold << " mm). " );
						numberOfFailures++;
					}

					double rotationError = PlusMath::GetOrientationDifference(baseTransMatrix, currentTransMatrix); 
					if ( rotationError > rotationErrorThreshold )
					{
						LOG_ERROR("TransformTemplateHomeToTemplate rotation error is higher than expected: " << rotationError << " degree (threshold: " << rotationErrorThreshold << " degree). " );
						numberOfFailures++;
					}
				
			}

			//********************************* TransformImageToTemplate *************************************
			double blTransformImageToTemplate[16]; 
			double cTransformImageToTemplate[16]; 

			if (!calibrationTransformBaseline->GetVectorAttribute("TransformImageToTemplate", 16, blTransformImageToTemplate))
			{
				LOG_ERROR("Baseline TransformImageToTemplate tag is missing");
				numberOfFailures++;			
			}
			else if (!calibrationTransform->GetVectorAttribute("TransformImageToTemplate", 16, cTransformImageToTemplate))
			{
				LOG_ERROR("Current TransformImageToTemplate tag is missing");
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

						baseTransMatrix->SetElement(i,j, blTransformImageToTemplate[4*i + j]); 
						currentTransMatrix->SetElement(i,j, cTransformImageToTemplate[4*i + j]); 
					}

				}
					double translationError = PlusMath::GetPositionDifference(baseTransMatrix, currentTransMatrix); 
					if ( translationError > translationErrorThreshold )
					{
						LOG_ERROR("TransformImageToTemplate translation error is higher than expected: " << translationError << " mm (threshold: " << translationErrorThreshold << " mm). " );
						numberOfFailures++;
					}

					double rotationError = PlusMath::GetOrientationDifference(baseTransMatrix, currentTransMatrix); 
					if ( rotationError > rotationErrorThreshold )
					{
						LOG_ERROR("TransformImageToTemplate rotation error is higher than expected: " << rotationError << " degree (threshold: " << rotationErrorThreshold << " degree). " );
						numberOfFailures++;
					}
				
			}

		}//</CalibrationTransform>

	}//</CalibrationResults>

	{	// <ErrorReports>
		vtkXMLDataElement* errorReportsBaseline = baselineRootElem->FindNestedElementWithName("ErrorReports"); 
		vtkXMLDataElement* errorReports = currentRootElem->FindNestedElementWithName("ErrorReports");

		if ( errorReportsBaseline == NULL) 
		{
			LOG_ERROR("Reading baseline ErrorReports tag failed: " << baselineFileName);
			numberOfFailures++;
			return numberOfFailures;
		}

		if ( errorReports == NULL) 
		{
			LOG_ERROR("Reading current ErrorReports tag failed: " << currentResultFileName);
			numberOfFailures++;
			return numberOfFailures;
		}

		{	// <PointReconstructionErrorAnalysis>
			vtkXMLDataElement* pointReconstructionErrorAnalysisBaseline = errorReportsBaseline->FindNestedElementWithName("PointReconstructionErrorAnalysis"); 
			vtkXMLDataElement* pointReconstructionErrorAnalysis = errorReports->FindNestedElementWithName("PointReconstructionErrorAnalysis");

			if ( pointReconstructionErrorAnalysisBaseline == NULL) 
			{
				LOG_ERROR("Reading baseline PointReconstructionErrorAnalysis tag failed: " << baselineFileName);
				numberOfFailures++;
				return numberOfFailures;
			}

			if ( pointReconstructionErrorAnalysis == NULL) 
			{
				LOG_ERROR("Reading current PointReconstructionErrorAnalysis tag failed: " << currentResultFileName);
				numberOfFailures++;
				return numberOfFailures;
			}

			double blPRE[9]; 
			double cPRE[9]; 

			if (!pointReconstructionErrorAnalysisBaseline->GetVectorAttribute("PRE", 9, blPRE))
			{
				LOG_ERROR("Baseline PRE is missing");
				numberOfFailures++;			
			}
			else if (!pointReconstructionErrorAnalysis->GetVectorAttribute("PRE", 9, cPRE))
			{
				LOG_ERROR("Current PRE is missing");
				numberOfFailures++;			
			}
			else
			{
				for ( int i = 0; i < 9; i++) 
				{
					double ratio = 1.0 * blPRE[i] / cPRE[i]; 

					if ( ratio > 1 + ERROR_THRESHOLD || ratio < 1 - ERROR_THRESHOLD )
					{
						LOG_ERROR("PRE element (" << i << ") mismatch: current=" << cPRE[i]<< ", baseline=" << blPRE[i]);
						numberOfFailures++;
					}
				}
			}

			double blValidationDataConfidenceLevel, cValidationDataConfidenceLevel; 
			if (!pointReconstructionErrorAnalysisBaseline->GetScalarAttribute("ValidationDataConfidenceLevel", blValidationDataConfidenceLevel))
			{
				LOG_ERROR("Baseline PRE ValidationDataConfidenceLevel is missing");
				numberOfFailures++;			
			}
			else if (!pointReconstructionErrorAnalysis->GetScalarAttribute("ValidationDataConfidenceLevel", cValidationDataConfidenceLevel))
			{
				LOG_ERROR("Current PRE ValidationDataConfidenceLevel is missing");
				numberOfFailures++;			
			}
			else
			{
				double ratio = 1.0 * blValidationDataConfidenceLevel / cValidationDataConfidenceLevel; 

				if ( ratio > 1 + ERROR_THRESHOLD || ratio < 1 - ERROR_THRESHOLD )
				{
					LOG_ERROR("PRE ValidationDataConfidenceLevel mismatch: current=" << cValidationDataConfidenceLevel << ", baseline=" << blValidationDataConfidenceLevel);
					numberOfFailures++;
				}
			}

		}// </PointReconstructionErrorAnalysis>

		{	// <PointLineDistanceErrorAnalysis>
			vtkXMLDataElement* pointLineDistanceErrorAnalysisBaseline = errorReportsBaseline->FindNestedElementWithName("PointLineDistanceErrorAnalysis"); 
			vtkXMLDataElement* pointLineDistanceErrorAnalysis = errorReports->FindNestedElementWithName("PointLineDistanceErrorAnalysis");

			if ( pointLineDistanceErrorAnalysisBaseline == NULL) 
			{
				LOG_ERROR("Reading baseline PointLineDistanceErrorAnalysis tag failed: " << baselineFileName);
				numberOfFailures++;
				return numberOfFailures;
			}

			if ( pointLineDistanceErrorAnalysis == NULL) 
			{
				LOG_ERROR("Reading current PointLineDistanceErrorAnalysis tag failed: " << currentResultFileName);
				numberOfFailures++;
				return numberOfFailures;
			}

			double blPLDE[3]; 
			double cPLDE[3]; 

			if (!pointLineDistanceErrorAnalysisBaseline->GetVectorAttribute("PLDE", 3, blPLDE))
			{
				LOG_ERROR("Baseline PLDE is missing");
				numberOfFailures++;			
			}
			else if (!pointLineDistanceErrorAnalysis->GetVectorAttribute("PLDE", 3, cPLDE))
			{
				LOG_ERROR("Current PLDE is missing");
				numberOfFailures++;			
			}
			else
			{
				for ( int i = 0; i < 3; i++) 
				{
					double ratio = 1.0 * blPLDE[i] / cPLDE[i]; 

					if ( ratio > 1 + ERROR_THRESHOLD || ratio < 1 - ERROR_THRESHOLD )
					{
						LOG_ERROR("PLDE element (" << i << ") mismatch: current=" << cPLDE[i]<< ", baseline=" << blPLDE[i]);
						numberOfFailures++;
					}
				}
			}

			double blValidationDataConfidenceLevel, cValidationDataConfidenceLevel; 
			if (!pointLineDistanceErrorAnalysisBaseline->GetScalarAttribute("ValidationDataConfidenceLevel", blValidationDataConfidenceLevel))
			{
				LOG_ERROR("Baseline PLDE ValidationDataConfidenceLevel is missing");
				numberOfFailures++;			
			}
			else if (!pointLineDistanceErrorAnalysis->GetScalarAttribute("ValidationDataConfidenceLevel", cValidationDataConfidenceLevel))
			{
				LOG_ERROR("Current PLDE ValidationDataConfidenceLevel is missing");
				numberOfFailures++;			
			}
			else
			{
				double ratio = 1.0 * blValidationDataConfidenceLevel / cValidationDataConfidenceLevel; 

				if ( ratio > 1 + ERROR_THRESHOLD || ratio < 1 - ERROR_THRESHOLD )
				{
					LOG_ERROR("PLDE ValidationDataConfidenceLevel mismatch: current=" << cValidationDataConfidenceLevel << ", baseline=" << blValidationDataConfidenceLevel);
					numberOfFailures++;
				}
			}

		}// </PointLineDistanceErrorAnalysis>
	} //</ErrorReports>

	return numberOfFailures; 

}

