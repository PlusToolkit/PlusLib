#ifndef VOLUMERECONSTRUCTIONTOOLBOX_H
#define VOLUMERECONSTRUCTIONTOOLBOX_H

#include "ui_VolumeReconstructionToolbox.h"

#include "AbstractToolbox.h"
#include "PlusConfigure.h"

#include <QWidget>

//-----------------------------------------------------------------------------

/*!
* \brief Volume reconstruction toolbox view class
*/
class VolumeReconstructionToolbox : public QWidget, public AbstractToolbox
{
	Q_OBJECT

public:
	/*!
	* \brief Constructor
	* \param aParentMainWindow Parent main window
	* \param aFlags widget flag
	*/
	VolumeReconstructionToolbox(fCalMainWindow* aParentMainWindow, Qt::WFlags aFlags = 0);

	/*!
	* \brief Destructor
	*/
	~VolumeReconstructionToolbox();

//	/*!
//	* \brief Executes operations needed after stopping the process - implementation of a pure virtual function
//	*/
//	void Stop();
//
//	/*!
//	* \brief Executes operations needed when changing to another toolbox - implementation of a pure virtual function
//	*/
//	void Clear();

	/*!
	* \brief Initialize toolbox (load session data) - overridden method
	*/
	void Initialize();

public slots:
	/*!
	* \brief Refresh contents (e.g. GUI elements) of toolbox according to the state in the toolbox controller - implementation of a pure virtual function
	*/
	void RefreshContent();

	/*!
	* \brief Sets display mode (visibility of actors) according to the current state - implementation of a pure virtual function
	*/
	void SetDisplayAccordingToState();

//signals:
//	/*!
//	* \brief Executes operations needed after stopping the process
//	* \param Enable/disable flag
//	*/
//	void SetTabsEnabled(bool);
//
//protected slots:
//	/*!
//	* \brief Slot handling open volume reconstruction config button click
//	*/
//	void OpenVolumeReconstructionConfigClicked();
//
//	/*!
//	* \brief Slot handling open input image button click
//	*/
//	void OpenInputImageClicked();
//
//	/*!
//	* \brief Slot handling open reconstruct button click
//	*/
//	void ReconstructClicked();
//
//	/*!
//	* \brief Slot handling open save button click
//	*/
//	void SaveClicked();
//
//protected:
//	Ui::VolumeReconstructionToolbox ui;
};

#endif
