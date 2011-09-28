#include "PlusConfigure.h"
#include "vtkPivotCalibrationAlgo.h"
#include "vtkPhantomRegistrationAlgo.h"
#include "vtkDataCollector.h"
#include "vtkTrackerTool.h"
#include "vtkFakeTracker.h"
#include "vtkPlusConfig.h"

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

	int verboseLevel=vtkPlusLogger::LOG_LEVEL_INFO;

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

	vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);

	std::string programPath("./"), errorMsg; 
	if ( !vtksys::SystemTools::FindProgramPath(argv[0], programPath, errorMsg) )
	{
		LOG_ERROR(errorMsg); 
	}
	programPath = vtksys::SystemTools::GetParentDirectory(programPath.c_str()); 

	LOG_INFO("Initialize"); 

  // Read configuration
  vtkSmartPointer<vtkXMLDataElement> configRootElement = vtkXMLUtilities::ReadElementFromFile(inputConfigFileName.c_str());
  if (configRootElement == NULL)
  {	
    LOG_ERROR("Unable to read configuration from file " << inputConfigFileName.c_str()); 
		exit(EXIT_FAILURE);
  }

  // Initialize data collection
	vtkSmartPointer<vtkDataCollector> dataCollector = vtkSmartPointer<vtkDataCollector>::New(); 
  if (dataCollector->ReadConfiguration(configRootElement) != PLUS_SUCCESS) {
    LOG_ERROR("Unable to parse configuration from file " << inputConfigFileName.c_str()); 
		exit(EXIT_FAILURE);
	}
	if (dataCollector->Initialize() != PLUS_SUCCESS) {
    LOG_ERROR("Unable to initialize data collection!");
		exit(EXIT_FAILURE);
	}
	if (dataCollector->Start() != PLUS_SUCCESS) {
    LOG_ERROR("Unable to start data collection!");
		exit(EXIT_FAILURE);
	}
	if ((dataCollector->GetTracker() == NULL) || (dataCollector->GetTracker()->GetNumberOfTools() < 1)) {
    LOG_ERROR("Unable to initialize tracker!");
		exit(EXIT_FAILURE);
	}

  dataCollector->SetTrackingOnly(true);

  // Read stylus calibration
  vtkSmartPointer<vtkPivotCalibrationAlgo> pivotCalibration = vtkSmartPointer<vtkPivotCalibrationAlgo>::New();
	if (pivotCalibration == NULL) {
		LOG_ERROR("Unable to instantiate pivot calibration algorithm class!");
		exit(EXIT_FAILURE);
	}
  if (pivotCalibration->ReadConfiguration(configRootElement, TRACKER_TOOL_STYLUS) != PLUS_SUCCESS) {
		LOG_ERROR("Unable to read stylus calibration configuration!");
		exit(EXIT_FAILURE);
	}

  // Set stylus calibration to stylus tool
  int stylusNumber = dataCollector->GetTracker()->GetFirstPortNumberByType(TRACKER_TOOL_STYLUS);
  vtkTrackerTool* stylus = dataCollector->GetTracker()->GetTool(stylusNumber);
  if (stylus == NULL) {
    LOG_ERROR("Unable to get stylus tool!");
    exit(EXIT_FAILURE);
  }
  vtkSmartPointer<vtkMatrix4x4> calibrationMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
  calibrationMatrix->DeepCopy(pivotCalibration->GetTooltipToToolTransform()->GetMatrix());
  stylus->SetCalibrationMatrix(calibrationMatrix);

  // Initialize phantom registration
	vtkSmartPointer<vtkPhantomRegistrationAlgo> phantomRegistration = vtkSmartPointer<vtkPhantomRegistrationAlgo>::New();
	if (phantomRegistration == NULL) {
		LOG_ERROR("Unable to instantiate phantom registration algorithm class!");
		exit(EXIT_FAILURE);
	}
  if (phantomRegistration->ReadConfiguration(configRootElement) != PLUS_SUCCESS) {
		LOG_ERROR("Unable to read phantom definition!");
		exit(EXIT_FAILURE);
	}

  if (phantomRegistration->GetDefinedLandmarks()->GetNumberOfPoints() != 8) {
    LOG_ERROR("Number of defined landmarks should be 8 instead of " << phantomRegistration->GetDefinedLandmarks()->GetNumberOfPoints() << "!");
		exit(EXIT_FAILURE);
  }

  // Acquire landmarks
	vtkFakeTracker *fakeTracker = dynamic_cast<vtkFakeTracker*>(dataCollector->GetTracker());
	if (fakeTracker == NULL) {
    LOG_ERROR("Invalid tracker object!");
		exit(EXIT_FAILURE);
  }

  for (int landmarkCounter=0; landmarkCounter<8; ++landmarkCounter) {
		fakeTracker->SetCounter(landmarkCounter);
		vtkAccurateTimer::Delay(1.1 / fakeTracker->GetFrequency());

    vtkSmartPointer<vtkMatrix4x4> stylusTipToReferenceMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
	  TrackerStatus status = TR_MISSING;
	  double timestamp;

	  if (dataCollector->GetTracker()->GetTool(stylusNumber)->GetEnabled()) {
		  dataCollector->GetTransformWithTimestamp(stylusTipToReferenceMatrix, timestamp, status, stylusNumber, true);
	  }

    if (status == TR_OK) {
      // Compute point position from matrix
      double elements[16];
	    double stylusTipPosition[4];
	    for (int i=0; i<4; ++i) for (int j=0; j<4; ++j) elements[4*j+i] = stylusTipToReferenceMatrix->GetElement(i,j);
	    double origin[4] = {0.0, 0.0, 0.0, 1.0};
	    vtkMatrix4x4::PointMultiply(elements, origin, stylusTipPosition);

	    // Add recorded point to algorithm
      phantomRegistration->GetRecordedLandmarks()->InsertPoint(landmarkCounter, stylusTipPosition[0], stylusTipPosition[1], stylusTipPosition[2]);
	    phantomRegistration->GetRecordedLandmarks()->Modified();

  		vtkPlusLogger::PrintProgressbar((100.0 * landmarkCounter) / 8); 
    }
  }

  if (phantomRegistration->Register() != PLUS_SUCCESS) {
    LOG_ERROR("Phantom registration failed!");
		exit(EXIT_FAILURE);
  }

	vtkPlusLogger::PrintProgressbar(100); 

  // Save result
  phantomRegistration->WriteConfiguration(configRootElement);
	vtkstd::string registrationResultFileName = "PhantomRegistrationTest.xml";
	vtksys::SystemTools::RemoveFile(registrationResultFileName.c_str());
  configRootElement->PrintXML(registrationResultFileName.c_str());

	if ( CompareRegistrationResultsWithBaseline( inputBaselineFileName.c_str(), registrationResultFileName.c_str() ) !=0 )
	{
		LOG_ERROR("Comparison of calibration data to baseline failed");
		std::cout << "Exit failure!!!" << std::endl; 
		return EXIT_FAILURE;
	}

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
