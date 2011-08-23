#ifndef BRACHYTRUSCALIBRATOR_H
#define BRACHYTRUSCALIBRATOR_H
// ===========================================================================
// Program Modification Record
// ===========================================================================
// Component:	Component_AutoTRUSCalibration
// ===========================================================================
// Class:		BrachyTRUSCalibrator
// ===========================================================================
// File Name:	BrachyTRUSCalibrator.h
// ===========================================================================
// Author:		Thomas Kuiran Chen <chent@cs.queensu.ca>
//				School of Computing, Queen's University at Kingston, Canada
// ===========================================================================
// Purposes: 
// 1. This is the class that defines our Brachy-TRUS Calibrator [1,2] for 
//    transrectal ultrasound (TRUS) probe calibration in brachytherapy.
// 2. The class is a child class of the parent Phantom class.
// 3. Uses the open source VXL-VNL (Numerical and Algebra C++ Library)
// 4. Uses C++ Standard Library and Standard Template Library
// ===========================================================================
// References:
// [1] Chen, T. K., Thurston, A. D., Ellis, R. E. & Abolmaesumi, P. A real-
//     time freehand ultrasound calibration system with automatic accuracy 
//     feedback and control. Ultrasound Med Biol, 2009, vol. 35, page 79-93.
// [2] Chen, T. K., Thurston, A. D., Moghari, M. H., Ellis, R. E. & 
//     Abolmaesumi, P. Miga, M. I. & Cleary, K. R. (ed.) A real-time 
//     ultrasound calibration system with automatic accuracy control and 
//     incorporation of ultrasound section thickness SPIE Medical Imaging 2008: 
//     Visualization, Image-guided Procedures, and Modeling, 2008,6918,69182A.
//     Best Student Paper Award - Second Place and Cum Laude Poster Award.
// ===========================================================================
// Change History:
//
// Author				Time						Release	Changes
// Thomas Kuiran Chen	Mon Mar 1 08:19 EST 2010	1.0		Creation
//
// ===========================================================================
//					  Copyright @ Thomas Kuiran Chen, 2010
// ===========================================================================


#ifndef PHANTOM_H
#include "Phantom.h"  // The parent class
#endif	

#include "FidPatternRecognition.h"

#include <vector>

// Phantom known points in template coordinate system
/*
class PhantomPoints
{
public: 
	struct PhantomPointPosition
	{
		double x; 
		double y; 
		double z; 
	}; 

	PhantomPoints::PhantomPoints() 
	{
		// Template holder position 
		TemplateHolderPosition.x = 5.0;   TemplateHolderFrameOriginToTemplateFrameOriginInTemplateFrame
		TemplateHolderPosition.y = 63.5; 
		TemplateHolderPosition.z = 0; 

		//********** Wire #1 ************
		PhantomPointPosition wire1front; 
		wire1front.x = 49.86; 
		wire1front.y = 34.385; 
		wire1front.z = 62.05; 
		WirePositionFrontWall.push_back(wire1front);  

		PhantomPointPosition wire1back; 
		wire1back.x = 49.86; 
		wire1back.y = 34.385; 
		wire1back.z = 142.05; 
		WirePositionBackWall.push_back(wire1back);  

		//********** Wire #2 ************
		PhantomPointPosition wire2front; 
		wire2front.x = 19.86; 
		wire2front.y = 34.385; 
		wire2front.z = 62.05; 
		WirePositionFrontWall.push_back(wire2front);  

		PhantomPointPosition wire2back; 
		wire2back.x = 39.86; 
		wire2back.y = 34.385; 
		wire2back.z = 142.05; 
		WirePositionBackWall.push_back(wire2back);  

		//********** Wire #3 ************
		PhantomPointPosition wire3front; 
		wire3front.x = 9.86; 
		wire3front.y = 34.385; 
		wire3front.z = 62.05; 
		WirePositionFrontWall.push_back(wire3front);  

		PhantomPointPosition wire3back; 
		wire3back.x = 9.86; 
		wire3back.y = 34.385; 
		wire3back.z = 142.05; 
		WirePositionBackWall.push_back(wire3back);  

		//********** Wire #4 ************
		PhantomPointPosition wire4front; 
		wire4front.x = 49.86; 
		wire4front.y = 54.385; 
		wire4front.z = 62.05; 
		WirePositionFrontWall.push_back(wire4front);  

		PhantomPointPosition wire4back; 
		wire4back.x = 49.86; 
		wire4back.y = 54.385; 
		wire4back.z = 142.05; 
		WirePositionBackWall.push_back(wire4back);  

		//********** Wire #5 ************
		PhantomPointPosition wire5front; 
		wire5front.x = 39.86; 
		wire5front.y = 54.385; 
		wire5front.z = 62.05; 
		WirePositionFrontWall.push_back(wire5front);  

		PhantomPointPosition wire5back; 
		wire5back.x = 19.86; 
		wire5back.y = 54.385; 
		wire5back.z = 142.05; 
		WirePositionBackWall.push_back(wire5back);  

		//********** Wire #6 ************
		PhantomPointPosition wire6front; 
		wire6front.x = 9.86; 
		wire6front.y = 54.385; 
		wire6front.z = 62.05; 
		WirePositionFrontWall.push_back(wire6front);  

		PhantomPointPosition wire6back; 
		wire6back.x = 9.86; 
		wire6back.y = 54.385; 
		wire6back.z = 142.05; 
		WirePositionBackWall.push_back(wire6back);  

	}

	// Wire positions from the phantom origin (A7 point)
	std::vector<PhantomPointPosition> WirePositionFrontWall; 
	std::vector<PhantomPointPosition> WirePositionBackWall; 
	
	// Center position of the left template holder rod from the phantom origin
	PhantomPointPosition TemplateHolderPosition; 
	
};
*/

//================================================ 
// class BrachyTRUSCalibrator
//================================================ 
class BrachyTRUSCalibrator : public Phantom
{
	public:
		
		//! Strings
		static const std::string mstrScope;

		//! Contants
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
		BrachyTRUSCalibrator ( FidPatternRecognition * patternRecognitionObject, const bool IsSystemLogOn = false );

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
		virtual void addDataPositionsPerImage( 
			std::vector<vnl_vector_double> SegmentedDataPositionListPerImage, 
			const vnl_matrix<double> TransformMatrixUSProbe2Stepper4x4 );
		// Interface-2: transformation of the image is given from
		//              - the US Probe frame to the Tracker frame, and
		//				- the DRB frame to the Tracker frame.
		virtual void addDataPositionsPerImage( 
			std::vector<vnl_vector_double> SegmentedDataPositionListPerImage, 
			const std::vector<double> TransformUSProbe2Tracker,
			const std::vector<double> TransformDRB2Tracker );

		//! Overridden Operation: Delete the data positions that were added in the last image.
		// This operation is helpful when the latest added input data does not actually
		// improve the calibration accuracy (i.e., judged by PRE3D).  For example, this 
		// could happen when the image quality is poor and typically results in an 
		// inaccurate segmentation, which in turn negatively influnce the final 
		// calibration outcome.  Using this method, the application can choose to 
		// eliminate a bad input data (e.g., in an iterative calibration process where
		// the calibration accuracy is evaluated in real time for each iteration).
		virtual void deleteLatestAddedDataPositionsPerImage ();

		virtual void deleteDataPositionsPerImage(int position); 

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
		virtual void addValidationPositionsPerImage( 
			std::vector<vnl_vector_double> SegmentedDataPositionListPerImage, 
			const vnl_matrix<double> TransformMatrixUSProbe2Stepper4x4 );
		// Interface-2: transformation of the image is given from
		//              - the US Probe frame to the Tracker frame, and
		//				- the DRB frame to the Tracker frame.
		virtual	void addValidationPositionsPerImage( 
			std::vector<vnl_vector_double> SegmentedDataPositionListPerImage, 
			const std::vector<double> TransformUSProbe2Tracker,
			const std::vector<double> TransformDRB2Tracker );

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


//		PhantomPoints GetPhantomPoints() { return this->mPhantomPoints; }; 
    const NWire& GetNWire(int i) { return this->mNWires[i]; }; 

		//! Compute wire intersections from phantom geometry
		virtual PlusStatus loadGeometry();

	private:

		// N-wire's Start and End Positions in the Front and Back Inner Walls
		// There are 5x5 matrix of holes on both front and back inner walls
		// [FORMAT: Front/Back [i][j], i & j are rows/columns of the hole matrix]
		//vnl_vector<double> mPhantomGeometryOnFrontInnerWall[5][5];
		//vnl_vector<double> mPhantomGeometryOnBackInnerWall[5][5];

		//PhantomPoints mPhantomPoints;

		std::vector<NWire> mNWires;

};


#endif // BRACHYTRUSCALIBRATOR_H
