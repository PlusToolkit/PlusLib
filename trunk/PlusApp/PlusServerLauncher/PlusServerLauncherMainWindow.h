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
class BroadcastedToolInfoWidget; 
class StatusIcon;
class vtkDataCollector; 

class QLabel;
class QProgressBar;
class QTimer;

/*! \class PlusServerLauncherMainWindow 
 * \brief Main window of the PlusServerLauncher application
 * \ingroup PlusAppPlusServerLauncher
 */
class PlusServerLauncherMainWindow : public QMainWindow
{
	Q_OBJECT

public:
	PlusServerLauncherMainWindow(QWidget *parent = 0, Qt::WFlags flags = 0);
	~PlusServerLauncherMainWindow();

protected:

	/*! Set up status bar (label and progress) */
	void setupStatusBar();
  
protected slots:
	
  /*! Updates every part of the GUI (called by ui refresh timer) */
	void updateGUI();
 
  /*! Save current device set configuration */
  void saveDeviceSetConfiguration();

  /*! Connect to devices by configuration file with */ 
  void connectToDevicesByConfigFile(std::string aConfigFile); 

protected:

  /*! Data collector instance */
  vtkDataCollector* m_DataCollector;

  /*! Device set selector widget */
	DeviceSetSelectorWidget*	m_DeviceSetSelectorWidget;

	/*! Label on the left of the statusbar */
	QLabel*					    m_StatusBarLabel;

  /*! Status icon instance */
  StatusIcon*         m_StatusIcon;

  /*! Container for transform tool info widgets */
  std::deque<BroadcastedToolInfoWidget*> m_BroadcastedTransformsInfo; 

  /*! Image tool info widget instance */
  BroadcastedToolInfoWidget* m_BroadcastedImageInfo; 

private:
	Ui::PlusServerLauncherMainWindow	ui;

};

#endif 
