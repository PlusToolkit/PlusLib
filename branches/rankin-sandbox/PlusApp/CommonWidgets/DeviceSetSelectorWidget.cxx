/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#include "DeviceSetSelectorWidget.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QDomDocument>

#ifdef _WIN32
#include "Shellapi.h"
#endif

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
  connect( ui.pushButton_ResetTracker, SIGNAL( clicked() ), this, SLOT( ResetTrackerButtonClicked() ) );

  ui.pushButton_ResetTracker->setVisible(false);

  SetConfigurationDirectory(vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationDirectory());
}

//-----------------------------------------------------------------------------

DeviceSetSelectorWidget::~DeviceSetSelectorWidget()
{
}

//-----------------------------------------------------------------------------

PlusStatus DeviceSetSelectorWidget::SetConfigurationDirectory(QString aDirectory)
{
  LOG_TRACE("DeviceSetSelectorWidget::SetConfigurationDirectory(" << aDirectory.toAscii().data() << ")");

  // Try to parse up directory and set UI according to the result
  if (ParseDirectory(aDirectory))
  {
    ui.lineEdit_ConfigurationDirectory->setText(aDirectory);
    ui.lineEdit_ConfigurationDirectory->setToolTip(aDirectory);

    m_ConfigurationDirectory = aDirectory;

    return PLUS_SUCCESS;
  }
  else
  {
    ui.lineEdit_ConfigurationDirectory->setText(tr("Invalid configuration directory"));
    ui.lineEdit_ConfigurationDirectory->setToolTip("No valid configuration files in directory, please select another");

    ui.textEdit_Description->setTextColor(QColor(Qt::darkRed));
    ui.textEdit_Description->setText("Selected directory does not contain valid device set configuration files!\n\nPlease select another directory");

    return PLUS_FAIL;
  }
}

//-----------------------------------------------------------------------------

void DeviceSetSelectorWidget::OpenConfigurationDirectory()
{
  LOG_TRACE("DeviceSetSelectorWidget::OpenConfigurationDirectoryClicked"); 

  // Directory open dialog for selecting configuration directory 
  QString dirName = QFileDialog::getExistingDirectory(NULL, QString( tr( "Open configuration directory" ) ), m_ConfigurationDirectory);
  if (dirName.isNull())
  {
    return;
  }

  if (SetConfigurationDirectory(dirName) == PLUS_SUCCESS)
  {
    // Save the selected directory to config object
    vtkPlusConfig::GetInstance()->SetDeviceSetConfigurationDirectory(dirName.toAscii().data());
    vtkPlusConfig::GetInstance()->SaveApplicationConfigurationToFile();
  }
  else
  {
    LOG_ERROR("Unable to open selected directory!");
  }
}

//-----------------------------------------------------------------------------

void DeviceSetSelectorWidget::InvokeConnect()
{
  LOG_TRACE("DeviceSetSelectorWidget::InvokeConnect"); 

  if ( ui.comboBox_DeviceSet->currentIndex() < 0 )
  {
    // combo box is empty 
    return; 
  }

  ui.pushButton_Connect->setEnabled(false);

  emit ConnectToDevicesByConfigFileInvoked(ui.comboBox_DeviceSet->itemData(ui.comboBox_DeviceSet->currentIndex()).toStringList().at(0).toAscii().data());
}

//-----------------------------------------------------------------------------

std::string DeviceSetSelectorWidget::GetSelectedDeviceSetDescription()
{
  LOG_TRACE("DeviceSetSelectorWidget::GetSelectedDeviceSetDescription"); 

  return ui.comboBox_DeviceSet->itemData(ui.comboBox_DeviceSet->currentIndex()).toStringList().at(1).toAscii().data(); 
}

//-----------------------------------------------------------------------------

void DeviceSetSelectorWidget::InvokeDisconnect()
{
  LOG_TRACE("DeviceSetSelectorWidget::InvokeDisconnect"); 

  RefreshFolder();

  ui.pushButton_Connect->setEnabled(false);

  emit ConnectToDevicesByConfigFileInvoked("");
}

//-----------------------------------------------------------------------------

void DeviceSetSelectorWidget::DeviceSetSelected(int aIndex)
{
  LOG_TRACE("DeviceSetSelectorWidget::DeviceSetSelected(" << aIndex << ")"); 

  if ((aIndex < 0) || (aIndex >= ui.comboBox_DeviceSet->count()))
  {
    return;
  }

  ui.textEdit_Description->setTextColor(QColor(Qt::black));

  ui.textEdit_Description->setText(
    ui.comboBox_DeviceSet->itemData(aIndex).toStringList().at(1)
    //+ "\n\n(" + ui.comboBox_DeviceSet->itemData(aIndex).toStringList().at(0) + ")"
    );

  ui.comboBox_DeviceSet->setToolTip(ui.comboBox_DeviceSet->currentText() + " (" + ui.comboBox_DeviceSet->itemData(aIndex).toStringList().at(0) + ")");

  QString configurationFilePath = ui.comboBox_DeviceSet->itemData(ui.comboBox_DeviceSet->currentIndex()).toStringList().at(0); 

  emit DeviceSetSelected( configurationFilePath.toAscii().data() ); 
}

//-----------------------------------------------------------------------------

void DeviceSetSelectorWidget::SetConnectionSuccessful(bool aConnectionSuccessful)
{
  LOG_TRACE("DeviceSetSelectorWidget::SetConnectionSuccessful(" << (aConnectionSuccessful?"true":"false") << ")"); 

  m_ConnectionSuccessful = aConnectionSuccessful;

  // If connect button has been pushed
  if (ui.pushButton_Connect->text() == "Connect")
  {
    if (m_ConnectionSuccessful)
    {
      ui.pushButton_Connect->setText(tr("Disconnect"));
      ui.comboBox_DeviceSet->setEnabled(false);

      ui.textEdit_Description->setTextColor(QColor(Qt::black));
      ui.textEdit_Description->setText("Connection successful!\n\n"
        + ui.comboBox_DeviceSet->itemData(ui.comboBox_DeviceSet->currentIndex()).toStringList().at(1)
        //+ "\n\n(" + ui.comboBox_DeviceSet->itemData(ui.comboBox_DeviceSet->currentIndex()).toStringList().at(0) + ")"
        );

      // Change the function to be invoked on clicking on the now Disconnect button to InvokeDisconnect
      disconnect( ui.pushButton_Connect, SIGNAL( clicked() ), this, SLOT( InvokeConnect() ) );
      connect( ui.pushButton_Connect, SIGNAL( clicked() ), this, SLOT( InvokeDisconnect() ) );

      // Set last used device set config file
      vtkPlusConfig::GetInstance()->SetDeviceSetConfigurationFileName(ui.comboBox_DeviceSet->itemData(ui.comboBox_DeviceSet->currentIndex()).toStringList().at(0).toAscii().data());
    }
    else
    {
      ui.textEdit_Description->setTextColor(QColor(Qt::darkRed));
      ui.textEdit_Description->setText("Connection failed!\n\nPlease select another device set and try again!");
    }
  }
  else
  { // If disconnect button has been pushed
    if (! m_ConnectionSuccessful)
    {
      ui.pushButton_Connect->setText(tr("Connect"));
      ui.comboBox_DeviceSet->setEnabled(true);

      ui.textEdit_Description->setTextColor(QColor(Qt::black));
      ui.textEdit_Description->setText(
        ui.comboBox_DeviceSet->itemData(ui.comboBox_DeviceSet->currentIndex()).toStringList().at(1)
        //+ "\n\n(" + ui.comboBox_DeviceSet->itemData(ui.comboBox_DeviceSet->currentIndex()).toStringList().at(0) + ")"
        );

      // Change the function to be invoked on clicking on the now Connect button to InvokeConnect
      disconnect( ui.pushButton_Connect, SIGNAL( clicked() ), this, SLOT( InvokeDisconnect() ) );
      connect( ui.pushButton_Connect, SIGNAL( clicked() ), this, SLOT( InvokeConnect() ) );
    }
    else
    {
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
  LOG_TRACE("DeviceSetSelectorWidget::ParseDirectory(" << aDirectory.toAscii().data() << ")"); 

  QDir configDir(aDirectory);
  QStringList fileList(configDir.entryList());

  if (fileList.size() > 200)
  {
    if (QMessageBox::No == QMessageBox::question(this, tr("Many files in the directory"), tr("There are more than 200 files in the selected directory. Do you really want to continue parsing the files?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes))
    {
      return PLUS_FAIL;
    }
  }

  int lastSelectedDeviceSetIndex = -1; 

  // Block signals before we add items
  ui.comboBox_DeviceSet->blockSignals(true); 
  ui.comboBox_DeviceSet->clear();

  QStringListIterator filesIterator(fileList);
  while (filesIterator.hasNext())
  {
    QString fileName(configDir.absoluteFilePath(filesIterator.next()));
    QString extension = fileName.mid(fileName.lastIndexOf("."));
    if( extension.compare(QString(".xml")) != 0 )
    {
      continue;
    }

    QFile file(fileName);
    QDomDocument doc;

    // If file is not readable then skip
    if (!file.open(QIODevice::ReadOnly))
    {
      continue;
    }

    // If parsing is successful then check the content and if data collection config file is found then add it to combo box
    if (doc.setContent(&file))
    {
      QDomElement docElem = doc.documentElement();

      // Check if the root element is PlusConfiguration and contains a DataCollection child
      if (! docElem.tagName().compare("PlusConfiguration"))
      {
        QDomNodeList list = docElem.elementsByTagName("DataCollection");

        if (list.count() > 0)
        { // If it has a DataCollection children then use the first one
          docElem = list.at(0).toElement();
        }
        else
        { // If it does not have a DataCollection then it cannot be used for connecting
          continue;
        }
      }
      else
      {
        continue;
      }

      // Add the name attribute to the first node named DeviceSet to the combo box
      QDomNodeList list(doc.elementsByTagName("DeviceSet"));
      if (list.size() <= 0)
      {
        continue;
      }

      QDomElement elem = list.at(0).toElement();

      QStringList datas;
      datas.append(fileName);
      datas.append(elem.attribute("Description", tr("Description not found")));
      QVariant userData(datas);

      QString name(elem.attribute("Name"));
      if (name.isEmpty())
      {
        LOG_WARNING("Name field is empty in device set configuration file '" << fileName.toAscii().data() << "', it is not added to the list");
        continue;
      }

      // Check if the same name already exists
      int foundIndex = ui.comboBox_DeviceSet->findText(name, Qt::MatchExactly);
      if (foundIndex > -1)
      {
        LOG_WARNING("Device set with name '" << name.toAscii().data() << "' already found, configuration file '" << fileName.toAscii().data() << "' is not added to the list");
        continue;
      }

      ui.comboBox_DeviceSet->addItem(name, userData);
      int currentIndex = ui.comboBox_DeviceSet->findText(name, Qt::MatchExactly);

      // Add tooltip
      ui.comboBox_DeviceSet->setItemData(currentIndex, name, Qt::ToolTipRole); 

      // If this item is the same as in the config file, select it by default
      if ( QDir::toNativeSeparators(QString(vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationFileName())) == QDir::toNativeSeparators(fileName) )
      {
        lastSelectedDeviceSetIndex = currentIndex; 
      }
    }
    else
    {
      LOG_WARNING("Unable to parse file '" << fileName.toLatin1().constData() << "' as an XML. It will not appear in the device set configuration file list!");
    }
  }

  // If no valid configuration files have been parsed then warn user
  if (ui.comboBox_DeviceSet->count() < 1)
  {
    LOG_ERROR("Selected directory ("<<aDirectory.toAscii().data()<<") does not contain valid device set configuration files!");
    return PLUS_FAIL;
  }

  // Set current index to default so that setting the last selected item raises the event even if it is the first item
  ui.comboBox_DeviceSet->setCurrentIndex(-1);

  // Unblock signals after we add items
  ui.comboBox_DeviceSet->blockSignals(false); 

  ui.comboBox_DeviceSet->setCurrentIndex(lastSelectedDeviceSetIndex); 

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------

void DeviceSetSelectorWidget::RefreshFolder()
{
  LOG_TRACE("DeviceSetSelectorWidget::RefreshFolderClicked"); 

  if (ParseDirectory(QString(vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationDirectory())) != PLUS_SUCCESS)
  {
    LOG_ERROR("Parsing up configuration files failed in: " << vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationDirectory());
  }
}

//-----------------------------------------------------------------------------

void DeviceSetSelectorWidget::EditConfiguration()
{
  LOG_TRACE("DeviceSetSelectorWidget::EditConfiguration"); 

  QString configurationFilePath;
  int deviceSetIndex=ui.comboBox_DeviceSet->currentIndex();
  if (deviceSetIndex>=0)
  {
    configurationFilePath=ui.comboBox_DeviceSet->itemData(deviceSetIndex).toStringList().at(0);
  }
  else
  {
    LOG_ERROR("Cannot edit configuration file. No configuration is selected."); 
    return;
  }
  QString editorApplicationExecutable(vtkPlusConfig::GetInstance()->GetEditorApplicationExecutable());

  if (editorApplicationExecutable.right(4).compare(QString(".exe")) != 0)
  {
    LOG_ERROR("Invalid XML editor application!");
    return;
  }

#ifdef _WIN32
  wchar_t wcharApplication[1024];
  wchar_t wcharFile[1024];

  QFileInfo fileInfo( QDir::toNativeSeparators( configurationFilePath ) );

  QString file = fileInfo.absoluteFilePath();
  int lenFile = file.toWCharArray( wcharFile );
  wcharFile[lenFile] = '\0';

  int lenApplication = editorApplicationExecutable.toWCharArray( wcharApplication );
  wcharApplication[lenApplication] = '\0';

  ShellExecuteW( 0, L"open", wcharApplication, wcharFile, NULL, SW_MAXIMIZE );
#else
  LOG_ERROR("Opening configuration files from the program is not supported on this platform.");
#endif
}

//-----------------------------------------------------------------------------

void DeviceSetSelectorWidget::ResetTrackerButtonClicked()
{
  LOG_TRACE("DeviceSetSelectorWidget::ResetTrackerButtonClicked()");

  emit ResetTracker();
}

//-----------------------------------------------------------------------------

void DeviceSetSelectorWidget::ShowResetTrackerButton( bool aValue )
{
  ui.pushButton_ResetTracker->setVisible(aValue);
}
