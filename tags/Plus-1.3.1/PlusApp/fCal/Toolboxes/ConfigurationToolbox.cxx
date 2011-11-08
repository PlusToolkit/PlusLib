/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#include "ConfigurationToolbox.h"

#include "fCalMainWindow.h"
#include "vtkToolVisualizer.h"

#include "DeviceSetSelectorWidget.h"
#include "ToolStateDisplayWidget.h"

#include <QDialog>
#include <QFileDialog>

#include "vtkXMLUtilities.h"
#include "vtkXMLDataElement.h"
#include "vtksys/SystemTools.hxx" 

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
  m_DeviceSetSelectorWidget->SetComboBoxMinWidth(400); 

	m_ToolStateDisplayWidget = new ToolStateDisplayWidget(this);
	m_ToolStateDisplayWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

	// Make connections
	connect( m_DeviceSetSelectorWidget, SIGNAL( ConnectToDevicesByConfigFileInvoked(std::string) ), this, SLOT( ConnectToDevicesByConfigFile(std::string) ) );
	connect( ui.pushButton_PopOut, SIGNAL( toggled(bool) ), this, SLOT( PopOutToggled(bool) ) );
	connect( ui.comboBox_LogLevel, SIGNAL( currentIndexChanged(int) ), this, SLOT( LogLevelChanged(int) ) );
	connect( ui.pushButton_SelectEditorApplicationExecutable, SIGNAL( clicked() ), this, SLOT( SelectEditorApplicationExecutable() ) );
	connect( ui.pushButton_SelectImageDirectory, SIGNAL( clicked() ), this, SLOT( SelectImageDirectory() ) );

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

  // Set application configuration
  ui.comboBox_LogLevel->blockSignals(true);
  ui.comboBox_LogLevel->setCurrentIndex(vtkPlusLogger::Instance()->GetLogLevel() - 1);
  ui.comboBox_LogLevel->blockSignals(false);

  ui.lineEdit_EditorApplicationExecutable->setText( QDir::toNativeSeparators(QString(vtkPlusConfig::GetInstance()->GetEditorApplicationExecutable())) );
  ui.lineEdit_ImageDirectory->setText( QDir::toNativeSeparators(QString(vtkPlusConfig::GetInstance()->GetImageDirectory())) );

  // Install event filters to capture key event for dumping raw buffer data
	this->installEventFilter(this); //TODO make this a button or something, this is unreliable
}

//-----------------------------------------------------------------------------

ConfigurationToolbox::~ConfigurationToolbox()
{
}

//-----------------------------------------------------------------------------

void ConfigurationToolbox::Initialize()
{
	LOG_TRACE("ConfigurationToolbox::Initialize"); 

  // No actions needed when tab becomes active
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

void ConfigurationToolbox::ConnectToDevicesByConfigFile(std::string aConfigFile)
{
	LOG_TRACE("ConfigurationToolbox::ConnectToDevicesByConfigFile");

  QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));

  // If not empty, then try to connect; empty parameter string means disconnect
  if (STRCASECMP(aConfigFile.c_str(), "") != 0) {
    // Read configuration
    vtkSmartPointer<vtkXMLDataElement> configRootElement = vtkSmartPointer<vtkXMLDataElement>::Take(
      vtkXMLUtilities::ReadElementFromFile(aConfigFile.c_str()));
    if (configRootElement == NULL) {	
      LOG_ERROR("Unable to read configuration from file " << aConfigFile); 
      return;
    }

    vtkPlusConfig::GetInstance()->SetDeviceSetConfigurationData(configRootElement); 

	  // If connection has been successfully created then this action should disconnect
	  if (! m_DeviceSetSelectorWidget->GetConnectionSuccessful()) {
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
		  if (m_ParentMainWindow->GetToolVisualizer()->StartDataCollection() != PLUS_SUCCESS) {
			  LOG_ERROR("Unable to start collecting data!");
			  m_DeviceSetSelectorWidget->SetConnectionSuccessful(false);
			  m_ToolStateDisplayWidget->InitializeTools(NULL, false);

		  } else {
        // Successful connection
			  m_DeviceSetSelectorWidget->SetConnectionSuccessful(true);

        // Load device models based on the new configuration
        m_ParentMainWindow->GetToolVisualizer()->InitializeDeviceVisualization();

        vtkPlusConfig::GetInstance()->SaveApplicationConfigurationToFile();

			  if (m_ToolStateDisplayWidget->InitializeTools(m_ParentMainWindow->GetToolVisualizer()->GetDataCollector(), true)) {
				  ui.toolStateDisplayWidget->setMinimumHeight(m_ToolStateDisplayWidget->GetDesiredHeight());
				  ui.toolStateDisplayWidget->setMaximumHeight(m_ToolStateDisplayWidget->GetDesiredHeight());
			  }
		  }

		  // Close dialog
		  connectDialog->done(0);
      connectDialog->hide();
      delete connectDialog;

      // Re-enable main window
     	m_ParentMainWindow->setEnabled(true);
    }

  } else { // Disconnect
		vtkDataCollector* dataCollector = m_ParentMainWindow->GetToolVisualizer()->GetDataCollector();
		if ((dataCollector != NULL) && (dataCollector->GetConnected())) {

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
          QString dirName = QFileDialog::getExistingDirectory(NULL, QString( tr( "Open output directory for buffer dump files" ) ), vtkPlusConfig::GetInstance()->GetOutputDirectory());
	        if ( (dirName.isNull()) || (m_ParentMainWindow->GetToolVisualizer()->DumpBuffersToDirectory(dirName.toAscii().data()) != PLUS_SUCCESS) ) {
              LOG_ERROR("Writing raw buffers into files failed (output directory: " << dirName.toAscii().data() << ")!");
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

  vtkPlusLogger::Instance()->SetLogLevel(aLevel+1);

	LOG_INFO("Log level changed to: " << ui.comboBox_LogLevel->currentText().ascii() << " (" << aLevel+1 << ")" );

  vtkPlusConfig::GetInstance()->SaveApplicationConfigurationToFile();
}

//-----------------------------------------------------------------------------

void ConfigurationToolbox::SelectEditorApplicationExecutable()
{
	LOG_TRACE("ConfigurationToolbox::SelectEditorApplicationExecutable");

	// File open dialog for selecting phantom definition xml
	QString filter = QString( tr( "Executables ( *.exe );;" ) );
  QString fileName = QFileDialog::getOpenFileName(NULL, QString( tr( "Select XML editor application" ) ), "C:", filter);
	if (fileName.isNull()) {
		return;
	}

  vtkPlusConfig::GetInstance()->SetEditorApplicationExecutable(fileName.toAscii().data());
  vtkPlusConfig::GetInstance()->SaveApplicationConfigurationToFile();

  ui.lineEdit_EditorApplicationExecutable->setText(fileName);
}

//-----------------------------------------------------------------------------

void ConfigurationToolbox::SelectImageDirectory()
{
  LOG_TRACE("ConfigurationToolbox::SelectImageDirectory"); 

	// Directory open dialog for selecting configuration directory 
  QString dirName = QFileDialog::getExistingDirectory(NULL, QString( tr( "Select image directory" ) ), QString(vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationDirectory()));
	if (dirName.isNull()) {
		return;
	}

  // Save the selected directory to config object
  vtkPlusConfig::GetInstance()->SetImageDirectory(dirName.toAscii().data());
  vtkPlusConfig::GetInstance()->SaveApplicationConfigurationToFile();

  ui.lineEdit_ImageDirectory->setText(dirName);
}
