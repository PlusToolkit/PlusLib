#include "ConfigFileSaverDialog.h"

#include <QFileDialog>
#include <QString>
#include <QSettings>

#include "vtkXMLUtilities.h"
#include "vtkConfigurationTools.h"
#include "vtkXMLDataElement.h"

//-----------------------------------------------------------------------------

ConfigFileSaverDialog::ConfigFileSaverDialog(QWidget* aParent, vtkXMLDataElement* aConfigurationData)
	: QDialog(aParent)
  , m_ConfigurationData(aConfigurationData)
{
	ui.setupUi(this);

	connect( ui.pushButton_OpenDestinationDirectory, SIGNAL( clicked() ), this, SLOT( OpenDestinationDirectoryClicked() ) );
	connect( ui.pushButton_Save, SIGNAL( clicked() ), this, SLOT( SaveClicked() ) );

  SetDestinationDirectoryFromRegistry();

  ReadConfiguration();
}

//-----------------------------------------------------------------------------

ConfigFileSaverDialog::~ConfigFileSaverDialog()
{
}

//-----------------------------------------------------------------------------

void ConfigFileSaverDialog::SetDestinationDirectoryFromRegistry()
{
  LOG_TRACE("ConfigFileSaverDialog::SetDestinationDirectoryFromRegistry");

  // Get configuration directory from registry if possible
	QSettings settings( QSettings::NativeFormat, QSettings::UserScope, "PerkLab", "Common" );
	m_DestinationDirectory = settings.value("ConfigurationDirectory", "").toString();
	if (! m_DestinationDirectory.isEmpty()) {
		SetDestinationDirectory(m_DestinationDirectory.toStdString());
	}
}

//-----------------------------------------------------------------------------

void ConfigFileSaverDialog::OpenDestinationDirectoryClicked()
{
  LOG_TRACE("ConfigFileSaverDialog::OpenDestinationDirectoryClicked"); 

	// Directory open dialog for selecting configuration directory 
	QString dirName = QFileDialog::getExistingDirectory(NULL, QString( tr( "Select destination directory" ) ), m_DestinationDirectory);
	if (dirName.isNull()) {
		return;
	}

  this->SetDestinationDirectory(dirName.toStdString()); 

	m_DestinationDirectory = dirName;

	ui.lineEdit_DestinationDirectory->setText(dirName);
	ui.lineEdit_DestinationDirectory->setToolTip(dirName);
}

//-----------------------------------------------------------------------------

void ConfigFileSaverDialog::SetDestinationDirectory(std::string aDirectory)
{
	LOG_TRACE("ConfigFileSaverDialog::SetDestinationDirectory(" << aDirectory << ")"); 

	m_DestinationDirectory = QString::fromStdString(aDirectory);

	ui.lineEdit_DestinationDirectory->setText(m_DestinationDirectory);
	ui.lineEdit_DestinationDirectory->setToolTip(m_DestinationDirectory);
}

//-----------------------------------------------------------------------------

PlusStatus ConfigFileSaverDialog::ReadConfiguration()
{
  LOG_TRACE("ConfigFileSaverDialog::ReadConfiguration");

  //Find Device set element
	vtkSmartPointer<vtkXMLDataElement> usDataCollection = m_ConfigurationData->FindNestedElementWithName("USDataCollection");
	if (usDataCollection == NULL) {
		LOG_ERROR("No USDataCollection element is found in the XML tree!");
		return PLUS_FAIL;
	}

	vtkSmartPointer<vtkXMLDataElement> deviceSet = usDataCollection->FindNestedElementWithName("DeviceSet");
	if (deviceSet == NULL) {
		LOG_ERROR("No DeviceSet element is found in the XML tree!");
		return PLUS_FAIL;
	}

  // Get name and description
  const char* name = deviceSet->GetAttribute("Name");
  if ((name == NULL) || (STRCASECMP(name, "") == 0)) {
    LOG_WARNING("Name attribute cannot be found in DeviceSet element!");
    return PLUS_FAIL;
  }

  const char* description = deviceSet->GetAttribute("Description");
  if ((description == NULL) || (STRCASECMP(description, "") == 0)) {
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

  // Find Device set element
	vtkSmartPointer<vtkXMLDataElement> usDataCollection = m_ConfigurationData->FindNestedElementWithName("USDataCollection");
	if (usDataCollection == NULL) {
		LOG_ERROR("No USDataCollection element is found in the XML tree!");
		return;
	}

	vtkSmartPointer<vtkXMLDataElement> deviceSet = usDataCollection->FindNestedElementWithName("DeviceSet");
	if (deviceSet == NULL) {
		LOG_ERROR("No DeviceSet element is found in the XML tree!");
		return;
	}

  // Set name and description to XML
  deviceSet->SetAttribute("Name", ui.lineEdit_DeviceSetName->text());
  deviceSet->SetAttribute("Description", ui.textEdit_Description->text());

  // Display file save dialog and save XML
	QString filter = QString( tr( "XML files ( *.xml );;" ) );
  QString destinationFile = QString("%1/%2").arg(m_DestinationDirectory).arg(QString::fromStdString(vtkConfigurationTools::GetInstance()->GetNewConfigurationFileName()));
  QString fileName = QFileDialog::getSaveFileName(NULL, tr("Save result configuration XML"), destinationFile, filter);

	if (! fileName.isNull() ) {
    m_ConfigurationData->PrintXML(fileName.toStdString().c_str());
	}

  accept();
}
