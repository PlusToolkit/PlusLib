#ifndef TRACEUTILS_H
#define TRACEUTILS_H
// ===========================================================================
// Program Modification Record
// ===========================================================================
// Component:	Utility
// ===========================================================================
// Class:		Trace
// ===========================================================================
// File Name:	TraceUtils.h
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


//================================================================== 
// Author: Thomas K. Chen
// Modifications Made:
// - Added includes of C++ Standard string class: std::string
//==================================================================
// C++ standard includes 
#include <string>
#include <strstream>

/*
The trace calls are implemented as macros so we can get the file name and line
number and to avoid the overhead of a method call when trace is disabled.
*/

//! For serious errors and exceptions.
#define TRACE_ERROR(str) \
	{if (TRACE_ENABLED(Trace::knError)){Trace::message(Trace::knError, __FILE__, __LINE__, (str));}}

//! For general information messages, for example user actions, recoverable errors, and numeric results.
#define TRACE_INFO(str) \
	{if (TRACE_ENABLED(Trace::knInfo)){Trace::message(Trace::knInfo, __FILE__, __LINE__, (str));}}

//! For execution tracing. (not used)
#define TRACE_TRACE(str) \
	{if (TRACE_ENABLED(Trace::knTrace)){Trace::message(Trace::knTrace, __FILE__, __LINE__, (str));}}

//! For debugging, for example numeric results.
#define TRACE_DEBUG(str) \
	{if (TRACE_ENABLED(Trace::knDebug)){Trace::message(Trace::knDebug, __FILE__, __LINE__, (str));}}

//! For communications with hardware.
#define TRACE_COMM(str) \
	{if (TRACE_ENABLED(Trace::knComm)){Trace::message(Trace::knComm, __FILE__, __LINE__, (str));}}

//! For heartbeat tracing, for example things that change regularly.
#define TRACE_HEARTBEAT(str) \
	{if (TRACE_ENABLED(Trace::knHeartbeat)){Trace::message(Trace::knHeartbeat, __FILE__, __LINE__, (str));}}

//! Is a given trace level enabled. Use this method before entering a loop that consists only of TRACE statements
#define TRACE_ENABLED(nLevel) \
	(0 != (Trace::inst().getLevels() & (nLevel)))

//! Manages debug output.
class Trace
{
    public:
	    //! Trace flags.
	    /*!
	    knNone = 0	No tracing
	    knError		Serious errors and exceptions
	    knInfo		General information (minimal output)
	    knTrace		Execution tracing information (moderate output)
	    knDebug		Debugging info (lots of output)
	    knComm		Communications traffic (lots of output)
	    knHeartbeat	Messages that occur every heartbeat (lots of output)
	    */
	    enum eLevel
	    {
		    knNone = 0x00,
		    knError = 0x01,
		    knInfo = 0x02,
		    knTrace = 0x04,
		    knDebug = 0x08,
		    knComm = 0x10,
		    knHeartbeat = 0x20
	    };

        //================================================================== 
        // Author: Thomas K. Chen
        // Modifications Made:
        // - Changed the Qstring from Qt framework to C++ std::string class.
        //==================================================================        
	    //! Prefixes for trace output
	    static const std::string kstrErrorPrefix;
	    static const std::string kstrInfoPrefix;
	    static const std::string kstrTracePrefix;
	    static const std::string kstrDebugPrefix;
	    static const std::string kstrCommPrefix;
	    static const std::string kstrHeartbeatPrefix;

        //================================================================== 
        // Author: Thomas K. Chen
        // Modifications Made:
        // - Changed the Qstring from Qt framework to C++ std::string class.
        //==================================================================        
	    //! Properties
	    static const std::string kstrScope;
	    static const std::string kstrLevel;

	    //! Get singleton instance.
	    static Trace& inst();

	    //! Set the trace levels. Trace messages for a given level if the corresponding bit is set.
	    void setLevels(unsigned int nLevels) {m_nLevels = nLevels;}

	    //! Get the trace levels
	    inline unsigned int getLevels() const {return m_nLevels;}

        //================================================================== 
        // Author: Thomas K. Chen
        // Modifications Made:
        // - Replace QTextStream with std::strstream.
        //==================================================================
	    //! Set the output stream.
	    void setOutputStream(std::strstream* pOutputStream) {m_pOutputStream = pOutputStream;}

        //================================================================== 
        // Author: Thomas K. Chen
        // Modifications Made:
        // - Replace QTextStream with std::strstream.
        //==================================================================
	    //! Get the output stream.
	    std::strstream* getOutputStream() { return m_pOutputStream; }

        //================================================================== 
        // Author: Thomas K. Chen
        // Modifications Made:
        // - Changed the Qstring from Qt framework to C++ std::string class.
        //==================================================================
	    //! Send a trace message. Use the macros instead of this method.
	    static void message(	eLevel nLevel,
							    const std::string& strFile,
							    int nLine,
							    const std::string& strMessage	);

    protected:

    private:
	    //! Constructor
	    Trace();

	    //! Not implemented
	    Trace(const Trace&);

	    //! Not implemented
	    void operator=(const Trace&);

  	    //! The current trace levels
  	    unsigned int m_nLevels;

        //================================================================== 
        // Author: Thomas K. Chen
        // Modifications Made:
        // - Replace QTextStream with std::strstream.
        //==================================================================
	    //! Output stream.
	    std::strstream* m_pOutputStream;

	    //! Singleton
	    static Trace* m_pTrace;
};

#endif
