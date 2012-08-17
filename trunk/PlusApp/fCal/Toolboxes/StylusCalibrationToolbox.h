/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __StylusCalibrationToolbox_h
#define __StylusCalibrationToolbox_h

#include "ui_StylusCalibrationToolbox.h"

#include "AbstractToolbox.h"
#include "PlusConfigure.h"

#include <QWidget>

class vtkPivotCalibrationAlgo;
class vtkMatrix4x4;

//-----------------------------------------------------------------------------

/*! \class StylusCalibrationToolbox 
* \brief Stylus calibration toolbox view class
* \ingroup PlusAppFCal
*/
class StylusCalibrationToolbox : public QWidget, public AbstractToolbox
{
  Q_OBJECT

public:
  /*!
  * Constructor
  * \param aParentMainWindow Parent main window
  * \param aFlags Widget flags
  */
  StylusCalibrationToolbox(fCalMainWindow* aParentMainWindow, Qt::WFlags aFlags = 0);

  /*! Destructor */
  ~StylusCalibrationToolbox();

  /*! \brief Refresh contents (e.g. GUI elements) of toolbox according to the state in the toolbox controller - implementation of a pure virtual function */
  void OnActivated();

  /*!
    Read stylus calibration configuration for fCal
    \param aConfig Root element of the input device set configuration XML data
  */
  PlusStatus ReadConfiguration(vtkXMLDataElement* aConfig);

  /*!
    Refresh contents (e.g. GUI elements) of toolbox according to the state
    in the toolbox controller - implementation of a pure virtual function
  */
  void RefreshContent();

  /*!
    Sets display mode (visibility of actors) according to the current state - implementation of a pure virtual function
  */
  void SetDisplayAccordingToState();

  /*!
  * Return pivot calibration algorithm object
  * \return Pivot calibration algo
  */
  vtkPivotCalibrationAlgo* GetPivotCalibrationAlgo() { return m_PivotCalibration; };

protected slots:
  /*! Start calibration */
  void Start();

  /*! Stop calibration */
  void Stop();

  /*!
  * Slot handling change of stylus calibration number of points
  * \param aLevel New number of points
  */
  void NumberOfStylusCalibrationPointsChanged(int aNumberOfPoints);

  /*!
  * Acquire stylus position and add it to the algorithm (called by the acquisition timer in object visualizer)
  */
  void AddStylusPositionToCalibration();

protected:
  /*! Pivot calibration algorithm */
  vtkPivotCalibrationAlgo*  m_PivotCalibration;

  /*! Number of points to acquire */
  int                        m_NumberOfPoints;

  /*! Number of points acquired so far */
  int                        m_CurrentPointNumber;

  /*! Stylus or stylus tip position (depending on the state) as string */
  QString                    m_StylusPositionString;

  /*! Previous stylus to reference transform matrix to determine the difference at each point acquisition */
  vtkMatrix4x4*             m_PreviousStylusToReferenceTransformMatrix;

protected:
  Ui::StylusCalibrationToolbox ui;

};

#endif
