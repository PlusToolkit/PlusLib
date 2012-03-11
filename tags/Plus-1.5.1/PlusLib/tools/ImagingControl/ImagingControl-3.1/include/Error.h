// Error.h: interface for the Error class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ERROR_H__6EB4029E_C2A2_4353_8938_FF398FE42826__INCLUDED_)
#define AFX_ERROR_H__6EB4029E_C2A2_4353_8938_FF398FE42826__INCLUDED_

#pragma once

#include <string>
#include "udshl_defs.h"
#include "simplectypes.h"

#include "int_interface_pre.h"

namespace _DSHOWLIB_NAMESPACE
{
	/** The Error class gives information about an error occurred during a call to the class library.
	 *
	 * Example usage :
	 * <br><code>
		Grabber g;
		// ...
		if( !g.startLive() )
		{
			Error e = g.getLastError();
			if( e.getVal() == eDEVICE_INVALID )
			{
				MessageBox( "Device has become invalid.", ... );
				// reopen device
			}
			else
			{
				MessageBox( e.toString(), ... );
				exit( -1 );
			}
		}
	   </code>
	 **/
	class _UDSHL_EXP_API Error 
	{
	public:
		/** Constructs an unknown Error **/
		Error();

		/** Constructs an unknown error with given message
		 * @param errordesc description of the error
		 **/
		Error( const std::string& errordesc );
		Error( const std::wstring& errordesc );

		/** constructs an error with given error code and uses the string from the resources
		 * @param e error code
		 **/
		Error( tErrorEnum e );

        /** constructs an error with given exception from DShowLib
         * @param e DShowLibException to handle
         **/
        Error( const icbase::IDShowError& e );

		/** constructs an error as a copy of the given one
		 * @param e Error to copy
		 **/
		Error( const Error& e );

		/** dtor */
		virtual ~Error();

		/** return a string representation for the error
		 * @return a string representation for the error
		 **/
		std::string		getString() const;
		std::wstring	getStringW() const;

		std::string		toString() const;
		std::wstring	toStringW() const;

		/** return a const char* to a string representing the error
		 * @return a const char* to a string representation of the error
		 **/
		const char*	c_str() const;

		/** return the error code of the error
		 * @return the error code of the error
		 **/
		tErrorEnum	getVal() const;

		/** return a string representation for the error
		 * @return a string representation for the error
		 **/
		operator std::string  () const;

		/** return a const char* to a string representing the error
		 * @return a const char* to a string representation of the error
		 **/
		operator const char*  () const;

		/** return the error code of the error
		 * @return the error code of the error
		 **/
		operator tErrorEnum  () const;

		/** return true, if the internal error value equals not eNOERROR
		 * @return true, if the internal error value equals not eNOERROR
		 **/
		bool	isError		() const;
	private:
		void		loadString(); // to load a String from resources
		long		getStringID( tErrorEnum error ) const;
    private: // members
		tErrorEnum	m_Enum;
		std::string	m_String;
	};
}

#endif // !defined(AFX_ERROR_H__6EB4029E_C2A2_4353_8938_FF398FE42826__INCLUDED_)
