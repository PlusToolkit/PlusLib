#ifndef __VTKCALIBRATIONCONTROLLER_H
#define __VTKCALIBRATIONCONTROLLER_H

#include "vtkObject.h"
#include "vtkTrackedFrameList.h"
#include "vtkCalibratorVisualizationComponent.h"
#include "vtkMatrix4x4.h"
#include "vtkImageData.h"
#include "vtkXMLUtilities.h"
#include "itkImage.h"
#include "vnl/vnl_matrix.h"
#include "vnl/vnl_vector.h"
#include <string>
#include <vector>
#include "UltraSoundFiducialSegmentation.h"

enum IMAGE_DATA_TYPE
{
	TEMPLATE_TRANSLATION=0,		// Pure template translation dataset
	PROBE_TRANSLATION,			// Pure translation dataset
	PROBE_ROTATION,				// Pure rotation dataset in different stepper positions
	RANDOM_STEPPER_MOTION_1,	// Calibration dataset
	RANDOM_STEPPER_MOTION_2,	// Validation dataset
	FREEHAND_MOTION_1,			// Calibration dataset for the freehand probe calibration
	FREEHAND_MOTION_2,			// Validation dataset for the freehand probe calibration
	NUMBER_OF_IMAGE_DATA_TYPES
}; 

enum CALIBRATION_MODE
{
	REALTIME, 
	OFFLINE
}; 

//! Description 
// Helper class for storing segmentation results with transformation
class SegmentedFrame
{
public: 
	SegmentedFrame()
	{
		this->TrackedFrameInfo = NULL; 
		DataType = TEMPLATE_TRANSLATION;
	}

	TrackedFrame* TrackedFrameInfo; 
	SegmentationResults SegResults;
	IMAGE_DATA_TYPE DataType; 
};


class vtkCalibrationController : public vtkObject
{
public:
	typedef unsigned char PixelType;
	typedef itk::Image< PixelType, 2 > ImageType;
	typedef std::vector<SegmentedFrame> SegmentedFrameList;

	//! Description 
	// Helper structure for storing saved dataset info
	struct SavedImageDataInfo
	{
		std::string SequenceMetaFileName;
		int NumberOfImagesToUse; 
		int StartingIndex; 
	};

	//! Description 
	// Helper structure for storing realtime dataset info
	struct RealtimeImageDataInfo
	{
		std::string OutputSequenceMetaFileSuffix;
		int NumberOfImagesToAcquire; 
		int NumberOfSegmentedImages; 
	};

	static vtkCalibrationController *New();
	vtkTypeRevisionMacro(vtkCalibrationController, vtkObject);
	virtual void PrintSelf(ostream& os, vtkIndent indent); 

	//! Description 
	// Initialize the calibration controller interface
	virtual PlusStatus Initialize(); 

	//! Description 
	// Read XML based configuration of the calibration controller
	virtual PlusStatus ReadConfiguration( const char* configFileNameWithPath ); 
	virtual PlusStatus ReadConfiguration( vtkXMLDataElement* configData ); 

	//! Description 
	// Add new tracked data for segmentation and save the segmentation result to the SegmentedFrameContainer
	// The class has to be initialized before the segmentation process. 
	virtual PlusStatus AddVtkImageData( vtkImageData* frame, vtkMatrix4x4* trackingTransform, IMAGE_DATA_TYPE dataType ); 

	//! Description 
	// Add new tracked data for segmentation and save the segmentation result to the SegmentedFrameContainer
	// The class has to be initialized before the segmentation process. 
	virtual PlusStatus AddItkImageData( const ImageType::Pointer& frame, vtkMatrix4x4* trackingTransform, IMAGE_DATA_TYPE dataType );

	//! Description 
	// Add new tracked data for segmentation and save the segmentation result to the SegmentedFrameContainer
	// The class has to be initialized before the segmentation process. 
	virtual PlusStatus AddTrackedFrameData( TrackedFrame* trackedFrame, IMAGE_DATA_TYPE dataType ); 

	//! Description 
	// VTK/VNL matrix conversion 
	static void ConvertVnlMatrixToVtkMatrix(vnl_matrix<double>& inVnlMatrix, vtkMatrix4x4* outVtkMatrix); 
	static void ConvertVtkMatrixToVnlMatrix(vtkMatrix4x4* inVtkMatrix, vnl_matrix<double>& outVnlMatrix ); 
	static void ConvertVtkMatrixToVnlMatrixInMeter(vtkMatrix4x4* inVtkMatrix, vnl_matrix<double>& outVnlMatrix ); 

	//! Description 
	// Returns the list of tracked frames of the selected data type
	virtual vtkTrackedFrameList* GetTrackedFrameList( IMAGE_DATA_TYPE dataType ); 
	
	//! Description 
	// Save the selected data type to sequence metafile 
	virtual void SaveTrackedFrameListToMetafile( IMAGE_DATA_TYPE dataType, const char* outputFolder, const char* sequenceMetafileName, bool useCompression = false ); 

	//! Description 
	// Flag to show the initialized state
	vtkGetMacro(Initialized, bool);
	vtkSetMacro(Initialized, bool);
	vtkBooleanMacro(Initialized, bool);

	//! Description 
	// Flag to enable the tracked sequence data saving to metafile
	vtkGetMacro(EnableTrackedSequenceDataSaving, bool);
	vtkSetMacro(EnableTrackedSequenceDataSaving, bool);
	vtkBooleanMacro(EnableTrackedSequenceDataSaving, bool);

	//! Attribute: Flag to enable the erroneously segmented data saving to metafile
	vtkGetMacro(EnableErroneouslySegmentedDataSaving, bool);
	vtkSetMacro(EnableErroneouslySegmentedDataSaving, bool);
	vtkBooleanMacro(EnableErroneouslySegmentedDataSaving, bool);

	//! Attribute: Flag to enable the visualization component
	vtkGetMacro(EnableVisualization, bool);
	vtkSetMacro(EnableVisualization, bool);
	vtkBooleanMacro(EnableVisualization, bool);

	//! Description 
	// The input image dimensions (in pixels)
	vtkGetMacro(ImageWidthInPixels, int);
	vtkSetMacro(ImageWidthInPixels, int);

	//! Description 
	// The input image dimensions (in pixels)
	vtkGetMacro(ImageHeightInPixels, int);
	vtkSetMacro(ImageHeightInPixels, int);

	//! Description 
	// Segmentation search region
	// These define the region within the image where we should search for 
	// the dots. Outside of this region the image data must be null. Around 
	// all sides of this region there must be at least 8 pixels of this null 
	// space (searchOrigin >= 8 and searchSize <= imageSize-16). 
	vtkGetMacro(SearchStartAtX, int);
	vtkSetMacro(SearchStartAtX, int);
	vtkGetMacro(SearchStartAtY, int);
	vtkSetMacro(SearchStartAtY, int);
	vtkGetMacro(SearchDimensionX, int);
	vtkSetMacro(SearchDimensionX, int);
	vtkGetMacro(SearchDimensionY, int);
	vtkSetMacro(SearchDimensionY, int);

	//! Description 
	// Set/get the calibration mode (see CALIBRATION_MODE)
	vtkGetMacro(CalibrationMode, int);
	vtkSetMacro(CalibrationMode, int);

	//! Attribute: Path to output calibration results
	vtkGetStringMacro(OutputPath);
	vtkSetStringMacro(OutputPath);

	//! Attribute: Path to program parent folder
	vtkGetStringMacro(ProgramFolderPath);
	vtkSetStringMacro(ProgramFolderPath);

	//! Description 
	// Set/get the configuration file name
	vtkSetStringMacro(ConfigurationFileName); 
	vtkGetStringMacro(ConfigurationFileName); 

	//! Description 
	// Set/get the phantom definition file name
	vtkSetStringMacro(PhantomDefinitionFileName); 
	vtkGetStringMacro(PhantomDefinitionFileName); 

	//! Attribute: Flag to enable the Segmentation Analysis
	vtkGetMacro(EnableSegmentationAnalysis, bool);
	vtkSetMacro(EnableSegmentationAnalysis, bool);
	vtkBooleanMacro(EnableSegmentationAnalysis, bool);

	//! Description 
	// Get segmentation parameters
	SegmentationParameters* GetSegParameters() { return this->SegParameters; };

	//! Description 
	// Returns the segmenter class 
	KPhantomSeg* GetSegmenter() { return this->mptrAutomatedSegmentation; }

	//! Description 
	// Get the segmentation result container 
	// Stores the segmentation results with transformation for each frame
	SegmentedFrameList GetSegmentedFrameContainer() { return this->SegmentedFrameContainer; }

	//! Return the visualization component
	vtkCalibratorVisualizationComponent* GetVisualizationComponent() { return this->VisualizationComponent; }
	virtual void SetVisualizationComponent( vtkCalibratorVisualizationComponent* visualizationComponent) { this->VisualizationComponent = visualizationComponent; } 

	//! Description 
	// Get/set the saved image data info
	SavedImageDataInfo GetSavedImageDataInfo( IMAGE_DATA_TYPE dataType ) { return this->SavedImageDataInfoContainer[dataType]; }
	virtual void SetSavedImageDataInfo( IMAGE_DATA_TYPE dataType, SavedImageDataInfo savedImageDataInfo ) { this->SavedImageDataInfoContainer[dataType] = savedImageDataInfo; }
	
	//! Description 
	// Get/set the saved image data info
	RealtimeImageDataInfo GetRealtimeImageDataInfo( IMAGE_DATA_TYPE dataType ) { return this->RealtimeImageDataInfoContainer[dataType]; }
	virtual void SetRealtimeImageDataInfo( IMAGE_DATA_TYPE dataType, RealtimeImageDataInfo realtimeImageDataInfo ) { this->RealtimeImageDataInfoContainer[dataType] = realtimeImageDataInfo; }
	
	//! Description 
	// Callback function that is executed each time a segmentation is finished
	typedef void (*SegmentationProgressPtr)(IMAGE_DATA_TYPE dataType);
    void SetSegmentationProgressCallbackFunction(SegmentationProgressPtr cb) { SegmentationProgressCallbackFunction = cb; } 

protected:

	vtkCalibrationController();
	virtual ~vtkCalibrationController();

	//! Description 
	// Run the segmentation algorithm on the image and return with the segmentation result 
	// The class has to be initialized before the segmentation process. 
	virtual PlusStatus SegmentImage(const ImageType::Pointer& imageData, SegmentationResults& segResult); 
	virtual PlusStatus SegmentImage(vtkImageData * imageData, SegmentationResults& segResult); 

	//! Description 
	// Create tracked frame with the inputs specified
	virtual void CreateTrackedFrame(const ImageType::Pointer& imageData, vtkMatrix4x4* transform, IMAGE_DATA_TYPE dataType, TrackedFrame& trackedFrame ); 
	virtual void CreateTrackedFrame(const ImageType::Pointer& imageData, const double probePosition, const double probeRotation, const double templatePosition, IMAGE_DATA_TYPE dataType, TrackedFrame& trackedFrame); 

	//! Operation: Add frame to renderer in offline mode
	virtual PlusStatus AddFrameToRenderer(vtkImageData* frame); 
	virtual PlusStatus AddFrameToRenderer(const ImageType::Pointer& frame); 

	//! Description 
	// Read CalibrationController data element
	virtual PlusStatus ReadCalibrationControllerConfiguration( vtkXMLDataElement* calibrationController ); 

	//! Description 
	// Read SegmentationParameters data element
	virtual PlusStatus ReadSegmentationParametersConfiguration( vtkXMLDataElement* segmentationParameters );

	//! Description 
	// Read RealtimeCalibration data element
	virtual PlusStatus ReadRealtimeCalibrationConfiguration( vtkXMLDataElement* realtimeCalibration );

	//! Description 
	// Read Phantom definition from XML
	virtual PlusStatus ReadPhantomDefinition();
	

protected:
	//! Attributes: a reference to the automated segmenation component
	KPhantomSeg* mptrAutomatedSegmentation;
	
	//! Attributes: a reference to the segmentation parameter 
	SegmentationParameters* SegParameters;  

	//! Attributes: a reference to the visualization component
	vtkCalibratorVisualizationComponent* VisualizationComponent; 

	//! Attribute: Flag to enable the tracked sequence data saving to metafile
	bool EnableTrackedSequenceDataSaving;

	//! Attribute: Flag to enable the erroneously segmented data saving to metafile
	bool EnableErroneouslySegmentedDataSaving; 

	//! Attribute: Flag to enable the visualization component
	bool EnableVisualization; 

	//! Attribute: Flag to enable the Segmentation Analysis file
	bool EnableSegmentationAnalysis; 

	//! Attribute: Flag to show the initialized state
	bool Initialized; 

	//! Attributes: The input image dimensions (in pixels)
	int ImageWidthInPixels;
	int ImageHeightInPixels;

	//! Attributes: Segmentation search region
	// These define the region within the image where we should search for 
	// the dots. Outside of this region the image data must be null. Around 
	// all sides of this region there must be at least 8 pixels of this null 
	// space (searchOrigin >= 8 and searchSize <= imageSize-16). 
	int SearchStartAtX;
	int SearchStartAtY;
	int SearchDimensionX;
	int SearchDimensionY;

	//! Attributes: calibration mode (see CALIBRATION_MODE)
	int CalibrationMode; 

	//! Attribute: Path to program parent folder
	char* ProgramFolderPath;

	//! Attribute: Path to output calibration results
	char* OutputPath;

	//! Attributes: config file name
	char* ConfigurationFileName;

	//! Attributes: phantom definition file name
	char* PhantomDefinitionFileName;

	//! Pointer to the callback function that is executed each time a segmentation is finished
    SegmentationProgressPtr SegmentationProgressCallbackFunction;

	//! Stores the tracked frames for each data type 
	std::vector<vtkTrackedFrameList*> TrackedFrameListContainer;

	//! Stores saved dataset information
	std::vector<SavedImageDataInfo> SavedImageDataInfoContainer; 
	
	//! Stores realtime dataset information
	std::vector<RealtimeImageDataInfo> RealtimeImageDataInfoContainer; 
	
	//! Stores the segmentation results with transformation for each frame
	SegmentedFrameList SegmentedFrameContainer; 

private:
	vtkCalibrationController(const vtkCalibrationController&);
	void operator=(const vtkCalibrationController&);
};

#endif //  __VTKCALIBRATIONCONTROLLER_H

