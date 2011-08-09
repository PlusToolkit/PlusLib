#include "FreehandMainWindow.h"

#include "ConfigurationToolbox.h"
#include "ConfigurationController.h"
#include "StylusCalibrationToolbox.h"
#include "StylusCalibrationController.h"
#include "PhantomRegistrationToolbox.h"
#include "PhantomRegistrationController.h"
#include "FreehandCalibrationToolbox.h"
#include "vtkFreehandCalibrationController.h"
#include "VolumeReconstructionToolbox.h"
#include "VolumeReconstructionController.h"
#include "StatusIcon.h"

#include <QTimer>
#include <QDir>
#include <QMessageBox>
#include <QLabel>
#include <QProgressBar>

#include "vtkRenderWindow.h"
#include "vtkFileFinder.h"

//-----------------------------------------------------------------------------

FreehandMainWindow::FreehandMainWindow(QWidget *parent, Qt::WFlags flags)
	:QMainWindow(parent, flags)
	,m_StatusBarLabel(NULL)
	,m_StatusBarProgress(NULL)
	,m_LockedTabIndex(-1)
{
	// Set up UI
	ui.setupUi(this);

	// Make connections
	connect(ui.tabWidgetToolbox, SIGNAL(currentChanged(int)), this, SLOT(CurrentTabChanged(int)) );
}

//-----------------------------------------------------------------------------

void FreehandMainWindow::Initialize()
{
	// Set up controller
	vtkFreehandController* controller = vtkFreehandController::GetInstance();
	if (controller == NULL) {
		LOG_ERROR("Failed to create controller!");
		return;
	}
	controller->SetCanvas(ui.canvas);
	controller->Initialize(true);

	// Locate directories and set them to file finder
	LocateDirectories();

	// Create toolboxes
	CreateToolboxes();

	// Set up status bar (message and progress bar)
	SetupStatusBar();

	// Set up canvas for 3D visualization
	SetupCanvas();

	// Initialize default tab widget
	CurrentTabChanged(ui.tabWidgetToolbox->currentIndex());

	// Set up timer for refreshing UI
	m_UiRefreshTimer = new QTimer(this);
	connect(m_UiRefreshTimer, SIGNAL(timeout()), this, SLOT(UpdateGUI()));
	m_UiRefreshTimer->start(50);
}

//-----------------------------------------------------------------------------

FreehandMainWindow::~FreehandMainWindow()
{
	if (vtkFreehandController::GetInstance() != NULL) {
		vtkFreehandController::GetInstance()->Delete();
	}

	PlusLogger::Instance()->SetDisplayMessageCallbackFunction(NULL);
	if (StatusIcon::GetInstance() != NULL) {
		delete StatusIcon::GetInstance();
	}

	if (m_UiRefreshTimer != NULL) {
    m_UiRefreshTimer->stop();
		delete m_UiRefreshTimer;
		m_UiRefreshTimer = NULL;
	}
}

//-----------------------------------------------------------------------------

void FreehandMainWindow::CreateToolboxes()
{
	LOG_DEBUG("Create toolboxes"); 

	// Configuration widget
	ConfigurationToolbox* configurationToolbox = new ConfigurationToolbox(ui.tab_Configuration);

	if (configurationToolbox != NULL) {
		QGridLayout* grid = new QGridLayout(ui.tab_Configuration, 1, 1, 0, 0, "");
		grid->addWidget(configurationToolbox);
		ui.tab_Configuration->setLayout(grid);

		connect( configurationToolbox, SIGNAL( SetTabsEnabled(bool) ), this, SLOT( SetTabsEnabled(bool) ) );
	}

	// Stylus calibration widget
	StylusCalibrationToolbox* stylusCalibrationToolbox = new StylusCalibrationToolbox(ui.tab_StylusCalibration);

	if (stylusCalibrationToolbox != NULL) {
		QGridLayout* grid = new QGridLayout(ui.tab_StylusCalibration, 1, 1, 0, 0, "");
		grid->addWidget(stylusCalibrationToolbox);
		ui.tab_StylusCalibration->setLayout(grid);

		connect( stylusCalibrationToolbox, SIGNAL( SetTabsEnabled(bool) ), this, SLOT( SetTabsEnabled(bool) ) );
	}

	// Phantom registration widget
	PhantomRegistrationToolbox* phantomRegistrationToolbox = new PhantomRegistrationToolbox(ui.tab_PhantomRegistration);

	if (phantomRegistrationToolbox != NULL) {
		QGridLayout* grid = new QGridLayout(ui.tab_PhantomRegistration, 1, 1, 0, 0, "");
		grid->addWidget(phantomRegistrationToolbox);
		ui.tab_PhantomRegistration->setLayout(grid);

		connect( phantomRegistrationToolbox, SIGNAL( SetTabsEnabled(bool) ), this, SLOT( SetTabsEnabled(bool) ) );
	}

	// Freehand calibration widget
	FreehandCalibrationToolbox* freehandCalibrationToolbox = new FreehandCalibrationToolbox(ui.tab_FreehandCalibration);

	if (freehandCalibrationToolbox != NULL) {
		QGridLayout* grid = new QGridLayout(ui.tab_FreehandCalibration, 1, 1, 0, 0, "");
		grid->addWidget(freehandCalibrationToolbox);
		ui.tab_FreehandCalibration->setLayout(grid);

		connect( freehandCalibrationToolbox, SIGNAL( SetTabsEnabled(bool) ), this, SLOT( SetTabsEnabled(bool) ) );
	}

	// volume reconstruction widget
	VolumeReconstructionToolbox* volumeReconstructionToolbox = new VolumeReconstructionToolbox(ui.tab_VolumeReconstruction);

	if (volumeReconstructionToolbox != NULL) {
		QGridLayout* grid = new QGridLayout(ui.tab_VolumeReconstruction, 1, 1, 0, 0, "");
		grid->addWidget(volumeReconstructionToolbox);
		ui.tab_VolumeReconstruction->setLayout(grid);

		connect( volumeReconstructionToolbox, SIGNAL( SetTabsEnabled(bool) ), this, SLOT( SetTabsEnabled(bool) ) );
	}
}

//-----------------------------------------------------------------------------

void FreehandMainWindow::SetupStatusBar()
{
	LOG_DEBUG("Set up statusbar"); 

	// Set up status bar
	QSizePolicy sizePolicy;
	sizePolicy.setHorizontalPolicy(QSizePolicy::Expanding);

	m_StatusBarLabel = new QLabel(ui.statusBar);
	m_StatusBarLabel->setSizePolicy(sizePolicy);

	m_StatusBarProgress = new QProgressBar(ui.statusBar);
	m_StatusBarProgress->setSizePolicy(sizePolicy);
	m_StatusBarProgress->hide();

	ui.statusBar->addWidget(m_StatusBarLabel, 1);
	ui.statusBar->addPermanentWidget(m_StatusBarProgress, 3);
	
	StatusIcon* statusIcon = StatusIcon::New(this);
	ui.statusBar->addPermanentWidget(statusIcon);

	// Set callback for logger to display errors
	PlusLogger::Instance()->SetDisplayMessageCallbackFunction(StatusIcon::AddMessage);
}

//-----------------------------------------------------------------------------

void FreehandMainWindow::SetupCanvas()
{
	LOG_DEBUG("Set up canvas"); 

	vtkFreehandController* controller = vtkFreehandController::GetInstance();
	if (controller == NULL) {
		return;
	}

	ui.canvas->GetRenderWindow()->AddRenderer(controller->GetCanvasRenderer());
}

//-----------------------------------------------------------------------------

void FreehandMainWindow::CurrentTabChanged(int aTabIndex)
{
	LOG_DEBUG("Change tab"); 

	vtkFreehandController* controller = vtkFreehandController::GetInstance();
	if (controller->GetInitialized() == false) {
		LOG_ERROR("Freehand controller is not initialized!");
		return;
	}

	// Clear previous toolbox if there was a previous
	if (controller->GetActiveToolbox() != FCAL_TOOLBOX_UNDEFINED) {
		AbstractToolboxController* toolboxController = GetToolboxControllerByType(controller->GetActiveToolbox());
		if (toolboxController) {
			controller->GetActiveToolboxController()->Clear();
		}
	}

	// Initialize new tab
	if (ui.tabWidgetToolbox->tabText(aTabIndex) == "Configuration") {
		// Initialize configuration
		ConfigurationController::GetInstance()->Initialize();
		controller->SetActiveToolbox(FCAL_TOOLBOX_CONFIGURATION);

	} else if (ui.tabWidgetToolbox->tabText(aTabIndex) == "Stylus Calibration") {
		// Initialize stylus calibration
		StylusCalibrationController::GetInstance()->Initialize();
 		controller->SetActiveToolbox(FCAL_TOOLBOX_STYLUS_CALIBRATION);

	} else if (ui.tabWidgetToolbox->tabText(aTabIndex) == "Phantom Registration") {
		// Initialize phantom registration
		PhantomRegistrationController::GetInstance()->Initialize();
    controller->SetActiveToolbox(FCAL_TOOLBOX_PHANTOM_REGISTRATION);

	} else if (ui.tabWidgetToolbox->tabText(aTabIndex) == "Freehand Calibration") {
		// Initialize freehand calibration
		vtkFreehandCalibrationController::GetInstance()->Initialize();
    controller->SetActiveToolbox(FCAL_TOOLBOX_FREEHAND_CALIBRATION);

	} else if (ui.tabWidgetToolbox->tabText(aTabIndex) == "Volume Reconstruction") {
		// Inialize volume reconstruction
		VolumeReconstructionController::GetInstance()->Initialize();
    controller->SetActiveToolbox(FCAL_TOOLBOX_VOLUME_RECONSTRUCTION);

	} else {
		LOG_ERROR("No tab with this title found!");
	}
}

//-----------------------------------------------------------------------------

void FreehandMainWindow::SetTabsEnabled(bool aEnabled)
{
	LOG_DEBUG((aEnabled ? "Enable" : "Disable") << " tabbing in main window"); 

	if (aEnabled) {
		m_LockedTabIndex = -1;
		disconnect(ui.tabWidgetToolbox, SIGNAL(currentChanged(int)), this, SLOT(ChangeBackTab(int)) );
		connect(ui.tabWidgetToolbox, SIGNAL(currentChanged(int)), this, SLOT(CurrentTabChanged(int)) );
	} else {
		m_LockedTabIndex = ui.tabWidgetToolbox->currentIndex();
		disconnect(ui.tabWidgetToolbox, SIGNAL(currentChanged(int)), this, SLOT(CurrentTabChanged(int)) );
		connect(ui.tabWidgetToolbox, SIGNAL(currentChanged(int)), this, SLOT(ChangeBackTab(int)) );
	}
}

//-----------------------------------------------------------------------------

void FreehandMainWindow::ChangeBackTab(int aTabIndex)
{
	LOG_DEBUG("Change back to locked tab"); 
	ui.tabWidgetToolbox->blockSignals(true);
	ui.tabWidgetToolbox->setCurrentIndex(m_LockedTabIndex);
	ui.tabWidgetToolbox->blockSignals(false);
}

//-----------------------------------------------------------------------------

void FreehandMainWindow::UpdateGUI()
{
	//LOG_TRACE("Update GUI");

	// We do not update the gui when a mouse button is pressed
	if (QApplication::mouseButtons() != Qt::NoButton) {
		return;
	}

	vtkFreehandController* controller = vtkFreehandController::GetInstance();
	if (controller->GetInitialized() == false) {
		LOG_ERROR("Freehand controller is not initialized!");
		return;
	}

	int tabIndex = ui.tabWidgetToolbox->currentIndex();

	if (controller->GetActiveToolbox() == FCAL_TOOLBOX_CONFIGURATION) {
		// Update toolbox (device states)
		ConfigurationController* toolboxController = ConfigurationController::GetInstance();

		if (toolboxController->State() != ToolboxState_Uninitialized) {
			toolboxController->GetToolbox()->RefreshToolboxContent();
		}

	} else if (controller->GetActiveToolbox() == FCAL_TOOLBOX_STYLUS_CALIBRATION) {
		StylusCalibrationController* toolboxController = StylusCalibrationController::GetInstance();

		// Update progress bar
		if (toolboxController->State() == ToolboxState_InProgress) {
			m_StatusBarLabel->setText(QString(" Recording stylus positions"));
			m_StatusBarProgress->setVisible(true);
			m_StatusBarProgress->setValue((int)(100.0 * (toolboxController->GetCurrentPointNumber() / (double)toolboxController->GetNumberOfPoints()) + 0.5));
		} else
		// If done
		if (toolboxController->State() == ToolboxState_Done) {
			m_StatusBarLabel->setText(QString(" Stylus calibration done"));
			m_StatusBarProgress->setVisible(false);
			m_StatusBarProgress->setValue(0);
		} else {
			m_StatusBarLabel->setText(QString(""));
			m_StatusBarProgress->setVisible(false);
			m_StatusBarProgress->setValue(0);
		}

		// Refresh toolbox content
		toolboxController->GetToolbox()->RefreshToolboxContent();
	} else if (controller->GetActiveToolbox() == FCAL_TOOLBOX_PHANTOM_REGISTRATION) {
		PhantomRegistrationController* toolboxController = PhantomRegistrationController::GetInstance();

		// Update progress bar
		if (toolboxController->State() == ToolboxState_InProgress) {
			m_StatusBarLabel->setText(QString(" Recording phantom landmarks"));
			m_StatusBarProgress->setVisible(true);
			m_StatusBarProgress->setValue((int)(100.0 * (toolboxController->GetCurrentLandmarkIndex() / (double)toolboxController->GetNumberOfLandmarks()) + 0.5));
		} else
		// If done
		if (toolboxController->State() == ToolboxState_Done) {
			m_StatusBarLabel->setText(QString(" Phantom registration done"));
			m_StatusBarProgress->setVisible(false);
			m_StatusBarProgress->setValue(0);
		} else {
			m_StatusBarLabel->setText(QString(""));
			m_StatusBarProgress->setVisible(false);
			m_StatusBarProgress->setValue(0);
		}

		// Refresh toolbox content
		toolboxController->GetToolbox()->RefreshToolboxContent();
	} else if (controller->GetActiveToolbox() == FCAL_TOOLBOX_FREEHAND_CALIBRATION) {
		vtkFreehandCalibrationController* toolboxController = vtkFreehandCalibrationController::GetInstance();

		// Update progress bar
		if (toolboxController->State() == ToolboxState_InProgress) {
			m_StatusBarLabel->setText(QString(" Acquiring and adding images to calibrator"));
			m_StatusBarProgress->setVisible(true);
			m_StatusBarProgress->setValue(toolboxController->GetProgressPercent());
		} else
		// If done
		if (toolboxController->State() == ToolboxState_Done) {
			m_StatusBarLabel->setText(QString(" Calibration done"));
			m_StatusBarProgress->setVisible(false);
			m_StatusBarProgress->setValue(100);
		} else {
			m_StatusBarLabel->setText(QString(""));
			m_StatusBarProgress->setVisible(false);
			m_StatusBarProgress->setValue(0);
		}

		// Refresh toolbox content
		toolboxController->GetToolbox()->RefreshToolboxContent();
	} else if (controller->GetActiveToolbox() == FCAL_TOOLBOX_VOLUME_RECONSTRUCTION) {
		VolumeReconstructionController* toolboxController = VolumeReconstructionController::GetInstance();

		// Update progress bar
		if (toolboxController->State() == ToolboxState_InProgress) {
			m_StatusBarLabel->setText(QString::fromStdString(toolboxController->GetProgressMessage()));
			m_StatusBarProgress->setVisible(true);
			m_StatusBarProgress->setValue(toolboxController->GetProgressPercent());
		} else
		// If done
		if (toolboxController->State() == ToolboxState_Done) {
			m_StatusBarLabel->setText(QString(" Reconstruction done"));
			m_StatusBarProgress->setVisible(false);
			m_StatusBarProgress->setValue(100);
		} else {
			m_StatusBarLabel->setText(QString(""));
			m_StatusBarProgress->setVisible(false);
			m_StatusBarProgress->setValue(0);
		}

		// Refresh toolbox content
		toolboxController->GetToolbox()->RefreshToolboxContent();
	} else {
		LOG_ERROR("No tab with this title found!");
	}

	// Refresh tool state display if detached
	if ((controller->GetActiveToolbox() == FCAL_TOOLBOX_CONFIGURATION) && (ConfigurationController::GetInstance()->State() != ToolboxState_Uninitialized) && (ConfigurationController::GetInstance()->IsToolDisplayDetached())) {
		ConfigurationController::GetInstance()->GetToolbox()->RefreshToolboxContent();
	}

	// Update canvas
	ui.canvas->update();

	// Process all events
	QApplication::processEvents(); //TODO is it needed here?
}

//-----------------------------------------------------------------------------

void FreehandMainWindow::LocateDirectories()
{
	// Locate program path
	std::string programPath("./"), errorMsg; 
	if ( !vtksys::SystemTools::FindProgramPath(qApp->argv()[0], programPath, errorMsg) ) {
		LOG_ERROR(errorMsg); 
	}
	programPath = vtksys::SystemTools::GetParentDirectory(programPath.c_str()); 

	// Locate configuration files directory
	QDir configDir(QString::fromStdString(programPath));
	bool success = configDir.cdUp();
	if (success) {
		configDir.cd("config");
	}
	if (!success) {
		configDir = QDir::root();
	} else {
		configDir.makeAbsolute();
	}

	vtkFileFinder::GetInstance()->SetConfigurationDirectory(configDir.path().toStdString().c_str());

	// Make output directory
	std::string outputPath = vtksys::SystemTools::CollapseFullPath("./Output", programPath.c_str()); 
	if (vtksys::SystemTools::MakeDirectory(outputPath.c_str())) {
		vtkFreehandController::GetInstance()->SetOutputFolder(outputPath.c_str());
	}
}
