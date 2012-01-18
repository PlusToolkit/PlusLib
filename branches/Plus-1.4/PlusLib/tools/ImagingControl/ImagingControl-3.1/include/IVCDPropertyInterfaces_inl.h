
#ifndef IVCDPROPERTYINTERFACES_INL_H_INC_
#define IVCDPROPERTYINTERFACES_INL_H_INC_

namespace _DSHOWLIB_NAMESPACE
{
	// methods of IVCDRangeProperty
	inline std::pair<long,long>	IVCDRangeProperty::getRange() const
	{
		HRESULT hr;
		std::pair<long,long>	rval;
		hr = get_RangeMin( &rval.first );
		if( FAILED( hr ) )	throw hr;
		hr = get_RangeMax( &rval.second );
		if( FAILED( hr ) )	throw hr;
		return rval;
	}
	inline long					IVCDRangeProperty::getRangeMin() const
	{
		long rval;
		HRESULT hr;
		hr = get_RangeMin( &rval );
		if( FAILED( hr ) )	throw hr;
		return rval;
	}
	inline long					IVCDRangeProperty::getRangeMax() const
	{
		long rval;
		HRESULT hr;
		hr = get_RangeMax( &rval );
		if( FAILED( hr ) )	throw hr;
		return rval;
	}
	inline long					IVCDRangeProperty::getValue() const
	{
		long rval;
		HRESULT hr = get_Value( &rval );
		if( FAILED( hr ) )	throw hr;
		return rval;
	}
	inline void					IVCDRangeProperty::setValue( long val )
	{
		HRESULT hr = put_Value( val );
		if( FAILED( hr ) )	throw hr;
	}

	inline long					IVCDRangeProperty::getDefault() const
	{
		long rval;
		HRESULT hr = get_Default( &rval );
		if( FAILED( hr ) )	throw hr;
		return rval;
	}
	inline long					IVCDRangeProperty::getDelta() const
	{
		long rval;
		HRESULT hr = get_Delta( &rval );
		if( FAILED( hr ) )	throw hr;
		return rval;
	}


	// methods of IVCDSwitchProperty
	inline void		IVCDSwitchProperty::setSwitch( bool b )
	{
		HRESULT hr = put_Switch( b ? OATRUE : OAFALSE );
		if( FAILED( hr ) )	throw hr;
	}
	inline bool		IVCDSwitchProperty::getSwitch()
	{
		VARIANT_BOOL rval;
		HRESULT hr = get_Switch( &rval );
		if( FAILED( hr ) )	throw hr;
		return rval == OATRUE ? true : false;
	}

	// methods of IVCDButtonProperty
	inline	void	IVCDButtonProperty::push()
	{
		HRESULT hr = Push();
		if( FAILED( hr ) )	throw hr;
	}

	// methods of IVCDMapStringsProperty
	inline IVCDMapStringsProperty::tStringVecW&	IVCDMapStringsProperty::getStringsW( 
		IVCDMapStringsProperty::tStringVecW& vec ) const
	{
		smart_com<IBSTRCollection> pEnum;
		HRESULT hr = get_Strings( &pEnum.get() );
		if( FAILED( hr ) )	throw hr;
		
		long count = 0;
		hr = pEnum->get_Count( &count );
		if( FAILED( hr ) )	throw hr;

		for( int i = 0; i < count; ++i )
		{
			BSTR pItem = 0;
			hr = pEnum->get_Item( i + 1, &pItem );
			if( FAILED( hr ) )	throw hr;

			vec.push_back( pItem );

			SysFreeString( pItem );
		}
		return vec;
	}
	inline IVCDMapStringsProperty::tStringVec	IVCDMapStringsProperty::getStrings() const
	{
		tStringVec vec;
		getStrings( vec );
		return vec;
	}
	inline IVCDMapStringsProperty::tStringVec&	IVCDMapStringsProperty::getStrings( 
		IVCDMapStringsProperty::tStringVec& vec ) const
	{
		smart_com<IBSTRCollection> pEnum;
		HRESULT hr = get_Strings( &pEnum.get() );
		if( FAILED( hr ) )	throw hr;
		
		long count = 0;
		hr = pEnum->get_Count( &count );
		if( FAILED( hr ) )	throw hr;

		for( int i = 0; i < count; ++i )
		{
			BSTR pItem = 0;
			hr = pEnum->get_Item( i + 1, &pItem );
			if( FAILED( hr ) )	throw hr;

			vec.push_back( wstoas( pItem ) );

			SysFreeString( pItem );
		}
		return vec;
	}

	inline void	IVCDMapStringsProperty::setString( const std::wstring& str )
	{
		BSTR pStr = SysAllocString( str.c_str() );
		HRESULT hr = put_String( pStr );
		SysFreeString( pStr );
		if( FAILED( hr ) )	throw hr;
	}
	inline void	IVCDMapStringsProperty::setString( const std::string& str )
	{
		setString( astows( str ) );
	}
	inline std::wstring	IVCDMapStringsProperty::getStringW() const
	{
		BSTR item;
		HRESULT hr = get_String( &item );
		if( FAILED( hr ) )	throw hr;
		std::wstring rval = item;
		::SysFreeString( item );
		return rval;
	}
	inline std::string		IVCDMapStringsProperty::getString() const
	{
		return wstoas( getStringW() );
	}

	// inline methods of IVCDAbsoluteValueProperty
	inline std::pair<double,double>	IVCDAbsoluteValueProperty::getRange() const
	{
		HRESULT hr;
		std::pair<double,double>	rval;
		hr = get_RangeMin( &rval.first );
		if( FAILED( hr ) )	throw hr;
		hr = get_RangeMax( &rval.second );
		if( FAILED( hr ) )	throw hr;
		return rval;
	}
	inline double					IVCDAbsoluteValueProperty::getRangeMin() const
	{
		double rval;
		HRESULT hr = get_RangeMin( &rval );
		if( FAILED( hr ) )	throw hr;
		return rval;
	}
	inline double					IVCDAbsoluteValueProperty::getRangeMax() const
	{
		double rval;
		HRESULT hr = get_RangeMax( &rval );
		if( FAILED( hr ) )	throw hr;
		return rval;
	}
	inline double					IVCDAbsoluteValueProperty::getValue() const
	{
		double rval;
		HRESULT hr = get_Value( &rval );
		if( FAILED( hr ) )	throw hr;
		return rval;
	}
	inline void					IVCDAbsoluteValueProperty::setValue( double val )
	{
		HRESULT hr = put_Value( val );
		if( FAILED( hr ) )	throw hr;
	}

	inline double					IVCDAbsoluteValueProperty::getDefault() const
	{
		double rval;
		HRESULT hr = get_Default( &rval );
		if( FAILED( hr ) )	throw hr;
		return rval;
	}

	inline std::wstring			IVCDAbsoluteValueProperty::getDimTypeW() const
	{
		BSTR name;
		std::wstring rval;
		HRESULT hr = get_DimType( &name );
		if( FAILED( hr ) ) throw hr;
		rval = name;
		SysFreeString( name );
		return rval;
	}
	inline std::string			IVCDAbsoluteValueProperty::getDimTypeA() const
	{
		std::string rval;
		std::wstring wstr = getDimTypeW();

		return wstoas( wstr, rval );
	}
	inline std::string			IVCDAbsoluteValueProperty::getDimType() const
	{
		return getDimTypeA();
	}

	inline IVCDAbsoluteValueProperty::tAbsDimFunction				IVCDAbsoluteValueProperty::getDimFunction() const
	{
		long l;
		get_DimFunction( &l );	// should never fail
		return (IVCDAbsoluteValueProperty::tAbsDimFunction) l;
	}
};

#endif // IVCDPROPERTYINTERFACES_INL_H_INC_