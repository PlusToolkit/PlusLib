/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#include "PlusMath.h"
#include "vtkMath.h"
#include "vtkMatrix4x4.h"
#include "vtkBrachyStepperPhantomRegistrationAlgo.h"
#include "vtkObjectFactory.h"
#include "vtkTrackedFrameList.h"
#include "vtkPoints.h"
#include "vtksys/SystemTools.hxx"
#include "vtkGnuplotExecuter.h"
#include "vtkHTMLGenerator.h"
#include "vtkDoubleArray.h"
#include "vtkVariantArray.h"
#include "vnl/vnl_cross.h"  

//----------------------------------------------------------------------------
vtkCxxRevisionMacro(vtkBrachyStepperPhantomRegistrationAlgo, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkBrachyStepperPhantomRegistrationAlgo); 

//----------------------------------------------------------------------------
vtkBrachyStepperPhantomRegistrationAlgo::vtkBrachyStepperPhantomRegistrationAlgo()
{
  this->TrackedFrameList = NULL; 
  this->PhantomToReferenceTransform = NULL; 
  this->SetSpacing(0,0); 
  this->SetCenterOfRotationPx(0, 0); 

  this->PhantomToReferenceTransform = vtkTransform::New(); 

  // TODO: remove it! 
  this->TransformTemplateHolderToPhantom = NULL; 
  this->TransformReferenceToTemplateHolder = vtkTransform::New(); 
}

//----------------------------------------------------------------------------
vtkBrachyStepperPhantomRegistrationAlgo::~vtkBrachyStepperPhantomRegistrationAlgo()
{
  this->SetTrackedFrameList(NULL); 

  if ( this->PhantomToReferenceTransform != NULL )
  {
    this->PhantomToReferenceTransform->Delete(); 
    this->PhantomToReferenceTransform = NULL; 
  }
  if ( this->TransformTemplateHolderToPhantom != NULL )
  {
    this->TransformTemplateHolderToPhantom->Delete(); 
    this->TransformTemplateHolderToPhantom = NULL; 
  }
  if ( this->TransformReferenceToTemplateHolder != NULL )
  {
    this->TransformReferenceToTemplateHolder->Delete(); 
    this->TransformReferenceToTemplateHolder = NULL; 
  }
}

//----------------------------------------------------------------------------
void vtkBrachyStepperPhantomRegistrationAlgo::PrintSelf(ostream& os, vtkIndent indent)
{
  os << std::endl;
  this->Superclass::PrintSelf(os,indent);
  os << indent << "Update time: " << UpdateTime.GetMTime() << std::endl; 
  os << indent << "Spacing: " << this->Spacing[0] << "  " << this->Spacing[1] << std::endl;
  os << indent << "Center of rotation (px): " << this->CenterOfRotationPx[0] << "  " << this->CenterOfRotationPx[1] << std::endl;

  if ( this->PhantomToReferenceTransform != NULL )
  {
    os << indent << "Phantom to reference transform: " << std::endl; 
    this->PhantomToReferenceTransform->PrintSelf(os, indent); 
  }

  if ( this->TrackedFrameList != NULL )
  {
    os << indent << "TrackedFrameList:" << std::endl;
    this->TrackedFrameList->PrintSelf(os, indent); 
  }
}


//----------------------------------------------------------------------------
void vtkBrachyStepperPhantomRegistrationAlgo::SetInputs(vtkTrackedFrameList* trackedFrameList, double spacing[2], double centerOfRotationPx[2], const std::vector<NWire>& nWires)
{
  LOG_TRACE("vtkBrachyStepperPhantomRegistrationAlgo::SetInput"); 
  this->SetTrackedFrameList(trackedFrameList); 
  this->SetSpacing(spacing); 
  this->SetCenterOfRotationPx(centerOfRotationPx); 
  this->NWires = nWires; 
  this->Modified(); 
}


//----------------------------------------------------------------------------
PlusStatus vtkBrachyStepperPhantomRegistrationAlgo::GetPhantomToReferenceTransform( vtkTransform* phantomToReferenceTransform)
{
  LOG_TRACE("vtkBrachyStepperPhantomRegistrationAlgo::GetRotationEncoderScale"); 
  // Update result 
  PlusStatus status = this->Update(); 

  if ( this->PhantomToReferenceTransform == NULL )
  {
    this->PhantomToReferenceTransform = vtkTransform::New(); 
  }

  phantomToReferenceTransform->DeepCopy( this->PhantomToReferenceTransform ); 
  return status; 
}

//----------------------------------------------------------------------------
PlusStatus vtkBrachyStepperPhantomRegistrationAlgo::Update()
{
  LOG_TRACE("vtkBrachyStepperPhantomRegistrationAlgo::Update"); 

  if ( this->GetMTime() < this->UpdateTime.GetMTime() )
  {
    LOG_DEBUG("Rotation encoder calibration result is up-to-date!"); 
    return PLUS_SUCCESS; 
  }

  if ( this->TrackedFrameList == NULL )
  {
    LOG_ERROR("Failed to construct linear equation for rotation encoder calibration - tracked frame list is NULL!"); 
    return PLUS_FAIL; 
  }
  
  // ==================================================================================
  // Compute the distance from the probe to phantom 
  // ==================================================================================
  // 1. This point-to-line distance holds the key to relate the position of the TRUS 
  //    rotation center to the precisely designed iCAL phantom geometry in Solid Edge CAD.
  // 2. Here we employ a straight-forward method based on vector theory as one of the 
  //    simplest and most efficient way to compute a point-line distance.
  //    FORMULA: D_O2AB = norm( cross(OA,OB) ) / norm(A-B)
  // ==================================================================================

  std::vector<vtkSmartPointer<vtkPoints>> vectorOfWirePoints; 
  for ( int index = 0; index < this->TrackedFrameList->GetNumberOfTrackedFrames(); ++index )
  {    
    // Get tracked frame from list 
    TrackedFrame* trackedFrame = this->TrackedFrameList->GetTrackedFrame(index); 

    if ( trackedFrame->GetFiducialPointsCoordinatePx() == NULL )
    {
      LOG_ERROR("Unable to get segmented fiducial points from tracked frame - FiducialPointsCoordinatePx is NULL, frame is not yet segmented (position in the list: " << index << ")!" ); 
      continue; 
    }

    if ( trackedFrame->GetFiducialPointsCoordinatePx()->GetNumberOfPoints() == 0 )
    {
      LOG_DEBUG("Unable to get segmented fiducial points from tracked frame - couldn't segment image (position in the list: " << index << ")!" ); 
      continue; 
    }

    // Add wire #4 (point A) wire #6 (point B) and wire #3 (point C) pixel coordinates to phantom to probe distance point set 
    vtkSmartPointer<vtkPoints> pointset = vtkSmartPointer<vtkPoints>::New();
    pointset->SetDataTypeToDouble(); 
    pointset->InsertNextPoint(trackedFrame->GetFiducialPointsCoordinatePx()->GetPoint(3)); 
    pointset->InsertNextPoint(trackedFrame->GetFiducialPointsCoordinatePx()->GetPoint(5)); 
    pointset->InsertNextPoint(trackedFrame->GetFiducialPointsCoordinatePx()->GetPoint(2)); 
    vectorOfWirePoints.push_back(pointset); 
  }


  vnl_vector<double> rotationCenter3x1InMm(3,0);
  rotationCenter3x1InMm.put(0, this->CenterOfRotationPx[0] * this->Spacing[0]); 
  rotationCenter3x1InMm.put(1, this->CenterOfRotationPx[1] * this->Spacing[1]);
  rotationCenter3x1InMm.put(2, 0);

  // Total number images used for this computation
  const int totalNumberOfImages2ComputePtLnDist = vectorOfWirePoints.size();

  if ( totalNumberOfImages2ComputePtLnDist == 0 )
  {
    LOG_ERROR("Failed to register phantom to reference. Probe distance calculation data is empty!"); 
    return PLUS_FAIL; 
  }

  // This will keep a trace on all the calculated distance
  vnl_vector<double> listOfPhantomToProbeVerticalDistanceInMm(totalNumberOfImages2ComputePtLnDist, 0 );
  vnl_vector<double> listOfPhantomToProbeHorizontalDistanceInMm(totalNumberOfImages2ComputePtLnDist, 0 );

  for (int i = 0; i < totalNumberOfImages2ComputePtLnDist; i++)
  {
    // Extract point A
    vnl_vector<double> pointAInMm(3,0);
    pointAInMm.put( 0, vectorOfWirePoints[i]->GetPoint(0)[0] * this->Spacing[0] );
    pointAInMm.put( 1, vectorOfWirePoints[i]->GetPoint(0)[1] * this->Spacing[1] );
    pointAInMm.put( 2, 0 ); 

    // Extract point B
    vnl_vector<double> pointBInMm(3,0);
    pointBInMm.put( 0, vectorOfWirePoints[i]->GetPoint(1)[0] * this->Spacing[0] );
    pointBInMm.put( 1, vectorOfWirePoints[i]->GetPoint(1)[1] * this->Spacing[1] );
    pointBInMm.put( 2, 0 ); 

    // Extract point C
    vnl_vector<double> pointCInMm(3,0);
    pointCInMm.put( 0, vectorOfWirePoints[i]->GetPoint(2)[0] * this->Spacing[0] );
    pointCInMm.put( 1, vectorOfWirePoints[i]->GetPoint(2)[1] * this->Spacing[1] );
    pointCInMm.put( 2, 0 ); 

    // Construct vectors among rotation center, point A, and point B.
    const vnl_vector<double> vectorRotationCenterToPointAInMm = pointAInMm - rotationCenter3x1InMm;
    const vnl_vector<double> vectorRotationCenterToPointBInMm = pointBInMm - rotationCenter3x1InMm;
    const vnl_vector<double> vectorRotationCenterToPointCInMm = pointCInMm - rotationCenter3x1InMm;
    const vnl_vector<double> vectorPointAToPointBInMm = pointBInMm - pointAInMm;
    const vnl_vector<double> vectorPointBToPointCInMm = pointCInMm - pointBInMm;

    // Compute the point-line distance from probe to the line passing through A and B points, based on the
    // standard vector theory. FORMULA: D_O2AB = norm( cross(OA,OB) ) / norm(A-B)
    const double thisPhantomToProbeVerticalDistanceInMm = vnl_cross_3d( vectorRotationCenterToPointAInMm, vectorRotationCenterToPointBInMm ).magnitude() / vectorPointAToPointBInMm.magnitude();

    // Compute the point-line distance from probe to the line passing through B and C points, based on the
    // standard vector theory. FORMULA: D_O2AB = norm( cross(OA,OB) ) / norm(A-B)
    const double thisPhantomToProbeHorizontalDistanceInMm = vnl_cross_3d( vectorRotationCenterToPointBInMm, vectorRotationCenterToPointCInMm ).magnitude() / vectorPointBToPointCInMm.magnitude();

    // Populate the data container
    listOfPhantomToProbeVerticalDistanceInMm.put(i, thisPhantomToProbeVerticalDistanceInMm );
    listOfPhantomToProbeHorizontalDistanceInMm.put(i, thisPhantomToProbeHorizontalDistanceInMm );
  }

  double wire6ToProbeDistanceInMm[2] = { listOfPhantomToProbeHorizontalDistanceInMm.mean(), listOfPhantomToProbeVerticalDistanceInMm.mean() }; 

  LOG_INFO("Wire #6 position in phantom (mm): " << this->NWires[1].Wires[2].EndPointFront[0] << "   " << this->NWires[1].Wires[2].EndPointFront[1]); 
  LOG_INFO("Wire #6 to probe distance (mm): " << wire6ToProbeDistanceInMm[0] << "   " << wire6ToProbeDistanceInMm[1]); 

  double phantomToReferenceDistanceInMm[2] = { 
    this->NWires[1].Wires[2].EndPointFront[0] + wire6ToProbeDistanceInMm[0], 
    this->NWires[1].Wires[2].EndPointFront[1] + wire6ToProbeDistanceInMm[1] }; 


  LOG_INFO("Phantom to probe distance (mm): " << phantomToReferenceDistanceInMm[0] << "   " << phantomToReferenceDistanceInMm[1] ); 

  //////////////////////////////////////////////////////////////////////////////////////

  
  /// **************** TemplateHolderToPhantom *********************
  // TODO: remove it!!! 
  double templateHolderPositionX = this->TransformTemplateHolderToPhantom->GetPosition()[0];
  double templateHolderPositionY = this->TransformTemplateHolderToPhantom->GetPosition()[1];
  double templateHolderPositionZ = this->TransformTemplateHolderToPhantom->GetPosition()[2];

  vtkSmartPointer<vtkTransform> tTemplateHolderToTemplate = vtkSmartPointer<vtkTransform>::New();
  tTemplateHolderToTemplate->Translate( templateHolderPositionX, templateHolderPositionY, templateHolderPositionZ);
  /// **************** TemplateHolderToPhantom *********************


  /// **************** ReferenceToTemplateHolder *********************
  //TODO: compute this transform in the application layer!!!!
  // Vertical distance from the template mounter hole center to the TRUS Rotation Center (reference)
  // :TODO: transform with the whole matrix instead of just using the XY position values
  double verticalDistanceTemplateMounterHoleToTRUSRotationCenterInMM = phantomToReferenceDistanceInMm[1] - templateHolderPositionY; 

  // Horizontal distance from the template mounter hole center to the TRUS Rotation Center (reference)
  // :TODO: transform with the whole matrix instead of just using the XY position values
  double horizontalDistanceTemplateMounterHoleToTRUSRotationCenterInMM = phantomToReferenceDistanceInMm[0] - templateHolderPositionX; 

  vtkSmartPointer<vtkTransform> tReferenceToTemplateHolder = vtkSmartPointer<vtkTransform>::New();
  tReferenceToTemplateHolder->Translate( horizontalDistanceTemplateMounterHoleToTRUSRotationCenterInMM, verticalDistanceTemplateMounterHoleToTRUSRotationCenterInMM, 0);
  LOG_INFO("Reference to template holder distance (mm): " << horizontalDistanceTemplateMounterHoleToTRUSRotationCenterInMM << "   " << verticalDistanceTemplateMounterHoleToTRUSRotationCenterInMM ); 
  this->TransformReferenceToTemplateHolder->DeepCopy(tReferenceToTemplateHolder); 
  /// **************** ReferenceToTemplateHolder *********************

  vtkSmartPointer<vtkTransform> tTemplateToReference = vtkSmartPointer<vtkTransform>::New();
  tTemplateToReference->PostMultiply(); 
  tTemplateToReference->Concatenate( tReferenceToTemplateHolder ); 
  tTemplateToReference->Concatenate( tTemplateHolderToTemplate ); 
  tTemplateToReference->Inverse(); 

  this->PhantomToReferenceTransform->DeepCopy(tTemplateToReference); 

  this->UpdateTime.Modified(); 

  return PLUS_SUCCESS; 
}

