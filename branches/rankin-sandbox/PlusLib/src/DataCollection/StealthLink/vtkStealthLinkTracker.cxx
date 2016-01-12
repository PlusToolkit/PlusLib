/*=Plus=header=begin======================================================
Program: Plus
Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
See License.txt for details.
=========================================================Plus=header=end*/


#include "PlusConfigure.h"
#include "vtkStealthLinkTracker.h"
#include "StealthLink/StealthLink.h"

#include "vtkDICOMImageReader.h"
#include "vtkDirectory.h"
#include "vtkImageData.h"
#include "vtkImageFlip.h"
#include "vtkMath.h"
#include "vtkMatrix4x4.h"
#include "vtkPlusChannel.h"
#include "vtkPlusDataSource.h"
#include "vtkTransformRepository.h"

#include <iostream>

static const int MAX_DEVICE_ID_LENGTH=15; //for OpenIGTLink message sending purposes, the image id will be created off of device id. It is better for it to be short

// Class for variables that are shared between the main thread and InternalUpdate thread
class vtkStealthLinkTracker::vtkInternalShared
{
public:
  
  vtkInternalShared(vtkStealthLinkTracker* external)
    :StealthLinkServer(NULL)
  {
    this->ExamIjkToRpiTransform = vtkSmartPointer<vtkMatrix4x4>::New();
    this->ExamIjkToRasTransform = vtkSmartPointer<vtkMatrix4x4>::New();

    this->StealthLinkServerMutex = vtkSmartPointer<vtkRecursiveCriticalSection>::New();
    this->ExamIjkToRpiMutex = vtkSmartPointer<vtkRecursiveCriticalSection>::New();
    this->ExamIjkToRasMutex = vtkSmartPointer<vtkRecursiveCriticalSection>::New();
    this->ExamValidMutex = vtkSmartPointer<vtkRecursiveCriticalSection>::New();
    this->CurrentExamMutex = vtkSmartPointer<vtkRecursiveCriticalSection>::New();
    this->CurrentRegistrationMutex = vtkSmartPointer<vtkRecursiveCriticalSection>::New();
  }
   /*! Destructor !*/
  ~vtkInternalShared()
  {	
    delete this->StealthLinkServer;
    this->StealthLinkServer=NULL;
    this->External = NULL;
  }
  // Begin - Thread Safe Set and Get Functions
  // IjkToExamRpi Set and Get
  void SetExamIjkToRpiTransformMatrix(vtkMatrix4x4* examIjkToRpiTransform)
  {
    PlusLockGuard<vtkRecursiveCriticalSection> updateMutexGuardedLock(this->ExamIjkToRpiMutex);
    this->ExamIjkToRpiTransform = examIjkToRpiTransform;
  }
  void GetExamIjkToRpiTransformMatrix(vtkMatrix4x4* examIjkToRpiTransform)
  {
    PlusLockGuard<vtkRecursiveCriticalSection> updateMutexGuardedLock(this->ExamIjkToRpiMutex);
    examIjkToRpiTransform->DeepCopy(this->ExamIjkToRpiTransform);
  }
  // IjkToRas Set and Get
  void SetExamIjkToRasTransformMatrix(vtkMatrix4x4* examIjkToRasTransform)
  {
    PlusLockGuard<vtkRecursiveCriticalSection> updateMutexGuardedLock(this->ExamIjkToRasMutex);
    this->ExamIjkToRasTransform = examIjkToRasTransform;
  }
  void GetExamIjkToRasTransformMatrix(vtkMatrix4x4* examIjkToRasTransform)
  {
    PlusLockGuard<vtkRecursiveCriticalSection> updateMutexGuardedLock(this->ExamIjkToRasMutex);
    examIjkToRasTransform->DeepCopy(this->ExamIjkToRasTransform);
  }
  // ExamValid Set and Get
  void SetExamValid(bool examValid)
  {
    PlusLockGuard<vtkRecursiveCriticalSection> updateMutexGuardedLock(this->ExamValidMutex);
    this->ExamValid = examValid;
  }
  bool GetExamValid()
  {
    PlusLockGuard<vtkRecursiveCriticalSection> updateMutexGuardedLock(this->ExamValidMutex);
    return this->ExamValid;
  }
  // Get for Navigation Data. It is called each time internalupdate is called
  PlusStatus GetCurrentNavigationData(MNavStealthLink::NavData& navData)
  {
    MNavStealthLink::Error err;
    {
      PlusLockGuard<vtkRecursiveCriticalSection> updateMutexGuardedLock(this->StealthLinkServerMutex);
      MNavStealthLink::DateTime myDateTime=this->StealthLinkServer->getServerTime();
      if(!this->StealthLinkServer->get(navData,myDateTime,err))
      {	
        LOG_ERROR(" Failed to acquire the navigation data from StealthLink Server: " <<  err.reason() << " " << err.what() << "\n");
        return PLUS_FAIL;
      }
    }
    return PLUS_SUCCESS;
  }
  //----------------------------------------------------------------------------
  void GetCurrentExam(MNavStealthLink::Exam& exam) 
  {
    PlusLockGuard<vtkRecursiveCriticalSection> updateMutexGuardedLock(this->CurrentExamMutex);
    exam = this->CurrentExam;
  }
  //----------------------------------------------------------------------------
  void GetCurrentRegistration(MNavStealthLink::Registration& registration) // TODO make it thread safe
  {
    PlusLockGuard<vtkRecursiveCriticalSection> updateMutexGuardedLock(this->CurrentRegistrationMutex);
    registration = this->CurrentRegistration;
  }
  //----------------------------------------------------------------------------
  //Port names that are present on the server
  PlusStatus GetValidToolPortNames(std::vector<std::string>& validToolPortNames)
  {
    PlusLockGuard<vtkRecursiveCriticalSection> updateMutexGuardedLock(this->StealthLinkServerMutex);
    MNavStealthLink::Error err;
    MNavStealthLink::InstrumentNameList instrumentNameList;
    // Get the instrument list
    if(!this->StealthLinkServer->get(instrumentNameList,err))
    {
      LOG_ERROR (" Could not retrieve the instument name list: " << err.reason() << std::endl);
      return PLUS_FAIL;
    }
    MNavStealthLink::FrameNameList frameNameList;
    // Get the frame name list
    if(!this->StealthLinkServer->get(frameNameList,err))
    {
      LOG_ERROR (" Could not retrieve the instument name list: " << err.reason() << std::endl);
      return PLUS_FAIL;
    }
    for(MNavStealthLink::InstrumentNameList::iterator instrumentNameIterator = instrumentNameList.begin(); instrumentNameIterator!=instrumentNameList.end(); instrumentNameIterator++)
    {
      validToolPortNames.push_back(*instrumentNameIterator);
    }
    for(MNavStealthLink::FrameNameList::iterator frameNameIterator = frameNameList.begin(); frameNameIterator!= frameNameList.end(); frameNameIterator++)
    {
      validToolPortNames.push_back(*frameNameIterator);
    }
    validToolPortNames.push_back(vtkInternalShared::GetRasRegistrationToolName());
    return PLUS_SUCCESS;
  }
  // Get for current exam. The exam is only acquired from the server when GET_IMGMETA, GET_IMAGE or GET_EXAM_DATA commands are requested. This function will make sure that different threads
  // do not modify the exam at the same time. Use GetCurrentExam function everytime the current exam is used. 
  PlusStatus UpdateCurrentExam()
  {
    PlusLockGuard<vtkRecursiveCriticalSection> updateMutexGuardedLock(this->CurrentExamMutex);
    MNavStealthLink::Error err;
    {
      PlusLockGuard<vtkRecursiveCriticalSection> updateMutexGuardedLock(this->StealthLinkServerMutex);
      MNavStealthLink::DateTime myDateTime=this->StealthLinkServer->getServerTime();
      if(!this->StealthLinkServer->get(this->CurrentExam,myDateTime,err))
      {
        LOG_ERROR(" Failed to acquire the current registraion: " <<  err.what() << "\n");
        return PLUS_FAIL;
      }
    }
    return PLUS_SUCCESS;
  }
  // Get for current registration. The registration is only acquired from the server when GET_IMAGE or GET_EXAM_DATA commands are requested. This function will make sure that different threads
  // do not modify the registration at the same time. Use GetCurrentRegistration function everytime the current registration is used. 
  PlusStatus UpdateCurrentRegistration()
  {
    PlusLockGuard<vtkRecursiveCriticalSection> updateMutexGuardedLock(this->CurrentRegistrationMutex);
    MNavStealthLink::Error err;
    {
      PlusLockGuard<vtkRecursiveCriticalSection> updateMutexGuardedLock(this->StealthLinkServerMutex);
      MNavStealthLink::DateTime myDateTime=this->StealthLinkServer->getServerTime();
      if(!this->StealthLinkServer->get(this->CurrentRegistration,myDateTime,err))
      {
        LOG_ERROR(" Failed to acquire the current registraion: " <<  err.what() << "\n");
        return PLUS_FAIL;
      }
    }
    return PLUS_SUCCESS;
  }
  //---------------------------------------------------------------------------
  bool IsStealthServerConnected()
  {
    PlusLockGuard<vtkRecursiveCriticalSection> updateMutexGuardedLock(this->StealthLinkServerMutex);
    if(this->StealthLinkServer == NULL)
    {
      return false;
    }
    return true;
  }
  //----------------------------------------------------------------------------
  PlusStatus GetSdkVersion(std::string& version)
  {
    PlusLockGuard<vtkRecursiveCriticalSection> updateMutexGuardedLock(this->StealthLinkServerMutex);
    MNavStealthLink::Version serverVersion;
    MNavStealthLink::Error err;
    {
      MNavStealthLink::DateTime myDateTime=this->StealthLinkServer->getServerTime();
      if(!this->StealthLinkServer->get(serverVersion, myDateTime,err))
      {
        LOG_ERROR("Failed to acquire the version of the StealthLinkServer " << err.reason() << "\n");
        return PLUS_FAIL;
      }
    }
    version = (unsigned) serverVersion.major + (unsigned) serverVersion.minor;
    return PLUS_SUCCESS;
  }
  PlusStatus GetExamNameList(MNavStealthLink::ExamNameList& examNameList)
  {
    PlusLockGuard<vtkRecursiveCriticalSection> updateMutexGuardedLock(this->StealthLinkServerMutex);
    MNavStealthLink::Error err;
    MNavStealthLink::DateTime myDateTime=this->StealthLinkServer->getServerTime();
    if(!this->StealthLinkServer->get(examNameList, myDateTime,err))
    {
      LOG_ERROR("Failed to acquire the version of the StealthLinkServer " << err.reason() << "\n");
      return PLUS_FAIL;
    }
    return PLUS_SUCCESS;
  }
  //--------------------------------------------------------------------------
  PlusStatus ConnectToStealthStation(std::string serverAddress, std::string serverPort)
  {
    PlusLockGuard<vtkRecursiveCriticalSection> updateMutexGuardedLock(this->StealthLinkServerMutex);
    delete this->StealthLinkServer;
    this->StealthLinkServer = NULL;
    this->StealthLinkServer = new MNavStealthLink::StealthServer(serverAddress,serverPort);

    MNavStealthLink::Error error;
    if (!this->StealthLinkServer->connect(error))
    {
      LOG_ERROR(" Failed to connect to Stealth server application on host: " << error.what() << "\n");
      return PLUS_FAIL;
    }
    return PLUS_SUCCESS;
  }
  //-------------------------------------------------------------------------
  PlusStatus IsLocalizerConnected(bool& connected)
  {
    PlusLockGuard<vtkRecursiveCriticalSection> updateMutexGuardedLock(this->StealthLinkServerMutex);
    MNavStealthLink::LocalizerInfo localizerInfo;
    MNavStealthLink::Error		   err;
    if(!this->StealthLinkServer->get(localizerInfo,err))
    {
      LOG_ERROR("Cannot retrieve the localizer info " << err.reason() << "\n");
      connected = false;
      return PLUS_FAIL;
    }
    if(localizerInfo.isConnected)
    {
      connected = true;
      return PLUS_SUCCESS;
    }
    connected = false;
    return PLUS_SUCCESS;
  }
  //-------------------------------------------------------------------------
  PlusStatus GetStealthStationServerTime(double& serverTime)
  {
    PlusLockGuard<vtkRecursiveCriticalSection> updateMutexGuardedLock(this->StealthLinkServerMutex);
    try
    {
      serverTime = (double) this->StealthLinkServer->getServerTime();
    }
    catch (MNavStealthLink::Error err)
    {
      LOG_ERROR("Cannot get server time " << err.reason());
      return PLUS_FAIL;
    }
    return PLUS_SUCCESS;
  }
  //-----------------------------------------------------------------------
  void DisconnectFromStealthStation()
  {
    this->StealthLinkServer->disconnect(); 
    delete this->StealthLinkServer;
    this->StealthLinkServer = NULL;
  }
  //----------------------------------------------------------------------
  PlusStatus GetExamData(MNavStealthLink::Exam exam,std::string examImageDirectory)
  {
    PlusLockGuard<vtkRecursiveCriticalSection> updateMutexGuardedLock(this->StealthLinkServerMutex);
    try 
    {
      exam.getExamData(*(this->StealthLinkServer),examImageDirectory);
    }
    catch (MNavStealthLink::Error error) 
    {
      LOG_ERROR("Error getting images from StealthLink: " << error.what() << "\n");
      return PLUS_FAIL;
    }
    return PLUS_SUCCESS;
  }
  //-----------------------------------------------------------------------
  static const char* GetRasRegistrationToolName() { return "RasRegistration"; }

private:
  MNavStealthLink::StealthServer *StealthLinkServer;
  MNavStealthLink::Registration CurrentRegistration;
  MNavStealthLink::Exam         CurrentExam;
  // We receive ExamIjkToRpiTransform from StealthStation.
  // StealthStation ignores the volume orientation (volume axis directions are RPI),
  // and origin (origin is at 0 0 0) => this is the ExamRPI coordinate system.
  vtkSmartPointer<vtkMatrix4x4> ExamIjkToRpiTransform;
  // We read ijkToLps from the header of the DICOM files that we receive from StealthLink and compute ijkToRas
  // from that.
  vtkSmartPointer<vtkMatrix4x4> ExamIjkToRasTransform;

  // Necessary mutex variables
  vtkSmartPointer<vtkRecursiveCriticalSection> StealthLinkServerMutex;
  vtkSmartPointer<vtkRecursiveCriticalSection> ExamIjkToRpiMutex;
  vtkSmartPointer<vtkRecursiveCriticalSection> ExamIjkToRasMutex;
  vtkSmartPointer<vtkRecursiveCriticalSection> ExamValidMutex;
  vtkSmartPointer<vtkRecursiveCriticalSection> CurrentExamMutex;
  vtkSmartPointer<vtkRecursiveCriticalSection> CurrentRegistrationMutex;

  bool ExamValid;

  vtkStealthLinkTracker *External;
};
//----------------------------------------------------------------------------
// Class for variables that are only accessed from the InternalUpdate thread and internalconnect 
class vtkStealthLinkTracker::vtkInternalUpdatePrivate
{
public:
  friend PlusStatus vtkStealthLinkTracker::InternalUpdate();
  friend PlusStatus vtkStealthLinkTracker::InternalConnect();
  vtkInternalUpdatePrivate(vtkStealthLinkTracker* external)
    :External(external)
  {
  }
private:
  double ServerInitialTimeInMicroSeconds;
  double TrackerTimeToSystemTimeSec;

  vtkStealthLinkTracker *External;
};
// Class for variables that are shared only from main thread
class vtkStealthLinkTracker::vtkInternal
{
public:

  friend class vtkStealthLinkTracker;

private:

  vtkStealthLinkTracker *External;

  // This is used in GetImage() to calculate the transformation of the image in the demanded reference frame.
  // The transform repository should be updated before calling GetImage() if the 
  vtkSmartPointer<vtkTransformRepository> TransformRepository;

  std::pair<std::string,std::string> PairImageIdAndName; // unique name created by exam name patient name patient id...
  std::string ServerAddress; // Host IP Address
  std::string ServerPort; // Host Port Address
  std::string DicomImagesOutputDirectory; //The folder where DICOM images that StealthLink sends will be saved

  /*!  How many image meta datasets are in total in the connected devices */
  int ImageMetaDatasetsCount;
  bool KeepReceivedDicomFiles;

  /*~ Constructor ~*/
  vtkInternal(vtkStealthLinkTracker* external) 
    : External(external)
  {
    
    this->TransformRepository       = vtkSmartPointer<vtkTransformRepository>::New();
    
    this->ServerAddress.clear();
    this->ServerPort.clear();
    this->DicomImagesOutputDirectory.clear();
  }

  /*! Destructor !*/
  ~vtkInternal()
  {	
  }
  std::string GetExamAndPatientInformationAsString(MNavStealthLink::Exam exam)
  {
    std::string examAndPatientInformation;
    examAndPatientInformation = exam.name + exam.description + exam.modality + exam.hospitalName + exam.examNumber + exam.patientId + exam.patientName + exam.patientDOB;
    return examAndPatientInformation;
  }
  
  /*! Update the transformation maxtrix of the current instrument !*/
  static void GetInstrumentInformation(MNavStealthLink::NavData navData,bool& instrumentOutOfView,vtkMatrix4x4* insToTrackerTransform)
  {
    if( !(navData.instVisibility == MNavStealthLink::Instrument::VISIBLE) && !(navData.instVisibility == MNavStealthLink::Instrument::ALMOST_BLOCKED))
    {
      instrumentOutOfView = true;
      return;
    }
    else
    {
      instrumentOutOfView = false;
    }
    for(int col=0; col < 4; col++)
    {
      for (int row=0; row < 4; row++)
      {
        insToTrackerTransform->SetElement(row, col, navData.localizer_T_instrument [row][col]);
      }
    }
  }
  /*! Update the transformation maxtrix of the current frame !*/
  static void GetFrameInformation(MNavStealthLink::NavData navData,bool& frameOutOfView,vtkMatrix4x4* frameToTrackerTransform)
  {
    if( !(navData.frameVisibility == MNavStealthLink::Frame::VISIBLE) && !(navData.frameVisibility == MNavStealthLink::Frame::ALMOST_BLOCKED))
    {
      frameOutOfView = true;
      return;
    }
    else
    {
      frameOutOfView = false;
    }
    vtkSmartPointer<vtkMatrix4x4> trackerToFrameTransform = vtkSmartPointer<vtkMatrix4x4>::New();
    for(int col=0; col < 4; col++)
    {
      for (int row=0; row < 4; row++)
      {
        trackerToFrameTransform->SetElement(row, col, navData.frame_T_localizer [row][col]); // from localizer Space to frame space
      }
    }
    //the transformation matrix given by NavData is from Localizer(Tracker) space to frame space and we need the inverse of it
    vtkMatrix4x4::Invert(trackerToFrameTransform,frameToTrackerTransform);   
  }
  /* Update the transformation maxtrix of the current image. Thread-safe. !*/
  void GetRasToTrackerTransform(vtkMatrix4x4* frameToTrackerTransform,vtkMatrix4x4* rasToTrackerTransform)
  {

    vtkSmartPointer<vtkMatrix4x4> frameToRegExamTransform = vtkSmartPointer<vtkMatrix4x4>::New(); // this is from frame to image used in the registration 
    vtkSmartPointer<vtkMatrix4x4> regExamToExamRpiTransform  = vtkSmartPointer<vtkMatrix4x4>::New();  // from the exam image to registration image, if they are the same then the matrix is idendity
    MNavStealthLink::Registration registration;
    MNavStealthLink::Exam exam;
    this->External->InternalShared->GetCurrentRegistration(registration); //this function is thread safe. If registration is  being updated, GetCurrentRegistration function will wait
    this->External->InternalShared->GetCurrentExam(exam);                 //this function is thread safe. If exam is being updated, GetCurrentExam function will wait
    for(int col=0; col < 4; col++)
    {
      for (int row=0; row < 4; row++)
      {
        frameToRegExamTransform->SetElement(row, col, registration.regExamMM_T_frame [row][col]);
        regExamToExamRpiTransform->SetElement(row,col,exam.examMM_T_regExamMM[row][col]);
      }
    }
    //examRpiToFrame = regExamToFrame * examRpiToRegExam so we need the inverse of the two matrices
    vtkSmartPointer<vtkMatrix4x4> regExamToFrameTransform = vtkSmartPointer<vtkMatrix4x4>::New();
    vtkMatrix4x4::Invert(frameToRegExamTransform,regExamToFrameTransform);
    vtkSmartPointer<vtkMatrix4x4> examRpiToRegExamTransform = vtkSmartPointer<vtkMatrix4x4>::New();
    vtkMatrix4x4::Invert(regExamToExamRpiTransform,examRpiToRegExamTransform);

    //examToFrame = regExamToFrame * ExamToRegExam
    vtkSmartPointer<vtkMatrix4x4> examRpiToFrameTransform = vtkSmartPointer<vtkMatrix4x4>::New();
    vtkMatrix4x4::Multiply4x4(regExamToFrameTransform,examRpiToRegExamTransform,examRpiToFrameTransform);

    // medtronic stores the image in rpi and does not include orientation.  so we need to do some extra math to include the orientation
    // We need to get to RasToFrame. We have examRpiToFrame which is stored in rpi with no information of orientation. We have ijkToRas and ijkToExamRpi transformation matrices.
    // RasToFrame = FrameFromRas = FrameFromExamRpi * ExamRpiFromRas
    //													 = FrameFromExamRpi * ExamRpiFromIjk * IjkFromRas
    //													 = ExamRpiToFrame * IjkToExamRpi * RasToIjk

    // we have ijkToRas so we need to invert it to have rasToIjk
    vtkSmartPointer<vtkMatrix4x4> rasToIjkTransform = vtkSmartPointer<vtkMatrix4x4>::New();
    vtkSmartPointer<vtkMatrix4x4> examIjkToRasTransform = vtkSmartPointer<vtkMatrix4x4>::New();
    this->External->InternalShared->GetExamIjkToRasTransformMatrix(examIjkToRasTransform); //thread safe. If ijkToRas is being updated, GetIjkToRasTransformationMatrix will wait
    vtkMatrix4x4::Invert(examIjkToRasTransform,rasToIjkTransform);

    //Now we just need to multiply the matrices
    //First, let's multiplly examRpiToFrame and IjkToExamRpi: this will give us ijkToFrame
    vtkSmartPointer<vtkMatrix4x4> ijkToFrameTransform = vtkSmartPointer<vtkMatrix4x4>::New();
    vtkSmartPointer<vtkMatrix4x4> examIjkToRpiTransform = vtkSmartPointer<vtkMatrix4x4>::New();
    this->External->InternalShared->GetExamIjkToRpiTransformMatrix(examIjkToRpiTransform); //thread safe. If ijkToExamRpi is being updated, GetIjkToExamRpiTransformationMatrix will wait
    vtkMatrix4x4::Multiply4x4(examRpiToFrameTransform,examIjkToRpiTransform,ijkToFrameTransform);

    // we now multiply ijkToFrameTransform and RasToIjk which will give rasToFrame
    vtkSmartPointer<vtkMatrix4x4> rasToFrameTransform = vtkSmartPointer<vtkMatrix4x4>::New();
    vtkMatrix4x4::Multiply4x4(ijkToFrameTransform,rasToIjkTransform,rasToFrameTransform);

    //Final step is to get rasToTracker which is done by frameToTracker * rasToFrame 
    vtkSmartPointer<vtkMatrix4x4> lpsToTrackerTransform = vtkSmartPointer<vtkMatrix4x4>::New();
    vtkMatrix4x4::Multiply4x4(frameToTrackerTransform,rasToFrameTransform,rasToTrackerTransform);
  }
};

/****************************************************************************/ 

vtkStandardNewMacro(vtkStealthLinkTracker);

//----------------------------------------------------------------------------
vtkStealthLinkTracker::vtkStealthLinkTracker()
{
  this->Internal = new vtkInternal(this);
  this->InternalShared = new vtkInternalShared(this);
  this->InternalUpdatePrivate = new vtkInternalUpdatePrivate(this);

  this->RequirePortNameInDeviceSetConfiguration = true;
  
  // No callback function provided by the device, so the data capture thread will be used to poll the hardware and add new items to the buffer
  this->StartThreadForInternalUpdates=true;
  this->AcquisitionRate = 30;
}

// Public Functions
//----------------------------------------------------------------------------
vtkStealthLinkTracker::~vtkStealthLinkTracker() 
{
  if (this->InternalShared->IsStealthServerConnected())
  {
    this->InternalDisconnect();
  }
  delete this->Internal;
  this->Internal = NULL;
}
//---------------------------------------------------------------------------
void vtkStealthLinkTracker::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
//----------------------------------------------------------------------------
PlusStatus vtkStealthLinkTracker::GetSdkVersion(std::string& version)
{
  return this->InternalShared->GetSdkVersion(version);
}
//----------------------------------------------------------------------------
PlusStatus vtkStealthLinkTracker::UpdateTransformRepository(vtkTransformRepository* sharedTransformRepository)
{
  if (sharedTransformRepository==NULL)
  {
    LOG_ERROR("vtkVirtualVolumeReconstructor::UpdateTransformRepository: shared transform repository is invalid");
    return PLUS_FAIL;
  }
  // Create a copy of the transform repository to allow using it for stealthlink while being also used in other threads
  this->Internal->TransformRepository->DeepCopy(sharedTransformRepository,true);
  return PLUS_SUCCESS;
}

//--------------------------------------------------------------------
PlusStatus vtkStealthLinkTracker::GetImageMetaData(PlusCommon::ImageMetaDataList &imageMetaData)
{
  if(!this->InternalShared->UpdateCurrentExam()) //this function is thread safe
  {
    return PLUS_FAIL;
  }
  LOG_INFO("Acquiring the image meta data from the device with DeviceId: "<<this->GetDeviceId());
  PlusCommon::ImageMetaDataItem imageMetaDataItem;
  MNavStealthLink::Exam exam;
  this->InternalShared->GetCurrentExam(exam); // thread safe

  imageMetaDataItem.Id = this->GetImageMetaDatasetsCountAsString();
  imageMetaDataItem.Description = exam.description;
  imageMetaDataItem.Modality = exam.modality;
  imageMetaDataItem.PatientId= exam.patientId;
  imageMetaDataItem.PatientName = exam.patientName ;
  imageMetaDataItem.ScalarType = 3; //TODO check this and correct this
  imageMetaDataItem.Size[0] = exam.size[0];
  imageMetaDataItem.Size[1] = exam.size[1];
  imageMetaDataItem.Size[2] = exam.size[2];
  imageMetaDataItem.TimeStampUtc = vtkAccurateTimer::GetUniversalTime();
  imageMetaData.push_back(imageMetaDataItem);
  this->Internal->PairImageIdAndName.first = imageMetaDataItem.Id;
  this->Internal->PairImageIdAndName.second = this->Internal->GetExamAndPatientInformationAsString(exam);
  return PLUS_SUCCESS;
}
//-------------------------------------------------------------------
PlusStatus vtkStealthLinkTracker::GetImage(const std::string& requestedImageId, std::string& assignedImageId, const std::string& imageReferencFrameName, vtkImageData* imageData, vtkMatrix4x4* ijkToReferenceTransform)
{
  std::string examImageDirectoryToDelete;
  {
    if(!this->InternalShared->UpdateCurrentExam())
    {
      return PLUS_FAIL;
    }
    if(!this->InternalShared->UpdateCurrentRegistration())
    {
      return PLUS_FAIL;
    }
    if(!requestedImageId.empty()) // The command is GET_IMAGE
    {
      if(STRCASECMP(this->Internal->PairImageIdAndName.first.c_str(),requestedImageId.c_str()) != 0)
      {
        LOG_ERROR("The image requested does not belong to " << this->GetDeviceId());
        return PLUS_FAIL;
      }
      MNavStealthLink::Exam exam;
      this->InternalShared->GetCurrentExam(exam);
      std::string examAndPatientInformation = this->Internal->GetExamAndPatientInformationAsString(exam);
      if(STRCASECMP(this->Internal->PairImageIdAndName.second.c_str(),examAndPatientInformation.c_str()) != 0)
      {
        LOG_INFO("Current exam on the server does not match the exam you have requested. Please either update image meta data or select the exam you request on the server");
        return PLUS_FAIL;
      }
      assignedImageId = requestedImageId;
    }
    else
    {
      assignedImageId = this->GetDeviceId() + std::string("-") + this->GetImageMetaDatasetsCountAsString();
    }
    std::string examImageDirectory;
    if(!this->AcquireDicomImage(this->GetDicomImagesOutputDirectory(),examImageDirectory))
    {
      return PLUS_FAIL;
    }
    examImageDirectoryToDelete = examImageDirectory;

    vtkSmartPointer<vtkDICOMImageReader> reader = vtkSmartPointer<vtkDICOMImageReader>::New();
    reader->SetDirectoryName(examImageDirectory.c_str()); 

    //to go from the vtk orientation to lps orientation, the vtk image has to be flipped around y and z axis
    vtkSmartPointer<vtkImageFlip> flipYFilter = vtkSmartPointer<vtkImageFlip>::New();
    flipYFilter->SetFilteredAxis(1); // flip y axis
    flipYFilter->SetInputConnection(reader->GetOutputPort());

    vtkSmartPointer<vtkImageFlip> flipZFilter = vtkSmartPointer<vtkImageFlip>::New();
    flipZFilter->SetFilteredAxis(2); // flip z axis
    flipZFilter->SetInputConnection(flipYFilter->GetOutputPort());
    flipZFilter->Update();
    imageData->DeepCopy(flipZFilter->GetOutput());

    float*  ijkOrigin_LPS = reader->GetImagePositionPatient(); //(0020,0032) ImagePositionPatient
    double* ijkVectorMagnitude_LPS = reader->GetPixelSpacing(); //(0020,0037) ImageOrientationPatient

    float*  iDirectionVector_LPS = reader->GetImageOrientationPatient(); 
    float*  jDirectionVector_LPS = reader->GetImageOrientationPatient()+3;
    float   kDirectionVector_LPS[3]={0}; // the third cosine direction is the cross product of the other two vectors
    vtkMath::Cross(iDirectionVector_LPS,jDirectionVector_LPS,kDirectionVector_LPS);

    vtkSmartPointer<vtkMatrix4x4> examIjkToRpiTransform = vtkSmartPointer<vtkMatrix4x4>::New(); // image to ExamRpi, medtronic exludes orientation
    examIjkToRpiTransform->SetElement(0,0,-1);
    examIjkToRpiTransform->SetElement(2,2,-1);
    int xMin,xMax,yMin,yMax,zMin,zMax; //Dimensions, necessary to calculate the new origin in exam rpi
    reader->GetDataExtent(xMin,xMax,yMin,yMax,zMin,zMax);
    //the origin is shifted from lps to rpi aka shifted along x and z axes
    double newOrigin_ExamRpi[3]; // medtronic uses rpi and considers the dicom origin to be zero and also the orientation to be idendity
    newOrigin_ExamRpi[0] = (xMax-xMin+1)*ijkVectorMagnitude_LPS[0];
    newOrigin_ExamRpi[1] = 0;
    newOrigin_ExamRpi[2] = (zMax-zMin+1)*ijkVectorMagnitude_LPS[2];
    examIjkToRpiTransform->SetElement(0,3,newOrigin_ExamRpi[0]);
    examIjkToRpiTransform->SetElement(1,3,newOrigin_ExamRpi[1]);
    examIjkToRpiTransform->SetElement(2,3,newOrigin_ExamRpi[2]);

    vtkSmartPointer<vtkMatrix4x4> examIjkToRasTransform = vtkSmartPointer<vtkMatrix4x4>::New(); // ijkToRas: 3x3 direction cosines + origin 1X3
    for(int i=0;i<3;i++)
    {	
      //the dicom image is in lps. To translate it to ras, x and y are "flipped" aka multipled by -1
      if(i==0 || i==1)
      {
        examIjkToRasTransform->SetElement(i,0,-iDirectionVector_LPS[i]);
        examIjkToRasTransform->SetElement(i,1,-jDirectionVector_LPS[i]);
        examIjkToRasTransform->SetElement(i,2,-kDirectionVector_LPS[i]);
      }
      else
      {
        examIjkToRasTransform->SetElement(i,0,iDirectionVector_LPS[i]);
        examIjkToRasTransform->SetElement(i,1,jDirectionVector_LPS[i]);
        examIjkToRasTransform->SetElement(i,2,kDirectionVector_LPS[i]);
      }	
    }
    //Set the elements of the transformation matrix, x and y are flipped here as well, multiplied by 1
    examIjkToRasTransform->SetElement(0,3,-ijkOrigin_LPS[0]);
    examIjkToRasTransform->SetElement(1,3,-ijkOrigin_LPS[1]);
    examIjkToRasTransform->SetElement(2,3,ijkOrigin_LPS[2]);

    //These matrices are needed to calculate rasToTracker
    this->InternalShared->SetExamIjkToRpiTransformMatrix(examIjkToRpiTransform); //thread safe with get function
    this->InternalShared->SetExamIjkToRasTransformMatrix(examIjkToRasTransform);         //thread safe with get function
    this->InternalShared->SetExamValid(true);                            // thread safe with get function

    if(imageReferencFrameName.compare("Ras") == 0) 
    {
      for(int i=0; i<4; i++)
      {
        for(int j=0;j<4;j++)
        {	
          ijkToReferenceTransform->SetElement(i,j,examIjkToRasTransform->GetElement(i,j));
        }
      }
    }
    else
    {
      MNavStealthLink::Exam exam;
      this->InternalShared->GetCurrentExam(exam);
      vtkAccurateTimer::Delay(1);  // Delay 1 second to make sure that this->Internal->RasToTracker is calculated correctly based on the new matrices, ijkToExamRpiTransform and ijkToRasTransform
      const PlusTransformName rasToTrackerTransformName("Ras","Tracker");

      MNavStealthLink::NavData navData;
      if(!this->InternalShared->GetCurrentNavigationData(navData))
      {
        return PLUS_FAIL;
      }

      vtkSmartPointer<vtkMatrix4x4> frameToTrackerTransform = vtkSmartPointer<vtkMatrix4x4>::New();
      bool frameOutOfView;
      this->Internal->GetFrameInformation(navData,frameOutOfView,frameToTrackerTransform);

      vtkSmartPointer<vtkMatrix4x4> rasToTrackerTransform = vtkSmartPointer<vtkMatrix4x4>::New();
      if(frameOutOfView == true)
      {
        LOG_WARNING("The frame is out of view. If you requested the image in Stylus, Frame or Tracker coordinate system, this will give wrong results. Please make sure that the frame is visible");
      }
      else
      {
        this->Internal->GetRasToTrackerTransform(frameToTrackerTransform,rasToTrackerTransform); // thread-safe
      }
      this->Internal->TransformRepository->SetTransform(rasToTrackerTransformName,rasToTrackerTransform);
      vtkSmartPointer<vtkMatrix4x4> rasToReferenceTransform = vtkSmartPointer<vtkMatrix4x4>::New();
      const PlusTransformName rasToReferenceTransformName("Ras",imageReferencFrameName.c_str());
      this->Internal->TransformRepository->GetTransform(rasToReferenceTransformName,rasToReferenceTransform);
      vtkMatrix4x4::Multiply4x4(rasToReferenceTransform,examIjkToRasTransform,ijkToReferenceTransform);
    }
  }
  this->Internal->DicomImagesOutputDirectory = vtkPlusConfig::GetInstance()->GetOutputDirectory() +  std::string("/StealthLinkDicomOutput");
  if(!this->Internal->KeepReceivedDicomFiles) //if it enters here then it means keepReceivedDicomFiles is false no need to set it to false again
  {
    return this->DeleteDicomImageOutputDirectory(examImageDirectoryToDelete);
  }
  this->SetKeepReceivedDicomFiles(false);
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkStealthLinkTracker::InternalStartRecording()
{
  if (!this->InternalShared->IsStealthServerConnected())
  {
    LOG_ERROR("InternalStartRecording failed: StealthLinkServer has not been initialized");
    return PLUS_FAIL;
  }
  return PLUS_SUCCESS;
}
//----------------------------------------------------------------------------
PlusStatus vtkStealthLinkTracker::InternalStopRecording()
{
  // No need to do anything here, as the StealthLinkServer only performs grabbing on request
  return PLUS_SUCCESS;
}
//----------------------------------------------------------------------------
PlusStatus vtkStealthLinkTracker::ReadConfiguration( vtkXMLDataElement* rootConfigElement )
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_READING(deviceConfig, rootConfigElement);

  XML_READ_STRING_ATTRIBUTE_REQUIRED(ServerAddress, deviceConfig);
  XML_READ_STRING_ATTRIBUTE_REQUIRED(ServerPort, deviceConfig);
  
  std::string deviceIdStr(this->GetDeviceId()?this->GetDeviceId():"");
  if(deviceIdStr.size() > MAX_DEVICE_ID_LENGTH)
  {
    LOG_WARNING("The device id "<<deviceIdStr<<" might be too long, as it may be used for generating identifiers for images that will be sent through OpenIGTLink. Consider choosing a shorter device Id. Example: SLD1");
  }
  
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkStealthLinkTracker::WriteConfiguration(vtkXMLDataElement* rootConfig)
{
  XML_FIND_DEVICE_ELEMENT_REQUIRED_FOR_WRITING(deviceElement, rootConfig);
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
PlusStatus vtkStealthLinkTracker::InternalConnect()
{ 
  LOG_TRACE( "vtkStealthLinkTracker::InternalConnect" );
  if (this->InternalShared->IsStealthServerConnected())
  {
    LOG_DEBUG("Already connected to StealthLink");
    return PLUS_SUCCESS;
  }

  if (this->Internal->ServerAddress.empty() || this->Internal->ServerPort.empty())
  {
    LOG_ERROR("Cannot connect: Server Address or Server Port are not assigned\n");
    return PLUS_FAIL;
  }
  LOG_TRACE("Server Address: " << this->Internal->ServerAddress << " " << "Server Port: " << this->Internal->ServerPort << "\n");

  if(!this->InternalShared->ConnectToStealthStation(this->Internal->ServerAddress,this->Internal->ServerPort))
  {
    return PLUS_FAIL;
  }
  
  //Check if the instrument port names in the config file are valid
  bool valid;
  if(!this->AreInstrumentPortNamesValid(valid))
  {
    return PLUS_FAIL;
  }
  if(!valid)
    return PLUS_FAIL;
  //Check of the Localizer(Tracker) is connected
  bool connected;
  if(!this->InternalShared->IsLocalizerConnected(connected))
  {
    return PLUS_FAIL;
  }
  if(!connected)
  {
    LOG_ERROR("Localizer(Tracker) is not connected. Please check the StealthLink Server\n")
    return PLUS_FAIL;
  }

  //Get the time difference between the StealthServer and the vtkAccurateTimer
  double serverTimeInMicroSeconds=0;
  if(!this->InternalShared->GetStealthStationServerTime(serverTimeInMicroSeconds))
  {
    return PLUS_FAIL;
  }
  this->InternalUpdatePrivate->ServerInitialTimeInMicroSeconds = serverTimeInMicroSeconds;  
  
  this->InternalUpdatePrivate->TrackerTimeToSystemTimeSec =  vtkAccurateTimer::GetSystemTime();

  this->Internal->DicomImagesOutputDirectory = vtkPlusConfig::GetInstance()->GetOutputDirectory() +  std::string("/StealthLinkDicomOutput");
  this->Internal->KeepReceivedDicomFiles = false;
  this->Internal->ImageMetaDatasetsCount = 1;
  this->InternalShared->SetExamValid(false);
  return PLUS_SUCCESS;
}
//----------------------------------------------------------------------------
PlusStatus vtkStealthLinkTracker::InternalDisconnect()
{ 
  if (this->InternalShared->IsStealthServerConnected())
  {
    this->InternalShared->DisconnectFromStealthStation();
  }
  return PLUS_SUCCESS;
}
//-----------------------------------------------------------------------------
PlusStatus vtkStealthLinkTracker::InternalUpdate()
{
  if (!this->InternalShared->IsStealthServerConnected()) //thread safe for stealthlink
  {
    LOG_ERROR("InternalUpdate failed: StealthLinkServer has not been initialized");
    return PLUS_FAIL;
  }	

  double serverTimeInMicroSec;
  if(!this->InternalShared->GetStealthStationServerTime(serverTimeInMicroSec))
  {
    return PLUS_FAIL;
  }
  double timeSystemSec=0.0;
  timeSystemSec = (serverTimeInMicroSec - this->InternalUpdatePrivate->ServerInitialTimeInMicroSeconds)/(1e6) + this->InternalUpdatePrivate->TrackerTimeToSystemTimeSec;
  double unfilteredTime = vtkAccurateTimer::GetSystemTime();
  //----------------------------------------------------------
  MNavStealthLink::NavData navData;
  if(!this->InternalShared->GetCurrentNavigationData(navData)) //thread safe
  {
    return PLUS_FAIL;
  }

  vtkSmartPointer<vtkMatrix4x4> instrumentToTrackerTransform = vtkSmartPointer<vtkMatrix4x4>::New();
  bool instrumentOutOfView=true;
  vtkStealthLinkTracker::vtkInternal::GetInstrumentInformation(navData,instrumentOutOfView,instrumentToTrackerTransform);

  vtkSmartPointer<vtkMatrix4x4> frameToTrackerTransform = vtkSmartPointer<vtkMatrix4x4>::New();
  bool frameOutOfView=true;
  vtkStealthLinkTracker::vtkInternal::GetFrameInformation(navData,frameOutOfView,frameToTrackerTransform);

  for ( DataSourceContainerConstIterator toolIterator = this->GetToolIteratorBegin(); toolIterator != this->GetToolIteratorEnd(); ++toolIterator)
  {
    //if the wanted transformation is the frameToTracker
    if(!strcmp(toolIterator->second->GetPortName(),navData.frameName.c_str())) // static!
    {
      if(frameOutOfView == false)
      {
        this->ToolTimeStampedUpdateWithoutFiltering( toolIterator->second->GetSourceId(), frameToTrackerTransform, TOOL_OK, unfilteredTime, timeSystemSec);
      }
      else if(frameOutOfView == true)
      {
        this->ToolTimeStampedUpdateWithoutFiltering( toolIterator->second->GetSourceId(), frameToTrackerTransform, TOOL_OUT_OF_VIEW, unfilteredTime, timeSystemSec);
      }
    }
    // if the wanted transformation is rasToTracker
    else if(!strcmp(toolIterator->second->GetPortName(),vtkInternalShared::GetRasRegistrationToolName())) // static!
    {
      // If frame is not out of view, the RasToTrackerTransform is invalid
      bool examValid =	this->InternalShared->GetExamValid(); //thread safe with the set function
      if(frameOutOfView == false && examValid == true)
      {
        vtkSmartPointer<vtkMatrix4x4> rasToTrackerTransform = vtkSmartPointer<vtkMatrix4x4>::New();
        this->Internal->GetRasToTrackerTransform(frameToTrackerTransform,rasToTrackerTransform); // thread-safe
        this->ToolTimeStampedUpdateWithoutFiltering( toolIterator->second->GetSourceId(), rasToTrackerTransform, TOOL_OK, unfilteredTime, timeSystemSec);
      }
      else      
      {
        vtkSmartPointer<vtkMatrix4x4> rasToTrackerTransform = vtkSmartPointer<vtkMatrix4x4>::New();
        this->ToolTimeStampedUpdateWithoutFiltering( toolIterator->second->GetSourceId(), rasToTrackerTransform, TOOL_OUT_OF_VIEW, unfilteredTime, timeSystemSec);
      }
    }
    // if the wanted Transform is any tool to Tracker example sytlusToTracker, probeToTracker etc
    else if(!strcmp(toolIterator->second->GetPortName(),navData.instrumentName.c_str()))
    {
      if(instrumentOutOfView == false)
      {
        this->ToolTimeStampedUpdateWithoutFiltering( toolIterator->second->GetSourceId(), instrumentToTrackerTransform, TOOL_OK, unfilteredTime, timeSystemSec);
      }
      else if(instrumentOutOfView == true)
      {
        this->ToolTimeStampedUpdateWithoutFiltering( toolIterator->second->GetSourceId(), instrumentToTrackerTransform, TOOL_OUT_OF_VIEW, unfilteredTime, timeSystemSec);
      }
    }
    else
    {
      vtkSmartPointer<vtkMatrix4x4> transformMatrixForNotTrackedTool = vtkSmartPointer<vtkMatrix4x4>::New();
      this->ToolTimeStampedUpdateWithoutFiltering( toolIterator->second->GetSourceId(), transformMatrixForNotTrackedTool, TOOL_OUT_OF_VIEW, unfilteredTime, timeSystemSec);
    }
  }
  return PLUS_SUCCESS;
}

//------------------------------------------------------------------------------

//--
void vtkStealthLinkTracker::RemoveForbiddenCharacters(std::string& str)
{
  std::vector<int> spacePlaces;
  for(int i=0; i<str.size(); i++)\
  {
    int asciiCode = (int) str[i];
    if(str[i] == ' ')
    {
      spacePlaces.push_back(i);
    }
    else if(str[i] == '\\' || str[i] == '/')
    {	
      str[i] = '-';
    }
    else if(asciiCode<48 || (asciiCode>57 && asciiCode<65) || (asciiCode>90 && asciiCode<97) || asciiCode>122)
    {
      str[i] = ' ';
      spacePlaces.push_back(i);
    }
  }
  std::string subStrPatientName;
  int placement = 0;
  for(std::vector<int>::iterator it = spacePlaces.begin(); it!=spacePlaces.end(); it++)
  {
    subStrPatientName=str.substr(*it+1-placement);
    str=str.substr(0,*it-placement);
    str = str + subStrPatientName;
    placement++;
  }
}
//---------------------------------------------------------------------------
std::string vtkStealthLinkTracker::GetDicomImagesOutputDirectory()
{
  return this->Internal->DicomImagesOutputDirectory;
}
void vtkStealthLinkTracker::SetDicomImagesOutputDirectory(std::string dicomImagesOutputDirectory)
{
  this->Internal->DicomImagesOutputDirectory = dicomImagesOutputDirectory;
}
//---------------------------------------------------------------------------
void vtkStealthLinkTracker::SetKeepReceivedDicomFiles(bool keepReceivedDicomFiles)
{
  this->Internal->KeepReceivedDicomFiles = keepReceivedDicomFiles;
}
//----------------------------------------------------------------------------
PlusStatus vtkStealthLinkTracker::AcquireDicomImage(std::string dicomImagesOutputDirectory, std::string& examImageDirectory)
{

  vtkDirectory::MakeDirectory(dicomImagesOutputDirectory.c_str());
  
  MNavStealthLink::Exam exam;
  this->InternalShared->GetCurrentExam(exam);
  if(exam.patientName.empty())
  {
    return PLUS_FAIL;
  }
  std::string description = exam.description;
  this->RemoveForbiddenCharacters(description);
  std::string patientName = exam.patientName;
  this->RemoveForbiddenCharacters(patientName);
  examImageDirectory = std::string(dicomImagesOutputDirectory) + std::string("/") + patientName + std::string("_") + description + std::string("_") + vtkAccurateTimer::GetInstance()->GetDateAndTimeString();
  if(!this->InternalShared->GetExamData(exam,examImageDirectory))
  {
    return PLUS_FAIL;
  }
  return PLUS_SUCCESS;
}
//----------------------------------------------------------------------
PlusStatus vtkStealthLinkTracker::AreInstrumentPortNamesValid(bool& valid)
{
  std::vector<std::string> validToolPortNames;
  if(!this->InternalShared->GetValidToolPortNames(validToolPortNames))
  {
    valid = false;
    return PLUS_FAIL;
  }
  for ( DataSourceContainerConstIterator toolIterator = this->GetToolIteratorBegin(); toolIterator != this->GetToolIteratorEnd(); ++toolIterator)
  {
    bool found = false;
    for(int i=0; i<validToolPortNames.size(); i++)
    {
      if(validToolPortNames[i].compare(toolIterator->second->GetPortName()) == 0)
      {
        found = true;
        break;
      }
    }
    if(found == false)
    {
      LOG_ERROR( toolIterator->second->GetPortName() << " has not been found in the server. Please make sure to use correct port names for the tools\n");
      valid = false;
      std::string strValidToolPortNames("Valid tool port names are:\n");
      for(int i=0; i<validToolPortNames.size(); i++)
      {
        strValidToolPortNames += validToolPortNames[i] + std::string("\n");
      }
      LOG_INFO(strValidToolPortNames);
      return PLUS_FAIL;
    }
    found = false;
  }
  valid = true;
  return PLUS_SUCCESS;
}
//----------------------------------------------------------------------------
PlusStatus vtkStealthLinkTracker::DeleteDicomImageOutputDirectory(std::string examImageDirectory)
{
  vtkSmartPointer<vtkDirectory> dicomDirectory = vtkSmartPointer<vtkDirectory>::New();
  if(!dicomDirectory->Open(examImageDirectory.c_str()))
  {
    LOG_ERROR("Cannot open the folder: " << examImageDirectory);
    return PLUS_FAIL;
  }

  // Delete the directory of the last exam
  vtkDirectory::DeleteDirectory(examImageDirectory.c_str());

  // Delete the parent directory if this was the only exam
  if(!dicomDirectory->Open(this->GetDicomImagesOutputDirectory().c_str()))
  {
    LOG_ERROR("Cannot open the folder: " << this->GetDicomImagesOutputDirectory());
    return PLUS_FAIL;
  }
  if(dicomDirectory->GetNumberOfFiles() == 2)
  {
    std::string file0(".");
    std::string file1("..");
    if((file0.compare(dicomDirectory->GetFile(0)) == 0 && file1.compare(dicomDirectory->GetFile(1)) ==0) || 
       (file0.compare(dicomDirectory->GetFile(1)) == 0 && file1.compare(dicomDirectory->GetFile(0)) ==0))
    {
      vtkDirectory::DeleteDirectory(this->GetDicomImagesOutputDirectory().c_str());
    }
  }
  
  return PLUS_SUCCESS;
}

//----------------------------------------------------------------------------
std::string vtkStealthLinkTracker::GetImageMetaDatasetsCountAsString()
{
  std::ostringstream message;
  message << std::setw(3) << std::setfill('0') << this->Internal->ImageMetaDatasetsCount << std::ends; 
  this->Internal->ImageMetaDatasetsCount++;
  return message.str();
}

//----------------------------------------------------------------------------
void vtkStealthLinkTracker::SetServerAddress(const char* serverAddress)
{
  this->Internal->ServerAddress = serverAddress?serverAddress:"";
}

//----------------------------------------------------------------------------
void vtkStealthLinkTracker::SetServerPort(const char* serverPort)
{
  this->Internal->ServerPort = serverPort?serverPort:"";
}
