/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#include "CaptureClientWindow.h"
#include "CaptureControlWidget.h"
#include "vtkDataCollector.h"
#include "vtkPlusChannel.h"
#include "vtkVirtualStreamDiscCapture.h"
#include "vtkXMLUtilities.h"
#include <QDialog>

//-----------------------------------------------------------------------------
CaptureClientWindow::CaptureClientWindow(QWidget *parent, Qt::WFlags flags)
: QMainWindow(parent, flags)
, m_DataCollector(NULL)
, m_SelectedChannel(NULL)
{
  // Set up UI
  ui.setupUi(this);

  connect( ui.deviceSetSelectorWidget, SIGNAL( ConnectToDevicesByConfigFileInvoked(std::string) ), this, SLOT( ConnectToDevicesByConfigFile(std::string) ) );

  m_DataCollector = vtkDataCollector::New();
}

//-----------------------------------------------------------------------------
CaptureClientWindow::~CaptureClientWindow()
{
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

        // Successful connection
        ui.deviceSetSelectorWidget->SetConnectionSuccessful(true);

        vtkPlusConfig::GetInstance()->SaveApplicationConfigurationToFile();

        if (ui.toolStateDisplayWidget->InitializeTools(m_SelectedChannel, true))
        {
          ui.toolStateDisplayWidget->setMinimumHeight(ui.toolStateDisplayWidget->GetDesiredHeight());
          ui.toolStateDisplayWidget->setMaximumHeight(ui.toolStateDisplayWidget->GetDesiredHeight());
        }
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
    m_DataCollector->Disconnect();

    this->ConfigureCaptureWidgets();

    ui.deviceSetSelectorWidget->SetConnectionSuccessful(false);
    ui.deviceSetSelectorWidget->ShowResetTrackerButton(false);
    ui.toolStateDisplayWidget->InitializeTools(NULL, false);
  }

  QApplication::restoreOverrideCursor();
}

//-----------------------------------------------------------------------------
PlusStatus CaptureClientWindow::StartDataCollection()
{
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
  m_SelectedChannel = *(aDevice->GetOutputChannelsStart());

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
      if( dynamic_cast<vtkVirtualStreamDiscCapture*>(aDevice) != NULL )
      {
        vtkVirtualStreamDiscCapture* capDevice = dynamic_cast<vtkVirtualStreamDiscCapture*>(aDevice);
        CaptureControlWidget* aWidget = new CaptureControlWidget(this, NULL);
        aWidget->SetCaptureDevice(*capDevice);
        ui.captureWidgetGrid->addWidget(aWidget);
        m_CaptureWidgets.push_back(aWidget);
      }
    }
  }

  return PLUS_SUCCESS;
}