#pragma once

#include "CmdCtrlTypes.h"

class ObservedCmdCtrlState;

/// <summary> Class that can be used to monitor the 
/// state of the scanner. 
/// This is a simplified version of the observer
/// design pattern (e.g. http://www.dofactory.com/Patterns/PatternObserver.aspx)
/// The idea is that the object of type 
/// CommandAndControl (CommandAndControl is a singleton) has states. 
/// CommandAndControl objects can subscribe to events from the scanner.
/// When the scanner changes state (Freeze/Unfreeze) CommandAndControl 
/// updates the information about the scanner state. A client application
/// can register an observer and visualize the changes to the user.
/// 
/// The observer must implement one function only - \c ObserverCmdCtrlState::Update()
/// In this function, the observer can query the Observed object about state and other
/// variables.
/// </summary>
class ObserverCmdCtrlState
{
public:
	ObserverCmdCtrlState()
	{
		this->observed = NULL;
	}

	virtual void Update() = 0;

	void SetObserved(ObservedCmdCtrlState* _observed)
	{
		this->observed = _observed;
	};

protected:
	ObservedCmdCtrlState* observed;
};


