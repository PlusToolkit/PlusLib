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
#include "vtkTubeFilter.h"
#include "vtkAppendPolyData.h"

vtkStandardNewMacro(vtkToolAxesActor);

//----------------------------------------------------------------------------
vtkToolAxesActor::AxisInfo::AxisInfo()
{
  // Shaft
  this->ArrowShaftActor=vtkActor::New();
  this->ArrowShaftLineSource=vtkLineSource::New();
  vtkSmartPointer<vtkPolyDataMapper> mapper=vtkSmartPointer<vtkPolyDataMapper>::New();
  this->ArrowShaftActor->SetMapper(mapper);
  this->TubeFilter = vtkTubeFilter::New();
  this->TubeFilter->SetInputConnection(this->ArrowShaftLineSource->GetOutputPort());
  this->TubeFilter->CappingOn();
  this->TubeFilter->SetNumberOfSides(32);
  this->TubeFilter->SetRadius(0.03);

  this->ArrowTipConeSource = vtkConeSource::New();
  this->ArrowTipConeSource->SetResolution(32);
  this->ArrowTipConeSource->SetRadius(0.06);
  this->ArrowTipConeSource->SetHeight(0.1);

  vtkSmartPointer<vtkAppendPolyData> appendFilter = vtkSmartPointer<vtkAppendPolyData>::New();
  appendFilter->AddInputConnection(this->TubeFilter->GetOutputPort());
  appendFilter->AddInputConnection(this->ArrowTipConeSource->GetOutputPort());

  mapper->SetInputConnection(appendFilter->GetOutputPort());

  // Label
  this->LabelActor=vtkCaptionActor2D::New();
  this->LabelActor->SetHeight(0.02);
  this->LabelActor->LeaderOff();
  this->LabelActor->SetPadding(0);
  this->LabelActor->BorderOff();
  vtkTextProperty* textprop = this->LabelActor->GetTextActor()->GetTextProperty();
  textprop->ItalicOff();
  textprop->SetJustificationToLeft();
  textprop->SetVerticalJustificationToCentered();
}

//----------------------------------------------------------------------------
vtkToolAxesActor::AxisInfo::~AxisInfo()
{
  if (this->ArrowShaftActor)
  {
    this->ArrowShaftActor->Delete();
    this->ArrowShaftActor=NULL;
  }

  if (this->TubeFilter)
  {
    this->TubeFilter->Delete();
    this->TubeFilter=NULL;
  }

  if (this->ArrowShaftLineSource)
  {
    this->ArrowShaftLineSource->Delete();
    this->ArrowShaftLineSource=NULL;
  }
  
  if (this->ArrowTipConeSource)
  {
    this->ArrowTipConeSource->Delete();
    this->ArrowTipConeSource=NULL;
  }

  if (this->LabelActor)
  {
    this->LabelActor->Delete();
    this->LabelActor=NULL;
  }
}

//----------------------------------------------------------------------------
void vtkToolAxesActor::AxisInfo::ReleaseGraphicsResources(vtkWindow *win)
{
  this->ArrowShaftActor->ReleaseGraphicsResources( win );
  this->LabelActor->ReleaseGraphicsResources( win );
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
vtkToolAxesActor::vtkToolAxesActor()
{
  this->Axes[0].LabelActor->SetCaption("X");  
  this->Axes[0].ArrowShaftActor->GetProperty()->SetColor(1, 0, 0);
  this->Axes[1].LabelActor->SetCaption("Y");
  this->Axes[1].ArrowShaftActor->GetProperty()->SetColor(0, 1, 0);
  this->Axes[2].LabelActor->SetCaption("Z");
  this->Axes[2].ArrowShaftActor->GetProperty()->SetColor(0, 0, 1);

  this->NameLabelActor=vtkCaptionActor2D::New();
  this->NameLabelActor->SetHeight(0.03);
  this->NameLabelActor->BorderOff();
  this->NameLabelActor->LeaderOff();
  this->NameLabelActor->SetPadding(0);
  vtkTextProperty* textprop = this->NameLabelActor->GetTextActor()->GetTextProperty();
  textprop->ItalicOff();
  textprop->SetJustificationToLeft();
  textprop->SetVerticalJustificationToCentered();

  this->ShaftLength = 0.0;
  this->SetShaftLength(100.0);

  this->ShowLabelsOn();
  this->ShowNameOff();

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
  ac->AddItem(this->Axes[1].ArrowShaftActor);
  ac->AddItem(this->Axes[2].ArrowShaftActor);

  if (this->ShowLabels)
  {
    ac->AddItem(this->Axes[0].LabelActor);
    ac->AddItem(this->Axes[1].LabelActor);
    ac->AddItem(this->Axes[2].LabelActor);
  }

  if (this->ShowName)
  {
    ac->AddItem(this->NameLabelActor);
  }
}

//----------------------------------------------------------------------------
int vtkToolAxesActor::RenderOpaqueGeometry(vtkViewport *vp)
{
  int renderedSomething = 0;

  this->UpdateProps();

  renderedSomething += this->Axes[0].ArrowShaftActor->RenderOpaqueGeometry( vp );
  renderedSomething += this->Axes[1].ArrowShaftActor->RenderOpaqueGeometry( vp );
  renderedSomething += this->Axes[2].ArrowShaftActor->RenderOpaqueGeometry( vp );

  if (this->ShowLabels)
  {
    renderedSomething += this->Axes[0].LabelActor->RenderOpaqueGeometry( vp );
    renderedSomething += this->Axes[1].LabelActor->RenderOpaqueGeometry( vp );
    renderedSomething += this->Axes[2].LabelActor->RenderOpaqueGeometry( vp );
  }

  if (this->ShowName)
  {
    renderedSomething += this->NameLabelActor->RenderOpaqueGeometry( vp );
  }

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

  if (this->ShowLabels)
  {
    renderedSomething += this->Axes[0].LabelActor->RenderTranslucentPolygonalGeometry( vp );
    renderedSomething += this->Axes[1].LabelActor->RenderTranslucentPolygonalGeometry( vp );
    renderedSomething += this->Axes[2].LabelActor->RenderTranslucentPolygonalGeometry( vp );
  }

  if (this->ShowName)
  {
    renderedSomething += this->NameLabelActor->RenderTranslucentPolygonalGeometry( vp );
  }

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

  if (this->ShowLabels)
  {
    result |= this->Axes[0].LabelActor->HasTranslucentPolygonalGeometry();
    result |= this->Axes[1].LabelActor->HasTranslucentPolygonalGeometry();
    result |= this->Axes[2].LabelActor->HasTranslucentPolygonalGeometry();
  }

  if (this->ShowName)
  {
    result |= this->NameLabelActor->HasTranslucentPolygonalGeometry();
  }

  return result;
}

//-----------------------------------------------------------------------------
int vtkToolAxesActor::RenderOverlay(vtkViewport *vp)
{
  int renderedSomething = 0;

  this->UpdateProps();

  if (this->ShowLabels)
  {
    renderedSomething += this->Axes[0].LabelActor->RenderOverlay( vp );
    renderedSomething += this->Axes[1].LabelActor->RenderOverlay( vp );
    renderedSomething += this->Axes[2].LabelActor->RenderOverlay( vp );
  }

  if (this->ShowName)
  {
    renderedSomething += this->NameLabelActor->RenderOverlay( vp );
  }

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

  // Compute the shaft origin and tip positions
  double origin_Tool[4]={0,0,0,1}; // Tool coordinate system origin position (in the Tool coordinate system)
  double origin_World[4]={0,0,0,1}; // Tool coordinate system origin position (in the World coordinate system)
  toolToWorldTransform->MultiplyPoint(origin_Tool,origin_World);

  double shaftTip0_Tool[4]={this->ShaftLength,0,0,1};  // tip of the X shaft (in the Tool coordinate system)
  double shaftTip0_World[4]={0,0,0,1}; // tip of the X shaft (in the World coordinate system)
  toolToWorldTransform->MultiplyPoint(shaftTip0_Tool,shaftTip0_World);

  double shaftTip1_Tool[4]={0,this->ShaftLength,0,1};  // tip of the Y shaft (in the Tool coordinate system)
  double shaftTip1_World[4]={0,0,0,1}; // tip of the Y shaft (in the World coordinate system)
  toolToWorldTransform->MultiplyPoint(shaftTip1_Tool,shaftTip1_World);

  double shaftTip2_Tool[4]={0,0,this->ShaftLength,1};  // tip of the Z shaft (in the Tool coordinate system)
  double shaftTip2_World[4]={0,0,0,1}; // tip of the Z shaft (in the World coordinate system)
  toolToWorldTransform->MultiplyPoint(shaftTip2_Tool,shaftTip2_World);

  // Set axes endpoints
  this->Axes[0].ArrowShaftLineSource->SetPoint1(origin_World);
  this->Axes[1].ArrowShaftLineSource->SetPoint1(origin_World);
  this->Axes[2].ArrowShaftLineSource->SetPoint1(origin_World);
  this->Axes[0].ArrowShaftLineSource->SetPoint2(shaftTip0_World);
  this->Axes[1].ArrowShaftLineSource->SetPoint2(shaftTip1_World);
  this->Axes[2].ArrowShaftLineSource->SetPoint2(shaftTip2_World);

  // Position arrows
  this->Axes[0].ArrowTipConeSource->SetCenter(shaftTip0_World[0], shaftTip0_World[1], shaftTip0_World[2]);
  this->Axes[1].ArrowTipConeSource->SetCenter(shaftTip1_World[0], shaftTip1_World[1], shaftTip1_World[2]);
  this->Axes[2].ArrowTipConeSource->SetCenter(shaftTip2_World[0], shaftTip2_World[1], shaftTip2_World[2]);
  this->Axes[0].ArrowTipConeSource->SetDirection(shaftTip0_World[0]-origin_World[0], shaftTip0_World[1]-origin_World[1], shaftTip0_World[2]-origin_World[2]);
  this->Axes[1].ArrowTipConeSource->SetDirection(shaftTip1_World[0]-origin_World[0], shaftTip1_World[1]-origin_World[1], shaftTip1_World[2]-origin_World[2]);
  this->Axes[2].ArrowTipConeSource->SetDirection(shaftTip2_World[0]-origin_World[0], shaftTip2_World[1]-origin_World[1], shaftTip2_World[2]-origin_World[2]);

  // Draw axis labels
  if (this->ShowLabels)
  {
    this->Axes[0].LabelActor->SetAttachmentPoint(shaftTip0_World);
    this->Axes[1].LabelActor->SetAttachmentPoint(shaftTip1_World);
    this->Axes[2].LabelActor->SetAttachmentPoint(shaftTip2_World);
  }

  if (this->ShowName)
  {
    this->NameLabelActor->SetAttachmentPoint(origin_World);
  }
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

//----------------------------------------------------------------------------
void vtkToolAxesActor::SetShaftLength(double shaftLength)
{
  this->ShaftLength = shaftLength;

  this->Axes[0].TubeFilter->SetRadius(this->ShaftLength * 0.03);
  this->Axes[1].TubeFilter->SetRadius(this->ShaftLength * 0.03);
  this->Axes[2].TubeFilter->SetRadius(this->ShaftLength * 0.03);

  this->Axes[0].ArrowTipConeSource->SetRadius(this->ShaftLength * 0.06);
  this->Axes[1].ArrowTipConeSource->SetRadius(this->ShaftLength * 0.06);
  this->Axes[2].ArrowTipConeSource->SetRadius(this->ShaftLength * 0.06);

  this->Axes[0].ArrowTipConeSource->SetHeight(this->ShaftLength * 0.12);
  this->Axes[1].ArrowTipConeSource->SetHeight(this->ShaftLength * 0.12);
  this->Axes[2].ArrowTipConeSource->SetHeight(this->ShaftLength * 0.12);
}
