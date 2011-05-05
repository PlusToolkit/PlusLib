#ifndef __VTKTRACKEDFRAMELIST_H
#define __VTKTRACKEDFRAMELIST_H

#include "vtkObject.h"
#include <vector>

#include "itkImage.h"
#include "vtkMatrix4x4.h"
#include "vtkTransform.h"

class VTK_EXPORT TrackedFrame
{

public: 
	typedef unsigned char PixelType;
	typedef itk::Image< PixelType, 2 > ImageType;
	// <CustomFrameFieldName, CustomFrameFieldValue>
	typedef std::pair<std::string, std::string> CustomFrameFieldPair; 
	// <CustomFieldName, CustomFieldValue>
	typedef std::pair<std::string, std::string> CustomFieldPair; 

	TrackedFrame()
	{
		this->Status = 0; 
		this->Timestamp = 0; 
		this->ImageData = NULL; 
	}

	~TrackedFrame()
	{
		if ( this->ImageData != NULL )
		{
			this->ImageData->UnRegister(); 
		}
	}

	TrackedFrame(const TrackedFrame& frame)
	{
		this->DefaultFrameTransformName = frame.DefaultFrameTransformName; 
		this->CustomFrameFieldList = frame.CustomFrameFieldList; 
		this->CustomFieldList = frame.CustomFieldList; 
		this->ImageData = frame.ImageData; 
		this->Timestamp = frame.Timestamp;
		this->Status = frame.Status; 
		if ( this->ImageData != NULL )
		{
			this->ImageData->Register(); 
		}
	}

	//! Operation: 
	// Set custom frame field
	void SetCustomFrameField( std::string name, std::string value )
	{
		CustomFrameFieldPair pair(name, value); 
		this->CustomFrameFieldList.push_back(pair); 
	}

	//! Operation: 
	// Get custom frame field value
	const char* GetCustomFrameField( std::string name )
	{
		std::vector<CustomFrameFieldPair>::iterator customFrameValue; 
		for ( customFrameValue = this->CustomFrameFieldList.begin(); customFrameValue != this->CustomFrameFieldList.end(); customFrameValue++ )
		{
			if ( customFrameValue->first.find(name) != std::string::npos )
			{ 
				return customFrameValue->second.c_str(); 
			}
		}

		return NULL; 
	}

	//! Operation: 
	// Set custom field 
	void SetCustomField( std::string name, std::string value )
	{
		CustomFieldPair pair(name, value); 
		this->CustomFieldList.push_back(pair); 
	}

	//! Operation: 
	// Get custom field value
	const char* GetCustomField( std::string name )
	{
		std::vector<CustomFieldPair>::iterator customValue; 
		for ( customValue = this->CustomFieldList.begin(); customValue != this->CustomFieldList.end(); customValue++ )
		{
			if ( customValue->first.find(name) != std::string::npos )
			{ 
				return customValue->second.c_str(); 
			}
		}

		return NULL; 
	}


	//! Operation: 
	// Get default frame transform
	bool GetDefaultFrameTransform(double transform[16]) 
	{
		// Find default frame transform 
		std::vector<CustomFrameFieldPair>::iterator defaultFrameTransform; 
		for ( defaultFrameTransform = this->CustomFrameFieldList.begin(); defaultFrameTransform != this->CustomFrameFieldList.end(); defaultFrameTransform++ )
		{
			if ( defaultFrameTransform->first.find(this->DefaultFrameTransformName) != std::string::npos )
			{
				std::istringstream transformFieldValue(defaultFrameTransform->second); 
				double item; 
				int i = 0; 
				while ( transformFieldValue >> item )
				{
					if ( i < 16 )
						transform[i++] = item; 
				}
				return true;
			}
		}

		LOG_ERROR("Unable to find default transform in sequence metafile!"); 
		return false; 
	}

	//! Operation: 
	// Get custom frame transform
	bool GetCustomFrameTransform(const char* frameTransformName, double transform[16]) 
	{
		// Find default frame transform 
		std::vector<CustomFrameFieldPair>::iterator customFrameTransform; 
		for ( customFrameTransform = this->CustomFrameFieldList.begin(); customFrameTransform != this->CustomFrameFieldList.end(); customFrameTransform++ )
		{
			if ( customFrameTransform->first.find(frameTransformName) != std::string::npos )
			{
				std::istringstream transformFieldValue(customFrameTransform->second); 
				double item; 
				int i = 0; 
				while ( transformFieldValue >> item )
				{
					if ( i < 16 )
						transform[i++] = item; 
				}
				return true;
			}
		}

		LOG_ERROR("Unable to find custom transform (" << frameTransformName << ") in sequence metafile!"); 
		return false; 
	}


	//! Operation: 
	// Set custom frame transform
	void SetCustomFrameTransform(std::string frameTransformName, double transform[16]) 
	{
		std::ostringstream strTransform; 
		for ( int i = 0; i < 16; ++ i )
		{
			strTransform << transform[ i ] << " ";
		}

		CustomFrameFieldPair pair(frameTransformName, strTransform.str()); 
		this->CustomFrameFieldList.push_back(pair); 
	}

	//! Operation: 
	// Set custom frame transform
	void SetCustomFrameTransform(std::string frameTransformName, vtkMatrix4x4* transform) 
	{
		double dTransform[ 16 ];
		vtkMatrix4x4::DeepCopy( dTransform, transform );
		this->SetCustomFrameTransform(frameTransformName, dTransform); 
	}

	bool operator< (TrackedFrame data) { return Timestamp < data.Timestamp; }
	bool operator== (const TrackedFrame& data) const 
	{
		return this->Timestamp == data.Timestamp; 
	}

	std::string DefaultFrameTransformName; 
	std::vector<CustomFrameFieldPair> CustomFrameFieldList; 
	std::vector<CustomFieldPair> CustomFieldList; 
	ImageType* ImageData;
	double Timestamp; 
	long Status; 
};

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
		double baseTransMatrix[16]; 
		if ( mTrackedFrame->GetCustomFrameTransform(mFrameTransformName.c_str(), baseTransMatrix) )
		{
			baseTransform->SetMatrix(baseTransMatrix); 
		}

		vtkSmartPointer<vtkTransform> newTransform = vtkSmartPointer<vtkTransform>::New(); 
		double newTransMatrix[16]; 
		if ( newFrame->GetCustomFrameTransform(mFrameTransformName.c_str(), newTransMatrix) )
		{
			newTransform->SetMatrix(newTransMatrix); 
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
	virtual void SaveToSequenceMetafile(const char* outputFolder, const char* sequenceDataFileName, SEQ_METAFILE_EXTENSION extension /*=SEQ_METAFILE_MHA*/ , bool useCompression /*=true*/);

	//! Operation: 
	// Read the tracked data from sequence metafile 
	virtual void ReadFromSequenceMetafile(const char* trackedSequenceDataFileName); 

	//! Operation: 
	// Get the tracked frame list 
	std::vector<TrackedFrame*> GetTrackedFrameList() { return this->TrackedFrameList; }

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
	


protected:
	vtkTrackedFrameList();
	virtual ~vtkTrackedFrameList();

	bool ValidateTimestamp(TrackedFrame* trackedFrame); 
	bool ValidateStatus(TrackedFrame* trackedFrame); 
	bool ValidatePosition(TrackedFrame* trackedFrame, char* frameTransformName); 

	std::vector<TrackedFrame*> TrackedFrameList; 

	int MaxNumOfFramesToWrite; 
	int NumberOfUniqueFrames; 

private:
	vtkTrackedFrameList(const vtkTrackedFrameList&);
	void operator=(const vtkTrackedFrameList&);
}; 

#endif // __VTKTRACKEDFRAMELIST_H


