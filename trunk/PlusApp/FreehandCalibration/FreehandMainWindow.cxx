#include "FreehandMainWindow.h"

#include "vtkFreehandController.h"
#include "ConfigurationToolbox.h"
#include "StylusCalibrationToolbox.h"
#include "PhantomRegistrationToolbox.h"
#include "FreehandCalibrationToolbox.h"
#include "VolumeReconstructionToolbox.h"
#include "StatusIcon.h"

#include <QTimer>
#include <QDir>
#include <QMessageBox>

#include "vtkRenderWindow.h"

//-----------------------------------------------------------------------------

FreehandMainWindow::FreehandMainWindow(QWidget *parent, Qt::WFlags flags)
	:QMainWindow(parent, flags)
	,m_StatusBarLabel(NULL)
	,m_StatusBarProgress(NULL)
	,m_LockedTabIndex(-1)
	,m_ActiveToolbox(ToolboxType_Undefined)
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
	controller->Initialize();

	// Locate directories and set them to freehand controller
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
	QTimer *uiRefreshTimer = new QTimer(this);
	connect(uiRefreshTimer, SIGNAL(timeout()), this, SLOT(UpdateGUI()));
	uiRefreshTimer->start(50);
}

//-----------------------------------------------------------------------------

FreehandMainWindow::~FreehandMainWindow()
{
	if (vtkFreehandController::GetInstance() != NULL) {
		vtkFreehandController::GetInstance()->Delete();
	}

	if (StatusIcon::GetInstance() != NULL) {
		delete StatusIcon::GetInstance();
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

	//TODO Volume reconstruction
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

AbstractToolboxController* FreehandMainWindow::GetToolboxControllerByType(ToolboxType aType)
{
	switch (aType) {
		case ToolboxType_Configuration:
			return NULL; //TODO If there will be a ConfigurationController, it has to go here
		case ToolboxType_StylusCalibration:
			return StylusCalibrationController::GetInstance();
		case ToolboxType_PhantomRegistration:
			return PhantomRegistrationController::GetInstance();
		case ToolboxType_FreehandCalibration:
			return vtkFreehandCalibrationController::GetInstance();
		case ToolboxType_VolumeReconstruction:
			return VolumeReconstructionController::GetInstance();
		default:
			LOG_ERROR("No toolbox of this type found!");
			return NULL;
	}
}

//-----------------------------------------------------------------------------

void FreehandMainWindow::CurrentTabChanged(int aTabIndex)
{
	LOG_DEBUG("Change tab"); 

	if ((vtkFreehandController::GetInstance() == NULL)
		|| (StylusCalibrationController::GetInstance() == NULL)
		|| (PhantomRegistrationController::GetInstance() == NULL)
		|| (vtkFreehandCalibrationController::GetInstance() == NULL)
		|| (VolumeReconstructionController::GetInstance() == NULL))
	{
		LOG_ERROR("Some controllers are not initialized!");
		return;
	}

	// Clear previous toolbox if there was a previous
	if (m_ActiveToolbox != ToolboxType_Undefined) {
		AbstractToolboxController* toolboxController = GetToolboxControllerByType(m_ActiveToolbox);
		if (toolboxController) {
			GetToolboxControllerByType(m_ActiveToolbox)->Clear();
		}
	}

	// Initialize new tab
	if (ui.tabWidgetToolbox->tabText(aTabIndex) == "Configuration") {
		// Initialize configuration
		//ConfigurationController::GetInstance()->Initialize();

		m_ActiveToolbox = ToolboxType_Configuration;

	} else if (ui.tabWidgetToolbox->tabText(aTabIndex) == "Stylus Calibration") {
		// Initialize stylus calibration
		StylusCalibrationController::GetInstance()->Initialize();
		vtkFreehandController::GetInstance()->TrackingOnlyOn();

		m_ActiveToolbox = ToolboxType_StylusCalibration;

	} else if (ui.tabWidgetToolbox->tabText(aTabIndex) == "Phantom Registration") {
		// Initialize phantom registration
		PhantomRegistrationController::GetInstance()->Initialize();
		vtkFreehandController::GetInstance()->TrackingOnlyOn();

		m_ActiveToolbox = ToolboxType_PhantomRegistration;

	} else if (ui.tabWidgetToolbox->tabText(aTabIndex) == "Freehand Calibration") {
		// Initialize freehand calibration
		vtkFreehandCalibrationController::GetInstance()->Initialize();
		vtkFreehandController::GetInstance()->TrackingOnlyOff();

		m_ActiveToolbox = ToolboxType_FreehandCalibration;

	} else if (ui.tabWidgetToolbox->tabText(aTabIndex) == "Volume Reconstruction") {
		// Inialize volume reconstruction
		VolumeReconstructionController::GetInstance()->Initialize();
		vtkFreehandController::GetInstance()->TrackingOnlyOff();

		m_ActiveToolbox = ToolboxType_VolumeReconstruction;

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

	vtkFreehandController* controller = vtkFreehandController::GetInstance();
	if ((controller == NULL)
		|| (StylusCalibrationController::GetInstance() == NULL)
		|| (PhantomRegistrationController::GetInstance() == NULL)
		|| (vtkFreehandCalibrationController::GetInstance() == NULL)) { //TODO add VolumeReconstructor and possibly Configuration if needed
		LOG_ERROR("Some controllers are not initialized!");
		return;
	}

	int tabIndex = ui.tabWidgetToolbox->currentIndex();

	if (ui.tabWidgetToolbox->tabText(tabIndex) == "Configuration") {
		// No update action
	} else if (ui.tabWidgetToolbox->tabText(tabIndex) == "Stylus Calibration") {
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
	} else if (ui.tabWidgetToolbox->tabText(tabIndex) == "Phantom Registration") {
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
	} else if (ui.tabWidgetToolbox->tabText(tabIndex) == "Freehand Calibration") {
		vtkFreehandCalibrationController* toolboxController = vtkFreehandCalibrationController::GetInstance();

		// Update progress bar TODO
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
		toolboxController->GetToolbox()->RefreshToolboxContent(); // TODO put outside block (toolboxController should be AbstractToolboxController)
	} else if (ui.tabWidgetToolbox->tabText(tabIndex) == "Volume Reconstruction") {
		// TODO
	} else {
		LOG_ERROR("No tab with this title found!");
	}

	// Update canvas
	ui.canvas->update();

	// Process all events
	QApplication::processEvents();
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

	vtkFreehandController::GetInstance()->SetProgramPath(programPath.c_str());

	// Locate configuration files directory
	QDir configDir(QString::fromStdString(vtkFreehandController::GetInstance()->GetProgramPath()));
	bool success = configDir.cdUp();
	if (success) {
		configDir.cd("config");
	}
	if (!success) {
		configDir = QDir::root();
	} else {
		configDir.makeAbsolute();
	}

	vtkFreehandController::GetInstance()->SetConfigDirectory(configDir.path());

	// Make output directory
	std::string outputPath = vtksys::SystemTools::CollapseFullPath("./Output", vtkFreehandController::GetInstance()->GetProgramPath()); 
	if (vtksys::SystemTools::MakeDirectory(outputPath.c_str())) {
		vtkFreehandController::GetInstance()->SetOutputFolder(outputPath.c_str());
	}
}

//-----------------------------------------------------------------------------
/*
void FreehandMainWindow::DisplayMessage(const char* aMessage, const int aLevel)
{
	QString message(aMessage);

	switch (aLevel) {
		case PlusLogger::LOG_LEVEL_ERROR:
			QMessageBox::critical(NULL, tr("Error"), message, QMessageBox::Ok, QMessageBox::Ok);
			break;
		case PlusLogger::LOG_LEVEL_WARNING:
			QMessageBox::warning(NULL, tr("Warning"), message, QMessageBox::Ok, QMessageBox::Ok);
			break;
		default:
			QMessageBox::information(NULL, tr("Information"), message, QMessageBox::Ok, QMessageBox::Ok);
			break;
	}
}
*/
