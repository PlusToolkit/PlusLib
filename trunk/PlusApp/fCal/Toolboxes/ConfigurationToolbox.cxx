#include "ConfigurationToolbox.h"

#include "vtkPlusConfig.h"
#include "fCalMainWindow.h"
#include "vtkToolVisualizer.h"

#include "DeviceSetSelectorWidget.h"
#include "ToolStateDisplayWidget.h"

#include <QDialog>
#include <QFileDialog>

#include "vtkXMLUtilities.h"

//-----------------------------------------------------------------------------

ConfigurationToolbox::ConfigurationToolbox(fCalMainWindow* aParentMainWindow, QWidget* aParent, Qt::WFlags aFlags)
	: AbstractToolbox(aParentMainWindow)
	, QWidget(aParent, aFlags)
	, m_ToolStatePopOutWindow(NULL)
  , m_IsToolDisplayDetached(false)
{
	ui.setupUi(this);

	// Create and setup device set selector widget
	m_DeviceSetSelectorWidget = new DeviceSetSelectorWidget(this);
  m_DeviceSetSelectorWidget->SetConfigurationDirectoryFromRegistry(); 
	m_DeviceSetSelectorWidget->SetConfigurationDirectory(vtkPlusConfig::GetInstance()->GetConfigurationDirectory());
  m_DeviceSetSelectorWidget->SetComboBoxMinWidth(400); 

	m_ToolStateDisplayWidget = new ToolStateDisplayWidget(this);
	m_ToolStateDisplayWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

	// Make connections
	connect( m_DeviceSetSelectorWidget, SIGNAL( ConfigurationDirectoryChanged(std::string) ), this, SLOT( SetConfigurationDirectory(std::string) ) );
	connect( m_DeviceSetSelectorWidget, SIGNAL( ConnectToDevicesByConfigFileInvoked(std::string) ), this, SLOT( ConnectToDevicesByConfigFile(std::string) ) );
	connect( ui.pushButton_PopOut, SIGNAL( toggled(bool) ), this, SLOT( PopOutToggled(bool) ) );
	connect( ui.comboBox_LogLevel, SIGNAL( currentIndexChanged(int) ), this, SLOT( LogLevelChanged(int) ) );

	// Insert widgets into placeholders
	QGridLayout* gridDeviceSetSelection = new QGridLayout(ui.deviceSetSelectionWidget, 1, 1, 0, 4, "");
	gridDeviceSetSelection->addWidget(m_DeviceSetSelectorWidget);
	ui.deviceSetSelectionWidget->setMinimumHeight(196);
	ui.deviceSetSelectionWidget->setLayout(gridDeviceSetSelection);

	QGridLayout* gridToolStateDisplay = new QGridLayout(ui.toolStateDisplayWidget, 1, 1, 0, 4, "");
	gridToolStateDisplay->addWidget(m_ToolStateDisplayWidget);
	ui.toolStateDisplayWidget->setLayout(gridToolStateDisplay);
	ui.toolStateDisplayWidget->setMinimumHeight(m_ToolStateDisplayWidget->GetDesiredHeight());
	ui.toolStateDisplayWidget->setMaximumHeight(m_ToolStateDisplayWidget->GetDesiredHeight());

	// Change log level to info (program default)
	ui.comboBox_LogLevel->setCurrentText("Info");
}

//-----------------------------------------------------------------------------

ConfigurationToolbox::~ConfigurationToolbox()
{
}

//-----------------------------------------------------------------------------

void ConfigurationToolbox::Initialize()
{
	LOG_TRACE("ConfigurationToolbox::Initialize"); 

  // Install event filters to capture key event for dumping raw buffer data
	this->installEventFilter(this); //TODO make this a button or something, this is unreliable
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

  // No state handling in this toolbox
  m_ParentMainWindow->GetToolVisualizer()->HideAll();
}

//-----------------------------------------------------------------------------

void ConfigurationToolbox::SetConfigurationDirectory(std::string aDirectory)
{
	LOG_TRACE("ConfigurationToolbox::SetConfigurationDirectory");

	vtkPlusConfig::GetInstance()->SetConfigurationDirectory(aDirectory.c_str());
}

//-----------------------------------------------------------------------------

void ConfigurationToolbox::ConnectToDevicesByConfigFile(std::string aConfigFile)
{
	LOG_TRACE("ConfigurationToolbox::ConnectToDevicesByConfigFile");

  QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));

  // If not empty, then try to connect; empty parameter string means disconnect
  if (STRCASECMP(aConfigFile.c_str(), "") != 0) {
	  vtkPlusConfig::GetInstance()->SetConfigurationFileName(aConfigFile.data());

    // Read configuration
    vtkSmartPointer<vtkXMLDataElement> configRootElement = vtkXMLUtilities::ReadElementFromFile(aConfigFile.c_str());
    if (configRootElement == NULL) {	
      LOG_ERROR("Unable to read configuration from file " << aConfigFile); 
      return;
    }

    vtkPlusConfig::GetInstance()->SetConfigurationData(configRootElement); 

	  // If connection has been successfully created then this action should disconnect
	  if (! m_DeviceSetSelectorWidget->GetConnectionSuccessful()) {
		  LOG_INFO("Connect to devices"); 

		  // Create dialog
		  QDialog* connectDialog = new QDialog(this, Qt::Dialog);
		  connectDialog->setModal(true);
		  connectDialog->setMinimumSize(QSize(360,80));
		  connectDialog->setCaption(tr("Freehand Ultrasound Calibration"));
		  connectDialog->setBackgroundColor(QColor(224, 224, 224));

		  QLabel* connectLabel = new QLabel(QString("Connecting to devices, please wait..."), connectDialog);
		  connectLabel->setFont(QFont("SansSerif", 16));

		  QHBoxLayout* layout = new QHBoxLayout();
		  layout->addWidget(connectLabel);

		  connectDialog->setLayout(layout);
		  connectDialog->show();

		  QApplication::processEvents();

		  // Connect to devices
		  if (m_ParentMainWindow->GetToolVisualizer()->StartDataCollection() != PLUS_SUCCESS) {
			  LOG_ERROR("Unable to start collecting data!");
			  m_DeviceSetSelectorWidget->SetConnectionSuccessful(false);
			  m_ToolStateDisplayWidget->InitializeTools(NULL, false);

		  } else {
			  m_DeviceSetSelectorWidget->SetConnectionSuccessful(true);
			  if (m_ToolStateDisplayWidget->InitializeTools(m_ParentMainWindow->GetToolVisualizer()->GetDataCollector(), true)) {
				  ui.toolStateDisplayWidget->setMinimumHeight(m_ToolStateDisplayWidget->GetDesiredHeight());
				  ui.toolStateDisplayWidget->setMaximumHeight(m_ToolStateDisplayWidget->GetDesiredHeight());
			  }
		  }

      // Load device models based on the new configuration
      m_ParentMainWindow->GetToolVisualizer()->InitializeDeviceVisualization();

		  // Close dialog
		  connectDialog->done(0);
    }

  } else { // Disconnect
		vtkDataCollector* dataCollector = m_ParentMainWindow->GetToolVisualizer()->GetDataCollector();
		if ((dataCollector != NULL) && (dataCollector->GetInitialized())) {

			dataCollector->Stop();
			dataCollector->Disconnect();

			m_DeviceSetSelectorWidget->SetConnectionSuccessful(false);
			m_ToolStateDisplayWidget->InitializeTools(NULL, false);

      m_ParentMainWindow->ResetAllToolboxes();
		}
	}

  QApplication::restoreOverrideCursor();
}

//-----------------------------------------------------------------------------

void ConfigurationToolbox::PopOutToggled(bool aOn)
{
	LOG_TRACE("ConfigurationToolbox::PopOutToggled");

	if (aOn) {
		// Create pop out window
		m_ToolStatePopOutWindow = new QWidget(this, Qt::Tool);
		m_ToolStatePopOutWindow->setMinimumSize(QSize(180, m_ToolStateDisplayWidget->GetDesiredHeight()));
		m_ToolStatePopOutWindow->setMaximumSize(QSize(180, m_ToolStateDisplayWidget->GetDesiredHeight()));
		m_ToolStatePopOutWindow->setCaption(tr("Tool state display"));
		m_ToolStatePopOutWindow->setBackgroundColor(QColor::fromRgb(255, 255, 255));

		QGridLayout* gridToolStateDisplay = new QGridLayout(m_ToolStatePopOutWindow, 1, 1, 0, 4, "");
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

	} else {
		// Insert tool state display back in toolbox
		ui.toolStateDisplayWidget->setMinimumHeight(m_ToolStateDisplayWidget->GetDesiredHeight());
		ui.toolStateDisplayWidget->setMaximumHeight(m_ToolStateDisplayWidget->GetDesiredHeight());
		QGridLayout* gridToolStateDisplay = new QGridLayout(ui.toolStateDisplayWidget, 1, 1, 0, 4, "");
		gridToolStateDisplay->addWidget(m_ToolStateDisplayWidget);
		ui.toolStateDisplayWidget->setLayout(gridToolStateDisplay);

		// Delete pop out window
		if (m_ToolStatePopOutWindow) {
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
	LOG_TRACE("ConfigurationToolbox::eventFilter"); 

  bool passToParent = true;

	if (obj == m_ToolStatePopOutWindow) {
		if (ev->type() == QEvent::Close) {
			ui.pushButton_PopOut->setChecked(false);
      return true;
		}
  } else {
    if (ev->type() == QEvent::KeyPress) {
      QKeyEvent* keyEvent = dynamic_cast<QKeyEvent*>(ev);
      if (keyEvent) {
	      if ( ( keyEvent->key() == Qt::Key_D ) && ( keyEvent->modifiers() == Qt::ControlModifier ) ) {
	        // Directory open dialog for selecting directory to save the buffers into 
          QString dirName = QFileDialog::getExistingDirectory(NULL, QString( tr( "Open output directory for buffer dump files" ) ), m_ParentMainWindow->GetToolVisualizer()->GetOutputFolder());
	        if ( (dirName.isNull()) || (m_ParentMainWindow->GetToolVisualizer()->DumpBuffersToDirectory(dirName.toStdString().c_str()) != PLUS_SUCCESS) ) {
              LOG_ERROR("Writing raw buffers into files failed (output directory: " << dirName.toStdString() << ")!");
          }
          return true;
        }
      }
		}
  }

	// Pass the event on to the parent class
	return QWidget::eventFilter( obj, ev );
}

//-----------------------------------------------------------------------------

void ConfigurationToolbox::LogLevelChanged(int aLevel)
{
	LOG_TRACE("ConfigurationToolbox::LogLevelChanged");

	if ( STRCASECMP(ui.comboBox_LogLevel->currentText().ascii(), "Error") == 0 )
	{
		vtkPlusLogger::Instance()->SetLogLevel(vtkPlusLogger::LOG_LEVEL_ERROR); 
    vtkPlusLogger::Instance()->SetDisplayLogLevel(vtkPlusLogger::LOG_LEVEL_ERROR); 
	}
	else if ( STRCASECMP(ui.comboBox_LogLevel->currentText().ascii(), "Warning") == 0 )
	{
		vtkPlusLogger::Instance()->SetLogLevel(vtkPlusLogger::LOG_LEVEL_WARNING); 
    vtkPlusLogger::Instance()->SetDisplayLogLevel(vtkPlusLogger::LOG_LEVEL_WARNING);
	}
	else if ( STRCASECMP(ui.comboBox_LogLevel->currentText().ascii(), "Info") == 0 )
	{
		vtkPlusLogger::Instance()->SetLogLevel(vtkPlusLogger::LOG_LEVEL_INFO); 
    vtkPlusLogger::Instance()->SetDisplayLogLevel(vtkPlusLogger::LOG_LEVEL_INFO);
	}
	else if ( STRCASECMP(ui.comboBox_LogLevel->currentText().ascii(), "Debug") == 0 )
	{
		vtkPlusLogger::Instance()->SetLogLevel(vtkPlusLogger::LOG_LEVEL_DEBUG); 
    vtkPlusLogger::Instance()->SetDisplayLogLevel(vtkPlusLogger::LOG_LEVEL_DEBUG);
	}
	else if ( STRCASECMP(ui.comboBox_LogLevel->currentText().ascii(), "Trace") == 0 )
	{
		vtkPlusLogger::Instance()->SetLogLevel(vtkPlusLogger::LOG_LEVEL_TRACE); 
    vtkPlusLogger::Instance()->SetDisplayLogLevel(vtkPlusLogger::LOG_LEVEL_TRACE);
	}

	LOG_INFO("Log level changed to: " << ui.comboBox_LogLevel->currentText().ascii() )
}
