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
\brief Landmark detection algorithm detects when a calibrated stylus is pivoting around its tip.
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
    \param newLandmarkDetected is assigned the number of detected landmarks if a new one is detected. It assigns -1 if there is not a new detected landmark.
  */
  PlusStatus InsertNextStylusTipToReferenceTransform(vtkMatrix4x4* stylusTipToReferenceTransform, int &newLandmarkDetected);

  /*! 
    Get detected landmark(s) positions as a string to display.
    \param aPrecision Number of decimals shown.
    \return detection result (e.g. landmark(s) position in reference coordinates system) string.
  */
  std::string GetDetectedLandmarksString(double aPrecision=3);

  /*! Set Device acquisition rate ([samples/sec]). This is required to determine the minimum number of stylus tip poses required for detecting a point. */
  PlusStatus SetAcquisitionRate(double acquisitionRateSamplesPerSec);

  /*! Set size of the time window used for sample averaging (default 0.2 [s]). Higher value makes the algorithm more robust (by tolerating more outliers) but may become less accurate. */
  PlusStatus SetFilterWindowTimeSec(double filterWindowTimeSec);

  /*! Set the minimum time the stylus tip has to be at a fixed position to detect a landmark point (default 1 [s]).*/
  PlusStatus SetDetectionTimeSec(double detectionTimeSec);
  /*! 
    Set the stylus shaft(a point 10 cm above the stylus tip) threshold is used to decide if the stylus is pivoting or static. During the detection time a point in the stylus shaft updates 
    a bounding box. The stylus position is consider for landmark detection if the box's lengths norm is bigger than StylusShaftMinimumDisplacementThresholdMm (default 30 [mm]).
  */
  PlusStatus SetStylusShaftMinimumDisplacementThresholdMm(double stylusShaftMinimumDisplacementThresholdMm);

    /*! 
    A landmark position will be detected if the filtered stylus tip position moves during detection time inside a bounding box 
    with lengths norm smaller than StylusTipMaximumMotionThresholdMm (default 1.5 [mm]).
  */
  PlusStatus SetStylusTipMaximumDisplacementThresholdMm(double stylusTipMaximumMotionThresholdMm);

  /*! Reset the detected landmark points and collected samples*/
  PlusStatus ResetDetection();

  /*! Delete the last landmark detected, in case a landmark is detected incorrectly and need to be detected again.*/
  PlusStatus DeleteLastLandmark();

  /*! Directly inserts a landmark position into the list, bypassing the detection. */
  PlusStatus InsertLandmark_Reference(double stylusTipPosition_Reference[4]);

  /*! Get position of all the detected landmark points in Reference coordinate system. */
  vtkGetObjectMacro(DetectedLandmarkPoints_Reference, vtkPoints);
  
  /*! The minimum distance in between any two defined landmarks in the phantom, it will be a NEW landmark detected only if it is further away from any other already detected landmark. By default 15.0 [mm]. */
  vtkSetMacro(MinimunDistanceBetweenLandmarksMm, double);

  /*! 
    Once a landmark is detected (after DetectionTime) the stylus could still be pivoting in the same place, this function determines if it is a new landmark position 
    or if it is near to an already detected one.
    \param stylusPosition_Reference.
    \return If it is new landmark position -1. The landmark Id if it is already detected.
  */
  int GetNearExistingLandmarkId(double stylusTipPosition_Reference[4]);

protected:

  vtkSetObjectMacro(DetectedLandmarkPoints_Reference, vtkPoints);
  vtkLandmarkDetectionAlgo();
  virtual  ~vtkLandmarkDetectionAlgo();

  /*
    During DetectionTime, the acquisition will be filtered in windows. This method calculates the filter window size as number of samples.
    \param  filterWindowSize = Round(AcquisitionRate*FilterWindowTimeSec);
  */
  PlusStatus ComputeFilterWindowSize(int & filterWindowSize);

  /*
    During DetectionTime, the acquisition will be filtered in windows. This method calculates the total number of windows needed to complete the detection time.
    \param numberOfWindows = DetectionTimeSec/FilterWindowTimeSec;
  */
  PlusStatus ComputeNumberOfWindows(int &  numberOfWindows);

  /* 
    Estimate landmark point position from the stylus tip points when enough samples are collected to fill all the windows.
    Computed as the average of (NumberOfWindows - skipped windows)*WindowSize consecutive points that remain in the same position
    within the LandmarkThresholdMm.
  */
  PlusStatus EstimateLandmarkPosition();

  /*!
    During detection time windows of stylus tip positions are filtered. When a pivot is detected or the stylus tip position is outside the bounding box threshold. 
    Then all the points acquired,except the last window, will be erased.
  */
  PlusStatus KeepLastWindow();

  /* Computes the average of the stylus tip positions in the last FilterWindowSize samples.*/
  PlusStatus FilterStylusTipPositionsWindow(double stylusTipFiltered_Reference[4]);

protected:
  /*! The detected landmark point position(s)(defined in the reference coordinate system).*/
  vtkPoints* DetectedLandmarkPoints_Reference;
  /*! Device acquisition rate. By default is zero so for the algorithm to work it HAS to be set.*/
  double AcquisitionRate;
  /*! Filter window time (default 0.2 [s]).*/
  double FilterWindowTimeSec;
  /*! Detection time (default 1 [s]).*/
  double DetectionTimeSec;
  /*! 
    Stylus shaft(a point 10 cm above the stylus tip) threshold is used to decide if the stylus is pivoting or static. During the detection time a point in the stylus shaft updates 
    a bounding box. The stylus position is consider for landmark detection only if the box's lengths norm is bigger than StylusShaftMinimumDisplacementThresholdMm by default is 30 [mm]).
  */
  double StylusShaftMinimumDisplacementThresholdMm;
  /*!
    A landmark position will be detected if the stylus tip position moves during detection time inside a bounding box with lengths norm smaller than StylusTipMaximumMotionThresholdMm 
    by default is 1.5 [mm].
  */
  double StylusTipMaximumDisplacementThresholdMm;
  /*! The minimum distance in between any two defined landmarks in the phantom.*/
  double MinimunDistanceBetweenLandmarksMm;
  /*! 
    The bounding box is updated during the detection time covering the path of the stylus shaft position. A landmark could be detected only if the box's lengths norm is bigger than
    StylusShaftMinimumDisplacementThresholdMm.
  */
  vtkBoundingBox StylusShaftPathBoundingBox;
  /*! 
    The bounding box is updated during the detection time covering the path of the stylus tip position, if box's lengths norm is smaller than
    StylusTipMaximumDisplacementThresholdMm a landmark is detected.
  */
  vtkBoundingBox StylusTipPathBoundingBox;
  /*! Double ended queue the input point transformations. These are used for determining the next landmark (once the landmark is detected this list is cleared).*/
  std::deque< vtkSmartPointer<vtkMatrix4x4> > StylusTipToReferenceTransformsDeque;
};

#endif
