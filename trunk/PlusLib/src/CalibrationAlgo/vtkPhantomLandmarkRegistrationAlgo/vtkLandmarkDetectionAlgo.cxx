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

//----------------------------------------------------------------------------
void vtkLandmarkDetectionAlgo::GetFilterWindowSize(int & filterWindowSize)
{
  if(this->AcquisitionRate<=0)
  {
    LOG_ERROR("Specified acquisition rate is not positive");
  }
  filterWindowSize=PlusMath::Round(this->AcquisitionRate*this->FilterWindowTimeSec);
  if(filterWindowSize<1)
  {
    LOG_WARNING("The smallest window size is set to 1");
    filterWindowSize=1;
  }
}

//----------------------------------------------------------------------------
void vtkLandmarkDetectionAlgo::SetAcquisitionRate(double aquisitionRateSamplePerSec)
{
  if(aquisitionRateSamplePerSec<=0)
  {
    LOG_ERROR("Specified acquisition rate "<< aquisitionRateSamplePerSec << "[SamplePerSec]is not positive");
  }
  this->AcquisitionRate=aquisitionRateSamplePerSec;
}

//----------------------------------------------------------------------------
void vtkLandmarkDetectionAlgo::SetDetectionTimeSec(double detectionTimeSec)
{
  if(detectionTimeSec<0 || detectionTimeSec<this->FilterWindowTimeSec)
  {
    LOG_ERROR("Specified detection time (" << detectionTimeSec<<" [s]) is smaller than filter window time");
  }
  this->DetectionTimeSec=detectionTimeSec;
}

//----------------------------------------------------------------------------
void vtkLandmarkDetectionAlgo::SetFilterWindowTimeSec(double filterWindowTimeSec)
{
  if(filterWindowTimeSec<0 || filterWindowTimeSec>this->DetectionTimeSec)
  {
    LOG_ERROR("Specified window time (" << filterWindowTimeSec << " [s]) is bigger than detection time");
    return;
  }
  this->FilterWindowTimeSec=filterWindowTimeSec;
}

//----------------------------------------------------------------------------
void vtkLandmarkDetectionAlgo::SetStylusShaftMinimumDisplacementThresholdMm(double stylusShaftMinimumDisplacementThresholdMm)
{
  if(stylusShaftMinimumDisplacementThresholdMm<0)
  {
    LOG_ERROR("Specified stylusShaftMinimumDisplacementThreshold (" << stylusShaftMinimumDisplacementThresholdMm << " [mm]) is negative");
    return;
  }
    this->StylusShaftMinimumDisplacementThresholdMm=stylusShaftMinimumDisplacementThresholdMm;
}

//----------------------------------------------------------------------------
void vtkLandmarkDetectionAlgo::SetStylusTipMaximumDisplacementThresholdMm(double stylusTipMaximumMotionThresholdMm)
{
  if(stylusTipMaximumMotionThresholdMm<0)
  {
    LOG_ERROR("Specified StylusTipMaximumDisplacementThreshold (" << stylusTipMaximumMotionThresholdMm << " [mm]) is negative");
    return;
  }
    this->StylusTipMaximumDisplacementThresholdMm=stylusTipMaximumMotionThresholdMm;
}

//----------------------------------------------------------------------------
void vtkLandmarkDetectionAlgo::SetNumberOfExpectedLandmarks(int numberOfExpectedLandmarks)
{
  if(numberOfExpectedLandmarks<0)
  {
    LOG_ERROR("Specified NumberOfExpectedLandmarks (" << numberOfExpectedLandmarks << " [s]) is negative");
    return;
  }
    this->NumberOfExpectedLandmarks=numberOfExpectedLandmarks;
}

//-----------------------------------------------------------------------------
vtkLandmarkDetectionAlgo::vtkLandmarkDetectionAlgo()
{
  this->ReferenceCoordinateFrame = NULL;
  this->DetectedLandmarkPoints_Reference = NULL;
  this->SetDetectedLandmarkPoints_Reference(vtkSmartPointer<vtkPoints>::New());

  this->NewLandmarkFound=false;
  this->AcquisitionRate=20.0;
  this->FilterWindowTimeSec=0.2;
  this->DetectionTimeSec=1.0;
  this->StylusShaftMinimumDisplacementThresholdMm = 30;
  this->StylusTipMaximumDisplacementThresholdMm = 1.5;
  this->NumberOfExpectedLandmarks=3;
  this->MinimunDistanceBetweenLandmarksMm=15.0;
}

//-----------------------------------------------------------------------------
vtkLandmarkDetectionAlgo::~vtkLandmarkDetectionAlgo()
{
  this->StylusTipToReferenceTransformsDeque.clear();
  this->ReferenceCoordinateFrame = NULL;
  this->SetDetectedLandmarkPoints_Reference(NULL);
}

//-----------------------------------------------------------------------------
PlusStatus vtkLandmarkDetectionAlgo::ResetDetection()
{
  this->NewLandmarkFound=false;
  LOG_INFO("Reset");
  this->StylusTipToReferenceTransformsDeque.clear();
  this->DetectedLandmarkPoints_Reference->Reset();
  this->DetectedLandmarkPoints_Reference->Initialize();
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkLandmarkDetectionAlgo::DeleteLastLandmark()
{
  this->NewLandmarkFound=false;
  if(this->DetectedLandmarkPoints_Reference->GetNumberOfPoints()>0)
  {
    this->DetectedLandmarkPoints_Reference->GetData()->RemoveTuple(this->DetectedLandmarkPoints_Reference->GetNumberOfPoints()-1);
    return PLUS_SUCCESS;
  }
  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkLandmarkDetectionAlgo::InsertLandmark_Reference(double* stylusTipPosition_Reference)
{
  if(GetNearExistingLandmarkId(stylusTipPosition_Reference)==-1)
  {
    this->DetectedLandmarkPoints_Reference->InsertNextPoint(stylusTipPosition_Reference);
    return PLUS_SUCCESS;
  }
  return PLUS_FAIL;
}

//----------------------------------------------------------------------------
PlusStatus vtkLandmarkDetectionAlgo::FilterStylusTipPositionsWindow(double* stylusTipFiltered_Reference)
{
  double stylusTipPositionSum_Reference [4] = {0,0,0,1};
  int filterWindowSize=0;
  GetFilterWindowSize(filterWindowSize);
  std::deque< vtkSmartPointer<vtkMatrix4x4> >::iterator stylusTipToReferenceTransformIt=this->StylusTipToReferenceTransformsDeque.end();

  if(this->StylusTipToReferenceTransformsDeque.size()>=filterWindowSize)
  {
    for(int i=0;i<filterWindowSize;i++)
    {
      stylusTipToReferenceTransformIt--;
      stylusTipPositionSum_Reference[0] +=(*stylusTipToReferenceTransformIt)->Element[0][3];
      stylusTipPositionSum_Reference[1] +=(*stylusTipToReferenceTransformIt)->Element[1][3];
      stylusTipPositionSum_Reference[2] +=(*stylusTipToReferenceTransformIt)->Element[2][3];
    }
    stylusTipFiltered_Reference[0]=stylusTipPositionSum_Reference[0]/filterWindowSize;
    stylusTipFiltered_Reference[1]=stylusTipPositionSum_Reference[1]/filterWindowSize;
    stylusTipFiltered_Reference[2]=stylusTipPositionSum_Reference[2]/filterWindowSize;
  }
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
void vtkLandmarkDetectionAlgo::KeepLastWindow()
{
  std::deque< vtkSmartPointer<vtkMatrix4x4> >::iterator stylusTipToReferenceTransformIt=this->StylusTipToReferenceTransformsDeque.begin();
  int dequeSize =this->StylusTipToReferenceTransformsDeque.size();
  int filterWindowSize=0;
  GetFilterWindowSize(filterWindowSize);
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
}

//----------------------------------------------------------------------------
PlusStatus vtkLandmarkDetectionAlgo::InsertNextStylusTipToReferenceTransform(vtkSmartPointer<vtkMatrix4x4> stylusTipToReferenceTransform, bool &newLandmarkDetected)
{
  //Point 10 cm above the stylus tip, if it moves(window change bigger than AboveLandmarkThresholdMm) while the tip is static (window change smaller than LandmarkThresholdMm then it is landmark point.
  double StylusShaftPoint_StylusTip[4]={100,0,0,1};
  double StylusShaftPoint_Reference[4]={0,0,0,1};
  stylusTipToReferenceTransform->MultiplyPoint(StylusShaftPoint_StylusTip, StylusShaftPoint_Reference);
  int filterWindowSize=0;
  int numberOfWindows=this->DetectionTimeSec/this->FilterWindowTimeSec;
  GetFilterWindowSize(filterWindowSize);
  this->StylusTipToReferenceTransformsDeque.push_back(stylusTipToReferenceTransform);
  int modulus =this->StylusTipToReferenceTransformsDeque.size()%(filterWindowSize);
  if(modulus==0)
  {
    modulus=3;
  }
  if(modulus>=filterWindowSize &&  this->DetectedLandmarkPoints_Reference->GetNumberOfPoints() < this->NumberOfExpectedLandmarks)
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
  IsNewLandmarkPointFound(newLandmarkDetected);
  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
int vtkLandmarkDetectionAlgo::GetNearExistingLandmarkId(double* stylusTipPosition_Reference)
{
  double detectedLandmark_Reference[4] = {0,0,0,1};
  double landmarkDifference_Reference[4] = {0,0,0,1};
  for(int id=0; id<this->DetectedLandmarkPoints_Reference->GetNumberOfPoints();id++)
  {
    this->DetectedLandmarkPoints_Reference->GetPoint(id, detectedLandmark_Reference);
    landmarkDifference_Reference[0]=detectedLandmark_Reference[0]-stylusTipPosition_Reference[0];
    landmarkDifference_Reference[1]=detectedLandmark_Reference[1]-stylusTipPosition_Reference[1];
    landmarkDifference_Reference[2]=detectedLandmark_Reference[2]-stylusTipPosition_Reference[2];
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
  int numberOfWindows=this->DetectionTimeSec/this->FilterWindowTimeSec;
  int numberOfWindowsSkip=  filterWindowSize=PlusMath::Round(numberOfWindows*PERCENTAGE_WINDOWS_SKIP);
  if(filterWindowSize<1)
  {
    LOG_WARNING("The smallest number of windows to skip is set to 1");
    filterWindowSize=1;
  }

  GetFilterWindowSize(filterWindowSize);
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
      LOG_DEBUG("\nSTD deviation ( " << stylusPositionStdev_Reference[0]<< ", "<< stylusPositionStdev_Reference[1]<< ", "<< stylusPositionStdev_Reference[2]<< ") " );
      LOG_DEBUG("STD deviation magnitude " << vtkMath::Norm(stylusPositionStdev_Reference));
      this->NewLandmarkFound=true;
    }
    this->StylusTipToReferenceTransformsDeque.clear();
  }
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkLandmarkDetectionAlgo::IsNewLandmarkPointFound(bool &found)
{
  found=this->NewLandmarkFound;
  if (this->NewLandmarkFound==true)
  {
    this->NewLandmarkFound=false;
  }
  return PLUS_SUCCESS;
}

//-----------------------------------------------------------------------------
PlusStatus vtkLandmarkDetectionAlgo::IsLandmarkDetectionCompleted(bool &completed)
{
  if(this->DetectedLandmarkPoints_Reference->GetNumberOfPoints()>=this->NumberOfExpectedLandmarks)
  {
    completed=true;
  }
  else
  {
    completed=false;
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
  XML_READ_STRING_ATTRIBUTE_REQUIRED(ReferenceCoordinateFrame, PhantomLandmarkLandmarkDetectionElement);

  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(double, AcquisitionRate, PhantomLandmarkLandmarkDetectionElement);

  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(int, NumberOfExpectedLandmarks, PhantomLandmarkLandmarkDetectionElement);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(double, FilterWindowTimeSec, PhantomLandmarkLandmarkDetectionElement);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(double, DetectionTimeSec, PhantomLandmarkLandmarkDetectionElement);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(double, StylusShaftMinimumDisplacementThresholdMm, PhantomLandmarkLandmarkDetectionElement);
  XML_READ_SCALAR_ATTRIBUTE_OPTIONAL(double, StylusTipMaximumDisplacementThresholdMm, PhantomLandmarkLandmarkDetectionElement);

  int filterWindowSize=0;
  int numberOfWindows=this->DetectionTimeSec/this->FilterWindowTimeSec;
  GetFilterWindowSize(filterWindowSize);

  LOG_DEBUG("AcquisitionRate = "<< AcquisitionRate << "[fps] WindowTimeSec = " << FilterWindowTimeSec<<"[s] DetectionTimeSec = "<< DetectionTimeSec <<"[s]");
  LOG_DEBUG("NumberOfWindows = "<< numberOfWindows<< " WindowSize = "<< filterWindowSize<< " MinimunDistanceBetweenLandmarksMm = "<< MinimunDistanceBetweenLandmarksMm << "[mm] LandmarkThreshold " << StylusTipMaximumDisplacementThresholdMm <<"[mm]");

  return PLUS_SUCCESS;
}



