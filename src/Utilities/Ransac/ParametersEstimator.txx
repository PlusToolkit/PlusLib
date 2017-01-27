#ifndef _PARAMETERS_ESTIMATOR_TXX_
#define _PARAMETERS_ESTIMATOR_TXX_

namespace itk {

template<class T, class S>
void ParametersEstimator<T,S>::SetMinimalForEstimate( unsigned int minForEstimate )
{
  if( minForEstimate == 0 )
    throw ExceptionObject(__FILE__,__LINE__,
                          "Invalid minimal number of objects for exact estimate.");

  this->minForEstimate = minForEstimate;
}


template<class T, class S>
unsigned int ParametersEstimator<T,S>::GetMinimalForEstimate()
{
  return this->minForEstimate;
}


} // end namespace itk

#endif //_PARAMETERS_ESTIMATOR_TXX_
