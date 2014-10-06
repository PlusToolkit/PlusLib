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
#include "vtkPoints.h"
#include "vtkBoundingBox.h"

#include <list>
#include <deque>
#include <set>

class vtkMatrix4x4;
class vtkTransformRepository;
class vtkXMLDataElement;

//-----------------------------------------------------------------------------

/*!
\class vtkLandmarkDetectionAlgo 
\brief Landmark detection algorithm detects when a calibrated stylus is pivoting.
The stylus pivoting point (landmark) is computed assuming that the stylus is calibrated.
\ingroup PlusLibCalibrationAlgorithm
*/
class vtkCalibrationAlgoExport vtkLandmarkDetectionAlgo : public vtkObject
{
public:
  vtkTypeMacro(vtkLandmarkDetectionAlgo,vtkObject);
  static vtkLandmarkDetectionAlgo *New();

  /*!
  Read configuration.
  \param aConfig Root element of the device set configuration.
  */
  PlusStatus ReadConfiguration(vtkXMLDataElement* aConfig);
  /*!
  Insert acquired pose to the stylusTipToReferenceTransform double ended queue.
  \param stylusTipToReferenceTransform New detection pose (stylus tip to reference transform).
  \param newLandmarkDetected flag is set if after the insertion a new landmark is detected.
  */
  PlusStatus InsertNextStylusTipToReferenceTransform(vtkSmartPointer<vtkMatrix4x4> stylusTipToReferenceTransform, bool &newLandmarkDetected);
  /*! 
  Get detected landmark(s) string to display.
  \param aPrecision Number of decimals shown.
  \return detection result (e.g. landmark(s) position in reference coordinates system) string.
  */
  std::string GetDetectedLandmarksString(double aPrecision=3);
  /*! Set Device acquisition rate. This is required to determine the minimum number of stylus tip poses required for detecting a point. */
  void SetAcquisitionRate(double acquisitionRateSamplesPerSec);
  /*! Set size of the time window used for sample averaging (default 1/3 [s]). Higher value makes the algorithm ignore more outliers. */
  void SetFilterWindowTimeSec(double filterWindowTimeSec);
  /*! Set the minimum time the stylus tip has to be at a fixed position to detect a landmark point (default 2 [s]).*/
  void SetDetectionTimeSec(double detectionTimeSec);
  /*!
    Set stylus shaft threshold. It is used to detect that stylus is pivoting and not static.
    When a point in the stylus shaft (10 cm above the stylus tip) move during detection time more than StylusShaftThresholdMm, the stylus is pivoting.
  */
  void SetStylusShaftMinimumDisplacementThresholdMm(double stylusShaftMinimumDisplacementThresholdMm);
  /*! Set the landmark threshold. A landmark position will be consider when the stylus tip position magnitude change is below SameLandmarkThresholdMm.*/
  void SetStylusTipMaximumDisplacementThresholdMm(double stylusTipMaximumMotionThresholdMm);
  /*! Set the number of expected landmarks to be found. Used for deciding if the algorithm is completed.*/
  void SetNumberOfExpectedLandmarks(int numberOfExpectedLandmarks);
  /* The flag completed will be true when the number of landmark points detected is equal to the expected number of landmark points.*/
  PlusStatus IsLandmarkDetectionCompleted(bool &completed);
  /* Reset the landmark point detected to start over */
  PlusStatus ResetDetection();
  /* It deletes the last landmark detected, in case it does not correspond to a landmark and could be detected again.*/
  PlusStatus DeleteLastLandmark();
  /*Directly inserts a landmark position into the list, bypassing the detection.*/
  PlusStatus InsertLandmark_Reference(double* stylusTipPosition_Reference);

  vtkGetStringMacro(ReferenceCoordinateFrame);
  vtkGetObjectMacro(DetectedLandmarkPoints_Reference, vtkPoints);
  vtkGetMacro(NumberOfExpectedLandmarks, int);
  vtkSetMacro(MinimunDistanceBetweenLandmarksMm, double);

  /*! 
  Once a landmark is detected (after DetectionTime) the stylus could still be pivoting in the same place, this function determines if it is a new landmark position 
  or if it is near to an already detected one.
  \param stylusPosition_Reference.
  \return If it is new landmark position -1. The landmark Id if it is already detected.
  */
  int GetNearExistingLandmarkId(double* stylusPosition_Reference);

protected:

  vtkSetStringMacro(ReferenceCoordinateFrame);
  vtkSetObjectMacro(DetectedLandmarkPoints_Reference, vtkPoints);
  vtkLandmarkDetectionAlgo();
  virtual  ~vtkLandmarkDetectionAlgo();

  /*
  During DetectionTime, the acquisition will be filtered in windows. This method calculates the filter window size.
  \param filterWindowSize=DetectionTime/FilterWindowTime.
  */
  void GetFilterWindowSize(int & filterWindowSize);
  /* 
  Estimate landmark point position from the stylus tip points. The average of NumberOfWindows*WindowSize consecutive points that remain in the same position 
  within the LandmarkThresholdMm.
  */
  PlusStatus EstimateLandmarkPosition();

  /*Sets the parameter boolean flag found true when a new landmark is found.*/
  PlusStatus IsNewLandmarkPointFound(bool &found);

  /*!
  During detection time windows of stylus tip positions are filtered. When a pivot is detected or the stylus tip position is outside the bounding box threshold. 
  The all the points acquired,except the last window, will be erarsed.
  */
  void KeepLastWindow();

  /* Computes the average of the stylus tip positions in the last FilterWindowSize samples.*/
  PlusStatus FilterStylusTipPositionsWindow(double* landmarkPoint_Reference);

protected:
  /*! The detected landmark point position(s)(defined in the reference coordinate system).*/
  vtkPoints* DetectedLandmarkPoints_Reference;
  /*! The flag is true when a landmark is detected it is reset again to false when LandmarkFound() is called.*/
  bool NewLandmarkFound;
  /*! The expected number of landmarks to be detected (default 3).*/
  int NumberOfExpectedLandmarks;
  /*! Device acquisition rate. By default is zero so for the algorithm to work it HAS to be set.*/
  double AcquisitionRate;
  /*! Filter window time (default 1/3 [s]).*/
  double FilterWindowTimeSec;
  /*! Detection time (default 2 [s]).*/
  double DetectionTimeSec;
  /*! 
  Stylus shaft threshold is used to decide if the stylus is pivoting or static. When a point, 10 cm above the stylus tip, in the stylus shaft moved, during the detection time, 
  more than StylusShaftThresholdMm the stylus is consider for landmark detection. By default 1.5 [mm].
  */
  double StylusShaftMinimumDisplacementThresholdMm;
  /*! A landmark position will be detected when the stylus tip position magnitude change is below StylusTipMaximumMotionThresholdMm. By default 30 [mm].*/
  double StylusTipMaximumDisplacementThresholdMm;
  /*! The minimum distance in between any two defined landmarks in the phantom, it will be a NEW landmark detected only if it is further away from any other already detected landmark. By default 15.0 [mm].*/
  double MinimunDistanceBetweenLandmarksMm;
  /*! The bounding box is updated during the detection time covering the path of the stylus shaft position.*/
  vtkBoundingBox StylusShaftPathBoundingBox;
  /*! 
  The bounding box is updated during the detection time covering the path of the stylus tip position, if length of the box norm is smaller than
  StylusTipMaximumDisplacementThresholdMm a landmark is detected.
  */
  vtkBoundingBox StylusTipPathBoundingBox;
  /*! Name of the reference coordinate frame (eg. Reference).*/
  char* ReferenceCoordinateFrame;
  /*! Double ended queue the input point transformations. */
  std::deque< vtkSmartPointer<vtkMatrix4x4> > StylusTipToReferenceTransformsDeque;
};

#endif
