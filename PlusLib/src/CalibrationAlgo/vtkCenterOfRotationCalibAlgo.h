/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __vtkCenterOfRotationCalibAlgo_h
#define __vtkCenterOfRotationCalibAlgo_h

#include "PlusConfigure.h"
#include "vtkObject.h"
#include "vtkTrackedFrameList.h"
#include "vtkTable.h"

class vtkHTMLGenerator; 
class vtkGnuplotExecuter; 

/*!
  \class vtkCenterOfRotationCalibAlgo 
  \brief Algorithm for computing the center of in-plane rotation of ultrasound images of a calibration phantom.
  
  The algorithm is used for computing the rotation axis position of brachytherapy steppers.
  The rotation is assumed to be in the image plane (the rotation axis is assumed to be perpendicular
  to the image plane). The images shall be taken of a calibration phantom and the frames shall be segmented
  (the fiducial point coordinates shall be computed) before calling this algorithm.

  \verbatim

  Asumptions:
    Distance between a non-moving wire position (denoted as S) and the rotation center 
    of the optical encoder (denoted as O) remains constant during rotation: 
        || Si - O || = || Sj - O ||       

  Model: 
        pow(Sxi - Ox, 2) + pow(Syi - Oy, 2) = pow(Sxj - Ox, 2) + pow(Syj - Oy, 2) where i != j   

  Unknowns: 
    Ox, Oy = center of rotation in pixel   

  Linear system: 
    Ax = B      

    [ (Sxi - Sxj) (Syi - Syj) ] * [ Ox ] = [ (0.5*( Sxi*Sxi + Syi*Syi - Sxj*Sxj - Syj*Syj )) ]
                                  [ Oy ]

  \endverbatim
  \ingroup PlusLibCalibrationAlgorithm
*/ 

class VTK_EXPORT vtkCenterOfRotationCalibAlgo : public vtkObject
{
public:

  static vtkCenterOfRotationCalibAlgo *New();
  vtkTypeRevisionMacro(vtkCenterOfRotationCalibAlgo , vtkObject);
  virtual void PrintSelf(ostream& os, vtkIndent indent); 

  /*!
    Set all required algorithm inputs.
    \param trackedFrameList Tracked frames with segmentation results 
    \param indices Tracked frame indices that the algoritm should use during calibration
    \param spacing Image spacing (mm/pixel)
  */
  virtual void SetInputs( vtkTrackedFrameList* trackedFrameList,  std::vector<int> &indices, double spacing[2]); 

  /*! Get the computed center of rotation position in pixel */
  virtual PlusStatus GetCenterOfRotationPx( double centerOfRotationPx[2] ); 

  /*! Get the center of rotation calibration error */
  virtual PlusStatus GetError(double &mean, double &stdev); 

  /*! Get the report table that is used for storing algorithm results */
  vtkGetObjectMacro(ReportTable, vtkTable); 

  /*! Add generated html report from center of rotation calibration to the existing html report */
  virtual PlusStatus GenerateReport( vtkHTMLGenerator* htmlReport, vtkGnuplotExecuter* plotter); 

  /*!
    Add generated html report from center of rotation calibration to the existing html report
    \param numberOfNWirePatterns Number of N-wire structure used 
    \param htmlReport Has to be defined by the caller function
    \param plotter Has to be defined by the caller function
    \param reportTable Computed output
    \param centerOfRotationPx Computed output
  */
  static PlusStatus GenerateCenterOfRotationReport( int numberOfNWirePatterns, 
    vtkHTMLGenerator* htmlReport, 
    vtkGnuplotExecuter* plotter, 
    vtkTable* reportTable,
    double centerOfRotationPx[2]); 

protected:
  vtkCenterOfRotationCalibAlgo();
  virtual ~vtkCenterOfRotationCalibAlgo(); 

  /*! Set tracked frame indices used for calibration */
  virtual void SetTrackedFrameListIndices( std::vector<int> &indices ); 

  /*! Get the number of N-wire patterns used in the tracked frame list */ 
  int GetNumberOfNWirePatterns(); 

  /*! Set the input tracked frame list */
  vtkSetObjectMacro(TrackedFrameList, vtkTrackedFrameList); 

  /*! Set the input image spacing (mm/pixel) */
  vtkSetVector2Macro(Spacing, double); 

  /*! Bring this algorithm's outputs up-to-date. */
  virtual PlusStatus Update();

  /*! Construct linear equation for center of rotation calibration */
  virtual PlusStatus ConstructLinearEquationForCalibration(std::vector<vnl_vector<double> > &aMatrix, std::vector<double> &bVector);

  /*! Add new column to the report table */
  PlusStatus AddNewColumnToReportTable( const char* columnName ); 

  /*! Update center of rotation calibration error report table */
  virtual PlusStatus UpdateReportTable(); 

  /*! Report table used for storing algorithm results */
  vtkSetObjectMacro(ReportTable, vtkTable); 

	/*! Set the rotation center in pixels. */
	vtkSetVector2Macro(CenterOfRotationPx, int); 

  /*! Stores the center of rotation in px space */
	double CenterOfRotationPx[2]; 

  /*! Image spacing (mm/pixel)*/
  double Spacing[2];

  /*! Tracked frames used for calibration with segmentation results */
  vtkTrackedFrameList* TrackedFrameList; 

  /*! Tracked frame indices used for calibration */
  std::vector<int> TrackedFrameListIndices; 

  /*! Table used for storing algorithm results */
  vtkTable* ReportTable; 

  /*! Calibration error mean */
  double ErrorMean; 
  
  /*! Calibration error standard deviation */
  double ErrorStdev; 

  /*! When the results were computed. The result is recomputed only if the inputs changed more recently than UpdateTime. */
  vtkTimeStamp UpdateTime;  
}; 

#endif
