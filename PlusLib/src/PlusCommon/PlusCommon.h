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

enum TrackerStatus {
  TR_OK,			      // Tool OK
  TR_MISSING,       // tool or tool port is not available
  TR_OUT_OF_VIEW,   // cannot obtain transform for tool
  TR_OUT_OF_VOLUME, // tool is not within the sweet spot of system
  TR_SWITCH1_IS_ON, // various buttons/switches on tool
  TR_SWITCH2_IS_ON,
  TR_SWITCH3_IS_ON, 
  TR_REQ_TIMEOUT   // Request timeout
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

// This class is used for locking a objects (buffers, mutexes, etc.)
// and releasing the lock automatically when the object is deleted
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
};


/*!
  \class PlusTransformName 
  \brief Stores the from and to coordinate frame names for transforms 

  The PlusTransformName stores and generates the from and to coordinate frame names for transforms. 

  Example usage:
  
  PlusTransformName tnImageToProbe("Image", "Probe"); 
  ... or

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

private: 

  /*! Check if the input string is capitalized, if not capitalize it */ 
  void Capitalize(std::string& aString ); 

  std::string m_From; /*! From coordinate frame name */
  std::string m_To; /*! To coordinate frame name */
}; 


#endif //__PlusCommon_h
