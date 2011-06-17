#ifndef __VTKTRACKEDFRAMELIST_H
#define __VTKTRACKEDFRAMELIST_H

#include "vtkObject.h"
#include <deque>

#include "itkImage.h"
#include "vtkMatrix4x4.h"
#include "vtkTransform.h"


//----------------------------------------------------------------------------
// ************************* TrackedFrame ************************************
//----------------------------------------------------------------------------
class VTK_EXPORT TrackedFrame
{
public:
	
	enum US_IMAGE_ORIENTATION
	{
		US_IMG_ORIENT_UF, // image x axis = unmarked transducer axis, image y axis = far transducer axis
		US_IMG_ORIENT_UN, // image x axis = unmarked transducer axis, image y axis = near transducer axis
		US_IMG_ORIENT_MF, // image x axis = marked transducer axis, image y axis = far transducer axis
		US_IMG_ORIENT_MN, // image x axis = marked transducer axis, image y axis = near transducer axis
		US_IMG_ORIENT_XX  // undefined
	}; 

	typedef unsigned char PixelType;
	typedef itk::Image< PixelType, 2 > ImageType;
	// <CustomFrameFieldName, CustomFrameFieldValue>
	typedef std::pair<std::string, std::string> CustomFrameFieldPair; 
	// <CustomFieldName, CustomFieldValue>
	typedef std::pair<std::string, std::string> CustomFieldPair; 

	TrackedFrame(); 
	~TrackedFrame(); 
	TrackedFrame(const TrackedFrame& frame); 

	//! Operation: 
	// Set custom frame field
	void SetCustomFrameField( std::string name, std::string value ); 

	//! Operation: 
	// Get custom frame field value
	const char* GetCustomFrameField( std::string name ); 

	//! Operation: 
	// Set custom field 
	void SetCustomField( std::string name, std::string value ); 

	//! Operation: 
	// Get custom field value
	const char* GetCustomField( std::string name ); 

	//! Operation: 
	// Get default frame transform
	bool GetDefaultFrameTransform(double transform[16]); 

	//! Operation: 
	// Get custom frame transform
	bool GetCustomFrameTransform(const char* frameTransformName, double transform[16]); 

	//! Operation: 
	// Set custom frame transform
	void SetCustomFrameTransform(std::string frameTransformName, double transform[16]); 

	//! Operation: 
	// Set custom frame transform
	void SetCustomFrameTransform(std::string frameTransformName, vtkMatrix4x4* transform); 

	//! Operation: 
	// Get US_IMAGE_ORIENTATION enum value from string 
	static US_IMAGE_ORIENTATION GetUsImageOrientationFromString( const char* usImgOrientation ); 

	//! Operation: 
	// Set/get ultrasound image orientation 
	// The ultrasound image axes are defined as follows:
	// - x axis: points towards the x coordinate increase direction
	// - y axis: points towards the y coordinate increase direction
	// The image orientation can be defined by specifying which transducer axis corresponds to the x and y image axes, respectively.
	// There are four possible orientations:
	// - UF: image x axis = unmarked transducer axis, image y axis = far transducer axis
	// - UN: image x axis = unmarked transducer axis, image y axis = near transducer axis
	// - MF: image x axis = marked transducer axis, image y axis = far transducer axis
	// - MN: image x axis = marked transducer axis, image y axis = near transducer axis
	const char* GetUltrasoundImageOrientation(); 
	void SetUltrasoundImageOrientation(const char* usImgOrientation); 

	//! Operation: 
	// Get oriented tracked ultrasound image 
	// User should call UnRegister() on returned image after this call 
	ImageType* GetOrientedImage( US_IMAGE_ORIENTATION usImageOrientation ); 
	ImageType* GetOrientedImage( const char* usImageOrientation ); 

	//! Operation: 
	// Get tracked frame size in pixel
	int* GetFrameSize(); 

	bool operator< (TrackedFrame data) { return Timestamp < data.Timestamp; }
	bool operator== (const TrackedFrame& data) const 
	{
		return this->Timestamp == data.Timestamp; 
	}

	US_IMAGE_ORIENTATION UltrasoundImageOrientation; 
	std::string DefaultFrameTransformName; 
	double Timestamp; 
	long Status; 
	ImageType* ImageData;

	std::vector<CustomFrameFieldPair> CustomFrameFieldList; 
	std::vector<CustomFieldPair> CustomFieldList; 
	int FrameSize[3]; 
};



//----------------------------------------------------------------------------
// ************************* vtkTrackedFrameList *****************************
//----------------------------------------------------------------------------


class TrackedFrameTimestampFinder
{	
public:
    TrackedFrameTimestampFinder(TrackedFrame* frame): mTrackedFrame(frame){}; 	
    bool operator()( TrackedFrame *newFrame )	
    {		
        return newFrame->Timestamp == mTrackedFrame->Timestamp;	
    }	
    TrackedFrame* mTrackedFrame;
};

class TrackedFramePositionFinder
{	
public:
    TrackedFramePositionFinder(TrackedFrame* frame, char* frameTransformName = NULL, double translationDistanceThresholdMm = 0.5, double rotationAngleThresholdDeg = 0.2)
		: mTrackedFrame(frame), 
		mRotationAngleThresholdDeg(rotationAngleThresholdDeg),
		mTranslationDistanceThresholdMm(translationDistanceThresholdMm)
	{
		if ( frameTransformName == NULL ) 
		{
			mFrameTransformName = frame->DefaultFrameTransformName; 
		}
		else
		{
			mFrameTransformName = frameTransformName; 
		}
	}
	
    bool operator()( TrackedFrame *newFrame )	
    {		
		vtkSmartPointer<vtkTransform> baseTransform = vtkSmartPointer<vtkTransform>::New(); 
		double baseTransMatrix[16]={0}; 
		if ( mTrackedFrame->GetCustomFrameTransform(mFrameTransformName.c_str(), baseTransMatrix) )
		{
			baseTransform->SetMatrix(baseTransMatrix); 
		}
		else
		{
			LOG_ERROR("TrackedFramePositionFinder: Unable to find base frame transform name for tracked frame validation!"); 
			return false; 
		}

		vtkSmartPointer<vtkTransform> newTransform = vtkSmartPointer<vtkTransform>::New(); 
		double newTransMatrix[16]={0}; 
		if ( newFrame->GetCustomFrameTransform(mFrameTransformName.c_str(), newTransMatrix) )
		{
			newTransform->SetMatrix(newTransMatrix); 
		}
		else
		{
			LOG_ERROR("TrackedFramePositionFinder: Unable to find frame transform name for new tracked frame validation!"); 
			return false; 
		}

		double bx(0), by(0), bz(0), cx(0), cy(0), cz(0); 
		if ( STRCASECMP(mFrameTransformName.c_str(), "StepperEncoderValues") == 0 )
		{
			// Probe positions are stored in matrix element (0,3)
			// Template positions are stored in matrix element (2,3)
			// Image position = probe position + grid position 
			bz = baseTransform->GetMatrix()->GetElement(0,3) + baseTransform->GetMatrix()->GetElement(2,3); 
			cz = newTransform->GetMatrix()->GetElement(0,3) + newTransform->GetMatrix()->GetElement(2,3);
		}
		else
		{
			bx = baseTransform->GetPosition()[0]; 
			by = baseTransform->GetPosition()[1]; 
			bz = baseTransform->GetPosition()[2]; 
			cx = newTransform->GetPosition()[0]; 
			cy = newTransform->GetPosition()[1]; 
			cz = newTransform->GetPosition()[2]; 
		}

		// Euclidean distance
		double distance = sqrt( pow(bx-cx,2) + pow(by-cy,2) + pow(bz-cz,2) ); 

		vtkSmartPointer<vtkMatrix4x4> diffTransMatrix = vtkSmartPointer<vtkMatrix4x4>::New(); 
		vtkSmartPointer<vtkMatrix4x4> invNewTransMatrix = vtkSmartPointer<vtkMatrix4x4>::New(); 
		vtkMatrix4x4::Invert(newTransform->GetMatrix(), invNewTransMatrix);  

		vtkMatrix4x4::Multiply4x4(baseTransform->GetMatrix(), invNewTransMatrix, diffTransMatrix); 
		vtkSmartPointer<vtkTransform> diffTransform = vtkSmartPointer<vtkTransform>::New(); 
		diffTransform->SetMatrix(diffTransMatrix); 

		double angleDifference(0); 
		if ( STRCASECMP(mFrameTransformName.c_str(), "StepperEncoderValues") == 0 )
		{
			// Rotation angles are stored in matrix element (1,3)
			angleDifference = baseTransform->GetMatrix()->GetElement(1,3) - newTransform->GetMatrix()->GetElement(1,3); 
		}
		else
		{
			angleDifference = diffTransform->GetOrientationWXYZ()[0]; 
		}
		
		if ( distance < this->mTranslationDistanceThresholdMm && abs(angleDifference) < this->mRotationAngleThresholdDeg )
		{
			return true; 
		}

		return false; 
	}	

	TrackedFrame* mTrackedFrame;
	double mRotationAngleThresholdDeg; 
	double mTranslationDistanceThresholdMm; 
	std::string mFrameTransformName;  

};

class VTK_EXPORT vtkTrackedFrameList : public vtkObject
{

public:
    typedef std::deque<TrackedFrame*> TrackedFrameListType; 
	enum SEQ_METAFILE_EXTENSION
	{
		SEQ_METAFILE_MHA, 
		SEQ_METAFILE_MHD
	}; 

	static vtkTrackedFrameList *New();
	vtkTypeRevisionMacro(vtkTrackedFrameList, vtkObject);
	virtual void PrintSelf(ostream& os, vtkIndent indent); 

	//! Operation: 
	// Add tracked frame to container and returns with the position in the vector
	virtual int AddTrackedFrame(TrackedFrame *trackedFrame); 

	//! Operation: 
	// Get tracked frame from container
	virtual TrackedFrame* GetTrackedFrame(int frameNumber); 

	//! Operation: 
	// Get number of tracked frames
	virtual unsigned int GetNumberOfTrackedFrames() { return this->TrackedFrameList.size(); } 

	//! Operation: 
	// Save the tracked data to sequence metafile 
	virtual void SaveToSequenceMetafile(const char* outputFolder, const char* sequenceDataFileName, SEQ_METAFILE_EXTENSION extension = SEQ_METAFILE_MHA, bool useCompression = true, const char* usImageOrientation = NULL);

	//! Operation: 
	// Read the tracked data from sequence metafile 
	virtual PlusStatus ReadFromSequenceMetafile(const char* trackedSequenceDataFileName); 

	//! Operation: 
	// Get the tracked frame list 
	TrackedFrameListType GetTrackedFrameList() { return this->TrackedFrameList; }

	//! Operation: 
	// Validate tracked frame before adding to the list.
	// Control arguments: 
	// validateTimestamp: the timestamp should be unique 
	// validateStatus: the tracking flags should be valid (TR_OK)
	// validatePosition: the frame position should be different from the previous ones 
	// validateSpeed: the frame acquisition speed should be less than a threshold
	virtual bool ValidateData(TrackedFrame* trackedFrame, bool validateTimestamp = true, bool validateStatus = true, bool validatePosition = true, char* frameTransformName = NULL, bool validateSpeed = true); 

	//! Operation: 
	// Clear tracked frame list and free memory
	virtual void Clear(); 

	virtual std::string GetDefaultFrameTransformName(); 

	//! Operation: 
	// Set/get the maximum number of frames to write into a single metafile. 
	vtkSetMacro(MaxNumOfFramesToWrite, int); 
	vtkGetMacro(MaxNumOfFramesToWrite, int); 

	//! Operation: 
	// Set/get the number of following unique frames needed in the tracked frame list
	vtkSetMacro(NumberOfUniqueFrames, int); 
	vtkGetMacro(NumberOfUniqueFrames, int); 
	
	//! Operation: 
	// Get tracked frame size in pixel
	virtual int* GetFrameSize(); 

protected:
	vtkTrackedFrameList();
	virtual ~vtkTrackedFrameList();

	//! Operation: 
	// Set tracked frame size in pixel
	vtkSetVector3Macro(FrameSize, int); 

	bool ValidateTimestamp(TrackedFrame* trackedFrame); 
	bool ValidateStatus(TrackedFrame* trackedFrame); 
	bool ValidatePosition(TrackedFrame* trackedFrame, char* frameTransformName); 

	TrackedFrameListType TrackedFrameList; 

	int MaxNumOfFramesToWrite; 
	int NumberOfUniqueFrames; 
	int FrameSize[3]; 

private:
	vtkTrackedFrameList(const vtkTrackedFrameList&);
	void operator=(const vtkTrackedFrameList&);
}; 

#endif // __VTKTRACKEDFRAMELIST_H


