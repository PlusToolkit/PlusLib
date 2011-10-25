
#ifndef IVCDPROPERTY_INL_H_INC_
#define IVCDPROPERTY_INL_H_INC_

#include <cassert>

namespace _DSHOWLIB_NAMESPACE
{	
	inline tVCDPropertyItemArray	IVCDPropertyItems::getItems()
	{
		tVCDPropertyItemArray rval;
		HRESULT hr;

		long count = 0;
		hr = get_Count( &count );
		if( FAILED( hr ) )		throw hr;

		rval.resize( count );
		for( long l = 0; l < count; ++l )
		{
			get_Item( l + 1, &rval[l].get() );
		}
		return rval;
	}

	inline tVCDCategoryMap IVCDPropertyItems::getCategoryMap()
	{
		tVCDCategoryMap rval;

		smart_com<IVCDCategoryMap> pMap;
		HRESULT hr = get_CategoryMap( &pMap.get() );
		if( FAILED(hr) ) return rval;

		smart_com<IBSTRCollection> pCategories;
		hr = pMap->get_Categories( &pCategories.get() );
		if( FAILED(hr) ) return rval;

		long catCount = 0;
		hr = pCategories->get_Count( &catCount );
		if( FAILED(hr) ) return rval;

		for( long n = 1; n <= catCount; ++n )
		{
			BSTR catName = 0;
			hr = pCategories->get_Item( n, &catName );
			if( FAILED(hr) ) return rval;

			std::string catStr = wstoas( catName );
			::SysFreeString( catName );

			smart_com<IBSTRCollection> pGUIDs;
			hr = pMap->get_ItemsInCategory( catName, &pGUIDs.get() );
			if( FAILED(hr) ) return rval;

			long guidCount = 0;
			hr = pGUIDs->get_Count( &guidCount );
			if( FAILED(hr) ) return rval;

			for( int i = 1; i <= guidCount; ++i )
			{
				BSTR itemGUID = 0;
				hr = pGUIDs->get_Item( i, &itemGUID );
				if( FAILED(hr) ) return rval;

				GUID guid = StringToGUID( itemGUID );

				rval[catStr].push_back( guid );
			}
		}

		return rval;		
	}

	inline tVCDCategoryMapW IVCDPropertyItems::getCategoryMapW()
	{
		tVCDCategoryMapW rval;

		smart_com<IVCDCategoryMap> pMap;
		HRESULT hr = get_CategoryMap( &pMap.get() );
		if( FAILED(hr) ) return rval;

		smart_com<IBSTRCollection> pCategories;
		hr = pMap->get_Categories( &pCategories.get() );
		if( FAILED(hr) ) return rval;

		long catCount = 0;
		hr = pCategories->get_Count( &catCount );
		if( FAILED(hr) ) return rval;

		for( long n = 1; n <= catCount; ++n )
		{
			BSTR catName = 0;
			hr = pCategories->get_Item( n, &catName );
			if( FAILED(hr) ) return rval;

			std::wstring catWStr = catName;
			::SysFreeString( catName );

			smart_com<IBSTRCollection> pGUIDs;
			hr = pMap->get_ItemsInCategory( catName, &pGUIDs.get() );
			if( FAILED(hr) ) return rval;

			long guidCount = 0;
			hr = pGUIDs->get_Count( &guidCount );
			if( FAILED(hr) ) return rval;

			for( int i = 1; i <= guidCount; ++i )
			{
				BSTR itemGUID = 0;
				hr = pGUIDs->get_Item( i, &itemGUID );
				if( FAILED(hr) ) return rval;

				GUID guid = StringToGUID( itemGUID );

				rval[catWStr].push_back( guid );
			}
		}

		return rval;		
	}


	inline smart_com<IVCDPropertyItem>	IVCDPropertyItems::findItem( const GUID& itemID ) const
	{
		smart_com<IVCDPropertyItem> pRval = 0;
		BSTR itemIDStr = GUIDToBSTR( itemID );
		HRESULT hr = FindItem( itemIDStr, &pRval.get() );
		::SysFreeString( itemIDStr );
		if( hr != S_OK )
		{
			return 0;
		}
		return pRval;
	}
	inline smart_com<IVCDPropertyElement>		IVCDPropertyItems::findElement( const GUID& itemID, const wchar_t* pElementName ) const
	{
		assert( pElementName != 0 );
		if( pElementName == 0 )		return 0;

		smart_com<IVCDPropertyElement> pRval;
		std::wstring elementPath( GUIDToWString( itemID ) );
		assert( !elementPath.empty() );
		if( elementPath.empty() )	return 0;

		elementPath += L":";
		elementPath += pElementName;

		BSTR elemPathArg = SysAllocString( elementPath.c_str() );
		if( elemPathArg == 0 )		return 0;

		HRESULT hr = FindElement( elemPathArg, &pRval.get() );
		::SysFreeString( elemPathArg );
		if( hr != S_OK )
		{
			return 0;
		}
		return pRval;
	}

	inline smart_com<IVCDPropertyElement>		IVCDPropertyItems::findElement( const GUID& itemID, const GUID& elementID ) const
	{
		smart_com<IVCDPropertyElement> pRval;
		std::wstring elementPath( GUIDToWString( itemID ) );
		assert( !elementPath.empty() );
		if( elementPath.empty() )	return 0;

		elementPath += L":";
		elementPath += GUIDToWString( elementID );

		BSTR elemPathArg = SysAllocString( elementPath.c_str() );
		if( elemPathArg == 0 )		return 0;

		HRESULT hr = FindElement( elemPathArg, &pRval.get() );
		::SysFreeString( elemPathArg );
		if( hr != S_OK )
		{
			return 0;
		}
		return pRval;
	}

	inline smart_com<IVCDPropertyInterface>		IVCDPropertyItems::findInterface( const GUID& itemID, 
		const wchar_t* pElementName, const GUID& itfTypeID ) const
	{
		if( pElementName == 0 )		return 0;

		smart_com<IVCDPropertyInterface> pRval;
		std::wstring elementPath( GUIDToWString( itemID ) );
		elementPath += L":";
		elementPath += pElementName;
		if( itfTypeID != GUID_NULL )
		{
			elementPath += L":";
			elementPath += GUIDToWString( itfTypeID );
		}

		BSTR elemPathArg = SysAllocString( elementPath.c_str() );
		if( elemPathArg == 0 )		return 0;

		HRESULT hr = FindInterface( elemPathArg, &pRval.get() );
		::SysFreeString( elemPathArg );
		if( hr != S_OK )
		{
			return 0;
		}
		return pRval;
	}

	inline smart_com<IVCDPropertyInterface>		IVCDPropertyItems::findInterface( const GUID& itemID, 
		const GUID& elementID, const GUID& itfTypeID ) const
	{
		smart_com<IVCDPropertyInterface> pRval;
		std::wstring elementPath( GUIDToWString( itemID ) );
		elementPath += L":";
		elementPath += GUIDToWString( elementID );
		if( itfTypeID != GUID_NULL )
		{
			elementPath += L":";
			elementPath += GUIDToWString( itfTypeID );
		}

		BSTR elemPathArg = SysAllocString( elementPath.c_str() );
		if( elemPathArg == 0 )		return 0;

		HRESULT hr = FindInterface( elemPathArg, &pRval.get() );
		::SysFreeString( elemPathArg );
		if( hr != S_OK )
		{
			return 0;
		}
		return pRval;
	}

	inline std::wstring		IVCDPropertyItems::saveW() const
	{
		std::wstring rval;
		
		BSTR str;
		HRESULT hr = Save( &str );
		if( FAILED( hr ) )
		{
			return rval;
		}
		rval = str;
		::SysFreeString( str );
		return rval;
	}
	
	inline void				IVCDPropertyItems::loadW( const std::wstring& s )
	{
		BSTR str = ::SysAllocString( s.c_str() );
		Load( str );
		::SysFreeString( str );
	}

	inline std::string		IVCDPropertyItems::save() const
	{
		return wstoas( saveW() );
	}

	inline void				IVCDPropertyItems::load( const std::string& s )
	{
		loadW( astows( s ) );
	}

	inline smart_com<IVCDPropertyElement>	IVCDPropertyElements::findElement( const wchar_t* pElementName ) const
	{
		assert( pElementName != 0 );
		if( pElementName == 0 )		return 0;

		smart_com<IVCDPropertyElement> pRval;

		std::wstring elementPath( pElementName );
		assert( !elementPath.empty() );
		if( elementPath.empty() )	return 0;

		BSTR elemPathArg = SysAllocString( elementPath.c_str() );
		if( elemPathArg == 0 )		return 0;

		HRESULT hr = FindElement( elemPathArg, &pRval.get() );
		::SysFreeString( elemPathArg );
		if( hr != S_OK )
		{
			return 0;
		}
		return pRval;
	}

	inline smart_com<IVCDPropertyElement>	IVCDPropertyElements::findElement( const GUID& elementID ) const
	{
		smart_com<IVCDPropertyElement> pRval;

		BSTR elemPathArg = GUIDToBSTR( elementID );
		if( elemPathArg == 0 )		return 0;

		HRESULT hr = FindElement( elemPathArg, &pRval.get() );
		::SysFreeString( elemPathArg );
		if( hr != S_OK )
		{
			return 0;
		}
		return pRval;
	}

	inline smart_com<IVCDPropertyInterface>	IVCDPropertyElements::findInterface( const wchar_t* pElementName, 
		const GUID& itfTypeID ) const
	{
		if( pElementName == 0 )		return 0;

		smart_com<IVCDPropertyInterface> pRval;
		std::wstring elementPath;
		elementPath += pElementName;
		if( itfTypeID != GUID_NULL )
		{
			elementPath += L":";
			elementPath += GUIDToCOMString( itfTypeID );
		}

		BSTR elemPathArg = ::SysAllocString( elementPath.c_str() );
		if( elemPathArg == 0 )		return 0;

		HRESULT hr = FindInterface( elemPathArg, &pRval.get() );
		::SysFreeString( elemPathArg );
		if( hr != S_OK )
		{
			return 0;
		}
		return pRval;
	}



	inline std::wstring				IVCDPropertyItem::getNameW() const
	{
		BSTR name;
		std::wstring rval;
		HRESULT hr = get_Name( &name );
		if( FAILED( hr ) ) throw hr;
		rval = name;
		SysFreeString( name );
		return rval;
	}

	inline std::string				IVCDPropertyItem::getName() const
	{
		return wstoas( getNameW() );
	}

	inline tVCDPropertyElementArray	IVCDPropertyItem::getElements() const
	{
		tVCDPropertyElementArray rval;
		smart_com<IVCDPropertyElements> pElems;

		HRESULT hr;

		hr = get_Elements( &pElems.get() );
		if( FAILED( hr ) )		throw hr;

		long count = 0;
		hr = pElems->get_Count( &count );
		if( FAILED( hr ) )		throw hr;

		rval.resize( count );
		for( long l = 0; l < count; ++l )
		{
			pElems->get_Item( l + 1, &rval[l].get() );
		}
		return rval;
	}

	inline smart_com<IVCDPropertyElement> IVCDPropertyItem::findElement( const GUID& elemGUID ) const
	{
		tVCDPropertyElementArray elements = getElements();
		for( tVCDPropertyElementArray::iterator i = elements.begin(); i != elements.end(); ++i )
		{
			if( (*i)->getElementID() == elemGUID )
				return smart_com<IVCDPropertyElement>( i->get() );
		}

		return 0;
	}

	inline std::wstring	IVCDPropertyItem::saveW() const
	{
		std::wstring rval;
		
		BSTR str;
		HRESULT hr = Save( &str );
		if( FAILED( hr ) )
		{
			return rval;
		}
		rval = str;
		::SysFreeString( str );
		return rval;
	}

	inline void			IVCDPropertyItem::loadW( const std::wstring& data )
	{
		BSTR val = ::SysAllocString( data.c_str() );
		if( val == 0 )		return;

		Load( val );

		::SysFreeString( val );
	}
	
	inline std::string	IVCDPropertyItem::save() const
	{
		return wstoas( saveW() );
	}

	inline void			IVCDPropertyItem::load( const std::string& data )
	{
		loadW( astows( data ) );
	}

	inline GUID			IVCDPropertyItem::getItemID() const
	{
		GUID id;
		get_ItemGUID( &id );
		return id;
	}

	inline tVCDPropertyInterfaceArray	IVCDPropertyElement::getInterfaces()
	{
		tVCDPropertyInterfaceArray rval;
		long count = 0;
		HRESULT hr;

		hr = get_Count( &count );
		if( FAILED( hr ) )		throw hr;

		rval.resize( count );
		for( long l = 0; l < count; ++l )
		{
			get_Item( l + 1, &rval[l].get() );
		}
		return rval;
	}

	inline smart_com<IVCDPropertyInterface> IVCDPropertyElement::getInterface( const GUID& id )
	{
		BSTR itfId = GUIDToBSTR( id );
		if( itfId == 0 )		return 0;

		smart_com<IVCDPropertyInterface> pRval;
		HRESULT hr = FindInterface( itfId, &pRval.get() );
		::SysFreeString( itfId );
		if( hr != S_OK )
		{
			return 0;
		}
		return pRval;
	}
	
	inline std::wstring				IVCDPropertyElement::getNameW() const
	{
		BSTR name;
		std::wstring rval;
		HRESULT hr = get_Name( &name );
		if( FAILED( hr ) ) throw hr;
		rval = name;
		SysFreeString( name );
		return rval;
	}

	inline std::string				IVCDPropertyElement::getName() const
	{
		return wstoas( getNameW() );
	}

	inline GUID						IVCDPropertyElement::getElementID() const
	{
		GUID id;
		HRESULT hr = get_ElementGUID( &id );
		UNREFERENCED_PARAMETER( hr );
		assert( SUCCEEDED( hr ) );
		return id;
	}

	inline smart_com<IVCDPropertyItem>	IVCDPropertyElement::getParent() const
	{
		smart_com<IVCDPropertyItem> rval;
		HRESULT hr = get_Parent( &rval.get() );
		if( FAILED(hr) ) throw hr;

		return rval;
	}


	inline bool			IVCDPropertyInterface::getAvailable() const
	{
		VARIANT_BOOL Val;
		HRESULT hr = get_Available( &Val );
		if( FAILED( hr ) )	return false;

		return Val == OATRUE ? true : false;
	}

	inline bool			IVCDPropertyInterface::getReadOnly() const
	{
		VARIANT_BOOL Val;
		HRESULT hr = get_ReadOnly( &Val );
		if( FAILED( hr ) )	return false;

		return Val == OATRUE ? true : false;
	}

	inline smart_com<IVCDPropertyElement>	IVCDPropertyInterface::getParent() const
	{
		smart_com<IVCDPropertyElement> rval;
		HRESULT hr = get_Parent( &rval.get() );
		if( FAILED(hr) ) throw hr;

		return rval;
	}

	inline GUID			IVCDPropertyInterface::getInterfaceID() const
	{
		GUID rval;
		get_InterfaceGUID( &rval );
		return rval;
	}

};

#endif // IVCDPROPERTY_INL_H_INC_