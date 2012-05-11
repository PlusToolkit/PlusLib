/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef FREEHANDCALIBRATIONTOOLBOX_H
#define FREEHANDCALIBRATIONTOOLBOX_H

#include "ui_FreehandCalibrationToolbox.h"

#include "AbstractToolbox.h"
#include "PlusConfigure.h"

#include "FidPatternRecognitionCommon.h"

#include <QWidget>

class vtkProbeCalibrationAlgo;
class FidPatternRecognition;
class vtkTrackedFrameList;
class vtkTable;
class vtkContextView;

//-----------------------------------------------------------------------------

/*! \class FreehandCalibrationToolbox 
* \brief Freehand calibration toolbox class
* \ingroup PlusAppFCal
*/
class FreehandCalibrationToolbox : public QWidget, public AbstractToolbox
{
  Q_OBJECT

public:
  /*!
  * Constructor
  * \param aParentMainWindow Parent main window
  * \param aFlags widget flag
  */
  FreehandCalibrationToolbox(fCalMainWindow* aParentMainWindow, Qt::WFlags aFlags = 0);

  /*! Destructor */
  ~FreehandCalibrationToolbox();

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
  * Determines if everything is OK to run spatial calibration
  * \return Ready flag
  */
  bool IsReadyToStartSpatialCalibration();

  /*! Prepares and shows the last segmented points from the current acquisition */
  void DisplaySegmentedPoints();

  /*! Set and save calibration results */
  PlusStatus SetAndSaveResults();

  /*!
	* \brief Filters events if this object has been installed as an event filter for the watched object
	* \param obj object
	* \param ev event
	* \return if you want to filter the event out, i.e. stop it being handled further, return true; otherwise return false
	*/
	bool eventFilter(QObject *obj, QEvent *ev);

protected slots:
  /*! Acquire tracked frames and segment them. Runs calibration if acquisition is ready */
  void DoSpatialCalibration();

  /*! Acquire tracked frames for temporal calibration and calls the algorithm when done */
  void DoTemporalCalibration();

  /*! Slot handling open phantom registration button click */
  void OpenPhantomRegistration();

  /*! Open XML file containing segmentation parameters and read the contents */
  void OpenSegmentationParameters();

  /*! Edit segmentation parameters */
  void EditSegmentationParameters();

  /*! Show/hide popup window with the plots in it when toggling the Show Plots button */
  void ShowPlotsToggled(bool aOn);

  /*! Slot handling start temporal calibration button click */
  void StartTemporal();

  /*! Slot handling start spatial calibration button click */
  void StartSpatial();

  /*! Slot handling cancel calibration event (button click or explicit call) */
  void CancelCalibration();

protected:
  /*! Calibration algorithm */
  vtkProbeCalibrationAlgo* m_Calibration;

  /*! Pattern recognition algorithm */
  FidPatternRecognition* m_PatternRecognition;

  /*! Tracked frame data for spatial calibration */
  vtkTrackedFrameList* m_SpatialCalibrationData;

  /*! Tracked frame data for validation of spatial calibration */
  vtkTrackedFrameList* m_SpatialValidationData;

  /*! Tracked frame for tracking data for temporal calibration */
  vtkTrackedFrameList* m_TemporalCalibrationTrackingData;

  /*! Tracked frame for video data for temporal calibration */
  vtkTrackedFrameList* m_TemporalCalibrationVideoData;

  /*! Timestamp of last recorded frame (the tracked frames acquired since this timestamp will be recorded) */
  double m_LastRecordedFrameTimestamp;

  /*! Flag if cancel is requested */
  bool m_CancelRequest;

  /*! Number of needed calibration images */
  int m_NumberOfCalibrationImagesToAcquire;

  /*! Number of needed validation images */
  int m_NumberOfValidationImagesToAcquire;

  /*! Number of segmented calibration images */
  int m_NumberOfSegmentedCalibrationImages;

  /*! Number of segmented validation images */
  int m_NumberOfSegmentedValidationImages;

  /*! Time interval between recording (sampling) cycles (in milliseconds) */
  int m_RecordingIntervalMs;

  /*! Maximum time spent with processing (getting tracked frames, segmentation) per second (in milliseconds) */
  int m_MaxTimeSpentWithProcessingMs;

  /*! Duration of the temporal calibration process in seconds */
  int m_TemporalCalibrationDurationSec;

  /*! Time needed to process one frame in the latest recording round (in milliseconds) */
  int m_LastProcessingTimePerFrameMs;

  /*! Time of starting temporal calibration */
  double m_StartTimeSec;

  /*! Saved tracker offset in case the temporal calibration is cancelled or unsuccessful */
  double m_PreviousTrackerOffset;

  /*! Saved video offset in case the temporal calibration is cancelled or unsuccessful */
  double m_PreviousVideoOffset;

  /*! Flag indicating if Spatial calibration is in progress */
  bool m_SpatialCalibrationInProgress;

  /*! Flag indicating if Temporal calibration is in progress */
  bool m_TemporalCalibrationInProgress;

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
  Ui::FreehandCalibrationToolbox ui;

};

#endif
