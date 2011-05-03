// ===========================================================================
// Program Modification Record
// ===========================================================================
// Component:	Utility
// ===========================================================================
// Class:		StringUtils
// ===========================================================================
// File Name:	StringUtils.cpp
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

#include "StringUtils.h"

// standard includes
#include <iomanip>

//================================================================== 
// Author: Thomas K. Chen
// Modifications Made:
// - Changed the Qstring from Qt framework to C++ std::string class.
//================================================================== 
// Error Messages
const std::string StringUtils::kstrScope = "StringUtils";
const std::string StringUtils::kstrStringTooLong = "Hex string '%1' is too long to convert to number.";
const std::string StringUtils::kstrInvalidCharInString = "Can't convert hex string '%1' to a number because it contains invalid characters.";

/*!
Creates a string containing the hexadecimal representation of the given number.
	
\param nValue		The number.
\param bUppercase	If true the returned string uses uppercase ('A' - 'F')
					otherwise lowercase ('a' - 'f')
	
\return A string containing the number.
*/
std::string StringUtils::toHexString(unsigned int nValue, bool bUppercase)
{
	std::strstream ss;
	
	if (bUppercase)
	{
		ss << std::setiosflags(std::ios::uppercase);
	}
	
	ss << std::setw(4) << std::setfill('0') << std::setbase(16) << nValue << std::ends;
	
	return ss.str();
}

/*!
Convert a hex string to an unsigned long
	
\param str	The string.
	
\return The number
*/
unsigned long StringUtils::parseHexString(const std::string& str)
{
	std::strstream ss;
	
	ss << str;
	
	unsigned long nValue;
	ss >> std::setbase(16) >> nValue;
	
	return nValue;
}

/*!
Get the next field in a string of the format field1<delim>field2<delim>...field3
	
\param str			The string
\param nOffset		The current offset, initialize to zero, updated by this method.
\param strDelimeter	The field delimiter
	
\return The field or the empty string if no fields are left.
*/
std::string StringUtils::getNextField(	const std::string& str,
										std::string::size_type* nOffset,
										const std::string& strDelimiter	)
{
	std::string strField = "";
	
	// make sure we haven't gone off the end
	if (*nOffset != std::string::npos)
	{
		std::string::size_type nNewOffset = str.find(strDelimiter, *nOffset);
		if (nNewOffset == std::string::npos)
		{
			// we've gone off the end of the string, grab the last field
			strField = str.substr(*nOffset, str.length() - *nOffset);
			*nOffset = std::string::npos;
		}
		else
		{
			// grab the field
			strField = str.substr(*nOffset, nNewOffset - *nOffset);
			
			// skip the delimeter
			nNewOffset += strDelimiter.length();
			
			// are we at the end of the string?
			if (nNewOffset >= str.length())
			{
				nNewOffset = std::string::npos;
			}
			
			*nOffset = nNewOffset;
		}
	}

	return strField;
}

/*!
Take off leading and trailing spaces from a string.
	
\param str	The string
	
\return	A new string with whitespace removed
*/
std::string StringUtils::trimWhitespace(const std::string& str)
{
	std::string::size_type nLength;
	
	// find first non-whitespace character
	std::string::size_type nBegin = str.find_first_not_of(' ');
	if (nBegin == std::string::npos)
	{
		// string was empty or all spaces
		nBegin = 0;
		nLength = 0;
	}
	else
	{
		// find last non-whitspace character
		std::string::size_type nEnd = str.find_last_not_of(' ');
		nLength = nEnd - nBegin + 1;
	}
	
	return str.substr(nBegin, nLength);
}

/*!
Calculate the 16-bit CRC for a sequence of characters.
	
\param ptr	Pointer to a buffer containing the characters.
\param nLen	The length of the buffer.
	
\return The 16-bit CRC.
*/
unsigned int StringUtils::calcCRC16(const char* ptr, int nLen)
{
	unsigned int nCRC16 = 0;
	
	const char *pEnd = ptr + nLen;
	for (; ptr < pEnd; ptr++)
	{
		calcCRC16((int) *ptr, &nCRC16);
	}
	
	return nCRC16;
}

/*!
Calculate a running CRC. Taken from the Polaris sample code. Initialize
*pnCRC16 to zero before using the first time.
	
\param nData	The current character.
\param pnCRC16	Pointer to the current CRC value.
*/
void StringUtils::calcCRC16(int nData, unsigned int *pnCRC16)
{
    static int pnOddParity[16] = {	0, 1, 1, 0, 1, 0, 0, 1,
                          			1, 0, 0, 1, 0, 1, 1, 0	};

    nData = (nData ^ (*pnCRC16 & 0xff)) & 0xff;
    *pnCRC16 >>= 8;

    if (pnOddParity[nData & 0x0f] ^ pnOddParity[nData >> 4])
    {
        *pnCRC16 ^= 0xc001;
    }

    nData <<= 6;
    *pnCRC16 ^= nData;
    nData <<= 1;
    *pnCRC16 ^= nData;
}

