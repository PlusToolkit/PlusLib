/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __StylusCalibrationToolbox_h
#define __StylusCalibrationToolbox_h

#include "ui_QStylusCalibrationToolbox.h"

#include "QAbstractToolbox.h"
#include "PlusConfigure.h"

#include <QWidget>
#include <QTime>

class vtkPlusPivotCalibrationAlgo;
class vtkMatrix4x4;

//-----------------------------------------------------------------------------

/*! \class StylusCalibrationToolbox
* \brief Stylus calibration toolbox view class
* \ingroup PlusAppFCal
*/
class QStylusCalibrationToolbox : public QWidget, public QAbstractToolbox
{
  Q_OBJECT

public:
  /*!
  * Constructor
  * \param aParentMainWindow Parent main window
  * \param aFlags Widget flags
  */
  QStylusCalibrationToolbox(fCalMainWindow* aParentMainWindow, Qt::WindowFlags aFlags = 0);

  /*! Destructor */
  ~QStylusCalibrationToolbox();

  /*! \brief Refresh contents (e.g. GUI elements) of toolbox according to the state in the toolbox controller - implementation of a pure virtual function */
  virtual void OnActivated();

  /*!
  * Finalize toolbox
  */
  virtual void OnDeactivated();

  /*!
    Read stylus calibration configuration for fCal
    \param aConfig Root element of the input device set configuration XML data
  */
  PlusStatus ReadConfiguration(vtkXMLDataElement* aConfig);

  /*!
    Refresh contents (e.g. GUI elements) of toolbox according to the state
    in the toolbox controller - implementation of a pure virtual function
  */
  virtual void RefreshContent();

  /*!
    Sets display mode (visibility of actors) according to the current state - implementation of a pure virtual function
  */
  void SetDisplayAccordingToState();

  /*!
  * Return pivot calibration algorithm object
  * \return Pivot calibration algo
  */
  vtkPlusPivotCalibrationAlgo* GetPivotCalibrationAlgo();

protected slots:
  void StopCalibration();
  void OnStartStopClicked();

  /*!
  * Slot handling change of stylus calibration number of points
  * \param aLevel New number of points
  */
  void NumberOfStylusCalibrationPointsChanged(int aNumberOfPoints);

  /*!
  * Acquire stylus position and add it to the algorithm (called by the acquisition timer in object visualizer)
  */
  void OnDataAcquired();

protected:
  /*! Start calibration */
  void StartCalibration();
  void SetFreeHandStartupDelaySec(int freeHandStartupDelaySec);

  vtkSmartPointer<vtkPlusPivotCalibrationAlgo>  m_PivotCalibration;
  int                                           m_NumberOfPoints;
  int                                           m_FreeHandStartupDelaySec;
  int                                           m_CurrentPointNumber;
  QString                                       m_StylusPositionString;
  vtkSmartPointer<vtkMatrix4x4>                 m_PreviousStylusToReferenceTransformMatrix;
  QTime                                         m_CalibrationStartupDelayStartTime;

protected:
  Ui::StylusCalibrationToolbox ui;

};

#endif