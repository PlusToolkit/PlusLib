#ifndef DEVICESETSELECTORWIDGET_H
#define DEVICESETSELECTORWIDGET_H

#include "ui_DeviceSetSelectorWidget.h"

#include "PlusConfigure.h"

#include <QWidget>

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

	//TODO
	void SetConfigurationDirectory(std::string aDirectory);

signals:
	//TODO
	void ConfigurationDirectoryChanged(std::string);
	void ConnectToDevicesByConfigFileInvoked(std::string);

protected:
	//TODO
	PlusStatus ParseDirectory(QString aDirectory);

protected slots:
	//TODO
	void OpenConfigurationDirectoryClicked();
	void DeviceSetSelected(int);
	void InvokeConnect();

protected:
	QString	m_ConfigurationDirectory;

protected:
	Ui::DeviceSetSelectorWidget ui;
};

#endif 