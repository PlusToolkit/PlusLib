

// ITK includes
#include <itkFiducialTransformComputation.h>
itkFiducialTransformComputation::itkFiducialTransformComputation()
{
  this->RmsError = -1;
  this->ImageToProbeTransformMatrixVnl = NULL;
}


double itkFiducialTransformComputation::computeSymmetricScale(const std::vector<itk::Point<double, 3> >& fixedPoints,
                                                              const std::vector<itk::Point<double, 3> >& movingPoints,
                                                              const itk::Point<double, 3>& fixedcenter,
                                                              const itk::Point<double, 3>& movingcenter)
{
  std::vector<double> centeredFixedPoints(fixedPoints.size(), 0.0);
  std::vector<double> centeredMovingPoints(movingPoints.size(), 0.0);

  std::transform(fixedPoints.begin(), fixedPoints.end(),
    centeredFixedPoints.begin(),
    SquaredPointDistance(fixedcenter) );

  std::transform(movingPoints.begin(), movingPoints.end(),
    centeredMovingPoints.begin(),
    SquaredPointDistance(movingcenter) );

  double fixedmag = 0.0, movingmag = 0.0;
  fixedmag = std::accumulate(centeredFixedPoints.begin(),
    centeredFixedPoints.end(),
    fixedmag);

  movingmag = std::accumulate(centeredMovingPoints.begin(),
    centeredMovingPoints.end(),
    movingmag);

  return sqrt(movingmag / fixedmag);
}

bool itkFiducialTransformComputation::computeTransform(std::vector<itk::Point<double, 3> > fixedPoints,
                                                       std::vector<itk::Point<double, 3> > movingPoints,
                                                       std::string transformType)
{ 


  // Our input into landmark based initialize will be of this form
  // The format for saving to slicer is defined later
  typedef itk::Similarity3DTransform<double> SimilarityTransformType;
  SimilarityTransformType::Pointer transform = SimilarityTransformType::New();

  //itk::Matrix<double,4,4> initialTransform(ImageToProbeSeedTransformMatrixVnl);
  //transform->SetMatrix(ImageToProbeSeedTransformMatrixVnl);


  transform->SetIdentity();
  // workaround a bug in older versions of ITK
  transform->SetScale(1.0);


  typedef itk::LandmarkBasedTransformInitializer<SimilarityTransformType,
    itk::Image<short, 3>, itk::Image<short, 3> > InitializerType;
  InitializerType::Pointer initializer = InitializerType::New();

  // This expects a VersorRigid3D.  The similarity transform works because
  // it derives from that class
  initializer->SetTransform(transform);

  initializer->SetFixedLandmarks(fixedPoints);
  initializer->SetMovingLandmarks(movingPoints);

  /* The transform computed gives the best fit transform that maps the fixed and moving images in a least squares sense. */
  //    The solution is based on
  //    Berthold K. P. Horn (1987),
  //    "Closed-form solution of absolute orientation using unit quaternions,"
  //    Journal of the Optical Society of America A, 4:629-642
  //
  //
  //    Original python implementation by David G. Gobbi
  //    Readpted from the code in VTK: Hybrid/vtkLandmarkTransform
  initializer->InitializeTransform();


  // Handle different transform types.

  if( transformType == "Translation" )
  {
    // Clear out the computed rotaitoin if we only requested translation
    itk::Versor<double> v;
    v.SetIdentity();
    transform->SetRotation(v);
  }
  else if( transformType == "Rigid" )
  {
    // do nothing
  }
  else if( transformType == "Similarity" )
  {
    // Compute the scaling factor and add that in
    itk::Point<double, 3> fixedCenter(transform->GetCenter() );
    itk::Point<double, 3> movingCenter(transform->GetCenter() + transform->GetTranslation() );

    double s = computeSymmetricScale(fixedPoints, movingPoints,
      fixedCenter, movingCenter);
    transform->SetScale(s);
    this->ScaleFactor = 1/s;
  }
  else if( transformType == "Affine" )
  {
    // itk::Matrix<double, 3> a = computeAffineTransform(fixedPoints, movingPoints, fixedCenter, movingCenter);
    std::cerr << "Unsupported transform type: " << transformType << std::endl;
    // return EXIT_FAILURE;
  }
  else
  {
    std::cerr << "Unsupported transform type: " << transformType << std::endl;
  }


  // Convert into an affine transform for saving to Slicer.

  typedef itk::AffineTransform<double, 3> AffineTransform;
  AffineTransform::Pointer fixedToMovingT = itk::AffineTransform<double, 3>::New();

  fixedToMovingT->SetCenter( transform->GetCenter() );
  fixedToMovingT->SetMatrix( transform->GetMatrix() );
  fixedToMovingT->SetTranslation( transform->GetTranslation() );


  // Compute RMS error in the target coordinate system.

  AffineTransform::Pointer movingToFixedT = AffineTransform::New();
  fixedToMovingT->GetInverse( movingToFixedT );

  typedef InitializerType::LandmarkPointContainer LandmarkPointContainerType;

  typedef LandmarkPointContainerType::const_iterator PointsContainerConstIterator;
  PointsContainerConstIterator mitr = movingPoints.begin();
  PointsContainerConstIterator fitr = fixedPoints.begin();

  SimilarityTransformType::OutputVectorType                 errortr;
  SimilarityTransformType::OutputVectorType::RealValueType  sum;
  InitializerType::LandmarkPointType                        movingPointInFixed;
  int                                                       counter;

  sum = itk::NumericTraits< SimilarityTransformType::OutputVectorType::RealValueType >::ZeroValue();
  counter = itk::NumericTraits< int >::ZeroValue();

  while( mitr != movingPoints.end() ) 
  {
    movingPointInFixed = movingToFixedT->TransformPoint( *mitr );
    errortr = *fitr - movingPointInFixed;
    sum = sum + errortr.GetSquaredNorm();
    ++mitr;
    ++fitr;
    counter++;
  }

  this->RmsError = sqrt( sum / counter );
  vnl_matrix<double> transformVnl(4,4);
  transform->SetIdentity();
  // set the rotation
  transformVnl.update(movingToFixedT->GetMatrix().GetVnlMatrix(),0,0);

  vnl_vector<double> translation = movingToFixedT->GetTranslation().Get_vnl_vector();
  //set the tranlation
  transformVnl(0,3)=translation(0);
  transformVnl(1,3)=translation(1);
  transformVnl(2,3)=translation(2);

  // set the last row
  transformVnl(3,0)=0;
  transformVnl(3,1)=0;
  transformVnl(3,2)=0;
  transformVnl(3,3)=1;
  this->ImageToProbeTransformMatrixVnl = transformVnl;

  return PLUS_SUCCESS;
}
