#ifndef log_library_h
#define log_library_h

#include <string>

/**
 \file LogLibrary.h
 \brief Declaration of Debug Utilities. 

 */

/** \mainpage LogLibrary
 *  \section ShortTut Short Tutorial
    The library \c LogLibrary makes it possible for the developer
	of a library to print Debug and Log messages without worring 
	about the target platform. 

	The two functions are \c DbgPrintf() and \c LogPrintf().
	Their syntax is like the old-fashioned \c printf() function.

	The user of a library which uses \c LogPrintf and 
	\c DbgPrintf can determine whether these messages will be displayed 
	or not. 

	To display the messages, the user of the library must set a callback
	function for Dbg messages and for Log messages using the 
	by calling \c SetDbgFunc() and \c SetLogFunc(), respectively.

	The debug and log functions come in two flavors - with and without
	"w" prepended. The version with "w" is meant to handle wide characters.
	

	\code
    // Inside a custom library
	void WorkWithFile()
	{
       char filename[] = "testfile.dat";
       LogPrintf("%s : Opening File : %s \n ", __FUNCTION__, filename);
	   ...
    }
	\endcode

	\code
	// Using of the custom library
    ...
	void LogFunc(char *msg)
	{
		cout << "LOG > " << msg << endl ;
	}

	void main()
	{
		SetLogFunc(LogFunc);   // Register callback. Otherwise no messages
		WorkWithFile();
    	...
	}
	\endcode

*/

#pragma once

/// <summary> Callback function (Log and Debug) for ASCII messages </summary>
typedef void (* TDbgFunc)(char* p);

/// <summary> Callback function (Log and Debug) for wide-character messages </summary>
typedef void (* TDbgFuncW)(wchar_t* p);


/// \brief Print Debug messages. 
///  Messages are using wide characters 
///  
/// \par fmt  Format string. Same as in printf.
void wDbgPrintf(const wchar_t *fmt, ...);  

/// \brief Print Debug messages. 
///  Messages are using 8-bit characters 
///  
/// \par [in] fmt Format string. Same as in printf.
void DbgPrintf(const char* fmt, ...);       

/// \brief Print Log messages. 
///  Messages are using wide characters 
///  
/// \par fmt  Format string. Same as in printf.
void wLogPrintf(const wchar_t *fmt, ...);   


/// \brief Print Log messages. 
///  Messages are using 8-bit characters 
///  
/// \par [in] fmt Format string. Same as in printf.
void LogPrintf(const char* fmt, ...);      

/// \brief Print Log message without formatting. 
///  Messages are using 8-bit characters 
///  
/// \par [in] msg Message string.
void LogPrintf(const std::string &msg);


/// \brief Register a callback function for debug messages
/// The function must be declared as
/// \code
/// void MyDbgPrintFunc(char* message)
/// \endcode
///  
/// \par [in] Pointer to the function
void SetDbgFunc(TDbgFunc);

/// \brief Register a callback function for debug messages 
/// with wide character strings 
/// The function must be declared as
/// \code
/// void MyDbgPrintFunc(wchar_t* message)
/// \endcode
///  
/// \par [in] Pointer to the function
void SetDbgFuncW(TDbgFuncW);

/// \brief Register a callback function for Log messages
/// The function must be declared as
/// \code
/// void MyLogPrintFunc(char* message)
/// \endcode
///  
/// \par [in] Pointer to the function
void SetLogFunc(TDbgFunc);


/// \brief Register a callback function for Log messages 
/// with wide character strings 
/// The function must be declared as
/// \code
/// void MyLogPrintFunc(wchar_t* message)
/// \endcode
///  
/// \par [in] Pointer to the function

void SetLogFuncW(TDbgFuncW);

#endif // log_library_h