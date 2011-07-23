#include "PlusConfigure.h"
#include "vtkFreehandCalibrationController.h"
#include "vtkFreehandController.h"
#include "StylusCalibrationController.h"
#include "PhantomRegistrationController.h"

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

#include <stdlib.h>
#include <iostream>

///////////////////////////////////////////////////////////////////
//const double ERROR_THRESHOLD = 0.05; // error threshold is 5% 

int CompareCalibrationResultsWithBaseline(const char* baselineFileName, const char* currentResultFileName, int inputErrorThreshold); 
void PrintLogsCallback(vtkObject* obj, unsigned long eid, void* clientdata, void* calldata); 
double GetCalibrationError(vtkMatrix4x4* baseTransMatrix, vtkMatrix4x4* currentTransMatrix); 

int main (int argc, char* argv[])
{
	std::string inputFreehandMotion1SeqMetafile;
	std::string inputFreehandMotion2SeqMetafile;

	std::string inputCalibrationConfigFileName;
	std::string inputDataCollectionConfigFileName;
	std::string inputBaselineFileName;

	std::string inputStylusCalibrationXmlFileName;
	std::string inputPhantomRegistrationXmlFileName;

	double inputErrorThreshold(0);

	int verboseLevel=PlusLogger::LOG_LEVEL_WARNING;

	vtksys::CommandLineArguments cmdargs;
	cmdargs.Initialize(argc, argv);

	cmdargs.AddArgument("--input-freehand-motion-1-sequence-metafile", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputFreehandMotion1SeqMetafile, "Sequence metafile name of saved freehand motion 1 dataset.");
	cmdargs.AddArgument("--input-freehand-motion-2-sequence-metafile", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputFreehandMotion2SeqMetafile, "Sequence metafile name of saved freehand motion 2 dataset.");

	cmdargs.AddArgument("--input-calibration-config-file-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputCalibrationConfigFileName, "Calibration configuration file name");
	cmdargs.AddArgument("--input-data-collection-config-file-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputDataCollectionConfigFileName, "Data collection configuration file name (arbitrary valid config file will do - only necessary to initialize FreehandController)");
	cmdargs.AddArgument("--input-baseline-file-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputBaselineFileName, "Name of file storing baseline calibration results");

	cmdargs.AddArgument("--input-stylus-calibration-xml-file-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputStylusCalibrationXmlFileName, "Name of file storing stylus calibration transform");
	cmdargs.AddArgument("--input-phantom-registration-xml-file-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputPhantomRegistrationXmlFileName, "Name of file storing phantom registration transform");

	cmdargs.AddArgument("--error-threshold", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputErrorThreshold, "Error threshold in mm.");

	cmdargs.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug)");	

	if ( !cmdargs.Parse() )
	{
		std::cerr << "Problem parsing arguments" << std::endl;
		std::cout << "Help: " << cmdargs.GetHelp() << std::endl;
		exit(EXIT_FAILURE);
	}

	PlusLogger::Instance()->SetLogLevel(verboseLevel);
  PlusLogger::Instance()->SetDisplayLogLevel(verboseLevel);

	VTK_LOG_TO_CONSOLE_ON; 

	std::string programPath("./"), errorMsg; 
	if ( !vtksys::SystemTools::FindProgramPath(argv[0], programPath, errorMsg) )
	{
		LOG_ERROR(errorMsg); 
	}
	programPath = vtksys::SystemTools::GetParentDirectory(programPath.c_str()); 

	// Read configuration
	vtkSmartPointer<vtkFreehandCalibrationController> freehandCalibration = vtkSmartPointer<vtkFreehandCalibrationController>::New(); 
	freehandCalibration->SetProgramFolderPath(programPath.c_str());
	freehandCalibration->ReadConfiguration(inputCalibrationConfigFileName.c_str()); 

	// Initialize related controllers with the input data
	vtkSmartPointer<vtkFreehandController> controller = vtkFreehandController::GetInstance();
	controller->SetInputConfigFileName(inputDataCollectionConfigFileName.c_str());
	controller->Initialize();
	controller->StartDataCollection(); // Make it so that these are not needed in case of offline calibration (input-data-collection-config won't be needed either)

	StylusCalibrationController* stylusCalibrationController = StylusCalibrationController::GetInstance();
	stylusCalibrationController->Initialize();
	stylusCalibrationController->LoadStylusCalibrationFromFile(inputStylusCalibrationXmlFileName);

	PhantomRegistrationController* phantomRegistrationController = PhantomRegistrationController::GetInstance();
	phantomRegistrationController->Initialize();
	phantomRegistrationController->LoadPhantomDefinitionFromFile(freehandCalibration->GetPhantomDefinitionFileName());
	phantomRegistrationController->LoadPhantomRegistrationFromFile(inputPhantomRegistrationXmlFileName);

	// Continue initializing freehand calibration controller
	vtkCalibrationController::SavedImageDataInfo freehandMotion1DataInfo = freehandCalibration->GetSavedImageDataInfo(FREEHAND_MOTION_1);
	freehandMotion1DataInfo.SequenceMetaFileName.assign(inputFreehandMotion1SeqMetafile.c_str());
	freehandCalibration->SetSavedImageDataInfo(FREEHAND_MOTION_1, freehandMotion1DataInfo);

	vtkCalibrationController::SavedImageDataInfo freehandMotion2DataInfo = freehandCalibration->GetSavedImageDataInfo(FREEHAND_MOTION_2);
	freehandMotion2DataInfo.SequenceMetaFileName.assign(inputFreehandMotion2SeqMetafile.c_str());
	freehandCalibration->SetSavedImageDataInfo(FREEHAND_MOTION_2, freehandMotion2DataInfo);

	freehandCalibration->Initialize();
	freehandCalibration->SetTemporalCalibrationDone(true);
	freehandCalibration->Start();

	// Register phantom geometry before calibration 
	freehandCalibration->DoOfflineCalibration();  
	freehandCalibration->ComputeCalibrationResults(); 

	vtkstd::string currentConfigFileName = freehandCalibration->GetCalibrationResultFileNameWithPath(); 

	if ( CompareCalibrationResultsWithBaseline( inputBaselineFileName.c_str(), currentConfigFileName.c_str(), inputErrorThreshold ) !=0 )
	{
		LOG_ERROR("Comparison of calibration data to baseline failed");
		std::cout << "Exit failure!!!" << std::endl; 
		return EXIT_FAILURE;
	}

	std::cout << "Exit success!!!" << std::endl; 
	return EXIT_SUCCESS;
}

// return the number of differences
int CompareCalibrationResultsWithBaseline(const char* baselineFileName, const char* currentResultFileName, int inputErrorThreshold)
{
	int numberOfFailures=0;
/* TODO
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


		{	// <UltrasoundImageDimensions>

			vtkXMLDataElement* ultrasoundImageDimensionsBaseline = calibrationResultsBaseline->FindNestedElementWithName("UltrasoundImageDimensions"); 
			vtkXMLDataElement* ultrasoundImageDimensions = calibrationResults->FindNestedElementWithName("UltrasoundImageDimensions");

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


		{	// <UltrasoundImageOrigin>
			vtkXMLDataElement* ultrasoundImageOriginBaseline = calibrationResultsBaseline->FindNestedElementWithName("UltrasoundImageOrigin"); 
			vtkXMLDataElement* ultrasoundImageOrigin = calibrationResults->FindNestedElementWithName("UltrasoundImageOrigin");

			if ( ultrasoundImageOriginBaseline == NULL) 
			{
				LOG_ERROR("Reading baseline UltrasoundImageOrigin tag failed: " << baselineFileName);
				numberOfFailures++;
				return numberOfFailures;
			}

			if ( ultrasoundImageOrigin == NULL) 
			{
				LOG_ERROR("Reading current UltrasoundImageOrigin tag failed: " << currentResultFileName);
				numberOfFailures++;
				return numberOfFailures;
			}

			int blUltrasoundImageOriginX, cUltrasoundImageOriginX; 
			if (!ultrasoundImageOriginBaseline->GetScalarAttribute("OriginX", blUltrasoundImageOriginX))
			{
				LOG_ERROR("Baseline UltrasoundImageOrigin X is missing");
				numberOfFailures++;			
			}
			else if (!ultrasoundImageOrigin->GetScalarAttribute("OriginX", cUltrasoundImageOriginX))
			{
				LOG_ERROR("Current UltrasoundImageOrigin X is missing");
				numberOfFailures++;			
			}
			else
			{
				if (blUltrasoundImageOriginX != cUltrasoundImageOriginX)
				{
					LOG_ERROR("UltrasoundImageOrigin X mismatch: current=" << cUltrasoundImageOriginX << ", baseline=" << blUltrasoundImageOriginX);
					numberOfFailures++;
				}
			}

			int blUltrasoundImageOriginY, cUltrasoundImageOriginY; 
			if (!ultrasoundImageOriginBaseline->GetScalarAttribute("OriginY", blUltrasoundImageOriginY))
			{
				LOG_ERROR("Baseline UltrasoundImageOrigin Y is missing");
				numberOfFailures++;			
			}
			else if (!ultrasoundImageOrigin->GetScalarAttribute("OriginY", cUltrasoundImageOriginY))
			{
				LOG_ERROR("Current UltrasoundImageOrigin Y is missing");
				numberOfFailures++;			
			}
			else
			{
				if (blUltrasoundImageOriginY != cUltrasoundImageOriginY)
				{
					LOG_ERROR("UltrasoundImageOrigin Y mismatch: current=" << cUltrasoundImageOriginY << ", baseline=" << blUltrasoundImageOriginY);
					numberOfFailures++;
				}
			}
			
		}// </UltrasoundImageOrigin>

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
			
			//********************************* TransformImageHomeToUserImageHome *************************************
			double *blTransformImageHomeToUserImageHome = new double[16]; 
			double *cTransformImageHomeToUserImageHome = new double[16]; 

			if (!calibrationTransformBaseline->GetVectorAttribute("TransformImageHomeToUserImageHome", 16, blTransformImageHomeToUserImageHome))
			{
				LOG_ERROR("Baseline TransformImageHomeToUserImageHome tag is missing");
				numberOfFailures++;			
			}
			else if (!calibrationTransform->GetVectorAttribute("TransformImageHomeToUserImageHome", 16, cTransformImageHomeToUserImageHome))
			{
				LOG_ERROR("Current TransformImageHomeToUserImageHome tag is missing");
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

						baseTransMatrix->SetElement(i,j, blTransformImageHomeToUserImageHome[4*i + j]); 
						currentTransMatrix->SetElement(i,j, cTransformImageHomeToUserImageHome[4*i + j]); 
					}

				}
					double translationError = PlusMath::GetPositionDifference(baseTransMatrix, currentTransMatrix); 
					if ( translationError > translationErrorThreshold )
					{
						LOG_ERROR("TransformImageHomeToUserImageHome translation error is higher than expected: " << translationError << " mm (threshold: " << translationErrorThreshold << " mm). " );
						numberOfFailures++;
					}

					double rotationError = PlusMath::GetOrientationDifference(baseTransMatrix, currentTransMatrix); 
					if ( rotationError > rotationErrorThreshold )
					{
						LOG_ERROR("TransformImageHomeToUserImageHome rotation error is higher than expected: " << rotationError << " degree (threshold: " << rotationErrorThreshold << " degree). " );
						numberOfFailures++;
					}
				
			}
			delete[] blTransformImageHomeToUserImageHome; 
			delete[] cTransformImageHomeToUserImageHome; 


			//********************************* TransformUserImageHomeToProbeHome *************************************
			double *blTransformUserImageHomeToProbeHome = new double[16]; 
			double *cTransformUserImageHomeToProbeHome = new double[16]; 

			if (!calibrationTransformBaseline->GetVectorAttribute("TransformUserImageHomeToProbeHome", 16, blTransformUserImageHomeToProbeHome))
			{
				LOG_ERROR("Baseline TransformUserImageHomeToProbeHome tag is missing");
				numberOfFailures++;			
			}
			else if (!calibrationTransform->GetVectorAttribute("TransformUserImageHomeToProbeHome", 16, cTransformUserImageHomeToProbeHome))
			{
				LOG_ERROR("Current TransformUserImageHomeToProbeHome tag is missing");
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

						baseTransMatrix->SetElement(i,j, blTransformUserImageHomeToProbeHome[4*i + j]); 
						currentTransMatrix->SetElement(i,j, cTransformUserImageHomeToProbeHome[4*i + j]); 
					}

				}
					double translationError = PlusMath::GetPositionDifference(baseTransMatrix, currentTransMatrix); 
					if ( translationError > translationErrorThreshold )
					{
						LOG_ERROR("TransformUserImageHomeToProbeHome translation error is higher than expected: " << translationError << " mm (threshold: " << translationErrorThreshold << " mm). " );
						numberOfFailures++;
					}

					double rotationError = PlusMath::GetOrientationDifference(baseTransMatrix, currentTransMatrix); 
					if ( rotationError > rotationErrorThreshold )
					{
						LOG_ERROR("TransformUserImageHomeToProbeHome rotation error is higher than expected: " << rotationError << " degree (threshold: " << rotationErrorThreshold << " degree). " );
						numberOfFailures++;
					}
				
			}
			delete[] blTransformUserImageHomeToProbeHome; 
			delete[] cTransformUserImageHomeToProbeHome; 


			//********************************* TransformProbeHomeToTemplateHolderHome *************************************
			double *blTransformProbeHomeToTemplateHolderHome = new double[16]; 
			double *cTransformProbeHomeToTemplateHolderHome = new double[16]; 

			if (!calibrationTransformBaseline->GetVectorAttribute("TransformProbeHomeToTemplateHolderHome", 16, blTransformProbeHomeToTemplateHolderHome))
			{
				LOG_ERROR("Baseline TransformProbeHomeToTemplateHolderHome tag is missing");
				numberOfFailures++;			
			}
			else if (!calibrationTransform->GetVectorAttribute("TransformProbeHomeToTemplateHolderHome", 16, cTransformProbeHomeToTemplateHolderHome))
			{
				LOG_ERROR("Current TransformProbeHomeToTemplateHolderHome tag is missing");
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

						baseTransMatrix->SetElement(i,j, blTransformProbeHomeToTemplateHolderHome[4*i + j]); 
						currentTransMatrix->SetElement(i,j, cTransformProbeHomeToTemplateHolderHome[4*i + j]); 
					}

				}
					double translationError = PlusMath::GetPositionDifference(baseTransMatrix, currentTransMatrix); 
					if ( translationError > translationErrorThreshold )
					{
						LOG_ERROR("TransformProbeHomeToTemplateHolderHome translation error is higher than expected: " << translationError << " mm (threshold: " << translationErrorThreshold << " mm). " );
						numberOfFailures++;
					}

					double rotationError = PlusMath::GetOrientationDifference(baseTransMatrix, currentTransMatrix); 
					if ( rotationError > rotationErrorThreshold )
					{
						LOG_ERROR("TransformProbeHomeToTemplateHolderHome rotation error is higher than expected: " << rotationError << " degree (threshold: " << rotationErrorThreshold << " degree). " );
						numberOfFailures++;
					}
				
			}
			delete[] blTransformProbeHomeToTemplateHolderHome; 
			delete[] cTransformProbeHomeToTemplateHolderHome; 

			//********************************* TransformTemplateHolderHomeToTemplateHome *************************************
			double *blTransformTemplateHolderHomeToTemplateHome = new double[16]; 
			double *cTransformTemplateHolderHomeToTemplateHome = new double[16]; 

			if (!calibrationTransformBaseline->GetVectorAttribute("TransformTemplateHolderHomeToTemplateHome", 16, blTransformTemplateHolderHomeToTemplateHome))
			{
				LOG_ERROR("Baseline TransformTemplateHolderHomeToTemplateHome tag is missing");
				numberOfFailures++;			
			}
			else if (!calibrationTransform->GetVectorAttribute("TransformTemplateHolderHomeToTemplateHome", 16, cTransformTemplateHolderHomeToTemplateHome))
			{
				LOG_ERROR("Current TransformTemplateHolderHomeToTemplateHome tag is missing");
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

						baseTransMatrix->SetElement(i,j, blTransformTemplateHolderHomeToTemplateHome[4*i + j]); 
						currentTransMatrix->SetElement(i,j, cTransformTemplateHolderHomeToTemplateHome[4*i + j]); 
					}

				}
					double translationError = PlusMath::GetPositionDifference(baseTransMatrix, currentTransMatrix); 
					if ( translationError > translationErrorThreshold )
					{
						LOG_ERROR("TransformTemplateHolderHomeToTemplateHome translation error is higher than expected: " << translationError << " mm (threshold: " << translationErrorThreshold << " mm). " );
						numberOfFailures++;
					}

					double rotationError = PlusMath::GetOrientationDifference(baseTransMatrix, currentTransMatrix); 
					if ( rotationError > rotationErrorThreshold )
					{
						LOG_ERROR("TransformTemplateHolderHomeToTemplateHome rotation error is higher than expected: " << rotationError << " degree (threshold: " << rotationErrorThreshold << " degree). " );
						numberOfFailures++;
					}
				
			}
			delete[] blTransformTemplateHolderHomeToTemplateHome; 
			delete[] cTransformTemplateHolderHomeToTemplateHome; 

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
	
*/
numberOfFailures = 100; //TODO
	return numberOfFailures; 

}

double GetCalibrationError(vtkMatrix4x4* baseTransMatrix, vtkMatrix4x4* currentTransMatrix)
{
	vtkSmartPointer<vtkTransform> baseTransform = vtkSmartPointer<vtkTransform>::New(); 
	baseTransform->SetMatrix(baseTransMatrix); 

	vtkSmartPointer<vtkTransform> currentTransform = vtkSmartPointer<vtkTransform>::New(); 
	currentTransform->SetMatrix(currentTransMatrix); 

	double bx = baseTransform->GetPosition()[0]; 
	double by = baseTransform->GetPosition()[1]; 
	double bz = baseTransform->GetPosition()[2]; 

	double cx = currentTransform->GetPosition()[0]; 
	double cy = currentTransform->GetPosition()[1]; 
	double cz = currentTransform->GetPosition()[2]; 

	// Euclidean distance
	double distance = sqrt( pow(bx-cx,2) + pow(by-cy,2) + pow(bz-cz,2) ); 

	return distance; 
}

// Callback function for error and warning redirects
void PrintLogsCallback(vtkObject* obj, unsigned long eid, void* clientdata, void* calldata)
{
	if ( eid == vtkCommand::GetEventIdFromString("WarningEvent") )
	{
		LOG_WARNING((const char*)calldata);
	}
	else if ( eid == vtkCommand::GetEventIdFromString("ErrorEvent") )
	{
		LOG_ERROR((const char*)calldata);
	}
}

 
