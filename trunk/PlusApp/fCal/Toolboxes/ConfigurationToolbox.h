#ifndef CONFIGURATIONTOOLBOX_H
#define CONFIGURATIONTOOLBOX_H

#include "ui_ConfigurationToolbox.h"

#include "AbstractToolbox.h"

#include <QWidget>

class DeviceSetSelectorWidget;
class ToolStateDisplayWidget;

//-----------------------------------------------------------------------------

/*!
* \brief Volume reconstruction toolbox view class
*/
class ConfigurationToolbox : public QWidget, public AbstractToolbox
{
	Q_OBJECT

public:
	/*!
	* \brief Constructor
	* \param aParent parent
	* \param aFlags widget flag
	*/
	ConfigurationToolbox(QWidget* aParent = 0, Qt::WFlags aFlags = 0);

	/*!
	* \brief Destructor
	*/
	~ConfigurationToolbox();

	/*!
	* \brief Initialize toolbox
	*/
	void Initialize();

	/*!
	* \brief Refresh contents (e.g. GUI elements) of toolbox according to the state in the toolbox controller - implementation of a pure virtual function
	*/
	void RefreshToolboxContent();

	/*!
	* \brief Executes operations needed after stopping the process - implementation of a pure virtual function
	*/
	void Stop();

	/*!
	* \brief Executes operations needed when changing to another toolbox - implementation of a pure virtual function
	*/
	void Clear();

signals:
	/*!
	* \brief Executes operations needed after stopping the process
	* \param Enable/disable flag
	*/
	void SetTabsEnabled(bool);

protected slots:
	/*!
	* \brief Slot setting the configuration directory according to device set selector
	* \param aDirectory Configuration directory path
	*/
	void SetConfigurationDirectory(std::string aDirectory);
	
	/*!
	* \brief Connect to devices described in the argument configuration file in response by clicking on the Connect button
	* \param aConfigFile DeviceSet configuration file path and name
	*/
	void ConnectToDevicesByConfigFile(std::string aConfigFile);

	/*!
	* \brief Slot handling pop out toggle button state change
	* \param aOn True if toggled, false otherwise
	*/
	void PopOutToggled(bool aOn);

	/*!
	* \brief Slot handling log level combobox item change
	* \param aLevel New log level
	*/
	void LogLevelChanged(int aLevel);

protected:
	/*!
	* \brief Filters events if this object has been installed as an event filter for the watched object
	* \param obj object
	* \param ev event
	* \return if you want to filter the event out, i.e. stop it being handled further, return true; otherwise return false
	*/
	bool eventFilter(QObject *obj, QEvent *ev);

protected:
	//! Device set selector widget
	DeviceSetSelectorWidget*	m_DeviceSetSelectorWidget;

	//! Tool state display widget
	ToolStateDisplayWidget*		m_ToolStateDisplayWidget;

	//! Window that is created when tool state display widget is popped out
	QWidget*					m_ToolStatePopOutWindow;

protected:
	Ui::ConfigurationToolbox	ui;
};

#endif
