#include "PlusConfigure.h"
#include "vtkFreehandController.h"
#include "PhantomRegistrationController.h"
#include "StylusCalibrationController.h"
#include "vtkConfigurationTools.h"

#include "vtkSmartPointer.h"
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
const double ERROR_THRESHOLD = 0.05; // error threshold is 5% 

int CompareRegistrationResultsWithBaseline(const char* baselineFileName, const char* currentResultFileName); 

int main (int argc, char* argv[])
{ 
	std::string inputConfigFileName;
	std::string inputBaselineFileName;

	int verboseLevel=PlusLogger::LOG_LEVEL_INFO;

	vtksys::CommandLineArguments cmdargs;
	cmdargs.Initialize(argc, argv);

	cmdargs.AddArgument("--input-config-file-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputConfigFileName, "Configuration file name");
	cmdargs.AddArgument("--input-baseline-file-name", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputBaselineFileName, "Name of file storing baseline calibration results");
	cmdargs.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug)");	

	if ( !cmdargs.Parse() )
	{
		std::cerr << "Problem parsing arguments" << std::endl;
		std::cout << "Help: " << cmdargs.GetHelp() << std::endl;
		exit(EXIT_FAILURE);
	}

	PlusLogger::Instance()->SetLogLevel(verboseLevel);

	VTK_LOG_TO_CONSOLE_ON; 

	std::string programPath("./"), errorMsg; 
	if ( !vtksys::SystemTools::FindProgramPath(argv[0], programPath, errorMsg) )
	{
		LOG_ERROR(errorMsg); 
	}
	programPath = vtksys::SystemTools::GetParentDirectory(programPath.c_str()); 

	LOG_INFO("Initialize"); 

  vtkConfigurationTools::GetInstance()->SetConfigurationFileName(inputConfigFileName.c_str());

	// Initialize the controllers
	vtkSmartPointer<vtkFreehandController> controller = vtkFreehandController::GetInstance();
	controller->TrackingOnlyOn();
	if (controller->Initialize() != PLUS_SUCCESS) {
		LOG_ERROR("Initialize failed!");
		return EXIT_FAILURE;
	}
	if (controller->StartDataCollection() != PLUS_SUCCESS) {
		LOG_ERROR("Initializing acquisition failed!");
		return EXIT_FAILURE;
	}

	StylusCalibrationController* stylusCalibrationController = StylusCalibrationController::GetInstance();
	stylusCalibrationController->Initialize();
	stylusCalibrationController->LoadStylusCalibrationFromFile(inputConfigFileName);

	PhantomRegistrationController* phantomRegistrationController = PhantomRegistrationController::GetInstance();
	phantomRegistrationController->Initialize();
	phantomRegistrationController->LoadPhantomDefinitionFromFile(inputConfigFileName);
	phantomRegistrationController->Start();
	
	// Acquire landmarks
	int numberOfLandmarks = phantomRegistrationController->GetNumberOfLandmarks();
	for (int i=0; i<numberOfLandmarks; ++i) {
		phantomRegistrationController->RequestRecording();
		phantomRegistrationController->DoAcquisition();

		PlusLogger::PrintProgressbar((100.0 * i) / numberOfLandmarks); 
	}

	PlusLogger::PrintProgressbar(100.0);

	// Do landmark registration
	phantomRegistrationController->Register();

	// Save result
	vtkstd::string registrationResultFileName = "PhantomRegistrationTest.xml";
	vtksys::SystemTools::RemoveFile(registrationResultFileName.c_str());
	phantomRegistrationController->SavePhantomRegistrationToFile(registrationResultFileName);

	if ( CompareRegistrationResultsWithBaseline( inputBaselineFileName.c_str(), registrationResultFileName.c_str() ) !=0 )
	{
		LOG_ERROR("Comparison of calibration data to baseline failed");
		std::cout << "Exit failure!!!" << std::endl; 

		delete phantomRegistrationController;
		delete stylusCalibrationController;

		return EXIT_FAILURE;
	}

	delete phantomRegistrationController;
	delete stylusCalibrationController;

	std::cout << "Exit success!!!" << std::endl; 
	return EXIT_SUCCESS; 
}

//-----------------------------------------------------------------------------

// return the number of differences
int CompareRegistrationResultsWithBaseline(const char* baselineFileName, const char* currentResultFileName)
{
	int numberOfFailures=0;

	double* transformCurrent = new double[16]; 
	double* transformBaseline = new double[16]; 
	for (int i=0; i<16; ++i) {
		transformCurrent[i] = 0.0;
		transformBaseline[i] = 0.0;
	}

	// Load current phantom registration
	vtkSmartPointer<vtkXMLDataElement> rootElementCurrent = vtkXMLUtilities::ReadElementFromFile(currentResultFileName);
	if (rootElementCurrent == NULL) {	
		LOG_ERROR("Unable to read the current configuration file: " << currentResultFileName); 
		return ++numberOfFailures;
	}
	vtkSmartPointer<vtkXMLDataElement> phantomDefinitionCurrent = rootElementCurrent->LookupElementWithName("PhantomDefinition");
	if (phantomDefinitionCurrent == NULL) {
		LOG_ERROR("No phantom definition section is found in test result!");
		return ++numberOfFailures;
	}
	vtkSmartPointer<vtkXMLDataElement> geometryCurrent = phantomDefinitionCurrent->FindNestedElementWithName("Geometry"); 
	if (geometryCurrent == NULL) {
		LOG_ERROR("Phantom geometry information not found in test result!");
		return ++numberOfFailures;
	}
	vtkSmartPointer<vtkXMLDataElement> registrationCurrent = geometryCurrent->FindNestedElementWithName("Registration"); 
	if (registrationCurrent == NULL) {
		LOG_ERROR("Registration element not found in test result!");
		return ++numberOfFailures;
	} else {
		registrationCurrent->GetVectorAttribute("Transform", 16, transformCurrent);
	}

	// Load baseline phantom registration
	vtkSmartPointer<vtkXMLDataElement> rootElementBaseline = vtkXMLUtilities::ReadElementFromFile(baselineFileName);
	if (rootElementBaseline == NULL) {	
		LOG_ERROR("Unable to read the baseline configuration file: " << baselineFileName); 
		return ++numberOfFailures;
	}
	vtkSmartPointer<vtkXMLDataElement> phantomDefinitionBaseline = rootElementBaseline->LookupElementWithName("PhantomDefinition");
	if (phantomDefinitionBaseline == NULL) {
		LOG_ERROR("No phantom definition section is found in baseline!");
		return ++numberOfFailures;
	}
	vtkSmartPointer<vtkXMLDataElement> geometryBaseline = phantomDefinitionBaseline->FindNestedElementWithName("Geometry"); 
	if (geometryBaseline == NULL) {
		LOG_ERROR("Phantom geometry information not found in baseline!");
		return ++numberOfFailures;
	}
	vtkSmartPointer<vtkXMLDataElement> registrationBaseline = geometryBaseline->FindNestedElementWithName("Registration"); 
	if (registrationBaseline == NULL) {
		LOG_ERROR("Registration element not found in baseline!");
		return ++numberOfFailures;
	} else {
		registrationBaseline->GetVectorAttribute("Transform", 16, transformBaseline);
	}

	// Compare the transforms
	for (int i=0; i<16; ++i) {
		double ratio = 1.0 * transformCurrent[i] / transformBaseline[i];
		double diff = fabs(transformCurrent[i] - transformBaseline[i]);

		if ( (ratio > 1 + ERROR_THRESHOLD || ratio < 1 - ERROR_THRESHOLD) && (diff > 10.0 * ERROR_THRESHOLD) ) // error has to be greater than 5% and also greater than 0.5mm
		{
			LOG_ERROR("Transform element (" << i << ") mismatch: current=" << transformCurrent[i]<< ", baseline=" << transformBaseline[i]);
			numberOfFailures++;
		}
	}

	delete[] transformCurrent;
	delete[] transformBaseline;

	return numberOfFailures;
}
