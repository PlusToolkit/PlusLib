/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#include "CaptureClientWindow.h"
#include "CaptureControlWidget.h"
#include "QCustomAction.h"
#include "vtkDataCollector.h"
#include "vtkPlusChannel.h"
#include "vtkPlusDevice.h"
#include "vtkVirtualDiscCapture.h"
#include "vtkVisualizationController.h"
#include "vtkXMLUtilities.h"
#include <QDialog>
#include <QMenu>
#include <QMouseEvent>
#include <QTimer>

//-----------------------------------------------------------------------------
CaptureClientWindow::CaptureClientWindow(QWidget *parent, Qt::WFlags flags)
: QMainWindow(parent, flags)
, m_DataCollector(NULL)
, m_SelectedChannel(NULL)
, m_VisualizationController(NULL)
, m_UiRefreshTimer(new QTimer(this))
{
  // Set up UI
  ui.setupUi(this);

  connect( ui.deviceSetSelectorWidget, SIGNAL( ConnectToDevicesByConfigFileInvoked(std::string) ), this, SLOT( ConnectToDevicesByConfigFile(std::string) ) );
  connect( m_UiRefreshTimer, SIGNAL( timeout() ), this, SLOT( UpdateGUI() ) );
  connect( ui.startAllButton, SIGNAL(clicked()), this, SLOT(StartAll()));
  connect( ui.stopAllButton, SIGNAL(clicked()), this, SLOT(StopAll()));

  // Create visualizer
  m_VisualizationController = vtkVisualizationController::New();
  m_VisualizationController->SetCanvas(ui.canvas);

  // Hide it until we have something to show
  ui.canvas->setVisible(false);

  ui.channelSelectButton->installEventFilter(this);
  ui.channelSelectButton->setEnabled(false);
  ui.startAllButton->setEnabled(false);
  ui.stopAllButton->setEnabled(false);

  m_UiRefreshTimer->start(50);
}

//-----------------------------------------------------------------------------
CaptureClientWindow::~CaptureClientWindow()
{
  m_VisualizationController->SetDataCollector(NULL);
  DELETE_IF_NOT_NULL(m_VisualizationController);
  DELETE_IF_NOT_NULL(m_DataCollector);
}

//-----------------------------------------------------------------------------
void CaptureClientWindow::ConnectToDevicesByConfigFile(std::string aConfigFile)
{
  LOG_TRACE("CaptureClientWindow::ConnectToDevicesByConfigFile");

  QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));

  // If not empty, then try to connect; empty parameter string means disconnect
  if (STRCASECMP(aConfigFile.c_str(), "") != 0)
  {
    // Read configuration
    vtkSmartPointer<vtkXMLDataElement> configRootElement = vtkSmartPointer<vtkXMLDataElement>::Take(vtkXMLUtilities::ReadElementFromFile(aConfigFile.c_str()));
    if (configRootElement == NULL)
    {  
      LOG_ERROR("Unable to read configuration from file " << aConfigFile);

      ui.deviceSetSelectorWidget->SetConnectionSuccessful(false);
      ui.toolStateDisplayWidget->InitializeTools(NULL, false);
      QApplication::restoreOverrideCursor();

      return;
    }

    LOG_INFO("Device set configuration is read from file: " << aConfigFile);
    std::ostringstream xmlFileContents; 
    PlusCommon::PrintXML(xmlFileContents, vtkIndent(1), configRootElement);
    LOG_DEBUG("Device set configuration file contents: " << std::endl << xmlFileContents.str());    

    vtkPlusConfig::GetInstance()->SetDeviceSetConfigurationData(configRootElement); 

    // If connection has been successfully created then start data collection
    if (!ui.deviceSetSelectorWidget->GetConnectionSuccessful())
    {
      LOG_INFO("Connect to devices"); 

      // Disable main window
      this->setEnabled(false);

      // Create dialog
      QDialog* connectDialog = new QDialog(this, Qt::Dialog);
      connectDialog->setMinimumSize(QSize(360,80));
      connectDialog->setWindowTitle(tr("Capture Client"));

      QLabel* connectLabel = new QLabel(QString("Connecting to devices, please wait..."), connectDialog);
      connectLabel->setFont(QFont("SansSerif", 16));

      QHBoxLayout* layout = new QHBoxLayout();
      layout->addWidget(connectLabel);

      connectDialog->setLayout(layout);
      connectDialog->show();

      QApplication::processEvents();

      // Connect to devices
      if (this->StartDataCollection() != PLUS_SUCCESS)
      {
        LOG_ERROR("Unable to start collecting data!");
        ui.deviceSetSelectorWidget->SetConnectionSuccessful(false);
        ui.toolStateDisplayWidget->InitializeTools(NULL, false);

        ui.canvas->setVisible(false);
        ui.channelSelectButton->setEnabled(false);
      }
      else
      {
        DeviceCollection aCollection;
        if( m_DataCollector->GetDevices(aCollection) != PLUS_SUCCESS )
        {
          LOG_ERROR("Unable to load the list of devices.");
          return;
        }

        // Read configuration
        if (ReadConfiguration(vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData()) != PLUS_SUCCESS)
        {
          LOG_ERROR("Failed to read fCal configuration");
        }

        // Allow object visualizer to load anything it needs
        m_VisualizationController->ReadConfiguration(vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData());
        m_VisualizationController->SetVisualizationMode(vtkVisualizationController::DISPLAY_MODE_2D);

        // Successful connection
        ui.deviceSetSelectorWidget->SetConnectionSuccessful(true);

        vtkPlusConfig::GetInstance()->SaveApplicationConfigurationToFile();

        this->BuildChannelMenu();
        ui.channelSelectButton->setEnabled(m_3DActionList.size() > 0);
        ui.startAllButton->setEnabled(true);
        ui.stopAllButton->setEnabled(true);

        if (ui.toolStateDisplayWidget->InitializeTools(m_SelectedChannel, true))
        {
          ui.toolStateDisplayWidget->setMinimumHeight(ui.toolStateDisplayWidget->GetDesiredHeight());
          ui.toolStateDisplayWidget->setMaximumHeight(ui.toolStateDisplayWidget->GetDesiredHeight());
        }

        ui.canvas->setVisible(true);
      }

      this->ConfigureCaptureWidgets();

      // Close dialog
      connectDialog->done(0);
      connectDialog->hide();
      delete connectDialog;
    }

    this->setEnabled(true);
  }
  else // Disconnect
  {
    ui.canvas->setVisible(false);
    ui.channelSelectButton->setEnabled(false);
    ui.startAllButton->setEnabled(false);
    ui.stopAllButton->setEnabled(false);
    m_DataCollector->Disconnect();

    this->ConfigureCaptureWidgets();

    this->m_VisualizationController->Reset();
    this->m_VisualizationController->ClearTransformRepository();
    ui.deviceSetSelectorWidget->SetConnectionSuccessful(false);
    ui.deviceSetSelectorWidget->ShowResetTrackerButton(false);
    ui.toolStateDisplayWidget->InitializeTools(NULL, false);
  }

  QApplication::restoreOverrideCursor();
}

//-----------------------------------------------------------------------------
PlusStatus CaptureClientWindow::StartDataCollection()
{
  if( this->m_DataCollector != NULL )
  {
    m_VisualizationController->SetDataCollector(NULL);
    m_VisualizationController->AssignDataCollector(NULL);
    DELETE_IF_NOT_NULL(this->m_DataCollector);
  }

  this->m_DataCollector = vtkDataCollector::New();

  if (this->m_DataCollector->ReadConfiguration(vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData()) != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }

  if (this->m_DataCollector->Connect() != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }

  if (this->m_DataCollector->Start() != PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }

  if (! this->m_DataCollector->GetConnected())
  {
    LOG_ERROR("Unable to initialize DataCollector!"); 
    return PLUS_FAIL;
  }

  vtkPlusDevice* aDevice = *(m_DataCollector->GetDeviceConstIteratorBegin());
  if( aDevice == NULL )
  {
    LOG_ERROR("No device connected, can't select a device to visualize.");
    return PLUS_FAIL;
  }
  m_SelectedChannel = *(aDevice->GetOutputChannelsStart());
  if( m_SelectedChannel == NULL )
  {
    LOG_ERROR("Device " << aDevice->GetDeviceId() << " has no channels to visualize.");
    return PLUS_FAIL;
  }
  m_VisualizationController->SetDataCollector(m_DataCollector);
  this->ChannelSelected(m_SelectedChannel->GetOwnerDevice(), m_SelectedChannel);
  m_VisualizationController->AssignDataCollector(m_DataCollector);

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus CaptureClientWindow::ReadConfiguration( vtkXMLDataElement* aConfig )
{
  // TODO : any reading that needs to be done

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus CaptureClientWindow::ConfigureCaptureWidgets()
{
  for( std::vector<CaptureControlWidget*>::iterator it = m_CaptureWidgets.begin(); it != m_CaptureWidgets.end(); ++it )
  {
    ui.captureWidgetGrid->removeWidget(*it);
    delete *it;
  }
  m_CaptureWidgets.clear();

  if( m_DataCollector->GetConnected() )
  {
    DeviceCollection aCollection;
    if( m_DataCollector->GetDevices(aCollection) != PLUS_SUCCESS )
    {
      LOG_ERROR("Unable to retrieve list of devices.");
      return PLUS_FAIL;
    }

    for( DeviceCollectionConstIterator it = aCollection.begin(); it != aCollection.end(); ++it)
    {
      vtkPlusDevice* aDevice = *it;
      if( dynamic_cast<vtkVirtualDiscCapture*>(aDevice) != NULL )
      {
        vtkVirtualDiscCapture* capDevice = dynamic_cast<vtkVirtualDiscCapture*>(aDevice);
        CaptureControlWidget* aWidget = new CaptureControlWidget(NULL);
        aWidget->SetCaptureDevice(*capDevice);
        ui.captureWidgetGrid->addWidget(aWidget);
        m_CaptureWidgets.push_back(aWidget);
      }
    }
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
void CaptureClientWindow::BuildChannelMenu()
{
  for( std::vector<QCustomAction*>::iterator it = m_3DActionList.begin(); it != m_3DActionList.end(); ++it )
  {
    QCustomAction* action = *it;
    disconnect(action, SIGNAL(triggered()));
    delete(action);
  }
  m_3DActionList.clear();

  DeviceCollection aCollection;
  if( this->m_VisualizationController == NULL || this->m_VisualizationController->GetDataCollector() == NULL || 
    !this->m_VisualizationController->GetDataCollector()->GetConnected() || this->m_VisualizationController->GetDataCollector()->GetDevices(aCollection) != PLUS_SUCCESS )
  {
    // Data collector might be disconnected
    return;
  }

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
    for( ChannelContainerIterator channelIter = device->GetOutputChannelsStart(); channelIter != device->GetOutputChannelsEnd(); ++channelIter )
    {
      vtkPlusChannel* aChannel = *channelIter;
      std::stringstream ss;
      ss << device->GetDeviceId() << " : " << aChannel->GetChannelId();
      QCustomAction* action = new QCustomAction(QString::fromAscii(ss.str().c_str()), ui.channelSelectButton, false, device, aChannel);
      action->setCheckable(true);
      action->setDisabled(numChannels == 1);
      vtkPlusChannel* currentChannel(NULL);
      action->setChecked(this->GetSelectedChannel() == aChannel);
      connect(action, SIGNAL(triggered()), action, SLOT(activated()));
      connect(action, SIGNAL(channelSelected(vtkPlusDevice*, vtkPlusChannel*)), this, SLOT(ChannelSelected(vtkPlusDevice*, vtkPlusChannel*)));
      m_3DActionList.push_back(action);
    }
  }
}

//-----------------------------------------------------------------------------
bool CaptureClientWindow::eventFilter(QObject *obj, QEvent *ev)
{
  if (ev->type() == QEvent::MouseButtonRelease)
  {
    QPushButton *button = static_cast<QPushButton*>(obj);
    QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(ev);
    if ( mouseEvent->button() == Qt::LeftButton )
    {
      QMenu* menu = NULL;
      if( obj == ui.channelSelectButton )
      {
        if( !ui.channelSelectButton->isEnabled() )
        {
          return true;
        }
        menu = new QMenu(tr("Actions"), ui.channelSelectButton);

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
        menu->move( QPoint( ui.channelSelectButton->x() - 40, ui.channelSelectButton->y() - (20*m_3DActionList.size()) ) );
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
void CaptureClientWindow::SetSelectedChannel( vtkPlusChannel& aChannel )
{
  m_SelectedChannel = &aChannel;
  this->m_VisualizationController->SetSelectedChannel(&aChannel);
}

//-----------------------------------------------------------------------------
void CaptureClientWindow::ChannelSelected( vtkPlusDevice* aDevice, vtkPlusChannel* aChannel )
{
  LOG_TRACE("CaptureClientWindow::ChannelSelected(" << aDevice->GetDeviceId() << ", channel: " << aChannel->GetChannelId() << ")");

  if( this->m_VisualizationController != NULL && this->m_VisualizationController->GetDataCollector() != NULL )
  {
    this->SetSelectedChannel(*aChannel);
  }
  if( aChannel->GetVideoDataAvailable() )
  {
    this->m_VisualizationController->SetInput( aChannel->GetBrightnessOutput() );
  }
  else
  {
    this->m_VisualizationController->DisconnectInput();
  }

  this->BuildChannelMenu();
}

//-----------------------------------------------------------------------------
void CaptureClientWindow::resizeEvent(QResizeEvent* aEvent)
{
  LOG_TRACE("CaptureClientWindow::resizeEvent");

  if( m_VisualizationController != NULL )
  {
    m_VisualizationController->resizeEvent(aEvent);
  }
}

//-----------------------------------------------------------------------------
void CaptureClientWindow::UpdateGUI()
{
  //LOG_TRACE("CaptureClientWindow::UpdateGUI");

  // We do not update the GUI when a mouse button is pressed
  if (QApplication::mouseButtons() != Qt::NoButton)
  {
    return;
  }

  if( m_VisualizationController != NULL && m_VisualizationController->GetDataCollector() != NULL && m_VisualizationController->GetDataCollector()->GetConnected() )
  {
    ui.canvas->update();
  }
}

//-----------------------------------------------------------------------------
void CaptureClientWindow::StartAll()
{
  for( std::vector<CaptureControlWidget*>::iterator it = m_CaptureWidgets.begin(); it != m_CaptureWidgets.end(); ++it )
  {
    CaptureControlWidget* aWidget = *it;

    aWidget->GetCaptureDevice()->SetEnableCapturing(true);
  }
}

//-----------------------------------------------------------------------------
void CaptureClientWindow::StopAll()
{
  for( std::vector<CaptureControlWidget*>::iterator it = m_CaptureWidgets.begin(); it != m_CaptureWidgets.end(); ++it )
  {
    CaptureControlWidget* aWidget = *it;

    aWidget->GetCaptureDevice()->SetEnableCapturing(false);
  }
}
