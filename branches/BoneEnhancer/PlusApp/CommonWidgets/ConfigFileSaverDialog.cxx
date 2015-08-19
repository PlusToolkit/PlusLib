/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#include "ConfigFileSaverDialog.h"

#include <QFileDialog>
#include <QString>
#include <QSettings>

#include "vtkXMLUtilities.h"
#include "vtkXMLDataElement.h"

//-----------------------------------------------------------------------------

ConfigFileSaverDialog::ConfigFileSaverDialog(QWidget* aParent)
	: QDialog(aParent)
{
	ui.setupUi(this);

	connect( ui.pushButton_OpenDestinationDirectory, SIGNAL( clicked() ), this, SLOT( OpenDestinationDirectoryClicked() ) );
	connect( ui.pushButton_Save, SIGNAL( clicked() ), this, SLOT( SaveClicked() ) );

  SetDestinationDirectory(vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationDirectory());

  ReadConfiguration();
}

//-----------------------------------------------------------------------------

ConfigFileSaverDialog::~ConfigFileSaverDialog()
{
}

//-----------------------------------------------------------------------------

void ConfigFileSaverDialog::OpenDestinationDirectoryClicked()
{
  LOG_TRACE("ConfigFileSaverDialog::OpenDestinationDirectoryClicked"); 

	// Directory open dialog for selecting configuration directory 
	QString dirName = QFileDialog::getExistingDirectory(NULL, QString( tr( "Select destination directory" ) ), m_DestinationDirectory);
	if (dirName.isNull())
  {
		return;
	}

  this->SetDestinationDirectory(dirName.toLatin1().constData()); 

	m_DestinationDirectory = dirName;

	ui.lineEdit_DestinationDirectory->setText(dirName);
	ui.lineEdit_DestinationDirectory->setToolTip(dirName);
}

//-----------------------------------------------------------------------------

void ConfigFileSaverDialog::SetDestinationDirectory(std::string aDirectory)
{
	LOG_TRACE("ConfigFileSaverDialog::SetDestinationDirectory(" << aDirectory << ")"); 

	m_DestinationDirectory = aDirectory.c_str();

	ui.lineEdit_DestinationDirectory->setText(m_DestinationDirectory);
	ui.lineEdit_DestinationDirectory->setToolTip(m_DestinationDirectory);
}

//-----------------------------------------------------------------------------

PlusStatus ConfigFileSaverDialog::ReadConfiguration()
{
  LOG_TRACE("ConfigFileSaverDialog::ReadConfiguration");

  // Find Device set element
	vtkXMLDataElement* dataCollection = vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData()->FindNestedElementWithName("DataCollection");
	if (dataCollection == NULL)
  {
		LOG_ERROR("No DataCollection element is found in the XML tree!");
		return PLUS_FAIL;
	}

	vtkXMLDataElement* deviceSet = dataCollection->FindNestedElementWithName("DeviceSet");
	if (deviceSet == NULL)
  {
		LOG_ERROR("No DeviceSet element is found in the XML tree!");
		return PLUS_FAIL;
	}

  // Get name and description
  const char* name = deviceSet->GetAttribute("Name");
  if ((name == NULL) || (STRCASECMP(name, "") == 0))
  {
    LOG_WARNING("Name attribute cannot be found in DeviceSet element!");
    return PLUS_FAIL;
  }

  const char* description = deviceSet->GetAttribute("Description");
  if ((description == NULL) || (STRCASECMP(description, "") == 0))
  {
    LOG_WARNING("Description attribute cannot be found in DeviceSet element!");
    return PLUS_FAIL;
  }

  // Set text field values
  ui.lineEdit_DeviceSetName->setText(name);
  ui.textEdit_Description->setText(description);

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

void ConfigFileSaverDialog::SaveClicked()
{

  LOG_TRACE("ConfigFileSaverDialog::SaveClicked");

  // Get root element
  vtkXMLDataElement* configRootElement = vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData();
	if (configRootElement == NULL)
  {
		LOG_ERROR("No configuration XML found!");
		return;
	}

  // Find Device set element
	vtkXMLDataElement* dataCollection = configRootElement->FindNestedElementWithName("DataCollection");
	if (dataCollection == NULL)
  {
		LOG_ERROR("No DataCollection element is found in the XML tree!");
		return;
	}

	vtkXMLDataElement* deviceSet = dataCollection->FindNestedElementWithName("DeviceSet");
	if (deviceSet == NULL)
  {
		LOG_ERROR("No DeviceSet element is found in the XML tree!");
		return;
	}
  // Set name and description to XML
	deviceSet->SetAttribute("Name", ui.lineEdit_DeviceSetName->text().toLatin1().constData());
	deviceSet->SetAttribute("Description", ui.textEdit_Description->toPlainText().toLatin1().constData());

  // Display file save dialog and save XML
  QString filter = QString( tr( "XML files ( *.xml );;" ) );
  QString destinationFile = QString("%1/%2").arg(m_DestinationDirectory).arg(vtkPlusConfig::GetInstance()->GetNewDeviceSetConfigurationFileName().c_str());
  QString fileName = QFileDialog::getSaveFileName(NULL, tr("Save result configuration XML"), destinationFile, filter);

	if (! fileName.isNull() )
  {
    PlusCommon::PrintXML(fileName.toLatin1().constData(), vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData());
    LOG_INFO("Device set configuration saved as '" << fileName.toLatin1().constData() << "'");
	}

  accept();
}
