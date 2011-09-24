#ifndef __VTKCALIBRATIONCONTROLLER_H
#define __VTKCALIBRATIONCONTROLLER_H

#include "vtkObject.h"
#include "vtkTrackedFrameList.h"
#include "vtkMatrix4x4.h"
#include "vtkImageData.h"
#include "itkImage.h"
#include "vnl/vnl_matrix.h"
#include "vnl/vnl_vector.h"
#include <string>
#include <vector>
#include <deque>
#include "vtkXMLDataElement.h"
#include "FidPatternRecognition.h"
#include "FidPatternRecognitionCommon.h"
#include "BrachyTRUSCalibrator.h"
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

	//! Description 
	// Helper structure for storing image dataset info
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

	//! Description 
	// Initialize the calibration controller interface
	virtual PlusStatus Initialize(); 

	//! Description 
	// Read XML based configuration of the calibration controller
	virtual PlusStatus ReadConfiguration( vtkXMLDataElement* configData ); 

	//! Description 
	// Read XML based configuration for probe calibration
  virtual PlusStatus ReadProbeCalibrationConfiguration( vtkXMLDataElement* configData );

  // Write configuration
  virtual PlusStatus WriteConfiguration( vtkXMLDataElement* configData );

	//! Description 
	// Add new tracked data for segmentation and save the segmentation result to the SegmentedFrameContainer
	// The class has to be initialized before the segmentation process. 
	virtual PlusStatus AddTrackedFrameData( TrackedFrame* trackedFrame, IMAGE_DATA_TYPE dataType ); 

	//! Description 
	// VTK/VNL matrix conversion 
	static void ConvertVtkMatrixToVnlMatrixInMeter(vtkMatrix4x4* inVtkMatrix, vnl_matrix<double>& outVnlMatrix ); 

	//! Description 
	// Returns the list of tracked frames of the selected data type
	virtual vtkTrackedFrameList* GetTrackedFrameList( IMAGE_DATA_TYPE dataType ); 
	
	//! Description 
	// Save the selected data type to sequence metafile 
	virtual PlusStatus SaveTrackedFrameListToMetafile( IMAGE_DATA_TYPE dataType, const char* outputFolder, const char* sequenceMetafileName, bool useCompression = false ); 

	// Returns the calibrator
	BrachyTRUSCalibrator* GetCalibrator() { return this->Calibrator; } 

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

	//! Flag to enable the erroneously segmented data saving to metafile
	vtkGetMacro(EnableErroneouslySegmentedDataSaving, bool);
	vtkSetMacro(EnableErroneouslySegmentedDataSaving, bool);
	vtkBooleanMacro(EnableErroneouslySegmentedDataSaving, bool);

	//! Flag to enable the visualization component
	vtkGetMacro(EnableVisualization, bool);
	vtkSetMacro(EnableVisualization, bool);
	vtkBooleanMacro(EnableVisualization, bool);

  //! Description 
	// Get offline image data
	vtkGetObjectMacro(OfflineImageData, vtkImageData);
  
  //! Description 
	// Set/get the calibration date and time in string format
	vtkSetStringMacro(CalibrationDate); 
	vtkGetStringMacro(CalibrationDate);

  //! Description 
	// Set/get the calibration date and time in string format for file names
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

	//! Description 
	// Get the segmentation result container 
	// Stores the segmentation results with transformation for each frame
	SegmentedFrameList GetSegmentedFrameContainer() { return this->SegmentedFrameContainer; };

  //! Description
  //Get the fiducial pattern recognition master object
  FidPatternRecognition * GetPatternRecognition() { return & this->PatternRecognition; };
  void SetPatternRecognition( FidPatternRecognition value) { PatternRecognition = value; };

  //! Description
  //Get the fiducial pattern recognition master object
  PatternRecognitionResult * GetPatRecognitionResult() { return & this->PatRecognitionResult; };
  void SetPatRecognitionResult( PatternRecognitionResult value) { PatRecognitionResult = value; };

  //! Description
  // Clear all datatype segmented frames from container 
  void ClearSegmentedFrameContainer(IMAGE_DATA_TYPE dataType); 

	//! Description 
	// Get/set the saved image data info
	ImageDataInfo GetImageDataInfo( IMAGE_DATA_TYPE dataType ) { return this->ImageDataInfoContainer[dataType]; }
	virtual void SetImageDataInfo( IMAGE_DATA_TYPE dataType, ImageDataInfo imageDataInfo ) { this->ImageDataInfoContainer[dataType] = imageDataInfo; }
	
	//! Description 
	// Callback function that is executed each time a segmentation is finished
	typedef void (*SegmentationProgressPtr)(int percent);
  void SetSegmentationProgressCallbackFunction(SegmentationProgressPtr cb) { SegmentationProgressCallbackFunction = cb; } 

public: // Former ProbeCalibrationController and FreehandCalibraitonController functions
	//! Operation: Register phantom geometry for calibrator 
	virtual void RegisterPhantomGeometry( double phantomToProbeDistanceInMm[2] );
  virtual void RegisterPhantomGeometry( vtkTransform* aPhantomToPhantomReferenceTransform ); 

	//! Operation: Computes the calibration results: 
	// - Compute the overall Point-Line Distance Error (PLDE)
	// - Print the final calibration results and error reports 
	// - Save the calibration results and error reports into a file 
	// - Save the PRE3D distribution plot to an image file
	// - Map the PRE3D distribution onto the US image
	virtual PlusStatus ComputeCalibrationResults();

  //! Operation 
	// Check user image home to probe home transform orthogonality 
  virtual bool IsUserImageToProbeTransformOrthogonal(); 

	//! Operation: Read and populate US to Template calibration image data in offline mode
	virtual PlusStatus OfflineUSToTemplateCalibration();  
		
	//! Operations: get the wire position of the the US frame and phantom intersection in template coordinate system
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
	virtual vtkstd::vector<double> GetPRE3DVector() { return this->Calibrator->getPRE3DAnalysis4ValidationDataSet(); } 

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
	virtual vnl_matrix<double> GetPRE3DMatrix() { return this->Calibrator->getPRE3DsRAWMatrixforValidationDataSet4xN(); } 

	// The calibration transform in 4x4 homogeneous matrix.
	// Transform: from the US image frame to the US probe frame.
	virtual vnl_matrix<double> GetCalibrationMatrix() { return this->Calibrator->getCalibrationResultsInMatrix(); }

	// The calibration transform in 8 parameters calculated from the original transformation matrix.
	//	The parameters are in the following format:
	// - EulerZYX Angles (in radians):		0-Alpha,  1-Beta,  2-Gamma 
	// - Scaling Factors (in mm/pixel):		3-Sx,	  4-Sy
	// - Translation of Origin (in mm):		5-Tx,	  6-Ty,	   7-Tz
	virtual vtkstd::vector<double> GetCalibrationParameters() { return this->Calibrator->getCalibrationResultsInParameters(); }

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
	vnl_vector<double> GetPointLineDistanceErrorVector() { return this->Calibrator->getOrigPLDEsforValidationDataSet(); }
	vnl_vector<double> GetPointLineDistanceErrorSortedVector() { return this->Calibrator->getSortedPLDEsforValidationDataSet(); }

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
	std::vector<double> GetPointLineDistanceErrorAnalysisVector() { return this->Calibrator->getPLDEAnalysis4ValidationDataSet(); } 

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
  PlusStatus UpdateLineReconstructionErrorAnalysisVectors();  
	PlusStatus GetLineReconstructionErrorAnalysisVector(int wireNumber, std::vector<double> &LRE);  

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

	//! Attributes: Minimum US elevation beamwidth and the focal zone in US Image Frame
	// 1. For a typical 1-D linear-array transducer, the ultrasound beam can only be 
	//    focused mechanically in the elevation (out-of-plane) axis by placing an 
	//    ascoustic lens in front or curving the crystal surface.  
	// 2. The elevation beam pattern are therefore solely determined by the size and 
	//    curvature of the crystal or the acoustic lens, with sharp focus only 
	//    possible at a narrow axial distance (the focal zone) to the transducer.
	// FORMAT: [FOCAL-ZONE (in US Image Frame), MINI-ELEVATION-WIDTH (in millimeters)]
	vnl_vector<double> *GetMinElevationBeamwidthAndFocalZoneInUSImageFrame() { return &this->MinElevationBeamwidthAndFocalZoneInUSImageFrame;}

	vnl_matrix<double>* GetSortedUS3DBeamwidthInAscendingAxialDepth2CrystalsMatrixNx4() { return &this->SortedUS3DBeamwidthInAscendingAxialDepth2CrystalsMatrixNx4; }

	// Interpolated US 3D beamwidth profile and weight calulated based on it
	// Here we used a simple linear interpolation between sampled data to obtain
	// the beamwidth at non-sampled axial depth.  In general, this approach works
	// fine as long as we have enough sample points that covers the entire US field.
	// FORMAT: each row of the matrix has the following columns:
	// [0]:		Ascending Axial Depth in US Image Frame (in pixels);
	// [1-3]:	Beamwith in axial, lateral and elevational axes respectively (in mm);
	// [4]:		Weight Factor = CurrentBeamWidth/MininumBeamWidth (>=1).
	vnl_matrix<double> *GetInterpUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xM() { return &this->InterpUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xM; }

	//! Attributes: US 3D beamwidth profile data in US Image Frame with weights factors along axial depth.
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
	vnl_matrix<double> *GetSortedUS3DBeamwidthAndWeightFactorsInAscendingAxialDepthInUSImageFrameMatrix5xN() { return &this->SortedUS3DBeamwidthAndWeightFactorsInAscendingAxialDepthInUSImageFrameMatrix5xN; }

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

	// Read freehand calibration configurations (from probe calibration data element of the config file)
	virtual PlusStatus ReadFreehandCalibrationConfiguration(vtkXMLDataElement* probeCalibration);

	//! Read and populate US to Template calibration image data in offline mode
	virtual PlusStatus DoOfflineCalibration();

  //! Description: 
	// Set/get phantom to probe distance in mm
  vtkSetVector2Macro(PhantomToProbeDistanceInMm, double); 
	vtkGetVector2Macro(PhantomToProbeDistanceInMm, double);

	//! Attribute: Flag to set if the US 3D beamwidth data is sucessfully loaded
	vtkGetMacro(US3DBeamwidthDataReady, bool);
	vtkSetMacro(US3DBeamwidthDataReady, bool);
	vtkBooleanMacro(US3DBeamwidthDataReady, bool);

	//! Attribute: Flag to enable the saving of segmented wire positions to file
	vtkGetMacro(EnableSegmentedWirePositionsSaving, bool);
	vtkSetMacro(EnableSegmentedWirePositionsSaving, bool);
	vtkBooleanMacro(EnableSegmentedWirePositionsSaving, bool);

	//! Attribute: Flags to incorporate the ultrasound 3D beam profile (beam width)
	// The flag is set when the 3D US beam width data is to be incorporated into
	// the calibration process (e.g., by adding weights to the least-squares 
	// optimization method w.r.t the beam width profile).
	// NOTE: if the flag is turned on (default is 0), then a US-3D-beam-
	// profile data file needs to be prepared for further processing.
	// OPTIONS: 0 - NO | 1 - BeamwidthVariance | 2 - BeamwidthRatio | 3 - BeamwidthVarianceAndThresholding
	vtkGetMacro(IncorporatingUS3DBeamProfile, int);
	vtkSetMacro(IncorporatingUS3DBeamProfile, int);

	//! Attributes: Axial position of the crystal surface in the TRUS Image Frame
	// Typically, the US machine has a bright mark in the display of the US image 
	// indicating the actual position where the sound starts propagation.
	// NOTE: this position has been converted to the TRUS Image Frame being defined
	// by the user in the calibration configuration file.
	vtkGetMacro(AxialPositionOfCrystalSurfaceInTRUSImageFrame, double);
	vtkSetMacro(AxialPositionOfCrystalSurfaceInTRUSImageFrame, double);

	//! Attributes: Number of the US 3D beamwidth profile data
	// This is the total number of US 3D-beam-width data (samples) collected.
	vtkGetMacro(NumUS3DBeamwidthProfileData, int);
	vtkSetMacro(NumUS3DBeamwidthProfileData, int);

	//! Attribute: Track the current position ID of the output in PRE3D distribution data
	vtkGetMacro(CurrentPRE3DdistributionID, int);
	vtkSetMacro(CurrentPRE3DdistributionID, int);

	//! Attribute: calibration config file name
	vtkGetStringMacro(CalibrationConfigFileNameWithPath);
	vtkSetStringMacro(CalibrationConfigFileNameWithPath);

	//! Attribute: calibration result file name
	vtkGetStringMacro(CalibrationResultFileNameWithPath);
	vtkSetStringMacro(CalibrationResultFileNameWithPath);

	//! Attribute: Segmentation error log file name with timestamp
	vtkGetStringMacro(SegmentationErrorLogFileNameWithTimeStamp);
	vtkSetStringMacro(SegmentationErrorLogFileNameWithTimeStamp);

	//! Attribute: Segmentation analysis file name with timestamp
	vtkGetStringMacro(SegmentationAnalysisFileNameWithTimeStamp);
	vtkSetStringMacro(SegmentationAnalysisFileNameWithTimeStamp);
	
	//! Attributes: US 3D beam profile name and path
	vtkGetStringMacro(US3DBeamProfileDataFileNameAndPath); 
	vtkSetStringMacro(US3DBeamProfileDataFileNameAndPath); 

	//! Attributes: suffix of the data files
	vtkGetStringMacro(DataFileSuffix);
	vtkSetStringMacro(DataFileSuffix);

	//! Attributes: segmented wire positions of calibration dataset
	vtkGetStringMacro(CalibrationSegWirePosInfoFileName);
	vtkSetStringMacro(CalibrationSegWirePosInfoFileName);

	//! Attributes: segmented wire positions of validation dataset
	vtkGetStringMacro(ValidationSegWirePosInfoFileName);
	vtkSetStringMacro(ValidationSegWirePosInfoFileName);
	
	//! Attributes: suffix of the calibration result file
	vtkGetStringMacro(CalibrationResultFileSuffix);
	vtkSetStringMacro(CalibrationResultFileSuffix);
	
	//! Attributes: suffix of the segmentation error log file 
	vtkGetStringMacro(SegmentationErrorLogFileNameSuffix);
	vtkSetStringMacro(SegmentationErrorLogFileNameSuffix);
	
	//! Attributes: suffix of the segmentation analysis file
	vtkGetStringMacro(SegmentationAnalysisFileNameSuffix);
	vtkSetStringMacro(SegmentationAnalysisFileNameSuffix);

	//! Attributes: suffix of the template to stepper calibration analysis file
	vtkGetStringMacro(Temp2StepCalibAnalysisFileNameSuffix);
	vtkSetStringMacro(Temp2StepCalibAnalysisFileNameSuffix);

	//! Attributes: Get/set final calibration transform 
	vtkGetObjectMacro(TransformImageToTemplate, vtkTransform);
	vtkSetObjectMacro(TransformImageToTemplate, vtkTransform);

	//! Attributes: Get/set image home to user defined image home constant transform
	// Should be defined in config file
	vtkGetObjectMacro(TransformImageToUserImage, vtkTransform);
	vtkSetObjectMacro(TransformImageToUserImage, vtkTransform);
	
	//! Attributes: Get/set the iCAL calibration result transformation 
	// between the user defined image home and probe home position 
	vtkGetObjectMacro(TransformUserImageToProbe, vtkTransform);
	vtkSetObjectMacro(TransformUserImageToProbe, vtkTransform);
	
	//! Attributes: Get/set transformation between the probe home and
	// probe position (e.g read from stepper)
	vtkGetObjectMacro(TransformProbeToReference, vtkTransform);
	vtkSetObjectMacro(TransformProbeToReference, vtkTransform);
	
	//! Attributes: Get/set the transformation between probe home and 
	// template holder home position. 
	// Result of the Template To Stepper Calibration 
	vtkGetObjectMacro(TransformReferenceToTemplateHolderHome, vtkTransform);
	vtkSetObjectMacro(TransformReferenceToTemplateHolderHome, vtkTransform);

	//! Attributes: Get/set the constant transformation between template holder home 
	// and template home position
	// Should be defined in config file
	vtkGetObjectMacro(TransformTemplateHolderToTemplate, vtkTransform);
	vtkSetObjectMacro(TransformTemplateHolderToTemplate, vtkTransform);
	
  //! Attributes: Get/set the constant transformation between template holder home 
	// and template home position
	// Should be defined in config file
  vtkGetObjectMacro(TransformTemplateHolderToPhantom, vtkTransform);
	vtkSetObjectMacro(TransformTemplateHolderToPhantom, vtkTransform);
	
	//! Attributes: Get/set the transformation between template home and template position
	// Should be identical to TransformTemplateHolderHomeToTemplateHolder transformation 
	vtkGetObjectMacro(TransformTemplateHomeToTemplate, vtkTransform);
	vtkSetObjectMacro(TransformTemplateHomeToTemplate, vtkTransform);

  //! Description: 
	// Set/Get the rotation center in pixels.
	// Origin: Left-upper corner (the original image frame)
	// Positive X: to the right;
	// Positive Y: to the bottom;
	vtkSetVector2Macro(CenterOfRotationPx, int); 
	vtkGetVector2Macro(CenterOfRotationPx, int);
		
protected:

	vtkCalibrationController();
	virtual ~vtkCalibrationController();

	//! Operation: Add frame to renderer in offline mode
	virtual PlusStatus SetOfflineImageData(vtkImageData* frame); 
	virtual PlusStatus SetOfflineImageData(const ImageType::Pointer& frame); 

	//! Description 
	// Read CalibrationController data element
	virtual PlusStatus ReadCalibrationControllerConfiguration(vtkXMLDataElement* rootElement); 

	// Populate the segmented N-fiducials to the data container
	virtual void PopulateSegmentedFiducialsToDataContainer(vnl_matrix<double> &transformUSProbe2StepperFrameMatrix4x4, IMAGE_DATA_TYPE dataType); 

	// Call the calibrator class and do the calibration process
	virtual void DoCalibration(); 

	//! Operation: print the calibration results as well as error reports to the stdout
	virtual void PrintCalibrationResultsAndErrorReports();

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

  //! Stores the fiducial pattern recognition master object
  FidPatternRecognition PatternRecognition;

  //! Stores the segmentation results of a single frame
  PatternRecognitionResult PatRecognitionResult;

  //! Stores the image data in offline mode
  vtkImageData* OfflineImageData; 

protected: // Former ProbeCalibrationController and FreehandCalibraitonController members
	//! Attribute: a reference to the calibration phantom
	BrachyTRUSCalibrator* Calibrator;

	//! Attributes: a reference to the calibration controller IO
	vtkProbeCalibrationControllerIO* CalibrationControllerIO; 

	//! Attribute: Flag to enable the saving of segmented wire positions to file
	bool EnableSegmentedWirePositionsSaving; 

	// Flag to set if the US 3D beamwidth data is sucessfully loaded
	bool US3DBeamwidthDataReady;

	//! Attribute: Flags to incorporate the ultrasound 3D beam profile (beam width)
	// The flag is set when the 3D US beam width data is to be incorporated into
	// the calibration process (e.g., by adding weights to the least-squares 
	// optimization method w.r.t the beam width profile).
	// NOTE: if the flag is turned on (default is 0), then a US-3D-beam-
	// profile data file needs to be prepared for further processing.
	// OPTIONS: 0 - NO | 1 - BeamwidthVariance | 2 - BeamwidthRatio | 3 - BeamwidthVarianceAndThresholding
	int IncorporatingUS3DBeamProfile;

	//! Attributes: Axial position of the crystal surface in the TRUS Image Frame
	// Typically, the US machine has a bright mark in the display of the US image 
	// indicating the actual position where the sound starts propagation.
	// NOTE: this position has been converted to the TRUS Image Frame being defined
	// by the user in the calibration configuration file.
	double AxialPositionOfCrystalSurfaceInTRUSImageFrame;

	//! Attributes: Number of the US 3D beamwidth profile data
	// This is the total number of US 3D-beam-width data (samples) collected.
	int NumUS3DBeamwidthProfileData;

	//! Attribute: Track the current position ID of the output in PRE3D distribution data
	int CurrentPRE3DdistributionID;

  int CenterOfRotationPx[2]; 

	//! Attribute: calibration config file name
	char* CalibrationConfigFileNameWithPath;

	//! Attribute: calibration result file name
	char* CalibrationResultFileNameWithPath; 

	//! Attribute: Segmentation error log file name with timestamp
	char* SegmentationErrorLogFileNameWithTimeStamp;

	//! Attribute: Segmentation analysis file name with timestamp
	char* SegmentationAnalysisFileNameWithTimeStamp;

	//! Attributes: US 3D beam profile name and path
	char* US3DBeamProfileDataFileNameAndPath;

	//! Attributes: suffix of the data files 
	char* DataFileSuffix; 
	
	//! Attributes: suffix of the calibration result file
	char* CalibrationResultFileSuffix; 
	
	//! Attributes: suffix of the segmentation error log file 
	char* SegmentationErrorLogFileNameSuffix; 
	
	//! Attributes: suffix of the segmentation analysis file
	char* SegmentationAnalysisFileNameSuffix; 

	//! Attributes: suffix of template to stepper calibration analysis file
	char* Temp2StepCalibAnalysisFileNameSuffix; 

	//! Attributes: segmented wire positions of calibration dataset
	char* CalibrationSegWirePosInfoFileName; 

	//! Attributes: segmented wire positions of validation dataset
	char* ValidationSegWirePosInfoFileName; 
	
	vnl_vector<double> MinElevationBeamwidthAndFocalZoneInUSImageFrame;

	vnl_matrix<double> SortedUS3DBeamwidthAndWeightFactorsInAscendingAxialDepthInUSImageFrameMatrix5xN;

	vnl_matrix<double> SortedUS3DBeamwidthInAscendingAxialDepth2CrystalsMatrixNx4; 

	vnl_matrix<double> InterpUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xM;

  double PhantomToProbeDistanceInMm[2]; 

	vtkTransform * TransformImageToTemplate;						// Final calibration transform 
	vtkTransform * TransformImageToUserImage;				// Constant transform, read from file 
	vtkTransform * TransformUserImageToProbe;				// The result of iCAL 
	vtkTransform * TransformProbeToReference;						// Read from stepper (tool number 1)
	//vtkTransform * TransformProbeToUserImage;						// Invert of TransformUserImageHomeToProbeHome
	//vtkTransform * TransformUserImageToImage;						// Invert of TransformImageHomeToUserImageHome
	vtkTransform * TransformReferenceToTemplateHolderHome;			// Result of the TemplateToStepperCalibrator
	vtkTransform * TransformTemplateHolderToTemplate;		// Constant transform (specific to the current template), read from file 
  vtkTransform * TransformTemplateHolderToPhantom;		// Constant transform (specific to the current calibration phantom), read from file 
	//vtkTransform * TransformTemplateHolderHomeToTemplateHolder;		// Read from stepper (tool number 0) -> TODO: 
	vtkTransform * TransformTemplateHomeToTemplate;					// Read from stepper (tool number 0), identical to TransformTemplateHolderHomeToTemplateHolder

  std::map<int, std::vector<double> > LineReconstructionErrors; 

private:
	vtkCalibrationController(const vtkCalibrationController&);
	void operator=(const vtkCalibrationController&);
};

#endif //  __VTKCALIBRATIONCONTROLLER_H

