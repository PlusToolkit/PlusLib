#ifndef BRACHYTRUSCALIBRATOR_H
#define BRACHYTRUSCALIBRATOR_H

// Purposes: 
// 1. This is the class that defines our Brachy-TRUS Calibrator [1,2] for 
//    transrectal ultrasound (TRUS) probe calibration in brachytherapy.
// 2. The class is a child class of the parent Phantom class.
// 3. Uses the open source VXL-VNL (Numerical and Algebra C++ Library)
// 4. Uses C++ Standard Library and Standard Template Library

#ifndef PHANTOM_H
#include "Phantom.h"  // The parent class
#endif	

#include "FidPatternRecognition.h"

#include <vector>

// class BrachyTRUSCalibrator
class BrachyTRUSCalibrator : public Phantom
{
	public:
		
		//! Constants
		// For BrachyTRUSCalibrator, row/col indices are of no interest
		// to us any more, because we only see two layers of N-wires with one
		// N-shape on each layer.  Therefore in each image only two phantom 
		// geometry indices are collected and the program knows their indices
		// in the pre-generated geometry.  This is a good start for automating 
		// the process since we are able to calculate alpha on-the-fly now
		// without requiring to specify the phantom indices one by one.
		static const int mNUMREFPOINTSPERIMAGE;  
		// This is the threshold to filter out input data acquired at large beamwidth
		// 1. We set the maximum tolerance to be the number of times of the current 
		//    minimum magnitude of the US beamwidth typically measured at the 
		//    elevation plane's focal zone. 
		// 2. According to ultrasound physics, the beamwidth loses most of its
		//    resolution after grows to two times that of at focal zone. So 10% 
		//    above the twice of the minimum beamwidth may serve a good cutoff
		//    point to quality control the imaging data for a reliable calibration.
		static const double mNUMOFTIMESOFMINBEAMWIDTH;

		//! Constructor with flag to turn on the system logging service
		// 1. A system log file with its name associated with a system 
		//    timestamp, when turned on, is used to record all the data pipeline 
		//    processing during the calibration.
		// 2. The system log should be turned on before any calibration related 
		//    activities (e.g., add data positions, validating, calibrating, etc.).
		// [DEFAULT: OFF (FALSE)]
		BrachyTRUSCalibrator ( FidPatternRecognition * patternRecognitionObject);

		//! Destructor
		virtual ~BrachyTRUSCalibrator ();

		//! Overridden Operation: Calculate and add positions in Phantom frame
		// This operation will use the segmented N-fiducials in each US image to 
		// calculate the position in Phantom frame base on the existing phantom 
		// geometry data.  The result would be added contineously into a private 
		// class data storage container.  Note: this operation is performed on 
		// one individual US image captured on Phantom, so natually, it can be 
		// called repeatedly to add in reference positions in multiple images 
		// in order to enhance the calibration accuracy.
		//
		// IMPORTANT:
		// - This is a very crucial point that would easily cause confusions
		//   or mistakes.  There is one and only one fixed correspondence between 
		//   the 6 segmented image positions (N-fiducials) and the wires.
		// - Closely examine the wiring design and set Ai and Bi accordingly.
		// 
		// Interface-1: transformation of the image is given from
		//              the US Probe frame to the DRB frame.
		virtual PlusStatus addDataPositionsPerImage( 
			std::vector<vnl_vector_double> SegmentedDataPositionListPerImage, 
			const vnl_matrix<double> TransformMatrixUSProbe2Stepper4x4 );

		//! Overridden Operation: Add positions per image to validate calibration accuracy
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
		//              the US Probe frame to the DRB frame.
		virtual PlusStatus addValidationPositionsPerImage( 
			std::vector<vnl_vector_double> SegmentedDataPositionListPerImage, 
			const vnl_matrix<double> TransformMatrixUSProbe2Stepper4x4 );

		//! Overridden Operation: calculate the 3D Point Reconstruction Error (PRE3D) for a given image.
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
			std::vector<vnl_vector_double> SegmentedDataPositionListPerImage, 
			const vnl_matrix<double> TransformMatrixUSProbe2DRB4x4 ) const;

    //! Get ith NWire
    const NWire& GetNWire(int i) { return this->mNWires[i]; }; 

		//! Compute wire intersections from phantom geometry
		virtual PlusStatus loadGeometry();

	private:
    //! NWires structure
		std::vector<NWire> mNWires;
};


#endif // BRACHYTRUSCALIBRATOR_H
