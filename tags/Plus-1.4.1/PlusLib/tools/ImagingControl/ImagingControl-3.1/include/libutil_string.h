#ifndef LIBUTIL_STRING_H_INC_
#define LIBUTIL_STRING_H_INC_

#pragma once

#include <string>
#include "libbasedefs.h"

namespace _DSHOWLIB_NAMESPACE
{
	/**	wide string to ascii string
	* \param op the wide input string
	* \param rval temporary ascii string
	* \return the resulting ascii string
	*/
	std::string&		wstoas( const std::wstring& op, std::string& rval );
	std::string			wstoas( const std::wstring& op );

	/**	ascii string to wide string
	* \param op the ascii input string
	* \param rval temporary wide string
	* \return the resulting wide string
	*/
	std::wstring&		astows( const std::string& op, std::wstring& rval );
	std::wstring		astows( const std::string& op );

	/** GUID <-> String conversion functions
	*/
	GUID			StringToGUID( const wchar_t* pString );
	std::wstring	GUIDToWString( const GUID& id );
	wchar_t*		GUIDToCOMString( const GUID& id );
	BSTR			GUIDToBSTR( const GUID& id );

	inline std::string convert( const std::wstring& ws )
	{	
		size_t reqLength = ::WideCharToMultiByte( CP_UTF8, 0, ws.c_str(), int(ws.length()), 0, 0, 0, 0 );

		std::string ret( reqLength, '\0' );
		::WideCharToMultiByte( CP_UTF8, 0, ws.c_str(), int(ws.length()), &ret[0], int(ret.length()), 0, 0 );

		return ret;
	}

	inline std::wstring convert( const std::string& as )
	{
		size_t reqLength = ::MultiByteToWideChar( CP_UTF8, 0, as.c_str(), int(as.length()), 0, 0 );

		std::wstring ret( reqLength, L'\0' );
		::MultiByteToWideChar( CP_UTF8, 0, as.c_str(), int(as.length()), &ret[0], int(ret.length()) );

		return ret;
	}

	/// wide string to ascii string
	inline std::string& wstoas( const std::wstring& op, std::string& rval )
	{
		return rval = convert( op );
	}

	/// multibyte string to wide string
	inline std::wstring& astows( const std::string& op, std::wstring& rval )
	{
		return rval = convert( op );
	}

	inline	std::string	wstoas( const std::wstring& op )
	{
		std::string temp;
		return wstoas( op, temp );
	}

	inline std::wstring astows( const std::string& op )
	{
		std::wstring temp;
		return astows( op, temp );
	}


	inline GUID			StringToGUID( const wchar_t* pString )
	{
		GUID id = GUID_NULL;
		CLSIDFromString( const_cast<wchar_t*>( pString ), &id );
		return id;
	}

	inline std::wstring		GUIDToWString( const GUID& id )
	{
		wchar_t* pStr = 0;
		std::wstring rval;
		StringFromCLSID( id, &pStr );
		if( pStr != 0 )
		{
			rval = pStr;
			CoTaskMemFree( pStr );
		}
		return rval;
	}


	inline wchar_t*		GUIDToCOMString( const GUID& id )
	{
		wchar_t* pRval = 0;
		StringFromCLSID( id, &pRval );
		return pRval;
	}

	inline BSTR			GUIDToBSTR( const GUID& id )
	{
		BSTR pRval = 0;
		wchar_t* pStr = GUIDToCOMString( id );
		if( pStr != 0 )
		{
			pRval = ::SysAllocString( pStr );
			CoTaskMemFree( pStr );
		}
		return pRval;
	}
};

#endif // LIBUTIL_STRING_H_INC_
