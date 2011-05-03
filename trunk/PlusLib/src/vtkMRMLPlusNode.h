
#ifndef __vtkMRMLUSTemplateCalibrationNode_h
#define __vtkMRMLUSTemplateCalibrationNode_h

#include "vtkUSTemplateCalibrationWin32Header.h"

#include "vtkMRML.h"
#include "vtkMRMLNode.h"
#include "vtkMRMLStorageNode.h"

#include "vtkMatrix4x4.h"
#include "vtkTransform.h"
#include "vtkImageData.h"

//#include "vtkUSTemplateCalibration.h"

class vtkImageData;

class VTK_EXPORT vtkMRMLUSTemplateCalibrationNode : public vtkMRMLNode
{
  public:
  static vtkMRMLUSTemplateCalibrationNode *New();
  vtkTypeMacro(vtkMRMLUSTemplateCalibrationNode,vtkMRMLNode);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Create instance of a GAD node.
  virtual vtkMRMLNode* CreateNodeInstance();

  // Description:
  // Set node attributes from name/value pairs
  virtual void ReadXMLAttributes( const char** atts);

  // Description:
  // Write this node's information to a MRML file in XML format.
  virtual void WriteXML(ostream& of, int indent);

  // Description:
  // Copy the node's attributes to this object
  virtual void Copy(vtkMRMLNode *node);

  // Description:
  // Get unique node XML tag name (like Volume, Model)
  virtual const char* GetNodeTagName() {return "USTCalibParameters";};

  // Description:
  // Get/Set input volume MRML Id
  vtkGetStringMacro(InputVolumeRef);
  vtkSetStringMacro(InputVolumeRef);
  
  // Description:
  // Get/Set output volume MRML Id
  vtkGetStringMacro(OutputVolumeRef);
  vtkSetStringMacro(OutputVolumeRef);

  // Description:
  // Update the stored reference to another node in the scene
  virtual void UpdateReferenceID(const char *oldID, const char *newID);

 
protected:
  vtkMRMLUSTemplateCalibrationNode();
  ~vtkMRMLUSTemplateCalibrationNode();
  vtkMRMLUSTemplateCalibrationNode(const vtkMRMLUSTemplateCalibrationNode&);
  void operator=(const vtkMRMLUSTemplateCalibrationNode&);

  char* InputVolumeRef;
  char* OutputVolumeRef;

};

#endif

