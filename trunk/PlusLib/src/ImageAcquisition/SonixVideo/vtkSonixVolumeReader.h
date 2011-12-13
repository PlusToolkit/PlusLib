/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef _VTKSONIXVOLUMEREADER_H_
#define _VTKSONIXVOLUMEREADER_H_

#include "PlusConfigure.h"
#include "vtkImageAlgorithm.h" 

#include <vtkstd/vector>
#include <vtkstd/string>

#include "ulterius_def.h"

#if PLUS_ULTRASONIX_SDK_MAJOR_VERSION == 1
#include "utx_imaging_modes.h"
#endif

class ulterius; 

/*!
  \class vtkSonixVolumeReader 
  \brief Reads a volume from file to vtkImageData
  \ingroup PlusLibImageAcquisition
*/ 
class VTK_EXPORT vtkSonixVolumeReader: public vtkImageAlgorithm
{
public:
	vtkTypeRevisionMacro(vtkSonixVolumeReader,vtkImageAlgorithm);
	static vtkSonixVolumeReader *New();
	virtual void PrintSelf(ostream& os, vtkIndent indent);

  /*! Specify file name for the volume file. */
	virtual void SetFileName(vtkstd::string fileName) { this->FileName = fileName; }
  /*! Get volume file name. */
	virtual vtkstd::string GetFileName() { return this->FileName; }

  /*! Get the number of frames  */
	vtkGetMacro(NumberOfFrames, int); 

  /*! Get the frame width */
	vtkGetMacro(FrameWidth, int); 

  /*! Get the frame height */
	vtkGetMacro(FrameHeight, int); 

  /*! Get the data sample size in bits  */
	vtkGetMacro(DataSampleSize, int); 

  /*! Get the data type  */
	vtkGetMacro(DataType, int); 

  /*! Read a volume at Ultrasonix format (.b8, .b32, .bpr) */
	virtual int ReadVolume(); 
  /*! Read a volume at Ultrasonix format (.b8, .b32, .bpr) */
	virtual int ReadVolume(const char* filename); 

  /*! Get a frame from volume */
	virtual vtkImageData *GetFrame(int ImageNumber = 0);

  /*! Get all frames from volume by vector of vtkImageData */
	virtual vtkstd::vector<vtkImageData *> *GetAllFrames(); 

  /*! Save frame as TIFF */
	virtual PlusStatus WriteFrameAsTIFF(int imageNumber, const char* filePrefix, const char* directory = "./"); 
  /*! Save all frames as TIFF */
	virtual PlusStatus WriteAllFramesAsTIFF(const char* filePrefix, const char* directory = "./"); 


protected:
	/*! Constructor */
	vtkSonixVolumeReader();
	/*! Destructor */
	~vtkSonixVolumeReader();

  /*! name of the input file containing the volume */
	vtkstd::string	FileName;
  /*! Number of frames */
	int		NumberOfFrames; 
  /*! Frame width */
	int		FrameWidth; 
  /*! Frame height */
	int		FrameHeight; 
  /*! Data sample size */
	int		DataSampleSize; 
  /*! Data type */
	int		DataType; 

  /*! Image data vector */
	vtkstd::vector<vtkImageData*> ImageDataVector; 

private:
	vtkSonixVolumeReader(const vtkSonixVolumeReader&);
	void operator=(const vtkSonixVolumeReader&);

}; 

#endif 
