/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#include "fCalMainWindow.h"

#include "vtkObjectVisualizer.h"
#include "ConfigurationToolbox.h"
#include "StylusCalibrationToolbox.h"
#include "PhantomRegistrationToolbox.h"
#include "FreehandCalibrationToolbox.h"
#include "CapturingToolbox.h"
#include "VolumeReconstructionToolbox.h"
#include "StatusIcon.h"
#include "ConfigFileSaverDialog.h"

#include <QTimer>
#include <QMenu>
#include <QLabel>
#include <QProgressBar>
#include <QFileDialog>

#include "vtkRenderWindow.h"

//-----------------------------------------------------------------------------

fCalMainWindow::fCalMainWindow(QWidget *parent, Qt::WFlags flags)
: QMainWindow(parent, flags)
, m_StatusBarLabel(NULL)
, m_StatusBarProgress(NULL)
, m_LockedTabIndex(-1)
, m_ActiveToolbox(ToolboxType_Undefined)
, m_ObjectVisualizer(NULL)
, m_StatusIcon(NULL)
, m_ImageCoordinateFrame("")
, m_ProbeCoordinateFrame("")
, m_ReferenceCoordinateFrame("")
, m_TransducerOriginCoordinateFrame("")
, m_TransducerOriginPixelCoordinateFrame("")
, m_ShowDevices(false)
, m_ShowPoints(false)
, m_ShowOrientationMarkerAction(NULL)
{
  // Set up UI
  ui.setupUi(this);

  // Maximize window
  this->setWindowState(this->windowState() ^ Qt::WindowMaximized);
}

//-----------------------------------------------------------------------------

fCalMainWindow::~fCalMainWindow()
{
  if (m_ObjectVisualizer != NULL)
  {
    m_ObjectVisualizer->Delete();
    m_ObjectVisualizer = NULL;
  }

  if (m_StatusIcon != NULL)
  {
    delete m_StatusIcon;
    m_StatusIcon = NULL;
  }

  if (m_UiRefreshTimer != NULL)
  {
    m_UiRefreshTimer->stop();
    delete m_UiRefreshTimer;
    m_UiRefreshTimer = NULL;
  }

  if( m_ShowOrientationMarkerAction != NULL )
  {
    delete m_ShowOrientationMarkerAction;
  }

  m_ToolboxList.clear();
}

//-----------------------------------------------------------------------------

void fCalMainWindow::Initialize()
{
  LOG_TRACE("fCalMainWindow::Initialize");

  // Create status icon
  m_StatusIcon = new StatusIcon(this);

  // Set up timer for refreshing UI
  m_UiRefreshTimer = new QTimer(this);

  // Set up menu items for tools button
  QAction* dumpBuffersAction = new QAction("Dump buffers into files...", ui.pushButton_Tools);
  connect(dumpBuffersAction, SIGNAL(triggered()), this, SLOT(DumpBuffers()));
  ui.pushButton_Tools->addAction(dumpBuffersAction);

  // Declare this class as the event handler
  ui.pushButton_Tools->installEventFilter(this);

  // Set up menu items for image manipulation button
  QAction* mfRightUpAction = new QAction("Marked Right, Far Up", ui.pushButton_ImageOrientation);
  connect(mfRightUpAction, SIGNAL(triggered()), this, SLOT(SetOrientationMRightFUp()));
  ui.pushButton_ImageOrientation->addAction(mfRightUpAction);
  QAction* mfLeftUpAction = new QAction("Marked Left, Far Up", ui.pushButton_ImageOrientation);
  connect(mfLeftUpAction, SIGNAL(triggered()), this, SLOT(SetOrientationMLeftFUp()));
  ui.pushButton_ImageOrientation->addAction(mfLeftUpAction);
  QAction* mfRightDownAction = new QAction("Marked Right, Far Down", ui.pushButton_ImageOrientation);
  connect(mfRightDownAction, SIGNAL(triggered()), this, SLOT(SetOrientationMRightFDown()));
  ui.pushButton_ImageOrientation->addAction(mfRightDownAction);
  QAction* mfLeftDownAction = new QAction("Marked Left, Far Down", ui.pushButton_ImageOrientation);
  connect(mfLeftDownAction, SIGNAL(triggered()), this, SLOT(SetOrientationMLeftFDown()));
  ui.pushButton_ImageOrientation->addAction(mfLeftDownAction);
  m_ShowOrientationMarkerAction = new QAction("Show Orientation Markers", ui.pushButton_ImageOrientation);
  connect(m_ShowOrientationMarkerAction, SIGNAL(triggered()), this, SLOT(EnableOrientationMarkers()));
  m_ShowOrientationMarkerAction->setCheckable(true);
  ui.pushButton_ImageOrientation->addAction(m_ShowOrientationMarkerAction);

  // Declare this class as the event handler
  ui.pushButton_ImageOrientation->installEventFilter(this);

  // Create visualizer
  m_ObjectVisualizer = vtkObjectVisualizer::New();
  m_ObjectVisualizer->Initialize();
  ui.canvas->GetRenderWindow()->AddRenderer(m_ObjectVisualizer->GetCanvasRenderer());

  // Create toolboxes
  CreateToolboxes();

  // Set up status bar (message and progress bar)
  SetupStatusBar();

  // Make connections
  connect( ui.tabWidgetToolbox, SIGNAL( currentChanged(int) ), this, SLOT( CurrentTabChanged(int) ) );
  connect( ui.pushButton_SaveConfiguration, SIGNAL( clicked() ), this, SLOT( SaveDeviceSetConfiguration() ) );
  connect( ui.pushButton_ShowDevices, SIGNAL( toggled(bool) ), this, SLOT( ShowDevicesToggled(bool) ) );
  connect( m_UiRefreshTimer, SIGNAL( timeout() ), this, SLOT( UpdateGUI() ) );

  // Tell the object visualizer to show orientation markers
  m_ShowOrientationMarkerAction->setChecked(true);
  m_ObjectVisualizer->EnableOrientationMarkers(true);

  // Initialize default tab widget
  CurrentTabChanged(ui.tabWidgetToolbox->currentIndex());

  // Start timer
  m_UiRefreshTimer->start(50);
}

//-----------------------------------------------------------------------------

void fCalMainWindow::CreateToolboxes()
{
  LOG_TRACE("fCalMainWindow::CreateToolboxes");

  // Resize toolbox list to the number of toolboxes
  m_ToolboxList.resize(6);

  // Configuration widget
  ConfigurationToolbox* configurationToolbox = new ConfigurationToolbox(this);
  if (configurationToolbox != NULL)
  {
    QGridLayout* grid = new QGridLayout(ui.tab_Configuration, 1, 1, 0, 0, "");
    grid->addWidget(configurationToolbox);
    ui.tab_Configuration->setLayout(grid);
  }

  m_ToolboxList[ToolboxType_Configuration] = configurationToolbox;

  // Stylus calibration widget
  StylusCalibrationToolbox* stylusCalibrationToolbox = new StylusCalibrationToolbox(this);
  if (stylusCalibrationToolbox != NULL)
  {
    QGridLayout* grid = new QGridLayout(ui.tab_StylusCalibration, 1, 1, 0, 0, "");
    grid->addWidget(stylusCalibrationToolbox);
    ui.tab_StylusCalibration->setLayout(grid);
  }

  m_ToolboxList[ToolboxType_StylusCalibration] = stylusCalibrationToolbox;

  // Phantom registration widget
  PhantomRegistrationToolbox* phantomRegistrationToolbox = new PhantomRegistrationToolbox(this);
  if (phantomRegistrationToolbox != NULL)
  {
    QGridLayout* grid = new QGridLayout(ui.tab_PhantomRegistration, 1, 1, 0, 0, "");
    grid->addWidget(phantomRegistrationToolbox);
    ui.tab_PhantomRegistration->setLayout(grid);
  }

  m_ToolboxList[ToolboxType_PhantomRegistration] = phantomRegistrationToolbox;

  // Freehand calibration widget
  FreehandCalibrationToolbox* freehandCalibrationToolbox = new FreehandCalibrationToolbox(this);
  if (freehandCalibrationToolbox != NULL)
  {
    QGridLayout* grid = new QGridLayout(ui.tab_FreehandCalibration, 1, 1, 0, 0, "");
    grid->addWidget(freehandCalibrationToolbox);
    ui.tab_FreehandCalibration->setLayout(grid);
  }

  m_ToolboxList[ToolboxType_FreehandCalibration] = freehandCalibrationToolbox;

  // Capturing widget
  CapturingToolbox* capturingToolbox = new CapturingToolbox(this);
  if (capturingToolbox != NULL)
  {
    QGridLayout* grid = new QGridLayout(ui.tab_Capturing, 1, 1, 0, 0, "");
    grid->addWidget(capturingToolbox);
    ui.tab_Capturing->setLayout(grid);
  }

  m_ToolboxList[ToolboxType_Capturing] = capturingToolbox;

  // Volume reconstruction widget
  VolumeReconstructionToolbox* volumeReconstructionToolbox = new VolumeReconstructionToolbox(this);
  if (volumeReconstructionToolbox != NULL)
  {
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
  if (ui.tabWidgetToolbox->tabText(aTabIndex) == "Configuration")
  {
    m_ActiveToolbox = ToolboxType_Configuration;
  }
  else if (ui.tabWidgetToolbox->tabText(aTabIndex) == "Stylus Calibration")
  {
    m_ActiveToolbox = ToolboxType_StylusCalibration;
  }
  else if (ui.tabWidgetToolbox->tabText(aTabIndex) == "Phantom Registration")
  {
    m_ActiveToolbox = ToolboxType_PhantomRegistration;
  }
  else if (ui.tabWidgetToolbox->tabText(aTabIndex) == "Freehand Calibration")
  {
    m_ActiveToolbox = ToolboxType_FreehandCalibration;
  }
  else if (ui.tabWidgetToolbox->tabText(aTabIndex) == "Capturing")
  {
    m_ActiveToolbox = ToolboxType_Capturing;
  }
  else if (ui.tabWidgetToolbox->tabText(aTabIndex) == "Volume Reconstruction")
  {
    m_ActiveToolbox = ToolboxType_VolumeReconstruction;
  }
  else
  {
    LOG_ERROR("No tab with this title found!");
    return;
  }

  m_ToolboxList[m_ActiveToolbox]->Initialize();
  m_ToolboxList[m_ActiveToolbox]->SetDisplayAccordingToState();

  LOG_INFO("Tab changed to " << ui.tabWidgetToolbox->tabText(aTabIndex).toAscii().data());
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

void fCalMainWindow::SetImageManipulationEnabled( bool aEnabled )
{
  LOG_TRACE("fCalMainWindow::SetImageManipulationEnabled(" << (aEnabled?"true":"false") << ")");

  ui.pushButton_ImageOrientation->setEnabled(aEnabled);
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
  if ((m_ObjectVisualizer->GetDataCollector() != NULL) && (m_ObjectVisualizer->GetDataCollector()->GetConnected()))
  {
    m_ObjectVisualizer->GetDataCollector()->Modified();
    ui.canvas->update();
  }
}

//-----------------------------------------------------------------------------

void fCalMainWindow::resizeEvent(QResizeEvent* aEvent)
{
  LOG_TRACE("fCalMainWindow::resizeEvent");

  if ((m_ObjectVisualizer != NULL) && (m_ObjectVisualizer->GetImageMode())) {
    m_ObjectVisualizer->CalculateImageCameraParameters();
  }
}

//-----------------------------------------------------------------------------

void fCalMainWindow::ResetAllToolboxes()
{
  LOG_TRACE("fCalMainWindow::ResetAllToolboxes");

  m_ObjectVisualizer->HideAll();

  for (std::vector<AbstractToolbox*>::iterator it = m_ToolboxList.begin(); it != m_ToolboxList.end(); ++it)
  {
    if ( ((*it) != NULL) && ((*it)->GetState() > ToolboxState_Idle) )
    {
      (*it)->Reset();
    }
  }
}

//-----------------------------------------------------------------------------

bool fCalMainWindow::eventFilter(QObject *obj, QEvent *ev)
{
  //LOG_TRACE("fCalMainWindow::eventFilter"); 

  if (obj == ui.pushButton_Tools)
  {
    if (ev->type() == QEvent::MouseButtonRelease)
    {
      QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(ev);
      if ( mouseEvent->button() == Qt::LeftButton )
      {
        QMenu* menu = new QMenu(tr("Options"), ui.pushButton_Tools);
        menu->addActions(ui.pushButton_Tools->actions());
        menu->move( QPoint( ui.pushButton_Tools->x(), ui.pushButton_Tools->y() + 23 ) );
        menu->exec();
        delete menu;

        ui.pushButton_Tools->setDown(false);

        return true;
      }	
    }
  }
  else if( obj == ui.pushButton_ImageOrientation)
  {
    if (ev->type() == QEvent::MouseButtonRelease && ui.pushButton_ImageOrientation->isEnabled())
    {
      QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(ev);
      if ( mouseEvent->button() == Qt::LeftButton )
      {
        QMenu* menu = new QMenu(tr("Actions"), ui.pushButton_ImageOrientation);
        menu->addActions(ui.pushButton_ImageOrientation->actions());
        menu->move( QPoint( ui.pushButton_ImageOrientation->x(), ui.pushButton_ImageOrientation->y() + 23 ) );
        menu->exec();
        delete menu;

        ui.pushButton_ImageOrientation->setDown(false);

        return true;
      }	
    }
  }

  // Pass the event on to the parent class
  return QWidget::eventFilter( obj, ev );
}

//-----------------------------------------------------------------------------

void fCalMainWindow::DumpBuffers()
{
  LOG_TRACE("fCalMainWindow::DumpBuffers");

  // Directory open dialog for selecting directory to save the buffers into 
  QString dirName = QFileDialog::getExistingDirectory(NULL, QString( tr( "Open output directory for buffer dump files" ) ), vtkPlusConfig::GetInstance()->GetOutputDirectory());

  if ( (dirName.isNull()) || (m_ObjectVisualizer->DumpBuffersToDirectory(dirName.toAscii().data()) != PLUS_SUCCESS) )
  {
    LOG_ERROR("Writing raw buffers into files failed (output directory: " << dirName.toAscii().data() << ")!");
  }

  LOG_INFO("Raw buffers dumped into directory '" << dirName.toAscii().data() << "'");
}

//-----------------------------------------------------------------------------

void fCalMainWindow::SetOrientationMRightFUp()
{
  LOG_TRACE("fCalMainWindow::SetOrientationMFRightUp");

  this->GetObjectVisualizer()->Modify2DImageOrientation(MF_SCREEN_RIGHT_UP);
}

//-----------------------------------------------------------------------------

void fCalMainWindow::SetOrientationMLeftFUp()
{
  LOG_TRACE("fCalMainWindow::SetOrientationMFLeftUp");

  this->GetObjectVisualizer()->Modify2DImageOrientation(MF_SCREEN_LEFT_UP);
}

//-----------------------------------------------------------------------------

void fCalMainWindow::SetOrientationMRightFDown()
{
  LOG_TRACE("fCalMainWindow::SetOrientationMFRightDown");

  this->GetObjectVisualizer()->Modify2DImageOrientation(MF_SCREEN_RIGHT_DOWN);
}

//-----------------------------------------------------------------------------

void fCalMainWindow::SetOrientationMLeftFDown()
{
  LOG_TRACE("fCalMainWindow::SetOrientationMFLeftDown");

  this->GetObjectVisualizer()->Modify2DImageOrientation(MF_SCREEN_LEFT_DOWN);
}

//-----------------------------------------------------------------------------

void fCalMainWindow::EnableOrientationMarkers()
{
  LOG_TRACE("fCalMainWindow::EnableOrientationMarkers");
  if( m_ShowOrientationMarkerAction->isChecked() )
  {
    if( this->GetObjectVisualizer()->EnableOrientationMarkers(true) != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to enable orientation markers in vtkObjectVisualiser.");
    }
  }
  else
  {
    if( this->GetObjectVisualizer()->EnableOrientationMarkers(false) != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to disable orientation markers in vtkObjectVisualiser.");
    }
  }
}

//-----------------------------------------------------------------------------

void fCalMainWindow::SaveDeviceSetConfiguration()
{
  LOG_TRACE("fCalMainWindow::SaveDeviceSetConfiguration");

  if (vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData() == NULL)
  {
    LOG_ERROR("Failed to save device set configuration, because it is missing. Connect to a device set first!");
    return;
  }

  ConfigFileSaverDialog* configSaverDialog = new ConfigFileSaverDialog(this);
  configSaverDialog->exec();

  delete configSaverDialog;
}

//-----------------------------------------------------------------------------

void fCalMainWindow::ShowDevicesToggled(bool aOn)
{
  LOG_TRACE("fCalMainWindow::ShowDevicesToggled(" << (aOn?"true":"false") << ")"); 

  m_ShowDevices = aOn;

  if (m_ShowDevices == true)
  {
    if (m_ObjectVisualizer->IsExistingTransform(m_ImageCoordinateFrame.c_str(), m_ProbeCoordinateFrame.c_str()) == PLUS_SUCCESS)
    {
      // Show image
      vtkDisplayableObject* imageDisplayable = NULL;
      if (m_ObjectVisualizer->GetDisplayableObject(m_ImageCoordinateFrame.c_str(), imageDisplayable) == PLUS_SUCCESS)
      {
        imageDisplayable->DisplayableOn();
      }
    }

    m_ObjectVisualizer->HideAll();
    m_ObjectVisualizer->EnableImageMode(false);
    m_ObjectVisualizer->ShowAllObjects(true);
    m_ObjectVisualizer->ShowInput(m_ShowPoints);
    m_ObjectVisualizer->ShowResult(m_ShowPoints);
    m_ObjectVisualizer->GetCanvasRenderer()->ResetCamera();

    SetImageManipulationEnabled(false);
    m_ObjectVisualizer->EnableOrientationMarkers(false);
  }
  else
  {
    m_ToolboxList[m_ActiveToolbox]->SetDisplayAccordingToState();
  }

  LOG_INFO("Show devices " << (aOn?"enabled":"disabled"));
}

//-----------------------------------------------------------------------------

bool fCalMainWindow::IsOrientationMarkersEnabled()
{
  return m_ShowOrientationMarkerAction->isChecked();
}
