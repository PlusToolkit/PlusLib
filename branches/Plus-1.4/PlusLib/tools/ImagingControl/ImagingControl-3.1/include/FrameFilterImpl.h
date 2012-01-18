
#ifndef FRAMEFILTERIMPL_H_INC_
#define FRAMEFILTERIMPL_H_INC_

#include <comutil.h>

#include "FrameFilter.h"
#include "libutil_string.h"

namespace _DSHOWLIB_NAMESPACE
{
	template<typename T>
	struct get_vt_type
	{
		enum { res = VT_UNKNOWN };
	};

	template<>
	struct get_vt_type<long>
	{
		enum { res = VT_I4 };
	};

	template<>
	struct get_vt_type<bool>
	{
		enum { res = VT_BOOL };
	};

	template<>
	struct get_vt_type<float>
	{
		enum { res = VT_R4 };
	};

	template<>
	struct get_vt_type<_bstr_t>
	{
		enum { res = VT_BSTR };
	};

	/**
	 *	Implements most pure virtual methods of IFrameFilter_param
	 */
	template<class T>
	class FrameFilterImpl_param_ : public IFrameFilter
	{
		mutable CRITICAL_SECTION	m_csParamTransfer;

	public:

		FrameFilterImpl_param_()
			:	m_refCount( 1 )
		{
			::InitializeCriticalSection( &m_csParamTransfer );
		}

		virtual ~FrameFilterImpl_param_()
		{
			::DeleteCriticalSection( &m_csParamTransfer );
		}

		virtual void			beginParamTransfer() const
		{
			::EnterCriticalSection( &m_csParamTransfer );
		}

		virtual void			endParamTransfer() const
		{
			::LeaveCriticalSection( &m_csParamTransfer );
		}

		virtual tFilterError	getFilterData_( wchar_t** ppDataOut ) const
		{
			if( ppDataOut == 0 )		return eINVALID_PARAM_VALUE;

			std::wstring ws = astows( saveFrameFilterData( this ) );
			if( ws.empty() ) {
				return ePARAM_NOT_FOUND;
			}

			size_t cbSettings = (ws.length()+1) * sizeof( ws[0] );
			*ppDataOut = (wchar_t*) ::CoTaskMemAlloc( cbSettings  );

			wcsncpy( *ppDataOut, ws.c_str(), cbSettings/sizeof( ws[0] ) );
			(*ppDataOut)[ws.size()] = L'\0';

			return eNO_ERROR;
		}
		virtual tFilterError	setFilterData_( const wchar_t* pData )
		{
			if( pData == 0 ) {
				return eINVALID_PARAM_VALUE;
			}

			if( restoreFrameFilterData( this, wstoas( pData ) ) )
			{
				return eNO_ERROR;
			}
			return eINCOMPATIBLE_PARAM_VALUE;
		}


	protected:
		/**
		*	Call this method to register a boolean parameter.
		*	@param name Name of the parameter
		*	@param getFunc	Member function to be called when the parameter's value is retrieved
		*	@param setFunc	Member function to be called when a new value is assigned to the parameter
		*/
		void addBoolParam( const std::wstring& name, tFilterError (T::*getFunc)( bool& ), tFilterError (T::*setFunc)( bool ) )
		{
			param_info<bool>* ppi = new param_info<bool>;

			ppi->getFunc = getFunc;
			ppi->setFunc = setFunc;

			IParameterCtrl::ParamDesc desc = { L"", IParameterCtrl::eParamBoolean, L"Boolean" };
			ZeroMemory( desc.name, sizeof(desc.name) );
			wcsncpy( desc.name, name.c_str(), (sizeof(desc.name)/sizeof(desc.name[0]))-1 );
			ppi->m_Desc = desc;

			m_paramInfo[name] = ppi;
		}

		void addBoolParam( const std::string& name, tFilterError (T::*getFunc)( bool& ), tFilterError (T::*setFunc)( bool ) )
		{
			addBoolParam( astows( name ), getFunc, setFunc );
		}

		/**
		*	Call this method to register a long parameter.
		*	@param name Name of the parameter
		*	@param getFunc	Member function to be called when the parameter's value is retrieved
		*	@param setFunc	Member function to be called when a new value is assigned to the parameter
		*/
		void addLongParam( const std::wstring& name, tFilterError (T::*getFunc)( long& ), tFilterError (T::*setFunc)( long ) )
		{
			param_info<long>* ppi = new param_info<long>;

			ppi->getFunc = getFunc;
			ppi->setFunc = setFunc;

			IParameterCtrl::ParamDesc desc = { L"", IParameterCtrl::eParamLong, L"Long" };
			ZeroMemory( desc.name, sizeof(desc.name) );
			wcsncpy( desc.name, name.c_str(), (sizeof(desc.name)/sizeof(desc.name[0]))-1 );
			ppi->m_Desc = desc;

			m_paramInfo[name] = ppi;
		}

		void addLongParam( const std::string& name, tFilterError (T::*getFunc)( long& ), tFilterError (T::*setFunc)( long ) )
		{
			addLongParam( astows( name ), getFunc, setFunc );
		}

		/**
		*	Call this method to register a string parameter.
		*	@param name Name of the parameter
		*	@param getFunc	Member function to be called when the parameter's value is retrieved
		*	@param setFunc	Member function to be called when a new value is assigned to the parameter
		*/
		void addStringParam( const std::wstring& name, tFilterError (T::*getFunc)( _bstr_t& ), tFilterError (T::*setFunc)( _bstr_t ) )
		{
			param_info<_bstr_t>* ppi = new param_info<_bstr_t>;

			ppi->getFunc = getFunc;
			ppi->setFunc = setFunc;

			IParameterCtrl::ParamDesc desc = { L"", IParameterCtrl::eParamString, L"String" };
			ZeroMemory( desc.name, sizeof(desc.name) );
			wcsncpy( desc.name, name.c_str(), (sizeof(desc.name)/sizeof(desc.name[0]))-1 );
			ppi->m_Desc = desc;

			m_paramInfo[name] = ppi;
		}
		void addStringParam( const std::string& name, tFilterError (T::*getFunc)( _bstr_t& ), tFilterError (T::*setFunc)( _bstr_t ) )
		{
			addStringParam( astows( name ), getFunc, setFunc );
		}

		/**
		*	Call this method to register a float parameter.
		*	@param name Name of the parameter
		*	@param getFunc	Member function to be called when the parameter's value is retrieved
		*	@param setFunc	Member function to be called when a new value is assigned to the parameter
		*/
		void addFloatParam( const std::wstring& name, tFilterError (T::*getFunc)( float& ), tFilterError (T::*setFunc)( float ) )
		{
			param_info<float>* ppi = new param_info<float>;

			ppi->getFunc = getFunc;
			ppi->setFunc = setFunc;

			IParameterCtrl::ParamDesc desc = { L"", IParameterCtrl::eParamFloat, L"Float" };
			ZeroMemory( desc.name, sizeof(desc.name) );
			wcsncpy( desc.name, name.c_str(), (sizeof(desc.name)/sizeof(desc.name[0]))-1 );
			ppi->m_Desc = desc;

			m_paramInfo[name] = ppi;
		}
		void addFloatParam( const std::string& name, tFilterError (T::*getFunc)( float& ), tFilterError (T::*setFunc)( float ) )
		{
			addFloatParam( astows( name ), getFunc, setFunc );
		}

		/**
		*	Call this method to register a data parameter.
		*	@param name Name of the parameter
		*	@param getLenFunc	Member function to be called when the size of the available data is retrieved
		*	@param getFunc	Member function to be called when the data is read
		*	@param setFunc	Member function to be called when data is written
		*/
		void addDataParam( const std::wstring& name, tFilterError (T::*getLenFunc)( size_t& length), tFilterError (T::*getFunc)( void*, size_t& ), tFilterError (T::*setFunc)( void*, size_t ) )
		{
			data_info* pdi = new data_info;
			pdi->getLenFunc = getLenFunc;
			pdi->getFunc = getFunc;
			pdi->setFunc = setFunc;

			IParameterCtrl::ParamDesc desc = { L"", IParameterCtrl::eParamData, L"Data" };
			ZeroMemory( desc.name, sizeof(desc.name) );
			wcsncpy( desc.name, name.c_str(), (sizeof(desc.name)/sizeof(desc.name[0]))-1 );
			pdi->m_Desc = desc;

			m_dataInfo[name] = pdi;
		}
		void addDataParam( const std::string& name, tFilterError (T::*getLenFunc)( size_t& length ), tFilterError (T::*getFunc)( void*, size_t& ), tFilterError (T::*setFunc)( void*, size_t ) )
		{
			addDataParam( astows( name ), getLenFunc, getFunc, setFunc );
		}

	private:

		struct param_info_base
		{		
			virtual VARENUM getVT() const = 0;
			virtual tFilterError getValue( T* pThis, _variant_t& value ) = 0;
			virtual tFilterError setValue( T* pThis, _variant_t value ) = 0;

			IParameterCtrl::ParamDesc	m_Desc;

			bool isCompatible( _variant_t v )
			{
				_variant_t test;
				return SUCCEEDED(::VariantChangeType( &test, &v, 0, (VARTYPE)getVT() ) );
			}
		};

		template<class U>
		struct param_info : public param_info_base
		{
			tFilterError (T::*getFunc)( U& );
			tFilterError (T::*setFunc)( U );

			virtual VARENUM getVT() const
			{
				return (VARENUM)get_vt_type<U>::res;
			}

			virtual tFilterError getValue( T* pThis, _variant_t& value )
			{
				if( !isCompatible( value ) )
					return eINCOMPATIBLE_PARAM_VALUE;

				U u;
				tFilterError e = (pThis->*getFunc)( u );
				if( !e )
				{
					value = u;
				}
				return e;
			}
			virtual tFilterError setValue( T* pThis, _variant_t value )
			{
				if( !isCompatible( value ) )
					return eINCOMPATIBLE_PARAM_VALUE;

				return (pThis->*setFunc)( (U)value );
			}
		};


		typedef std::map<std::wstring,smart_ptr<param_info_base> > tParamInfoMap;
		tParamInfoMap	m_paramInfo;

		struct data_info
		{
			tFilterError (T::*getLenFunc)( size_t& );
			tFilterError (T::*getFunc)( void*, size_t& );
			tFilterError (T::*setFunc)( void*, size_t );

			IParameterCtrl::ParamDesc	m_Desc;
		};

		typedef std::map<std::wstring,smart_ptr<data_info> >	tDataInfoMap;
		tDataInfoMap	m_dataInfo;


		virtual tFilterError getParameter_( const wchar_t* name, _variant_t& value )
		{
			tParamInfoMap::iterator it = m_paramInfo.find( name );
			if( it != m_paramInfo.end() )
				return it->second->getValue( static_cast<T*>( this ), value );
			else
				return ePARAM_NOT_FOUND;
		}

		virtual tFilterError setParameter_( const wchar_t* name, const _variant_t& value )
		{
			tParamInfoMap::iterator it = m_paramInfo.find( name );
			if( it != m_paramInfo.end() )
				return it->second->setValue( static_cast<T*>( this ), value );
			else
				return ePARAM_NOT_FOUND;
		}

		virtual tFilterError	setData_( const wchar_t* name, void* pData, size_t length )
		{
			tDataInfoMap::iterator it = m_dataInfo.find( name );
			T* pThis = static_cast<T*>( this );
			if( it != m_dataInfo.end() )
				return  (pThis->*(it->second->setFunc))( pData, length );
			else
				return ePARAM_NOT_FOUND;
		}

		virtual tFilterError	getDataLength_( const wchar_t* name, size_t& length )
		{
			tDataInfoMap::iterator it = m_dataInfo.find( name );
			T* pThis = static_cast<T*>( this );
			if( it != m_dataInfo.end() )
				return (pThis->*(it->second->getLenFunc))( length );
			else
				return ePARAM_NOT_FOUND;
		}

		virtual tFilterError	getData_( const wchar_t* name, void* pData, size_t& length )
		{
			tDataInfoMap::iterator it = m_dataInfo.find( name );
			T* pThis = static_cast<T*>( this );
			if( it != m_dataInfo.end() )
				return  (pThis->*(it->second->getFunc))( pData, length );
			else
				return ePARAM_NOT_FOUND;
		}

		virtual	bool	enumParameters_( unsigned int n, IParameterCtrl::ParamDesc* pPD ) const
		{
			assert( pPD );

			if( pPD == 0 )
				return false;
			if( n >= m_paramInfo.size() )
			{
				size_t m = m_paramInfo.size() - n;

				if( m >= m_dataInfo.size() )
					return false;

				tDataInfoMap::const_iterator it = m_dataInfo.begin(); 
				std::advance( it, m );

				*pPD = it->second->m_Desc;

				return true;
			}

			tParamInfoMap::const_iterator it = m_paramInfo.begin(); 
			std::advance( it, n );

			*pPD = it->second->m_Desc;

			return true;
		}


	public:
		virtual ULONG STDMETHODCALLTYPE AddRef()
		{
			return ::InterlockedIncrement( &m_refCount );
		}
		virtual ULONG STDMETHODCALLTYPE Release()
		{
			ULONG rval = ::InterlockedDecrement( &m_refCount );
			if( rval == 0 )
			{
				delete this;
			}
			return rval;
		}
		virtual STDMETHODIMP	QueryInterface( REFIID riid, void** ppvObject )
		{
			if( riid == IID_IParameterCtrl ) {
				*ppvObject = (IParameterCtrl*) this;
				return S_OK;
			}
			return IFrameFilter::QueryInterface( riid, ppvObject );
		}
	protected:
		virtual	void	getFilterInfo( FilterInfo* pInfo ) const
		{
			assert( pInfo != 0 );
			*pInfo = T::getStaticFilterInfo();

			if( !m_moduleName.empty() )
			{
				wcsncpy( pInfo->module, m_moduleName.c_str(), sizeof(pInfo->module)/sizeof(pInfo->module[0]) );
			}
		}
		virtual void	setFilterModule( const wchar_t* module )
		{
			m_moduleName = module;
		}

	private:
		long			m_refCount;
		std::wstring	m_moduleName;
	};


	/**
	 *	To create an own frame transform filter that can be loaded from a DLL,
	 *	derive from this class.
	 *	You have to implement the pure virtual methods of <link>IFrameTransformCB</link>.
	 *
	 *	To register a parameter, use <link>addBoolParam</link>, <link>addStringParam</link>,
	 *	<link>addLongParam</link> or <link>addFloatParam</link> or <link>addDataParam</link>.
	 *
	 */
	template<class TDerived>
	class FrameFilterImpl	: public FrameFilterImpl_param_<TDerived>
	{
	public:
	};
	template<class TDerived>
	class FrameUpdateFilterImpl	: public IFrameUpdateFilterWrapper< FrameFilterImpl_param_<TDerived> >
	{
	public:
	};
}

#endif
