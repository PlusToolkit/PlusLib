#ifndef FREEHANDCALIBRATIONTOOLBOX_H
#define FREEHANDCALIBRATIONTOOLBOX_H

#include "ui_FreehandCalibrationToolbox.h"

#include "AbstractToolbox.h"
#include "PlusConfigure.h"

#include <QWidget>

class vtkCalibrationController;

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
	* \param aParentMainWindow Parent main window
	* \param aFlags widget flag
	*/
	FreehandCalibrationToolbox(fCalMainWindow* aParentMainWindow, Qt::WFlags aFlags = 0);

	/*!
	* \brief Destructor
	*/
	~FreehandCalibrationToolbox();

	/*!
	* \brief Initialize toolbox (load session data) - implementation of a pure virtual function
	*/
	void Initialize();

	/*!
	* \brief Refresh contents (e.g. GUI elements) of toolbox according to the state in the toolbox controller - implementation of a pure virtual function
	*/
	void RefreshContent();

	/*!
	* \brief Sets display mode (visibility of actors) according to the current state - implementation of a pure virtual function
	*/
	void SetDisplayAccordingToState();

protected:
	/*!
	* \brief Determines if everything is OK to run spatial calibration
  * \return Ready flag
	*/
  bool IsReadyToStartSpatialCalibration();

	/*!
	* \brief Execute spatial calibration
  * \return Success flag
	*/
  PlusStatus DoSpatialCalibration();

	/*!
	* \brief Prepares and shows the currently segmented points (or hide if unsuccessful)
  * \param aSuccess True if segmentation was successful
  * \return Success flag
	*/
  PlusStatus DisplaySegmentedPoints(bool aSuccess);

protected slots:
	/*!
	* \brief Slot handling open phantom registration button click
	*/
	void OpenPhantomRegistration();

	/*!
	* \brief Slot handling open calibration configuration button click
	*/
	void OpenCalibrationConfiguration();

	/*!
	* \brief Slot handling edit calibration configuration button click
	*/
	void EditCalibrationConfiguration();

	/*!
	* \brief Slot handling start temporal calibration button click
	*/
	void StartTemporal();

	/*!
	* \brief Slot handling reset temporal calibration button click
	*/
	void ResetTemporal();

	/*!
	* \brief Slot handling skip temporal calibration button click
	*/
	void SkipTemporal();

	/*!
	* \brief Slot handling start spatial calibration button click
	*/
	void StartSpatial();

	/*!
	* \brief Slot handling reset spatial calibration button click
	*/
	void ResetSpatial();

	/*!
	* \brief Slot handling save results button click
	*/
	void Save();

	/*!
	* \brief Slot handling show devices combobox state change
	* \param Slot aOn True if toggled, false otherwise
	*/
	void ShowDevicesToggled(bool aOn);

protected:
	//! Flag indicating temporal calibration is done (needed for properly setting the state of UI elements)
	bool m_TemporalCalibrationDone;

	//! Flag indicating spatial calibration is done (needed for properly setting the state of UI elements)
  bool m_SpatialCalibrationDone;

  //! Calibration algorithm
  vtkCalibrationController* m_Calibration;

  //! Flag if cancel is requested
  bool m_CancelRequest;

protected:
	Ui::FreehandCalibrationToolbox ui;

};

#endif
