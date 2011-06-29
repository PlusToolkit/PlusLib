#ifndef __TrackedUltrasoundCapturing_h
#define __TrackedUltrasoundCapturing_h

#include "PlusConfigure.h"
#include "vtkObject.h"
#include "vtkDataCollector.h"
#include "vtkImageActor.h" 
#include "vtkRenderer.h"
#include "vtkMultiThreader.h"
#include "vtkCriticalSection.h"
#include "itkImage.h"

class vtkTrackedFrameList; 

class TrackedUltrasoundCapturing : public vtkObject
{

public:
	static TrackedUltrasoundCapturing *New();
		// Description:
	// Return the singleton instance with no reference counting.
	static TrackedUltrasoundCapturing* GetInstance();

	// Description:
	// Supply a user defined output window. Call ->Delete() on the supplied
	// instance after setting it.
	static void SetInstance(TrackedUltrasoundCapturing *instance);

	// Description:

	vtkTypeRevisionMacro(TrackedUltrasoundCapturing, vtkObject);

	// Description:

	virtual void PrintSelf(ostream& os, vtkIndent indent); 

	typedef unsigned char PixelType;
	typedef itk::Image< PixelType, 2 > ImageType;

	// Description:	
	virtual void Initialize(); 
	
	// Description:
	virtual void SaveData(); 

	// Description:
	virtual void SaveAsData( const char* filePath ); 

	// Description:
	virtual void StartRecording(); 

	// Description:
	virtual void StopRecording(); 

	// Description:
	virtual PlusStatus UpdateRecording(); 

	// Description:
	virtual PlusStatus RecordTrackedFrame(const double timestamp = 0); 
	
	// Description:
	virtual void AddTrackedFrame( TrackedFrame* trackedFrame ); 

	// Description:
	// Clear frames from tracked frame container
	virtual void ClearTrackedFrameContainer(); 
	
	// Description:
	virtual void SynchronizeFrameToTracker(); 

	// Description:
	virtual double GetVideoOffsetMs(); 

	// Description:
	virtual void SetLocalTimeOffset(double videoOffset, double trackerOffset); 
	
	// Description:
	virtual int GetNumberOfRecordedFrames(); 

	// Description:
	virtual double GetLastRecordedFrameTimestamp(); 

	// Description:
	vtkSetStringMacro(ImageSequenceFileName); 
	vtkGetStringMacro(ImageSequenceFileName); 

	// Description:
	vtkSetStringMacro(InputConfigFileName); 
	vtkGetStringMacro(InputConfigFileName); 
	
	// Description:
	vtkSetStringMacro(OutputFolder); 
	vtkGetStringMacro(OutputFolder); 

	// Description:
	vtkSetMacro(RecordingStartTime, double); 
	vtkGetMacro(RecordingStartTime, double); 

	// Description:	
	vtkSetMacro(FrameRate, int); 
	vtkGetMacro(FrameRate, int);

	// Description:	
	vtkGetObjectMacro(DataCollector, vtkDataCollector);
	vtkSetObjectMacro(DataCollector, vtkDataCollector);
			
	// Description:	
	vtkGetObjectMacro(RealtimeImageActor, vtkImageActor);
	vtkSetObjectMacro(RealtimeImageActor, vtkImageActor);

	// Description:	
	vtkGetObjectMacro(RealtimeRenderer, vtkRenderer);
	vtkSetObjectMacro(RealtimeRenderer, vtkRenderer);

	// Description:
	// Set/get the synchronizing flag
	vtkSetMacro(Synchronizing, bool); 
	vtkGetMacro(Synchronizing, bool); 
	vtkBooleanMacro(Synchronizing, bool); 

	// Description:
	// Set/get the recording flag
	vtkSetMacro(Recording, bool); 
	vtkGetMacro(Recording, bool); 
	vtkBooleanMacro(Recording, bool); 
	
	//! Description 
	//! Callback function that is executed each time an update requested
	typedef void (*UpdateRequestPtr)(void);
    void SetUpdateRequestCallbackFunction(UpdateRequestPtr cb) { UpdateRequestCallbackFunction = cb; } 

	//! Description 
	// Callback function for progress bar refreshing
	typedef void (*ProgressBarUpdatePtr)(int percent);
    void SetProgressBarUpdateCallbackFunction(ProgressBarUpdatePtr cb) { ProgressBarUpdateCallbackFunction = cb; } 

protected: 
	TrackedUltrasoundCapturing();
	virtual ~TrackedUltrasoundCapturing();
	
	vtkDataCollector* DataCollector; 
	vtkRenderer* RealtimeRenderer; 
	vtkImageActor* RealtimeImageActor; 

	vtkTrackedFrameList* TrackedFrameContainer;

	char* InputConfigFileName;
	char* ImageSequenceFileName; 
	char* OutputFolder; 
	bool Recording;
	bool Synchronizing; 

	int FrameRate; 
	double RecordingStartTime; 

	//! Pointer to the callback function that is executed each time an update requested
    UpdateRequestPtr UpdateRequestCallbackFunction;

	//! Pointer to the progress bar update callback function 
	ProgressBarUpdatePtr ProgressBarUpdateCallbackFunction; 
	
private:
	static TrackedUltrasoundCapturing* Instance;
	TrackedUltrasoundCapturing(const TrackedUltrasoundCapturing&);
	void operator=(const TrackedUltrasoundCapturing&);

}; 

#endif  