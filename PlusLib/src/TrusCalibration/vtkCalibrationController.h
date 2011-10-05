#ifndef __VTKCALIBRATIONCONTROLLER_H
#define __VTKCALIBRATIONCONTROLLER_H

#include "vtkObject.h"
#include "vtkTrackedFrameList.h"
#include "vtkMatrix4x4.h"
#include "vtkImageData.h"
#include "vtkXMLDataElement.h"

#include "itkImage.h"
#include "vnl/vnl_matrix.h"
#include "vnl/vnl_vector.h"

#include <string>
#include <vector>
#include <deque>

#include "FidPatternRecognition.h"
#include "FidPatternRecognitionCommon.h"
#include "vtkProbeCalibrationControllerIO.h"

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
	PatternRecognitionResult SegResults;
	IMAGE_DATA_TYPE DataType; 
};


class vtkCalibrationController : public vtkObject
{
public:
	typedef unsigned char PixelType;
	typedef itk::Image< PixelType, 2 > ImageType;
	typedef std::deque<SegmentedFrame> SegmentedFrameList;

	//! Helper structure for storing image dataset info
	struct ImageDataInfo
	{
		std::string OutputSequenceMetaFileSuffix;
    std::string InputSequenceMetaFileName;
		int NumberOfImagesToAcquire; 
		int NumberOfSegmentedImages; 
	};

	static vtkCalibrationController *New();
	vtkTypeRevisionMacro(vtkCalibrationController, vtkObject);
	virtual void PrintSelf(ostream& os, vtkIndent indent); 

	//! Initialize the calibration controller interface
	virtual PlusStatus Initialize(); 

	//! Read XML based configuration of the calibration controller
	virtual PlusStatus ReadConfiguration( vtkXMLDataElement* configData ); 

	//! Read XML based configuration for probe calibration
  virtual PlusStatus ReadProbeCalibrationConfiguration( vtkXMLDataElement* configData );

	//! Read freehand calibration configurations (from probe calibration data element of the config file)
	virtual PlusStatus ReadFreehandCalibrationConfiguration(vtkXMLDataElement* probeCalibration);

  //! Write configuration
  virtual PlusStatus WriteConfiguration( vtkXMLDataElement* configData );

	//! Add new tracked data for segmentation and save the segmentation result to the SegmentedFrameContainer
	// The class has to be initialized before the segmentation process. 
	virtual PlusStatus AddTrackedFrameData( TrackedFrame* trackedFrame, IMAGE_DATA_TYPE dataType, const char* defaultTransformName ); 

	//! Returns the list of tracked frames of the selected data type
	virtual vtkTrackedFrameList* GetTrackedFrameList( IMAGE_DATA_TYPE dataType ); 
	
	//! Save the selected data type to sequence metafile 
	virtual PlusStatus SaveTrackedFrameListToMetafile( IMAGE_DATA_TYPE dataType, const char* outputFolder, const char* sequenceMetafileName, bool useCompression = false ); 

	//! Flag to show the initialized state
	vtkGetMacro(Initialized, bool);
	vtkSetMacro(Initialized, bool);
	vtkBooleanMacro(Initialized, bool);

	//! Flag to enable the tracked sequence data saving to metafile
	vtkGetMacro(EnableTrackedSequenceDataSaving, bool);
	vtkSetMacro(EnableTrackedSequenceDataSaving, bool);
	vtkBooleanMacro(EnableTrackedSequenceDataSaving, bool);

	//! Flag to enable the erroneously segmented data saving to metafile
	vtkGetMacro(EnableErroneouslySegmentedDataSaving, bool);
	vtkSetMacro(EnableErroneouslySegmentedDataSaving, bool);
	vtkBooleanMacro(EnableErroneouslySegmentedDataSaving, bool);

	//! Flag to enable the visualization component
	vtkGetMacro(EnableVisualization, bool);
	vtkSetMacro(EnableVisualization, bool);
	vtkBooleanMacro(EnableVisualization, bool);

  //! Get offline image data
	vtkGetObjectMacro(OfflineImageData, vtkImageData);
  
  //! Set/get the calibration date and time in string format
	vtkSetStringMacro(CalibrationDate); 
	vtkGetStringMacro(CalibrationDate);

  //! Set/get the calibration date and time in string format for file names
	vtkSetStringMacro(CalibrationTimestamp); 
	vtkGetStringMacro(CalibrationTimestamp);

  //! Flag to identify the calibration state 
	vtkGetMacro(CalibrationDone, bool);
	vtkSetMacro(CalibrationDone, bool);
	vtkBooleanMacro(CalibrationDone, bool);
  
	//! Flag to enable the Segmentation Analysis
	vtkGetMacro(EnableSegmentationAnalysis, bool);
	vtkSetMacro(EnableSegmentationAnalysis, bool);
	vtkBooleanMacro(EnableSegmentationAnalysis, bool);

	//! Get the segmentation result container (stores the segmentation results with transformation for each frame)
	SegmentedFrameList GetSegmentedFrameContainer() { return this->SegmentedFrameContainer; };

  const char* GetSegmentedFrameDefaultTransformName() { return this->SegmentedFrameDefaultTransformName.c_str();};

  //! Get the fiducial pattern recognition master object
  FidPatternRecognition * GetPatternRecognition() { return & this->PatternRecognition; };
  void SetPatternRecognition( FidPatternRecognition value) { PatternRecognition = value; };

  //! Get the fiducial pattern recognition master object
  PatternRecognitionResult * GetPatRecognitionResult() { return & this->PatRecognitionResult; };
  void SetPatRecognitionResult( PatternRecognitionResult value) { PatRecognitionResult = value; };

  //! Clear all datatype segmented frames from container 
  void ClearSegmentedFrameContainer(IMAGE_DATA_TYPE dataType); 

	//! Get/set the saved image data info
	ImageDataInfo GetImageDataInfo( IMAGE_DATA_TYPE dataType ) { return this->ImageDataInfoContainer[dataType]; }
	virtual void SetImageDataInfo( IMAGE_DATA_TYPE dataType, ImageDataInfo imageDataInfo ) { this->ImageDataInfoContainer[dataType] = imageDataInfo; }
	
	//! Callback function that is executed each time a segmentation is finished
	typedef void (*SegmentationProgressPtr)(int percent);
  void SetSegmentationProgressCallbackFunction(SegmentationProgressPtr cb) { SegmentationProgressCallbackFunction = cb; } 

public: // Former ProbeCalibrationController and FreehandCalibraitonController functions
	//! Register phantom geometry for calibrator 
	virtual void RegisterPhantomGeometry( double phantomToProbeDistanceInMm[2] );
  virtual void RegisterPhantomGeometry( vtkTransform* aPhantomToPhantomReferenceTransform ); 

	//! Computes the calibration results: 
	// - Compute the overall Point-Line Distance Error (PLDE)
	// - Print the final calibration results and error reports 
	// - Save the calibration results and error reports into a file 
	// - Save the PRE3D distribution plot to an image file
	// - Map the PRE3D distribution onto the US image
	virtual PlusStatus ComputeCalibrationResults();

  //! Check user image home to probe home transform orthogonality 
  virtual bool IsUserImageToProbeTransformOrthogonal(); 

	//! Read and populate US to Template calibration image data in offline mode
	virtual PlusStatus OfflineUSToTemplateCalibration();  
		
	//! get the wire position of the the US frame and phantom intersection in template coordinate system
	virtual PlusStatus GetWirePosInTemplateCoordinate( int wireNum, double* wirePosInTemplate ); 

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
	virtual vtkstd::vector<double> GetPRE3DVector() { return mAbsPRE3DAnalysis4ValidationPositionsInUSProbeFrame; } 

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
	virtual vnl_matrix<double> GetPRE3DMatrix() { return mRawPRE3DsforValidationPositionsInUSProbeFrameMatrix4xN; } 

	//! Point-Line Distance Error for validation positions in US probe frame
	// This contains the Point-Line Distance Error (PLDE) for the validation dataset. 
	// The PLDE was defined as the absolute point-line distance from the projected
	// positions to the N-Wire (the ground truth), both in the US probe frame.  
	// If there was no error, the PLDE would be zero and the projected postions
	// would reside right on the N-Wire.  The physical position of the N-Wire
	// was measured based on the phantom geometry and converted into the US
	// US probe frame by the optical tracking device affixed on the phantom.
	// NOTE: this data may be used for statistical analysis if desired.
	// FORMAT: vector 1xN (with N being the total number of validation positions)
	vnl_vector<double> GetPointLineDistanceErrorVector() { return mPLDEsforValidationPositionsInUSProbeFrame; }
	vnl_vector<double> GetPointLineDistanceErrorSortedVector() { return mSortedPLDEsAscendingforValidationInUSProbeFrame; }

	//! Point-Line Distance Error Analysis for Validation Positions in US probe frame
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
	std::vector<double> GetPointLineDistanceErrorAnalysisVector() { return mPLDEAnalysis4ValidationPositionsInUSProbeFrame; } 

	//! Line Reconstruction Error Analysis for the validation positions in the US probe frame
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
  PlusStatus UpdateLineReconstructionErrorAnalysisVectors();  
	PlusStatus GetLineReconstructionErrorAnalysisVector(int wireNumber, std::vector<double> &LRE);  

	//! Line reconstruction error (LRE) matrix for validation positions in US probe frame
	// This keeps all the original PRE3Ds for the validation dataset with signs in
	// the US probe frame (from the Projected positions to the true positions).
	// FORMAT: matrix 4xN (with N being the total number of validation positions)
	// [ Row-0: PRE3Ds in x-axis from projected to true positions ]
	// [ Row-1: PRE3Ds in y-axis from projected to true positions ]
	// [ Row-2: should be all zeros ]
	// [ Row-3: Euclidean LRE distance ]
	// NOTE: this matrix can be obtained for statistical analysis if desired.
	vnl_matrix<double> GetLineReconstructionErrorMatrix(int wireNumber);

	//! Get Minimum US elevation beamwidth and the focal zone in US Image Frame (see MinElevationBeamwidthAndFocalZoneInUSImageFrame member)
	vnl_vector<double> *GetMinElevationBeamwidthAndFocalZoneInUSImageFrame() { return &this->MinElevationBeamwidthAndFocalZoneInUSImageFrame; } // TODO this and the similar get functions become unnecessary once vtkProbeCalibrationControllerIO is consolidated

	//! Get US 3D beamwidth profile data in US Image Frame with weights factors along axial depth (see SortedUS3DBeamwidthAndWeightFactorsInAscendingAxialDepthInUSImageFrameMatrix5xN member)
	vnl_matrix<double> *GetSortedUS3DBeamwidthAndWeightFactorsInAscendingAxialDepthInUSImageFrameMatrix5xN() { return &this->SortedUS3DBeamwidthAndWeightFactorsInAscendingAxialDepthInUSImageFrameMatrix5xN; }

  vnl_matrix<double>* GetSortedUS3DBeamwidthInAscendingAxialDepth2CrystalsMatrixNx4() { return &this->SortedUS3DBeamwidthInAscendingAxialDepth2CrystalsMatrixNx4; }

	// Get Interpolated US 3D beamwidth profile and weight calulated based on it (see InterpUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xM member)
	vnl_matrix<double> *GetInterpUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xM() { return &this->InterpUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xM; }

  //! Set/get the calibration controller IO
	vtkSetObjectMacro(CalibrationControllerIO, vtkProbeCalibrationControllerIO); 
	vtkGetObjectMacro(CalibrationControllerIO, vtkProbeCalibrationControllerIO); 

  std::map<int, std::vector<double>>* GetLineReconstructionErrors() { return &this->LineReconstructionErrors; };  

	/*!
	* \brief Reset calibration - clear objects that are used during calibration
  * \return Success flag
	*/
  PlusStatus ResetFreehandCalibration();

  /*!
	* \brief Assembles the result string to display
	* \return String containing results
	*/
	std::string GetResultString();

	//! Read and populate US to Template calibration image data in offline mode
	virtual PlusStatus DoOfflineCalibration();

  //! Set/get phantom to probe distance in mm
  vtkSetVector2Macro(PhantomToProbeDistanceInMm, double); 
	vtkGetVector2Macro(PhantomToProbeDistanceInMm, double);

	//! Flag to set if the US 3D beamwidth data is sucessfully loaded
	vtkGetMacro(US3DBeamwidthDataReady, bool);
	vtkSetMacro(US3DBeamwidthDataReady, bool);
	vtkBooleanMacro(US3DBeamwidthDataReady, bool);

	//! Flag to enable the saving of segmented wire positions to file
	vtkGetMacro(EnableSegmentedWirePositionsSaving, bool);
	vtkSetMacro(EnableSegmentedWirePositionsSaving, bool);
	vtkBooleanMacro(EnableSegmentedWirePositionsSaving, bool);

	//! Flags to incorporate the ultrasound 3D beam profile (beam width)
	// The flag is set when the 3D US beam width data is to be incorporated into
	// the calibration process (e.g., by adding weights to the least-squares 
	// optimization method w.r.t the beam width profile).
	// NOTE: if the flag is turned on (default is 0), then a US-3D-beam-
	// profile data file needs to be prepared for further processing.
	// OPTIONS: 0 - NO | 1 - BeamwidthVariance | 2 - BeamwidthRatio | 3 - BeamwidthVarianceAndThresholding
	vtkGetMacro(IncorporatingUS3DBeamProfile, int);
	vtkSetMacro(IncorporatingUS3DBeamProfile, int);

	//! Axial position of the crystal surface in the TRUS Image Frame
	// Typically, the US machine has a bright mark in the display of the US image 
	// indicating the actual position where the sound starts propagation.
	// NOTE: this position has been converted to the TRUS Image Frame being defined
	// by the user in the calibration configuration file.
	vtkGetMacro(AxialPositionOfCrystalSurfaceInTRUSImageFrame, double);
	vtkSetMacro(AxialPositionOfCrystalSurfaceInTRUSImageFrame, double);

	//! Number of the US 3D beamwidth profile data (this is the total number of US 3D-beam-width data (samples) collected)
	vtkGetMacro(NumUS3DBeamwidthProfileData, int);
	vtkSetMacro(NumUS3DBeamwidthProfileData, int);

	//! Track the current position ID of the output in PRE3D distribution data
	vtkGetMacro(CurrentPRE3DdistributionID, int);
	vtkSetMacro(CurrentPRE3DdistributionID, int);

	//! Calibration config file name
	vtkGetStringMacro(CalibrationConfigFileNameWithPath);
	vtkSetStringMacro(CalibrationConfigFileNameWithPath);

	//! Calibration result file name
	vtkGetStringMacro(CalibrationResultFileNameWithPath);
	vtkSetStringMacro(CalibrationResultFileNameWithPath);

	//! Segmentation error log file name with timestamp
	vtkGetStringMacro(SegmentationErrorLogFileNameWithTimeStamp);
	vtkSetStringMacro(SegmentationErrorLogFileNameWithTimeStamp);

	//! Segmentation analysis file name with timestamp
	vtkGetStringMacro(SegmentationAnalysisFileNameWithTimeStamp);
	vtkSetStringMacro(SegmentationAnalysisFileNameWithTimeStamp);
	
	//! US 3D beam profile name and path
	vtkGetStringMacro(US3DBeamProfileDataFileNameAndPath); 
	vtkSetStringMacro(US3DBeamProfileDataFileNameAndPath); 

	//! Suffix of the data files
	vtkGetStringMacro(DataFileSuffix);
	vtkSetStringMacro(DataFileSuffix);

	//! Segmented wire positions of calibration dataset
	vtkGetStringMacro(CalibrationSegWirePosInfoFileName);
	vtkSetStringMacro(CalibrationSegWirePosInfoFileName);

	//! Segmented wire positions of validation dataset
	vtkGetStringMacro(ValidationSegWirePosInfoFileName);
	vtkSetStringMacro(ValidationSegWirePosInfoFileName);
	
	//! Suffix of the calibration result file
	vtkGetStringMacro(CalibrationResultFileSuffix);
	vtkSetStringMacro(CalibrationResultFileSuffix);
	
	//! Suffix of the segmentation error log file 
	vtkGetStringMacro(SegmentationErrorLogFileNameSuffix);
	vtkSetStringMacro(SegmentationErrorLogFileNameSuffix);
	
	//! Suffix of the segmentation analysis file
	vtkGetStringMacro(SegmentationAnalysisFileNameSuffix);
	vtkSetStringMacro(SegmentationAnalysisFileNameSuffix);

	//! Suffix of the template to stepper calibration analysis file
	vtkGetStringMacro(Temp2StepCalibAnalysisFileNameSuffix);
	vtkSetStringMacro(Temp2StepCalibAnalysisFileNameSuffix);

	//! Get/set final calibration transform 
	vtkGetObjectMacro(TransformImageToTemplate, vtkTransform);
	vtkSetObjectMacro(TransformImageToTemplate, vtkTransform);

	//! Get/set image home to user defined image home constant transform
	// Should be defined in config file
	vtkGetObjectMacro(TransformImageToUserImage, vtkTransform);
	vtkSetObjectMacro(TransformImageToUserImage, vtkTransform);
	
	//! Get/set the iCAL calibration result transformation between the user defined image home and probe home position 
	vtkGetObjectMacro(TransformUserImageToProbe, vtkTransform);
	vtkSetObjectMacro(TransformUserImageToProbe, vtkTransform);
	
	//! Get/set transformation between the probe home and probe position (e.g read from stepper)
	vtkGetObjectMacro(TransformProbeToReference, vtkTransform);
	vtkSetObjectMacro(TransformProbeToReference, vtkTransform);
	
	//! Get/set the transformation between probe home and template holder home position.
	// Result of the Template To Stepper Calibration 
	vtkGetObjectMacro(TransformReferenceToTemplateHolderHome, vtkTransform);
	vtkSetObjectMacro(TransformReferenceToTemplateHolderHome, vtkTransform);

	//! Get/set the constant transformation between template holder home and template home position
	// Should be defined in config file
	vtkGetObjectMacro(TransformTemplateHolderToTemplate, vtkTransform);
	vtkSetObjectMacro(TransformTemplateHolderToTemplate, vtkTransform);
	
  //! Get/set the constant transformation between template holder home and template home position
	// Should be defined in config file
  vtkGetObjectMacro(TransformTemplateHolderToPhantom, vtkTransform);
	vtkSetObjectMacro(TransformTemplateHolderToPhantom, vtkTransform);
	
	//! Get/set the transformation between template home and template position
	// Should be identical to TransformTemplateHolderHomeToTemplateHolder transformation 
	vtkGetObjectMacro(TransformTemplateHomeToTemplate, vtkTransform);
	vtkSetObjectMacro(TransformTemplateHomeToTemplate, vtkTransform);

  //! Set/Get the rotation center in pixels.
	// Origin: Left-upper corner (the original image frame)
	// Positive X: to the right;
	// Positive Y: to the bottom;
	vtkSetVector2Macro(CenterOfRotationPx, int); 
	vtkGetVector2Macro(CenterOfRotationPx, int);
		
protected:
	vtkCalibrationController();
	virtual ~vtkCalibrationController();

protected:
	//! Add frame to renderer in offline mode
	virtual PlusStatus SetOfflineImageData(vtkImageData* frame); 
	virtual PlusStatus SetOfflineImageData(const ImageType::Pointer& frame); 

	//! Read CalibrationController data element
	virtual PlusStatus ReadCalibrationControllerConfiguration(vtkXMLDataElement* rootElement); 

	//! Populate the segmented N-fiducials to the data container
	virtual void PopulateSegmentedFiducialsToDataContainer(vnl_matrix<double> &transformUSProbe2StepperFrameMatrix4x4, IMAGE_DATA_TYPE dataType); 

	//! Call the calibrator class and do the calibration process
	virtual void DoCalibration(); 

protected: // from former Phantom class
	// This will construct the validation data matrices
	// NOTE: since the validation data set is separated from the calibration 
	//       data and acquired before the calibration, the construction of
	//       the validation data matrices needs to be done once and only 
	//       once (e.g., before the 1st validation task is performed) with
	//       the flag set to true.  This would save the system runtime
	//       during the iterative calibration/validation process.
	PlusStatus constructValidationDataMatrices();

	//! Frames where the LRE is larger than LRE stdev * OutlierDetectionThreshold will be considered as outliers (calibration DataPositions)
	virtual void setOutlierFlags(); 

	//! Clear outlier flags 
	virtual void resetOutlierFlags();

  //! Reset data containers
  void resetDataContainers();

	//! Fill the ultrasound beamwidth profile and weight factors (see mUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xM member)
  void FillUltrasoundBeamwidthAndWeightFactorsTable();

  //! Compute intersections of wires in the loaded NWires structure
  PlusStatus ComputeNWireInstersections();

	//! Perform the final calibration task and output the results.
	// Given the input data (the data positions in both the US image frame and the US
	// probe frame, a least-squares based algorithm is employ to find the optimal
	// homogeneous transformation matrix from the US image frame to the US probe frame.
	PlusStatus calibrate();

	//! It calculates the 3D point reconstruction error (PRE3D) from
	// the validation data sets if they are imported and ready.
	virtual PlusStatus compute3DPointReconstructionError();

	//! This operation also computes the Point-Line Distance Error (PLDE) from
	// the validation data sets.  It reconstructs the NWire point positions
	// in the N-wire Phantom space using the existing calibration parameters
	// and then calculates the point-to-line distance to the physical wire
	// location that has been precisly measured on the phantom geometry.
	PlusStatus computeIndependentPointLineReconstructionError();

	//! Calculate and add positions in Phantom frame
	// This operation will use the segmented N-fiducials in each US image to 
	// calculate the position in Phantom frame base on the existing phantom 
	// geometry data.  The result would be added contineously into a private 
	// class data storage container.  Note: this operation is performed on 
	// one individual US image captured on Phantom, so natually, it can be 
	// called repeatedly to add in reference positions in multiple images 
	// in order to enhance the calibration accuracy.
	virtual PlusStatus addDataPositionsPerImage( 
		std::vector< vnl_vector<double> > SegmentedDataPositionListPerImage, 
		const vnl_matrix<double> TransformMatrixUSProbe2Stepper4x4 );

	//! Add positions per image to validate calibration accuracy
	// This operation performs similarly to the ::addDataPositionsPerImage
	// method, except that these added positions are stored to validate the 
	// overall calibration accuracy.  These data positions are kept 
	// separately from those that are used for the calibration so as not to 
	// be biased toward the calibration results in the validation process.
	// Note: like ::addDataPositionsPerImage, this operation is performed 
	// on individual US images acquired from the phantom, so natually, it 
	// can be called repeatedly to add in as many data necessary to enhance 
	// the calibration accuracy.
	virtual PlusStatus addValidationPositionsPerImage( 
		std::vector< vnl_vector<double> > SegmentedDataPositionListPerImage, 
		const vnl_matrix<double> TransformMatrixUSProbe2Stepper4x4 );

	//! Calculate the 3D Point Reconstruction Error (PRE3D) for a given image.
	// This operation obtain the PRE3D for a given image (captured in real time) using
	// the latest updated calibration result.
	//
	// NOTE: The purpose of this method is to enable a real-time recursive calibration
	// control process to iteratively examine the current calibration accuracy using
	// a fresh real-time image data before the image could be further used for 
	// the calibration.  So the real-time image data would be used for validation 
	// while still remains independant to the calibration process (since the 
	// validation takes place before the data is used).
	//
	// FORMAT: PRE3Ds are averaged for all the data positions in the image and given
	// in the following format:
	// [vector 0 - 2   : Averaged absolute PRE3D in the US probe frame in X,Y and Z axis ]
	// [vector 3,4,... : Back-projected positions in the US image frame in (Xi, Yi) ...]
	// NOTE: For BrachyTRUSCalibrator, there are only two data positions per image:
	//       Hence, [vector 3-6: X1, Y1, X2, Y2]
	virtual std::vector<double> getPRE3DforRealtimeImage(			
		std::vector< vnl_vector<double> > SegmentedDataPositionListPerImage, 
		const vnl_matrix<double> TransformMatrixUSProbe2DRB4x4 );

	//! Get Line reconstruction error (LRE) vector for validation positions in US probe frame
	// FORMAT:
	// [ 0: PRE3Ds in x-axis from projected to true positions ]
	// [ 1: PRE3Ds in y-axis from projected to true positions ]
	vnl_vector<double> getPointLineReconstructionError(vnl_vector<double> NWirePositionInUSImageFrame, 
		vnl_vector<double> NWirePositionInUSProbeFrame);  

protected:
	//! Flag to enable the tracked sequence data saving to metafile
	bool EnableTrackedSequenceDataSaving;

	//! Flag to enable the erroneously segmented data saving to metafile
	bool EnableErroneouslySegmentedDataSaving; 

	//! Flag to enable the visualization component
	bool EnableVisualization; 

	//! Flag to enable the Segmentation Analysis file
	bool EnableSegmentationAnalysis; 

	//! Flag to show the initialized state
	bool Initialized; 

  //! Flag to identify the calibration state 
	bool CalibrationDone; 

  //! Calibration date in string format 
  char* CalibrationDate; 

  //! Calibration date and time in string format for file names
  char* CalibrationTimestamp; 

  //! Pointer to the callback function that is executed each time a segmentation is finished
  SegmentationProgressPtr SegmentationProgressCallbackFunction;

	//! Stores the tracked frames for each data type 
	std::vector<vtkTrackedFrameList*> TrackedFrameListContainer;

	//! Stores dataset information
	std::vector<ImageDataInfo> ImageDataInfoContainer; 
	
	//! Stores the segmentation results with transformation for each frame
	SegmentedFrameList SegmentedFrameContainer;

  //! Default transform name for the frames stored in the SegmentedFrameContainer
  std::string SegmentedFrameDefaultTransformName;


  //! Stores the fiducial pattern recognition master object
  FidPatternRecognition PatternRecognition;

  //! Stores the segmentation results of a single frame
  PatternRecognitionResult PatRecognitionResult;

  //! Stores the image data in offline mode
  vtkImageData* OfflineImageData; 

protected: // Former ProbeCalibrationController and FreehandCalibrationController members
	//! a reference to the calibration controller IO
	vtkProbeCalibrationControllerIO* CalibrationControllerIO; 

	//! Flag to enable the saving of segmented wire positions to file
	bool EnableSegmentedWirePositionsSaving; 

	// Flag to set if the US 3D beamwidth data is sucessfully loaded
	bool US3DBeamwidthDataReady;

	//! Flags to incorporate the ultrasound 3D beam profile (beam width)
	// The flag is set when the 3D US beam width data is to be incorporated into
	// the calibration process (e.g., by adding weights to the least-squares 
	// optimization method w.r.t the beam width profile).
	// NOTE: if the flag is turned on (default is 0), then a US-3D-beam-
	// profile data file needs to be prepared for further processing.
	// OPTIONS: 0 - NO | 1 - BeamwidthVariance | 2 - BeamwidthRatio | 3 - BeamwidthVarianceAndThresholding
	int IncorporatingUS3DBeamProfile;

	//! Axial position of the crystal surface in the TRUS Image Frame
	// Typically, the US machine has a bright mark in the display of the US image 
	// indicating the actual position where the sound starts propagation.
	// NOTE: this position has been converted to the TRUS Image Frame being defined
	// by the user in the calibration configuration file.
	double AxialPositionOfCrystalSurfaceInTRUSImageFrame;

	//! Number of the US 3D beamwidth profile data
	// This is the total number of US 3D-beam-width data (samples) collected.
	int NumUS3DBeamwidthProfileData;

	//! Track the current position ID of the output in PRE3D distribution data
	int CurrentPRE3DdistributionID;

  //! Center of rotation position in pixels
  int CenterOfRotationPx[2]; 

	//! calibration config file name
	char* CalibrationConfigFileNameWithPath;

	//! calibration result file name
	char* CalibrationResultFileNameWithPath; 

	//! Segmentation error log file name with timestamp
	char* SegmentationErrorLogFileNameWithTimeStamp;

	//! Segmentation analysis file name with timestamp
	char* SegmentationAnalysisFileNameWithTimeStamp;

	//! US 3D beam profile name and path
	char* US3DBeamProfileDataFileNameAndPath;

	//! Suffix of the data files 
	char* DataFileSuffix; 
	
	//! Suffix of the calibration result file
	char* CalibrationResultFileSuffix; 
	
	//! Suffix of the segmentation error log file 
	char* SegmentationErrorLogFileNameSuffix; 
	
	//! Suffix of the segmentation analysis file
	char* SegmentationAnalysisFileNameSuffix; 

	//! Suffix of template to stepper calibration analysis file
	char* Temp2StepCalibAnalysisFileNameSuffix; 

	//! Segmented wire positions of calibration dataset
	char* CalibrationSegWirePosInfoFileName; 

	//! Segmented wire positions of validation dataset
	char* ValidationSegWirePosInfoFileName; 
	
	//! Minimum US elevation beamwidth and the focal zone in US Image Frame
	// 1. For a typical 1-D linear-array transducer, the ultrasound beam can only be 
	//    focused mechanically in the elevation (out-of-plane) axis by placing an 
	//    ascoustic lens in front or curving the crystal surface.  
	// 2. The elevation beam pattern are therefore solely determined by the size and 
	//    curvature of the crystal or the acoustic lens, with sharp focus only 
	//    possible at a narrow axial distance (the focal zone) to the transducer.
	// FORMAT: [FOCAL-ZONE (in US Image Frame), MINI-ELEVATION-WIDTH (in millimeters)]
	vnl_vector<double> MinElevationBeamwidthAndFocalZoneInUSImageFrame;

	//! US 3D beamwidth profile data in US Image Frame with weights factors along axial depth.
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
	vnl_matrix<double> SortedUS3DBeamwidthAndWeightFactorsInAscendingAxialDepthInUSImageFrameMatrix5xN;

	vnl_matrix<double> SortedUS3DBeamwidthInAscendingAxialDepth2CrystalsMatrixNx4; 

	//! Interpolated US 3D beamwidth profile and weight calulated based on it
	// Here we used a simple linear interpolation between sampled data to obtain
	// the beamwidth at non-sampled axial depth.  In general, this approach works
	// fine as long as we have enough sample points that covers the entire US field.
	// FORMAT: each row of the matrix has the following columns:
	// [0]:		Ascending Axial Depth in US Image Frame (in pixels);
	// [1-3]:	Beamwith in axial, lateral and elevational axes respectively (in mm);
	// [4]:		Weight Factor = CurrentBeamWidth/MininumBeamWidth (>=1).
	vnl_matrix<double> InterpUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xM;

  double PhantomToProbeDistanceInMm[2]; 

  //! Final calibration transform
	vtkTransform * TransformImageToTemplate;

	//! Transform matrix from the original image frame to the TRUS image frame. Constant transform, read from file
	vtkTransform * TransformImageToUserImage;

  //! The result of iCAL
	vtkTransform * TransformUserImageToProbe;

  //! Actual tracker position of the probe in the reference frame
	vtkTransform * TransformProbeToReference;

  //! Result of the TemplateToStepperCalibrator
	vtkTransform * TransformReferenceToTemplateHolderHome;

  //! Constant transform (specific to the current template), read from file
	vtkTransform * TransformTemplateHolderToTemplate;

  //! Constant transform (specific to the current calibration phantom), read from file
  vtkTransform * TransformTemplateHolderToPhantom;

  //! Actual template position, read from stepper
	vtkTransform * TransformTemplateHomeToTemplate;

  //! TODO
  std::map<int, std::vector<double> > LineReconstructionErrors; 

protected: // From former Phantom class
	//! Flags that control the registration between phantom and DRB
	// The flag to be set when the phantom is registered to the DRB frame
	// on-the-fly (before the calibration procedure starts)
	bool mHasPhantomBeenRegistered;

	//! The flag to be set when the data positions in US probe frame is ready
	bool mAreOutliersRemoved;

	//! The flag to be set when the PRE3Ds for validation positions are ready
	bool mArePRE3DsForValidationPositionsReady;
	//! The flag to be set when the independent point/line reconstruction errors are ready
	bool mAreIndependentPointLineReconErrorsReady;

	//! The US 3D beamwidth profile data and weight factors based on it
	// 1. This contains the interpolated US 3D beamwidth profile data at various axial depths
	//    as well as the weight factors calculated based on the beamwidth for each depth.
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
	// FORMAT: each row of the matrix table has the following columns:
	// [COL-0]:			Ascending Axial Depth in US Image Frame (in pixels);
	// [COL-1 to 3]:	Beamwith in axial, lateral and elevational axes respectively (in mm);
	// [COL-4]:			Weight Factor = CurrentBeamWidth/MininumBeamWidth (>=1).
	vnl_matrix<double> mUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xM;

	//! This keeps a copy of the original non-interpolated US beamwidth profile
	// mainly for the purposes of logging and future reference 
	vnl_matrix<double> mOrigUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xN;

	//! The Minimum US elevation beamwidth and the focal zone in US Image Frame
	// 1. For a typical 1-D linear-array transducer, the ultrasound beam can only be 
	//    focused mechanically in the elevation (out-of-plane) axis by placing an 
	//    ascoustic lens in front or curving the crystal surface.  
	// 2. The elevation beam pattern are therefore solely determined by the size and 
	//    curvature of the crystal or the acoustic lens, with sharp focus only 
	//    possible at a narrow axial distance (the focal zone) to the transducer.
	// FORMAT: [FOCAL-ZONE (in US Image Frame), MINI-ELEVATION-WIDTH (in millimeters)]
	vnl_vector<double> mMinimumUSElevationBeamwidthAndFocalZoneInUSImageFrame;

	//! The nearest and farest axial depth in the table (for fast access) 
	int mTheNearestAxialDepthInUSBeamwidthAndWeightTable;
	int mTheFarestAxialDepthInUSBeamwidthAndWeightTable;

	//! The 3D beamwidth elements at the nearest/farest axial depth
	vnl_vector<double> mUS3DBeamwidthAtNearestAxialDepth;
	vnl_vector<double> mUS3DBeamwidthAtFarestAxialDepth;

	//! The flag to be set if beamwidth and weight factors are set.
	bool mIsUSBeamwidthAndWeightFactorsTableReady;

	//! Weights for the data positions defined by prior knowledge of the imaging condition
	// E.g.: the ultrasound 3D beamwidth in axial, lateral and elevational axes
	std::vector<double> mWeightsForDataPositions;

	//! This will keep a track of the US 3D beamwidth (euclidean magnitude) at each data position
	std::vector<double> mUSBeamWidthEuclideanMagAtDataPositions;

	//! This will keep a track of the US 3D beamwidth (euclidean magnitude) at each validation position
	std::vector<double> mUSBeamWidthEuclideanMagAtValidationPositions;

	//! The flag to set when the ultrasound probe has been calibrated
	bool mHasBeenCalibrated;

	//! The 4x4 homogeneous transform matrices after registration of phantom geometry
	vnl_matrix<double> mTransformMatrixPhantom2DRB4x4;

	//! Data positions collected as inputs for the US calibration
	std::vector< vnl_vector<double> > mDataPositionsInPhantomFrame;
	std::vector< vnl_vector<double> > mDataPositionsInUSProbeFrame;
	std::vector< vnl_vector<double> > mDataPositionsInUSImageFrame;
	std::vector<int> mOutlierDataPositions; 

	//! Validation positions collected to validate the calibration accuracy
	std::vector< vnl_vector<double> > mValidationPositionsInPhantomFrame;
	std::vector< vnl_vector<double> > mValidationPositionsInUSProbeFrame;
	std::vector< vnl_vector<double> > mValidationPositionsInUSImageFrame;

	//! Validation positions for point-line distance
	std::vector< vnl_vector<double> > mValidationPositionsNWireStartInUSProbeFrame;
	std::vector< vnl_vector<double> > mValidationPositionsNWireEndInUSProbeFrame;

	//! Validation positions for parallel wires in US image frame
	std::vector< vnl_vector<double> > mValidationPositionsNWire1InUSImageFrame;
	std::vector< vnl_vector<double> > mValidationPositionsNWire3InUSImageFrame;
	std::vector< vnl_vector<double> > mValidationPositionsNWire4InUSImageFrame;
	std::vector< vnl_vector<double> > mValidationPositionsNWire6InUSImageFrame;

	// In US probe frame
	std::vector< vnl_vector<double> > mValidationPositionsNWire1InUSProbeFrame;
	std::vector< vnl_vector<double> > mValidationPositionsNWire3InUSProbeFrame;
	std::vector< vnl_vector<double> > mValidationPositionsNWire4InUSProbeFrame;
	std::vector< vnl_vector<double> > mValidationPositionsNWire6InUSProbeFrame;

	// NOTE: since the validation data set is separated from the calibration 
	//       data and acquired before the calibration, the construction of
	//       the validation data matrices needs to be done once and only 
	//       once (e.g., before the 1st validation task is performed) with
	//       the flag set to true.  This would save the system runtime
	//       during the iterative calibration/validation process.
	// Validation Positions to compute point-reconstruction errors (PREs)
	vnl_matrix<double> mValidationPositionsInUSImageFrameMatrix4xN;
	vnl_matrix<double> mValidationPositionsInUSProbeFrameMatrix4xN;

	//! Validation Positions to compute independent point-line distance errors (PLDEs)
	vnl_matrix<double> mValidationPositionsNWireStartInUSProbeFrame3xN;
	vnl_matrix<double> mValidationPositionsNWireEndInUSProbeFrame3xN;

	//! Validation Positions to compute independent parallel line reconstruction errors (LREs)
	vnl_matrix<double> mValidationPositionsNWire1InUSImageFrame4xN;
	vnl_matrix<double> mValidationPositionsNWire3InUSImageFrame4xN;
	vnl_matrix<double> mValidationPositionsNWire4InUSImageFrame4xN;
	vnl_matrix<double> mValidationPositionsNWire6InUSImageFrame4xN;
	vnl_matrix<double> mValidationPositionsNWire1InUSProbeFrame4xN;
	vnl_matrix<double> mValidationPositionsNWire3InUSProbeFrame4xN;
	vnl_matrix<double> mValidationPositionsNWire4InUSProbeFrame4xN;
	vnl_matrix<double> mValidationPositionsNWire6InUSProbeFrame4xN;

	//! Flag to set when the validation data matrices are populated 
	bool mAreValidationDataMatricesConstructed;

	//! Weights for the validation positions defined by prior knowledge of the imaging condition
	// E.g.: the ultrasound 3D beamwidth in axial, lateral and elevational axes
	std::vector<double> mWeightsForValidationPositions; //Not in use yet!

	//! Final calibration transform in vnl_matrix format
	// The homogeneous transform matrix from the US image frame to the US probe frame
	vnl_matrix<double> mTransformUSImageFrame2USProbeFrameMatrix4x4;

	//! Validation data confidence level
	// This sets the confidence level (trusted zone) as a percentage
	// of the independent validation data used to produce the final
	// validation results.  It serves as an effective way to get rid
	// of corrupted data (or outliers) in the validation dataset.
	// Default value: 0.95 (or 95%), meaning the top ranked 95% of 
	// the ascendingly-ordered PRE3D values from the validation data 
	// would be accepted as the valid PRE3D values.
	double mValidationDataConfidenceLevel;

	//! 3D point reconstruction error (PRE3D) Analysis for the validation positions in the US probe frame
	// FORMAT: (all positions are in the US probe frame)
	// [ vector 0 - 2:  PRE3D_X_mean, PRE3D_X_rms, PRE3D_X_std ]
	// [ vector 3 - 5:  PRE3D_Y_mean, PRE3D_Y_rms, PRE3D_Y_std ]
	// [ vector 6 - 8:  PRE3D_Z_mean, PRE3D_Z_rms, PRE3D_Z_std ]
	// [ vector 9	 :	Validation data confidence level ]
	// 
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
	std::vector<double> mAbsPRE3DAnalysis4ValidationPositionsInUSProbeFrame;

	//! 3D point reconstruction error (PRE3D) matrix for validation positions in US probe frame
	// This keeps all the original PRE3Ds for the validation dataset with signs in
	// the US probe frame (from the Projected positions to the true positions).
	// FORMAT: matrix 4xN (with N being the total number of validation positions)
	// [ Row-0: PRE3Ds in x-axis from projected to true positions ]
	// [ Row-1: PRE3Ds in y-axis from projected to true positions ]
	// [ Row-2: PRE3Ds in z-axis from projected to true positions ]
	// [ Row-3: should be all zeros ]
	// NOTE: this matrix can be obtained for statistical analysis if desired.
	vnl_matrix<double> mRawPRE3DsforValidationPositionsInUSProbeFrameMatrix4xN;

	//! Sorted 3D point reconstruction error (PRE3D) matrix for validation positions in US probe frame
	// This matrix sorts all the original PRE3Ds for the validation dataset with 
	// signs in the US probe frame in a ascending order with respect the 
	// absolute PRE3D root-squared value (length of the PRE3D vector or the 
	// euclidean distance between the true positions and the project positions).
	// FORMAT: matrix 4xN (with N being the total number of validation positions)
	// [ Row-0: PRE3Ds in x-axis from projected to true positions ]
	// [ Row-1: PRE3Ds in y-axis from projected to true positions ]
	// [ Row-2: PRE3Ds in z-axis from projected to true positions ]
	// [ Row-3: Euclidean PRE3D distance sorted in ascending order]
	// NOTE: this matrix can be obtained for statistical analysis if desired.
	vnl_matrix<double> mSortedRawPRE3DsInAscendingOrderInUSProbeFrameMatrix4xN;

	//! Point-Line Distance Error for validation positions in US probe frame
	// This contains the Point-Line Distance Error (PLDE) for the validation dataset. 
	// The PLDE was defined as the absolute point-line distance from the projected
	// positions to the N-Wire (the ground truth), both in the US probe frame.  
	// If there was no error, the PLDE would be zero and the projected postions
	// would reside right on the N-Wire.  The physical position of the N-Wire
	// was measured based on the phantom geometry and converted into the US
	// US probe frame by the optical tracking device affixed on the phantom.
	// NOTE: this data may be used for statistical analysis if desired.
	// FORMAT: vector 1xN (with N being the total number of validation positions)
	vnl_vector<double> mPLDEsforValidationPositionsInUSProbeFrame;
	vnl_vector<double> mSortedPLDEsAscendingforValidationInUSProbeFrame;

	//! Point-Line Distance Error Analysis for Validation Positions in US probe frame
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
	std::vector<double> mPLDEAnalysis4ValidationPositionsInUSProbeFrame;

	//! Line reconstruction error (LRE) matrix for validation positions in US probe frame
	// This keeps all the original PRE3Ds for the validation dataset with signs in
	// the US probe frame (from the Projected positions to the true positions).
	// FORMAT: matrix 4xN (with N being the total number of validation positions)
	// [ Row-0: PRE3Ds in x-axis from projected to true positions ]
	// [ Row-1: PRE3Ds in y-axis from projected to true positions ]
	// [ Row-2: should be all zeros ]
	// [ Row-3: Euclidean LRE distance ]
	// NOTE: this matrix can be obtained for statistical analysis if desired.
	vnl_matrix<double> mNWire1LREOrigInUSProbeFrameMatrix4xN;
	vnl_matrix<double> mNWire3LREOrigInUSProbeFrameMatrix4xN;
	vnl_matrix<double> mNWire4LREOrigInUSProbeFrameMatrix4xN;
	vnl_matrix<double> mNWire6LREOrigInUSProbeFrameMatrix4xN;

	//! Sorted Line reconstruction error (LRE) matrix for validation positions in US probe frame
	// This matrix sorts all the original LREs for the validation dataset with 
	// signs in the US probe frame in a ascending order with respect the 
	// absolute LRE root-squared value (length of the LRE vector or the 
	// euclidean distance between the true positions and the project positions).
	// FORMAT: matrix 4xN (with N being the total number of validation positions)
	// [ Row-0: LREs in x-axis from projected to true positions ]
	// [ Row-1: LREs in y-axis from projected to true positions ]
	// [ Row-2: should be all zeros ]
	// [ Row-3: Euclidean LRE distance sorted in ascending order]
	// NOTE: this matrix can be obtained for statistical analysis if desired.
	vnl_matrix<double> mNWire1LRESortedAscendingInUSProbeFrameMatrix4xN;
	vnl_matrix<double> mNWire3LRESortedAscendingInUSProbeFrameMatrix4xN;
	vnl_matrix<double> mNWire4LRESortedAscendingInUSProbeFrameMatrix4xN;
	vnl_matrix<double> mNWire6LRESortedAscendingInUSProbeFrameMatrix4xN;

	//! Line Reconstruction Error Analysis for the validation positions in the US probe frame
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
	std::vector<double> mNWire1AbsLREAnalysisInUSProbeFrame;
	std::vector<double> mNWire3AbsLREAnalysisInUSProbeFrame;
	std::vector<double> mNWire4AbsLREAnalysisInUSProbeFrame;
	std::vector<double> mNWire6AbsLREAnalysisInUSProbeFrame;

	//! frames where the LRE is larger than LRE stdev * OutlierDetectionThreshold will be considered as outliers
	double mOutlierDetectionThreshold;

	//! This is the threshold to filter out input data acquired at large beamwidth
	double mNumOfTimesOfMinBeamWidth;

private:
	vtkCalibrationController(const vtkCalibrationController&);
	void operator=(const vtkCalibrationController&);
};

#endif //  __VTKCALIBRATIONCONTROLLER_H

