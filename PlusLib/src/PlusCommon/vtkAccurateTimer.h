// .NAME vtkAccurateTimer - class for accurate time measurement
// .SECTION Description
// This class is used for accuretly measuring elapsed time 

#ifndef __vtkAccurateTimer_h
#define __vtkAccurateTimer_h

#include "vtkObject.h"

//----------------------------------------------------------------------------
// Singleton cleanup 
//BTX
class VTK_EXPORT vtkAccurateTimerCleanup
{
public:
  vtkAccurateTimerCleanup();
  ~vtkAccurateTimerCleanup();
};
//ETX 

class VTK_EXPORT vtkAccurateTimer : public vtkObject
{
public: 
	vtkTypeRevisionMacro(vtkAccurateTimer,vtkObject);
	void PrintSelf(ostream& os, vtkIndent indent); 

	// Description:
	// This is a singleton pattern New.  There will only be ONE
	// reference to a vtkAccurateTimer object per process.  Clients that
	// call this must call Delete on the object so that the reference
	// counting will work.   The single instance will be unreferenced when
	// the program exits.
	static vtkAccurateTimer* New();
	
	// Description:
	// Return the singleton instance with no reference counting.
	static vtkAccurateTimer* GetInstance();
	
	// Description:
	// Supply a user defined instance. Call Delete() on the supplied
	// instance after setting it to fix the reference count. 
	static void SetInstance(vtkAccurateTimer *instance); 

	// Description:
	// Wait until specified time in seconds
	static void Delay(double sec); 

	// Description:
	// Get system time (elapsed time since last reboot)
	static double GetInternalSystemTime(); 
  
  // Description:
	// Get the elapsed time since class instantiation
  static double GetSystemTime(); 
  
	// Description:
	// Get current date in string 
	// Format: [MMDDYY]
	static std::string GetDateString(); 

	// Description:
	// Get current time in string 
	// Format: [HHMMSS]
	static std::string GetTimeString(); 

	// Description:
	// Get current date with time in string 
	// Format: [MMDDYY_HHMMSS]
	static std::string GetDateAndTimeString(); 

	// Description:
	// Get current date with time ans ms in string 
	// Format: [MMDDYY_HHMMSS.MS]
	static std::string GetDateAndTimeMSecString(); 

protected:
	vtkAccurateTimer();
	virtual ~vtkAccurateTimer(); 

private:
	vtkAccurateTimer(const vtkAccurateTimer&);  // Not implemented.
	void operator=(const vtkAccurateTimer&);  // Not implemented.
  static double SystemStartTime; 

	// The singleton instance and the singleton cleanup instance
	static vtkAccurateTimer* Instance;
	static vtkAccurateTimerCleanup Cleanup;
}; 

#endif