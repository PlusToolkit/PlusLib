/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

/*=========================================================================
The following copyright notice is applicable to parts of this file:
Copyright (c) 2014, Robarts Research Institute, The University of Western Ontario, London, Ontario, Canada
All rights reserved.
Authors include:
* Elvis Chen (Robarts Research Institute and The University of Western Ontario)
* Adam Rankin (Robarts Research Institute and The University of Western Ontario)
=========================================================================*/  

#include "PlusConfigure.h"
#include "vtkIEEListener.h"
#include "StreamMgr.h"

//----------------------------------------------------------------------------

vtkStandardNewMacro(vtkIEEListener);

//----------------------------------------------------------------------------
vtkIEEListener::vtkIEEListener()
: vtkObject()
, StreamManager(new CStreamMgr())
, MachineName("")
, Port(4013)
, Connected(false)
{
}

//----------------------------------------------------------------------------
vtkIEEListener::~vtkIEEListener()
{
  //delete StreamManager;
}

//----------------------------------------------------------------------------
PlusStatus vtkIEEListener::Connect(CLIENT_POSTSCANCONVERT_CALLBACK callback)
{
  if( this->MachineName.empty() || this->Port <= 0 )
  {
    LOG_ERROR("MachineName not set or invalid port: " << this->MachineName << ":" << this->Port );
    return PLUS_FAIL;
  }
  if( !StreamManager->Initialize() )
  {
    LOG_ERROR("Failed to initialize:");
    return PLUS_FAIL;
  }

  StreamManager->RegisterCallback(callback);

  HRESULT resultCode = StreamManager->Start((char*)this->MachineName.c_str(), this->Port);
  if( resultCode < 0)
  {
    LOG_ERROR("Unable to connect to Philips system at " << this->MachineName << ":" << this->Port << ". Error code: " << std::hex << resultCode);
    return PLUS_FAIL;
  }
  else
  {
    this->Connected = true;
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkIEEListener::Disconnect()
{
  if (StreamManager != NULL && StreamManager->IsConnected())
  {
    LOG_INFO("Stopping stream:");
    StreamManager->Stop();

    LOG_INFO("Shutdown stream:");
    StreamManager->Shutdown();

    LOG_INFO("Delete streamMgr:");

    this->Connected = false;
    return PLUS_SUCCESS;
  }
  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
void vtkIEEListener::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  
  os << indent << "MachineName: " << this->MachineName << "\n";
  os << indent << "Port: " << this->Port << "\n";
  os << indent << "Connected: " << this->Connected << "\n";
}

//----------------------------------------------------------------------------
void vtkIEEListener::SetMachineName(const std::string& machineName)
{
  this->MachineName = machineName;
}

//----------------------------------------------------------------------------
void vtkIEEListener::SetPortNumber(unsigned int port)
{
  this->Port = port;
}

//----------------------------------------------------------------------------
bool vtkIEEListener::IsConnected()
{
  return this->Connected;
}
