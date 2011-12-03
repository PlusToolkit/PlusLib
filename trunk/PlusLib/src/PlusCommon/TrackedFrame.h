/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __TRACKEDFRAME_H
#define __TRACKEDFRAME_H

#include "vtkObject.h"
#include <deque>
#include "PlusVideoFrame.h" 

class vtkMatrix4x4; 
class vtkTransform; 
class vtkPoints; 

/*! 
  Tracked frame field status \n
  - Image filed is valid if the image data is not NULL \n
  - Tool status is valid only if the ToolStatus is TOOL_OK \n
  */
enum TrackedFrameFieldStatus 
{
  FIELD_OK,			      /*!< Field is valid */
  FIELD_INVALID       /*!< Field is invalid */
};

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
  void SetImageData(const PlusVideoFrame &value); 

  /*! Get image data */
  PlusVideoFrame* GetImageData() { return &(this->ImageData); };

  /*! Set timestamp */
  void SetTimestamp(double value) { this->Timestamp = value; }; 

  /*! Get timestamp */
  double GetTimestamp() { return this->Timestamp; }; 

  /*! Set custom frame field */
  void SetCustomFrameField( std::string name, std::string value ); 

  /*! Get custom frame field value */
  const char* GetCustomFrameField(const char* fieldName); 

  /*! Delete custom frame field */
  PlusStatus DeleteCustomFrameField( const char* fieldName ); 

  /*! 
    Check if a custom frame field is defined or not 
    \return true, if the field is defined; false, if the field is not defined
  */
  bool IsCustomFrameFieldDefined(const char* fieldName);

  /*! 
    Check if a custom frame transform name field is defined or not 
    \return true, if the field is defined; false, if the field is not defined
  */
  bool IsCustomFrameTransformNameDefined(PlusTransformName& transformName); 

  /*! Get custom frame transform */
  PlusStatus GetCustomFrameTransform(PlusTransformName& frameTransformName, double transform[16]); 
  /*! Get custom frame transform */
  PlusStatus GetCustomFrameTransform(PlusTransformName& frameTransformName, vtkMatrix4x4* transformMatrix); 
  
  /*! Get custom frame status */
  PlusStatus GetCustomFrameTransformStatus(PlusTransformName& frameTransformName, TrackedFrameFieldStatus& status); 
  /*! Set custom frame status */
  PlusStatus SetCustomFrameTransformStatus(PlusTransformName& frameTransformName, TrackedFrameFieldStatus status); 

  /*! Set custom frame transform */
  PlusStatus SetCustomFrameTransform(PlusTransformName& frameTransformName, double transform[16]); 

  /*! Set custom frame transform */
  PlusStatus SetCustomFrameTransform(PlusTransformName& frameTransformName, vtkMatrix4x4* transform); 

  /*! Get the list of the name of all custom frame fields */
  void GetCustomFrameFieldNameList(std::vector<std::string> &fieldNames);

  /*! Get the list of the transform name of all custom frame transforms*/
  void GetCustomFrameTransformNameList(std::vector<PlusTransformName> &transformNames); 

  /*! Get tracked frame size in pixel. Returns: int[2].  */
  int* GetFrameSize(); 

  /*! Get tracked frame size in pixels */
  void GetFrameSize(int dim[2]);

  /*! Get tracked frame pixel size in bits */
  int GetNumberOfBitsPerPixel();

  /*! Set Segmented fiducial point pixel coordinates */
  void SetFiducialPointsCoordinatePx(vtkPoints* fiducialPoints); 

  /*! Get Segmented fiducial point pixel coordinates */
  vtkPoints* GetFiducialPointsCoordinatePx() { return this->FiducialPointsCoordinatePx; };

  /*! Write image with image to tracker transform to file */ 
  PlusStatus WriteToFile(std::string &filename, vtkMatrix4x4* mImageToTracker);

  /*! Convert from field status string to field status enum */
  static TrackedFrameFieldStatus ConvertFieldStatusFromString(const char* statusStr);

  /*! Convert from field status enum to field status string */
  static std::string ConvertFieldStatusToString(TrackedFrameFieldStatus status);

public:
  bool operator< (TrackedFrame data) { return Timestamp < data.Timestamp; }
  bool operator== (const TrackedFrame& data) const 
  {
    return (Timestamp == data.Timestamp);
  }

protected:
  PlusVideoFrame ImageData;
  double Timestamp; 

  FieldMapType CustomFrameFields;

  int FrameSize[2]; 

  /*! Stores segmented fiducial point pixel coordinates */
  vtkPoints* FiducialPointsCoordinatePx; 
};

//----------------------------------------------------------------------------

/*! 
 *  \enum Tracked frame validation requirements 
 *
 *  \brief If any of the requested requirement is not fulfilled then the validation fails. 
 *  \ingroup PlusLibImageAcquisition
 */
enum TrackedFrameValidationRequirements
{
  REQUIRE_UNIQUE_TIMESTAMP = 0x0001, /*!< the timestamp shall be unique */  
  REQUIRE_TRACKING_OK = 0x0002, /*!<  the tracking flags shall be valid (TOOL_OK) */  
  REQUIRE_CHANGED_ENCODER_POSITION = 0x0004, /*!<  the stepper encoder position shall be different from the previous ones  */  
  REQUIRE_SPEED_BELOW_THRESHOLD = 0x0008, /*!<  the frame acquisition speed shall be less than a threshold */  
  REQUIRE_CHANGED_TRANSFORM = 0x0010, /*!<  the transform defined by name shall be different from the previous ones  */  
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

//----------------------------------------------------------------------------

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
  TrackedFrameEncoderPositionFinder(TrackedFrame* frame, double minRequiredTranslationDifferenceMm, double minRequiredAngleDifferenceDeg); 
  ~TrackedFrameEncoderPositionFinder(); 

  // TODO: use BrachyTracker::GetStepperEncoderValues if the design allows 
  PlusStatus GetStepperEncoderValues( TrackedFrame* trackedFrame, double &probePosition, double &probeRotation, double &templatePosition ); 

  /*! 
    Predicate unary function for std::find_if to validate encoder position 
    @return Returning true if the encoder position difference is less than required 
  */
  bool operator()( TrackedFrame *newFrame ); 	

protected: 
  TrackedFrame* mTrackedFrame;
  double mMinRequiredTranslationDifferenceMm;
  double mMinRequiredAngleDifferenceDeg;
};

//----------------------------------------------------------------------------

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
  TrackedFrameTransformFinder(TrackedFrame* frame, PlusTransformName& frameTransformName, double minRequiredTranslationDifferenceMm, double minRequiredAngleDifferenceDeg); 
  ~TrackedFrameTransformFinder(); 

  /*! Predicate unary function for std::find_if to validate transform 
      @return Returning true if the transform difference is less than required 
  */
  bool operator()( TrackedFrame *newFrame ); 

protected: 
  TrackedFrame* mTrackedFrame;
  double mMinRequiredTranslationDifferenceMm;
  double mMinRequiredAngleDifferenceDeg;
  PlusTransformName mFrameTransformName;  
}; 

#endif 
