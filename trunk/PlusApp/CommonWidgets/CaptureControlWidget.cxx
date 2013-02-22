/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#include "CaptureControlWidget.h"
#include "vtkPlusChannel.h"
#include "vtkVirtualStreamDiscCapture.h"
#include "vtksys/SystemTools.hxx"
#include <QFileDialog>
#include <QMessageBox>
#include <QTimer>

//-----------------------------------------------------------------------------
CaptureControlWidget::CaptureControlWidget(QWidget* aParent, Qt::WFlags aFlags)
: QWidget(aParent, aFlags)
, m_RecordingTimer(NULL)
, m_Device(NULL)
{
  ui.setupUi(this);

  // Create and connect recording timer
  m_RecordingTimer = new QTimer(this); 
  connect(m_RecordingTimer, SIGNAL(timeout()), this, SLOT(Capture()) );
}

//-----------------------------------------------------------------------------
CaptureControlWidget::~CaptureControlWidget()
{
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
void CaptureControlWidget::Reset()
{

}

//-----------------------------------------------------------------------------
PlusStatus CaptureControlWidget::SaveToMetafile( std::string aOutput )
{
  return PLUS_SUCCESS;
}
