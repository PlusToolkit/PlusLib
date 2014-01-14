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

class vtkPhantomLandmarkRegistrationAlgo;
class vtkPhantomLinearObjectRegistrationAlgo;
class vtkActor;
class vtkPolyData;
class vtkRenderer;

enum TabIndex
{
  TabIndex_Landmark,
  TabIndex_LinearObject
};

enum LinearObjectRegistrationState
{
  LinearObjectRegistrationState_Incomplete,
  LinearObjectRegistrationState_InProgress,
  LinearObjectRegistrationState_Complete
};

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
  virtual void OnActivated();

  /*!
  * Finalize toolbox
  */
  virtual void OnDeactivated();

  /*!
  * Read stylus calibration configuration for fCal
  * \param aConfig Root element of the input device set configuration XML data
  */
  PlusStatus ReadConfiguration(vtkXMLDataElement* aConfig);

  /*!
  * Refresh contents (e.g. GUI elements) of toolbox according to the state in the toolbox controller - implementation of a pure virtual function
  */
  virtual void RefreshContent();

  /*!
  * Sets display mode (visibility of actors) according to the current state - implementation of a pure virtual function
  */
  void SetDisplayAccordingToState();

  /*!
  * Return phantom landmark registration algorithm object
  * \return Phantom registration algo
  */
  vtkPhantomLandmarkRegistrationAlgo* GetPhantomLandmarkRegistrationAlgo();;

  /*!
  * Return phantom linear object registration algorithm object
  * \return Phantom registration algo
  */
  vtkPhantomLinearObjectRegistrationAlgo* GetPhantomLinearObjectRegistrationAlgo();;

  /*!
  * Sets the state of 
  */
  void SetLinearObjectRegistrationState(LinearObjectRegistrationState state);

  /*!
  * Return state of the linear object registration
  */
  LinearObjectRegistrationState GetLinearObjectRegistrationState();;

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
  * Slot handling record button for the landmark registration click
  */
  void RecordPoint();

  /*!
  * Slot handling undo button for the landmark registration click
  */
  void Undo();

  /*!
  * Slot handling reset button for the landmark registration click (and also is an overridden method of AbstractToolbox which is called when disconnecting from the device set)
  */
  void Reset();

  /*!
  Slot handling start button for the linear object registration click
  */
  void StartLinearObjectRegistration();
  
  /*!
  Slot handling stop button for the linear object registration click
  */
  void StopLinearObjectRegistration();
  
  /*!
  Slot handling reset button for the linear object registration click
  */
  void ResetLinearObjectRegistration();
  
  /*!
  Slot handling the continuous point acquisition during the linear object registration
  */
  void AddStylusTipPositionToLinearObjectRegistration();

protected:
  /*! Phantom landmark registration algorithm */
  vtkPhantomLandmarkRegistrationAlgo*     m_PhantomLandmarkRegistration;

  /*! Phantom linear object registration algorithm */
  vtkPhantomLinearObjectRegistrationAlgo* m_PhantomLinearObjectRegistration;

  /*! Renderer for the canvas */
  vtkRenderer*                            m_PhantomRenderer;

  /*! Actor for displaying the phantom geometry in phantom canvas */
  vtkActor*                               m_PhantomActor;

  /*! Actor for displaying the defined landmark from the configuration file */
  vtkActor*                               m_RequestedLandmarkActor;

  /*! Polydata holding the requested landmark for highlighting in phantom canvas */
  vtkPolyData*                            m_RequestedLandmarkPolyData;

  /*! Index of current landmark */
  int                                     m_CurrentLandmarkIndex;

  /*! State of the collection of linear objects */
  LinearObjectRegistrationState           m_LinearObjectRegistrationState;

  /*! Number of points acquired for linear object registration so far */
  int                                     m_CurrentPointNumber;

 /*! Stylus or stylus tip position (depending on the state) as string */
  QString                                 m_StylusPositionString;

  /*! Previous stylus tip to reference transform matrix to determine the difference at each point acquisition */
  vtkMatrix4x4*                           m_PreviousStylusTipToReferenceTransformMatrix;

protected:
  Ui::PhantomRegistrationToolbox ui;

};

#endif
