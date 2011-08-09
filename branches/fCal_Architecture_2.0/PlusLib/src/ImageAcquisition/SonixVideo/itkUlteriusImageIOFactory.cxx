#include "itkUlteriusImageIOFactory.h"
#include "itkCreateObjectFunction.h"
#include "itkUlteriusImageIO.h"
#include "itkVersion.h"

  
namespace itk
{
UlteriusImageIOFactory::UlteriusImageIOFactory()
{
  this->RegisterOverride("itkImageIOBase",
                         "itkUlteriusImageIO",
                         "Ulterius Image IO",
                         1,
                         CreateObjectFunction<UlteriusImageIO>::New());
}
  
UlteriusImageIOFactory::~UlteriusImageIOFactory()
{
}

const char* 
UlteriusImageIOFactory::GetITKSourceVersion() const
{
  return ITK_SOURCE_VERSION;
}

const char* 
UlteriusImageIOFactory::GetDescription() const
{
  return "Ulterius ImageIO Factory, allows the loading of Ulterius images into insight";
}

} // end namespace itk
