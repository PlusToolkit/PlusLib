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
#include <QComboBox>
#include <QtNetwork/QHostAddress>
#include <QtNetwork/QHostInfo>
#include <QtNetwork/QNetworkInterface>
#include <QStringList>
#include <QTimer>


//-----------------------------------------------------------------------------
PlusServerLauncherMainWindow::PlusServerLauncherMainWindow(QWidget *parent, Qt::WindowFlags flags, bool autoConnect)
  : QMainWindow(parent, flags|Qt::WindowMinimizeButtonHint|Qt::WindowCloseButtonHint)
  , m_DeviceSetSelectorWidget(NULL)
  , m_CurrentServerInstance(NULL)
{
  // Set up UI
  ui.setupUi(this);

  // Create device set selector widget
  m_DeviceSetSelectorWidget = new DeviceSetSelectorWidget(NULL);
  m_DeviceSetSelectorWidget->setMaximumWidth(1200);
  m_DeviceSetSelectorWidget->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding);
  m_DeviceSetSelectorWidget->SetConnectButtonText(QString("Launch Server"));
  connect( m_DeviceSetSelectorWidget, SIGNAL( ConnectToDevicesByConfigFileInvoked(std::string) ), this, SLOT( connectToDevicesByConfigFile(std::string) ) );

  // Create status icon
  StatusIcon* statusIcon = new StatusIcon(NULL);
  // Show only the last few thousand messages
  // (it should be enough, as all the messages are available in log files anyway)
  statusIcon->SetMaxMessageCount(3000);

  // Put the status icon in a frame with the log level selector

  ui.statusBarLayout->insertWidget(0, statusIcon);
  ui.comboBox_LogLevel->addItem("Error", QVariant(vtkPlusLogger::LOG_LEVEL_ERROR));
  ui.comboBox_LogLevel->addItem("Warning", QVariant(vtkPlusLogger::LOG_LEVEL_WARNING));
  ui.comboBox_LogLevel->addItem("Info", QVariant(vtkPlusLogger::LOG_LEVEL_INFO));
  ui.comboBox_LogLevel->addItem("Debug", QVariant(vtkPlusLogger::LOG_LEVEL_DEBUG));
  ui.comboBox_LogLevel->addItem("Trace", QVariant(vtkPlusLogger::LOG_LEVEL_TRACE));
  if( autoConnect )
  {
    ui.comboBox_LogLevel->setCurrentIndex(ui.comboBox_LogLevel->findData(QVariant(vtkPlusLogger::Instance()->GetLogLevel())));
  }
  else
  {
    ui.comboBox_LogLevel->setCurrentIndex(ui.comboBox_LogLevel->findData(QVariant(vtkPlusLogger::LOG_LEVEL_INFO)));
  }

  // Insert widgets into placeholders
  ui.centralLayout->setMargin(4);
  ui.centralLayout->insertWidget(0, m_DeviceSetSelectorWidget);

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

  // Log server host name, domain, and IP addresses
  LOG_INFO("Server host name: "<<QHostInfo::localHostName().toLatin1().constData());
  if (!QHostInfo::localDomainName().isEmpty())
  {
    LOG_INFO("Server host domain: "<<QHostInfo::localDomainName().toLatin1().constData());
  }
  QString ipAddresses;
  QList<QHostAddress> list = QNetworkInterface::allAddresses();
  for(int hostIndex=0; hostIndex<list.count(); hostIndex++)
  {
    if(list[hostIndex].protocol() == QAbstractSocket::IPv4Protocol )
    {
      if (!ipAddresses.isEmpty())
      {
        ipAddresses.append(",  ");
      }
      ipAddresses.append(list[hostIndex].toString());
    }
  }

  LOG_INFO("Server IP addresses: "<<ipAddresses.toLatin1().constData());
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
bool PlusServerLauncherMainWindow::startServer(const QString& configFilePath)
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
#if QT_VERSION >> 16 == 4
  QString cmdLine = QString("\"%1\" --config-file=\"%2\" --verbose=%3").arg(plusServerExecutable.c_str()).arg(configFilePath).arg(ui.comboBox_LogLevel->itemData(m_ComboBox_LogLevel->currentIndex()).toInt());
#else if QT_VERSION >> 16 == 5
  QString cmdLine = QString("\"%1\" --config-file=\"%2\" --verbose=%3").arg(plusServerExecutable.c_str()).arg(configFilePath).arg(ui.comboBox_LogLevel->currentData().toInt());
#endif
  LOG_INFO("Server process command line: "<<cmdLine.toLatin1().constData());
  m_CurrentServerInstance->start(cmdLine);
  m_CurrentServerInstance->waitForFinished(500);
  // During waitForFinished an error signal may be emitted, which may delete m_CurrentServerInstance,
  // therefore we need to check if m_CurrentServerInstance is still not NULL
  if( m_CurrentServerInstance && m_CurrentServerInstance->state() == QProcess::Running )
  {
    LOG_INFO("Server process started successfully");
    ui.comboBox_LogLevel->setEnabled(false);
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
  disconnect(m_CurrentServerInstance, SIGNAL(error(QProcess::ProcessError)), this, SLOT(errorReceived()));
  disconnect(m_CurrentServerInstance, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(serverExecutableFinished(int, QProcess::ExitStatus)));
  bool forcedShutdown=false;
  if( m_CurrentServerInstance->state() == QProcess::Running )
  {
    m_CurrentServerInstance->terminate();
    if( m_CurrentServerInstance->state() == QProcess::Running )
    {
      LOG_INFO("Server process stop request sent successfully");
    }
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
        LOG_WARNING("Server process did not stop on request for "<<timePassedSec<<" seconds, force it to quit now");
        m_CurrentServerInstance->kill();
        forcedShutdown=true;
        break;
      }
    }
    LOG_INFO("Server process stopped successfully");
    ui.comboBox_LogLevel->setEnabled(true);
  }
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
    m_DeviceSetSelectorWidget->SetConnectButtonText(QString("Launch Server"));
    return; 
  }

  LOG_INFO("Connect using configuration file: " << aConfigFile);

  // Connect
  if(startServer(QString(aConfigFile.c_str())))
  {
    m_DeviceSetSelectorWidget->SetConnectionSuccessful(true);
    m_DeviceSetSelectorWidget->SetConnectButtonText(QString("Stop Server"));
    vtkPlusConfig::GetInstance()->SaveApplicationConfigurationToFile();
  }
  else
  {
    m_DeviceSetSelectorWidget->SetConnectionSuccessful(false);
    m_DeviceSetSelectorWidget->SetConnectButtonText(QString("Launch Server"));
  }
}

//-----------------------------------------------------------------------------
void PlusServerLauncherMainWindow::keyPressEvent(QKeyEvent *e)
{
  // If ESC key is pressed don't quit the application, just minimize
  if(e->key() == Qt::Key_Escape)
  {
    showMinimized();
  }
  else
  {
    QMainWindow::keyPressEvent(e);
  }
}

//-----------------------------------------------------------------------------
void PlusServerLauncherMainWindow::sendServerOutputToLogger(const QByteArray &strData, vtkPlusLogger::LogLevelType defaultLogLevel)
{
  QString logString(strData);
#if _WIN32
  QStringList logLines = logString.split("\r\n", QString::SkipEmptyParts);
#else
  QStringList logLines = logString.split("\n", QString::SkipEmptyParts);
#endif
  foreach (const QString &logLine, logLines)
  {
    if( logLine.isEmpty() )
      continue;

    // Log line: time|level|timeoffset|message|location
    const int numberOfFieldsPerLogLine = 5;
    QStringList logFields = logLine.split('|');
    QString updatedLogLine;
    vtkPlusLogger::LogLevelType logLevel = defaultLogLevel;
    if (logFields.size() == numberOfFieldsPerLogLine)
    {
      // Try matching log level, starting with the most frequently used levels
      if (logFields[1]=="DEBUG")
      {
        logLevel = vtkPlusLogger::LOG_LEVEL_DEBUG;
      }
      else if (logFields[1]=="INFO")
      {
        logLevel = vtkPlusLogger::LOG_LEVEL_INFO;
      }
      else if (logFields[1]=="WARNING")
      {
        logLevel = vtkPlusLogger::LOG_LEVEL_WARNING;
      }
      else if (logFields[1]=="ERROR")
      {
        logLevel = vtkPlusLogger::LOG_LEVEL_ERROR;
      }
      else if (logFields[1]=="TRACE")
      {
        logLevel = vtkPlusLogger::LOG_LEVEL_TRACE;
      }
      logFields.pop_front(); // strip time
      logFields.pop_front(); // strip level
      logFields.pop_front(); // strip timeOffset
      updatedLogLine = logFields.join(";");
    }
    else
    {
      // cannot parse this line, log as is
      updatedLogLine = logLine;
    }
    vtkPlusLogger::Instance()->LogMessage(logLevel, updatedLogLine.toLatin1().constData(), "PlusServerLauncher", 0, "SERVER");
  }
}

//-----------------------------------------------------------------------------
void PlusServerLauncherMainWindow::stdOutMsgReceived()
{
  QByteArray strData = m_CurrentServerInstance->readAllStandardOutput();
  sendServerOutputToLogger(strData, vtkPlusLogger::LOG_LEVEL_INFO);
}

//-----------------------------------------------------------------------------
void PlusServerLauncherMainWindow::stdErrMsgReceived()
{
  QByteArray strData = m_CurrentServerInstance->readAllStandardError();
  sendServerOutputToLogger(strData, vtkPlusLogger::LOG_LEVEL_ERROR);
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
  LOG_ERROR("Server stopped unexpectedly. Return code: " << returnCode);
  this->connectToDevicesByConfigFile("");
  m_DeviceSetSelectorWidget->SetConnectionSuccessful(false);
}
