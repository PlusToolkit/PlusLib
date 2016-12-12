/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkFcsvWriter_h
#define __vtkFcsvWriter_h

// Local includes
#include "vtkPlusDataCollectionExport.h"
#include "vtkFcsvReader.h"

// VTK includes
#include <vtkObject.h>

/*!
  \class FcsvWriter
  \brief Writes a fiducial list (fcsv) file
  The fcsv file is the standard file format of 3D Slicer for storing a fiducial list
  \ingroup PlusLibDataCollection
*/

class vtkPlusDataCollectionExport vtkFcsvWriter : public vtkObject
{
public:
  static vtkFcsvWriter* New();
  vtkTypeMacro(vtkFcsvWriter, vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);

  /* Load an FCSV file */
  void Update(void);

  /*! Set the filename */
  vtkSetMacro(FileName, std::string);
  /*! Get the filename */
  vtkGetMacro(FileName, std::string);

  void CopyFcsvDataObject(const FcsvData& dataObject);
  const FcsvData& GetFcsvDataObject()
  {
    return FcsvDataObject;
  };

protected:
  vtkFcsvWriter();
  virtual ~vtkFcsvWriter();

  std::string FileName;
  FcsvData    FcsvDataObject;

private:
  vtkFcsvWriter(const vtkFcsvWriter&);              //purposely not implemented
  vtkFcsvWriter& operator=(const vtkFcsvWriter&);   //purposely not implemented
};

#endif // __FcsvWriter_h