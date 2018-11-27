/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#ifndef DEVICESETSELECTORWIDGET_H
#define DEVICESETSELECTORWIDGET_H

// Local includes
#include "PlusConfigure.h"
#include "PlusWidgetsExport.h"
#include "ui_QPlusDeviceSetSelectorWidget.h"
#include "vtkIGSIOTransformRepository.h"

// Qt includes
#include <QString>
#include <QWidget>

class QAction;
class QDomDocument;
class QDragEnterEvent;
class QDropEvent;
class QMenu;

// VTK includes
#include <vtkSmartPointer.h>

//-----------------------------------------------------------------------------

/*! \class DeviceSetSelectorWidget
* \brief Data selector widget class
* \ingroup PlusAppCommonWidgets
*/
class PlusWidgetsExport QPlusDeviceSetSelectorWidget : public QWidget
{
  Q_OBJECT

public:
  /*!
  * Constructor
  * \param aParent parent
  * \param aFlags widget flag
  */
  QPlusDeviceSetSelectorWidget(QWidget* aParent = 0);

  /*!
  * Set the device set combo box maximum size ratio
  * \param ratio the ratio to set
  */
  void SetDeviceSetComboBoxMaximumSizeRatio(double ratio);

  /*!
  * Destructor
  */
  ~QPlusDeviceSetSelectorWidget();

  /*!
  * Set device set configuration directory
  * \param aDirectory Input configuration directory
  * \return Success flag
  */
  PlusStatus SetConfigurationDirectory(const QString& aDirectory);

  /*!
  * Set device set configuration file, if it contains a folder structure, the directory is changed and the file is selected
  * \param aFilename Input configuration filename
  * \return Success flag
  */
  PlusStatus SetConfigurationFile(const QString& aFilename);

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

  /*!
  * Provide access to the Connect push button text
  */
  void SetConnectButtonText(QString text);

  /*!
  * Allow settings of an optional suffix in the description box separated by a blank line
  */
  void SetDescriptionSuffix(const QString& string);

  /*!
  * Clear optional suffix in the description box separated by a blank line
  */
  void ClearDescriptionSuffix();

  /*!
  * Fill the description text box with prefix + body + suffix
  * Newline separated if prefix or suffix is not empty
  */
  void UpdateDescriptionText();

signals:
  /*!
  * Emitted when configuration directory is changed (notifies application)
  * \param Configuration directory path
  */
  void ConfigurationDirectoryChanged(std::string);

  /*!
  * Emitted when device set selected (notifies application)
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
  PlusStatus ParseDirectory(const QString& aDirectory);

  virtual void resizeEvent(QResizeEvent* event);

  QString FindCalibrationDetails(const QDomDocument& doc,
                                 vtkSmartPointer<vtkIGSIOTransformRepository> tr,
                                 const QString& tagName,
                                 const QString& outputPrefix,
                                 const QString& firstFrame,
                                 const QString& secondFrame);

  void FixComboBoxDropDownListSizeAdjustemnt(QComboBox* cb);

  virtual void dragEnterEvent(QDragEnterEvent* event);
  virtual void dropEvent(QDropEvent* event);

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
  * Called when Edit configuration button is left-clicked - opens currently selected configuration in editor application
  */
  void EditConfiguration();

  /*!
  * Called when the context menu is requested on the edit button
  */
  void ShowEditContextMenu(QPoint);

  /*!
  * Called when the select editor menu item is called
  */
  void SelectEditor();

  /*!
  * Called when the edit application config file is called
  */
  void EditAppConfig();

  /*
  * Reset tracker button has been pushed
  */
  void ResetTrackerButtonClicked();

protected:
  /*! Configuration directory path */
  QString       m_ConfigurationDirectory;

  /*! Action triggered when button right-clicked */
  QAction*      m_EditorSelectAction;
  QAction*      m_EditApplicationConfigFileAction;
  QMenu*        m_EditMenu;

  /*! Flag telling whether connection has been successful */
  bool          m_ConnectionSuccessful;

  /*! If not -1, define the maximum size of the device set combobox relative to the current screen size */
  double        m_DeviceSetComboBoxMaximumSizeRatio;

  /*! Prefix to the description in the main text box */
  QString       m_DescriptionPrefix;

  /*! Description in the main text box */
  QString       m_DescriptionBody;

  /*! Suffix to the description in the main text box */
  QString       m_DescriptionSuffix;

protected:
  Ui::DeviceSetSelectorWidget ui;
};

#endif
