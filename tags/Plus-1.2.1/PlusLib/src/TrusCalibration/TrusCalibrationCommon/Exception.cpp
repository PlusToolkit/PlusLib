// ===========================================================================
// Program Creation Record
// Copyright 2003 @ The Queen's University at Kingston, Canada
// ===========================================================================
// Component:	Utility
// ===========================================================================
// Class:		Exception
// ===========================================================================
// File Name:	Exception.cpp
// ===========================================================================
// Author:		Thomas K. Chen <chent@cs.queensu.ca>
//				School of Computing, Queen's University at Kingston, Canada
// ===========================================================================
// Comments: 
// 1. This is a utility class created for exception handling.
// 2. It is intended to serve as a reusable class for general error handling.  
// 3. It is a direct derived-class to std::exception from C++ Standard Library. 
// 4. To use the class, include Exception.h and compile with Exception.cpp.
// ===========================================================================
// Change History:
// Author			Time						Release	Changes
// Thomas K. Chen	Mon Jun 16 15:15 EDT 2003	1.0		Creation
//
// ===========================================================================


#ifndef EXCEPTION_H
#include "Exception.h"
#endif

// =============
// class Error
// =============
Error::Error(	
    const string& strScope,
	const string& strError,
	const string& strArg1,
	const string& strArg2,
	const string& strArg3,
	const string& strArg4	) 
    : 
    m_strScope(strScope),
	m_strError(strError),
	m_strArg1(strArg1),
	m_strArg2(strArg2),
	m_strArg3(strArg3),
	m_strArg4(strArg4)
{

}	

const string& Error::getScope() const 
{
    return m_strScope;
}

const string& Error::getError() const 
{
    return m_strError;
}

const string& Error::getArg1() const 
{
    return m_strArg1;
}

const string& Error::getArg2() const 
{
    return m_strArg2;
}

const string& Error::getArg3() const 
{
    return m_strArg3;
}

const string& Error::getArg4() const 
{
    return m_strArg4;
}

bool Error::isScope(const string& strScope) const 
{
    return m_strScope == strScope;
}

bool Error::isError(const string& strError) const 
{
    return m_strError == strError;

}   
// END of class Error


// ================
// class Exception
// ================
Exception::Exception(
    const string& file,     
    const int line,
	const string& strScope,
	const string& strError,
	const string& strArg1,
	const string& strArg2,
	const string& strArg3,
	const string& strArg4	)
    : 
    mOrigErrWhat((strScope.empty() || strError.empty()) ? UNSPEC_EXCP : strScope + "::" + strError),
    mOrigErrFile((EMPTY == file) ? UNSPEC_FILE : file),
    mOrigErrLine((0 > line) ? UNSPEC_LINE : itoa(line)),
    mOrigErrSummary(EMPTY)
{
	// add this error message to the exception
	addError(strScope, strError, strArg1, strArg2, strArg3, strArg4);
}	

void Exception::addError(
	const string& strScope,
	const string& strError,
	const string& strArg1,
	const string& strArg2,
	const string& strArg3,
	const string& strArg4	)
{
	Error error(strScope, strError, strArg1, strArg2, strArg3, strArg4);					
	m_lstErrors.push_back(error);
}

bool Exception::hasScope(const string& strScope) const
{
	bool bFound = false;
	
	for (ErrorIterator p = begin(); p != end(); ++p)
	{
		if (p->isScope(strScope))
		{
			bFound = true;
			break;
		}
	}
	
	return bFound;
}

bool Exception::hasError(const string& strError) const
{
	bool bFound = false;
	
	for (ErrorIterator p = begin(); p != end(); ++p)
	{
		if (p->isError(strError))
		{
			bFound = true;
			break;
		}
	}
	
	return bFound;
}

Exception::ErrorIterator Exception::begin() const
{
	// latest errors are appended to the end so get a reverse iterator
	return m_lstErrors.rbegin();
}

Exception::ErrorIterator Exception::end() const
{
	// latest errors are appended to the end so get a reverse iterator
	return m_lstErrors.rend();

}   

const char* Exception::what () const throw ()
{
    return mOrigErrWhat.c_str();
}

const string Exception::what_file () const
{
    return mOrigErrFile;
}

const string Exception::what_line () const
{
    return mOrigErrLine;
}

const string Exception::summary ()
{
    if (mOrigErrSummary.empty())
    {
        mOrigErrSummary += "Exception: %Exception was detected, ";
        mOrigErrSummary += Exception::what();
        mOrigErrSummary += " (In File: ";
        mOrigErrSummary += what_file();
        mOrigErrSummary += ", On Line: ";
        mOrigErrSummary += what_line();
        mOrigErrSummary += ")";
    }

    return mOrigErrSummary;
}

const string Exception::itoa (const int ival)
{
    int i(ival);
    std::string str("");
    if (i < 0)
    {
        str+="-";
    }
    do
    {
        str.insert((std::string::size_type)0, (std::string::size_type)1, (i%10 + '0'));
    }
    
	while ((i/=10)>0);

    return str;
}
// END of class Exception

