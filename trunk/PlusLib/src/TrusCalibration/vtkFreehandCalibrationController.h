#ifndef __vtkFreehandCalibrationController_h
#define __vtkFreehandCalibrationController_h

#include "vtkCalibrationController.h"

#include "BrachyTRUSCalibrator.h"
#include "vtkDataCollector.h"

#include "vtkImageActor.h"
#include "vtkActor.h"
#include "vtkCamera.h"
#include "vtkPolyData.h"

//-----------------------------------------------------------------------------

/*!
* \brief Control operations for freehand calibration toolbox - singleton class
*/
class vtkFreehandCalibrationController : public vtkCalibrationController
{
public:

	vtkTypeRevisionMacro(vtkFreehandCalibrationController, vtkCalibrationController);

	/*!
	* \brief New
	*/
	static vtkFreehandCalibrationController *New();

	/*!
	* \brief VTK PrintSelf method
	*/
	virtual void PrintSelf(ostream& os, vtkIndent indent); 

	/*!
	* \brief Initialize segmenter and calibrator
  * \param aPhantomToPhantomReferenceTransform Phantom registration transform
  * \return Success flag
	*/
  PlusStatus InitializeCalibration(vtkTransform* aPhantomToPhantomReferenceTransform);

	/*!
	* \brief Reset calibration - clear objects that are used during calibration
  * \return Success flag
	*/
  PlusStatus ResetCalibration();

	/*!
	* \brief Execute temporal calibration
  * \return Success flag
	*/
	PlusStatus DoTemporalCalibration();

	/*!
	* \brief Execute spatial calibration
  * \return Success flag
	*/
  PlusStatus DoSpatialCalibration();

	/*!
	* \brief Assembles the result string to display
	* \return String containing results
	*/
	std::string GetResultString();

	/*!
	* \brief Callback function for updating progress - called by the temporal calibration process
  * \param aPercent Progress percent
	*/
 	static void UpdateProgress(int aPercent);

	virtual void SetUSImageFrameOriginInPixels(int originX, int originY); 
	virtual void SetUSImageFrameOriginInPixels(int* origin); 

	// Returns the calibrator
	BrachyTRUSCalibrator* GetCalibrator() { return Calibrator; }

	// Read XML based configuration of the calibration controller
	virtual PlusStatus ReadConfiguration( vtkXMLDataElement* aConfig ); 

  // Write configuration
	PlusStatus WriteConfiguration(vtkXMLDataElement* aConfig);

	// Read freehand calibration configurations (from probe calibration data element of the config file)
	virtual PlusStatus ReadFreehandCalibrationConfiguration(vtkXMLDataElement* probeCalibration);

	// Add new tracked data for segmentation and save the segmentation result to the SegmentedFrameContainer
	// The class has to be initialized before the segmentation process. 
	virtual PlusStatus AddTrackedFrameData( TrackedFrame* trackedFrame, IMAGE_DATA_TYPE dataType ); 

	//! Computes the calibration results: 
	virtual PlusStatus ComputeCalibrationResults();

	//! Read and populate US to Template calibration image data in offline mode
	virtual PlusStatus DoOfflineCalibration();

	//! print the calibration results as well as error reports to the stdout
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

	// File the calibration results 
	// This operation writes the final calibration results to a file.
	virtual PlusStatus SaveCalibrationResultsAndErrorReportsToXML(); 

	virtual void SaveCalibrationDataToSequenceMetafile();

public:
	//TODO--------------------------
	vtkSetMacro(ProgressPercent, int); 
	vtkGetMacro(ProgressPercent, int); 

	vtkSetMacro(CancelRequest, bool); 
	vtkGetMacro(CancelRequest, bool); 
	vtkBooleanMacro(CancelRequest, bool); 

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

  // TODO TEMPORARY
	vtkGetObjectMacro(DataCollector, vtkDataCollector); 
	vtkSetObjectMacro(DataCollector, vtkDataCollector); 

protected:
	//TODO
	// Call the calibrator class and do the calibration process
	virtual void Calibrate(); 
	// Populate the segmented N-fiducials to the data container
	virtual PlusStatus PopulateSegmentedFiducialsToDataContainer(vnl_matrix<double> &transformUSProbe2StepperFrameMatrix4x4, IMAGE_DATA_TYPE dataType); 

protected:
	/*!
	* \brief Constructor
	*/
	vtkFreehandCalibrationController();

	/*!
	* \brief Destructor
	*/
	virtual	~vtkFreehandCalibrationController();

protected:
	//! Attribute: a reference to the calibration phantom
	BrachyTRUSCalibrator* Calibrator;

	//! Result of the calibration - Image to Probe transform
	vtkTransform*	TransformImageToProbe;

	//! Attribute: Flag to enable the calibration log file
	bool EnableSystemLog;

	//! Attributes: The US image frame origin (in pixels) - These are the US image frame origin in pixels W.R.T. the left-upper corner of the original image, with X pointing to the right (column) and Y pointing down to the bottom (row)
	int USImageFrameOriginXInPixels;
	int USImageFrameOriginYInPixels;

	//! Attribute: calibration result file name
	char* CalibrationResultFileNameWithPath; 

	//! Attributes: suffix of the calibration result file
	char* CalibrationResultFileSuffix; 

	int ProgressPercent;

	bool CancelRequest;

	//! Data collector object TEMPORARY UNTIL TRACKED FRAME ACQUISITION GOES INTO APPLICATIONS
	vtkDataCollector*	                DataCollector;
};

#endif
