#include<vector>
#include <iostream>
#include <itkPoint.h>
#include "RandomNumberGenerator.h"
#include "SphereParametersEstimator.h"


bool Test3D( std::ostream &out );
bool Test2D( std::ostream &out );

template<unsigned int dimension>
  bool TestnD( std::ostream &out );

template<unsigned int dimension>
  bool CompareAndPrint( const std::string &title, double threshold, 
                        std::vector<double> &knownSphere,
                        std::vector<double> &estimatedSphere, 
                        std::ostream &out );

/**
 * Generate points on a (hyper)sphere with additive zero mean Gaussian noise.
 * @param numPoints How many points to generate.
 * @param data The points are added to the end of this vector. Theses points are
 *             with noise.
 * @param cleanData The points are added to the end of this vector. Theses points are
 *             without noise.
 * @param sphereParameters [c,r], sphere center and radius. Sphere is 
 *                        defined as the set of points p such that 
 *                        (p-c)^T(p-c)=r^2. 
 */
template<unsigned int dimension>
void GenerateData( unsigned int numPoints, double noiseStandardDeviation,
                   std::vector< itk::Point<double,dimension> > &data,
                   std::vector< itk::Point<double,dimension> > &cleanData,
                   std::vector<double> &sphereParameters );

/*
 * Specifically test 2D and 3D as the code for sphere estimation of these 
 * dimensions is not the same as for general nD spheres.
 * Then test 4D as this covers all the code for dimensionality greater than 3.
 */
int main( int argc, char *argv[] )
{
  
  bool succeeded2D = Test2D( std::cout );
  bool succeeded3D = Test3D( std::cout );
  bool succeedednD = TestnD<4>( std::cout );

  if( succeeded2D && succeeded3D && succeedednD )
    return EXIT_SUCCESS;
  return EXIT_FAILURE;
}

/*
 * The following is taken from the comp.graphics.algorithms newsgroup faq.
 * It describes a method for uniformly generating random points on the unit 
 * sphere. This is the method used for generating the data.
 *
 * There are several methods. Perhaps the easiest to understand is the rejection 
 * method: generate random points in an origin- centered cube with opposite 
 * corners (r,r,r) and (-r,-r,-r). Reject any point p that falls outside of the 
 * sphere of radius r. Scale the vector to lie on the surface. Because the cube 
 * to sphere volume ratio is pi/6, the average number of iterations before an 
 * acceptable vector is found is 6/pi = 1.90986. This essentially doubles the 
 * effort, and makes this method slower than the trig method. A timing 
 * comparison conducted by Ken Sloan showed that the trig method runs in about 
 * 2/3’s the time of the rejection method. He found that methods based on the 
 * use of normal distributions are twice as slow as the rejection method.
 * The trig method:
 * This method works only in 3-space, but it is very fast. It depends on the 
 * slightly counterintuitive fact (see proof below) that each of the three 
 * coordinates is uniformly distributed on [-1,1] (but the three are not 
 * independent, obviously). Therefore, it suffices to choose one axis (Z, say) 
 * and generate a uniformly distributed value on that axis. This constrains the 
 * chosen point to lie on a circle parallel to the X-Y plane, and the obvious 
 * trig method may be used to obtain the remaining coordinates.
 *
 *  1. Choose z uniformly distributed in [-1,1].
 *  2. Choose t uniformly distributed on [0, 2pi).
 *  3. Let r = sqrt(1-z^2).
 *  4. Let x = r * cos(t).
 *  5. Let y = r * sin(t). 
 *
 * This method uses uniform deviates (faster to generate than normal deviates), 
 * and no set of coordinates is ever rejected.
 * Here is a proof of correctness for the fact that the z-coordinate is 
 * uniformly distributed. The proof also works for the x- and y- coordinates, 
 * but note that this works only in 3-space.
 *
 * The area of a surface of revolution in 3-space is given by
 * A = 2 * pi * int_a^b f(x) * sqrt(1 + [f'(x}]^2) dx
 *
 * Consider a zone of a sphere of radius R.  Since we are integrating in the z 
 * direction, we have
 * f(z) = sqrt(R^2 - z^2)
 * f'(z) = -z / sqrt(R^2-z^2)
 * 1 + [f'(z)]^2 = r^2 / (R^2-z^2)
 * A = 2 * pi * int_a^b sqrt(R^2-z^2) * R/sqrt(R^2-z^2) dz
 *   = 2 * pi * R int_a^b dz
 *	 = 2 * pi * R * (b-a)
 *	 = 2 * pi * R * h
 * where h = b-a is the vertical height of the zone.  Notice how the integrand
 * reduces to a constant.  The density is therefore uniform.
 */
bool Test3D( std::ostream &out )
{
  typedef itk::Point<double,3> Point3D;
  bool succeedExact, succeedAlgebraic, succeedGeometric;

  std::vector<double> trueSphereParameters;
  Point3D pnt;
	std::vector<Point3D> pointData, cleanPointData, minPointData;
          //number of points sampled on circle
	int numSamples = 20; 
         //noise is distributed IID ~N(0,noiseSigma)
  double noiseSigma = 1.0;
        //random circle radius is in the range [0,maxRadius]
	double maxRadius = 50.0; 
	int i;
  RandomNumberGenerator random;

	    //points sampled with a uniform distribution over the sphere, with 
      //additive Gaussian noise. randomly select the radius in the range 
      //[0,maxRadius] and create a sphere whose center is at the origin with 
      //this radius.
	trueSphereParameters.push_back( 0.0 );
  trueSphereParameters.push_back( 0.0 );
  trueSphereParameters.push_back( 0.0 );

	        //get random radius
  trueSphereParameters.push_back( random.uniform( 0.0, maxRadius ) );

	        //add 'numSamples' points
	for( i=0; i<numSamples; i++ ) {
		             
                 //uniform distribution in [-R,R]
		pnt[2] = random.uniform( -trueSphereParameters[3], trueSphereParameters[3] ); 
                        //uniform distribution in [0,2pi)
		double t = random.uniform( 0, 6.283185307179586476925286766559 );
    double r = sqrt( trueSphereParameters[3]*trueSphereParameters[3] - pnt[2]*pnt[2] );
    pnt[0] = r * cos( t );
    pnt[1] = r * sin( t );
                   //move the point according to sphere center and add noise
		pnt[0] += trueSphereParameters[0];
		pnt[1] += trueSphereParameters[1];
		pnt[2] += trueSphereParameters[2];		
    cleanPointData.push_back( pnt );
		pnt[0] += random.normal( noiseSigma );
		pnt[1] += random.normal( noiseSigma );
		pnt[2] += random.normal( noiseSigma );		
    pointData.push_back( pnt );
	}

  //done generating data

               //Test the code. Compare the known sphere parameters to 
               // (a) their estimate using four random points;
	             // (b) their estimate using algebraic least squares; and 
               // (c) their estimate using geometric least squares.
	std::vector<double> sphereParameters;
  itk::SphereParametersEstimator<3>::Pointer spEstimator =
    itk::SphereParametersEstimator<3>::New();
  spEstimator->SetDelta( 0.5 );

	               //The known circle parameters
	out<<"Known sphere parameters [c_x,c_y,c_z,r]:\n\t";
  out<<"["<<trueSphereParameters[0]<<","<<trueSphereParameters[1]<<",";
  out<<trueSphereParameters[2]<<","<<trueSphereParameters[3]<<"]\n\n";

	              //Take the first four points and compute the sphere parameters
	int numForEstimate = spEstimator->GetMinimalForEstimate();
	for( i=0; i<numForEstimate; i++ )
		minPointData.push_back( cleanPointData[i] );
	spEstimator->Estimate( minPointData, sphereParameters );
  succeedExact = 
    CompareAndPrint<3>( "Sphere going through four points [c_x,c_y,c_z,r], no noise:\n\t", 
                        3*noiseSigma, trueSphereParameters,
                        sphereParameters, out );

	    //Algebraic least squares estimate of circle parameters
	spEstimator->SetLeastSquaresType( itk::SphereParametersEstimator<3>::ALGEBRAIC );
	spEstimator->LeastSquaresEstimate( pointData, sphereParameters );
  succeedAlgebraic = 
    CompareAndPrint<3>( "Algebraic least squares sphere parameters [c_x,c_y,c_z,r]:\n\t", 
                        3*noiseSigma, trueSphereParameters,
                        sphereParameters, out );

		    //Geometric least squares estimate of circle parameters
	spEstimator->SetLeastSquaresType( itk::SphereParametersEstimator<3>::GEOMETRIC );
	spEstimator->LeastSquaresEstimate( pointData, sphereParameters );
  succeedGeometric = 
    CompareAndPrint<3>( "Geometric least squares sphere parameters [c_x,c_y,c_z,r]:\n\t", 
                        3*noiseSigma, trueSphereParameters,
                        sphereParameters, out );
  return ( succeedExact && succeedAlgebraic && succeedGeometric );
}


bool Test2D( std::ostream &out )
{
  typedef itk::Point<double,2> Point2D;
  bool succeedExact, succeedAlgebraic, succeedGeometric;

	double angleStepSize;
  std::vector<double> trueCircleParameters;
	Point2D pnt;
	std::vector<Point2D> pointData, cleanPointData, minPointData;
	int numSamples = 20; //number of points sampled on circle
  double noiseSigma = 1.0; //noise is distributed IID ~N(0,noiseSigma)
	double maxRadius = 50.0; //random circle radius is in the range [0,maxRadius]
	int i;

	      //1.Create data with noise: randomly select the radius in the range 
        //  [0,maxRadius] and create a circle whose center is at the origin with 
        //  this radius. Points are sampled uniformly and random noise is added.
  trueCircleParameters.push_back( 0.0 );
  trueCircleParameters.push_back( 0.0 );
                      //uniform sampling around the circle 
	angleStepSize = 6.283185307179586476925286766559/numSamples; 

	RandomNumberGenerator random;
	                  //get random radius
  trueCircleParameters.push_back( random.uniform(0.0,maxRadius) );
	        //add 'numSamples' points
	for(i=0; i<numSamples; i++) {
		pnt[0] = trueCircleParameters[2]*cos( angleStepSize*i ) + 
             trueCircleParameters[0];
		pnt[1] = trueCircleParameters[2]*sin( angleStepSize*i ) + 
             trueCircleParameters[1];
    cleanPointData.push_back( pnt );
		pnt[0] += random.normal( noiseSigma );
		pnt[1] += random.normal( noiseSigma );
		pointData.push_back( pnt );
	}

  //done generating data

               //2. Test the code. Compare the known circle parameters to 
               //   (a) their estimate using three "equally" spaced points;
	             //   (b) their estimate using algebraic least squares; and 
               //   (c) their estimate using geometric least squares.
	std::vector<double> circleParameters;
	itk::SphereParametersEstimator<2>::Pointer cpEstimator =
    itk::SphereParametersEstimator<2>::New();
  cpEstimator->SetDelta( 0.5 );

	               //The known circle parameters
	out<<"Known circle parameters [c_x,c_y,r]:\n\t ["<<trueCircleParameters[0]<<",";
  out<<trueCircleParameters[1]<<","<<trueCircleParameters[2]<<"]\n\n";
     
	             //Take three equally spaced points from the sampling without noise
               //and compute the circle parameters the reason we use the "clean" 
               //data is that otherwise the estimates are often far from the 
               //correct values
	minPointData.push_back( cleanPointData[0] );
	minPointData.push_back( cleanPointData[(int)(numSamples/3)] );
  minPointData.push_back( cleanPointData[2*((int)(numSamples/3))] );
	cpEstimator->Estimate( minPointData, circleParameters );
  succeedExact =
    CompareAndPrint<2>( "Circle going through three points [c_x,c_y,r], no noise:\n\t", 
                        3*noiseSigma, trueCircleParameters,
                        circleParameters, out );

	    //Algebraic least squares estimate of circle parameters
	cpEstimator->SetLeastSquaresType( itk::SphereParametersEstimator<2>::ALGEBRAIC );
	cpEstimator->LeastSquaresEstimate( pointData, circleParameters );
  succeedAlgebraic =
    CompareAndPrint<2>( "Algebraic least squares circle parameters [c_x,c_y,r]:\n\t", 
                        3*noiseSigma, trueCircleParameters,
                        circleParameters, out );

		    //Geometric least squares estimate of circle parameters
	cpEstimator->SetLeastSquaresType( itk::SphereParametersEstimator<2>::GEOMETRIC );
	cpEstimator->LeastSquaresEstimate( pointData, circleParameters );
  succeedGeometric =
    CompareAndPrint<2>( "Geometric least squares circle parameters [c_x,c_y,r]:\n\t", 
                        3*noiseSigma, trueCircleParameters,
                        circleParameters, out );
       //clear the data
	minPointData.clear();
	pointData.clear();

            //The following data is taken from: 
	          // W. Gander, G.H. Golub, R. Strebel, "Least-Squares Fitting of 
            // Circles and Ellipses", BIT, 34, pp. 558-578, 1994.
            // The circle they got with an algebraic distance is:
            //       (centerX = 5.3794, centerY = 7.2532, r = 3.0370).
	          // The circle they got with a geometric distance is: 
            //       (centerX = 4.7398, centerY = 2.9835, r = 4.7142).
            //The algebaric estimate we obtain is much closer to their geometric
            //one, as a consequence the improvement between algebraic and 
            //geometric is not as dramatic as in their work. This is due to the
            //difference between our algebraic formulation and theirs.
	pnt[0] = 1; pnt[1] = 7;
	pointData.push_back( pnt );
	pnt[0] = 2; pnt[1] = 6;
	pointData.push_back( pnt );
	pnt[0] = 5; pnt[1] = 8;
	pointData.push_back( pnt );
	pnt[0] = 7; pnt[1] = 7;
	pointData.push_back( pnt );
	pnt[0] = 9; pnt[1] = 5;
	pointData.push_back( pnt );
	pnt[0] = 3; pnt[1] = 7;
	pointData.push_back( pnt );
	double sum,err;
	int numPoints = pointData.size();
      
	out<<"Circle parameters from data found in the paper by Gander et al. :\n\n";

	cpEstimator->SetLeastSquaresType( itk::SphereParametersEstimator<2>::ALGEBRAIC );
	cpEstimator->LeastSquaresEstimate( pointData, circleParameters );
	if( circleParameters.size() == 0 ) {
		out<<"Algebraic least squares circle parameters [c_x,c_y,r]: ";
    out<<"DEGENERATE CONFIGURATION\n\n";
  }
	else {
		sum = 0.0;
		for( i=0; i<numPoints; i++ ) {
			Point2D &p = pointData[i];
			err = circleParameters[2] - sqrt((p[0] - circleParameters[0])*
                                       (p[0] - circleParameters[0]) + 
                                       (p[1] - circleParameters[1])*
                                       (p[1] - circleParameters[1]));
			sum += err*err;
		}
		out<<"\tAlgebraic least squares circle parameters [c_x,c_y,r]:\n\t\t [ ";
    out<<circleParameters[0]<<", "<<circleParameters[1]<<", ";
		out<<circleParameters[2]<<" ]\n";
		out<<"\tSum of squared errors: "<<sum<<"\n\n";
		circleParameters.clear();
	}

	cpEstimator->SetLeastSquaresType( itk::SphereParametersEstimator<2>::GEOMETRIC );
	cpEstimator->LeastSquaresEstimate( pointData, circleParameters );
	if( circleParameters.size() == 0 ) {
		out<<"Geometric least squares circle parameters [c_x,c_y,r]: ";
    out<<"DEGENERATE CONFIGURATION\n\n";
  }
	else {
		sum = 0.0;
		for( i=0; i<numPoints; i++ ) {
			Point2D &p = pointData[i];
			err = circleParameters[2] - sqrt((p[0] - circleParameters[0])*
                                       (p[0] - circleParameters[0]) + 
                                       (p[1] - circleParameters[1])*
                                       (p[1] - circleParameters[1]));
			sum += err*err;
		}
		out<<"\tGeometric least squares circle parameters [c_x,c_y,r]:\n\t\t [ ";
    out<<circleParameters[0]<<", "<<circleParameters[1]<<", ";
		out<<circleParameters[2]<<" ]\n";
		out<<"\tSum of squared errors: "<<sum<<"\n";
	}
	pointData.clear();
  return ( succeedExact && succeedAlgebraic && succeedGeometric );
}


template<unsigned int dimension>
bool TestnD( std::ostream &out )
{
  typedef itk::Point<double,dimension> PointnD;
  unsigned int i;
  unsigned int numPoints = 10*(dimension+1);
  std::vector< PointnD > data, cleanData;
  itk::Vector<double,dimension> tmp;
  std::vector<double> trueSphereParameters, sphereParameters;
  double noiseSigma = 1.0; //noise standard deviation
  std::ostringstream titleStr;
  bool succeedExact, succeedAlgebraic, succeedGeometric;

	typename itk::SphereParametersEstimator<dimension>::Pointer estimator;
  estimator = itk::SphereParametersEstimator<dimension>::New();
  estimator->SetDelta( 0.5 );

  GenerateData<dimension>( numPoints, noiseSigma, data, cleanData,
                           trueSphereParameters );

  out<<"Known (hyper)sphere parameters [c,r]\n\t [ ";
  for( i=0; i<dimension; i++ )
    out<<trueSphereParameters[i]<<", ";
  out<<trueSphereParameters[i]<<"]\n\n";

           //estimate using minimal number of points, use clean (no noise) data
           //the reason we use the "clean" data is that otherwise the estimates
           //are often far from the correct values
  estimator->Estimate( cleanData, sphereParameters );  
  titleStr<<"Sphere going through "<<dimension+1<<" points [c,r], no noise:\n\t";
  succeedExact =
    CompareAndPrint<dimension>( titleStr.str(), 3*noiseSigma, trueSphereParameters,
                                sphereParameters, out );

                  //estimate using algebraic least squares
  estimator->SetLeastSquaresType( itk::SphereParametersEstimator<dimension>::ALGEBRAIC );
  estimator->LeastSquaresEstimate( data, sphereParameters );  
  succeedAlgebraic =
    CompareAndPrint<dimension>( "Algebraic least squares estimate [c,r]:\n\t", 
                                3*noiseSigma, trueSphereParameters,
                                sphereParameters, out );

                 //estimate using geometric least squares
  estimator->SetLeastSquaresType( itk::SphereParametersEstimator<dimension>::GEOMETRIC );
  estimator->LeastSquaresEstimate( data, sphereParameters );
  succeedGeometric =
    CompareAndPrint<dimension>( "Geometric least squares estimate [c,r]:\n\t", 
                                3*noiseSigma, trueSphereParameters,
                                sphereParameters, out );

  return ( succeedExact && succeedAlgebraic && succeedGeometric );
}


template<unsigned int dimension>
void GenerateData( unsigned int numPoints, double noiseStandardDeviation,
                   std::vector< itk::Point<double,dimension> > &data,
                   std::vector< itk::Point<double,dimension> > &cleanData,
                   std::vector<double> &sphereParameters )
{
  itk::Vector<double, dimension> tmp, noise;
  itk::Point<double, dimension> sphereCenter, randomPoint;
  double sphereRadius;
  double coordinateMax = 1000.0;
  unsigned int i, j;

  RandomNumberGenerator random;

  sphereParameters.clear();
         //random (hyper) sphere
  for( i=0; i<dimension; i++ ) {
    sphereCenter[i] = random.uniform( -coordinateMax, coordinateMax );
  }
  sphereRadius = random.uniform(0.0, coordinateMax);

  for( i=0; i<dimension; i++ ) 
    sphereParameters.push_back( sphereCenter[i] );
  sphereParameters.push_back( sphereRadius );
           //generate points    
  for( i=0; i<numPoints; i++ ) {
    for( j=0; j<dimension; j++ ) {
      tmp[j] = random.uniform( -1.0, 1.0 );
      noise[j] = random.normal( noiseStandardDeviation );
    }
            //project random point onto the sphere and add noise
    tmp.Normalize();
    randomPoint = sphereCenter + tmp*sphereRadius;
    cleanData.push_back( randomPoint );
    randomPoint = sphereCenter + noise + tmp*sphereRadius;
    data.push_back( randomPoint );
  }

}


template<unsigned int dimension>
bool CompareAndPrint( const std::string &title, double threshold, 
                      std::vector<double> &knownSphere,
                      std::vector<double> &estimatedSphere, 
                      std::ostream &out )
{
  unsigned int i;

  out.setf(std::ios_base::fixed, std::ios_base::floatfield);
  out.precision(2);

  out<<title;

  if( estimatedSphere.size() == 0 )
    out<<"DEGENERATE CONFIGURATION\n\n";

  if( knownSphere.size()-1 != dimension ||
      estimatedSphere.size()-1 != dimension ||
      knownSphere.size() != estimatedSphere.size() )
    return false;

  out<<"[";
  for( i=0; i<dimension; i++ )
    out<<estimatedSphere[i]<<", ";
  out<<estimatedSphere[i]<<"]\n";
   
  itk::Vector<double,dimension> tmp;
  for( i=0; i<dimension; i++ )
    tmp[i] = estimatedSphere[i] - knownSphere[i];
  out<<"\t Distance between estimated and known sphere centers [0=correct]: ";
  out<<tmp.GetNorm()<<"\n";
  out<<"\t Difference between estimated and known sphere radius [0=correct]: ";
  out<<fabs( estimatedSphere[dimension] - knownSphere[dimension] )<<"\n\n";

  if( tmp.GetNorm() > threshold || 
      (estimatedSphere[dimension] - knownSphere[dimension]) > threshold )
    return false;

  return true;
}