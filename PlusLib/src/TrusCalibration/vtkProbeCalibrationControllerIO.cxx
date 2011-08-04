#include "PlusConfigure.h"

#include "vtkProbeCalibrationControllerIO.h" 
#include "vtkProbeCalibrationController.h"
#include "vtkObjectFactory.h"
#include "vtkImageImport.h"
#include "vtkImageFlip.h"
#include "vtkTIFFWriter.h"
#include "vtkXMLUtilities.h"
#include "vtkDirectory.h"
#include "vtksys/SystemTools.hxx"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkMetaImageSequenceIO.h"
#include <itkImageDuplicator.h>


//----------------------------------------------------------------------------
vtkCxxRevisionMacro(vtkProbeCalibrationControllerIO, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkProbeCalibrationControllerIO); 


//----------------------------------------------------------------------------
vtkProbeCalibrationControllerIO::vtkProbeCalibrationControllerIO()
{

}


//----------------------------------------------------------------------------
vtkProbeCalibrationControllerIO::~vtkProbeCalibrationControllerIO()
{

}


//----------------------------------------------------------------------------
void vtkProbeCalibrationControllerIO::SaveSegmentationResultToImage( int imgIndex, const ImageType::Pointer& frame )
{
	// ==============================================
	// Output the segmentation results on the images
	// ==============================================

	typedef itk::ImageDuplicator<ImageType> DuplicatorType; 
	DuplicatorType::Pointer duplicator = DuplicatorType::New(); 
	duplicator->SetInputImage(frame); 
	duplicator->Update(); 

	ImageType::Pointer resultImage = duplicator->GetOutput();

	// These will modify the image buffer (only in the memory)
	// to update the segmenation outcomes (dots, lines, and pairs).
	//segmenter->printResults();
	this->CalibrationController->GetSegmenter()->drawResults( resultImage->GetBufferPointer() );

	// create an importer to read the data back in VTK image pipeline
	vtkSmartPointer<vtkImageImport> importer = vtkSmartPointer<vtkImageImport>::New();
	importer->SetWholeExtent(0,this->CalibrationController->GetSegParameters()->GetFrameSize()[0] - 1,0,this->CalibrationController->GetSegParameters()->GetFrameSize()[1] - 1,0,0);
	importer->SetDataExtentToWholeExtent();
	importer->SetDataScalarTypeToUnsignedChar();
	importer->SetImportVoidPointer(duplicator->GetOutput()->GetBufferPointer() );
	importer->SetNumberOfScalarComponents(1); 
	importer->Update(); 

	vtkSmartPointer<vtkImageFlip> imageFlip = vtkSmartPointer<vtkImageFlip>::New(); 
	imageFlip->SetInput( importer->GetOutput() ); 
	imageFlip->SetFilteredAxis(1); 
	imageFlip->Update(); 

	std::ostringstream fileName; 
	fileName << this->CalibrationController->GetOutputPath() << "/Frame" << std::setfill('0') << std::setw(4) << imgIndex << ".tiff" << std::ends; 

	vtkSmartPointer<vtkTIFFWriter> writer = vtkSmartPointer<vtkTIFFWriter>::New();
	writer->SetInput( imageFlip->GetOutput() );
	writer->SetFileDimensionality(2);
	writer->SetFileName( fileName.str().c_str() );
	writer->Update();
}


//----------------------------------------------------------------------------
void vtkProbeCalibrationControllerIO::PrintSelf(std::ostream &os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os,indent);
}


//----------------------------------------------------------------------------
void vtkProbeCalibrationControllerIO::Initialize(vtkProbeCalibrationController* calibrationController)
{
	this->CalibrationController = calibrationController; 
}

//----------------------------------------------------------------------------
void vtkProbeCalibrationControllerIO::SaveSegmentedWirePositionsToFile()
{	
	std::ostringstream posInfoHeader; 
	posInfoHeader << "DataType\tStepperPosition\tStepperRotation\tWire1xInImage\tWire1yInImage\tWire2xInImage\tWire2yInImage\tWire3xInImage\tWire3yInImage\tWire4xInImage\tWire4yInImage\tWire5xInImage\tWire5yInImage\tWire6xInImage\tWire6yInImage\t" 
		<< "ExpectedWire1xInImage\tExpectedWire1yInImage\tExpectedWire2xInImage\tExpectedWire2yInImage\tExpectedWire3xInImage\tExpectedWire3yInImage\tExpectedWire4xInImage\tExpectedWire4yInImage\tExpectedWire5xInImage\tExpectedWire5yInImage\tExpectedWire6xInImage\tExpectedWire6yInImage\t" 
		<< "Wire1xInProbe\tWire1yInProbe\tWire2xInProbe\tWire2yInProbe\tWire3xInProbe\tWire3yInProbe\tWire4xInProbe\tWire4yInProbe\tWire5xInProbe\tWire5yInProbe\tWire6xInProbe\tWire6yInProbe\t"  
		<< "ExpectedWire1xInProbe\tExpectedWire1yInProbe\tExpectedWire2xInProbe\tExpectedWire2yInProbe\tExpectedWire3xInProbe\tExpectedWire3yInProbe\tExpectedWire4xInProbe\tExpectedWire4yInProbe\tExpectedWire5xInProbe\tExpectedWire5yInProbe\tExpectedWire6xInProbe\tExpectedWire6yInProbe\t"  
		<< "Wire1xInTemplate\tWire1yInTemplate\tWire2xInTemplate\tWire2yInTemplate\tWire3xInTemplate\tWire3yInTemplate\tWire4xInTemplate\tWire4yInTemplate\tWire5xInTemplate\tWire5yInTemplate\tWire6xInTemplate\tWire6yInTemplate\t" 
		<< "ExpectedWire1xInTemplate\tExpectedWire1yInTemplate\tExpectedWire2xInTemplate\tExpectedWire2yInTemplate\tExpectedWire3xInTemplate\tExpectedWire3yInTemplate\tExpectedWire4xInTemplate\tExpectedWire4yInTemplate\tExpectedWire5xInTemplate\tExpectedWire5yInTemplate\tExpectedWire6xInTemplate\tExpectedWire6yInTemplate\t" 
		<< std::endl;  

	std::ofstream calibPosInfo;
	std::ostringstream calibrationSegWirePosFileName; 
	calibrationSegWirePosFileName << this->CalibrationController->GetCalibrator()->getCalibrationTimeStampInString() << "_CalibrationSegWirePos.txt"; 
	std::ostringstream calibrationSegWirePosPath; 
	calibrationSegWirePosPath << this->CalibrationController->GetOutputPath() << "/" << calibrationSegWirePosFileName.str(); 
	this->CalibrationController->SetCalibrationSegWirePosInfoFileName(calibrationSegWirePosPath.str().c_str()); 
	calibPosInfo.open (calibrationSegWirePosPath.str().c_str(), ios::out);
	calibPosInfo << "# Segmented wire positions of the Calibration dataset" << std::endl; 
	calibPosInfo << posInfoHeader.str(); 

	std::ofstream validPosInfo;
	std::ostringstream validationSegWirePosFileName; 
	validationSegWirePosFileName << this->CalibrationController->GetCalibrator()->getCalibrationTimeStampInString() << "_ValidationSegWirePos.txt"; 
	std::ostringstream validationSegWirePosPath; 
	validationSegWirePosPath << this->CalibrationController->GetOutputPath() << "/" << validationSegWirePosFileName.str(); 
	this->CalibrationController->SetValidationSegWirePosInfoFileName(validationSegWirePosPath.str().c_str()); 
	validPosInfo.open (validationSegWirePosPath.str().c_str(), ios::out);
	validPosInfo << "# Segmented wire positions of the Validation dataset" << std::endl; 
	validPosInfo << posInfoHeader.str();

	
	// Save the current ProbeHomeToProbe transformation 
	vtkSmartPointer<vtkTransform> currentTransformProbeHomeToProbe = vtkSmartPointer<vtkTransform>::New(); 
	currentTransformProbeHomeToProbe->DeepCopy(this->CalibrationController->GetTransformProbeHomeToProbe()); 

	// define the Template to Image transformation 
	vtkSmartPointer<vtkTransform> tTemplateToImage = vtkSmartPointer<vtkTransform>::New(); 
	tTemplateToImage->Concatenate(this->CalibrationController->GetTransformImageToTemplate()); 
	tTemplateToImage->Inverse();

	// define the Template to Probe transformation 
	vtkSmartPointer<vtkTransform> tTemplateToProbe = vtkSmartPointer<vtkTransform>::New(); 
	tTemplateToProbe->PostMultiply(); 
	tTemplateToProbe->Concatenate(this->CalibrationController->GetTransformProbeHomeToTemplateHolderHome()); 
	tTemplateToProbe->Concatenate(this->CalibrationController->GetTransformTemplateHolderHomeToTemplateHome());
	tTemplateToProbe->Concatenate(this->CalibrationController->GetTransformTemplateHomeToTemplate());
	tTemplateToProbe->Inverse();


	// Save segmented wire positions
	for( int frameNum = 0; frameNum < this->CalibrationController->GetSegmentedFrameContainer().size(); frameNum++ )
	{
		double probeHomeToProbe[16]; 
		this->CalibrationController->GetSegmentedFrameContainer()[frameNum].TrackedFrameInfo->GetDefaultFrameTransform(probeHomeToProbe); 
		vtkSmartPointer<vtkTransform> tProbeHomeToProbe = vtkSmartPointer<vtkTransform>::New(); 
		tProbeHomeToProbe->SetMatrix(probeHomeToProbe); 

		// Update the transformation to the current image position
		this->CalibrationController->GetTransformProbeHomeToProbe()->SetMatrix(probeHomeToProbe); 
		this->CalibrationController->GetTransformImageToTemplate()->Update(); 

		SegmentationResults segResults = this->CalibrationController->GetSegmentedFrameContainer()[frameNum].SegResults; 

		std::string dataType; 
		switch(this->CalibrationController->GetSegmentedFrameContainer()[frameNum].DataType)
		{	
		case(RANDOM_STEPPER_MOTION_1): 
			dataType = "Calibration"; 
			break; 
		case(RANDOM_STEPPER_MOTION_2): 
			dataType = "Validation"; 
			break; 
		}

		double posZ = tProbeHomeToProbe->GetPosition()[2]; 
		double rotZ = tProbeHomeToProbe->GetOrientation()[2]; 

		std::ostringstream os; 
		os << dataType << "\t" << posZ << "\t" << rotZ << "\t"; 

		//************************* Image frame *****************************
		for (int i=0; i<segResults.GetFoundDotsCoordinateValue().size(); i++)
		{
			os << segResults.GetFoundDotsCoordinateValue()[i][0] << "\t" << segResults.GetFoundDotsCoordinateValue()[i][1] << "\t"; 
		}

		// Save ground truth wire positions 
		for ( int wireNum = 1; wireNum < 7; wireNum++ )
		{
			double wireposontemplate[3] = {0,0,0}; 
			if ( ! this->CalibrationController->GetWirePosInTemplateCoordinate(wireNum, wireposontemplate) )
			{
				// plane and line are parallel
				os << "NaN\tNaN\t"; 
				continue; 
			}

			vtkSmartPointer<vtkTransform> point = vtkSmartPointer<vtkTransform>::New(); 
			point->PreMultiply();
			point->Concatenate(tTemplateToImage); 
			point->Translate(wireposontemplate[0], wireposontemplate[1], wireposontemplate[2]); 
			point->Update(); 

			double* wireposonimage = point->GetPosition();
			os << wireposonimage[0] << "\t" << wireposonimage[1] << "\t";
		}

		//************************* Probe frame *****************************
		for (int i=0; i<segResults.GetFoundDotsCoordinateValue().size(); i++)
		{
			vtkSmartPointer<vtkTransform> point = vtkSmartPointer<vtkTransform>::New(); 
			point->PreMultiply();
			point->Concatenate(tProbeHomeToProbe);
			point->Concatenate(this->CalibrationController->GetTransformUserImageHomeToProbeHome()); 
			point->Concatenate(this->CalibrationController->GetTransformImageHomeToUserImageHome()); 
			point->Translate(segResults.GetFoundDotsCoordinateValue()[i][0], segResults.GetFoundDotsCoordinateValue()[i][1], 0); 
			point->Update(); 

			double* wireposInProbeHome = point->GetPosition();
			os << wireposInProbeHome[0] << "\t" << wireposInProbeHome[1] << "\t"; 
		}

		// Save ground truth wire positions 
		for ( int wireNum = 1; wireNum < 7; wireNum++ )
		{
			double wireposontemplate[3] = {0,0,0}; 
			if ( ! this->CalibrationController->GetWirePosInTemplateCoordinate(wireNum, wireposontemplate) )
			{
				// plane and line are parallel
				os << "NaN\tNaN\t";
				continue; 
			}

			vtkSmartPointer<vtkTransform> point = vtkSmartPointer<vtkTransform>::New(); 
			point->PreMultiply();
			point->Concatenate(tTemplateToProbe); 
			point->Translate(wireposontemplate[0], wireposontemplate[1], wireposontemplate[2]); 
			point->Update(); 

			double* wireposonprobe = point->GetPosition();
			os << wireposonprobe[0] << "\t" << wireposonprobe[1] << "\t";
		}

		//************************* Template frame *****************************
		for (int i=0; i<segResults.GetFoundDotsCoordinateValue().size(); i++)
		{
			vtkSmartPointer<vtkTransform> point = vtkSmartPointer<vtkTransform>::New(); 
			point->PostMultiply();
			point->Translate(segResults.GetFoundDotsCoordinateValue()[i][0], segResults.GetFoundDotsCoordinateValue()[i][1], 0); 
			point->Concatenate(this->CalibrationController->GetTransformImageToTemplate()); 
			point->Update(); 

			double* wireposInTemplateHome = point->GetPosition();
			os << wireposInTemplateHome[0] << "\t" << wireposInTemplateHome[1] << "\t"; 
		}

		// Save ground truth wire positions 
		for ( int wireNum = 1; wireNum < 7; wireNum++ )
		{
			double wireposontemplate[3] = {0,0,0}; 
			if ( ! this->CalibrationController->GetWirePosInTemplateCoordinate(wireNum, wireposontemplate) )
			{
				// plane and line are parallel
				os << "NaN\tNaN\t";
				continue; 
			}

			os << wireposontemplate[0] << "\t" << wireposontemplate[1] << "\t";
		}

		os << std::endl; 


		switch( this->CalibrationController->GetSegmentedFrameContainer()[frameNum].DataType )
		{
		case(RANDOM_STEPPER_MOTION_1):
			{
				calibPosInfo << os.str(); 
			}
			break; 
		case(RANDOM_STEPPER_MOTION_2):
			{
				validPosInfo << os.str(); 
			}
			break; 
		}
	}

	calibPosInfo.close(); 
	validPosInfo.close(); 

	// set back the current TransformProbeHomeToProbe
	this->CalibrationController->GetTransformProbeHomeToProbe()->SetMatrix(currentTransformProbeHomeToProbe->GetMatrix()); 
}

//----------------------------------------------------------------------------
void vtkProbeCalibrationControllerIO::SaveCalibrationResultsAndErrorReportsToXML()
{

	// Construct the calibration result file name with path and timestamp
	const std::string calibrationResultFileName = this->CalibrationController->GetCalibrator()->getCalibrationTimeStampInString() + this->CalibrationController->GetCalibrationResultFileSuffix() + ".xml";
	const std::string calibrationResultFileNameWithPath = this->CalibrationController->GetOutputPath() + std::string("/") + calibrationResultFileName;
	this->CalibrationController->SetCalibrationResultFileNameWithPath(calibrationResultFileNameWithPath.c_str()); 

	// <USTemplateCalibrationResult>
	vtkSmartPointer<vtkXMLDataElement> xmlCalibrationResults = vtkSmartPointer<vtkXMLDataElement>::New(); 
	xmlCalibrationResults->SetName("USTemplateCalibrationResult"); 
	xmlCalibrationResults->SetAttribute("version", "1.0"); 

	//***********************************************************************************************
	// <CalibrationFile> 
	vtkSmartPointer<vtkXMLDataElement> tagCalibrationFile = vtkSmartPointer<vtkXMLDataElement>::New(); 
	tagCalibrationFile->SetName("CalibrationFile"); 
	tagCalibrationFile->SetAttribute("Timestamp", this->CalibrationController->GetCalibrator()->getCalibrationTimeStampWithFormat().c_str()); 
	tagCalibrationFile->SetAttribute("FileName", calibrationResultFileName.c_str()); 
	vtkstd::string commentCalibrationFile("# Timestamp format: MM/DD/YY HH:MM:SS"); 
	tagCalibrationFile->AddCharacterData(commentCalibrationFile.c_str(), commentCalibrationFile.size()); 
	// </CalibrationFile> 

	//***********************************************************************************************
	// <CalibrationResults>
	vtkSmartPointer<vtkXMLDataElement> tagCalibrationResults = vtkSmartPointer<vtkXMLDataElement>::New(); 
	tagCalibrationResults->SetName("CalibrationResults"); 

	//***********************************************************************************************
	// <UltrasoundImageDimensions>
	vtkSmartPointer<vtkXMLDataElement> tagUltrasoundImageDimensions = vtkSmartPointer<vtkXMLDataElement>::New(); 
	tagUltrasoundImageDimensions->SetName("UltrasoundImageDimensions"); 
	tagUltrasoundImageDimensions->SetIntAttribute("Width", this->CalibrationController->GetSegParameters()->GetFrameSize()[0]); 
	tagUltrasoundImageDimensions->SetIntAttribute("Height",this->CalibrationController->GetSegParameters()->GetFrameSize()[1]); 
	vtkstd::string commentUltrasoundImageDimensions("# UltrasoundImageDimensions format: image width and height in pixels."); 
	tagUltrasoundImageDimensions->AddCharacterData(commentUltrasoundImageDimensions.c_str(), commentUltrasoundImageDimensions.size()); 
	// </UltrasoundImageDimensions>

	//***********************************************************************************************
	// <UltrasoundImageOrigin>
	// # FORMAT: X to the right; Y to the bottom; w.r.t the left-upper corner in original image.
	vtkSmartPointer<vtkXMLDataElement> tagUltrasoundImageOrigin = vtkSmartPointer<vtkXMLDataElement>::New(); 
	tagUltrasoundImageOrigin->SetName("UltrasoundImageOrigin"); 
	tagUltrasoundImageOrigin->SetIntAttribute("OriginX", this->CalibrationController->GetUSImageFrameOriginXInPixels()); 
	tagUltrasoundImageOrigin->SetIntAttribute("OriginY", this->CalibrationController->GetUSImageFrameOriginYInPixels()); 
	vtkstd::string commentUltrasoundImageOrigin("# UltrasoundImageOrigin format: X to the right; Y to the bottom; w.r.t the left-upper corner in original image."); 
	tagUltrasoundImageOrigin->AddCharacterData(commentUltrasoundImageOrigin.c_str(), commentUltrasoundImageOrigin.size()); 
	// </UltrasoundImageOrigin>

	//***********************************************************************************************

	double *imageHomeToUserImageHomeMatrix = new double[16]; 
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++) 
		{
			imageHomeToUserImageHomeMatrix[i*4+j] = this->CalibrationController->GetTransformImageHomeToUserImageHome()->GetMatrix()->GetElement(i,j); 
		}
	}

	double *userImageHomeToProbeHomeMatrix = new double[16]; 
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++) 
		{
			userImageHomeToProbeHomeMatrix[i*4+j] = this->CalibrationController->GetTransformUserImageHomeToProbeHome()->GetMatrix()->GetElement(i,j); 
		}
	}

	double *probeHomeToTemplateHolderHomeMatrix = new double[16]; 
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++) 
		{
			probeHomeToTemplateHolderHomeMatrix[i*4+j] = this->CalibrationController->GetTransformProbeHomeToTemplateHolderHome()->GetMatrix()->GetElement(i,j); 
		}
	}

	double *templateHolderHomeToTemplateHomeMatrix = new double[16]; 
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++) 
		{
			templateHolderHomeToTemplateHomeMatrix[i*4+j] = this->CalibrationController->GetTransformTemplateHolderHomeToTemplateHome()->GetMatrix()->GetElement(i,j); 
		}
	}

	double *templateHomeToTemplateMatrix = new double[16]; 
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++) 
		{
			templateHomeToTemplateMatrix[i*4+j] = this->CalibrationController->GetTransformTemplateHomeToTemplate()->GetMatrix()->GetElement(i,j); 
		}
	}

	double *imageToTemplateMatrix = new double[16]; 
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++) 
		{
			imageToTemplateMatrix[i*4+j] = this->CalibrationController->GetTransformImageToTemplate()->GetMatrix()->GetElement(i,j); 
		}
	}

	// <CalibrationTransform>
	vtkSmartPointer<vtkXMLDataElement> tagCalibrationTransform = vtkSmartPointer<vtkXMLDataElement>::New(); 
	tagCalibrationTransform->SetName("CalibrationTransform"); 
	tagCalibrationTransform->SetVectorAttribute("TransformImageHomeToUserImageHome", 16, imageHomeToUserImageHomeMatrix); 
	tagCalibrationTransform->SetVectorAttribute("TransformUserImageHomeToProbeHome", 16, userImageHomeToProbeHomeMatrix); 
	tagCalibrationTransform->SetVectorAttribute("TransformProbeHomeToTemplateHolderHome", 16, probeHomeToTemplateHolderHomeMatrix); 
	tagCalibrationTransform->SetVectorAttribute("TransformTemplateHolderHomeToTemplateHome", 16, templateHolderHomeToTemplateHomeMatrix); 
	tagCalibrationTransform->SetVectorAttribute("TransformTemplateHomeToTemplate", 16, templateHomeToTemplateMatrix); 
	tagCalibrationTransform->SetVectorAttribute("TransformImageToTemplate", 16, imageToTemplateMatrix); 
	// </CalibrationTransform>

	delete[] imageHomeToUserImageHomeMatrix; 
	delete[] userImageHomeToProbeHomeMatrix; 
	delete[] probeHomeToTemplateHolderHomeMatrix; 
	delete[] templateHolderHomeToTemplateHomeMatrix; 
	delete[] templateHomeToTemplateMatrix; 
	delete[] imageToTemplateMatrix; 

	tagCalibrationResults->AddNestedElement(tagUltrasoundImageDimensions); 
	tagCalibrationResults->AddNestedElement(tagUltrasoundImageOrigin); 
	tagCalibrationResults->AddNestedElement(tagCalibrationTransform); 
	// </CalibrationResults>

	//***********************************************************************************************
	// <ErrorReports>
	vtkSmartPointer<vtkXMLDataElement> tagErrorReports = vtkSmartPointer<vtkXMLDataElement>::New(); 
	tagErrorReports->SetName("ErrorReports"); 

	//***********************************************************************************************
	double *preAnalysis = new double[9]; 
	for (int i = 0; i < 9; i++)
	{
		preAnalysis[i] = this->CalibrationController->GetPRE3DVector()[i]; 
	}

	// <PointReconstructionErrorAnalysis>
	vtkSmartPointer<vtkXMLDataElement> tagPointReconstructionErrorAnalysis = vtkSmartPointer<vtkXMLDataElement>::New(); 
	tagPointReconstructionErrorAnalysis->SetName("PointReconstructionErrorAnalysis"); 
	tagPointReconstructionErrorAnalysis->SetVectorAttribute("PRE", 9, preAnalysis);  
	// # The percentage of top-ranked validation data used for evaluation
	tagPointReconstructionErrorAnalysis->SetDoubleAttribute("ValidationDataConfidenceLevel", this->CalibrationController->GetPRE3DVector()[9]);  
	vtkstd::string commentPointReconstructionErrorAnalysis("# PRE format: PRE3D_X_mean, PRE3D_X_rms, PRE3D_X_std PRE3D_Y_mean, PRE3D_Y_rms, PRE3D_Y_std PRE3D_Z_mean, PRE3D_Z_rms, PRE3D_Z_std"); 
	tagPointReconstructionErrorAnalysis->AddCharacterData(commentPointReconstructionErrorAnalysis.c_str(), commentPointReconstructionErrorAnalysis.size()); 
	// </PointReconstructionErrorAnalysis>
	delete[] preAnalysis; 

	//***********************************************************************************************
	double *rawPointReconstructionErrors = new double[this->CalibrationController->GetPRE3DMatrix().size()]; 
	for ( int row = 0; row < this->CalibrationController->GetPRE3DMatrix().rows(); row++)
	{
		for (int column = 0; column < this->CalibrationController->GetPRE3DMatrix().cols(); column++)
		{
			rawPointReconstructionErrors[row*this->CalibrationController->GetPRE3DMatrix().cols() + column ] = this->CalibrationController->GetPRE3DMatrix().get(row, column); 
		}
	}

	// <PointReconstructionErrors>
	vtkSmartPointer<vtkXMLDataElement> tagPointReconstructionErrors = vtkSmartPointer<vtkXMLDataElement>::New(); 
	tagPointReconstructionErrors->SetName("PointReconstructionErrors"); 
	tagPointReconstructionErrors->SetVectorAttribute("Raw", this->CalibrationController->GetPRE3DMatrix().size(), rawPointReconstructionErrors); 
	vtkstd::ostringstream commentPointReconstructionErrors; 
	commentPointReconstructionErrors << "# PointReconstructionErrors format: 4xN matrix, N = "; 
	commentPointReconstructionErrors << this->CalibrationController->GetPRE3DMatrix().columns(); 
	commentPointReconstructionErrors << ": the number of validation points"; 
	tagPointReconstructionErrors->AddCharacterData(commentPointReconstructionErrors.str().c_str(), commentPointReconstructionErrors.str().size()); 
	// </PointReconstructionErrors>
	delete[] rawPointReconstructionErrors; 

	//***********************************************************************************************
	double *pldeAnalysis = new double[3]; 
	for (int i = 0; i < 3; i++)
	{
		pldeAnalysis[i] = this->CalibrationController->GetPointLineDistanceErrorAnalysisVector()[i]; 
	}

	// <PointLineDistanceErrorAnalysis>
	vtkSmartPointer<vtkXMLDataElement> tagPointLineDistanceErrorAnalysis = vtkSmartPointer<vtkXMLDataElement>::New(); 
	tagPointLineDistanceErrorAnalysis->SetName("PointLineDistanceErrorAnalysis"); 
	tagPointLineDistanceErrorAnalysis->SetVectorAttribute("PLDE", 3, pldeAnalysis);  
	// # The percentage of top-ranked validation data used for evaluation
	tagPointLineDistanceErrorAnalysis->SetDoubleAttribute("ValidationDataConfidenceLevel", this->CalibrationController->GetPointLineDistanceErrorAnalysisVector()[3]);  
	vtkstd::string commentPointLineDistanceErrorAnalysis("# PLDE format: PLDE_mean, PLDE_rms, PLDE_std"); 
	tagPointLineDistanceErrorAnalysis->AddCharacterData(commentPointLineDistanceErrorAnalysis.c_str(), commentPointLineDistanceErrorAnalysis.size()); 
	// </PointLineDistanceErrorAnalysis>
	delete[] pldeAnalysis; 

	//***********************************************************************************************
	double *rawPointLineDistanceErrors = new double[this->CalibrationController->GetPointLineDistanceErrorVector().size()]; 
	for (int i = 0; i < this->CalibrationController->GetPointLineDistanceErrorVector().size(); i++)
	{
		rawPointLineDistanceErrors[i] = this->CalibrationController->GetPointLineDistanceErrorVector().get(i); 
	}

	double *sortedPointLineDistanceErrors = new double[this->CalibrationController->GetPointLineDistanceErrorSortedVector().size()]; 
	for (int i = 0; i < this->CalibrationController->GetPointLineDistanceErrorSortedVector().size(); i++)
	{
		sortedPointLineDistanceErrors[i] = this->CalibrationController->GetPointLineDistanceErrorSortedVector().get(i); 
	}

	// <PointLineDistanceErrors>
	vtkSmartPointer<vtkXMLDataElement> tagPointLineDistanceErrors = vtkSmartPointer<vtkXMLDataElement>::New(); 
	tagPointLineDistanceErrors->SetName("PointLineDistanceErrors"); 
	tagPointLineDistanceErrors->SetVectorAttribute("Raw", this->CalibrationController->GetPointLineDistanceErrorVector().size(), rawPointLineDistanceErrors); 
	//tagPointLineDistanceErrors->SetVectorAttribute("Sorted", this->CalibrationController->GetPointLineDistanceErrorSortedVector().size(), sortedPointLineDistanceErrors); 
	vtkstd::ostringstream commentPointLineDistanceErrors; 
	commentPointLineDistanceErrors << "# PointLineDistanceErrors format: 1xN vector, N = "; 
	commentPointLineDistanceErrors << this->CalibrationController->GetPointLineDistanceErrorVector().size(); 
	commentPointLineDistanceErrors << ": the number of validation points"; 
	tagPointLineDistanceErrors->AddCharacterData(commentPointLineDistanceErrors.str().c_str(), commentPointLineDistanceErrors.str().size()); 
	// </PointLineDistanceErrors>
	delete[] rawPointLineDistanceErrors; 
	delete[] sortedPointLineDistanceErrors; 

	//***********************************************************************************************
	double *w1LREAnalysis = new double[6]; 
	for (int i = 0; i < 6; i++)
	{
		w1LREAnalysis[i] = this->CalibrationController->GetLineReconstructionErrorAnalysisVector(1)[i]; 
	}

	// <Wire1LineReconstructionErrorAnalysis>
	vtkSmartPointer<vtkXMLDataElement> tagWire1LineReconstructionErrorAnalysis = vtkSmartPointer<vtkXMLDataElement>::New(); 
	tagWire1LineReconstructionErrorAnalysis->SetName("Wire1LineReconstructionErrorAnalysis"); 
	tagWire1LineReconstructionErrorAnalysis->SetVectorAttribute("LRE", 6, w1LREAnalysis);  
	// # The percentage of top-ranked validation data used for evaluation
	tagWire1LineReconstructionErrorAnalysis->SetDoubleAttribute("ValidationDataConfidenceLevel", this->CalibrationController->GetLineReconstructionErrorAnalysisVector(1)[6]);  
	vtkstd::string commentWire1LineReconstructionErrorAnalysis("# LRE format: LRE_X_mean, LRE_X_std, LRE_Y_mean, LRE_Y_std, LRE_EUC_mean, LRE_EUC_std"); 
	tagWire1LineReconstructionErrorAnalysis->AddCharacterData(commentWire1LineReconstructionErrorAnalysis.c_str(), commentWire1LineReconstructionErrorAnalysis.size()); 
	// </LineReconstructionErrorAnalysis>
	delete[] w1LREAnalysis; 

	//***********************************************************************************************
	const int w1NumOfRows = this->CalibrationController->GetLineReconstructionErrorMatrix(1).rows(); 
	const int w1NumOfCols = this->CalibrationController->GetLineReconstructionErrorMatrix(1).cols(); 
	double *w1rawXLineReconstructionErrors = new double[w1NumOfCols]; 
	double *w1rawYLineReconstructionErrors = new double[w1NumOfCols]; 
	double *w1rawEUCLineReconstructionErrors = new double[w1NumOfCols]; 

	for (int column = 0; column < w1NumOfCols; column++)
	{
		w1rawXLineReconstructionErrors[column] = this->CalibrationController->GetLineReconstructionErrorMatrix(1).get(0, column); 
		w1rawYLineReconstructionErrors[column] = this->CalibrationController->GetLineReconstructionErrorMatrix(1).get(1, column); 
		w1rawEUCLineReconstructionErrors[column] = this->CalibrationController->GetLineReconstructionErrorMatrix(1).get(3, column); 
		
	}

	// <Wire1LineReconstructionErrors>
	vtkSmartPointer<vtkXMLDataElement> tagW1LineReconstructionErrors = vtkSmartPointer<vtkXMLDataElement>::New(); 
	tagW1LineReconstructionErrors->SetName("Wire1LineReconstructionErrors"); 
	tagW1LineReconstructionErrors->SetVectorAttribute("X", w1NumOfCols, w1rawXLineReconstructionErrors); 
	tagW1LineReconstructionErrors->SetVectorAttribute("Y", w1NumOfCols, w1rawYLineReconstructionErrors); 
	tagW1LineReconstructionErrors->SetVectorAttribute("EUC", w1NumOfCols, w1rawEUCLineReconstructionErrors); 
	vtkstd::ostringstream commentW1LineReconstructionErrors; 
	commentW1LineReconstructionErrors << "# LineReconstructionErrors format: 4xN matrix, N = "; 
	commentW1LineReconstructionErrors << w1NumOfCols; 
	commentW1LineReconstructionErrors << ": the number of validation points"; 
	tagW1LineReconstructionErrors->AddCharacterData(commentW1LineReconstructionErrors.str().c_str(), commentW1LineReconstructionErrors.str().size()); 
	// </Wire1LineReconstructionErrors>
	delete[] w1rawXLineReconstructionErrors; 
	delete[] w1rawYLineReconstructionErrors; 
	delete[] w1rawEUCLineReconstructionErrors;

	//***********************************************************************************************
	double *w3LREAnalysis = new double[6]; 
	for (int i = 0; i < 6; i++)
	{
		w3LREAnalysis[i] = this->CalibrationController->GetLineReconstructionErrorAnalysisVector(3)[i]; 
	}

	// <Wire3LineReconstructionErrorAnalysis>
	vtkSmartPointer<vtkXMLDataElement> tagWire3LineReconstructionErrorAnalysis = vtkSmartPointer<vtkXMLDataElement>::New(); 
	tagWire3LineReconstructionErrorAnalysis->SetName("Wire3LineReconstructionErrorAnalysis"); 
	tagWire3LineReconstructionErrorAnalysis->SetVectorAttribute("LRE", 6, w3LREAnalysis);  
	// # The percentage of top-ranked validation data used for evaluation
	tagWire3LineReconstructionErrorAnalysis->SetDoubleAttribute("ValidationDataConfidenceLevel", this->CalibrationController->GetLineReconstructionErrorAnalysisVector(3)[6]);  
	vtkstd::string commentWire3LineReconstructionErrorAnalysis("# LRE format: LRE_X_mean, LRE_X_std, LRE_Y_mean, LRE_Y_std, LRE_EUC_mean, LRE_EUC_std"); 
	tagWire3LineReconstructionErrorAnalysis->AddCharacterData(commentWire3LineReconstructionErrorAnalysis.c_str(), commentWire3LineReconstructionErrorAnalysis.size()); 
	// </LineReconstructionErrorAnalysis>
	delete[] w3LREAnalysis;

	//***********************************************************************************************
	const int w3NumOfRows = this->CalibrationController->GetLineReconstructionErrorMatrix(3).rows(); 
	const int w3NumOfCols = this->CalibrationController->GetLineReconstructionErrorMatrix(3).cols(); 
	double *w3rawXLineReconstructionErrors = new double[w3NumOfCols]; 
	double *w3rawYLineReconstructionErrors = new double[w3NumOfCols]; 
	double *w3rawEUCLineReconstructionErrors = new double[w3NumOfCols];

	for (int column = 0; column < w3NumOfCols; column++)
	{
		w3rawXLineReconstructionErrors[column] = this->CalibrationController->GetLineReconstructionErrorMatrix(3).get(0, column); 
		w3rawYLineReconstructionErrors[column] = this->CalibrationController->GetLineReconstructionErrorMatrix(3).get(1, column); 
		w3rawEUCLineReconstructionErrors[column] = this->CalibrationController->GetLineReconstructionErrorMatrix(3).get(3, column); 
	}

	// <Wire3LineReconstructionErrors>
	vtkSmartPointer<vtkXMLDataElement> tagW3LineReconstructionErrors = vtkSmartPointer<vtkXMLDataElement>::New(); 
	tagW3LineReconstructionErrors->SetName("Wire3LineReconstructionErrors"); 
	tagW3LineReconstructionErrors->SetVectorAttribute("X", w3NumOfCols, w3rawXLineReconstructionErrors); 
	tagW3LineReconstructionErrors->SetVectorAttribute("Y", w3NumOfCols, w3rawYLineReconstructionErrors); 
	tagW3LineReconstructionErrors->SetVectorAttribute("EUC", w3NumOfCols, w3rawEUCLineReconstructionErrors); 
	vtkstd::ostringstream commentW3LineReconstructionErrors; 
	commentW3LineReconstructionErrors << "# LineReconstructionErrors format: 4xN matrix, N = "; 
	commentW3LineReconstructionErrors << w3NumOfCols; 
	commentW3LineReconstructionErrors << ": the number of validation points"; 
	tagW3LineReconstructionErrors->AddCharacterData(commentW3LineReconstructionErrors.str().c_str(), commentW3LineReconstructionErrors.str().size()); 
	// </Wire3LineReconstructionErrors>
	delete[] w3rawXLineReconstructionErrors; 
	delete[] w3rawYLineReconstructionErrors; 
	delete[] w3rawEUCLineReconstructionErrors;

	//***********************************************************************************************
	double *w4LREAnalysis = new double[6]; 
	for (int i = 0; i < 6; i++)
	{
		w4LREAnalysis[i] = this->CalibrationController->GetLineReconstructionErrorAnalysisVector(4)[i]; 
	}

	// <Wire4LineReconstructionErrorAnalysis>
	vtkSmartPointer<vtkXMLDataElement> tagWire4LineReconstructionErrorAnalysis = vtkSmartPointer<vtkXMLDataElement>::New(); 
	tagWire4LineReconstructionErrorAnalysis->SetName("Wire4LineReconstructionErrorAnalysis"); 
	tagWire4LineReconstructionErrorAnalysis->SetVectorAttribute("LRE", 6, w4LREAnalysis);  
	// # The percentage of top-ranked validation data used for evaluation
	tagWire4LineReconstructionErrorAnalysis->SetDoubleAttribute("ValidationDataConfidenceLevel", this->CalibrationController->GetLineReconstructionErrorAnalysisVector(4)[6]);  
	vtkstd::string commentWire4LineReconstructionErrorAnalysis("# LRE format: LRE_X_mean, LRE_X_std, LRE_Y_mean, LRE_Y_std, LRE_EUC_mean, LRE_EUC_std"); 
	tagWire4LineReconstructionErrorAnalysis->AddCharacterData(commentWire4LineReconstructionErrorAnalysis.c_str(), commentWire4LineReconstructionErrorAnalysis.size()); 
	// </LineReconstructionErrorAnalysis>
	delete[] w4LREAnalysis;

	//***********************************************************************************************
	const int w4NumOfRows = this->CalibrationController->GetLineReconstructionErrorMatrix(4).rows(); 
	const int w4NumOfCols = this->CalibrationController->GetLineReconstructionErrorMatrix(4).cols(); 
	double *w4rawXLineReconstructionErrors = new double[w4NumOfCols]; 
	double *w4rawYLineReconstructionErrors = new double[w4NumOfCols]; 
	double *w4rawEUCLineReconstructionErrors = new double[w4NumOfCols];

	for (int column = 0; column < w4NumOfCols; column++)
	{
		w4rawXLineReconstructionErrors[column] = this->CalibrationController->GetLineReconstructionErrorMatrix(4).get(0, column); 
		w4rawYLineReconstructionErrors[column] = this->CalibrationController->GetLineReconstructionErrorMatrix(4).get(1, column); 
		w4rawEUCLineReconstructionErrors[column] = this->CalibrationController->GetLineReconstructionErrorMatrix(4).get(3, column); 
	}

	// <Wire4LineReconstructionErrors>
	vtkSmartPointer<vtkXMLDataElement> tagW4LineReconstructionErrors = vtkSmartPointer<vtkXMLDataElement>::New(); 
	tagW4LineReconstructionErrors->SetName("Wire4LineReconstructionErrors"); 
	tagW4LineReconstructionErrors->SetVectorAttribute("X", w4NumOfCols, w4rawXLineReconstructionErrors); 
	tagW4LineReconstructionErrors->SetVectorAttribute("Y", w4NumOfCols, w4rawYLineReconstructionErrors); 
	tagW4LineReconstructionErrors->SetVectorAttribute("EUC", w4NumOfCols, w4rawEUCLineReconstructionErrors); 
	vtkstd::ostringstream commentW4LineReconstructionErrors; 
	commentW4LineReconstructionErrors << "# LineReconstructionErrors format: 4xN matrix, N = "; 
	commentW4LineReconstructionErrors << w4NumOfCols; 
	commentW4LineReconstructionErrors << ": the number of validation points"; 
	tagW4LineReconstructionErrors->AddCharacterData(commentW4LineReconstructionErrors.str().c_str(), commentW4LineReconstructionErrors.str().size()); 
	// </Wire4LineReconstructionErrors>
	delete[] w4rawXLineReconstructionErrors; 
	delete[] w4rawYLineReconstructionErrors; 
	delete[] w4rawEUCLineReconstructionErrors;

	//***********************************************************************************************
	double *w6LREAnalysis = new double[6]; 
	for (int i = 0; i < 6; i++)
	{
		w6LREAnalysis[i] = this->CalibrationController->GetLineReconstructionErrorAnalysisVector(6)[i]; 
	}

	// <Wire6LineReconstructionErrorAnalysis>
	vtkSmartPointer<vtkXMLDataElement> tagWire6LineReconstructionErrorAnalysis = vtkSmartPointer<vtkXMLDataElement>::New(); 
	tagWire6LineReconstructionErrorAnalysis->SetName("Wire6LineReconstructionErrorAnalysis"); 
	tagWire6LineReconstructionErrorAnalysis->SetVectorAttribute("LRE", 6, w6LREAnalysis);  
	// # The percentage of top-ranked validation data used for evaluation
	tagWire6LineReconstructionErrorAnalysis->SetDoubleAttribute("ValidationDataConfidenceLevel", this->CalibrationController->GetLineReconstructionErrorAnalysisVector(6)[6]);  
	vtkstd::string commentWire6LineReconstructionErrorAnalysis("# LRE format: LRE_X_mean, LRE_X_std, LRE_Y_mean, LRE_Y_std, LRE_EUC_mean, LRE_EUC_std"); 
	tagWire6LineReconstructionErrorAnalysis->AddCharacterData(commentWire6LineReconstructionErrorAnalysis.c_str(), commentWire6LineReconstructionErrorAnalysis.size()); 
	// </LineReconstructionErrorAnalysis>
	delete[] w6LREAnalysis;

	//***********************************************************************************************
	const int w6NumOfRows = this->CalibrationController->GetLineReconstructionErrorMatrix(6).rows(); 
	const int w6NumOfCols = this->CalibrationController->GetLineReconstructionErrorMatrix(6).cols(); 
	double *w6rawXLineReconstructionErrors = new double[w6NumOfCols]; 
	double *w6rawYLineReconstructionErrors = new double[w6NumOfCols]; 
	double *w6rawEUCLineReconstructionErrors = new double[w6NumOfCols];

	for (int column = 0; column < w6NumOfCols; column++)
	{
		w6rawXLineReconstructionErrors[column] = this->CalibrationController->GetLineReconstructionErrorMatrix(6).get(0, column); 
		w6rawYLineReconstructionErrors[column] = this->CalibrationController->GetLineReconstructionErrorMatrix(6).get(1, column); 
		w6rawEUCLineReconstructionErrors[column] = this->CalibrationController->GetLineReconstructionErrorMatrix(6).get(3, column); 
	}

	// <Wire6LineReconstructionErrors>
	vtkSmartPointer<vtkXMLDataElement> tagW6LineReconstructionErrors = vtkSmartPointer<vtkXMLDataElement>::New(); 
	tagW6LineReconstructionErrors->SetName("Wire6LineReconstructionErrors"); 
	tagW6LineReconstructionErrors->SetVectorAttribute("X", w6NumOfCols, w6rawXLineReconstructionErrors); 
	tagW6LineReconstructionErrors->SetVectorAttribute("Y", w6NumOfCols, w6rawYLineReconstructionErrors); 
	tagW6LineReconstructionErrors->SetVectorAttribute("EUC", w6NumOfCols, w6rawEUCLineReconstructionErrors); 
	vtkstd::ostringstream commentW6LineReconstructionErrors; 
	commentW6LineReconstructionErrors << "# LineReconstructionErrors format: 6xN matrix, N = "; 
	commentW6LineReconstructionErrors << w6NumOfCols; 
	commentW6LineReconstructionErrors << ": the number of validation points"; 
	tagW6LineReconstructionErrors->AddCharacterData(commentW6LineReconstructionErrors.str().c_str(), commentW6LineReconstructionErrors.str().size()); 
	// </Wire6LineReconstructionErrors>
	delete[] w6rawXLineReconstructionErrors; 
	delete[] w6rawYLineReconstructionErrors;  
	delete[] w6rawEUCLineReconstructionErrors;

	tagErrorReports->AddNestedElement(tagPointReconstructionErrorAnalysis); 
	tagErrorReports->AddNestedElement(tagPointReconstructionErrors);
	tagErrorReports->AddNestedElement(tagPointLineDistanceErrorAnalysis);
	tagErrorReports->AddNestedElement(tagPointLineDistanceErrors);
	tagErrorReports->AddNestedElement(tagWire1LineReconstructionErrorAnalysis);
	tagErrorReports->AddNestedElement(tagW1LineReconstructionErrors);
	tagErrorReports->AddNestedElement(tagWire3LineReconstructionErrorAnalysis);
	tagErrorReports->AddNestedElement(tagW3LineReconstructionErrors);
	tagErrorReports->AddNestedElement(tagWire4LineReconstructionErrorAnalysis);
	tagErrorReports->AddNestedElement(tagW4LineReconstructionErrors);
	tagErrorReports->AddNestedElement(tagWire6LineReconstructionErrorAnalysis);
	tagErrorReports->AddNestedElement(tagW6LineReconstructionErrors);
	// <ErrorReports>

	xmlCalibrationResults->AddNestedElement(tagCalibrationFile); 
	xmlCalibrationResults->AddNestedElement(tagCalibrationResults); 
	xmlCalibrationResults->AddNestedElement(tagErrorReports); 
	// </USTemplateCalibrationResult>

	xmlCalibrationResults->PrintXML(this->CalibrationController->GetCalibrationResultFileNameWithPath()); 

}

//----------------------------------------------------------------------------
// TODO: Read it from XML
void vtkProbeCalibrationControllerIO::ReadUs3DBeamwidthDataFromFile()
{
	try
	{
		// #1. Read the US 3D Beamwidth Data from a pre-populated file
		// ============================================================

		std::ifstream USBeamProfileFile( this->CalibrationController->GetUS3DBeamProfileDataFileNameAndPath(), std::ios::in );
		if( !USBeamProfileFile.is_open() )
		{
			vtkErrorMacro("ReadUs3DBeamwidthDataFromFile: Failed to open the US 3D beam profile data file: " << this->CalibrationController->GetUS3DBeamProfileDataFileNameAndPath() << "!"); 
			throw;
		}

		std::string SectionName("");
		std::string ThisConfiguration("");

		// Start from the beginning of the file
		USBeamProfileFile.seekg( 0, ios::beg );

		// Axial Position of the Transducer's Crystal in Original Image Frame
		// -------------------------------------------------------------------
		ThisConfiguration = "AXIAL_POSITION_OF_TRANSDUCER_CRYSTAL_SURFACE_IN_PIXELS]";
		while ( USBeamProfileFile.eof() != true && SectionName != ThisConfiguration )
		{
			USBeamProfileFile.ignore(1024, '[');
			USBeamProfileFile >> SectionName;
		}
		if(  SectionName != ThisConfiguration )
		{	// If the designated configuration is not found, throw up error
			std::cerr << "\n\n" << __FILE__ << "," << __LINE__ << "\n"
				<< "::CANNOT find the config section named: [" << ThisConfiguration 
				<< " in the configuration file!!!  Throw up ...\n";
			USBeamProfileFile.close();
			throw;
		}
		USBeamProfileFile.ignore(1024, '#');
		USBeamProfileFile.ignore(1024, '#');
		USBeamProfileFile.ignore(1024, '#');
		USBeamProfileFile.ignore(1024, '#');
		USBeamProfileFile.ignore(1024, '\n');
		int axialPositionOfCrystalSurfaceInOrigImageFrame(-1); 
		USBeamProfileFile >> axialPositionOfCrystalSurfaceInOrigImageFrame;

		// Minimum US Elevation Beamwidth and its Focal Zone in US Image Frame
		// --------------------------------------------------------------------
		ThisConfiguration = "MINIMUM_ELEVATION_BEAMWIDTH_AND_FOCAL_ZONE]";
		while ( USBeamProfileFile.eof() != true && SectionName != ThisConfiguration )
		{
			USBeamProfileFile.ignore(1024, '[');
			USBeamProfileFile >> SectionName;
		}
		if(  SectionName != ThisConfiguration )
		{	// If the designated configuration is not found, throw up error
			std::cerr << "\n\n" << __FILE__ << "," << __LINE__ << "\n"
				<< "::CANNOT find the config section named: [" << ThisConfiguration 
				<< " in the configuration file!!!  Throw up ...\n";
			USBeamProfileFile.close();
			throw;
		}
		USBeamProfileFile.ignore(1024, '#');
		USBeamProfileFile.ignore(1024, '#');
		USBeamProfileFile.ignore(1024, '#');
		USBeamProfileFile.ignore(1024, '#');
		USBeamProfileFile.ignore(1024, '#');
		USBeamProfileFile.ignore(1024, '#');
		USBeamProfileFile.ignore(1024, '#');
		USBeamProfileFile.ignore(1024, '\n');
		//USBeamProfileFile >> this->CalibrationController->GetMinElevationBeamwidthAndFocalZoneInUSImageFrame();

		double ElevationFocalZoneAtAxialDistance2TheCrystals(-1);
		double MinElevationBeamwidth(-1);
		USBeamProfileFile >> ElevationFocalZoneAtAxialDistance2TheCrystals >> MinElevationBeamwidth;

		vnl_matrix<double> transformOrigImageFrame2TRUSImageFrameMatrix4x4(4,4);
		this->CalibrationController->ConvertVtkMatrixToVnlMatrixInMeter(this->CalibrationController->GetTransformImageHomeToUserImageHome()->GetMatrix(), transformOrigImageFrame2TRUSImageFrameMatrix4x4); 

		// Convert the crystal surface position to TRUS image frame
		double axialPositionOfCrystalSurfaceInTRUSImageFrame =
			transformOrigImageFrame2TRUSImageFrameMatrix4x4.get(1,1) *
			axialPositionOfCrystalSurfaceInOrigImageFrame +
			transformOrigImageFrame2TRUSImageFrameMatrix4x4.get(1,3);

		this->CalibrationController->SetAxialPositionOfCrystalSurfaceInTRUSImageFrame(axialPositionOfCrystalSurfaceInTRUSImageFrame);

		// Then convert the axial distance of the focal-zone into the TRUS Image Frame
		const double ElevationFocalZoneInUSImageFrame = 
			axialPositionOfCrystalSurfaceInTRUSImageFrame +
			ElevationFocalZoneAtAxialDistance2TheCrystals;

		this->CalibrationController->GetMinElevationBeamwidthAndFocalZoneInUSImageFrame()->put(0, 
			ElevationFocalZoneInUSImageFrame );
		this->CalibrationController->GetMinElevationBeamwidthAndFocalZoneInUSImageFrame()->put(1,
			MinElevationBeamwidth );

		// US 3D Beamwidth Profile Data
		// -----------------------------
		ThisConfiguration = "US_3D_BEAM_PROFILE]";
		while ( USBeamProfileFile.eof() != true && SectionName != ThisConfiguration )
		{
			USBeamProfileFile.ignore(1024, '[');
			USBeamProfileFile >> SectionName;
		}
		if(  SectionName != ThisConfiguration )
		{	// If the designated configuration is not found, throw up error
			std::cerr << "\n\n" << __FILE__ << "," << __LINE__ << "\n"
				<< "::CANNOT find the config section named: [" << ThisConfiguration 
				<< " in the configuration file!!!  Throw up ...\n";
			USBeamProfileFile.close();
			throw;
		}
		USBeamProfileFile.ignore(1024, '#');
		USBeamProfileFile.ignore(1024, '#');
		USBeamProfileFile.ignore(1024, '#');
		USBeamProfileFile.ignore(1024, '#');
		USBeamProfileFile.ignore(1024, '\n');
		int numUS3DBeamwidthProfileData; 
		USBeamProfileFile >> numUS3DBeamwidthProfileData;

		this->CalibrationController->SetNumUS3DBeamwidthProfileData(numUS3DBeamwidthProfileData); 

		if(  this->CalibrationController->GetNumUS3DBeamwidthProfileData() <= 0 )
		{	// If the number of data is invalid, throw up error
			vtkErrorMacro("ReadUs3DBeamwidthDataFromFile: The number of US 3D beamwidth profile data is invalid!!!  Please double check the data file!"); 
			throw;
		}		
		USBeamProfileFile.ignore(1024, '#');
		USBeamProfileFile.ignore(1024, '\n');

		// NOTE: here the profile read from file is unsorted (for ascending axial depth),
		//       but we will do the sorting in the next step.

		const int numOfUS3DBeamwidthProfileData = this->CalibrationController->GetNumUS3DBeamwidthProfileData(); 
		vnl_matrix<double> SortedUS3DBeamwidthInAscendingAxialDepth2CrystalsMatrixNx4(numOfUS3DBeamwidthProfileData, 4);
		USBeamProfileFile >> SortedUS3DBeamwidthInAscendingAxialDepth2CrystalsMatrixNx4;

		// Close the file for reading
		USBeamProfileFile.close();

		this->CalibrationController->GetSortedUS3DBeamwidthInAscendingAxialDepth2CrystalsMatrixNx4()->set_size(numOfUS3DBeamwidthProfileData, 4); 
		for ( int r = 0; r < numOfUS3DBeamwidthProfileData; r++)
		{
			this->CalibrationController->GetSortedUS3DBeamwidthInAscendingAxialDepth2CrystalsMatrixNx4()->set_row(r,SortedUS3DBeamwidthInAscendingAxialDepth2CrystalsMatrixNx4.get_row(r)); 
		}
		
	}
	catch(...)
	{
		vtkErrorMacro("ReadUs3DBeamwidthDataFromFile: Failed to read the US 3D Beam Profile Data from File!"); 
		throw;
	}
}


//----------------------------------------------------------------------------
void vtkProbeCalibrationControllerIO::LoadUS3DBeamProfileData()
{
	this->CalibrationController->SetUS3DBeamwidthDataReady(false); 

	try
	{
		// #1. Read the US 3D Beamwidth Data from a pre-populated file
		// ============================================================
		this->ReadUs3DBeamwidthDataFromFile(); 

		// #2. Sort the beamwidth in an ascending order with respect to the axial depth
		// =============================================================================

		// Sorting the beamwidth profile w.r.t the axial distance to the transducer
		// Sorting algorithm employed: Insertion Sort
		for( int i = 0; i < this->CalibrationController->GetNumUS3DBeamwidthProfileData(); i++ )
			for( int j = i; j > 0 && 
				this->CalibrationController->GetSortedUS3DBeamwidthInAscendingAxialDepth2CrystalsMatrixNx4()->get(j-1, 0) > 
				this->CalibrationController->GetSortedUS3DBeamwidthInAscendingAxialDepth2CrystalsMatrixNx4()->get(j, 0); j-- )
			{
				// Swap the positions of j-th and j-1-th elements
				const vnl_vector<double> SwapVector = 
					this->CalibrationController->GetSortedUS3DBeamwidthInAscendingAxialDepth2CrystalsMatrixNx4()->get_row(j-1);

				this->CalibrationController->GetSortedUS3DBeamwidthInAscendingAxialDepth2CrystalsMatrixNx4()->set_row( j-1, 
					this->CalibrationController->GetSortedUS3DBeamwidthInAscendingAxialDepth2CrystalsMatrixNx4()->get_row(j) );

				this->CalibrationController->GetSortedUS3DBeamwidthInAscendingAxialDepth2CrystalsMatrixNx4()->set_row( j, 
					SwapVector ); 
			}

			// #3. US 3D beamwidth profile data in ascending axial depth with weight factors
			// =============================================================================
			// 1. 3D beam width samples are measured at various axial depth/distance away 
			//    from the transducer crystals surface, i.e., the starting position of 
			//    the sound propagation in an ultrasound image.
			// 2. We have three ways to incorporate the US beamidth to the calibration: Use the 
			//    variance of beamwidth (BWVar) to weight the calibration, use the beamwidth ratio
			//    (BWRatio) to weight the calibration, or use the beamwidth to threshold the input
			//    data (BWTHEVar) in order to eliminate potentially unreliable or error-prone data.
			//    This is determined by the choice of the input flag (Option-1, 2, or 3).
			//    [1] BWVar: This conforms to the standard way of applying weights to least squares, 
			//        where the weights should, ideally, be equal to the reciprocal of the variance of 
			//        the measurement of the data if they are uncorrelated.  Since we know the US beam
			//        width at a given axial depth, resonably assuming the data acquired by the sound 
			//        field is normally distributed, the standard deviation (Sigma) of the data can be
			//        roughly estimated as in the equation: Sigma = USBeamWidth/4 (for 95% of data).
			//	  [2] BWRatio: The fifth row of the matrix is the overall weight defined at that axial 
			//        depth. The weight factor was calculated using: CurrentBeamwidth/MinimumBeamwidth.
			//        The weight is inversely proportional to the weight factor, as obviously, the larger 
			//        the beamwidth, the less reliable the data is than those with the minimum beamwidth.  
			//        We found the weight factor to be a good indicator for how reliable the data is, 
			//        because the larger the beamwidth the larger the uncertainties and errors in data 
			//        acquired from that US field.  E.g., at the axial depth where the beamwidth is two 
			//        times that of minimum beamwidth, the uncertainties are doubled than the imaging 
			//        region that has the minimum beamwidth.
			//	  [3] BWTHEVar: This utilizes the beamwidth to quality control the input calibration 
			//        data, by filtering out those that has a larger beamwidth (e.g., larger than twice
			//        of the minimum beamwidth at the current imaging settings).  According to ultrasound
			//        physics, data acquired in the sound field that doubles the minimum beamwidth at
			//        the scanplane or elevation plane focal zone are typically much less reliable than
			//        those closer to the focal zone.  In addition, the filtered, remainng data would be
			//        weighted for calibration using their beamwidth (BWVar in [1]).
			// FORMAT: each column in the matrices has the following rows:
			// [0]:		Sorted in ascending axial depth in US Image Frame (in pixels);
			// [1-3]:	Beamwith in axial, lateral and elevational axes respectively (in mm);
			// [4]:		Weight Factor = CurrentBeamWidth/MininumBeamWidth (>=1).
			this->CalibrationController->GetSortedUS3DBeamwidthAndWeightFactorsInAscendingAxialDepthInUSImageFrameMatrix5xN()->set_size(5, this->CalibrationController->GetNumUS3DBeamwidthProfileData());

			// Convert the depth of the beamwidth data into the TRUS Image Frame
			const vnl_vector<double> YPositionsOfUSBeamwidthInTRUSImageFramePixels = 
				this->CalibrationController->GetSortedUS3DBeamwidthInAscendingAxialDepth2CrystalsMatrixNx4()->get_column(0) + 
				this->CalibrationController->GetAxialPositionOfCrystalSurfaceInTRUSImageFrame();

			// Reset the axial-depth position and populate the remaining unchanged data
			this->CalibrationController->GetSortedUS3DBeamwidthAndWeightFactorsInAscendingAxialDepthInUSImageFrameMatrix5xN()->set_row(0, 
				YPositionsOfUSBeamwidthInTRUSImageFramePixels );
			this->CalibrationController->GetSortedUS3DBeamwidthAndWeightFactorsInAscendingAxialDepthInUSImageFrameMatrix5xN()->set_row(1, 
				this->CalibrationController->GetSortedUS3DBeamwidthInAscendingAxialDepth2CrystalsMatrixNx4()->get_column(1));
			this->CalibrationController->GetSortedUS3DBeamwidthAndWeightFactorsInAscendingAxialDepthInUSImageFrameMatrix5xN()->set_row(2, 
				this->CalibrationController->GetSortedUS3DBeamwidthInAscendingAxialDepth2CrystalsMatrixNx4()->get_column(2));
			this->CalibrationController->GetSortedUS3DBeamwidthAndWeightFactorsInAscendingAxialDepthInUSImageFrameMatrix5xN()->set_row(3, 
				this->CalibrationController->GetSortedUS3DBeamwidthInAscendingAxialDepth2CrystalsMatrixNx4()->get_column(3));

			// Calculate the weight factor using: CurrentBeamwidth/MinimumBeamwidth
			// NOTE: For the moment, we are only using the elevation beamwidth (the 4th column) 
			// which has the largest beamwidth among axial, lateral and elevational axes and 
			// plays an dominant role in the error distributions.  However, if axial and lateral
			// beamwidth can also be added into the calculation if they are available.
			this->CalibrationController->GetSortedUS3DBeamwidthAndWeightFactorsInAscendingAxialDepthInUSImageFrameMatrix5xN()->set_row(4,
				this->CalibrationController->GetSortedUS3DBeamwidthInAscendingAxialDepth2CrystalsMatrixNx4()->get_column(3)/
				this->CalibrationController->GetMinElevationBeamwidthAndFocalZoneInUSImageFrame()->get(1) );

			// #4. Interpolate the beamwidth profile at non-sampled axial depth
			// =================================================================

			// Initialize the Interpolated Beamwidth list to zero first
			std::vector<vnl_vector<double>> InterpUS3DBeamwidthAndWeightFactorsList;
			InterpUS3DBeamwidthAndWeightFactorsList.resize(0);

			for( int i = 0; i < this->CalibrationController->GetNumUS3DBeamwidthProfileData() - 1; i++ )
			{
				// Obtain the starting and ending sampled beamwidth data for interpolation
				const vnl_vector<double> StartBeamWidthDataVector = 
					this->CalibrationController->GetSortedUS3DBeamwidthAndWeightFactorsInAscendingAxialDepthInUSImageFrameMatrix5xN()->get_column(i);
				const vnl_vector<double> EndBeamWidthDataVector = 
					this->CalibrationController->GetSortedUS3DBeamwidthAndWeightFactorsInAscendingAxialDepthInUSImageFrameMatrix5xN()->get_column(i+1);

				// Obtain the starting and ending axial depth in US Image Frame
				const int StartDepthInUSImageFrame = StartBeamWidthDataVector.get(0);
				const int EndDepthInUSImageFrame = EndBeamWidthDataVector.get(0);

				// Start the interpolation of the beamwidth data
				for( int j = StartDepthInUSImageFrame; j < EndDepthInUSImageFrame; j++ )
				{
					// Perform the linear interpolation
					vnl_vector<double> InterpolatedBeamwidthDataVector = 
						(EndBeamWidthDataVector - StartBeamWidthDataVector)*
						(double(j - StartDepthInUSImageFrame)/double(EndDepthInUSImageFrame - StartDepthInUSImageFrame))
						+ StartBeamWidthDataVector;

					// Set the axial depth correctly
					InterpolatedBeamwidthDataVector.put(0, j);

					// Populate the interpolation list
					InterpUS3DBeamwidthAndWeightFactorsList.push_back( InterpolatedBeamwidthDataVector );
				}

				if( this->CalibrationController->GetNumUS3DBeamwidthProfileData() - 2 == i )
				{
					// Add in the last data in the list (which was not accounted for in the process)
					InterpUS3DBeamwidthAndWeightFactorsList.push_back( EndBeamWidthDataVector );
				}
			}

			// Populate the data to matrix container
			const int TotalNumOfInterpolatedUSBeamwidthData( InterpUS3DBeamwidthAndWeightFactorsList.size() );
			this->CalibrationController->GetInterpUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xM()->set_size(5, 
				TotalNumOfInterpolatedUSBeamwidthData);
			for( int i = 0; i < TotalNumOfInterpolatedUSBeamwidthData; i++ )
			{
				this->CalibrationController->GetInterpUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xM()->set_column( i,
					InterpUS3DBeamwidthAndWeightFactorsList.at(i) );
			}

			// Set the flag to signal the data is now ready
			this->CalibrationController->SetUS3DBeamwidthDataReady(true); 

	}
	catch(...)
	{
		LOG_ERROR("Failed to read the US 3D Beam Profile Data from File!"); 
		throw;
	}
}

//----------------------------------------------------------------------------
void vtkProbeCalibrationControllerIO::ReadProbeCalibrationConfiguration(vtkXMLDataElement* probeCalibration)
{
	if ( probeCalibration == NULL) 
	{	
		LOG_WARNING("Unable to read ProbeCalibration XML data element!"); 
		return; 
	} 

	// To enable/disable the system logging
	const char* enableLogFile = probeCalibration->GetAttribute("EnableLogFile"); 
	if (  enableLogFile != NULL &&  STRCASECMP( "TRUE", enableLogFile ) == 0 ) 
	{
		this->CalibrationController->EnableSystemLogOn(); 
	}
	else
	{
		this->CalibrationController->EnableSystemLogOff(); 
	}

	// To enable/disable the saving of segmented wire positions to file 
	const char* enableSegmentedWirePositionsSaving = probeCalibration->GetAttribute("EnableSegmentedWirePositionsSaving"); 
	if ( enableSegmentedWirePositionsSaving != NULL &&  STRCASECMP( "TRUE", enableSegmentedWirePositionsSaving ) == 0 ) 
	{
		this->CalibrationController->EnableSegmentedWirePositionsSavingOn(); 
	}
	else
	{
		this->CalibrationController->EnableSegmentedWirePositionsSavingOff(); 
	}

	//Transform: from image home position to user defined image home position
	double* imageHomeToUserImageHomeTransform = new double[16]; 
	if ( probeCalibration->GetVectorAttribute("ImageHomeToUserImageHomeTransform", 16, imageHomeToUserImageHomeTransform) )
	{
		this->CalibrationController->GetTransformImageHomeToUserImageHome()->SetMatrix(imageHomeToUserImageHomeTransform); 
		this->CalibrationController->GetTransformUserImageToImage()->SetMatrix(imageHomeToUserImageHomeTransform); 
		this->CalibrationController->GetTransformUserImageToImage()->Inverse(); 
	}
	delete [] imageHomeToUserImageHomeTransform; 

	// Sets the suffix of the data files
	const char* dataFileSuffix = probeCalibration->GetAttribute("DataFileSuffix"); 
	if ( dataFileSuffix != NULL) 
	{
		this->CalibrationController->SetDataFileSuffix(dataFileSuffix); 
	}
	else 
	{
		this->CalibrationController->SetDataFileSuffix(".data"); 
	}

	// Sets the suffix of the calibration result file
	const char* calibrationResultFileSuffix = probeCalibration->GetAttribute("CalibrationResultFileSuffix"); 
	if ( calibrationResultFileSuffix != NULL) 
	{
		this->CalibrationController->SetCalibrationResultFileSuffix(calibrationResultFileSuffix); 
	}
	else 
	{
		this->CalibrationController->SetCalibrationResultFileSuffix(".Calibration.results"); 
	}

	// Sets the suffix of the segmentation error log file
	const char* segmentationErrorLogFileNameSuffix = probeCalibration->GetAttribute("SegmentationErrorLogFileNameSuffix"); 
	if ( segmentationErrorLogFileNameSuffix != NULL) 
	{
		this->CalibrationController->SetSegmentationErrorLogFileNameSuffix(segmentationErrorLogFileNameSuffix); 
	}
	else 
	{
		this->CalibrationController->SetSegmentationErrorLogFileNameSuffix(".Segmentation.errors"); 
	}

	// Sets the suffix of the segmentation analysis file
	const char* segmentationAnalysisFileNameSuffix = probeCalibration->GetAttribute("SegmentationAnalysisFileNameSuffix"); 
	if ( segmentationAnalysisFileNameSuffix != NULL) 
	{
		this->CalibrationController->SetSegmentationAnalysisFileNameSuffix(segmentationAnalysisFileNameSuffix); 
	}
	else 
	{
		this->CalibrationController->SetSegmentationAnalysisFileNameSuffix(".Segmentation.analysis"); 
	}

	// Sets the suffix of the Template2StepperCalibration analysis file
	const char* temp2StepCalibAnalysisFileNameSuffix = probeCalibration->GetAttribute("Temp2StepCalibAnalysisFileNameSuffix"); 
	if ( temp2StepCalibAnalysisFileNameSuffix != NULL) 
	{
		this->CalibrationController->SetTemp2StepCalibAnalysisFileNameSuffix(temp2StepCalibAnalysisFileNameSuffix); 
	}
	else 
	{
		this->CalibrationController->SetTemp2StepCalibAnalysisFileNameSuffix(".Template2StepperCalibration.analysis"); 
	}

  	// RandomStepperMotionData1 data set specifications
	//********************************************************************
	vtkSmartPointer<vtkXMLDataElement> randomStepperMotionData1 = probeCalibration->FindNestedElementWithName("RandomStepperMotionData1"); 
	if ( randomStepperMotionData1 != NULL) 
	{
		vtkCalibrationController::ImageDataInfo imageDataInfo = this->CalibrationController->GetImageDataInfo(RANDOM_STEPPER_MOTION_1); 
		int numberOfImagesToUse = -1;
		if ( randomStepperMotionData1->GetScalarAttribute("NumberOfImagesToAcquire", numberOfImagesToUse) ) 
		{
			imageDataInfo.NumberOfImagesToAcquire = numberOfImagesToUse; 
		}

		const char* sequenceMetaFile = randomStepperMotionData1->GetAttribute("OutputSequenceMetaFileSuffix"); 
		if ( sequenceMetaFile != NULL) 
		{
			imageDataInfo.OutputSequenceMetaFileSuffix.assign(sequenceMetaFile); 
		}

		this->CalibrationController->SetImageDataInfo(RANDOM_STEPPER_MOTION_1, imageDataInfo); 
	}
	else
	{
		LOG_DEBUG("Unable to find RandomStepperMotionData1 XML data element, default 200 is used"); 
		vtkCalibrationController::ImageDataInfo imageDataInfo = this->CalibrationController->GetImageDataInfo(RANDOM_STEPPER_MOTION_1); 
		imageDataInfo.NumberOfImagesToAcquire = 200;
    this->CalibrationController->SetImageDataInfo(RANDOM_STEPPER_MOTION_1, imageDataInfo); 
	}

	// RandomStepperMotionData2 data set specifications
	//********************************************************************
	vtkSmartPointer<vtkXMLDataElement> randomStepperMotionData2 = probeCalibration->FindNestedElementWithName("RandomStepperMotionData2"); 
	if ( randomStepperMotionData2 != NULL) 
	{
		vtkCalibrationController::ImageDataInfo imageDataInfo = this->CalibrationController->GetImageDataInfo(RANDOM_STEPPER_MOTION_2); 
		int numberOfImagesToUse = -1;
		if ( randomStepperMotionData2->GetScalarAttribute("NumberOfImagesToAcquire", numberOfImagesToUse) ) 
		{
			imageDataInfo.NumberOfImagesToAcquire = numberOfImagesToUse; 
		}

		const char* sequenceMetaFile = randomStepperMotionData2->GetAttribute("OutputSequenceMetaFileSuffix"); 
		if ( sequenceMetaFile != NULL) 
		{
			imageDataInfo.OutputSequenceMetaFileSuffix.assign(sequenceMetaFile); 
		}

		this->CalibrationController->SetImageDataInfo(RANDOM_STEPPER_MOTION_2, imageDataInfo); 
	}
	else
	{
		LOG_DEBUG("Unable to find RandomStepperMotionData2 XML data element, default 100 is used"); 
		vtkCalibrationController::ImageDataInfo imageDataInfo = this->CalibrationController->GetImageDataInfo(RANDOM_STEPPER_MOTION_2); 
		imageDataInfo.NumberOfImagesToAcquire = 100; 
    this->CalibrationController->SetImageDataInfo(RANDOM_STEPPER_MOTION_2, imageDataInfo); 
	}

	// Template model specifications
	//********************************************************************
	vtkSmartPointer<vtkXMLDataElement> templateModel = probeCalibration->FindNestedElementWithName("TemplateModel"); 
	if ( templateModel != NULL) 
	{
		// Path to the template model config file 
		const char* configFile = templateModel->GetAttribute("ConfigFile"); 
		if ( configFile != NULL) 
		{
			std::string fullTemplateModelPath = vtksys::SystemTools::CollapseFullPath(configFile, this->CalibrationController->GetProgramFolderPath()); 
			this->CalibrationController->SetTemplateModelConfigFileName(fullTemplateModelPath.c_str()); 
		}
	}
	else
	{
		LOG_WARNING("Unable to find TemplateModel XML data element"); 
	}

	// US3DBeamwidth specifications
	//********************************************************************
	vtkSmartPointer<vtkXMLDataElement> us3DBeamProfile = probeCalibration->FindNestedElementWithName("US3DBeamProfile"); 
	if ( us3DBeamProfile != NULL) 
	{
		// To incorporate the ultrasound beam profile (3D beam width)
		int incorporatingUS3DBeamProfile = 0;
		if ( us3DBeamProfile->GetScalarAttribute("IncorporatingUS3DBeamProfile", incorporatingUS3DBeamProfile) ) 
		{
			this->CalibrationController->SetIncorporatingUS3DBeamProfile(incorporatingUS3DBeamProfile); 
		}

		// The US-3D-beam-profile data file name and path (if choose to incorporate the beam profile)
		const char* configFile = us3DBeamProfile->GetAttribute("ConfigFile"); 
		if ( configFile != NULL) 
		{
			this->CalibrationController->SetUS3DBeamProfileDataFileNameAndPath(configFile); 
		}

		if( this->CalibrationController->GetIncorporatingUS3DBeamProfile() > 0 && this->CalibrationController->GetIncorporatingUS3DBeamProfile() < 4 )
		{
			this->LoadUS3DBeamProfileData();
		}
	}
	else
	{
		LOG_WARNING("Unable to find US3DBeamProfile XML data element"); 
	}
}

