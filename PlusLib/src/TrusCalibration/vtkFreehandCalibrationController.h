#ifndef __vtkFreehandCalibrationController_h
#define __vtkFreehandCalibrationController_h

#include "AbstractToolboxController.h"
#include "vtkCalibrationController.h"

#include "BrachyTRUSCalibrator.h"

#include "vtkImageActor.h"
#include "vtkPolyData.h"

//-----------------------------------------------------------------------------

/*!
* \brief Control operations for freehand calibration toolbox - singleton class
*/
class vtkFreehandCalibrationController : public vtkCalibrationController, public AbstractToolboxController
{
public:

	vtkTypeRevisionMacro(vtkFreehandCalibrationController, vtkCalibrationController);

	/*!
	* \brief New
	*/
	static vtkFreehandCalibrationController *New();

	/*!
	* \brief Instance getter for the singleton class
	* \return Instance object
	*/
	static vtkFreehandCalibrationController* GetInstance();

	/*!
	* \brief Destructor
	*/
	virtual	~vtkFreehandCalibrationController();

	/*!
	* \brief VTK PrintSelf method
	*/
	virtual void PrintSelf(ostream& os, vtkIndent indent); 

	/*!
	* \brief Initialize - implementation of a pure virtual function
	* \return Success flag
	*/
	PlusStatus Initialize();

	/*!
	* \brief Initialize visualization (actors, cameras etc.)
	* \return Success flag
	*/
	PlusStatus InitializeVisualization();

	/*!
	* \brief Initialize visualization (actors, cameras etc.)
	* \return Success flag
	*/
	PlusStatus InitializeDeviceVisualization();

	/*!
	* \brief Clear - implementation of a pure virtual function
	* \return Success flag
	*/
	PlusStatus Clear();

	/*!
	* \brief Acquire new tracker positions and updates toolbox and canvas - implementation of a pure virtual function
	* \return Success flag
	*/
	PlusStatus DoAcquisition();

	/*!
	* \brief Start calibration; initialization of segmenter and calibrator - implementation of a pure virtual function
	* \return Success flag
	*/
	PlusStatus Start();

	/*!
	* \brief Stop calibration - implementation of a pure virtual function
	* \return Success flag
	*/
	PlusStatus Stop();

	/*!
	* \brief Toggle visualization of devices on or off
	* \param aOn Visualization state flag
	*/
	void ToggleDeviceVisualization(bool aOn);

	/*!
	* \brief Gets video time offset from video source
	* \return Currently used video time offset in seconds
	*/
	double GetVideoTimeOffset();

	/*!
	* \brief Assembles the result string to display
	* \return String containing results
	*/
	std::string GetResultString();

	//TODO--------------------------
	PlusStatus DoTemporalCalibration();
	static void UpdateProgress(int aPercent);
	bool IsReadyToStartSpatialCalibration();
	PlusStatus SaveCalibrationResults();
	PlusStatus CalculateImageCameraParameters();
	PlusStatus Reset();
	PlusStatus DoSpatialCalibration();

	virtual void SetUSImageFrameOriginInPixels(int originX, int originY); 
	virtual void SetUSImageFrameOriginInPixels(int* origin); 

	// Returns the calibrator
	BrachyTRUSCalibrator* GetCalibrator() { return Calibrator; }

	// Read XML based configuration of the calibration controller
	virtual PlusStatus ReadConfiguration( const char* configFileNameWithPath ); 
	virtual PlusStatus ReadConfiguration( vtkXMLDataElement* configData ); 
	// Read freehand calibration configurations (from probe calibration data element of the config file)
	virtual PlusStatus ReadFreehandCalibrationConfiguration(vtkXMLDataElement* probeCalibration);

	//! Operation 
	// Add new tracked data for segmentation and save the segmentation result to the SegmentedFrameContainer
	// The class has to be initialized before the segmentation process. 
	virtual PlusStatus AddTrackedFrameData( TrackedFrame* trackedFrame, IMAGE_DATA_TYPE dataType ); 

	// Get/set the saved image data info
	RealtimeImageDataInfo GetRealtimeImageDataInfo(IMAGE_DATA_TYPE dataType);
	virtual void SetRealtimeImageDataInfo(IMAGE_DATA_TYPE dataType, RealtimeImageDataInfo realtimeImageDataInfo);

	//! Operation: Computes the calibration results: 
	// - Compute the overall Point-Line Distance Error (PLDE)
	// - Print the final calibration results and error reports 
	// - Save the calibration results and error reports into a file 
	// - Save the PRE3D distribution plot to an image file
	// - Map the PRE3D distribution onto the US image
	virtual PlusStatus ComputeCalibrationResults();

	//! Operation: Read and populate US to Template calibration image data in offline mode
	virtual PlusStatus DoOfflineCalibration();

	//! Operation: print the calibration results as well as error reports to the stdout
	virtual PlusStatus PrintCalibrationResultsAndErrorReports();

//TODO these should go to a base class of this and ProbeCalibrationController?
	//! Attribute: Point-Line Distance Error Analysis for Validation Positions in US probe frame
	std::vector<double> GetPointLineDistanceErrorAnalysisVector() { return this->GetCalibrator()->getPLDEAnalysis4ValidationDataSet(); } 

	virtual vtkstd::vector<double> GetPRE3DVector() { return this->GetCalibrator()->getPRE3DAnalysis4ValidationDataSet(); } 

	virtual vnl_matrix<double> GetPRE3DMatrix() { return this->GetCalibrator()->getPRE3DsRAWMatrixforValidationDataSet4xN(); } 

	//! Attribute: Point-Line Distance Error for validation positions in US probe frame
	vnl_vector<double> GetPointLineDistanceErrorVector() { return this->GetCalibrator()->getOrigPLDEsforValidationDataSet(); }
	vnl_vector<double> GetPointLineDistanceErrorSortedVector() { return this->GetCalibrator()->getSortedPLDEsforValidationDataSet(); }

	//! Attributes: Line Reconstruction Error Analysis for the validation positions in the US probe frame
	std::vector<double> GetLineReconstructionErrorAnalysisVector(int wireNumber);  

	//! Attribute: Line reconstruction error (LRE) matrix for validation positions in US probe frame
	vnl_matrix<double> GetLineReconstructionErrorMatrix(int wireNumber);

	//! Description
	// File the calibration results 
	// This operation writes the final calibration results to a file.
	virtual PlusStatus SaveCalibrationResultsAndErrorReportsToXML(); 

	virtual void SaveCalibrationDataToSequenceMetafile();

	//TODO--------------------------

	// Set/Get macros for member variables
	vtkSetMacro(TemporalCalibrationDone, bool); 
	vtkGetMacro(TemporalCalibrationDone, bool); 
	vtkBooleanMacro(TemporalCalibrationDone, bool); 

	vtkSetMacro(SpatialCalibrationDone, bool); 
	vtkGetMacro(SpatialCalibrationDone, bool); 
	vtkBooleanMacro(SpatialCalibrationDone, bool); 

	vtkSetMacro(ProgressPercent, int); 
	vtkGetMacro(ProgressPercent, int); 

	vtkSetMacro(CancelRequest, bool); 
	vtkGetMacro(CancelRequest, bool); 
	vtkBooleanMacro(CancelRequest, bool); 

	vtkSetMacro(ShowDevices, bool); 
	vtkGetMacro(ShowDevices, bool); 
	vtkBooleanMacro(ShowDevices, bool); 

	vtkGetObjectMacro(CanvasImageActor, vtkImageActor);
	vtkSetObjectMacro(CanvasImageActor, vtkImageActor);

	vtkGetObjectMacro(PhantomBodyActor, vtkActor);
	vtkSetObjectMacro(PhantomBodyActor, vtkActor);

	vtkGetObjectMacro(ProbeActor, vtkActor);
	vtkSetObjectMacro(ProbeActor, vtkActor);

	vtkGetObjectMacro(StylusActor, vtkActor);
	vtkSetObjectMacro(StylusActor, vtkActor);

	vtkGetObjectMacro(NeedleActor, vtkActor);
	vtkSetObjectMacro(NeedleActor, vtkActor);
	
	vtkGetObjectMacro(SegmentedPointsActor, vtkActor);
	vtkSetObjectMacro(SegmentedPointsActor, vtkActor);

	vtkGetObjectMacro(SegmentedPointsPolyData, vtkPolyData);
	vtkSetObjectMacro(SegmentedPointsPolyData, vtkPolyData);

	vtkGetObjectMacro(ImageCamera, vtkCamera);
	vtkSetObjectMacro(ImageCamera, vtkCamera);

	vtkGetMacro(EnableSystemLog, bool);
	vtkSetMacro(EnableSystemLog, bool);
	vtkBooleanMacro(EnableSystemLog, bool);

	vtkGetMacro(USImageFrameOriginXInPixels, int);
	vtkSetMacro(USImageFrameOriginXInPixels, int);
	vtkGetMacro(USImageFrameOriginYInPixels, int);
	vtkSetMacro(USImageFrameOriginYInPixels, int);

	vtkGetObjectMacro(TransformImageToProbe, vtkTransform);
	vtkSetObjectMacro(TransformImageToProbe, vtkTransform);

	vtkGetStringMacro(CalibrationResultFileNameWithPath);
	vtkSetStringMacro(CalibrationResultFileNameWithPath);

	vtkGetStringMacro(CalibrationResultFileSuffix);
	vtkSetStringMacro(CalibrationResultFileSuffix);

protected:
	/*!
	* \brief Constructor
	*/
	vtkFreehandCalibrationController();

	//TODO
	// Call the calibrator class and do the calibration process
	virtual void Calibrate(); 
	// Populate the segmented N-fiducials to the data container
	virtual PlusStatus PopulateSegmentedFiducialsToDataContainer(vnl_matrix<double> &transformUSProbe2StepperFrameMatrix4x4, IMAGE_DATA_TYPE dataType); 
	//
	PlusStatus DisplaySegmentedPoints(bool aSuccess);

protected:
	//TODO
	bool TemporalCalibrationDone;
	bool SpatialCalibrationDone;
	int ProgressPercent;
	bool CancelRequest;
	bool ShowDevices;
	//! Attribute: a reference to the calibration phantom
	BrachyTRUSCalibrator* Calibrator;

	//! Actor displaying the image
	vtkImageActor*	CanvasImageActor;

	//! Camera of the scene
	vtkCamera*		ImageCamera;

	//! Result of the calibration - Image to Probe transform
	vtkTransform*	TransformImageToProbe;

	//! Actor for displaying the phantom body
	vtkActor* PhantomBodyActor;

	//! Actor for displaying the probe
	vtkActor* ProbeActor;

	//! Actor for displaying the stylus
	vtkActor* StylusActor;

	//! Actor for displaying the needle
	vtkActor* NeedleActor;

	//! Actor for displaying segmented points
	vtkActor* SegmentedPointsActor;

	//! Poly data for holding the segmented points
	vtkPolyData* SegmentedPointsPolyData;

	//! Attribute: Flag to enable the calibration log file
	bool EnableSystemLog;
	//! Attributes: The US image frame origin (in pixels) - These are the US image frame origin in pixels W.R.T. the left-upper corner of the original image, with X pointing to the right (column) and Y pointing down to the bottom (row)
	int USImageFrameOriginXInPixels;
	int USImageFrameOriginYInPixels;
	//! Attribute: calibration result file name
	char* CalibrationResultFileNameWithPath; 
	//! Attributes: suffix of the calibration result file
	char* CalibrationResultFileSuffix; 

private:
	//! Instance of the singleton
	static vtkFreehandCalibrationController*	Instance;
};

#endif
