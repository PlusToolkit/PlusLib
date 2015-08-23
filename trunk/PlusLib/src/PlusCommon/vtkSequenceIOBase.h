/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkSequenceIOBase_h
#define __vtkSequenceIOBase_h

#include "PlusCommon.h"
#include "vtkPlusCommonExport.h"

class vtkTrackedFrameList;

/*!
  \class vtkSequenceIOBase
  \brief Class to abstract away specific sequence file read/write details
  \ingroup PlusLibCommon
*/
class vtkPlusCommonExport vtkSequenceIOBase : public vtkObject
{
public:
  /*! Append image data to the sequence, compression not allowed */
  virtual PlusStatus AppendImages() = 0;

  /*! 
    Append the frames in tracked frame list to the header, if the onlyTrackerData flag is true it will not save
    in the header the image data related fields. 
  */
  virtual PlusStatus AppendImagesToHeader() = 0;

  /*! Flag to enable/disable compression of image data */
  virtual void SetUseCompression(bool useCompression) = 0;

  /*! Set the TrackedFrameList where the images are stored */
  virtual void SetTrackedFrameList(vtkTrackedFrameList *trackedFrameList) = 0;

  /*!
    Set input/output file name. The file contains only the image header in case of
    MHD images and the full image (including pixel data) in case of MHA images.
  */
  virtual PlusStatus SetFileName(const std::string& aFilename) = 0;

  /*! Update the number of frames in the header 
      This is used primarly by vtkVirtualDiscCapture to update the final tally of frames, as it continually appends new frames to the file
      /param numberOfFrames the new number of frames to write
      /param addPadding this should only be true if this is the first time this function is called, which typically happens in OpenImageHeader
  */
  virtual PlusStatus OverwriteNumberOfFramesInHeader(int numberOfFrames, bool addPadding=false) = 0;

  /*! Update a field in the image header with its current value */
  virtual PlusStatus UpdateFieldInImageHeader(const char* fieldName) = 0;

  /*! Finalize the header */
  virtual PlusStatus FinalizeHeader() = 0;

  /*! Close the sequence */
  virtual PlusStatus Close() = 0;

  /*! Close the sequence without saving anything (temporary files are deleted) */
  virtual PlusStatus Discard() = 0;

  /*! Get the TrackedFrameList where the images are stored */
  virtual vtkTrackedFrameList* GetTrackedFrameList() = 0;

  /*! Prepare the sequence for writing */
  virtual PlusStatus PrepareHeader() = 0;
protected:
  vtkSequenceIOBase(){};
  virtual ~vtkSequenceIOBase(){};
};

#endif // __vtkSequenceIOBase_h 
