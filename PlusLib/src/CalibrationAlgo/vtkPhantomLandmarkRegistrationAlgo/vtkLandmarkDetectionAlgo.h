/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __vtkLandmarkDetectionAlgo_h
#define __vtkLandmarkDetectionAlgo_h

#include "PlusConfigure.h"
#include "vtkCalibrationAlgoExport.h"

#include "vtkObject.h"
#include "vtkMatrix4x4.h"
#include "vtkPoints.h"
#include "vtkBoundingBox.h"

#include <list>
#include <set>

class vtkTransformRepository;
class vtkXMLDataElement;

//-----------------------------------------------------------------------------

/*!
\class vtkLandmarkDetectionAlgo 
\brief Landmark detection algorithm detects when a calibrated stylus is pivoting.
The stylus pivoting point is computed assuming that the stylus is calibrated.

\ingroup PlusLibCalibrationAlgorithm
*/
class vtkCalibrationAlgoExport vtkLandmarkDetectionAlgo : public vtkObject
{
public:
  vtkTypeRevisionMacro(vtkLandmarkDetectionAlgo,vtkObject);
  static vtkLandmarkDetectionAlgo *New();

  /*!
    Read configuration
    \param aConfig Root element of the device set configuration
  */
  PlusStatus ReadConfiguration(vtkXMLDataElement* aConfig);

  /*!
    Insert acquired pose to the detection pose list
    \param stylusTipToReferenceTransform New detection pose (stylus tip to reference transform)
  */
  PlusStatus InsertNextStylusTipToReferenceTransform(vtkSmartPointer<vtkMatrix4x4> stylusTipToReferenceTransform);

  /*! Get detected landmark(s) string to display
    \param aPrecision Number of decimals shown
    \return detection result (e.g. landmark(s) position in reference coordinates system) string
  */
  std::string GetDetectedLandmarksString(double aPrecision=3);

  /*! Set Device acquisition rate. This is required to determine the minimum number of transforms required for detecting a point. */
  void SetAcquisitionRate(double aquisitionRate);
  /*! Set size of the time window used for sample averaging (default 1/3 [s]). Higher value makes the algorithm ignore more outliers. */
  void SetWindowTimeSec(double windowTimeSec);
  /*! Set the minimum time the stylus tip has to be at a fixed position to detect a landmark point (default 2 [s]) */
  void SetDetectionTimeSec(double detectionTime);
  /*!
    Set stylus shaft threshold. It is used to detect that stylus is pivoting and not static.
    When a point in the stylus shaft (10 cm above the stylus tip) change is bigger than StylusShaftThresholdMm, the stylus is pivoting.
  */
  void SetStylusShaftThresholdMm(double stylusShaftThreshold);
  /*! Set the landmark threshold. A landmark position will be consider when the stylus tip position magnitude change is below SameLandmarkThresholdMm.  */
  void SetLandmarkThresholdMm(double sameLandmarkThreshold);
  /*! Set the number of expected landmarks to be found. Used for deciding if the algorithm is completed. */
  void SetExpectedLandmarksNumber(int expectedLandmarksNumber);

  /* The flag completed will be true when the number of landmark points detected is equal to the expected number of landmark points */
  PlusStatus IsLandmarkDetectionCompleted(bool &completed);

  /* Reset the landmark point detected to start over */
  PlusStatus ResetDetection();

  /*It deletes the last landmark detected, in case it does not correspond to a landmark and could be detected again*/
  PlusStatus DeleteLastLandmark();

  /*Sets the boolean flag true when a new landmark is found*/
  PlusStatus IsNewLandmarkPointFound(bool &found);

  /*Directly inserts a landmark position into the list, bypassing the detection*/
  PlusStatus InsertLandmark(double* stylusTipPosition);

  vtkGetStringMacro(ReferenceCoordinateFrame);
  vtkGetObjectMacro(LandmarkPointsReference, vtkPoints);
  vtkGetMacro(ExpectedLandmarksNumber, int);
  vtkSetMacro(MinimunDistanceBetweenLandmarksMm, double);

  /*! Once the landmark is detected (after DetectionTime) the stylus could still be pivoting in the same place, this function determines if it is a new landmark position if it is not the landmark position will be averaged with the existing one.*/
  int IsNewLandmarkPointPosition(double* stylusPosition);
  //int GetNearExistingLandmark(double* stylusPosition);

protected:

  vtkSetStringMacro(ReferenceCoordinateFrame);
  vtkSetObjectMacro(LandmarkPointsReference, vtkPoints);
  vtkLandmarkDetectionAlgo();
  virtual  ~vtkLandmarkDetectionAlgo();

  /*!
    Remove all previously inserted points.
    Call this method to get rid of previously added points before starting a new detection.
  */
  void RemoveAllDetectionPoints();

  /* Estimate landmark point position from the stylus tip points. The average of NumberOfWindows*WindowSize consecutive points that remain in the same position within the LandmarkThresholdMm.*/
  PlusStatus EstimateLandmarkPointPosition();

  /*!
    The change in landmark positions is measured in windows of points.
    If the current window is not considered the same pivoting point as the one before. The points acquired that belong to the first window will be erased by this function.
  */
  void EraseLastPoints();

  /* Computes the average of the stylus tip positions in the last FilterWindowSize samples */
  PlusStatus GetStylusTipPositionWindowAverage(double* landmarkPoint_Reference);

protected:
  /*! The detected landmark point position(s)(defined in the reference coordinate system) */
  vtkPoints* LandmarkPointsReference;

  /*The vector is used to average position of the same landmark found*/
  std::vector<double> NumberOfWindowsFoundPerLandmark;

  /*! The flag is true when a landmark is detected it is reset again to false when LandmarkFound() is called.*/
  bool NewLandmarkFound;
  /*! The number of windows to be detected as landmark in DetectionTime (DetectionTime/WindowTime) */
  int NumberOfWindows;
  /*! The number of acquisitions in WindowTime (AcquisitonRate/WindowTime) */
  int WindowSize;

  /*! The expected number of landmarks to be detected */
  int ExpectedLandmarksNumber;
  /*! Device acquisition rate */
  double AcquisitionRate;
  /*! WindowTime (1/3 [s])*/
  double WindowTimeSec;
  /*! DetectionTime (2 [s])*/
  double DetectionTimeSec;
  /*! Stylus Shaft threshold is used to detect stylus pivoting and not static. When a point 10 cm above the stylus tip magnitude change is bigger than AboveLandmarkThresholdMm, the stylus is pivoting.*/
  double StylusShaftThresholdMm;
  /*! A landmark position will be consider when the stylus tip position magnitude change is below LandmarkThresholdMm.*/
  double LandmarkThresholdMm;
  /*! The minimum distance in between any two landmarks, it will be a NEW landmark detected only if it is further away from any other already detected landmark .*/
  double MinimunDistanceBetweenLandmarksMm;

  /*! TODO: add doc */
  vtkBoundingBox BoundingBox;

  /*! Name of the reference coordinate frame (eg. Reference) */
  char* ReferenceCoordinateFrame;

  /*! Array of the input point transformations */
  std::list< vtkSmartPointer<vtkMatrix4x4> > StylusTipToReferenceTransformsList;
  /*! Iterators to track stylus tip transformations in the list*/
  std::list< vtkSmartPointer<vtkMatrix4x4> >::iterator CurrentStylusTipIterator;
  /*! TODO: add doc */
  std::list< vtkSmartPointer<vtkMatrix4x4> >::iterator LastStylusTipIterator;
  /*! The number of partial inserted points*/
  int PartialInsertedPoints;
  /*! Landmark point position in the Reference coordinate system */
  double AboveStylusTipAverage[4];
  /*! Landmark point position in the Reference coordinate system */
  double LastAboveStylusTipAverage[4];
  /*! Landmark point position average per window list (defined in the reference coordinate system)*/
  std::list< std::vector<double> > StylusTipWindowAverage_Reference_List;
};

#endif
