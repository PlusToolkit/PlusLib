/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __OpenIGTLinkBroadcasterWidget_h
#define __OpenIGTLinkBroadcasterWidget_h

#include "ui_OpenIGTLinkBroadcaster.h"

#include "PlusConfigure.h"
#include <QtGui/QWidget>


class QTimer;

class vtkDataCollector;
class vtkOpenIGTLinkBroadcaster;
class vtkTransformRepository;


class OpenIGTLinkBroadcasterWidget : public QWidget
{
  Q_OBJECT

public:
  /*!
  * \brief Constructor
  * \param aParent parent
  */
  OpenIGTLinkBroadcasterWidget( QWidget *parent = 0 );

  /*!
  * \brief Destructor
  */
  ~OpenIGTLinkBroadcasterWidget();
  
  void Initialize( std::string configFileName );
  

protected slots:
  
  void PlayPausePressed();
  void SendMessages();
  

private:
  
  Ui::OpenIGTLinkBroadcasterForm ui;
  
  vtkDataCollector* m_DataCollector;
  vtkOpenIGTLinkBroadcaster* m_OpenIGTLinkBroadcaster;
  vtkTransformRepository* m_TransformRepository;
  
  QTimer* m_Timer;
  
  bool Paused;
  bool StylusCalibrationOn;
  
};


#endif
