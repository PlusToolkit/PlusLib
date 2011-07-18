#include "ConfigurationToolbox.h"

#include "ConfigurationController.h"
#include "vtkFreehandController.h"
#include "vtkFileFinder.h"
#include "FreehandMainWindow.h"

#include "DeviceSetSelectorWidget.h"
#include "ToolStateDisplayWidget.h"

#include <QDialog>

//-----------------------------------------------------------------------------

ConfigurationToolbox::ConfigurationToolbox(QWidget* aParent, Qt::WFlags aFlags)
	: AbstractToolbox()
	, QWidget(aParent, aFlags)
	, m_ToolStatePopOutWindow(NULL)
{
	ui.setupUi(this);

	// Initialize toolbox controller
	ConfigurationController* toolboxController = ConfigurationController::GetInstance();
	if (toolboxController == NULL) {
		LOG_ERROR("Configuration calibration toolbox controller is not initialized!");
		return;
	}

	toolboxController->SetToolbox(this);

	// Create and setup device set selector widget
	m_DeviceSetSelectorWidget = new DeviceSetSelectorWidget(this);
	m_DeviceSetSelectorWidget->SetConfigurationDirectory(vtkFileFinder::GetInstance()->GetConfigurationDirectory());

	m_ToolStateDisplayWidget = new ToolStateDisplayWidget(this);
	m_ToolStateDisplayWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

	// Make connections
	connect( m_DeviceSetSelectorWidget, SIGNAL( ConfigurationDirectoryChanged(std::string) ), this, SLOT( SetConfigurationDirectory(std::string) ) );
	connect( m_DeviceSetSelectorWidget, SIGNAL( ConnectToDevicesByConfigFileInvoked(std::string) ), this, SLOT( ConnectToDevicesByConfigFile(std::string) ) );
	connect( ui.pushButton_PopOut, SIGNAL( toggled(bool) ), this, SLOT( PopOutToggled(bool) ) );

	// Insert widgets into placeholders
	QGridLayout* gridDeviceSetSelection = new QGridLayout(ui.deviceSetSelectionWidget, 1, 1, 0, 4, "");
	gridDeviceSetSelection->addWidget(m_DeviceSetSelectorWidget);
	ui.deviceSetSelectionWidget->setLayout(gridDeviceSetSelection);

	QGridLayout* gridToolStateDisplay = new QGridLayout(ui.toolStateDisplayWidget, 1, 1, 0, 4, "");
	gridToolStateDisplay->addWidget(m_ToolStateDisplayWidget);
	ui.toolStateDisplayWidget->setLayout(gridToolStateDisplay);

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
	if (m_ToolStateDisplayWidget->IsInitialized()) {
		m_ToolStateDisplayWidget->Update();
	}
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
			m_ToolStateDisplayWidget->InitializeTools(NULL, false);
		} else {
			m_DeviceSetSelectorWidget->SetConnectionSuccessful(true);
			m_ToolStateDisplayWidget->InitializeTools(vtkFreehandController::GetInstance()->GetDataCollector(), true);
		}
	}

	// Close dialog
	connectDialog->done(0);

	QApplication::restoreOverrideCursor();
}

//-----------------------------------------------------------------------------

void ConfigurationToolbox::PopOutToggled(bool aOn)
{
	if (aOn) {
		// Create pop out window
		m_ToolStatePopOutWindow = new QWidget(this, Qt::Tool);
		m_ToolStatePopOutWindow->setMinimumSize(QSize(180, 60));
		m_ToolStatePopOutWindow->setMaximumSize(QSize(180, 180));
		m_ToolStatePopOutWindow->setCaption(tr("Tool state display"));
		m_ToolStatePopOutWindow->setBackgroundColor(QColor::fromRgb(255, 255, 255));

		QGridLayout* gridToolStateDisplay = new QGridLayout(m_ToolStatePopOutWindow, 1, 1, 0, 4, "");
		gridToolStateDisplay->addWidget(m_ToolStateDisplayWidget);
		m_ToolStatePopOutWindow->setLayout(gridToolStateDisplay);
		m_ToolStatePopOutWindow->move( mapToGlobal( QPoint( ui.pushButton_PopOut->x(), ui.pushButton_PopOut->y() ) ) );
		m_ToolStatePopOutWindow->show();

		// Install event filter that is called on closing the window
		m_ToolStatePopOutWindow->installEventFilter(this);

		// Delete layout from the toolbox (to be able to add again)
		delete ui.toolStateDisplayWidget->layout();
	} else {
		// Insert tool state display back in toolbox
		QGridLayout* gridToolStateDisplay = new QGridLayout(ui.toolStateDisplayWidget, 1, 1, 0, 4, "");
		gridToolStateDisplay->addWidget(m_ToolStateDisplayWidget);
		ui.toolStateDisplayWidget->setLayout(gridToolStateDisplay);

		// Delete pop out window
		if (m_ToolStatePopOutWindow) {
			delete m_ToolStatePopOutWindow;
		}
		m_ToolStatePopOutWindow = NULL;
	}
}

//-----------------------------------------------------------------------------

bool ConfigurationToolbox::eventFilter(QObject *obj, QEvent *ev)
{
	if ( obj == m_ToolStatePopOutWindow ) {
		if ( ev->type() == QEvent::Close ) {
			ui.pushButton_PopOut->setChecked(false);
		} else {
			// Pass the event on to the parent class
			return QWidget::eventFilter( obj, ev );
		}
	}

	return true;
}
