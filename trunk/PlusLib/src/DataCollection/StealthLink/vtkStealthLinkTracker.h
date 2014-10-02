/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkStealthLinkTracker_h
#define __vtkStealthLinkTracker_h

#include "vtkDataCollectionExport.h"

#include "vtkPlusDevice.h"
#include <string>

class vtkTransformRepository;

/*!
\class StealthServer 
\brief Interface for the Medtronic StealthLink Server 

\ingroup PlusLibDataCollection

*/

class vtkDataCollectionExport vtkStealthLinkTracker : public vtkPlusDevice
{
public:

  static vtkStealthLinkTracker *New();
  vtkTypeMacro(vtkStealthLinkTracker,vtkPlusDevice);
  void PrintSelf( ostream& os, vtkIndent indent );

  /*! Hardware device SDK version. */
  virtual PlusStatus GetSdkVersion(std::string&); 

  virtual bool IsTracker() const { return true; }

  /*! Return a list of items that desrcibe what image volumes stealthlink device can provide */
  virtual PlusStatus GetImageMetaData(PlusCommon::ImageMetaDataList &imageMetaData);

  /*!
    Return the volume  with the given id that this device can provide.
    If requestedImageId is empty then GetImage will return the default image and set assignedImageId to the ID of this default image.
    If requestedImageId is not empty then assignedImageId is the same as the requestedImageId (if requestedImageId does not match the ID of the
    current image then the method will return with error).
    If imageReferenceFrameName is not "Ras" then TransformRepository must be updated using UpdateTransformRepository() right before this call.
  */
  virtual PlusStatus GetImage(const std::string& requestedImageId,std::string& assignedImageId, const std::string& imageReferenceFrameName, vtkImageData* imageData, vtkMatrix4x4* ijkToReferenceTransform);

  /*! Get the dicom directory where the dicom images will be saved when acquired from the server */
  std::string GetDicomImagesOutputDirectory();

  /*! Deep copies the transform repository from the server into the TransformRepository attribute */
  PlusStatus UpdateTransformRepository(vtkTransformRepository* sharedTransformRepository);
  
  /*! Set the dicom directory where the dicom images will be saved when acquired from the server */
  void SetDicomImagesOutputDirectory(std::string dicomImagesOutputDirectory);

  /*! Set the boolean for keeping the received dicom images */
  void SetKeepReceivedDicomFiles(bool keepReceivedDicomFiles);

  /*! Set StealthStation IP address */
  void SetServerAddress(const char* serverAddress);

  /*! Set StealthStation IP port number */
  void SetServerPort(const char* serverPort);

protected:
  /*! Constructor */
  vtkStealthLinkTracker();
  /*! Destructor */
  ~vtkStealthLinkTracker();

  /*! Connect to the tracker hardware */
  virtual PlusStatus InternalConnect();
  /*! Disconnect from the tracker hardware */
  virtual PlusStatus InternalDisconnect();

  /*! Start recording !*/
  virtual PlusStatus InternalStartRecording();
  /*! Stop the tracking system and bring it back to its initial state. */
  virtual PlusStatus InternalStopRecording();

  /*! Read StealthLinkDevice configuration and update the tracker settings accordingly */
  virtual PlusStatus ReadConfiguration( vtkXMLDataElement* config );
  /*! Write current StealthLinkDevice configuration settings to XML */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* rootConfigElement);

  /*! The internal function that does the grab */
  PlusStatus InternalUpdate(); 

  /*! Get image from the StealthLink into the specified folder. The Dicom images will be saved in the directory spesified by "ExamImageDirectory". */
  PlusStatus AcquireDicomImage(std::string dicomImagesOutputDirectory, std::string& examImageDirectory);

  /*! For internal storage of additional variables (to minimize the number of included headers) */
  class vtkInternal;
  vtkInternal* Internal; 

  class vtkInternalShared;
  vtkInternalShared* InternalShared; 

  class vtkInternalUpdatePrivate;
  vtkInternalUpdatePrivate* InternalUpdatePrivate; 

  /*! Is Tracker Connected? */
  PlusStatus IsLocalizerConnected(bool&);

  /*! Make sure the PortNames defined in the config file are also defined in the server */
  PlusStatus AreInstrumentPortNamesValid(bool& valid);

  /*! Remove the characters that cannot be used in folder name */
  static void RemoveForbiddenCharacters(std::string& str);
 
  /*! Remove the folder after having read the dicom files */
  PlusStatus DeleteDicomImageOutputDirectory(std::string examImageDirectory);

  /*! returns the ImageMetaDatasetsCount as std::string */
  std::string GetImageMetaDatasetsCountAsString();

private:
  vtkStealthLinkTracker(const vtkStealthLinkTracker&);
  void operator=(const vtkStealthLinkTracker&);  
};

#endif
