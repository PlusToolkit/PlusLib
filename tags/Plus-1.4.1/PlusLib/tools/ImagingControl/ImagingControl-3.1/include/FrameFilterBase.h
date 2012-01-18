
#ifndef FRAMEFILTERBASE_H_INC_
#define FRAMEFILTERBASE_H_INC_

#pragma once

#include <string>
#include "udshl_defs.h"
#include "smart_com.h"

#include "FrameType.h"

namespace _DSHOWLIB_NAMESPACE
{
	class IFrameFilterBase;
	class IFrameFilter;

	typedef std::vector<IFrameFilter*> tFrameFilterList;

	/** Function to create a filter chain form a tFrameFilterList vector.
	 * @return 0 when the list is empty, otherwise a frame filter containing a chain of frame filters.
	 */
	_UDSHL_EXP_API	smart_com<IFrameFilter>		createChain( const tFrameFilterList& list );
	/** Function which creates a compact copy of a FrameTypeInfoArray, by comparing the FrameTypeInfo objects
	 * and eliminating the superfluous entries.
	 */
	FrameTypeInfoArray		compactFrameTypeInfoArray( const FrameTypeInfoArray& arr );


	/** Internally used base class. Implemented inline from the FrameFilter derived classes. */
	class IFrameFilterBase_ : public IUnknown
	{
	public:
		virtual HRESULT	_transform( const IFrame* pSrc, IFrame* pDest ) = 0;

		virtual HRESULT	_getTransformOutputTypes( const FrameTypeInfo* pInType, FrameTypeInfo** ppTypeArr, long* pCount ) const = 0;
		virtual HRESULT _checkInput( const FrameTypeInfo* pTypeToTest ) const = 0;
		virtual HRESULT	_getSupportedInputTypes( FrameTypeInfo** ppTypeArr, long* pCount ) const = 0;
		virtual HRESULT _checkTransform( const FrameTypeInfo* pInType, const FrameTypeInfo* pOutType ) const = 0;

		virtual HRESULT	_notifyStart( const FrameTypeInfo* pInType, const FrameTypeInfo* pOutType )	= 0;
		virtual HRESULT	_notifyStop()																	= 0;

		struct tFrameFilterDesc {
			DWORD	size;
			DWORD	data_blob;
			bool	bUpdateAvailable;
		};
		virtual HRESULT	_fetchDesc( tFrameFilterDesc* pDesc ) = 0;
	};

	/************************************************************************
	 * Process of frame type negotiation :
	 *	1) getSupportedInputTypes	This list is used as a starting point so here only partially defined types should
	 *									be used.
	 *	2) checkInputType			Tests this specific input type, if the handler accepts it.
	 *	3) getTransformOutputTypes	This list is then used to find a partial defined output type
	 *									as defined by the user.
	 *	4) isSupportedTransform		Determines if a combination of input and output type
	 *									can be 'transformed' by the filter.
	 *
	 *	Now either all methods returned a valid input/output frame type pair is found or the
	 *		connection process failed.
	 *	You don't have to implement checkInputType and isSupportedTransform when the standard implementation
	 *		can match the types on its own, but if do have certain combinations of width/height you don't
	 *		accept, you should override the corresponding check function.
	 ************************************************************************/

	/** You have to derive from this class to implement a transform filter.
	 * There are several methods you have to implement, to get a valid transform filter.
	 *
	 * As implementor you have to implement at least the following methods :
		virtual void	getSupportedInputTypes( FrameTypeInfoArray& arr ) const;
		virtual bool	transform( const IFrame& src, IFrame& dest );
		virtual bool	getTransformOutputTypes( const FrameTypeInfo& in_type, FrameTypeInfoArray& out_types ) const;
	 *
	 * The following methods are already implemented, but may be overridden to specify different behavior :
		virtual void	notifyStart( const FrameTypeInfo& in_type, const FrameTypeInfo& out_type );
		virtual void	notifyStop();
		virtual	bool	checkTransformTypes( const FrameTypeInfo& in_type, const FrameTypeInfo& out_type ) const;
		virtual bool	checkInputType( const FrameTypeInfo& inpType ) const;
	 */
	class IFrameFilterBase : public IFrameFilterBase_
	{
	public:
		virtual ~IFrameFilterBase()	{}
		/** Called by the handler when a frame must be transformed or copied from the source to the dest frame.
		 * @param src	The source frame. You should not write to this frame.
		 * @param dest	The dest frame. You should only write to the dest frame when you also return true, because
		 *					otherwise the same dest frame as in the call before may be presented to the method again
		 *					without passing it on.
		 * @return true when the frame should be delivered downstream, false otherwise.
		 */
		virtual bool	transform( const IFrame& src, IFrame& dest ) = 0;

		/** Called by the sink to request the types, which this transform CB supports as input types.
		 * @param arr	The array of frame types, to which the transform CB should add its supported types.
		 */
		virtual void	getSupportedInputTypes( FrameTypeInfoArray& arr ) const = 0;
		/** Called by the framework when it tries to connect the update CB.
		 * In this method you decide which media types to accept as input types.
		 *
		 * The standard implementation uses the list returned by getSupportedInputTypes to
		 * check if inpType is accepted.
		 *
		 * @param inpType	Partial or fully qualified frame type.
		 * @return true when inpType is a acceptable frame type for this CB,
		 *			false otherwise
		 */
		virtual bool	checkInputType( const FrameTypeInfo& inpType ) const;

		/** This method is called by the handler when it wants to request the possible output types.
		 * This may happen in the FrameHandlerSink, when the destination type of the MemBufferCollection is 
		 * evaluated.
		 * The types should be ordered according to preference of the transform operation.
		 * The output type of this transform filter will be chosen based on the capabilities and input preference
		 * of the next filter and this filter.
		 *
		 * @param in_type	The type of the frames passed in the input buffers.
		 * @param out_types A collection of frame types this FrameTransformCB may generate.
		 * @return true when this method creates a valid type, otherwise false (e.g. input type is not supported).
		 */
		virtual bool	getTransformOutputTypes( const FrameTypeInfo& in_type, FrameTypeInfoArray& out_types ) const = 0;

		/** Called by the container to determine, if this conversion is supported by the CB.
		 * This implementation just skims through the output types and tries to match anyone to the out_type
		 * tested.
		 * @param in_type		The input type of this frame transform CB.
		 * @param out_type		The output type of this frame transform CB.
		 * @return true/false
		 */
		virtual	bool	checkTransformTypes( const FrameTypeInfo& in_type, const FrameTypeInfo& out_type ) const;
		/** This method is called when the CB is about to start the transformation process.
		 * You can allocate buffers, etc in this method.
		 * @param in_type		The input type of the following frame transform operation.
		 * @param out_type		The output type of the following frame transform operation.
		 */
		virtual void	notifyStart( const FrameTypeInfo& in_type, const FrameTypeInfo& out_type )	{}
		/** This method is called when the transformation process is stopped. */
		virtual void	notifyStop()																	{}

	public:
		// this section contains the needed implementations for the base class which needs to be
		// more complex
		virtual HRESULT					_fetchDesc( tFrameFilterDesc* pDesc );
		virtual STDMETHODIMP			QueryInterface( REFIID riid, void** ppvObject );
		virtual ULONG STDMETHODCALLTYPE AddRef()		{ return 1; }
		virtual ULONG STDMETHODCALLTYPE Release()		{ return 1; }
	protected:
		virtual HRESULT	_transform( const IFrame* pSrc, IFrame* pDest );
		virtual HRESULT	_getSupportedInputTypes( FrameTypeInfo** ppTypeArr, long* pCount ) const;

		virtual HRESULT _checkInput( const FrameTypeInfo* pTypeToTest ) const;
		virtual HRESULT	_notifyStart( const FrameTypeInfo* pInType, const FrameTypeInfo* pOutType );
		virtual HRESULT	_notifyStop();

		virtual HRESULT _checkTransform( const FrameTypeInfo* pInType, const FrameTypeInfo* pOutType ) const;
		virtual HRESULT	_getTransformOutputTypes( const FrameTypeInfo* pInType, FrameTypeInfo** ppTypeArr, long* pCount ) const;
	};

	/** You have to derive from this class to implement a transform filter.
	 * There are several methods you have to implement, to get a valid transform filter.
	 *
	 * As implementor you have to implement at least the following methods :
		virtual void	getSupportedInputTypes( FrameTypeInfoArray& arr ) const;
		virtual bool	transform( const IFrame& src, IFrame& dest );
		virtual bool	getTransformOutputTypes( const FrameTypeInfo& in_type, FrameTypeInfoArray& out_types ) const;
	 *
	 * The following methods are already implemented, but may be overridden to specify different behavior :
		virtual void	notifyStart( const FrameTypeInfo& in_type, const FrameTypeInfo& out_type );
		virtual void	notifyStop();
		virtual	bool	checkTransformTypes( const FrameTypeInfo& in_type, const FrameTypeInfo& out_type ) const;
		virtual bool	checkInputType( const FrameTypeInfo& inpType ) const;
	 */
	template<class TBase = _DSHOWLIB_NAMESPACE::IFrameFilterBase>
	class IFrameUpdateFilterWrapper :  public TBase
	{
	public:
		/** Called by the sink to request the types, which this transform CB supports as input types.
		 * @param arr	The array of frame types, to which the transform CB should add its supported types.
		 */
		virtual void	getSupportedInputTypes( FrameTypeInfoArray& arr ) const = 0;
		/** Called by the framework when it tries to connect the update CB.
		 * In this method you decide which media types to accept as input types.
		 *
		 * The standard implementation uses the list returned by getSupportedInputTypes to
		 * check if inpType is accepted.
		 *
		 * @param inpType	Partial or fully qualified frame type.
		 * @return true when inpType is a acceptable frame type for this CB,
		 *			false otherwise
		 */
		virtual bool	checkInputType( const FrameTypeInfo& inpType ) const
		{
			return TBase::checkInputType( inpType );
		}

		virtual bool	updateInPlace( IFrame& frame ) = 0;

		/** This method is called when the CB is about to start the transformation process.
		 * You can allocate buffers, etc in this method.
		 * @param in_type		The input type of the following frame transform operation.
		 * @param out_type		The output type of the following frame transform operation.
		 */
		virtual void	notifyStart( const FrameTypeInfo& in_type )				{}
		/** This method is called when the transformation process is stopped. */
		virtual void	notifyStop()												{}

		virtual bool	modifiesData() const	{ return true; }
	protected:
		virtual void	notifyStart( const FrameTypeInfo& in_type, const FrameTypeInfo& out_type )
		{
			notifyStart( in_type );
		}

		virtual bool	transform( const IFrame& src, IFrame& dest )
		{
			bool rval = true;
			if( src.getPtr() == 0 ) {	// update mode
                rval = updateInPlace( dest );
			}
			else
			{
				if( modifiesData() ) {
					if( !dest.getPtr() ) {	// no buffer to change ...
						return false;
					}
					memcpy( dest.getPtr(), src.getPtr(), src.getFrameType().buffersize );
					rval = updateInPlace( dest );
				}
				else
				{
					// the filter did promise us that it won't alter the data of the frame
					// so we cast away the const tag
					rval = updateInPlace( const_cast<IFrame&>( src ) );
					if( rval && dest.getPtr() != 0 )
					{
						memcpy( dest.getPtr(), src.getPtr(), src.getFrameType().buffersize );
					}
				}
			}
			return rval;
		}
		virtual bool	getTransformOutputTypes( const FrameTypeInfo& in_type, FrameTypeInfoArray& out_types ) const
		{
			out_types.push_back( in_type );
			return true;
		}

		/** Called by the container to determine, if this conversion is supported by the CB.
		 * This implementation just skims through the output types and tries to match anyone to the out_type
		 * tested.
		 * @param in_type		The input type of this frame transform CB.
		 * @param out_type		The output type of this frame transform CB.
		 * @return true/false
		 */
		virtual	bool	checkTransformTypes( const FrameTypeInfo& in_type, const FrameTypeInfo& out_type ) const
		{
			if( in_type == out_type || isPartiallyEqual( in_type, out_type ) ) {
				return true;
			}
			return false;
		}
		typedef typename TBase::tFrameFilterDesc tFrameFilterDesc;

		virtual HRESULT	_fetchDesc( tFrameFilterDesc* pDesc )
		{
			if( pDesc == 0 )		return E_POINTER;

			pDesc->bUpdateAvailable = true;

			return S_OK;
		}
	};

	inline bool	IFrameFilterBase::checkInputType( const FrameTypeInfo& inpType ) const
	{
		FrameTypeInfoArray arr;
		getSupportedInputTypes( arr );
		for( FrameTypeInfoArray::iterator i = arr.begin(); i != arr.end(); ++i ) {
			if( isPartiallyEqual( inpType, *i ) ) {
				return true;
			}
		}
		return false;
	}

	inline bool	IFrameFilterBase::checkTransformTypes( const FrameTypeInfo& in_type, const FrameTypeInfo& out_type ) const
	{
		FrameTypeInfoArray arr;
		if( getTransformOutputTypes( in_type, arr ) )
		{
			for( FrameTypeInfoArray::iterator i = arr.begin(); i != arr.end(); ++i ) {
				if( isPartiallyEqual( *i, out_type ) ) {
					return true;
				}
			}
		}
		return false;
	}

	inline HRESULT			IFrameFilterBase::_fetchDesc( tFrameFilterDesc* pDesc )
	{
		if( pDesc == 0 )		return E_POINTER;

		pDesc->bUpdateAvailable = false;

		return S_OK;
	}
	inline STDMETHODIMP	IFrameFilterBase::QueryInterface( REFIID riid, void** ppvObject )
	{
		if( riid == IID_IUnknown ) {
			*ppvObject = (IUnknown*) this;
			AddRef();
			return S_OK;
		}
		return E_NOINTERFACE;
	}

	inline HRESULT	IFrameFilterBase::_transform( const IFrame* pSrc, IFrame* pDest )
	{
		return transform( *pSrc, *pDest ) == true ? S_OK : S_FALSE;
	}
	inline HRESULT	IFrameFilterBase::_getSupportedInputTypes( FrameTypeInfo** ppTypeArr, long* pCount ) const
	{
		if( pCount == 0 || ppTypeArr == 0 )		return E_POINTER;

		FrameTypeInfoArray arr;
		getSupportedInputTypes( arr );

		if( arr.empty() )
		{
			*ppTypeArr = 0;
			*pCount = 0;
			return S_FALSE;
		}

		*ppTypeArr = (FrameTypeInfo*) CoTaskMemAlloc( sizeof( FrameTypeInfo ) * arr.size() );
		if( *ppTypeArr == 0 )			return E_OUTOFMEMORY;

		*pCount = (long) arr.size();

		std::copy( arr.begin(), arr.end(), *ppTypeArr );

		return S_OK;
	}

	inline HRESULT IFrameFilterBase::_checkInput( const FrameTypeInfo* pTypeToTest ) const
	{
		return checkInputType( *pTypeToTest ) ? S_OK : E_FAIL;
	}

	inline HRESULT	IFrameFilterBase::_notifyStart( const FrameTypeInfo* pInType, const FrameTypeInfo* pOutType )
	{
		notifyStart( *pInType, *pOutType );
		return S_OK;
	}
	inline HRESULT	IFrameFilterBase::_notifyStop()
	{
		notifyStop();
		return S_OK;
	}

	inline HRESULT IFrameFilterBase::_checkTransform( const FrameTypeInfo* pInType, const FrameTypeInfo* pOutType ) const
	{
		return checkTransformTypes( *pInType, *pOutType ) ? S_OK : S_FALSE;
	}

	inline HRESULT	IFrameFilterBase::_getTransformOutputTypes( const FrameTypeInfo* pInType, FrameTypeInfo** ppTypeArr, long* pCount ) const
	{
		if( pCount == 0 || ppTypeArr == 0 )		return E_POINTER;

		FrameTypeInfoArray arr;
		if( !getTransformOutputTypes( *pInType, arr ) )		return E_FAIL;

		*ppTypeArr = (FrameTypeInfo*) CoTaskMemAlloc( sizeof( FrameTypeInfo ) * arr.size() );
		if( *ppTypeArr == 0 )			return E_POINTER;

		*pCount = (long) arr.size();

		std::copy( arr.begin(), arr.end(), *ppTypeArr );

		return S_OK;
	}
	inline FrameTypeInfoArray	compactFrameTypeInfoArray( const FrameTypeInfoArray& arr )
	{
		FrameTypeInfoArray rval_arr;
		for( FrameTypeInfoArray::const_iterator i = arr.begin(); i != arr.end(); ++i ) {
			bool bUnique = true;
			for( FrameTypeInfoArray::const_iterator j = i; j != arr.end(); ++j ) {
				if( isPartiallyEqual( *i, *j ) )
				{
					bUnique = false;
				}
			}
			if( bUnique )	rval_arr.push_back( *i );
		}
		return rval_arr;
	}
};

#endif // FRAMEFILTERBASE_H_INC_
