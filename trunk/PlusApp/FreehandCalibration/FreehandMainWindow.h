#ifndef FREEHANDMAINWINDOW_H
#define FREEHANDMAINWINDOW_H

#include "ui_FreehandMainWindow.h"

#include <QtGui/QMainWindow>

#include <QLabel>
#include <QProgressBar>

class AbstractToolboxController;

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

	/*!
	* \brief Callback function for logger to display messages in popup window
	* \param aMessage Message string to display
	* \param aLevel Log level of the message
	*/
	//static void DisplayMessage(const char* aMessage, const int aLevel); TODO remove (body too)

public slots:
	/*!
	* \brief Determint configuration files location
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

	/*!
	* \brief Get toolbox controller pointer by the type identifier
	* \param aType Toolbox type (ToolboxType enum)
	* \return Toolbox controller pointer as abstract toolbox controller
	*/
	AbstractToolboxController* GetToolboxControllerByType(ToolboxType aType);

	/*!
	* \brief Locates and sets directory paths to freehand controller
	*/
	void LocateDirectories();

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
	QLabel*						m_StatusBarLabel;

	//! Progress bar on the right of the statusbar
	QProgressBar*				m_StatusBarProgress;

	//! Index of locked (current) tab if tabbing is disabled
	int							m_LockedTabIndex;

	//! Active toolbox identifier
	ToolboxType					m_ActiveToolbox;

private:
	Ui::fCalMainWindow			ui;

};

#endif // FREEHANDMAINWINDOW_H
