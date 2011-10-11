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

class PLUS_EXPORT FcsvWriter : public Object
{
public:

  // SmartPointer typedef support 
  typedef FcsvWriter						Self;									// correct????
  typedef SmartPointer<Self>                Pointer;

  // Method for creation through the object factory 
  itkNewMacro(Self);

  // Run-time type information (and related methods). 
  typedef Object Superclass;
  itkTypeMacro(FcsvWriter, Object);

  //Load a FCSV file.
  void Update(void);

  // Set the filename  
  itkSetStringMacro(FileName);

  // Get the filename 
  itkGetStringMacro(FileName);

  FcsvData* GetFcsvDataObject() { return &m_FcsvDataObject; };
protected:
  FcsvWriter(const Self&);		//purposely not implemented
  Self& operator=(const Self&);	//purposely not implemented

  std::string m_FileName;

  FcsvWriter();
  virtual ~FcsvWriter();

private:
  FcsvData m_FcsvDataObject;
};


} // namespace itk

#endif // __FcsvWriter_h
