
#ifndef __vtkPlusCommandFactory_h
#define __vtkPlusCommandFactory_h

#include <string>

#include "vtkObject.h"

class vtkPlusCommand;
class vtkPlusCommandCollection;
class vtkPlusCommandCleanup;

class
VTK_EXPORT
vtkPlusCommandFactory : public vtkObject
{
public:
  static vtkPlusCommandFactory *New();
  vtkTypeMacro( vtkPlusCommandFactory, vtkObject );
  void PrintSelf(ostream& os, vtkIndent indent);   
  
  static void RegisterCommand( vtkPlusCommand* r );
  
  static vtkPlusCommand* CreatePlusCommand( std::string str ); 
  
  static void GetRegisteredCommands( vtkPlusCommandCollection* );
  
  
protected:
  vtkPlusCommandFactory();
  ~vtkPlusCommandFactory();

  static void InitializeCommands();

private:
  static vtkPlusCommandCollection* AvailableCommands;
  vtkPlusCommandFactory(const vtkPlusCommandFactory&);  // Not implemented.
  void operator=(const vtkPlusCommandFactory&);  // Not implemented.
//BTX
  friend class vtkPlusCommandFactoryCleanup;
//ETX
};

#endif
