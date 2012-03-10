/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#include "vtkAccurateTimer.h"
#include "vtkObjectFactory.h"
#include "vtkTimerLog.h"
#include "vtksys/SystemTools.hxx"
#include <sstream>
#include <time.h>

#ifdef _WIN32
#include "WindowsAccurateTimer.h"
WindowsAccurateTimer WindowsAccurateTimer::m_Instance;
double vtkAccurateTimer::SystemStartTime=0;
#endif


//----------------------------------------------------------------------------
vtkCxxRevisionMacro(vtkAccurateTimer, "$Revision: 1.0 $");
vtkInstantiatorNewMacro(vtkAccurateTimer);

//----------------------------------------------------------------------------
// The singleton, and the singleton cleanup

vtkAccurateTimer* vtkAccurateTimer::Instance = NULL;
vtkAccurateTimerCleanup vtkAccurateTimer::Cleanup; 

vtkAccurateTimerCleanup::vtkAccurateTimerCleanup(){}

vtkAccurateTimerCleanup::~vtkAccurateTimerCleanup()
{
	// Destroy any remaining output window.
	vtkAccurateTimer::SetInstance(NULL);
}

//----------------------------------------------------------------------------
vtkAccurateTimer* vtkAccurateTimer::GetInstance()
{
	if (!vtkAccurateTimer::Instance)
	{
		vtkAccurateTimer::Instance = static_cast<vtkAccurateTimer *>(
			vtkObjectFactory::CreateInstance("vtkAccurateTimer"));
		if (!vtkAccurateTimer::Instance)
		{
			vtkAccurateTimer::Instance = new vtkAccurateTimer;
		}
    vtkAccurateTimer::Instance->SystemStartTime = vtkAccurateTimer::Instance->GetInternalSystemTime(); 
	}
	return vtkAccurateTimer::Instance;
}

//----------------------------------------------------------------------------
void vtkAccurateTimer::SetInstance(vtkAccurateTimer* instance)
{
	if (vtkAccurateTimer::Instance == instance)
	{
		return;
	}

	if (vtkAccurateTimer::Instance)
	{
		vtkAccurateTimer::Instance->Delete();
	}

	vtkAccurateTimer::Instance = instance;

	// User will call ->Delete() after setting instance

	if (instance)
	{
		instance->Register(NULL);
	}
}

//----------------------------------------------------------------------------
vtkAccurateTimer* vtkAccurateTimer::New()
{
	vtkAccurateTimer* ret = vtkAccurateTimer::GetInstance();
	ret->Register(NULL);
	return ret;
} 

//----------------------------------------------------------------------------
vtkAccurateTimer::vtkAccurateTimer()
{

}

//----------------------------------------------------------------------------
vtkAccurateTimer::~vtkAccurateTimer()
{

}

//----------------------------------------------------------------------------
void vtkAccurateTimer::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
void vtkAccurateTimer::Delay(double sec)
{
#ifdef _WIN32
	WindowsAccurateTimer* timer = WindowsAccurateTimer::Instance();
	timer->Wait( sec * 1000 ); 
#else
	vtksys::SystemTools::Delay( sec * 1000 ); 
#endif
}

//----------------------------------------------------------------------------
double vtkAccurateTimer::GetInternalSystemTime()
{
#ifdef _WIN32
	return WindowsAccurateTimer::GetSystemTime(); 
#else
	return vtkTimerLog::GetUniversalTime();
#endif
}

//----------------------------------------------------------------------------
double vtkAccurateTimer::GetSystemTime()
{
  return (vtkAccurateTimer::GetInternalSystemTime() - vtkAccurateTimer::SystemStartTime); 
}

//----------------------------------------------------------------------------
std::string vtkAccurateTimer::GetDateString()
{
	char dateStr[128];
	_strdate_s( dateStr, 128 );
	std::string dateInString(dateStr);
	dateInString.erase(5,1);	// remove '/'
	dateInString.erase(2,1);	// remove '/'
	// DATE IN STRING: [MMDDYY]
	return dateInString; 
}

//----------------------------------------------------------------------------
std::string vtkAccurateTimer::GetTimeString()
{
	char timeStr[128];
	_strtime_s( timeStr, 128 );
	std::string timeInString(timeStr);
	timeInString.erase(5,1);	// remove ':'
	timeInString.erase(2,1);	// remove ':'
	// TIME IN STRING: [HHMMSS]
	return timeInString; 
}

//----------------------------------------------------------------------------
std::string vtkAccurateTimer::GetDateAndTimeString()
{
	std::string date = vtkAccurateTimer::GetInstance()->GetDateString(); 

	std::string time = vtkAccurateTimer::GetInstance()->GetTimeString(); 

	// DATE AND TIME IN STRING: [MMDDYY_HHMMSS]
	std::string dateAndTime = (date + "_" + time); 
	return dateAndTime; 
}

//----------------------------------------------------------------------------
std::string vtkAccurateTimer::GetDateAndTimeMSecString()
{
	std::string date = vtkAccurateTimer::GetInstance()->GetDateString(); 

	std::string time = vtkAccurateTimer::GetInstance()->GetTimeString(); 

	struct _timeb tstruct;
	_ftime_s( &tstruct );
	unsigned short ms = tstruct.millitm; 

	// DATE AND TIME WITH MS IN STRING: [MMDDYY_HHMMSS.MS]
	std::ostringstream dateAndTimeMs; 
	dateAndTimeMs << date << "_"  << time << "." << std::setw(3) << std::right << std::setfill('0') << ms;  
	return dateAndTimeMs.str(); 
}
