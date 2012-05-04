/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __BroadcastedToolEditorDialog_h
#define __BroadcastedToolEditorDialog_h

#include "ui_BroadcastedToolEditorDialog.h"
#include "PlusConfigure.h"
#include "vtkOpenIGTLinkBroadcaster.h"

#include <QtGui/QDialog>


/*! \class BroadcastedToolEditorDialog 
 * \brief Form used for editing broadcasted tool information
 * \ingroup PlusAppPlusDaemon
 */
class BroadcastedToolEditorDialog : public QDialog
{
	Q_OBJECT

public:
	BroadcastedToolEditorDialog(QWidget *parent = 0, Qt::WFlags flags = 0);
	~BroadcastedToolEditorDialog();

  /*! Set IGT tool information */
  void setToolInfo(const IgtToolInfo& toolInfo); 

  /*! Get IGT tool information */
  IgtToolInfo* getToolInfo(); 

signals: 
  /*!
	* Emmitted when IgtToolInfo modified 
	* \param Modified IGT tool information
	*/
  void modified(const IgtToolInfo&); 
protected:

  /*! Fill out form from IGT transform information  */
  void updateGUI(); 

  /*! IGT tool information */
  IgtToolInfo m_ToolInfo; 

protected slots:
  /*! Validate form input fields and emit modified signal */
  void checkAndModify(); 

  private:
	Ui::BroadcastedToolEditorDialog ui;

}; 

#endif