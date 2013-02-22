/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __CaptureClientWindow_h
#define __CaptureClientWindow_h

#include "PlusConfigure.h"
#include "ui_CaptureClientWindow.h"
#include <QtGui/QMainWindow>

class CaptureControlWidget;
class vtkDataCollector;
class vtkPlusChannel;

//-----------------------------------------------------------------------------

/*! \class CaptureClientWindow 
* \brief Main window of the CaptureClient application
* \ingroup PlusAppCaptureClient
*/
class CaptureClientWindow : public QMainWindow
{
  Q_OBJECT

public:
  /*!
  * Constructor
  * \param aParent parent
  * \param aFlags widget flag
  */
  CaptureClientWindow(QWidget *parent = 0, Qt::WFlags flags = 0);

  /*!
  * Destructor
  */
  ~CaptureClientWindow();

  /*!
  * Read capture client configuration
  * \param aConfig Root element of the configuration XML data element
  */
  PlusStatus ReadConfiguration(vtkXMLDataElement* aConfig);

 protected slots:
    /*!
    * Connect to devices described in the argument configuration file in response by clicking on the Connect button
    * \param aConfigFile DeviceSet configuration file path and name
    */
    void ConnectToDevicesByConfigFile(std::string aConfigFile);

protected:
  PlusStatus ConfigureCaptureWidgets();

  PlusStatus StartDataCollection();

  vtkDataCollector* m_DataCollector;

  vtkPlusChannel* m_SelectedChannel;

  std::vector<CaptureControlWidget*> m_CaptureWidgets;

private:
  Ui::CaptureClientMainWindow	ui;
};

#endif // FCALMAINWINDOW_H
