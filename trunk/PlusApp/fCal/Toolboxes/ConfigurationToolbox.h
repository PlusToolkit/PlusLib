/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef CONFIGURATIONTOOLBOX_H
#define CONFIGURATIONTOOLBOX_H

#include "ui_ConfigurationToolbox.h"

#include "AbstractToolbox.h"
#include "PlusConfigure.h"

#include <QWidget>

class DeviceSetSelectorWidget;
class ToolStateDisplayWidget;

//-----------------------------------------------------------------------------

/*! \class ConfigurationToolbox 
* \brief Configuration toolbox that handles device sets and common preferences of fCal
* \ingroup PlusAppFCal
*/
class ConfigurationToolbox : public QWidget, public AbstractToolbox
{
  Q_OBJECT

public:
  /*!
  * Constructor
  * \param aParentWindow Parent main window
  * \param aFlags widget flag
  */
  ConfigurationToolbox(fCalMainWindow* aParentMainWindow, Qt::WFlags aFlags = 0);

  /*!
  * Destructor
  */
  ~ConfigurationToolbox();

  /*!
  * Initialize toolbox
  */
  void Initialize();

  /*!
  * \brief Enable/Disable the toolbox
  */
  virtual void Enable(bool aEnable);

  /*!
  * Refresh contents (e.g. GUI elements) of toolbox according to the state in the toolbox controller - implementation of a pure virtual function
  */
  void RefreshContent();

  /*!
  * Refresh contents if tool display is detached
  */
  void RefreshToolDisplayIfDetached();

  /*!
  * Sets display mode (visibility of actors) according to the current state - implementation of a pure virtual function
  */
  void SetDisplayAccordingToState();

protected:
  /*!
  * Read fCal configuration
  * \param aConfig Root element of the configuration XML data element
  */
  PlusStatus ReadConfiguration(vtkXMLDataElement* aConfig);

  /*! Read wire pattern and add it to visualization */
  PlusStatus ReadAndAddPhantomWiresToVisualization();

  /*!
  * \brief Filters events if this object has been installed as an event filter for the watched object
  * \param obj object
  * \param ev event
  * \return if you want to filter the event out, i.e. stop it being handled further, return true; otherwise return false
  */
  bool eventFilter(QObject *obj, QEvent *ev);

signals:
  /*!
  * Executes operations needed after stopping the process
  * \param Enable/disable flag
  */
  void SetTabsEnabled(bool);

  protected slots:
    /*!
    * Connect to devices described in the argument configuration file in response by clicking on the Connect button
    * \param aConfigFile DeviceSet configuration file path and name
    */
    void ConnectToDevicesByConfigFile(std::string aConfigFile);

    /*!
    * Slot handling pop out toggle button state change
    * \param aOn True if toggled, false otherwise
    */
    void PopOutToggled(bool aOn);

    /*!
    * Slot handling log level combobox item change
    * \param aLevel New log level
    */
    void LogLevelChanged(int aLevel);

    /*!
    * Slot handling select editor application executable button click
    */
    void SelectEditorApplicationExecutable();

    /*!
    * Pops up open directory dialog and saves the selected one as image directory. It is the directory that contains the usually used input images (to act as home for relative paths in device set configuration files)
    */
    void SelectImageDirectory();

protected:
  /*! Device set selector widget */
  DeviceSetSelectorWidget*  m_DeviceSetSelectorWidget;

  /*! Tool state display widget */
  ToolStateDisplayWidget*   m_ToolStateDisplayWidget;

  /*! Window that is created when tool state display widget is popped out */
  QWidget*                  m_ToolStatePopOutWindow;

  /*! Flag indicating if tool state display widget is detached */
  bool                      m_IsToolDisplayDetached;

  /*! String to hold the last location of data saved */
  QString                   m_LastEditorLocation;

  /*! String to hold the last location of data saved */
  QString                   m_LastImageDirectoryLocation;

protected:
  Ui::ConfigurationToolbox  ui;
};

#endif
