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

  /*!
  * Destructor
  */
  ~FreehandCalibrationToolbox();

  /*!
  * Initialize toolbox (load session data) - implementation of a pure virtual function
  */
  void Initialize();

  /*!
  * Read freehand calibration configuration for fCal
  * \param aConfig Root element of the input device set configuration XML data
  */
  PlusStatus ReadConfiguration(vtkXMLDataElement* aConfig);

  /*!
  * Refresh contents (e.g. GUI elements) of toolbox according to the state in the toolbox controller - implementation of a pure virtual function
  */
  void RefreshContent();

  /*!
  * Sets display mode (visibility of actors) according to the current state - implementation of a pure virtual function
  */
  void SetDisplayAccordingToState();

  /*!
  * Overridden method of AbstractToolbox which is called when disconnecting from the device set
  */
  void Reset();

protected:
  /*!
  * Determines if everything is OK to run spatial calibration
  * \return Ready flag
  */
  bool IsReadyToStartSpatialCalibration();

  /*!
  * Execute spatial calibration
  * \return Success flag
  */
  PlusStatus DoSpatialCalibration();

  /*!
  * Prepares and shows the currently segmented points (or hide if unsuccessful)
  * \param aSegmentationResult Segmentation result structure holding the possible segmented points and the candidates
  * \return Success flag
  */
  PlusStatus DisplaySegmentedPoints(PatternRecognitionResult* aSegmentationResult);

protected slots:
  /*!
  * Slot handling open phantom registration button click
  */
  void OpenPhantomRegistration();

  /*!
  * Open XML file containing segmentation parameters and read the contents
  */
  void OpenSegmentationParameters();

  /*!
  * Edit segmentation parameters
  */
  void EditSegmentationParameters();

  /*!
  * Slot handling start temporal calibration button click
  */
  void StartTemporal();

  /*!
  * Slot handling cancel temporal calibration button click
  */
  void CancelTemporal();

  /*!
  * Slot handling start spatial calibration button click
  */
  void StartSpatial();

  /*!
  * Slot handling cancel spatial calibration button click
  */
  void CancelSpatial();

  /*!
  * Slot handling save results button click
  */
  void Save();

  /*!
  * Slot handling show devices combobox state change
  * \param Slot aOn True if toggled, false otherwise
  */
  void ShowDevicesToggled(bool aOn);

protected:
  /*! Calibration algorithm */
  vtkProbeCalibrationAlgo* m_Calibration;

  /*! Flag if cancel is requested */
  bool m_CancelRequest;

  /*! Number of needed calibration images */
  int m_NumberOfCalibrationImagesToAcquire;

  /*! Number of needed validation images */
  int m_NumberOfValidationImagesToAcquire;

protected:
  Ui::FreehandCalibrationToolbox ui;

};

#endif
