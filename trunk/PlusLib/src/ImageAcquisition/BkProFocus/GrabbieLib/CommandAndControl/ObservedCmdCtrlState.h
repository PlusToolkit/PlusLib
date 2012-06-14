#pragma once

#include "CmdCtrlTypes.h"
#include "OemParams.h"
#include <vector>

class ObserverCmdCtrlState;   // Forward declaration 

/// <summary> A base class for a objects that can attach observers.
/// A class that will be observed must:
/// \li Inherit ObservedCmdCtrlState
/// \li Implement a function Notify()
/// \li Implement the function GetScannerState()
/// \li Implement the function GetCmdCtrlState()
/// 
/// ObservedCmdCtrlState implements the operations related to 
/// attaching / detaching observers.
/// 
///  </summary>
class ObservedCmdCtrlState
{
public:
	ObservedCmdCtrlState();
	~ObservedCmdCtrlState();

	/// <summary> Attach a new observer. The observer must be an object of 
	/// type ObserverCmdCtrlState, or one of its descendents </summary>
	/// <param name="observer">	[in] If non-null, pointer to the observer. </param>
	void Attach(ObserverCmdCtrlState* observer);

	/// <summary> Detach an observer. The observer must be an object of 
	/// type ObserverCmdCtrlState, or one of its descendents.
	/// The object itself is NOT freed from memory  </summary>
	/// <param name="observer">	[in] If non-null, pointer to the observer. </param>
	void Detach(ObserverCmdCtrlState* observer);

	/// <summary> Notifies all observers about a change in the state </summary>
	virtual void Notify();

	/// <summary> Get the state of the scanner </summary>
	/// <returns> State of the scanner. </returns>
	virtual ScannerState GetScannerState() = 0;

	/// <summary> Gets the state of the CommandAndControl object. </summary>
	/// <returns> The state of CommandAndControl object </returns>
	virtual CmdCtrlState GetCmdCtrlState() = 0;

	virtual ScannerMode GetScannerMode() = 0;

public:
	std::vector<ObserverCmdCtrlState* > observerList;    //!< List of observers
};



