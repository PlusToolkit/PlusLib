/*!=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkPlusPrincipalMotionDetectionAlgo_h
#define __vtkPlusPrincipalMotionDetectionAlgo_h

#include <deque>
#include "vtkObject.h"

//class vtkIGSIOTrackedFrameList;

/*!
  \class vtkPlusPrincipalMotionDetectionAlgo
  \brief Extract the motion component along the the principal axis of the motion. Used for computing a position metric from a periodically moving tool.
  \ingroup PlusLibCalibrationAlgorithm
*/
class vtkPlusPrincipalMotionDetectionAlgo: public vtkObject
{
public:
  static vtkPlusPrincipalMotionDetectionAlgo* New();
  vtkTypeMacro(vtkPlusPrincipalMotionDetectionAlgo, vtkObject);
  virtual void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

  /*! Sets the input US video frames */
  void SetTrackerFrames(vtkIGSIOTrackedFrameList* trackerFrames);

  /*! Sets the time range where the signal will be extracted from. If rangeMax<rangeMin then all the input frames will be used to genereate the signal. */
  void SetSignalTimeRange(double rangeMin, double rangeMax);

  /*! Sets the name of the transform to be used for tracking data. Default is "ProbeToReference" */
  void SetProbeToReferenceTransformName(const std::string& probeToReferenceTransformName);

  /*!
    Run the line detection algorithm on the input video frames
    \param errorDetail if the algorithm fails then the details of the problem are returned in this string
  */
  PlusStatus Update();

  /*! Get the timestamps of the frames where a line was successfully detected*/
  void GetDetectedTimestamps(std::deque<double>& timestamps);

  /*! Get the line positions on the frames where a line was successfully detected*/
  void GetDetectedPositions(std::deque<double>& positions);

  void ComputePrincipalAxis(std::deque<itk::Point<double, 3> >& trackerPositions, itk::Point<double, 3>& principalAxisOfMotion, int numValidFrames);

protected:
  vtkPlusPrincipalMotionDetectionAlgo();
  virtual ~vtkPlusPrincipalMotionDetectionAlgo();

  PlusStatus VerifyInputFrames();
  PlusStatus ComputeTrackerPositionMetric();

  vtkIGSIOTrackedFrameList* m_TrackerFrames;
  std::string m_ProbeToReferenceTransformName;

  std::deque<double> m_SignalValues;
  std::deque<double> m_SignalTimestamps;

  double m_SignalTimeRangeMin;
  double m_SignalTimeRangeMax;

private:
  vtkPlusPrincipalMotionDetectionAlgo(const vtkPlusPrincipalMotionDetectionAlgo&);
  void operator=(const vtkPlusPrincipalMotionDetectionAlgo&);
};

#endif //  __vtkPlusPrincipalMotionDetectionAlgo_h 
