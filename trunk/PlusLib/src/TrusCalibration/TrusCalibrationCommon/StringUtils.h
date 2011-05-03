#ifndef STRING_UTILS_H
#define STRING_UTILS_H
// ===========================================================================
// Program Modification Record
// ===========================================================================
// Component:	Utility
// ===========================================================================
// Class:		StringUtils
// ===========================================================================
// File Name:	StringUtils.h
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
// Thomas K. Chen	Mon Jun 26 09:22 EDT 2003	1.0		Generalization
//
// ===========================================================================

// standard includes
#include <string>
#include <strstream>
	
//! Various string utilities
class StringUtils
{
    public:
	    // Error Messages
	    static const std::string kstrScope;
	    static const std::string kstrStringTooLong;
	    static const std::string kstrInvalidCharInString;


	    //! Convert a number to its hexadecimal representation
	    static std::string toHexString(unsigned int nValue, bool bUppercase = false);

	    //! Convert a hex string to an unsigned long
	    static unsigned long parseHexString(const std::string& str);
	    
	    //! Get the next field of a delimited string
	    static std::string getNextField(
            const std::string& str,
    	    std::string::size_type* nOffset,
	        const std::string& strDelimiter	);
								    
	    //! Trim whitespace from a string
	    static std::string trimWhitespace(const std::string& str);

	    //! Calculate a 16-bit CRC for a sequence of characters
	    static unsigned int calcCRC16(const char* ptr, int nLen);
					    
    private:
 	    //! Calculate a running CRC. Taken from the Polaris sample code.
	    static void calcCRC16(const int data, unsigned int *puCRC16);
};


/*!
Global template method for converting a value to a string.

\param value	The value;

\return	A string containing the value.
*/
template <class T> std::string toString(const T& value)
{
	std::strstream ss;
	ss << value << std::ends;
	
    return ss.str();
}


#endif
