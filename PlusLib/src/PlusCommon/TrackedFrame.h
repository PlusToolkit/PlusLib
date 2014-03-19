/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/ 

#ifndef __TRACKEDFRAME_H
#define __TRACKEDFRAME_H

#include "PlusVideoFrame.h" 

class vtkMatrix4x4; 
class vtkPoints; 

/*!
  \class vtkGnuplotExecuter 
  Class for executing gnuplot for diagram generation
*/ 

/*! 
  \enum TrackedFrameFieldStatus
  \brief Tracked frame field status
  Image field is valid if the image data is not NULL.
  Tool status is valid only if the ToolStatus is TOOL_OK.
  \ingroup PlusLibCommon
*/
enum TrackedFrameFieldStatus 
{
  FIELD_OK,            /*!< Field is valid */
  FIELD_INVALID       /*!< Field is invalid */
};

/*!
  \class TrackedFrame 
  \brief Stores tracked frame (image + pose information)
  \ingroup PlusLibCommon
*/ 
class VTK_EXPORT TrackedFrame
{
public:
  static const std::string TransformPostfix;
  static const std::string TransformStatusPostfix;
  typedef std::map<std::string,std::string> FieldMapType; 

public:
  TrackedFrame();
  ~TrackedFrame(); 
  TrackedFrame(const TrackedFrame& frame); 
  TrackedFrame& operator=(TrackedFrame const&trackedFrame); 

public:
  /*! Set image data */
  void SetImageData(const PlusVideoFrame &value); 

  /*! Get image data */
  PlusVideoFrame* GetImageData() { return &(this->ImageData); };

  /*! Set timestamp */
  void SetTimestamp(double value); 

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
  bool IsCustomFrameTransformNameDefined(const PlusTransformName& transformName); 

  /*! Get custom frame transform */
  PlusStatus GetCustomFrameTransform(const PlusTransformName& frameTransformName, double transform[16]); 
  /*! Get custom frame transform */
  PlusStatus GetCustomFrameTransform(const PlusTransformName& frameTransformName, vtkMatrix4x4* transformMatrix); 
  
  /*! Get custom frame status */
  PlusStatus GetCustomFrameTransformStatus(const PlusTransformName& frameTransformName, TrackedFrameFieldStatus& status); 
  /*! Set custom frame status */
  PlusStatus SetCustomFrameTransformStatus(const PlusTransformName& frameTransformName, TrackedFrameFieldStatus status); 

  /*! Set custom frame transform */
  PlusStatus SetCustomFrameTransform(const PlusTransformName& frameTransformName, double transform[16]); 
    
  /*! Set custom frame transform */
  PlusStatus SetCustomFrameTransform(const PlusTransformName& frameTransformName, vtkMatrix4x4* transform); 

  /*! Get the list of the name of all custom frame fields */
  void GetCustomFrameFieldNameList(std::vector<std::string> &fieldNames);

  /*! Get the list of the transform name of all custom frame transforms*/
  void GetCustomFrameTransformNameList(std::vector<PlusTransformName> &transformNames); 

  /*! Get tracked frame size in pixel. Returns: int[2].  */
  int* GetFrameSize(); 

  /*! Get tracked frame size in pixels */
  void GetFrameSize(int dim[2]);

  /*! Get tracked frame pixel size in bits */
  int GetNumberOfBitsPerScalar();

  /*! Set Segmented fiducial point pixel coordinates */
  void SetFiducialPointsCoordinatePx(vtkPoints* fiducialPoints); 

  /*! Get Segmented fiducial point pixel coordinates */
  vtkPoints* GetFiducialPointsCoordinatePx() { return this->FiducialPointsCoordinatePx; };

  /*! Write image with image to tracker transform to file */ 
  PlusStatus WriteToFile(const std::string &filename, vtkMatrix4x4* imageToTracker);

  /*! Print tracked frame human readable serialization data to XML data */ 
  PlusStatus PrintToXML(vtkXMLDataElement* xmlData); 

  /*! Serialize Tracked frame human readable data to xml data and return in string */ 
  PlusStatus GetTrackedFrameInXmlData( std::string& strXmlData ); 

  /*! Deserialize TrackedFrame human readable data from xml data string */
  PlusStatus SetTrackedFrameFromXmlData( const char* strXmlData ); 
  /*! Deserialize TrackedFrame human readable data from xml data string */
  PlusStatus SetTrackedFrameFromXmlData( const std::string& xmlData );

  /*! Convert from field status string to field status enum */
  static TrackedFrameFieldStatus ConvertFieldStatusFromString(const char* statusStr);

  /*! Convert from field status enum to field status string */
  static std::string ConvertFieldStatusToString(TrackedFrameFieldStatus status);

  /*! Return all custom fields in a map */ 
  const FieldMapType& GetCustomFields() { return this->CustomFrameFields; }

  /*! Returns true if the input string ends with "Transform", else false */
  static bool IsTransform( std::string str );

  /*! Returns true if the input string ends with "TransformStatus", else false */
  static bool IsTransformStatus( std::string str );

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
  \enum TrackedFrameValidationRequirements
  \brief If any of the requested requirement is not fulfilled then the validation fails. 
  \ingroup PlusLibCommon
*/
enum TrackedFrameValidationRequirements
{
  REQUIRE_UNIQUE_TIMESTAMP = 0x0001, /*!< the timestamp shall be unique */  
  REQUIRE_TRACKING_OK = 0x0002, /*!<  the tracking flags shall be valid (TOOL_OK) */  
  REQUIRE_CHANGED_ENCODER_POSITION = 0x0004, /*!<  the stepper encoder position shall be different from the previous ones  */  
  REQUIRE_SPEED_BELOW_THRESHOLD = 0x0008, /*!<  the frame acquisition speed shall be less than a threshold */  
  REQUIRE_CHANGED_TRANSFORM = 0x0010, /*!<  the transform defined by name shall be different from the previous ones  */  
}; 

/*!
  \class TrackedFrameTimestampFinder 
  \brief Helper class used for validating timestamps in a tracked frame list
  \ingroup PlusLibCommon
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

/*!
  \class TrackedFrameEncoderPositionFinder 
  \brief Helper class used for validating encoder position in a tracked frame list
  \ingroup PlusLibCommon
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
    \return Returning true if the encoder position difference is less than required 
  */
  bool operator()( TrackedFrame *newFrame );   

protected: 
  TrackedFrame* mTrackedFrame;
  double mMinRequiredTranslationDifferenceMm;
  double mMinRequiredAngleDifferenceDeg;
};

//----------------------------------------------------------------------------

/*!
  \class TrackedFrameTransformFinder 
  \brief Helper class used for validating frame transform in a tracked frame list
  \ingroup PlusLibCommon
*/ 
class TrackedFrameTransformFinder
{  
public:
  TrackedFrameTransformFinder(TrackedFrame* frame, const PlusTransformName& frameTransformName, double minRequiredTranslationDifferenceMm, double minRequiredAngleDifferenceDeg); 
  ~TrackedFrameTransformFinder(); 

  /*! 
    Predicate unary function for std::find_if to validate transform 
    \return Returning true if the transform difference is less than required 
  */
  bool operator()( TrackedFrame *newFrame ); 

protected: 
  TrackedFrame* mTrackedFrame;
  double mMinRequiredTranslationDifferenceMm;
  double mMinRequiredAngleDifferenceDeg;
  PlusTransformName mFrameTransformName;  
}; 

#endif 
