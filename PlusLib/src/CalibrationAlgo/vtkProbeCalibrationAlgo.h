/*!=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __vtkProbeCalibrationAlgo_h
#define __vtkProbeCalibrationAlgo_h

#include "vtkObject.h"

#include "vtkTransform.h"

#include "vnl/vnl_matrix.h"

#include <string>
#include <vector>

class TrackedFrame; 
class vtkTrackedFrameList; 
class vtkTransformRepository;
class vtkXMLDataElement; 
class NWire;

/*!
  \class vtkProbeCalibrationAlgo 
  \brief Probe calibration algorithm class
  \ingroup PlusLibCalibrationAlgorithm
*/
class vtkProbeCalibrationAlgo : public vtkObject
{
public:
	static vtkProbeCalibrationAlgo *New();
	vtkTypeRevisionMacro(vtkProbeCalibrationAlgo, vtkObject);
	virtual void PrintSelf(ostream& os, vtkIndent indent); 

	/*!
    Read XML based configuration of the calibration controller
    \param aConfig Root element of device set configuration data
  */
	PlusStatus ReadConfiguration( vtkXMLDataElement* aConfig ); 

  /*!
	  Run calibration algorithm on the two input frame lists. It uses only a certain range of the input sequences (so it is possible to use the same sequence but different sections of it).
	  \param validationTrackedFrameList TrackedFrameList with segmentation results for the validation
    \param validationStartFrame First frame that is used from the validation tracked frame list for the validation (in case of -1 it starts with the first)
    \param validationEndFrame Last frame that is used from the validation tracked frame list for the validation (in case of -1 it starts with the last)
	  \param calibrationTrackedFrameList TrackedFrameList with segmentation results for the calibration
    \param calibrationStartFrame First frame that is used from the calibration tracked frame list for the calibration (in case of -1 it starts with the first)
    \param calibrationEndFrame Last frame that is used from the calibration tracked frame list for the calibration (in case of -1 it starts with the last)
    \param transformRepository Transform repository object to be able to get the default transform
    \param nWires NWire structure that contains the computed imaginary intersections. It used to determine the computed position
	*/
  PlusStatus Calibrate( vtkTrackedFrameList* validationTrackedFrameList, int validationStartFrame, int validationEndFrame, vtkTrackedFrameList* calibrationTrackedFrameList, int calibrationStartFrame, int calibrationEndFrame, vtkTransformRepository* transformRepository, std::vector<NWire> &nWires ); 

  /*!
    Run calibration algorithm on the two input frame lists (uses every frame in the two sequences)
	  \param validationTrackedFrameList TrackedFrameList with segmentation results for the validation
	  \param calibrationTrackedFrameList TrackedFrameList with segmentation results for the calibration
    \param transformRepository Transform repository object to be able to get the default transform
    \param nWires NWire structure that contains the computed imaginary intersections. It used to determine the computed position
	*/
  PlusStatus Calibrate( vtkTrackedFrameList* validationTrackedFrameList, vtkTrackedFrameList* calibrationTrackedFrameList, vtkTransformRepository* transformRepository, std::vector<NWire> &nWires ); 

  /*! Check user image home to probe home transform orthogonality */
  bool IsImageToProbeTransformOrthogonal(); 

  /*!
	  Assembles the result string to display
    \param precision Number of decimals printed in the string
	  \return String containing results
	*/
	std::string GetResultString(int precision = 3);

public:
  /*! Set/get the calibration date and time in string format */
	vtkSetStringMacro(CalibrationDate); 
  /*! Set/get the calibration date and time in string format */
	vtkGetStringMacro(CalibrationDate);

	/*! Get mean validation 3D reprojection error */
	vtkGetMacro(ValidationReprojectionError3DMean, double);
	/*! Get standard deviation of validation 3D reprojection errors */
	vtkGetMacro(ValidationReprojectionError3DStdDev, double);
	/*! Get mean calibration 3D reprojection error */
	vtkGetMacro(CalibrationReprojectionError3DMean, double);
	/*! Get standard deviation of calibration 3D reprojection errors */
	vtkGetMacro(CalibrationReprojectionError3DStdDev, double);

  /*! Get the image coordinate frame name */
  vtkGetStringMacro(ImageCoordinateFrame);
  /*! Get the probe coordinate frame name */
  vtkGetStringMacro(ProbeCoordinateFrame);
  /*! Get the phantom coordinate frame name */
  vtkGetStringMacro(PhantomCoordinateFrame);
  /*! Get the reference coordinate frame name */
  vtkGetStringMacro(ReferenceCoordinateFrame);
  /*! Get the transducer origin coordinate frame name */
  vtkGetStringMacro(TransducerOriginCoordinateFrame);
  /*! Get the transducer origin pixel coordinate frame name */
  vtkGetStringMacro(TransducerOriginPixelCoordinateFrame);

	/*! Get/set image home to transducer origin (pixel) constant transform. Should be defined in config file */
	vtkGetObjectMacro(TransformImageToTransducerOriginPixel, vtkTransform);
	/*! Get/set image home to transducer origin (pixel) constant transform. Should be defined in config file */
	vtkSetObjectMacro(TransformImageToTransducerOriginPixel, vtkTransform);
	
	/*! Get/set the iCAL calibration result transformation between the image and probe home position */
	vtkGetObjectMacro(TransformImageToProbe, vtkTransform);
  /*! Get/set the iCAL calibration result transformation between the image and probe home position */
	vtkSetObjectMacro(TransformImageToProbe, vtkTransform);

public:
  /*! Get 2D reprojection statistics for a specified wire made from the validation or the calibration data */
  PlusStatus GetReprojectionError2DStatistics(double &xMean, double &yMean, double &xStdDev, double &yStdDev, int wireNumber, bool isValidation);

protected:
  /*!
    Calculate and add positions of an individual image for calibration or validation
    \param trackedFrame The actual tracked frame (already segmented) to add for calibration or validation
    \param transformRepository Transform repository object to be able to get the default transform
    \param isValidation Flag whether the added data is for calibration or validation
  */
	PlusStatus AddPositionsPerImage( TrackedFrame* trackedFrame, vtkTransformRepository* transformRepository, bool isValidation );

  /*!
    Calculate 3D reprojection errors
    \param trackedFrameList Tracked frame list for error computation
    \param startFrame First frame that is used from the tracked frame list for the error computation (in case of -1 it starts with the first)
    \param endFrame Last frame that is used from the tracked frame list for the error computation (in case of -1 it starts with the last)
    \param transformRepository Transform repository object to be able to get the default transform
    \param isValidation Flag whether the input tracked frame list is of calibration or validation
  */
  PlusStatus ComputeReprojectionErrors3D( vtkTrackedFrameList* trackedFrameList, int startFrame, int endFrame, vtkTransformRepository* transformRepository, bool isValidation );

  /*!
    Calculate 2D reprojection errors
    \param trackedFrameList Tracked frame list for validation
    \param startFrame First frame that is used from the tracked frame list for the error computation (in case of -1 it starts with the first)
    \param endFrame Last frame that is used from the tracked frame list for the error computation (in case of -1 it starts with the last)
    \param transformRepository Transform repository object to be able to get the default transform
    \param isValidation Flag whether the input tracked frame list is of calibration or validation
  */
  PlusStatus ComputeReprojectionErrors2D( vtkTrackedFrameList* trackedFrameList, int startFrame, int endFrame, vtkTransformRepository* transformRepository, bool isValidation );

  /*! Set ImageToProbe calibration result matrix and validate it */
  void SetAndValidateImageToProbeTransform( vnl_matrix<double> imageToProbeTransformMatrixVnl, vtkTransformRepository* transformRepository );

  /*! Save results and error report to XML */
  PlusStatus SaveCalibrationResultAndErrorReportToXML(vtkTrackedFrameList* validationTrackedFrameList, int validationStartFrame, int validationEndFrame, vtkTrackedFrameList* calibrationTrackedFrameList, int calibrationStartFrame, int calibrationEndFrame);

protected:
  /*! Set the image coordinate frame name */
  vtkSetStringMacro(ImageCoordinateFrame);
  /*! Set the probe coordinate frame name */
  vtkSetStringMacro(ProbeCoordinateFrame);
  /*! Set the phantom coordinate frame name */
  vtkSetStringMacro(PhantomCoordinateFrame);
  /*! Set the reference coordinate frame name */
  vtkSetStringMacro(ReferenceCoordinateFrame);
  /*! Set the transducer origin coordinate frame name */
  vtkSetStringMacro(TransducerOriginCoordinateFrame);
  /*! Set the transducer origin pixel coordinate frame name */
  vtkSetStringMacro(TransducerOriginPixelCoordinateFrame);

protected:
	vtkProbeCalibrationAlgo();
	virtual ~vtkProbeCalibrationAlgo();

protected:
  /*! Calibration date in string format */
  char* CalibrationDate; 

  /*! Name of the image coordinate frame (eg. Image) */
  char* ImageCoordinateFrame;

  /*! Name of the probe coordinate frame (eg. Probe) */
  char* ProbeCoordinateFrame;

  /*! Name of the phantom coordinate frame (eg. Phantom) */
  char* PhantomCoordinateFrame;

  /*! Name of the reference coordinate frame (eg. Reference) */
  char* ReferenceCoordinateFrame;

  /*! Name of the transducer origin (mm) coordinate frame (eg. TransducerOrigin) */
  char* TransducerOriginCoordinateFrame;

  /*! Name of the transducer origin (pixel) coordinate frame (eg. TransducerOriginPixel) */
  char* TransducerOriginPixelCoordinateFrame;

	/*! Transform matrix from the original image frame to the transducer origin (pixel). Constant transform, read from file */
	vtkTransform* TransformImageToTransducerOriginPixel;

  /*! The result of the calibration */
	vtkTransform* TransformImageToProbe;


  /*! List of NWires used for calibration and error computation */
  std::vector<NWire> NWires;

  /*! Positions of segmented points in image frame - input of optimization algorithm */
  std::vector< vnl_vector<double> > DataPositionsInImageFrame;

  /*! Positions of segmented points in probe frame - input of optimization algorithm */
  std::vector< vnl_vector<double> > DataPositionsInProbeFrame;

  /*!
    Confidence level (trusted zone) as a percentage of the independent validation data used to produce the final error computation results.  It serves as an effective way to get rid of corrupted data
    (or outliers) in the validation dataset. Default value: 0.95 (or 95%), meaning the top ranked 95% of the ascendingly-ordered PRE values from the validation data would be accepted as the valid PRE values.
  */
	double ErrorConfidenceLevel;


  /*! Stored positions of the middle wires in the phantom frame for the validation data. These positions come from the segmented positions and the phantom geometry.
    First index is for tracked frame indices, second is for NWires.
  */
  std::vector< std::vector< vnl_vector<double> > > ValidationMiddleWirePositionsInPhantomFrame;

  /*! Vector holding the 3D reprojection errors for each NWire in all validation images (outer vector is for the NWires, inner one is for the images)
    Computed as a distance between the actual segmented position of the middle wire transformed into phantom frame and the computed positions (see MiddleWirePositionsInPhantomFrame)
  */
  std::vector< std::vector<double> > ValidationReprojectionError3Ds;

  /*! Indices of the validation 3D reprojection errors in ascending order in ReprojectionError3Ds */
  std::vector< std::vector<int> > SortedValidationReprojectionError3DIndices;

  /*! Mean validation 3D reprojection error */
  double ValidationReprojectionError3DMean;

  /*! Standard deviation of validation 3D reprojection errors */
  double ValidationReprojectionError3DStdDev;


  /*! Stored positions of the middle wires in the phantom frame for the calibration data. These positions come from the segmented positions and the phantom geometry.
    First index is for tracked frame indices, second is for NWires.
  */
  std::vector< std::vector< vnl_vector<double> > > CalibrationMiddleWirePositionsInPhantomFrame;

  /*! Vector holding the 3D reprojection errors for each NWire in all calibration images (outer vector is for the NWires, inner one is for the images)
    Computed as a distance between the actual segmented position of the middle wire transformed into phantom frame and the computed positions (see MiddleWirePositionsInPhantomFrame)
  */
  std::vector< std::vector<double> > CalibrationReprojectionError3Ds;

  /*! Indices of the calibration 3D reprojection errors in ascending order in ReprojectionError3Ds */
  std::vector< std::vector<int> > SortedCalibrationReprojectionError3DIndices;

  /*! Mean calibration 3D reprojection error */
  double CalibrationReprojectionError3DMean;

  /*! Standard deviation of calibration 3D reprojection errors */
  double CalibrationReprojectionError3DStdDev;


  /*! Vector holding the 2D reprojection errors for each wire in all validation images (outermost vector holds the wires, the one inside it holds the images, and the inner holds the X and Y errors)
    Computed as X and Y distances between the actual segmented position of the wires and the intersections of the wires with the image planes
  */
  std::vector< std::vector< std::vector<double> > > ValidationReprojectionError2Ds;

  /*! Indices of the validation 2D reprojection errors in ascending order for each wire in ReprojectionError2Ds */
  std::vector< std::vector<int> > SortedValidationReprojectionError2DIndices;

  /*! Mean validation 2D reprojection error for each wire (two elements, first for the X axis and the other for Y) */
  std::vector< std::vector<double> > ValidationReprojectionError2DMeans;

  /*! Standard deviation of validation 2D reprojection errors for each wire (two elements, first for the X axis and the other for Y) */
  std::vector< std::vector<double> > ValidationReprojectionError2DStdDevs;


  /*! Vector holding the 2D reprojection errors for each wire in all calibration images (outermost vector holds the wires, the one inside it holds the images, and the inner holds the X and Y errors)
    Computed as X and Y distances between the actual segmented position of the wires and the intersections of the wires with the image planes
  */
  std::vector< std::vector< std::vector<double> > > CalibrationReprojectionError2Ds;

  /*! Indices of the calibration 2D reprojection errors in ascending order for each wire in ReprojectionError2Ds */
  std::vector< std::vector<int> > SortedCalibrationReprojectionError2DIndices;

  /*! Mean calibration 2D reprojection error for each wire (two elements, first for the X axis and the other for Y) */
  std::vector< std::vector<double> > CalibrationReprojectionError2DMeans;

  /*! Standard deviation of calibration 2D reprojection errors for each wire (two elements, first for the X axis and the other for Y) */
  std::vector< std::vector<double> > CalibrationReprojectionError2DStdDevs;

private:
	vtkProbeCalibrationAlgo(const vtkProbeCalibrationAlgo&);
	void operator=(const vtkProbeCalibrationAlgo&);
};

#endif //  __vtkProbeCalibrationAlgo_h
