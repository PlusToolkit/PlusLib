/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef _vtkPlusSONIXVOLUMEREADER_H_
#define _vtkPlusSONIXVOLUMEREADER_H_

#include "PlusConfigure.h"
#include "vtkPlusDataCollectionExport.h"

#include "vtkImageAlgorithm.h" 

//class vtkIGSIOTrackedFrameList; 

/*!
  \class vtkPlusSonixVolumeReader 
  \brief Reads a volume from file to tracked frame list
  \ingroup PlusLibDataCollection
*/ 
class vtkPlusDataCollectionExport vtkPlusSonixVolumeReader: public vtkImageAlgorithm
{
public:
  static vtkPlusSonixVolumeReader *New();
  vtkTypeMacro(vtkPlusSonixVolumeReader,vtkImageAlgorithm);
  virtual void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

  /*! Read a volume from Ultrasonix format (.b8, .b32, .bpr, .rf) and convert it to tracked frame */
  static PlusStatus GenerateTrackedFrameFromSonixVolume(const char* volumeFileName, vtkIGSIOTrackedFrameList* trackedFrameList, double acquisitionFrameRate = 10); 

protected:
  /*! Constructor */
  vtkPlusSonixVolumeReader();
  /*! Destructor */
  ~vtkPlusSonixVolumeReader();
private:
  vtkPlusSonixVolumeReader(const vtkPlusSonixVolumeReader&);
  void operator=(const vtkPlusSonixVolumeReader&);
}; 

#endif 
