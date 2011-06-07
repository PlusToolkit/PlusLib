#ifndef VOLUMERECONSTRUCTIONCONTROLLER_H
#define VOLUMERECONSTRUCTIONCONTROLLER_H

#include "AbstractToolboxController.h"

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
	*/
	PlusStatus Initialize();

	/*!
	* \brief Clear - implementation of a pure virtual function
	*/
	PlusStatus Clear();

	/*!
	* \brief Acquire new tracker positions and updates toolbox and canvas - implementation of a pure virtual function
	*/
	PlusStatus DoAcquisition();

	/*!
	* \brief Start - implementation of a pure virtual function
	*/
	PlusStatus Start();

	/*!
	* \brief Stop - implementation of a pure virtual function
	*/
	PlusStatus Stop();

protected:
	/*!
	* \brief Constructor
	*/
	VolumeReconstructionController();

protected:

private:
	//! Instance of the singleton
	static VolumeReconstructionController*	m_Instance;
};

#endif
