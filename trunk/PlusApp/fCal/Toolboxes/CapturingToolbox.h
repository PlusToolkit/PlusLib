/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef CAPTURINGTOOLBOX_H
#define CAPTURINGTOOLBOX_H

#include "ui_CapturingToolbox.h"

#include "AbstractToolbox.h"
#include "PlusConfigure.h"
#include "vtkTimestampedCircularBuffer.h"

#include <QWidget>

#include <deque>

class vtkTrackedFrameList;
class QTimer;

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

  /*!
  * Initialize toolbox (load session data) - overridden method
  */
  void Initialize();

  /*!
  * Refresh contents (e.g. GUI elements) of toolbox according to the state in the toolbox controller - implementation of a pure virtual function
  */
  void RefreshContent();

  /*!
  * Sets display mode (visibility of actors) according to the current state - implementation of a pure virtual function
  */
  void SetDisplayAccordingToState();

  /*!
  * Get recorded tracked frame list
  */
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
  * Slot handling open save button click
  */
  void Save();

  /*!
  * Slot handling value change of sampling rate slider
  * \param aValue Tick index (rightmost means record every frame, and each one to the left halves it)
  */
  void SamplingRateChanged(int aValue);

  /*!
  * Record tracked frames (the recording timer calls it)
  */
  void Capture();

protected:
  /*! Recorded tracked frame list */
  vtkTrackedFrameList* m_RecordedFrames;

  /*! Timer triggering the */
  QTimer* m_RecordingTimer;

  /*! Timestamp of last recorded frame (the tracked frames acquired since this timestamp will be recorded) */
  double m_LastRecordedFrameTimestamp;

  /*! Frame rate of the sampling */
  const int m_SamplingFrameRate;

  /*! Requested frame rate (frames per second) */
  double m_RequestedFrameRate;

  /*! Actual frame rate (frames per second) */
  double m_ActualFrameRate;

  /*! Queue storing the number of recorded frames in each round in the last two seconds */
  std::deque<int> m_RecordedFrameNumberQueue;

protected:
  Ui::CapturingToolbox ui;
};

#endif
