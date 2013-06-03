/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __vtkSingleWallCalibrationAlgo_h
#define __vtkSingleWallCalibrationAlgo_h

#include "vtkLineSegmentationAlgo.h"
#include "vtkTransformRepository.h"

class vtkTrackedFrameList;

/*!
  \class vtkSingleWallCalibrationAlgo 

  \brief fill this in with details

  \ingroup PlusLibDataCollection
*/
class VTK_EXPORT vtkSingleWallCalibrationAlgo : public vtkObject
{
public:
  static vtkSingleWallCalibrationAlgo *New();
  vtkTypeRevisionMacro(vtkSingleWallCalibrationAlgo, vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);

  struct ExtractResult 
  {
    vnl_matrix<double> NormalVectorsOfWallPlane;
    vnl_matrix<double> LateralDirectionsOfImagePlane;
    vnl_matrix<double> AxialDirectionsOfWallPlane;
    double PixelPerMMInYToPixelPerMMInX;
  };

  /*!
    Read XML based configuration for the calibration
    \param aConfig Root element of device set configuration data
  */
  PlusStatus ReadConfiguration( vtkXMLDataElement* aConfig ); 

  PlusStatus Calibrate();

  /*!
    Assembles the result string to display
    \param precision Number of decimals printed in the string
    \return String containing results
  */
  std::string GetResultString(int precision = 3);

  const vtkMatrix4x4& GetImageToProbeTransformation() const;

  void SetTrackedFrameList(vtkTrackedFrameList* aTrackedFrameList);

  vtkGetStringMacro(ImageCoordinateFrame);
  vtkGetStringMacro(ProbeCoordinateFrame);
  vtkGetStringMacro(ReferenceCoordinateFrame);

protected:
  vtkSingleWallCalibrationAlgo();
  ~vtkSingleWallCalibrationAlgo();

  vtkSetStringMacro(ImageCoordinateFrame);
  vtkSetStringMacro(ProbeCoordinateFrame);
  vtkSetStringMacro(ReferenceCoordinateFrame);

  vtkSetObjectMacro(Repository, vtkTransformRepository);
  vtkGetObjectMacro(Repository, vtkTransformRepository);

  vtkGetObjectMacro(TrackedFrameList, vtkTrackedFrameList);
  vtkSetObjectMacro(ImageToProbeTransformation, vtkMatrix4x4);
  vtkSetObjectMacro(LineSegmenter, vtkLineSegmentationAlgo);

  PlusStatus ExtractVectorsOfImageAndNormalOfWall(const vnl_vector<double>& anXEstimate, vtkSingleWallCalibrationAlgo::ExtractResult& aResult);
  PlusStatus Validate() const;
  void CleanMatrix( vtkMatrix4x4* aMatrix );

  vtkTrackedFrameList*      TrackedFrameList;
  vtkLineSegmentationAlgo*  LineSegmenter;
  vtkMatrix4x4*             ImageToProbeTransformation;
  double                    MmPerPixelX; // image width in mm / image pixel size X
  double                    MmPerPixelY; // image depth in mm / image pixel size Y

  char* ImageCoordinateFrame;
  char* ProbeCoordinateFrame;
  char* ReferenceCoordinateFrame;
  
  bool                  UpToDate;

  vtkTransformRepository*   Repository;

private:
  vtkSingleWallCalibrationAlgo(const vtkSingleWallCalibrationAlgo&);
  void operator=(const vtkSingleWallCalibrationAlgo&);
};

#endif
