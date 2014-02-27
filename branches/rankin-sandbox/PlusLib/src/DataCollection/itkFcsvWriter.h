/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __itkFcsvWriter_h
#define __itkFcsvWriter_h

#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif

#define PLUS_EXPORTS

#include "vtkPlusWin32Header.h"

#include <vector>
#include <fstream>
#include "itkProcessObject.h"

#include "itkFcsvReader.h"

#include "itkImageIOBase.h"

namespace itk
{

/*!
  \class FcsvWriter
  \brief Writes a fiducial list (fcsv) file 
  The fcsv file is the standard file format of 3D Slicer for storing a fiducial list
  \ingroup PlusLibDataCollection
*/

class PLUS_EXPORT FcsvWriter : public Object
{
public:

  /*! SmartPointer typedef support */
  typedef FcsvWriter Self;
  /*! SmartPointer typedef support */
  typedef SmartPointer<Self> Pointer;

  /*! Method for creation through the object factory */
  itkNewMacro(Self);

  /*! For run-time type information */
  typedef Object Superclass;
  /*! For run-time type information */
  itkTypeMacro(FcsvWriter, Object);

  /* Load an FCSV file */
  void Update(void);

  /*! Set the filename */
  itkSetStringMacro(FileName);
  /*! Get the filename */
  itkGetStringMacro(FileName);

  FcsvData* GetFcsvDataObject() { return &m_FcsvDataObject; };

protected:
  FcsvWriter();
  virtual ~FcsvWriter();

  std::string m_FileName;
  FcsvData m_FcsvDataObject;

private:
  FcsvWriter(const Self&);    //purposely not implemented
  Self& operator=(const Self&);  //purposely not implemented
};


} // namespace itk

#endif // __FcsvWriter_h
