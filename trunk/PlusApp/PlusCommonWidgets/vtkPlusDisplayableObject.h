/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkDisplayableObject_h
#define __vtkDisplayableObject_h

// PlusLib includes
#include <PlusConfigure.h>

// VTK includes
#include <vtkObject.h>
#include <vtkTransform.h>

class vtkProp3D;
class vtkMapper;
class vtkPolyData;
class vtkPolyDataMapper;

//-----------------------------------------------------------------------------

/*! \class vtkPlusDisplayableObject
 * \brief Class that encapsulates the objects needed for visualizing a tool - the tool object, the actor, a flag indicating whether it is displayable
 * \ingroup PlusAppCommonWidgets
 */
class vtkPlusDisplayableObject : public vtkObject
{
public:
  vtkTypeMacro( vtkPlusDisplayableObject, vtkObject );
  static vtkPlusDisplayableObject* New();

  /*!
  * New function that gets the type and instantiates the proper displayable object class
  * \param aType Type that is read from the DisplayabelObject element of the configuration (eg. "Model")
  */
  static vtkPlusDisplayableObject* New( const char* aType );

  /*! Returns displayable status */
  virtual bool IsDisplayable() = 0;

  /*!
  * Read displayable object configuration
  * \param aConfig DisplayableObject element from the input device set configuration (not the root as usually!)
  */
  virtual PlusStatus ReadConfiguration( vtkXMLDataElement* aConfig );

public:
  vtkSetMacro( ObjectCoordinateFrame, std::string );
  vtkGetMacro( ObjectCoordinateFrame, std::string );

  void SetActor( vtkProp3D* );
  vtkGetObjectMacro( Actor, vtkProp3D );

  vtkSetMacro( Displayable, bool );
  vtkBooleanMacro( Displayable, bool );

  vtkSetMacro( LastOpacity, double );
  vtkGetMacro( LastOpacity, double );

  vtkSetMacro( ObjectId, std::string );
  vtkGetMacro( ObjectId, std::string );

  virtual void SetOpacity( double aOpacity ) = 0;
  virtual double GetOpacity() = 0;

protected:
  vtkPlusDisplayableObject();
  virtual ~vtkPlusDisplayableObject();

protected:
  /* Object coordinate frame name */
  std::string         ObjectCoordinateFrame;

  /*! Id of the model, for lookup purposes */
  std::string         ObjectId;

  /*! Actor displaying the tool model */
  vtkProp3D*          Actor;

  /*! Flag that can disable displaying of this tool */
  bool                Displayable;

  /*! Previously set opacity */
  double              LastOpacity;
};

//-----------------------------------------------------------------------------

/*! \class vtkDisplayableImage
 * \brief Specialized vtkPlusDisplayableObject that displays an image
 * \ingroup PlusAppCommonWidgets
 */
class vtkDisplayableImage : public vtkPlusDisplayableObject
{
public:
  vtkTypeMacro( vtkDisplayableImage, vtkPlusDisplayableObject );
  static vtkDisplayableImage* New();

  /*! Returns displayable status (true if displayable flag is on) */
  bool IsDisplayable();

public:
  /*! Set opacity */
  void SetOpacity( double aOpacity );
  /*! Get opacity */
  double GetOpacity();

protected:
  vtkDisplayableImage();
  virtual ~vtkDisplayableImage();
};

//-----------------------------------------------------------------------------

/*! \class vtkDisplayableAxes
 * \brief Specialized vtkPlusDisplayableObject that displays axes of a coordinate system
 * \ingroup PlusAppCommonWidgets
 */
class vtkDisplayableAxes : public vtkPlusDisplayableObject
{
public:
  vtkTypeMacro( vtkDisplayableAxes, vtkPlusDisplayableObject );
  static vtkDisplayableAxes* New();

  /*! Returns displayable status (true if displayable flag is on) */
  bool IsDisplayable();

  /*! Overridden set function for object coordinate frame that sets the axes name */
  virtual void SetObjectCoordinateFrame( const char* objectCoordinateFrame );

public:
  /*! Set opacity */
  void SetOpacity( double aOpacity );
  /*! Get opacity */
  double GetOpacity();

protected:
  vtkDisplayableAxes();
  virtual ~vtkDisplayableAxes();
};

//-----------------------------------------------------------------------------

/*! \class vtkDisplayablePolyData
 * \brief Specialized vtkPlusDisplayableObject that displays a poly data
 * \ingroup PlusAppCommonWidgets
 */
class vtkDisplayablePolyData : public vtkPlusDisplayableObject
{
public:
  vtkTypeMacro( vtkDisplayablePolyData, vtkPlusDisplayableObject );
  static vtkDisplayablePolyData* New();

  /*! Returns displayable status (true if displayable flag is on and valid actor is present) */
  bool IsDisplayable();

  /*! Set color */
  void SetColor( double aR, double aG, double aB );

  /* Get poly data */
  vtkGetObjectMacro( PolyData, vtkPolyData );

  /* Set poly data */
  virtual void SetPolyData( vtkPolyData* polyData );

  /* Set poly data mapper */
  virtual void SetPolyDataMapper( vtkPolyDataMapper* aPolyDataMapper );

  /*! Appends a polydata to the already existing one */
  PlusStatus AppendPolyData( vtkPolyData* aPolyData );

public:
  /*! Set opacity */
  void SetOpacity( double aOpacity );
  /*! Get opacity */
  double GetOpacity();

protected:
  vtkDisplayablePolyData();
  virtual ~vtkDisplayablePolyData();

protected:
  /*! Displayed poly data */
  vtkPolyData* PolyData;
};

//-----------------------------------------------------------------------------

/*! \class vtkDisplayableModel
 * \brief Specialized vtkPlusDisplayableObject that displays a model
 * \ingroup PlusAppCommonWidgets
 */
class vtkDisplayableModel : public vtkDisplayablePolyData
{
public:
  vtkTypeMacro( vtkDisplayableModel, vtkDisplayablePolyData );
  static vtkDisplayableModel* New();

  /*!
  * Read displayable object configuration
  * \param aConfig DisplayableObject element from the input device set configuration (not the root as usually!)
  */
  PlusStatus ReadConfiguration( vtkXMLDataElement* aConfig );

public:
  /*! Set STL model file name */
  vtkSetStringMacro( STLModelFileName );
  /*! Get STL model file name */
  vtkGetStringMacro( STLModelFileName );

  /*! Get model to tool transform */
  vtkGetObjectMacro( ModelToObjectTransform, vtkTransform );

protected:
  /*! Set model to tool transform */
  vtkSetObjectMacro( ModelToObjectTransform, vtkTransform );

  /*! Assemble and set default stylus model for stylus tool actor */
  PlusStatus SetDefaultStylusModel();

protected:
  vtkDisplayableModel();
  virtual ~vtkDisplayableModel();

protected:
  /*! Name of the STL file containing the 3D model of this tool */
  char*               STLModelFileName;

  /* Model to tool transform */
  vtkTransform*       ModelToObjectTransform;
};

#endif