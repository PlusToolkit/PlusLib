This is the source code for a C++ templated implementation of the RANSAC
algorithm. The implementation is multi-threaded. In addition to the RANSAC
algorithm the code includes estimators for two parametric entities, n
dimensional planes and spheres. Example programs showing the use of the RANSAC
algorithm combined with the parameter estimators are also given. Testing
programs are only provided for the two parameter estimators.

The code is "in the style of ITK". That is, it is very similar to the official
ITK style but does not follow all of the required conventions.

Manifest:

RANSAC.{h,txx} - Multi-threaded implementation of the generic RANSAC algorithm.

ParametersEstimator.{h,txx} - Super class of all parameter estimation objects
that can be used with the RANSAC algorithm. This is an abstract class that
defines an interface.

PlaneParametersEstimator.{h,txx} - Estimation code for n-dimensional planes.

SphereParametersEstimator.{h,txx} - Estimation code for n-dimensional spheres.

Examples/{sphereEstimation.cxx,planeEstimation.cxx} - Example programs combining
RANSAC with the parameter estimators. The dimensionality is hard coded (set to 3).
The programs work for any dimensionality, just change the DIMENSION constant. For
DIMENSION==3 the programs have a side effect of writing two open inventor scene
files corresponding to the least squares and RANSAC based estimates.

Testing/*.cxx - Tests of the two parameter estimators.

Utility/coin3dSceneViewer.cxx - Viewer for open inventor scene files. Requires Coin3D
and SoWin libraries (www.coin3d.org)

Common/RandomNumberGenerator.h - Wrapper for the vnl random number generator. Used by
the testing code and the example code.
