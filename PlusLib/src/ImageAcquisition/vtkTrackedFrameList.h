/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __VTKTRACKEDFRAMELIST_H
#define __VTKTRACKEDFRAMELIST_H

#include "PlusConfigure.h"
#include "vtkObject.h"
#include <deque>
#include "PlusMath.h"

#include "vtkMatrix4x4.h"
#include "vtkTransform.h"
#include "vtkPoints.h"
#include "PlusVideoFrame.h"

class vtkXMLDataElement; 

/** \class TrackedFrame 
 *
 *  \brief Stores tracked frame (image + pose information)
 *
 *  \ingroup PlusLibImageAcquisition
 *
 */ 
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
  /*! Set image data */
  void SetImageData(PlusVideoFrame value) { this->ImageData = value; }; 

  /*! Get image data */
  PlusVideoFrame* GetImageData() { return &(this->ImageData); };

  /*! Set Status of the item (out of view, ...) */
  void SetStatus(TrackerStatus value) { this->Status = value; }; 

  /*! Get Status of the item (out of view, ...) */
  TrackerStatus GetStatus() { return this->Status; };

  /*! Set timestamp */
  void SetTimestamp(double value) { this->Timestamp = value; }; 

  /*! Get timestamp */
  double GetTimestamp() { return this->Timestamp; }; 

  /*! Set custom frame field */
  void SetCustomFrameField( std::string name, std::string value ); 

  /*! Get custom frame field value */
  const char* GetCustomFrameField(const char* fieldName); 

  /*! Get custom frame transform */
  PlusStatus GetCustomFrameTransform(const char* frameTransformName, double transform[16]); 
  PlusStatus GetCustomFrameTransform(const char* frameTransformName, vtkMatrix4x4* transformMatrix); 

  /*! Set custom frame transform */
  void SetCustomFrameTransform(std::string frameTransformName, double transform[16]); 

  /*! Set custom frame transform */
  void SetCustomFrameTransform(std::string frameTransformName, vtkMatrix4x4* transform); 

  /*! Get the list of the name of all custom frame fields */
  void GetCustomFrameFieldNameList(std::vector<std::string> &fieldNames);

  /*! Get tracked frame size in pixel. Returns: int[2].  */
  int* GetFrameSize(); 

  /*! Get tracked frame pixel size in bits */
  int GetNumberOfBitsPerPixel();

  /*! Set Segmented fiducial point pixel coordinates */
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

  /*! Get Segmented fiducial point pixel coordinates */
  vtkPoints* GetFiducialPointsCoordinatePx() { return this->FiducialPointsCoordinatePx; };

  /*! Write image with image to tracker transform to file */ 
  PlusStatus WriteToFile(std::string &filename, vtkMatrix4x4* mImageToTracker);

  /*! Convert from status string to status enum */
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

  /*! Stores segmented fiducial point pixel coordinates */
  vtkPoints* FiducialPointsCoordinatePx; 
};

/*! 
 *  \enum Tracked frame validation requirements 
 *
 *  \brief If any of the requested requirement is not fulfilled then the validation fails. 
 *  \ingroup PlusLibImageAcquisition
 */
enum TrackedFrameValidationRequirements
{
  REQUIRE_UNIQUE_TIMESTAMP = 0x0001, /*!< the timestamp shall be unique */  
  REQUIRE_TRACKING_OK = 0x0002, /*!<  the tracking flags shall be valid (TR_OK) */  
  REQUIRE_CHANGED_ENCODER_POSITION = 0x0004, /*!<  the stepper encoder position shall be different from the previous ones  */  
  REQUIRE_SPEED_BELOW_THRESHOLD = 0x0008, /*!<  the frame acquisition speed shall be less than a threshold */  
  REQUIRE_CHANGED_TRANSFORM = 0x0016, /*!<  the transform defined by name shall be different from the previous ones  */  
}; 

/** \class TrackedFrameTimestampFinder 
 *
 *  \brief Helper class used for validating timestamps in a tracked frame list
 *
 *  \ingroup PlusLibImageAcquisition
 *
 */ 
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


/** \class TrackedFrameEncoderPositionFinder 
*
*  \brief Helper class used for validating encoder position in a tracked frame list
*
*  \ingroup PlusLibImageAcquisition
*
*/ 
class TrackedFrameEncoderPositionFinder
{	
public:
  TrackedFrameEncoderPositionFinder(TrackedFrame* frame, double minRequiredTranslationDifferenceMm, double minRequiredAngleDifferenceDeg)
    : mTrackedFrame(frame), 
    mMinRequiredTranslationDifferenceMm(minRequiredTranslationDifferenceMm),
    mMinRequiredAngleDifferenceDeg(minRequiredAngleDifferenceDeg)
  {
  }

  // TODO: use BrachyTracker::GetStepperEncoderValues if the design allows 
  PlusStatus GetStepperEncoderValues( TrackedFrame* trackedFrame, double &probePosition, double &probeRotation, double &templatePosition )
  {
    // Get the probe position from tracked frame info
    const char* cProbePos = trackedFrame->GetCustomFrameField("ProbePosition"); 
    if ( cProbePos != NULL )
    {
      probePosition = atof(cProbePos); 
    }
    else
    {
      LOG_ERROR("Unable to find frame field: ProbePosition"); 
      return PLUS_FAIL;
    }

    // Get the probe rotation from tracked frame info
    const char* cProbeRot = trackedFrame->GetCustomFrameField("ProbeRotation"); 
    if ( cProbeRot != NULL )
    {
      probeRotation = atof(cProbeRot); 
    }
    else
    {
      LOG_ERROR("Unable to find frame field: ProbeRotation"); 
      return PLUS_FAIL;
    }

    // Get the template position from tracked frame info
    const char* cTemplatePos = trackedFrame->GetCustomFrameField("TemplatePosition"); 
    if ( cTemplatePos != NULL )
    {
      templatePosition = atof(cTemplatePos); 
    }
    else
    {
      LOG_ERROR("Unable to find frame field: TemplatePosition"); 
      return PLUS_FAIL;
    }

    return PLUS_SUCCESS; 
  }

  /*! Predicate unary function for std::find_if to validate encoder position 
  @return Returning true if the encoder position difference is less than required 
  */
  bool operator()( TrackedFrame *newFrame )	
  {
    if (mMinRequiredTranslationDifferenceMm<=0 || mMinRequiredAngleDifferenceDeg<=0)
    {
      // threshold is zero, so the frames are different for sure
      return false;
    }

    double baseProbePos(0), baseProbeRot(0), baseTemplatePos(0); 
    

    if ( GetStepperEncoderValues(mTrackedFrame, baseProbePos, baseProbeRot, baseTemplatePos ) != PLUS_SUCCESS ) 
    {
      LOG_WARNING("Unable to get raw encoder values from tracked frame!" ); 
      return false; 
    }

    double newProbePos(0), newProbeRot(0), newTemplatePos(0); 
    if ( GetStepperEncoderValues(newFrame, newProbePos, newProbeRot, newTemplatePos ) != PLUS_SUCCESS ) 
    {
      LOG_WARNING("Unable to get raw encoder values from tracked frame!" ); 
      return false; 
    }

    double positionDifference = fabs(baseProbePos - newProbePos) + fabs(baseTemplatePos - newTemplatePos); 
    double rotationDifference = fabs(baseProbeRot - newProbeRot); 

    if ( positionDifference < this->mMinRequiredTranslationDifferenceMm && rotationDifference < this->mMinRequiredAngleDifferenceDeg )
    {
      // same as the reference frame
      return true; 
    }
    return false; 
  }	

  TrackedFrame* mTrackedFrame;
  double mMinRequiredTranslationDifferenceMm;
  double mMinRequiredAngleDifferenceDeg;
};

/** \class TrackedFrameTransformFinder 
 *
 *  \brief Helper class used for validating frame transform in a tracked frame list
 *
 *  \ingroup PlusLibImageAcquisition
 *
 */ 
class TrackedFrameTransformFinder
{	
public:
  TrackedFrameTransformFinder(TrackedFrame* frame, const char* frameTransformName, double minRequiredTranslationDifferenceMm /*= 0.5*/, double minRequiredAngleDifferenceDeg /*= 0.2*/)
    : mTrackedFrame(frame), 
    mMinRequiredTranslationDifferenceMm(minRequiredTranslationDifferenceMm),
    mMinRequiredAngleDifferenceDeg(minRequiredAngleDifferenceDeg)
  {
    if (frameTransformName)
    {
      mFrameTransformName = frameTransformName;
    }
    else
    {
      mFrameTransformName.empty();
    }
  }

  /*! Predicate unary function for std::find_if to validate transform 
      @return Returning true if the transform difference is less than required 
  */
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

    double positionDifference = PlusMath::GetPositionDifference( baseTransform->GetMatrix(), newTransform->GetMatrix()); 
    double angleDifference = PlusMath::GetOrientationDifference(baseTransform->GetMatrix(), newTransform->GetMatrix()); 

    if ( abs(positionDifference) < this->mMinRequiredTranslationDifferenceMm && abs(angleDifference) < this->mMinRequiredAngleDifferenceDeg )
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

/** \class vtkTrackedFrameList 
 *
 *  \brief Stores a list of tracked frames (image + pose information)
 *
 *  \ingroup PlusLibImageAcquisition
 *
 */
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

  /*! Add tracked frame to container */
  virtual PlusStatus AddTrackedFrame(TrackedFrame *trackedFrame); 

  /*! Add all frames from a tracked frame list to the container */
  virtual PlusStatus AddTrackedFrameList(vtkTrackedFrameList* inTrackedFrameList); 

  /*! Get tracked frame from container */
  virtual TrackedFrame* GetTrackedFrame(int frameNumber); 

  /*! Get number of tracked frames */
  virtual unsigned int GetNumberOfTrackedFrames() { return this->TrackedFrameList.size(); } 

  /*! Read configuration from xml data */
  virtual PlusStatus ReadConfiguration(vtkXMLDataElement* config); 

  /*! Save the tracked data to sequence metafile */
  PlusStatus SaveToSequenceMetafile(const char* outputFolder, const char* sequenceDataFileName, SEQ_METAFILE_EXTENSION extension = SEQ_METAFILE_MHA, bool useCompression = true);
  
  template <class OutputPixelType>
  PlusStatus SaveToSequenceMetafileGeneric(const char* outputFolder, const char* sequenceDataFileName, SEQ_METAFILE_EXTENSION extension = SEQ_METAFILE_MHA, bool useCompression = true);

  /*! Read the tracked data from sequence metafile */
  virtual PlusStatus ReadFromSequenceMetafile(const char* trackedSequenceDataFileName); 

  /*! Get the tracked frame list */
  TrackedFrameListType GetTrackedFrameList() { return this->TrackedFrameList; }

  /*! Perform validation on a tracked frame before adding to the list. If any of the requested requirement is not fulfilled then the validation fails
   \param trackedFrame Input tracked frame 
   \param validationRequirements Data validation regurirements (like: REQUIRE_UNIQUE_TIMESTAMP | REQUIRE_TRACKING_OK )
   \param frameTransformNameforPositionValidation Frame transform name used for position validation
   \sa TrackedFrameValidationRequirements 
  */
  virtual bool ValidateData(TrackedFrame* trackedFrame, long validationRequirements, const char* frameTransformNameForValidation = NULL ); 

  /*! Clear tracked frame list and free memory */
  virtual void Clear(); 

  /*! Set/getdefault frame transform name */
  virtual std::string GetDefaultFrameTransformName(); 
  virtual void SetDefaultFrameTransformName(const char* name); 

  /*! Set/get the maximum number of frames to write into a single metafile. */
  vtkSetMacro(MaxNumOfFramesToWrite, int); 
  vtkGetMacro(MaxNumOfFramesToWrite, int); 

  /*! Set/get the number of following unique frames needed in the tracked frame list */
  vtkSetMacro(NumberOfUniqueFrames, int); 
  vtkGetMacro(NumberOfUniqueFrames, int); 

  /*! Set/get the threshold of acceptable speed of position change */
  vtkSetMacro(MinRequiredTranslationDifferenceMm, double); 
  vtkGetMacro(MinRequiredTranslationDifferenceMm, double); 

  /*! Set/get the threshold of acceptable speed of orientation change in degrees */
  vtkSetMacro(MinRequiredAngleDifferenceDeg, double); 
  vtkGetMacro(MinRequiredAngleDifferenceDeg, double); 

  /*! Get tracked frame pixel size in bits */
  virtual int GetNumberOfBitsPerPixel(); 

  /*! Get tracked frame pixel type */
  PlusCommon::ITKScalarPixelType GetPixelType(); 

  /*! Get the value of the custom field. If we couldn't find it, return NULL */
  virtual const char* GetCustomString( const char* fieldName ); 

  /*! Set custom string value to <fieldValue>. If <fieldValue> is NULL then the field is deleted. */
  virtual PlusStatus SetCustomString(const char* fieldName, const char* fieldValue); 

  /*! Get the custom transformation matrix from metafile by custom frame transform name
  * It will search for a field like: Seq_Frame<frameNumber>_<frameTransformName>
  * Return false if the the field is missing */
  virtual PlusStatus GetCustomTransform( const char* frameTransformName, vtkMatrix4x4* transformMatrix ); 
  virtual PlusStatus GetCustomTransform( const char* frameTransformName, double* transformMatrix ); 

  /*! Set the custom transformation matrix from metafile by custom frame transform name
  * It will search for a field like: Seq_Frame<frameNumber>_<frameTransformName> */
  virtual void SetCustomTransform( const char* frameTransformName, vtkMatrix4x4* transformMatrix ); 
  virtual void SetCustomTransform( const char* frameTransformName, double* transformMatrix ); 

  /*! Get custom field name list */
  void GetCustomFieldNameList(std::vector<std::string> &fieldNames);

  /*! Get/Set global transform (stored in the Offset and TransformMatrix fields) */
  PlusStatus GetGlobalTransform(vtkMatrix4x4* globalTransform);
  PlusStatus SetGlobalTransform(vtkMatrix4x4* globalTransform);

protected:
  vtkTrackedFrameList();
  virtual ~vtkTrackedFrameList();

  bool ValidateTimestamp(TrackedFrame* trackedFrame); 
  bool ValidateStatus(TrackedFrame* trackedFrame); 
  bool ValidateTransform(TrackedFrame* trackedFrame, const char* frameTransformNameForValidation); 
  bool ValidateEncoderPosition(TrackedFrame* trackedFrame);
  bool ValidateSpeed(TrackedFrame* trackedFrame);

  TrackedFrameListType TrackedFrameList; 
  FieldMapType CustomFields;

  int MaxNumOfFramesToWrite;
  int NumberOfUniqueFrames;

  /*! If the threshold==0 it means that no checking is needed (the frame is always accepted). \n
  * If the threshold>0 then the frame is considered valid only if the position/angle difference compared to all previously acquired frames is larger than 
  * the position/angle minimum value and the translation/rotation speed is lower than the maximum allowed translation/rotation. */
  double MinRequiredTranslationDifferenceMm;
  double MinRequiredAngleDifferenceDeg;
  double MaxAllowedTranslationSpeedMmPerSec;
  double MaxAllowedRotationSpeedDegPerSec;

private:
  vtkTrackedFrameList(const vtkTrackedFrameList&);
  void operator=(const vtkTrackedFrameList&);
}; 

#endif // __VTKTRACKEDFRAMELIST_H


