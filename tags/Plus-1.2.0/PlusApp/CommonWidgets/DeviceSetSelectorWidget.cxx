#include "DeviceSetSelectorWidget.h"

#include <QFileDialog>
#include <QString>
#include <QMessageBox>
#include <QDomDocument>
#include <QSettings>

//-----------------------------------------------------------------------------

DeviceSetSelectorWidget::DeviceSetSelectorWidget(QWidget* aParent)
	: QWidget(aParent)
	, m_ConnectionSuccessful(false)
{
	ui.setupUi(this);

	connect( ui.pushButton_OpenConfigurationDirectory, SIGNAL( clicked() ), this, SLOT( OpenConfigurationDirectoryClicked() ) );
	connect( ui.pushButton_Connect, SIGNAL( clicked() ), this, SLOT( InvokeConnect() ) );
	connect( ui.comboBox_DeviceSet, SIGNAL( currentIndexChanged(int) ), this, SLOT( DeviceSetSelected(int) ) );

	// Get configuration directory from registry if possible
	QSettings settings( QSettings::NativeFormat, QSettings::UserScope, "PerkLab", "Common" );
	m_ConfigurationDirectory = settings.value("ConfigurationDirectory", "").toString();
	if (! m_ConfigurationDirectory.isEmpty()) {
		SetConfigurationDirectory(m_ConfigurationDirectory.toStdString(), true);
	}
}

//-----------------------------------------------------------------------------

DeviceSetSelectorWidget::~DeviceSetSelectorWidget()
{
}

//-----------------------------------------------------------------------------

void DeviceSetSelectorWidget::OpenConfigurationDirectoryClicked()
{
	LOG_TRACE("DeviceSetSelectorWidget: Open configuration directory clicked"); 

	// Directory open dialog for selecting phantom 
	QString dirName = QFileDialog::getExistingDirectory(NULL, QString( tr( "Open configuration directory" ) ), m_ConfigurationDirectory);
	if (dirName.isNull()) {
		return;
	}

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

	if (m_ConnectionSuccessful) {
		ui.pushButton_Connect->setText(tr("Disconnect"));

		// Change the function to be invoked on clicking on the now Disconnect button to InvokeDisconnect
		disconnect( ui.pushButton_Connect, SIGNAL( clicked() ), this, SLOT( InvokeConnect() ) );
		connect( ui.pushButton_Connect, SIGNAL( clicked() ), this, SLOT( InvokeDisconnect() ) );
	} else {
		ui.textEdit_Description->setTextColor(QColor(Qt::darkRed));
		ui.textEdit_Description->setText("Connection failed!\n\nPlease select another device set and try again!");
	}
}

//-----------------------------------------------------------------------------

void DeviceSetSelectorWidget::InvokeDisconnect()
{
	LOG_TRACE("ToolStateDisplayWidget::InvokeDisconnect"); 

	emit ConnectToDevicesByConfigFileInvoked("");

	if (! m_ConnectionSuccessful) {
		ui.pushButton_Connect->setText(tr("Connect"));

		// Change the function to be invoked on clicking on the now Connect button to InvokeConnect
		disconnect( ui.pushButton_Connect, SIGNAL( clicked() ), this, SLOT( InvokeDisconnect() ) );
		connect( ui.pushButton_Connect, SIGNAL( clicked() ), this, SLOT( InvokeConnect() ) );
	} else {
		LOG_ERROR("Disconnect failed!");
	}
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
}

//-----------------------------------------------------------------------------

void DeviceSetSelectorWidget::SetConfigurationDirectory(std::string aDirectory, bool aForce)
{
	LOG_TRACE("ToolStateDisplayWidget::SetConfigurationDirectory(" << aDirectory << ", " << (aForce?"true":"false") << ")"); 

	if (m_ConfigurationDirectory.isEmpty() || aForce) {
		if (ParseDirectory(QString::fromStdString(aDirectory))) {
			m_ConfigurationDirectory = QString::fromStdString(aDirectory);

			ui.lineEdit_ConfigurationDirectory->setText(m_ConfigurationDirectory);
			ui.lineEdit_ConfigurationDirectory->setToolTip(m_ConfigurationDirectory);

			// Notify the application about the directory change
			emit ConfigurationDirectoryChanged(m_ConfigurationDirectory.toStdString());
		} else {
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

	ui.comboBox_DeviceSet->clear();

	QDir configDir(aDirectory);
	QStringList fileList(configDir.entryList());

	if (fileList.size() > 200) {
		if (QMessageBox::No == QMessageBox::question(this, tr("Many files in the directory"), tr("There are more than 200 files in the selected directory. Do you really want to continue parsing the files?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes)) {
			return PLUS_FAIL;
		}
	}

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
			
			// If the root element is not USDataCollection then skip
			if (docElem.tagName().compare("USDataCollection")) {
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
		}
	}

	return PLUS_SUCCESS;
}


//-----------------------------------------------------------------------------

void DeviceSetSelectorWidget::SetComboBoxMinWidth(int minWidth)
{
  ui.comboBox_DeviceSet->setMinimumWidth(minWidth);
}
