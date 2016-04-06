/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __PlusServerLauncherMainWindow_h
#define __PlusServerLauncherMainWindow_h

#include "PlusConfigure.h"
#include "ui_PlusServerLauncherMainWindow.h"

#include <QMainWindow>
#include <QProcess>

class DeviceSetSelectorWidget;
class vtkPlusOpenIGTLinkServer;
class vtkPlusDataCollector;
class vtkPlusTransformRepository;
class QComboBox;
class QWidget;

//-----------------------------------------------------------------------------

/*!
  \class PlusServerLauncherMainWindow 
  \brief GUI application for starting an OpenIGTLink server with the selected device configuration file
  \ingroup PlusAppPlusServerLauncher
 */
class PlusServerLauncherMainWindow : public QMainWindow
{
  Q_OBJECT

public:
  /*!
    Constructor
    \param aParent parent
    \param aFlags widget flag
  */
  PlusServerLauncherMainWindow(QWidget *parent = 0, Qt::WindowFlags flags = 0, bool autoConnect=false);

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

  void logLevelChanged();

protected:
  /*! Receive standard output or error and send it to the log */
  void sendServerOutputToLogger(const QByteArray &strData);

  /*! Start server process, connect outputs to logger. Returns with true on success. */
  bool startServer(const QString& configFilePath);

  /*! Stop server process, disconnect outputs. Returns with true on success (shutdown on request was successful, without forcing). */
  bool stopServer();

  /*! Parse a given log line for salient information from the PlusServer */
  void ParseContent(const std::string& message);

protected:
  /*! Device set selector widget */
  DeviceSetSelectorWidget* m_DeviceSetSelectorWidget;

  /*! PlusServer instance that is responsible for all data collection and network transfer */
  QProcess* m_CurrentServerInstance;

  /*! List of active ports for PlusServers */
  std::vector<int> PortList;

private:
  Ui::PlusServerLauncherMainWindow ui;
};

#endif // __PlusServerLauncherMainWindow_h
