// ===========================================================================
// Program Modification Record
// ===========================================================================
// Component:	Utility
// ===========================================================================
// Class:		Trace
// ===========================================================================
// File Name:	TraceUtils.cpp
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
// Thomas K. Chen	Mon Jun 26 09:29 EDT 2003	1.0		Generalization
//
// ===========================================================================


#ifndef TRACEUTILS_H
#include "TraceUtils.h"
#endif

//================================================================== 
// Author: Thomas K. Chen
// Modifications Made:
// - Added includes of C++ Standard string class: std::string
//==================================================================
// C++ standard includes 
#include <string>
#include <strstream>

// C++ standard includes
#include <iostream>

//! Singleton.
Trace* Trace::m_pTrace = 0;

//================================================================== 
// Author: Thomas K. Chen
// Modifications Made:
// - Changed the Qstring from Qt framework to C++ std::string class.
//==================================================================	
//! Prefixes for trace output
const std::string Trace::kstrErrorPrefix = "ERROR";
const std::string Trace::kstrInfoPrefix = "INFO";
const std::string Trace::kstrTracePrefix = "TRACE";
const std::string Trace::kstrDebugPrefix = "DEBUG";
const std::string Trace::kstrCommPrefix = "COMM";
const std::string Trace::kstrHeartbeatPrefix = "HEARTBEAT";
	
//================================================================== 
// Author: Thomas K. Chen
// Modifications Made:
// - Changed the Qstring from Qt framework to C++ std::string class.
//==================================================================	
//! Properties
const std::string Trace::kstrScope = "Trace";
const std::string Trace::kstrLevel = "Level";

/*!
Send a trace message to cout if the trace level is high enough. Do not call
this method directly, use the macros defined in trace.h instead.

\param strPrefix	The prefix for the message.
\param strFile		The name of the file where the message originated.
\param nLine		The line in the file where the message originated.
\param strMessage	The message.
*/
//================================================================== 
// Author: Thomas K. Chen
// Modifications Made:
// - Changed the Qstring from Qt framework to C++ std::string class.
//==================================================================	
void Trace::message(
	eLevel nLevel,
	const std::string& strFile,
	int nLine,
	const std::string& strMessage	)
{
    //================================================================== 
    // Author: Thomas K. Chen
    // Modifications Made:
    // - Replace QTextStream with std::strstream.
    //==================================================================
	// Trace if we have an output stream. Trace level is checked by macro
	std::strstream* pOutputStream = Trace::inst().getOutputStream();

	if (0 != pOutputStream )
	{
		std::string strPrefix;

		switch (nLevel)
		{
			case knError:
				strPrefix = kstrErrorPrefix;
				break;

			case knInfo:
				strPrefix = kstrInfoPrefix;
				break;

			case knTrace:
				strPrefix = kstrTracePrefix;
				break;

			case knDebug:
				strPrefix = kstrDebugPrefix;
				break;

			case knComm:
				strPrefix = kstrCommPrefix;
				break;

			case knHeartbeat:
				strPrefix = kstrHeartbeatPrefix;
				break;

			case knNone:
				strPrefix = "";
				break;
		}

		// Message format is: prefix (file, line): message
		{
			(*pOutputStream)	<< strPrefix
								<< " (" << strFile << ", "
								<< nLine << "): "
								<< strMessage
                                << std::endl;
		}
	}
}	

/*!
Constructor
*/
Trace::Trace() :
	m_nLevels( knNone ),
	m_pOutputStream( 0 )
{
}

/*!
Get singleton instance.
*/
Trace& Trace::inst()
{
	if (0 == m_pTrace)
	{
		m_pTrace = new Trace();
	}
	return *m_pTrace;
}
