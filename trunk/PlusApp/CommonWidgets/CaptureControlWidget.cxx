/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#include "CaptureControlWidget.h"
#include "vtkPlusChannel.h"
#include "vtkVirtualDiscCapture.h"
#include "vtksys/SystemTools.hxx"
#include <QFileDialog>
#include <QMessageBox>
#include <QTimer>

//-----------------------------------------------------------------------------
CaptureControlWidget::CaptureControlWidget(QWidget* aParent)
: QWidget(aParent)
, m_UpdateTimer(NULL)
, m_Device(NULL)
{
  ui.setupUi(this);

  // Create and connect recording timer
  m_UpdateTimer = new QTimer(this); 
  connect(m_UpdateTimer, SIGNAL(timeout()), this, SLOT(Update()) );

  m_UpdateTimer->start(200);

  connect(ui.startStopButton, SIGNAL(clicked()), this, SLOT(StartStopButtonPressed()) );
  connect(ui.saveButton, SIGNAL(clicked()), this, SLOT(SaveButtonPressed()) );
  connect(ui.requestedFrameRateSpinBox, SIGNAL(valueChanged(int) ), this, SLOT( RequestedFrameRateChanged(int) ) );

  ui.startStopButton->setText("Start");

  ui.startStopButton->setPaletteBackgroundColor(QColor::fromRgb(255, 255, 255));
}

//-----------------------------------------------------------------------------
CaptureControlWidget::~CaptureControlWidget()
{
  delete m_UpdateTimer;
}

//-----------------------------------------------------------------------------
void CaptureControlWidget::WriteToFile( QString& aFilename )
{
  if (! aFilename.isNull() )
  {
    QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));

    // Actual saving
    std::string filePath(aFilename.toLatin1().data());
    std::string path = vtksys::SystemTools::GetFilenamePath(filePath); 
    std::string filename = vtksys::SystemTools::GetFilenameWithoutExtension(filePath); 
    std::string extension = vtksys::SystemTools::GetFilenameExtension(filePath); 

    vtkTrackedFrameList::SEQ_METAFILE_EXTENSION ext(vtkTrackedFrameList::SEQ_METAFILE_MHA); 
    if ( STRCASECMP(".mhd", extension.c_str() ) == 0 )
    {
      ext = vtkTrackedFrameList::SEQ_METAFILE_MHD; 
    }
    else
    {
      ext = vtkTrackedFrameList::SEQ_METAFILE_MHA; 
    }

    vtkTrackedFrameList* aList = vtkTrackedFrameList::New();
    vtkPlusChannel* aChannel = *m_Device->GetOutputChannelsStart();
    double timestampFrom;
    aChannel->GetTrackedFrameList(timestampFrom, aList, 200);

    if ( aList->SaveToSequenceMetafile(path.c_str(), filename.c_str(), ext, false) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to save tracked frames to sequence metafile!"); 
      return;
    }

    QString result = "File saved to\n";
    result += aFilename;
    ui.extraInformationLabel->setText(result);

    // Save config file next to the tracked frame list
    std::string configFileName = path + "/" + filename + "_config.xml";
    PlusCommon::PrintXML(configFileName.c_str(), vtkPlusConfig::GetInstance()->GetDeviceSetConfigurationData());

    aList->Delete();

    QApplication::restoreOverrideCursor();
  }	
}

//-----------------------------------------------------------------------------
double CaptureControlWidget::GetMaximumFrameRate() const
{
  LOG_TRACE("CaptureControlWidget::GetMaximumFrameRate");

  if (m_Device == NULL )
  {
    LOG_ERROR("Unable to reach valid device!");
    return 0.0;
  }

  return m_Device->GetAcquisitionRate();
}

//-----------------------------------------------------------------------------
void CaptureControlWidget::UpdateBasedOnState()
{
  if( m_Device != NULL )
  {
    ui.startStopButton->setEnabled(true);

    vtkPlusChannel* aChannel = (*m_Device->GetOutputChannelsStart());
    std::stringstream ss;
    ss << m_Device->GetDeviceId() << "::" << aChannel->GetChannelId();
    ui.channelIdentifierLabel->setText(QString(ss.str().c_str()));

    ui.saveButton->setEnabled( m_Device->HasUnsavedData() );

    if( m_Device->GetEnableCapturing() )
    {
      ui.startStopButton->setText("Stop");
      ui.recordStatusLabel->setPaletteForegroundColor(QColor::fromRgb(0, 255, 0));
      ui.recordStatusLabel->setText(QString("Recording"));
      ui.requestedFrameRateSpinBox->setEnabled(true);
    }
    else
    {
      ui.startStopButton->setText("Start");
      ui.recordStatusLabel->setPaletteForegroundColor(QColor::fromRgb(255, 0, 0));
      ui.recordStatusLabel->setText(QString("Stopped"));
      ui.requestedFrameRateSpinBox->setEnabled(false);
    }
    ui.extraInformationLabel->setText("");
  }
  else
  {
    ui.startStopButton->setEnabled(false);
    ui.saveButton->setEnabled(false);
    ui.channelIdentifierLabel->setText("");
    ui.recordStatusLabel->setText("");
    ui.extraInformationLabel->setText("");
    ui.requestedFrameRateSpinBox->setEnabled(false);
  }
}

//-----------------------------------------------------------------------------
PlusStatus CaptureControlWidget::SaveToMetafile( std::string aOutput )
{
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
void CaptureControlWidget::Update()
{
  this->UpdateBasedOnState();
}

//-----------------------------------------------------------------------------
void CaptureControlWidget::StartStopButtonPressed()
{
  if( m_Device != NULL )
  {
    QString text = ui.startStopButton->text();
    if( QString::compare(text, QString("Start")) == 0 )
    {
      m_Device->SetEnableCapturing(true);
    }
    else
    {
      m_Device->SetEnableCapturing(false);
    }

    this->UpdateBasedOnState();
  }
}

//-----------------------------------------------------------------------------
void CaptureControlWidget::SetCaptureDevice(vtkVirtualDiscCapture& aDevice)
{
  m_Device = &aDevice;

  this->UpdateBasedOnState();
}

//-----------------------------------------------------------------------------
void CaptureControlWidget::SaveButtonPressed()
{
  bool isCapturing = m_Device->GetEnableCapturing();

  // Stop recording
  m_Device->SetEnableCapturing(false);

  // Present dialog, get filename
  QFileDialog* dialog = new QFileDialog(this, QString("Select save file"), QString(vtkPlusConfig::GetInstance()->GetOutputDirectory()), QString("All MetaSequence files (*.mha *.mhd)") );
  dialog->setMinimumSize(QSize(640, 480));
  dialog->setAcceptMode(QFileDialog::AcceptSave);
  dialog->setFileMode(QFileDialog::AnyFile);
  dialog->setViewMode(QFileDialog::Detail);
  dialog->setDefaultSuffix("mha");
  dialog->exec();

  QApplication::processEvents();

  if( dialog->selectedFiles().size() == 0 )
  {
    m_Device->SetEnableCapturing(isCapturing);
    delete dialog;
    return;
  }

  QString fileName = dialog->selectedFiles().first();
  delete dialog;

  m_Device->SetFilename(fileName.toLatin1());

  // Save
  if( m_Device->CloseFile() != PLUS_SUCCESS )
  {
    ui.extraInformationLabel->setText(QString("Saving failed. Unable to close device."));
    return;
  }

  m_Device->ClearRecordedFrames();

  this->UpdateBasedOnState();
}

//-----------------------------------------------------------------------------
void CaptureControlWidget::RequestedFrameRateChanged( int aValue )
{
  this->m_Device->SetRequestedFrameRate(aValue);
}
