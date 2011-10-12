#ifndef VOLUMERECONSTRUCTIONTOOLBOX_H
#define VOLUMERECONSTRUCTIONTOOLBOX_H

#include "ui_VolumeReconstructionToolbox.h"

#include "AbstractToolbox.h"
#include "PlusConfigure.h"

#include <QWidget>

class vtkVolumeReconstructor;
class vtkImageData;

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

	/*!
	* \brief Initialize toolbox (load session data) - overridden method
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
	* \brief Reconstructs volume from input file
	* \param aInputImage Input sequence metafile image
	* \return Success flag
	*/
	PlusStatus ReconstructVolumeFromInputImage(std::string aInputImage);

	/*!
	* \brief Saves volume to file
	* \param aOutput Output file
	* \return Success flag
	*/
	PlusStatus SaveVolumeToFile(std::string aOutput);

	/*!
	* \brief Display reconstructed volume in canvas
	*/
	void DisplayReconstructedVolume();

protected slots:
	/*!
	* \brief Slot handling open volume reconstruction config button click
	*/
	void OpenVolumeReconstructionConfig();

	/*!
	* \brief Slot handling open input image button click
	*/
	void OpenInputImage();

	/*!
	* \brief Slot handling open reconstruct button click
	*/
	void Reconstruct();

	/*!
	* \brief Slot handling open save button click
	*/
	void Save();

protected:
	//! Volume reconstructor instance
	vtkVolumeReconstructor*	m_VolumeReconstructor;

	//! Reconstructed volume
	vtkImageData*			      m_ReconstructedVolume;

	//! Flag indicating whether a volume reconstruction config file has been loaded successfully
	bool					          m_VolumeReconstructionConfigFileLoaded;

  //! Contouring threshold
	double					        m_ContouringThreshold;

protected:
	Ui::VolumeReconstructionToolbox ui;
};

#endif
