
#ifndef __VTKPLUSCOMMAND_H
#define __VTKPLUSCOMMAND_H


#include <string>
#include <vector>

#include "vtkObject.h"

#include "igtlClientSocket.h"
#include "igtlMessageHeader.h"
#include "igtlOSUtil.h"

#include "vtkDataCollector.h"



/**
 * This is an abstract superclass for commands in the OpenIGTLink
 * network interface for Plus.
 * A Pluggable Factory pattern is used to allow different kinds of
 * commands to be registered without having to modify the code in this class.
 */
class
VTK_EXPORT 
vtkPlusCommand
: public vtkObject
{
public:
  
  vtkTypeRevisionMacro( vtkPlusCommand, vtkObject );
  virtual void PrintSelf( ostream& os, vtkIndent indent );
  
  virtual bool CanExecute( std::string str ) = 0;
  virtual bool Execute( std::string stringMessage ) = 0;
  virtual std::string GetStringRepresentation() = 0;
  virtual void ProcessResponse( igtl::MessageBase::Pointer header,
                                igtl::ClientSocket::Pointer clientSocket ) = 0;
  
  typedef std::vector< std::string > CommandStringsType;
  const CommandStringsType GetSupportedCommandStrings() const;
  
  vtkSetObjectMacro( DataCollector, vtkDataCollector );  // void SetDataCollector( vtkDataCollector* d ) {...}
  
  
protected:
  
  vtkPlusCommand();
  virtual ~vtkPlusCommand();
  
  void AddSupportedCommandString( std::string commandString );
  
  vtkDataCollector* DataCollector;
  
  std::string StringRepresentation;
  
  
private:
	
  vtkPlusCommand( const vtkPlusCommand& );
  void operator=( const vtkPlusCommand& );
  
  CommandStringsType CommandStrings;
  
};


#endif

