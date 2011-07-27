#ifndef __vtkBufferedTracker_h
#define __vtkBufferedTracker_h

#include "vtkTracker.h"


class vtkFrameToTimeConverter;
class vtkTransform;

class VTK_EXPORT vtkBufferedTracker : public vtkTracker
{
public:

  static vtkBufferedTracker *New();
  vtkTypeMacro(vtkBufferedTracker,vtkTracker);
  void PrintSelf(ostream& os, vtkIndent indent);
 
  // Description:
  // Probe to see if the tracking system is present on the
  // specified serial port.  If the SerialPort is set to -1,
  // then all serial ports will be checked.
  PlusStatus Probe();

  // Description:
  // Get an update from the tracking system and push the new transforms
  // to the tools.  This should only be used within vtkTracker.cxx.
  PlusStatus InternalUpdate();

  virtual PlusStatus AddTransform( vtkMatrix4x4* transformMatrix, double timestamp ); 


protected:
  vtkBufferedTracker();
  ~vtkBufferedTracker();

  // Description:
  // Start the tracking system.  The tracking system is brought from
  // its ground state into full tracking mode.  The device will
  // only be reset if communication cannot be established without
  // a reset.
  PlusStatus InternalStartTracking();

  // Description:
  // Stop the tracking system and bring it back to its ground state:
  // Initialized, not tracking, at 9600 Baud.
  PlusStatus InternalStopTracking();

  // Description:
  // Class for updating the virtual clock that accurately times the
  // arrival of each transform, more accurately than is possible with
  // the system clock alone because the virtual clock averages out the
  // jitter.
  vtkFrameToTimeConverter *Timer;

  int ToolNumber; 

private:
  vtkBufferedTracker(const vtkBufferedTracker&);
  void operator=(const vtkBufferedTracker&);  
};

#endif
