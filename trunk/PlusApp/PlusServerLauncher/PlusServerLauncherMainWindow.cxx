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
  stopServer(); // deletes m_CurrentServerInstance

  if ( m_DeviceSetSelectorWidget != NULL )
  {
    delete m_DeviceSetSelectorWidget; 
    m_DeviceSetSelectorWidget = NULL; 
  }
}

//-----------------------------------------------------------------------------
bool PlusServerLauncherMainWindow::startServer(QString& configFilePath)
{
  if (m_CurrentServerInstance!=NULL)
  {
    stopServer();
  }

  m_CurrentServerInstance = new QProcess();
  std::string plusServerExecutable = vtkPlusConfig::GetInstance()->GetPlusExecutablePath("PlusServer");
  std::string plusServerLocation = vtksys::SystemTools::GetFilenamePath(plusServerExecutable);
  m_CurrentServerInstance->setWorkingDirectory(QString(plusServerLocation.c_str()));
  connect(m_CurrentServerInstance, SIGNAL(readyReadStandardOutput()), this, SLOT(stdOutMsgReceived()));
  connect(m_CurrentServerInstance, SIGNAL(readyReadStandardError()), this, SLOT(stdErrMsgReceived()));
  connect(m_CurrentServerInstance, SIGNAL(error(QProcess::ProcessError)), this, SLOT(errorReceived(QProcess::ProcessError)));
  connect(m_CurrentServerInstance, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(serverExecutableFinished(int, QProcess::ExitStatus)));
  QString arg = QString("--config-file=\"").append(configFilePath).append("\"");
  m_CurrentServerInstance->start(QString(plusServerExecutable.c_str()).append(" ").append(arg).append(" --verbose=3"));
  m_CurrentServerInstance->waitForFinished(500);
  if( m_CurrentServerInstance->state() == QProcess::Running )
  {
    LOG_INFO("Server process started successfully");
    return true;
  }
  else
  {
    LOG_ERROR("Failed to start server process");
    return false;
  }
}

//-----------------------------------------------------------------------------
bool PlusServerLauncherMainWindow::stopServer()
{
  if (m_CurrentServerInstance==NULL)
  {
    // already stopped
    return true;
  }
  disconnect(m_CurrentServerInstance, SIGNAL(readyReadStandardOutput()), this, SLOT(stdOutMsgReceived()));
  disconnect(m_CurrentServerInstance, SIGNAL(readyReadStandardError()), this, SLOT(stdErrMsgReceived()));
  m_CurrentServerInstance->terminate();
  if( m_CurrentServerInstance->state() == QProcess::Running )
  {
    LOG_INFO("Server process stop request sent successfully");
  }
  bool forcedShutdown=false;
  const int totalTimeoutSec=15;
  const double retryDelayTimeoutSec=0.3;
  double timePassedSec=0;
  while (!m_CurrentServerInstance->waitForFinished(retryDelayTimeoutSec*1000))
  {
    m_CurrentServerInstance->terminate(); // in release mode on Windows the first terminate request may go unnoticed
    timePassedSec+=retryDelayTimeoutSec;
    if (timePassedSec>totalTimeoutSec)
    {
      // graceful termination was not successful, force the process to quit
      LOG_INFO("Server process did not stop on request for "<<timePassedSec<<" seconds, force it to quit now");
      m_CurrentServerInstance->kill();
      forcedShutdown=true;
      break;
    }
  }
  LOG_INFO("Server process stopped successfully");
  disconnect(m_CurrentServerInstance, SIGNAL(error(QProcess::ProcessError)), this, SLOT(errorReceived()));
  disconnect(m_CurrentServerInstance, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(serverExecutableFinished(int, QProcess::ExitStatus)));
  delete m_CurrentServerInstance;
  m_CurrentServerInstance = NULL;
  return (!forcedShutdown);
}


//-----------------------------------------------------------------------------
void PlusServerLauncherMainWindow::connectToDevicesByConfigFile(std::string aConfigFile)
{
  // Either a connect or disconnect, we always start from a clean slate: delete any previously active servers
  if ( m_CurrentServerInstance != NULL )
  {
    stopServer();
  }

  // Disconnect
  // Empty parameter string means disconnect from device
  if ( aConfigFile.empty() )
  {
    LOG_INFO("Disconnect request successful");
    m_DeviceSetSelectorWidget->SetConnectionSuccessful(false);
    return; 
  }

  LOG_INFO("Connect using configuration file: " << aConfigFile);

  // Connect
  if(startServer(QString(aConfigFile.c_str())))
  {
    m_DeviceSetSelectorWidget->SetConnectionSuccessful(true);
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
void PlusServerLauncherMainWindow::errorReceived(QProcess::ProcessError errorCode)
{
  const char* errorString="unknown";
  switch (errorCode)
  {
    case QProcess::FailedToStart: errorString="FailedToStart"; break;
    case QProcess::Crashed: errorString="Crashed"; break;
    case QProcess::Timedout: errorString="Timedout"; break;
    case QProcess::WriteError: errorString="WriteError"; break;
    case QProcess::ReadError: errorString="ReadError"; break;
    case QProcess::UnknownError:
    default:
      errorString="UnknownError";
  }
  LOG_ERROR("Server process error: "<<errorString);
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
