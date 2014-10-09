/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#include "PlusConfigure.h"
#include "PlusXmlUtils.h"
#include "PlusMath.h"

#include "vtkLandmarkDetectionAlgo.h"

#include "vtkMatrix4x4.h"

vtkStandardNewMacro(vtkLandmarkDetectionAlgo);

//-----------------------------------------------------------------------------
// Default algorithm parameters
static const double PERCENTAGE_WINDOWS_SKIP = 0.1;//The first windows detected as pivoting wont be used for averaging.

//-----------------------------------------------------------------------------
vtkLandmarkDetectionAlgo::vtkLandmarkDetectionAlgo()
{
  this->DetectedLandmarkPoints_Reference = NULL;
  this->SetDetectedLandmarkPoints_Reference(vtkSmartPointer<vtkPoints>::New());

  this->AcquisitionRate=20.0;
  this->FilterWindowTimeSec=0.2;
  this->DetectionTimeSec=1.0;
  this->StylusShaftMinimumDisplacementThresholdMm = 30;
  this->StylusTipMaximumDisplacementThresholdMm = 1.5;
  this->MinimunDistanceBetweenLandmarksMm=15.0;
}

//-----------------------------------------------------------------------------
vtkLandmarkDetectionAlgo::~vtkLandmarkDetectionAlgo()
{
  this->StylusTipToReferenceTransformsDeque.clear();
  this->SetDetectedLandmarkPoints_Reference(NULL);
}

//----------------------------------------------------------------------------
PlusStatus vtkLandmarkDetectionAlgo::SetAcquisitionRate(double aquisitionRateSamplePerSec)
{
  if(aquisitionRateSamplePerSec<=0)
  {
    LOG_ERROR("Specified acquisition rate "<< aquisitionRateSamplePerSec << "[SamplePerSec]is not positive");
    return PLUS_FAIL;
  }
  this->AcquisitionRate=aquisitionRateSamplePerSec;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkLandmarkDetectionAlgo::SetDetectionTimeSec(double detectionTimeSec)
{
  if(detectionTimeSec<=0)
  {
    LOG_ERROR("Specified detection time (" << detectionTimeSec<<" [s]) is not positive");
    return PLUS_FAIL;
  }
  this->DetectionTimeSec=detectionTimeSec;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkLandmarkDetectionAlgo::SetFilterWindowTimeSec(double filterWindowTimeSec)
{
  if(filterWindowTimeSec<=0)
  {
    LOG_ERROR("Specified window time (" << filterWindowTimeSec << " [s]) is not positive");
    return PLUS_FAIL;
  }
  this->FilterWindowTimeSec=filterWindowTimeSec;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkLandmarkDetectionAlgo::SetStylusShaftMinimumDisplacementThresholdMm(double stylusShaftMinimumDisplacementThresholdMm)
{
  if(stylusShaftMinimumDisplacementThresholdMm<0)
  {
    LOG_ERROR("Specified stylusShaftMinimumDisplacementThreshold (" << stylusShaftMinimumDisplacementThresholdMm << " [mm]) is negative");
    return PLUS_FAIL;
  }
  this->StylusShaftMinimumDisplacementThresholdMm=stylusShaftMinimumDisplacementThresholdMm;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkLandmarkDetectionAlgo::SetStylusTipMaximumDisplacementThresholdMm(double stylusTipMaximumMotionThresholdMm)
{
  if(stylusTipMaximumMotionThresholdMm<0)
  {
    LOG_ERROR("Specified StylusTipMaximumDisplacementThreshold (" << stylusTipMaximumMotionThresholdMm << " [mm]) is negative");
    return PLUS_FAIL;
  }
  this->StylusTipMaximumDisplacementThresholdMm=stylusTipMaximumMotionThresholdMm;
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkLandmarkDetectionAlgo::ComputeFilterWindowSize(int & filterWindowSize)
{
  if(this->AcquisitionRate<=0)
  {
    LOG_ERROR("Specified acquisition rate is not positive");
    return PLUS_FAIL;
  }
  filterWindowSize=PlusMath::Round(this->AcquisitionRate*this->FilterWindowTimeSec);
  if(filterWindowSize<1)
  {
    LOG_WARNING("The smallest window size is set to 1");
    filterWindowSize=1;
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkLandmarkDetectionAlgo::ComputeNumberOfWindows(int & numberOfWindows)
{
  if(this->FilterWindowTimeSec<=0&&this->DetectionTimeSec<=this->FilterWindowTimeSec)
  {
    LOG_ERROR("Detection or filter window times are not correct!");
    return PLUS_FAIL;
  }
  numberOfWindows=this->DetectionTimeSec/this->FilterWindowTimeSec;
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkLandmarkDetectionAlgo::ResetDetection()
{
  LOG_DEBUG("Reset");
  this->StylusTipToReferenceTransformsDeque.clear();
  this->DetectedLandmarkPoints_Reference->Reset();
  //this->DetectedLandmarkPoints_Reference->Initialize();
  return PLUS_SUCCESS;
} 

//----------------------------------------------------------------------------
PlusStatus vtkLandmarkDetectionAlgo::DeleteLastLandmark()
{
  if(this->DetectedLandmarkPoints_Reference->GetNumberOfPoints()<=0)
  {
    LOG_ERROR("There were no landmark detected");
    return PLUS_FAIL;
  }
  this->DetectedLandmarkPoints_Reference->GetData()->RemoveTuple(this->DetectedLandmarkPoints_Reference->GetNumberOfPoints()-1);
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkLandmarkDetectionAlgo::InsertLandmark_Reference(double stylusTipPosition_Reference[4])
{
  if(GetNearExistingLandmarkId(stylusTipPosition_Reference)!=-1)
  {
    LOG_ERROR("It was attempted to insert a landmark where there is already one detected");
    return PLUS_FAIL;
  }
  this->DetectedLandmarkPoints_Reference->InsertNextPoint(stylusTipPosition_Reference);
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkLandmarkDetectionAlgo::FilterStylusTipPositionsWindow(double stylusTipFiltered_Reference[4])
{
  int filterWindowSize=0;
  if (ComputeFilterWindowSize(filterWindowSize)==PLUS_FAIL)
  {
    return PLUS_FAIL;
  }

  if(this->StylusTipToReferenceTransformsDeque.size()<filterWindowSize)
  {
    LOG_ERROR("There are not enough stylus tip positions acquired yet");
    return PLUS_FAIL;
  }

  std::deque< vtkSmartPointer<vtkMatrix4x4> >::iterator stylusTipToReferenceTransformIt=this->StylusTipToReferenceTransformsDeque.end();
  double stylusTipPositionSum_Reference[4] = {0,0,0,1};
  for(int i=0;i<filterWindowSize;i++)
  {
    stylusTipToReferenceTransformIt--;
    stylusTipPositionSum_Reference[0] +=(*stylusTipToReferenceTransformIt)->Element[0][3];
    stylusTipPositionSum_Reference[1] +=(*stylusTipToReferenceTransformIt)->Element[1][3];
    stylusTipPositionSum_Reference[2] +=(*stylusTipToReferenceTransformIt)->Element[2][3];
  }
  //it is checked to be at least one in the ComputeFilterWindowSize I think this is redundant
  if(filterWindowSize==0)
  {
    LOG_ERROR("Filter window size is zero can not be used, how does it happened!");
    return PLUS_FAIL;
  }
  stylusTipFiltered_Reference[0]=stylusTipPositionSum_Reference[0]/filterWindowSize;
  stylusTipFiltered_Reference[1]=stylusTipPositionSum_Reference[1]/filterWindowSize;
  stylusTipFiltered_Reference[2]=stylusTipPositionSum_Reference[2]/filterWindowSize;
  stylusTipFiltered_Reference[3]=1.0;
    return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkLandmarkDetectionAlgo::KeepLastWindow()
{
  std::deque< vtkSmartPointer<vtkMatrix4x4> >::iterator stylusTipToReferenceTransformIt=this->StylusTipToReferenceTransformsDeque.begin();
  int dequeSize =this->StylusTipToReferenceTransformsDeque.size();
  int filterWindowSize=0;
  if (ComputeFilterWindowSize(filterWindowSize)==PLUS_FAIL)
  {
    return PLUS_FAIL;
  }
  if(dequeSize>filterWindowSize)
  {
    int j=0;
    for(int i=0;i<dequeSize-filterWindowSize;i++)
    {
      this->StylusTipToReferenceTransformsDeque.pop_front();
      j++;
    }
  }
  //DEBUG
  for (std::deque< vtkSmartPointer<vtkMatrix4x4> >::iterator markerToReferenceTransformIt=this->StylusTipToReferenceTransformsDeque.begin();
    markerToReferenceTransformIt!=this->StylusTipToReferenceTransformsDeque.end(); ++markerToReferenceTransformIt)
  {
    LOG_DEBUG( "\n P( " << -(*markerToReferenceTransformIt)->Element[0][3]<<", "<< -(*markerToReferenceTransformIt)->Element[1][3]<<", "<< -(*markerToReferenceTransformIt)->Element[2][3]<<") ");
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkLandmarkDetectionAlgo::InsertNextStylusTipToReferenceTransform(vtkSmartPointer<vtkMatrix4x4> stylusTipToReferenceTransform, int &newLandmarkDetected)
{
  int numberOfLandmarksBefore = this->DetectedLandmarkPoints_Reference->GetNumberOfPoints();
  //Point 10 cm above the stylus tip, if it moves(window change bigger than AboveLandmarkThresholdMm) while the tip is static (window change smaller than LandmarkThresholdMm then it is landmark point.
  double StylusShaftPoint_StylusTip[4]={100,0,0,1};
  double StylusShaftPoint_Reference[4]={0,0,0,1};
  stylusTipToReferenceTransform->MultiplyPoint(StylusShaftPoint_StylusTip, StylusShaftPoint_Reference);
  this->StylusTipToReferenceTransformsDeque.push_back(stylusTipToReferenceTransform);

  int filterWindowSize=0;
  int numberOfWindows=0;
  if (ComputeFilterWindowSize(filterWindowSize)==PLUS_FAIL || ComputeNumberOfWindows(numberOfWindows)==PLUS_FAIL)
  {
    return PLUS_FAIL;
  }

  int modulus =this->StylusTipToReferenceTransformsDeque.size()%(filterWindowSize);
  if(modulus==0)
  {
    modulus=3;
  }
  if(modulus>=filterWindowSize)
  {
    double stylusTipFiltered_Reference[4]={0,0,0,1};
    if (FilterStylusTipPositionsWindow( stylusTipFiltered_Reference)!=PLUS_SUCCESS)
    {
      return PLUS_FAIL;
    }
    std::vector<double> stylusTipFilteredVector_Reference (stylusTipFiltered_Reference, stylusTipFiltered_Reference+sizeof(stylusTipFiltered_Reference)/sizeof(stylusTipFiltered_Reference[0]));

    int numberFilteredWindows =PlusMath::Floor(this->StylusTipToReferenceTransformsDeque.size()/filterWindowSize);
    if(numberFilteredWindows>1)
    {
      this->StylusShaftPathBoundingBox.AddPoint(StylusShaftPoint_Reference);
      this->StylusTipPathBoundingBox.AddPoint(stylusTipFiltered_Reference);
      double lengthsTip[3];
      this->StylusTipPathBoundingBox.GetLengths(lengthsTip);
      if( vtkMath::Norm(lengthsTip)<this->StylusTipMaximumDisplacementThresholdMm)
      {
        LOG_DEBUG("Window Landmark ("<< stylusTipFiltered_Reference[0]<< ", "<< stylusTipFiltered_Reference[1]<< ", "<< stylusTipFiltered_Reference[2]<< ") found keep going!!");
        //DEBUG
        int i =0;
        for (std::deque< vtkSmartPointer<vtkMatrix4x4> >::iterator markerToReferenceTransformIt=this->StylusTipToReferenceTransformsDeque.begin();
          markerToReferenceTransformIt!=this->StylusTipToReferenceTransformsDeque.end(); ++markerToReferenceTransformIt)
        {
          if(i%filterWindowSize==0)
          {
            LOG_DEBUG( "\n");
          }
          LOG_DEBUG("\n P( " << -(*markerToReferenceTransformIt)->Element[0][3]<<", " << -(*markerToReferenceTransformIt)->Element[1][3]<<", " << -(*markerToReferenceTransformIt)->Element[2][3]<<")");
          i++;
        }
      }
      else
      {
        LOG_DEBUG("\nStylusTipBoundingBox norm = " <<vtkMath::Norm(lengthsTip)<<"\n");
        KeepLastWindow();

        this->StylusShaftPathBoundingBox.Reset();
        this->StylusShaftPathBoundingBox.AddPoint(StylusShaftPoint_Reference);
        this->StylusTipPathBoundingBox.Reset();
        this->StylusTipPathBoundingBox.AddPoint(stylusTipFiltered_Reference);
      }
    }
    double lengths[3];
    this->StylusShaftPathBoundingBox.GetLengths(lengths);
    if(numberFilteredWindows >=numberOfWindows)
    {
      this->StylusTipPathBoundingBox.Reset();
      this->StylusShaftPathBoundingBox.Reset();
      if((lengths[0]+lengths[1]+lengths[2])>this->StylusShaftMinimumDisplacementThresholdMm)
      {
        EstimateLandmarkPosition();
      }
      else
      {
        this->StylusShaftPathBoundingBox.AddPoint(StylusShaftPoint_Reference);
        this->StylusTipPathBoundingBox.AddPoint(stylusTipFiltered_Reference);
        KeepLastWindow();
      }
    }
  }
  if(numberOfLandmarksBefore != this->DetectedLandmarkPoints_Reference->GetNumberOfPoints())
  {
    newLandmarkDetected = this->DetectedLandmarkPoints_Reference->GetNumberOfPoints();
  }
  else
  {
    newLandmarkDetected=-1;
  }
  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
int vtkLandmarkDetectionAlgo::GetNearExistingLandmarkId(double stylusTipPosition_Reference[4])
{
  double detectedLandmark_Reference[4] = {0,0,0,1};
  double landmarkDifference_Reference[4] = {0,0,0,0};
  for(int id=0; id<this->DetectedLandmarkPoints_Reference->GetNumberOfPoints();id++)
  {
    this->DetectedLandmarkPoints_Reference->GetPoint(id, detectedLandmark_Reference);
    landmarkDifference_Reference[0]=detectedLandmark_Reference[0]-stylusTipPosition_Reference[0];
    landmarkDifference_Reference[1]=detectedLandmark_Reference[1]-stylusTipPosition_Reference[1];
    landmarkDifference_Reference[2]=detectedLandmark_Reference[2]-stylusTipPosition_Reference[2];
    landmarkDifference_Reference[3]=detectedLandmark_Reference[3]-stylusTipPosition_Reference[3];
    if(vtkMath::Norm(landmarkDifference_Reference)<this->MinimunDistanceBetweenLandmarksMm/3)
    {
      return id;
    }
  }
  return -1;
}

//----------------------------------------------------------------------------
PlusStatus vtkLandmarkDetectionAlgo::EstimateLandmarkPosition()
{
  int i=0; int j=0;
  std::vector<double> values[3];
  int filterWindowSize=0;
  int numberOfWindows=0;
  if (ComputeFilterWindowSize(filterWindowSize)==PLUS_FAIL ||ComputeNumberOfWindows(numberOfWindows)==PLUS_FAIL)
  {
    return PLUS_FAIL;
  }
  int numberOfWindowsSkip=  filterWindowSize=PlusMath::Round(numberOfWindows*PERCENTAGE_WINDOWS_SKIP);
  if(filterWindowSize<1)
  {
    LOG_WARNING("The smallest number of windows to skip is set to 1");
    filterWindowSize=1;
  }

  for (std::deque< vtkSmartPointer<vtkMatrix4x4> >::iterator stylusTipToReferenceTransformIt=this->StylusTipToReferenceTransformsDeque.begin();
    stylusTipToReferenceTransformIt!=this->StylusTipToReferenceTransformsDeque.end(); ++stylusTipToReferenceTransformIt)
  {
    //Don't use first numberOfWindowsSkip windows
    if(i>=(filterWindowSize*numberOfWindowsSkip) && i <(numberOfWindows)*filterWindowSize)
    {
      values[0].push_back( (*stylusTipToReferenceTransformIt)->Element[0][3] );
      values[1].push_back( (*stylusTipToReferenceTransformIt)->Element[1][3] );
      values[2].push_back( (*stylusTipToReferenceTransformIt)->Element[2][3] );
      j++;
    }
    i++;
  }

  if(j==(numberOfWindows-numberOfWindowsSkip)*filterWindowSize)
  {
    double stylusPositionMean_Reference[4] = {0,0,0,1};
    double stylusPositionStdev_Reference[4] = {0,0,0,0};
    for ( int r = 0; r < 3; ++ r )
    {
      PlusMath::ComputeMeanAndStdev( values[r], stylusPositionMean_Reference[r], stylusPositionStdev_Reference[r]);
    }
    if(GetNearExistingLandmarkId(stylusPositionMean_Reference)==-1)
    {
      this->DetectedLandmarkPoints_Reference->InsertNextPoint(stylusPositionMean_Reference);
      LOG_INFO("\nSTD deviation ( " << stylusPositionMean_Reference[0]<< ", "<< stylusPositionMean_Reference[1]<< ", "<< stylusPositionMean_Reference[2]<< ", "<< stylusPositionMean_Reference[3]<< ") " );
      LOG_DEBUG("\nSTD deviation ( " << stylusPositionStdev_Reference[0]<< ", "<< stylusPositionStdev_Reference[1]<< ", "<< stylusPositionStdev_Reference[2]<< ") " );
      LOG_DEBUG("STD deviation magnitude " << vtkMath::Norm(stylusPositionStdev_Reference));
    }
    this->StylusTipToReferenceTransformsDeque.clear();
  }
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
std::string vtkLandmarkDetectionAlgo::GetDetectedLandmarksString( double aPrecision/*=3*/ )
{
  if (this->DetectedLandmarkPoints_Reference->GetNumberOfPoints()>0)
  {
    std::ostrstream s;
    double landmarkDetected_Reference[4] = {0,0,0,1};
    s << std::fixed << std::setprecision(aPrecision);
    for (int id =0; id<this->DetectedLandmarkPoints_Reference->GetNumberOfPoints();id++)
    {
      this->DetectedLandmarkPoints_Reference->GetPoint(id, landmarkDetected_Reference);
      s <<"\nLandmark "<< id+1 << " (" << landmarkDetected_Reference[0]<<", " << landmarkDetected_Reference[1]<<", " << landmarkDetected_Reference[2]<<")";
    }
    s << std::ends;  
    return s.str();
  }
  else
  {
    return "\nNo landmarks found";
  }
}

//-----------------------------------------------------------------------------
PlusStatus vtkLandmarkDetectionAlgo::ReadConfiguration(vtkXMLDataElement* aConfig)
{
  XML_FIND_NESTED_ELEMENT_REQUIRED(PhantomLandmarkLandmarkDetectionElement, aConfig, "vtkPhantomLandmarkRegistrationAlgo");
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(double, AcquisitionRate, PhantomLandmarkLandmarkDetectionElement);

  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(double, FilterWindowTimeSec, PhantomLandmarkLandmarkDetectionElement);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(double, DetectionTimeSec, PhantomLandmarkLandmarkDetectionElement);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(double, StylusShaftMinimumDisplacementThresholdMm, PhantomLandmarkLandmarkDetectionElement);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(double, StylusTipMaximumDisplacementThresholdMm, PhantomLandmarkLandmarkDetectionElement);

  int filterWindowSize=0;
  int numberOfWindows=0;
  if (ComputeFilterWindowSize(filterWindowSize)==PLUS_FAIL || ComputeNumberOfWindows(numberOfWindows)==PLUS_FAIL)
  {
    return PLUS_FAIL;
  }

  LOG_DEBUG("AcquisitionRate = "<< AcquisitionRate << "[fps] WindowTimeSec = " << FilterWindowTimeSec<<"[s] DetectionTimeSec = "<< DetectionTimeSec <<"[s]");
  LOG_DEBUG("NumberOfWindows = "<< numberOfWindows<< " WindowSize = "<< filterWindowSize<< " MinimunDistanceBetweenLandmarksMm = "<< MinimunDistanceBetweenLandmarksMm << "[mm] LandmarkThreshold " << StylusTipMaximumDisplacementThresholdMm <<"[mm]");

  return PLUS_SUCCESS;
}



