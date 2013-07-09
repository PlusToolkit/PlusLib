/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef TEMPORALCALIBRATIONTOOLBOX_H
#define TEMPORALCALIBRATIONTOOLBOX_H

#include "AbstractToolbox.h"
#include "PlusConfigure.h"
#include "ui_TemporalCalibrationToolbox.h"
#include "vtkTemporalCalibrationAlgo.h"
#include <QWidget>
class vtkContextView;
class vtkPlusChannel;
class vtkTable;
class vtkTrackedFrameList;

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

  /*! \brief Refresh contents (e.g. GUI elements) of toolbox according to the state in the toolbox controller - implementation of a pure virtual function */
  virtual void OnActivated();

  /*!
  * Finalize toolbox
  */
  virtual void OnDeactivated();

  /*!
  * Read freehand calibration configuration for fCal
  * \param aConfig Root element of the input device set configuration XML data
  */
  PlusStatus ReadConfiguration(vtkXMLDataElement* aConfig);

  /*! Refresh contents (e.g. GUI elements) of toolbox according to the state in the toolbox controller - implementation of a pure virtual function */
  virtual void RefreshContent();

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
  
  /*! Compute calibration results from the collected data and display the results */
  void ComputeCalibrationResults();

  /*! A signal combo box was changed */
  void FixedSignalChanged(int newIndex);

  void MovingSignalChanged(int newIndex);

  void FixedSourceChanged(int newIndex);

  void MovingSourceChanged(int newIndex);

protected:
  /*! Tracked frame for tracking data for temporal calibration */
  vtkTrackedFrameList* TemporalCalibrationFixedData;

  /*! Tracked frame for video data for temporal calibration */
  vtkTrackedFrameList* TemporalCalibrationMovingData;

  /*! Flag if cancel is requested */
  bool CancelRequest;

  /*! Duration of the temporal calibration process in seconds */
  int TemporalCalibrationDurationSec;

  /*! Timestamp of last recorded video item (items acquired since this timestamp will be recorded) */
  double LastRecordedFixedItemTimestamp;

  /*! Timestamp of last recorded tracker item (items acquired since this timestamp will be recorded) */
  double LastRecordedMovingItemTimestamp;

  /*! Time interval between recording (sampling) cycles (in milliseconds) */
  int RecordingIntervalMs;

  /*! Time of starting temporal calibration */
  double StartTimeSec;

  /*! Saved tracker offset in case the temporal calibration is canceled or unsuccessful */
  double PreviousFixedOffset;

  /*! Saved video offset in case the temporal calibration is canceled or unsuccessful */
  double PreviousMovingOffset;

  /*! Metric table of video positions for temporal calibration */
  vtkTable* FixedPositionMetric;

  /*! Metric table of uncalibrated tracker positions for temporal calibration */
  vtkTable* UncalibratedMovingPositionMetric;

  /*! Metric table of calibrated tracker positions for temporal calibration */
  vtkTable* CalibratedMovingPositionMetric;

	/*! Window that is created/deleted when Show Plots button is toggled */
	QWidget* TemporalCalibrationPlotsWindow;

  /*! Chart view for the uncalibrated plot */
  vtkContextView* UncalibratedPlotContextView;

  /*! Chart view for the calibrated plot */
  vtkContextView* CalibratedPlotContextView;

  vtkPlusChannel* FixedChannel;
  vtkTemporalCalibrationAlgo::FRAME_TYPE FixedType;
  vtkPlusChannel* MovingChannel;
  vtkTemporalCalibrationAlgo::FRAME_TYPE MovingType;

  PlusTransformName FixedValidationTransformName;
  PlusTransformName MovingValidationTransformName;

  vtkTemporalCalibrationAlgo* TemporalCalibrationAlgo;

  std::string RequestedFixedChannel;
  std::string RequestedMovingChannel;
  std::string RequestedFixedSource;
  std::string RequestedMovingSource;

protected:
  Ui::TemporalCalibrationToolbox ui;
};

#endif
