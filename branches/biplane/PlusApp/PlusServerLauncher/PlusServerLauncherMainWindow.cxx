/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#include "DeviceSetSelectorWidget.h"
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
  , m_Server(NULL)
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

  // Either a connect or disconnect, we always start from a clean slate: delete any previously active servers
  if ( m_Server != NULL )
  {
    m_Server->Stop();
    m_Server->Delete();
    m_Server=NULL;
  }

  // Disconnect
  // Empty parameter string means disconnect from device
  if (STRCASECMP(aConfigFile.c_str(), "") == 0)
  {
    m_DeviceSetSelectorWidget->SetConnectionSuccessful(false);

    m_DataCollector->Stop();

    m_DataCollector->Delete();
    m_DataCollector = NULL;

    m_TransformRepository->Delete();
    m_TransformRepository = NULL;

    m_Server->Delete();
    m_Server = NULL;
    return; 
  }

  // Connect
  // Read main configuration file
  std::string configFilePath=aConfigFile;
  if (!vtksys::SystemTools::FileExists(configFilePath.c_str(), true))
  {
    configFilePath = vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationPath(aConfigFile);
    if (!vtksys::SystemTools::FileExists(configFilePath.c_str(), true))
    {
      LOG_ERROR("Reading device set configuration file failed: "<<aConfigFile<<" does not exist in the current directory or in "<<vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationDirectory());
      return;
    }
  }
  vtkSmartPointer<vtkXMLDataElement> configRootElement = vtkSmartPointer<vtkXMLDataElement>::Take(vtkXMLUtilities::ReadElementFromFile(configFilePath.c_str()));
  if (configRootElement == NULL)
  {
    LOG_ERROR("Reading device set configuration file failed: syntax error in "<<aConfigFile);
    return;
  }

  vtkPlusConfig::GetInstance()->SetDeviceSetConfigurationData(configRootElement);

  // Print configuration file contents for debugging purposes
  LOG_DEBUG("Device set configuration is read from file: " << aConfigFile);
  std::ostringstream xmlFileContents; 
  PlusCommon::PrintXML(xmlFileContents, vtkIndent(1), configRootElement);
  LOG_DEBUG("Device set configuration file contents: " << std::endl << xmlFileContents.str());

  // Create data collector instance 
  m_DataCollector = vtkDataCollector::New();
  if ( m_DataCollector->ReadConfiguration( configRootElement ) != PLUS_SUCCESS )
  {
    LOG_ERROR("Datacollector failed to read configuration"); 
    m_DataCollector->Delete();
    m_DataCollector = NULL;
    return;
  }

  // Create transform repository instance 
  m_TransformRepository = vtkTransformRepository::New(); 
  if ( m_TransformRepository->ReadConfiguration( configRootElement ) != PLUS_SUCCESS )
  {
    LOG_ERROR("Transform repository failed to read configuration"); 
    m_TransformRepository->Delete();
    m_TransformRepository = NULL;
    m_DataCollector->Delete();
    m_DataCollector = NULL;
    return;
  }

  LOG_DEBUG( "Initializing data collector... " );
  if ( m_DataCollector->Connect() != PLUS_SUCCESS )
  {
    LOG_ERROR("Datacollector failed to connect to devices"); 
    m_TransformRepository->Delete();
    m_TransformRepository = NULL;
    m_DataCollector->Delete();
    m_DataCollector = NULL;
    return;
  }

  if ( m_DataCollector->Start() != PLUS_SUCCESS )
  {
    LOG_ERROR("Datacollector failed to start"); 
    m_TransformRepository->Delete();
    m_TransformRepository = NULL;
    m_DataCollector->Delete();
    m_DataCollector = NULL;
    return;
  }

  if (configRootElement == NULL)
  {
    LOG_ERROR("Invalid device set configuration: unable to find required PlusOpenIGTLinkServer element");
    return;
  }
  vtkXMLDataElement* serverElement = configRootElement->FindNestedElementWithName("PlusOpenIGTLinkServer"); 
  if (serverElement == NULL)
  {
    LOG_ERROR("Unable to find required PlusOpenIGTLinkServer element in device set configuration"); 
    return;
  }

  // Start server
  QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));
  m_Server = vtkPlusOpenIGTLinkServer::New();
  PlusStatus status=m_Server->Start(m_DataCollector, m_TransformRepository, serverElement, configFilePath) ;
  QApplication::restoreOverrideCursor();
  if ( status != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to start the server"); 
    m_DeviceSetSelectorWidget->SetConnectionSuccessful(false);
    m_TransformRepository->Delete();
    m_TransformRepository = NULL;
    m_DataCollector->Delete();
    m_DataCollector = NULL;
    m_Server->Delete();
    m_Server = NULL;
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
