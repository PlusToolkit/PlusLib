/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#include "PlusConfigure.h"

#include "vtkPlusCommandProcessor.h"
#include "vtkPlusReconstructVolumeCommand.h"

vtkStandardNewMacro( vtkPlusReconstructVolumeCommand );

//----------------------------------------------------------------------------
vtkPlusReconstructVolumeCommand::vtkPlusReconstructVolumeCommand()
: InputSeqFilename(NULL)
, OutputVolFilename(NULL)
{
}

//----------------------------------------------------------------------------
vtkPlusReconstructVolumeCommand::~vtkPlusReconstructVolumeCommand()
{
  SetInputSeqFilename(NULL);
  SetOutputVolFilename(NULL);
}

//----------------------------------------------------------------------------
void vtkPlusReconstructVolumeCommand::PrintSelf( ostream& os, vtkIndent indent )
{
  this->Superclass::PrintSelf( os, indent );
}

//----------------------------------------------------------------------------
void vtkPlusReconstructVolumeCommand::GetCommandNames(std::list<std::string> &cmdNames)
{ 
  cmdNames.clear(); 
  cmdNames.push_back("ReconstructVolume");
}

//----------------------------------------------------------------------------
std::string vtkPlusReconstructVolumeCommand::GetDescription(const char* commandName)
{ 
  return "ReconstructVolume: Reconstructs a volume from a file and writes the result to a file. Attributes: \
         InputSeqFilename: name of the input sequence metafile name that contains the list of frames\
         OutputVolFilename: name of the output volume file name\
         ";
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusReconstructVolumeCommand::ReadConfiguration(vtkXMLDataElement* aConfig)
{  
  if (vtkPlusCommand::ReadConfiguration(aConfig)!=PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }
  SetInputSeqFilename(aConfig->GetAttribute("InputSeqFilename"));
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusReconstructVolumeCommand::WriteConfiguration(vtkXMLDataElement* aConfig)
{  
  if (vtkPlusCommand::ReadConfiguration(aConfig)!=PLUS_SUCCESS)
  {
    return PLUS_FAIL;
  }
  aConfig->SetAttribute("InputSeqFilename",this->InputSeqFilename);
  aConfig->SetAttribute("OutputVolFilename",this->OutputVolFilename);
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkPlusReconstructVolumeCommand::Execute()
{
  // TODO: implement this
  PlusStatus status=PLUS_SUCCESS;
  if ( status == PLUS_FAIL )
  {
    LOG_INFO("vtkPlusReconstructVolumeCommand::Execute: failed");
    SetCommandCompleted(PLUS_FAIL,"Volume reconstruction failed");
  }
  else
  {
    LOG_INFO("vtkPlusReconstructVolumeCommand::Execute: completed");
    SetCommandCompleted(PLUS_SUCCESS,"Volume reconstruction completed");
  }
  return PLUS_SUCCESS;
}
