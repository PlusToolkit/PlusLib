#include "PlusConfigure.h"
#include "PlusMath.h"
#include "vtkCalibrationController.h"
#include "vtkStepperCalibrationController.h"
#include "vtkBrachyStepperPhantomRegistrationAlgo.h"

#include "vtkBMPReader.h"
#include "vtkSmartPointer.h"
#include "vtkCommand.h"
#include "vtkCallbackCommand.h"
#include "vtkXMLDataElement.h"
#include "vtkXMLUtilities.h"
#include "vtksys/CommandLineArguments.hxx" 
#include "vtksys/SystemTools.hxx"
#include "vtkMatrix4x4.h"
#include "vtkTransform.h"
#include "vtkMath.h"
#include "vtkPlusConfig.h"

#include <stdlib.h>
#include <iostream>

///////////////////////////////////////////////////////////////////
const double ERROR_THRESHOLD = 0.05; // error threshold is 5% 

int CompareCalibrationResultsWithBaseline(const char* baselineFileName, const char* currentResultFileName, int translationErrorThreshold, int rotationErrorThreshold); 

int main (int argc, char* argv[])
{ 
  int numberOfFailures(0); 
	std::string inputRandomStepperMotion1SeqMetafile;
	std::string inputRandomStepperMotion2SeqMetafile;
	std::string inputProbeRotationSeqMetafile;
	std::string inputProbeTranslationSeqMetafile; 
	std::string inputTemplateTranslationSeqMetafile; 

	std::string inputConfigFileName;
	std::string inputBaselineFileName;
	double inputTranslationErrorThreshold(0); 
	double inputRotationErrorThreshold(0); 

	int verboseLevel=vtkPlusLogger::LOG_LEVEL_INFO;

	vtksys::CommandLineArguments cmdargs;
	cmdargs.Initialize(argc, argv);

	cmdargs.AddArgument("--input-random-stepper-motion-1-sequence-metafile", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputRandomStepperMotion1SeqMetafile, "Sequence metafile name of saved random stepper motion 1 dataset.");
	cmdargs.AddArgument("--input-random-stepper-motion-2-sequence-metafile", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputRandomStepperMotion2SeqMetafile, "Sequence metafile name of saved random stepper motion 2 dataset.");
	cmdargs.AddArgument("--input-probe-rotation-sequence-metafile", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputProbeRotationSeqMetafile, "Sequence metafile name of saved probe rotation dataset.");
	cmdargs.AddArgument("--input-probe-translation-sequence-metafile", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputProbeTranslationSeqMetafile, "Sequence metafile name of saved probe translation dataset.");
	cmdargs.AddArgument("--input-template-translation-sequence-metafile", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputTemplateTranslationSeqMetafile, "Sequence metafile name of saved template translation dataset.");
	
	cmdargs.AddArgument("--input-config-file-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputConfigFileName, "Configuration file name");
	
	cmdargs.AddArgument("--input-baseline-file-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputBaselineFileName, "Name of file storing baseline calibration results");
	cmdargs.AddArgument("--translation-error-threshold", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputTranslationErrorThreshold, "Translation error threshold in mm.");	
	cmdargs.AddArgument("--rotation-error-threshold", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputRotationErrorThreshold, "Rotation error threshold in degrees.");	
	cmdargs.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug)");	

	if ( !cmdargs.Parse() )
	{
		std::cerr << "Problem parsing arguments" << std::endl;
		std::cout << "Help: " << cmdargs.GetHelp() << std::endl;
		exit(EXIT_FAILURE);
	}

	std::string programPath("./"), errorMsg; 
	if ( !vtksys::SystemTools::FindProgramPath(argv[0], programPath, errorMsg) )
	{
		LOG_ERROR(errorMsg); 
	}
	programPath = vtksys::SystemTools::GetParentDirectory(programPath.c_str()); 
  // Read configuration
  vtkSmartPointer<vtkXMLDataElement> configRootElement = vtkXMLUtilities::ReadElementFromFile(inputConfigFileName.c_str());
  if (configRootElement == NULL)
  {	
    LOG_ERROR("Unable to read configuration from file " << inputConfigFileName.c_str()); 
		exit(EXIT_FAILURE);
  }
  vtkPlusConfig::GetInstance()->SetDeviceSetConfigurationData(configRootElement);
  vtkPlusConfig::GetInstance()->SetProgramPath(programPath.c_str());

	vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);
  vtkPlusLogger::Instance()->SetDisplayLogLevel(verboseLevel); 

	// Initialize the probe calibration controller 
	vtkSmartPointer<vtkStepperCalibrationController> stepperCal = vtkSmartPointer<vtkStepperCalibrationController>::New(); 
	stepperCal->ReadConfiguration(configRootElement); 

	vtkCalibrationController::ImageDataInfo probeRotationDataInfo = stepperCal->GetImageDataInfo(PROBE_ROTATION); 
	probeRotationDataInfo.InputSequenceMetaFileName.assign(inputProbeRotationSeqMetafile.c_str());
	stepperCal->SetImageDataInfo(PROBE_ROTATION, probeRotationDataInfo); 

	vtkCalibrationController::ImageDataInfo probeTranslationDataInfo = stepperCal->GetImageDataInfo(PROBE_TRANSLATION); 
	probeTranslationDataInfo.InputSequenceMetaFileName.assign(inputProbeTranslationSeqMetafile.c_str());
	stepperCal->SetImageDataInfo(PROBE_TRANSLATION, probeTranslationDataInfo); 

	vtkCalibrationController::ImageDataInfo templateTranslationDataInfo = stepperCal->GetImageDataInfo(TEMPLATE_TRANSLATION); 
	templateTranslationDataInfo.InputSequenceMetaFileName.assign(inputTemplateTranslationSeqMetafile.c_str());
	stepperCal->SetImageDataInfo(TEMPLATE_TRANSLATION, templateTranslationDataInfo); 

  stepperCal->SetCalibrationMode(OFFLINE); 
  stepperCal->EnableVisualizationOff(); 
	stepperCal->Initialize(); 

	if ( stepperCal->OfflineProbeRotationAxisCalibration() != PLUS_SUCCESS )
    {
        numberOfFailures++; 
        LOG_ERROR("OfflineProbeRotationAxisCalibration failed!"); 

    }

	if ( stepperCal->OfflineProbeTranslationAxisCalibration() != PLUS_SUCCESS )
    {
        numberOfFailures++; 
        LOG_ERROR("OfflineProbeTranslationAxisCalibration failed!"); 
    }

	if ( stepperCal->OfflineTemplateTranslationAxisCalibration() != PLUS_SUCCESS )
    {
        numberOfFailures++; 
        LOG_ERROR("OfflineTemplateTranslationAxisCalibration failed!"); 
    }



  FidPatternRecognition patternRecognition; 
	patternRecognition.ReadConfiguration(configRootElement);

  vtkSmartPointer<vtkTrackedFrameList> probeRotationTrackedFrameList = vtkSmartPointer<vtkTrackedFrameList>::New(); 
  if ( probeRotationTrackedFrameList->ReadFromSequenceMetafile(inputProbeRotationSeqMetafile.c_str()) != PLUS_SUCCESS )
  {
      LOG_ERROR("Failed to read probe rotation data from sequence metafile: " << inputProbeRotationSeqMetafile); 
      return EXIT_FAILURE;
  }

  LOG_INFO("Testing image data segmentation...");
  for ( int currentFrameIndex = 0; currentFrameIndex < probeRotationTrackedFrameList->GetNumberOfTrackedFrames(); currentFrameIndex++)
  {
    patternRecognition.RecognizePattern(probeRotationTrackedFrameList->GetTrackedFrame(currentFrameIndex));
  }

	// Initialize the stepper calibration controller 
	vtkSmartPointer<vtkCalibrationController> probeCal = vtkSmartPointer<vtkCalibrationController>::New(); 
	probeCal->ReadConfiguration(configRootElement); 
  probeCal->ReadProbeCalibrationConfiguration(configRootElement);
  probeCal->EnableSegmentationAnalysisOn(); // So that results are drawn (there was a condition for that if the calibration is in OFFLINE mode - now that enum has been removed)

	vtkCalibrationController::ImageDataInfo randomStepperMotion1DataInfo = probeCal->GetImageDataInfo(RANDOM_STEPPER_MOTION_1); 
	randomStepperMotion1DataInfo.InputSequenceMetaFileName.assign(inputRandomStepperMotion1SeqMetafile.c_str());
	probeCal->SetImageDataInfo(RANDOM_STEPPER_MOTION_1, randomStepperMotion1DataInfo); 

	vtkCalibrationController::ImageDataInfo randomStepperMotion2DataInfo = probeCal->GetImageDataInfo(RANDOM_STEPPER_MOTION_2); 
	randomStepperMotion2DataInfo.InputSequenceMetaFileName.assign(inputRandomStepperMotion2SeqMetafile.c_str());
	probeCal->SetImageDataInfo(RANDOM_STEPPER_MOTION_2, randomStepperMotion2DataInfo); 

  probeCal->EnableVisualizationOff(); 
	probeCal->Initialize(); 

  double* centerOfRotationPx = stepperCal->GetCenterOfRotationPx(); 
  double* spacing = stepperCal->GetSpacing(); 
  vtkTransform* tTemplateHolderToPhantom = probeCal->GetTransformTemplateHolderToPhantom(); 

  vtkSmartPointer<vtkBrachyStepperPhantomRegistrationAlgo> phantomRegistrationAlgo = vtkSmartPointer<vtkBrachyStepperPhantomRegistrationAlgo>::New(); 
  phantomRegistrationAlgo->SetInputs(probeRotationTrackedFrameList, spacing, centerOfRotationPx, patternRecognition.GetFidLabeling()->GetNWires()); 
  phantomRegistrationAlgo->SetTransformTemplateHolderToPhantom( tTemplateHolderToPhantom ); 

  vtkSmartPointer<vtkTransform> tPhantomToReference = vtkSmartPointer<vtkTransform>::New(); 
  if ( phantomRegistrationAlgo->GetPhantomToReferenceTransform( tPhantomToReference ) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to register phantom frame to reference frame!"); 
    return EXIT_FAILURE; 
  }

	// Register phantom geometry before calibration 
	probeCal->RegisterPhantomGeometry( tPhantomToReference ); 
  
  // TODO: remove it!
  probeCal->GetTransformTemplateHolderToTemplate()->SetMatrix(probeCal->GetTransformTemplateHolderToPhantom()->GetMatrix() ); 
  probeCal->GetTransformReferenceToTemplateHolderHome()->SetMatrix( phantomRegistrationAlgo->GetTransformReferenceToTemplateHolder()->GetMatrix() ); 


	if ( probeCal->OfflineUSToTemplateCalibration() == PLUS_SUCCESS )
    {
	    probeCal->ComputeCalibrationResults(); 
    }
    else
    {
        numberOfFailures++; 
        LOG_ERROR("OfflineUSToTemplateCalibration failed!"); 
    }

	vtkstd::string currentConfigFileName = probeCal->GetCalibrationResultFileNameWithPath(); 

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

	vtkSmartPointer<vtkXMLDataElement> baselineRootElem = vtkXMLUtilities::ReadElementFromFile(baselineFileName);
	vtkSmartPointer<vtkXMLDataElement> currentRootElem = vtkXMLUtilities::ReadElementFromFile(currentResultFileName); 
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
		vtkSmartPointer<vtkXMLDataElement> calibrationResultsBaseline = baselineRootElem->FindNestedElementWithName("CalibrationResults"); 
		vtkSmartPointer<vtkXMLDataElement> calibrationResults = currentRootElem->FindNestedElementWithName("CalibrationResults"); 

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


		{	// <UltrasoundImageDimensions>

			vtkSmartPointer<vtkXMLDataElement> ultrasoundImageDimensionsBaseline = calibrationResultsBaseline->FindNestedElementWithName("UltrasoundImageDimensions"); 
			vtkSmartPointer<vtkXMLDataElement> ultrasoundImageDimensions = calibrationResults->FindNestedElementWithName("UltrasoundImageDimensions");

			if ( ultrasoundImageDimensionsBaseline == NULL) 
			{
				LOG_ERROR("Reading baseline UltrasoundImageDimensions tag failed: " << baselineFileName);
				numberOfFailures++;
				return numberOfFailures;
			}

			if ( ultrasoundImageDimensions == NULL) 
			{
				LOG_ERROR("Reading current UltrasoundImageDimensions tag failed: " << currentResultFileName);
				numberOfFailures++;
				return numberOfFailures;
			}

			int blImageDimensionWidth, cImageDimensionWidth; 
			if (!ultrasoundImageDimensionsBaseline->GetScalarAttribute("Width", blImageDimensionWidth))
			{
				LOG_ERROR("Baseline UltrasoundImageDimensions width is missing");
				numberOfFailures++;			
			}
			else if (!ultrasoundImageDimensions->GetScalarAttribute("Width", cImageDimensionWidth))
			{
				LOG_ERROR("Current UltrasoundImageDimensions width is missing");
				numberOfFailures++;			
			}
			else
			{
				if (blImageDimensionWidth != cImageDimensionWidth)
				{
					LOG_ERROR("UltrasoundImageDimensions width mismatch: current=" << cImageDimensionWidth << ", baseline=" << blImageDimensionWidth);
					numberOfFailures++;
				}
			}

			int blImageDimensionHeight, cImageDimensionHeight; 
			if (!ultrasoundImageDimensionsBaseline->GetScalarAttribute("Height", blImageDimensionHeight))
			{
				LOG_ERROR("Baseline UltrasoundImageDimensions height is missing");
				numberOfFailures++;			
			}
			else if (!ultrasoundImageDimensions->GetScalarAttribute("Height", cImageDimensionHeight))
			{
				LOG_ERROR("Current UltrasoundImageDimensions height is missing");
				numberOfFailures++;			
			}
			else
			{
				if (blImageDimensionHeight != cImageDimensionHeight)
				{
					LOG_ERROR("UltrasoundImageDimensions height mismatch: current=" << cImageDimensionHeight << ", baseline=" << blImageDimensionHeight);
					numberOfFailures++;
				}
			}
		}// </UltrasoundImageDimensions>

		{	// <CalibrationTransform>
			vtkSmartPointer<vtkXMLDataElement> calibrationTransformBaseline = calibrationResultsBaseline->FindNestedElementWithName("CalibrationTransform"); 
			vtkSmartPointer<vtkXMLDataElement> calibrationTransform = calibrationResults->FindNestedElementWithName("CalibrationTransform");

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
			
			//********************************* TransformImageToUserImage *************************************
			double *blTransformImageToUserImage = new double[16]; 
			double *cTransformImageToUserImage = new double[16]; 

			if (!calibrationTransformBaseline->GetVectorAttribute("TransformImageToUserImage", 16, blTransformImageToUserImage))
			{
				LOG_ERROR("Baseline TransformImageToUserImage tag is missing");
				numberOfFailures++;			
			}
			else if (!calibrationTransform->GetVectorAttribute("TransformImageToUserImage", 16, cTransformImageToUserImage))
			{
				LOG_ERROR("Current TransformImageToUserImage tag is missing");
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

						baseTransMatrix->SetElement(i,j, blTransformImageToUserImage[4*i + j]); 
						currentTransMatrix->SetElement(i,j, cTransformImageToUserImage[4*i + j]); 
					}

				}
					double translationError = PlusMath::GetPositionDifference(baseTransMatrix, currentTransMatrix); 
					if ( translationError > translationErrorThreshold )
					{
						LOG_ERROR("TransformImageToUserImage translation error is higher than expected: " << translationError << " mm (threshold: " << translationErrorThreshold << " mm). " );
						numberOfFailures++;
					}

					double rotationError = PlusMath::GetOrientationDifference(baseTransMatrix, currentTransMatrix); 
					if ( rotationError > rotationErrorThreshold )
					{
						LOG_ERROR("TransformImageToUserImage rotation error is higher than expected: " << rotationError << " degree (threshold: " << rotationErrorThreshold << " degree). " );
						numberOfFailures++;
					}
				
			}
			delete[] blTransformImageToUserImage; 
			delete[] cTransformImageToUserImage; 


			//********************************* TransformUserImageToProbe *************************************
			double *blTransformUserImageToProbe = new double[16]; 
			double *cTransformUserImageToProbe = new double[16]; 

			if (!calibrationTransformBaseline->GetVectorAttribute("TransformUserImageToProbe", 16, blTransformUserImageToProbe))
			{
				LOG_ERROR("Baseline TransformUserImageToProbe tag is missing");
				numberOfFailures++;			
			}
			else if (!calibrationTransform->GetVectorAttribute("TransformUserImageToProbe", 16, cTransformUserImageToProbe))
			{
				LOG_ERROR("Current TransformUserImageToProbe tag is missing");
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

						baseTransMatrix->SetElement(i,j, blTransformUserImageToProbe[4*i + j]); 
						currentTransMatrix->SetElement(i,j, cTransformUserImageToProbe[4*i + j]); 
					}

				}
					double translationError = PlusMath::GetPositionDifference(baseTransMatrix, currentTransMatrix); 
					if ( translationError > translationErrorThreshold )
					{
						LOG_ERROR("TransformUserImageToProbe translation error is higher than expected: " << translationError << " mm (threshold: " << translationErrorThreshold << " mm). " );
						numberOfFailures++;
					}

					double rotationError = PlusMath::GetOrientationDifference(baseTransMatrix, currentTransMatrix); 
					if ( rotationError > rotationErrorThreshold )
					{
						LOG_ERROR("TransformUserImageToProbe rotation error is higher than expected: " << rotationError << " degree (threshold: " << rotationErrorThreshold << " degree). " );
						numberOfFailures++;
					}
				
			}
			delete[] blTransformUserImageToProbe; 
			delete[] cTransformUserImageToProbe; 


			//********************************* TransformReferenceToTemplateHolderHome *************************************
			double *blTransformReferenceToTemplateHolderHome = new double[16]; 
			double *cTransformReferenceToTemplateHolderHome = new double[16]; 

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
			delete[] blTransformReferenceToTemplateHolderHome; 
			delete[] cTransformReferenceToTemplateHolderHome; 

			//********************************* TransformTemplateHolderToTemplate *************************************
			double *blTransformTemplateHolderToTemplate = new double[16]; 
			double *cTransformTemplateHolderToTemplate = new double[16]; 

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
			delete[] blTransformTemplateHolderToTemplate; 
			delete[] cTransformTemplateHolderToTemplate; 

			//********************************* TransformTemplateHomeToTemplate *************************************
			double *blTransformTemplateHomeToTemplate = new double[16]; 
			double *cTransformTemplateHomeToTemplate = new double[16]; 

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
			delete[] blTransformTemplateHomeToTemplate; 
			delete[] cTransformTemplateHomeToTemplate; 

			//********************************* TransformImageToTemplate *************************************
			double *blTransformImageToTemplate = new double[16]; 
			double *cTransformImageToTemplate = new double[16]; 

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
			delete[] blTransformImageToTemplate; 
			delete[] cTransformImageToTemplate; 

		}//</CalibrationTransform>

	}//</CalibrationResults>

	{	// <ErrorReports>
		vtkSmartPointer<vtkXMLDataElement> errorReportsBaseline = baselineRootElem->FindNestedElementWithName("ErrorReports"); 
		vtkSmartPointer<vtkXMLDataElement> errorReports = currentRootElem->FindNestedElementWithName("ErrorReports");

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
			vtkSmartPointer<vtkXMLDataElement> pointReconstructionErrorAnalysisBaseline = errorReportsBaseline->FindNestedElementWithName("PointReconstructionErrorAnalysis"); 
			vtkSmartPointer<vtkXMLDataElement> pointReconstructionErrorAnalysis = errorReports->FindNestedElementWithName("PointReconstructionErrorAnalysis");

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

			double *blPRE = new double[9]; 
			double *cPRE = new double[9]; 

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
			delete[] blPRE; 
			delete[] cPRE; 

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
			vtkSmartPointer<vtkXMLDataElement> pointLineDistanceErrorAnalysisBaseline = errorReportsBaseline->FindNestedElementWithName("PointLineDistanceErrorAnalysis"); 
			vtkSmartPointer<vtkXMLDataElement> pointLineDistanceErrorAnalysis = errorReports->FindNestedElementWithName("PointLineDistanceErrorAnalysis");

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

			double *blPLDE = new double[3]; 
			double *cPLDE= new double[3]; 

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
			delete[] blPLDE; 
			delete[] cPLDE; 

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

