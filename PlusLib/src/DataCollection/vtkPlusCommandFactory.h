
#ifndef __vtkPlusCommandFactory_h
#define __vtkPlusCommandFactory_h

#include <string>

#include "vtkObject.h"
#include "vtkObjectFactory.h"


class vtkPlusCommand;
class vtkPlusCommandCollection;
class vtkPlusCommandCleanup;

class
VTK_EXPORT
vtkPlusCommandFactory : public vtkObjectFactory
{
public:
  static vtkPlusCommandFactory *New();
  vtkTypeMacro( vtkPlusCommandFactory, vtkObject );
  void PrintSelf(ostream& os, vtkIndent indent);   
  
  static void RegisterPlusCommand( vtkPlusCommand* r );
  
  static vtkPlusCommand* CreatePlusCommand( std::string str ); 
  
  static void GetRegisteredCommands( vtkPlusCommandCollection* );
  
  virtual const char* GetVTKSourceVersion();
  virtual const char* GetDescription();
  
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
