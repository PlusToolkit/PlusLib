
#ifndef IVCDPROPERTYINTERFACES_H_INC_
#define IVCDPROPERTYINTERFACES_H_INC_

#include "IVCDProperty.h"

namespace _DSHOWLIB_NAMESPACE
{
	// {99B44940-BFE1-4083-ADA1-BE703F4B8E03}
	static const GUID IID_IVCDRangeProperty	=	{ 0x99B44940, 0xBFE1, 0x4083, { 0xAD, 0xA1, 0xBE, 0x70, 0x3F, 0x4B, 0x8E, 0x03 } };
	// {99B44940-BFE1-4083-ADA1-BE703F4B8E04}
	static const GUID IID_IVCDSwitchProperty =	{ 0x99B44940, 0xBFE1, 0x4083, { 0xAD, 0xA1, 0xBE, 0x70, 0x3F, 0x4B, 0x8E, 0x04 } };
	// {99B44940-BFE1-4083-ADA1-BE703F4B8E05}
	static const GUID IID_IVCDButtonProperty =	{ 0x99B44940, 0xBFE1, 0x4083, { 0xAD, 0xA1, 0xBE, 0x70, 0x3F, 0x4B, 0x8E, 0x05 } };
	// {99B44940-BFE1-4083-ADA1-BE703F4B8E06}
	static const GUID IID_IVCDMapStringsProperty =	{ 0x99B44940, 0xBFE1, 0x4083, { 0xAD, 0xA1, 0xBE, 0x70, 0x3F, 0x4B, 0x8E, 0x06 } };
	// {99B44940-BFE1-4083-ADA1-BE703F4B8E08}
	static const GUID IID_IVCDAbsoluteValueProperty =	{ 0x99B44940, 0xBFE1, 0x4083, { 0xAD, 0xA1, 0xBE, 0x70, 0x3F, 0x4B, 0x8E, 0x08 } };

	struct DEF_INTERFACE_GUID("99B44940-BFE1-4083-ADA1-BE703F4B8E03") IVCDRangeProperty;
	struct DEF_INTERFACE_GUID("99B44940-BFE1-4083-ADA1-BE703F4B8E04") IVCDSwitchProperty;
	struct DEF_INTERFACE_GUID("99B44940-BFE1-4083-ADA1-BE703F4B8E05") IVCDButtonProperty;
	struct DEF_INTERFACE_GUID("99B44940-BFE1-4083-ADA1-BE703F4B8E06") IVCDMapStringsProperty;
	struct DEF_INTERFACE_GUID("99B44940-BFE1-4083-ADA1-BE703F4B8E08") IVCDAbsoluteValueProperty;

	struct IVCDRangeProperty : public IVCDPropertyInterface
	{
	public:
		STDMETHOD(get_Value)( long* pVal ) const = 0;
		STDMETHOD(put_Value)( long Val ) = 0;
		STDMETHOD(get_RangeMin)( long* pMin ) const = 0;
		STDMETHOD(get_RangeMax)( long* pMax ) const = 0;
		STDMETHOD(get_Default)( long* pDef ) const = 0;
		STDMETHOD(get_Delta)( long* pDelta ) const = 0;

	public: // C++ methods
		std::pair<long,long>	getRange() const;
		long					getRangeMin() const;
		long					getRangeMax() const;

		long					getValue() const;
		void					setValue( long val );

		long					getDefault() const;
		long					getDelta() const;
	};
	
	struct IVCDSwitchProperty : public IVCDPropertyInterface
	{
	public:
		STDMETHOD(get_Switch)( VARIANT_BOOL* pVal ) const = 0;
		STDMETHOD(put_Switch)( VARIANT_BOOL Val ) = 0;

		void					setSwitch( bool b );
		bool					getSwitch();
	};

	struct IVCDButtonProperty : public IVCDPropertyInterface
	{
	public:
		STDMETHOD(Push)()= 0;

		void					push();
	};

	struct IVCDMapStringsProperty : public IVCDRangeProperty
	{
	public:
		STDMETHOD(get_Strings)( IBSTRCollection** pStringMap ) const = 0;
		STDMETHOD(get_String)( BSTR* pString ) const = 0;
		STDMETHOD(put_String)( BSTR String ) = 0;

	public:	// C++ interface
		typedef std::vector<std::wstring> tStringVecW;
		typedef std::vector<std::string> tStringVec;

		tStringVecW&	getStringsW( tStringVecW& vec ) const;
		tStringVec		getStrings() const;
		tStringVec&		getStrings( tStringVec& vec ) const;

		void			setString( const std::wstring& str );
		void			setString( const std::string& str );
		std::wstring	getStringW() const;
		std::string		getString() const;
	};

	struct IVCDAbsoluteValueProperty : public IVCDPropertyInterface
	{
	public:
		STDMETHOD(get_Value)( double* pVal ) const = 0;
		STDMETHOD(put_Value)( double Val ) = 0;
		STDMETHOD(get_RangeMin)( double* pMin ) const = 0;
		STDMETHOD(get_RangeMax)( double* pMax ) const = 0;
		STDMETHOD(get_Default)( double* pDef ) const = 0;
		STDMETHOD(get_DimType)( BSTR* pDimType ) const = 0;
		STDMETHOD(get_DimFunction)( long* pDimFunction ) const = 0;
	public: // C++ methods
		enum tAbsDimFunction
		{
			eLinear = 0x1,
			eLog	= 0x2,
			eOther	= 0x3,	// >= eOther are other types of functions, currently not defined (use eLinear)
		};

		std::pair<double,double>	getRange() const;
		double						getRangeMin() const;
		double						getRangeMax() const;

		double						getValue() const;
		void						setValue( double val );
		double						getDefault() const;
		
		std::wstring				getDimTypeW() const;
		std::string					getDimTypeA() const;
		std::string					getDimType() const;

		tAbsDimFunction				getDimFunction() const;
	};

	
	typedef smart_com<IVCDRangeProperty>			tIVCDRangePropertyPtr;
	typedef smart_com<IVCDSwitchProperty>			tIVCDSwitchPropertyPtr;
	typedef smart_com<IVCDButtonProperty>			tIVCDButtonPropertyPtr;
	typedef smart_com<IVCDMapStringsProperty>		tIVCDMapStringsPropertyPtr;
	typedef smart_com<IVCDAbsoluteValueProperty>	tIVCDAbsoluteValuePropertyPtr;
};

#include "IVCDPropertyInterfaces_inl.h"

#endif // IVCDPROPERTYINTERFACES_H_INC_