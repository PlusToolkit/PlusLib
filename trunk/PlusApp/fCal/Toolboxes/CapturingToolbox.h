/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef CAPTURINGTOOLBOX_H
#define CAPTURINGTOOLBOX_H

#include "AbstractToolbox.h"
#include "PlusConfigure.h"
#include "ui_CapturingToolbox.h"
#include "vtkTimestampedCircularBuffer.h"
#include <QString>
#include <QWidget>
#include <deque>

class CaptureControlWidget;
class QTimer;
class vtkTrackedFrameList;

//-----------------------------------------------------------------------------

/*! \class CapturingToolbox 
* \brief Tracked frame capturing class
* \ingroup PlusAppFCal
*/
class CapturingToolbox : public QWidget, public AbstractToolbox
{
  Q_OBJECT

public:
  /*!
  * Constructor
  * \param aParentMainWindow Parent main window
  * \param aFlags widget flag
  */
  CapturingToolbox(fCalMainWindow* aParentMainWindow, Qt::WFlags aFlags = 0);

  /*!
  * Destructor
  */
  ~CapturingToolbox();

  /*! \brief Refresh contents (e.g. GUI elements) of toolbox according to the state in the toolbox controller - implementation of a pure virtual function */
  void OnActivated();

  /*! Refresh contents (e.g. GUI elements) of toolbox according to the state in the toolbox controller - implementation of a pure virtual function */
  void RefreshContent();

  /*! \brief Reset toolbox to initial state - */
  virtual void Reset();

  /*! Sets display mode (visibility of actors) according to the current state - implementation of a pure virtual function */
  void SetDisplayAccordingToState();

  /*! Get recorded tracked frame list */
  vtkTrackedFrameList* GetRecordedFrames() { return m_RecordedFrames; };

protected:
  /*!
  * Saves recorded tracked frame list to file
  * \param aOutput Output file
  * \return Success flag
  */
  PlusStatus SaveToMetafile(std::string aOutput);

  /*!
  * Get the maximum frame rate from the video source. If there is none then the tracker
  * \return Maximum frame rate
  */
  double GetMaximumFrameRate();

  /*!
  * Actual clearing of frames
  */
  void ClearRecordedFramesInternal();
  /*!
  * Save data to file
  */
  void WriteToFile(QString& aFilename);

  /*! Get the sampling period length (in seconds). Frames are copied from the devices to the data collection buffer once in every sampling period. */
  double GetSamplingPeriodSec();
  
protected slots:
  /*!
  * Take snapshot (record the current frame only)
  */
  void TakeSnapshot();

  /*!
  * Slot handling record button click
  */
  void Record();

  /*!
  * Slot handling stop button click (Record button becomes Stop after clicking)
  */
  void Stop();

  /*!
  * Slot handling clear recorded frames button click
  */
  void ClearRecordedFrames();

  /*!
  * Slot handling Save button click
  */
  void Save();

  /*!
  * Slot handling Save As button click
  */
  void SaveAs();

  /*!
  * Slot handling value change of sampling rate slider
  * \param aValue Tick index (rightmost means record every frame, and each one to the left halves it)
  */
  void SamplingRateChanged(int aValue);

  /*!
  * Record tracked frames (the recording timer calls it)
  */
  void Capture();

  /*!
  * Handle status message from any sub capture widgets
  */
  void HandleStatusMessage(const std::string& aMessage);

protected:
  /*! Recorded tracked frame list */
  vtkTrackedFrameList* m_RecordedFrames;

  /*! Timer triggering the */
  QTimer* m_RecordingTimer;

  /*! Timestamp of last recorded frame (only frames that have more recent timestamp will be added) */
  double m_RecordingLastAlreadyRecordedFrameTimestamp;

  /*! Desired timestamp of the next frame to be recorded */
  double m_RecordingNextFrameToBeRecordedTimestamp;

  /*! Frame rate of the sampling */
  const int m_SamplingFrameRate;

  /*! Requested frame rate (frames per second) */
  double m_RequestedFrameRate;

  /*! Actual frame rate (frames per second) */
  double m_ActualFrameRate;
  
  /*!
    Frame index of the first frame that is recorded in this segment (since pressed the record button).
    It is used when estimating the actual frame rate: frames that are acquired before this frame index (i.e.,
    those that were acquired in a different recording segment) will not be taken into account in the actual
    frame rate computation.
  */
  int m_RecordingFirstFrameIndexInThisSegment;

  /*! String to hold the last location of data saved */
  QString m_LastSaveLocation;

  /* Container holding capture widgets */
  std::vector<CaptureControlWidget*>  m_CaptureWidgets;

protected:
  Ui::CapturingToolbox ui;
};

#endif
