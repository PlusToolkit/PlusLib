/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkFcsvReader_h
#define __vtkFcsvReader_h

// Local includes
#include "vtkPlusDataCollectionExport.h"

// VTK includes
#include <vtkObject.h>

// STL includes
#include <vector>
#include <stdexcept>

/*!
  \class FcsvPoint
  \brief Describes a single fiducial point
  \ingroup PlusLibDataCollection
*/
class vtkPlusDataCollectionExport FcsvPoint
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
  \ingroup PlusLibDataCollection
*/
struct vtkPlusDataCollectionExport FcsvData
{
  std::string                 filePath;         // L.fcsv
  std::string                 name;             //= L
  int                         numPoints;        //= 16
  int                         symbolScale;      // = 5
  int                         symbolType;       // = 11
  int                         visibility;       //= 1
  float                       version;          //= 2
  float                       textScale;        //= 4.5
  float                       color[3];         //= 0.4,1,1
  float                       selectedColor[3]; //= 1,0.5,0.5
  float                       opacity;          //= 1
  float                       ambient;          //= 0
  float                       diffuse;          //= 1
  float                       specular;         //= 0
  float                       power;            //= 1
  int                         locked;           //= 0
  std::vector<std::string>    columns;          //= label,x,y,z,sel,vis
  int                         numberingScheme;  //= 0
  std::vector<FcsvPoint>      points;
};

/*!
  \class FcsvReader
  \brief Reads a fiducial list (fcsv) file
  The fcsv file is the standard file format of 3D Slicer for storing a fiducial list
  \ingroup PlusLibDataCollection
*/
class vtkPlusDataCollectionExport vtkFcsvReader : public vtkObject
{
public:
  static vtkFcsvReader* New();
  vtkTypeMacro(vtkFcsvReader, vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);

  /*! Load a FCSV file */
  void Update(void);

  /*! Set the filename */
  vtkSetMacro(FileName, std::string);
  /*! Get the filename */
  vtkGetMacro(FileName, std::string);

  const FcsvData& GetFcsvDataObject()
  {
    return FcsvDataObject;
  };
  FcsvPoint& GetFcsvPoint(unsigned int index)
  {
    if (index < FcsvDataObject.points.size())
    {
      return FcsvDataObject.points[index];
    }
    throw std::out_of_range("Point out of range.");
  }

protected:
  vtkFcsvReader();
  virtual ~vtkFcsvReader();

  void StripCharsFromString(std::string& str, const std::string& chars);
  void ProcessHeaderEntry(const std::string& headerEntry, const std::string& headerEntryValue);

  std::string FileName;
  FcsvData    FcsvDataObject;

private:
  vtkFcsvReader(const vtkFcsvReader&);              //purposely not implemented
  vtkFcsvReader& operator=(const vtkFcsvReader&);   //purposely not implemented
};

#endif // __FcsvReader_h