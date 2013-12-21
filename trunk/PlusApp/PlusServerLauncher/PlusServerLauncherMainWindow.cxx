/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#include "DeviceSetSelectorWidget.h"
#include "PlusServerLauncherMainWindow.h"
#include "StatusIcon.h"
#include "vtkPlusDeviceFactory.h"
#include "vtkPlusOpenIGTLinkServer.h"
#include <QIcon>
#include <QTimer>

//-----------------------------------------------------------------------------
PlusServerLauncherMainWindow::PlusServerLauncherMainWindow(QWidget *parent, Qt::WFlags flags, bool autoConnect)
  : QDialog(parent, flags|Qt::WindowMinimizeButtonHint)
  , m_DeviceSetSelectorWidget(NULL)
  , m_Server(NULL)
{
  m_Server = vtkPlusOpenIGTLinkServer::New();

  setWindowIcon(QPixmap( ":/icons/Resources/icon_ConnectLarge.png" ));

  setMinimumSize(480, 320);
  setMaximumSize(1200, 800);

  // Create device set selector widget
  m_DeviceSetSelectorWidget = new DeviceSetSelectorWidget(this);
  m_DeviceSetSelectorWidget->setMaximumWidth(1200);
  m_DeviceSetSelectorWidget->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding);
  connect( m_DeviceSetSelectorWidget, SIGNAL( ConnectToDevicesByConfigFileInvoked(std::string) ), this, SLOT( connectToDevicesByConfigFile(std::string) ) );

  // Create status icon
  StatusIcon* statusIcon = new StatusIcon(this);
  // Show only the last few thousand messages
  // (it should be enough, as all the messages are available in log files anyway)
  statusIcon->SetMaxMessageCount(3000);

  // Insert widgets into placeholders
  QGridLayout* mainGrid = new QGridLayout(this);
  mainGrid->setMargin(4);
  mainGrid->setSpacing(4);
  mainGrid->addWidget(m_DeviceSetSelectorWidget, 0, 0);
  mainGrid->addWidget(statusIcon, 1, 0, Qt::AlignRight);
  this->setLayout(mainGrid);

  // Set up timer for processing pending commands
  m_ProcessPendingCommandsTimer = new QTimer(this);
  connect( m_ProcessPendingCommandsTimer, SIGNAL( timeout() ), this, SLOT( processPendingCommands() ) );
  m_ProcessPendingCommandsTimer->start(50);

  // Log basic info (Plus version, supported devices)
  std::string strPlusLibVersion = std::string(" Software version: ") + PlusCommon::GetPlusLibVersionString(); 
  LOG_INFO(strPlusLibVersion);
  LOG_INFO("Loging at level "<<vtkPlusLogger::Instance()->GetLogLevel()<<" to file: "<<vtkPlusLogger::Instance()->GetLogFileName());
  vtkSmartPointer<vtkPlusDeviceFactory> deviceFactory = vtkSmartPointer<vtkPlusDeviceFactory>::New(); 
  std::ostringstream supportedDevices; 
  deviceFactory->PrintAvailableDevices(supportedDevices, vtkIndent()); 
  LOG_INFO(supportedDevices.str());

  if (autoConnect)
  {
    std::string configFileName=vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationFileName();
    if (configFileName.empty())
    {
      LOG_ERROR("Auto-connect failed: device set configuration file is not specified");
    }
    else
    {
      connectToDevicesByConfigFile(configFileName);
      if (m_DeviceSetSelectorWidget->GetConnectionSuccessful())
      {
        showMinimized();
      }
    }    
  }
}

//-----------------------------------------------------------------------------
PlusServerLauncherMainWindow::~PlusServerLauncherMainWindow()
{
  if ( m_ProcessPendingCommandsTimer != NULL )
  {
    m_ProcessPendingCommandsTimer->stop(); 
    delete m_ProcessPendingCommandsTimer; 
    m_ProcessPendingCommandsTimer = NULL; 
  } 

  if ( m_Server != NULL )
  {
    m_Server->Stop();
    m_Server->Delete();
    m_Server = NULL; 
  }

  if ( m_DeviceSetSelectorWidget != NULL )
  {
    delete m_DeviceSetSelectorWidget; 
    m_DeviceSetSelectorWidget = NULL; 
  }
}

//-----------------------------------------------------------------------------
void PlusServerLauncherMainWindow::processPendingCommands()
{
  if (m_Server!=NULL)
  {
    // Process commands that we've received from the data receiver thread
    // and put command responses into the queue for the data sending thread
    m_Server->ProcessPendingCommands();
  }
}

//-----------------------------------------------------------------------------
void PlusServerLauncherMainWindow::connectToDevicesByConfigFile(std::string aConfigFile)
{
  LOG_INFO("Connect using configuration file: " << aConfigFile);

  // Empty parameter string means disconnect from device
  if (STRCASECMP(aConfigFile.c_str(), "") == 0)
  {
    if ( m_Server != NULL )
    {
      m_Server->Stop(); 
    }
    m_DeviceSetSelectorWidget->SetConnectionSuccessful(false);
    return; 
  }

  // Start server 
  QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));
  PlusStatus status=m_Server->Start(aConfigFile) ;
  QApplication::restoreOverrideCursor();
  if ( status != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to start the server"); 
    m_DeviceSetSelectorWidget->SetConnectionSuccessful(false);
    return; 
  }
  m_DeviceSetSelectorWidget->SetConnectionSuccessful(true);
  vtkPlusConfig::GetInstance()->SaveApplicationConfigurationToFile();
}

//-----------------------------------------------------------------------------
void PlusServerLauncherMainWindow::keyPressEvent(QKeyEvent *e)
{
  // If ESC key is pressed don't quit the application, just minimize
  if(e->key() != Qt::Key_Escape)
  {
    QDialog::keyPressEvent(e);
  }
  else
  {
    showMinimized();
  }
}
