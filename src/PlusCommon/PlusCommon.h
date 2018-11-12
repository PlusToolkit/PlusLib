/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __PlusCommon_h
#define __PlusCommon_h

// PLUS includes
#include "vtkPlusCommonExport.h"
#include "itkImageIOBase.h"
#include "vtkPlusLogger.h"
#include "vtkPlusMacro.h"
#include "vtkPlusRecursiveCriticalSection.h"

// VTK includes
#include <vtkImageData.h>
#include <vtksys/SystemTools.hxx>

// System includes
#include <float.h>

// STL includes
#include <array>
#include <list>
#include <locale>
#include <sstream>

class vtkPlusUsScanConvert;
class vtkPlusTrackedFrameList;

enum PlusStatus
{
  PLUS_FAIL = 0,
  PLUS_SUCCESS = 1
};

enum PlusImagingMode
{
  Plus_UnknownMode,
  Plus_BMode,
  Plus_RfMode
};

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

enum ToolStatus
{
  TOOL_UNKNOWN,         /*!< Value unknown */
  TOOL_OK,              /*!< Tool OK */
  TOOL_MISSING,         /*!< Tool or tool port is not available */
  TOOL_OUT_OF_VIEW,     /*!< Cannot obtain transform for tool */
  TOOL_OUT_OF_VOLUME,   /*!< Tool is not within the sweet spot of system */
  TOOL_SWITCH1_IS_ON,   /*!< Various buttons/switches on tool */
  TOOL_SWITCH2_IS_ON,   /*!< Various buttons/switches on tool */
  TOOL_SWITCH3_IS_ON,   /*!< Various buttons/switches on tool */
  TOOL_REQ_TIMEOUT,     /*!< Request timeout status */
  TOOL_INVALID,         /*!< Invalid tool status */
  TOOL_PATH_NOT_FOUND   /*!< Transform cannot be computed from existing transforms */
};

typedef std::array<unsigned int, 3> FrameSizeType;

#define UNDEFINED_TIMESTAMP DBL_MAX

/* Define case insensitive string compare for Windows. */
#if defined( _WIN32 ) && !defined(__CYGWIN__)
  #if defined(__BORLANDC__)
    #define STRCASECMP stricmp
  #else
    #define STRCASECMP _stricmp
  #endif
#else
  #define STRCASECMP strcasecmp
#endif

///////////////////////////////////////////////////////////////////
// Logging

class vtkPlusCommonExport vtkPlusLogHelper
{
public:
  double m_MinimumTimeBetweenLoggingSec;
  unsigned long m_MinimumCountBetweenLogging;
  vtkPlusLogger::LogLevelType m_LogLevel;

  // the parameters provide the maximum frequency of logging
  vtkPlusLogHelper(double minimumTimeBetweenLoggingSec = 60.0,
                   unsigned long minimumCountBetweenLogging = 5000,
                   vtkPlusLogger::LogLevelType logLevel = vtkPlusLogger::LOG_LEVEL_ERROR)
    : m_MinimumTimeBetweenLoggingSec(minimumTimeBetweenLoggingSec),
      m_MinimumCountBetweenLogging(minimumCountBetweenLogging),
      m_LogLevel(logLevel)
  {
    m_LastError = -std::numeric_limits<double>::max() / 2;
    m_Count = -2;
  }
  bool ShouldWeLog(bool errorPresent); //should the error be logged this time?
private:
  double m_LastError; //last time an error was logged
  unsigned long m_Count; //how many times the error was encountered

};

#define LOG_ERROR(msg) \
  { \
  std::ostringstream msgStream; \
  msgStream << msg << std::ends; \
  vtkPlusLogger::Instance()->LogMessage(vtkPlusLogger::LOG_LEVEL_ERROR, msgStream.str().c_str(), __FILE__, __LINE__); \
  }

#define LOG_WARNING(msg) \
  { \
  std::ostringstream msgStream; \
  msgStream << msg << std::ends; \
  vtkPlusLogger::Instance()->LogMessage(vtkPlusLogger::LOG_LEVEL_WARNING, msgStream.str().c_str(), __FILE__, __LINE__); \
  }

#define LOG_INFO(msg) \
  { \
  std::ostringstream msgStream; \
  msgStream << msg << std::ends; \
  vtkPlusLogger::Instance()->LogMessage(vtkPlusLogger::LOG_LEVEL_INFO, msgStream.str().c_str(), __FILE__, __LINE__); \
  }

#define LOG_DEBUG(msg) \
  { \
  std::ostringstream msgStream; \
  msgStream << msg << std::ends; \
  vtkPlusLogger::Instance()->LogMessage(vtkPlusLogger::LOG_LEVEL_DEBUG, msgStream.str().c_str(), __FILE__, __LINE__); \
  }

#define LOG_TRACE(msg) \
  { \
    if (vtkPlusLogger::Instance()->GetLogLevel()>=vtkPlusLogger::LOG_LEVEL_TRACE) \
    { \
      std::ostringstream msgStream; \
      msgStream << msg << std::ends; \
      vtkPlusLogger::Instance()->LogMessage(vtkPlusLogger::LOG_LEVEL_TRACE, msgStream.str().c_str(), __FILE__, __LINE__); \
    } \
  }

#define LOG_DYNAMIC(msg, logLevel) \
{ \
  std::ostringstream msgStream; \
  msgStream << msg << std::ends; \
  vtkPlusLogger::Instance()->LogMessage(logLevel, msgStream.str().c_str(), __FILE__, __LINE__); \
  }

// If condition is satisfied, logs error periodically
// and returns PLUS_FAIL each time
#define RETURN_WITH_FAIL_IF(condition, msg) \
  { \
  static vtkPlusLogHelper logHelper; \
  bool result = condition; \
  if (logHelper.ShouldWeLog(result)) \
  { \
    LOG_ERROR(msg); \
  } \
  else \
  { \
    LOG_TRACE(msg); \
  } \
  \
  if (result) \
  { \
    return PLUS_FAIL; \
  } \
  }

// If condition is satisfied, logs error periodically
// and returns PLUS_FAIL each time. Uses a vtkPlusLogHelper logHelper,
// which needs to be available in current scope.
#define CUSTOM_RETURN_WITH_FAIL_IF(condition, msg) \
  { \
  bool result = condition; \
  if (logHelper.ShouldWeLog(result)) \
  { \
    LOG_ERROR(msg); \
  } \
  else \
  { \
    LOG_TRACE(msg); \
  } \
  \
  if (result) \
  { \
    return PLUS_FAIL; \
  } \
  }

#define LOG_ERROR_W(msg) \
  { \
  std::wostringstream msgStream; \
  msgStream << msg << std::ends; \
  vtkPlusLogger::Instance()->LogMessage(vtkPlusLogger::LOG_LEVEL_ERROR, msgStream.str(), __FILE__, __LINE__); \
  }

#define LOG_WARNING_W(msg) \
  { \
  std::wostringstream msgStream; \
  msgStream << msg << std::ends; \
  vtkPlusLogger::Instance()->LogMessage(vtkPlusLogger::LOG_LEVEL_WARNING, msgStream.str(), __FILE__, __LINE__); \
  }

#define LOG_INFO_W(msg) \
  { \
  std::wostringstream msgStream; \
  msgStream << msg << std::ends; \
  vtkPlusLogger::Instance()->LogMessage(vtkPlusLogger::LOG_LEVEL_INFO, msgStream.str(), __FILE__, __LINE__); \
  }

#define LOG_DEBUG_W(msg) \
  { \
  std::wostringstream msgStream; \
  msgStream << msg << std::ends; \
  vtkPlusLogger::Instance()->LogMessage(vtkPlusLogger::LOG_LEVEL_DEBUG, msgStream.str(), __FILE__, __LINE__); \
  }

#define LOG_TRACE_W(msg) \
  { \
    if (vtkPlusLogger::Instance()->GetLogLevel()>=vtkPlusLogger::LOG_LEVEL_TRACE) \
    { \
      std::wostringstream msgStream; \
      msgStream << msg << std::ends; \
      vtkPlusLogger::Instance()->LogMessage(vtkPlusLogger::LOG_LEVEL_TRACE, msgStream.str(), __FILE__, __LINE__); \
    } \
  }

#define LOG_DYNAMIC_W(msg, logLevel) \
{ \
  std::wostringstream msgStream; \
  msgStream << msg << std::ends; \
  vtkPlusLogger::Instance()->LogMessage(logLevel, msgStream.str(), __FILE__, __LINE__); \
  }

// If condition is satisfied, logs error periodically
// and returns PLUS_FAIL each time
#define RETURN_WITH_FAIL_IF_W(condition, msg) \
  { \
  static vtkPlusLogHelper logHelper; \
  bool result = condition; \
  if (logHelper.ShouldWeLog(result)) \
  { \
    LOG_ERROR_W(msg); \
  } \
  else \
  { \
    LOG_TRACE_W(msg); \
  } \
  \
  if (result) \
  { \
    return PLUS_FAIL; \
  } \
  }

// If condition is satisfied, logs error periodically
// and returns PLUS_FAIL each time. Uses a vtkPlusLogHelper logHelper,
// which needs to be available in current scope.
#define CUSTOM_RETURN_WITH_FAIL_IF_W(condition, msg) \
  { \
  bool result = condition; \
  if (logHelper.ShouldWeLog(result)) \
  { \
    LOG_ERROR_W(msg); \
  } \
  else \
  { \
    LOG_TRACE_W(msg); \
  } \
  \
  if (result) \
  { \
    return PLUS_FAIL; \
  } \
  }

///////////////////////////////////////////////////////////////////

/*!
  \class PlusLockGuard
  \brief A class for automatically unlocking objects

  This class is used for locking a objects (buffers, mutexes, etc.)
  and releasing the lock automatically when the guard object is deleted
  (typically by getting out of scope).

  Example:
  \code
  PlusLockGuard<vtkPlusRecursiveCriticalSection> updateMutexGuardedLock(this->UpdateMutex);
  \endcode

  \ingroup PlusLibCommon
*/
template <typename T>
class PlusLockGuard
{
public:
  PlusLockGuard(T* lockableObject)
  {
    m_LockableObject = lockableObject;
    m_LockableObject->Lock();
  }
  ~PlusLockGuard()
  {
    m_LockableObject->Unlock();
    m_LockableObject = NULL;
  }
private:
  PlusLockGuard(PlusLockGuard&);
  void operator=(PlusLockGuard&);

  T* m_LockableObject;
};

/*!
  \def DELETE_IF_NOT_NULL(Object)
  \brief A macro to safely delete a VTK object (usable if the VTK object pointer is already NULL).
  \ingroup PlusLibCommon
*/
#define DELETE_IF_NOT_NULL( Object ) {\
  if ( Object != NULL ) {\
    Object->Delete();\
    Object = NULL;\
  }\
}

//
// Get character string.  Creates member Get"name"()
// (e.g., char *GetFilename());
//
#define vtkGetStdStringMacro(name) \
virtual std::string Get##name () const { \
  return this->name; \
}

#define GetStdStringMacro(name) \
virtual std::string Get##name () const { \
  return this->name; \
}

//
// Set character string.  Creates member Set"name"()
// (e.g., SetFilename(char *));
//
#define vtkSetStdStringMacro(name) \
virtual void Set##name (const std::string& _arg) \
{ \
  if ( this->name.compare(_arg) == 0 ) { return;} \
  this->name = _arg; \
  this->Modified(); \
} \
virtual void Set##name (const char* _arg) \
{ \
  this->Set##name(std::string(_arg ? _arg : "")); \
}

#define SetStdStringMacro(name) \
virtual void Set##name (const std::string& _arg) \
{ \
  if ( this->name.compare(_arg) == 0 ) { return;} \
  this->name = _arg; \
} \
virtual void Set##name (const char* _arg) \
{ \
  this->Set##name(std::string(_arg ? _arg : "")); \
}

#define vtkGetMacroConst(name,type) \
virtual type Get##name () const { \
  return this->name; \
}

#define SetMacro(name,type) \
virtual void Set##name (type _arg) \
{ \
  if (this->name != _arg) \
  { \
    this->name = _arg; \
  } \
}

#define GetMacro(name,type) \
virtual void Set##name (type _arg) \
{ \
  if (this->name != _arg) \
  { \
    this->name = _arg; \
  } \
}

class vtkPlusTrackedFrameList;
class vtkXMLDataElement;

namespace PlusCommon
{
  typedef itk::ImageIOBase::IOComponentType ITKScalarPixelType;
  typedef int VTKScalarPixelType;
  typedef int IGTLScalarPixelType;

  //----------------------------------------------------------------------------
  /*! Quick and robust string to int conversion */
  template<class T>
  PlusStatus StringToInt(const char* strPtr, T& result)
  {
    if (strPtr == NULL || strlen(strPtr) == 0)
    {
      return PLUS_FAIL;
    }
    char* pEnd = NULL;
    result = static_cast<int>(strtol(strPtr, &pEnd, 10));
    if (pEnd != strPtr + strlen(strPtr))
    {
      return PLUS_FAIL;
    }
    return PLUS_SUCCESS;
  }

  //----------------------------------------------------------------------------
  /*! Quick and robust string to int conversion */
  template<class T>
  PlusStatus StringToUInt(const char* strPtr, T& result)
  {
    if (strPtr == NULL || strlen(strPtr) == 0)
    {
      return PLUS_FAIL;
    }
    char* pEnd = NULL;
    result = static_cast<unsigned int>(strtol(strPtr, &pEnd, 10));
    if (pEnd != strPtr + strlen(strPtr))
    {
      return PLUS_FAIL;
    }
    return PLUS_SUCCESS;
  }

  //----------------------------------------------------------------------------
  /*! Quick and robust string to double conversion */
  template<class T>
  PlusStatus StringToDouble(const char* strPtr, T& result)
  {
    if (strPtr == NULL || strlen(strPtr) == 0)
    {
      return PLUS_FAIL;
    }
    char* pEnd = NULL;
    result = strtod(strPtr, &pEnd);
    if (pEnd != strPtr + strlen(strPtr))
    {
      return PLUS_FAIL;
    }
    return PLUS_SUCCESS;
  }
  //---------------------------------------------------------------------------
  struct ImageMetaDataItem
  {
    std::string Id;  /* device name to query the IMAGE and COLORT */
    std::string Description;        /* name / description (< 64 bytes)*/
    std::string Modality;    /* modality name (< 32 bytes) */
    std::string PatientName; /* patient name (< 64 bytes) */
    std::string PatientId;   /* patient ID (MRN etc.) (< 64 bytes) */
    double TimeStampUtc;   /* scan time in UTC*/
    unsigned int Size[3];     /* entire image volume size */
    unsigned char ScalarType;  /* scalar type. see scalar_type in IMAGE message */
  };

  typedef std::list<ImageMetaDataItem> ImageMetaDataList;
  //----------------------------------------------------------------------------
  /*! Quick and robust string to int conversion */
  template<class T>
  PlusStatus StringToLong(const char* strPtr, T& result)
  {
    if (strPtr == NULL || strlen(strPtr) == 0)
    {
      return PLUS_FAIL;
    }
    char* pEnd = NULL;
    result = strtol(strPtr, &pEnd, 10);
    if (pEnd != strPtr + strlen(strPtr))
    {
      return PLUS_FAIL;
    }
    return PLUS_SUCCESS;
  }

  //----------------------------------------------------------------------------
  enum LINE_STYLE
  {
    LINE_STYLE_SOLID,
    LINE_STYLE_DOTS,
  };

  enum ALPHA_BEHAVIOR
  {
    ALPHA_BEHAVIOR_SOURCE,
    ALPHA_BEHAVIOR_OPAQUE
  };

  //----------------------------------------------------------------------------
  vtkPlusCommonExport PlusStatus DrawLine(vtkImageData& imageData,
                                          const std::array<float, 3>& colour,
                                          LINE_STYLE style,
                                          const std::array<int, 3>& startPixel,
                                          const std::array<int, 3>& endPixel,
                                          unsigned int numberOfPoints,
                                          ALPHA_BEHAVIOR alphaBehavior = ALPHA_BEHAVIOR_OPAQUE);

  //----------------------------------------------------------------------------
  vtkPlusCommonExport PlusStatus DrawLine(vtkImageData& imageData,
                                          float greyValue,
                                          LINE_STYLE style,
                                          const std::array<int, 3>& startPixel,
                                          const std::array<int, 3>& endPixel,
                                          unsigned int numberOfPoints,
                                          ALPHA_BEHAVIOR alphaBehavior = ALPHA_BEHAVIOR_OPAQUE);

  //----------------------------------------------------------------------------
  bool vtkPlusCommonExport IsEqualInsensitive(std::string const& a, std::string const& b);
  bool vtkPlusCommonExport IsEqualInsensitive(std::wstring const& a, std::wstring const& b);
  bool vtkPlusCommonExport HasSubstrInsensitive(std::string const& a, std::string const& b);
  bool vtkPlusCommonExport HasSubstrInsensitive(std::wstring const& a, std::wstring const& b);

  //----------------------------------------------------------------------------
  typedef std::array<int, 3> PixelPoint;
  typedef std::pair<PixelPoint, PixelPoint> PixelLine;
  typedef std::vector<PixelLine> PixelLineList;
  vtkPlusCommonExport PlusStatus DrawScanLines(int* inputImageExtent, float greyValue, const PixelLineList& scanLineEndPoints, vtkPlusTrackedFrameList* trackedFrameList);
  vtkPlusCommonExport PlusStatus DrawScanLines(int* inputImageExtent, const std::array<float, 3>& colour, const PixelLineList& scanLineEndPoints, vtkPlusTrackedFrameList* trackedFrameList);
  vtkPlusCommonExport PlusStatus DrawScanLines(int* inputImageExtent, float greyValue, const PixelLineList& scanLineEndPoints, vtkImageData* imageData);
  vtkPlusCommonExport PlusStatus DrawScanLines(int* inputImageExtent, const std::array<float, 3>& colour, const PixelLineList& scanLineEndPoints, vtkImageData* imageData);

  //----------------------------------------------------------------------------
  template<typename T>
  static std::string ToString(T number)
  {
#if defined(_MSC_VER) && _MSC_VER < 1700
    // This method can be used for number to string conversion
    // until std::to_string is supported by more compilers.
    std::ostringstream ss;
    ss << number;
    return ss.str();
#else
    return std::to_string(number);
#endif
  }

  static const int NO_CLIP = -1;
  vtkPlusCommonExport bool IsClippingRequested(const std::array<int, 3>& clipOrigin, const std::array<int, 3>& clipSize);
  vtkPlusCommonExport bool IsClippingWithinExtents(const std::array<int, 3>& clipOrigin, const std::array<int, 3>& clipSize, const int extents[6]);

  vtkPlusCommonExport void SplitStringIntoTokens(const std::string& s, char delim, std::vector<std::string>& elems, bool keepEmptyParts = true);
  vtkPlusCommonExport std::vector<std::string> SplitStringIntoTokens(const std::string& s, char delim, bool keepEmptyParts = true);
  template<typename ElemType>
  vtkPlusCommonExport void JoinTokensIntoString(const std::vector<ElemType>& elems, std::string& output, char separator = ' ');

  vtkPlusCommonExport PlusStatus CreateTemporaryFilename(std::string& aString, const std::string& anOutputDirectory);

  vtkPlusCommonExport std::vector<std::string> GetSequenceExtensions();
  vtkPlusCommonExport std::string GetSequenceFilenameWithoutExtension(std::string name);
  vtkPlusCommonExport std::string GetSequenceFilenameExtension(std::string name);

  /*! Trim whitespace characters from the left and right */
  vtkPlusCommonExport std::string& Trim(std::string& str);

  /*!
    On some systems fwrite may fail if a large chunk of data is attempted to written in one piece.
    This method writes the data in smaller chunks as long as all data is written or no data
    can be written anymore.
  */
  vtkPlusCommonExport PlusStatus RobustFwrite(FILE* fileHandle, void* data, size_t dataSize, size_t& writtenSize);

  vtkPlusCommonExport std::string GetPlusLibVersionString();

  //----------------------------------------------------------------------------
  namespace XML
  {
    /*!
      Writes an XML element to file. The output is nicer that with the built-in vtkXMLDataElement::PrintXML, as
      there are no extra lines, if there are many attributes then each of them is printed on separate line, and
      matrix elements (those that contain Matrix or Transform in the attribute name and 16 numerical elements in the attribute value)
      are printed in 4 lines.
    */
    vtkPlusCommonExport PlusStatus PrintXML(const std::string& filename, vtkXMLDataElement* elem);
    /*!
      Writes an XML element to a stream. The output is nicer that with the built-in vtkXMLDataElement::PrintXML, as
      there are no extra lines, if there are many attributes then each of them is printed on separate line, and
      matrix elements (those that contain Matrix or Transform in the attribute name and 16 numerical elements in the attribute value)
      are printed in 4 lines.
    */
    vtkPlusCommonExport PlusStatus PrintXML(ostream& os, vtkIndent indent, vtkXMLDataElement* elem);

    vtkPlusCommonExport PlusStatus SafeCheckAttributeValueInsensitive(vtkXMLDataElement& element, const std::string& attributeName, const std::string& value, bool& isEqual);
    vtkPlusCommonExport PlusStatus SafeCheckAttributeValueInsensitive(vtkXMLDataElement& element, const std::wstring& attributeName, const std::wstring& value, bool& isEqual);

    vtkPlusCommonExport PlusStatus SafeGetAttributeValueInsensitive(vtkXMLDataElement& element, const std::string& attributeName, std::string& value);
    vtkPlusCommonExport PlusStatus SafeGetAttributeValueInsensitive(vtkXMLDataElement& element, const std::wstring& attributeName, std::string& value);
    template<typename T> vtkPlusCommonExport PlusStatus SafeGetAttributeValueInsensitive(vtkXMLDataElement& element, const std::string& attributeName, T& value);
    template<typename T> vtkPlusCommonExport PlusStatus SafeGetAttributeValueInsensitive(vtkXMLDataElement& element, const std::wstring& attributeName, T& value);
  }

  //--------------- Tool Details ----------------------
  vtkPlusCommonExport ToolStatus ConvertStringToToolStatus(const std::string& status);
  vtkPlusCommonExport std::string ConvertToolStatusToString(const ToolStatus& status);
  vtkPlusCommonExport TrackedFrameFieldStatus ConvertToolStatusToTrackedFrameFieldStatus(const ToolStatus& status);
  vtkPlusCommonExport ToolStatus ConvertTrackedFrameFieldStatusToToolStatus(TrackedFrameFieldStatus fieldStatus);
};

/*!
  \class PlusTransformName
  \brief Stores the from and to coordinate frame names for transforms

  The PlusTransformName stores and generates the from and to coordinate frame names for transforms.
  To enable robust serialization to/from a simple string (...To...Transform), the coordinate frame names must
  start with an uppercase character and it shall not contain "To" followed by an uppercase character. E.g., valid
  coordinate frame names are Tracker, TrackerBase, Tool; invalid names are tracker, trackerBase, ToImage.

  Example usage:
  Setting a transform name:
  \code
  PlusTransformName tnImageToProbe("Image", "Probe");
  \endcode
  or
  \code
  PlusTransformName tnImageToProbe;
  if ( tnImageToProbe->SetTransformName("ImageToProbe") != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to set transform name!");
    return PLUS_FAIL;
  }
  \endcode
  Getting coordinate frame or transform names:
  \code
  std::string fromFrame = tnImageToProbe->From();
  std::string toFrame = tnImageToProbe->To();
  \endcode
  or
  \code
  std::string strImageToProbe;
  if ( tnImageToProbe->GetTransformName(strImageToProbe) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to get transform name!");
    return PLUS_FAIL;
  }
  \endcode

  \ingroup PlusLibCommon
*/
class vtkPlusCommonExport PlusTransformName
{
public:
  PlusTransformName();
  ~PlusTransformName();
  PlusTransformName(std::string aFrom, std::string aTo);
  PlusTransformName(const std::string& transformName);

  /*!
    Set 'From' and 'To' coordinate frame names from a combined transform name with the following format [FrameFrom]To[FrameTo].
    The combined transform name might contain only one 'To' phrase followed by a capital letter (e.g. ImageToToProbe is not allowed)
    and the coordinate frame names should be in camel case format starting with capitalized letters.
  */
  PlusStatus SetTransformName(const char* aTransformName);
  PlusStatus SetTransformName(const std::string& aTransformName);

  /*! Return combined transform name between 'From' and 'To' coordinate frames: [From]To[To] */
  PlusStatus GetTransformName(std::string& aTransformName) const;
  std::string GetTransformName() const;

  /*! Return 'From' coordinate frame name, give a warning if it's not capitalized and capitalize it*/
  std::string From() const;

  /*! Return 'To' coordinate frame name, give a warning if it's not capitalized and capitalize it */
  std::string To() const;

  /*! Clear the 'From' and 'To' fields */
  void Clear();

  /*! Check if the current transform name is valid */
  bool IsValid() const;

  inline bool operator== (const PlusTransformName& in) const
  {
    return (in.m_From == m_From && in.m_To == m_To);
  }

  inline bool operator!= (const PlusTransformName& in) const
  {
    return !(in == *this);
  }

  friend std::ostream& operator<< (std::ostream& os, const PlusTransformName& transformName)
  {
    os << transformName.GetTransformName();
    return os;
  }

private:
  /*! Check if the input string is capitalized, if not capitalize it */
  void Capitalize(std::string& aString);
  std::string m_From; /*! From coordinate frame name */
  std::string m_To; /*! To coordinate frame name */
};


#define RETRY_UNTIL_TRUE(command_, numberOfRetryAttempts_, delayBetweenRetryAttemptsSec_) \
  { \
    bool success = false; \
    int numOfTries = 0; \
    while ( !success && numOfTries < numberOfRetryAttempts_ ) \
    { \
      success = (command_);   \
      if (success)  \
      { \
        /* command successfully completed, continue without waiting */ \
        break; \
      } \
      /* command failed, wait for some time and retry */ \
      numOfTries++;   \
      vtkPlusAccurateTimer::Delay(delayBetweenRetryAttemptsSec_); \
    } \
  }

#include "vtkPlusConfig.h"
#include "PlusXmlUtils.h"
#include "PlusCommon.txx"

#endif //__PlusCommon_h
