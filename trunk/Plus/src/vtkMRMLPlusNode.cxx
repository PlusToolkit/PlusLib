
#include <string>
#include <iostream>
#include <sstream>

#include "vtkObjectFactory.h"

#include "vtkMRMLUSTemplateCalibrationNode.h"
#include "vtkMRMLScene.h"


//------------------------------------------------------------------------------
vtkMRMLUSTemplateCalibrationNode* vtkMRMLUSTemplateCalibrationNode::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMRMLUSTemplateCalibrationNode");
  if(ret)
    {
      return (vtkMRMLUSTemplateCalibrationNode*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkMRMLUSTemplateCalibrationNode;
}

//----------------------------------------------------------------------------

vtkMRMLNode* vtkMRMLUSTemplateCalibrationNode::CreateNodeInstance()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMRMLUSTemplateCalibrationNode");
  if(ret)
    {
      return (vtkMRMLUSTemplateCalibrationNode*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkMRMLUSTemplateCalibrationNode;
}

//----------------------------------------------------------------------------
vtkMRMLUSTemplateCalibrationNode::vtkMRMLUSTemplateCalibrationNode()
{
   //this->Conductance = 1.0;
   //this->NumberOfIterations = 1;
   //this->TimeStep = 0.1;
   this->InputVolumeRef = NULL;
   this->OutputVolumeRef = NULL;
}

//----------------------------------------------------------------------------
vtkMRMLUSTemplateCalibrationNode::~vtkMRMLUSTemplateCalibrationNode()
{
   this->SetInputVolumeRef( NULL );
   this->SetOutputVolumeRef( NULL );
}

//----------------------------------------------------------------------------
void vtkMRMLUSTemplateCalibrationNode::WriteXML(ostream& of, int nIndent)
{
  Superclass::WriteXML(of, nIndent);

  // Write all MRML node attributes into output stream

  vtkIndent indent(nIndent);
  {
    std::stringstream ss;
    if ( this->InputVolumeRef )
      {
      ss << this->InputVolumeRef;
      of << indent << " InputVolumeRef=\"" << ss.str() << "\"";
     }
  }
  {
    std::stringstream ss;
    if ( this->OutputVolumeRef )
      {
      ss << this->OutputVolumeRef;
      of << indent << " OutputVolumeRef=\"" << ss.str() << "\"";
      }
  }
}

//----------------------------------------------------------------------------
void vtkMRMLUSTemplateCalibrationNode::ReadXMLAttributes(const char** atts)
{
  vtkMRMLNode::ReadXMLAttributes(atts);

  // Read all MRML node attributes from two arrays of names and values
  const char* attName;
  const char* attValue;
  while (*atts != NULL) 
    {
    attName = *(atts++);
    attValue = *(atts++);
    if (!strcmp(attName, "InputVolumeRef"))
      {
      this->SetInputVolumeRef(attValue);
      this->Scene->AddReferencedNodeID(this->InputVolumeRef, this);
      }
    else if (!strcmp(attName, "OutputVolumeRef"))
      {
      this->SetOutputVolumeRef(attValue);
      this->Scene->AddReferencedNodeID(this->OutputVolumeRef, this);
      }
    }
}

//----------------------------------------------------------------------------
// Copy the node's attributes to this object.
// Does NOT copy: ID, FilePrefix, Name, VolumeID
void vtkMRMLUSTemplateCalibrationNode::Copy(vtkMRMLNode *anode)
{
  Superclass::Copy(anode);
  vtkMRMLUSTemplateCalibrationNode *node = (vtkMRMLUSTemplateCalibrationNode *) anode;

  //this->SetConductance(node->Conductance);
  //this->SetNumberOfIterations(node->NumberOfIterations);
  //this->SetTimeStep(node->TimeStep);
  this->SetInputVolumeRef(node->InputVolumeRef);
  this->SetOutputVolumeRef(node->OutputVolumeRef);
}

//----------------------------------------------------------------------------
void vtkMRMLUSTemplateCalibrationNode::PrintSelf(ostream& os, vtkIndent indent)
{
  
  vtkMRMLNode::PrintSelf(os,indent);

  os << indent << "InputVolumeRef:   " << 
   (this->InputVolumeRef ? this->InputVolumeRef : "(none)") << "\n";
  os << indent << "OutputVolumeRef:   " << 
   (this->OutputVolumeRef ? this->OutputVolumeRef : "(none)") << "\n";
}

//----------------------------------------------------------------------------
void vtkMRMLUSTemplateCalibrationNode::UpdateReferenceID(const char *oldID, const char *newID)
{
  if (!strcmp(oldID, this->InputVolumeRef))
    {
    this->SetInputVolumeRef(newID);
    }
  if (!strcmp(oldID, this->OutputVolumeRef))
    {
    this->SetOutputVolumeRef(newID);
    }
}
