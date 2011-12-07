/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __vtkDisplayableObject_h
#define __vtkDisplayableObject_h

#include "PlusConfigure.h"

#include "vtkObject.h"
#include "vtkTransform.h"

class vtkTransform;
class vtkActor;

//-----------------------------------------------------------------------------

/*! \class vtkDisplayableObject 
 * \brief Class that encapsulates the objects needed for visualizing a tool - the tool object, the actor, a flag indicating whether it is displayable
 * \ingroup PlusAppFCal
 */
class vtkDisplayableObject : public vtkObject
{
public:
	static vtkDisplayableObject *New();

  /*! Returns displayable status (true if displayable flag is on and valid actor is present) */
  bool IsDisplayable();

  /*!
  * Read freehand calibration configuration for fCal
  * \param aConfig Root element of the input device set configuration XML data
  */
  PlusStatus ReadConfiguration(vtkXMLDataElement* aConfig);

public:
  /*! Set object coordinate frame name */
  vtkSetStringMacro(ObjectCoordinateFrame);
  /*! Get object coordinate frame name */
  vtkGetStringMacro(ObjectCoordinateFrame);

  /*! Set STL modle file name */
	vtkSetStringMacro(STLModelFileName);
  /*! Get STL modle file name */
	vtkGetStringMacro(STLModelFileName);

  /*! Get model to tool transform */
	vtkGetObjectMacro(ModelToObjectTransform, vtkTransform);

  /*! Set actor */
  void SetActor(vtkActor*);
  /*! Get actor */
  vtkGetObjectMacro(Actor, vtkActor);

	vtkBooleanMacro(Displayable, bool);
  /*! Set displayable flag */
	vtkSetMacro(Displayable, bool);
  /*! Get displayable flag */
	vtkGetMacro(Displayable, bool);

  /*! Set previously set opacity */
	vtkSetMacro(LastOpacity, double);
  /*! Get previously set opacity */
	vtkGetMacro(LastOpacity, double);

protected:
  /*! Set model to tool transform */
	vtkSetObjectMacro(ModelToObjectTransform, vtkTransform);

protected:
  /*! Constructor */
  vtkDisplayableObject();

  /*! Destructor */
  virtual ~vtkDisplayableObject();

protected:
  /*! Name of the STL file containing the 3D model of this tool */
  char*               STLModelFileName;

  /* Object coordinate frame name */
  char*               ObjectCoordinateFrame;

  /* Model to tool transform */
  vtkTransform*       ModelToObjectTransform;

  /*! Actor displaying the tool model */
  vtkActor*           Actor;

  /*! Flag that can disable displaying of this tool */
  bool                Displayable; //TODO is it needed?

  /*! Previously set opacity */
  double              LastOpacity;
};

#endif
