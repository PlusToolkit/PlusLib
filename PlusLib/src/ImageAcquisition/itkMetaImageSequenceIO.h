#ifndef __vtkMetaImageSequenceIO_h
#define __vtkMetaImageSequenceIO_h

#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif 

#define PLUS_EXPORTS

#include "vtkPlusWin32Header.h"
#include "UsImageConverterCommon.h"

#include "itkImageIOBase.h"
#include "vtkMatrix4x4.h"

class vtkTrackedFrameList;
class TrackedFrame;

// The following coordinate frames are used with this metafile: 
// Image: image frame coordinate system, origin is the bottom-left corner, unit is pixel
// Tool: coordinate system of the DRB attached to the probe, unit is mm
// Reference: coordinate system of the DRB attached to the reference body, unit is mm
// Tracker: coordinate system of the tracker, unit is mm
// World: world coordinate system, orientation is usually patient RAS, unit is mm
//
// ImageToToolTransform: calibration matrix
// ToolToTrackerTransform: for each frame, pose of the image in the tracker coordinate system
// ReferenceToTrackerTransform: for each frame, pose of the reference body in the tracker coordinate system
// TrackerToWorldTransform: rarely used 

/** \class MetaImageIO
*
*  \brief Read MetaImage file format.
*
*  \ingroup IOFilters
*/

class PLUS_EXPORT vtkMetaImageSequenceIO : public vtkObject
{
public:

	static vtkMetaImageSequenceIO *New();
	vtkTypeRevisionMacro(vtkMetaImageSequenceIO, vtkObject);
	virtual void PrintSelf(ostream& os, vtkIndent indent);

  //! Get/Set the TrackedFrameList where the images are stored
  virtual void SetTrackedFrameList(vtkTrackedFrameList *trackedFrameList);
  vtkGetObjectMacro(TrackedFrameList, vtkTrackedFrameList); 

  // Set/get the default frame transform name 
  const char* GetDefaultFrameTransformName();
  void SetDefaultFrameTransformName(const std::string& strTransform);

  // Set/get the ultrasound image orientation for file storage.
  // Note that the image data is always stored in MF orientation in the TrackedFrameList.
  // The ultrasound image axes are defined as follows:
  // - x axis: points towards the x coordinate increase direction
  // - y axis: points towards the y coordinate increase direction
  // The image orientation can be defined by specifying which transducer axis corresponds to the x and y image axes, respectively.
  // There are four possible orientations:
  // - UF: image x axis = unmarked transducer axis, image y axis = far transducer axis
  // - UN: image x axis = unmarked transducer axis, image y axis = near transducer axis
  // - MF: image x axis = marked transducer axis, image y axis = far transducer axis
  // - MN: image x axis = marked transducer axis, image y axis = near transducer axis
  vtkSetMacro(ImageOrientationInFile, US_IMAGE_ORIENTATION);

  virtual PlusStatus Write();

  virtual PlusStatus Read();

  virtual bool CanReadFile(const char*);

  virtual TrackedFrame* GetTrackedFrame(int frameNumber);

  //! Set input/output file name. The file contains only the image header in case of
  // MHD images and the full image (including pixel data) in case of MHA images.
  vtkSetStringMacro(FileName); 
	vtkGetStringMacro(FileName); 

  //! Flag to enable/disable compression of image data
  // Not yet supported.
	vtkGetMacro(UseCompression, bool);
	vtkSetMacro(UseCompression, bool);
	vtkBooleanMacro(UseCompression, bool);

protected:
  vtkMetaImageSequenceIO();
  virtual ~vtkMetaImageSequenceIO();

  PlusStatus SetCustomFrameString(int frameNumber, const char* fieldName,  const char* fieldValue);
  
  bool SetCustomString(const char* fieldName, const char* fieldValue);
  const char* GetCustomString(const char* fieldName);

  //! Read all the fields in the metaimage file header
  virtual PlusStatus ReadImageHeader();

  //! Read pixel data from the metaimage
  virtual PlusStatus ReadImagePixels();

  //! Write all the fields to the metaimage file header
  virtual PlusStatus WriteImageHeader();

  //! Write pixel data to the metaimage
  virtual PlusStatus WriteImagePixels();

  virtual void CreateTrackedFrameIfNonExisting(int frameNumber);

  //! Set file name for storing the pixel data
  vtkSetStringMacro(PixelDataFileName); 
	vtkGetStringMacro(PixelDataFileName); 

  std::string GetPixelDataFilePath();
  PlusStatus ConvertMetaElementTypeToItkPixelType(const std::string &elementTypeStr, PlusCommon::ITKScalarPixelType &itkPixelType);
  PlusStatus ConvertItkPixelTypeToMetaElementType(PlusCommon::ITKScalarPixelType itkPixelType, std::string &elementTypeStr);

  //! Update a field in the image header with its current value
  PlusStatus UpdateFieldInImageHeader(const char* fieldName);

  //! Writes the compressed pixel data directly into file. The compression is performed in chunks, so no excessive memory is used for the compression.
  // compressedDataSize returns the size of the total compressed data that is written to the file.
  virtual PlusStatus WriteCompressedImagePixelsToFile(FILE *outputFileStream, int &compressedDataSize);


private:

  typedef __int64 FilePositionOffsetType;
    
  // Custom frame fields and image data are stored in the m_FrameList for each frame
  vtkTrackedFrameList* TrackedFrameList;

  char* FileName;
  bool UseCompression;
  itk::ImageIOBase::FileType FileType;
  PlusCommon::ITKScalarPixelType PixelType;
  int NumberOfComponents;
  int NumberOfDimensions;
  int Dimensions[3];

  US_IMAGE_ORIENTATION ImageOrientationInFile;

  FilePositionOffsetType PixelDataFileOffset;
  char* PixelDataFileName;
  
  vtkMetaImageSequenceIO(const vtkMetaImageSequenceIO&); //purposely not implemented
  void operator=(const vtkMetaImageSequenceIO&); //purposely not implemented
};

#endif // __vtkMetaImageSequenceIO_h 
