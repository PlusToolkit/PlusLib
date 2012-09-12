/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __VTKPLUSCOMMAND_H
#define __VTKPLUSCOMMAND_H

#include <string>
#include <vector>

#include "vtkObjectFactory.h"

#include "igtlClientSocket.h"
#include "igtlMessageHeader.h"
#include "igtlOSUtil.h"

#include "vtkDataCollector.h"

/*!
  \class vtkPlusCommand 
  \brief This is an abstract superclass for commands in the OpenIGTLink network interface for Plus.

  All commands have a unique string representation to enable sending commands as string messages.
  For e.g. through OpenIGTLink.
  
  \ingroup PlusLibDataCollection
*/
class VTK_EXPORT vtkPlusCommand : public vtkObjectFactory
{
public:
  
  vtkTypeRevisionMacro( vtkPlusCommand, vtkObject );
  virtual void PrintSelf( ostream& os, vtkIndent indent );
  
  /*!
    Determines if a string can be converted to a specific command.
    \param str String representation of the command to be examined.
  */
  virtual bool CanExecute( std::string str ) = 0;
  
  /*! Performs the actions defined in a specific command. */
  virtual bool Execute() = 0;
  
  /*! Returns the string representation of a specific command. */
  virtual std::string GetStringRepresentation() = 0;
  
  typedef std::vector< std::string > CommandStringsType;
  const CommandStringsType GetSupportedCommandStrings() const;
  
  vtkSetObjectMacro( DataCollector, vtkDataCollector );  // void SetDataCollector( vtkDataCollector* d ) {...}
  
  const char* GetVTKSourceVersion();
  const char* GetDescription();
  
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

