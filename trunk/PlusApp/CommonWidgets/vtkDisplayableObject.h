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

class vtkProp3D;
class vtkMapper;
class vtkPolyData; 

//-----------------------------------------------------------------------------

/*! \class vtkDisplayableObject 
 * \brief Class that encapsulates the objects needed for visualizing a tool - the tool object, the actor, a flag indicating whether it is displayable
 * \ingroup PlusAppCommonWidgets
 */
class vtkDisplayableObject : public vtkObject
{
public:
	vtkTypeRevisionMacro(vtkDisplayableObject,vtkObject);
  static vtkDisplayableObject *New() { return NULL; };

  /*!
  * New function that gets the type and instantiates the proper displayable object class
  * \param aType Type that is read from the DisplayabelObject element of the configuration (eg. "Model")
  */
  static vtkDisplayableObject *New(const char* aType);

  /*! Returns displayable status */
  virtual bool IsDisplayable() = 0;

  /*!
  * Read displayable object configuration
  * \param aConfig DisplayableObject element from the input device set configuration (not the root as usually!)
  */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* aConfig);

public:
  /*! Set object coordinate frame name */
  vtkSetStringMacro(ObjectCoordinateFrame);
  /*! Get object coordinate frame name */
  vtkGetStringMacro(ObjectCoordinateFrame);

  /*! Set actor */
  void SetActor(vtkProp3D*);
  /*! Get actor */
  vtkGetObjectMacro(Actor, vtkProp3D);

  /*! Set displayable flag */
	vtkSetMacro(Displayable, bool);
	vtkBooleanMacro(Displayable, bool);

  /*! Set previously set opacity */
	vtkSetMacro(LastOpacity, double);
  /*! Get previously set opacity */
	vtkGetMacro(LastOpacity, double);

  /*! Set opacity */
  virtual void SetOpacity(double aOpacity) = 0;
  /*! Get opacity */
  virtual double GetOpacity() = 0;

protected:
  /*! Constructor */
  vtkDisplayableObject();

  /*! Destructor */
  virtual ~vtkDisplayableObject();

protected:
  /* Object coordinate frame name */
  char*               ObjectCoordinateFrame;

  /*! Actor displaying the tool model */
  vtkProp3D*          Actor;

  /*! Flag that can disable displaying of this tool */
  bool                Displayable;

  /*! Previously set opacity */
  double              LastOpacity;
};

//-----------------------------------------------------------------------------

/*! \class vtkDisplayableImage 
 * \brief Specialized vtkDisplayableObject that displays an image
 * \ingroup PlusAppCommonWidgets
 */
class vtkDisplayableImage : public vtkDisplayableObject
{
public:
	vtkTypeRevisionMacro(vtkDisplayableImage,vtkDisplayableObject);

	static vtkDisplayableImage *New();

  /*! Returns displayable status (true if displayable flag is on) */
  bool IsDisplayable();

public:
  /*! Set opacity */
  void SetOpacity(double aOpacity);
  /*! Get opacity */
  double GetOpacity();

protected:
  /*! Constructor */
  vtkDisplayableImage();

  /*! Destructor */
  virtual ~vtkDisplayableImage();
};

//-----------------------------------------------------------------------------

/*! \class vtkDisplayableAxes
 * \brief Specialized vtkDisplayableObject that displays axes of a coordinate system
 * \ingroup PlusAppCommonWidgets
 */
class vtkDisplayableAxes : public vtkDisplayableObject
{
public:
	vtkTypeRevisionMacro(vtkDisplayableAxes,vtkDisplayableObject);

	static vtkDisplayableAxes *New();

  /*! Returns displayable status (true if displayable flag is on) */
  bool IsDisplayable();

public:
  /*! Set opacity */
  void SetOpacity(double aOpacity);
  /*! Get opacity */
  double GetOpacity();

protected:
  /*! Constructor */
  vtkDisplayableAxes();

  /*! Destructor */
  virtual ~vtkDisplayableAxes();
};

//-----------------------------------------------------------------------------

/*! \class vtkDisplayablePolyData 
 * \brief Specialized vtkDisplayableObject that displays a poly data
 * \ingroup PlusAppCommonWidgets
 */
class vtkDisplayablePolyData : public vtkDisplayableObject
{
public:
	vtkTypeRevisionMacro(vtkDisplayablePolyData,vtkDisplayableObject);

  static vtkDisplayablePolyData *New();

  /*! Returns displayable status (true if displayable flag is on and valid actor is present) */
  bool IsDisplayable();

  /*! Set color */
  void SetColor(double aR, double aG, double aB);

  /* Get poly data */
  vtkGetObjectMacro(PolyData, vtkPolyData); 

  /* Set poly data */
  virtual void SetPolyData(vtkPolyData* polyData); 

public:
  /*! Set opacity */
  void SetOpacity(double aOpacity);
  /*! Get opacity */
  double GetOpacity();

protected:
  /*! Constructor */
  vtkDisplayablePolyData();

  /*! Destructor */
  virtual ~vtkDisplayablePolyData();

protected:
  /*! Displayed poly data */
  vtkPolyData* PolyData; 
};

//-----------------------------------------------------------------------------

/*! \class vtkDisplayableModel 
 * \brief Specialized vtkDisplayableObject that displays a model
 * \ingroup PlusAppCommonWidgets
 */
class vtkDisplayableModel : public vtkDisplayablePolyData
{
public:
	vtkTypeRevisionMacro(vtkDisplayableModel,vtkDisplayablePolyData);

  static vtkDisplayableModel *New();

  /*!
  * Read displayable object configuration
  * \param aConfig DisplayableObject element from the input device set configuration (not the root as usually!)
  */
  PlusStatus ReadConfiguration(vtkXMLDataElement* aConfig);

  /*! Appends a polydata to the already existing one */
  PlusStatus AppendPolyData(vtkPolyData* aPolyData);

public:
  /*! Set STL modle file name */
	vtkSetStringMacro(STLModelFileName);
  /*! Get STL modle file name */
	vtkGetStringMacro(STLModelFileName);

  /*! Get model to tool transform */
	vtkGetObjectMacro(ModelToObjectTransform, vtkTransform);

protected:
  /*! Set model to tool transform */
	vtkSetObjectMacro(ModelToObjectTransform, vtkTransform);

	/*! Assemble and set default stylus model for stylus tool actor	*/
	PlusStatus SetDefaultStylusModel();

protected:
  /*! Constructor */
  vtkDisplayableModel();

  /*! Destructor */
  virtual ~vtkDisplayableModel();

protected:
  /*! Name of the STL file containing the 3D model of this tool */
  char*               STLModelFileName;

  /* Model to tool transform */
  vtkTransform*       ModelToObjectTransform;

};


#endif
