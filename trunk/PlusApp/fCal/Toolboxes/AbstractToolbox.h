/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __AbstractToolbox_h
#define __AbstractToolbox_h

class fCalMainWindow;

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

/*! \class AbstractToolbox 
 *
 * \brief This class is the super class of all the toolboxes for standard handling
 *
 * \ingroup PlusAppFCal
 *
 */
class AbstractToolbox
{
public:
	/*!
	* \brief Constructor
	*/
	AbstractToolbox(fCalMainWindow* aParentMainWindow)
  {
    m_ParentMainWindow = aParentMainWindow;
		m_State = ToolboxState_Uninitialized;
  };

	/*!
	* \brief Destructor
	*/
	virtual ~AbstractToolbox() { };

	/*!
	* \brief Refresh contents (e.g. GUI elements) of toolbox according to the state in the toolbox controller - pure virtual function (implemented as slots!)
	*/
	virtual void RefreshContent() = 0;

	/*!
	* \brief Initialize toolbox (eg. based on session data) - pure virtual function
	*/
	virtual void Initialize() = 0;

	/*!
	* \brief Reset toolbox to initial state - virtual function
	*/
  virtual void Reset() { };

  /*!
	* \brief Sets display mode (visibility of actors) according to the current state - pure virtual function (implemented as slots!)
	*/
	virtual void SetDisplayAccordingToState() = 0;

	/*!
	* \brief Properly sets new toolbox state
  * \param aState New state of the toolbox
	*/
  void SetState(ToolboxState aState) {
    m_State = aState;
    SetDisplayAccordingToState();
  };

	/*!
	* \brief Get toolbox state
  * \return Current state
	*/
  ToolboxState GetState() { return m_State; };

protected:
  //! Main window object
  fCalMainWindow* m_ParentMainWindow;

	//! Toolbox state
	ToolboxState		m_State;

};

#endif
