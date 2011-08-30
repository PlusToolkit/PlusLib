#ifndef FREEHANDCALIBRATIONTOOLBOX_H
#define FREEHANDCALIBRATIONTOOLBOX_H

#include "ui_FreehandCalibrationToolbox.h"

#include "AbstractToolbox.h"

#include <QWidget>

//-----------------------------------------------------------------------------

/*!
* \brief Freehand calibration toolbox view class
*/
class FreehandCalibrationToolbox : public QWidget, public AbstractToolbox
{
	Q_OBJECT

public:
	/*!
	* \brief Constructor
	* \param aParent parent
	* \param aFlags widget flag
	*/
	FreehandCalibrationToolbox(QWidget* aParent = 0, Qt::WFlags aFlags = 0);

	/*!
	* \brief Destructor
	*/
	~FreehandCalibrationToolbox();

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
	* \brief Slot handling open phantom registration button click
	*/
	void OpenPhantomRegistrationClicked();

	/*!
	* \brief Slot handling open calibration configuration button click
	*/
	void OpenCalibrationConfigurationClicked();

	/*!
	* \brief Slot handling edit calibration configuration button click
	*/
	void EditCalibrationConfigurationClicked();

	/*!
	* \brief Slot handling start temporal calibration button click
	*/
	void StartTemporalClicked();

	/*!
	* \brief Slot handling reset temporal calibration button click
	*/
	void ResetTemporalClicked();

	/*!
	* \brief Slot handling skip temporal calibration button click
	*/
	void SkipTemporalClicked();

	/*!
	* \brief Slot handling start spatial calibration button click
	*/
	void StartSpatialClicked();

	/*!
	* \brief Slot handling reset spatial calibration button click
	*/
	void ResetSpatialClicked();

	/*!
	* \brief Slot handling save results button click
	*/
	void SaveClicked();

	/*!
	* \brief Slot handling show devices combobox state change
	* \param Slot aOn True if toggled, false otherwise
	*/
	void ShowDevicesToggled(bool aOn);

	/*!
	* \brief Calls acquire positions function in controller (called by the acquisition timer)
	*/
	void RequestDoAcquisition();

protected:
	Ui::FreehandCalibrationToolbox ui;

	//! Timer for acquisition (in device visualization mode)
	QTimer*	m_AcquisitionTimer;
};

#endif
