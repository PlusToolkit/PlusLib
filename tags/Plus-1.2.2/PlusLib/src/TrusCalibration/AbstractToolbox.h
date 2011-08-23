#ifndef ABSTRACTTOOLBOX_H
#define ABSTRACTTOOLBOX_H

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
	virtual void RefreshToolboxContent() = 0;

	/*!
	* \brief Initialize toolbox (eg. based on session data)
	*/
	virtual void Initialize() {};

	/*!
	* \brief Execute operations needed after stopping the process
	*/
	virtual void Stop() {};

	/*!
	* \brief Clear (deinitialize) the toolbox
	*/
	virtual void Clear() = 0;

protected:
	/*!
	* \brief Calls DoAcquisition function in controller
	*/
	virtual void RequestDoAcquisition() {};

};

#endif
