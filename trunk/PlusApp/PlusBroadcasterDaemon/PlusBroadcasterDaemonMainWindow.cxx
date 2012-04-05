/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#include "PlusBroadcasterDaemonMainWindow.h"

#include "StatusIcon.h"
#include "ConfigFileSaverDialog.h"
#include "DeviceSetSelectorWidget.h"
#include "BroadcastedToolInfoWidget.h"
#include "BroadcastedToolEditorDialog.h"
#include "vtkDataCollector.h"
#include "vtkXMLUtilities.h" 
#include "vtkOpenIGTLinkBroadcaster.h"

#include <QTimer>
#include <QMenu>
#include <QLabel>
#include <QProgressBar>
#include <QFileDialog>



//-----------------------------------------------------------------------------
PlusBroadcasterDaemonMainWindow::PlusBroadcasterDaemonMainWindow(QWidget *parent, Qt::WFlags flags)
: QMainWindow(parent, flags)
{
  this->m_DataCollector = NULL; 
  this->m_Broadcaster = NULL; 
  this->m_BroadcastedImageInfo = NULL; 
  this->m_BroadcastingIntervalMs = 200; 


  // Set up UI
  this->ui.setupUi(this);

  // Connect signals and slots

  

  //****************************** Device selector widget ************************
  // Create and setup device set selector widget
  this->m_DeviceSetSelectorWidget = new DeviceSetSelectorWidget(this);
  connect( m_DeviceSetSelectorWidget, SIGNAL( ConnectToDevicesByConfigFileInvoked(std::string) ), this, SLOT( connectToDevicesByConfigFile(std::string) ) );

  // Setup device set selector widget
  this->m_DeviceSetSelectorWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  this->m_DeviceSetSelectorWidget->resize(this->width(), this->height());

  // Re-parse the directory to properly select the previously selected device set (it is called in its constructor but there are no connects set up yet)
  this->m_DeviceSetSelectorWidget->SetConfigurationDirectory(vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationDirectory());

  // Insert widgets into placeholders
  this->ui.deviceSelectorWidget->addWidget(m_DeviceSetSelectorWidget);

  //// Set application configuration
  //ui.comboBox_LogLevel->blockSignals(true);
  //ui.comboBox_LogLevel->setCurrentIndex(vtkPlusLogger::Instance()->GetLogLevel() - 1);
  //ui.comboBox_LogLevel->blockSignals(false);

  // Create status icon
  this->m_StatusIcon = new StatusIcon(this);

  //********************** Timers *************************
  // Set up timer for refreshing UI
  this->m_UiRefreshTimer = new QTimer(this);
  this->m_BroadcastingIntervalTimer = new QTimer(this);

  // Set up status bar (message and progress bar)
  this->setupStatusBar();

  //*************************** Connections ************************
  connect( ui.addTransformButton, SIGNAL(clicked()), this, SLOT(openTransformToolInfoEditor()) ); 
  connect( ui.broadcastImageButton, SIGNAL(clicked()), this, SLOT(openImageToolInfoEditor()) ); 
  //connect( ui.pushButton_SaveConfiguration, SIGNAL( clicked() ), this, SLOT( SaveDeviceSetConfiguration() ) );
  connect( m_UiRefreshTimer, SIGNAL( timeout() ), this, SLOT( updateGUI() ) );
  connect( m_BroadcastingIntervalTimer, SIGNAL( timeout() ), this, SLOT( sendMessages() ) );
  connect( ui.broadcastingIntervalSpinBox, SIGNAL( valueChanged (int) ), SLOT( setBroadcastingIntervalMs(int) ) ); 

  
  // Start timers 
  m_UiRefreshTimer->start(50);
  m_BroadcastingIntervalTimer->start(this->m_BroadcastingIntervalMs); 
  ui.broadcastingIntervalSpinBox->setValue(this->m_BroadcastingIntervalMs); 

}

//-----------------------------------------------------------------------------
PlusBroadcasterDaemonMainWindow::~PlusBroadcasterDaemonMainWindow()
{
  this->clearBroadcastedTransformToolInfoWidgets(); 

  if ( this->m_BroadcastedImageInfo != NULL )
  {
    delete this->m_BroadcastedImageInfo; 
    this->m_BroadcastedImageInfo = NULL; 
  }

  if ( this->m_UiRefreshTimer != NULL )
  {
    this->m_UiRefreshTimer->stop(); 
    delete this->m_UiRefreshTimer; 
    this->m_UiRefreshTimer = NULL; 
  }

  if ( this->m_BroadcastingIntervalTimer != NULL )
  {
    this->m_BroadcastingIntervalTimer->stop(); 
    delete this->m_BroadcastingIntervalTimer; 
    this->m_BroadcastingIntervalTimer = NULL; 
  }

  if ( this->m_StatusIcon != NULL )
  {
    delete this->m_StatusIcon; 
    this->m_StatusIcon = NULL; 
  }

  if ( this->m_DeviceSetSelectorWidget != NULL )
  {
    delete this->m_DeviceSetSelectorWidget; 
    this->m_DeviceSetSelectorWidget = NULL; 
  }
}

//-----------------------------------------------------------------------------
void PlusBroadcasterDaemonMainWindow::setupStatusBar()
{
  LOG_TRACE("PlusBroadcasterDaemonMainWindow::setupStatusBar");

  // Set up status bar
  QSizePolicy sizePolicy;
  sizePolicy.setHorizontalPolicy(QSizePolicy::Expanding);

  ui.statusbar->addPermanentWidget(m_StatusIcon);
}

//-----------------------------------------------------------------------------
void PlusBroadcasterDaemonMainWindow::setBroadcastingIntervalMs(int intervalMs)
{
  this->m_BroadcastingIntervalMs = intervalMs; 
  this->m_BroadcastingIntervalTimer->setInterval(this->m_BroadcastingIntervalMs); 
}

//-----------------------------------------------------------------------------
void PlusBroadcasterDaemonMainWindow::addBroadcastedTransformToolInfoWidget(IgtToolInfo& toolInfo)
{
  BroadcastedToolInfoWidget* widget = new BroadcastedToolInfoWidget(this); 
  widget->setToolInfo(toolInfo);
  widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  
  // Connect signals and slots
  connect( widget, SIGNAL( modified(const IgtToolInfo&, const IgtToolInfo&) ), 
    this, SLOT( modifyBroadcastedTransformTool(const IgtToolInfo&, const IgtToolInfo&) ) );
  connect( widget, SIGNAL( deleted(const IgtToolInfo&) ), 
    this, SLOT( deleteBroadcastedTransformTool(const IgtToolInfo&) ) );
  connect( widget, SIGNAL( changePauseStatus(const IgtToolInfo&) ), 
    this, SLOT( changeBroadcastedTransformToolPauseStatus(const IgtToolInfo&) ) );
  
  this->ui.toolsInfoLayout->addWidget(widget);
  
  this->m_BroadcastedTransformsInfo.push_back(widget); 
}

//-----------------------------------------------------------------------------
void PlusBroadcasterDaemonMainWindow::deleteBroadcastedTransformToolInfoWidget(BroadcastedToolInfoWidget* widget)
{
  std::deque<BroadcastedToolInfoWidget*>::iterator bToolInfoWidgetIterator = std::find_if(this->m_BroadcastedTransformsInfo.begin(), 
    this->m_BroadcastedTransformsInfo.end(), BroadcastedToolInfoWidgetFinder(widget)); 

  if ( bToolInfoWidgetIterator == this->m_BroadcastedTransformsInfo.end() )
  {
    LOG_ERROR("Unable to delete broadcasted tool info widget!"); 
    return; 
  }

  // Disconnect and remove widget 
  disconnect( widget, SIGNAL( modified(const IgtToolInfo&, const IgtToolInfo&) ), 
    this, SLOT( modifyBroadcastedTransformTool(const IgtToolInfo&, const IgtToolInfo&) ) ); 
  disconnect( widget, SIGNAL( deleted(const IgtToolInfo&) ), 
    this, SLOT( deleteBroadcastedTransformTool(const IgtToolInfo&) ) );
  disconnect( widget, SIGNAL( changePauseStatus(const IgtToolInfo&) ), 
    this, SLOT( changeBroadcastedTransformToolPauseStatus(const IgtToolInfo&) ) );

  ui.toolsInfoLayout->removeWidget(widget); 

  // remove from container 
  this->m_BroadcastedTransformsInfo.erase(bToolInfoWidgetIterator); 

  delete widget;
}

//-----------------------------------------------------------------------------
void PlusBroadcasterDaemonMainWindow::clearBroadcastedTransformToolInfoWidgets()
{
  while ( !this->m_BroadcastedTransformsInfo.empty() )
  {
    this->deleteBroadcastedTransformToolInfoWidget(*this->m_BroadcastedTransformsInfo.begin()); 
  } 
}

//-----------------------------------------------------------------------------
void PlusBroadcasterDaemonMainWindow::addBroadcastedImageInfoWidget()
{
  if ( this->m_BroadcastedImageInfo == NULL 
    && this->m_Broadcaster != NULL )
  {
    IgtToolInfo imageToolInfo = this->m_Broadcaster->GetImageInfo();
    this->m_BroadcastedImageInfo = new BroadcastedToolInfoWidget(this); 
    this->m_BroadcastedImageInfo->setToolInfo(imageToolInfo);
    this->m_BroadcastedImageInfo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // Connect signals and slots
    connect( this->m_BroadcastedImageInfo, SIGNAL( modified(const IgtToolInfo&, const IgtToolInfo&) ), 
      this, SLOT( modifyBroadcastedImageTool(const IgtToolInfo&, const IgtToolInfo&) ) );
    connect( this->m_BroadcastedImageInfo, SIGNAL( deleted(const IgtToolInfo&) ), 
      this, SLOT( deleteBroadcastedImageTool(const IgtToolInfo&) ) );
    connect( this->m_BroadcastedImageInfo, SIGNAL( changePauseStatus(const IgtToolInfo&) ), 
    this, SLOT( changeBroadcastedImageToolPauseStatus(const IgtToolInfo&) ) );

    this->ui.imageInfoLayout->addWidget(this->m_BroadcastedImageInfo);
  }
}

//-----------------------------------------------------------------------------
void PlusBroadcasterDaemonMainWindow::deleteBroadcastedImageInfoWidget()
{
  if ( this->m_BroadcastedImageInfo != NULL )
  {
    // Disconnect and remove widget 
    disconnect( this->m_BroadcastedImageInfo, SIGNAL( modified(const IgtToolInfo&, const IgtToolInfo&) ), 
      this, SLOT( modifyBroadcastedImageTool(const IgtToolInfo&, const IgtToolInfo&) ) ); 
    disconnect( this->m_BroadcastedImageInfo, SIGNAL( deleted(const IgtToolInfo&) ), 
      this, SLOT( deleteBroadcastedImageTool(const IgtToolInfo&) ) );
    disconnect( this->m_BroadcastedImageInfo, SIGNAL( changePauseStatus(const IgtToolInfo&) ), 
    this, SLOT( changeBroadcastedImageToolPauseStatus(const IgtToolInfo&) ) );

    ui.imageInfoLayout->removeWidget(this->m_BroadcastedImageInfo); 

    this->m_Broadcaster->RemoveBroadcastedImageInfo(); 

    delete this->m_BroadcastedImageInfo;
    this->m_BroadcastedImageInfo = NULL; 
  }
}

//-----------------------------------------------------------------------------
void PlusBroadcasterDaemonMainWindow::updateGUI()
{
  // We do not update the gui when a mouse button is pressed
  if (QApplication::mouseButtons() != Qt::NoButton)
  {
    return;
  }

  if ( this->m_Broadcaster )
  {
    // enable buttons
    ui.broadcastImageButton->setEnabled(true); 
    ui.addTransformButton->setEnabled(true); 

    // *************** Broadcast transforms *****************
    std::vector<IgtToolInfo> toolsInfo = this->m_Broadcaster->GetToolInfos(); 

    for ( int i = 0; i < toolsInfo.size(); ++i)
    {
      BroadcastedToolInfoWidget bToolInfoWidget; 
      bToolInfoWidget.setToolInfo(toolsInfo[i]); 
      
      std::deque<BroadcastedToolInfoWidget*>::iterator bToolInfoWidgetIterator = std::find_if(this->m_BroadcastedTransformsInfo.begin(), 
        this->m_BroadcastedTransformsInfo.end(), BroadcastedToolInfoWidgetFinder(&bToolInfoWidget)); 

      if ( bToolInfoWidgetIterator == this->m_BroadcastedTransformsInfo.end() )
      {
        // Couldn't find widget, add new one
        this->addBroadcastedTransformToolInfoWidget(toolsInfo[i]); 
      }
      else
      {
        // Widget found, update tool info
        (*bToolInfoWidgetIterator)->setToolInfo(toolsInfo[i]); 
      }
    }

    // Update tool info
    for ( int i = 0; i < this->m_BroadcastedTransformsInfo.size(); ++i)
    {
      this->m_BroadcastedTransformsInfo[i]->updateGUI(); 
    }

    // *************** Broadcast image *****************
    IgtToolInfo imageToolInfo = this->m_Broadcaster->GetImageInfo(); 
    if ( imageToolInfo.IgtlSocketInfo.Socket.IsNotNull() 
      && imageToolInfo.IgtlSocketInfo.Socket->GetConnected() )
    {
      // hide broadcastImageButton
      ui.broadcastImageButton->hide(); 
      this->addBroadcastedImageInfoWidget(); 
      this->m_BroadcastedImageInfo->setToolInfo(imageToolInfo); 
      this->m_BroadcastedImageInfo->updateGUI(); 
    }
    else // Image not broadcasted
    {
      // show broadcastImageButton
      ui.broadcastImageButton->show(); 
      this->deleteBroadcastedImageInfoWidget(); 
    }
 
  }
  else
  {
    // disable buttons
    ui.broadcastImageButton->setEnabled(false); 
    ui.addTransformButton->setEnabled(false); 
  }

  QApplication::processEvents(); 

}

//-----------------------------------------------------------------------------
void PlusBroadcasterDaemonMainWindow::saveDeviceSetConfiguration()
{
  LOG_TRACE("PlusBroadcasterDaemonMainWindow::SaveDeviceSetConfiguration");

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
void PlusBroadcasterDaemonMainWindow::connectToDevicesByConfigFile(std::string aConfigFile)
{
  LOG_TRACE("PlusBroadcasterDaemonMainWindow::connectToDevicesByConfigFile");
  LOG_INFO("Connect using configuration file: " << aConfigFile);

  // empty parameter string means disconnect from device
  if (STRCASECMP(aConfigFile.c_str(), "") == 0)
  {
    //ui.ConnectionStatusLabel->setText("Device not connected."); 
    if ( this->m_DataCollector != NULL )
    {
      this->m_DataCollector->Stop(); 
      this->m_DataCollector->Disconnect(); 
    }

    if ( this->m_Broadcaster != NULL )
    {
      this->m_Broadcaster->DisconnectSockets(); 
    }

    this->clearBroadcastedTransformToolInfoWidgets();     

    m_DeviceSetSelectorWidget->SetConnectionSuccessful(false);
    return; 
  }

  // *********************** Connect to devices and start data collection ************************

  // Read configuration
  vtkSmartPointer<vtkXMLDataElement> configRootElement = vtkSmartPointer<vtkXMLDataElement>::Take(vtkXMLUtilities::ReadElementFromFile(aConfigFile.c_str()));
  if (configRootElement == NULL)
  {	
    LOG_ERROR("Unable to read configuration from file " << aConfigFile); 
    return;
  }

  vtkPlusConfig::GetInstance()->SetDeviceSetConfigurationData(configRootElement); 

  if ( this->m_DataCollector == NULL )
  {
    this->m_DataCollector = vtkDataCollector::New(); 
  }

  if ( this->m_DataCollector->ReadConfiguration(configRootElement ) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to read configuration file for data collector!"); 
    return; 
  }

  QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));

  //ui.ConnectionStatusLabel->setText("Connecting to devices, please wait..."); 

  QApplication::processEvents();

  // Connect to devices
  if ( this->m_DataCollector->Connect() != PLUS_SUCCESS )
  { 
    LOG_ERROR("Failed to connect devices!"); 
    m_DeviceSetSelectorWidget->SetConnectionSuccessful(false);

    //ui.ConnectionStatusLabel->setText("Connection failed."); 
    QApplication::restoreOverrideCursor();
    return;
  }

  if ( this->setupBroadcaster() != PLUS_SUCCESS )
  {
    m_DeviceSetSelectorWidget->SetConnectionSuccessful(false);

    //ui.ConnectionStatusLabel->setText("Connection failed."); 
    QApplication::restoreOverrideCursor();
    return; 
  }

  //ui.ConnectionStatusLabel->setText("Device connected."); 
  m_DeviceSetSelectorWidget->SetConnectionSuccessful(true);

  vtkPlusConfig::GetInstance()->SaveApplicationConfigurationToFile();

  QApplication::restoreOverrideCursor();

}

//-----------------------------------------------------------------------------
PlusStatus PlusBroadcasterDaemonMainWindow::setupBroadcaster()
{
  if ( this->m_Broadcaster == NULL )
  {
    this->m_Broadcaster = vtkOpenIGTLinkBroadcaster::New();
  }

  this->m_Broadcaster->SetDataCollector(this->m_DataCollector); 

  if ( this->m_Broadcaster->Initialize() != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to initialize broadcaster!");
    return PLUS_FAIL;
  }

  if (this->m_Broadcaster->ReadConfiguration(vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData()) != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to read OpenIGTLinkBroadcaster configuration!");
    return PLUS_FAIL;
  }

  this->clearBroadcastedTransformToolInfoWidgets();  

  return PLUS_SUCCESS; 
}

//-----------------------------------------------------------------------------
void PlusBroadcasterDaemonMainWindow::sendMessages()
{
  if ( this->m_Broadcaster == NULL )
  {
    return; 
  }

  if ( this->m_Broadcaster->SendMessages() != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to broadcast messages!");
  }
}

//-----------------------------------------------------------------------------
void PlusBroadcasterDaemonMainWindow::deleteBroadcastedImageTool(const IgtToolInfo& toolInfo)
{
  // Remove tool info
    this->m_Broadcaster->RemoveBroadcastedImageInfo(); 
    this->deleteBroadcastedImageInfoWidget(); 
}

//-----------------------------------------------------------------------------
void PlusBroadcasterDaemonMainWindow::deleteBroadcastedTransformTool(const IgtToolInfo& toolInfo)
{
  BroadcastedToolInfoWidget* bToolInfo = new BroadcastedToolInfoWidget(this); 
  bToolInfo->setToolInfo(toolInfo);

  std::deque<BroadcastedToolInfoWidget*>::iterator toolInfoIterator = std::find_if(this->m_BroadcastedTransformsInfo.begin(), 
    this->m_BroadcastedTransformsInfo.end(), BroadcastedToolInfoWidgetFinder(bToolInfo)); 

  if ( toolInfoIterator != this->m_BroadcastedTransformsInfo.end() )
  {
    // Remove old tool info
    this->m_Broadcaster->RemoveBroadcastedToolInfo(toolInfo); 
    this->deleteBroadcastedTransformToolInfoWidget(*toolInfoIterator); 
    return; 
  }

  LOG_ERROR("Unable to remove tool info - not in the list!"); 
  return; 
}

//-----------------------------------------------------------------------------
void PlusBroadcasterDaemonMainWindow::addNewTransformToolInfo(const IgtToolInfo& toolInfo)
{
  std::ostringstream sendToLink; 
  sendToLink << toolInfo.IgtlSocketInfo.Host << ":" << toolInfo.IgtlSocketInfo.Port; 

  if ( this->m_Broadcaster->AddTransformForBroadcasting(toolInfo.Name.c_str(), sendToLink.str().c_str()) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to add transform for broadcasting!"); 
    return; 
  }
}

//-----------------------------------------------------------------------------
void PlusBroadcasterDaemonMainWindow::addNewImageToolInfo(const IgtToolInfo& toolInfo)
{
  std::ostringstream sendToLink; 
  sendToLink << toolInfo.IgtlSocketInfo.Host << ":" << toolInfo.IgtlSocketInfo.Port; 

  if ( this->m_Broadcaster->AddImageForBroadcasting(toolInfo.Name.c_str(), sendToLink.str().c_str()) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to add image for broadcasting!"); 
    return; 
  }
}

//-----------------------------------------------------------------------------
void PlusBroadcasterDaemonMainWindow::modifyBroadcastedImageTool(const IgtToolInfo& originalToolInfo, const IgtToolInfo& modifiedToolInfo)
{
  this->addNewImageToolInfo(modifiedToolInfo); 
}

//-----------------------------------------------------------------------------
void PlusBroadcasterDaemonMainWindow::modifyBroadcastedTransformTool(const IgtToolInfo& originalToolInfo, const IgtToolInfo& modifiedToolInfo)
{

  BroadcastedToolInfoWidget* bToolInfo = new BroadcastedToolInfoWidget(this); 
  bToolInfo->setToolInfo(originalToolInfo);

  std::deque<BroadcastedToolInfoWidget*>::iterator toolInfoIterator = std::find_if(this->m_BroadcastedTransformsInfo.begin(), 
    this->m_BroadcastedTransformsInfo.end(), BroadcastedToolInfoWidgetFinder(bToolInfo)); 

  if ( toolInfoIterator != this->m_BroadcastedTransformsInfo.end() )
  {
    // Add the new tool info
    this->addNewTransformToolInfo(modifiedToolInfo); 

    // Remove old tool info
    this->m_Broadcaster->RemoveBroadcastedToolInfo(originalToolInfo); 
    this->deleteBroadcastedTransformToolInfoWidget(*toolInfoIterator); 
    return; 
  }

  LOG_ERROR("Unable to modify tool info - not in the list!"); 
  return; 
}

//-----------------------------------------------------------------------------
void PlusBroadcasterDaemonMainWindow::changeBroadcastedTransformToolPauseStatus(const IgtToolInfo& toolInfo)
{
  if ( this->m_Broadcaster->ChangeBroadcastedToolPauseStatus(toolInfo) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to change broadcasted transform tool pause status!"); 
    return; 
  }
}

//-----------------------------------------------------------------------------
void PlusBroadcasterDaemonMainWindow::changeBroadcastedImageToolPauseStatus(const IgtToolInfo& toolInfo)
{
  if ( this->m_Broadcaster->ChangeImageToolPauseStatus() != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to change broadcasted image tool pause status!"); 
    return; 
  }
}

//-----------------------------------------------------------------------------
void PlusBroadcasterDaemonMainWindow::openTransformToolInfoEditor()
{
  BroadcastedToolEditorDialog editor; 
  connect( &editor, SIGNAL( modified(const IgtToolInfo&) ), this, SLOT( addNewTransformToolInfo(const IgtToolInfo&) ) ); 
  editor.exec(); 
}

//-----------------------------------------------------------------------------
void PlusBroadcasterDaemonMainWindow::openImageToolInfoEditor()
{
  BroadcastedToolEditorDialog editor; 
  connect( &editor, SIGNAL( modified(const IgtToolInfo&) ), this, SLOT( addNewImageToolInfo(const IgtToolInfo&) ) ); 
  editor.exec(); 
}
