#include "PlusConfigure.h"
#include "vtkGnuplotExecuter.h"
#include "vtkObjectFactory.h"
#include "vtksys/SystemTools.hxx" 
#include "vtksys/Process.h"
#include "vtkDirectory.h"
#include "vtkDelimitedTextWriter.h"


//----------------------------------------------------------------------------
vtkCxxRevisionMacro(vtkGnuplotExecuter, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkGnuplotExecuter);

//----------------------------------------------------------------------------
vtkGnuplotExecuter::vtkGnuplotExecuter()
{
  this->WorkingDirectory = NULL; 
  this->GnuplotCommand = NULL; 

  this->HideWindowOff(); 
  this->SetTimeout(5); 
  this->SetWorkingDirectory(vtksys::SystemTools::GetCurrentWorkingDirectory().c_str() ); 

}

//----------------------------------------------------------------------------
vtkGnuplotExecuter::~vtkGnuplotExecuter()
{

}


//----------------------------------------------------------------------------
void vtkGnuplotExecuter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
PlusStatus vtkGnuplotExecuter::DumpTableToFileInGnuplotFormat( vtkTable* table, const char* filename)
{
  LOG_TRACE("vtkGnuplotExecuter::DumpTableToFileInGnuplotFormat"); 

  if ( table == NULL ) 
  {
    LOG_ERROR("Failed to dump table to file in gnuplot format - Input table is NULL!"); 
    return PLUS_FAIL; 
  }

  if ( filename == NULL )
  {
    LOG_ERROR("Failed to dump table to file in gnuplot format - Input filename is NULL!"); 
    return PLUS_FAIL; 
  }

  vtkSmartPointer<vtkDelimitedTextWriter> textWriter = vtkSmartPointer<vtkDelimitedTextWriter>::New(); 
  textWriter->SetFieldDelimiter("\t"); 
  textWriter->SetUseStringDelimiter(false); 
  textWriter->SetFileName(filename); 
  textWriter->SetInput( table ); 
  textWriter->Update(); 

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
void vtkGnuplotExecuter::AddArgument(const char* argument)
{
  this->GnuplotArguments.push_back(argument); 
}

//----------------------------------------------------------------------------
void vtkGnuplotExecuter::ClearArguments()
{
  this->GnuplotArguments.clear(); 
}

//----------------------------------------------------------------------------
PlusStatus vtkGnuplotExecuter::Execute()
{
  if ( this->GetGnuplotCommand() == NULL )
  {
    LOG_ERROR("Gnuplot command has to be set before execution!"); 
    return PLUS_FAIL; 
  }

  if ( !vtksys::SystemTools::FileExists( this->GetGnuplotCommand(), true) )
  {
    LOG_ERROR("Unable to find Gnuplot command at: " << this->GetGnuplotCommand()); 
    return PLUS_FAIL; 
  }

  try 
  {
    std::vector<const char*> command;
    command.clear();

    // Add gnuplot command 
    LOG_DEBUG("Add gnuplot command: " << this->GnuplotCommand); 
    command.push_back(this->GetGnuplotCommand());

    // Add custom arguments
    for ( unsigned int arg = 0; arg < this->GnuplotArguments.size(); arg++)
    {
      LOG_DEBUG("Add argument: " << this->GnuplotArguments[arg]); 
      command.push_back(this->GnuplotArguments[arg].c_str());
    }

    // The array must end with a NULL pointer.
    command.push_back(0); 

    // Create new process 
    vtksysProcess* gp = vtksysProcess_New();

    // Set command
    vtksysProcess_SetCommand(gp, &*command.begin());

    // Set hide window
    if ( this->GetHideWindow() )
    {
      LOG_DEBUG("Hide gnuplot window ..." ); 
      vtksysProcess_SetOption(gp,vtksysProcess_Option_HideWindow, 1);
    }
    else
    {
      vtksysProcess_SetOption(gp,vtksysProcess_Option_HideWindow, 0);
    }

    LOG_DEBUG("Set working directory to: " << this->GetWorkingDirectory()); 
    vtkSmartPointer<vtkDirectory> dir = vtkSmartPointer<vtkDirectory>::New(); 
    if ( dir->Open(this->GetWorkingDirectory()) == 0 ) 
    {	
      LOG_DEBUG("Make directoty: " << this->GetWorkingDirectory()); 
      dir->MakeDirectory(this->GetWorkingDirectory()); 
    }

    vtksysProcess_SetWorkingDirectory(gp, this->GetWorkingDirectory()); 

    // Run the application
    LOG_DEBUG("Execute gnuplot ..." ); 
    vtksysProcess_Execute(gp); 

    LOG_DEBUG("Wait for exit (Timeout: " << this->GetTimeout() << "s) ..." ); 
    std::string buffer = "";
    double timeout = this->GetTimeout(); 
    char* data = NULL;
    int length;

    while(vtksysProcess_WaitForData(gp,&data,&length,&timeout)) // wait for 1s
    {
      for(int i=0;i<length;i++)
      {
        buffer += data[i];
      }
    }

    vtksysProcess_WaitForExit(gp, 0); 
    LOG_DEBUG("Execution time was: " << this->GetTimeout() - timeout << "s ..." ); 

    PlusStatus status = PLUS_SUCCESS; 
    int result(0); 
    switch ( vtksysProcess_GetState(gp) )
    {
    case vtksysProcess_State_Exited: 
      {
        result = vtksysProcess_GetExitValue(gp); 
      }
      break; 
    case vtksysProcess_State_Error: 
      {
        LOG_ERROR("Error during execution: " << vtksysProcess_GetErrorString(gp)); 
        LOG_ERROR("Program output: " << buffer); 
        status = PLUS_FAIL; 
      }
      break;
    case vtksysProcess_State_Exception: 
      {
        LOG_ERROR("Exception during execution: " << vtksysProcess_GetExceptionString(gp)); 
        LOG_ERROR("Program output: " << buffer); 
        status = PLUS_FAIL; 
      }
      break;
    case vtksysProcess_State_Starting: 
    case vtksysProcess_State_Executing:
    case vtksysProcess_State_Expired: 
    case vtksysProcess_State_Killed: 
      {
        LOG_ERROR("Unexpected ending state after running " << this->GetGnuplotCommand() ); 
        LOG_ERROR("Program output: " << buffer); 
        status = PLUS_FAIL; 
      }
      break;
    }

    LOG_DEBUG("Deleting process ..."); 
    vtksysProcess_Delete(gp); 

    return status; 
  }
  catch (...)
  {
    LOG_ERROR("Failed to execute Gnuplot commnad!"); 
    return PLUS_FAIL; 
  }
}