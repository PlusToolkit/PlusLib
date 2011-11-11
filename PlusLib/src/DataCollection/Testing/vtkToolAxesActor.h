/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __vtkToolAxesActor_h
#define __vtkToolAxesActor_h

#include "vtkProp3D.h"

class vtkActor;
class vtkCaptionActor2D;
class vtkConeSource;
class vtkCylinderSource;
class vtkLineSource;
class vtkPolyData;
class vtkPropCollection;
class vtkProperty;
class vtkRenderer;
class vtkSphereSource;

class VTK_EXPORT vtkToolAxesActor : public vtkProp3D
{
public:
  static vtkToolAxesActor *New();
  vtkTypeMacro(vtkToolAxesActor,vtkProp3D);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description: 
  // For some exporters and other other operations we must be
  // able to collect all the actors or volumes. These methods
  // are used in that process.
  virtual void GetActors(vtkPropCollection *);

  // Description:
  // Support the standard render methods.
  virtual int RenderOpaqueGeometry(vtkViewport *viewport);
  virtual int RenderTranslucentPolygonalGeometry(vtkViewport *viewport);
  virtual int RenderOverlay(vtkViewport *viewport);

  // Description:
  // Does this prop have some translucent polygonal geometry?
  virtual int HasTranslucentPolygonalGeometry();
  
  // Description:
  // Shallow copy of an axes actor. Overloads the virtual vtkProp method.
  void ShallowCopy(vtkProp *prop);

  // Description:
  // Release any graphics resources that are being consumed by this actor.
  // The parameter window could be used to determine which graphic
  // resources to release.
  void ReleaseGraphicsResources(vtkWindow *);

  // Description:
  // Get the bounds for this Actor as (Xmin,Xmax,Ymin,Ymax,Zmin,Zmax). (The
  // method GetBounds(double bounds[6]) is available from the superclass.)
  void GetBounds(double bounds[6]);
  double *GetBounds();

  // Description:
  // Get the actors mtime plus consider its properties and texture if set.
  unsigned long int GetMTime();
  
  // Description:
  // Return the mtime of anything that would cause the rendered image to 
  // appear differently. Usually this involves checking the mtime of the 
  // prop plus anything else it depends on such as properties, textures
  // etc.
  virtual unsigned long GetRedrawMTime();

  // Description:
  // Set the total length of the axes in 3 dimensions.
  void SetTotalLength( double v[3] ) 
    { this->SetTotalLength( v[0], v[1], v[2] ); }
  void SetTotalLength( double x, double y, double z );
  vtkGetVectorMacro( TotalLength, double, 3 );

  // Description:
  // Set the normalized (0-1) length of the shaft.
  void SetNormalizedShaftLength( double v[3] )
    { this->SetNormalizedShaftLength( v[0], v[1], v[2] ); }
  void SetNormalizedShaftLength( double x, double y, double z );
  vtkGetVectorMacro( NormalizedShaftLength, double, 3 );

  // Description:
  // Set the normalized (0-1) length of the tip.  Normally, this would be
  // 1 - the normalized length of the shaft.
  void SetNormalizedTipLength( double v[3] )
    { this->SetNormalizedTipLength( v[0], v[1], v[2] ); }
  void SetNormalizedTipLength( double x, double y, double z );
  vtkGetVectorMacro( NormalizedTipLength, double, 3 );

  // Description:
  // Set the normalized (0-1) position of the label along the length of
  // the shaft.  A value > 1 is permissible.
  void SetNormalizedLabelPosition( double v[3] )
    { this->SetNormalizedLabelPosition( v[0], v[1], v[2] ); }
  void SetNormalizedLabelPosition( double x, double y, double z );
  vtkGetVectorMacro( NormalizedLabelPosition, double, 3 );   

  // Description:
  // Set/get the resolution of the pieces of the axes actor.
  vtkSetClampMacro(ConeResolution, int, 3, 128);
  vtkGetMacro(ConeResolution, int);
  vtkSetClampMacro(SphereResolution, int, 3, 128);
  vtkGetMacro(SphereResolution, int);
  vtkSetClampMacro(CylinderResolution, int, 3, 128);
  vtkGetMacro(CylinderResolution, int);
  
  // Description:
  // Set/get the radius of the pieces of the axes actor.
  vtkSetClampMacro(ConeRadius, double, 0, VTK_LARGE_FLOAT);
  vtkGetMacro(ConeRadius, double);
  vtkSetClampMacro(SphereRadius, double, 0, VTK_LARGE_FLOAT);
  vtkGetMacro(SphereRadius, double);
  vtkSetClampMacro(CylinderRadius, double, 0, VTK_LARGE_FLOAT);
  vtkGetMacro(CylinderRadius, double);

  // Description:
  // Set the type of the shaft to a cylinder, line, or user defined geometry.
  void SetShaftType( int type );
  void SetShaftTypeToCylinder()
    { this->SetShaftType( vtkToolAxesActor::CYLINDER_SHAFT ); }
  void SetShaftTypeToLine()
    { this->SetShaftType( vtkToolAxesActor::LINE_SHAFT ); }
  void SetShaftTypeToUserDefined()
    { this->SetShaftType( vtkToolAxesActor::USER_DEFINED_SHAFT ); }
  vtkGetMacro(ShaftType, int);

  // Description:
  // Set the type of the tip to a cone, sphere, or user defined geometry.
  void SetTipType( int type );
  void SetTipTypeToCone()
    { this->SetTipType( vtkToolAxesActor::CONE_TIP ); }
  void SetTipTypeToSphere()
    { this->SetTipType( vtkToolAxesActor::SPHERE_TIP ); }
  void SetTipTypeToUserDefined()
    { this->SetTipType( vtkToolAxesActor::USER_DEFINED_TIP ); }
  vtkGetMacro(TipType, int);

  // Description:
  // Set the user defined tip polydata.
  void SetUserDefinedTip( vtkPolyData * );
  vtkGetObjectMacro( UserDefinedTip, vtkPolyData );

  // Description:
  // Set the user defined shaft polydata.
  void SetUserDefinedShaft( vtkPolyData * );
  vtkGetObjectMacro( UserDefinedShaft, vtkPolyData );

  // Description:
  // Get the tip properties.
  vtkProperty *GetXAxisTipProperty();
  vtkProperty *GetYAxisTipProperty();
  vtkProperty *GetZAxisTipProperty();

  // Description:
  // Get the shaft properties.
  vtkProperty *GetXAxisShaftProperty();
  vtkProperty *GetYAxisShaftProperty();
  vtkProperty *GetZAxisShaftProperty();

  // Description:
  // Retrieve handles to the X, Y and Z axis (so that you can set their text
  // properties for example)
  vtkCaptionActor2D *GetXAxisCaptionActor2D()
    {return this->XAxisLabel;}
  vtkCaptionActor2D *GetYAxisCaptionActor2D()
    {return this->YAxisLabel;}
  vtkCaptionActor2D *GetZAxisCaptionActor2D()
    {return this->ZAxisLabel;}

  // Description:
  // Set/get the label text.
  vtkSetStringMacro( XAxisLabelText );
  vtkGetStringMacro( XAxisLabelText );
  vtkSetStringMacro( YAxisLabelText );
  vtkGetStringMacro( YAxisLabelText );
  vtkSetStringMacro( ZAxisLabelText );
  vtkGetStringMacro( ZAxisLabelText );

  // Description:
  // Enable/disable drawing the axis labels.
  vtkSetMacro(AxisLabels, int);
  vtkGetMacro(AxisLabels, int);
  vtkBooleanMacro(AxisLabels, int);

//BTX
  enum
  {
    CYLINDER_SHAFT,
    LINE_SHAFT,
    USER_DEFINED_SHAFT
  };

  enum
  {
    CONE_TIP,
    SPHERE_TIP,
    USER_DEFINED_TIP
  };  
//ETX

protected:
  vtkToolAxesActor();
  ~vtkToolAxesActor();

  vtkCylinderSource *CylinderSource;
  vtkLineSource     *LineSource;
  vtkConeSource     *ConeSource;
  vtkSphereSource   *SphereSource;

  vtkActor          *XAxisShaft;
  vtkActor          *YAxisShaft;
  vtkActor          *ZAxisShaft;

  vtkActor          *XAxisTip;
  vtkActor          *YAxisTip;
  vtkActor          *ZAxisTip;

  void               UpdateProps();

  double             TotalLength[3];
  double             NormalizedShaftLength[3];
  double             NormalizedTipLength[3];
  double             NormalizedLabelPosition[3];

  int                ShaftType;
  int                TipType;

  vtkPolyData       *UserDefinedTip;
  vtkPolyData       *UserDefinedShaft;

  char              *XAxisLabelText;
  char              *YAxisLabelText;
  char              *ZAxisLabelText;

  vtkCaptionActor2D *XAxisLabel;
  vtkCaptionActor2D *YAxisLabel;
  vtkCaptionActor2D *ZAxisLabel;

  int                AxisLabels;


  int                ConeResolution;
  int                SphereResolution;
  int                CylinderResolution;

  double             ConeRadius;
  double             SphereRadius;
  double             CylinderRadius;

private:
  vtkToolAxesActor(const vtkToolAxesActor&);  // Not implemented.
  void operator=(const vtkToolAxesActor&);  // Not implemented.
};

#endif

