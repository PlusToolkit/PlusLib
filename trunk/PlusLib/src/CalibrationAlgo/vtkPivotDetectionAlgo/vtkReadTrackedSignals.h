/*!=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __vtkReadTrackedSignals_h
#define __vtkReadTrackedSignals_h

#include <deque>
#include "vtkObject.h"

class vtkTrackedFrameList;

/*!
  \class vtkReadTrackedSignals
  \brief Extract the motion component along the the principal axis of the motion. Used for computing a position metric from a periodically moving tool.
  \ingroup PlusLibCalibrationAlgorithm
*/
class vtkReadTrackedSignals: public vtkObject
{
public:
  static vtkReadTrackedSignals* New();
  vtkTypeRevisionMacro(vtkReadTrackedSignals, vtkObject);
  virtual void PrintSelf(ostream& os, vtkIndent indent); 
 
  /*! Sets the input US video frames */  
  void SetTrackerFrames(vtkTrackedFrameList* trackerFrames);

  /*! Sets the time range where the signal will be extracted from. If rangeMax<rangeMin then all the input frames will be used to genereate the signal. */
  void SetSignalTimeRange(double rangeMin, double rangeMax);

  /*! Sets the name of the transform to be used for tracking data. Default is "ProbeToReference" */  
  void SetProbeToReferenceTransformName(const std::string& probeToReferenceTransformName);

  void SetCalibrationConfigName(const std::string& calibrationConfigName);

  /*!
    Run the line detection algorithm on the input video frames
    \param errorDetail if the algorithm fails then the details of the problem are returned in this string
  */
  PlusStatus Update(); 

  /*! Get the timestamps of the frames where a line was successfully detected*/
  void GetDetectedTimestamps(std::deque<double> &timestamps);


  void GetDetectedSignalStylusRef(std::deque<double> &signalComponent);
  void GetDetectedSignalStylusTipRef(std::deque<double> &signalComponent);
  void GetDetectedSignalZ(std::deque<double> &signalComponent);
  void GetDetectedSignalStylusTipFromPivot(std::deque<double> &signalComponent);

  /*! Get the line positions on the frames where a line was successfully detected*/
  void GetDetectedPositions(std::deque<double> &positions); 

  void ComputePrincipalAxis(std::deque<itk::Point<double, 3> > &trackerPositions, itk::Point<double,3> &principalAxisOfMotion, int numValidFrames);

protected:
  vtkReadTrackedSignals();
  virtual ~vtkReadTrackedSignals();

  PlusStatus VerifyInputFrames();
  PlusStatus ComputeTrackerPositionMetric();

  vtkTrackedFrameList* m_TrackerFrames;
  std::string m_ProbeToReferenceTransformName;
  std::string m_CalibrationConfigName;

  std::deque<double> m_SignalValues; 
  std::deque<double> m_SignalTimestamps;
  std::deque<double> m_SignalStylusRef;
  std::deque<double> m_SignalStylusTipRef;
  std::deque<double> m_SignalZ;
  std::deque<double> m_SignalStylusTipSpeed;

  double m_SignalTimeRangeMin;
  double m_SignalTimeRangeMax;

private:
  vtkReadTrackedSignals(const vtkReadTrackedSignals&);
  void operator=(const vtkReadTrackedSignals&);
};

#endif //  __vtkReadTrackedSignals_h 
