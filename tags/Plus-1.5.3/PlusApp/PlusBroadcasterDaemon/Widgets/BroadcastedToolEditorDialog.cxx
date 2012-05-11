/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#include "BroadcastedToolEditorDialog.h"
#include <QString>

//-----------------------------------------------------------------------------
BroadcastedToolEditorDialog::BroadcastedToolEditorDialog(QWidget *parent, Qt::WFlags flags)
: QDialog(parent, flags)
{
  ui.setupUi(this);
  connect( ui.saveButton, SIGNAL( clicked() ), this, SLOT( checkAndModify() ) );
  connect( ui.cancelButton, SIGNAL( clicked() ), this, SLOT( close() ) );
}


//-----------------------------------------------------------------------------
BroadcastedToolEditorDialog::~BroadcastedToolEditorDialog()
{
}

//-----------------------------------------------------------------------------
void BroadcastedToolEditorDialog::setToolInfo(const IgtToolInfo& toolInfo)
{
  this->m_ToolInfo = toolInfo; 
  this->updateGUI(); 
}

//-----------------------------------------------------------------------------
void BroadcastedToolEditorDialog::updateGUI()
{
  // Name
  ui.name->setText( tr(this->m_ToolInfo.Name.c_str()) ); 

  // Address 
  ui.address->setText( tr( this->m_ToolInfo.IgtlSocketInfo.Host.c_str()) ); 

  // Port
  ui.port->setText( tr( QString::number(this->m_ToolInfo.IgtlSocketInfo.Port) )); 
}

//-----------------------------------------------------------------------------
IgtToolInfo* BroadcastedToolEditorDialog::getToolInfo()
{
  return &this->m_ToolInfo; 
}


//-----------------------------------------------------------------------------
void BroadcastedToolEditorDialog::checkAndModify()
{
  // Check inputs
  if ( ui.name->text().isEmpty() )
  {
    ui.validationLabel->setText( tr("* Name is missing!") ); 
    return; 
  }

  if ( ui.address->text().isEmpty() )
  {
    ui.validationLabel->setText( tr("* Domain name or IP is invalid!") ); 
    return; 
  }

  if ( ui.port->text().isEmpty() )
  {
    ui.validationLabel->setText( tr("* Port number is missing!") ); 
    return; 
  }

  // Update tool info
  bool conversionOK(false); 
  int port = ui.port->text().toInt(&conversionOK); 
  if ( conversionOK )
  {
    this->m_ToolInfo.IgtlSocketInfo.Port = port;  
  }
  else
  {
    ui.validationLabel->setText( tr("* Port number is invalid!") ); 
    return;
  }

  this->m_ToolInfo.Name = ui.name->text().toStdString(); 
  std::string host = ui.address->text().toStdString(); 
  this->m_ToolInfo.IgtlSocketInfo.Host = host;

  // Emit modified event
  emit this->modified(this->m_ToolInfo); 

  this->accept();  
}

