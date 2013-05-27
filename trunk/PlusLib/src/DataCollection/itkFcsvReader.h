/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __itkFcsvReader_h
#define __itkFcsvReader_h

#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif

#define PLUS_EXPORTS

#include "vtkPlusWin32Header.h"

#include <vector>
#include <fstream>
#include "itkProcessObject.h"
#include "itkSpatialObject.h"
#include "itkMetaEvent.h"
#include <itkDefaultStaticMeshTraits.h>
#include "itkSpatialObjectReader.h"

#include "itkImageIOBase.h"

namespace itk
{
/*!
  \class FcsvPoint
  \brief Describes a single fiducial point
  \ingroup PlusLibImageAcquisition
*/
class FcsvPoint 
{
  public:
    std::string label;
    double position[3];
    int selected;  
    int visibility;  
};

/*!
  \class FcsvData
  \brief Describes a fiducial list
  \ingroup PlusLibImageAcquisition
*/
struct FcsvData
{
    std::string filePath;  // L.fcsv
    std::string name;    //= L
    int numPoints;      //= 16
    int symbolScale;    // = 5
    int symbolType;      // = 11
    int visibility;      //= 1
    float version;      //= 2
    float textScale;    //= 4.5
    float color[3];      //= 0.4,1,1 
    float selectedColor[3]; //= 1,0.5,0.5  
    float opacity;      //= 1 
    float ambient;      //= 0
    float diffuse;      //= 1  
    float specular;      //= 0  
    float power;      //= 1
    int   locked;      //= 0
    std::vector<std::string> columns;      //= label,x,y,z,sel,vis
    int numberingScheme;  //= 0
    std::vector<FcsvPoint> points;        
};

static const int FcsvNDimensions=3;

/*!
  \class FcsvReader
  \brief Reads a fiducial list (fcsv) file 
  The fcsv file is the standard file format of 3D Slicer for storing a fiducial list
  \ingroup PlusLibImageAcquisition
*/
class PLUS_EXPORT FcsvReader : public Object
{
public:

  /*! SmartPointer typedef support */
  typedef FcsvReader Self;
  /*! SmartPointer typedef support */
  typedef SmartPointer<Self> Pointer;

  /*! Method for creation through the object factory */
  itkNewMacro(Self);

  /*! For run-time type information */
  typedef Object Superclass;
  /*! For run-time type information */
  itkTypeMacro(FcsvReader, Object);

  /*! Load a FCSV file */
  void Update(void);

  /*! Set the filename */
  itkSetStringMacro(FileName);
  /*! Get the filename */
  itkGetStringMacro(FileName);

  FcsvData* GetFcsvDataObject() { return &m_FcsvDataObject; };

protected:
  FcsvReader();
  virtual ~FcsvReader();

  void StripCharsFromString(std::string& str, const std::string& chars);
  void ProcessHeaderEntry( const std::string &headerEntry, const std::string& headerEntryValue );

  std::string m_FileName;
  FcsvData m_FcsvDataObject;

private:
  FcsvReader(const Self&);    //purposely not implemented
  Self& operator=(const Self&);  //purposely not implemented
};

} // namespace itk

#endif // __FcsvReader_h
