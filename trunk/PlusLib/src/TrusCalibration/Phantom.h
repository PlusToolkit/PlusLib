#ifndef PHANTOM_H
#define PHANTOM_H
// ===========================================================================
// Program Modification Record
// ===========================================================================
// Component:	Component_AutoTRUSCalibration
// ===========================================================================
// Class:		Phantom
// ===========================================================================
// File Name:	Phantom.h
// ===========================================================================
// Author:		Thomas Kuiran Chen <chent@cs.queensu.ca>
//				School of Computing, Queen's University at Kingston, Canada
// ===========================================================================
// Purposes: 
// 1. This the parent class of all current or future calibration phantoms.
// 2. This class defines some generic features and behaviors that one would
//    typically expect from an ultrasound probe calibration phantom.  By 
//    inheriting (or extending) this class, the child class would only need
//    to define some specific features of its own (e.g., the unique phantom 
//    geometries that differ from phantom to phantom).
// 3. Uses the open source VXL-VNL (Numerical and Algebra C++ Library)
// 4. Uses C++ Standard Library and Standard Template Library
// ===========================================================================
// Change History:
//
// Author				Time						Release	Changes
// Thomas Kuiran Chen	Mon Mar 1 08:19 EST 2010	1.0		Creation
//
// ===========================================================================
//					  Copyright @ Thomas Kuiran Chen, 2010
// ===========================================================================


// C++ Standard Includes
#include <string>
#include <vector>
#include <cmath>		

// VXL/VNL Includes
#include "vnl/vnl_matrix.h"
#include "vnl/vnl_vector.h"
#include "vnl/vnl_cross.h"


//======================================================================
// class Phantom
//====================================================================== 
class Phantom
{
	public:

		//! VNL vector in double format
		typedef vnl_vector<double> vnl_vector_double;		

		//! Strings
		static const std::string mstrScope;
		static const std::string mstrCalibrationLogFileNameSuffix;

		//! Constants
		static const double mPI;

		//! Constructor with flag to turn on the system logging service
		// 1. A system log file with its name associated with a system 
		//    timestamp, when turned on, is used to record all the data pipeline 
		//    processing during the calibration.
		// 2. The system log should be turned on before any calibration related 
		//    activities (e.g., add data positions, validating, calibrating, etc.).
		// [DEFAULT: OFF (FALSE)]
		Phantom ( const bool IsSystemLogOn = false );

		//! Destructor
		~Phantom ();

		//! Operation: set the ultrasound image frame in pixels.
		// IMPORTANT:
		// 1. This operation defines the US image frame origin in pixels W.R.T. 
		//    the left-upper corner of the original image, with X pointing to 
		//    the right (column) and Y pointing down to the bottom (row).
		// 2. It is important to note that here the left-upper corner of an
		//    image that we used was the standard way to define an image origin,
		//	  which was different than VTK Image Reader that uses left-lower
		//    corner (a way to comform to Computer Graphics as VTK is designed
		//    for visualization in the first place).
		void setUltrasoundImageFrameOriginInPixels(
			const double USImageFrameOriginXInPixels,
			const double USImageFrameOriginYInPixels );

		double getUSImageFrameOriginXInPixels() { return mUSImageFrameOriginInPixels.get(0); }
		double getUSImageFrameOriginYInPixels() { return mUSImageFrameOriginInPixels.get(1); }

		//! Operation: set the transform matrix from orig image frame to TRUS image frame
		// IMPORTANT:
		// 1. This operation defines the 4x4 homogeneous transform matrix that transfers
		//    a pixel location from the original image frame to the TRUS image frame.
		// 2. Note the original image frame conforms to the standard way to define an image
		//    with the image origin set at the top-left corner, positive X axis to the 
		//    right and positive Y axis to the bottom of the image.  This is different than 
		//    VTK Image Reader that uses left-lower corner (a way to comform to Computer 
		//    Graphics as VTK is designed for visualization in the first place).
		// 3. The TRUS image frame is on the other hand completely defined by the user or
		//    the application.  The default is: origin at the middle-bottom of the imaeg,
		//    with the positive X to the right and positive Y to the top of the image. This
		//    conforms to the standard way when TRUS is used during imaging (e.g., in
		//    brachytherapy prostate cancer treatment).
		void setTransformOrigImageToTRUSImageFrame4x4(
			const vnl_matrix<double> TransformOrigImageFrame2TRUSImageFrameMatrix4x4 );

		//! Operation: set the ultrasound beamwidth profile and weight factors
		// 1. This operation (optional) will set the ultraosund 3D beamwidth profile
		//    data and the weight factors calculated based on it. 
		//	  Note: the 3nd parameter passes the original US 3D beamwidth profile
		//          before interpolation, just for reference and logging purposes.
		// 2. If this operation is called, the calibration process will incorporate
		//    the US 3D beamwidth to its calculation.
		// 3. 3D beam width samples are measured at various axial depth/distance away 
		//    from the transducer crystals surface, i.e., the starting position of 
		//    the sound propagation in an ultrasound image.
		// 4. We have three ways to incorporate the US beamidth to the calibration: Use the 
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
		// FORMAT: each column in the matrix has the following rows:
		// [0]:		Sorted in ascending axial depth in US Image Frame (in pixels);
		// [1-3]:	Beamwith in axial, lateral and elevational axes respectively (in mm);
		// [4]:		Weight Factor = CurrentBeamWidth/MininumBeamWidth (>=1).
		void setUltrasoundBeamwidthAndWeightFactorsTable(
			const int FlagOnHowToIncorporateBeamWidth,
			vnl_matrix<double> const & US3DBeamwidthAndWeightFactorsInUSImageFrameTable,
			vnl_matrix<double> const & OrigUS3DBeamwidthAndWeightFactorsInUSImageFrameTable,
			vnl_vector<double> const & MinimumUSElevationBeamwidthAndFocalZoneInUSImageFrame );

		//! Operation: get the list of names of the phantom specific reference points.
		// Every phantom has its own geometry and defined reference points to relate the 
		// phantom geometry (w.r.t the phantom frame) to the tracker's (or DRB's) frame.
		std::vector<std::string> getNamesOfPhantomSpecificReferencePoints ();

		//! Operation: register the phantom geometry to the DRB frame (mounted on the phantom)
		// This will register the specific phantom geometric points from the
		// predefined physical Phantom frame to the DRB frame (affixed on the phantom with an
		// tracked target (e.g., a traxtal DRB).  The inputs are a list of reference points
		// in the affixed DRB frame. The operation calculates the transform matrix from the 
		// Phantom frame to the mounted DRB frame given the inputs.  
		// NOTE: The operation must be called every time the phantom is used in calibration 
		// (especially when the DRB is removed from and remounted on the phantom between 
		// calibrations).
		void registerPhantomGeometry( 
			const std::vector<vnl_vector_double> PhantomSpecificReferencePointsInDRBFrame );

		//! Operation: register the phantom geometry to the DRB frame in the "Emulator" mode.
		// In the "Emulator" mode, the phantom is not required to registered on-the-fly 
		// to the DRB, instead, a previously recorded registration transform would be used.  
		// NOTE:
		// - The "Emulator" mode is very helpful during the testing stage or demo to 
		//   examine or show the functionalities of the calibration component.  
		// - In the "Emulator" mode, as long as the input data and the registration
		//   transform are all ready, no hardware tracking is required.
		void registerPhantomGeometryInEmulatorMode( 
			const vnl_matrix<double> TransformMatrixPhantom2DRB4x4 );

		//! Operations: Calculate and add positions in Phantom frame
		// This operation will use the segmented N-fiducials in each US image to 
		// calculate the position in Phantom frame base on the existing phantom 
		// geometry data.  The result would be added contineously into a private 
		// class data storage container.  Note: this operation is performed on 
		// one individual US image captured on Phantom, so natually, it can be 
		// called repeatedly to add in reference positions in multiple images 
		// in order to enhance the calibration accuracy.
		// IMPORTANT:
		// This is a very critical point that would easily cause confusions
		// or mistakes.  There is one and only one fixed correspondence between 
		// the 6 segmented image positions (N-fiducials) and the N-wires (look
		// at 3D view of the N-wire phantom (/phantom/3DViewOfPhantomGeometry.png)
		// for a visual understanding).
		// NOTE: N-fiducials are those positions where the US image plane 
		// intersects the N-wires therefore appear to be a bright spot (fiducial) 
		// in the US images.
		// - Segmented N-fiducial-0 is on N-wire 1;
		// - Segmented N-fiducial-1 is on N-wire 2;
		// - Segmented N-fiducial-2 is on N-wire 3;
		// - Segmented N-fiducial-3 is on N-wire 4;
		// - Segmented N-fiducial-4 is on N-wire 5;
		// - Segmented N-fiducial-5 is on N-wire 6;
		//
		// Interface-1: transformation of the image is given from
		//              the US Probe frame to the DRB frame.
		virtual void addDataPositionsPerImage( 
			std::vector<vnl_vector_double> SegmentedDataPositionListPerImage, 
			const vnl_matrix<double> TransformMatrixUSProbe2DRB4x4 ) = 0;
		// Interface-2: transformation of the image is given from
		//              - the US Probe frame to the Tracker frame, and
		//				- the DRB frame to the Tracker frame.
		virtual void addDataPositionsPerImage( 
			std::vector<vnl_vector_double> SegmentedDataPositionListPerImage, 
			const std::vector<double> TransformUSProbe2Tracker,
			const std::vector<double> TransformDRB2Tracker ) = 0;

		//! Operation: Delete the data positions that were added in the last image.
		// This operation is helpful when the latest added input data does not actually
		// improve the calibration accuracy (i.e., judged by PRE3D).  For example, this 
		// could happen when the image quality is poor and typically results in an 
		// inaccurate segmentation, which in turn negatively influnce the final 
		// calibration outcome.  Using this method, the application can choose to 
		// eliminate a bad input data (e.g., in an iterative calibration process where
		// the calibration accuracy is evaluated in real time for each iteration).
		virtual void deleteLatestAddedDataPositionsPerImage () = 0;
		virtual void deleteDataPositionsPerImage(int position) = 0; 

		//! Operation: Add positions per image to validate calibration accuracy
		// This operation performs similarly to the ::addDataPositionsPerImage
		// method, except that these added positions are stored to validate the 
		// overall calibration accuracy.  These data positions are kept 
		// separately from those that are used for the calibration so as not to 
		// be biased toward the calibration results in the validation process.
		// Note: like ::addDataPositionsPerImage, this operation is performed 
		// on individual US images acquired from the phantom, so natually, it 
		// can be called repeatedly to add in as many data necessary to enhance 
		// the calibration accuracy.
		//
		// Interface-1: transformation of the image is given from
		//              the US Probe frame to the DRB frame
		virtual void addValidationPositionsPerImage( 
			std::vector<vnl_vector_double> SegmentedDataPositionListPerImage, 
			const vnl_matrix<double> TransformMatrixUSProbe2DRB4x4 ) = 0;
		// Interface-2: transformation of the image is given from
		//              - the US Probe frame to the Tracker frame, and
		//				- the DRB frame to the Tracker frame.
		virtual	void addValidationPositionsPerImage( 
			std::vector<vnl_vector_double> SegmentedDataPositionListPerImage, 
			const std::vector<double> TransformUSProbe2Tracker,
			const std::vector<double> TransformDRB2Tracker ) = 0;

		//! Operation: get data positions collected as inputs for the US calibration
		std::vector<vnl_vector_double> getDataPositionsInPhantomFrame() { 
			return mDataPositionsInPhantomFrame; 
		}
		std::vector<vnl_vector_double> getDataPositionsInUSProbeFrame() {
			return mDataPositionsInUSProbeFrame;
		}
		std::vector<vnl_vector_double> getDataPositionsInUSImageFrame() {
			return mDataPositionsInUSImageFrame;
		}
	
		//! Operation: Set the validation data confidence level
		// This sets the confidence level (trusted zone) as a percentage
		// of the independent validation data used to produce the final
		// validation results.  It serves as an effective way to get rid
		// of corrupted data (or outliers) in the validation dataset.
		// Default value: 0.95 (or 95%), meaning the top ranked 95% of 
		// the ascendingly-ordered PRE3D values from the validation data 
		// would be accepted as the valid PRE3D values.
		void setValidationDataConfidenceLevel( const double ValidationDataConfidenceLevel );

		//! Operation: return the number of data used for calibration
		int getNumberOfDataForCalibration ();

		//! Operation: return the number of data used for validation
		int getNumberOfDataForValidation ();

		//! Operation: check to see if the data is ready for calibration
		// IMPORTANT:
		// - The calibration phantom requires at least a certain amount of data populated
		//   in order to perform a successful calibration.
		//	 For example, for our freehand DoubleN (Kingston) phantom, at least four sets 
		//   of reference positions must be successfully populated before calibration 
		//   process could proceed.
		// - Use this interface to check if the data positions are ready for calibration.
		bool areDataPositionsReadyForCalibration ();

		//! Operation: perform the final calibration task and output the results.
		// Given the input data (the data positions in both the US image frame and the US
		// probe frame, a least-squares based algorithm is employ to find the optimal
		// homogeneous transformation matrix from the US image frame to the US probe frame.
		void calibrate ();

		//! Operation
		// It calculates the 3D point reconstruction error (PRE3D) from
		// the validation data sets if they are imported and ready.
		virtual void compute3DPointReconstructionError ();
		//! Operation
		// This operation also computes the Point-Line Distance Error (PLDE) from
		// the validation data sets.  It reconstructs the NWire point positions
		// in the N-wire Phantom space using the existing calibration parameters
		// and then calculates the point-to-line distance to the physical wire
		// location that has been precisly measured on the phantom geometry.
		void computeIndependentPointLineReconstructionError ();

		//! Operation 
		// Get Line reconstruction error (LRE) vector for validation positions in US probe frame
		// FORMAT:
		// [ 0: PRE3Ds in x-axis from projected to true positions ]
		// [ 1: PRE3Ds in y-axis from projected to true positions ]
		vnl_vector<double> getPointLineReconstructionError(vnl_vector<double> NWirePositionInUSImageFrame, 
			vnl_vector<double> NWirePositionInUSProbeFrame);  

		//! Operation: calculate the 3D point reconstruction error (PRE3D) for a given image.
		// This operation obtain the RRE3D for a given image (captured in real time) using
		// the latest updated calibration result.
		//
		// NOTE: The purpose of this method is to enable a real-time recursive calibration
		// control process to iteratively examine the current calibration accuracy using
		// a fresh real-time image data before the image could be further used for 
		// the calibration.  So the real-time image data would be used for validation 
		// while still remains independant to the calibration process (since the 
		// validation takes place before the data is used).
		// 
		// FORMAT: PRE3D are averaged for all the data positions in the image and given
		// in the following format:
		// [vector 0 - 2   : Averaged absolute PRE3D in the US probe frame in X,Y and Z axis ]
		// [vector 3,4,... : Back-projected positions in the US image frame in (Xi, Yi) ...]
		virtual std::vector<double> getPRE3DforRealtimeImage(			
			std::vector<vnl_vector_double> SegmentedDataPositionListPerImage, 
			const vnl_matrix<double> TransformMatrixUSProbe2DRB4x4 ) const = 0;

		//! Operation: Obtain the 3D point reconstruction error (PRE3D) Analysis for the validation data set
		// RETURNED-DATA FORMAT: (all positions are in the US probe frame)
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
		std::vector<double> getPRE3DAnalysis4ValidationDataSet () const;

		//! Operation: Obtain the 3D point reconstruction error (PRE3D) matrix using the validation data set
		// This will return the original PRE3Ds matrix for the validation dataset with signs 
		// in the US probe frame (from the Projected positions to the true positions).
		// FORMAT: matrix 4xN (with N being the total number of validation positions)
		// [ Row-0: PRE3Ds in x-axis from projected to true positions ]
		// [ Row-1: PRE3Ds in y-axis from projected to true positions ]
		// [ Row-2: PRE3Ds in z-axis from projected to true positions ]
		// [ Row-3: should be all zeros ]
		// NOTE: the operation is useful to obtain all PRE3Ds for statistical analysis if desired.
		vnl_matrix <double> getPRE3DsRAWMatrixforValidationDataSet4xN () const;

		//! Operation: Obtain the sorted 3D point reconstruction error (PRE3D) matrix using the validation data set
		// This will return the sorted PRE3D matrix using validation dataset with 
		// signs in the US probe frame in a ascending order with respect the 
		// absolute PRE3D root-squared value (length of the PRE3D vector or the 
		// euclidean distance between the true positions and the project positions).
		// FORMAT: matrix 4xN (with N being the total number of validation positions)
		// [ Row-0: PRE3Ds in x-axis from projected to true positions ]
		// [ Row-1: PRE3Ds in y-axis from projected to true positions ]
		// [ Row-2: PRE3Ds in z-axis from projected to true positions ]
		// [ Row-3: Euclidean PRE3D distance sorted in ascending order]
		// NOTE: the operation is useful to obtain all PRE3Ds for statistical analysis if desired.
		vnl_matrix <double> getSortedPRE3DsRAWMatrixforValidationDataSet4xN () const;

		//! Operations: Obtain the original and sorted Point-Line Distance Error for the validation dataset
		// This returns the Point-Line Distance Error (PLDE) for the validation dataset. 
		// The PLDE was defined as the absolute point-line distance from the projected
		// positions to the N-Wire (the ground truth), both in the US probe frame.  
		// If there was no error, the PLDE would be zero and the projected postions
		// would reside right on the N-Wire.  The physical position of the N-Wire
		// was measured based on the phantom geometry and converted into the US
		// US probe frame by the optical tracking device affixed on the phantom.
		// NOTE: this data may be used for statistical analysis if desired.
		// FORMAT: vector 1xN (with N being the total number of validation positions)
		vnl_vector<double> getOrigPLDEsforValidationDataSet () const;
		vnl_vector<double> getSortedPLDEsforValidationDataSet () const;

		//! Operation: Obtain the Point-Line Distance Error Analysis for Validation Positions in US probe frame
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
		std::vector<double> getPLDEAnalysis4ValidationDataSet () const;

		//! Operation: Obtain Line reconstruction error (LRE) matrix for validation positions in US probe frame
		// This keeps all the original PRE3Ds for the validation dataset with signs in
		// the US probe frame (from the Projected positions to the true positions).
		// FORMAT: matrix 4xN (with N being the total number of validation positions)
		// [ Row-0: PRE3Ds in x-axis from projected to true positions ]
		// [ Row-1: PRE3Ds in y-axis from projected to true positions ]
		// [ Row-2: should be all zeros ]
		// [ Row-3: Euclidean LRE distance ]
		// NOTE: this matrix can be obtained for statistical analysis if desired.
		vnl_matrix <double> getLREOrigMatrix4xNForNWire1 () const;
		vnl_matrix <double> getLREOrigMatrix4xNForNWire3 () const;
		vnl_matrix <double> getLREOrigMatrix4xNForNWire4 () const;
		vnl_matrix <double> getLREOrigMatrix4xNForNWire6 () const;

		//! Operation: Obtain Sorted Line reconstruction error (LRE) matrix for validation positions in US probe frame
		// This matrix sorts all the original LREs for the validation dataset with 
		// signs in the US probe frame in a ascending order with respect to the 
		// absolute LRE root-squared value (length of the LRE vector or the 
		// euclidean distance between the true positions and the project positions).
		// FORMAT: matrix 4xN (with N being the total number of validation positions)
		// [ Row-0: LREs in x-axis from projected to true positions ]
		// [ Row-1: LREs in y-axis from projected to true positions ]
		// [ Row-2: should be all zeros ]
		// [ Row-3: Euclidean LRE distance sorted in ascending order]
		// NOTE: this matrix can be obtained for statistical analysis if desired.
		vnl_matrix <double> getLRESortedAscendingMatrix4xNForNWire1 () const;
		vnl_matrix <double> getLRESortedAscendingMatrix4xNForNWire3 () const;
		vnl_matrix <double> getLRESortedAscendingMatrix4xNForNWire4 () const;
		vnl_matrix <double> getLRESortedAscendingMatrix4xNForNWire6 () const;

		//! Operation: Obtain Line Reconstruction Error Analysis for the validation positions in the US probe frame
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
		std::vector<double> getLREAbsAnalysisForNWire1 () const;
		std::vector<double> getLREAbsAnalysisForNWire3 () const;
		std::vector<double> getLREAbsAnalysisForNWire4 () const;
		std::vector<double> getLREAbsAnalysisForNWire6 () const;

		//! Operation: Obtain the validation data set in the US image frame
		// This will return the original validation data positions in the US image frame 
		// that is used from the independent validation task.
		// FORMAT: for each of the N vectors (with N being the total number of validation positions)
		// [ Element-0: x-coordinates in the US image frame of the validation data set ]
		// [ Element-1: y-coordinates in the US image frame of the validation data set ]
		// [ Element-2: should be all zeros since there is no z-elements for US image  ]
		// [ Element-3: should be all ones ]
		// NOTE: the operation is useful to observe the PRE3D distribution on US image if desired.
		std::vector<vnl_vector_double> getValidationDataSetInUSImageFrame () const;

		//! Operation: get the final calibration transform in 4x4 homogeneous matrix format
		vnl_matrix<double> getCalibrationResultsInMatrix () const;

		//! Operation: Get the calibration transform in parameters
		// The parameters are stored in the following format:
		// - EulerZYX Angles (in radians):		0-Alpha,  1-Beta,  2-Gamma 
		// - Scaling Factors (in meters/pixel):	3-Sx,	  4-Sy
		// - Translation of Origin (in meters):	5-Tx,	  6-Ty,	   7-Tz
		std::vector<double> getCalibrationResultsInParameters ();

		//! Operation: Get the system timestamp for the particular calibration process
		// Calibration system timestamp provides an unique identification label to the 
		// system-logging and exporting of the calibration results.
		// DATE/TIME IN STRING: [MMDDYY_HHMMSS]
		std::string getCalibrationTimeStampInString();
		// DATE/TIME WITH FORMAT: [MM/DD/YY HH:MM:SS]
		std::string getCalibrationTimeStampWithFormat();

	protected:
	 	
		//! Virtual Operation
		// This calculates the phantom geomtry on the fly. It must be overridden
		// in the child class that extends this class.
		virtual void loadGeometry () = 0;

		//! Operation: Convert the calibration transform to parameters
		// The parameters are stored in the following format:
		// - EulerZYX Angles (in radians):			0-Alpha,  1-Beta,  2-Gamma 
		// - Scaling Factors (in milimeters/pixel):	3-Sx,	  4-Sy
		// - Translation of Origin (in milimeters):	5-Tx,	  6-Ty,	   7-Tz
		void convertCalibrationResultsToParameters ();

		//! Operation: Convert a homogeneous 4x4 matrix to parameters
		// IMPORTANT: Please follow the interface closely.
		// 1. The input is a homogeneous 4x4 matrix;
		// 2. The parameters are returned in the following format:
		// - EulerZYX Angles (in radians):			0-Alpha,  1-Beta,  2-Gamma 
		// - Scaling Factors (in milimeters/pixel):	3-Sx,	  4-Sy
		// - Translation of Origin (in milimeters):	5-Tx,	  6-Ty,	   7-Tz
		std::vector<double> convertHomogeneousMatrixToParameters(
			const vnl_matrix<double> HomogeneousTransformMatrix4x4 );

		//! Operation
		// This will construct the validation data matrices
		// NOTE: since the validation data set is separated from the calibration 
		//       data and acquired before the calibration, the construction of
		//       the validation data matrices needs to be done once and only 
		//       once (e.g., before the 1st validation task is performed) with
		//       the flag set to true.  This would save the system runtime
		//       during the iterative calibration/validation process.
		void constructValidationDataMatrices ();

		//! Operation
		// This operation will convert the transform data to 4x4 homogeneous matrix.
		// IMPORTANT: Please follow the interface closely.
		// 1. The input transform is in a vector array format:
		//		- [0]     = Q0			(in degrees - the rotation angle)
		//		- [1 - 3] = Qx, Qy, Qz	(the unit rotation axis)
		//		- [4 - 6] = Tx, Ty, Tz	(translation); 
		// 2. The output is a 4x4 homogeneous transform matrix.
		vnl_matrix<double> convertTransform2HomoMatrix(
			const std::vector<double> &TransformInVectorArray);

		//! Operation
		// Frames where the LRE is larger than LRE stdev * OutlierDetectionThreshold 
		// will be considered as outliers (calibration DataPositions)
		virtual void setOutlierFlags(); 

		//! Operation
		// Clear outlier flags 
		virtual void resetOutlierFlags(); 

		//! Attributes: The US image frame origin in pixels
		// These are the US image frame origin in pixels W.R.T. the left-upper 
		// corner of the original image, with X pointing to the right (column)
		// and Y pointing down to the bottom (row).
		vnl_vector<double> mUSImageFrameOriginInPixels;
		// ! Attributes: the 4x4 homogeneous transform matrix from the original image frame to the TRUS image frame.
		// 1. Note the original image frame conforms to the standard way to define an image
		//    with the image origin set at the top-left corner, positive X axis to the 
		//    right and positive Y axis to the bottom of the image.  This is different than 
		//    VTK Image Reader that uses left-lower corner (a way to comform to Computer 
		//    Graphics as VTK is designed for visualization in the first place).
		// 2. The TRUS image frame is on the other hand completely defined by the user or
		//    the application.  The default is: origin at the middle-bottom of the imaeg,
		//    with the positive X to the right and positive Y to the top of the image. This
		//    conforms to the standard way when TRUS is used during imaging (e.g., in
		//    brachytherapy prostate cancer treatment).
		vnl_matrix<double> mTransformOrigImageFrame2TRUSImageFrameMatrix4x4;
		// Flag to be set when the US image frame origin is set
		bool mHasUSImageFrameOriginBeenSet;

		//! Attribute: The name list of the phantom-specfic reference points
		std::vector<std::string> mNamesOfPhantomSpecificReferencePoints;

		//! Attribute: The reference points on the phantom
		// 1. These are fixed physical positions measurable using a Stylus probe.
		// 2. All positions are all kept w.r.t the phantom frame as defined above.
		// 3. They are used to register the phantom geomtry from the phantom 
		//	  frame to the DRB reference frame to be mounted on the calibrator.
		// 4. All units are in meters.
		std::vector<vnl_vector_double> mPhantomSpecificReferencePoints;

		//! Attributes: flags that control the registration between phantom and DRB
		// The flag to be set when the phantom is registered to the DRB frame
		// on-the-fly (before the calibration procedure starts)
		bool mHasPhantomBeenRegistered;

		//! Attribute: the flag to be set when the data positions in US probe frame is ready
		bool mAreDataPositionsReady;

		//! Attribute: the flag to be set when the validation positions are ready
		bool mAreValidationPositionsReady;

		//! Attribute: the flag to be set when the data positions in US probe frame is ready
		bool mAreOutliersRemoved;

		//! Attribute: the flag to be set when the PRE3Ds for validation positions are ready
		bool mArePRE3DsForValidationPositionsReady;
		//! Attribute: the flag to be set when the independent point/line reconstruction errors are ready
		bool mAreIndependentPointLineReconErrorsReady;

		//! Attribute: the flag to be set after the phantom geometry is loaded sucessfully.
		bool mIsPhantomGeometryLoaded;

		//! Attribute: The US 3D beamwidth profile data and weight factors based on it
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
		// This keeps a copy of the original non-interpolated US beamwidth profile
		// mainly for the purposes of logging and future reference 
		vnl_matrix<double> mOrigUS3DBeamwidthAndWeightFactorsInUSImageFrameTable5xN;
		// The Minimum US elevation beamwidth and the focal zone in US Image Frame
		// 1. For a typical 1-D linear-array transducer, the ultrasound beam can only be 
		//    focused mechanically in the elevation (out-of-plane) axis by placing an 
		//    ascoustic lens in front or curving the crystal surface.  
		// 2. The elevation beam pattern are therefore solely determined by the size and 
		//    curvature of the crystal or the acoustic lens, with sharp focus only 
		//    possible at a narrow axial distance (the focal zone) to the transducer.
		// FORMAT: [FOCAL-ZONE (in US Image Frame), MINI-ELEVATION-WIDTH (in millimeters)]
		vnl_vector<double> mMinimumUSElevationBeamwidthAndFocalZoneInUSImageFrame;
		// Choice of method to incorporate the ultrasund beamwidth
		// Options: 1 - BeamwidthVariance | 2 - BeamwidthRatio | 3 - BeamwidthVarianceAndThresholding
		int mMethodToIncorporateBeamWidth;
		// Keep a track of the total number of the complete beam-width data 
		int mNumOfTotalBeamWidthData;
		// The nearest and farest axial depth in the table (for fast access) 
		int mTheNearestAxialDepthInUSBeamwidthAndWeightTable;
		int mTheFarestAxialDepthInUSBeamwidthAndWeightTable;
		// The 3D beamwidth elements at the nearest/farest axial depth
		vnl_vector<double> mUS3DBeamwidthAtNearestAxialDepth;
		vnl_vector<double> mUS3DBeamwidthAtFarestAxialDepth;
		// The flag to be set if beamwidth and weight factors are set.
		bool mIsUSBeamwidthAndWeightFactorsTableReady;
		// Weights for the data positions defined by prior knowledge of the imaging condition
		// E.g.: the ultrasound 3D beamwidth in axial, lateral and elevational axes
		std::vector<double> mWeightsForDataPositions;
		// This will keep a track of the US 3D beamwidth (euclidean magnitude) at each data position
		std::vector<double> mUSBeamWidthEuclideanMagAtDataPositions;
		// This will keep a track of the US 3D beamwidth (euclidean magnitude) at each validation position
		std::vector<double> mUSBeamWidthEuclideanMagAtValidationPositions;

		//! Attribute: The flag to set when the ultrasound probe has been calibrated
		bool mHasBeenCalibrated;

		//! Attributes: The system logging service 
		// 1. The system log file with its name associated with a system 
		//    timestamp, when turned on, is used to record all the data pipeline 
		//    processing during the calibration.
		// 2. The system log should be turned on before any calibration related 
		//    activities (e.g., add data positions, validating, calibrating, etc.).
		// Flag to set when the system log is turned on.
		bool mIsSystemLogOn;
		// The record of the system timestamp in string.
		// This would be helpful to logging service and generating results with a unique
		// system timestamp.
		// DATE/TIME IN STRING: [MMDDYY_HHMMSS]
		std::string mSystemTimestampInString;
		// DATE/TIME WITH FORMAT: [MM/DD/YY HH:MM:SS]
		std::string mSystemTimestampWithFormat;
		// The calibration system log file
		std::string mSystemLogFileNameWithTimeStamp;

		//! Attributes: the 4x4 homogeneous transform matrices after registration of phantom geometry
		vnl_matrix<double> mTransformMatrixPhantom2DRB4x4;
		vnl_matrix<double> mTransformMatrixDRB2Phantom4x4;

		//! Attributes: data positions collected as inputs for the US calibration
		std::vector<vnl_vector_double> mDataPositionsInPhantomFrame;
		std::vector<vnl_vector_double> mDataPositionsInUSProbeFrame;
		std::vector<vnl_vector_double> mDataPositionsInUSImageFrame;
		std::vector<int> mOutlierDataPositions; 

		//! Attributes: validation positions collected to validate the calibration accuracy
		std::vector<vnl_vector_double> mValidationPositionsInPhantomFrame;
		std::vector<vnl_vector_double> mValidationPositionsInUSProbeFrame;
		std::vector<vnl_vector_double> mValidationPositionsInUSImageFrame;
		//! Attributes: validation positions for point-line distance
		std::vector<vnl_vector_double> mValidationPositionsNWireStartInUSProbeFrame;
		std::vector<vnl_vector_double> mValidationPositionsNWireEndInUSProbeFrame;
		//! Attributes: validation positions for parallel wires
		// In US image frame
		std::vector<vnl_vector_double> mValidationPositionsNWire1InUSImageFrame;
		std::vector<vnl_vector_double> mValidationPositionsNWire3InUSImageFrame;
		std::vector<vnl_vector_double> mValidationPositionsNWire4InUSImageFrame;
		std::vector<vnl_vector_double> mValidationPositionsNWire6InUSImageFrame;
		// In US probe frame
		std::vector<vnl_vector_double> mValidationPositionsNWire1InUSProbeFrame;
		std::vector<vnl_vector_double> mValidationPositionsNWire3InUSProbeFrame;
		std::vector<vnl_vector_double> mValidationPositionsNWire4InUSProbeFrame;
		std::vector<vnl_vector_double> mValidationPositionsNWire6InUSProbeFrame;
		// NOTE: since the validation data set is separated from the calibration 
		//       data and acquired before the calibration, the construction of
		//       the validation data matrices needs to be done once and only 
		//       once (e.g., before the 1st validation task is performed) with
		//       the flag set to true.  This would save the system runtime
		//       during the iterative calibration/validation process.
		// Validation Positions to compute point-reconstruction errors (PREs)
		vnl_matrix<double> mValidationPositionsInUSImageFrameMatrix4xN;
		vnl_matrix<double> mValidationPositionsInUSProbeFrameMatrix4xN;
		// Validation Positions to compute independent point-line distance errors (PLDEs)
		vnl_matrix<double> mValidationPositionsNWireStartInUSProbeFrame3xN;
		vnl_matrix<double> mValidationPositionsNWireEndInUSProbeFrame3xN;
		// Validation Positions to compute independent parallel line reconstruction errors (LREs)
		vnl_matrix<double> mValidationPositionsNWire1InUSImageFrame4xN;
		vnl_matrix<double> mValidationPositionsNWire3InUSImageFrame4xN;
		vnl_matrix<double> mValidationPositionsNWire4InUSImageFrame4xN;
		vnl_matrix<double> mValidationPositionsNWire6InUSImageFrame4xN;
		vnl_matrix<double> mValidationPositionsNWire1InUSProbeFrame4xN;
		vnl_matrix<double> mValidationPositionsNWire3InUSProbeFrame4xN;
		vnl_matrix<double> mValidationPositionsNWire4InUSProbeFrame4xN;
		vnl_matrix<double> mValidationPositionsNWire6InUSProbeFrame4xN;

		// Flag to set when the validation data matrices are populated 
		bool mAreValidationDataMatricesConstructed;
		// weights for the validation positions defined by prior knowledge of the imaging condition
		// E.g.: the ultrasound 3D beamwidth in axial, lateral and elevational axes
		std::vector<double> mWeightsForValidationPositions; //Not in use yet!

		//! Attribute: Final calibration transform in vnl_matrix format
		// The homogeneous transform matrix from the US image frame to the US probe frame
		vnl_matrix<double> mTransformUSImageFrame2USProbeFrameMatrix4x4;

		//! Attribute: Final calibration transform in 8 parameters
		// The parameters are in the following format
		// - EulerZYX Angles (in radians):		0-Alpha,  1-Beta,  2-Gamma 
		// - Scaling Factors (in meters/pixel):	3-Sx,	  4-Sy
		// - Translation of Origin (in meters):	5-Tx,	  6-Ty,	   7-Tz
		std::vector<double> mTransformUSImageFrame2USProbeFrameParameters;

		//! Attribute: Validation data confidence level
		// This sets the confidence level (trusted zone) as a percentage
		// of the independent validation data used to produce the final
		// validation results.  It serves as an effective way to get rid
		// of corrupted data (or outliers) in the validation dataset.
		// Default value: 0.95 (or 95%), meaning the top ranked 95% of 
		// the ascendingly-ordered PRE3D values from the validation data 
		// would be accepted as the valid PRE3D values.
		double mValidationDataConfidenceLevel;

		//! Attribute: 3D point reconstruction error (PRE3D) Analysis for the validation positions in the US probe frame
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

		//! Attribute: 3D point reconstruction error (PRE3D) matrix for validation positions in US probe frame
		// This keeps all the original PRE3Ds for the validation dataset with signs in
		// the US probe frame (from the Projected positions to the true positions).
		// FORMAT: matrix 4xN (with N being the total number of validation positions)
		// [ Row-0: PRE3Ds in x-axis from projected to true positions ]
		// [ Row-1: PRE3Ds in y-axis from projected to true positions ]
		// [ Row-2: PRE3Ds in z-axis from projected to true positions ]
		// [ Row-3: should be all zeros ]
		// NOTE: this matrix can be obtained for statistical analysis if desired.
		vnl_matrix<double> mRawPRE3DsforValidationPositionsInUSProbeFrameMatrix4xN;

		//! Attribute: Sorted 3D point reconstruction error (PRE3D) matrix for validation positions in US probe frame
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
		vnl_vector<double> mPLDEsforValidationPositionsInUSProbeFrame;
		vnl_vector<double> mSortedPLDEsAscendingforValidationInUSProbeFrame;

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
		std::vector<double> mPLDEAnalysis4ValidationPositionsInUSProbeFrame;

		//! Attribute: Line reconstruction error (LRE) matrix for validation positions in US probe frame
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

		//! Attribute: Sorted Line reconstruction error (LRE) matrix for validation positions in US probe frame
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
		std::vector<double> mNWire1AbsLREAnalysisInUSProbeFrame;
		std::vector<double> mNWire3AbsLREAnalysisInUSProbeFrame;
		std::vector<double> mNWire4AbsLREAnalysisInUSProbeFrame;
		std::vector<double> mNWire6AbsLREAnalysisInUSProbeFrame;

		// frames where the LRE is larger than LRE stdev * OutlierDetectionThreshold 
		// will be considered as outliers
		static double mOutlierDetectionThreshold; 
};


#endif // PHANTOM_H
