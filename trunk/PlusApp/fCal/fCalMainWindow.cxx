/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#include "CapturingToolbox.h"
#include "ConfigFileSaverDialog.h"
#include "ConfigurationToolbox.h"
#include "PhantomRegistrationToolbox.h"
#include "QCustomAction.h"
#include "SpatialCalibrationToolbox.h"
#include "StatusIcon.h"
#include "StylusCalibrationToolbox.h"
#include "TemporalCalibrationToolbox.h"
#include "VolumeReconstructionToolbox.h"
#include "fCalMainWindow.h"
#include "vtkRenderWindow.h"
#include "vtkVirtualDiscCapture.h"
#include "vtkVirtualMixer.h"
#include "vtkVisualizationController.h"
#include <QFileDialog>
#include <QLabel>
#include <QMenu>
#include <QProgressBar>
#include <QTimer>

//-----------------------------------------------------------------------------

fCalMainWindow::fCalMainWindow(QWidget *parent, Qt::WFlags flags)
: QMainWindow(parent, flags)
, m_StatusBarLabel(NULL)
, m_StatusBarProgress(NULL)
, m_LockedTabIndex(-1)
, m_ActiveToolbox(ToolboxType_Undefined)
, m_VisualizationController(NULL)
, m_StatusIcon(NULL)
, m_ImageCoordinateFrame("")
, m_ProbeCoordinateFrame("")
, m_ReferenceCoordinateFrame("")
, m_TransducerOriginCoordinateFrame("")
, m_TransducerOriginPixelCoordinateFrame("")
, m_PhantomModelId(NULL)
, m_ShowPoints(false)
, m_ForceShowAllDevicesIn3D(false)
, m_ShowOrientationMarkerAction(NULL)
, m_ShowROIAction(NULL)
, m_Show3DObjectsAction(NULL)
, m_ShowPhantomModelAction(NULL)
, m_SelectedChannel(NULL)
{
  // Set up UI
  ui.setupUi(this);

  // Maximize window
  this->setWindowState(this->windowState() ^ Qt::WindowMaximized);

  this->Set3DManipulationMenuEnabled(false);
  this->SetImageManipulationMenuEnabled(false);
}

//-----------------------------------------------------------------------------

fCalMainWindow::~fCalMainWindow()
{
  if (m_VisualizationController != NULL)
  {
    m_VisualizationController->Delete();
    m_VisualizationController = NULL;
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
    m_ShowOrientationMarkerAction = NULL;
  }

  this->SetPhantomModelId(NULL);
  this->SetStylusModelId(NULL);
  this->SetTransducerModelId(NULL);
  this->SetImageObjectId(NULL);

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
  QCustomAction* mfRightUpAction = new QCustomAction("Marked Right, Far Up", ui.pushButton_ImageOrientation);
  connect(mfRightUpAction, SIGNAL(triggered()), this, SLOT(SetOrientationMRightFUp()));
  m_ImageManipulationActionList.push_back(mfRightUpAction);
  QCustomAction* mfLeftUpAction = new QCustomAction("Marked Left, Far Up", ui.pushButton_ImageOrientation);
  connect(mfLeftUpAction, SIGNAL(triggered()), this, SLOT(SetOrientationMLeftFUp()));
  m_ImageManipulationActionList.push_back(mfLeftUpAction);
  QCustomAction* mfRightDownAction = new QCustomAction("Marked Right, Far Down", ui.pushButton_ImageOrientation);
  connect(mfRightDownAction, SIGNAL(triggered()), this, SLOT(SetOrientationMRightFDown()));
  m_ImageManipulationActionList.push_back(mfRightDownAction);
  QCustomAction* mfLeftDownAction = new QCustomAction("Marked Left, Far Down", ui.pushButton_ImageOrientation);
  connect(mfLeftDownAction, SIGNAL(triggered()), this, SLOT(SetOrientationMLeftFDown()));
  m_ImageManipulationActionList.push_back(mfLeftDownAction);
  m_ShowOrientationMarkerAction = new QCustomAction("Show Orientation Markers", ui.pushButton_ImageOrientation);
  connect(m_ShowOrientationMarkerAction, SIGNAL(triggered()), this, SLOT(EnableOrientationMarkers()));
  m_ShowOrientationMarkerAction->setCheckable(true);
  m_ImageManipulationActionList.push_back(m_ShowOrientationMarkerAction);
  QCustomAction* separator = new QCustomAction("", NULL, true);
  m_ImageManipulationActionList.push_back(separator);
  m_ShowROIAction = new QCustomAction("Show Region of Interest", ui.pushButton_ImageOrientation);
  connect(m_ShowROIAction, SIGNAL(triggered()), this, SLOT(EnableROI()));
  m_ShowROIAction->setCheckable(true);
  m_ImageManipulationActionList.push_back(m_ShowROIAction);

  BuildChannelMenu();

  // Declare this class as the event handler
  ui.pushButton_ShowDevices->installEventFilter(this);

  // Declare this class as the event handler
  ui.pushButton_ImageOrientation->installEventFilter(this);

  // Create visualizer
  m_VisualizationController = vtkVisualizationController::New();
  m_VisualizationController->SetCanvas(ui.canvas);

  // Hide it until we have something to show
  ui.canvas->setVisible(false);

  // Create toolboxes
  CreateToolboxes();

  // Set up status bar (message and progress bar)
  SetupStatusBar();

  // Make connections
  connect( ui.toolbox, SIGNAL( currentChanged(int) ), this, SLOT( CurrentToolboxChanged(int) ) );
  connect( ui.pushButton_SaveConfiguration, SIGNAL( clicked() ), this, SLOT( SaveDeviceSetConfiguration() ) );
  connect( m_UiRefreshTimer, SIGNAL( timeout() ), this, SLOT( UpdateGUI() ) );

  // Tell the object visualizer to show orientation markers
  m_ShowOrientationMarkerAction->setChecked(true);

  // Initialize default toolbox widget
  CurrentToolboxChanged(ui.toolbox->currentIndex());

  // Start timer
  m_UiRefreshTimer->start(50);
}

//-----------------------------------------------------------------------------

void fCalMainWindow::CreateToolboxes()
{
  LOG_TRACE("fCalMainWindow::CreateToolboxes");

  // Resize toolbox list to the number of toolboxes
  m_ToolboxList.resize(7);

  // Configuration widget
  ConfigurationToolbox* configurationToolbox = new ConfigurationToolbox(this);
  if (configurationToolbox != NULL)
  {
    QGridLayout* grid = new QGridLayout(ui.toolbox_Configuration);
    grid->setColumnStretch(1, 1);
    grid->setRowStretch(1, 1);
    grid->setMargin(3);
    grid->setSpacing(0);
    grid->addWidget(configurationToolbox);
    ui.toolbox_Configuration->setLayout(grid);
  }
  m_ToolboxList[ToolboxType_Configuration] = configurationToolbox;

  // Capturing widget
  CapturingToolbox* capturingToolbox = new CapturingToolbox(this);
  if (capturingToolbox != NULL)
  {
    QGridLayout* grid = new QGridLayout(ui.toolbox_Capturing);
    grid->setColumnStretch(1, 1);
    grid->setRowStretch(1, 1);
    grid->setMargin(5);
    grid->setSpacing(0);
    grid->addWidget(capturingToolbox);
    ui.toolbox_Capturing->setLayout(grid);
  }
  m_ToolboxList[ToolboxType_Capturing] = capturingToolbox;

  // Stylus calibration widget
  StylusCalibrationToolbox* stylusCalibrationToolbox = new StylusCalibrationToolbox(this);
  if (stylusCalibrationToolbox != NULL)
  {
    QGridLayout* grid = new QGridLayout(ui.toolbox_StylusCalibration);
    grid->addWidget(stylusCalibrationToolbox);
    ui.toolbox_StylusCalibration->setLayout(grid);
  }
  m_ToolboxList[ToolboxType_StylusCalibration] = stylusCalibrationToolbox;

  // Phantom registration widget
  PhantomRegistrationToolbox* phantomRegistrationToolbox = new PhantomRegistrationToolbox(this);
  if (phantomRegistrationToolbox != NULL)
  {
    QGridLayout* grid = new QGridLayout(ui.toolbox_PhantomRegistration);
    grid->setColumnStretch(1, 1);
    grid->setRowStretch(1, 1);
    grid->setMargin(5);
    grid->setSpacing(0);
    grid->addWidget(phantomRegistrationToolbox);
    ui.toolbox_PhantomRegistration->setLayout(grid);
  }
  m_ToolboxList[ToolboxType_PhantomRegistration] = phantomRegistrationToolbox;

  // Temporal calibration widget
  TemporalCalibrationToolbox* temporalCalibrationToolbox = new TemporalCalibrationToolbox(this);
  if (temporalCalibrationToolbox != NULL)
  {
    QGridLayout* grid = new QGridLayout(ui.toolbox_TemporalCalibration);
    grid->setColumnStretch(1, 1);
    grid->setRowStretch(1, 1);
    grid->setMargin(5);
    grid->setSpacing(0);
    grid->addWidget(temporalCalibrationToolbox);
    ui.toolbox_TemporalCalibration->setLayout(grid);
  }
  m_ToolboxList[ToolboxType_TemporalCalibration] = temporalCalibrationToolbox;

  // Spatial calibration widget
  SpatialCalibrationToolbox* spatialCalibrationToolbox = new SpatialCalibrationToolbox(this);
  if (spatialCalibrationToolbox != NULL)
  {
    QGridLayout* grid = new QGridLayout(ui.toolbox_SpatialCalibration);
    grid->setColumnStretch(1, 1);
    grid->setRowStretch(1, 1);
    grid->setMargin(5);
    grid->setSpacing(0);
    grid->setName("");
    grid->addWidget(spatialCalibrationToolbox);
    ui.toolbox_SpatialCalibration->setLayout(grid);
  }
  m_ToolboxList[ToolboxType_SpatialCalibration] = spatialCalibrationToolbox;

  // Volume reconstruction widget
  VolumeReconstructionToolbox* volumeReconstructionToolbox = new VolumeReconstructionToolbox(this);
  if (volumeReconstructionToolbox != NULL)
  {
    QGridLayout* grid = new QGridLayout(ui.toolbox_VolumeReconstruction);
    grid->setColumnStretch(1, 1);
    grid->setRowStretch(1, 1);
    grid->setMargin(5);
    grid->setSpacing(0);
    grid->addWidget(volumeReconstructionToolbox);
    ui.toolbox_VolumeReconstruction->setLayout(grid);
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

void fCalMainWindow::CurrentToolboxChanged(int aToolboxIndex)
{
  LOG_TRACE("fCalMainWindow::CurrentToolboxChanged(" << aToolboxIndex << ")");

  // Initialize new toolbox
  QString currentToolboxText = ui.toolbox->itemText( aToolboxIndex );
  if (currentToolboxText == QString("Configuration"))
  {
    m_ActiveToolbox = ToolboxType_Configuration;
  }
  else if (currentToolboxText == QString("Stylus calibration"))
  {
    m_ActiveToolbox = ToolboxType_StylusCalibration;
  }
  else if (currentToolboxText == QString("Phantom registration"))
  {
    m_ActiveToolbox = ToolboxType_PhantomRegistration;
  }
  else if (currentToolboxText == QString("Temporal calibration"))
  {
    m_ActiveToolbox = ToolboxType_TemporalCalibration;
  }
  else if (currentToolboxText == QString("Spatial calibration"))
  {
    m_ActiveToolbox = ToolboxType_SpatialCalibration;
  }
  else if (currentToolboxText == QString("Capturing"))
  {
    m_ActiveToolbox = ToolboxType_Capturing;
  }
  else if (currentToolboxText == QString("Volume reconstruction"))
  {
    m_ActiveToolbox = ToolboxType_VolumeReconstruction;
  }
  else
  {
    LOG_ERROR("No toolbox with this title found!");
    return;
  }

  m_ToolboxList[m_ActiveToolbox]->OnActivated();
  m_ToolboxList[m_ActiveToolbox]->SetDisplayAccordingToState();

  LOG_INFO("Toolbox changed to " << currentToolboxText.toLatin1().constData());
}

//-----------------------------------------------------------------------------

void fCalMainWindow::SetToolboxesEnabled(bool aEnabled)
{
  LOG_TRACE("fCalMainWindow::SetToolboxesEnabled(" << (aEnabled?"true":"false") << ")");

  if (aEnabled) {
    m_LockedTabIndex = -1;
    disconnect(ui.toolbox, SIGNAL(currentChanged(int)), this, SLOT(ChangeBackToolbox(int)) );
    connect(ui.toolbox, SIGNAL(currentChanged(int)), this, SLOT(CurrentToolboxChanged(int)) );
  } else {
    m_LockedTabIndex = ui.toolbox->currentIndex();
    disconnect(ui.toolbox, SIGNAL(currentChanged(int)), this, SLOT(CurrentToolboxChanged(int)) );
    connect(ui.toolbox, SIGNAL(currentChanged(int)), this, SLOT(ChangeBackToolbox(int)) );
  }
}

//-----------------------------------------------------------------------------

void fCalMainWindow::SetImageManipulationMenuEnabled( bool aEnabled )
{
  LOG_TRACE("fCalMainWindow::SetImageManipulationEnabled(" << (aEnabled?"true":"false") << ")");

  ui.pushButton_ImageOrientation->setEnabled(aEnabled);
}

//-----------------------------------------------------------------------------

void fCalMainWindow::ChangeBackToolbox(int aTabIndex)
{
  LOG_TRACE("fCalMainWindow::ChangeBackToolbox(" << aTabIndex << ")");

  ui.toolbox->blockSignals(true);
  ui.toolbox->setCurrentIndex(m_LockedTabIndex);
  ui.toolbox->blockSignals(false);
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

  ui.canvas->update();
}

//-----------------------------------------------------------------------------

void fCalMainWindow::resizeEvent(QResizeEvent* aEvent)
{
  LOG_TRACE("fCalMainWindow::resizeEvent");

  if( m_VisualizationController != NULL )
  {
    m_VisualizationController->resizeEvent(aEvent);
  }
}

//-----------------------------------------------------------------------------

void fCalMainWindow::ResetAllToolboxes()
{
  LOG_TRACE("fCalMainWindow::ResetAllToolboxes");

  m_VisualizationController->HideAll();

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
  if (ev->type() == QEvent::MouseButtonRelease)
  {
    QPushButton *button = static_cast<QPushButton*>(obj);
    QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(ev);
    if ( mouseEvent->button() == Qt::LeftButton )
    {
      QMenu* menu = NULL;
      if (obj == ui.pushButton_Tools)
      {
        if( !ui.pushButton_Tools->isEnabled() )
        {
          return true;
        }
        else
        {
          menu = new QMenu(tr("Options"), ui.pushButton_Tools);
          menu->addActions(ui.pushButton_Tools->actions());
          menu->move( QPoint( ui.pushButton_Tools->x() + 10, ui.pushButton_Tools->y() + 33 ) );
        }
      }
      else if( obj == ui.pushButton_ImageOrientation )
      {
        if( !ui.pushButton_ImageOrientation->isEnabled() )
        {
          return true;
        }
        menu = new QMenu(tr("Orientations"), ui.pushButton_ImageOrientation);
        for( std::vector<QCustomAction*>::iterator it = m_ImageManipulationActionList.begin(); it != m_ImageManipulationActionList.end(); ++it )
        {
          QCustomAction* action = (*it);
          if( action->IsSeparator() )
          {
            menu->addSeparator();
          }
          else
          {
            menu->addAction(action);
          }
        }
        menu->move( QPoint( ui.pushButton_ImageOrientation->x() + 10, ui.pushButton_ImageOrientation->y() + 33 ) );
      }
      else
      {
        if( !ui.pushButton_ShowDevices->isEnabled() )
        {
          return true;
        }
        menu = new QMenu(tr("Devices"), ui.pushButton_ShowDevices);

        for( std::vector<QCustomAction*>::iterator it = m_3DActionList.begin(); it != m_3DActionList.end(); ++it )
        {
          QCustomAction* action = (*it);
          if( action->IsSeparator() )
          {
            menu->addSeparator();
          }
          else
          {
            menu->addAction(action);
          }
        }
        menu->move( QPoint( ui.pushButton_ShowDevices->x() + 10, ui.pushButton_ShowDevices->y() + 33 ) );
      }

      menu->exec();
      delete menu;

      button->setDown(false);

      return true;
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
  QString dirName = QFileDialog::getExistingDirectory(NULL, QString( tr( "Open output directory for buffer dump files" ) ), vtkPlusConfig::GetInstance()->GetOutputDirectory().c_str());

  if( dirName == "" )
  {
    // Cancel button hit, just cancel gracefully
    return;
  }

  if ( (dirName.isNull()) || (m_VisualizationController->DumpBuffersToDirectory(dirName.toLatin1().constData()) != PLUS_SUCCESS) )
  {
    LOG_ERROR("Writing raw buffers into files failed (output directory: " << dirName.toLatin1().constData() << ")!");
  }

  LOG_INFO("Raw buffers dumped into directory '" << dirName.toLatin1().constData() << "'");
}

//-----------------------------------------------------------------------------

void fCalMainWindow::SetOrientationMRightFUp()
{
  LOG_TRACE("fCalMainWindow::SetOrientationMFRightUp");

  this->GetVisualizationController()->SetScreenRightDownAxesOrientation(US_IMG_ORIENT_MN);
}

//-----------------------------------------------------------------------------

void fCalMainWindow::SetOrientationMLeftFUp()
{
  LOG_TRACE("fCalMainWindow::SetOrientationMFLeftUp");

  this->GetVisualizationController()->SetScreenRightDownAxesOrientation(US_IMG_ORIENT_UN);
}

//-----------------------------------------------------------------------------

void fCalMainWindow::SetOrientationMRightFDown()
{
  LOG_TRACE("fCalMainWindow::SetOrientationMFRightDown");

  this->GetVisualizationController()->SetScreenRightDownAxesOrientation(US_IMG_ORIENT_MF);
}

//-----------------------------------------------------------------------------

void fCalMainWindow::SetOrientationMLeftFDown()
{
  LOG_TRACE("fCalMainWindow::SetOrientationMFLeftDown");

  this->GetVisualizationController()->SetScreenRightDownAxesOrientation(US_IMG_ORIENT_UF);
}

//-----------------------------------------------------------------------------

void fCalMainWindow::EnableOrientationMarkers()
{
  LOG_TRACE("fCalMainWindow::EnableOrientationMarkers");
  if( m_ShowOrientationMarkerAction->isChecked() )
  {
    if( this->GetVisualizationController()->ShowOrientationMarkers(true) != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to enable orientation markers in vtkObjectVisualiser.");
    }
  }
  else
  {
    if( this->GetVisualizationController()->ShowOrientationMarkers(false) != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to disable orientation markers in vtkObjectVisualiser.");
    }
  }
}

//-----------------------------------------------------------------------------

void fCalMainWindow::EnableROI()
{
  LOG_TRACE("fCalMainWindow::EnableROI()");
  if( m_ShowROIAction->isChecked() )
  {
    if( this->GetVisualizationController()->EnableROI(true) != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to enable region of interest in vtkObjectVisualiser.");
    }
  }
  else
  {
    if( this->GetVisualizationController()->EnableROI(false) != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to disable region of interest in vtkObjectVisualiser.");
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

  // Write the current state into the device set configuration XML
  GetVisualizationController()->WriteConfiguration(vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData());

  ConfigFileSaverDialog* configSaverDialog = new ConfigFileSaverDialog(this);
  configSaverDialog->exec();

  delete configSaverDialog;
}

//-----------------------------------------------------------------------------

void fCalMainWindow::ShowDevicesToggled()
{
  bool aOn = m_Show3DObjectsAction->isChecked();

  LOG_TRACE("fCalMainWindow::ShowDevicesToggled(" << (aOn?"true":"false") << ")"); 

  m_ForceShowAllDevicesIn3D = aOn;

  if( aOn )
  {
    // Force override, show 3D and ALLLLLL devices
    m_VisualizationController->SetVisualizationMode(vtkVisualizationController::DISPLAY_MODE_3D);
    m_VisualizationController->ShowAllObjects(true);
    m_VisualizationController->ShowInput(m_ShowPoints);
    m_VisualizationController->ShowResult(m_ShowPoints);

    SetImageManipulationMenuEnabled(!aOn);
  }
  else
  {
    // Let the toolbox restore its default state
    m_ToolboxList[m_ActiveToolbox]->SetDisplayAccordingToState();
  }

  LOG_INFO("Show devices " << (aOn?"enabled":"disabled"));
}


//-----------------------------------------------------------------------------

void fCalMainWindow::ShowPhantomToggled()
{
  bool aOn = m_ShowPhantomModelAction->isChecked();

  LOG_TRACE("fCalMainWindow::ShowPhantomToggled(" << (aOn?"true":"false") << ")"); 

  if( m_PhantomModelId != NULL )
  {
    if( m_VisualizationController->ShowObjectById(m_PhantomModelId, aOn) != PLUS_SUCCESS )
    {
      LOG_WARNING("Unable to hide/show the object: " << m_PhantomModelId);
    }
  }
}

//-----------------------------------------------------------------------------

bool fCalMainWindow::IsOrientationMarkersEnabled()
{
  return m_ShowOrientationMarkerAction->isChecked();
}

//-----------------------------------------------------------------------------

bool fCalMainWindow::IsForceShowDevicesEnabled()
{
  return m_ForceShowAllDevicesIn3D;
}

//-----------------------------------------------------------------------------

void fCalMainWindow::ResetShowDevices()
{
  ui.pushButton_ShowDevices->setChecked(false);
}

//-----------------------------------------------------------------------------

void fCalMainWindow::EnablePhantomToggle( bool aEnable )
{
  m_ShowPhantomModelAction->setDisabled(!aEnable);
}

//-----------------------------------------------------------------------------

void fCalMainWindow::Set3DManipulationMenuEnabled( bool aEnable)
{
  LOG_TRACE("fCalMainWindow::Set3DManipulationMenuEnabled(" << (aEnable?"true":"false") << ")");

  ui.pushButton_ShowDevices->setEnabled(aEnable);
}

//-----------------------------------------------------------------------------

void fCalMainWindow::BuildChannelMenu()
{
  for( std::vector<QCustomAction*>::iterator it = m_3DActionList.begin(); it != m_3DActionList.end(); ++it )
  {
    QCustomAction* action = *it;
    disconnect(action, SIGNAL(triggered()));
    delete(action);
  }
  m_3DActionList.clear();

  // Set up menu items for 3D manipulation menu
  m_Show3DObjectsAction = new QCustomAction("Show all devices / Show content for current toolbox", ui.pushButton_ShowDevices);
  connect(m_Show3DObjectsAction, SIGNAL(triggered()), this, SLOT(ShowDevicesToggled()));
  m_Show3DObjectsAction->setCheckable(true);
  m_ShowPhantomModelAction = new QCustomAction("Show phantom model", ui.pushButton_ShowDevices);
  connect(m_ShowPhantomModelAction, SIGNAL(triggered()), this, SLOT(ShowPhantomToggled()));
  m_ShowPhantomModelAction->setCheckable(true);
  m_ShowPhantomModelAction->setChecked(true);
  m_3DActionList.push_back(m_Show3DObjectsAction);
  QCustomAction* separator = new QCustomAction("", NULL, true);
  m_3DActionList.push_back(separator);
  m_3DActionList.push_back(m_ShowPhantomModelAction);

  DeviceCollection aCollection;
  if( this->GetVisualizationController() == NULL || this->GetVisualizationController()->GetDataCollector() == NULL || 
    !this->GetVisualizationController()->GetDataCollector()->GetConnected() || this->GetVisualizationController()->GetDataCollector()->GetDevices(aCollection) != PLUS_SUCCESS )
  {
    // Data collector might be disconnected
    return;
  }

  separator = new QCustomAction("", NULL, true);
  m_3DActionList.push_back(separator);

  // Determine total number of output channels
  int numChannels(0);
  for( DeviceCollectionIterator it = aCollection.begin(); it != aCollection.end(); ++it )
  {
    vtkPlusDevice* device = *it;
    numChannels += device->OutputChannelCount();
  }

  // now add an entry for each device
  for( DeviceCollectionIterator it = aCollection.begin(); it != aCollection.end(); ++it )
  {
    vtkPlusDevice* device = *it;
    if( dynamic_cast<vtkVirtualDiscCapture*>(device) != NULL )
    {
      continue;
    }
    for( ChannelContainerIterator channelIter = device->GetOutputChannelsStart(); channelIter != device->GetOutputChannelsEnd(); ++channelIter )
    {
      vtkPlusChannel* aChannel = *channelIter;
      std::stringstream ss;
      ss << device->GetDeviceId() << " : " << aChannel->GetChannelId();
      QCustomAction* action = new QCustomAction(QString::fromAscii(ss.str().c_str()), ui.pushButton_ShowDevices, false, aChannel);
      action->setCheckable(true);
      action->setDisabled(numChannels == 1);
      vtkPlusChannel* currentChannel(NULL);
      action->setChecked(this->GetSelectedChannel() == aChannel);
      connect(action, SIGNAL(triggered()), action, SLOT(activated()));
      connect(action, SIGNAL(channelSelected(vtkPlusChannel*)), this, SLOT(ChannelSelected(vtkPlusChannel*)));
      m_3DActionList.push_back(action);
    }
  }
}

//-----------------------------------------------------------------------------

void fCalMainWindow::ChannelSelected( vtkPlusChannel* aChannel )
{
  LOG_TRACE("fCalMainWindow::ChannelSelected(channel: " << aChannel->GetChannelId() << ")");

  if( aChannel == this->GetSelectedChannel() )
  {
    this->BuildChannelMenu();
    return;
  }

  if( this->GetVisualizationController() != NULL && this->GetVisualizationController()->GetDataCollector() != NULL )
  {
    this->SetSelectedChannel(*aChannel);
  }
  if( aChannel->GetVideoDataAvailable() && aChannel->GetBrightnessOutput() != NULL )
  {
    this->GetVisualizationController()->SetInput( aChannel->GetBrightnessOutput() );
  }
  else
  {
    LOG_ERROR("Unable to visualize video data due to missing data or image is not B-mode.");
    this->GetVisualizationController()->DisconnectInput();
    this->GetVisualizationController()->SetVisualizationMode(vtkVisualizationController::DISPLAY_MODE_NONE);
  }

  this->BuildChannelMenu();

  ConfigurationToolbox* aToolbox = dynamic_cast<ConfigurationToolbox*>(this->m_ToolboxList[ToolboxType_Configuration]);
  if( aToolbox != NULL )
  {
    aToolbox->ChannelChanged(*aChannel); 
  }
  else
  {
    LOG_ERROR("Expecting configuration toolbox at index: " << ToolboxType_Configuration << " but didn't find it.");
  }

  this->m_ToolboxList[m_ActiveToolbox]->Reset();
  this->m_ToolboxList[m_ActiveToolbox]->SetDisplayAccordingToState();
}

//-----------------------------------------------------------------------------

void fCalMainWindow::BuildChannelOwners( DeviceCollection devices )
{
  m_ChannelOwners.clear();

  for( DeviceCollectionIterator it = devices.begin(); it != devices.end(); ++it )
  {
    for( ChannelContainerIterator chanIt = (*it)->GetOutputChannelsStart(); chanIt != (*it)->GetOutputChannelsEnd(); ++chanIt )
    {
      m_ChannelOwners[*chanIt] = *it;
    }
  }
}

//-----------------------------------------------------------------------------

void fCalMainWindow::SetSelectedChannel( vtkPlusChannel& aChannel )
{
  m_SelectedChannel = &aChannel;

  this->GetVisualizationController()->SetSelectedChannel(&aChannel);

  this->m_ToolboxList[m_ActiveToolbox]->SetDisplayAccordingToState();
}
