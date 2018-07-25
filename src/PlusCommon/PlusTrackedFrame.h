/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __TRACKEDFRAME_H
#define __TRACKEDFRAME_H

#include "vtkPlusCommonExport.h"

#include "PlusVideoFrame.h"

class vtkMatrix4x4;
class vtkPoints;

/*!
  \enum TrackedFrameFieldStatus
  \brief Tracked frame field status
  Image field is valid if the image data is not NULL.
  Tool status is valid only if the ToolStatus is TOOL_OK.
  \ingroup PlusLibCommon
*/
enum TrackedFrameFieldStatus
{
  FIELD_OK,           /*!< Field is valid */
  FIELD_INVALID       /*!< Field is invalid */
};

/*!
  \class TrackedFrame
  \brief Stores tracked frame (image + pose information + field data)
  \ingroup PlusLibCommon
*/
class vtkPlusCommonExport PlusTrackedFrame
{
public:
  static const char* FIELD_FRIENDLY_DEVICE_NAME;

public:
  static const std::string TransformPostfix;
  static const std::string TransformStatusPostfix;
  typedef std::map<std::string, std::string> FieldMapType;

public:
  PlusTrackedFrame();
  ~PlusTrackedFrame();
  PlusTrackedFrame(const PlusTrackedFrame& frame);
  PlusTrackedFrame& operator=(PlusTrackedFrame const& trackedFrame);

public:
  /*! Set image data */
  void SetImageData(const PlusVideoFrame& value);

  /*! Get image data */
  PlusVideoFrame* GetImageData() { return &(this->ImageData); };

  /*! Set timestamp */
  void SetTimestamp(double value);

  /*! Get timestamp */
  double GetTimestamp() { return this->Timestamp; };

  /*! Set frame field */
  void SetFrameField(std::string name, std::string value);

  /*! Get frame field value */
  const char* GetFrameField(const char* fieldName);
  const char* GetFrameField(const std::string& fieldName);

  /*! Delete frame field */
  PlusStatus DeleteFrameField(const char* fieldName);

  /*!
    Check if a frame field is defined or not
    \return true, if the field is defined; false, if the field is not defined
  */
  bool IsFrameFieldDefined(const char* fieldName);

  /*!
    Check if a frame transform name field is defined or not
    \return true, if the field is defined; false, if the field is not defined
  */
  bool IsFrameTransformNameDefined(const PlusTransformName& transformName);

  /*! Get frame transform */
  PlusStatus GetFrameTransform(const PlusTransformName& frameTransformName, double transform[16]);
  /*! Get frame transform */
  PlusStatus GetFrameTransform(const PlusTransformName& frameTransformName, vtkMatrix4x4* transformMatrix);

  /*! Get frame status */
  PlusStatus GetFrameTransformStatus(const PlusTransformName& frameTransformName, TrackedFrameFieldStatus& status);
  /*! Set frame status */
  PlusStatus SetFrameTransformStatus(const PlusTransformName& frameTransformName, TrackedFrameFieldStatus status);

  /*! Set frame transform */
  PlusStatus SetFrameTransform(const PlusTransformName& frameTransformName, double transform[16]);

  /*! Set frame transform */
  PlusStatus SetFrameTransform(const PlusTransformName& frameTransformName, vtkMatrix4x4* transform);

  /*! Get the list of the name of all frame fields */
  void GetFrameFieldNameList(std::vector<std::string>& fieldNames);

  /*! Get the list of the transform name of all frame transforms*/
  void GetFrameTransformNameList(std::vector<PlusTransformName>& transformNames);

  /*! Get tracked frame size in pixel. Returns: FrameSizeType.  */
  FrameSizeType GetFrameSize();

  /*! Get tracked frame pixel size in bits (scalar size * number of scalar components) */
  int GetNumberOfBitsPerScalar();

  /*! Get number of scalar components in a pixel */
  PlusStatus GetNumberOfScalarComponents(unsigned int& numberOfScalarComponents);

  /*! Get number of bits in a pixel */
  int GetNumberOfBitsPerPixel();

  /*! Set Segmented fiducial point pixel coordinates */
  void SetFiducialPointsCoordinatePx(vtkPoints* fiducialPoints);

  /*! Get Segmented fiducial point pixel coordinates */
  vtkPoints* GetFiducialPointsCoordinatePx() { return this->FiducialPointsCoordinatePx; };

  /*! Write image with image to tracker transform to file */
  PlusStatus WriteToFile(const std::string& filename, vtkMatrix4x4* imageToTracker);

  /*! Print tracked frame human readable serialization data to XML data
      If requestedTransforms is empty, all stored FrameFields are sent
  */
  PlusStatus PrintToXML(vtkXMLDataElement* xmlData, const std::vector<PlusTransformName>& requestedTransforms);

  /*! Serialize Tracked frame human readable data to xml data and return in string */
  PlusStatus GetTrackedFrameInXmlData(std::string& strXmlData, const std::vector<PlusTransformName>& requestedTransforms);

  /*! Deserialize TrackedFrame human readable data from xml data string */
  PlusStatus SetTrackedFrameFromXmlData(const char* strXmlData);
  /*! Deserialize TrackedFrame human readable data from xml data string */
  PlusStatus SetTrackedFrameFromXmlData(const std::string& xmlData);

  /*! Convert from field status string to field status enum */
  static TrackedFrameFieldStatus ConvertFieldStatusFromString(const char* statusStr);

  /*! Convert from field status enum to field status string */
  static std::string ConvertFieldStatusToString(TrackedFrameFieldStatus status);

  /*! Return all custom fields in a map */
  const FieldMapType& GetCustomFields() { return this->FrameFields; }

  /*! Returns true if the input string ends with "Transform", else false */
  static bool IsTransform(std::string str);

  /*! Returns true if the input string ends with "TransformStatus", else false */
  static bool IsTransformStatus(std::string str);

public:
  bool operator< (PlusTrackedFrame data) { return Timestamp < data.Timestamp; }
  bool operator== (const PlusTrackedFrame& data) const
  {
    return (Timestamp == data.Timestamp);
  }

protected:
  PlusVideoFrame ImageData;
  double Timestamp;

  FieldMapType FrameFields;

  FrameSizeType FrameSize;

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
  TrackedFrameTimestampFinder(PlusTrackedFrame* frame): mTrackedFrame(frame) {};
  bool operator()(PlusTrackedFrame* newFrame)
  {
    return newFrame->GetTimestamp() == mTrackedFrame->GetTimestamp();
  }
  PlusTrackedFrame* mTrackedFrame;
};

//----------------------------------------------------------------------------

/*!
  \class TrackedFrameEncoderPositionFinder
  \brief Helper class used for validating encoder position in a tracked frame list
  \ingroup PlusLibCommon
*/
class vtkPlusCommonExport PlusTrackedFrameEncoderPositionFinder
{
public:
  PlusTrackedFrameEncoderPositionFinder(PlusTrackedFrame* frame, double minRequiredTranslationDifferenceMm, double minRequiredAngleDifferenceDeg);
  ~PlusTrackedFrameEncoderPositionFinder();

  /*! Get stepper encoder values from the tracked frame */
  static PlusStatus GetStepperEncoderValues(PlusTrackedFrame* trackedFrame, double& probePosition, double& probeRotation, double& templatePosition);

  /*!
    Predicate unary function for std::find_if to validate encoder position
    \return Returning true if the encoder position difference is less than required
  */
  bool operator()(PlusTrackedFrame* newFrame);

protected:
  PlusTrackedFrame* mTrackedFrame;
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
  TrackedFrameTransformFinder(PlusTrackedFrame* frame, const PlusTransformName& frameTransformName, double minRequiredTranslationDifferenceMm, double minRequiredAngleDifferenceDeg);
  ~TrackedFrameTransformFinder();

  /*!
    Predicate unary function for std::find_if to validate transform
    \return Returning true if the transform difference is less than required
  */
  bool operator()(PlusTrackedFrame* newFrame);

protected:
  PlusTrackedFrame* mTrackedFrame;
  double mMinRequiredTranslationDifferenceMm;
  double mMinRequiredAngleDifferenceDeg;
  PlusTransformName mFrameTransformName;
};

#endif
