#include <fstream>
#include "RANSAC.h"
#include "RandomNumberGenerator.h"
#include "SphereParametersEstimator.h"


/**
 * Generate points on a (hyper)sphere with additive zero mean Gaussian noise and
 * outliers.
 * @param numInliers How many points are inliers.
 * @param numOutliers How many points are outliers.
 * @param outlierDistance Threshold defining outliers, points that are further
 *                        than this distance from the sphere.
 * @param data The points are added to the end of this vector.
 * @param sphereParameters [c,r], sphere center and radius. Sphere is 
 *                        defined as the set of points p such that 
 *                        (p-c)^T(p-c)=r^2. 
 */
template<unsigned int dimension>
void GenerateData( unsigned int numInliers, unsigned int numOutliers, 
                   double outlierDistance,
                   std::vector< itk::Point<double,dimension> > &data,
                   std::vector<double> &sphereParameters );

/**
 * Given the hard coded dimension, and number of outliers and inliers generate
 * a random dataset accordingly. Then estimate the (hyper)sphere parameter values
 * using a least squares estimate and the RANSAC algorithm. Compare the results
 * to the known (hyper)sphere. Code is written for nD data except the 
 * visualization which is limited to 3D. If DIMENSION is set to three, then two
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
  std::string leastSquaresOutputFileName = "leastSquaresSphereEstimation.iv";
  std::string ransacOutputFileName = "RANSACSphereEstimation.iv";

  typedef itk::SphereParametersEstimator<DIMENSION> SphereEstimatorType;
  typedef itk::RANSAC<itk::Point<double, DIMENSION>, double> RANSACType;
  
  std::vector< itk::Point<double,DIMENSION> > data;
  std::vector<double> trueSphereParameters, sphereParameters; 
  double outlierDistance = 20.0;
  unsigned int i;
  itk::Vector<double, DIMENSION> tmp; 

  GenerateData<DIMENSION>( INLIERS, OUTLIERS, outlierDistance,
                           data, trueSphereParameters );

  std::cout<<"Known hyper(sphere) parameters [c,r]\n\t [ ";
  for( i=0; i<DIMENSION; i++ )
    std::cout<<trueSphereParameters[i]<<", ";
  std::cout<<trueSphereParameters[i]<<"]\n\n";
                        //create and initialize the parameter estimator
  double maximalDistanceFromSphere = 0.5;
  SphereEstimatorType::Pointer sphereEstimator = SphereEstimatorType::New();
  sphereEstimator->SetDelta( maximalDistanceFromSphere );
  sphereEstimator->SetLeastSquaresType( itk::SphereParametersEstimator<DIMENSION>::GEOMETRIC );
  sphereEstimator->LeastSquaresEstimate( data, sphereParameters );
  if( sphereParameters.empty() )
    std::cout<<"Least squares estimate failed, degenerate configuration?\n";
  else {
    std::cout<<"Least squares hyper(sphere) parameters: [c,r]\n\t [ ";
    for( i=0; i<DIMENSION; i++ )
      std::cout<<sphereParameters[i]<<", ";
    std::cout<<sphereParameters[i]<<"]\n\n";
              //distance between known (hyper)sphere center and estimated one,
              //and difference between the two radii
    for( i=0; i<DIMENSION; i++ )
      tmp[i] = sphereParameters[i] - trueSphereParameters[i];
    std::cout<<"\t Distance between estimated and known sphere centers [0=correct]: ";
    std::cout<<tmp.GetNorm()<<"\n";
    std::cout<<"\t Difference between estimated and known sphere radius [0=correct]: ";
    std::cout<<fabs( sphereParameters[DIMENSION] - trueSphereParameters[DIMENSION] )<<"\n";
  }
                          //create and initialize the RANSAC algorithm
  double desiredProbabilityForNoOutliers = 0.999;
  RANSACType::Pointer ransacEstimator = RANSACType::New();
  ransacEstimator->SetData( data );
  ransacEstimator->SetParametersEstimator( sphereEstimator.GetPointer() );
  double percentageOfDataUsed = 
    ransacEstimator->Compute( sphereParameters, desiredProbabilityForNoOutliers );
  std::cout<<"Percentage of data used: "<<percentageOfDataUsed<<std::endl;
  if( sphereParameters.empty() )
    std::cout<<"RANSAC estimate failed, degenerate configuration?\n";
  else
  {
    std::cout<<"RANSAC hyper(sphere) parameters: [c,r]\n\t [ ";
    for( i=0; i<DIMENSION; i++ )
      std::cout<<sphereParameters[i]<<", ";
    std::cout<<sphereParameters[i]<<"]\n\n";
              //distance between known hyper(sphere) center and estimated one,
              //and difference between the two radii
    for( i=0; i<DIMENSION; i++ )
      tmp[i] = sphereParameters[i] - trueSphereParameters[i];
    std::cout<<"\t Distance between estimated and known sphere centers [0=correct]: ";
    std::cout<<tmp.GetNorm()<<"\n";
    std::cout<<"\t Difference between estimated and known sphere radius [0=correct]: ";
    std::cout<<fabs( sphereParameters[DIMENSION] - trueSphereParameters[DIMENSION] )<<"\n";
  }
  return EXIT_SUCCESS;
}


template<unsigned int dimension>
void GenerateData( unsigned int numInliers, unsigned int numOutliers, 
                   double outlierDistance,
                   std::vector< itk::Point<double,dimension> > &data,
                   std::vector<double> &sphereParameters )
{
  itk::Vector<double, dimension> tmp, noise;
  itk::Point<double, dimension> sphereCenter, randomPoint;
  double sphereRadius;
  double noiseStandardDeviation = 0.4; //noise standard deviation
  double coordinateMax = 1000.0;
  unsigned int i, j;

  RandomNumberGenerator random;

  sphereParameters.clear();
         //generate points on random (hyper) sphere
  for( i=0; i<dimension; i++ ) {
    sphereCenter[i] = random.uniform( -coordinateMax, coordinateMax );
  }
  sphereRadius = random.uniform(0.0, coordinateMax);

  for( i=0; i<dimension; i++ ) 
    sphereParameters.push_back( sphereCenter[i] );
  sphereParameters.push_back( sphereRadius );

               //generate inliers
  for( i=0; i<numInliers; i++ ) {
    for( j=0; j<dimension; j++ ) {
      tmp[j] = random.uniform( -1.0, 1.0 );
      noise[j] = random.normal( noiseStandardDeviation );
    }
            //project random point onto the sphere and add noise
    tmp.Normalize();
    randomPoint = sphereCenter + noise + tmp*sphereRadius;
    data.push_back( randomPoint );
  }
           //generate outliers (via rejection)
  for( i=0; i<numOutliers; i++ ) {
    for( j=0; j<dimension; j++ ) {
      randomPoint[j] = random.uniform( -coordinateMax, coordinateMax );      
    }
    tmp = randomPoint - sphereCenter;
    if( tmp.GetNorm() >= outlierDistance )
      data.push_back( randomPoint );
    else
      i--;
  }
}
