/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __PlusServerLauncherMainWindow_h
#define __PlusServerLauncherMainWindow_h

#include "PlusConfigure.h"

#include <QDialog>

class DeviceSetSelectorWidget;
class vtkPlusOpenIGTLinkServer;

//-----------------------------------------------------------------------------

/*!
  \class PlusServerLauncherMainWindow 
  \brief GUI application for starting an OpenIGTLink server with the selected device configuration file
  \ingroup PlusAppPlusServerLauncher
 */
class PlusServerLauncherMainWindow : public QDialog
{
  Q_OBJECT

public:
  /*!
    Constructor
    \param aParent parent
    \param aFlags widget flag
  */
  PlusServerLauncherMainWindow(QWidget *parent = 0, Qt::WFlags flags = 0, bool autoConnect=false);

  /*! Destructor */
  ~PlusServerLauncherMainWindow();

protected slots:
  /*!
    Connect to devices described in the argument configuration file in response by clicking on the Connect button
    \param aConfigFile DeviceSet configuration file path and name
  */
  void connectToDevicesByConfigFile(std::string);

  /*! Processes pending OpenIGTLink command requests and sends responses */
	void processPendingCommands();

  /*! Called whenever a key is pressed while the windows is active, used for intercepting the ESC key */
  void keyPressEvent(QKeyEvent *e);

protected:
  /*! Device set selector widget */
  DeviceSetSelectorWidget* m_DeviceSetSelectorWidget;

  /*! PlusServer instance that is responsible for all data collection and network transfer */
  vtkPlusOpenIGTLinkServer* m_Server;

  /*! Timer that refreshes the UI */
  QTimer* m_ProcessPendingCommandsTimer; 
};

#endif // __PlusServerLauncherMainWindow_h
