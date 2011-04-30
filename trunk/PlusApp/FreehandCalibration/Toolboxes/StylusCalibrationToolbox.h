#ifndef STYLUSCALIBRATIONTOOLBOX_H
#define STYLUSCALIBRATIONTOOLBOX_H

#include "ui_StylusCalibrationToolbox.h"

#include "AbstractToolbox.h"
#include "StylusCalibrationController.h"

#include <QWidget>

//-----------------------------------------------------------------------------

/*!
* \brief Stylus calibration toolbox view class
*/
class StylusCalibrationToolbox : public QWidget, public AbstractToolbox
{
	Q_OBJECT

public:
	/*!
	* \brief Constructor
	* \param aParent parent
	* \param aFlags widget flag
	*/
	StylusCalibrationToolbox(QWidget* aParent = 0, Qt::WFlags aFlags = 0);

	/*!
	* \brief Destructor
	*/
	~StylusCalibrationToolbox();

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

signals:
	/*!
	* \brief Executes operations needed after stopping the process
	* \param Enable/disable flag
	*/
	void SetTabsEnabled(bool);

protected slots:
	/*!
	* \brief Start calibration
	*/
	void StartClicked();

	/*!
	* \brief Start calibration
	*/
	void StopClicked();

	/*!
	* \brief Save result to XML file
	*/
	void SaveResultClicked();

	/*!
	* \brief Calls acquire positions function in controller (called by the acquisition timer) - implementation of a pure virtual function
	*/
	void RequestDoAcquisition();

protected:
	Ui::StylusCalibrationToolbox ui;

	//! Timer for acquisition
	QTimer*	m_AcquisitionTimer;
};

#endif
