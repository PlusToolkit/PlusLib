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
class QCustomAction;
class QTimer;
class vtkDataCollector;
class vtkPlusChannel;
class vtkPlusDevice;
class vtkVisualizationController;

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

    /*!
    * Activate a certain device
    */
    void ChannelSelected(vtkPlusDevice* aDevice, vtkPlusChannel* aChannel);

    /*!
    * Resize event handler
    * \param aEvent Resize event
    */
    virtual void resizeEvent(QResizeEvent* aEvent);

    /*!
    * Updates every part of the GUI (called by ui refresh timer)
    */
    void UpdateGUI();

    void StartAll();

    void StopAll();

protected:
  PlusStatus ConfigureCaptureWidgets();

  PlusStatus StartDataCollection();

  /*! Dynamically build the devices menu based on the values returned from the data collector */
  void BuildChannelMenu();

  /*! Accessors for selected channel functionality */
  void SetSelectedChannel(vtkPlusChannel& aChannel);
  vtkPlusChannel* GetSelectedChannel(){ return m_SelectedChannel; }

  /*!
  * Filters events if this object has been installed as an event filter for the watched object
  * \param obj object
  * \param ev event
  * \return if you want to filter the event out, i.e. stop it being handled further, return true; otherwise return false
  */
  bool eventFilter(QObject *obj, QEvent *ev);

  vtkDataCollector* m_DataCollector;

  vtkPlusChannel* m_SelectedChannel;

  std::vector<CaptureControlWidget*> m_CaptureWidgets;

  //vtkVisualizationController* m_VisualizationController;

  /*! Reference to all actions that will show up in ROI list */
  std::vector<QCustomAction*> m_3DActionList;

  /*! Timer that refreshes the UI */
  QTimer* m_UiRefreshTimer;

private:
  Ui::CaptureClientMainWindow	ui;
};

#endif // FCALMAINWINDOW_H
