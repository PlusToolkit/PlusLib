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

#include "PlusConfigure.h"

#include "Properties.h"

//================================================================== 
// Author: Thomas K. Chen
// Modifications Made:
// - Added includes of C++ Standard Library
//      1. std::string - to replace QString
//      2. std::stringstream - for conversion between data types
//==================================================================
// C++ standard includes 
#include <string>       // std::string
#include <fstream>
#include <iostream>
#include <ctime>

//================================================================== 
// Author: Thomas K. Chen
// Modifications Made:
// - StringUtil class was modified to elimate the trace of Qt frame-
//   work, in particular, the QString and QObject classes. 
// - The include file was renamed to be the same as the class name.
//==================================================================
// local includes
#include "StringUtils.h"

//================================================================== 
// Author: Thomas K. Chen
// Modifications Made:
// - Changed the Qstring from Qt framework to C++ std::string class.
//==================================================================
// file extension for properties files
const std::string Properties::kstrFileExtension = ".properties";

/*!
Open the specified properties file. ".properties" is appended to the name.
If the file is not present, it is be created when the properties object is
destroyed. The application will append the default values for any properties
requested but not found, to the properties file.

\param strFileName	The name of the properties file.
*/
//================================================================== 
// Author: Thomas K. Chen
// Modifications Made:
// - Modified the constructor of Properties to pass strFilePath as
//   one of the parameters.  This elimates the need to fetch the 
//   program application path again (which has been done in the
//   class AppServices that is the owner of the Properties class.
// - Changed the Qstring from Qt framework to C++ std::string class.
//==================================================================
Properties::Properties(
    const std::string& strFilePath, 
    const std::string& strFileName) 
    :
	m_file(strFilePath + "\\" + strFileName + getFileExtension()),
	m_mapFileProps(),
	m_mapDefaultProps()
{
	// make the filename
    //m_file.append("\\" + strFileName + getFileExtension());

    //================================================================== 
    // Author: Thomas K. Chen
    // Modifications Made:
    // - Replace QTextStream with std::ifstream for file I/O processing.
    //==================================================================
	// open the file
	std::ifstream inStream(m_file.c_str(), std::ios::in);
	if (inStream)
	{
		std::string strLine;

		// for each line in the file
		while (std::getline(inStream, strLine))
		{
			// remove leading and trailing spaces
			strLine = StringUtils::trimWhitespace(strLine);

			// ignore empty lines and comment lines
			if ((strLine.length() > 0) && (strLine[0] != '#'))
			{
				// extract the key and value
			    std::string::size_type nOffset = strLine.find('=');
				if (nOffset == std::string::npos)
				{
					// can't find separator, ignore the line
					// can't use Trace because the trace level is not set yet
				}
				else
				{
                    //================================================================== 
                    // Author: Thomas K. Chen
                    // Modifications Made:
                    // - Changed the Qstring from Qt framework to C++ std::string class.
                    //==================================================================
					// add key and value to our map
				    std::string strKey = StringUtils::trimWhitespace(strLine.substr(0, nOffset));
				    std::string strValue = StringUtils::trimWhitespace(strLine.substr(nOffset + 1));

					m_mapFileProps[strKey] = strValue;
				}
			}
		}   // while
    }   // if(inStream)
	else
	{
		// unable to open properties file, probably because it doesn't exist
		// default values for properties will be returned
		// the properties file will be created in the destructor
	}
}

/*!
The destructor creates the properties file if not present and appends default
values for any properties requested but not found, to the file.
*/	
Properties::~Properties()
{
	// if we have some default properties, append them to the properties file
	if (m_mapDefaultProps.size() > 0)
	{
		// get the current time
		time_t now;
		::time(&now);

		// open the properties file for appending, create if it doesn't exist
		std::ofstream outStream(m_file.c_str(), std::ios::app);
		if (outStream)
        {
			// add header info if the file is new
			outStream.seekp(0, std::ios::end);
			if (outStream.tellp() <= 0)
			{
				outStream << "# Properties file '" << m_file <<
					"' created: " << ::ctime(&now);
			}

			// append new properties to the file
			outStream << std::endl;
            outStream << "# New properties added: " << ::ctime(&now) << std::endl;
				
			PropertiesMap::const_iterator itProps = m_mapDefaultProps.begin();
			for (; itProps != m_mapDefaultProps.end(); itProps++)
			{
                outStream << itProps->first << "=" << itProps->second << std::endl;
			}

		}
		else
		{
			// failed to write properties file
		}
	}
}

/*!
Get a property value, return the default if none found. Default properties are
added to the properties file when this object is destructed.

\param strScope			The scope of the key, typically the class name.
\param strKey			The key, a unique identifier within scope that identifies the value.
\param strDefaultValue	The default value to be returned if none is found.

\return	The value corresponding to the given scope and key.
*/
//================================================================== 
// Author: Thomas K. Chen
// Modifications Made:
// - Changed the Qstring from Qt framework to C++ std::string class.
//==================================================================
const std::string Properties::getProperty(
	const std::string& strScope,
	const std::string& strKey,
	const std::string& strDefaultValue	)
{
    //================================================================== 
    // Author: Thomas K. Chen
    // Modifications Made:
    // - Changed the Qstring from Qt framework to C++ std::string class.
    //==================================================================	
    // make the key
	const std::string strRealKey = makeKey(strScope, strKey);
	
	// check the properties we loaded from file
	PropertiesMap::const_iterator itProps = m_mapFileProps.find(strRealKey);
	if (itProps == m_mapFileProps.end())
	{
		// check properties added this session
		itProps = m_mapDefaultProps.find(strRealKey);
		if (itProps == m_mapDefaultProps.end())
		{
			// property was not found, add it to our list of default values
			m_mapDefaultProps[strRealKey] = strDefaultValue;
			itProps = m_mapDefaultProps.find(strRealKey);
		}
	}
	
	return itProps->second;
}

/*!
Check if a property exists.

\param strScope			The scope of the key, typically the class name.
\param strKey			The key, a unique identifier within scope that identifies the value.

\return	true if the property exists, otherwise false.
*/
//================================================================== 
// Author: Thomas K. Chen
// Modifications Made:
// - Changed the Qstring from Qt framework to C++ std::string class.
//==================================================================
bool Properties::propertyExists(	
    const std::string& strScope,
	const std::string& strKey) const
{
	bool bExists = true;

    //================================================================== 
    // Author: Thomas K. Chen
    // Modifications Made:
    // - Changed the Qstring from Qt framework to C++ std::string class.
    //==================================================================
	// make the key
	const std::string strRealKey = makeKey(strScope, strKey);
	
	// check the properties we loaded from file
	PropertiesMap::const_iterator itProps = m_mapFileProps.find(strRealKey);
	if (itProps == m_mapFileProps.end())
	{
		// check properties added this session
		itProps = m_mapDefaultProps.find(strRealKey);
		if (itProps == m_mapDefaultProps.end())
		{
			// property was not found
			bExists = false;
		}
	}
	
	return bExists;
}
										
/*!
Make a property key value from a scope and key.

\param strScope	The scope of the key, typically the class name.
\param strKey	The key, a unique identifier within scope that identifies the value.

\return	The key for the properties maps.
*/
//================================================================== 
// Author: Thomas K. Chen
// Modifications Made:
// - Changed the Qstring from Qt framework to C++ std::string class.
//==================================================================
const std::string Properties::makeKey(	
    const std::string& strScope,
    const std::string& strKey) const
{
	return	StringUtils::trimWhitespace(strScope) +
			"." +
			StringUtils::trimWhitespace(strKey);
}

/*!
Get a property value, return the default if none found. Default properties are
added to the properties file when this object is destructed. Specialized for
strings.

\param props			The properties object.
\param strScope			The scope of the key, typically the class name.
\param strKey			The key, a unique identifier within scope that identifies the value.
\param strDefaultValue	The default value to be returned if none is found.

\return	The value corresponding to the given scope and key.
*/
//================================================================== 
// Author: Thomas K. Chen
// Modifications Made:
// - Changed the Qstring from Qt framework to C++ std::string class.
//==================================================================
template<> const std::string getProperty(	
    Properties& props,
	const std::string& strScope,
	const std::string& strKey,
	const std::string& strDefaultValue)
{
	return props.getProperty(strScope, strKey, strDefaultValue);
}

/*!
Get a property value, return the default if none found. Default properties are
added to the properties file when this object is destructed. Specialized for
bool.

\param props			The properties object.
\param strScope			The scope of the key, typically the class name.
\param strKey			The key, a unique identifier within scope that identifies the value.
\param bDefaultValue	The default value to be returned if none is found.

\return	The value corresponding to the given scope and key.
*/
//================================================================== 
// Author: Thomas K. Chen
// Modifications Made:
// - Changed the Qstring from Qt framework to C++ std::string class.
//==================================================================
template<> const bool getProperty(	
    Properties& props,
    const std::string& strScope,
    const std::string& strKey,
    const bool& bDefaultValue )
{
	std::string strValue = props.getProperty(strScope, strKey, bDefaultValue ? "1" : "0");

	return (strValue == "1");
}
