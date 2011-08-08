#ifndef RECORDPHANTOMPOINTSTOOLBOX_H
#define RECORDPHANTOMPOINTSTOOLBOX_H

#include "ui_PhantomRegistrationToolbox.h"

#include "AbstractToolbox.h"

#include <QWidget>

//-----------------------------------------------------------------------------

/*!
* \brief Phantom registration toolbox view class
*/
class PhantomRegistrationToolbox : public QWidget, public AbstractToolbox
{
	Q_OBJECT

public:
	/*!
	* \brief Constructor
	* \param aParent parent
	* \param aFlags widget flag
	*/
	PhantomRegistrationToolbox(QWidget* aParent = 0, Qt::WFlags aFlags = 0);

	/*!
	* \brief Destructor
	*/
	~PhantomRegistrationToolbox();

	/*!
	* \brief Refresh contents (e.g. GUI elements) of toolbox according to the state in the toolbox controller - implementation of a pure virtual function
	*/
	void RefreshToolboxContent();

	/*!
	* \brief Executes operations needed after stopping the process - implementation of a pure virtual function
	*/
	void Stop();

	/*!
	* \brief Executes operations needed when changing to another toolbox - implementation of a pure virtual function
	*/
	void Clear();

	/*!
	* \brief Initialize toolbox (load session data) - overridden method
	*/
	void Initialize();

signals:
	/*!
	* \brief Executes operations needed after stopping the process
	* \param Enable/disable flag
	*/
	void SetTabsEnabled(bool);

protected slots:
	/*!
	* \brief Calls acquire positions function in controller (called by the acquisition timer)
	*/
	void RequestDoAcquisition();

	/*!
	* \brief Slot handling open phantom definition button click
	*/
	void OpenPhantomDefinitionClicked();

	/*!
	* \brief Slot handling open stylus calibration button click
	*/
	void OpenStylusCalibrationClicked();

	/*!
	* \brief Slot handling record button click
	*/
	void RecordPointClicked();

	/*!
	* \brief Slot handling undo button click
	*/
	void UndoClicked();

	/*!
	* \brief Slot handling reset button click
	*/
	void ResetClicked();

	/*!
	* \brief Slot handling save button click
	*/
	void SaveClicked();

protected:
	Ui::PhantomRegistrationToolbox ui;

	//! Timer for acquisition
	QTimer*	m_AcquisitionTimer;
};

#endif
