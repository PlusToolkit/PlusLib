/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

// Local includes
#include "QPlusDeviceSetSelectorWidget.h"

// PlusLib includes
//#include <vtkIGSIOTransformRepository.h>

// Qt includes
#include <QAbstractItemView>
#include <QAction>
#include <QComboBox>
#include <QDesktopServices>
#include <QDesktopWidget>
#include <QDomDocument>
#include <QFileDialog>
#include <QKeyEvent>
#include <QMenu>
#include <QMessageBox>
#include <QMimeData>
#include <QProcess>
#include <QUrl>

#if !defined(_WIN32)
  #include <unistd.h>
#endif

enum DataItemRoles
{
  FileNameRole = Qt::UserRole + 0,
  DescriptionRole = Qt::UserRole + 1
};

//-----------------------------------------------------------------------------

QPlusDeviceSetSelectorWidget::QPlusDeviceSetSelectorWidget(QWidget* aParent)
  : QWidget(aParent)
  , m_ConnectionSuccessful(false)
  , m_EditMenu(NULL)
  , m_EditorSelectAction(NULL)
  , m_EditApplicationConfigFileAction(NULL)
  , m_DeviceSetComboBoxMaximumSizeRatio(-1)
{
  // Accept drop of files from explorer (and others)
  setAcceptDrops(true);

  ui.setupUi(this);

  // Make the directory and configuration selector textbox/combobox the same height as the pushbuttons
  // to make the layout nicer (they are shown in the same row, so it does not look nice if their height is different)
  ui.lineEdit_ConfigurationDirectory->setFixedHeight(ui.pushButton_OpenConfigurationDirectory->sizeHint().height());
  ui.comboBox_DeviceSet->setFixedHeight(ui.pushButton_EditConfiguration->sizeHint().height());

  ui.pushButton_EditConfiguration->setContextMenuPolicy(Qt::CustomContextMenu);

  connect(ui.pushButton_OpenConfigurationDirectory, SIGNAL(clicked()), this, SLOT(OpenConfigurationDirectory()));
  connect(ui.pushButton_Connect, SIGNAL(clicked()), this, SLOT(InvokeConnect()));
  connect(ui.pushButton_RefreshFolder, SIGNAL(clicked()), this, SLOT(RefreshFolder()));
  connect(ui.pushButton_EditConfiguration, SIGNAL(clicked()), this, SLOT(EditConfiguration()));
  connect(ui.comboBox_DeviceSet, SIGNAL(currentIndexChanged(int)), this, SLOT(DeviceSetSelected(int)));
  connect(ui.pushButton_ResetTracker, SIGNAL(clicked()), this, SLOT(ResetTrackerButtonClicked()));
  ui.pushButton_ResetTracker->setVisible(false);

  connect(ui.pushButton_EditConfiguration, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(ShowEditContextMenu(QPoint)));

  SetConfigurationDirectory(vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationDirectory().c_str());
}

//----------------------------------------------------------------------------
void QPlusDeviceSetSelectorWidget::SetDeviceSetComboBoxMaximumSizeRatio(double ratio)
{
  this->m_DeviceSetComboBoxMaximumSizeRatio = ratio;
}

//-----------------------------------------------------------------------------
QPlusDeviceSetSelectorWidget::~QPlusDeviceSetSelectorWidget()
{
  if (this->m_EditMenu)
  {
    delete this->m_EditMenu;
    m_EditMenu = NULL;
  }
  if (this->m_EditorSelectAction)
  {
    disconnect(m_EditorSelectAction, &QAction::triggered, this, &QPlusDeviceSetSelectorWidget::SelectEditor);
    delete this->m_EditorSelectAction;
    m_EditorSelectAction = NULL;
  }
  if (this->m_EditApplicationConfigFileAction)
  {
    disconnect(m_EditApplicationConfigFileAction, &QAction::triggered, this, &QPlusDeviceSetSelectorWidget::EditAppConfig);
    delete this->m_EditApplicationConfigFileAction;
    m_EditApplicationConfigFileAction = NULL;
  }
}

//-----------------------------------------------------------------------------
PlusStatus QPlusDeviceSetSelectorWidget::SetConfigurationDirectory(const QString& aDirectory)
{
  LOG_TRACE("DeviceSetSelectorWidget::SetConfigurationDirectory(" << aDirectory.toStdString() << ")");

  // Try to parse up directory and set UI according to the result
  if (ParseDirectory(aDirectory))
  {
    ui.lineEdit_ConfigurationDirectory->setText(aDirectory);
    ui.lineEdit_ConfigurationDirectory->setToolTip(aDirectory);

    m_ConfigurationDirectory = aDirectory;

    // Save the selected directory to config object
    vtkPlusConfig::GetInstance()->SetDeviceSetConfigurationDirectory(m_ConfigurationDirectory.toStdString());
    vtkPlusConfig::GetInstance()->SaveApplicationConfigurationToFile();

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

//----------------------------------------------------------------------------
PlusStatus QPlusDeviceSetSelectorWidget::SetConfigurationFile(const QString& aFilename)
{
  QString fileName = QDir::toNativeSeparators(aFilename);
  QFileInfo info(fileName);
  if (info.exists())
  {
    if (this->SetConfigurationDirectory(info.absoluteDir().absolutePath()) == PLUS_SUCCESS)
    {
      QString fileName = QDir::toNativeSeparators(info.absoluteFilePath());
      ui.comboBox_DeviceSet->setCurrentIndex(ui.comboBox_DeviceSet->findData(fileName));
      return PLUS_SUCCESS;
    }
  }

  return PLUS_FAIL;
}

//-----------------------------------------------------------------------------
void QPlusDeviceSetSelectorWidget::OpenConfigurationDirectory()
{
  LOG_TRACE("DeviceSetSelectorWidget::OpenConfigurationDirectoryClicked");

  // Directory open dialog for selecting configuration directory
  QString dirName = QFileDialog::getExistingDirectory(NULL, QString(tr("Open configuration directory")), m_ConfigurationDirectory);
  if (dirName.isNull())
  {
    return;
  }

  if (SetConfigurationDirectory(dirName) != PLUS_SUCCESS)
  {
    LOG_ERROR("Unable to open selected directory!");
  }
}

//-----------------------------------------------------------------------------
void QPlusDeviceSetSelectorWidget::InvokeConnect()
{
  LOG_TRACE("DeviceSetSelectorWidget::InvokeConnect");

  if (ui.comboBox_DeviceSet->currentIndex() < 0)
  {
    // combo box is empty
    return;
  }

  // Save selected device set configuration file name
  std::string configFile = ui.comboBox_DeviceSet->itemData(ui.comboBox_DeviceSet->currentIndex(), FileNameRole).toString().toStdString();
  vtkPlusConfig::GetInstance()->SetDeviceSetConfigurationFileName(configFile);
  vtkPlusConfig::GetInstance()->SaveApplicationConfigurationToFile();

  ui.pushButton_Connect->setEnabled(false);
  QCoreApplication::processEvents();

  emit ConnectToDevicesByConfigFileInvoked(configFile);
}

//-----------------------------------------------------------------------------
std::string QPlusDeviceSetSelectorWidget::GetSelectedDeviceSetDescription()
{
  LOG_TRACE("DeviceSetSelectorWidget::GetSelectedDeviceSetDescription");

  return ui.comboBox_DeviceSet->itemData(ui.comboBox_DeviceSet->currentIndex(), DescriptionRole).toString().toStdString();
}

//-----------------------------------------------------------------------------
void QPlusDeviceSetSelectorWidget::InvokeDisconnect()
{
  LOG_TRACE("DeviceSetSelectorWidget::InvokeDisconnect");

  RefreshFolder();

  ui.pushButton_Connect->setEnabled(false);
  QCoreApplication::processEvents();

  emit ConnectToDevicesByConfigFileInvoked("");
}

//-----------------------------------------------------------------------------
void QPlusDeviceSetSelectorWidget::DeviceSetSelected(int aIndex)
{
  LOG_TRACE("DeviceSetSelectorWidget::DeviceSetSelected(" << aIndex << ")");

  if ((aIndex < 0) || (aIndex >= ui.comboBox_DeviceSet->count()))
  {
    return;
  }

  ui.textEdit_Description->setTextColor(QColor(Qt::black));

  ui.textEdit_Description->setText(ui.comboBox_DeviceSet->itemData(aIndex, DescriptionRole).toString());

  ui.comboBox_DeviceSet->setToolTip(ui.comboBox_DeviceSet->currentText() + "\n" + ui.comboBox_DeviceSet->itemData(aIndex, FileNameRole).toString());

  QString configurationFilePath = ui.comboBox_DeviceSet->itemData(ui.comboBox_DeviceSet->currentIndex(), FileNameRole).toString();

  emit DeviceSetSelected(configurationFilePath.toStdString());
}

//-----------------------------------------------------------------------------
void QPlusDeviceSetSelectorWidget::SetConnectionSuccessful(bool aConnectionSuccessful)
{
  LOG_TRACE("DeviceSetSelectorWidget::SetConnectionSuccessful(" << (aConnectionSuccessful ? "true" : "false") << ")");

  m_ConnectionSuccessful = aConnectionSuccessful;

  // If connect button has been pushed
  if (!ui.pushButton_Connect->property("connected").toBool())
  {
    if (m_ConnectionSuccessful)
    {
      ui.pushButton_Connect->setText(tr("Disconnect"));
      ui.comboBox_DeviceSet->setEnabled(false);

      ui.textEdit_Description->setTextColor(QColor(Qt::black));
      m_DescriptionPrefix = "Connection successful!";
      m_DescriptionBody = ui.comboBox_DeviceSet->itemData(ui.comboBox_DeviceSet->currentIndex(), DescriptionRole).toString();
      this->UpdateDescriptionText();

      // Change the function to be invoked on clicking on the now Disconnect button to InvokeDisconnect
      disconnect(ui.pushButton_Connect, SIGNAL(clicked()), this, SLOT(InvokeConnect()));
      connect(ui.pushButton_Connect, SIGNAL(clicked()), this, SLOT(InvokeDisconnect()));

      // Set last used device set config file
      vtkPlusConfig::GetInstance()->SetDeviceSetConfigurationFileName(ui.comboBox_DeviceSet->itemData(ui.comboBox_DeviceSet->currentIndex(), FileNameRole).toString().toStdString());

      ui.pushButton_Connect->setProperty("connected", QVariant(true));
    }
    else
    {
      ui.textEdit_Description->setTextColor(QColor(Qt::darkRed));
      m_DescriptionPrefix = "Connection failed!";
      m_DescriptionBody = "Please select another device set and try again!";
      this->UpdateDescriptionText();
    }
  }
  else
  {
    // If disconnect button has been pushed
    if (!m_ConnectionSuccessful)
    {
      ui.pushButton_Connect->setProperty("connected", QVariant(false));
      ui.pushButton_Connect->setText(tr("Connect"));
      ui.comboBox_DeviceSet->setEnabled(true);

      ui.textEdit_Description->setTextColor(QColor(Qt::black));
      if (ui.comboBox_DeviceSet->currentIndex() >= 0)
      {
        m_DescriptionPrefix = "";
        m_DescriptionBody = ui.comboBox_DeviceSet->itemData(ui.comboBox_DeviceSet->currentIndex(), DescriptionRole).toString();
        this->UpdateDescriptionText();
      }

      // Change the function to be invoked on clicking on the now Connect button to InvokeConnect
      disconnect(ui.pushButton_Connect, SIGNAL(clicked()), this, SLOT(InvokeDisconnect()));
      connect(ui.pushButton_Connect, SIGNAL(clicked()), this, SLOT(InvokeConnect()));
    }
    else
    {
      LOG_ERROR("Disconnect failed!");
    }
  }

  ui.pushButton_Connect->setEnabled(true);
}

//-----------------------------------------------------------------------------
bool QPlusDeviceSetSelectorWidget::GetConnectionSuccessful()
{
  LOG_TRACE("DeviceSetSelectorWidget::GetConnectionSuccessful");

  return m_ConnectionSuccessful;
}

//-----------------------------------------------------------------------------
PlusStatus QPlusDeviceSetSelectorWidget::ParseDirectory(const QString& aDirectory)
{
  LOG_TRACE("DeviceSetSelectorWidget::ParseDirectory(" << aDirectory.toStdString() << ")");

  QDir configDir(aDirectory);
  QStringList fileList(configDir.entryList());

  if (fileList.size() > 200)
  {
    if (QMessageBox::No == QMessageBox::question(this, tr("Many files in the directory"), tr("There are more than 200 files in the selected directory. Do you really want to continue parsing the files?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes))
    {
      return PLUS_FAIL;
    }
  }

  // Block signals before we add items
  ui.comboBox_DeviceSet->blockSignals(true);
  ui.comboBox_DeviceSet->clear();

  QStringListIterator filesIterator(fileList);
  QHash<QString, int> deviceSetVersion;

  while (filesIterator.hasNext())
  {
    QString fileName = QDir::toNativeSeparators(QString(configDir.absoluteFilePath(filesIterator.next())));
    QString extension = fileName.mid(fileName.lastIndexOf("."));
    if (!igsioCommon::IsEqualInsensitive(extension.toStdString(), ".xml"))
    {
      continue;
    }

    QFile file(fileName);
    QFileInfo fileInfo(fileName);
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
      if (!docElem.tagName().compare("PlusConfiguration", Qt::CaseInsensitive))
      {
        QDomNodeList list = docElem.elementsByTagName("DataCollection");

        if (list.count() > 0)
        {
          // If it has a DataCollection children then use the first one
          docElem = list.at(0).toElement();
        }
        else
        {
          // If it does not have a DataCollection then it cannot be used for connecting
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

      // Detect previous calibrations and if so, display details
      QString calibDetails;
      vtkSmartPointer<vtkXMLDataElement> configRootElement = vtkSmartPointer<vtkXMLDataElement>::Take(vtkXMLUtilities::ReadElementFromFile(fileName.toStdString().c_str()));
      if (configRootElement != NULL)
      {
        auto tr = vtkSmartPointer<vtkIGSIOTransformRepository>::New();
        if (tr->ReadConfiguration(configRootElement) == PLUS_SUCCESS)
        {
          QString pivotString;
          pivotString = FindCalibrationDetails(doc, tr, "vtkPlusPivotCalibrationAlgo", "Pivot calibration: ", "ObjectPivotPointCoordinateFrame", "ObjectMarkerCoordinateFrame");
          if (!pivotString.isEmpty())
          {
            calibDetails += pivotString;
          }

          QString phantomString;
          phantomString = FindCalibrationDetails(doc, tr, "vtkPlusPhantomLandmarkRegistrationAlgo", "Phantom calibration: ", "PhantomCoordinateFrame", "ReferenceCoordinateFrame");
          if (!phantomString.isEmpty())
          {
            calibDetails += phantomString;
          }

          QString probeString;
          probeString = FindCalibrationDetails(doc, tr, "vtkPlusProbeCalibrationAlgo", "Probe calibration: ", "ImageCoordinateFrame", "ProbeCoordinateFrame");
          if (!probeString.isEmpty())
          {
            calibDetails += probeString;
          }
        }
      }

      QDomElement elem = list.at(0).toElement();

      QString name(elem.attribute("Name"));
      QString description(elem.attribute("Description"));
      if (name.isEmpty())
      {
        LOG_WARNING("Name field is empty in device set configuration file '" << fileName.toStdString() << "', it is not added to the list");
        continue;
      }

      if (!calibDetails.isEmpty())
      {
        description += "\n\n";
        description += calibDetails;
      }

      // Check if the same name already exists, add a version number if it does.
      int foundIndex = ui.comboBox_DeviceSet->findText(name, Qt::MatchExactly);
      if (foundIndex > -1)
      {
        QHash<QString, int>::iterator deviceIt = deviceSetVersion.find(name);
        if (deviceIt == deviceSetVersion.end())
        {
          deviceSetVersion.insert(name, 0);
          name.append(" [0]");
        }
        else
        {
          deviceIt.value() += 1;
          name.append(" [" + QString::number(deviceIt.value()) + "]");
        }
      }

      ui.comboBox_DeviceSet->addItem(name, fileName);
      int currentIndex = ui.comboBox_DeviceSet->findText(name, Qt::MatchExactly);

      ui.comboBox_DeviceSet->setItemData(currentIndex, description, DescriptionRole);

      // Add tooltip word wrapped rich text
      name.prepend("<p>");
      name.append(tr("</p> <p>") + fileInfo.fileName() + tr("</p> <p>") + description + tr("</p>"));
      ui.comboBox_DeviceSet->setItemData(currentIndex, name, Qt::ToolTipRole);
    }
    else
    {
      LOG_WARNING("Unable to parse file '" << fileName.toStdString() << "' as an XML. It will not appear in the device set configuration file list!");
    }
  }

  // If no valid configuration files have been parsed then warn user
  if (ui.comboBox_DeviceSet->count() < 1)
  {
    LOG_ERROR("Selected directory (" << aDirectory.toStdString() << ") does not contain valid device set configuration files!");
    return PLUS_FAIL;
  }

  // Set current index to default so that setting the last selected item raises the event even if it is the first item
  ui.comboBox_DeviceSet->setCurrentIndex(-1);

  ui.comboBox_DeviceSet->model()->sort(0);

  // Unblock signals after we add items
  ui.comboBox_DeviceSet->blockSignals(false);

  // Restore the saved selection
  int lastSelectedDeviceSetIndex = ui.comboBox_DeviceSet->findData(QDir::toNativeSeparators(QString(vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationFileName().c_str())));
  ui.comboBox_DeviceSet->setCurrentIndex(lastSelectedDeviceSetIndex);

  this->FixComboBoxDropDownListSizeAdjustemnt(ui.comboBox_DeviceSet);

  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
QString QPlusDeviceSetSelectorWidget::FindCalibrationDetails(const QDomDocument& aDocument,
    vtkSmartPointer<vtkIGSIOTransformRepository> aTransformRepository,
    const QString& aTagName,
    const QString& aOutputPrefix,
    const QString& aFirstFrame,
    const QString& aSecondFrame)
{
  QDomNodeList list(aDocument.elementsByTagName(aTagName));
  if (list.count() > 0)
  {
    auto elem = list.at(0).toElement();
    QString firstFrame(elem.attribute(aFirstFrame));
    QString secondFrame(elem.attribute(aSecondFrame));
    if (!firstFrame.isEmpty() && !secondFrame.isEmpty())
    {
      igsioTransformName tName(firstFrame.toStdString(), secondFrame.toStdString());
      std::string date;
      double error;
      if (aTransformRepository->GetTransformDate(tName, date, true) == PLUS_SUCCESS &&
          aTransformRepository->GetTransformError(tName, error, true) == PLUS_SUCCESS)
      {
        bool valid(false);
        std::tm tm = {};
        std::stringstream ss(date);
        ss >> std::get_time(&tm, "%Y.%m.%d %H:%M:%S");
        if (tm.tm_hour == 0 || tm.tm_mday == 0 || tm.tm_mon == 0 || tm.tm_year == 0)
        {
          if (date.length() == 13)
          {
            // %m%d%y_%H%M%S doesn't parse with std::get_time, reformat it manually
            std::string month(date.substr(0, 2));
            std::string day(date.substr(2, 2));
            std::string year(date.substr(4, 2));
            std::string hour(date.substr(7, 2));
            std::string minute(date.substr(9, 2));
            std::string second(date.substr(11, 2));
            std::stringstream ss;
            ss << month << " " << day << " " << year << "_" << hour << " " << minute << " " << second;
            ss >> std::get_time(&tm, "%m %d %y_%H %M %S");

            if (!(tm.tm_hour == 0 || tm.tm_mday == 0 || tm.tm_mon == 0 || tm.tm_year == 0))
            {
              valid = true;
            }
          }
        }
        else
        {
          valid = true;
        }

        QString output = aOutputPrefix;
        if (valid)
        {
          std::stringstream ss;
          ss << std::put_time(&tm, "%b %d %Y - %H:%M");
          output += QString::fromStdString(ss.str());
        }
        else
        {
          output += QString::fromStdString(date);
        }
        return output + ", error: " + QString::number(error, 'g', 2) + "\n";
      }
    }
  }

  return "";
}

//----------------------------------------------------------------------------
void QPlusDeviceSetSelectorWidget::dragEnterEvent(QDragEnterEvent* event)
{
  if (event->mimeData()->hasUrls() && event->mimeData()->urls().size() == 1)
  {
    QString filename = event->mimeData()->urls()[0].toLocalFile();
    if (filename.lastIndexOf('.') == -1)
    {
      return;
    }
    QString extension = filename.mid(filename.lastIndexOf('.'));
    if (extension.contains("xml"))
    {
      event->acceptProposedAction();
    }
  }
}

//----------------------------------------------------------------------------
void QPlusDeviceSetSelectorWidget::dropEvent(QDropEvent* event)
{
  QUrl url = event->mimeData()->urls().first();
  QString fileName = url.toLocalFile();

  this->SetConfigurationFile(fileName);
}

//----------------------------------------------------------------------------
void QPlusDeviceSetSelectorWidget::FixComboBoxDropDownListSizeAdjustemnt(QComboBox* cb)
{
  int scroll = cb->count() <= cb->maxVisibleItems() ? 0 :
               QApplication::style()->pixelMetric(QStyle::PM_ScrollBarExtent);

  int max = 0;

  for (int i = 0; i < cb->count(); i++)
  {
    int width = cb->view()->fontMetrics().width(cb->itemText(i));
    if (max < width) { max = width; }
  }

  cb->view()->setMinimumWidth(scroll + max);
}

//----------------------------------------------------------------------------
void QPlusDeviceSetSelectorWidget::resizeEvent(QResizeEvent* event)
{
  QWidget::resizeEvent(event);

  if (m_DeviceSetComboBoxMaximumSizeRatio != -1)
  {
    QDesktopWidget desktop;
    int screenWidth = desktop.screenGeometry(desktop.screenNumber(this)).width();
    ui.comboBox_DeviceSet->setMaximumWidth(screenWidth * m_DeviceSetComboBoxMaximumSizeRatio);
  }
}

//----------------------------------------------------------------------------
void QPlusDeviceSetSelectorWidget::UpdateDescriptionText()
{
  QString text;
  if (!m_DescriptionPrefix.isEmpty())
  {
    text += m_DescriptionPrefix + "\n\n";
  }

  text += m_DescriptionBody;

  if (!m_DescriptionSuffix.isEmpty())
  {
    text += "\n\n" + m_DescriptionSuffix;
  }

  ui.textEdit_Description->setText(text);
}

//-----------------------------------------------------------------------------
void QPlusDeviceSetSelectorWidget::RefreshFolder()
{
  LOG_TRACE("DeviceSetSelectorWidget::RefreshFolderClicked");

  if (ParseDirectory(QString(vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationDirectory().c_str())) != PLUS_SUCCESS)
  {
    LOG_ERROR("Parsing up configuration files failed in: " << vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationDirectory());
  }
}

//-----------------------------------------------------------------------------
void QPlusDeviceSetSelectorWidget::EditConfiguration()
{
  LOG_TRACE("DeviceSetSelectorWidget::EditConfiguration");

  QString configurationFilePath;
  int deviceSetIndex = ui.comboBox_DeviceSet->currentIndex();
  if (deviceSetIndex >= 0)
  {
    configurationFilePath = ui.comboBox_DeviceSet->itemData(deviceSetIndex).toStringList().at(0);
  }
  else
  {
    LOG_ERROR("Cannot edit configuration file. No configuration is selected.");
    return;
  }
  QString editorApplicationExecutable(vtkPlusConfig::GetInstance()->GetEditorApplicationExecutable().c_str());

  if (!editorApplicationExecutable.isEmpty())
  {
    QFileInfo fileInfo(QDir::toNativeSeparators(configurationFilePath));
    QString file = fileInfo.absoluteFilePath();

    QProcess::startDetached(editorApplicationExecutable, QStringList() << file);
    return;
  }

  // No editor application defined, try using the system default
  if (!QDesktopServices::openUrl(QUrl("file:///" + configurationFilePath, QUrl::TolerantMode)))
  {
    LOG_ERROR("Failed to open file in default application: " << configurationFilePath.toStdString());
  }
}

//----------------------------------------------------------------------------
void QPlusDeviceSetSelectorWidget::ShowEditContextMenu(QPoint point)
{
  if (m_EditorSelectAction == NULL)
  {
    m_EditorSelectAction = new QAction(this);
    m_EditorSelectAction->setText("Select Editor");
    connect(m_EditorSelectAction, &QAction::triggered, this, &QPlusDeviceSetSelectorWidget::SelectEditor);
  }

  if (m_EditApplicationConfigFileAction == NULL)
  {
    m_EditApplicationConfigFileAction = new QAction(this);
    m_EditApplicationConfigFileAction->setText("Edit App Config");
    connect(m_EditApplicationConfigFileAction, &QAction::triggered, this, &QPlusDeviceSetSelectorWidget::EditAppConfig);
  }

  if (m_EditMenu == NULL)
  {
    m_EditMenu = new QMenu(this);
    m_EditMenu->addAction(m_EditorSelectAction);
    m_EditMenu->addAction(m_EditApplicationConfigFileAction);
  }
  m_EditMenu->exec(ui.pushButton_EditConfiguration->mapToGlobal(point));
}

//-----------------------------------------------------------------------------
void QPlusDeviceSetSelectorWidget::SelectEditor()
{
  // File open dialog for selecting editor application
  QString filter = QString(tr("Executables ( *.exe );;"));
  QString fileName = QFileDialog::getOpenFileName(NULL, QString(tr("Select XML editor application")), "", filter);
  if (fileName.isNull())
  {
    return;
  }

  vtkPlusConfig::GetInstance()->SetEditorApplicationExecutable(fileName.toStdString());
  vtkPlusConfig::GetInstance()->SaveApplicationConfigurationToFile();
}

//----------------------------------------------------------------------------
void QPlusDeviceSetSelectorWidget::EditAppConfig()
{
  LOG_TRACE("DeviceSetSelectorWidget::EditConfiguration");

  QString configurationFilePath = QString::fromStdString(vtkPlusConfig::GetInstance()->GetApplicationConfigurationFilePath());
  QString editorApplicationExecutable(vtkPlusConfig::GetInstance()->GetEditorApplicationExecutable().c_str());

  if (!editorApplicationExecutable.isEmpty())
  {
    QFileInfo fileInfo(QDir::toNativeSeparators(configurationFilePath));
    QString file = fileInfo.absoluteFilePath();

    QProcess::startDetached(editorApplicationExecutable, QStringList() << file);
    return;
  }

  // No editor application defined, try using the system default
  if (!QDesktopServices::openUrl(QUrl("file:///" + configurationFilePath, QUrl::TolerantMode)))
  {
    LOG_ERROR("Failed to open file in default application: " << configurationFilePath.toStdString());
  }
}

//-----------------------------------------------------------------------------
void QPlusDeviceSetSelectorWidget::ResetTrackerButtonClicked()
{
  LOG_TRACE("DeviceSetSelectorWidget::ResetTrackerButtonClicked()");

  emit ResetTracker();
}

//-----------------------------------------------------------------------------
void QPlusDeviceSetSelectorWidget::ShowResetTrackerButton(bool aValue)
{
  ui.pushButton_ResetTracker->setVisible(aValue);
}

//-----------------------------------------------------------------------------
void QPlusDeviceSetSelectorWidget::SetConnectButtonText(QString text)
{
  ui.pushButton_Connect->setText(text);
}

//----------------------------------------------------------------------------
void QPlusDeviceSetSelectorWidget::SetDescriptionSuffix(const QString& string)
{
  this->m_DescriptionSuffix = string;
  this->UpdateDescriptionText();
}

//----------------------------------------------------------------------------
void QPlusDeviceSetSelectorWidget::ClearDescriptionSuffix()
{
  this->SetDescriptionSuffix("");
}
