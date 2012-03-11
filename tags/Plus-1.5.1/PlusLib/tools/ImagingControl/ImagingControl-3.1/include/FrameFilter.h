
#ifndef FRAMEFILTER_H_INC_
#define FRAMEFILTER_H_INC_

#include <comutil.h>
#include <Shlwapi.h>

#include "FrameFilterBase.h"
#include "libutil_string.h"

namespace _DSHOWLIB_NAMESPACE
{

	enum tFilterClass
	{
		eFC_GENERIC		=	0x00000001,
		eFC_INTERNAL	=	0x00000002,
		eFC_ALL			=	0xFFFFFFFF,
	};

	/** Description data of a frame filter.
	 */
	struct FilterInfo
	{
		wchar_t			name[30];			///< The name of the filter
		wchar_t			module[MAX_PATH];	///< The complete path to the filter module dll.
		tFilterClass	filterClass;		///< Description of the frame filter class. 

		std::string		getFilterName() const		{ return wstoas( name ); }
		std::wstring	getFilterNameW() const		{ return name; }

		/** Returns file name and path of the filter module. 
		 */
		std::string		getModulePath() const		{ return wstoas( module ); }
		std::wstring	getModulePathW() const		{ return module; }

		/** Returns file name of the filter module. 
		 * This is not the fully qualified path.
		 */
		std::string		getModuleName() const		{ return wstoas( getModuleNameW() ); }
		std::wstring	getModuleNameW() const		{ return ::PathFindFileNameW( module ); }

		tFilterClass	getFilterClass() const		{ return filterClass; }

		bool operator<( const FilterInfo& op2 ) const;
	};
	
	// 5EA71600-B1EF-430E-9672-D16F231C8A9B
	static const GUID IID_IParameterCtrl = { 0x5EA71600, 0xB1EF, 0x430E, { 0x96, 0x72, 0xD1, 0x6F, 0x23, 0x1C, 0x8A, 0x9B } };

	enum tFilterError
	{
		eNO_ERROR = 0,
		ePARAM_NOT_FOUND,
		eINCOMPATIBLE_PARAM_VALUE,
		eINVALID_PARAM_VALUE,
		eINVALID_FILTER_STATE,
	};

	class IParameterCtrl;

	/** Internally used method to serialize the data of a filter into an xml string. */
	_UDSHL_EXP_API	std::string saveFrameFilterData( const IParameterCtrl* pParams );
	/** Internally used method to restor the data to a filter from an xml string. */
	_UDSHL_EXP_API	bool		restoreFrameFilterData( IParameterCtrl* pParams, const std::string& data );

	/**
	 *	This abstract class provides methods to access the parameters of a
	 *	frame transform filter or frame update filter.
	 *
	 * exported functions :
		 bool			setParameter( const string& name, const _variant_t& value );
		 bool			getParameter( const string& name, string& value );
		 bool			getParameter( const string& name, long& value );
		 bool			getParameter( const string& name, bool& value );
		 bool			getParameter( const string& name, float& value );
		 bool			setData( const string& name, void* pData, size_t length );
		 bool			getData( const string& name, void* pData, size_t length );
		 size_t			getDataLength( const string& name );
		 tParamArray	getAvailableParameters() const;

		 bool			hasDialog() const;
		 bool			callDialog( HWND hparent );
	 */
	class IParameterCtrl
	{
	public:
		virtual ~IParameterCtrl()	{}

		/**
		 *	Sets a parameter to a new value.
		 *	@param name Name of the parameter
		 *	@param value New value
		 *	@return tFilterError describing a possible error
		 */
		tFilterError	setParameter( const std::wstring& name, const _variant_t& value )
		{
			return setParameter_( name.c_str(), value );
		}
		tFilterError	setParameter( const std::wstring& name, const std::wstring& value )
		{
			return setParameter_( name.c_str(), value.c_str() );
		}
		tFilterError	setParameter( const std::wstring& name, bool value )
		{
			return setParameter_( name.c_str(), _variant_t( value ) );
		}
		tFilterError	setParameter( const std::wstring& name, long value )
		{
			return setParameter_( name.c_str(), _variant_t( value ) );
		}
		tFilterError	setParameter( const std::wstring& name, float value )
		{
			return setParameter_( name.c_str(), _variant_t( value ) );
		}
		tFilterError	setParameter( const std::wstring& name, int value )
		{
			return setParameter_( name.c_str(), _variant_t( long(value) ) );
		}
		tFilterError	setParameter( const std::string& name, const _variant_t& value )
		{
			return setParameter( astows( name ), value );
		}
		tFilterError	setParameter( const std::string& name, const std::string& value )
		{
			return setParameter( astows( name ), astows( value ) );
		}
		tFilterError	setParameter( const std::string& name, bool value )
		{
			return setParameter( astows( name ), value );
		}
		tFilterError	setParameter( const std::string& name, long value )
		{
			return setParameter( astows( name ), value );
		}
		tFilterError	setParameter( const std::string& name, float value )
		{
			return setParameter( astows( name ), value );
		}
		tFilterError	setParameter( const std::string& name, int value )
		{
			return setParameter( astows( name ), value );
		}

		/**
		 *	Retrieves the current value of a parameter.
		 *	@param name Name of the parameter
		 *	@param value Current value of the parameter
		 *	@return tFilterError describing a possible error
		 */
		tFilterError	getParameter( const std::wstring& name, std::wstring& value )
		{
			_variant_t v = L"";
			tFilterError e = getParameter_( name.c_str(), v );
			if( !e )
			{
				value = (_bstr_t)v;
			}
			return e;
		}
		tFilterError	getParameter( const std::string& name, std::string& value )
		{
			std::wstring tmp;
			tFilterError e = getParameter( astows( name ), tmp );
			if( !e )
			{
				value = wstoas( tmp );
			}
			return e;
		}

		tFilterError	getParameter( const std::wstring& name, bool& value )
		{
			return getParameterT( name, value );
		}
		tFilterError	getParameter( const std::string& name, bool& value )
		{
			return getParameter( astows( name ), value );
		}

		tFilterError	getParameter( const std::wstring& name, long& value )
		{
			return getParameterT( name, value );
		}
		tFilterError	getParameter( const std::string& name, long& value )
		{
			return getParameter( astows( name ), value );
		}
		tFilterError	getParameter( const std::wstring& name, int& value )
		{
			return getParameterT( name, (long&)value );
		}
		tFilterError	getParameter( const std::string& name, int& value )
		{
			return getParameter( astows( name ), (long&)value );
		}

		tFilterError	getParameter( const std::wstring& name, float& value )
		{
			return getParameterT( name, value );
		}
		tFilterError	getParameter( const std::string& name, float& value )
		{
			return getParameter( astows( name ), value );
		}

		/**
		 *	Sends a pointer to a blob of raw data to the filter.
		 *	@param name Parameter name
		 *	@param pData Pointer to the data
		 *	@param length Length of the data
		 *	@return tFilterError describing a possible error
		 */
		tFilterError	setData( const std::wstring& name, void* pData, size_t length )
		{
			return setData_( name.c_str(), pData, length );
		}
		tFilterError	setData( const std::string& name, void* pData, size_t length )
		{
			return setData( astows( name ), pData, length );
		}

		/**
		 *	Retrieves the size of a data blob that can be retrieved from the filter.
		 *
		 *	Call this function before using <link>getData</link>, and create a buffer of
		 *	appropriate size.
		 *
		 *	@param name Parameter name
		 *	@param length The length of the buffer that can be used in <link>getData</link>
		 *	@return tFilterError describing a possible error
		 */
		tFilterError	getDataLength( const std::wstring& name, size_t& length )
		{
			return getDataLength_( name.c_str(), length );
		}
		tFilterError	getDataLength( const std::string& name, size_t& length )
		{
			return getDataLength( astows( name ), length );
		}

		/**
		 *	Retrieves a blob of raw data from the filter.
		 *	The filter fills the buffer that has to be provided by the caller.
		 *	@param name Parameter name
		 *	@param pData Pointer to the buffer
		 *	@param length Length of the buffer
		 *	@return tFilterError describing a possible error
		 */
		tFilterError	getData( const std::wstring& name, void* pData, size_t& length )
		{
			return getData_( name.c_str(), pData, length );
		}
		tFilterError	getData( const std::string& name, void* pData, size_t& length )
		{
			return getData( astows( name ), pData, length );
		}

		/**
		 *	Describes the type of a parameter.
		 */
		enum tParamType
		{
			eParamLong,
			eParamBoolean,
			eParamFloat,
			eParamString,
			eParamData
		};

		/**
		 *	This structure describes a parameter
		 */
		struct ParamDesc
		{
			wchar_t		name[30];
			tParamType	type;
			wchar_t		typeDesc[30];

			std::string	getTypeDesc() const						{ return wstoas( typeDesc ); }
			std::string getName() const							{ return wstoas( name ); }
		};

		typedef std::vector<ParamDesc> tParamArray;

		/**
		 *	Retrieves a list with the descriptions of the parameters of this filter.
		 *	@return A vector containing <link>ParamDesc</link> structures.
		 */
		tParamArray		getAvailableParameters() const
		{
			tParamArray rval;

			int i = 0;
			ParamDesc pd;
			while( enumParameters_( i++, &pd ) )
			{
				rval.push_back( pd );
			}

			return rval;
		}
		virtual tFilterError	getParameter_( const wchar_t* name, _variant_t& value ) = 0;

	public:
		virtual void		beginParamTransfer() const = 0;
		virtual void		endParamTransfer() const = 0;

		virtual bool		hasDialog() const					{ return false; }
		virtual bool		callDialog( HWND hparent )			{ return false; }

		std::string			getSettings() const				{ return wstoas( getSettingsW() ); }
		std::wstring		getSettingsW() const
		{
			std::wstring rval;
			wchar_t* pDataOut = 0;
			if( getFilterData_( &pDataOut ) == eNO_ERROR )
			{
				rval = pDataOut;
				::CoTaskMemFree( pDataOut );
			}
			return rval;
		}

		bool				setSettings( const std::string& data )	{ return setSettings( astows( data ) ); }
		bool				setSettings( const std::wstring& data )	{ return setFilterData_( data.c_str() ) == eNO_ERROR; }
	protected:
		virtual	bool			enumParameters_( unsigned int n, ParamDesc* pPD ) const = 0;
		virtual tFilterError	setParameter_( const wchar_t* name, const _variant_t& value ) = 0;

		virtual tFilterError	setData_( const wchar_t* name, void* pData, size_t length ) = 0;
		virtual tFilterError	getDataLength_( const wchar_t* name, size_t& length ) = 0;
		virtual tFilterError	getData_( const wchar_t* name, void* pData, size_t& length ) = 0;

		virtual tFilterError	getFilterData_( wchar_t** ppDataOut ) const = 0;
		virtual tFilterError	setFilterData_( const wchar_t* pData ) = 0;
	private:
		template<class T>
		tFilterError	getParameterT( const std::wstring& name, T& value )
		{
			_variant_t v = T();
			tFilterError e = getParameter_( name.c_str(), v );
			if( !e )
			{
				value = (T)v;
			}
			return e;
		}
	};

	/**
	 *	Frame transform filters loaded from DLLs are derived from this class.
	 *	They allow the manipulation of parameters and can be used as source transformation
	 *	or <link>FrameHandlerSink</link> copy operation.
	 */
	class IFrameFilter
		:	public IFrameFilterBase,
			public IParameterCtrl
	{
	public:
		FilterInfo		getFilterInfo() const
		{
			FilterInfo info;
			getFilterInfo( &info );
			return info;
		}

	public:
		// Called by the filter loader to let the filter know the dll file it came from.
		// Do not use in any other case.
		virtual void	setFilterModule( const wchar_t* module ) = 0;
	protected:
		virtual	void	getFilterInfo( FilterInfo* pInfo ) const = 0;
	};

	inline 		bool FilterInfo::operator<( const FilterInfo& op2 ) const
	{
		std::wstring module1 = std::wstring( module );
		std::wstring module2 = std::wstring( op2.module );

		if( module1 != module2 )
		{
			return module1 < module2;
		}
		else
		{
			return std::wstring( name ) < std::wstring( op2.name );
		}
	}

}

#endif
