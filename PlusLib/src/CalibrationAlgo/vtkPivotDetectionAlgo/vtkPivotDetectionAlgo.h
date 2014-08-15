/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __vtkPivotDetectionAlgo_h
#define __vtkPivotDetectionAlgo_h

#include "PlusConfigure.h"

#include "vtkObject.h"
#include "vtkMatrix4x4.h"
#include "vtkPoints.h"

#include <list>
#include <set>

class vtkTransformRepository;
class vtkXMLDataElement;

//-----------------------------------------------------------------------------

/*!
\class vtkPivotDetectionAlgo 
\brief Pivot detection algorithm to detect when a calibrated stylus is pivoting.
The stylus pivoting point is computed assuming that the stylus is calibrated. The algorithm lo

\ingroup PlusLibCalibrationAlgorithm
*/
class vtkPivotDetectionAlgo : public vtkObject
{
public:
  vtkTypeRevisionMacro(vtkPivotDetectionAlgo,vtkObject);
  static vtkPivotDetectionAlgo *New();

  /*!
  * Read configuration
  * \param aConfig Root element of the device set configuration
  */
  PlusStatus ReadConfiguration(vtkXMLDataElement* aConfig);

  /*!
  Remove all previously inserted points.
  Call this method to get rid of previously added points before starting a new detection.
  */
  void RemoveAllDetectionPoints();

  /*!
  Insert acquired point to the detection point list
  \param stylusTipToReferenceTransform New detection point (stylus tip to reference transform)
  */
  PlusStatus InsertNextDetectionPoint(vtkMatrix4x4* stylusTipToReferenceTransform);

  /*! Get detected pivot(s) string to display
  \param aPrecision Number of decimals shown
  \return detection result (e.g. pivot(s) position in reference coordinates system) string
  */
  std::string GetDetectedPivotsString(double aPrecision=3);

  /*! Set Device acquisition rate. This is required.*/
  void SetAcquisitionRate(double aquisitionRate); 
  /*! Set WindowTime (default 1/3 [s])*/
  void SetWindowTimeSec(double windowTime);
  /*! Set total DetectionTime (default 2 [s])*/
  void SetDetectionTimeSec(double detectionTime);
  /*! Set the above the pivot threshold. It is used to detect that stylus is pivoting and not static. When a point 10 cm above the stylus tip magnitude change is bigger than AbovePivotThresholdMM, the stylus is pivoting.*/
  void SetAbovePivotThresholdMM(double abovePivotThreshold);
  /*! Set the pivot threshold. A pivot position will be consider when the stylus tip position magnitude change is below SamePivotThresholdMM.  */
  void SetPivotThresholdMM(double samePivotThreshold);

  //detected when the difference of two windows 
  //Set the above the pivot threshold. It is used to detect that stylus is pivoting, not static. When the difference of two windows point 10 cm above the stylus tip average is bigger than this AbovePivotThresholdMM.
  //  stylus tip position average is below this threshold.

public:

  vtkGetStringMacro(ObjectMarkerCoordinateFrame);
  vtkGetStringMacro(ReferenceCoordinateFrame);
  vtkGetStringMacro(ObjectPivotPointCoordinateFrame);
  vtkGetObjectMacro(PivotPointsReference, vtkPoints);
  PlusStatus PivotFound();

protected:

  vtkSetStringMacro(ObjectMarkerCoordinateFrame);
  vtkSetStringMacro(ReferenceCoordinateFrame);
  vtkSetStringMacro(ObjectPivotPointCoordinateFrame);
  vtkSetObjectMacro(PivotPointsReference, vtkPoints);
  vtkPivotDetectionAlgo();
  virtual  ~vtkPivotDetectionAlgo();
  /* Estimate pivot point position from the stylus tip points. The average of NumberOfWindows*WindowSize consecutive points that remain in the same position within the PivotThresholdMM.*/
  PlusStatus EstimatePivotPointPosition();
  /*! Once the pivot is detected (after DetectionTime) the stylus could still be pivoting in the same place, this function determines if it is a new pivot position if it is not the pivot position will be averaged with the existing one.*/
  bool IsNewPivotPointPosition(double* stylusPosition);
  /*! The change in pivot positions is measured in windows of points. If the current window is not considered the same pivoting point as the one before. The points acquired that belong to the first window will be erased by this function.*/
  void EraseLastPoints();
/* Computes the average of the stylus tip positions in the latest acquired window */
  PlusStatus GetStylusTipPositionWindowAverage(double* pivotPoint_Reference);

protected:
  /*! The detected pivot point position(s)(defined in the reference coordinate system) */
  vtkPoints* PivotPointsReference;
  /*! The flag is true when a pivot is detected it is false when PivotFound() is called.*/
  bool PivotDetected;
  bool Verbose;
  //The number of windows to be detected as pivot in DetectionTime (DetectionTime/WindowTime)
  int NumberOfWindows;
  //The number of acquisitions in WindowTime (AcquisitonRate/WindowTime)
  int WindowSize;

  /*! Device acquisition rate */
  double AcquisitionRate;
  /*! WindowTime (1/3 [s])*/
  double WindowTimeSec;
  /*! DetectionTime (2 [s])*/
  double DetectionTimeSec;
 /*! Above the pivot threshold is used to detect stylus pivoting and not static. When a point 10 cm above the stylus tip magnitude change is bigger than AbovePivotThresholdMM, the stylus is pivoting.*/
  double AbovePivotThresholdMM;
  /*! A pivot position will be consider when the stylus tip position magnitude change is below PivotThresholdMM.*/
  double PivotThresholdMM;
  /*! Name of the object marker coordinate frame (eg. Stylus) */
  char*               ObjectMarkerCoordinateFrame;
  /*! Name of the reference coordinate frame (eg. Reference) */
  char*               ReferenceCoordinateFrame;
  /*! Name of the object pivot point coordinate frame (eg. StylusTip) */
  char*               ObjectPivotPointCoordinateFrame;

  /*! Array of the input point transformations*/
  std::list< vtkMatrix4x4* > StylusTipToReferenceTransformsList;
  /*! Iterators to track stylus tip transformations in the list*/
  std::list< vtkMatrix4x4* >::iterator CurrentStylusTipIterator;
  std::list< vtkMatrix4x4* >::iterator LastStylusTipIterator;
  /*! The number of partial inserted points*/
  int PartialInsertedPoints;
  /*! Pivot point position in the Reference coordinate system */
  double              AboveStylusTipAverage[4];
  /*! Pivot point position in the Reference coordinate system */
  double              LastAboveStylusTipAverage[4];
  /*! Pivot point position average per window list (defined in the reference coordinate system)*/
  std::list< std::vector<double> > StylusTipWindowAverage_Reference_List;
};

#endif
