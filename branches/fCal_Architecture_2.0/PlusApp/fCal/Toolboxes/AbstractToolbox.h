#ifndef ABSTRACTTOOLBOX_H
#define ABSTRACTTOOLBOX_H

class vtkFCalVisualizer;

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
class AbstractToolbox
{
public:
	/*!
	* \brief Constructor
	*/
	AbstractToolbox() {	};

	/*!
	* \brief Destructor
	*/
	virtual ~AbstractToolbox() { };

	/*!
	* \brief Refresh contents (e.g. GUI elements) of toolbox according to the state in the toolbox controller - pure virtual function
	*/
	virtual PlusStatus RefreshToolboxContent() = 0;

	/*!
	* \brief Initialize toolbox (eg. based on session data)
	*/
	virtual PlusStatus Initialize() = 0;

	/*!
	* \brief Start process - pure virtual function
	* \return Success flag
	*/
	virtual PlusStatus Start() = 0;

  /*!
	* \brief Execute operations needed after stopping the process
	*/
	virtual PlusStatus Stop() = 0;

	/*!
	* \brief Clear (deinitialize) the toolbox
	*/
	virtual PlusStatus Clear() = 0;

protected:
	/*!
	* \brief Calls DoAcquisition function in controller
	*/
	virtual void RequestDoAcquisition() {};

protected:
	//! Toolbox state
	ToolboxState						m_State;

  //! TODO
  vtkFCalVisualizer*      m_Visualizer;
};

#endif
