/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef TEMPORALCALIBRATIONTOOLBOX_H
#define TEMPORALCALIBRATIONTOOLBOX_H

#include "ui_TemporalCalibrationToolbox.h"

#include "AbstractToolbox.h"
#include "PlusConfigure.h"

#include <QWidget>

class vtkTrackedFrameList;
class vtkTable;
class vtkContextView;

//-----------------------------------------------------------------------------

/*! \class TemporalCalibrationToolbox 
* \brief Temporal calibration toolbox class
* \ingroup PlusAppFCal
*/
class TemporalCalibrationToolbox : public QWidget, public AbstractToolbox
{
  Q_OBJECT

public:
  /*!
  * Constructor
  * \param aParentMainWindow Parent main window
  * \param aFlags widget flag
  */
  TemporalCalibrationToolbox(fCalMainWindow* aParentMainWindow, Qt::WFlags aFlags = 0);

  /*! Destructor */
  ~TemporalCalibrationToolbox();

  /*! Initialize toolbox (load session data) - implementation of a pure virtual function */
  void Initialize();

  /*!
  * Read freehand calibration configuration for fCal
  * \param aConfig Root element of the input device set configuration XML data
  */
  PlusStatus ReadConfiguration(vtkXMLDataElement* aConfig);

  /*! Refresh contents (e.g. GUI elements) of toolbox according to the state in the toolbox controller - implementation of a pure virtual function */
  void RefreshContent();

  /*! Sets display mode (visibility of actors) according to the current state - implementation of a pure virtual function */
  void SetDisplayAccordingToState();

protected:
  /*!
	* \brief Filters events if this object has been installed as an event filter for the watched object
	* \param obj object
	* \param ev event
	* \return if you want to filter the event out, i.e. stop it being handled further, return true; otherwise return false
	*/
	bool eventFilter(QObject *obj, QEvent *ev);

protected slots:
  /*! Acquire tracked frames for temporal calibration and calls the algorithm when done */
  void DoCalibration();

  /*! Show/hide popup window with the plots in it when toggling the Show Plots button */
  void ShowPlotsToggled(bool aOn);

  /*! Slot handling start temporal calibration button click */
  void StartCalibration();

  /*! Slot handling cancel calibration event (button click or explicit call) */
  void CancelCalibration();

protected:
  /*! Tracked frame for tracking data for temporal calibration */
  vtkTrackedFrameList* m_TemporalCalibrationTrackingData;

  /*! Tracked frame for video data for temporal calibration */
  vtkTrackedFrameList* m_TemporalCalibrationVideoData;

  /*! Flag if cancel is requested */
  bool m_CancelRequest;

  /*! Duration of the temporal calibration process in seconds */
  int m_TemporalCalibrationDurationSec;

  /*! Timestamp of last recorded frame (the tracked frames acquired since this timestamp will be recorded) */
  double m_LastRecordedFrameTimestamp;

  /*! Time interval between recording (sampling) cycles (in milliseconds) */
  int m_RecordingIntervalMs;

  /*! Time of starting temporal calibration */
  double m_StartTimeSec;

  /*! Saved tracker offset in case the temporal calibration is canceled or unsuccessful */
  double m_PreviousTrackerOffset;

  /*! Saved video offset in case the temporal calibration is canceled or unsuccessful */
  double m_PreviousVideoOffset;

  /*! Metric table of video positions for temporal calibration */
  vtkTable* m_VideoPositionMetric;

  /*! Metric table of uncalibrated tracker positions for temporal calibration */
  vtkTable* m_UncalibratedTrackerPositionMetric;

  /*! Metric table of calibrated tracker positions for temporal calibration */
  vtkTable* m_CalibratedTrackerPositionMetric;

	/*! Window that is created/deleted when Show Plots button is toggled */
	QWidget* m_TemporalCalibrationPlotsWindow;

  /*! Chart view for the uncalibrated plot */
  vtkContextView* m_UncalibratedPlotContextView;

  /*! Chart view for the calibrated plot */
  vtkContextView* m_CalibratedPlotContextView;

protected:
  Ui::TemporalCalibrationToolbox ui;

};

#endif
