/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __PlusServerLauncherMainWindow_h
#define __PlusServerLauncherMainWindow_h

#include "ui_PlusServerLauncherMainWindow.h"


#include "PlusConfigure.h"

#include <QtGui/QMainWindow>
#include <deque>

class DeviceSetSelectorWidget;
class StatusIcon;
class vtkPlusOpenIGTLinkServer; 

class QLabel;
class QProgressBar;
class QTimer;

/*!
  \class PlusServerLauncherMainWindow 
  \brief Main window of the PlusServerLauncher application
  \ingroup PlusAppPlusServerLauncher
*/
class PlusServerLauncherMainWindow : public QMainWindow
{
	Q_OBJECT

public:
	PlusServerLauncherMainWindow(QWidget *parent = 0, Qt::WFlags flags = 0);
	~PlusServerLauncherMainWindow();

protected:
  
protected slots:
	
  /*! Updates every part of the GUI (called by ui refresh timer) */
	void processPendingCommands();
 
  /*! Connect to devices by configuration file with */ 
  void connectToDevicesByConfigFile(std::string aConfigFile); 

protected:

  /*! Device set selector widget */
	DeviceSetSelectorWidget*	m_DeviceSetSelectorWidget;

	/*! Label on the left of the statusbar */
	QLabel*					    m_StatusBarLabel;

  /*! Status icon instance */
  StatusIcon*         m_StatusIcon;

  /*! PlusServer instance that is responsible for all data collection and network transfer */
  vtkPlusOpenIGTLinkServer* m_Server;

  /*! Timer that refreshes the UI */
  QTimer*             m_ProcessPendingCommandsTimer; 

private:
	Ui::PlusServerLauncherMainWindow	ui;

};

#endif 
