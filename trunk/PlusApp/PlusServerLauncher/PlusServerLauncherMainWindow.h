/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __PlusServerLauncherMainWindow_h
#define __PlusServerLauncherMainWindow_h

#include "PlusConfigure.h"

#include <QDialog>
#include <QProcess>

class DeviceSetSelectorWidget;
class vtkPlusOpenIGTLinkServer;
class vtkDataCollector;
class vtkTransformRepository;

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

  /*! Called whenever a key is pressed while the windows is active, used for intercepting the ESC key */
  void keyPressEvent(QKeyEvent *e);

  void stdOutMsgReceived();

  void stdErrMsgReceived();

  void errorReceived(QProcess::ProcessError);

  void serverExecutableFinished(int returnCode, QProcess::ExitStatus status);

protected:

  void sendServerOutputToLogger(const QByteArray &strData, vtkPlusLogger::LogLevelType defaultLogLevel);

  /*! Start server process, connect outputs to logger. Returns with true on success. */
  bool startServer(const QString& configFilePath);

  /*! Stop server process, disconnect outputs. Returns with true on success (shutdown on request was successful, without forcing). */
  bool stopServer();

  /*! Device set selector widget */
  DeviceSetSelectorWidget* m_DeviceSetSelectorWidget;

  /*! PlusServer instance that is responsible for all data collection and network transfer */
  QProcess* m_CurrentServerInstance;
};

#endif // __PlusServerLauncherMainWindow_h
