#ifndef APPSERVICES_H
#define APPSERVICES_H
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


//================================================================== 
// Author: Thomas K. Chen
// Modifications Made:
// - Added includes of C++ Standard string class: std::string
//==================================================================
// C++ standard includes 
#include <string>

//================================================================== 
// Author: Thomas K. Chen
// Modifications Made:
// - Properties class was modified to elimate the trace of Qt frame-
//   work, in particular, the QString and QObject classes. 
// - The include file was renamed to be the same as the class name.
//==================================================================
// local includes
#include "Properties.h"

// standard includes
	
//! Manages services used by the application.
/*!
This class manages a set of services provided to the application. It currently
manages a set of application properties and trace settings.

\code
int main(int, char* [])
{
	AppServices::initialize(strAppName);

	// Application body here.

	AppServices::destroy();
	return 0;
}
\endcode
*/

class AppServices
{
    public:
        //================================================================== 
        // Author: Thomas K. Chen
        // Modifications Made:
        // - Changed the Qstring from Qt framework to C++ std::string class.
        //================================================================== 
	    // Error Messages
	    static const std::string kstrScope;
        static const std::string kstrUnableToGetCurrentDir;
        static const std::string kstrUnableToGetMemBuffer;
        static const std::string kstrUnableToOpenPropertyFile;

        //================================================================== 
        // Author: Thomas K. Chen
        // Modifications Made:
        // - Changed the Qstring from Qt framework to C++ std::string class.
        //==================================================================	    
	    //! Initialize the application services
	    static void initialize(const std::string& strAppName);
	    
	    //! Destroy the application services
	    static void destroy();

        //================================================================== 
        // Author: Thomas K. Chen
        // Modifications Made:
        // - Changed the Qstring from Qt framework to C++ std::string class.
        //==================================================================	    
	    //! Get the path to the application
	    static const std::string& getAppPath() {return m_strAppPath;}
	    
	    //! The application properties
	    static Properties* m_pProps;

    protected:

    private:
	    //! Default constructor defined so compiler won't generate public one
	    AppServices();

	    //! Copy constructor defined so compiler won't generate public one
	    AppServices(const AppServices&);

	    //! Assignment operator defined so compiler won't generate public one
	    void operator= (const AppServices&);

        //================================================================== 
        // Author: Thomas K. Chen
        // Modifications Made:
        // - Changed the Qstring from Qt framework to C++ std::string class.
        //==================================================================	    
	    //! Path to application directory
	    static std::string m_strAppPath;
};

//================================================================== 
// Author: Thomas K. Chen
// Modifications Made:
// - Changed the Qstring from Qt framework to C++ std::string class.
//==================================================================	    
//! Does an application property exist?
bool appPropertyExists(	const std::string& strScope,
						const std::string& strKey	);

/*!
Global function template for getting an application property.

\sa Properties::getProperty
\relates AppServices
*/
//================================================================== 
// Author: Thomas K. Chen
// Modifications Made:
// - Changed the Qstring from Qt framework to C++ std::string class.
//==================================================================	    
template<class T> const T getAppProperty(	const std::string& strScope,
											const std::string& strKey,
											const T& defaultValue	)
{
	// use default value if not initialized
	T outValue = defaultValue;
	
	if (0 != AppServices::m_pProps)
	{
		outValue = getProperty(*(AppServices::m_pProps), strScope, strKey, defaultValue);
	}
	
	return outValue;
};

#endif

