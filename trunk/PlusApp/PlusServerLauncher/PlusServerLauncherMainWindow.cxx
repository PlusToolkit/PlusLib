/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#include "PlusServerLauncherMainWindow.h"
#include "vtkPlusDeviceFactory.h"
#include "vtkXMLDataElement.h"
#include "vtkXMLUtilities.h"

#include "vtkPlusOpenIGTLinkServer.h"

#include "StatusIcon.h"
#include "DeviceSetSelectorWidget.h"

#include <QTimer>

//-----------------------------------------------------------------------------
PlusServerLauncherMainWindow::PlusServerLauncherMainWindow(QWidget *parent, Qt::WFlags flags)
: QMainWindow(parent, flags)
{
  m_Server = vtkPlusOpenIGTLinkServer::New();

  ui.setupUi(this);

  // Create and setup device set selector widget
  m_DeviceSetSelectorWidget = new DeviceSetSelectorWidget(this);
  connect( m_DeviceSetSelectorWidget, SIGNAL( ConnectToDevicesByConfigFileInvoked(std::string) ), this, SLOT( connectToDevicesByConfigFile(std::string) ) );
  // Re-parse the directory to properly select the previously selected device set (it is called in its constructor but there are no connects set up yet)
  m_DeviceSetSelectorWidget->SetConfigurationDirectory(vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationDirectory().c_str());
  // Insert widgets into placeholders
  ui.deviceSelectorWidget->addWidget(m_DeviceSetSelectorWidget);

  // Create status icon
  m_StatusIcon = new StatusIcon(this);
  ui.statusbar->addPermanentWidget(m_StatusIcon);

  // Set up timer for processing pending commands
  m_ProcessPendingCommandsTimer = new QTimer(this);
  connect( m_ProcessPendingCommandsTimer, SIGNAL( timeout() ), this, SLOT( processPendingCommands() ) );
  m_ProcessPendingCommandsTimer->start(50);

  std::string strPlusLibVersion = std::string(" Software version: ") + PlusCommon::GetPlusLibVersionString(); 
  LOG_INFO(strPlusLibVersion);

  LOG_INFO("Loging at level "<<vtkPlusLogger::Instance()->GetLogLevel()<<" to file: "<<vtkPlusLogger::Instance()->GetLogFileName());

  vtkSmartPointer<vtkPlusDeviceFactory> deviceFactory = vtkSmartPointer<vtkPlusDeviceFactory>::New(); 
  std::ostringstream supportedDevices; 
  deviceFactory->PrintAvailableDevices(supportedDevices, vtkIndent()); 
  LOG_INFO(supportedDevices.str());

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

  if ( m_StatusIcon != NULL )
  {
    delete m_StatusIcon; 
    m_StatusIcon = NULL; 
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
    //ui.ConnectionStatusLabel->setText("Device not connected."); 
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
