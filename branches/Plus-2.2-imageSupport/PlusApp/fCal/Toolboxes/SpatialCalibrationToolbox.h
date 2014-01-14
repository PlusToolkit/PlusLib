/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef SPATIALCALIBRATIONTOOLBOX_H
#define SPATIALCALIBRATIONTOOLBOX_H

#include "ui_SpatialCalibrationToolbox.h"

#include "AbstractToolbox.h"
#include "PlusConfigure.h"

#include "FidPatternRecognitionCommon.h"

#include <QWidget>

class vtkProbeCalibrationAlgo;
class FidPatternRecognition;
class vtkTrackedFrameList;

//-----------------------------------------------------------------------------

/*! \class SpatialCalibrationToolbox 
* \brief Spatial calibration toolbox class
* \ingroup PlusAppFCal
*/
class SpatialCalibrationToolbox : public QWidget, public AbstractToolbox
{
  Q_OBJECT

public:
  /*!
  * Constructor
  * \param aParentMainWindow Parent main window
  * \param aFlags widget flag
  */
  SpatialCalibrationToolbox(fCalMainWindow* aParentMainWindow, Qt::WFlags aFlags = 0);

  /*! Destructor */
  ~SpatialCalibrationToolbox();

  /*! \brief Refresh contents (e.g. GUI elements) of toolbox according to the state in the toolbox controller - implementation of a pure virtual function */
  virtual void OnActivated();

  /*!
  * Finalize toolbox
  */
  virtual void OnDeactivated();

  /*! \brief Reset toolbox to initial state - */
  virtual void Reset();

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
  * Determines if everything is OK to run spatial calibration
  * \return Ready flag
  */
  bool IsReadyToStartSpatialCalibration();

  /*! Prepares and shows the last segmented points from the current acquisition */
  void DisplaySegmentedPoints();

  /*! Set and save calibration results */
  PlusStatus SetAndSaveResults();

protected slots:
  /*! Acquire tracked frames and segment them. Runs calibration if acquisition is ready */
  void DoCalibration();

  /*! Slot handling open phantom registration button click */
  void OpenPhantomRegistration();

  /*! Open XML file containing segmentation parameters and read the contents */
  void OpenSegmentationParameters();

  /*! Edit segmentation parameters */
  void EditSegmentationParameters();

  /*! Slot handling start spatial calibration button click */
  void StartCalibration();

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

  /*! Time needed to process one frame in the latest recording round (in milliseconds) */
  int m_LastProcessingTimePerFrameMs;

protected:
  Ui::SpatialCalibrationToolbox ui;

};

#endif
