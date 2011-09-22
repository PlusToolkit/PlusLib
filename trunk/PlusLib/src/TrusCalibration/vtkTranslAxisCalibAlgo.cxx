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
  this->SetDataType(UNKNOWN_DATA); 
}

//----------------------------------------------------------------------------
vtkTranslAxisCalibAlgo::~vtkTranslAxisCalibAlgo()
{

}

//----------------------------------------------------------------------------
void vtkTranslAxisCalibAlgo::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}


//----------------------------------------------------------------------------
void vtkTranslAxisCalibAlgo::SetInput(vtkTrackedFrameList* trackedFrameList)
{
  this->SetTrackedFrameList(trackedFrameList); 
}

 
//----------------------------------------------------------------------------
double * vtkTranslAxisCalibAlgo::GetOutput()
{
  return this->TranslationAxisOrientation; 
}

//----------------------------------------------------------------------------
void vtkTranslAxisCalibAlgo::Update()
{

   LOG_TRACE("vtkTranslAxisCalibAlgo::Update"); 

  // Construct linear equations Ax = b, where A is a matrix with m rows and 
  // n columns, b is an m-vector. 
  std::vector<vnl_vector<double>> aMatrix;
  std::vector<double> bVector; 
  int numberOfUnknowns = 0; 

  // Construct linear equation for translation axis calibration
  if ( this->ConstrLinEqForTransAxisCalib(aMatrix, bVector, numberOfUnknowns) != PLUS_SUCCESS )
  {
    LOG_ERROR("Unable to contruct linear equations for translation axis calibration!"); 
    return; 
  }

  if ( aMatrix.size() == 0 || bVector.size() == 0 || numberOfUnknowns == 0)
  {
    LOG_ERROR("Translation axis calibration failed, no data found!"); 
    return; 
  }

  // [tx, ty, w1x0, w1y0, w3x0, w3y0, w4x0, w4y0, w6x0, w6y0... ]
  vnl_vector<double> translationAxisCalibResult(numberOfUnknowns, 0);
  if ( PlusMath::LSQRMinimize(aMatrix, bVector, translationAxisCalibResult) != PLUS_SUCCESS)
  {
    LOG_WARNING("Failed to run LSQRMinimize!"); 
  }

  if ( translationAxisCalibResult.empty() )
  {
    LOG_ERROR("Unable to calibrate translation axis! Minimizer returned empty result."); 
    return; 
  }

  // Set translation axis orientation 
  // NOTE: If the probe goes down the wires goes down on the MF oriented image 
  // => we need to change the sign of the axis to compensate it
  this->SetTranslationAxisOrientation(-translationAxisCalibResult[0], translationAxisCalibResult[1], 1); 

  return; 
}


//----------------------------------------------------------------------------
PlusStatus vtkTranslAxisCalibAlgo::ConstrLinEqForTransAxisCalib( std::vector<vnl_vector<double>> &aMatrix, std::vector<double> &bVector, int& numberOfUnknowns)
{
  LOG_TRACE("vtkTranslAxisCalibAlgo::ConstrLinEqForTransAxisCalib"); 
  aMatrix.clear(); 
  bVector.clear(); 
  numberOfUnknowns = 0; 

  if ( this->GetTrackedFrameList() == NULL )
  {
    LOG_ERROR("Failed to construct linear equation for translation axis calibration - tracked frame list is NULL!"); 
    return PLUS_FAIL; 
  }

  for ( int frame = 0; frame < this->GetTrackedFrameList()->GetNumberOfTrackedFrames(); ++frame )
  {
    TrackedFrame* trackedFrame = this->GetTrackedFrameList()->GetTrackedFrame(frame); 
    if ( trackedFrame == NULL ) 
    {
      LOG_ERROR("Unable to get tracked frame from the list - tracked frame is NULL (position in the list: " << frame << ")!"); 
      continue; 
    }

    // Get the encoder value in mm 
    double probePos(0), probeRot(0), templatePos(0); 
    if ( !vtkStepperCalibrationController::GetStepperEncoderValues(trackedFrame, probePos, probeRot, templatePos) )
    {
      LOG_WARNING("Translation axis calibration: Unable to get stepper encoder values from tracked frame info for frame #" << frame); 
      continue; 
    }

    double z(0); 
    switch ( this->GetDataType() )
    {
    case PROBE_TRANSLATION: 
      {
        z = probePos; 
      }
      break; 
    case TEMPLATE_TRANSLATION: 
      {
        z = templatePos; 
      }
      break; 
    default: 
      LOG_ERROR("Unable to construct linear equations for translation axis calibration - this data type is not supported: " << this->GetDataType()); 
      return PLUS_FAIL;
    }

    if ( trackedFrame->GetFiducialPointsCoordinatePx() == NULL )
    {
      LOG_ERROR("Unable to get segmented fiducial points from tracked frame - FiducialPointsCoordinatePx is NULL, frame is not yet segmented (position in the list: " << frame << ")!" ); 
      continue; 
    }

    if ( trackedFrame->GetFiducialPointsCoordinatePx()->GetNumberOfPoints() == 0 )
    {
      LOG_DEBUG("Unable to get segmented fiducial points from tracked frame - couldn't segment image (position in the list: " << frame << ")!" ); 
      continue; 
    }

    // Every N fiducial has 3 points on the image 
    // numberOfNFiducials = NumberOfPoints / 3 
    const int numberOfNFiduacials = trackedFrame->GetFiducialPointsCoordinatePx()->GetNumberOfPoints() / 3; 

    // We're using 2 out of 3 wires per each N-wire => ( 2 * numberOfNFiduacials )
    // We need the X and Y coordinates of each wires => need to multiply by 2 
    // We have two more unknown variables (tx, ty) the translation axis orientation for each axes
    // numberOfUnknowns = ( 2 * numberOfNFiduacials ) * 2 + 2 
    numberOfUnknowns = ( 2 * numberOfNFiduacials ) * 2 + 2;

    // Matrix column indecies used for value 1 in the matrix
    // e.g. for w4x = w4x0 + z * tx 
    // [ z 0 0 0 0 0 1 0 0 0 ] => movingIndex = 6, the index position of value 1 
    int movingIndex = numberOfUnknowns; 

    // Construct the A matrix and b vector for each side fiducials  
    for ( int i = 0; i < trackedFrame->GetFiducialPointsCoordinatePx()->GetNumberOfPoints(); ++i)
    {
      if ( ( (i+1) % 3 ) != 2 ) // wire #1,#3,#4,#6... => non moving points of the N-wire 
      {
        // Wire X coordinate in mm (#1,#3,#4,#6)
        double bX = trackedFrame->GetFiducialPointsCoordinatePx()->GetPoint(i)[0] * this->GetSpacing()[0]; 

        // Wire Y coordinate in mm (#1,#3,#4,#6)
        double bY = trackedFrame->GetFiducialPointsCoordinatePx()->GetPoint(i)[1] * this->GetSpacing()[1]; 

        bVector.push_back(bX); 
        bVector.push_back(bY); 

        if ( movingIndex >= numberOfUnknowns - 1 )
        {
          movingIndex = 1; 
        }
        
        vnl_vector<double> a1(numberOfUnknowns,0); 
        a1.put(0, z);  // raw encoder value in mm
        a1.put(++movingIndex, 1);  // the index position of matrix row value 1 

        vnl_vector<double> a2(numberOfUnknowns,0); 
        a2.put(1, z); // raw encoder value in mm
        a2.put(++movingIndex, 1);  // the index position of matrix row value 1 

        aMatrix.push_back(a1); 
        aMatrix.push_back(a2); 
      }
    } // end of fiduacials 

  } // end of frames 

  return PLUS_SUCCESS; 
}

