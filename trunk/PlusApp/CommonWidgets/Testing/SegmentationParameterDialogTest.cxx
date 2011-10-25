/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#include "SegmentationParameterDialogTest.h"

#include "vtkXMLUtilities.h"

#include "vtkTracker.h"
#include "vtkDataCollector.h"
#include "DeviceSetSelectorWidget.h"
#include "SegmentationParameterDialog.h"
#include "ConfigFileSaverDialog.h"
#include "StatusIcon.h"

//-----------------------------------------------------------------------------

SegmentationParameterDialogTest::SegmentationParameterDialogTest(QWidget *parent, Qt::WFlags flags)
  : QDialog(parent, flags)
  , m_DeviceSetSelectorWidget(NULL)
  , m_DataCollector(NULL)
{
  this->setMinimumSize(480, 320);
  this->setMaximumSize(480, 320);

  // Create device set selector widget
	m_DeviceSetSelectorWidget = new DeviceSetSelectorWidget(this);
  m_DeviceSetSelectorWidget->setMinimumWidth(472);
  m_DeviceSetSelectorWidget->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
	connect( m_DeviceSetSelectorWidget, SIGNAL( ConnectToDevicesByConfigFileInvoked(std::string) ), this, SLOT( ConnectToDevicesByConfigFile(std::string) ) );

  // Create status icon
  StatusIcon* statusIcon = new StatusIcon(this);

  // Create save configuration button
  m_SaveButton = new QPushButton(tr("Save configuration..."), this);
  m_SaveButton->setMinimumWidth(120);
  m_SaveButton->setEnabled(false);
	connect( m_SaveButton, SIGNAL( clicked() ), this, SLOT( SaveConfigurationClicked() ) );

  // Insert widgets into placeholders
  QGridLayout* mainGrid = new QGridLayout(this, 2, 2, 4, 4, "");
  mainGrid->setColumnMinimumWidth(0, 296);
  mainGrid->addWidget(m_DeviceSetSelectorWidget, 0, 0, 1, 2, Qt::AlignHCenter);
  mainGrid->addWidget(m_SaveButton, 1, 0, Qt::AlignRight);
  mainGrid->addWidget(statusIcon, 1, 1, Qt::AlignRight);
  this->setLayout(mainGrid);
}

//-----------------------------------------------------------------------------

SegmentationParameterDialogTest::~SegmentationParameterDialogTest()
{
  if (m_DataCollector != NULL) {
	  m_DataCollector->Stop();
  }
  m_DataCollector = NULL;
}

//-----------------------------------------------------------------------------

void SegmentationParameterDialogTest::ConnectToDevicesByConfigFile(std::string aConfigFile)
{
	LOG_TRACE("SegmentationParameterDialogTest::ConnectToDevicesByConfigFile");

  QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));

  // If not empty, then try to connect; empty parameter string means disconnect
  if (STRCASECMP(aConfigFile.c_str(), "") != 0) {
    // Read configuration
    vtkSmartPointer<vtkXMLDataElement> configRootElement = vtkXMLUtilities::ReadElementFromFile(aConfigFile.c_str());
    if (configRootElement == NULL) {	
      LOG_ERROR("Unable to read configuration from file " << aConfigFile); 
      return;
    }

    vtkPlusConfig::GetInstance()->SetDeviceSetConfigurationData(configRootElement); 

    // If connection has been successfully created then this action should disconnect
    if (! m_DeviceSetSelectorWidget->GetConnectionSuccessful()) {
	    LOG_INFO("Connect to devices"); 

      // Disable main window
   	  this->setEnabled(false);

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
	    if (this->StartDataCollection() != PLUS_SUCCESS) {
		    LOG_ERROR("Unable to start collecting data!");
		    m_DeviceSetSelectorWidget->SetConnectionSuccessful(false);
        m_SaveButton->setEnabled(false);

	    } else {
        // Successful connection
		    m_DeviceSetSelectorWidget->SetConnectionSuccessful(true);

        vtkPlusConfig::GetInstance()->SaveApplicationConfigurationToFile();

        QApplication::restoreOverrideCursor();

        // Show segmentation parameter dialog
        SegmentationParameterDialog* segmentationParamDialog = new SegmentationParameterDialog(this, m_DataCollector);
        segmentationParamDialog->exec();

        delete segmentationParamDialog;

        // Disconnect after closing the segmentation parameter dialog (else the user would have to disconnect manually anyway)
		    m_DataCollector->Stop();
		    m_DataCollector->Disconnect();

		    m_DeviceSetSelectorWidget->SetConnectionSuccessful(false);

        m_SaveButton->setEnabled(true);
	    }

	    // Close dialog
	    connectDialog->done(0);
      connectDialog->hide();
      delete connectDialog;

      // Re-enable main window
   	  this->setEnabled(true);
    }
  }

  QApplication::restoreOverrideCursor();
}

//-----------------------------------------------------------------------------

PlusStatus SegmentationParameterDialogTest::StartDataCollection()
{
  LOG_TRACE("SegmentationParameterDialogTest::StartDataCollection"); 

  // Stop data collection if already started
  if (m_DataCollector != NULL) {
	  m_DataCollector->Stop();
  } else {
	  m_DataCollector = vtkDataCollector::New();
  }

  // Initialize data collector and read configuration
  if (m_DataCollector->ReadConfiguration(vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData()) != PLUS_SUCCESS) {
	  return PLUS_FAIL;
  }

  if (m_DataCollector->Connect() != PLUS_SUCCESS) {
	  return PLUS_FAIL;
  }

  if (m_DataCollector->Start() != PLUS_SUCCESS) {
	  return PLUS_FAIL;
  }

  if ((m_DataCollector->GetTracker() == NULL) || (m_DataCollector->GetTracker()->GetNumberOfTools() < 1)) {
	  LOG_INFO("Tracker is not initialized"); 
  }

  if (! m_DataCollector->GetConnected()) {
	  LOG_ERROR("Unable to initialize DataCollector!"); 
	  return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

void SegmentationParameterDialogTest::SaveConfigurationClicked()
{
	LOG_TRACE("SegmentationParameterDialogTest::SaveConfigurationClicked"); 

  ConfigFileSaverDialog* configSaverDialog = new ConfigFileSaverDialog(this);
  configSaverDialog->exec();

  delete configSaverDialog;
}
