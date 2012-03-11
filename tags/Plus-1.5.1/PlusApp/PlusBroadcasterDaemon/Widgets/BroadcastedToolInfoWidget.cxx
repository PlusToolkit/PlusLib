/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#include "BroadcastedToolInfoWidget.h"
#include "BroadcastedToolEditorDialog.h"
#include <QMessageBox>

//-----------------------------------------------------------------------------
BroadcastedToolInfoWidget::BroadcastedToolInfoWidget(QWidget *parent, Qt::WFlags flags)
: QWidget(parent, flags)
{
  ui.setupUi(this);

  // Connect signals and slots
  connect( ui.edit, SIGNAL(clicked()), this, SLOT(editToolInfo()) ); 
  connect( ui.del, SIGNAL(clicked()), this, SLOT(deleteToolInfo()) ); 
  connect( ui.pause, SIGNAL(clicked()), this, SLOT(pauseBroadcasting()) ); 
}

//-----------------------------------------------------------------------------
BroadcastedToolInfoWidget::~BroadcastedToolInfoWidget()
{
}

//-----------------------------------------------------------------------------
void BroadcastedToolInfoWidget::editToolInfo()
{
  BroadcastedToolEditorDialog editor; 
  connect( &editor, SIGNAL( modified(const IgtToolInfo&) ), this, SLOT( modifyBroadcastedTool(const IgtToolInfo&) ) ); 
  editor.setToolInfo(this->m_ToolInfo); 
  editor.exec(); 
}

//-----------------------------------------------------------------------------
void BroadcastedToolInfoWidget::deleteToolInfo()
{
  std::ostringstream message; 
  message << "Are you sure you want to remove the following broadcasted tool?\n\n" 
    << this->m_ToolInfo.Name << " (" << this->m_ToolInfo.IgtlSocketInfo.Host << ":" 
    << this->m_ToolInfo.IgtlSocketInfo.Port << ")"; 

  int ret = QMessageBox::warning(this, tr("Remove tool"),
    tr(message.str().c_str()), QMessageBox::Yes | QMessageBox::Cancel,
     QMessageBox::Cancel);

  if ( ret == QMessageBox::Yes )
  {
    // Emits deleted signal
    emit this->deleted(this->m_ToolInfo); 
  }
}

//-----------------------------------------------------------------------------
void BroadcastedToolInfoWidget::pauseBroadcasting()
{
  emit this->changePauseStatus(this->m_ToolInfo); 
}

//-----------------------------------------------------------------------------
void BroadcastedToolInfoWidget::setToolInfo(const IgtToolInfo& toolInfo)
{
  this->m_ToolInfo = toolInfo; 
  this->updateGUI(); 
}

//-----------------------------------------------------------------------------
void BroadcastedToolInfoWidget::modifyBroadcastedTool(const IgtToolInfo& newToolInfo)
{
  // Emits modified signal
  emit this->modified(this->m_ToolInfo, newToolInfo); 
}

//-----------------------------------------------------------------------------
void BroadcastedToolInfoWidget::updateGUI()
{
  // Name 
  ui.name->setText( tr(this->m_ToolInfo.Name.c_str()) ); 
  ui.name->setToolTip( tr(QString("Tool name: ") + QString(this->m_ToolInfo.Name.c_str()) ) ); 
  
  // Address 
  std::ostringstream addr; 
  addr << this->m_ToolInfo.IgtlSocketInfo.Host << ":" << this->m_ToolInfo.IgtlSocketInfo.Port; 
  ui.address->setText( tr(addr.str().c_str()) ); 
  ui.address->setToolTip( tr( QString("Address: ") + QString(addr.str().c_str()) ) ); 

  // Pause
  ui.pause->setIcon( QIcon(":/icons/Resources/icon_Pause.png") ); 
  ui.pause->setToolTip(tr("Pause broadcasting"));

  // Status
  if ( this->m_ToolInfo.Paused ) 
  {
    ui.status->setIcon( QIcon(":/icons/Resources/icon_StatusGray.png") ); 
    ui.status->setToolTip(tr("Status: Paused")); 

    ui.pause->setIcon( QIcon(":/icons/Resources/icon_Play.png") ); 
    ui.pause->setToolTip(tr("Continue broadcasting"));
  }
  else if ( this->m_ToolInfo.Valid )
  {
    ui.status->setIcon( QIcon(":/icons/Resources/icon_StatusGreen.png") ); 
    ui.status->setToolTip(tr("Status: OK")); 
  }
  else
  {
    ui.status->setIcon( QIcon(":/icons/Resources/icon_StatusRed.png") ); 
    ui.status->setToolTip(tr("Status: Invalid")); 
  }

}

//-----------------------------------------------------------------------------
IgtToolInfo* BroadcastedToolInfoWidget::getToolInfo()
{
  return &this->m_ToolInfo; 
}
