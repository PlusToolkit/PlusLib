#ifndef _VTKSONIXVOLUMEREADER_H_
#define _VTKSONIXVOLUMEREADER_H_

#include "PlusConfigure.h"
#include "vtkImageAlgorithm.h" 

#include <vtkstd/vector>
#include <vtkstd/string>

#include "ulterius_def.h"

#if PLUS_ULTERIUS_MAJOR_VERSION == 1
#include "utx_imaging_modes.h"
#endif

class ulterius; 

class VTK_EXPORT vtkSonixVolumeReader: public vtkImageAlgorithm
{
public:
	vtkTypeRevisionMacro(vtkSonixVolumeReader,vtkImageAlgorithm);
	static vtkSonixVolumeReader *New();
	virtual void PrintSelf(ostream& os, vtkIndent indent);

	// Description:
	// Specify file name for the volume file.
	virtual void SetFileName(vtkstd::string fileName) { this->FileName = fileName; }
	virtual vtkstd::string GetFileName() { return this->FileName; }

	// Description:
	// Get the number of frames 
	vtkGetMacro(NumberOfFrames, int); 

	// Description:
	// Get the frame width
	vtkGetMacro(FrameWidth, int); 

	// Description:
	// Get the frame height
	vtkGetMacro(FrameHeight, int); 

	// Description:
	// Get the data sample size in bits 
	vtkGetMacro(DataSampleSize, int); 

	// Description:
	// Get the data type 
	vtkGetMacro(DataType, int); 

	// Description:
	// Read a volume at Ultrasonix format (.b8, .b32, .bpr)
	virtual int ReadVolume(); 
	virtual int ReadVolume(const char* filename); 

	// Description:
	// Get a frame from volume
	virtual vtkImageData *GetFrame(int ImageNumber = 0);

	// Description:
	// Get all frames from volume by vector of vtkImageData
	virtual vtkstd::vector<vtkImageData *> *GetAllFrames(); 

	// Description:
	// Save frame as TIFF
	virtual void WriteFrameAsTIFF(int imageNumber, const char* filePrefix, const char* directory = "./"); 
	virtual void WriteAllFramesAsTIFF(const char* filePrefix, const char* directory = "./"); 


protected:
	vtkSonixVolumeReader();
	~vtkSonixVolumeReader();

	vtkstd::string	FileName;
	int		NumberOfFrames; 
	int		FrameWidth; 
	int		FrameHeight; 
	int		DataSampleSize; 
	int		DataType; 

	vtkstd::vector<vtkImageData*> ImageDataVector; 

private:
	vtkSonixVolumeReader(const vtkSonixVolumeReader&);
	void operator=(const vtkSonixVolumeReader&);

}; 

#endif 