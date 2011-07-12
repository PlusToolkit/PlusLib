#include "ConfigurationToolbox.h"

#include "vtkFreehandController.h"
#include "vtkFileFinder.h"
#include "FreehandMainWindow.h"

#include <QDialog>

//-----------------------------------------------------------------------------

ConfigurationToolbox::ConfigurationToolbox(QWidget* aParent, Qt::WFlags aFlags)
	: AbstractToolbox()
	, QWidget(aParent, aFlags)
{
	ui.setupUi(this);

	vtkFreehandController* controller = vtkFreehandController::GetInstance();
	if ((controller == NULL) || (controller->GetInitialized() == false)) {
		LOG_ERROR("vtkFreehandController is not initialized!");
		return;
	}

	// Create and setup device set selector widget
	m_DeviceSetSelectorWidget = new DeviceSetSelectorWidget(this);
	m_DeviceSetSelectorWidget->SetConfigurationDirectory(vtkFileFinder::GetInstance()->GetConfigurationDirectory());

	connect( m_DeviceSetSelectorWidget, SIGNAL( ConfigurationDirectoryChanged(std::string) ), this, SLOT( SetConfigurationDirectory(std::string) ) );
	connect( m_DeviceSetSelectorWidget, SIGNAL( ConnectToDevicesByConfigFileInvoked(std::string) ), this, SLOT( ConnectToDevicesByConfigFile(std::string) ) );

	QGridLayout* grid = new QGridLayout(ui.deviceSetSelectionWidget, 1, 1, 0, 0, "");
	grid->addWidget(m_DeviceSetSelectorWidget);
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

//-----------------------------------------------------------------------------

void ConfigurationToolbox::SetConfigurationDirectory(std::string aDirectory)
{
	vtkFileFinder::GetInstance()->SetConfigurationDirectory(aDirectory.c_str());
}

//-----------------------------------------------------------------------------

void ConfigurationToolbox::ConnectToDevicesByConfigFile(std::string aConfigFile)
{
	vtkFreehandController::GetInstance()->SetInputConfigFileName(aConfigFile.data());

	LOG_INFO("Connect to devices"); 

	QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));

	// Create dialog
	QDialog* connectDialog = new QDialog(this, Qt::Dialog);
	connectDialog->setModal(true);
	connectDialog->setMinimumSize(QSize(360,80));
	connectDialog->setCaption(tr("Freehand Ultrasound Calibration"));
	connectDialog->setBackgroundColor(QColor(224, 224, 224));

	QLabel* connectLabel = new QLabel(QString("Connecting to devices, please wait..."), connectDialog);
	connectLabel->setFont(QFont("SansSerif", 16));

	QHBoxLayout* layout = new QHBoxLayout();
	layout->addWidget(connectLabel);

	connectDialog->setLayout(layout);
	connectDialog->show();

	QApplication::processEvents();

	// Connect to devices
	vtkFreehandController* controller = vtkFreehandController::GetInstance();
	if ((controller != NULL) && (controller->GetInitialized() == true)) {
		if (vtkFreehandController::GetInstance()->StartDataCollection() != PLUS_SUCCESS) {
			LOG_ERROR("Unable to start collecting data!");
			m_DeviceSetSelectorWidget->SetConnectionSuccessful(false);
		} else {
			m_DeviceSetSelectorWidget->SetConnectionSuccessful(true);
		}
	}

	// Close dialog
	connectDialog->done(0);

	QApplication::restoreOverrideCursor();
}
