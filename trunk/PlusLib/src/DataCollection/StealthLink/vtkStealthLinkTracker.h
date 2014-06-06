/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __vtkStealthLinkTracker_h
#define __vtkStealthLinkTracker_h

#include "vtkTransformRepository.h"
#include "vtkPlusDevice.h"
#include <string>
#include <deque>

/*!
\\class StealthServer 
\brief Interface for the Medtronic StealthLink Server 

This class talks with StealthLink Server and acquires the transformation matrix data for both the current frame and the instrument
nomenclature: Localizer(tracker), frame(reference),instrument(tool).
Only one active instrument and a frame at a time.
The reference and instruments are defined by the words "Frame" and the instrument names respectively. Specify the instrument names as the PortName in the configuration file

\\ingroup PlusLibDataCollection

<!--- Before creating the connection between Plus and StealthLink Server, please follow the steps below in the configuration file:
      ->Change the ServerAddress to the IP address of the Stealth Server 
      ->Change the PortAddress to the Port Address of the Stealth Server
      ->The PortName of the Reference must stay "Frame" regardless of what frame is in use 
      ->Make sure that the PortNames of the instruments to be tracked are   
      the instrument names in the Stealth Server and that they are defined in the Server
---!>
!*/

class VTK_EXPORT vtkStealthLinkTracker : public vtkPlusDevice
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

	/*! Return the volume  with the given id that this device can provide */
	virtual PlusStatus GetImage(const std::string& requestedImageId,std::string& assignedImageId, const std::string& imageReferencFrameName, vtkImageData* imageData, vtkMatrix4x4* ijkToReferenceTransform);

  /*! Get the Patient Name. The UpdateCurrentExam functions needs calling before GetPatientName!*/
  PlusStatus GetPatientName(std::string& patientName);

  /*! Get the Patient Name. The UpdateCurrentExam functions needs calling before GetPatientName!*/
  PlusStatus GetPatientId(std::string& patientId);

	/*! Get the dicom directory where the dicom images will be saved when acquired from the server !*/
	std::string GetDicomImagesOutputDirectory();

	/*! Set the dicom directory where the dicom images will be saved when acquired from the server !*/
	void SetDicomImagesOutputDirectory(std::string dicomImagesOutputDirectory);

	/*! Set the boolean for keeping the received dicom images !*/
	void SetKeepReceivedDicomFiles(bool keepReceivedDicomFiles);

	/*! Deep copies the transform repository from the server into the TransformRepository attribute !*/
	PlusStatus UpdateTransformRepository(vtkTransformRepository* sharedTransformRepository);

protected:
	/*! Constructor !*/
	vtkStealthLinkTracker();
	/*! Destructor !*/
	~vtkStealthLinkTracker();

	/*! Connect to the tracker hardware */
	virtual PlusStatus InternalConnect();
	/*! Disconnect from the tracker hardware */
  virtual PlusStatus InternalDisconnect();

	/*! Start recording !*/
	virtual PlusStatus InternalStartRecording();
	/*! Stop the tracking system and bring it back to its initial state. */
	virtual PlusStatus InternalStopRecording();
	/*! The internal function that does the grab !*/
	PlusStatus InternalUpdate(); 

	/*! Acquire the current Instrument and Frame from the server !*/
	PlusStatus UpdateCurrentNavigationData();

	/*! Acquire the current registration from the server !*/
  PlusStatus UpdateCurrentRegistration();

	/*! Acquire the current exam from the server !*/
  PlusStatus UpdateCurrentExam();
  
	/*! Acquire the requested exam from the server with the imageId !*/
  PlusStatus UpdateRequestedExam(const std::string& imageId);

	/*! Get image from the StealthLink into VTK images. The Dicom images will be saved in the directory spesified by "ExamImageDirectory". !*/
  PlusStatus vtkStealthLinkTracker::AcquireDicomImage(std::string dicomImagesOutputDirectory, std::string& examImageDirectory);

	// For internal storage of additional variables (to minimize the number of included headers)
	class vtkInternal;
	vtkInternal* Internal; 

	double TrackerTimeToSystemTimeSec;

	/* Generates string of the number of exams requested from the server before*/
	std::string GetExamCountInString();

	/*! Is Tracker Connected? !*/
	PlusStatus IsLocalizerConnected(bool&);

	/*! Make sure the PortNames defined in the config file are also defined in the server!*/
	PlusStatus AreInstrumentPortNamesValid(bool& valid);

	/*! Remove the characters that cannot be used in folder name !*/
	static void RemoveForbiddenCharactersFromPatientsName(std::string& patientName);
 
	/*! Remove the folder after having read the dicom files !*/
	PlusStatus DeleteDicomImageOutputDirectory(std::string examImageDirectory);

  /*! Read MicronTracker configuration and update the tracker settings accordingly */
  virtual PlusStatus ReadConfiguration( vtkXMLDataElement* config );

  /*! Write current MicronTracker configuration settings to XML */
  virtual PlusStatus WriteConfiguration(vtkXMLDataElement* rootConfigElement);

private:
  vtkStealthLinkTracker(const vtkStealthLinkTracker&);
  void operator=(const vtkStealthLinkTracker&);  
};

#endif
