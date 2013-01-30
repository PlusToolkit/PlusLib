/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef RECORDPHANTOMPOINTSTOOLBOX_H
#define RECORDPHANTOMPOINTSTOOLBOX_H

#include "ui_PhantomRegistrationToolbox.h"

#include "AbstractToolbox.h"
#include "PlusConfigure.h"

#include <QWidget>

class vtkPhantomRegistrationAlgo;
class vtkActor;
class vtkPolyData;
class vtkRenderer;

//-----------------------------------------------------------------------------

/*! \class PhantomRegistrationToolbox 
 * \brief Phantom registration toolbox class
 * \ingroup PlusAppFCal
 */
class PhantomRegistrationToolbox : public QWidget, public AbstractToolbox
{
  Q_OBJECT

public:
  /*!
  * Constructor
  * \param aParentMainWindow Parent main window
  * \param aFlags widget flag
  */
  PhantomRegistrationToolbox(fCalMainWindow* aParentMainWindow, Qt::WFlags aFlags = 0);

  /*! Destructor */
  ~PhantomRegistrationToolbox();

  /*! \brief Refresh contents (e.g. GUI elements) of toolbox according to the state in the toolbox controller - implementation of a pure virtual function */
  void OnActivated();

  /*!
  * Read stylus calibration configuration for fCal
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
  * Return phantom registration algorithm object
  * \return Phantom registration algo
  */
  vtkPhantomRegistrationAlgo* GetPhantomRegistrationAlgo() { return m_PhantomRegistration; };

protected:
  /*!
  * Initialize 3D visualization
  * \return Success flag
  */
  PlusStatus LoadPhantomModel();

  /*!
  * Put state into in progress if all prerequisites are done
  * \return Success flag
  */
  PlusStatus Start();

  /*! Get message telling the state of the calibration */
  QString GetCalibrationStateMessage();

protected slots:
  /*!
  * Slot handling open stylus calibration button click
  */
  void OpenStylusCalibration();

  /*!
  * Slot handling record button click
  */
  void RecordPoint();

  /*!
  * Slot handling undo button click
  */
  void Undo();

  /*!
  * Slot handling reset button click (and also is an overridden method of AbstractToolbox which is called when disconnecting from the device set)
  */
  void Reset();

protected:
  /*! Phantom registration algorithm */
  vtkPhantomRegistrationAlgo* m_PhantomRegistration;

  /*! Renderer for the canvas */
  vtkRenderer*                m_PhantomRenderer;

  /*! Actor for displaying the phantom geometry in phantom canvas */
  vtkActor*                    m_PhantomActor;

  /*! Actor for displaying the defined landmark from the configuration file */
  vtkActor*                    m_RequestedLandmarkActor;

  /*! Polydata holding the requested landmark for highlighting in phantom canvas */
  vtkPolyData*                m_RequestedLandmarkPolyData;

  /*! Index of current landmark */
  int                          m_CurrentLandmarkIndex;

protected:
  Ui::PhantomRegistrationToolbox ui;

};

#endif
