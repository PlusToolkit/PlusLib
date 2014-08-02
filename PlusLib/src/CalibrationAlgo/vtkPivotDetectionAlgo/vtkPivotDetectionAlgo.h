/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __vtkPivotDetectionAlgo_h
#define __vtkPivotDetectionAlgo_h

#include "PlusConfigure.h"

#include "vtkObject.h"
#include "vtkMatrix4x4.h"
#include "vtkPoints.h"

#include <list>
#include <set>

class vtkTransformRepository;
class vtkXMLDataElement;

//-----------------------------------------------------------------------------

/*!
\class vtkPivotDetectionAlgo 
\brief Pivot detection algorithm to detect when a calibrated stylus is pivoting. 
The stylus pivoting point is computed assuming that the stylus is calibrated, which is often a good approximation.

\ingroup PlusLibCalibrationAlgorithm
*/
class vtkPivotDetectionAlgo : public vtkObject
{
public:
  vtkTypeRevisionMacro(vtkPivotDetectionAlgo,vtkObject);
  static vtkPivotDetectionAlgo *New();

  /*!
  * Read configuration
  * \param aConfig Root element of the device set configuration
  */
  PlusStatus ReadConfiguration(vtkXMLDataElement* aConfig);

  /*!
  Remove all previously inserted points.
  Call this method to get rid of previously added points
  before starting a new detection.
  */
  void RemoveAllCalibrationPoints();

  /*!
  Insert acquired point to detection point list
  \param StylusToReferenceTransform New calibration point (tool to reference transform)
  */
  PlusStatus InsertNextCalibrationPoint(vtkMatrix4x4* StylusToReferenceTransform);

  PlusStatus GetPivot(double* pivotPointReference);

  /*!
  Get calibration result string to display
  \param aPrecision Number of decimals shown
  \return Calibration result (e.g. stylus tip to stylus translation) string
  */
  std::string GetStylusTipToStylusTranslationString(double aPrecision=3);

  /*!
  Get the number of outlier points. It is recommended to display a warning to the user
  if the percentage of outliers vs total number of points is larger than a few percent.
  */
  //int GetNumberOfDetectedOutliers();

public:

  vtkGetMacro(CalibrationError, double);

  vtkGetObjectMacro(StylusTipToStylusTransformMatrix, vtkMatrix4x4); 

  //vtkGetVector3Macro(PivotPointPosition_Reference, double);

  vtkGetStringMacro(ObjectMarkerCoordinateFrame);
  vtkGetStringMacro(ReferenceCoordinateFrame);
  vtkGetStringMacro(ObjectPivotPointCoordinateFrame);
  vtkSetObjectMacro(StylusTipToStylusTransformMatrix, vtkMatrix4x4);

protected:

  vtkSetStringMacro(ObjectMarkerCoordinateFrame);
  vtkSetStringMacro(ReferenceCoordinateFrame);
  vtkSetStringMacro(ObjectPivotPointCoordinateFrame);
  vtkSetObjectMacro(PivotPointsReference, vtkPoints);

protected:
  vtkPivotDetectionAlgo();
  virtual  ~vtkPivotDetectionAlgo();

protected:
  /*! Compute the mean position error of the pivot point (in mm) */
  void ComputeCalibrationError();
  PlusStatus EstimatePivotPointPosition();

  void EraseLastPoints();

  PlusStatus GetPartialStylusPositionAverage(double* pivotPoint_Reference);
  //PlusStatus GetPivotPointPosition(double* pivotPoint_Marker, double* pivotPoint_Reference);

protected:
  /*! Pivot point to marker transform (eg. stylus tip to stylus) - the result of the calibration */
  vtkMatrix4x4*        StylusTipToStylusTransformMatrix;
   vtkSmartPointer<vtkPoints> PivotPointsReference;

  /*! Mean error of the calibration result in mm */
  double              CalibrationError;

  /*! Array of the input points */
  std::list< vtkMatrix4x4* > StylusTipToReferenceTransformsList;

  /*! Iterators to track marker transformations in the list*/
  std::list< vtkMatrix4x4* >::iterator CurrentStylusTipIterator;
  std::list< vtkMatrix4x4* >::iterator LastStylusTipIterator;

  /*! The number of partial inserted points*/
  int PartialInsertedPoints;

  /*! Name of the object marker coordinate frame (eg. Stylus) */
  char*               ObjectMarkerCoordinateFrame;

  /*! Name of the reference coordinate frame (eg. Reference) */
  char*               ReferenceCoordinateFrame;

  /*! Name of the object pivot point coordinate frame (eg. StylusTip) */
  char*               ObjectPivotPointCoordinateFrame;

  /*! Pivot point position in the Reference coordinate system */
  //double              PivotPointPosition_Reference[4];

  /*! Pivot point position in the Reference coordinate system */
  double              StylusTipAverage_Reference[4];

  /*! Chunk pivot point position in the Reference coordinate system list*/
  std::list< std::vector<double> > PivotPointPosition_Reference_List;

  /*! List of outlier sample indices */
  std::set<unsigned int> OutlierIndices;
};

#endif
