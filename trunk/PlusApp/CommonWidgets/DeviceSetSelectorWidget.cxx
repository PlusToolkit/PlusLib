#include "DeviceSetSelectorWidget.h"

#include <QFileDialog>
#include <QString>
#include <QMessageBox>
#include <QDomDocument>
#include <QSettings>

#include "vtkFileFinder.h"

//-----------------------------------------------------------------------------

DeviceSetSelectorWidget::DeviceSetSelectorWidget(QWidget* aParent)
	: QWidget(aParent)
	, m_ConnectionSuccessful(false)
{
	ui.setupUi(this);

	connect( ui.pushButton_OpenConfigurationDirectory, SIGNAL( clicked() ), this, SLOT( OpenConfigurationDirectoryClicked() ) );
	connect( ui.pushButton_Connect, SIGNAL( clicked() ), this, SLOT( InvokeConnect() ) );
	connect( ui.comboBox_DeviceSet, SIGNAL( currentIndexChanged(int) ), this, SLOT( DeviceSetSelected(int) ) );

  ui.comboBox_DeviceSet->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon); 
}

//-----------------------------------------------------------------------------

DeviceSetSelectorWidget::~DeviceSetSelectorWidget()
{

}

void DeviceSetSelectorWidget::SetConfigurationDirectoryFromRegistry()
{
  // Get configuration directory from registry if possible
	QSettings settings( QSettings::NativeFormat, QSettings::UserScope, "PerkLab", "Common" );
	m_ConfigurationDirectory = settings.value("ConfigurationDirectory", "").toString();
	if (! m_ConfigurationDirectory.isEmpty()) {
		SetConfigurationDirectory(m_ConfigurationDirectory.toStdString(), true);
	}
}

//-----------------------------------------------------------------------------

void DeviceSetSelectorWidget::OpenConfigurationDirectoryClicked()
{
	LOG_TRACE("DeviceSetSelectorWidget: Open configuration directory clicked"); 

	// Directory open dialog for selecting configuration directory 
	QString dirName = QFileDialog::getExistingDirectory(NULL, QString( tr( "Open configuration directory" ) ), m_ConfigurationDirectory);
	if (dirName.isNull()) {
		return;
	}

  this->SetConfigurationDirectory(dirName.toStdString(), true); 
  
  // Write the selected directory to registry
  QSettings settings( QSettings::NativeFormat, QSettings::UserScope, "PerkLab", "Common" );
  settings.setValue("ConfigurationDirectory", dirName);
  settings.sync();
  return; 

	// Parse up selected directory and populate combobox
	if (ParseDirectory(dirName) == PLUS_SUCCESS) {
		m_ConfigurationDirectory = dirName;

		ui.lineEdit_ConfigurationDirectory->setText(dirName);
		ui.lineEdit_ConfigurationDirectory->setToolTip(dirName);

		// Write the selected directory to registry
		QSettings settings( QSettings::NativeFormat, QSettings::UserScope, "PerkLab", "Common" );
		settings.setValue("ConfigurationDirectory", dirName);
		settings.sync();

		// Notify the application about the directory change
		emit ConfigurationDirectoryChanged(m_ConfigurationDirectory.toStdString());
	}
}

//-----------------------------------------------------------------------------

void DeviceSetSelectorWidget::InvokeConnect()
{
	LOG_TRACE("ToolStateDisplayWidget::InvokeConnect"); 

	emit ConnectToDevicesByConfigFileInvoked(ui.comboBox_DeviceSet->itemData(ui.comboBox_DeviceSet->currentIndex()).toStringList().at(0).toStdString());
}

//-----------------------------------------------------------------------------

void DeviceSetSelectorWidget::InvokeDisconnect()
{
	LOG_TRACE("ToolStateDisplayWidget::InvokeDisconnect"); 

	emit ConnectToDevicesByConfigFileInvoked("");
}

//-----------------------------------------------------------------------------

void DeviceSetSelectorWidget::DeviceSetSelected(int aIndex)
{
	LOG_TRACE("ToolStateDisplayWidget::DeviceSetSelected(" << aIndex << ")"); 

	if ((aIndex < 0) || (aIndex >= ui.comboBox_DeviceSet->count())) {
		return;
	}

	ui.textEdit_Description->setTextColor(QColor(Qt::black));

	ui.textEdit_Description->setText(
		ui.comboBox_DeviceSet->itemData(aIndex).toStringList().at(1)
		+ "\n\n(" + ui.comboBox_DeviceSet->itemData(aIndex).toStringList().at(0) + ")"
		);

	ui.comboBox_DeviceSet->setToolTip(ui.comboBox_DeviceSet->currentText() + " (" + ui.comboBox_DeviceSet->itemData(aIndex).toStringList().at(0) + ")");

  QString configurationFilePath = ui.comboBox_DeviceSet->itemData(ui.comboBox_DeviceSet->currentIndex()).toStringList().at(0); 
  // Write the selected configuration file path to registry
  QSettings settings( QSettings::NativeFormat, QSettings::UserScope, "PerkLab", "Common" );
  settings.setValue("ConfigurationFilePath", configurationFilePath);
  settings.sync();

  emit DeviceSetSelected( configurationFilePath.toStdString() ); 
}

//-----------------------------------------------------------------------------

void DeviceSetSelectorWidget::SetConfigurationDirectory(std::string aDirectory, bool aForce)
{
	LOG_TRACE("ToolStateDisplayWidget::SetConfigurationDirectory(" << aDirectory << ", " << (aForce?"true":"false") << ")"); 
  
  // Set configuration directory before we parse it 
  std::string oldDirectory = vtkFileFinder::GetInstance()->GetConfigurationDirectory();
  vtkFileFinder::GetInstance()->SetConfigurationDirectory(aDirectory.c_str());

	if (m_ConfigurationDirectory.isEmpty() || aForce) {
		if (ParseDirectory(QString::fromStdString(aDirectory))) {
			m_ConfigurationDirectory = QString::fromStdString(aDirectory);

			ui.lineEdit_ConfigurationDirectory->setText(m_ConfigurationDirectory);
			ui.lineEdit_ConfigurationDirectory->setToolTip(m_ConfigurationDirectory);

			// Notify the application about the directory change
			emit ConfigurationDirectoryChanged(m_ConfigurationDirectory.toStdString());
		} else {
      vtkFileFinder::GetInstance()->SetConfigurationDirectory(oldDirectory.c_str());
			ui.lineEdit_ConfigurationDirectory->setText(tr("Invalid configuration directory"));
			ui.lineEdit_ConfigurationDirectory->setToolTip("No valid configuration files in directory, please select another");
		}
	}
}

//-----------------------------------------------------------------------------

void DeviceSetSelectorWidget::SetConnectionSuccessful(bool aConnectionSuccessful)
{
	LOG_TRACE("ToolStateDisplayWidget::SetConnectionSuccessful(" << (aConnectionSuccessful?"true":"false") << ")"); 

	m_ConnectionSuccessful = aConnectionSuccessful;

	// If connect button has been pushed
	if (ui.pushButton_Connect->text() == "Connect") {
		if (m_ConnectionSuccessful) {
			ui.pushButton_Connect->setText(tr("Disconnect"));

			// Change the function to be invoked on clicking on the now Disconnect button to InvokeDisconnect
			disconnect( ui.pushButton_Connect, SIGNAL( clicked() ), this, SLOT( InvokeConnect() ) );
			connect( ui.pushButton_Connect, SIGNAL( clicked() ), this, SLOT( InvokeDisconnect() ) );
		} else {
			ui.textEdit_Description->setTextColor(QColor(Qt::darkRed));
			ui.textEdit_Description->setText("Connection failed!\n\nPlease select another device set and try again!");
		}
	} else { // If disconnect button has been pushed
		if (! m_ConnectionSuccessful) {
			ui.pushButton_Connect->setText(tr("Connect"));

			// Change the function to be invoked on clicking on the now Connect button to InvokeConnect
			disconnect( ui.pushButton_Connect, SIGNAL( clicked() ), this, SLOT( InvokeDisconnect() ) );
			connect( ui.pushButton_Connect, SIGNAL( clicked() ), this, SLOT( InvokeConnect() ) );
		} else {
			LOG_ERROR("Disconnect failed!");
		}
	}
}

//-----------------------------------------------------------------------------

bool DeviceSetSelectorWidget::GetConnectionSuccessful()
{
	LOG_TRACE("ToolStateDisplayWidget::GetConnectionSuccessful"); 

	return m_ConnectionSuccessful;
}

//-----------------------------------------------------------------------------

PlusStatus DeviceSetSelectorWidget::ParseDirectory(QString aDirectory)
{
	LOG_TRACE("ToolStateDisplayWidget::ParseDirectory(" << aDirectory.toStdString() << ")"); 

	

	QDir configDir(aDirectory);
	QStringList fileList(configDir.entryList());

	if (fileList.size() > 200) {
		if (QMessageBox::No == QMessageBox::question(this, tr("Many files in the directory"), tr("There are more than 200 files in the selected directory. Do you really want to continue parsing the files?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes)) {
			return PLUS_FAIL;
		}
	}

  // Get last selected configuration file from registry if possible
	QSettings settings( QSettings::NativeFormat, QSettings::UserScope, "PerkLab", "Common" );
	QString lastSelectedConfigFile = settings.value("ConfigurationFilePath", "").toString();
  int lastSelectedDeviceSetIndex(0); 
  
  // Block signals before we add items
  ui.comboBox_DeviceSet->blockSignals(true); 
  ui.comboBox_DeviceSet->clear();

	QStringListIterator filesIterator(fileList);
	while (filesIterator.hasNext()) {
		QString fileName(configDir.absoluteFilePath(filesIterator.next()));
		QFile file(fileName);
		QDomDocument doc;

		// If file is not readable then skip
		if (!file.open(QIODevice::ReadOnly)) {
			continue;
		}

		// If parsing is successful then check the content and if data collection config file is found then add it to combo box
		if (doc.setContent(&file)) {
			QDomElement docElem = doc.documentElement();
			
			// Check if the root element is PlusConfiguration and contains a USDataCollection child
			if (! docElem.tagName().compare("PlusConfiguration")) {
				QDomNodeList list = docElem.elementsByTagName("USDataCollection");

				if (list.count() > 0) { // If it has a USDataCollection children then use the first one
					docElem = list.at(0).toElement();
				} else { // If it does not have a USDataCollection then it cannot be used for connecting
					continue;
				}
			} else {
				continue;
			}

			// Add the name attribute to the first node named DeviceSet to the combo box
			QDomNodeList list(doc.elementsByTagName("DeviceSet"));
			if (list.size() <= 0) {
				continue;
			}

			QDomElement elem = list.at(0).toElement();

			QStringList datas;
			datas.append(fileName);
			datas.append(elem.attribute("Description", tr("Description not found")));
			QVariant userData(datas);

			QString name(elem.attribute("Name"));
			if (name.isEmpty()) {
				continue;
			}

			ui.comboBox_DeviceSet->addItem(name, userData);
      int currentIndex = ui.comboBox_DeviceSet->findText(name, Qt::MatchExactly); 
      // Add tooltip
      ui.comboBox_DeviceSet->setItemData(currentIndex, name, Qt::ToolTipRole); 

      // If this item is the same as in the registry, select it by default
      if ( lastSelectedConfigFile == fileName )
      {
        lastSelectedDeviceSetIndex = currentIndex; 
      }

		}
	}

  ui.comboBox_DeviceSet->setCurrentIndex(-1); 

  // Unblock signals after we add items
  ui.comboBox_DeviceSet->blockSignals(false); 

  ui.comboBox_DeviceSet->setCurrentIndex(lastSelectedDeviceSetIndex); 

	return PLUS_SUCCESS;
}


//-----------------------------------------------------------------------------

void DeviceSetSelectorWidget::SetComboBoxMinWidth(int minWidth)
{
  ui.comboBox_DeviceSet->setMinimumWidth(minWidth);
}
