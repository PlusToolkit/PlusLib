/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __itkUlteriusImageIOFactory_h
#define __itkUlteriusImageIOFactory_h

#include "itkObjectFactoryBase.h"
#include "itkImageIOBase.h"

namespace itk
{
/** \class UlteriusImageIOFactory
 * \brief Create instances of UlteriusImageIO objects using an object factory.
 */
class UlteriusImageIOFactory : public ObjectFactoryBase
{
public:  
  /** Standard class typedefs. */
  typedef UlteriusImageIOFactory       Self;
  typedef ObjectFactoryBase        Superclass;
  typedef SmartPointer<Self>       Pointer;
  typedef SmartPointer<const Self> ConstPointer;
  
  /** Class methods used to interface with the registered factories. */
  virtual const char* GetITKSourceVersion() const;
  virtual const char* GetDescription() const;
  
  /** Method for class instantiation. */
  itkFactorylessNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(UlteriusImageIOFactory, ObjectFactoryBase);

  /** Register one factory of this type  */
  static void RegisterOneFactory()
    {
    Pointer ulteriusFactory = New();
    ObjectFactoryBase::RegisterFactory(ulteriusFactory);
    }

protected:
  UlteriusImageIOFactory();
  ~UlteriusImageIOFactory();

private:
  UlteriusImageIOFactory(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

};
 
} // end namespace itk

#endif
