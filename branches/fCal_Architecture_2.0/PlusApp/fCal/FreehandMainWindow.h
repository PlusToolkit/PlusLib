#ifndef FREEHANDMAINWINDOW_H
#define FREEHANDMAINWINDOW_H

#include "ui_FreehandMainWindow.h"

#include <QtGui/QMainWindow>

#include "vtkFreehandController.h"

class AbstractToolboxController;
class QLabel;
class QProgressBar;
class QTimer;

//-----------------------------------------------------------------------------

/*!
* \brief Main window of the freehand calibration application
*/
class FreehandMainWindow : public QMainWindow
{
	Q_OBJECT

public:
	/*!
	* \brief Constructor
	* \param aParent parent
	* \param aFlags widget flag
	*/
	FreehandMainWindow(QWidget *parent = 0, Qt::WFlags flags = 0);

	/*!
	* \brief Destructor
	*/
	~FreehandMainWindow();

	/*!
	* \brief Initialize controller, toolboxes, canvas and connect to devices
	*/
	void Initialize();

public slots:
	/*!
	* \brief Enable/disable tab changing
	* \param Enable/Disable flag
	*/
	void SetTabsEnabled(bool);

protected:
	/*!
	* \brief Create toolboxes
	*/
	void CreateToolboxes();

	/*!
	* \brief Set up status bar (label and progress)
	*/
	void SetupStatusBar();

	/*!
	* \brief Set up canvas for 3D visualization
	*/
	void SetupCanvas();

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

protected:
	//! Label on the left of the statusbar
	QLabel*					m_StatusBarLabel;

	//! Progress bar on the right of the statusbar
	QProgressBar*		m_StatusBarProgress;

	//! Index of locked (current) tab if tabbing is disabled
	int							m_LockedTabIndex;

  //! Timer that refreshes the UI
  QTimer*         m_UiRefreshTimer;

private:
	Ui::fCalMainWindow			ui;

};

#endif // FREEHANDMAINWINDOW_H
