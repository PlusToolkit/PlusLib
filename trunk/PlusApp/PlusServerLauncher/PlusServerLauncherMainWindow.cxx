/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#include "PlusServerLauncherMainWindow.h"

#include "StatusIcon.h"
#include "ConfigFileSaverDialog.h"
#include "DeviceSetSelectorWidget.h"
#include "vtkDataCollector.h"
#include "vtkXMLUtilities.h" 

#include <QTimer>
#include <QMenu>
#include <QLabel>
#include <QProgressBar>
#include <QFileDialog>



//-----------------------------------------------------------------------------
PlusServerLauncherMainWindow::PlusServerLauncherMainWindow(QWidget *parent, Qt::WFlags flags)
: QMainWindow(parent, flags)
{
  this->m_DataCollector = NULL; 

  // Set up UI
  this->ui.setupUi(this);

  // Connect signals and slots

  

  //****************************** Device selector widget ************************
  // Create and setup device set selector widget
  this->m_DeviceSetSelectorWidget = new DeviceSetSelectorWidget(this);
  connect( m_DeviceSetSelectorWidget, SIGNAL( ConnectToDevicesByConfigFileInvoked(std::string) ), this, SLOT( connectToDevicesByConfigFile(std::string) ) );

  // Setup device set selector widget
  this->m_DeviceSetSelectorWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
//  this->m_DeviceSetSelectorWidget->SetComboBoxMinWidth(0); 
  this->m_DeviceSetSelectorWidget->resize(this->width(), this->height());

  // Re-parse the directory to properly select the previously selected device set (it is called in its constructor but there are no connects set up yet)
  this->m_DeviceSetSelectorWidget->SetConfigurationDirectory(vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationDirectory().c_str());

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

  // Set up status bar (message and progress bar)
  this->setupStatusBar();

  //*************************** Connections ************************
 /* connect( ui.addTransformButton, SIGNAL(clicked()), this, SLOT(openTransformToolInfoEditor()) ); 
  connect( ui.broadcastImageButton, SIGNAL(clicked()), this, SLOT(openImageToolInfoEditor()) ); 
  //connect( ui.pushButton_SaveConfiguration, SIGNAL( clicked() ), this, SLOT( SaveDeviceSetConfiguration() ) );
  connect( m_UiRefreshTimer, SIGNAL( timeout() ), this, SLOT( updateGUI() ) );
  connect( m_BroadcastingIntervalTimer, SIGNAL( timeout() ), this, SLOT( sendMessages() ) );
  connect( ui.broadcastingIntervalSpinBox, SIGNAL( valueChanged (int) ), SLOT( setBroadcastingIntervalMs(int) ) ); 
  */

}

//-----------------------------------------------------------------------------
PlusServerLauncherMainWindow::~PlusServerLauncherMainWindow()
{

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
void PlusServerLauncherMainWindow::setupStatusBar()
{
  LOG_TRACE("PlusServerLauncherMainWindow::setupStatusBar");

  // Set up status bar
  QSizePolicy sizePolicy;
  sizePolicy.setHorizontalPolicy(QSizePolicy::Expanding);

  ui.statusbar->addPermanentWidget(m_StatusIcon);
}


//-----------------------------------------------------------------------------
void PlusServerLauncherMainWindow::updateGUI()
{
  // We do not update the gui when a mouse button is pressed
  if (QApplication::mouseButtons() != Qt::NoButton)
  {
    return;
  }
  /*

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
*/
  QApplication::processEvents(); 

}

//-----------------------------------------------------------------------------
void PlusServerLauncherMainWindow::saveDeviceSetConfiguration()
{
  LOG_TRACE("PlusServerLauncherMainWindow::SaveDeviceSetConfiguration");

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
void PlusServerLauncherMainWindow::connectToDevicesByConfigFile(std::string aConfigFile)
{
  LOG_TRACE("PlusServerLauncherMainWindow::connectToDevicesByConfigFile");
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

/*  if ( this->setupBroadcaster() != PLUS_SUCCESS )
  {
    m_DeviceSetSelectorWidget->SetConnectionSuccessful(false);

    //ui.ConnectionStatusLabel->setText("Connection failed."); 
    QApplication::restoreOverrideCursor();
    return; 
  }
*/
  //ui.ConnectionStatusLabel->setText("Device connected."); 
  m_DeviceSetSelectorWidget->SetConnectionSuccessful(true);

  vtkPlusConfig::GetInstance()->SaveApplicationConfigurationToFile();

  QApplication::restoreOverrideCursor();

}
