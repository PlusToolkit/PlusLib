/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef SINGLEWALLCALIBRATIONTOOLBOX_H
#define SINGLEWALLCALIBRATIONTOOLBOX_H

#include "AbstractToolbox.h"
#include "PlusConfigure.h"
#include "ui_SingleWallCalibrationToolbox.h"
#include <QWidget>
class vtkLineSegmentationAlgo;
class vtkSingleWallCalibrationAlgo;
class vtkTrackedFrameList;

//-----------------------------------------------------------------------------

/*! \class SingleWallCalibrationToolbox 
* \brief Single wall calibration toolbox class
* \ingroup PlusAppFCal
*/
class SingleWallCalibrationToolbox : public QWidget, public AbstractToolbox
{
  Q_OBJECT

public:
  /*!
  * Constructor
  * \param aParentMainWindow Parent main window
  * \param aFlags widget flag
  */
  SingleWallCalibrationToolbox(fCalMainWindow* aParentMainWindow, Qt::WFlags aFlags = 0);

  /*! Destructor */
  ~SingleWallCalibrationToolbox();

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

  /*! Set and save calibration results */
  PlusStatus SetAndSaveResults();

protected slots:
  /*! Acquire tracked frames and segment them. Runs calibration if acquisition is ready */
  void DoCalibration();

  /*! Slot handling start spatial calibration button click */
  void StartCalibration();

  /*! Slot handling cancel calibration event (button click or explicit call) */
  void CancelCalibration();

  void SingleFrame();

protected:
  /*! Calibration algorithm */
  vtkSingleWallCalibrationAlgo* m_Calibration;

  vtkLineSegmentationAlgo* m_Segmentation;

  /*! Tracked frame data for spatial calibration */
  vtkTrackedFrameList* m_SpatialCalibrationData;

  /*! Tracked frame data for validation of spatial calibration */
  vtkTrackedFrameList* m_SpatialValidationData;

  vtkTrackedFrameList* m_RecordingBuffer;
  int m_SingleFrameCount;

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
  Ui::SingleWallCalibrationToolbox ui;

};

#endif
