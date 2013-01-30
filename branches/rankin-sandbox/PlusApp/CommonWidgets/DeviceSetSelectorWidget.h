/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef DEVICESETSELECTORWIDGET_H
#define DEVICESETSELECTORWIDGET_H

#include "ui_DeviceSetSelectorWidget.h"

#include "PlusConfigure.h"

#include <QWidget>
#include <QString>

//-----------------------------------------------------------------------------

/*! \class DeviceSetSelectorWidget 
* \brief Data selector widget class
* \ingroup PlusAppCommonWidgets
*/
class DeviceSetSelectorWidget : public QWidget
{
  Q_OBJECT

public:
  /*!
  * Constructor
  * \param aParent parent
  * \param aFlags widget flag
  */
  DeviceSetSelectorWidget(QWidget* aParent = 0);

  /*!
  * Destructor
  */
  ~DeviceSetSelectorWidget();

  /*!
  * Set device set configuration directory
  * \param aDirectory Input configuration directory
  * \param Success flag
  */
  PlusStatus SetConfigurationDirectory(QString aDirectory);

  /*!
  * Set connection successful flag
  * \param aConnectionSuccessful If true, Connect button will be disabled until changing another device set
  */
  void SetConnectionSuccessful(bool aConnectionSuccessful);

  /*!
  * Get connection successful flag (if true then it is connected so on InvokeConnect, the application should disconnect)
  * \return Connection successful flag
  */
  bool GetConnectionSuccessful();

  /*!
  * Return currently selected device set description 
  */
  std::string GetSelectedDeviceSetDescription();

  /*!
  * Hide/show the reset tracker button
  */
  void ShowResetTrackerButton(bool aValue);

signals:
  /*!
  * Emmitted when configuration directory is changed (notifies application)
  * \param Configuration directory path
  */
  void ConfigurationDirectoryChanged(std::string);

  /*!
  * Emmitted when device set selected (notifies application)
  * \param Configuration file path
  */
  void DeviceSetSelected(std::string);

  /*!
  * Emitted when connecting to devices
  * \param Device set configuration file
  */
  void ConnectToDevicesByConfigFileInvoked(std::string);

  /*!
  * Called when the reset tracker button is pushed
  * Emits a signal to tell the tracker to reset itself
  */
  void ResetTracker();

protected:
  /*!
  * Fills the combo box with the valid device set configuration files found in input directory
  * \param aDirectory The directory to search in
  * \param Success flag
  */
  PlusStatus ParseDirectory(QString aDirectory);

  protected slots:
    /*!
    * Pops up open directory dialog and saves the selected one into application
    */
    void OpenConfigurationDirectory();

    /*!
    * Called when device set selection has been changed
    */
    void DeviceSetSelected(int);

    /*!
    * Called when Connect button is pushed - connects to devices
    */
    void InvokeConnect();

    /*!
    * Called when Disconnect button is pushed - disconnects from devices
    */
    void InvokeDisconnect();

    /*!
    * Called when Refresh folder button is clicked - refreshes device set list
    */
    void RefreshFolder();

    /*!
    * Called when Edit configuration button is clicked - opens currently selected configuration in editor application
    */
    void EditConfiguration();

    /*
    * Reset tracker button has been pushed
    */
    void ResetTrackerButtonClicked();

protected:
  /*! Configuration directory path */
  QString	m_ConfigurationDirectory;

  /*! Flag telling whether connection has been successful */
  bool	  m_ConnectionSuccessful;

protected:
  Ui::DeviceSetSelectorWidget ui;
};

#endif 
