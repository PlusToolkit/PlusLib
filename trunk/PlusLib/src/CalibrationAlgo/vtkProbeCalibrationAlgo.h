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

#include "vtkSpatialCalibrationOptimizer.h"

#include <set>

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
  PlusStatus Calibrate( vtkTrackedFrameList* validationTrackedFrameList, int validationStartFrame, int validationEndFrame, vtkTrackedFrameList* calibrationTrackedFrameList, int calibrationStartFrame, int calibrationEndFrame, vtkTransformRepository* transformRepository, const std::vector<NWire> &nWires ); 

  /*!
    Run calibration algorithm on the two input frame lists (uses every frame in the two sequences)
    \param validationTrackedFrameList TrackedFrameList with segmentation results for the validation
    \param calibrationTrackedFrameList TrackedFrameList with segmentation results for the calibration
    \param transformRepository Transform repository object to be able to get the default transform
    \param nWires NWire structure that contains the computed imaginary intersections. It used to determine the computed position
  */
  PlusStatus Calibrate( vtkTrackedFrameList* validationTrackedFrameList, vtkTrackedFrameList* calibrationTrackedFrameList, vtkTransformRepository* transformRepository, const std::vector<NWire> &nWires ); 

  /*!
    Assembles the result string to display
    \param precision Number of decimals printed in the string
    \return String containing results
  */
  std::string GetResultString(int precision = 3);

  /*!
    Get calibration result and error report in XML format 
    \param validationTrackedFrameList TrackedFrameList with segmentation results for the validation
    \param validationStartFrame First frame that is used from the validation tracked frame list for the validation (in case of -1 it starts with the first)
    \param validationEndFrame Last frame that is used from the validation tracked frame list for the validation (in case of -1 it starts with the last)
    \param calibrationTrackedFrameList TrackedFrameList with segmentation results for the calibration
    \param calibrationStartFrame First frame that is used from the calibration tracked frame list for the calibration (in case of -1 it starts with the first)
    \param calibrationEndFrame Last frame that is used from the calibration tracked frame list for the calibration (in case of -1 it starts with the last)
    \param probeCalibrationResult Output XML data element 
  */
  PlusStatus GetXMLCalibrationResultAndErrorReport(vtkTrackedFrameList* validationTrackedFrameList, int validationStartFrame, 
    int validationEndFrame, vtkTrackedFrameList* calibrationTrackedFrameList, int calibrationStartFrame, int calibrationEndFrame, 
    vtkXMLDataElement* probeCalibrationResult); 

public:
  /*! Set/get the calibration date and time in string format */
  vtkSetStringMacro(CalibrationDate); 
  /*! Set/get the calibration date and time in string format */
  vtkGetStringMacro(CalibrationDate);

  double GetCalibrationReprojectionError3DMean();
  double GetCalibrationReprojectionError3DStdDev();

  double GetValidationReprojectionError3DMean();
  double GetValidationReprojectionError3DStdDev();

  /*! Get the image coordinate frame name */
  vtkGetStringMacro(ImageCoordinateFrame);
  /*! Get the probe coordinate frame name */
  vtkGetStringMacro(ProbeCoordinateFrame);
  /*! Get the phantom coordinate frame name */
  vtkGetStringMacro(PhantomCoordinateFrame);
  /*! Get the reference coordinate frame name */
  vtkGetStringMacro(ReferenceCoordinateFrame);
  
  /*! Get/set the calibration result transformation matrix */
  void GetImageToProbeTransformMatrix(vtkMatrix4x4* imageToProbeMatrix);

public:
  /*! Get 2D reprojection statistics for a specified wire made from the validation or the calibration data */
  PlusStatus GetReprojectionError2DStatistics(double &xMean, double &yMean, double &xStdDev, double &yStdDev, int wireNumber, bool isValidation);


  PlusStatus GetCalibrationReport(std::vector<double> *calibError,std::vector<double> *validError,vnl_matrix_fixed<double,4,4> *imageToProbeTransformMatrix); 

  vtkSpatialCalibrationOptimizer* GetSpatialCalibrationOptimizer()
  {
    return SpatialCalibrationOptimizer;
  };

  void ComputeError2d(const vnl_matrix_fixed<double,4,4> &imageToProbeMatrix, double &errorMean, double &errorStDev, double &errorRms);
  void ComputeError3d(const vnl_matrix_fixed<double,4,4> &imageToProbeMatrix, double &errorMean, double &errorStDev, double &errorRms);

protected:

  enum PreProcessedWirePositionIdType
  {
    CALIBRATION_ALL=0,
    VALIDATION_ALL,
    CALIBRATION_NOT_OUTLIER,
    LAST_PREPROCESSED_WIRE_POS_ID // this must be the last type
  };

  void ComputeError3d(std::vector<double> &reprojectionErrors, PreProcessedWirePositionIdType datasetType, const vnl_matrix_fixed<double,4,4> &imageToProbeMatrix);

  void ComputeError2d(PreProcessedWirePositionIdType datasetType, const vnl_matrix_fixed<double,4,4> &imageToProbeMatrix, 
    double &errorMean, double &errorStDev, double &errorRms,
    std::vector< std::vector< vnl_vector_fixed<double,2> > >* ReprojectionError2Ds=NULL);

  /*!
    Calculate and add positions of an individual image for calibration or validation
    \param trackedFrame The actual tracked frame (already segmented) to add for calibration or validation
    \param transformRepository Transform repository object to be able to get the default transform
    \param isValidation Flag whether the added data is for calibration or validation
  */
  PlusStatus AddPositionsPerImage( TrackedFrame* trackedFrame, vtkTransformRepository* transformRepository, PreProcessedWirePositionIdType datasetType);

  /*!
    Calculate 3D reprojection errors
    \param trackedFrameList Tracked frame list for error computation
    \param startFrame First frame that is used from the tracked frame list for the error computation (in case of -1 it starts with the first)
    \param endFrame Last frame that is used from the tracked frame list for the error computation (in case of -1 it starts with the last)
    \param transformRepository Transform repository object to be able to get the default transform
    \param isValidation Flag whether the input tracked frame list is of calibration or validation
  */  
  PlusStatus ComputeReprojectionErrors3D(PreProcessedWirePositionIdType datasetType, const vnl_matrix_fixed<double,4,4> &imageToProbeTransformMatrix);

  /*!
    Calculate 2D reprojection errors
    \param trackedFrameList Tracked frame list for validation
    \param startFrame First frame that is used from the tracked frame list for the error computation (in case of -1 it starts with the first)
    \param endFrame Last frame that is used from the tracked frame list for the error computation (in case of -1 it starts with the last)
    \param transformRepository Transform repository object to be able to get the default transform
    \param isValidation Flag whether the input tracked frame list is of calibration or validation
  */
  PlusStatus ComputeReprojectionErrors2D(PreProcessedWirePositionIdType datasetType, const vnl_matrix_fixed<double,4,4> &imageToProbeTransformMatrix);

  /*! 
    Set ImageToProbe calibration result matrix and validate it. It doesn't modify the original transform to make the rotation orthogonal
    \param imageToProbeTransformMatrix the calculated image to probe matrix
    \param transformRepository the transform repository to populate   
  */
  void SetAndValidateImageToProbeTransform( const vnl_matrix_fixed<double,4,4> &imageToProbeTransformMatrix, vtkTransformRepository* transformRepository);

  /*! 
    Save results and error report to XML 
  */
  PlusStatus SaveCalibrationResultAndErrorReportToXML(vtkTrackedFrameList* validationTrackedFrameList, int validationStartFrame, int validationEndFrame, vtkTrackedFrameList* calibrationTrackedFrameList, int calibrationStartFrame, int calibrationEndFrame);

  /*!
    \param outliers indices of the measurement points that was found to be an outlier when computing any matrix row
  */
  PlusStatus ComputeImageToProbeTransformByLinearLeastSquaresMethod(vnl_matrix_fixed<double,4,4> &imageToProbeTransformMatrix, std::set<int> &outliers);

  /*! Remove outliers from calibration data
  */
  void UpdateNonOutlierData(const std::set<int>& outliers);

  static double PointToWireDistance(const vnl_double_3 &aPoint, const vnl_double_3 &aLineEndPoint1, const vnl_double_3 &aLineEndPoint2);

protected:
  /*! Set the image coordinate frame name */
  vtkSetStringMacro(ImageCoordinateFrame);
  /*! Set the probe coordinate frame name */
  vtkSetStringMacro(ProbeCoordinateFrame);
  /*! Set the phantom coordinate frame name */
  vtkSetStringMacro(PhantomCoordinateFrame);
  /*! Set the reference coordinate frame name */
  vtkSetStringMacro(ReferenceCoordinateFrame);

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

  /*! The result of the calibration */
  vnl_matrix_fixed<double,4,4> ImageToProbeTransformMatrix;

  /*! List of NWires used for calibration and error computation */
  std::vector<NWire> NWires;

  /*! Stores wire intersection positions for each frame. */
  struct NWirePositionType
  {
    /*! 
      Positions of segmented points in image frame - input of optimization algorithm, contains ALL the segmented points
      nwire x 3 values
    */
    std::vector< vnl_vector_fixed<double,4> > AllWiresIntersectionPointsPos_Image;
    /*! 
      Positions of segmented points in probe frame - input of optimization algorithm 
      nwire values
    */
    std::vector< vnl_vector_fixed<double,4> > MiddleWireIntersectionPointsPos_Probe;

    /*! 
      Vector containing all Probe to Phantom transforms 
    */
    vnl_matrix_fixed<double,4,4> ProbeToPhantomTransform;
  };

  struct NWireErrorType
  {
    /*! 
      Vector holding the 3D reprojection errors for each NWire in all validation images (outer vector is for the NWires, inner one is for the images)
      Computed as a distance between the actual segmented position of the middle wire transformed into phantom frame and the computed positions (see MiddleWirePositionsInPhantomFrame)
      indices: [wire][frame]
    */
    std::vector< std::vector<double> > ReprojectionError3Ds;

    /*! 
      Vector holding the 2D reprojection errors for each wire in all validation images (outermost vector holds the wires, the one inside it holds the images, and the inner holds the X and Y errors)
      Computed as X and Y distances between the actual segmented position of the wires and the intersections of the wires with the image planes
      indices: [wire][frame][x/y]
    */
    std::vector< std::vector< vnl_vector_fixed<double,2> > > ReprojectionError2Ds;

    /*! Mean validation 2D reprojection error for each wire (two elements, first for the X axis and the other for Y) (using confidence interval) 
    indices: [wire][x/y]
    */
    std::vector< vnl_vector_fixed<double,2> > ReprojectionError2DMeans;

    /*! Standard deviation of validation 2D reprojection errors for each wire (two elements, first for the X axis and the other for Y) (using confidence interval) 
    indices: [wire][x/y]
    */
    std::vector< vnl_vector_fixed<double,2> > ReprojectionError2DStdDevs;

    /*! Mean validation 3D reprojection error (using confidence interval) */
    double ReprojectionError3DMean;

    /*! Standard deviation of validation 3D reprojection errors (using confidence interval) */
    double ReprojectionError3DStdDev;

    /*! Maximum 3D reprojection error (using confidence interval) */
    double ReprojectionError3DMax;
    
    double RmsError2D;

    NWireErrorType()
      : ReprojectionError3DMean(-1.0)
      , ReprojectionError3DStdDev(-1.0)
      , RmsError2D(-1.0)
    {}

  };

  struct PreProcessedWirePositionsType
  {
    std::vector<NWirePositionType> FramePositions;
    NWireErrorType NWireErrors;
    
    void Clear()
    {
      FramePositions.clear();

      NWireErrors.ReprojectionError3Ds.clear();
      NWireErrors.ReprojectionError2Ds.clear();
      NWireErrors.ReprojectionError2DStdDevs.clear();
      NWireErrors.ReprojectionError2DMeans.clear();
    }
  };

  PreProcessedWirePositionsType PreProcessedWirePositions[LAST_PREPROCESSED_WIRE_POS_ID];

  /*!
    Confidence level (trusted zone) as a percentage of the independent validation data used to produce the final error computation results.  It serves as an effective way to get rid of corrupted data
    (or outliers) in the validation dataset. Default value: 0.95 (or 95%), meaning the top ranked 95% of the ascendingly-ordered PRE values from the validation data would be accepted as the valid PRE values.
  */
  double ErrorConfidenceLevel;

  vtkSpatialCalibrationOptimizer* SpatialCalibrationOptimizer;

private:
  vtkProbeCalibrationAlgo(const vtkProbeCalibrationAlgo&);
  void operator=(const vtkProbeCalibrationAlgo&);
};

#endif //  __vtkProbeCalibrationAlgo_h
