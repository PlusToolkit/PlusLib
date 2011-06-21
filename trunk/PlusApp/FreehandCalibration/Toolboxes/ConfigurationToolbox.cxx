#include "ConfigurationToolbox.h"

#include "FreehandMainWindow.h"
#include "DeviceSetSelectorWidget.h"

//-----------------------------------------------------------------------------

ConfigurationToolbox::ConfigurationToolbox(QWidget* aParent, Qt::WFlags aFlags)
	: AbstractToolbox()
	, QWidget(aParent, aFlags)
{
	ui.setupUi(this);

	// Create and setup device set selector widget
	DeviceSetSelectorWidget* deviceSetSelectorWidget = new DeviceSetSelectorWidget(this);

	QGridLayout* grid = new QGridLayout(ui.deviceSetSelectionWidget, 1, 1, 0, 0, "");
	grid->addWidget(deviceSetSelectorWidget);
	ui.deviceSetSelectionWidget->setLayout(grid);

	//TODO tooltips
}

//-----------------------------------------------------------------------------

ConfigurationToolbox::~ConfigurationToolbox()
{
}

//-----------------------------------------------------------------------------

void ConfigurationToolbox::Initialize()
{
}

//-----------------------------------------------------------------------------

void ConfigurationToolbox::RefreshToolboxContent()
{
}

//-----------------------------------------------------------------------------

void ConfigurationToolbox::Stop()
{
}

//-----------------------------------------------------------------------------

void ConfigurationToolbox::Clear()
{
}
