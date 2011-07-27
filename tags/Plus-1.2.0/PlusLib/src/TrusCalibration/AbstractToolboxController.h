#ifndef ABSTRACTTOOLBOXCONTROLLER_H
#define ABSTRACTTOOLBOXCONTROLLER_H

#include "AbstractToolbox.h"

#include "PlusConfigure.h"

//-----------------------------------------------------------------------------

enum ToolboxState
{
	ToolboxState_Uninitialized = 0,
	ToolboxState_Idle,
	ToolboxState_InProgress,
	ToolboxState_Done,
	ToolboxState_Error
};

//-----------------------------------------------------------------------------

/*!
* \brief  This class is the super class of all the toolboxes for standard handling
*/
class AbstractToolboxController
{

protected:
	/*!
	* \brief Constructor
	*/
	AbstractToolboxController()
		:m_Toolbox()
		,m_State(ToolboxState_Uninitialized)
	{ };

public:
	/*!
	* \brief Destructor
	*/
	virtual ~AbstractToolboxController() { };

	/*!
	* \brief Initialize - pure virtual function
	* \return Success flag
	*/
	virtual PlusStatus Initialize() = 0;

	/*!
	* \brief Clear (deinitialize) - pure virtual function
	* \return Success flag
	*/
	virtual PlusStatus Clear() = 0;

	/*!
	* \brief Acquire data - pure virtual function
	* \return Success flag
	*/
	virtual PlusStatus DoAcquisition() = 0;

	/*!
	* \brief Start process - pure virtual function
	* \return Success flag
	*/
	virtual PlusStatus Start() = 0;

	/*!
	* \brief Stop process - pure virtual function
	* \return Success flag
	*/
	virtual PlusStatus Stop() = 0;

	/*!
	* \brief Setter function
	* \param aToolbox Toolbox object for the toolbox controller
	*/
	void SetToolbox(AbstractToolbox* aToolbox) { m_Toolbox = aToolbox; };

	/*!
	* \brief Getter function
	* \return Toolbox object for the toolbox controller
	*/
	AbstractToolbox* GetToolbox() { return m_Toolbox; };

	/*!
	* \brief Getter function
	* \return Toolbox state
	*/
	ToolboxState State() { return m_State; };

protected:
	//! Toolbox of the controller
	AbstractToolbox*					m_Toolbox;

	//! Toolbox state
	ToolboxState						m_State;
};

#endif
