#ifndef PROPERTIES_H
#define PROPERTIES_H
// ===========================================================================
// Program Modification Record
// ===========================================================================
// Component:	Utility
// ===========================================================================
// Class:		Properties
// ===========================================================================
// File Name:	Properties.h
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
// Thomas K. Chen	Mon Jun 30 19:03 EDT 2003	1.0		Generalization
//
// ===========================================================================


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
// - Added includes of C++ Standard string class: std::string
//==================================================================
// C++ standard includes 
#include <string>
//==================================================
// Thomas: Modified for .NET 2003 (more ANSI C++)
// Timestamp: Wed Aug 6 22:29 EDT 2005
// - Replace strstream (C_str) with sstream (string)
//==================================================
//#include <strstream>
#include <sstream>

// standard includes
#include <map>
	
//! Manages properties used by the application
/*!
This class is loosely modelled from the Java Properties class. It manages a
file of key, value pairs that provide application configuration information.

In the file, blank lines and lines beginning with # are ignored. The syntax
of a key value pair is:

key=value<nl>

By convention, the key has the format scope.subkey where scope is the name of
the class that uses the value and subkey is a unique identifier for the
property. Leading and trailing spaces are removed from both key and value.

An example properties file looks like the following:

# Properties file 'FluoroGuide.properties' created: Mon Feb 21 12:56:34 2000

OpticalTracker.type=Polaris
OpticalTracker.baudRate=57600
OpticalTracker.parity=E

# New properties added: Mon Feb 21 16:21:02 2000

DRB.ID=8700223
DRB.IsActive=true
DRB.IsDynamic=false
Drill.ID=8700247
Drill.IsActive=false
Drill.IsDynamic=true

*/
class Properties
{
    public:
        //================================================================== 
        // Author: Thomas K. Chen
        // Modifications Made:
        // - Modified the constructor of Properties to pass strFilePath as
        //   one of the parameters.  This elimates the need to fetch the 
        //   program application path again (which has been done in the
        //   class AppServices that is the owner of the Properties class.
        // - Changed the Qstring from Qt framework to C++ std::string class.
        //==================================================================	    
        //! Construct a properties object from a file, create the file if not present.
	    explicit Properties(
            const std::string& strFilePath, 
            const std::string& strFileName);
	    
	    //! Destructor, write default values for any new properties to the file.
	    ~Properties();

        //================================================================== 
        // Author: Thomas K. Chen
        // Modifications Made:
        // - Changed the Qstring from Qt framework to C++ std::string class.
        //==================================================================	            
	    //! Get the file extension used for the properties file
	    static const std::string& getFileExtension() {return kstrFileExtension;}

        //================================================================== 
        // Author: Thomas K. Chen
        // Modifications Made:
        // - Changed the Qstring from Qt framework to C++ std::string class.
        //==================================================================	    
	    //! Get a property value, return the default if none found.
	    const std::string getProperty(
		    const std::string& strScope,
		    const std::string& strKey,
		    const std::string& strDefaultValue	);

        //================================================================== 
        // Author: Thomas K. Chen
        // Modifications Made:
        // - Changed the Qstring from Qt framework to C++ std::string class.
        //==================================================================	    
	    //! Check if a property exists.
	    bool propertyExists(	const std::string& strScope,
							    const std::string& strKey	) const;											

        //================================================================== 
        // Author: Thomas K. Chen
        // Modifications Made:
        // - Changed the Qstring from Qt framework to C++ std::string class.
        //==================================================================	    
	    //! Make a property key value from a scope and key.
	    const std::string makeKey(	const std::string& strScope,
							        const std::string& strKey	) const;

    protected:

    private:
        //================================================================== 
        // Author: Thomas K. Chen
        // Modifications Made:
        // - Changed the Qstring from Qt framework to C++ std::string class.
        //==================================================================	    
	    //! The extension used for properties files
	    static const std::string kstrFileExtension;

        //================================================================== 
        // Author: Thomas K. Chen
        // Modifications Made:
        // - Changed the Qstring from Qt framework to C++ std::string class.
        //==================================================================	            
	    //! The properties file
	    std::string m_file;

        //================================================================== 
        // Author: Thomas K. Chen
        // Modifications Made:
        // - Changed the Qstring from Qt framework to C++ std::string class.
        //==================================================================	            
	    //! A properties map
	    typedef std::map<std::string, std::string> PropertiesMap;
	    
 	    //! The key value pairs read from the file
	    PropertiesMap m_mapFileProps;
	    
	    //! The key value pairs not found in the file
	    PropertiesMap m_mapDefaultProps;
};

/*!
Get a property value, return the default if none found. Default properties are
added to the properties file when this object is destructed.

\param props	The properties object.
\param strScope	The scope of the key, typically the class name.
\param strKey	The key, a unique identifier within scope that identifies the value.
\param defaultValue	The default value to be returned if none is found.

\return	The value corresponding to the given scope and key.
*/
//================================================================== 
// Author: Thomas K. Chen
// Modifications Made:
// - Changed the Qstring from Qt framework to C++ std::string class.
//==================================================================	    
template<class T> const T getProperty(	Properties& props,
										const std::string& strScope,
										const std::string& strKey,
										const T& defaultValue )
{
    //================================================================== 
    // Author: Thomas K. Chen
    // Modifications Made:
    // - Replace QTextStream with std::strstream.
    //==================================================================
	// convert default value to a string, << trims whitespace
	std::strstream strDefaultStream;
	strDefaultStream << defaultValue;

	// call specialized form of this template for strings
	std::string strDef;
	strDefaultStream >> strDef;
	const std::string strValue = props.getProperty(strScope, strKey, strDef);

    //================================================================== 
    // Author: Thomas K. Chen
    // Modifications Made:
    // - Replace QTextStream with std::strstream.
    //==================================================================
	//==================================================
	// Thomas: Modified for .NET 2003 (more ANSI C++)
	// Timestamp: Wed Aug 6 22:29 EDT 2005
	//==================================================
	// convert the value from a string
	T outValue;
	std::strstream strOutStream;
	strOutStream << strValue.c_str(); 
	strOutStream >> outValue;

	if (!strOutStream.eof() && (strOutStream.fail() || strOutStream.bad()))
	{
		std::string strFullKey = props.makeKey(strScope, strKey);
		
		// conversion failed, use the default value
		TRACE_ERROR(	"Conversion failed, returning default: key = " +
						strFullKey +
						" default value = " +
						strDef	);

		outValue = defaultValue;
	}
	
	return outValue;
}

//================================================================== 
// Author: Thomas K. Chen
// Modifications Made:
// - Changed the Qstring from Qt framework to C++ std::string class.
//==================================================================	    
//! Specialization for QString of getProperty() template method.
template<> const std::string getProperty(	
    Properties& props,
	const std::string& strScope,
	const std::string& strKey,
	const std::string& strDefaultValue);

//================================================================== 
// Author: Thomas K. Chen
// Modifications Made:
// - Changed the Qstring from Qt framework to C++ std::string class.
//==================================================================	    
//! Specialization for bool of getProperty() template method.
template<> const bool getProperty(	
    Properties& props,
	const std::string& strScope,
	const std::string& strKey,
	const bool& bDefaultValue);

#endif
