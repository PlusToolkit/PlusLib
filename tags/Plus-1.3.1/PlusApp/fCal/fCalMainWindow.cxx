/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#include "fCalMainWindow.h"

#include "vtkToolVisualizer.h"
#include "ConfigurationToolbox.h"
#include "StylusCalibrationToolbox.h"
#include "PhantomRegistrationToolbox.h"
#include "FreehandCalibrationToolbox.h"
#include "VolumeReconstructionToolbox.h"
#include "StatusIcon.h"

#include <QTimer>
#include <QDir>
#include <QMessageBox>
#include <QLabel>
#include <QProgressBar>

#include "vtkRenderWindow.h"

//-----------------------------------------------------------------------------

fCalMainWindow::fCalMainWindow(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
	, m_StatusBarLabel(NULL)
	, m_StatusBarProgress(NULL)
	, m_LockedTabIndex(-1)
	, m_ActiveToolbox(ToolboxType_Undefined)
  , m_ToolVisualizer(NULL)
{
	// Set up UI
	ui.setupUi(this);

  // Maximize window
  this->setWindowState(this->windowState() ^ Qt::WindowMaximized);

  m_ToolboxList.resize(5);

	// Make connections
	connect(ui.tabWidgetToolbox, SIGNAL(currentChanged(int)), this, SLOT(CurrentTabChanged(int)) );
}

//-----------------------------------------------------------------------------

fCalMainWindow::~fCalMainWindow()
{
	if (m_ToolVisualizer != NULL) {
		m_ToolVisualizer->Delete();
    m_ToolVisualizer = NULL;
	}

  if (m_StatusIcon != NULL) {
		delete m_StatusIcon;
    m_StatusIcon = NULL;
	}

	if (m_UiRefreshTimer != NULL) {
    m_UiRefreshTimer->stop();
		delete m_UiRefreshTimer;
		m_UiRefreshTimer = NULL;
	}

  m_ToolboxList.clear();
}

//-----------------------------------------------------------------------------

void fCalMainWindow::Initialize()
{
  LOG_TRACE("fCalMainWindow::Initialize");

	m_StatusIcon = new StatusIcon(this);

  // Create visualizer
  m_ToolVisualizer = vtkToolVisualizer::New();
  m_ToolVisualizer->Initialize();
	ui.canvas->GetRenderWindow()->AddRenderer(m_ToolVisualizer->GetCanvasRenderer());

	// Create toolboxes
	CreateToolboxes();

	// Set up status bar (message and progress bar)
	SetupStatusBar();

	// Initialize default tab widget
	CurrentTabChanged(ui.tabWidgetToolbox->currentIndex());

	// Set up timer for refreshing UI
	m_UiRefreshTimer = new QTimer(this);
	connect(m_UiRefreshTimer, SIGNAL(timeout()), this, SLOT(UpdateGUI()));
	m_UiRefreshTimer->start(50);
}

//-----------------------------------------------------------------------------

void fCalMainWindow::CreateToolboxes()
{
  LOG_TRACE("fCalMainWindow::CreateToolboxes");

	// Configuration widget
	ConfigurationToolbox* configurationToolbox = new ConfigurationToolbox(this, this);
	if (configurationToolbox != NULL) {
		QGridLayout* grid = new QGridLayout(ui.tab_Configuration, 1, 1, 0, 0, "");
		grid->addWidget(configurationToolbox);
		ui.tab_Configuration->setLayout(grid);
	}

  m_ToolboxList[ToolboxType_Configuration] = configurationToolbox;

	// Stylus calibration widget
	StylusCalibrationToolbox* stylusCalibrationToolbox = new StylusCalibrationToolbox(this);
	if (stylusCalibrationToolbox != NULL) {
		QGridLayout* grid = new QGridLayout(ui.tab_StylusCalibration, 1, 1, 0, 0, "");
		grid->addWidget(stylusCalibrationToolbox);
		ui.tab_StylusCalibration->setLayout(grid);
	}

  m_ToolboxList[ToolboxType_StylusCalibration] = stylusCalibrationToolbox;

	// Phantom registration widget
	PhantomRegistrationToolbox* phantomRegistrationToolbox = new PhantomRegistrationToolbox(this);
	if (phantomRegistrationToolbox != NULL) {
		QGridLayout* grid = new QGridLayout(ui.tab_PhantomRegistration, 1, 1, 0, 0, "");
		grid->addWidget(phantomRegistrationToolbox);
		ui.tab_PhantomRegistration->setLayout(grid);
	}

  m_ToolboxList[ToolboxType_PhantomRegistration] = phantomRegistrationToolbox;

	// Freehand calibration widget
	FreehandCalibrationToolbox* freehandCalibrationToolbox = new FreehandCalibrationToolbox(this);
	if (freehandCalibrationToolbox != NULL) {
		QGridLayout* grid = new QGridLayout(ui.tab_FreehandCalibration, 1, 1, 0, 0, "");
		grid->addWidget(freehandCalibrationToolbox);
		ui.tab_FreehandCalibration->setLayout(grid);
	}

  m_ToolboxList[ToolboxType_FreehandCalibration] = freehandCalibrationToolbox;

	// Volume reconstruction widget
	VolumeReconstructionToolbox* volumeReconstructionToolbox = new VolumeReconstructionToolbox(this);
	if (volumeReconstructionToolbox != NULL) {
		QGridLayout* grid = new QGridLayout(ui.tab_VolumeReconstruction, 1, 1, 0, 0, "");
		grid->addWidget(volumeReconstructionToolbox);
		ui.tab_VolumeReconstruction->setLayout(grid);
	}

  m_ToolboxList[ToolboxType_VolumeReconstruction] = volumeReconstructionToolbox;
}

//-----------------------------------------------------------------------------

void fCalMainWindow::SetupStatusBar()
{
  LOG_TRACE("fCalMainWindow::SetupStatusBar");

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
	
	ui.statusBar->addPermanentWidget(m_StatusIcon);
}

//-----------------------------------------------------------------------------

void fCalMainWindow::CurrentTabChanged(int aTabIndex)
{
  LOG_TRACE("fCalMainWindow::CurrentTabChanged(" << aTabIndex << ")");

	// Initialize new tab
	if (ui.tabWidgetToolbox->tabText(aTabIndex) == "Configuration") {
		m_ActiveToolbox = ToolboxType_Configuration;
	} else if (ui.tabWidgetToolbox->tabText(aTabIndex) == "Stylus Calibration") {
		m_ActiveToolbox = ToolboxType_StylusCalibration;
	} else if (ui.tabWidgetToolbox->tabText(aTabIndex) == "Phantom Registration") {
		m_ActiveToolbox = ToolboxType_PhantomRegistration;
	} else if (ui.tabWidgetToolbox->tabText(aTabIndex) == "Freehand Calibration") {
		m_ActiveToolbox = ToolboxType_FreehandCalibration;
	} else if (ui.tabWidgetToolbox->tabText(aTabIndex) == "Volume Reconstruction") {
		m_ActiveToolbox = ToolboxType_VolumeReconstruction;
	} else {
		LOG_ERROR("No tab with this title found!");
    return;
	}

  m_ToolboxList[m_ActiveToolbox]->Initialize();
  m_ToolboxList[m_ActiveToolbox]->SetDisplayAccordingToState();
}

//-----------------------------------------------------------------------------

void fCalMainWindow::SetTabsEnabled(bool aEnabled)
{
  LOG_TRACE("fCalMainWindow::SetTabsEnabled(" << (aEnabled?"true":"false") << ")");

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

void fCalMainWindow::ChangeBackTab(int aTabIndex)
{
  LOG_TRACE("fCalMainWindow::ChangeBackTab(" << aTabIndex << ")");

	ui.tabWidgetToolbox->blockSignals(true);
	ui.tabWidgetToolbox->setCurrentIndex(m_LockedTabIndex);
	ui.tabWidgetToolbox->blockSignals(false);
}

//-----------------------------------------------------------------------------

void fCalMainWindow::SetStatusBarText(QString aText)
{
  //LOG_TRACE("fCalMainWindow::SetStatusBarText");

  m_StatusBarLabel->setText(aText);
}

//-----------------------------------------------------------------------------

void fCalMainWindow::SetStatusBarProgress(int aPercent)
{
  //LOG_TRACE("fCalMainWindow::SetStatusBarText");

  if (aPercent == -1) {
    m_StatusBarProgress->setVisible(false);
  } else {
		m_StatusBarProgress->setValue(aPercent);
		m_StatusBarProgress->setVisible(true);
  }
}

//-----------------------------------------------------------------------------

void fCalMainWindow::UpdateGUI()
{
  //LOG_TRACE("fCalMainWindow::UpdateGUI");

	// We do not update the gui when a mouse button is pressed
	if (QApplication::mouseButtons() != Qt::NoButton)
  {
		return;
	}

  m_ToolboxList[m_ActiveToolbox]->RefreshContent();

	// Refresh tool state display if detached
  if (m_ActiveToolbox != ToolboxType_Configuration)
  {
    ConfigurationToolbox* configurationToolbox = dynamic_cast<ConfigurationToolbox*>(m_ToolboxList[ToolboxType_Configuration]);
    if (configurationToolbox)
    {
      configurationToolbox->RefreshToolDisplayIfDetached();
    }
	}

	// Update canvas
  if ((m_ToolVisualizer->GetDataCollector() != NULL) && (m_ToolVisualizer->GetDataCollector()->GetConnected()))
  {
	  ui.canvas->update();
  }
}

//-----------------------------------------------------------------------------

void fCalMainWindow::resizeEvent(QResizeEvent* aEvent)
{
  LOG_TRACE("fCalMainWindow::resizeEvent");

  if ((m_ToolVisualizer != NULL) && (m_ToolVisualizer->GetImageMode())) {
    m_ToolVisualizer->CalculateImageCameraParameters();
  }
}

//-----------------------------------------------------------------------------

void fCalMainWindow::ResetAllToolboxes()
{
  LOG_TRACE("fCalMainWindow::ResetAllToolboxes");

  m_ToolVisualizer->HideAll();

  for (std::vector<AbstractToolbox*>::iterator it = m_ToolboxList.begin(); it != m_ToolboxList.end(); ++it) {
    if ( (*it) != NULL ) {
      (*it)->Reset();
    }
  }
}
