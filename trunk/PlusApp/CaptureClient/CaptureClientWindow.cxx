/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#include "CaptureClientWindow.h"
#include "vtkDataCollector.h"
#include "vtkPlusChannel.h"
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

        //m_ParentMainWindow->BuildChannelOwners(aCollection);

        // Read configuration
        if (ReadConfiguration(vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData()) != PLUS_SUCCESS)
        {
          LOG_ERROR("Failed to read fCal configuration");
        }

        //m_ParentMainWindow->GetVisualizationController()->AssignDataCollector(m_ParentMainWindow->GetVisualizationController()->GetDataCollector());

        // Allow object visualizer to load anything it needs
        //m_ParentMainWindow->GetVisualizationController()->ReadConfiguration(vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData());

        // Successful connection
        ui.deviceSetSelectorWidget->SetConnectionSuccessful(true);

        vtkPlusConfig::GetInstance()->SaveApplicationConfigurationToFile();

        if (ui.toolStateDisplayWidget->InitializeTools(m_SelectedChannel, true))
        {
          ui.toolStateDisplayWidget->setMinimumHeight(ui.toolStateDisplayWidget->GetDesiredHeight());
          ui.toolStateDisplayWidget->setMaximumHeight(ui.toolStateDisplayWidget->GetDesiredHeight());
        }

        /*
        if (ReadAndAddPhantomWiresToVisualization() != PLUS_SUCCESS)
        {
          LOG_WARNING("Unable to initialize phantom wires visualization");
        }
        */
      }

      // Close dialog
      connectDialog->done(0);
      connectDialog->hide();
      delete connectDialog;
    }

    // Rebuild the devices menu to 
    //m_ParentMainWindow->BuildChannelMenu();

    // Re-enable main window
    this->setEnabled(true);

    // Re-enable manipulation buttons
    //m_ParentMainWindow->Set3DManipulationMenuEnabled(true);
    //if( m_ParentMainWindow->GetSelectedChannel() != NULL && m_ParentMainWindow->GetSelectedChannel()->GetVideoEnabled() )
    //{
//      m_ParentMainWindow->SetImageManipulationMenuEnabled(true);
    //}
  }
  else // Disconnect
  {
    m_DataCollector->Disconnect();
    //m_ParentMainWindow->Set3DManipulationMenuEnabled(false);
    //m_ParentMainWindow->SetImageManipulationMenuEnabled(false);

    //m_ParentMainWindow->ResetShowDevices();
    //m_ParentMainWindow->ResetAllToolboxes();
    //m_ParentMainWindow->GetVisualizationController()->StopAndDisconnectDataCollector();
    //m_ParentMainWindow->GetVisualizationController()->Reset();
    //m_ParentMainWindow->GetVisualizationController()->ClearTransformRepository();
    ui.deviceSetSelectorWidget->SetConnectionSuccessful(false);
    ui.deviceSetSelectorWidget->ShowResetTrackerButton(false);
    ui.toolStateDisplayWidget->InitializeTools(NULL, false);

    //DeviceCollection emptyCollection;
    //this->m_ParentMainWindow->BuildChannelOwners(emptyCollection);

    // Rebuild the devices menu to clear out any previous devices
    //m_ParentMainWindow->BuildChannelMenu();
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
