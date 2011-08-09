
#ifndef __vtkOpenIGTLinkPlusController_h
#define __vtkOpenIGTLinkPlusController_h


#include "vtkObject.h"

#include "igtlOSUtil.h"
#include "igtlPositionMessage.h"
#include "igtlClientSocket.h"

#include "vtkDataCollector.h"



/**
 * Parses commands in string messages, and controls Plus functions accordingly.
 */
class
VTK_EXPORT 
vtkOpenIGTLinkPlusController
: public vtkObject
{
public:
  
  enum ReturnValue {
    RETURN_SUCCESS,
    RETURN_FAILURE
  };
  
  static vtkOpenIGTLinkPlusController *New();
	vtkTypeRevisionMacro( vtkOpenIGTLinkPlusController, vtkObject );
	virtual void PrintSelf( ostream& os, vtkIndent indent );
  
  
  void SetDataCollector( vtkDataCollector* dataCollector );
  ReturnValue Initialize( std::string &strError );
  
  
protected:
  
  vtkOpenIGTLinkPlusController();
	virtual ~vtkOpenIGTLinkPlusController();
  
  
private:
	
  vtkOpenIGTLinkPlusController( const vtkOpenIGTLinkPlusController& );
	void operator=( const vtkOpenIGTLinkPlusController& );
  
  
  vtkDataCollector*  DataCollector;
};



#endif
