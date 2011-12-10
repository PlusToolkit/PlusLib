/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __fCalMainWindow_h
#define __fCalMainWindow_h

#include "ui_fCalMainWindow.h"

#include "PlusConfigure.h"
#include <QtGui/QMainWindow>

class vtkToolVisualizer;
class AbstractToolbox;
class StatusIcon;

class QLabel;
class QProgressBar;
class QTimer;

//-----------------------------------------------------------------------------

enum ToolboxType
{
	ToolboxType_Undefined = -1,
	ToolboxType_Configuration,
	ToolboxType_StylusCalibration,
	ToolboxType_PhantomRegistration,
	ToolboxType_FreehandCalibration,
  ToolboxType_Capturing,
	ToolboxType_VolumeReconstruction
};

//-----------------------------------------------------------------------------

/*! \class fCalMainWindow 
 * \brief Main window of the fCal application
 * \ingroup PlusAppFCal
 */
class fCalMainWindow : public QMainWindow
{
	Q_OBJECT

public:
	/*!
	* Constructor
	* \param aParent parent
	* \param aFlags widget flag
	*/
	fCalMainWindow(QWidget *parent = 0, Qt::WFlags flags = 0);

	/*!
	* Destructor
	*/
	~fCalMainWindow();

	/*!
	* Initialize controller, toolboxes, canvas and connect to devices
	*/
	void Initialize();

	/*!
	* Get tool visualizer object
  * \return Tool visualizer
	*/
  vtkToolVisualizer* GetToolVisualizer() { return m_ToolVisualizer; };

	/*!
	* Set status bar text
  * \param aText Status bar text
	*/
  void SetStatusBarText(QString aText);

	/*!
	* Set status bar progress
  * \param aPercent Progress percent of the status bar (if -1 then hide progress bar)
	*/
  void SetStatusBarProgress(int aPercent);

	/*!
	* Enable/disable tab changing
	* \param Enable/Disable flag
	*/
	void SetTabsEnabled(bool);

	/*!
	* Reset all toolboxes and hide all tools (called when disconnected from a device set)
	*/
  void ResetAllToolboxes();

	/*!
	* Return a toolbox
  * \param aType Toolbox type identifier
  * \return Toolbox object
	*/
  AbstractToolbox* GetToolbox(ToolboxType aType) { return m_ToolboxList[aType]; };

  /*! Get image coordinate frame name */
  std::string GetImageCoordinateFrame() { return m_ImageCoordinateFrame; };
  /*! Set image coordinate frame name */
  void SetImageCoordinateFrame(const char* aImageCoordinateFrame) { m_ImageCoordinateFrame = aImageCoordinateFrame; };

  /*! Get probe coordinate frame name */
  std::string GetProbeCoordinateFrame() { return m_ProbeCoordinateFrame; };
  /*! Set probe coordinate frame name */
  void SetProbeCoordinateFrame(const char* aProbeCoordinateFrame) { m_ProbeCoordinateFrame = aProbeCoordinateFrame; };

  /*! Get reference coordinate frame name */
  std::string GetReferenceCoordinateFrame() { return m_ReferenceCoordinateFrame; };
  /*! Set reference coordinate frame name */
  void SetReferenceCoordinateFrame(const char* aReferenceCoordinateFrame) { m_ReferenceCoordinateFrame = aReferenceCoordinateFrame; };

protected:
	/*!
	* Create toolboxes
	*/
	void CreateToolboxes();

	/*!
	* Set up status bar (label and progress)
	*/
	void SetupStatusBar();

  /*!
	* Filters events if this object has been installed as an event filter for the watched object
	* \param obj object
	* \param ev event
	* \return if you want to filter the event out, i.e. stop it being handled further, return true; otherwise return false
	*/
	bool eventFilter(QObject *obj, QEvent *ev);

protected slots:
	/*!
	* Handle tab change
	* \param aTabIndex Index of the currently active tab
	*/
	void CurrentTabChanged(int aTabIndex);

	/*!
	* Changes tab back to the locked one if tabbing is disabled
	* \param Mandatory but unused argument to match the signal
	*/
	void ChangeBackTab(int);

	/*!
	* Updates every part of the GUI (called by ui refresh timer)
	*/
	void UpdateGUI();

	/*!
	* Resize event handler
  * \param aEvent Resize event
	*/
  virtual void resizeEvent(QResizeEvent* aEvent);

  /*! Save buffers into files */
  void DumpBuffers();

  /*! Save current device set configuration */
  void SaveDeviceSetConfiguration();

protected:
  /*! Tool visualizer */
  vtkToolVisualizer*  m_ToolVisualizer;

	/*! Label on the left of the statusbar */
	QLabel*					    m_StatusBarLabel;

	/*! Progress bar on the right of the statusbar */
	QProgressBar*		    m_StatusBarProgress;

	/*! Index of locked (current) tab if tabbing is disabled */
	int							    m_LockedTabIndex;

	/*! Active toolbox identifier */
	ToolboxType			    m_ActiveToolbox;

  /*! Timer that refreshes the UI */
  QTimer*             m_UiRefreshTimer;

  /*! Status icon instance */
  StatusIcon*         m_StatusIcon;

  /*! List of toolbox objects (the indices are the type identifiers) */
  std::vector<AbstractToolbox*> m_ToolboxList;

  /*! Image coordinate frame name for Volume reconstruction */
  std::string m_ImageCoordinateFrame;

  /*! Probe coordinate frame name for Capturing */
  std::string m_ProbeCoordinateFrame;

  /*! Reference coordinate frame name for Capturing */
  std::string m_ReferenceCoordinateFrame;

private:
	Ui::fCalMainWindow	ui;

};

#endif // FREEHANDMAINWINDOW_H
