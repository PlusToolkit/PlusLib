#include "PlusConfigure.h"

#include "vtkUltrasoundDataCollector.h"
#include "vtkUltrasoundDataCollectorHelper.h"
#include "vtkObjectFactory.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkMath.h"
#include "vtkCriticalSection.h"
#include "vtkMutexLock.h"
#include "vtkSignalBox.h"
#include "vtkXMLUtilities.h"
#include "vtkXMLDataElement.h"
#include "vtkBMPWriter.h"
#include "vtkImageFlip.h"
#include "vtkSmartPointer.h"
#include <sstream>
#include <time.h>


#include "itkImage.h"
#include "itkMeanSquaresImageToImageMetric.h"
#include "itkTranslationTransform.h"
#include "itkNearestNeighborInterpolateImageFunction.h"
//#include "itkVTKImageToImageFilter.h"


vtkCxxRevisionMacro(vtkUltrasoundDataCollector, "$Revisions: 1.0 $");
vtkStandardNewMacro(vtkUltrasoundDataCollector);

//----------------------------------------------------------------------------
vtkUltrasoundDataCollector::vtkUltrasoundDataCollector()
{
	//this->FlipFrame = FLIP_X;
	this->FlipFrame = FLIP_NONE;
	//this->SliceTransform->TransformDoubleNormal(30,0,0); 
	//vtkSmartPointer<vtkTransform> tRAS2Template = vtkTransform::New(); 
	//tRAS2Template = static_cast<vtkTransform *>(this->GetSliceTransform()); 
	//tRAS2Template->Identity(); 
	//tRAS2Template->Translate(30,0,0); 
}

//----------------------------------------------------------------------------
vtkUltrasoundDataCollector::~vtkUltrasoundDataCollector()
{

}

//----------------------------------------------------------------------------
void vtkUltrasoundDataCollector::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os,indent);

}

//----------------------------------------------------------------------------
// Start doing real-time reconstruction from the video source.
// This will spawn a thread that does the reconstruction in the
// background
void vtkUltrasoundDataCollector::StartRealTimeReconstruction()
{

	// if the real time reconstruction isn't already running...
	if (this->ReconstructionThreadId == -1)
	{
		this->RealTimeReconstruction = 1; // we are doing realtime reconstruction

		// Setup the slice axes matrix
		vtkMatrix4x4 *matrix = vtkMatrix4x4::New();
		this->SetSliceAxes(matrix);
		matrix->Delete();

		// initialize the real time reconstruction - this is overridden in derived
		// classes
		if (this->InitializeRealTimeReconstruction())
		{
			this->InternalExecuteInformation();

			this->ReconstructionThreadId = \
				this->Threader->SpawnThread((vtkThreadFunctionType)\
				&vtkReconstructionThread,
				this);
		}
		else
		{
			vtkErrorMacro(<< "Could not initialize real-time reconstruction ... stopping");
		}
	}
}

//****************************************************************************
// SAVING INSERTED TIMESTAMPS / FRAMES
//****************************************************************************

//----------------------------------------------------------------------------
// Sets the (relative!) directory for saving inserted timestamps and/or slices, and creates
// it if it doesn't exist
void vtkUltrasoundDataCollector::SetSaveInsertedDirectory(const char* directory)
{
	if (this->ReconstructionThreadId != -1)
	{
		return;
	}

	// if we are switching directories
	if (this->SaveInsertedDirectory)
	{
		delete [] this->SaveInsertedDirectory;
	}

	// create the directory for the timestamps and/or slices
	int res;
#ifdef _WIN32
	res = _mkdir(directory);
#else
	int mode = 0777;
	res = mkdir(directory, mode);
#endif

	this->SaveInsertedDirectory = new char[512];
	sprintf(this->SaveInsertedDirectory, "%s", directory);

	// reset this to make the timestamps file for the new directory
	if (this->SaveInsertedTimestamps)
	{
		this->SaveInsertedTimestampsOn();
	}

}


//----------------------------------------------------------------------------
// If turned on, keeps a record of the timestamps used to insert slices
// Save the frame timestamps in the directory previously specified
// frameTimestamps.txt - a file with the frame timestamps inserted
void vtkUltrasoundDataCollector::SaveInsertedTimestampsOn()
{
	if (this->ReconstructionThreadId != -1)
	{
		return;
	}

	/*if (!this->SaveInsertedDirectory) // annoying with AV setup
	{
	vtkWarningMacro(<< "Can't set saving inserted timestamps if we don't have a directory yet");
	return;
	}*/

	// in case we're switching directories
	if (this->SaveInsertedTimestampsOutputFile)
	{
		fclose(this->SaveInsertedTimestampsOutputFile);
	}
	if (this->SaveInsertedTimestampsOutputFilename)
	{
		delete [] this->SaveInsertedTimestampsOutputFilename;
	}

	// create the timestamps file

	char path[512];
#ifdef _WIN32
	sprintf(path, "%s\\%s", this->SaveInsertedDirectory, "frameTimestamps.txt");
#else
	sprintf(path, "%s/%s", this->SaveInsertedDirectory, "frameTimestamps.txt");
#endif

	this->SaveInsertedTimestampsOutputFilename = new char[512];
	sprintf(this->SaveInsertedTimestampsOutputFilename, "%s", path);
	this->SaveInsertedTimestampsOutputFile = fopen(path,"a");

	// if it failed to open...
	if (this->SaveInsertedTimestampsOutputFile == NULL)
	{
		this->SaveInsertedTimestamps = 0;
		this->SaveInsertedTimestampsCounter = 0;
		//delete [] this->SaveInsertedDirectory;
		delete [] this->SaveInsertedTimestampsOutputFile;
		delete [] this->SaveInsertedTimestampsOutputFilename;
		//this->SaveInsertedDirectory = NULL;
		this->SaveInsertedTimestampsOutputFilename = NULL;
		return;
	}

	//fprintf(this->SaveInsertedTimestampsOutputFile, "Slice#\tTimestamp\t\t\tPhase\t\tVideoTime\t\t\tECGRate\n");
	this->SaveInsertedTimestamps = 1;
	//this->SaveInsertedTimestampsCounter = 0;
	fclose(this->SaveInsertedTimestampsOutputFile); // will open again in initialize reconstruction

}

//----------------------------------------------------------------------------
// If turned on, saves the inserted 2D slices as .bmps in the directory previously specified.
// Will also automatically turn saving inserted timestamps on
void vtkUltrasoundDataCollector::SaveInsertedSlicesOn()
{
	if (this->ReconstructionThreadId != -1)
	{
		return;
	}

	/*if (!this->SaveInsertedDirectory) // annoying with AV setup
	{
	vtkWarningMacro(<< "Can't set saving inserted timestamps if we don't have a directory yet");
	return;
	}*/

	// create the image writer
	this->SaveInsertedSlicesWriter = vtkBMPWriter::New();
	this->SaveInsertedSlicesWriter->SetFileDimensionality(2);

	this->SaveInsertedSlices = 1;

	// set saving timestamps on by default, because otherwise the slices are useless
	// for offline reconstruction
	if (!this->SaveInsertedTimestamps)
	{
		this->SaveInsertedTimestampsOn();
	}
}

//----------------------------------------------------------------------------
void vtkUltrasoundDataCollector::SaveInsertedTimestampsOff()
{

	if (this->ReconstructionThreadId != -1)
	{
		return;
	}

	// need to keep saving inserted timestamps if we are saving inserted slicse
	if (this->SaveInsertedSlices)
	{
		return;
	}

	// sanity check - make sure we are changing settings
	if (!this->SaveInsertedTimestamps)
	{
		return;
	}

	if (this->SaveInsertedTimestampsOutputFile)
	{
		fclose(this->SaveInsertedTimestampsOutputFile);
		this->SaveInsertedTimestampsOutputFile = NULL;
	}

	/*if (this->SaveInsertedDirectory)
	{
	delete [] this->SaveInsertedDirectory;
	this->SaveInsertedDirectory = NULL;
	}*/

	if (this->SaveInsertedTimestampsOutputFilename)
	{
		delete [] this->SaveInsertedTimestampsOutputFilename;
		this->SaveInsertedTimestampsOutputFilename = NULL;
	}

	this->SaveInsertedTimestamps = 0;
	//this->SaveInsertedTimestampsCounter = 0;
}

//----------------------------------------------------------------------------
void vtkUltrasoundDataCollector::SaveInsertedSlicesOff()
{

	if (this->ReconstructionThreadId != -1)
	{
		return;
	}

	// sanity check - make sure we are changing settings
	if (!this->SaveInsertedSlices)
	{
		return;
	}

	this->SaveInsertedSlices = 0;

	if (this->SaveInsertedSlicesWriter)
	{
		this->SaveInsertedSlicesWriter->Delete();
		this->SaveInsertedSlicesWriter = NULL;
	}

}

//----------------------------------------------------------------------------
// Insert a timestamp into the buffer, if we are saving timestamps (note that
// the timestamp counter is handled whenever a slice is actually inserted, not here)
// Give the phase, the timestamp we are reconstructing, and the timestamp that
// we calculated before we applied temporal calibration
void vtkUltrasoundDataCollector::SaveSliceAndTransform(vtkImageData* inData, vtkMatrix4x4* transMatrix, double timestamp)
{
	CalibDataStruct calibData; 

	this->ActiveFlagLock->Lock();

	calibData.ImageData = vtkImageData::New(); 

	// Flip the output image for RAS representation 
	if (this->FlipFrame != FLIP_NONE)
	{
		vtkSmartPointer<vtkImageFlip> flip = vtkImageFlip::New(); 
		flip->SetInput(inData); 
		flip->SetFilteredAxis(this->FlipFrame); 
		flip->Update(); 
		calibData.ImageData->DeepCopy(flip->GetOutput()); 
	}
	else
	{
		calibData.ImageData->DeepCopy(inData); 
	}



	calibData.Timestamp = timestamp; 

	calibData.TProbe2World = vtkMatrix4x4::New(); 
	calibData.TProbe2World->DeepCopy(transMatrix); 

	calibData.TFrame2World  = vtkMatrix4x4::New(); 
	calibData.TFrame2World->DeepCopy(this->GetFrameToWorldMatrix(transMatrix, this->SliceTransform)); 

	calibData.TFrame2RAS = vtkMatrix4x4::New(); 
	calibData.TFrame2RAS->DeepCopy(this->GetIndexMatrixHelper(transMatrix, this->SliceTransform)); 

	this->ActiveFlagLock->Unlock();

	CalibData.push_back(calibData); 
}

void vtkUltrasoundDataCollector::SaveDataToFolder(const char* folder)
{
	vtkSmartPointer<vtkBMPWriter> bmpWriter = vtkBMPWriter::New(); 
	vtkSmartPointer<vtkMatrix4x4> tProbe2World = vtkMatrix4x4::New(); 
	vtkSmartPointer<vtkMatrix4x4> tFrame2RAS = vtkMatrix4x4::New(); 
	vtkSmartPointer<vtkMatrix4x4> tFrame2World = vtkMatrix4x4::New(); 
	vtkSmartPointer<vtkImageFlip> flip = vtkImageFlip::New(); 

	vtkSmartPointer<vtkTransform> helperTransform = vtkTransform::New(); 

	std::cout << "\nThere are "<< this->CalibData.size() << " image files to save." ; 


	time_t rawtime;
	time ( &rawtime );
	struct tm * timeinfo = localtime ( &rawtime );
	char dirDateStamp[256];  
	int mkdirRes; 

#ifdef _WIN32
	sprintf(dirDateStamp,"%s\\%i-%i-%i_%i.%i.%i", folder, (timeinfo->tm_year+1900), (timeinfo->tm_mon+1), timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec); 
	mkdirRes = _mkdir(dirDateStamp);
#else
	sprintf(dirDateStamp,"%s\\%i-%i-%i_%i.%i.%i", folder, timeinfo->tm_year, timeinfo->tm_mon, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec); 
	int mode = 0777;
	mkdirRes = mkdir(dirDateStamp, mode);
#endif

	this->ActiveFlagLock->Lock();
	for (int i = 0; i < this->CalibData.size(); i++)
	{
		CalibDataStruct data = CalibData[i]; 

		vtkstd::ostringstream osIndex; 

#ifdef _WIN32
		if (mkdirRes != -1) 
		{
			osIndex << dirDateStamp << "\\CapturedImageID_NO_"; 
		}
		else 
		{
			osIndex << folder << "\\CapturedImageID_NO_"; 
		}
#else
		if (mkdirRes != -1) 
		{
			osIndex << dirDateStamp << "/CapturedImageID_NO_"; 
		}
		else 
		{
			osIndex << folder << "/CapturedImageID_NO_"; 
		}

#endif

		if (i<10)
		{
			osIndex << "000" << i; 
		} else if (i<100)
		{
			osIndex << "00" << i; 
		}
		else if (i < 1000)
		{
			osIndex << "0" << i; 
		}
		else
		{
			osIndex << i; 
		}


		std::string imageFileName_RAS(osIndex.str()); 
		imageFileName_RAS.append("_RAS.bmp"); 

		std::string imageFileName_raw(osIndex.str()); 
		imageFileName_raw.append("_raw.bmp"); 


		flip->SetInput(data.ImageData); 
		flip->SetFilteredAxis(FLIP_X); 
		flip->Update(); 

		try 
		{
			bmpWriter->SetFileName(imageFileName_RAS.c_str()); 
			bmpWriter->SetInput(flip->GetOutput()); 
			bmpWriter->Update(); 

			bmpWriter->SetFileName(imageFileName_raw.c_str()); 
			bmpWriter->SetInput(data.ImageData); 
			bmpWriter->Update(); 
		}
		catch (char * str) 
		{
			vtkErrorMacro(<<"Unable to save image to BMP file: " << str); 
			continue; 
		}

		tProbe2World->DeepCopy(data.TProbe2World); 
		tFrame2RAS->DeepCopy(data.TFrame2RAS); 
		tFrame2World->DeepCopy(data.TFrame2World); 

		std::string transformFileName(osIndex.str()); 
		transformFileName.append(".bmp.transforms"); 

		vtkstd::string transformData; 
		transformData+= "# ================================ #\n";
		transformData+= "# Transform Data of Captured Image #\n";
		transformData+= "# ================================ #\n";
		transformData+= "# THIS FILE CONTAINS THE REAL-TIME TRANSFORM DATA FOR THE CAPTURED IMAGE.\n";
		transformData+= "# DATA IS RECORDED IN THE FOLLOWING FORMAT:\n";
		transformData+= "# [FORMAT: Quaternions(Angle - in Degrees, Qx, Qy, Qz) Position(x, y, z)]\n";
		transformData+= "# THIS FILE IS AUTO-GENERATED BY THE PROGRAM.  DO NOT EDIT!\n";
		transformData+= "\n";
		transformData+= "# NAME OF THE CAPTURED IMAGE WITH PATH\n";
		transformData+= imageFileName_raw;
		transformData+= "\n\n";
		transformData+= imageFileName_RAS;
		transformData+= "\n\n";

		FILE * transformFile = fopen ( transformFileName.c_str(), "w");
		if (transformFile != NULL)
		{
			double wxyz[4], xyz[3]; 
			vtkstd::ostringstream os; 

			os  << "# --------------------------------------------------------------- #\n"
				<< "# TRANSFORMATION FROM TRACKING DEVICE\n"
				<< std::endl; 

			helperTransform->SetMatrix(tProbe2World); 
			helperTransform->GetOrientationWXYZ(wxyz); 
			helperTransform->GetPosition(xyz); 

			//os  << "# [TRANSFORM_QUATERNIONS_USPROBE_TO_WORLD]\n"
			os	<< "# [TRANSFORM_QUATERNIONS_USPROBE_TO_STEPPER_FRAME]\n"
				<< wxyz[0] << "\t" << wxyz[1] << "\t" << wxyz[2] << "\t" << wxyz[3] << "\t"
				<<  xyz[0] << "\t" <<  xyz[1] << "\t" <<  xyz[2] << "\n" 
				<< "\n"
				//<< "# [TRANSFORM_HOMOGENEOUS4x4_USPROBE_TO_WORLD]\n"
				<< "# [TRANSFORM_HOMOGENEOUS4x4_USPROBE_TO_STEPPER_FRAME]\n"
				<< tProbe2World->GetElement(0,0) << "\t" << tProbe2World->GetElement(0,1) << "\t" 
				<< tProbe2World->GetElement(0,2) << "\t" << tProbe2World->GetElement(0,3) << "\n"
				<< tProbe2World->GetElement(1,0) << "\t" << tProbe2World->GetElement(1,1) << "\t" 
				<< tProbe2World->GetElement(1,2) << "\t" << tProbe2World->GetElement(1,3) << "\n"
				<< tProbe2World->GetElement(2,0) << "\t" << tProbe2World->GetElement(2,1) << "\t" 
				<< tProbe2World->GetElement(2,2) << "\t" << tProbe2World->GetElement(2,3) << "\n"
				<< tProbe2World->GetElement(3,0) << "\t" << tProbe2World->GetElement(3,1) << "\t" 
				<< tProbe2World->GetElement(3,2) << "\t" << tProbe2World->GetElement(3,3) << "\n"
				<< std::endl; 

			os  << "# --------------------------------------------------------------- #\n"  
				<< "# SCALE BY FRAME SPACING AND TRANSLATE TO FRAME ORIGIN\n"
				<< "# APPLY TRACKING TRANSFORMATION\n"
				<< std::endl; 

			helperTransform->SetMatrix(tFrame2World); 
			helperTransform->GetOrientationWXYZ(wxyz); 
			helperTransform->GetPosition(xyz); 

			os  << "# [TRANSFORM_QUATERNIONS_FRAME_TO_WORLD]\n"
				<< wxyz[0] << "\t" << wxyz[1] << "\t" << wxyz[2] << "\t" << wxyz[3] << "\t"
				<<  xyz[0] << "\t" <<  xyz[1] << "\t" <<  xyz[2] << "\n" 
				<< "\n"
				<< "# [TRANSFORM_HOMOGENEOUS4x4_FRAME_TO_WORLD]\n"
				<< tFrame2World->GetElement(0,0) << "\t" << tFrame2World->GetElement(0,1) << "\t" 
				<< tFrame2World->GetElement(0,2) << "\t" << tFrame2World->GetElement(0,3) << "\n"
				<< tFrame2World->GetElement(1,0) << "\t" << tFrame2World->GetElement(1,1) << "\t" 
				<< tFrame2World->GetElement(1,2) << "\t" << tFrame2World->GetElement(1,3) << "\n"
				<< tFrame2World->GetElement(2,0) << "\t" << tFrame2World->GetElement(2,1) << "\t" 
				<< tFrame2World->GetElement(2,2) << "\t" << tFrame2World->GetElement(2,3) << "\n"
				<< tFrame2World->GetElement(3,0) << "\t" << tFrame2World->GetElement(3,1) << "\t" 
				<< tFrame2World->GetElement(3,2) << "\t" << tFrame2World->GetElement(3,3) << "\n"
				<< std::endl;



			os  << "# --------------------------------------------------------------- #\n"  
				<< "# SCALE BY FRAME SPACING AND TRANSLATE TO FRAME ORIGIN\n"
				<< "# APPLY TRACKING TRANSFORMATION\n"
				<< "# TRANSFORM TO RAS\n"
				<< std::endl; 

			helperTransform->SetMatrix(tFrame2RAS); 
			helperTransform->GetOrientationWXYZ(wxyz); 
			helperTransform->GetPosition(xyz); 


			os  << "# [TRANSFORM_QUATERNIONS_FRAME_TO_RAS]\n"
				<< "# [TRANSFORM_QUATERNIONS_USPROBE_TO_TEMPLATE-STEPPER_FRAME]\n"
				<< wxyz[0] << "\t" << wxyz[1] << "\t" << wxyz[2] << "\t" << wxyz[3] << "\t"
				<<  xyz[0] << "\t" <<  xyz[1] << "\t" <<  xyz[2] << "\n" 
				<< "\n"
				<< "# [TRANSFORM_HOMOGENEOUS4x4_FRAME_TO_RAS]\n"
				<< "# [TRANSFORM_HOMOGENEOUS4x4_USPROBE_TO_TEMPLATE-STEPPER_FRAME]\n"
				<< tFrame2RAS->GetElement(0,0) << "\t" << tFrame2RAS->GetElement(0,1) << "\t" 
				<< tFrame2RAS->GetElement(0,2) << "\t" << tFrame2RAS->GetElement(0,3) << "\n"
				<< tFrame2RAS->GetElement(1,0) << "\t" << tFrame2RAS->GetElement(1,1) << "\t" 
				<< tFrame2RAS->GetElement(1,2) << "\t" << tFrame2RAS->GetElement(1,3) << "\n"
				<< tFrame2RAS->GetElement(2,0) << "\t" << tFrame2RAS->GetElement(2,1) << "\t" 
				<< tFrame2RAS->GetElement(2,2) << "\t" << tFrame2RAS->GetElement(2,3) << "\n"
				<< tFrame2RAS->GetElement(3,0) << "\t" << tFrame2RAS->GetElement(3,1) << "\t" 
				<< tFrame2RAS->GetElement(3,2) << "\t" << tFrame2RAS->GetElement(3,3) << "\n"
				<< std::endl;

			
			/*os  << "# --------------------------------------------------------------- #\n"
				<< "# TRANSFORMATION TO THE TEMPLATE ORIGIN (A7 point)\n"
				<< std::endl; 

			helperTransform->SetMatrix(this->SliceTransform->GetMatrix()); 
			helperTransform->GetOrientationWXYZ(wxyz); 
			helperTransform->GetPosition(xyz); 

			os  << "# [TRANSFORM_QUATERNIONS_RAS_TO_TEMPLATE]\n"
				<< wxyz[0] << "\t" << wxyz[1] << "\t" << wxyz[2] << "\t" << wxyz[3] << "\t"
				<<  xyz[0] << "\t" <<  xyz[1] << "\t" <<  xyz[2] << "\n" 
				<< "\n"
				<< "# [TRANSFORM_HOMOGENEOUS4x4_RAS_TO_TEMPLATE]\n"
				<< this->SliceTransform->GetMatrix()->GetElement(0,0) << "\t" << this->SliceTransform->GetMatrix()->GetElement(0,1) << "\t" 
				<< this->SliceTransform->GetMatrix()->GetElement(0,2) << "\t" << this->SliceTransform->GetMatrix()->GetElement(0,3) << "\n"
				<< this->SliceTransform->GetMatrix()->GetElement(1,0) << "\t" << this->SliceTransform->GetMatrix()->GetElement(1,1) << "\t" 
				<< this->SliceTransform->GetMatrix()->GetElement(1,2) << "\t" << this->SliceTransform->GetMatrix()->GetElement(1,3) << "\n"
				<< this->SliceTransform->GetMatrix()->GetElement(2,0) << "\t" << this->SliceTransform->GetMatrix()->GetElement(2,1) << "\t" 
				<< this->SliceTransform->GetMatrix()->GetElement(2,2) << "\t" << this->SliceTransform->GetMatrix()->GetElement(2,3) << "\n"
				<< this->SliceTransform->GetMatrix()->GetElement(3,0) << "\t" << this->SliceTransform->GetMatrix()->GetElement(3,1) << "\t" 
				<< this->SliceTransform->GetMatrix()->GetElement(3,2) << "\t" << this->SliceTransform->GetMatrix()->GetElement(3,3) << "\n"
				<< std::endl; */

			os << "# --------------------------------------------------------------- #\n\n"; 

			transformData += os.str(); 
			fprintf(transformFile, transformData.c_str()); 
			fclose(transformFile); 
		}
	}

	this->ActiveFlagLock->Unlock();
}

//----------------------------------------------------------------------------
// Inserts a single slice into the volume
void vtkUltrasoundDataCollector::ReconstructSlice(double timestamp, vtkImageData* inData)
{
	this->InsertSlice(this->GetOutput(), this->GetAccumulationBuffer());

	// save timestamps to buffer
	if (this->SaveInsertedTimestamps || this->SaveInsertedSlices)
	{
		// Get the transformation matrix from timestamp
		vtkMatrix4x4* transMatrix = vtkMatrix4x4::New(); 
		this->GetTrackerTool()->GetBuffer()->GetFlagsAndMatrixFromTime(transMatrix, timestamp);

		this->SaveSliceAndTransform(inData, transMatrix, timestamp);
	}
}

//****************************************************************************
// I/O
//****************************************************************************

//----------------------------------------------------------------------------
vtkXMLDataElement* vtkUltrasoundDataCollector::MakeXMLElement()
{
	vtkXMLDataElement* elem;
	elem = this->Superclass::MakeXMLElement();

	// triggering parameters
	vtkXMLDataElement* triggeringParams = vtkXMLDataElement::New();
	triggeringParams->SetName("TriggeringParameters");
	(this->Triggering ? triggeringParams->SetAttribute("Triggering", "On") : triggeringParams->SetAttribute("Triggering", "Off"));
	(this->Retrospective ? triggeringParams->SetAttribute("Gating", "Retrospective") : triggeringParams->SetAttribute("Gating", "Prospective"));
	triggeringParams->SetIntAttribute("NumOutputVolumes", this->NumberOfOutputVolumes);
	elem->AddNestedElement(triggeringParams);

	// signal box parameters
	vtkXMLDataElement *signalBoxParams = vtkXMLDataElement::New();
	signalBoxParams->SetName("SignalBox");
	if (this->SignalBox)
	{
		signalBoxParams->SetIntAttribute("NumPhases", this->SignalBox->GetNumberOfPhases());
	}
	else
	{
		signalBoxParams->SetAttribute("SignalBox", "NULL");
	}
	elem->AddNestedElement(signalBoxParams);

	// check heart rate parameters
	vtkXMLDataElement* checkParams = vtkXMLDataElement::New();
	checkParams->SetName("CheckHeartRateParameters");
	(this->CheckHeartRate ? checkParams->SetAttribute("CheckHeartRate", "On") : checkParams->SetAttribute("CheckHeartRate", "Off"));
	checkParams->SetDoubleAttribute("ECGMonitoringTime", this->ECGMonitoringTime);
	checkParams->SetIntAttribute("NumECGTrials", this->NumECGTrials);
	checkParams->SetDoubleAttribute("PercentIncreaseHRAllowed", this->PercentageIncreasedHeartRateAllowed);
	checkParams->SetDoubleAttribute("PercentDecreasedHRAllowed", this->PercentageDecreasedHeartRateAllowed);
	checkParams->SetDoubleAttribute("MeanHR", this->MeanHeartRate);
	elem->AddNestedElement(checkParams);

	// selecting phases
	vtkXMLDataElement* selectingParams = vtkXMLDataElement::New();
	selectingParams->SetName("UseSelectPhasesParameters");
	(this->UseSelectPhases ? selectingParams->SetAttribute("UseSelectPhases", "On") : selectingParams->SetAttribute("UseSelectPhases", "Off"));
	selectingParams->SetIntAttribute("NumSignalBoxPhases", this->NumSignalBoxPhases);
	selectingParams->SetVectorAttribute("MapPhaseToOutputVolume", this->NumSignalBoxPhases, this->MapPhaseToOutputVolume);
	elem->AddNestedElement(selectingParams);

	// save inserted timestamps
	//vtkXMLDataElement *saveParams = vtkXMLDataElement::New();
	//saveParams->SetName("SaveInserted");
	//(this->SaveInsertedTimestamps ? saveParams->SetAttribute("SaveInsertedTimestamps", "On") : saveParams->SetAttribute("SaveInsertedTimestamps", "Off"));
	//(this->SaveInsertedSlices ? saveParams->SetAttribute("SaveInsertedSlices", "On") : saveParams->SetAttribute("SaveInsertedSlices", "Off"));

	// buffering options
	vtkXMLDataElement *bufferOptions = elem->FindNestedElementWithName("BufferOptions");
	if (bufferOptions && this->SignalBox)
	{
		bufferOptions->SetDoubleAttribute("SignalBoxSleepInterval", this->SignalBox->GetSleepInterval());
	}

	// clean up
	triggeringParams->Delete();
	signalBoxParams->Delete();
	checkParams->Delete();
	selectingParams->Delete();
	//  saveParams->Delete();

	return elem;
}



//----------------------------------------------------------------------------
// Read the freehand parameters from the filename specified in the (relative!)
// directory
// File should have been created using SaveSummaryFile()
int vtkUltrasoundDataCollector::ReadSummaryFile(const char *filename)
{

	if (this->ReconstructionThreadId != -1)
	{
		return 0;
	}

	// read in the freehand information
	vtkXMLUtilities* util = vtkXMLUtilities::New();

	vtkXMLDataElement* elem = util->ReadElementFromFile(filename);

	// check to make sure we have the right element
	if (elem == NULL)
	{
		vtkErrorMacro(<< "ReadRawData - invalid file " << filename);
		util->Delete();
		return 0;
	}

	if (strcmp(elem->GetName(), "Freehand") != 0)
	{
		vtkErrorMacro(<< "ReadRawData - invalid file " << filename);
		elem->Delete();
		util->Delete();
		return 0;
	}

	// get the base information
	this->Superclass::ReadSummaryFile(filename);

	// triggering options
	vtkXMLDataElement* triggeringParams = elem->FindNestedElementWithName("TriggeringParameters");
	if (triggeringParams)
	{
		if (triggeringParams->GetAttribute("Triggering"))
		{
			((strcmp(triggeringParams->GetAttribute("Triggering"), "On") == 0) ? this->TriggeringOn() : this->TriggeringOff());
		}
		if (triggeringParams->GetAttribute("Gating"))
		{
			if (strcmp(triggeringParams->GetAttribute("Gating"), "Retrospective") == 0)
			{
				this->RetrospectiveOn();
			}
			else if (strcmp(triggeringParams->GetAttribute("Gating"), "Prospective") == 0)
			{
				this->RetrospectiveOff();
			}
		}
	}

	// signal box options
	vtkXMLDataElement* signalBoxParams = elem->FindNestedElementWithName("SignalBox");
	if (signalBoxParams)
	{
		if (signalBoxParams->GetAttribute("SignalBox"))
		{
			// this is set to NULL, meaning we didn't have a signal box
			// when saving to file, but if we have one here we can just
			// leave it because the triggering flags will let us decide
			// wheteher or not to trigger
		}
		else
		{
			int temp;
			// we should have a signal box, so make one if we don't have one
			if (this->SignalBox == NULL)
			{
				vtkSignalBox* signalBox = vtkSignalBox::New();
				if (signalBoxParams->GetScalarAttribute("NumPhases", temp))
				{
					signalBox->SetNumberOfPhases(temp);
				}
				this->SetSignalBox(signalBox);
			}
			// change the number of phases if we do have a signal box
			else
			{
				if (signalBoxParams->GetScalarAttribute("NumPhases", temp))
				{
					if (temp != this->SignalBox->GetNumberOfPhases())
					{
						this->SignalBox->SetNumberOfPhases(temp);
						this->SetSignalBox(this->SignalBox);
					}
				}
			}
		}
	}

	vtkXMLDataElement* bufferOptions = elem->FindNestedElementWithName("BufferOptions");
	if (bufferOptions)
	{
		if (this->SignalBox)
		{
			double tempd = this->SignalBox->GetSleepInterval();
			bufferOptions->GetScalarAttribute("SignalBoxSleepInterval", tempd);
			this->SignalBox->SetSleepInterval(tempd);
		}
	}

	//check heart rate parameters
	vtkXMLDataElement* checkParams = elem->FindNestedElementWithName("CheckHeartRateParameters");
	if (checkParams)
	{
		if (checkParams->GetAttribute("CheckHeartRate"))
		{
			((strcmp(checkParams->GetAttribute("CheckHeartRate"), "On") == 0) ? this->CheckHeartRateOn() : this->CheckHeartRateOff());  
		}
		checkParams->GetScalarAttribute("ECGMonitoringTime", this->ECGMonitoringTime);
		checkParams->GetScalarAttribute("NumECGTrials", this->NumECGTrials);
		checkParams->GetScalarAttribute("PercentIncreaseHRAllowed", this->PercentageIncreasedHeartRateAllowed);
		checkParams->GetScalarAttribute("PercentDecreaseHRAllowed", this->PercentageDecreasedHeartRateAllowed);
		checkParams->GetScalarAttribute("MeanHR", this->MeanHeartRate);

		this->MaxAllowedHeartRate = this->MeanHeartRate + (this->MeanHeartRate * this->PercentageIncreasedHeartRateAllowed / 100.0);
		this->MinAllowedHeartRate = this->MeanHeartRate - (this->MeanHeartRate * this->PercentageDecreasedHeartRateAllowed / 100.0);
	}

	// saving inserted timestamps/slices
	//vtkXMLDataElement *saveParams = elem->FindNestedElementWithName("SaveInserted");
	//if (saveParams)
	//  {
	//  if (saveParams->GetAttribute("SaveInsertedTimestamps"))
	//    {
	//    ((strcmp(saveParams->GetAttribute("SaveInsertedTimestamps"), "On") == 0) ? this->SaveInsertedTimestampsOn() : this->SaveInsertedTimestampsOff());
	//    }
	//  if (saveParams->GetAttribute("SaveInsertedSlices"))
	//    {
	//    ((strcmp(saveParams->GetAttribute("SaveInsertedSlices"), "On") == 0) ? this->SaveInsertedSlicesOn() : this->SaveInsertedSlicesOff());
	//    }
	//  }

	// using select phases
	vtkXMLDataElement* selectingParams = vtkXMLDataElement::New();
	selectingParams = elem->FindNestedElementWithName("UseSelectPhasesParameters");
	if (selectingParams)
	{
		selectingParams->GetScalarAttribute("NumSignalBoxPhases", this->NumSignalBoxPhases);
		selectingParams->GetVectorAttribute("MapPhaseToOutputVolume", this->NumSignalBoxPhases, this->MapPhaseToOutputVolume);
		if (selectingParams->GetAttribute("UseSelectPhases"))
		{
			((strcmp(selectingParams->GetAttribute("UseSelectPhases"), "On") == 0) ? this->UseSelectPhasesOn() : this->UseSelectPhasesOff());
		}
	}

	// clean up
	elem->Delete();
	util->Delete();

	return 1;

}

//int vtkUltrasoundDataCollector::SyncronizeUSToTracker()
//{
//
//	const     unsigned int   Dimension = 2;
//	typedef   unsigned char  PixelType;
//
//	typedef itk::Image< PixelType, Dimension >   ImageType;
//	typedef itk::VTKImageToImageFilter<ImageType> ConnectorType;
//
//	ConnectorType::Pointer fixedImgConnector= ConnectorType::New();
//	ConnectorType::Pointer movingImgConnector= ConnectorType::New();
//
//	typedef itk::MeanSquaresImageToImageMetric<ImageType, ImageType >  MetricType;
//
//	MetricType::Pointer metric = MetricType::New();
//
//	typedef itk::TranslationTransform< double, Dimension >  TransformType;
//	TransformType::Pointer transform = TransformType::New();
//
//	typedef itk::NearestNeighborInterpolateImageFunction<ImageType, double>  InterpolatorType;
//	InterpolatorType::Pointer interpolator = InterpolatorType::New();
//
//	//fixedImgConnector->SetInput(this->GetSlice()); 
//	vtkImageData* fixedVtkImg = vtkImageData::New(); 
//	fixedVtkImg->DeepCopy(this->GetSlice()); 
//	int ext[6]; 
//	fixedVtkImg->GetWholeExtent(ext); 
//
//	fixedImgConnector->SetInput(fixedVtkImg); 
//
//
//	ImageType::ConstPointer fixedImage  = fixedImgConnector->GetOutput();
//	ImageType::ConstPointer movingImage = movingImgConnector->GetOutput();
//
//	for (int i=0; i<100; i++)
//	{
//		movingImgConnector->SetInput(this->GetSlice()); 
//
//		transform->SetIdentity();
//
//		metric->SetTransform( transform );
//		metric->SetInterpolator( interpolator );
//
//		metric->SetFixedImage(  fixedImage  );
//		metric->SetMovingImage( movingImage );
//
//		metric->SetFixedImageRegion(  fixedImage->GetBufferedRegion());
//
//		try 
//		{
//			metric->Initialize();
//		}
//		catch( itk::ExceptionObject & excep )
//		{
//			std::cerr << "Exception catched !" << std::endl;
//			std::cerr << excep << std::endl;
//			return EXIT_FAILURE;
//		}
//
//		MetricType::TransformParametersType displacement( Dimension );
//
//		//const int rangex = 50;
//		//const int rangey = 50;
//
//		//for( int dx = -rangex; dx <= rangex; dx++ )
//		//{
//		//	for( int dy = -rangey; dy <= rangey; dy++ )
//		//	{
//		displacement[0] = 0;
//		displacement[1] = 0;
//		const double value = metric->GetValue(displacement );
//		std::cout << value << std::endl;
//		//	}
//		//}
//	}
//
//	return 0; 
//
//}

//----------------------------------------------------------------------------
// Does the actual work for GetIndexMatrix, to give a matrix that converts output
// pixel indices to input pixel indices
// Saves the result to this->IndexMatrix
// transform = tWorld2RAS * (sliceTransform * tProbe2World) * tFrame2Probe
vtkMatrix4x4 *vtkUltrasoundDataCollector::GetIndexMatrixHelper(vtkMatrix4x4* tProbe2World, vtkLinearTransform* sliceTransform)
{

	if (this->IndexMatrix == NULL)
	{
		this->IndexMatrix = vtkMatrix4x4::New();
	}

	// get the origin and spacing for the output volume
	vtkFloatingPointType outOrigin[3];
	vtkFloatingPointType outSpacing[3];

	this->GetOutput()->GetSpacing(outSpacing);
	this->GetOutput()->GetOrigin(outOrigin);

	// make the transformations we'll need
	vtkSmartPointer<vtkTransform> transform = vtkTransform::New();
	vtkSmartPointer<vtkTransform> tWorld2RAS = vtkTransform::New();

	// transform = tProbe2World * tFrame2Probe
	transform->SetMatrix(this->GetFrameToWorldMatrix(tProbe2World, sliceTransform)); 

	// transform = sliceTransform * tProbe2World * tFrame2Probe
	if (sliceTransform)
	{
		transform->PostMultiply();
		transform->Concatenate(sliceTransform->GetMatrix());
	}

	tWorld2RAS->PostMultiply(); 
	tWorld2RAS->Translate(-outOrigin[0], -outOrigin[1], -outOrigin[2]); 
	tWorld2RAS->Scale(1.0f/outSpacing[0], 1.0f/outSpacing[1], 1.0f/outSpacing[2]);

	// transform = tWorld2RAS * (sliceTransform * tProbe2World) * tFrame2Probe
	transform->PostMultiply();
	transform->Concatenate(tWorld2RAS->GetMatrix());

	// save the transform's matrix in this->IndexMatrix
	transform->GetMatrix(this->IndexMatrix);

	return this->IndexMatrix;
}

// transform = tProbe2World * tFrame2Probe
vtkMatrix4x4 *vtkUltrasoundDataCollector::GetFrameToWorldMatrix(vtkMatrix4x4* tProbe2World, vtkLinearTransform* sliceTransform)
{

	// get the origin and spacing for the input slice 
	vtkFloatingPointType inOrigin[3];
	vtkFloatingPointType inSpacing[3];

	this->GetSlice()->GetSpacing(inSpacing);
	this->GetSlice()->GetOrigin(inOrigin);

	// make the transformations we'll need
	vtkSmartPointer<vtkTransform> transform = vtkTransform::New();
	vtkSmartPointer<vtkTransform> tFrame2Probe = vtkTransform::New();

	// transform = tProbe2World
	if (tProbe2World)
	{
		transform->SetMatrix(tProbe2World);
	}

	tFrame2Probe->PreMultiply();
	tFrame2Probe->Translate(inOrigin); 
	tFrame2Probe->Scale(inSpacing); 

	// transform = tProbe2World * tFrame2Probe
	transform->PreMultiply();
	transform->Concatenate(tFrame2Probe->GetMatrix());

	return transform->GetMatrix();
}




