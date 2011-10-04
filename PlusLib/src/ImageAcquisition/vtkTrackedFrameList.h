// Store a list of tracked frames (image + pose information)

#ifndef __VTKTRACKEDFRAMELIST_H
#define __VTKTRACKEDFRAMELIST_H

#include "PlusConfigure.h"
#include "vtkObject.h"
#include <deque>

#include "itkImage.h"
#include "vtkMatrix4x4.h"
#include "vtkTransform.h"
#include "UsImageConverterCommon.h"
#include "vtkPoints.h"
#include "vtkXMLDataElement.h"
#include "PlusVideoFrame.h"

class vtkPoints; 

//----------------------------------------------------------------------------
// ************************* TrackedFrame ************************************
//----------------------------------------------------------------------------
class VTK_EXPORT TrackedFrame
{
public:
  typedef std::map<std::string,std::string> FieldMapType; 

public:
  TrackedFrame();
  ~TrackedFrame(); 
  TrackedFrame(const TrackedFrame& frame); 
  TrackedFrame& TrackedFrame::operator=(TrackedFrame const&trackedFrame); 

public:
  // Set image data
  void SetImageData(PlusVideoFrame value) { this->ImageData = value; }; 

  // Get image data
  PlusVideoFrame* GetImageData() { return &(this->ImageData); };

  // Set Status of the item (out of view, ...)
  void SetStatus(TrackerStatus value) { this->Status = value; }; 

  // Get Status of the item (out of view, ...)
  TrackerStatus GetStatus() { return this->Status; };

  // Set timestamp
  void SetTimestamp(double value) { this->Timestamp = value; }; 

  // Get timestamp
  double GetTimestamp() { return this->Timestamp; }; 

  //! Set custom frame field
  void SetCustomFrameField( std::string name, std::string value ); 

  //! Get custom frame field value
  const char* GetCustomFrameField(const char* fieldName); 

  //! Operation: 
  // Get custom frame transform
  PlusStatus GetCustomFrameTransform(const char* frameTransformName, double transform[16]); 
  PlusStatus GetCustomFrameTransform(const char* frameTransformName, vtkMatrix4x4* transformMatrix); 

  //! Set custom frame transform
  void SetCustomFrameTransform(std::string frameTransformName, double transform[16]); 

  //! Set custom frame transform
  void SetCustomFrameTransform(std::string frameTransformName, vtkMatrix4x4* transform); 

  //! Get the list of the name of all custom frame fields
  void GetCustomFrameFieldNameList(std::vector<std::string> &fieldNames);

  //! Operation: 
  // Returns: int[2]. Get tracked frame size in pixel
  int* GetFrameSize(); 

  //! Get tracked frame pixel size in bits 
  int GetNumberOfBitsPerPixel();

  // Set Segmented fiducial point pixel coordinates
  void SetFiducialPointsCoordinatePx(vtkPoints* fiducialPoints)
  {
    if ( this->FiducialPointsCoordinatePx != fiducialPoints )
    {
      vtkPoints* tempFiducialPoints = this->FiducialPointsCoordinatePx; 
      
      this->FiducialPointsCoordinatePx = fiducialPoints; 
      if ( this->FiducialPointsCoordinatePx != NULL ) 
      {
        this->FiducialPointsCoordinatePx->Register(NULL); 
      } 

      if ( tempFiducialPoints != NULL ) 
      {
        tempFiducialPoints->UnRegister(NULL); 
      }
    }
  };

  // Get Segmented fiducial point pixel coordinates
  vtkPoints* GetFiducialPointsCoordinatePx() { return this->FiducialPointsCoordinatePx; };

  //! 
  PlusStatus WriteToFile(std::string &filename, vtkMatrix4x4* mImageToTracker);

  //! Convert from status string to status enum
  static TrackerStatus GetStatusFromString(const char* statusStr);

public:
  bool operator< (TrackedFrame data) { return Timestamp < data.Timestamp; }
  bool operator== (const TrackedFrame& data) const 
  {
    return (Timestamp == data.Timestamp);
  }

protected:
  PlusVideoFrame ImageData;
  TrackerStatus Status; 
  double Timestamp; 

  FieldMapType CustomFrameFields;

  int FrameSize[2]; 

  // Stores segmented fiducial point pixel coordinates 
  vtkPoints* FiducialPointsCoordinatePx; 
};



//----------------------------------------------------------------------------
// ************************* vtkTrackedFrameList *****************************
//----------------------------------------------------------------------------


class TrackedFrameTimestampFinder
{	
public:
  TrackedFrameTimestampFinder(TrackedFrame* frame): mTrackedFrame(frame){}; 	
  bool operator()( TrackedFrame *newFrame )	
  {		
    return newFrame->GetTimestamp() == mTrackedFrame->GetTimestamp();	
  }	
  TrackedFrame* mTrackedFrame;
};

class TrackedFramePositionFinder
{	
public:
  TrackedFramePositionFinder(TrackedFrame* frame, const char* frameTransformName, double minRequiredTranslationDifferenceMm /*= 0.5*/, double minRequiredAngleDifferenceDeg /*= 0.2*/)
    : mTrackedFrame(frame), 
    mMinRequiredTranslationDifferenceMm(minRequiredTranslationDifferenceMm),
    mMinRequiredAngleDifferenceDeg(minRequiredAngleDifferenceDeg),
    mFrameTransformName(frameTransformName) 
  {
  }

  //! TODO Description
  bool operator()( TrackedFrame *newFrame )	
  {
    if (mMinRequiredTranslationDifferenceMm<=0 || mMinRequiredAngleDifferenceDeg<=0)
    {
      // threshold is zero, so the frames are different for sure
      return false;
    }

    vtkSmartPointer<vtkTransform> baseTransform = vtkSmartPointer<vtkTransform>::New(); 
    double baseTransMatrix[16]={0}; 
    if ( mTrackedFrame->GetCustomFrameTransform(mFrameTransformName.c_str(), baseTransMatrix) )
    {
      baseTransform->SetMatrix(baseTransMatrix); 
    }
    else
    {
      LOG_ERROR("TrackedFramePositionFinder: Unable to find base frame transform name for tracked frame validation!"); 
      return false; 
    }

    vtkSmartPointer<vtkTransform> newTransform = vtkSmartPointer<vtkTransform>::New(); 
    double newTransMatrix[16]={0}; 
    if ( newFrame->GetCustomFrameTransform(mFrameTransformName.c_str(), newTransMatrix) )
    {
      newTransform->SetMatrix(newTransMatrix); 
    }
    else
    {
      LOG_ERROR("TrackedFramePositionFinder: Unable to find frame transform name for new tracked frame validation!"); 
      return false; 
    }

    double bx(0), by(0), bz(0), cx(0), cy(0), cz(0); 
    if ( STRCASECMP(mFrameTransformName.c_str(), "StepperEncoderValues") == 0 )
    {
      // Probe positions are stored in matrix element (0,3)
      // Template positions are stored in matrix element (2,3)
      // Image position = probe position + grid position 
      bz = baseTransform->GetMatrix()->GetElement(0,3) + baseTransform->GetMatrix()->GetElement(2,3); 
      cz = newTransform->GetMatrix()->GetElement(0,3) + newTransform->GetMatrix()->GetElement(2,3);
    }
    else
    {
      bx = baseTransform->GetPosition()[0]; 
      by = baseTransform->GetPosition()[1]; 
      bz = baseTransform->GetPosition()[2]; 
      cx = newTransform->GetPosition()[0]; 
      cy = newTransform->GetPosition()[1]; 
      cz = newTransform->GetPosition()[2]; 
    }

    // Euclidean distance
    double distance = sqrt( pow(bx-cx,2) + pow(by-cy,2) + pow(bz-cz,2) ); 

    vtkSmartPointer<vtkMatrix4x4> diffTransMatrix = vtkSmartPointer<vtkMatrix4x4>::New(); 
    vtkSmartPointer<vtkMatrix4x4> invNewTransMatrix = vtkSmartPointer<vtkMatrix4x4>::New(); 
    vtkMatrix4x4::Invert(newTransform->GetMatrix(), invNewTransMatrix);  

    vtkMatrix4x4::Multiply4x4(baseTransform->GetMatrix(), invNewTransMatrix, diffTransMatrix); 
    vtkSmartPointer<vtkTransform> diffTransform = vtkSmartPointer<vtkTransform>::New(); 
    diffTransform->SetMatrix(diffTransMatrix); 

    double angleDifference(0); 
    if ( STRCASECMP(mFrameTransformName.c_str(), "StepperEncoderValues") == 0 )
    {
      // Rotation angles are stored in matrix element (1,3)
      angleDifference = baseTransform->GetMatrix()->GetElement(1,3) - newTransform->GetMatrix()->GetElement(1,3); 
    }
    else
    {
      angleDifference = diffTransform->GetOrientationWXYZ()[0]; 
    }

    if ( distance < this->mMinRequiredTranslationDifferenceMm && abs(angleDifference) < this->mMinRequiredAngleDifferenceDeg )
    {
      // same as the reference frame
      return true; 
    }

    return false; 
  }	

  TrackedFrame* mTrackedFrame;
  double mMinRequiredTranslationDifferenceMm;
  double mMinRequiredAngleDifferenceDeg;
  std::string mFrameTransformName;  

};

class VTK_EXPORT vtkTrackedFrameList : public vtkObject
{

public:
  typedef std::deque<TrackedFrame*> TrackedFrameListType; 
  typedef std::map<std::string,std::string> FieldMapType; 
  enum SEQ_METAFILE_EXTENSION
  {
    SEQ_METAFILE_MHA, 
    SEQ_METAFILE_MHD
  }; 

  static vtkTrackedFrameList *New();
  vtkTypeRevisionMacro(vtkTrackedFrameList, vtkObject);
  virtual void PrintSelf(ostream& os, vtkIndent indent); 

  //! Operation: 
  // Add tracked frame to container and returns with the position in the vector
  virtual int AddTrackedFrame(TrackedFrame *trackedFrame); 

  //! Operation: 
  // Get tracked frame from container
  virtual TrackedFrame* GetTrackedFrame(int frameNumber); 

  //! Operation: 
  // Get number of tracked frames
  virtual unsigned int GetNumberOfTrackedFrames() { return this->TrackedFrameList.size(); } 

  // Description:
  // Read configuration from xml data
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* config); 

  //! Operation: 
  // Save the tracked data to sequence metafile 
  PlusStatus SaveToSequenceMetafile(const char* outputFolder, const char* sequenceDataFileName, SEQ_METAFILE_EXTENSION extension = SEQ_METAFILE_MHA, bool useCompression = true);
  
  template <class OutputPixelType>
  PlusStatus SaveToSequenceMetafileGeneric(const char* outputFolder, const char* sequenceDataFileName, SEQ_METAFILE_EXTENSION extension = SEQ_METAFILE_MHA, bool useCompression = true);

  //! Operation: 
  // Read the tracked data from sequence metafile 
  virtual PlusStatus ReadFromSequenceMetafile(const char* trackedSequenceDataFileName); 

  //! Operation: 
  // Get the tracked frame list 
  TrackedFrameListType GetTrackedFrameList() { return this->TrackedFrameList; }

  //! Operation: 
  // Validate tracked frame before adding to the list.
  // Control arguments: 
  // validateTimestamp: the timestamp should be unique 
  // validateStatus: the tracking flags should be valid (TR_OK)
  // validatePosition: the frame position should be different from the previous ones 
  // validateSpeed: the frame acquisition speed should be less than a threshold
  virtual bool ValidateData(TrackedFrame* trackedFrame, bool validateTimestamp = true, bool validateStatus = true, bool validatePosition = true, const char* frameTransformName = NULL, bool validateSpeed = true); 

  //! Operation: 
  // Clear tracked frame list and free memory
  virtual void Clear(); 

  virtual std::string GetDefaultFrameTransformName(); 
  virtual void SetDefaultFrameTransformName(const char* name); 

  //! Operation: 
  // Set/get the maximum number of frames to write into a single metafile. 
  vtkSetMacro(MaxNumOfFramesToWrite, int); 
  vtkGetMacro(MaxNumOfFramesToWrite, int); 

  //! Operation: 
  // Set/get the number of following unique frames needed in the tracked frame list
  vtkSetMacro(NumberOfUniqueFrames, int); 
  vtkGetMacro(NumberOfUniqueFrames, int); 

  //! Operation: 
  // Set/get the threshold of acceptable speed of position change
  vtkSetMacro(MinRequiredTranslationDifferenceMm, double); 
  vtkGetMacro(MinRequiredTranslationDifferenceMm, double); 

  //! Operation: 
  // Set/get the threshold of acceptable speed of orientation change in degrees
  vtkSetMacro(MinRequiredAngleDifferenceDeg, double); 
  vtkGetMacro(MinRequiredAngleDifferenceDeg, double); 

  //! Operation: 
  // Get tracked frame size in pixel
  virtual int* GetFrameSize(); 

  //! Operation: 
  // Get tracked frame pixel size in bits 
  virtual int GetNumberOfBitsPerPixel(); 

  //! Operation: 
  // Get tracked frame pixel type 
  PlusCommon::ITKScalarPixelType GetPixelType(); 

  // Get the value of the custom field
  // If we couldn't find it, return NULL
  virtual const char* GetCustomString( const char* fieldName ); 

  // Set custom string value to <fieldValue>. If <fieldValue> is NULL then the field is deleted.
  virtual PlusStatus SetCustomString(const char* fieldName, const char* fieldValue); 

  // Get the custom transformation matrix from metafile by custom frame transform name
  // It will search for a field like: Seq_Frame<frameNumber>_<frameTransformName>
  // Return false if the the field is missing 
  virtual PlusStatus GetCustomTransform( const char* frameTransformName, vtkMatrix4x4* transformMatrix ); 
  virtual PlusStatus GetCustomTransform( const char* frameTransformName, double* transformMatrix ); 

  // Set the custom transformation matrix from metafile by custom frame transform name
  // It will search for a field like: Seq_Frame<frameNumber>_<frameTransformName>
  virtual void SetCustomTransform( const char* frameTransformName, vtkMatrix4x4* transformMatrix ); 
  virtual void SetCustomTransform( const char* frameTransformName, double* transformMatrix ); 

  void GetCustomFieldNameList(std::vector<std::string> &fieldNames);

  //! Get/Set global transform (stored in the Offset and TransformMatrix fields)
  PlusStatus GetGlobalTransform(vtkMatrix4x4* globalTransform);
  PlusStatus SetGlobalTransform(vtkMatrix4x4* globalTransform);

protected:
  vtkTrackedFrameList();
  virtual ~vtkTrackedFrameList();

  //! Operation: 
  // Set tracked frame size in pixel
  vtkSetVector2Macro(FrameSize, int); 

  bool ValidateTimestamp(TrackedFrame* trackedFrame); 
  bool ValidateStatus(TrackedFrame* trackedFrame); 
  bool ValidatePosition(TrackedFrame* trackedFrame, const char* frameTransformName); 
  bool ValidateSpeed(TrackedFrame* trackedFrame);

  TrackedFrameListType TrackedFrameList; 
  FieldMapType CustomFields;

  int MaxNumOfFramesToWrite;
  int NumberOfUniqueFrames;
  int FrameSize[2];

  // If the threshold==0 it means that no checking is needed (the frame is always accepted).
  // If the threshold>0 then the frame is considered valid only if the position/angle difference compared to all previously acquired frames is larger than
  // the position/angle minimum value and the translation/rotation speed is lower than the maximum allowed translation/rotation.
  double MinRequiredTranslationDifferenceMm;
  double MinRequiredAngleDifferenceDeg;
  double MaxAllowedTranslationSpeedMmPerSec;
  double MaxAllowedRotationSpeedDegPerSec;

private:
  vtkTrackedFrameList(const vtkTrackedFrameList&);
  void operator=(const vtkTrackedFrameList&);
}; 

#endif // __VTKTRACKEDFRAMELIST_H


