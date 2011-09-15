#include "DeviceSetSelectorWidget.h"

#include <QFileDialog>
#include <QString>
#include <QMessageBox>
#include <QDomDocument>
#include <QSettings>

#include "vtkPlusConfig.h"

#include "Shellapi.h"

//-----------------------------------------------------------------------------

DeviceSetSelectorWidget::DeviceSetSelectorWidget(QWidget* aParent)
	: QWidget(aParent)
	, m_ConnectionSuccessful(false)
{
	ui.setupUi(this);

	connect( ui.pushButton_OpenConfigurationDirectory, SIGNAL( clicked() ), this, SLOT( OpenConfigurationDirectory() ) );
	connect( ui.pushButton_Connect, SIGNAL( clicked() ), this, SLOT( InvokeConnect() ) );
	connect( ui.pushButton_RefreshFolder, SIGNAL( clicked() ), this, SLOT( RefreshFolder() ) );
	connect( ui.pushButton_EditConfiguration, SIGNAL( clicked() ), this, SLOT( EditConfiguration() ) );
	connect( ui.comboBox_DeviceSet, SIGNAL( currentIndexChanged(int) ), this, SLOT( DeviceSetSelected(int) ) );

  ui.comboBox_DeviceSet->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon); 
}

//-----------------------------------------------------------------------------

DeviceSetSelectorWidget::~DeviceSetSelectorWidget()
{
}

//-----------------------------------------------------------------------------

void DeviceSetSelectorWidget::SetConfigurationDirectoryFromRegistry()
{
  LOG_TRACE("DeviceSetSelectorWidget:SetConfigurationDirectoryFromRegistry"); 

  // Get configuration directory from registry if possible
	QSettings settings( QSettings::NativeFormat, QSettings::UserScope, "PerkLab", "Common" );
	m_ConfigurationDirectory = settings.value("ConfigurationDirectory", "").toString();
	if (! m_ConfigurationDirectory.isEmpty()) {
		SetConfigurationDirectory(m_ConfigurationDirectory.toStdString(), true);
	}
}

//-----------------------------------------------------------------------------

void DeviceSetSelectorWidget::OpenConfigurationDirectory()
{
  LOG_TRACE("DeviceSetSelectorWidget::OpenConfigurationDirectoryClicked"); 

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
}

//-----------------------------------------------------------------------------

void DeviceSetSelectorWidget::InvokeConnect()
{
	LOG_TRACE("DeviceSetSelectorWidget::InvokeConnect"); 

  ui.pushButton_Connect->setEnabled(false);

	emit ConnectToDevicesByConfigFileInvoked(ui.comboBox_DeviceSet->itemData(ui.comboBox_DeviceSet->currentIndex()).toStringList().at(0).toStdString());
}

//-----------------------------------------------------------------------------

std::string DeviceSetSelectorWidget::GetSelectedDeviceSetDescription()
{
	LOG_TRACE("DeviceSetSelectorWidget::GetSelectedDeviceSetDescription"); 

  return ui.comboBox_DeviceSet->itemData(ui.comboBox_DeviceSet->currentIndex()).toStringList().at(1).toStdString(); 
}

//-----------------------------------------------------------------------------

void DeviceSetSelectorWidget::InvokeDisconnect()
{
	LOG_TRACE("DeviceSetSelectorWidget::InvokeDisconnect"); 

  ui.pushButton_Connect->setEnabled(false);

	emit ConnectToDevicesByConfigFileInvoked("");
}

//-----------------------------------------------------------------------------

void DeviceSetSelectorWidget::DeviceSetSelected(int aIndex)
{
	LOG_TRACE("DeviceSetSelectorWidget::DeviceSetSelected(" << aIndex << ")"); 

	if ((aIndex < 0) || (aIndex >= ui.comboBox_DeviceSet->count())) {
		return;
	}

	ui.textEdit_Description->setTextColor(QColor(Qt::black));

	ui.textEdit_Description->setText(
		ui.comboBox_DeviceSet->itemData(aIndex).toStringList().at(1)
		//+ "\n\n(" + ui.comboBox_DeviceSet->itemData(aIndex).toStringList().at(0) + ")"
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
	LOG_TRACE("DeviceSetSelectorWidget::SetConfigurationDirectory(" << aDirectory << ", " << (aForce?"true":"false") << ")"); 
  
  // Set configuration directory before we parse it 
  std::string oldDirectory = vtkPlusConfig::GetInstance()->GetConfigurationDirectory();
  vtkPlusConfig::GetInstance()->SetConfigurationDirectory(aDirectory.c_str());

	if (m_ConfigurationDirectory.isEmpty() || aForce) {
		if (ParseDirectory(QString::fromStdString(aDirectory))) {
			m_ConfigurationDirectory = QString::fromStdString(aDirectory);

			ui.lineEdit_ConfigurationDirectory->setText(m_ConfigurationDirectory);
			ui.lineEdit_ConfigurationDirectory->setToolTip(m_ConfigurationDirectory);

			// Notify the application about the directory change
			emit ConfigurationDirectoryChanged(m_ConfigurationDirectory.toStdString());
		} else {
      vtkPlusConfig::GetInstance()->SetConfigurationDirectory(oldDirectory.c_str());
			ui.lineEdit_ConfigurationDirectory->setText(tr("Invalid configuration directory"));
			ui.lineEdit_ConfigurationDirectory->setToolTip("No valid configuration files in directory, please select another");
		}
	}
}

//-----------------------------------------------------------------------------

void DeviceSetSelectorWidget::SetConnectionSuccessful(bool aConnectionSuccessful)
{
	LOG_TRACE("DeviceSetSelectorWidget::SetConnectionSuccessful(" << (aConnectionSuccessful?"true":"false") << ")"); 

	m_ConnectionSuccessful = aConnectionSuccessful;

	// If connect button has been pushed
	if (ui.pushButton_Connect->text() == "Connect") {
		if (m_ConnectionSuccessful) {
			ui.pushButton_Connect->setText(tr("Disconnect"));
      ui.comboBox_DeviceSet->setEnabled(false);

      ui.textEdit_Description->setTextColor(QColor(Qt::black));
	    ui.textEdit_Description->setText("Connection successful!\n\n"
        + ui.comboBox_DeviceSet->itemData(ui.comboBox_DeviceSet->currentIndex()).toStringList().at(1)
		    + "\n\n(" + ui.comboBox_DeviceSet->itemData(ui.comboBox_DeviceSet->currentIndex()).toStringList().at(0) + ")"
		    );

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
      ui.comboBox_DeviceSet->setEnabled(true);

      ui.textEdit_Description->setTextColor(QColor(Qt::black));
	    ui.textEdit_Description->setText(
		    ui.comboBox_DeviceSet->itemData(ui.comboBox_DeviceSet->currentIndex()).toStringList().at(1)
		    + "\n\n(" + ui.comboBox_DeviceSet->itemData(ui.comboBox_DeviceSet->currentIndex()).toStringList().at(0) + ")"
		    );

      // Change the function to be invoked on clicking on the now Connect button to InvokeConnect
			disconnect( ui.pushButton_Connect, SIGNAL( clicked() ), this, SLOT( InvokeDisconnect() ) );
			connect( ui.pushButton_Connect, SIGNAL( clicked() ), this, SLOT( InvokeConnect() ) );
		} else {
			LOG_ERROR("Disconnect failed!");
		}
	}

  ui.pushButton_Connect->setEnabled(true);
}

//-----------------------------------------------------------------------------

bool DeviceSetSelectorWidget::GetConnectionSuccessful()
{
	LOG_TRACE("DeviceSetSelectorWidget::GetConnectionSuccessful"); 

	return m_ConnectionSuccessful;
}

//-----------------------------------------------------------------------------

PlusStatus DeviceSetSelectorWidget::ParseDirectory(QString aDirectory)
{
	LOG_TRACE("DeviceSetSelectorWidget::ParseDirectory(" << aDirectory.toStdString() << ")"); 

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
	LOG_TRACE("DeviceSetSelectorWidget::SetComboBoxMinWidth(" << minWidth << ")"); 

  ui.comboBox_DeviceSet->setMinimumWidth(minWidth);
}

//-----------------------------------------------------------------------------

void DeviceSetSelectorWidget::RefreshFolder()
{
	LOG_TRACE("DeviceSetSelectorWidget::RefreshFolderClicked"); 

  if (ParseDirectory(m_ConfigurationDirectory) != PLUS_SUCCESS) {
    LOG_ERROR("Parsing up configuration files failed in: " << m_ConfigurationDirectory.toStdString().c_str());
  }
}

//-----------------------------------------------------------------------------

void DeviceSetSelectorWidget::EditConfiguration()
{
	LOG_TRACE("DeviceSetSelectorWidget::EditConfiguration"); 

  QString configurationFilePath = ui.comboBox_DeviceSet->itemData(ui.comboBox_DeviceSet->currentIndex()).toStringList().at(0);

	wchar_t wcharFile[1024];

	QFileInfo fileInfo( QDir::toNativeSeparators( configurationFilePath ) );

  QString file = fileInfo.absoluteFilePath();
	int lenFile = file.toWCharArray( wcharFile );
	wcharFile[lenFile] = '\0';

	ShellExecuteW( 0, L"edit", wcharFile, NULL, NULL, SW_MAXIMIZE );
}
