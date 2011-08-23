// ===========================================================================
// Program Modification Record
// ===========================================================================
// Component:	Utility
// ===========================================================================
// Class:		AppServices
// ===========================================================================
// File Name:	AppServices.h
// ===========================================================================
// Author:		Thomas K. Chen <chent@cs.queensu.ca>
//				School of Computing, Queen's University at Kingston, Canada
// ===========================================================================
// Purposes: 
// 1. Expand the reusability of class by generalization.
// 2. Elimate class dependency on Qt framework
// 3. Elimate class dependency on IGO framework
// 4. Change the dependency to C++ Standard Library
// ===========================================================================
// Change History:
// Author			Time						Release	Changes
// Thomas K. Chen	Mon Jun 30 22:22 EDT 2003	1.0		Generalization
//
// ===========================================================================


#ifndef APPSERVICES_H
#include "AppServices.h"
#endif

//================================================================== 
// Author: Thomas K. Chen
// Modifications Made:
// - Trace class was modified to elimate the trace of Qt frame-
//   work, in particular, the QString and QObject classes. 
// - The include file was renamed to be different to the std::trace.
//==================================================================
// local includes
#include "TraceUtils.h"

//================================================================== 
// Author: Thomas K. Chen
// Modifications Made:
// - Replaced IGOException class with new Exception class
// - Exception class was implemented with and derived from standard
//   C++ library <exception>, therefore has maximum independence.
//==================================================================
// local includes
#include "Exception.h"

//================================================================== 
// Author: Thomas K. Chen
// Modifications Made:
// - Added includes of C++ Standard string class: std::string
//==================================================================
// C++ standard includes 
#include <string>
#include <memory>

//================================================================== 
// Author: Thomas K. Chen
// Modifications Made:
// - Added includes of Windows system header, windows.h, to elimate 
//   the trace of Qt framework, QDir class.
// - Replace QDir class with Windows function GetCurrentDirectory.
//==================================================================
// Windows system includes
#include <windows.h>

//================================================================== 
// Author: Thomas K. Chen
// Modifications Made:
// - Changed the Qstring from Qt framework to C++ std::string class.
//================================================================== 
// Error Messages
const std::string AppServices::kstrScope = "AppServices";
const std::string AppServices::kstrUnableToGetCurrentDir = "Unable to get current directory.";
const std::string AppServices::kstrUnableToGetMemBuffer = "Unable to set memory buffer using memset().";
const std::string AppServices::kstrUnableToOpenPropertyFile = "Unable to open the property file.";

//================================================================== 
// Author: Thomas K. Chen
// Modifications Made:
// - Changed the Qstring from Qt framework to C++ std::string class.
//==================================================================	    
//! The path to the application
std::string AppServices::m_strAppPath;

//! The application properties
Properties* AppServices::m_pProps = 0;
	
/*!
Initialize the application services. Read the properties, setup trace.

\param strAppName	The application name, the base of the properties filename
*/
//================================================================== 
// Author: Thomas K. Chen
// Modifications Made:
// - Changed the Qstring from Qt framework to C++ std::string class.
//==================================================================	    
void AppServices::initialize(const std::string& strAppName)
{
    //================================================================== 
    // Author: Thomas K. Chen
    // Modifications Made:
    // - Replace QDir class with Windows function GetCurrentDirectory.
    //==================================================================
	// get the path to the application
    char bufferDirectory[256] = {'\0'};
    int getCurrentDirSuccessfully = GetCurrentDirectory(256, bufferDirectory); 
    if(0 == getCurrentDirSuccessfully)
    {
        //================================================================== 
        // Author: Thomas K. Chen
        // Modifications Made:
        // - Replaced IGOException class with new Exception class
        // - Exception class was implemented with and derived from standard
        //   C++ library <exception>, therefore has maximum independence.
        //==================================================================				
        // unable to get current directory from Windows
		throw Exception(__FILE__, __LINE__, kstrScope, kstrUnableToGetCurrentDir);
    }
    else if( getCurrentDirSuccessfully > 256 ) // BufferSize not enough
    {
		const int extBufferSize(getCurrentDirSuccessfully);
        char *extBufferDirectory = new char[extBufferSize];
        if(NULL == memset(extBufferDirectory, '\0', extBufferSize))
        {
            //================================================================== 
            // Author: Thomas K. Chen
            // Modifications Made:
            // - Replaced IGOException class with new Exception class
            // - Exception class was implemented with and derived from standard
            //   C++ library <exception>, therefore has maximum independence.
            //==================================================================				
            // unable to get memory buffer from windows
			delete[] extBufferDirectory;
			extBufferDirectory = NULL;
		    throw Exception(__FILE__, __LINE__, kstrScope, kstrUnableToGetMemBuffer);                    
        }

        getCurrentDirSuccessfully = GetCurrentDirectory(extBufferSize, extBufferDirectory); 
        m_strAppPath = extBufferDirectory;

        delete[] extBufferDirectory;
        extBufferDirectory = NULL;
    }
    else
    {
        m_strAppPath = bufferDirectory;
    }

	try
    {
        // get the application properties
	    m_pProps = new Properties(m_strAppPath, strAppName);
    }
    catch(...)
    {
        // Author: Thomas K. Chen
        // Modifications Made:
        // - Replaced IGOException class with new Exception class
        // - Exception class was implemented with and derived from standard
        //   C++ library <exception>, therefore has maximum independence.
        //==================================================================				
        // unable to get memory buffer from windows
		throw Exception(__FILE__, __LINE__, kstrScope, kstrUnableToOpenPropertyFile);   
    }
			
	// setup tracing, default to moderate tracing
	Trace::inst().setLevels(
        getAppProperty(	
            Trace::kstrScope,
            Trace::kstrLevel,
	        static_cast<unsigned int>(	
                Trace::knError | Trace::knTrace | Trace::knInfo	)));
}
	
/*!
Destroy the application services and append new properties to properties file.
*/
void AppServices::destroy()
{
	// write out the application properties
	delete m_pProps;
	m_pProps = 0;
}

/*!
Global method for determining if an application property exists.

\sa Properties::propertyExists
\relates AppServices	
*/
//================================================================== 
// Author: Thomas K. Chen
// Modifications Made:
// - Changed the Qstring from Qt framework to C++ std::string class.
//==================================================================	    
bool appPropertyExists(	const std::string& strScope,
						const std::string& strKey	)
{
	return AppServices::m_pProps->propertyExists(strScope, strKey);
}

