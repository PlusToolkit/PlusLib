/*=Plus=header=begin======================================================
  Program: Plus
  Copyright (c) Laboratory for Percutaneous Surgery. All rights reserved.
  See License.txt for details.
=========================================================Plus=header=end*/

#ifndef __PlusCommon_h
#define __PlusCommon_h

#include "vtkOutputWindow.h"
#include "vtkPlusLogger.h"
#include "itkImageIOBase.h"

enum PlusStatus
{   
  PLUS_FAIL=0,
  PLUS_SUCCESS=1
};


/* Define case insensitive string compare for Windows. */
#if defined( _WIN32 ) && !defined(__CYGWIN__)
#  if defined(__BORLANDC__)
#    define STRCASECMP stricmp
#  else
#    define STRCASECMP _stricmp
#  endif
#else
#  define STRCASECMP STRCASECMP
#endif

#define ROUND(x) (static_cast<int>(floor( x + 0.5 )))

///////////////////////////////////////////////////////////////////
// Logging

#define LOG_ERROR(msg) \
	{ \
	std::ostrstream msgStream; \
  msgStream << " " << msg << std::ends; \
	vtkPlusLogger::Instance()->LogMessage(vtkPlusLogger::LOG_LEVEL_ERROR, msgStream.str(), __FILE__, __LINE__); \
	msgStream.rdbuf()->freeze(0); \
	}	

#define LOG_WARNING(msg) \
	{ \
	std::ostrstream msgStream; \
	msgStream << " " << msg << std::ends; \
  vtkPlusLogger::Instance()->LogMessage(vtkPlusLogger::LOG_LEVEL_WARNING, msgStream.str(), __FILE__, __LINE__); \
  msgStream.rdbuf()->freeze(0); \
	}
		
#define LOG_INFO(msg) \
	{ \
	std::ostrstream msgStream; \
	msgStream << " " << msg << std::ends; \
	vtkPlusLogger::Instance()->LogMessage(vtkPlusLogger::LOG_LEVEL_INFO, msgStream.str(), __FILE__, __LINE__); \
	msgStream.rdbuf()->freeze(0); \
	}
	
#define LOG_DEBUG(msg) \
	{ \
	std::ostrstream msgStream; \
	msgStream << " " << msg << std::ends; \
	vtkPlusLogger::Instance()->LogMessage(vtkPlusLogger::LOG_LEVEL_DEBUG, msgStream.str(), __FILE__, __LINE__); \
	msgStream.rdbuf()->freeze(0); \
	}	
	
#define LOG_TRACE(msg) \
	{ \
	std::ostrstream msgStream; \
	msgStream << " " << msg << std::ends; \
	vtkPlusLogger::Instance()->LogMessage(vtkPlusLogger::LOG_LEVEL_TRACE, msgStream.str(), __FILE__, __LINE__); \
	msgStream.rdbuf()->freeze(0); \
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
  PlusLockGuard<vtkCriticalSection> updateMutexGuardedLock(this->UpdateMutex);
  \endcode

  \ingroup PlusLibCommon
*/
template <typename T> class PlusLockGuard
{
public:
  PlusLockGuard(T* lockableObject)
  {
    m_LockableObject=lockableObject;
    m_LockableObject->Lock();
  }
  ~PlusLockGuard()
  {    
    m_LockableObject->Unlock();
    m_LockableObject=NULL;
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

namespace PlusCommon
{
  typedef itk::ImageIOBase::IOComponentType ITKScalarPixelType;
  typedef int VTKScalarPixelType;

  //----------------------------------------------------------------------------
  /*! Quick and robust string to int conversion */
  template<class T>
  static PlusStatus StringToInt(const char* strPtr, T &result)
  {
    if (strPtr==NULL || strlen(strPtr) == 0 )
    {
      return PLUS_FAIL;
    }
    char * pEnd=NULL;
    result = static_cast<int>(strtol(strPtr, &pEnd, 10)); 
    if (pEnd != strPtr+strlen(strPtr) ) 
    {
      return PLUS_FAIL;
    }
    return PLUS_SUCCESS;
  }

  //----------------------------------------------------------------------------
  /*! Quick and robust string to double conversion */
  template<class T>
  static PlusStatus StringToDouble(const char* strPtr, T &result)
  {
    if (strPtr==NULL || strlen(strPtr) == 0 )
    {
      return PLUS_FAIL;
    }
    char * pEnd=NULL;
    result = strtod(strPtr, &pEnd);
    if (pEnd != strPtr+strlen(strPtr)) 
    {
      return PLUS_FAIL;
    }
    return PLUS_SUCCESS;
  }

  //----------------------------------------------------------------------------
  /*! Quick and robust string to int conversion */
  template<class T>
  static PlusStatus StringToLong(const char* strPtr, T &result)
  {
    if (strPtr==NULL || strlen(strPtr) == 0 )
    {
      return PLUS_FAIL;
    }
    char * pEnd=NULL;
    result = strtol(strPtr, &pEnd, 10);
    if (pEnd != strPtr+strlen(strPtr) ) 
    {
      return PLUS_FAIL;
    }
    return PLUS_SUCCESS;
  }

  //----------------------------------------------------------------------------
  /*! Trim whitespace characters from the left and right */
  static void Trim(std::string &str)
  {
    str.erase(str.find_last_not_of(" \t\r\n")+1);
    str.erase(0,str.find_first_not_of(" \t\r\n"));
  }
};

/*!
  \class PlusTransformName 
  \brief Stores the from and to coordinate frame names for transforms 

  The PlusTransformName stores and generates the from and to coordinate frame names for transforms. 

  Example usage:
  \code
  PlusTransformName tnImageToProbe("Image", "Probe"); 
  \endcode
  or
  \code
  PlusTransformName tnImageToProbe; 
  if ( tnImageToProbe->SetTransformName("ImageToProbe") != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to set transform name!"); 
  }
  std::string fromFrame = tnImageToProbe->GetFrom(); 
  std::string toFrame = tnImageToProbe->GetFrom(); 
  std::string strImageToProbe; 
  if ( tnImageToProbe->GetTransformName(strImageToProbe) != PLUS_SUCCESS )
  {
    LOG_ERROR("Failed to get transform name!"); 
  }
  \endcode

  \ingroup PlusLibCommon
*/
class VTK_EXPORT PlusTransformName
{
public:
  PlusTransformName(); 
  ~PlusTransformName(); 
  PlusTransformName(std::string aFrom, std::string aTo ); 

  /*! 
    Set 'From' and 'To' coordinate frame names from a combined transform name with the following format [FrameFrom]To[FrameTo]. 
    The combined transform name might contain only one 'To' phrase followed by a capital letter (e.g. ImageToToProbe is not allowed) 
    and the coordiante frame names should be in camel case format starting with capitalized letters. 
  */
  PlusStatus SetTransformName(const char* aTransformName); 

  /*! Return combined transform name between 'From' and 'To' coordinate frames: [From]To[To] */
  PlusStatus GetTransformName(std::string& aTransformName); 

  /*! Return 'From' coordinate frame name, give a warning if it's not capitalized and capitalize it*/ 
  std::string From(); 

  /*! Return 'To' coordinate frame name, give a warning if it's not capitalized and capitalize it */ 
  std::string To(); 

  /*! Check if the current transform name is valid */ 
  bool IsValid(); 

  bool operator== (const PlusTransformName& in) const
  {
    return (in.m_From == m_From && in.m_To == m_To ); 
  }

private: 

  /*! Check if the input string is capitalized, if not capitalize it */ 
  void Capitalize(std::string& aString ); 

  std::string m_From; /*! From coordinate frame name */
  std::string m_To; /*! To coordinate frame name */
}; 


#endif //__PlusCommon_h
