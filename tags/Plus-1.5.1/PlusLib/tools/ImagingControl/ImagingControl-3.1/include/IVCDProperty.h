// VCDPropertyBase.h: interface for the CVCDPropertyBase class.
// 
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VCDPROPERTYBASE_H__E608E93D_D18F_46F4_8FB8_FB1D28240E79__INCLUDED_)
#define AFX_VCDPROPERTYBASE_H__E608E93D_D18F_46F4_8FB8_FB1D28240E79__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <vector>
#include <functional>
#include <map>

#include "libbasedefs.h"
#include "smart_com.h"
#include "libutil_string.h"

#undef DEF_INTERFACE_GUID
#ifdef _MSC_VER
#define DEF_INTERFACE_GUID(id) __declspec(uuid(id))
#else
#define DEF_INTERFACE_GUID(id)
#endif

#ifndef OATRUE
#define OATRUE	(-1)
#endif
#ifndef OAFALSE
#define OAFALSE (0)
#endif

namespace _DSHOWLIB_NAMESPACE
{
	// {99B44939-BFE1-4083-ADA1-BE703F4B8E00}
	static const GUID IID_IBSTRCollection =		{ 0x99B44939, 0xBFE1, 0x4083, { 0xAD, 0xA1, 0xBE, 0x70, 0x3F, 0x4B, 0x8E, 0x00 } };
	// {99B44939-BFE1-4083-ADA1-BE703F4B8E01}
	static const GUID IID_IVCDCategoryMap =		{ 0x99B44939, 0xBFE1, 0x4083, { 0xAD, 0xA1, 0xBE, 0x70, 0x3F, 0x4B, 0x8E, 0x01 } };

	// {99B44938-BFE1-4083-ADA1-BE703F4B8E00}
	static const GUID IID_IVCDPropertyItems =	{ 0x99B44938, 0xBFE1, 0x4083, { 0xAD, 0xA1, 0xBE, 0x70, 0x3F, 0x4B, 0x8E, 0x00 } };
	// {99B44938-BFE1-4083-ADA1-BE703F4B8E01}
	static const GUID IID_IVCDPropertyItem =	{ 0x99B44938, 0xBFE1, 0x4083, { 0xAD, 0xA1, 0xBE, 0x70, 0x3F, 0x4B, 0x8E, 0x01 } };
	// {99B44938-BFE1-4083-ADA1-BE703F4B8E02}
	static const GUID IID_IVCDPropertyElements = { 0x99B44938, 0xBFE1, 0x4083, { 0xAD, 0xA1, 0xBE, 0x70, 0x3F, 0x4B, 0x8E, 0x02 } };
	// {99B44938-BFE1-4083-ADA1-BE703F4B8E03}
	static const GUID IID_IVCDPropertyElement = { 0x99B44938, 0xBFE1, 0x4083, { 0xAD, 0xA1, 0xBE, 0x70, 0x3F, 0x4B, 0x8E, 0x03 } };
	// {99B44938-BFE1-4083-ADA1-BE703F4B8E04}
//	static const GUID IID_IVCDPropertyInterfaces = { 0x99B44938, 0xBFE1, 0x4083, { 0xAD, 0xA1, 0xBE, 0x70, 0x3F, 0x4B, 0x8E, 0x04 } };
	// {99B44938-BFE1-4083-ADA1-BE703F4B8E05}
	static const GUID IID_IVCDPropertyInterface = { 0x99B44938, 0xBFE1, 0x4083, { 0xAD, 0xA1, 0xBE, 0x70, 0x3F, 0x4B, 0x8E, 0x05 } };

	struct DEF_INTERFACE_GUID("99B44939-BFE1-4083-ADA1-BE703F4B8E00") IBSTRCollection;
	struct DEF_INTERFACE_GUID("99B44939-BFE1-4083-ADA1-BE703F4B8E01") IVCDCategoryMap;

	struct DEF_INTERFACE_GUID("99B44938-BFE1-4083-ADA1-BE703F4B8E00") IVCDPropertyItems;
	struct DEF_INTERFACE_GUID("99B44938-BFE1-4083-ADA1-BE703F4B8E01") IVCDPropertyItem;
	struct DEF_INTERFACE_GUID("99B44938-BFE1-4083-ADA1-BE703F4B8E02") IVCDPropertyElements;
	struct DEF_INTERFACE_GUID("99B44938-BFE1-4083-ADA1-BE703F4B8E03") IVCDPropertyElement;
	struct DEF_INTERFACE_GUID("99B44938-BFE1-4083-ADA1-BE703F4B8E04") IVCDPropertyInterfaces;
	struct DEF_INTERFACE_GUID("99B44938-BFE1-4083-ADA1-BE703F4B8E05") IVCDPropertyInterface;


	struct IVCDPropertyInterface : public IDispatch
	{
	public:
		/* convenience methods for QueryInterface */
		using IDispatch::QueryInterface;

#if !defined _MSC_VER || _MSC_VER >= 1300
		template<class TItf>
		inline HRESULT QueryInterface( REFIID iid, smart_com<TItf>& rval )
		{
			rval = 0;
			return QueryInterface( iid, reinterpret_cast<void**>( &rval.getImpl() ) );
		}
		template<class TItf>
		inline smart_com<TItf>& QueryInterface( smart_com<TItf>& rval )
		{
			rval = 0;
			HRESULT hr = QueryInterface( __uuidof( TItf ), reinterpret_cast<void**>( &rval.getImpl() ) );
			if( FAILED( hr ) )	rval = 0;

			return rval;
		}
#else
		template<class TItf>
		TItf								QueryInterface( TItf& pItf )
		{
			pItf = 0;
			if( SUCCEEDED( QueryInterface( TItf::getIID(), (void**)&pItf.get() ) ) )
			{
				// success
			}
			else
			{
				pItf = 0;
			}
			return pItf;
		}
#endif

		STDMETHOD(get_InterfaceID)( BSTR* pItfID ) const = 0;
		STDMETHOD(get_InterfaceGUID)( GUID* pItfID ) const = 0;
		STDMETHOD(get_Parent)( IVCDPropertyElement** pParentItem ) const = 0;

		STDMETHOD(get_Available)( VARIANT_BOOL* pVal ) const = 0;
		STDMETHOD(get_ReadOnly)( VARIANT_BOOL* pVal ) const = 0;

		STDMETHOD(Update)() = 0;
	public:
		bool			getAvailable() const;
		bool			getReadOnly() const;
		smart_com<IVCDPropertyElement>	getParent() const;

		GUID			getInterfaceID() const;


	};

	typedef smart_com<IVCDPropertyInterface>		tIVCDPropertyInterfacePtr;


	typedef std::vector< tIVCDPropertyInterfacePtr > tVCDPropertyInterfaceArray;

	struct IVCDPropertyElement : public IDispatch
	{
	public:	// this class is a collection of IVCDPropertyInterface objects
		STDMETHOD(_NewEnum)( IUnknown** ppEnum ) = 0;
		STDMETHOD(get_Item)( long n, IVCDPropertyInterface** ppItem ) = 0;
		STDMETHOD(get_Count)( long* pnCount ) = 0;

	public:
		STDMETHOD(get_Name)( BSTR* pName ) const = 0;

		STDMETHOD(get_ElementID)( BSTR* pItfID ) const = 0;
		STDMETHOD(get_ElementGUID)( GUID* pItfID ) const = 0;

		STDMETHOD(FindInterface)( BSTR id, IVCDPropertyInterface** pItf ) const = 0;
		STDMETHOD(get_Parent)( IVCDPropertyItem** pParentItem ) const = 0;

		STDMETHOD(Update)() = 0;
	public:

		tVCDPropertyInterfaceArray			getInterfaces();
		tVCDPropertyInterfaceArray			getInterfaces() const
		{
			return const_cast<IVCDPropertyElement* const>( this )->getInterfaces();
		}
		smart_com<IVCDPropertyInterface>	getInterface( const GUID& id = GUID_NULL );

#if !defined _MSC_VER || _MSC_VER >= 1300
		template<class TItf>
		smart_com<TItf>						getInterfacePtr( smart_com<TItf>& pRval )
		{
			pRval = 0;

			smart_com<IVCDPropertyInterface> pItf = getInterface( __uuidof( TItf ) );
			if( pItf != 0 )
			{
				pItf->QueryInterface( pRval );
			}
			return pRval;
		}
#else
		template<class TItfPtr>
		TItfPtr								getInterfacePtr( TItfPtr& pItf )
		{
			pItf = 0;
			tIVCDPropertyInterfacePtr pTmp = getInterface( TItfPtr::getIID() );
			if( pTmp != 0 && SUCCEEDED( pTmp->QueryInterface( &pItf.get() ) ) )
			{
				// success
			}
			else
			{
				pItf = 0;
			}
			return pItf;
		}
#endif

		std::wstring	getNameW() const;
		std::string		getName() const;

		GUID			getElementID() const;

		smart_com<IVCDPropertyItem>	getParent() const;
	};

	typedef smart_com<IVCDPropertyElement>			tIVCDPropertyElementPtr;


	typedef std::vector< tIVCDPropertyElementPtr >	tVCDPropertyElementArray;

	struct IVCDPropertyItem : public IDispatch
	{
	public:
		STDMETHOD(get_Name)( BSTR* pName ) const = 0;

		STDMETHOD(get_Elements)( IVCDPropertyElements** pVCDCol ) const = 0;

		STDMETHOD(get_ItemID)( BSTR* pItfID ) const = 0;
		STDMETHOD(get_ItemGUID)( GUID* pItfID ) const = 0;

		STDMETHOD(Update)() = 0;

		STDMETHOD(Save)( BSTR* pXmlStr ) const = 0;
		STDMETHOD(Load)( BSTR dataStr ) = 0;
	public:		

		tVCDPropertyElementArray		getElements() const;

		smart_com<IVCDPropertyElement>	findElement( const GUID& elemGUID ) const;

		std::wstring					getNameW() const;
		std::string						getName() const;
									
		std::wstring					saveW() const;
		void							loadW( const std::wstring& data );
		std::string						save() const;
		void							load( const std::string& data );
									
		GUID							getItemID() const;
	};

	typedef smart_com<IVCDPropertyItem>				tIVCDPropertyItemPtr;

	
	typedef std::vector<tIVCDPropertyItemPtr>	tVCDPropertyItemArray;

	typedef std::vector<GUID>					tGUIDArray;
	typedef std::map<std::string, tGUIDArray>	tVCDCategoryMap;
	typedef std::map<std::wstring, tGUIDArray>	tVCDCategoryMapW;

	/** a collection of IVCDPropertyItem items.
	 * This object behaves like any other COM collection.
	 */
	struct IVCDPropertyItems : public IDispatch
	{
	public:
		STDMETHOD(_NewEnum)( IUnknown** ppEnum ) = 0;
		STDMETHOD(get_Item)( long n, IVCDPropertyItem** ppItem ) = 0;
		STDMETHOD(get_Count)( long* pnCount ) = 0;

		STDMETHOD(Add)( IVCDPropertyItem* pItem ) = 0;

		STDMETHOD(Update)() = 0;

		STDMETHOD(Save)( BSTR* SavedData ) const = 0;
		STDMETHOD(Load)( BSTR DataToLoadFrom ) = 0;

		STDMETHOD(get_Supported)( BSTR ItemName, VARIANT_BOOL* pVal ) const = 0;

		STDMETHOD(FindItem)( BSTR ItemID, IVCDPropertyItem** pVal ) const = 0;
		STDMETHOD(FindElement)( BSTR ElementPath, IVCDPropertyElement** pVal ) const = 0;
		STDMETHOD(FindInterface)( BSTR InterfacePath, IVCDPropertyInterface** pVal ) const = 0;

		STDMETHOD(get_CategoryMap)( IVCDCategoryMap** ppMap ) const = 0;
	public:		
		tVCDPropertyItemArray				getItems();

		smart_com<IVCDPropertyItem>			findItem( const GUID& itemID ) const;
		smart_com<IVCDPropertyElement>		findElement( const GUID& itemID, const wchar_t* pElementName ) const;
		smart_com<IVCDPropertyElement>		findElement( const GUID& itemID, const GUID& elementID ) const;
		smart_com<IVCDPropertyInterface>	findInterface( const GUID& itemID, const wchar_t* pElementName, 
															const GUID& itfTypeID = GUID_NULL ) const;
		smart_com<IVCDPropertyInterface>	findInterface( const GUID& itemID, const GUID& elementID, 
															const GUID& itfTypeID = GUID_NULL ) const;

#if !defined _MSC_VER || _MSC_VER >= 1300
		template<class TItf>
			smart_com<TItf>	findInterfacePtr( const GUID& itemID, const GUID& elementID, smart_com<TItf>& pRvalItf ) const
		{
			pRvalItf = 0;
			smart_com<IVCDPropertyInterface> pItf = 
				findInterface( itemID, elementID, smart_com<TItf>::getIID() );
			if( pItf != 0 )
			{
				pItf->QueryInterface( pRvalItf );
			}
			return pRvalItf;
		}
#else
		template<class TItfPtr>
		TItfPtr			findInterfacePtr( const GUID& itemID, const GUID& elementID, TItfPtr& pRvalItf ) const
		{
			pRvalItf = 0;
			smart_com<IVCDPropertyInterface> pItf = 
				findInterface( itemID, elementID, TItfPtr::getIID() );
			if( pItf != 0 )
			{
				HRESULT hr = pItf->QueryInterface( &pRvalItf.get() );
				if( FAILED( hr ) )
				{
					return pRvalItf = 0;
				}
			}
			return pRvalItf;
		}
#endif

		tVCDCategoryMap					getCategoryMap();
		tVCDCategoryMapW				getCategoryMapW();

		std::wstring					saveW() const;
		void							loadW( const std::wstring& s );
		std::string						save() const;
		void							load( const std::string& s );
	};

	struct IVCDPropertyElements : public IDispatch
	{
	public:
		STDMETHOD(_NewEnum)( IUnknown** ppEnum ) = 0;
		STDMETHOD(get_Item)( long n, IVCDPropertyElement** ppItem ) = 0;
		STDMETHOD(get_Count)( long* pnCount ) = 0;
	
		STDMETHOD(FindElement)( BSTR ElementName, IVCDPropertyElement** ppElem ) const = 0;
		STDMETHOD(FindInterface)( BSTR InterfacePath, IVCDPropertyInterface** pVal ) const = 0;

		smart_com<IVCDPropertyElement>		findElement( const wchar_t* pElementName ) const;
		smart_com<IVCDPropertyElement>		findElement( const GUID& elementID ) const;
		smart_com<IVCDPropertyInterface>	findInterface( const wchar_t* pElementName, const GUID& itfTypeID = GUID_NULL ) const;
	};

	/** a collection of BSTRs.
	 * used only in Visual Basic.
	 *
	 * From C++ you should not use the enumerator interface.
	 */
	struct IBSTRCollection : public IDispatch
	{
	public:
		STDMETHOD(_NewEnum)( IUnknown** ppEnum ) = 0;
		STDMETHOD(get_Item)( long n, BSTR* ppItem ) = 0;
		STDMETHOD(get_Count)( long* pnCount ) = 0;
		STDMETHOD(FindItem)( BSTR Item, long* pIndex ) = 0;
	};

	struct IVCDCategoryMap : public IDispatch
	{
	public:
		STDMETHOD(get_Categories)( IBSTRCollection** pCollection ) const = 0;
		STDMETHOD(get_ItemsInCategory)( BSTR Category, IBSTRCollection** pItemIDCollection ) const = 0;
	};

	typedef smart_com<IVCDPropertyItems>			tIVCDPropertyItemsPtr;
};

// this must be placed here, because this file needs the types defined above
#include "IVCDProperty_inl.h"

#endif // !defined(AFX_VCDPROPERTYBASE_H__E608E93D_D18F_46F4_8FB8_FB1D28240E79__INCLUDED_)
