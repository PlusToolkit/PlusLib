#include <fstream>
#include "RANSAC.h"
#include "PlaneParametersEstimator.h"
#include "RandomNumberGenerator.h"


/**
 * Generate points on a (hyper)plane with additive zero mean Gaussian noise and
 * outliers.
 * @param numInliers How many points are inliers.
 * @param numOutliers How many points are outliers.
 * @param outlierDistance Threshold defining outliers, points that are further
 *                        than this distance from the plane.
 * @param data The points are added to the end of this vector.
 * @param planeParameters [n,a], plane normal and point on plane. Plane is 
 *                        defined as the set of points p such that n^T(p-a)=0. 
 */
template<unsigned int dimension>
void GenerateData( unsigned int numInliers, unsigned int numOutliers, 
                   double outlierDistance,
                   std::vector< itk::Point<double,dimension> > &data,
                   std::vector<double> &planeParameters );

/**
 * Given the hard coded dimension, and number of outliers and inliers generate
 * a random dataset accordingly. Then estimate the (hyper)plane parameter values
 * using a least squares estimate and the RANSAC algorithm. Compare the results
 * to the known (hyper)plane. Code is written for nD data except the 
 * visualization which is limited to 3D. If DIMENSION is set to three, two
 * open inventor scene files are written, showing the least squares and RANSAC
 * estimates. Data points are colored spheres, those that agree with the 
 * estimated model are green, otherwise they are red.
 *
 * @author Ziv Yaniv (zivy@isis.georgetown.edu)
 */
int main(int argc, char *argv[])
{
  const unsigned int DIMENSION = 3;
  const unsigned int INLIERS = 90;
  const unsigned int OUTLIERS = 10;
  std::string leastSquaresOutputFileName = "leastSquaresPlaneEstimation.iv";
  std::string ransacOutputFileName = "RANSACPlaneEstimation.iv";

  typedef itk::PlaneParametersEstimator<DIMENSION> PlaneEstimatorType;
  typedef itk::RANSAC<itk::Point<double, DIMENSION>, double> RANSACType;
  
  std::vector< itk::Point<double,DIMENSION> > data;
  std::vector<double> truePlaneParameters, planeParameters; 
  double outlierDistance = 20.0;
  unsigned int i;  
  double dotProduct;

  GenerateData<DIMENSION>( INLIERS, OUTLIERS, outlierDistance,
                          data, truePlaneParameters );

  std::cout<<"Known (hyper)plane parameters [n,a]\n\t [ ";
  for( i=0; i<(2*DIMENSION-1); i++ )
    std::cout<<truePlaneParameters[i]<<", ";
  std::cout<<truePlaneParameters[i]<<"]\n\n";

                        //create and initialize the parameter estimator
  double maximalDistanceFromPlane = 0.5;
  PlaneEstimatorType::Pointer planeEstimator = PlaneEstimatorType::New();
  planeEstimator->SetDelta( maximalDistanceFromPlane );
  planeEstimator->LeastSquaresEstimate( data, planeParameters );
  if( planeParameters.empty() )
    std::cout<<"Least squares estimate failed, degenerate configuration?\n";
  else
  {
    std::cout<<"Least squares hyper(plane) parameters: [n,a]\n\t [ ";
    for( i=0; i<(2*DIMENSION-1); i++ )
      std::cout<<planeParameters[i]<<", ";
    std::cout<<planeParameters[i]<<"]\n\n";
                //cos(theta), theta is the angle between the two unit normals
    dotProduct = 0.0;
    for( i=0; i<DIMENSION; i++ )
      dotProduct+= planeParameters[i]*truePlaneParameters[i];
    std::cout<<"\tDot product of real and computed normals[+-1=correct]: ";
    std::cout<<dotProduct<<"\n";
              //distance between known hyper(plane) and estimated point on plane
    dotProduct = 0.0;
    for( i=0; i<DIMENSION; i++ )
      dotProduct+= (planeParameters[DIMENSION+i] - 
                    truePlaneParameters[DIMENSION+i])*truePlaneParameters[i];
    std::cout<<"\tCheck if computed point is on known plane [0=correct]: ";
    std::cout<<dotProduct<<"\n\n";
  }

                          //create and initialize the RANSAC algorithm
  double desiredProbabilityForNoOutliers = 0.999;
  double percentageOfDataUsed;
  RANSACType::Pointer ransacEstimator = RANSACType::New();
  ransacEstimator->SetData( data );
  ransacEstimator->SetParametersEstimator( planeEstimator.GetPointer() );
  percentageOfDataUsed = 
    ransacEstimator->Compute( planeParameters, desiredProbabilityForNoOutliers );
  if( planeParameters.empty() )
    std::cout<<"RANSAC estimate failed, degenerate configuration?\n";
  else
  {
    std::cout<<"RANSAC hyper(plane) parameters: [n,a]\n\t [ ";
    for( i=0; i<(2*DIMENSION-1); i++ )
      std::cout<<planeParameters[i]<<", ";
    std::cout<<planeParameters[i]<<"]\n\n";
                //cos(theta), theta is the angle between the two unit normals
    dotProduct = 0.0;
    for( i=0; i<DIMENSION; i++ )
      dotProduct+= planeParameters[i]*truePlaneParameters[i];
    std::cout<<"\tDot product of real and computed normals[+-1=correct]: ";
    std::cout<<dotProduct<<"\n";
              //distance between known hyper(plane) and estimated point on plane
    dotProduct = 0.0;
    for( i=0; i<DIMENSION; i++ )
      dotProduct+= ( planeParameters[DIMENSION+i] - 
                     truePlaneParameters[DIMENSION+i] )*truePlaneParameters[i];
    std::cout<<"\tCheck if computed point is on known plane [0=correct]: ";
    std::cout<<dotProduct<<"\n\n";
    std::cout<<"\tPercentage of points which were used for final estimate: ";
    std::cout<<percentageOfDataUsed<<"\n\n";
    
  }
  return EXIT_SUCCESS;
}


template<unsigned int dimension>
void GenerateData( unsigned int numInliers, unsigned int numOutliers, 
                   double outlierDistance, 
                   std::vector< itk::Point<double,dimension> > &data,
                   std::vector<double> &planeParameters )
{
  itk::Vector<double, dimension> normal, noise, tmp;
  itk::Point<double, dimension> pointOnPlane, randomPoint;
  double noiseStandardDeviation = 0.4; //noise standard deviation
  double coordinateMax = 1000.0;
  unsigned int i, j;

  RandomNumberGenerator random;

  planeParameters.clear();
         //generate points on random (hyper) plane
  for( i=0; i<dimension; i++ ) {
    normal[i] = random.uniform();
    pointOnPlane[i] = random.uniform( -coordinateMax, coordinateMax );
  }
  normal.Normalize();
  for( i=0; i<dimension; i++ ) 
    planeParameters.push_back( normal[i] ); 
  for( i=0; i<dimension; i++ ) 
    planeParameters.push_back( pointOnPlane[i] );

               //generate inliers
  for( i=0; i<numInliers; i++ ) {
    for( j=0; j<dimension; j++ ) {
      randomPoint[j] = random.uniform( -coordinateMax, coordinateMax );
      noise[j] = random.normal( noiseStandardDeviation );
    }
            //project random point onto the plane and add noise
    tmp = randomPoint - pointOnPlane;
    randomPoint = pointOnPlane + noise + (tmp - (tmp*normal)*normal);
    data.push_back( randomPoint );
  }
           //generate outliers (via rejection)
  for( i=0; i<numOutliers; i++ ) {
    for( j=0; j<dimension; j++ ) {
      randomPoint[j] = random.uniform( -coordinateMax, coordinateMax );      
    }
    tmp = randomPoint - pointOnPlane;
    if( fabs(tmp*normal)>= outlierDistance )
      data.push_back( randomPoint );
    else
      i--;
  }
}
