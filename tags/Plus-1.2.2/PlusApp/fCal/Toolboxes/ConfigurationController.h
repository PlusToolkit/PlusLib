#ifndef CONFIGURATIONCONTROLLER_H
#define CONFIGURATIONCONTROLLER_H

#include "AbstractToolboxController.h"

//-----------------------------------------------------------------------------

/*!
* \brief Control operations for volume reconstruction toolbox - singleton class
*/
class ConfigurationController : public AbstractToolboxController
{
public:
	/*!
	* \brief Instance getter for the singleton class
	* \return Instance object
	*/
	static ConfigurationController* GetInstance();

	/*!
	* \brief Destructor
	*/
	virtual	~ConfigurationController();

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

	//!TODO
	bool IsToolDisplayDetached();
	//!TODO
	void SetToolDisplayDetached(bool aDetached);

protected:
	/*!
	* \brief Constructor
	*/
	ConfigurationController();

protected:
	//! Flag indicating if tool state display widget is detached
	bool m_IsToolDisplayDetached;

private:
	//! Instance of the singleton
	static ConfigurationController*	m_Instance;
};

#endif
