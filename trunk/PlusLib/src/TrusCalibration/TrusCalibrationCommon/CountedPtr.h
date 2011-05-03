#ifndef COUNTED_PTR
#define COUNTED_PTR
// ===========================================================================
// Program Modification Record
// ===========================================================================
// Component:	Utility
// ===========================================================================
// Class:		CountedPtr
// ===========================================================================
// File Name:	CountedPtr.h
// ===========================================================================
// Author:		Thomas K. Chen <chent@cs.queensu.ca>
//				School of Computing, Queen's University at Kingston, Canada
// ===========================================================================
// Purposes: 
// 1. Expand the reusability of class by generalization.
// 2. Elimate class dependency on Qt framework
// 3. Elimate class dependency on IGO framework
// 4. Change the dependency to C++ Standard Library
// ===========================================================================
// Change History:
// Author			Time						Release	Changes
// Thomas K. Chen	Mon Jun 30 23:35 EDT 2003	1.0		Generalization
//
// ===========================================================================


/*!
This is a reference counted pointer intended to be used with the STL
containers. The interface closely follows that of std::auto_ptr. The
difference between std::auto_ptr and this class is that assigning one
CountedPtrImpl to another does not invalidate the lvalue ptr, neither does
copying a CountedPtrImpl. Basically, you can have more than one pointer
to an object at a time. This class is not thread safe.

Before adding any functionality to this class take a look at shared_ptr at
www.boost.org. I found it too late.
*/
template <class T>
class CountedPtrImpl
{
public:
	//! The type pointed to.
	typedef T element_type;

	//! Constructor.
	explicit CountedPtrImpl( T* p = 0 )
		: ptr( p ), count( new int( 1 ) ) {}

	//! Copy constructor.
	CountedPtrImpl( const CountedPtrImpl<T>& a )
		: ptr( a.ptr ), count( a.count ) { capture(); }


	//! Destructor.
	~CountedPtrImpl() { release(); }

	//! Assignment operator.
	CountedPtrImpl& operator=( const CountedPtrImpl<T>& a ) {
		if ( &a != this ) {
			release();
			ptr = a.ptr;
			count = a.count;
			capture();
		}
		return *this;
	}



	//! Pointer dereference operator.
	T& operator*() { return *ptr; }
	const T& operator*() const { return *ptr; }

	//! Get the internal pointer the object. Try not to call this method.
	T* get() { return ptr; }

	//! Pointer to member operator.
	T* operator->() { return ptr; }
	const T* operator->() const { return ptr; }

#if 0

	/* VC++ 6.0 does not support this functionality. If it did, this
	implementation would be simpler. The work around
	is to only use CountedPtrImpl to base type. For example,

		class Base;
		class Derived : public Base;
		CountedPtrImpl<Base> ptr( new Derived() );

	The risk is that if Base and Derived declare the same non-virtual
	method, the wrong one will be called. Good news is that if your
	classes declare the the same non-virtual methods, you've got bigger
	problems anyway.
	*/

	//! Copy constructor for derived types.
	template <class D> CountedPtrImpl( const CountedPtrImpl<D>& a )
		: ptr( a.ptr ), count( a.count ) { capture(); }

	//! Assignment operator for derived types.
	template <class D>
	CountedPtrImpl& operator=( const CountedPtrImpl<D>& a ) {
		if ( a.ptr != ptr ) {
			release();
			ptr = a.ptr;
			count = a.count;
			capture();
		}
		return *this;
	}

	//! Support the template copy constructor and assignment operator.
	template <class D> friend class CountedPtrImpl;

#endif

//private:

	//! Decrement the reference count. Delete the object if required.
	void release() {
		if ( 0 == --*count ) {
			delete ptr;
			delete count;
		}
		ptr = 0;
		count = 0;
	}

	//! Increment the reference count.
	void capture() { ++*count; }

	//! Pointer to the object.
	T* ptr;

	//! Count of pointers to object. Shared by all pointers to the object.
	int* count;
};

/*!
This class works around limitations in the VC++ 6.0 compiler. See
CountedPtrImpl for detailed documentation.

\sa CountedPtrImpl
*/
template < class T >
class CountedPtr : public CountedPtrImpl<T> {
public:
	explicit CountedPtr( T* p = 0 )
		: CountedPtrImpl<T>( p ) {}

	template < class D > CountedPtr( const CountedPtr<D>& a )
		{ ptr = a.ptr; count = a.count; capture(); }
};

#endif
