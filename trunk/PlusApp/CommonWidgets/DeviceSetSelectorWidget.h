#ifndef DEVICESETSELECTORWIDGET_H
#define DEVICESETSELECTORWIDGET_H

#include "ui_DeviceSetSelectorWidget.h"

#include "PlusConfigure.h"

#include <QWidget>
#include <QString>

//-----------------------------------------------------------------------------

/*!
* \brief Data selector widget class
*/
class DeviceSetSelectorWidget : public QWidget
{
	Q_OBJECT

public:
	/*!
	* \brief Constructor
	* \param aParent parent
	* \param aFlags widget flag
	*/
	DeviceSetSelectorWidget(QWidget* aParent = 0);

	/*!
	* \brief Destructor
	*/
	~DeviceSetSelectorWidget();

	/*!
	* \brief Set configuration directory to search in
	* \param aDirectory Input configuration directory
	* \param aForce If true, it is set even when it is not empty (false if omitted)
	*/
	void SetConfigurationDirectory(std::string aDirectory, bool aForce = false);

	/*!
	* \brief Set connection successful flag
	* \param aConnectionSuccessful If true, Connect button will be disabled until changing another device set
	*/
	void SetConnectionSuccessful(bool aConnectionSuccessful);

	/*!
	* \brief Get connection successful flag (if true then it is connected so on InvokeConnect, the application should disconnect)
	* \return Connection successful flag
	*/
	bool GetConnectionSuccessful();

  /*!
	* \brief Set device set combo box minimum width 
	*/
  void SetComboBoxMinWidth(int minWidth);

  /*!
	* \brief Return currently selected device set description 
	*/
  std::string GetSelectedDeviceSetDescription();

  /*!
	* \brief Set editor application executable
  * \param Path and filename of the editor application executable to be used
	*/
  void SetEditorApplicationExecutable(QString aExecutable);

  /*!
	* \brief Get editor application executable
  * \return Path and filename of the editor application executable to be used
	*/
  QString GetEditorApplicationExecutable() { return m_EditorApplicationExecutable; };

  /*!
	* \brief Set last selected device set config file
  * \param Path and filename of the device set config file that was used last time
	*/
  void SetLastDeviceSetConfigFile(QString aFile) { m_LastDeviceSetConfigFile = aFile; };

  /*!
	* \brief Get last selected device set config file
  * \return Path and filename of the device set config file that was used last time
	*/
  QString GetLastDeviceSetConfigFile() { return m_LastDeviceSetConfigFile; };

signals:
	/*!
	* \brief Emmitted when configuration directory is changed (notifies application)
	* \param Configuration directory path
	*/
	void ConfigurationDirectoryChanged(std::string);

  /*!
	* \brief Emmitted when device set selected (notifies application)
	* \param Configuration file path
	*/
	void DeviceSetSelected(std::string);

	/*!
	* \brief Emitted when connecting to devices
	* \param Device set configuration file
	*/
	void ConnectToDevicesByConfigFileInvoked(std::string);

protected:
	/*!
	* \brief Fills the combo box with the valid device set configuration files found in input directory
	* \param aDirectory The directory to search in
	*/
	PlusStatus ParseDirectory(QString aDirectory);

protected slots:
	/*!
	* \brief Pops up open directory dialog and saves the selected one into application
	*/
	void OpenConfigurationDirectory();

	/*!
	* \brief Called when device set selection has been changed
	*/
	void DeviceSetSelected(int);

	/*!
	* \brief Called when Connect button is pushed - connects to devices
	*/
	void InvokeConnect();

	/*!
	* \brief Called when Disconnect button is pushed - disconnects from devices
	*/
	void InvokeDisconnect();

	/*!
	* \brief Called when Refresh folder button is clicked - refreshes device set list
	*/
  void RefreshFolder();

	/*!
	* \brief Called when Edit configuration button is clicked - opens currently selected configuration in editor application
	*/
  void EditConfiguration();

protected:
	//! Configuration directory path
	QString	m_ConfigurationDirectory;

	//! Flag telling whether connection has been successful
	bool	  m_ConnectionSuccessful;

  //! Path and filename of the editor application executable to be used
  QString m_EditorApplicationExecutable;

  //! Path and filename of the device set config file that was used last time
  QString m_LastDeviceSetConfigFile;

protected:
	Ui::DeviceSetSelectorWidget ui;
};

#endif 
