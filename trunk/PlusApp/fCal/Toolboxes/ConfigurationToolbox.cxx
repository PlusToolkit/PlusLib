/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#include "ConfigurationToolbox.h"
#include "DeviceSetSelectorWidget.h"
#include "FidPatternRecognition.h"
#include "ToolStateDisplayWidget.h"
#include "fCalMainWindow.h"
#include "vtkLineSource.h"
#include "vtkPhantomRegistrationAlgo.h"
#include "vtkPlusChannel.h"
#include "vtkVisualizationController.h"
#include "vtkXMLDataElement.h"
#include "vtkXMLUtilities.h"
#include "vtksys/SystemTools.hxx"
#include <QDialog>
#include <QFile>
#include <QFileDialog>

//-----------------------------------------------------------------------------

ConfigurationToolbox::ConfigurationToolbox(fCalMainWindow* aParentMainWindow, Qt::WFlags aFlags)
: AbstractToolbox(aParentMainWindow)
, QWidget(aParentMainWindow, aFlags)
, m_ToolStatePopOutWindow(NULL)
, m_IsToolDisplayDetached(false)
{
  ui.setupUi(this);

  // Create and setup device set selector widget
  m_DeviceSetSelectorWidget = new DeviceSetSelectorWidget(this);

  QFile styleSheetFile(":/styles/Resources/style_DeviceSetComboBox.qss");
  styleSheetFile.open(QIODevice::ReadOnly);
  QByteArray styleByteArray = styleSheetFile.readAll();
  QString styleSheet(styleByteArray.data());
  m_DeviceSetSelectorWidget->setStyleSheet(styleSheet);

  // Create tool box state widget
  m_ToolStateDisplayWidget = new ToolStateDisplayWidget(this);
  m_ToolStateDisplayWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

  // Make connections
  connect( m_DeviceSetSelectorWidget, SIGNAL( ConnectToDevicesByConfigFileInvoked(std::string) ), this, SLOT( ConnectToDevicesByConfigFile(std::string) ) );
  connect( ui.pushButton_PopOut, SIGNAL( toggled(bool) ), this, SLOT( PopOutToggled(bool) ) );
  connect( ui.comboBox_LogLevel, SIGNAL( currentIndexChanged(int) ), this, SLOT( LogLevelChanged(int) ) );
  connect( ui.pushButton_SelectEditorApplicationExecutable, SIGNAL( clicked() ), this, SLOT( SelectEditorApplicationExecutable() ) );
  connect( m_DeviceSetSelectorWidget, SIGNAL( ResetTracker() ), this, SLOT( ResetTracker() ) );
  connect( ui.pushButton_SelectImageDirectory, SIGNAL( clicked() ), this, SLOT( SelectImageDirectory() ) );

  // Insert widgets into placeholders
  QGridLayout* gridDeviceSetSelection = new QGridLayout(ui.deviceSetSelectionWidget);
  gridDeviceSetSelection->setMargin(0);
  gridDeviceSetSelection->setSpacing(4);
  gridDeviceSetSelection->addWidget(m_DeviceSetSelectorWidget);
  ui.deviceSetSelectionWidget->setMinimumHeight(196);
  ui.deviceSetSelectionWidget->setLayout(gridDeviceSetSelection);

  QGridLayout* gridToolStateDisplay = new QGridLayout(ui.toolStateDisplayWidget);
  gridToolStateDisplay->setMargin(0);
  gridToolStateDisplay->setSpacing(4);
  gridToolStateDisplay->addWidget(m_ToolStateDisplayWidget);
  ui.toolStateDisplayWidget->setLayout(gridToolStateDisplay);
  ui.toolStateDisplayWidget->setMinimumHeight(m_ToolStateDisplayWidget->GetDesiredHeight());
  ui.toolStateDisplayWidget->setMaximumHeight(m_ToolStateDisplayWidget->GetDesiredHeight());

  // Set application configuration
  ui.comboBox_LogLevel->blockSignals(true);
  ui.comboBox_LogLevel->setCurrentIndex(vtkPlusLogger::Instance()->GetLogLevel() - 1);
  ui.comboBox_LogLevel->blockSignals(false);

  ui.lineEdit_EditorApplicationExecutable->setText( QDir::toNativeSeparators(QString(vtkPlusConfig::GetInstance()->GetEditorApplicationExecutable())) );
  ui.lineEdit_ImageDirectory->setText( QDir::toNativeSeparators(QString(vtkPlusConfig::GetInstance()->GetImageDirectory().c_str())) );

  m_LastEditorLocation = QString("C:");
  m_LastImageDirectoryLocation = vtkPlusConfig::GetInstance()->GetImageDirectory().c_str();
}

//-----------------------------------------------------------------------------

ConfigurationToolbox::~ConfigurationToolbox()
{
}

//-----------------------------------------------------------------------------

void ConfigurationToolbox::OnActivated()
{
  LOG_TRACE("ConfigurationToolbox::OnActivated"); 

  SetDisplayAccordingToState();
}

//-----------------------------------------------------------------------------

void ConfigurationToolbox::RefreshContent()
{
  //LOG_TRACE("ConfigurationToolbox::RefreshToolboxContent"); 

  if (m_ToolStateDisplayWidget->IsInitialized()) {
    m_ToolStateDisplayWidget->Update();
  }
}

//-----------------------------------------------------------------------------

void ConfigurationToolbox::RefreshToolDisplayIfDetached()
{
  //LOG_TRACE("ConfigurationToolbox::RefreshToolDisplayIfDetached"); 

  if (m_IsToolDisplayDetached && m_ToolStateDisplayWidget->IsInitialized()) {
    m_ToolStateDisplayWidget->Update();
  }
}

//-----------------------------------------------------------------------------

void ConfigurationToolbox::SetDisplayAccordingToState()
{
  LOG_TRACE("ConfigurationToolbox::SetDisplayAccordingToState");

  // No state handling in this toolbox, hide all visualization
  m_ParentMainWindow->GetVisualizationController()->HideRenderer();

  // Disable the image manipulation menu
  m_ParentMainWindow->SetImageManipulationMenuEnabled(false);
}

//-----------------------------------------------------------------------------

void ConfigurationToolbox::ConnectToDevicesByConfigFile(std::string aConfigFile)
{
  LOG_TRACE("ConfigurationToolbox::ConnectToDevicesByConfigFile");

  QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));

  // If not empty, then try to connect; empty parameter string means disconnect
  if (STRCASECMP(aConfigFile.c_str(), "") != 0)
  {
    // Read configuration
    vtkSmartPointer<vtkXMLDataElement> configRootElement = vtkSmartPointer<vtkXMLDataElement>::Take(vtkXMLUtilities::ReadElementFromFile(aConfigFile.c_str()));
    if (configRootElement == NULL)
    {  
      LOG_ERROR("Unable to read configuration from file " << aConfigFile);

      m_DeviceSetSelectorWidget->SetConnectionSuccessful(false);
      m_ToolStateDisplayWidget->InitializeTools(NULL, false);
      QApplication::restoreOverrideCursor();

      return;
    }

    LOG_INFO("Device set configuration is read from file: " << aConfigFile);
    std::ostringstream xmlFileContents; 
    PlusCommon::PrintXML(xmlFileContents, vtkIndent(1), configRootElement);
    LOG_DEBUG("Device set configuration file contents: " << std::endl << xmlFileContents.str());    

    vtkPlusConfig::GetInstance()->SetDeviceSetConfigurationData(configRootElement); 

    // If connection has been successfully created then start data collection
    if (! m_DeviceSetSelectorWidget->GetConnectionSuccessful())
    {
      LOG_INFO("Connect to devices"); 

      // Disable main window
      m_ParentMainWindow->setEnabled(false);

      // Create dialog
      QDialog* connectDialog = new QDialog(this, Qt::Dialog);
      connectDialog->setMinimumSize(QSize(360,80));
      connectDialog->setCaption(tr("fCal"));
      connectDialog->setBackgroundColor(QColor(224, 224, 224));

      QLabel* connectLabel = new QLabel(QString("Connecting to devices, please wait..."), connectDialog);
      connectLabel->setFont(QFont("SansSerif", 16));

      QHBoxLayout* layout = new QHBoxLayout();
      layout->addWidget(connectLabel);

      connectDialog->setLayout(layout);
      connectDialog->show();

      QApplication::processEvents();

      // Connect to devices
      if (m_ParentMainWindow->GetVisualizationController()->StartDataCollection() != PLUS_SUCCESS)
      {
        LOG_ERROR("Unable to start collecting data!");
        m_DeviceSetSelectorWidget->SetConnectionSuccessful(false);
        m_ToolStateDisplayWidget->InitializeTools(NULL, false);
      }
      else
      {
        DeviceCollection aCollection;
        if( this->m_ParentMainWindow->GetVisualizationController()->GetDataCollector()->GetDevices(aCollection) != PLUS_SUCCESS )
        {
          LOG_ERROR("Unable to load the list of devices.");
          return;
        }

        // Read configuration
        if (this->ReadConfiguration(vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData()) != PLUS_SUCCESS)
        {
          LOG_ERROR("Failed to read fCal configuration");
        }

        this->ChannelChanged(*m_ParentMainWindow->GetSelectedChannel());

        m_ParentMainWindow->GetVisualizationController()->AssignDataCollector(m_ParentMainWindow->GetVisualizationController()->GetDataCollector());

        // Allow object visualizer to load anything it needs
        m_ParentMainWindow->GetVisualizationController()->ReadConfiguration(vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData());

        // Successful connection
        m_DeviceSetSelectorWidget->SetConnectionSuccessful(true);

        vtkPlusConfig::GetInstance()->SaveApplicationConfigurationToFile();

        if (ReadAndAddPhantomWiresToVisualization() != PLUS_SUCCESS)
        {
          LOG_WARNING("Unable to initialize phantom wires visualization");
        }
      }

      // Close dialog
      connectDialog->done(0);
      connectDialog->hide();
      delete connectDialog;
    }

    // Rebuild the devices menu to 
    m_ParentMainWindow->BuildChannelMenu();

    // Re-enable main window
    m_ParentMainWindow->setEnabled(true);

    // Re-enable manipulation buttons
    m_ParentMainWindow->Set3DManipulationMenuEnabled(true);
    if( m_ParentMainWindow->GetSelectedChannel() != NULL && m_ParentMainWindow->GetSelectedChannel()->GetVideoEnabled() )
    {
      m_ParentMainWindow->SetImageManipulationMenuEnabled(true);
    }
  }
  else // Disconnect
  {
    m_ParentMainWindow->SetSelectedChannel(NULL);
    m_ParentMainWindow->Set3DManipulationMenuEnabled(false);
    m_ParentMainWindow->SetImageManipulationMenuEnabled(false);

    m_ParentMainWindow->GetVisualizationController()->StopAndDisconnectDataCollector();
    m_ParentMainWindow->ResetShowDevices();
    m_ParentMainWindow->ResetAllToolboxes();
    m_ParentMainWindow->GetVisualizationController()->Reset();
    m_ParentMainWindow->GetVisualizationController()->ClearTransformRepository();
    m_DeviceSetSelectorWidget->SetConnectionSuccessful(false);
    m_DeviceSetSelectorWidget->ShowResetTrackerButton(false);
    m_ToolStateDisplayWidget->InitializeTools(NULL, false);

    // Rebuild the devices menu to clear out any previous devices
    m_ParentMainWindow->BuildChannelMenu();
  }

  QApplication::restoreOverrideCursor();
}

//-----------------------------------------------------------------------------

void ConfigurationToolbox::PopOutToggled(bool aOn)
{
  LOG_TRACE("ConfigurationToolbox::PopOutToggled");

  if (aOn)
  {
    // Create pop out window
    m_ToolStatePopOutWindow = new QWidget(this, Qt::Tool);
    m_ToolStatePopOutWindow->setMinimumSize(QSize(180, m_ToolStateDisplayWidget->GetDesiredHeight()));
    m_ToolStatePopOutWindow->setMaximumSize(QSize(180, m_ToolStateDisplayWidget->GetDesiredHeight()));
    m_ToolStatePopOutWindow->setCaption(tr("Tool state display"));
    m_ToolStatePopOutWindow->setBackgroundColor(QColor::fromRgb(255, 255, 255));

    QGridLayout* gridToolStateDisplay = new QGridLayout(m_ToolStatePopOutWindow);
    gridToolStateDisplay->setColumnStretch(1, 1);
    gridToolStateDisplay->setRowStretch(1, 1);
    gridToolStateDisplay->setMargin(0);
    gridToolStateDisplay->setSpacing(4);
    gridToolStateDisplay->addWidget(m_ToolStateDisplayWidget);
    m_ToolStatePopOutWindow->setLayout(gridToolStateDisplay);
    m_ToolStatePopOutWindow->move( mapToGlobal( QPoint( ui.pushButton_PopOut->x() + ui.pushButton_PopOut->width(), ui.pushButton_PopOut->y() ) ) );
    m_ToolStatePopOutWindow->show();

    // Install event filter that is called on closing the window
    m_ToolStatePopOutWindow->installEventFilter(this);

    // Delete layout from the toolbox (to be able to add again)
    delete ui.toolStateDisplayWidget->layout();

    // Reduce size of empty space in toolbox
    ui.toolStateDisplayWidget->setMinimumHeight(0);
    ui.toolStateDisplayWidget->setMaximumHeight(0);
  }
  else
  {
    // Insert tool state display back in toolbox
    ui.toolStateDisplayWidget->setMinimumHeight(m_ToolStateDisplayWidget->GetDesiredHeight());
    ui.toolStateDisplayWidget->setMaximumHeight(m_ToolStateDisplayWidget->GetDesiredHeight());
    QGridLayout* gridToolStateDisplay = new QGridLayout(ui.toolStateDisplayWidget);
    gridToolStateDisplay->setColumnStretch(1, 1);
    gridToolStateDisplay->setRowStretch(1, 1);
    gridToolStateDisplay->setMargin(0);
    gridToolStateDisplay->setSpacing(4);
    gridToolStateDisplay->addWidget(m_ToolStateDisplayWidget);
    ui.toolStateDisplayWidget->setLayout(gridToolStateDisplay);

    // Delete pop out window
    if (m_ToolStatePopOutWindow)
    {
      delete m_ToolStatePopOutWindow;
    }
    m_ToolStatePopOutWindow = NULL;
  }

  // Set detached flag
  m_IsToolDisplayDetached = aOn;
}

//-----------------------------------------------------------------------------

bool ConfigurationToolbox::eventFilter(QObject *obj, QEvent *ev)
{
  if ( obj == m_ToolStatePopOutWindow )
  {
    if ( ev->type() == QEvent::Close )
    {
      ui.pushButton_PopOut->setChecked(false);
    }
    else
    {
      // Pass the event on to the parent class
      return QWidget::eventFilter( obj, ev );
    }
  }

  return true;
}

//-----------------------------------------------------------------------------

void ConfigurationToolbox::LogLevelChanged(int aLevel)
{
  LOG_TRACE("ConfigurationToolbox::LogLevelChanged");

  vtkPlusLogger::Instance()->SetLogLevel(aLevel+1);

  LOG_INFO("Log level changed to: " << ui.comboBox_LogLevel->currentText().toLatin1().constData() << " (" << aLevel+1 << ")" );

  vtkPlusConfig::GetInstance()->SaveApplicationConfigurationToFile();
}

//-----------------------------------------------------------------------------

void ConfigurationToolbox::SelectEditorApplicationExecutable()
{
  LOG_TRACE("ConfigurationToolbox::SelectEditorApplicationExecutable");

  // File open dialog for selecting phantom definition xml
  QString filter = QString( tr( "Executables ( *.exe );;" ) );
  QString fileName = QFileDialog::getOpenFileName(NULL, QString( tr( "Select XML editor application" ) ), m_LastEditorLocation, filter);
  if (fileName.isNull()) {
    return;
  }

  m_LastEditorLocation = fileName.mid(0, fileName.lastIndexOf('/'));

  vtkPlusConfig::GetInstance()->SetEditorApplicationExecutable(fileName.toLatin1().constData());
  vtkPlusConfig::GetInstance()->SaveApplicationConfigurationToFile();

  ui.lineEdit_EditorApplicationExecutable->setText(fileName);
}

//-----------------------------------------------------------------------------

void ConfigurationToolbox::SelectImageDirectory()
{
  LOG_TRACE("ConfigurationToolbox::SelectImageDirectory"); 

  // Directory open dialog for selecting configuration directory 
  QString dirName = QFileDialog::getExistingDirectory(NULL, QString( tr( "Select image directory" ) ), m_LastImageDirectoryLocation);
  if (dirName.isNull()) {
    return;
  }

  m_LastImageDirectoryLocation = dirName;

  // Save the selected directory to config object
  vtkPlusConfig::GetInstance()->SetImageDirectory(dirName.toLatin1().constData());
  vtkPlusConfig::GetInstance()->SaveApplicationConfigurationToFile();

  ui.lineEdit_ImageDirectory->setText(dirName);
}

//-----------------------------------------------------------------------------

PlusStatus ConfigurationToolbox::ReadConfiguration(vtkXMLDataElement* aConfig)
{
  LOG_TRACE("ConfigurationToolbox::ReadConfiguration");

  if (aConfig == NULL)
  {
    LOG_ERROR("Unable to read configuration"); 
    return PLUS_FAIL;
  }

  vtkXMLDataElement* fCalElement = aConfig->FindNestedElementWithName("fCal"); 

  vtkPlusChannel* aChannel(NULL);
  if( this->SelectChannel(aChannel, fCalElement) != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to select a channel.");
    return PLUS_FAIL;
  }
  this->m_ParentMainWindow->SetSelectedChannel(aChannel);

  if( fCalElement == NULL)
  {
    LOG_ERROR("Failed to find fCal configuration!");
    return PLUS_FAIL;
  }

  // Image coordinate frame
  const char* imageCoordinateFrame = fCalElement->GetAttribute("ImageCoordinateFrame");
  if (imageCoordinateFrame == NULL)
  {
    LOG_ERROR("Image coordinate frame is not specified in the fCal section of the configuration!");
    return PLUS_FAIL;
  }

  m_ParentMainWindow->SetImageCoordinateFrame(imageCoordinateFrame);

  // Probe coordinate frame
  const char* probeCoordinateFrame = fCalElement->GetAttribute("ProbeCoordinateFrame");
  if (probeCoordinateFrame == NULL)
  {
    LOG_ERROR("Probe coordinate frame is not specified in the fCal section of the configuration!");
    return PLUS_FAIL;
  }

  m_ParentMainWindow->SetProbeCoordinateFrame(probeCoordinateFrame);

  // Reference coordinate frame
  const char* referenceCoordinateFrame = fCalElement->GetAttribute("ReferenceCoordinateFrame");
  if (referenceCoordinateFrame == NULL)
  {
    LOG_ERROR("Reference coordinate frame not specified in the fCal section of the configuration!");
    return PLUS_FAIL;
  }

  m_ParentMainWindow->SetReferenceCoordinateFrame(referenceCoordinateFrame);

  // Transducer origin coordinate frame
  const char* transducerOriginCoordinateFrame = fCalElement->GetAttribute("TransducerOriginCoordinateFrame");
  if (transducerOriginCoordinateFrame == NULL)
  {
    LOG_ERROR("Transducer origin coordinate frame not specified in the fCal section of the configuration!");
    return PLUS_FAIL;
  }

  m_ParentMainWindow->SetTransducerOriginCoordinateFrame(transducerOriginCoordinateFrame);

  // Transducer origin pixel coordinate frame
  const char* transducerOriginPixelCoordinateFrame = fCalElement->GetAttribute("TransducerOriginPixelCoordinateFrame");
  if (transducerOriginPixelCoordinateFrame == NULL)
  {
    LOG_ERROR("Transducer origin pixel coordinate frame not specified in the fCal section of the configuration!");
    return PLUS_FAIL;
  }

  m_ParentMainWindow->SetTransducerOriginPixelCoordinateFrame(transducerOriginPixelCoordinateFrame);

  // phantom model id
  const char* phantomModelId = fCalElement->GetAttribute("PhantomModelId");
  if (phantomModelId == NULL)
  {
    LOG_WARNING("Phantom model ID not specified in the fCal section of the configuration. Can't hide/show the phantom model.");
  }
  m_ParentMainWindow->SetPhantomModelId(phantomModelId);
  m_ParentMainWindow->EnablePhantomToggle(phantomModelId != NULL);

  // stylus model id
  const char* stylusModelId = fCalElement->GetAttribute("StylusModelId");
  if (stylusModelId == NULL)
  {
    LOG_WARNING("Stylus model ID not specified in the fCal section of the configuration. Can't visualize the stylus.");
  }
  m_ParentMainWindow->SetStylusModelId(stylusModelId);

  // stylus model id
  const char* probeModelId = fCalElement->GetAttribute("TransducerModelId");
  if (probeModelId == NULL)
  {
    LOG_WARNING("Transducer model ID not specified in the fCal section of the configuration. Can't visualize the probe.");
  }
  m_ParentMainWindow->SetTransducerModelId(probeModelId);

  // stylus model id
  const char* imageObjectId = fCalElement->GetAttribute("ImageDisplayableObjectId");
  if (imageObjectId == NULL)
  {
    LOG_WARNING("Image object ID not specified in the fCal section of the configuration. Can't visualize the stylus.");
  }
  m_ParentMainWindow->SetImageObjectId(imageObjectId);

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

PlusStatus ConfigurationToolbox::ReadAndAddPhantomWiresToVisualization()
{
  LOG_TRACE("ConfigurationToolbox::ReadAndAddPhantomWiresToVisualization"); 

  // Get phantom coordinate frame name
  vtkXMLDataElement* phantomRegistrationElement = vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData()->FindNestedElementWithName( vtkPhantomRegistrationAlgo::GetConfigurationElementName().c_str() ); 
  if (phantomRegistrationElement == NULL)
  {
    LOG_INFO("No phantom registration algorithm configuration are found - no phantom will be shown");
    return PLUS_SUCCESS;
  }

  vtkSmartPointer<vtkPhantomRegistrationAlgo> phantomRegistration = vtkSmartPointer<vtkPhantomRegistrationAlgo>::New();

  if (phantomRegistration->ReadConfiguration(vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData()) != PLUS_SUCCESS)
  {
    LOG_WARNING("Reading phantom registration algorithm configuration failed!");
    return PLUS_FAIL;
  }

  // Get phantom displayable model object
  vtkDisplayableObject* object = m_ParentMainWindow->GetVisualizationController()->GetObjectById(m_ParentMainWindow->GetPhantomModelId());
  vtkDisplayableModel* phantomDisplayableModel = dynamic_cast<vtkDisplayableModel*>(object);
  if( phantomDisplayableModel == NULL)
  {
    LOG_ERROR("Unable to retreive phantom model by ID. Is the phantom model ID well defined?");
    return PLUS_FAIL;
  }

  // Get wire pattern
  FidPatternRecognition patternRecognition;
  if (patternRecognition.ReadPhantomDefinition(vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData()) != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to read phantom wire configuration!");
    return PLUS_FAIL;
  }

  std::vector<Pattern*> patterns( patternRecognition.GetFidLineFinder()->GetPatterns() );

  vtkSmartPointer<vtkMatrix4x4> phantomModelToPhantomTransformMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
  phantomModelToPhantomTransformMatrix->DeepCopy(phantomDisplayableModel->GetModelToObjectTransform()->GetMatrix());
  vtkSmartPointer<vtkMatrix4x4> phantomToPhantomModelTransformMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
  vtkMatrix4x4::Invert(phantomModelToPhantomTransformMatrix, phantomToPhantomModelTransformMatrix);

  // Construct wires poly data
  for (std::vector<Pattern*>::iterator patternIt = patterns.begin(); patternIt != patterns.end(); ++patternIt)
  {
    for (std::vector<Wire>::iterator wireIt = (*patternIt)->Wires.begin(); wireIt != (*patternIt)->Wires.end(); ++wireIt)
    {
      double endPointFrontInPhantomFrame[4] = { wireIt->EndPointFront[0], wireIt->EndPointFront[1], wireIt->EndPointFront[2], 1.0 };
      double endPointBackInPhantomFrame[4] = { wireIt->EndPointBack[0], wireIt->EndPointBack[1], wireIt->EndPointBack[2], 1.0 };
      double endPointFrontInPhantomModelFrame[4];
      double endPointBackInPhantomModelFrame[4];
      phantomToPhantomModelTransformMatrix->MultiplyPoint(endPointFrontInPhantomFrame, endPointFrontInPhantomModelFrame);
      phantomToPhantomModelTransformMatrix->MultiplyPoint(endPointBackInPhantomFrame, endPointBackInPhantomModelFrame);

      vtkSmartPointer<vtkLineSource> wireLineSource = vtkSmartPointer<vtkLineSource>::New();
      wireLineSource->SetPoint1(endPointFrontInPhantomModelFrame);
      wireLineSource->SetPoint2(endPointBackInPhantomModelFrame);

      phantomDisplayableModel->AppendPolyData(wireLineSource->GetOutput());
    }
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
void ConfigurationToolbox::ResetTracker()
{
  if( m_DeviceSetSelectorWidget->GetConnectionSuccessful() )
  {
    vtkPlusChannel* aChannel = m_ParentMainWindow->GetSelectedChannel();
    if( m_ParentMainWindow->GetSelectedChannel() == NULL )
    {
      LOG_ERROR("No selected channel. Unable to reset tracker.");
      return;
    }

    if( aChannel != NULL )
    {
      m_ParentMainWindow->GetSelectedChannel()->GetOwnerDevice()->Reset();
    }
  }
}

//-----------------------------------------------------------------------------
PlusStatus ConfigurationToolbox::SelectChannel(vtkPlusChannel*& aChannel, vtkXMLDataElement* fCalElement)
{
  const char* selectedChannelId(NULL);
  if( fCalElement != NULL )
  {
    // default selected device
    selectedChannelId = fCalElement->GetAttribute("DefaultSelectedChannelId");
  }

  DeviceCollection aCollection;
  if( this->m_ParentMainWindow->GetVisualizationController()->GetDataCollector()->GetDevices(aCollection) != PLUS_SUCCESS )
  {
    LOG_ERROR("No devices to choose from! Cannot update configuration toolbox to match the new channel.");
    return PLUS_FAIL;
  }

  if( selectedChannelId == NULL )
  {
    for( DeviceCollectionConstIterator it = aCollection.begin(); it != aCollection.end(); ++it )
    {
      vtkPlusDevice* aDevice = *it;
      if( aDevice->OutputChannelCount() > 0 )
      {
        aChannel = *(aDevice->GetOutputChannelsStart());
        LOG_WARNING("No default channel selected, first channel found is now active: "<<aChannel->GetChannelId()<<". To avoid this warning add the DefaultSelectedChannelId attribute to the fCal element in the configuration file");
        return PLUS_SUCCESS;
      }
    }
  }
  
  if( aCollection.size() > 0 )
  {
    for( DeviceCollectionConstIterator it = aCollection.begin(); it != aCollection.end(); ++it )
    {
      vtkPlusDevice* aDevice = *it;
      if( aDevice->GetOutputChannelByName(aChannel, selectedChannelId) == PLUS_SUCCESS )
      {
        return PLUS_SUCCESS;
      }
    }
  }
  else
  {
    LOG_ERROR("No devices in device list! Unable to select channel " << selectedChannelId);
    return PLUS_FAIL;
  }

  // If it made it here, they've entered a default channel that doesn't exist... tsk stk
  LOG_WARNING("DefaultSelectedChannelId: " << selectedChannelId << " is not a valid channel. Selecting first available.");
  for( DeviceCollectionConstIterator it = aCollection.begin(); it != aCollection.end(); ++it )
  {
    vtkPlusDevice* aDevice = *it;
    if( aDevice->OutputChannelCount() > 0 )
    {
      aChannel = *(aDevice->GetOutputChannelsStart());
      LOG_WARNING("No default channel selected, first channel found is now active.");
      return PLUS_SUCCESS;
    }
  }

  LOG_ERROR("No devices have any output channels. Unable to visualize.");
  return PLUS_FAIL;
}

//-----------------------------------------------------------------------------
void ConfigurationToolbox::ChannelChanged( vtkPlusChannel& aChannel )
{
  if (m_ToolStateDisplayWidget->InitializeTools(&aChannel, true))
  {
    ui.toolStateDisplayWidget->setMinimumHeight(m_ToolStateDisplayWidget->GetDesiredHeight());
    ui.toolStateDisplayWidget->setMaximumHeight(m_ToolStateDisplayWidget->GetDesiredHeight());
  }

  m_DeviceSetSelectorWidget->ShowResetTrackerButton(aChannel.GetOwnerDevice()->IsResettable());
}

//-----------------------------------------------------------------------------
void ConfigurationToolbox::OnDeactivated()
{

}
