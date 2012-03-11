/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __PlusBroadcasterDaemonMainWindow_h
#define __PlusBroadcasterDaemonMainWindow_h

#include "ui_PlusBroadcasterDaemonMainWindow.h"


#include "PlusConfigure.h"
#include "vtkOpenIGTLinkBroadcaster.h"

#include <QtGui/QMainWindow>
#include <deque>

class DeviceSetSelectorWidget;
class BroadcastedToolInfoWidget; 
class StatusIcon;
class vtkDataCollector; 

class QLabel;
class QProgressBar;
class QTimer;

/*! \class PlusBroadcasterDaemonMainWindow 
 * \brief Main window of the PlusBroadcasterDaemon application
 * \ingroup PlusAppPlusBroadcasterDaemon
 */
class PlusBroadcasterDaemonMainWindow : public QMainWindow
{
	Q_OBJECT

public:
	PlusBroadcasterDaemonMainWindow(QWidget *parent = 0, Qt::WFlags flags = 0);
	~PlusBroadcasterDaemonMainWindow();

protected:

	/*! Set up status bar (label and progress) */
	void setupStatusBar();
  
  /*! Create and add BroadcastedToolInfoWidget to container for transforms. Connect signals and slots, add widget to layout */
  void addBroadcastedTransformToolInfoWidget(IgtToolInfo& toolInfo); 
  
  /*! Remove and delete BroadcastedToolInfoWidget from container. Disconnect signals and slots, remove widget from layout */
  void deleteBroadcastedTransformToolInfoWidget(BroadcastedToolInfoWidget* widget); 

  /*! Remove and delete all BroadcastedToolInfoWidget from container. Disconnect signals and slots, remove widgets from layout */
  void clearBroadcastedTransformToolInfoWidgets(); 

  /*! Create and add BroadcastedToolInfoWidget for image broadcasting. Connect signals and slots, add widget to layout */
  void addBroadcastedImageInfoWidget(); 

  /*! Remove and delete BroadcastedToolInfoWidget for image broadcasting. Disconnect signals and slots, remove widget from layout */
  void deleteBroadcastedImageInfoWidget(); 

  /*! Setup and initialize OpenIGTLink broadcaster */
  PlusStatus setupBroadcaster(); 

protected slots:
	
  /*! Updates every part of the GUI (called by ui refresh timer) */
	void updateGUI();
 
  /*! Send messages on m_BroadcastingIntervalTimer timer timeout */ 
  void sendMessages(); 

  /*! Save current device set configuration */
  void saveDeviceSetConfiguration();

  /*! Connect to devices by configuration file with */ 
  void connectToDevicesByConfigFile(std::string aConfigFile); 

  /*! Modify broadcasted transform tool info */ 
  void modifyBroadcastedTransformTool(const IgtToolInfo& originalToolInfo, const IgtToolInfo& modifiedToolInfo); 

  /*! Modify broadcasted transform tool info */ 
  void modifyBroadcastedImageTool(const IgtToolInfo& originalToolInfo, const IgtToolInfo& modifiedToolInfo); 

  /*! Delete broadcasted transform tool info */ 
  void deleteBroadcastedTransformTool(const IgtToolInfo& toolInfo); 

  /*! Delete broadcasted image tool info */ 
  void deleteBroadcastedImageTool(const IgtToolInfo& toolInfo); 

  /*! Change broadcasted transform tool pause status */ 
  void changeBroadcastedTransformToolPauseStatus(const IgtToolInfo& toolInfo); 

  /*! Change broadcasted image tool pause status */ 
  void changeBroadcastedImageToolPauseStatus(const IgtToolInfo& toolInfo);

  /*! Open tool info editor for transforms */ 
  void openTransformToolInfoEditor(); 

  /*! Open tool info editor for image */ 
  void openImageToolInfoEditor(); 

  /*! Add new transform tool info to broadcaster */ 
  void addNewTransformToolInfo(const IgtToolInfo& toolInfo); 

  /*! Add new image tool info to broadcaster */ 
  void addNewImageToolInfo(const IgtToolInfo& toolInfo); 

  /*! Set broadcasting interval in ms */
  void setBroadcastingIntervalMs(int intervalMs); 

protected:

  /*! Data collector instance */
  vtkDataCollector* m_DataCollector;

  /*! OpenIGTLink broadcaster instance */
  vtkOpenIGTLinkBroadcaster* m_Broadcaster; 

  /*! Device set selector widget */
	DeviceSetSelectorWidget*	m_DeviceSetSelectorWidget;

	/*! Label on the left of the statusbar */
	QLabel*					    m_StatusBarLabel;

  /*! Timer that refreshes the UI */
  QTimer*             m_UiRefreshTimer;

  /*! Timer that sends OpenIGTLink messages */
  QTimer*             m_BroadcastingIntervalTimer;

  /*! Broadcasting interval in ms */
  int m_BroadcastingIntervalMs; 

  /*! Status icon instance */
  StatusIcon*         m_StatusIcon;

  /*! Container for transform tool info widgets */
  std::deque<BroadcastedToolInfoWidget*> m_BroadcastedTransformsInfo; 

  /*! Image tool info widget instance */
  BroadcastedToolInfoWidget* m_BroadcastedImageInfo; 

private:
	Ui::PlusBroadcasterDaemonMainWindow	ui;

};

#endif 
