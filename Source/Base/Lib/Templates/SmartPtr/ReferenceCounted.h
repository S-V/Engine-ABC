/*
=============================================================================
	File:	ReferenceCounted.h
	Desc:	A very simple basic class for reference counted objects.
=============================================================================
*/

#ifndef __MX_REFERENCE_COUNTED_H__
#define __MX_REFERENCE_COUNTED_H__

/*
=======================================================

	ReferenceCounted

=======================================================
*/

namespace abc {

//
//	ReferenceCounted
//
class ReferenceCounted {
public:
	void	Grab() const;
	bool	Drop() const;	// ( Returns true if the object has been deleted. )

	INT	GetReferenceCount() const;

protected:
			ReferenceCounted();
	virtual	~ReferenceCounted();

private:
	volatile mutable INT	referenceCounter;	// Number of references to this object.

#ifdef MX_DEBUG
private:
	// this value should be 0 at the application exit point
	static INT	_totalNumReferences;
public:
	static INT	GetTotalReferenceCount() { return _totalNumReferences; }
#endif // MX_DEBUG
};

//
//	Grab( ReferenceCounted* ) - increments the reference count of the given object.
//
FORCEINLINE void GRAB( ReferenceCounted* p )
{
	if( p != null )
	{
		p->Grab();
	}
}

//
//	Drop( ReferenceCounted * ) - decrements the reference count of the given object.
//
FORCEINLINE void DROP( ReferenceCounted * p )
{
	if( p != null )
	{
		p->Drop();
	}
}

/*================================
		ReferenceCounted
================================*/

FORCEINLINE ReferenceCounted::ReferenceCounted()
	: referenceCounter( 0 )
{
}

FORCEINLINE ReferenceCounted::~ReferenceCounted()
{
}

FORCEINLINE void ReferenceCounted::Grab() const {
	++referenceCounter;

#ifdef MX_DEBUG
	++_totalNumReferences;
#endif // MX_DEBUG
}

FORCEINLINE bool ReferenceCounted::Drop() const
{	Assert( referenceCounter != 0 );
	--referenceCounter;

#ifdef MX_DEBUG
	--_totalNumReferences;
	Assert( _totalNumReferences >= 0 );
#endif // MX_DEBUG

	if ( referenceCounter == 0 ) {
		delete this;
		return true;
	}
	return false;
}

FORCEINLINE INT ReferenceCounted::GetReferenceCount() const
{
	return referenceCounter;
}

//
//	RefPtr< T > - a pointer to a reference-counted object.
//
template< typename T >
class RefPtr {
public:
			RefPtr( T* refCounted = null );
			RefPtr( const RefPtr& other );
			~RefPtr();

	// Implicit conversions.

			operator T*	() const;
	T &		operator *	() const;
	T *		operator ->	() const;

	// Assignment.

	RefPtr &	operator = ( T* pObject );
	RefPtr &	operator = ( const RefPtr& other );

	// Comparisons.

	bool	operator == ( T* pObject ) const;
	bool	operator != ( T* pObject ) const;
	bool	operator == ( const RefPtr& other ) const;
	bool	operator != ( const RefPtr& other ) const;

	// Unsafe...
	T *		get()		{ return m_pObject; }
	T *&	get_ref()	{ return m_pObject; }

private:
	T *		m_pObject;	// The shared reference counted object.
};

template< typename T >
FORCEINLINE RefPtr< T >::RefPtr( T* refCounted )
	: m_pObject( refCounted )
{
	if ( m_pObject ) {
		m_pObject->Grab();
	}
}

template< typename T >
FORCEINLINE RefPtr< T >::RefPtr( const RefPtr& other )
	: m_pObject( other.m_pObject )
{
	if ( m_pObject ) {
		m_pObject->Grab();
	}
}

template< typename T >
FORCEINLINE RefPtr< T >::~RefPtr()
{
	if ( m_pObject ) {
		m_pObject->Drop();
	}
}

template< typename T >
FORCEINLINE RefPtr< T >::operator T* () const
{	Assert( m_pObject );
	return m_pObject;
}

template< typename T >
FORCEINLINE T & RefPtr< T >::operator * () const
{	Assert( m_pObject );
	return *m_pObject;
}

template< typename T >
FORCEINLINE T * RefPtr< T >::operator -> () const
{
	return m_pObject;
}

template< typename T >
FORCEINLINE RefPtr< T > & RefPtr< T >::operator = ( T* pObject )
{
	if ( pObject == null )
	{
		if ( m_pObject ) {
			m_pObject->Drop();
			m_pObject = null;
		}
		return *this;
	}
	if ( m_pObject != pObject )
	{
		if ( m_pObject ) {
			m_pObject->Drop();
		}
		pObject->Grab();
		m_pObject = pObject;
		return *this;
	}
	return *this;
}

template< typename T >
FORCEINLINE RefPtr< T > & RefPtr< T >::operator = ( const RefPtr& other )
{
	return ( *this = other.m_pObject );
}

template< typename T >
FORCEINLINE bool RefPtr< T >::operator == ( T* pObject ) const
{
	return m_pObject == pObject;
}

template< typename T >
FORCEINLINE bool RefPtr< T >::operator != ( T* pObject ) const
{
	return m_pObject != pObject;
}

template< typename T >
FORCEINLINE bool RefPtr< T >::operator == ( const RefPtr& other ) const
{
	return m_pObject == other.m_pObject;
}

template< typename T >
FORCEINLINE bool RefPtr< T >::operator != ( const RefPtr& other ) const
{
	return m_pObject != other.m_pObject;
}

}//End of namespace abc

#endif // ! __MX_REFERENCE_COUNTED_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
