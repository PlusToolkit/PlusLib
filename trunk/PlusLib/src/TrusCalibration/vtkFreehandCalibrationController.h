#ifndef __vtkFreehandCalibrationController_h
#define __vtkFreehandCalibrationController_h

#include "AbstractToolboxController.h"
#include "vtkCalibrationController.h"

#include "BrachyTRUSCalibrator.h"

#include "vtkImageActor.h" 

//-----------------------------------------------------------------------------

/*!
* \brief Control operations for freehand calibration toolbox - singleton class
*/
class vtkFreehandCalibrationController : public vtkCalibrationController, public AbstractToolboxController //TODOsajt a filet is atnevezni
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
	*/
	void Initialize();

	/*!
	* \brief Clear - implementation of a pure virtual function
	*/
	void Clear();

	/*!
	* \brief Acquire new tracker positions and updates toolbox and canvas - implementation of a pure virtual function
	*/
	void DoAcquisition();

	/*!
	* \brief Start calibration - implementation of a pure virtual function
	*/
	void Start();

	/*!
	* \brief Stop calibration - implementation of a pure virtual function
	*/
	void Stop();

	//TODO--------------------------
	void StartTemporalCalibration();
	void StartSpatialCalibration();
	static void UpdateProgress(int aPercent);

	virtual void SetUSImageFrameOriginInPixels(int originX, int originY); 
	virtual void SetUSImageFrameOriginInPixels(int* origin); 

	// Returns the calibrator
	BrachyTRUSCalibrator* GetCalibrator() { return CalibrationPhantom; }

	// Read XML based configuration of the calibration controller
	virtual void ReadConfiguration( const char* configFileNameWithPath ); 
	virtual void ReadConfiguration( vtkXMLDataElement* configData ); 

	//! Operation: Register phantom geometry for calibrator 
	virtual void RegisterPhantomGeometry();
	//! Operation 
	// Add new tracked data for segmentation and save the segmentation result to the SegmentedFrameContainer
	// The class has to be initialized before the segmentation process. 
	virtual bool AddTrackedFrameData( TrackedFrame* trackedFrame, IMAGE_DATA_TYPE dataType ); 

	// Get/set the saved image data info
	RealtimeImageDataInfo GetRealtimeImageDataInfo(IMAGE_DATA_TYPE dataType);
	virtual void SetRealtimeImageDataInfo(IMAGE_DATA_TYPE dataType, RealtimeImageDataInfo realtimeImageDataInfo);

	//! Operation: Computes the calibration results: 
	// - Compute the overall Point-Line Distance Error (PLDE)
	// - Print the final calibration results and error reports 
	// - Save the calibration results and error reports into a file 
	// - Save the PRE3D distribution plot to an image file
	// - Map the PRE3D distribution onto the US image
	virtual void ComputeCalibrationResults();

	//! Operation: Read and populate US to Template calibration image data in offline mode
	virtual void DoOfflineCalibration();

	//! Operation: print the calibration results as well as error reports to the stdout
	virtual void PrintCalibrationResultsAndErrorReports();

//TODO ezek itt lent az ososztalyba???
	//! Attribute: Point-Line Distance Error Analysis for Validation Positions in US probe frame
	// FORMAT: (all positions are in the US probe frame)
	// [ vector 0 - 2:  PLDE_mean, PLDE_rms, PLDE_std ]
	// [ vector 3	 :	Validation data confidence level ]
	// where: 
	// - mean: linearly averaged;
	// - rms : root mean square;
	// - std : standard deviation.
	// - validation data confidence level: this is a percentage of 
	//   the independent validation data used to produce the final
	//   validation results.  It serves as an effective way to get 
	//   rid of corrupted data (or outliers) in the validation 
	//   dataset.  Default value: 0.95 (or 95%), meaning the top 
	//   ranked 95% of the ascendingly-ordered results from the 
	//   validation data would be accepted as the valid error values.
	std::vector<double> GetPointLineDistanceErrorAnalysisVector() { return this->GetCalibrator()->getPLDEAnalysis4ValidationDataSet(); } 

	// The 3D Point Reconstruction Error (PRE3D) analysis for the 
	// validation positions in the US probe frame.
	// FORMAT: (all positions are in the US probe frame)
	// [ vector 0 - 2:  PRE3D_X_mean, PRE3D_X_rms, PRE3D_X_std ]
	// [ vector 3 - 5:  PRE3D_Y_mean, PRE3D_Y_rms, PRE3D_Y_std ]
	// [ vector 6 - 8:  PRE3D_Z_mean, PRE3D_Z_rms, PRE3D_Z_std ]
	// [ vector 9	 :	Validation data confidence level ]
	// where: 
	// - mean: linearly averaged;
	// - rms : root mean square;
	// - std : standard deviation.
	// - validation data confidence level: this is a percentage of 
	//   the independent validation data used to produce the final
	//   validation results.  It serves as an effective way to get 
	//   rid of corrupted data (or outliers) in the validation 
	//   dataset.  Default value: 0.95 (or 95%), meaning the top 
	//   ranked 95% of the ascendingly-ordered PRE3D values from the 
	//   validation data would be accepted as the valid PRE3D values.
	virtual vtkstd::vector<double> GetPRE3DVector() { return this->GetCalibrator()->getPRE3DAnalysis4ValidationDataSet(); } 

	// The raw 3D Point Reconstruction Error (PRE3D) matrix from the 
	// validation data set.  This will return the original PRE3Ds 
	// matrix for the validation dataset with signs in the US 
	// probe frame (from the Projected positions to the true 
	// positions).  
	// NOTE: It may be useful for the user to obtain all PRE3Ds for 
	//       further statistical analysis if desired.
	// FORMAT: matrix 4xN (N: the total number of validation points)
	// [ Row-0: PRE3Ds in x-axis from projected to true positions ]
	// [ Row-1: PRE3Ds in y-axis from projected to true positions ]
	// [ Row-2: PRE3Ds in z-axis from projected to true positions ]
	// [ Row-3: should be all zeros ]
	virtual vnl_matrix<double> GetPRE3DMatrix() { return this->GetCalibrator()->getPRE3DsRAWMatrixforValidationDataSet4xN(); } 

	//! Attribute: Point-Line Distance Error for validation positions in US probe frame
	// This contains the Point-Line Distance Error (PLDE) for the validation dataset. 
	// The PLDE was defined as the absolute point-line distance from the projected
	// positions to the N-Wire (the ground truth), both in the US probe frame.  
	// If there was no error, the PLDE would be zero and the projected postions
	// would reside right on the N-Wire.  The physical position of the N-Wire
	// was measured based on the phantom geometry and converted into the US
	// US probe frame by the optical tracking device affixed on the phantom.
	// NOTE: this data may be used for statistical analysis if desired.
	// FORMAT: vector 1xN (with N being the total number of validation positions)
	vnl_vector<double> GetPointLineDistanceErrorVector() { return this->GetCalibrator()->getOrigPLDEsforValidationDataSet(); }
	vnl_vector<double> GetPointLineDistanceErrorSortedVector() { return this->GetCalibrator()->getSortedPLDEsforValidationDataSet(); }

	//! Attributes: Line Reconstruction Error Analysis for the validation positions in the US probe frame
	// FORMAT: (all positions are in the US probe frame)
	// For parallel NWires N1, N3, N4, N6:
	// [ vector 0 - 1:  LRE_X_mean,   LRE_X_std   ]
	// [ vector 2 - 3:  LRE_Y_mean,   LRE_Y_std   ]
	// [ vector 4 - 5:  LRE_EUC_mean, LRE_EUC_std ]
	// [ vector 6	 :	Validation data confidence level ]
	// where: 
	// - mean: linearly averaged;
	// - std : standard deviation;
	// - EUC: Euclidean (norm) measurement.
	// - validation data confidence level: this is a percentage of 
	//   the independent validation data used to produce the final
	//   validation results.  It serves as an effective way to get 
	//   rid of corrupted data (or outliers) in the validation 
	//   dataset.  Default value: 0.95 (or 95%), meaning the top 
	//   ranked 95% of the ascendingly-ordered results from the 
	//   validation data would be accepted as the valid error values.
	std::vector<double> GetLineReconstructionErrorAnalysisVector(int wireNumber);  

	//! Attribute: Line reconstruction error (LRE) matrix for validation positions in US probe frame
	// This keeps all the original PRE3Ds for the validation dataset with signs in
	// the US probe frame (from the Projected positions to the true positions).
	// FORMAT: matrix 4xN (with N being the total number of validation positions)
	// [ Row-0: PRE3Ds in x-axis from projected to true positions ]
	// [ Row-1: PRE3Ds in y-axis from projected to true positions ]
	// [ Row-2: should be all zeros ]
	// [ Row-3: Euclidean LRE distance ]
	// NOTE: this matrix can be obtained for statistical analysis if desired.
	vnl_matrix<double> GetLineReconstructionErrorMatrix(int wireNumber);

	//! Description
	// File the calibration results 
	// This operation writes the final calibration results to a file.
	virtual void SaveCalibrationResultsAndErrorReportsToXML(); 

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

	vtkGetObjectMacro(CanvasImageActor, vtkImageActor);
	vtkSetObjectMacro(CanvasImageActor, vtkImageActor);

	vtkGetMacro(EnableSystemLog, bool);
	vtkSetMacro(EnableSystemLog, bool);
	vtkBooleanMacro(EnableSystemLog, bool);

	vtkGetMacro(USImageFrameOriginXInPixels, int);
	vtkSetMacro(USImageFrameOriginXInPixels, int);
	vtkGetMacro(USImageFrameOriginYInPixels, int);
	vtkSetMacro(USImageFrameOriginYInPixels, int);

	vtkGetObjectMacro(TransformProbeToPhantomReference, vtkTransform);
	vtkSetObjectMacro(TransformProbeToPhantomReference, vtkTransform);

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
	virtual void DoCalibration(); 
	// Populate the segmented N-fiducials to the data container
	virtual void PopulateSegmentedFiducialsToDataContainer(vnl_matrix<double> &transformUSProbe2StepperFrameMatrix4x4, IMAGE_DATA_TYPE dataType); 

protected:
	//TODO
	bool TemporalCalibrationDone;
	bool SpatialCalibrationDone;
	int ProgressPercent;
	//! Attribute: a reference to the calibration phantom
	BrachyTRUSCalibrator* CalibrationPhantom;
	//! Attribute: Flag to enable the calibration log file
	bool EnableSystemLog;
	//! Attributes: The US image frame origin (in pixels) - These are the US image frame origin in pixels W.R.T. the left-upper corner of the original image, with X pointing to the right (column) and Y pointing down to the bottom (row)
	int USImageFrameOriginXInPixels;
	int USImageFrameOriginYInPixels;
	//! TODO
	vtkImageActor*				CanvasImageActor;
	vtkTransform*	TransformProbeToPhantomReference;
	//! Attribute: calibration result file name
	char* CalibrationResultFileNameWithPath; 
	//! Attributes: suffix of the calibration result file
	char* CalibrationResultFileSuffix; 

private:
	//! Instance of the singleton
	static vtkFreehandCalibrationController*	Instance;
};

#endif
