#ifndef EXCEPTION_H
#define EXCEPTION_H
// ===========================================================================
// Program Creation Record
// Copyright 2003 @ The Queen's University at Kingston, Canada
// ===========================================================================
// Component:	Utility
// ===========================================================================
// Class:		Exception
// ===========================================================================
// File Name:	Exception.h
// ===========================================================================
// Author:		Thomas K. Chen <chent@cs.queensu.ca>
//				School of Computing, Queen's University at Kingston, Canada
// ===========================================================================
// Comments: 
// 1. This is a utility class created for exception handling.
// 2. It is intended to serve as a reusable class for general error handling.  
// 3. It is a direct derived-class to std::exception from C++ Standard Library. 
// 4. A layer structure of different level of exception details are implemented.
// 4. To use the class, include Exception.h and compile with Exception.cpp.
// ===========================================================================
// Change History:
// Author			Time						Release	Changes
// Thomas K. Chen	Mon Jun 16 15:15 EDT 2003	1.0		Creation
//
// ===========================================================================


// C++ Standard Library Includes
#include <string>
#include <exception>
#include <list>

const std::string UNSPEC_EXCP("*UNSPECIFIED EXCEPTION*");
const std::string UNSPEC_FILE("*UNSPECIFIED FILE*");
const std::string UNSPEC_LINE("*UNSPECIFIED LINE*");
const std::string EMPTY("");

using namespace std;

//======================================================================
// class Error
// 1. Entity that contains error message that correspond to different
//    level of scope (class) for one exception.
// 2. An exception could have multiple level of errors during throwing.
// 3. An error is made up of a scope (the classname where the error 
//    string is defined), an error string containing up to 4 
//    replaceable tags of the form %n where n is 1-4, and up to 4 
//    arguments corresponding to the tags. Care must be taken to provide 
//    values for all tags in the error string.
// 4. Construct an Error object:
//    - strScope    The scope where error occurs (usually the class name).
//    - strError    The error message containing up to 4 replaceable 
//                  parameters as described above.
//    - strArg1	    The string that replaces tag %1. (Optional)
//    - strArg2	    The string that replaces tag %2. (Optional)
//    - strArg3	    The string that replaces tag %3. (Optional)
//    - strArg4	    The string that replaces tag %4. (Optional)
// 5. Errors are not generally created directly but either as a result 
//    of creating an Exception object or by calling the addError() 
//    method for an existing Exception object.
//====================================================================== 
class Error
{
    public:
	    // Constructor
	    Error (
            const string& strScope, 
            const string& strError, 
            const string& strArg1 = EMPTY,
			const string& strArg2 = EMPTY,
			const string& strArg3 = EMPTY,
			const string& strArg4 = EMPTY);

        // Operation	        
	    // Get the scope (class) in which the error occurs 
	    const string& getScope() const;

        // Operation					    
	    // Get the error message
	    const string& getError() const;

        // Operation					    
	    // Get arg 1
	    const string& getArg1() const;

        // Operation			    
	    // Get arg 2
	    const string& getArg2() const;

        // Operation					    
	    // Get arg 3
	    const string& getArg3() const;
        
        // Operation					    
	    // Get arg 4
	    const string& getArg4() const;
        
        // Operation					    
	    // Determines if this error has the given scope.
	    bool isScope(const string& strScope) const;

        // Operation					    
	    // Determines if this error has the given error.
	    bool isError(const string& strError) const;
				    
    private:
	    //! The class name where the error message is defined
	    const string m_strScope;
	    
	    //! The error string
	    const string m_strError;
	    
	    //! First of four optional replaceable arguments in error string
	    const string m_strArg1;
	    
	    //! Second of four optional replaceable arguments in error string
	    const string m_strArg2;
	    
	    //! Third of four optional replaceable arguments in error string
	    const string m_strArg3;
	    
	    //! Fourth of four optional replaceable arguments in error string
	    const string m_strArg4;

};  // class Error


//======================================================================
// class Exception
// 1. Entity that contains individual execption thrown out.
// 2. An exception could have multiple layers of errors during throwing.
//    Therefore, it's the owner of one or more Error objects.
// 3. Typically, the error that originates the problem is the on the
//    lowest layer of the structure (thus the most detailed).  A higher
//    lever error message is normally generated when the same exception
//    gets thrown all way up to the control component, and is generally
//    more abstract and lack of details than the lowest error.
// 4. a std::list object of a chain of errors from bottom up is used to
//    illustrate the error information, from general to specific.
// 5. The class is a direct derived-class of std::exception
//
//    Example
//    . . .
//
//    const string MyImagerType::kstrScope("MyImagerType");
//    const string MyImagerType::kstrUnknownFrameGrabberType("Unknown frame grabber type: '%1'.");
//
//      . . .
//
//	    if (strFrameGrabberType == "SomeFrameGrabber")
//	    {
//		    m_pFrameGrabber = new SomeFrameGrabber();
//	    }
//	    else
//	    {
//		    throw Exception(__FILE__, __LINE__, kstrScope, kstrUnknownFrameGrabberType, strFrameGrabberType);
//	    }
//
//      . . .
//====================================================================== 
class Exception : public std::exception
{
    public:
	    //Iterator for errors.
        typedef std::list<Error>::const_reverse_iterator ErrorIterator;

	    // Constructor
	    Exception(
            const string& file,     // File where the exception originates 
            const int line,         // Line where the exception originates
            const string& strScope, // The class in which exception originates
            const string& strError, // Error message
			const string& strArg1 = EMPTY,  // Parameter1 of Error message, if any
			const string& strArg2 = EMPTY,  // Parameter2 of Error message, if any
			const string& strArg3 = EMPTY,  // Parameter3 of Error message, if any
			const string& strArg4 = EMPTY); // Parameter4 of Error message, if any

        // Operation	
	    // Add an error to this exception.  This operation is useful in a 
        // layered exception handling approach, where exceptions from lower 
        // level are caught, another error added and the exception rethrown. 
        // This makes it possible for the topmost exception handler to drill 
        // down into the exception for additional detail if desired. See class
        // Error for a description of the parameters.
	    void addError(	
            const string& strScope,
			const string& strError,
			const string& strArg1 = EMPTY,
			const string& strArg2 = EMPTY,
			const string& strArg3 = EMPTY,
			const string& strArg4 = EMPTY);

        // Operation	
		// Report error
        virtual const char* what () const throw ();

        // Operation        
		// Report the name of file in which error occurs
        const string what_file () const;

        // Operation        
		// Report the line error occurred
        const string what_line () const;

        // Operation	
        // Report the error location
        virtual const string summary ();
	    
        // Operation	
	    // Does this exception contain the given scope?
        // Used to determine if the exception contains a given class of error. 
        // This is useful since we uses a flat exception hierarchy. For example, 
        // if an exception handler wanted to find out if the exception was a 
        // serial IO exception, it'll pass SerialIO::<nameScope> as the parameter.
	    // Return true if the error in the exception has the given scope.
	    bool hasScope(const string& strScope) const;

        // Operation	
	    // Does this exception contain the given error?
        // Used to determine if the exception contains a specific error. For 
        // example, if an exception handler wanted to find out if the exception 
        // was due to a serial IO read timeout, it would pass SerialIO::<nameError>
        // as the parameter.  Since the error string is not expanded until it is 
        // displayed then the handed in string can be compared to the stored ones.
        // Return true if the exception contains the given error.
	    bool hasError(const string& strError)const;

        // Operation	
	    // Get an iterator to the first error
        // Get an iterator to the first error. Used to step through the errors 
        // associated with an exception when more detail is required.
        // Return an iterator pointing to the first error associated with this 
        // exception.
	    ErrorIterator begin() const;

        // Operation	
	    // Get an iterator to the last error
        // Get an iterator to the last error. Used to step through the errors 
        // associated with an exception when more detail is required.
        // Return an iterator pointing just after the last error associated with 
        // this exception.
	    ErrorIterator end() const;

    protected:

        // Operation        
		// Convert an integer to a string
        const string itoa (const int ival);

    private:

        // Data Members for Class Attributes

        // Attribute
        string mOrigErrWhat;

		// Attribute
		string mOrigErrFile;

        // Attribute
		string mOrigErrLine;

        // Attribute
		string mOrigErrSummary;

	    // The list of errors added to this exception
	    list<Error> m_lstErrors;

};  // class Exception


#endif


