#ifndef _SMARTPTR_H_INC_
#define _SMARTPTR_H_INC_

//__declspec( dllimport )	void	lockPtrLock();
//__declspec( dllimport )	void	unlockPtrLock();

//#pragma comment ( lib, "ptrdll.lib" )

inline void	lockPtrLock()
{
}

inline void	unlockPtrLock()
{
}


// forward declarations
template<typename TType,class TPointerPolicy> class smart_ptr;
template<typename TType,class TPointerPolicy> void swap( smart_ptr<TType,TPointerPolicy>&, smart_ptr<TType,TPointerPolicy>& );

template<typename TType>
class refcount_ptr
{
public:
	typedef TType			value_type;
	typedef TType*			pointer;
	typedef const TType*	const_pointer;
	typedef TType&			reference;
	typedef const TType&	const_reference;
private:
	typedef LONG			refcount_type;
public:
	refcount_ptr( pointer p )
	{
		// does not need to be locked, because the object is not yet fully constructed and so cannot be changed
		m_pData = p;
		m_piRefCount = new refcount_type( 1 );
	}
	template<typename TTOp2>
	refcount_ptr( const refcount_ptr<TTOp2>& op2 )
	{
		// we need to lock here, so that we don't get caught in a copy operation
		lockPtrLock();
		m_pData = static_cast<pointer>( op2.get() );
		m_piRefCount = op2.getRefCountPtr();
		::InterlockedIncrement( m_piRefCount );
		unlockPtrLock();
	}
	template<typename TTOp2>
	refcount_ptr( TTOp2 p, refcount_type* rp )
	: m_pData( reinterpret_cast<pointer>( p ) ), m_piRefCount( rp )
	{
		// does not need to be locked, because the object is not yet fully constructed and so cannot be changed
		::InterlockedIncrement( m_piRefCount );
	}
	refcount_ptr( const refcount_ptr& op2 )
	{
		// we need to lock here, so that we don't get caught in a copy operation
		lockPtrLock();
		m_pData =op2.m_pData;
		m_piRefCount = op2.m_piRefCount;
		::InterlockedIncrement( m_piRefCount );
		unlockPtrLock();
	}
	~refcount_ptr()
	{
		if( ::InterlockedDecrement( m_piRefCount ) == 0 )
		{
			if( get() )
			{
				delete get();
			}
			delete m_piRefCount;
		}
	}

	void			swap( refcount_ptr& op2 )
	{
		// we need to lock here, so that we don't get caught in a copy operation
		lockPtrLock();
		std::swap( m_pData, op2.m_pData );
		std::swap( m_piRefCount, op2.m_piRefCount );
		unlockPtrLock();
	}

	pointer			get()						{ return m_pData; }
	pointer			get() const					{ return m_pData; }

	// deprecated
	refcount_type*	getRefCountPtr() const		{ return m_piRefCount; }
private:
	refcount_type*	m_piRefCount;
	pointer			m_pData;
};

/** smart_ptr is a reference counting pointer object.
 * it may be empty. So calling operator* and operator-> on an empty smart_ptr may fail with a null pointer exception.
 *
 * the TPointerPolicy object is used to pass an own reference counter object.
 * <br>
 * internal representation :
 *		TPointerPolicy* always points to an object, which may be empty
 *		when an operation like release, destroy or the dtor is called the object releases its reference count and may destroy the
 *			TPointerPolicy object when that returns a pointer on itself
 */
template<typename TType,class TPointerPolicy = refcount_ptr<TType> >
class smart_ptr : public TPointerPolicy
{
public:
	typedef TType			value_type;
	typedef TType*			pointer;
	typedef const TType*	const_pointer;
	typedef TType&			reference;
	typedef const TType&	const_reference;
	
	typedef smart_ptr<TType,TPointerPolicy>	object_type;
	typedef TPointerPolicy	tPointerPolicy;
public:
	/** ctor */
	smart_ptr( pointer p = 0 );

	template<typename TTOp2,class TOwnershipPolOp2>
	smart_ptr( const smart_ptr<TTOp2,TOwnershipPolOp2>& op2 )
	: TPointerPolicy( op2 )
	{
	}
	template<typename TTOp2,class TOwnershipPolOp2>
	smart_ptr( const TPointerPolicy& op2 )
	: TPointerPolicy( op2 )
	{
	}
	/** copy constructor */
	smart_ptr( const object_type& op2 );
	/** dtor */
	~smart_ptr();

	/** assignment operator*/
	smart_ptr&	operator=( smart_ptr op2 ); // take a copy which is much better

	/** member selection operator
	 * may fail when the pointer is empty
	 */
	reference			operator*();
	/** member selection operator
	 * may fail when the pointer is empty
	 */
	const_reference		operator*() const;
	/** member selection operator
	 * may fail when the pointer is empty
	 */
	pointer				operator->();
	/** member selection operator
	 * may fail when the pointer is empty
	 */
	const_pointer		operator->() const;

	/** compare operator== */
	bool		operator==( const smart_ptr& op2 ) const;
	/** compare operator!= */
	bool		operator!=( const smart_ptr& op2 ) const;
	
	/** compare operator==.
	 * implemented cause of performance reasons
	 */
	bool		operator==( const pointer p ) const;
	/** compare operator!=.
	 * implemented cause of performance reasons
	 */
	bool		operator!=( const pointer p ) const;

	/** lets you get the internal pointer representation
	 * <strong>do not delete the internal rep, use destroy</strong>
	 */
	pointer			get();
	/** lets you get the internal pointer representation
	 * <strong>do not delete the internal rep, use destroy</strong>
	 */
	const_pointer	get() const;

	/** destroys the internal reference and sets this to zero */
	void			destroy();
	/** swaps this with op2 */
	void			swap( smart_ptr& op2 );
};

template<typename TType,class TPointerPolicy>
inline smart_ptr<TType,TPointerPolicy>::smart_ptr( pointer p )
: TPointerPolicy( p )
{
}

template<typename TType,class TPointerPolicy>
inline smart_ptr<TType,TPointerPolicy>::smart_ptr( const smart_ptr<TType,TPointerPolicy>& op2 )
: TPointerPolicy( op2 )
{
}

template<typename TType,class TPointerPolicy>
smart_ptr<TType,TPointerPolicy>::~smart_ptr()
{
}

template<typename TType,class TPointerPolicy>
smart_ptr<TType,TPointerPolicy>& smart_ptr<TType,TPointerPolicy>::operator=( smart_ptr<TType,TPointerPolicy> op2 )
{
	swap( op2 );
	return *this;
}

template<typename TType,class TPointerPolicy>
inline typename smart_ptr<TType,TPointerPolicy>::reference	smart_ptr<TType,TPointerPolicy>::operator*()
{
	return *get();
}

template<typename TType,class TPointerPolicy>
inline const typename smart_ptr<TType,TPointerPolicy>::value_type&	smart_ptr<TType,TPointerPolicy>::operator*() const
{
	return *get();
}

template<typename TType,class TPointerPolicy>
inline typename smart_ptr<TType,TPointerPolicy>::pointer	smart_ptr<TType,TPointerPolicy>::operator->()
{
	return get();
}

template<typename TType,class TPointerPolicy>
inline typename smart_ptr<TType,TPointerPolicy>::const_pointer	smart_ptr<TType,TPointerPolicy>::operator->() const
{
	return get();
}

template<typename TType,class TPointerPolicy>
inline bool		smart_ptr<TType,TPointerPolicy>::operator==( const smart_ptr<TType,TPointerPolicy>& op2 ) const
{
	return op2.get() == get();
}
template<typename TType,class TPointerPolicy>
inline bool		smart_ptr<TType,TPointerPolicy>::operator!=( const smart_ptr<TType,TPointerPolicy>& op2 ) const
{
	return op2.get() != get();
}

template<typename TType,class TPointerPolicy>
inline bool		smart_ptr<TType,TPointerPolicy>::operator==( typename smart_ptr<TType,TPointerPolicy>::pointer p ) const
{
	return get() == p;
}

template<typename TType,class TPointerPolicy>
inline bool		smart_ptr<TType,TPointerPolicy>::operator!=( typename smart_ptr<TType,TPointerPolicy>::pointer p ) const
{
	return get() != p;
}

template<typename TType,class TPointerPolicy>
inline bool		operator==( const void* p, const smart_ptr<TType,TPointerPolicy>& op2 )
{
	return op2.get() == p;
}

template<typename TType,class TPointerPolicy>
inline bool		operator!=( const void* p, const smart_ptr<TType,TPointerPolicy>& op2 )
{
	return op2.get() != p;
}

template<typename TType,class TPointerPolicy>
inline typename smart_ptr<TType,TPointerPolicy>::pointer	smart_ptr<TType,TPointerPolicy>::get()
{
	return static_cast<pointer>( TPointerPolicy::get() );
}

template<typename TType,class TPointerPolicy>
inline typename smart_ptr<TType,TPointerPolicy>::const_pointer	smart_ptr<TType,TPointerPolicy>::get() const
{
	return static_cast<const_pointer>( TPointerPolicy::get() );
}

template<typename TType,class TPointerPolicy>
inline void		smart_ptr<TType,TPointerPolicy>::destroy()
{
	*this = 0;
/*	object_type tmp();
	swap( tmp );
	*/
}

template<typename TType,class TPointerPolicy>
inline void	smart_ptr<TType,TPointerPolicy>::swap( smart_ptr<TType,TPointerPolicy>& op2 )
{
	TPointerPolicy::swap( op2 );
}

template<typename TType,class TPointerPolicy>
inline void	swap( smart_ptr<TType,TPointerPolicy>& op1, smart_ptr<TType,TPointerPolicy>& op2 )	{ op1.swap( op2 ); }

#endif