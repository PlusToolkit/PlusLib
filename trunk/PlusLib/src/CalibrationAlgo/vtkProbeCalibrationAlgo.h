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

	/*! Get mean 3D reprojection error */
	vtkGetMacro(ReprojectionError3DMean, double);
	/*! Get standard deviation of 3D reprojection errors */
	vtkGetMacro(ReprojectionError3DStdDev, double);

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

protected:
  /*!
    Calculate and add positions of an individual image for calibration or validation
    \param trackedFrame The actual tracked frame (already segmented) to add for calibration or validation
    \param transformRepository Transform repository object to be able to get the default transform
    \param nWires NWire structure that contains the computed imaginary intersections. It used to determine the computed position
    \param isValidation Flag whether the added data is for calibration or validation
  */
	PlusStatus AddPositionsPerImage( TrackedFrame* trackedFrame, vtkTransformRepository* transformRepository, std::vector<NWire> &nWires, bool isValidation );

  /*!
    Calculate 3D reprojection errors
    \param validationTrackedFrameList Tracked frame list for validation
    \param validationStartFrame First frame that is used from the validation tracked frame list for the validation (in case of -1 it starts with the first)
    \param validationEndFrame Last frame that is used from the validation tracked frame list for the validation (in case of -1 it starts with the last)
    \param transformRepository Transform repository object to be able to get the default transform
    \param nWires NWire structure that contains the computed imaginary intersections. It used to determine the computed position
  */
  PlusStatus ComputeReprojectionErrors3D( vtkTrackedFrameList* validationTrackedFrameList, int validationStartFrame, int validationEndFrame, vtkTransformRepository* transformRepository, std::vector<NWire> &nWires );

  /*!
    Calculate 2D reprojection errors
    \param validationTrackedFrameList Tracked frame list for validation
    \param validationStartFrame First frame that is used from the validation tracked frame list for the validation (in case of -1 it starts with the first)
    \param validationEndFrame Last frame that is used from the validation tracked frame list for the validation (in case of -1 it starts with the last)
    \param transformRepository Transform repository object to be able to get the default transform
    \param nWires NWire structure that contains the computed imaginary intersections. It used to determine the computed position
  */
  PlusStatus ComputeReprojectionErrors2D( vtkTrackedFrameList* validationTrackedFrameList, int validationStartFrame, int validationEndFrame, vtkTransformRepository* transformRepository, std::vector<NWire> &nWires );

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

  /*! Positions of segmented points in image frame - input of optimization algorithm */
  std::vector< vnl_vector<double> > DataPositionsInImageFrame;

  /*! Positions of segmented points in probe frame - input of optimization algorithm */
  std::vector< vnl_vector<double> > DataPositionsInProbeFrame;

  /*!
    Confidence level (trusted zone) as a percentage of the independent validation data used to produce the final validation results.  It serves as an effective way to get rid of corrupted data
    (or outliers) in the validation dataset. Default value: 0.95 (or 95%), meaning the top ranked 95% of the ascendingly-ordered PRE values from the validation data would be accepted as the valid PRE values.
  */
	double ValidationDataConfidenceLevel;

  /*! Stored positions of the middle wires in the phantom frame. These positions come from the segmented positions and the phantom geometry.
    First index is for tracked frame indices, second is for NWires.
  */
  std::vector< std::vector< vnl_vector<double> > > MiddleWirePositionsInPhantomFrame;

  /*! Vector holding the 3D reprojection errors for all NWires
    Computed as a distance between the actual segmented position of the middle wire transformed into phantom frame and the computed positions (see MiddleWirePositionsInPhantomFrame)
  */
  std::vector<double> ReprojectionError3DList;

  /*! Indices of the 3D reprojection errors in ascending order in ReprojectionError3DList */
  std::vector<int> SortedReprojectionError3DIndices;

  /*! Mean 3D reprojection error */
  double ReprojectionError3DMean;

  /*! Standard deviation of 3D reprojection errors */
  double ReprojectionError3DStdDev;

  /*! Vector holding the 2D reprojection errors for all Wires (two elements in the inner vector, first for the X axis and the other for Y)
    Computed as X and Y distances between the actual segmented position of the wires and the intersections of the wires with the image planes
  */
  std::vector< std::vector<double> > ReprojectionError2DList;

  /*! Indices of the 2D reprojection errors in ascending order in ReprojectionError2DList */
  std::vector<int> SortedReprojectionError2DIndices;

  /*! Mean 2D reprojection error (two elements, first for the X axis and the other for Y) */
  std::vector<double> ReprojectionError2DMean;

  /*! Standard deviation of 2D reprojection errors (two elements, first for the X axis and the other for Y) */
  std::vector<double> ReprojectionError2DStdDev;

private:
	vtkProbeCalibrationAlgo(const vtkProbeCalibrationAlgo&);
	void operator=(const vtkProbeCalibrationAlgo&);
};

#endif //  __vtkProbeCalibrationAlgo_h
