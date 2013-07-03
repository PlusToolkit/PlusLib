#include "PlusConfigure.h"

// ITK includes
#include <itkAffineTransform.h>
#include <itkImage.h>
#include <itkLandmarkBasedTransformInitializer.h>
#include <itkSimilarity3DTransform.h>
#include <itkTransformFileWriter.h>

// STD includes
#include <numeric>
#include <algorithm>
#include <vnl/vnl_matrix.h>

/*!
\class itkFiducialTransformComputation
\brief This class compute the similarity transform between two sets of ordered points.
It is an adaptation of the slicer fiducial registration module.
\ingroup PlusLibCalibrationAlgorithm
*/

/*! Operator to compute the squared distance between two points */
class SquaredPointDistance
{
public:
  explicit SquaredPointDistance(const itk::Point<double, 3>& ctr)
    : m_Point(ctr)
  {
  }

  double operator()(const itk::Point<double, 3>& p)
  {
    return (p - m_Point).GetSquaredNorm();
  }

private:
  itk::Point<double, 3> m_Point;

};


/*!
// Function to compute the scaling factor between two sets of points.
// This is the symmetric form given by
//    Berthold K. P. Horn (1987),
//    "Closed-form solution of absolute orientation using unit quaternions,"
//    Journal of the Optical Society of America A, 4:629-642
*/
class itkFiducialTransformComputation
{
public:
  itkFiducialTransformComputation();

  /*! Returns the similarity transform between fixed and moving points */
  bool computeTransform(std::vector<itk::Point<double, 3> > fixedPoints, std::vector<itk::Point<double, 3> > movingPoints,
    std::string transformType);

  /*! Returns the scale factor */
  double GetScaleFactor()
  {
    return ScaleFactor;
  };

  /*! Returns the rms error */
  double GetRmsError()
  {
    return RmsError;
  };

  /*! Returns the image to probe similarity transform */
  vnl_matrix<double> GetImageToProbeTransformMatrixVnl()
  {
    return this->ImageToProbeTransformMatrixVnl;
  };
private:
  /*! Compute the scale factor of the similarity transform */
  static double computeSymmetricScale(const std::vector<itk::Point<double, 3> >& fixedPoints,
    const std::vector<itk::Point<double, 3> >& movingPoints,
    const itk::Point<double, 3>& fixedcenter,
    const itk::Point<double, 3>& movingcenter);
  double RmsError;


  /*! Store the result of the optimization process */
  vnl_matrix<double> ImageToProbeTransformMatrixVnl;
  double ScaleFactor;
};