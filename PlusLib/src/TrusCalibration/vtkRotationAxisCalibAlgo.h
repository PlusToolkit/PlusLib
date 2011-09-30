// Stepper translation axis calibration 
// Asumptions
//    Wire 1, 3, 4, 6 are parallel to the template z axis. The stepper translation and rotation axes 
//    have slightly different orientation compared to the template z axis
// Measurements:
//    z = translation value received from the stepper, in mm
//    w1x, w1y, w3x, w3y, w4x, w4y, w6x, w6y = detected wire x and y positions in mm
// Translation model:
//    w1x = w1x0 + z * tx
//    w1y = w1y0 + z * ty
//    w3x = w3x0 + z * tx
//    w3y = w3y0 + z * ty
//    w4x = w4x0 + z * tx
//    w4y = w4y0 + z * ty
//    w6x = w6x0 + z * tx
//    w6y = w6y0 + z * ty
// Unknowns:
//    tx, ty = translation axis orientation
//    w1x0, w1y0, w3x0, w3y0, w4x0, w4y0, w6x0, w6y0 = wire initial positions (at z=0), 
//    they are not needed after the axis calibration, as the wire positions will be determined 
//    in a later calibration step anyway
// Linear system:
//    A x = b
//                           [ tx ]
// [ z 0 1 0 0 0 0 0 0 0 ]   [ ty ]   [w1x]
// [ 0 z 0 1 0 0 0 0 0 0 ]   [w1x0]   [w1y]
// [ z 0 0 0 1 0 0 0 0 0 ]   [w1y0]   [w3x]
// [ 0 z 0 0 0 1 0 0 0 0 ]   [w3x0]   [w3y]
// [ z 0 0 0 0 0 1 0 0 0 ] * [w3y0] = [w4x]
// [ 0 z 0 0 0 0 0 1 0 0 ]   [w4x0]   [w4y]
// [ z 0 0 0 0 0 0 0 1 0 ]   [w4y0]   [w6x]
// [ 0 z 0 0 0 0 0 0 0 1 ]   [w6x0]   [w6y]
//                           [w6y0]
//

#ifndef __vtkRotationAxisCalibAlgo_h
#define __vtkRotationAxisCalibAlgo_h

#include "PlusConfigure.h"
#include "vtkObject.h"
#include "vtkTrackedFrameList.h"
#include "vtkTable.h"

class vtkHTMLGenerator; 
class vtkGnuplotExecuter; 

class VTK_EXPORT vtkRotationAxisCalibAlgo : public vtkObject
{
public:

  static vtkRotationAxisCalibAlgo *New();
  vtkTypeRevisionMacro(vtkRotationAxisCalibAlgo , vtkObject);
  virtual void PrintSelf(ostream& os, vtkIndent indent); 

  // Description:
  // Add generated html report from the selected data type (probe or template) translation axis calibration to the existing html report
  // htmlReport and plotter arguments has to be defined by the caller function
  virtual PlusStatus GenerateReport( vtkHTMLGenerator* htmlReport, vtkGnuplotExecuter* plotter, const char* gnuplotScriptsFolder); 

  // Description:
  // Set inputs: 
  // - TrackedFrameList with segmentation results 
  // - Image spacing (mm/px)
  virtual void SetInputs( vtkTrackedFrameList* trackedFrameList, double spacing[2]); 

    //! Description: 
  // Set/get tracked frame list
  vtkSetObjectMacro(TrackedFrameList, vtkTrackedFrameList); 
  vtkGetObjectMacro(TrackedFrameList, vtkTrackedFrameList); 

  //! Description: 
	// Set/Get the minimum number of clusters for rotation axis calibration
	vtkSetMacro(MinNumberOfRotationClusters, int); 
	vtkGetMacro(MinNumberOfRotationClusters, int);

  //! Description: 
  // Set/Get the image spacing.
  // (x: lateral axis, y: axial axis)
  vtkSetVector2Macro(Spacing, double); 
  vtkGetVector2Macro(Spacing, double); 

  //! Description: 
	// Set/Get the rotation center in pixels.
	vtkSetVector2Macro(CenterOfRotationPx, double); 
	vtkGetVector2Macro(CenterOfRotationPx, double);

  //! Description: 
  // Get report table used for storing algorithm results 
  vtkGetObjectMacro(ReportTable, vtkTable); 

  //! Description: 
  // Get report table used for center of rotation algo results for each cluster 
  vtkGetObjectMacro(CenterOfRotationReportTable, vtkTable); 

  // Description:
  // Get the rotation axis orientation 
  virtual PlusStatus GetRotationAxisOrientation(double rotationAxisOrientation[3] ); 

  // Description:
  // Get the rotation axis calibration error 
  virtual PlusStatus GetError(double &mean, double &stdev); 

protected:
  vtkRotationAxisCalibAlgo();
  virtual ~vtkRotationAxisCalibAlgo(); 

  // Description:
  // Bring this algorithm's outputs up-to-date.
  virtual PlusStatus Update();

  //! Description: 
  // Construct linear equation for rotation axis calibration
  virtual PlusStatus ConstructLinearEquationForCalibration( 
    const std::vector< std::pair<double, double> > &listOfCenterOfRotations, 
    const std::vector< double > &listOfClusterPositions, 
    std::vector<vnl_vector<double>> &aMatrix, 
    std::vector<double> &bVector);

  //! Description: 
  // Add new column to the report table 
  PlusStatus AddNewColumnToReportTable( const char* columnName ); 

  //! Description: 
  // Update rotation axis calibration error report table
  virtual PlusStatus UpdateReportTable(
    const std::vector<vnl_vector<double>> &aMatrix, 
    const std::vector<double> &bVector, 
    const vnl_vector<double> &resultVector); 

  // Description: 
  // Add center of rotation report to the current table (used with different clusters)
  virtual PlusStatus UpdateCenterOfRotationReportTable( vtkTable* reportTable ); 

	//! Description:
	// Clustering tracked frames by Z position 
  virtual PlusStatus ClusterTrackedFrames(std::vector< std::vector<int> > &clusterOfIndecies); 

	//! Description:
	// Get Z position of the cluster (aka. the z position of the point cloud)
	virtual PlusStatus GetClusterZPosition(std::vector<int> &cluster, double &clusterPosition ); 

  //! Description: 
  // Set rotation axis orientation [Rx, Ry, 1]
  vtkSetVector3Macro(RotationAxisOrientation, double); 

  //! Description: 
  // Report table used for storing algorithm results 
  vtkSetObjectMacro(ReportTable, vtkTable); 

  //! Description: 
  // Report table used for center of rotation algo results for each cluster 
  vtkSetObjectMacro(CenterOfRotationReportTable, vtkTable); 

  int MinNumberOfRotationClusters; 

  // Stores the center of rotation in px space
	double CenterOfRotationPx[2]; 

  // Image scaling factors (x: lateral axis, y: axial axis)
  double Spacing[2];

  // Rotation axis orientation [Rx, Ry, 1]
  double RotationAxisOrientation[3]; 

  vtkTrackedFrameList* TrackedFrameList; 

  // Table used for storing algo results 
  vtkTable* ReportTable; 

  // Table used for storing center of rotation algo results for each cluster 
  vtkTable* CenterOfRotationReportTable; 

  double ErrorMean; 
  double ErrorStdev; 

  // When was this data last updated
  vtkTimeStamp UpdateTime;  

}; 

#endif