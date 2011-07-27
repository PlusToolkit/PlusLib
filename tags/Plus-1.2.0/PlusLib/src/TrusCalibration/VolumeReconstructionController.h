#ifndef VOLUMERECONSTRUCTIONCONTROLLER_H
#define VOLUMERECONSTRUCTIONCONTROLLER_H

#include "AbstractToolboxController.h"

class vtkVolumeReconstructor;
class vtkImageData;
class vtkActor;

//-----------------------------------------------------------------------------

/*!
* \brief Control operations for volume reconstruction toolbox - singleton class
*/
class VolumeReconstructionController : public AbstractToolboxController
{
public:
	/*!
	* \brief Instance getter for the singleton class
	* \return Instance object
	*/
	static VolumeReconstructionController* GetInstance();

	/*!
	* \brief Destructor
	*/
	virtual	~VolumeReconstructionController();

	/*!
	* \brief Initialize - implementation of a pure virtual function
	* \return Success flag
	*/
	PlusStatus Initialize();

	/*!
	* \brief Clear - implementation of a pure virtual function
	* \return Success flag
	*/
	PlusStatus Clear();

	/*!
	* \brief Acquire new tracker positions and updates toolbox and canvas - implementation of a pure virtual function
	* \return Success flag
	*/
	PlusStatus DoAcquisition();

	/*!
	* \brief Start - implementation of a pure virtual function
	* \return Success flag
	*/
	PlusStatus Start();

	/*!
	* \brief Stop - implementation of a pure virtual function
	* \return Success flag
	*/
	PlusStatus Stop();

	/*!
	* \brief Load volume reconstruction configuration from XML file
	* \param aFile XML file name and path
	* \return Success flag
	*/
	PlusStatus LoadVolumeReconstructionConfigFromFile(std::string aFile);

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
	* \brief Get whether a volume reconstruction config file has been loaded successfully
	* \return Success flag
	*/
	bool GetVolumeReconstructionConfigFileLoaded();

	/*!
	* \brief Getter function
	* \return Progress percent
	*/
	int GetProgressPercent();

	/*!
	* \brief Getter function
	* \return Progress message
	*/
	std::string GetProgressMessage();

	/*!
	* \brief Getter function
	* \return Reconstructed volume
	*/
	vtkImageData* GetReconstructedVolume();

	/*!
	* \brief Setter function
	* \param aThreshold Contouring threshold
	*/
	void SetContouringThreshold(double aThreshold);

protected:
	/*!
	* \brief Constructor
	*/
	VolumeReconstructionController();

	/*!
	* \brief Initialize 3D visualization
	*/
	void InitializeVisualization();

	/*!
	* \brief Display reconstructed volume in canvas
	*/
	void DisplayReconstructedVolume();

protected:
	//! Volume reconstructor instance
	vtkVolumeReconstructor*	m_VolumeReconstructor;

	//! Flag indicating whether a volume reconstruction config file has been loaded successfully
	bool					m_VolumeReconstructionConfigFileLoaded;

	//! Progress percent
	int						m_ProgressPercent;

	//! Progress message
	std::string				m_ProgressMessage;

	//! Reconstructed volume
	vtkImageData*			m_ReconstructedVolume;

	//! Contour actor
	vtkActor*				m_ContourActor;

	//! Contouring threshold
	double					m_ContouringThreshold;

private:
	//! Instance of the singleton
	static VolumeReconstructionController*	m_Instance;
};

#endif
