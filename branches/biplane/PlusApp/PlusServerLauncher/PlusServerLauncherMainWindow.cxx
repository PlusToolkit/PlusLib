/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#include "DeviceSetSelectorWidget.h"
#include "PlusCommon.h"
#include "PlusServerLauncherMainWindow.h"
#include "StatusIcon.h"
#include "vtkDataCollector.h"
#include "vtkPlusDeviceFactory.h"
#include "vtkPlusOpenIGTLinkServer.h"
#include "vtkTransformRepository.h"
#include <QIcon>
#include <QKeyEvent>
#include <QTimer>

//-----------------------------------------------------------------------------
PlusServerLauncherMainWindow::PlusServerLauncherMainWindow(QWidget *parent, Qt::WFlags flags, bool autoConnect)
  : QDialog(parent, flags|Qt::WindowMinimizeButtonHint)
  , m_DeviceSetSelectorWidget(NULL)
  , m_CurrentServerInstance(NULL)
{
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

  // Log basic info (Plus version, supported devices)
  std::string strPlusLibVersion = std::string(" Software version: ") + PlusCommon::GetPlusLibVersionString(); 
  LOG_INFO(strPlusLibVersion);
  LOG_INFO("Logging at level " << vtkPlusLogger::Instance()->GetLogLevel() << " to file: " << vtkPlusLogger::Instance()->GetLogFileName());
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
  if ( m_CurrentServerInstance != NULL )
  {
    m_CurrentServerInstance->terminate();
    m_CurrentServerInstance->waitForFinished(-1);
    delete m_CurrentServerInstance;
    m_CurrentServerInstance = NULL;
  }

  if ( m_DeviceSetSelectorWidget != NULL )
  {
    delete m_DeviceSetSelectorWidget; 
    m_DeviceSetSelectorWidget = NULL; 
  }
}

//-----------------------------------------------------------------------------
void PlusServerLauncherMainWindow::connectToDevicesByConfigFile(std::string aConfigFile)
{
  // Either a connect or disconnect, we always start from a clean slate: delete any previously active servers
  if ( m_CurrentServerInstance != NULL )
  {
    disconnect(m_CurrentServerInstance, SIGNAL(readyReadStandardOutput()), this, SLOT(stdOutMsgReceived()));
    disconnect(m_CurrentServerInstance, SIGNAL(readyReadStandardError()), this, SLOT(stdErrMsgReceived()));
    m_CurrentServerInstance->terminate();
    if( m_CurrentServerInstance->state() == QProcess::Running )
    {
      LOG_INFO("Terminate request sent successfully.");
    }
    m_CurrentServerInstance->waitForFinished(-1);
    LOG_INFO("Server cleaned up successfully.");
    disconnect(m_CurrentServerInstance, SIGNAL(error()), this, SLOT(errorReceived()));
    disconnect(m_CurrentServerInstance, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(serverExecutableFinished(int, QProcess::ExitStatus)));
    delete m_CurrentServerInstance;
    m_CurrentServerInstance = NULL;
  }

  // Disconnect
  // Empty parameter string means disconnect from device
  if ( aConfigFile.empty() )
  {
    LOG_INFO("Disconnect request successful.");
    m_DeviceSetSelectorWidget->SetConnectionSuccessful(false);
    return; 
  }

  LOG_INFO("Connect using configuration file: " << aConfigFile);

  // Connect

  // Start server
  m_CurrentServerInstance = new QProcess();
  std::string plusServerExecutable = vtkPlusConfig::GetInstance()->GetPlusExecutablePath("PlusServer");
  std::string plusServerLocation = vtksys::SystemTools::GetFilenamePath(plusServerExecutable);
  m_CurrentServerInstance->setWorkingDirectory(QString(plusServerLocation.c_str()));
  connect(m_CurrentServerInstance, SIGNAL(readyReadStandardOutput()), this, SLOT(stdOutMsgReceived()));
  connect(m_CurrentServerInstance, SIGNAL(readyReadStandardError()), this, SLOT(stdErrMsgReceived()));
  connect(m_CurrentServerInstance, SIGNAL(error()), this, SLOT(errorReceived()));
  connect(m_CurrentServerInstance, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(serverExecutableFinished(int, QProcess::ExitStatus)));
  QString arg = QString("--config-file=\"").append(QString(aConfigFile.c_str())).append("\"");
  m_CurrentServerInstance->start(QString(plusServerExecutable.c_str()).append(" ").append(arg).append(" --verbose=3"));
  m_CurrentServerInstance->waitForFinished(500);
  if( m_CurrentServerInstance->state() == QProcess::Running )
  {
    m_DeviceSetSelectorWidget->SetConnectionSuccessful(true);
    LOG_INFO("Server starting...");
    vtkPlusConfig::GetInstance()->SaveApplicationConfigurationToFile();
  }
  else
  {
    m_DeviceSetSelectorWidget->SetConnectionSuccessful(false);
  }
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

//-----------------------------------------------------------------------------
void PlusServerLauncherMainWindow::stdOutMsgReceived()
{
  QByteArray strData = m_CurrentServerInstance->readAllStandardOutput();
  // Strip the preceding INFO text
  std::string output = std::string(strData);
  int vertBarPos = 0;
  for( int i = 0; i < 2; i++ )//looking for the third one (start counting from 0)
    vertBarPos = output.find_first_of( '|', vertBarPos + 1 );

  LOG_INFO_PREFIX(output.substr(vertBarPos+1), "SERVER");
}

//-----------------------------------------------------------------------------
void PlusServerLauncherMainWindow::stdErrMsgReceived()
{
  QByteArray strData = m_CurrentServerInstance->readAllStandardError();
  // Strip the preceding ERROR/WARNING text
  std::string output = std::string(strData);
  int vertBarPos = 0;
  for( int i = 0; i < 2; i++ )//looking for the third one (start counting from 0)
    vertBarPos = output.find_first_of( '|', vertBarPos + 1 );

  if( output.find("|WARNING|") == 0 )
  {
    LOG_WARNING_PREFIX(output.substr(vertBarPos+1), "SERVER");
  }
  else
  {
    LOG_ERROR_PREFIX(output.substr(vertBarPos+1), "SERVER");
  }
}

//-----------------------------------------------------------------------------
void PlusServerLauncherMainWindow::errorReceived()
{
  m_DeviceSetSelectorWidget->SetConnectionSuccessful(false);
}

//-----------------------------------------------------------------------------
void PlusServerLauncherMainWindow::serverExecutableFinished(int returnCode, QProcess::ExitStatus status)
{
  if( returnCode != 0 )
  {
    LOG_ERROR("Server exited abnormally. Return code: " << returnCode);
    this->connectToDevicesByConfigFile("");
  }
}
