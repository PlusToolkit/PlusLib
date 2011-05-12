#ifndef FREEHANDCALIBRATIONTOOLBOX_H
#define FREEHANDCALIBRATIONTOOLBOX_H

#include "ui_FreehandCalibrationToolbox.h"

#include "AbstractToolbox.h"
#include "FreehandCalibrationController.h"

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
	* \brief Calls acquire positions function in controller (called by the acquisition timer) - implementation of a pure virtual function
	*/
	void RequestDoAcquisition();

	//TODO
	void OpenPhantomDefinitionClicked();
	void OpenPhantomRegistrationClicked();
	void OpenCalibrationConfigurationClicked();
	void StartTemporalClicked();
	void ResetTemporalClicked();
	void StartSpatialClicked();
	void ResetSpatialClicked();
	void SaveClicked();

protected:
	Ui::FreehandCalibrationToolbox ui;

	//! Timer for acquisition
	QTimer*	m_AcquisitionTimer;
};

#endif
