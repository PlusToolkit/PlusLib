/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __vtkDisplayableTool_h
#define __vtkDisplayableTool_h

#include "PlusConfigure.h"

#include "vtkObject.h"
#include "vtkTransform.h"

class vtkTransform;
class vtkActor;

//-----------------------------------------------------------------------------

/*! \class vtkDisplayableTool 
 * \brief Class that encapsulates the objects needed for visualizing a tool - the tool object, the actor, a flag indicating whether it is displayable
 * \ingroup PlusAppFCal
 */
class vtkDisplayableTool : public vtkObject
{
public:
	static vtkDisplayableTool *New();

  /*! Returns displayable status (true if displayable flag is on and valid actor is present) */
  bool IsDisplayable();

  /*!
  * Read freehand calibration configuration for fCal
  * \param aConfig Root element of the input device set configuration XML data
  */
  PlusStatus ReadConfiguration(vtkXMLDataElement* aConfig);

public:
  /*! Get tool to world transform name */
  PlusTransformName GetToolToWorldTransformName() { return this->ToolToWorldTransformName;  };

  /*! Get STL modle file name */
	vtkGetStringMacro(STLModelFileName);

  /*! Get model to tool transform */
	vtkGetObjectMacro(ModelToToolTransform, vtkTransform);

  /*! Get actor */
  vtkGetObjectMacro(Actor, vtkActor);

	vtkBooleanMacro(Displayable, bool);
  /*! Set displayable flag */
	vtkSetMacro(Displayable, bool);
  /*! Get displayable flag */
	vtkGetMacro(Displayable, bool);

protected:
  /*! Set tool to world transform name */
  void SetToolToWorldTransformName(PlusTransformName aName) { this->ToolToWorldTransformName = aName;  };

  /*! Set actor */
  void SetActor(vtkActor*);

  /*! Set model to tool transform */
	vtkSetObjectMacro(ModelToToolTransform, vtkTransform);

  /*! Set STL modle file name */
	vtkSetStringMacro(STLModelFileName);

protected:
  /*! Constructor */
  vtkDisplayableTool();

  /*! Destructor */
  virtual ~vtkDisplayableTool();

protected:
  /*! Name of the STL file containing the 3D model of this tool */
  char*               STLModelFileName;

  /* Tool to world transform name */
  PlusTransformName   ToolToWorldTransformName;

  /* Model to tool transform */
  vtkTransform*       ModelToToolTransform;

  /*! Actor displaying the tool model */
  vtkActor*           Actor;

  /*! Flag that can disable displaying of this tool */
  bool                Displayable;
};

#endif
