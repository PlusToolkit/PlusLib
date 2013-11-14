/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/

#include "PlusConfigure.h"

#include "SpatialModel.h"

#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkSTLReader.h"
#include "vtkXMLPolyDataReader.h"

// If fraction of the transmitted beam intensity is smaller then this value then we consider the beam to be completely absorbed
const double MINIMUM_BEAM_INTENSITY=1e-9;

//-----------------------------------------------------------------------------
SpatialModel::SpatialModel()
{  
  this->DensityKgPerM3 = 0; 
  this->SoundVelocityMPerSec = 0; 
  this->AttenuationCoefficientDbPerCmMhz = 0;
  this->SurfaceReflectionIntensityDecayDbPerMm = 20;
  this->BackscatterDiffuseReflectionCoefficient = 0;
  this->BackscatterSpecularReflectionCoefficient = 0; 
  this->ImagingFrequencyMhz = 0;
  this->ModelToObjectTransform = vtkMatrix4x4::New(); 
  this->ReferenceToObjectTransform = vtkMatrix4x4::New();
  this->ModelLocalizer=vtkModifiedBSPTree::New();
  this->PolyData=NULL;
  this->ModelFileNeedsUpdate=false;
  this->TransducerSpatialModelMaxOverlapMm=10;
}

//-----------------------------------------------------------------------------
SpatialModel::~SpatialModel()
{
  SetModelToObjectTransform(NULL);
  SetReferenceToObjectTransform(NULL);
  SetModelLocalizer(NULL);
  SetPolyData(NULL);
}

//-----------------------------------------------------------------------------
SpatialModel::SpatialModel(const SpatialModel& model)
{
  this->Name=model.Name;
  this->ObjectCoordinateFrame=model.ObjectCoordinateFrame;
  this->ModelFile=model.ModelFile;
  this->ImagingFrequencyMhz=model.ImagingFrequencyMhz;
  this->DensityKgPerM3=model.DensityKgPerM3;
  this->SoundVelocityMPerSec=model.SoundVelocityMPerSec;
  this->AttenuationCoefficientDbPerCmMhz=model.AttenuationCoefficientDbPerCmMhz;
  this->SurfaceReflectionIntensityDecayDbPerMm=model.SurfaceReflectionIntensityDecayDbPerMm;
  this->BackscatterDiffuseReflectionCoefficient=model.BackscatterDiffuseReflectionCoefficient;
  this->BackscatterSpecularReflectionCoefficient=model.BackscatterSpecularReflectionCoefficient;
  this->ModelToObjectTransform = NULL; 
  this->ReferenceToObjectTransform = NULL; 
  this->ModelLocalizer = NULL;
  this->PolyData = NULL;
  SetModelToObjectTransform(model.ModelToObjectTransform);
  SetReferenceToObjectTransform(model.ReferenceToObjectTransform);
  SetModelLocalizer(model.ModelLocalizer);
  SetPolyData(model.PolyData);
  this->ModelFileNeedsUpdate=model.ModelFileNeedsUpdate;
  this->PrecomputedAttenuations=model.PrecomputedAttenuations;
}

//-----------------------------------------------------------------------------
void SpatialModel::operator=(const SpatialModel& model)
{
  this->Name=model.Name;
  this->ObjectCoordinateFrame=model.ObjectCoordinateFrame;
  this->ModelFile=model.ModelFile;
  this->ImagingFrequencyMhz=model.ImagingFrequencyMhz;
  this->DensityKgPerM3=model.DensityKgPerM3;
  this->SoundVelocityMPerSec=model.SoundVelocityMPerSec;
  this->AttenuationCoefficientDbPerCmMhz=model.AttenuationCoefficientDbPerCmMhz;
  this->SurfaceReflectionIntensityDecayDbPerMm=model.SurfaceReflectionIntensityDecayDbPerMm;
  this->BackscatterDiffuseReflectionCoefficient=model.BackscatterDiffuseReflectionCoefficient;
  this->BackscatterSpecularReflectionCoefficient=model.BackscatterSpecularReflectionCoefficient;
  SetModelToObjectTransform(model.ModelToObjectTransform);
  SetReferenceToObjectTransform(model.ReferenceToObjectTransform);
  SetModelLocalizer(model.ModelLocalizer);
  SetPolyData(model.PolyData);
  this->ModelFileNeedsUpdate=model.ModelFileNeedsUpdate;
  this->PrecomputedAttenuations=model.PrecomputedAttenuations;
}

//-----------------------------------------------------------------------------
void SpatialModel::SetModelToObjectTransform(vtkMatrix4x4* modelToObjectTransform)
{
  if (this->ModelToObjectTransform==modelToObjectTransform)
  {
    return;
  }
  if (this->ModelToObjectTransform!=NULL)
  {
    this->ModelToObjectTransform->Delete();
  }
  this->ModelToObjectTransform=modelToObjectTransform;
  if (this->ModelToObjectTransform!=NULL)
  {
    this->ModelToObjectTransform->Register(NULL);
  }
}

//-----------------------------------------------------------------------------
void SpatialModel::SetReferenceToObjectTransform(vtkMatrix4x4* referenceToObjectTransform)
{
  if (this->ReferenceToObjectTransform==referenceToObjectTransform)
  {
    return;
  }
  if (this->ReferenceToObjectTransform!=NULL)
  {
    this->ReferenceToObjectTransform->Delete();
  }
  this->ReferenceToObjectTransform=referenceToObjectTransform;
  if (this->ReferenceToObjectTransform!=NULL)
  {
    this->ReferenceToObjectTransform->Register(NULL);
  }
}


//-----------------------------------------------------------------------------
void SpatialModel::SetPolyData(vtkPolyData* polyData)
{
  if (this->PolyData==polyData)
  {
    return;
  }
  if (this->PolyData!=NULL)
  {
    this->PolyData->Delete();
  }
  this->PolyData=polyData;
  if (this->PolyData!=NULL)
  {
    this->PolyData->Register(NULL);
  }
}

//-----------------------------------------------------------------------------
void SpatialModel::SetModelLocalizer(vtkModifiedBSPTree* modelLocalizer)
{
  if (this->ModelLocalizer==modelLocalizer)
  {
    return;
  }
  if (this->ModelLocalizer!=NULL)
  {
    this->ModelLocalizer->Delete();
  }
  this->ModelLocalizer=modelLocalizer;
  if (this->ModelLocalizer!=NULL)
  {
    this->ModelLocalizer->Register(NULL);
  }
}

//-----------------------------------------------------------------------------
PlusStatus SpatialModel::ReadConfiguration(vtkXMLDataElement* spatialModelElement)
{ 
  if ( spatialModelElement == NULL )
  {
    LOG_ERROR("Unable to configure SpatialModel (XML data element is NULL)"); 
    return PLUS_FAIL; 
  }

  if ( spatialModelElement->GetName() == NULL || STRCASECMP(spatialModelElement->GetName(),"SpatialModel")!=0)
  {
    LOG_ERROR("Unable to read SpatialModel element: unexpected name: "<<(spatialModelElement->GetName()?spatialModelElement->GetName():"(unspecified)")); 
    return PLUS_FAIL; 
  }

  PlusStatus status=PLUS_SUCCESS;

  //Set data elements

  const char* name=spatialModelElement->GetAttribute("Name");
  this->Name = name?name:"";

  const char* objectCoordinateFrame=spatialModelElement->GetAttribute("ObjectCoordinateFrame");
  this->ObjectCoordinateFrame = objectCoordinateFrame?objectCoordinateFrame:"";

  SetModelFile(spatialModelElement->GetAttribute("ModelFile"));

  double vectorModelFileToSpatialModelTransform[16]={0}; 
  if ( spatialModelElement->GetVectorAttribute("ModelToObjectTransform", 16, vectorModelFileToSpatialModelTransform) )
  {
    this->ModelToObjectTransform->DeepCopy(vectorModelFileToSpatialModelTransform); 
  }

  double densityKGPerM3 = 0; 
  if(spatialModelElement->GetScalarAttribute("DensityKgPerM3",densityKGPerM3))
  {
    this->DensityKgPerM3 = densityKGPerM3;  
  }

  double soundVelocityMperSec = 0;
  if(spatialModelElement->GetScalarAttribute("SoundVelocityMPerSec",soundVelocityMperSec)) 
  {
    this->SoundVelocityMPerSec = soundVelocityMperSec;
  }

  double attenuationCoefficientDbPerCmMhz = 0;
  if(spatialModelElement->GetScalarAttribute("AttenuationCoefficientDbPerCmMhz",attenuationCoefficientDbPerCmMhz)) 
  {
    this->AttenuationCoefficientDbPerCmMhz = attenuationCoefficientDbPerCmMhz;
  }

  double surfaceReflectionIntensityDecayDbPerMm = 0;
  if(spatialModelElement->GetScalarAttribute("SurfaceReflectionIntensityDecayDbPerMm",surfaceReflectionIntensityDecayDbPerMm)) 
  {
    this->SurfaceReflectionIntensityDecayDbPerMm = surfaceReflectionIntensityDecayDbPerMm;
  }

  double diffuseReflectionCoefficient = 0;
  if(spatialModelElement->GetScalarAttribute("BackscatterDiffuseReflectionCoefficient", diffuseReflectionCoefficient)) 
  {
    this->BackscatterDiffuseReflectionCoefficient = diffuseReflectionCoefficient;
  }

  double specularReflectionCoefficient = 0;
  if(spatialModelElement->GetScalarAttribute("BackscatterSpecularReflectionCoefficient",specularReflectionCoefficient)) 
  {
    this->BackscatterSpecularReflectionCoefficient = specularReflectionCoefficient;
  }

  double transducerSpatialModelMaxOverlapMm = 0;
  if(spatialModelElement->GetScalarAttribute("TransducerSpatialModelMaxOverlapMm",transducerSpatialModelMaxOverlapMm)) 
  {
    this->TransducerSpatialModelMaxOverlapMm = transducerSpatialModelMaxOverlapMm;
  }

  return status;
}

//-----------------------------------------------------------------------------
void SpatialModel::SetImagingFrequencyMhz(double frequencyMhz)
{
  this->ImagingFrequencyMhz = frequencyMhz; 
}

//-----------------------------------------------------------------------------
double SpatialModel::GetAcousticImpedanceMegarayls()
{
  double acousticImpedanceRayls = this->DensityKgPerM3 * this->SoundVelocityMPerSec; // kg / (s * m2)
  return acousticImpedanceRayls * 1e-6; // megarayls
}

//-----------------------------------------------------------------------------
void SpatialModel::CalculateIntensity(std::vector<double>& reflectedIntensity, int numberOfFilledPixels, double distanceBetweenScanlineSamplePointsMm, double previousModelAcousticImpedanceMegarayls, double incidentIntensity, double &transmittedIntensity)
{
  UpdateModelFile();

  if (numberOfFilledPixels<=0)
  {
    transmittedIntensity=incidentIntensity;
    return;
  }

  // Make sure there is enough space to store the results
  if (reflectedIntensity.size()<numberOfFilledPixels)
  {
    reflectedIntensity.resize(numberOfFilledPixels);
  }

  // Compute reflection from the surface of the previous and this model
  double acousticImpedanceMegarayls = GetAcousticImpedanceMegarayls();
  // intensityReflectionCoefficient: reflected beam intensity / incident beam intensity => can be computed from acoustic impedance mismatch
  double intensityReflectionCoefficient =
    (previousModelAcousticImpedanceMegarayls - acousticImpedanceMegarayls)*(previousModelAcousticImpedanceMegarayls - acousticImpedanceMegarayls)
    /(previousModelAcousticImpedanceMegarayls + acousticImpedanceMegarayls)/(previousModelAcousticImpedanceMegarayls + acousticImpedanceMegarayls);
  double surfaceReflectedBeamIntensity = incidentIntensity * intensityReflectionCoefficient;
  double surfaceTransmittedBeamIntensity = incidentIntensity - surfaceReflectedBeamIntensity;
  // backscatteredReflectedIntensity: intensity reflected from the surface
  // TODO: take into account specular reflection as well
  double backscatteredReflectedIntensity = this->BackscatterDiffuseReflectionCoefficient * surfaceReflectedBeamIntensity;

  // Compute attenuation within this model
  double intensityAttenuationCoefficientdBPerPixel = this->AttenuationCoefficientDbPerCmMhz*(distanceBetweenScanlineSamplePointsMm/10.0)*this->ImagingFrequencyMhz;
  // intensityAttenuationCoefficientPerPixel: should be close to 1, as it's the ratio of (transmitted beam intensity / incident beam intensity) after traversing through a single pixel
  double intensityAttenuationCoefficientPerPixel = pow(10.0,-intensityAttenuationCoefficientdBPerPixel/10.0);
  // intensityAttenuatedFractionPerPixel: how big fraction of the intensity is attenuated during traversing through one voxel
  double intensityAttenuatedFractionPerPixel = (1-intensityAttenuationCoefficientPerPixel);
  // intensityTransmittedFractionPerPixelTwoWay: how big fraction of the intensity is transmitted during traversing through one voxel; takes into account both propagation directions
  double intensityTransmittedFractionPerPixelTwoWay = intensityAttenuationCoefficientPerPixel*intensityAttenuationCoefficientPerPixel;  

  transmittedIntensity = surfaceTransmittedBeamIntensity * intensityTransmittedFractionPerPixelTwoWay;

  if (numberOfFilledPixels>0 && this->PrecomputedAttenuations.size()<numberOfFilledPixels || intensityTransmittedFractionPerPixelTwoWay!=this->PrecomputedAttenuations[0])
  {
    UpdatePrecomputedAttenuations(intensityTransmittedFractionPerPixelTwoWay, numberOfFilledPixels);
  }

  // We iterate until transmittedIntensity * intensityTransmittedFractionPerPixelTwoWay^n > MINIMUM_BEAM_INTENSITY
  // So, n = log(MINIMUM_BEAM_INTENSITY/transmittedIntensity) / log(intensityTransmittedFractionPerPixelTwoWay)
  int numberOfIterationsToReachMinimumBeamIntensity = 0;
  if (transmittedIntensity>MINIMUM_BEAM_INTENSITY)
  {
    numberOfIterationsToReachMinimumBeamIntensity=std::min<int>(numberOfFilledPixels, floor(log(MINIMUM_BEAM_INTENSITY/transmittedIntensity) / log(intensityTransmittedFractionPerPixelTwoWay))+1);
    double backScatterFactor = transmittedIntensity * intensityAttenuatedFractionPerPixel * this->BackscatterDiffuseReflectionCoefficient / intensityTransmittedFractionPerPixelTwoWay;
    for(int currentPixelInFilledPixels = 0; currentPixelInFilledPixels<numberOfIterationsToReachMinimumBeamIntensity; currentPixelInFilledPixels++)  
    {
      // a fraction of the attenuation is caused by backscattering, the backscattering is sensed by the transducer
      // TODO: take into account specular reflection as well
      //reflectedIntensity[currentPixelInFilledPixels] = *(attenuation++) * backScatterFactor;
      reflectedIntensity[currentPixelInFilledPixels] = this->PrecomputedAttenuations[currentPixelInFilledPixels] * backScatterFactor;
    }
    transmittedIntensity*=pow(intensityTransmittedFractionPerPixelTwoWay,numberOfIterationsToReachMinimumBeamIntensity);
  }
  else
  {
    transmittedIntensity=0;
  }   
  // The beam intensity is very close to 0, so fill the remaining values with 0 instead of computing miniscule values
  for(int currentPixelInFilledPixels = numberOfIterationsToReachMinimumBeamIntensity; currentPixelInFilledPixels<numberOfFilledPixels; currentPixelInFilledPixels++)  
  {
    reflectedIntensity[currentPixelInFilledPixels]=0.0;
  }

  // Add surface reflection
  if (backscatteredReflectedIntensity>MINIMUM_BEAM_INTENSITY)
  {
    double surfaceReflectionIntensityDecayPerPixel = pow(10.0,-this->SurfaceReflectionIntensityDecayDbPerMm*distanceBetweenScanlineSamplePointsMm/10.0);
    // We iterate until backscatteredReflectedIntensity * surfaceReflectionIntensityDecayPerPixel^n > MINIMUM_BEAM_INTENSITY
    // So, n = log(MINIMUM_BEAM_INTENSITY/backscatteredReflectedIntensity) / log(surfaceReflectionIntensityDecayPerPixel)
    int numberOfIterationsToReachMinimumBackscatteredIntensity = std::min<int>(numberOfFilledPixels, floor(log(MINIMUM_BEAM_INTENSITY/backscatteredReflectedIntensity) / log(surfaceReflectionIntensityDecayPerPixel))+1);
    for(int currentPixelInFilledPixels = 0; currentPixelInFilledPixels<numberOfIterationsToReachMinimumBackscatteredIntensity; currentPixelInFilledPixels++)  
    {
      // a fraction of the attenuation is caused by backscattering, the backscattering is sensed by the transducer
      reflectedIntensity[currentPixelInFilledPixels] += backscatteredReflectedIntensity;
      backscatteredReflectedIntensity *= surfaceReflectionIntensityDecayPerPixel;
    }
  }
  // TODO: to simulate beamwidth, take into account the incidence angle and disperse the reflection on a larger area if the angle is large
}

//-----------------------------------------------------------------------------
void SpatialModel::GetLineIntersections(std::deque<LineIntersectionInfo>& lineIntersections, double* scanLineStartPoint_Reference, double* scanLineEndPoint_Reference)
{
  UpdateModelFile();

  if (this->ModelFile.empty())
  {
    // no model is defined, which means that the model is everywhere
    // add an intersection point at 0 distance, which means that the whole scanline is in this model
    LineIntersectionInfo intersectionInfo;
    intersectionInfo.Model=this;
    intersectionInfo.IntersectionIncidenceAngleDeg=0;
    intersectionInfo.IntersectionDistanceFromStartPointMm=0;    
    lineIntersections.push_back(intersectionInfo);
    return;
  }

  // non-normalized direction vector of the scanline
  double scanLineDirectionVector_Reference[4] =
  {
    scanLineEndPoint_Reference[0]-scanLineStartPoint_Reference[0],
    scanLineEndPoint_Reference[1]-scanLineStartPoint_Reference[1],
    scanLineEndPoint_Reference[2]-scanLineStartPoint_Reference[2],
    0
  };
  double scanLineDirectionVectorNorm_Reference=vtkMath::Norm(scanLineDirectionVector_Reference);
  double searchLineStartPoint_Reference[4] = {0,0,0,1};
  for (int i=0; i<3; i++)
  {
    searchLineStartPoint_Reference[i]=scanLineStartPoint_Reference[i]-this->TransducerSpatialModelMaxOverlapMm*scanLineDirectionVector_Reference[i]/scanLineDirectionVectorNorm_Reference;
  }

  vtkSmartPointer<vtkMatrix4x4> objectToModelMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
  vtkMatrix4x4::Invert(this->ModelToObjectTransform, objectToModelMatrix);
  vtkSmartPointer<vtkMatrix4x4> referenceToModelMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
  vtkMatrix4x4::Multiply4x4(objectToModelMatrix,this->ReferenceToObjectTransform,referenceToModelMatrix);

  double searchLineStartPoint_Model[4] = {0,0,0,1};
  double scanLineEndPoint_Model[4] = {0,0,0,1};
  referenceToModelMatrix->MultiplyPoint(searchLineStartPoint_Reference,searchLineStartPoint_Model);
  referenceToModelMatrix->MultiplyPoint(scanLineEndPoint_Reference,scanLineEndPoint_Model);

  vtkSmartPointer<vtkPoints> intersectionPoints_Model=vtkSmartPointer<vtkPoints>::New();
  this->ModelLocalizer->IntersectWithLine(searchLineStartPoint_Model, scanLineEndPoint_Model, 0.0, intersectionPoints_Model, NULL);

  if (intersectionPoints_Model->GetNumberOfPoints()<1)
  {
    // no intersections with this model
    return;
  }

  vtkSmartPointer<vtkMatrix4x4> modelToReferenceMatrix= vtkSmartPointer<vtkMatrix4x4>::New();
  vtkMatrix4x4::Invert(referenceToModelMatrix, modelToReferenceMatrix);

  // Measure the distance from the starting point in the reference coordinate system
  double intersectionPoint_Model[4] = {0,0,0,1};
  double intersectionPoint_Reference[4] = {0,0,0,1};
  int intersectionPointIndex=0;
  bool scanLineStartPointInsideModel=false;
  // Search for intersection points in the search line that are not part of the scanline to detect
  // potential model/transducer overlap
  for (; intersectionPointIndex<intersectionPoints_Model->GetNumberOfPoints(); intersectionPointIndex++)
  {
    intersectionPoints_Model->GetPoint(intersectionPointIndex,intersectionPoint_Model);
    modelToReferenceMatrix->MultiplyPoint(intersectionPoint_Model,intersectionPoint_Reference);
    double intersectionDistanceFromSearchLineStartPointMm = sqrt(vtkMath::Distance2BetweenPoints(searchLineStartPoint_Reference, intersectionPoint_Reference));
    if (intersectionDistanceFromSearchLineStartPointMm<=this->TransducerSpatialModelMaxOverlapMm)
    {
      // there is an intersection point in the searchline that is not part of the scanline
      scanLineStartPointInsideModel = (!scanLineStartPointInsideModel);
    }
    else
    {
      // we reached the scanline starting point
      break;
    }
  }
  LineIntersectionInfo intersectionInfo;
  intersectionInfo.Model=this;
  if (scanLineStartPointInsideModel)
  {
    // the scanline starting point is inside the model, so add an intersection point at 0 distance
    intersectionInfo.IntersectionDistanceFromStartPointMm=0;    
    lineIntersections.push_back(intersectionInfo);
  }
  for (; intersectionPointIndex<intersectionPoints_Model->GetNumberOfPoints(); intersectionPointIndex++)
  {
    intersectionPoints_Model->GetPoint(intersectionPointIndex,intersectionPoint_Model);
    modelToReferenceMatrix->MultiplyPoint(intersectionPoint_Model,intersectionPoint_Reference);
    intersectionInfo.IntersectionDistanceFromStartPointMm = sqrt(vtkMath::Distance2BetweenPoints(scanLineStartPoint_Reference, intersectionPoint_Reference));
    lineIntersections.push_back(intersectionInfo);    
    // TODO: also compute and set the intersectionInfo.IntersectionIncidenceAngleDeg
    //  It may be useful to return the full normal to make the beamwidth-related computations more accurate.    
  }

}

//-----------------------------------------------------------------------------
PlusStatus SpatialModel::UpdateModelFile()
{
  if (!this->ModelFileNeedsUpdate)
  {
    return PLUS_SUCCESS;
  }

  this->ModelFileNeedsUpdate=false;

  if (this->PolyData!=NULL)
  {
    this->PolyData->Delete();
    this->PolyData=NULL;
  }

  if (this->ModelFile.empty())
  {
    LOG_DEBUG("ModelFileName is not specified for SpatialModel "<<(this->Name.empty()?"(undefined)":this->Name)<<" it will be used as background media");
    return PLUS_SUCCESS;
  }

  std::string foundAbsoluteImagePath;
  // FindImagePath is used instead of FindModelPath, as the model is expected to be in the image directory
  // it might be more reasonable to move the model to the model directory and change this to FindModelPath
  if (vtkPlusConfig::GetInstance()->FindImagePath(this->ModelFile, foundAbsoluteImagePath) != PLUS_SUCCESS)
  {
    LOG_ERROR("Cannot find input model file "<<this->ModelFile);
    return PLUS_FAIL;
  }

  std::string fileExt=vtksys::SystemTools::GetFilenameLastExtension(foundAbsoluteImagePath);
  if (STRCASECMP(fileExt.c_str(),".stl")==0)
  {  
    vtkSmartPointer<vtkSTLReader> modelReader = vtkSmartPointer<vtkSTLReader>::New();
    modelReader->SetFileName(foundAbsoluteImagePath.c_str());
    modelReader->Update();
    this->PolyData = modelReader->GetOutput();
    this->PolyData->Register(NULL);
  }
  else //if (STRCASECMP(fileExt.c_str(),".vtp")==0)
  {
    vtkSmartPointer<vtkXMLPolyDataReader> modelReader = vtkSmartPointer<vtkXMLPolyDataReader>::New();
    modelReader->SetFileName(foundAbsoluteImagePath.c_str());
    modelReader->Update();
    this->PolyData = modelReader->GetOutput();
    this->PolyData->Register(NULL);
  }

  if (this->PolyData==NULL || this->PolyData->GetNumberOfPoints()==0)
  {
    LOG_ERROR("Model specified cannot be found: "<<foundAbsoluteImagePath);    
    return PLUS_FAIL;
  }

  this->ModelLocalizer->SetDataSet(this->PolyData); 
  this->ModelLocalizer->SetMaxLevel(24); 
  this->ModelLocalizer->SetNumberOfCellsPerNode(32);
  this->ModelLocalizer->BuildLocator();

  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
void SpatialModel::SetModelFile(const char* modelFile)
{
  std::string oldModelFile=this->ModelFile;
  this->ModelFile = modelFile?modelFile:"";
  if (this->ModelFile.compare(oldModelFile)!=0)
  {
    this->ModelFileNeedsUpdate=true;
  }
}

//-----------------------------------------------------------------------------
void SpatialModel::UpdatePrecomputedAttenuations(double intensityTransmittedFractionPerPixelTwoWay, int numberOfElements)
{
  this->PrecomputedAttenuations.resize(numberOfElements);
  double attenuation=intensityTransmittedFractionPerPixelTwoWay;  
  for (int i=0; i<numberOfElements; i++)
  {
    this->PrecomputedAttenuations[i]=attenuation;
    attenuation*=intensityTransmittedFractionPerPixelTwoWay;
  }
}
