#include "DeviceSetSelectorWidget.h"

#include <QFileDialog>
#include <QString>
#include <QMessageBox>
#include <QDomDocument>

//-----------------------------------------------------------------------------

DeviceSetSelectorWidget::DeviceSetSelectorWidget(QWidget* aParent)
	: QWidget(aParent)
	, m_ConfigurationDirectory("")
{
	ui.setupUi(this);

	connect( ui.pushButton_OpenConfigurationDirectory, SIGNAL( pressed() ), this, SLOT( OpenConfigurationDirectoryClicked() ) );
	connect( ui.pushButton_Connect, SIGNAL( pressed() ), this, SLOT( InvokeConnect() ) );
	connect( ui.comboBox_DeviceSet, SIGNAL( currentIndexChanged(int) ), this, SLOT( DeviceSetSelected(int) ) );
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

		emit ConfigurationDirectoryChanged(m_ConfigurationDirectory.toStdString());
	}
}

//-----------------------------------------------------------------------------

void DeviceSetSelectorWidget::InvokeConnect()
{
	emit ConnectToDevicesByConfigFileInvoked(ui.comboBox_DeviceSet->itemData(ui.comboBox_DeviceSet->currentIndex()).toStringList().at(0).toStdString());
}

//-----------------------------------------------------------------------------

void DeviceSetSelectorWidget::DeviceSetSelected(int aIndex)
{
	if ((aIndex < 0) || (aIndex >= ui.comboBox_DeviceSet->count())) {
		return;
	}

	ui.label_Description->setText(ui.comboBox_DeviceSet->currentText()
		+ " (" + ui.comboBox_DeviceSet->itemData(aIndex).toStringList().at(0) + ")\n\n"
		+ ui.comboBox_DeviceSet->itemData(aIndex).toStringList().at(1));

	ui.comboBox_DeviceSet->setToolTip(ui.comboBox_DeviceSet->currentText() + " (" + ui.comboBox_DeviceSet->itemData(aIndex).toStringList().at(0) + ")");
}

//-----------------------------------------------------------------------------

void DeviceSetSelectorWidget::SetConfigurationDirectory(std::string aDirectory)
{
	if (ParseDirectory(QString::fromStdString(aDirectory))) {
		m_ConfigurationDirectory = QString::fromStdString(aDirectory);

		ui.lineEdit_ConfigurationDirectory->setText(m_ConfigurationDirectory);
		ui.lineEdit_ConfigurationDirectory->setToolTip(m_ConfigurationDirectory);
	} else {
		ui.lineEdit_ConfigurationDirectory->setText(tr("Invalid configuration directory"));
		ui.lineEdit_ConfigurationDirectory->setToolTip("No valid configuration files in directory, please select another");
	}
}

//-----------------------------------------------------------------------------

PlusStatus DeviceSetSelectorWidget::ParseDirectory(QString aDirectory)
{
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
