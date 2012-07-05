/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#include "PlusConfigure.h"

#include "vtkToolAxesActor.h"

#include "vtkSmartPointer.h"
#include "vtkActor.h"
#include "vtkCaptionActor2D.h"
#include "vtkConeSource.h"
#include "vtkLineSource.h"
#include "vtkObject.h"
#include "vtkObjectFactory.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkPropCollection.h"
#include "vtkProperty.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkTextProperty.h"
#include "vtkTransform.h"
#include "vtkTextActor.h"

vtkStandardNewMacro(vtkToolAxesActor);

//----------------------------------------------------------------------------
vtkToolAxesActor::AxisInfo::AxisInfo()
{
  ArrowShaftActor=vtkActor::New();
  ArrowShaftLine=vtkLineSource::New();
  vtkSmartPointer<vtkPolyDataMapper> mapper=vtkSmartPointer<vtkPolyDataMapper>::New();
  ArrowShaftActor->SetMapper(mapper);
  mapper->SetInputConnection(ArrowShaftLine->GetOutputPort());

  LabelActor=vtkCaptionActor2D::New();
  LabelActor->SetHeight(0.03);
  LabelActor->LeaderOff();
  LabelActor->SetPadding(0);
  vtkTextProperty* textprop = LabelActor->GetTextActor()->GetTextProperty();
  textprop->ItalicOff();
  textprop->SetJustificationToLeft();
  textprop->SetVerticalJustificationToCentered(); 
}

//----------------------------------------------------------------------------
vtkToolAxesActor::AxisInfo::~AxisInfo()
{
  ArrowShaftActor->Delete();
  ArrowShaftActor=NULL;
  ArrowShaftLine->Delete();
  ArrowShaftLine=NULL;
  LabelActor->Delete();
  LabelActor=NULL;
}

//----------------------------------------------------------------------------
void vtkToolAxesActor::AxisInfo::ReleaseGraphicsResources(vtkWindow *win)
{
  ArrowShaftActor->ReleaseGraphicsResources( win );
  LabelActor->ReleaseGraphicsResources( win );
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
vtkToolAxesActor::vtkToolAxesActor()
{
  this->Axes[0].LabelActor->SetCaption("X");  
  this->Axes[0].LabelActor->BorderOff();
  this->Axes[0].ArrowShaftActor->GetProperty()->SetColor(1, 0, 0);
  this->Axes[1].LabelActor->SetCaption("Y");
  this->Axes[1].LabelActor->BorderOff();
  this->Axes[1].ArrowShaftActor->GetProperty()->SetColor(0, 1, 0);
  this->Axes[2].LabelActor->SetCaption("Z");
  this->Axes[2].LabelActor->BorderOff();
  this->Axes[2].ArrowShaftActor->GetProperty()->SetColor(0, 0, 1);

  this->NameLabelActor=vtkCaptionActor2D::New();
  this->NameLabelActor->BorderOff();
  
  this->ShaftLength = 100.0;

  this->UpdateProps();
}

//----------------------------------------------------------------------------
vtkToolAxesActor::~vtkToolAxesActor()
{
  this->NameLabelActor->Delete();
  this->NameLabelActor=NULL;
}

//----------------------------------------------------------------------------
void vtkToolAxesActor::GetActors(vtkPropCollection *ac)
{
  ac->AddItem(this->Axes[0].ArrowShaftActor);
  ac->AddItem(this->Axes[0].LabelActor);
  ac->AddItem(this->Axes[1].ArrowShaftActor);
  ac->AddItem(this->Axes[1].LabelActor);
  ac->AddItem(this->Axes[2].ArrowShaftActor);
  ac->AddItem(this->Axes[2].LabelActor);
  
  ac->AddItem(this->NameLabelActor);  
}

//----------------------------------------------------------------------------
int vtkToolAxesActor::RenderOpaqueGeometry(vtkViewport *vp)
{
  int renderedSomething = 0;

  this->UpdateProps();

  renderedSomething += this->Axes[0].ArrowShaftActor->RenderOpaqueGeometry( vp );
  renderedSomething += this->Axes[1].ArrowShaftActor->RenderOpaqueGeometry( vp );
  renderedSomething += this->Axes[2].ArrowShaftActor->RenderOpaqueGeometry( vp );

  renderedSomething += this->Axes[0].LabelActor->RenderOpaqueGeometry( vp );
  renderedSomething += this->Axes[1].LabelActor->RenderOpaqueGeometry( vp );
  renderedSomething += this->Axes[2].LabelActor->RenderOpaqueGeometry( vp );

  renderedSomething += this->NameLabelActor->RenderOpaqueGeometry( vp );

  renderedSomething = (renderedSomething > 0)?(1):(0);
  return renderedSomething;
}

//-----------------------------------------------------------------------------
int vtkToolAxesActor::RenderTranslucentPolygonalGeometry(vtkViewport *vp)
{
  int renderedSomething = 0;

  this->UpdateProps();

  renderedSomething += this->Axes[0].ArrowShaftActor->RenderTranslucentPolygonalGeometry( vp );
  renderedSomething += this->Axes[1].ArrowShaftActor->RenderTranslucentPolygonalGeometry( vp );
  renderedSomething += this->Axes[2].ArrowShaftActor->RenderTranslucentPolygonalGeometry( vp );

  renderedSomething += this->Axes[0].LabelActor->RenderTranslucentPolygonalGeometry( vp );
  renderedSomething += this->Axes[1].LabelActor->RenderTranslucentPolygonalGeometry( vp );
  renderedSomething += this->Axes[2].LabelActor->RenderTranslucentPolygonalGeometry( vp );

  renderedSomething += this->NameLabelActor->RenderTranslucentPolygonalGeometry( vp );

  renderedSomething = (renderedSomething > 0)?(1):(0);
  return renderedSomething;
}

//-----------------------------------------------------------------------------
// Description:
// Does this prop have some translucent polygonal geometry?
int vtkToolAxesActor::HasTranslucentPolygonalGeometry()
{
  int result = 0;

  this->UpdateProps();

  result |= this->Axes[0].ArrowShaftActor->HasTranslucentPolygonalGeometry();
  result |= this->Axes[1].ArrowShaftActor->HasTranslucentPolygonalGeometry();
  result |= this->Axes[2].ArrowShaftActor->HasTranslucentPolygonalGeometry();

  result |= this->Axes[0].LabelActor->HasTranslucentPolygonalGeometry();
  result |= this->Axes[1].LabelActor->HasTranslucentPolygonalGeometry();
  result |= this->Axes[2].LabelActor->HasTranslucentPolygonalGeometry();

  result |= this->NameLabelActor->HasTranslucentPolygonalGeometry();

  return result;
}

//-----------------------------------------------------------------------------
int vtkToolAxesActor::RenderOverlay(vtkViewport *vp)
{
  int renderedSomething = 0;

  this->UpdateProps();

  renderedSomething += this->Axes[0].LabelActor->RenderOverlay( vp );
  renderedSomething += this->Axes[1].LabelActor->RenderOverlay( vp );
  renderedSomething += this->Axes[2].LabelActor->RenderOverlay( vp );

  renderedSomething += this->NameLabelActor->RenderOverlay( vp );

  return (renderedSomething > 0)?(1):(0);
}

//----------------------------------------------------------------------------
void vtkToolAxesActor::ReleaseGraphicsResources(vtkWindow *win)
{
  this->Axes[0].ReleaseGraphicsResources( win );
  this->Axes[1].ReleaseGraphicsResources( win );
  this->Axes[2].ReleaseGraphicsResources( win );
  
  this->NameLabelActor->ReleaseGraphicsResources( win );  
}

//----------------------------------------------------------------------------
void vtkToolAxesActor::GetBounds(double bounds[6])
{
  double *bds = this->GetBounds();
  bounds[0] = bds[0];
  bounds[1] = bds[1];
  bounds[2] = bds[2];
  bounds[3] = bds[3];
  bounds[4] = bds[4];
  bounds[5] = bds[5];
}

//----------------------------------------------------------------------------
void vtkToolAxesActor::ExtendMaxBounds(vtkActor *actor, double* bounds)
{
  double thisActorsBounds[6]={0};
  actor->GetBounds(thisActorsBounds);
  for ( int i = 0; i < 3; ++i )
  {
    if (thisActorsBounds[2*i]<bounds[2*i])
    {
      bounds[2*i] = thisActorsBounds[2*i];
    }
    if (thisActorsBounds[2*i+1]>bounds[2*i+1])
    {
      bounds[2*i+1] = thisActorsBounds[2*i+1];
    }
  }
}

//----------------------------------------------------------------------------
// Get the bounds for this Actor as (Xmin,Xmax,Ymin,Ymax,Zmin,Zmax).
double *vtkToolAxesActor::GetBounds()
{
  this->Axes[0].ArrowShaftActor->GetBounds(this->Bounds);
  ExtendMaxBounds(this->Axes[1].ArrowShaftActor, this->Bounds);
  ExtendMaxBounds(this->Axes[2].ArrowShaftActor, this->Bounds);

  return this->Bounds;
}

//----------------------------------------------------------------------------
unsigned long int vtkToolAxesActor::GetMTime()
{
  unsigned long mTime = this->Superclass::GetMTime();
  return mTime;
}

//----------------------------------------------------------------------------
unsigned long int vtkToolAxesActor::GetRedrawMTime()
{
  unsigned long mTime = this->GetMTime();
  return mTime;
}

//----------------------------------------------------------------------------
void vtkToolAxesActor::UpdateProps()
{
  // Get the Tool to World transform
  if ( vtkTransform::SafeDownCast(this->GetUserTransform())==NULL )
  {
    vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
    this->SetUserTransform(transform);
  }
  vtkTransform* toolToWorldTransform=vtkTransform::SafeDownCast(this->GetUserTransform());
  if (toolToWorldTransform==NULL)
  {
    LOG_ERROR("vtkToolAxesActor UserTransform is invalid");
    return;
  }

  // Set the shaft origins
  double origin_Tool[4]={0,0,0,1}; // Tool coordinate system origin position (in the Tool cordinate system)
  double origin_World[4]={0,0,0,1}; // Tool coordinate system origin position (in the World cordinate system)
  toolToWorldTransform->MultiplyPoint(origin_Tool,origin_World);
  this->Axes[0].ArrowShaftLine->SetPoint1(origin_World);
  this->Axes[1].ArrowShaftLine->SetPoint1(origin_World);
  this->Axes[2].ArrowShaftLine->SetPoint1(origin_World);

  // Set shaft tips

  double shaftTip0_Tool[4]={this->ShaftLength,0,0,1};  // tip of the X shaft (in the Tool cordinate system)
  double shaftTip0_World[4]={0,0,0,1}; // tip of the X shaft (in the World cordinate system)
  toolToWorldTransform->MultiplyPoint(shaftTip0_Tool,shaftTip0_World);
  this->Axes[0].ArrowShaftLine->SetPoint2(shaftTip0_World);
  
  double shaftTip1_Tool[4]={0,this->ShaftLength,0,1};  // tip of the Y shaft (in the Tool cordinate system)
  double shaftTip1_World[4]={0,0,0,1}; // tip of the Y shaft (in the World cordinate system)
  toolToWorldTransform->MultiplyPoint(shaftTip1_Tool,shaftTip1_World);
  this->Axes[1].ArrowShaftLine->SetPoint2(shaftTip1_World);

  double shaftTip2_Tool[4]={0,0,this->ShaftLength,1};  // tip of the Z shaft (in the Tool cordinate system)
  double shaftTip2_World[4]={0,0,0,1}; // tip of the Z shaft (in the World cordinate system)
  toolToWorldTransform->MultiplyPoint(shaftTip2_Tool,shaftTip2_World);
  this->Axes[2].ArrowShaftLine->SetPoint2(shaftTip2_World);  
  
  // Draw axis labels
  this->Axes[0].LabelActor->SetAttachmentPoint(shaftTip0_World);
  this->Axes[1].LabelActor->SetAttachmentPoint(shaftTip1_World);
  this->Axes[2].LabelActor->SetAttachmentPoint(shaftTip2_World);

  this->NameLabelActor->SetAttachmentPoint(origin_World);
  this->NameLabelActor->SetHeight(0.05);
}


//----------------------------------------------------------------------------
void vtkToolAxesActor::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "Name: " << this->NameLabelActor->GetCaption() << endl;  
  os << indent << "ShaftLength: " << this->ShaftLength << endl;
}

//----------------------------------------------------------------------------
void vtkToolAxesActor::SetName(const std::string& name)
{
  this->NameLabelActor->SetCaption(name.c_str());
}
