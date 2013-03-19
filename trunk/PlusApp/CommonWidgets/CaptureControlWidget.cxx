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
, m_ResultDisplayTimer(NULL)
, m_ResultTimerActive(false)
{
  ui.setupUi(this);

  // Create and connect recording timer
  m_UpdateTimer = new QTimer(this); 
  connect(m_UpdateTimer, SIGNAL(timeout()), this, SLOT(Update()) );

  m_UpdateTimer->start(200);

  connect(ui.startStopButton, SIGNAL(clicked()), this, SLOT(StartStopButtonPressed()) );
  connect(ui.saveButton, SIGNAL(clicked()), this, SLOT(SaveButtonPressed()) );
  connect(ui.saveAsButton, SIGNAL(clicked()), this, SLOT(SaveAsButtonPressed()) );
  connect(ui.clearRecordedFramesButton, SIGNAL(clicked()), this, SLOT(ClearButtonPressed()) );
  connect(ui.samplingRateSlider, SIGNAL(valueChanged(int) ), this, SLOT( SamplingRateChanged(int) ) );

  ui.startStopButton->setText("Start");

  ui.startStopButton->setPaletteBackgroundColor(QColor::fromRgb(255, 255, 255));
}

//-----------------------------------------------------------------------------
CaptureControlWidget::~CaptureControlWidget()
{
  delete m_UpdateTimer;
}

//-----------------------------------------------------------------------------
PlusStatus CaptureControlWidget::WriteToFile( QString& aFilename )
{
  m_Device->SetFilename(aFilename.toLatin1());

  // Save
  if( m_Device->CloseFile() != PLUS_SUCCESS )
  {
    LOG_ERROR("Saving failed. Unable to close device.");
    return PLUS_FAIL;
  }

  return PLUS_SUCCESS;
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
    ui.channelIdentifierLabel->setText(QString(m_Device->GetDeviceId()));

    ui.saveAsButton->setEnabled( m_Device->HasUnsavedData() );
    ui.saveButton->setEnabled( m_Device->HasUnsavedData() );
    ui.clearRecordedFramesButton->setEnabled( m_Device->HasUnsavedData() );

    if( m_Device->GetEnableCapturing() )
    {
      ui.actualFrameRateValueLabel->setText( QString::number(m_Device->GetActualFrameRate(), 'f', 2) );
      ui.samplingRateSlider->setEnabled(false);
      ui.startStopButton->setText(QString("Stop"));
      ui.startStopButton->setIcon( QPixmap( ":/icons/Resources/icon_Stop.png" ) );
      ui.startStopButton->setEnabled(true);

      ui.numberOfRecordedFramesValueLabel->setText( QString::number(m_Device->GetTotalFramesRecorded(), 10) );
    }
    else
    {
      ui.startStopButton->setText(QString("Record"));
      ui.startStopButton->setIcon( QPixmap( ":/icons/Resources/icon_Record.png" ) );
      ui.startStopButton->setFocus();
      ui.startStopButton->setEnabled(true);

      ui.actualFrameRateValueLabel->setText( QString::number(0.0, 'f', 2) );
      ui.numberOfRecordedFramesValueLabel->setText( QString::number(0, 10) );
      ui.samplingRateSlider->setEnabled(true);
    }
  }
  else
  {
    ui.startStopButton->setText("Record");
    ui.startStopButton->setIcon( QPixmap( ":/icons/Resources/icon_Record.png" ) );
    ui.startStopButton->setEnabled(false);

    ui.startStopButton->setEnabled(false);
    ui.saveButton->setEnabled(false);
    ui.clearRecordedFramesButton->setEnabled(false);
    ui.channelIdentifierLabel->setText("");
    ui.samplingRateSlider->setEnabled(false);
    ui.actualFrameRateValueLabel->setText( QString::number(0.0, 'f', 2) );
    ui.numberOfRecordedFramesValueLabel->setText( QString::number(0, 10) );
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
    if( QString::compare(text, QString("Record")) == 0 )
    {
      m_Device->SetEnableCapturing(true);
    }
    else
    {
      m_Device->SetEnableCapturing(false);
      ui.actualFrameRateValueLabel->setText(QString("0.0"));
    }

    this->UpdateBasedOnState();
  }
}

//-----------------------------------------------------------------------------
void CaptureControlWidget::SetCaptureDevice(vtkVirtualDiscCapture& aDevice)
{
  m_Device = &aDevice;

  this->SamplingRateChanged(10);

  this->UpdateBasedOnState();
}

//-----------------------------------------------------------------------------
void CaptureControlWidget::SaveButtonPressed()
{
  LOG_TRACE("CaptureControlWidget::SaveButtonPressed"); 

  // Stop recording
  m_Device->SetEnableCapturing(false);

  QString fileName = QString("%1/TrackedImageSequence_%2_%3.mha").arg(vtkPlusConfig::GetInstance()->GetOutputDirectory()).arg(m_Device->GetDeviceId()).arg(vtksys::SystemTools::GetCurrentDateTime("%Y%m%d_%H%M%S").c_str());

  this->ShowResultIcon(this->WriteToFile(fileName) != PLUS_FAIL, 2000);

  this->UpdateBasedOnState();

  LOG_INFO("Captured tracked frame list saved into '" << fileName.toLatin1().begin() << "'");
}

//-----------------------------------------------------------------------------
void CaptureControlWidget::SaveAsButtonPressed()
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

  this->ShowResultIcon(this->WriteToFile(fileName) != PLUS_FAIL, 2000);

  this->UpdateBasedOnState();
}

//-----------------------------------------------------------------------------
void CaptureControlWidget::SamplingRateChanged( int aValue )
{
  LOG_TRACE("CaptureControlWidget::RecordingFrameRateChanged(" << aValue << ")"); 

  double maxFrameRate = GetMaximumFrameRate();
  int samplingRate = (int)(pow(2.0, ui.samplingRateSlider->maxValue() - aValue));
  double requestedFrameRate(0.0);

  if (samplingRate>0)
  {
    requestedFrameRate = maxFrameRate / (double)samplingRate;
  }
  else
  {
    LOG_WARNING("samplingRate value is invalid");
    requestedFrameRate = maxFrameRate;
  }

  ui.samplingRateSlider->setToolTip(tr("1 / ").append(QString::number((int)samplingRate)));
  ui.requestedFrameRateValueLabel->setText(QString::number(requestedFrameRate, 'f', 2));

  LOG_INFO("Sampling rate changed to " << aValue << " (matching requested frame rate is " << requestedFrameRate << ")");
  this->m_Device->SetRequestedFrameRate(requestedFrameRate);
}

//-----------------------------------------------------------------------------
void CaptureControlWidget::ClearButtonPressed()
{
  m_Device->SetEnableCapturing(false);
  m_Device->Reset();

  this->UpdateBasedOnState();

  this->ShowResultIcon(true, 2000);
}

//-----------------------------------------------------------------------------
void CaptureControlWidget::ShowResultIcon( bool success, int timerInMSec /*= 2000*/ )
{
  if( m_ResultTimerActive )
  {
    return;
  }

  m_ResultTimerActive = true;
  m_ResultDisplayTimer = new QTimer(this);
  connect(m_ResultDisplayTimer, SIGNAL(timeout()), this, SLOT(UpdateResultIcon()) );

  QPixmap pixmap;

  if( success )
  {
    if (!pixmap.load( ":/icons/Resources/icon_Success.png" )) {
      LOG_ERROR("Failed to load images/icon_Success.png");
    }

  }
  else
  {
    if (!pixmap.load( ":/icons/Resources/icon_Fail.png" )) {
      LOG_ERROR("Failed to load images/icon_Fail.png");
    }
  }

  if( !pixmap.isNull() )
  {
    ui.resultIconLabel->setPixmap(pixmap);
  }

  m_ResultDisplayTimer->start(timerInMSec);
}

//-----------------------------------------------------------------------------
void CaptureControlWidget::UpdateResultIcon()
{
  ui.resultIconLabel->setPixmap(NULL);
  delete m_ResultDisplayTimer;
  m_ResultTimerActive = false;
}
