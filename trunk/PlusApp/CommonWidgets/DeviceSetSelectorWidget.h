#ifndef DEVICESETSELECTORWIDGET_H
#define DEVICESETSELECTORWIDGET_H

#include "ui_DeviceSetSelectorWidget.h"

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

signals:

protected slots:

protected:
	Ui::DeviceSetSelectorWidget ui;
};

#endif 