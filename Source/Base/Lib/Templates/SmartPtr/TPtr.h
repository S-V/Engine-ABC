/*
=============================================================================
	File:	TPtr.h
	Desc:	Weak pointer - doesn't take ownership over the object,
			doesn't know about reference counting,
			used to catch programming errors (automatic null pointer checks).
=============================================================================
*/

#ifndef __MX_CONTAINTERS_WEAK_POINTER_H__
#define __MX_CONTAINTERS_WEAK_POINTER_H__

namespace abc {

template< typename T >
class TPtr {
public:
	TPtr()
		: m_pointer( 0 )
	{}

	TPtr( T *pointer )
	{
		AssertPtr( pointer );
		m_pointer = pointer;
	}

	explicit TPtr( const TPtr<T>& other )
	{
		*this = other;
	}

	~TPtr()
	{}

	T * operator -> () const
	{
		AssertPtr( m_pointer );
		return m_pointer;
	}
	
	T & operator * () const
	{
		AssertPtr( m_pointer );
		return *m_pointer;
	}

	operator T* ()
	{
		return m_pointer;
	}
	operator const T* () const
	{
		return m_pointer;
	}

	void operator = ( T* newPointer )
	{
	//	AssertPtr( newPointer );
		m_pointer = newPointer;
	}
	void operator = ( const TPtr<T>& other )
	{
		Assert( other.IsValid() );
		m_pointer = other.m_pointer;
	}

	bool operator == ( T* pObject ) const
	{
		return ( m_pointer == pObject );
	}
	bool operator != ( T* pObject ) const
	{
		return ( m_pointer != pObject );
	}
	bool operator == ( const TPtr<T>& other ) const
	{
		return ( m_pointer == other.m_pointer );
	}
	bool operator != ( const TPtr<T>& other ) const
	{
		return ( m_pointer != other.m_pointer );
	}

	bool IsNull() const
	{
		return ( m_pointer == 0 );
	}
	bool IsValid() const
	{
		return ( m_pointer != 0 );
	}

	// An unsafe way to get the pointer.
	T*& get_ref()
	{
		return m_pointer;
	}
	const T* get() const
	{
		//Assert( this->IsValid() );
		return m_pointer;
	}

private:
	T *		m_pointer;
};

}//end of namespace abc

#endif // ! __MX_CONTAINTERS_WEAK_POINTER_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
