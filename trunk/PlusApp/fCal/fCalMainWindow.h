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
	ToolboxType_VolumeReconstruction
};

//-----------------------------------------------------------------------------

/*!
* \brief Main window of the freehand calibration application
*/
class fCalMainWindow : public QMainWindow
{
	Q_OBJECT

public:
	/*!
	* \brief Constructor
	* \param aParent parent
	* \param aFlags widget flag
	*/
	fCalMainWindow(QWidget *parent = 0, Qt::WFlags flags = 0);

	/*!
	* \brief Destructor
	*/
	~fCalMainWindow();

	/*!
	* \brief Initialize controller, toolboxes, canvas and connect to devices
	*/
	void Initialize();

	/*!
	* \brief Get tool visualizer object
  * \return Tool visualizer
	*/
  vtkToolVisualizer* GetToolVisualizer() { return m_ToolVisualizer; };

	/*!
	* \brief Set status bar text
  * \param aText Status bar text
	*/
  void SetStatusBarText(QString aText);

	/*!
	* \brief Set status bar progress
  * \param aPercent Progress percent of the status bar (if -1 then hide progress bar)
	*/
  void SetStatusBarProgress(int aPercent);

	/*!
	* \brief Enable/disable tab changing
	* \param Enable/Disable flag
	*/
	void SetTabsEnabled(bool);

	/*!
	* \brief Reset all toolboxes and hide all tools (called when disconnected from a device set)
	*/
  void ResetAllToolboxes();

	/*!
	* \brief Return a toolbox
  * \param aType Toolbox type identifier
  * \return Toolbox object
	*/
  AbstractToolbox* GetToolbox(ToolboxType aType) { return m_ToolboxList[aType]; };

protected:
	/*!
	* \brief Create toolboxes
	*/
	void CreateToolboxes();

	/*!
	* \brief Set up status bar (label and progress)
	*/
	void SetupStatusBar();

protected slots:
	/*!
	* \brief Handle tab change
	* \param aTabIndex Index of the currently active tab
	*/
	void CurrentTabChanged(int aTabIndex);

	/*!
	* \brief Changes tab back to the locked one if tabbing is disabled
	* \param Mandatory but unused argument to match the signal
	*/
	void ChangeBackTab(int);

	/*!
	* \brief Updates every part of the GUI (called by ui refresh timer)
	*/
	void UpdateGUI();

	/*!
	* \brief Resize event handler
  * \param aEvent Resize event
	*/
  virtual void resizeEvent(QResizeEvent* aEvent);

protected:
  //! Tool visualizer
  vtkToolVisualizer*  m_ToolVisualizer;

	//! Label on the left of the statusbar
	QLabel*					    m_StatusBarLabel;

	//! Progress bar on the right of the statusbar
	QProgressBar*		    m_StatusBarProgress;

	//! Index of locked (current) tab if tabbing is disabled
	int							    m_LockedTabIndex;

	//! Active toolbox identifier
	ToolboxType			    m_ActiveToolbox;

  //! Timer that refreshes the UI
  QTimer*             m_UiRefreshTimer;

  //! Status icon instance
  StatusIcon*         m_StatusIcon;

  //! List of toolbox objects (the indices are the type identifiers)
  std::vector<AbstractToolbox*> m_ToolboxList;

private:
	Ui::fCalMainWindow	ui;

};

#endif // FREEHANDMAINWINDOW_H
