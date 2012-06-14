#include "ObservedCmdCtrlState.h"
#include "ObserverCmdCtrlState.h"
#include "LogLibrary.h"

ObservedCmdCtrlState::ObservedCmdCtrlState()
{
		
};


ObservedCmdCtrlState::~ObservedCmdCtrlState()
{

}


void ObservedCmdCtrlState::Attach(ObserverCmdCtrlState* observer)
{
	observerList.push_back(observer);
	observer->SetObserved(this);
}


void ObservedCmdCtrlState::Detach(ObserverCmdCtrlState* observer)
{
	
	for (unsigned int n = 0; n < observerList.size(); n++)
	{
		if (observerList[n] == observer)
		{
			observerList.erase(observerList.begin() + n);
			break;
		}
	}
}


void ObservedCmdCtrlState:: Notify()
{

	for (unsigned int n = 0; n < this->observerList.size(); n++)
	{
		observerList[n]->Update();
	}
}

