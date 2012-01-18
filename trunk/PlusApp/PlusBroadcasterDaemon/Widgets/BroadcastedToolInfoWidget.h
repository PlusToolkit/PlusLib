/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __BroadcastedToolInfoWidget_h
#define __BroadcastedToolInfoWidget_h

#include "ui_BroadcastedToolInfoWidget.h"
#include "PlusConfigure.h"
#include "vtkOpenIGTLinkBroadcaster.h"

#include <QtGui/QWidget>

/*! \class BroadcastedToolInfoWidget 
* \brief Widget used for displaying broadcasted tools 
* \ingroup PlusAppPlusDaemon
*/
class BroadcastedToolInfoWidget : public QWidget
{
  Q_OBJECT

public:
  BroadcastedToolInfoWidget(QWidget *parent = 0, Qt::WFlags flags = 0);
  ~BroadcastedToolInfoWidget();

  /*! Set IGT tool information */ 
  void setToolInfo(const IgtToolInfo& toolInfo); 

  /*! Get IGT tool information */ 
  IgtToolInfo* getToolInfo(); 

  /*! Updates GUI widgets from IGT tool info */
  void updateGUI(); 

signals: 
  /*!
  * Emmitted when tool info modified
  * \param Original tool info
  * \param Modified tool info
  */
  void modified(const IgtToolInfo&, const IgtToolInfo&);

  /*!
  * Emmitted when tool info deleted
  * \param Tool info to delete
  */
  void deleted(const IgtToolInfo&);

  /*!
  * Emmitted when pause status changed
  * \param Tool info to delete
  */
  void changePauseStatus(const IgtToolInfo&); 

protected:
  /*! IGT tool info */
  IgtToolInfo m_ToolInfo; 

protected slots:

  /*! Calls tool info editor dialog */
  void editToolInfo(); 

  /*! Emits deleted signal */
  void deleteToolInfo(); 

  /*! Emits changePauseStatus signal */
  void pauseBroadcasting(); 

  /*! Emits modified signal */
  void modifyBroadcastedTool(const IgtToolInfo& newToolInfo); 

private:
  Ui::BroadcastedToolInfo	ui;

}; 

/*! \class BroadcastedToolInfoWidgetFinder 
* \brief Helper class for std::find_if predicate function  
* \ingroup PlusAppPlusDaemon
*/
class BroadcastedToolInfoWidgetFinder
{	
public:
  BroadcastedToolInfoWidgetFinder(BroadcastedToolInfoWidget* widget): m_BroadcastedToolInfoWidget(widget){}; 	
  /*! Unary function operator for BroadcastedToolInfoWidget equality */
  bool operator()( BroadcastedToolInfoWidget *widget )	
  {		
    bool socketEquality = this->m_BroadcastedToolInfoWidget->getToolInfo()->IgtlSocketInfo.Socket == widget->getToolInfo()->IgtlSocketInfo.Socket; 
    bool nameEquality = this->m_BroadcastedToolInfoWidget->getToolInfo()->Name == widget->getToolInfo()->Name; 
    return (socketEquality && nameEquality); 
  }	
  BroadcastedToolInfoWidget* m_BroadcastedToolInfoWidget;
};

#endif