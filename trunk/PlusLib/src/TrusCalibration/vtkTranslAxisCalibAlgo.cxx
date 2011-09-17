#include "PlusMath.h"
#include "vtkTranslAxisCalibAlgo.h"
#include "vtkObjectFactory.h"
#include "vtkTrackedFrameList.h"
#include "vtkPoints.h"

//----------------------------------------------------------------------------
vtkCxxRevisionMacro(vtkTranslAxisCalibAlgo, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkTranslAxisCalibAlgo); 

//----------------------------------------------------------------------------
vtkTranslAxisCalibAlgo::vtkTranslAxisCalibAlgo()
{
  this->TrackedFrameList = NULL; 
  this->SetTranslationAxisOrientation(0,0,1);
  this->SetSpacing(0,0); 
}

//----------------------------------------------------------------------------
vtkTranslAxisCalibAlgo::~vtkTranslAxisCalibAlgo()
{

}

//----------------------------------------------------------------------------
void vtkTranslAxisCalibAlgo::SetInput(vtkTrackedFrameList* trackedFrameList)
{
  this->SetTrackedFrameList(trackedFrameList); 
}

//----------------------------------------------------------------------------
void vtkTranslAxisCalibAlgo::Update()
{

}


//----------------------------------------------------------------------------
PlusStatus vtkTranslAxisCalibAlgo::ConstrLinEqForTransAxisCalib( std::vector<vnl_vector<double>> &aMatrix, std::vector<double> &bVector)
{
  LOG_TRACE("vtkTranslAxisCalibAlgo::ConstrLinEqForTransAxisCalib"); 
  aMatrix.clear(); 
  bVector.clear(); 

  if ( this->GetTrackedFrameList() == NULL )
  {
    LOG_ERROR("Failed to construct linear equation for translation axis calibration - tracked frame list is NULL!"); 
    return PLUS_FAIL; 
  }

  for ( int frame = 0; frame < this->GetTrackedFrameList()->GetNumberOfTrackedFrames(); ++frame )
  {
    TrackedFrame* trackedFrame = this->GetTrackedFrameList()->GetTrackedFrame(frame); 
    if ( trackedFrame = NULL ) 
    {
      LOG_ERROR("Unable to get tracked frame from the list - tracked frame is NULL (position in the list: " << frame << ")!"); 
      continue; 
    }

    // Get the encoder value in mm 
    double z(0); 
    LOG_WARNING("Not yet implemented!!!"); 
    /*switch (dataType)
    {
    case PROBE_TRANSLATION: 
      {
        double probePos(0), probeRot(0), templatePos(0); 
        if ( !vtkStepperCalibrationController::GetStepperEncoderValues(this->SegmentedFrameContainer[frame].TrackedFrameInfo, probePos, probeRot, templatePos) )
        {
          LOG_WARNING("Probe translation axis calibration: Unable to get probe position from tracked frame info for frame #" << frame); 
          continue; 
        }

        z = probePos; 
      }
      break; 
    case TEMPLATE_TRANSLATION: 
      {
        double probePos(0), probeRot(0), templatePos(0); 
        if ( !this->GetStepperEncoderValues(this->SegmentedFrameContainer[frame].TrackedFrameInfo, probePos, probeRot, templatePos) )
        {
          LOG_WARNING("Template translation axis calibration: Unable to get template position from tracked frame info for frame #" << frame); 
          continue; 
        }

        z = templatePos; 
      }

      break; 
    }*/

    if ( trackedFrame->GetFiducialPointsCoordinatePx() == NULL )
    {
      LOG_ERROR("Unable to get segmented fiducial points from tracked frame - FiducialPointsCoordinatePx is NULL (position in the list: " << frame << ")!" ); 
      continue; 
    }

    // Construct the A matrix and b vector for each side fiducials  
    for ( int i = 0; i < trackedFrame->GetFiducialPointsCoordinatePx()->GetNumberOfPoints(); ++i)
    {
      if ( ( (i+1) % 3 ) % 2 == 0 ) // wire #1,#3,#4,#6... => non moving points of the N-wire 
      {
        // Wire X coordinate in mm (#1,#3,#4,#6)
        double bX = trackedFrame->GetFiducialPointsCoordinatePx()->GetPoint(i)[0] * this->GetSpacing()[0]; 

        // Wire Y coordinate in mm (#1,#3,#4,#6)
        double bY = trackedFrame->GetFiducialPointsCoordinatePx()->GetPoint(i)[1] * this->GetSpacing()[1]; 

        bVector.push_back(bX); 
        bVector.push_back(bY); 

        // We're using 2 out of 3 wires per each N-wire => ( 2 * numberOfNFiduacials )
        // We're using the X and Y coordinates => need to multiply by 2 
        // we have two unknown variables 
        // ( 2 * numberOfNFiduacials ) * 2 + 2 = 2 point out 
        
        vnl_vector<double> a1(10,0); // TODO: it works only with double N phantoms 
        a1.put(0, z);  // raw encoder value in mm
        a1.put(2, 1); 

        vnl_vector<double> a2(10,0); // TODO: it works only with double N phantoms 
        a2.put(1, z); // raw encoder value in mm
        a2.put(3, 1); 

        aMatrix.push_back(a1); 
        aMatrix.push_back(a2); 
      }
    } // end of fiduacials 

  } // end of frames 
}

