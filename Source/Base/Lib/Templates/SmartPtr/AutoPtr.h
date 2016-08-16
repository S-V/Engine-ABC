/*
=============================================================================
	File:	AutoPtr.h
	Desc:	Smart pointer for automatic memory deallocation.
			Analog of std::auto_ptr.
=============================================================================
*/

#ifndef __MX_AUTO_PTR_H__
#define __MX_AUTO_PTR_H__

namespace abc {

//
//	AutoPtr< T >
//
//	NOTE: This class is not safe for array new's.
//	It will not properly call the destructor for each element
//	and you will silently leak memory.
//	It does work for classes requiring no destructor however.
//
template< typename T >
class AutoPtr {
public:
	explicit AutoPtr( T *pointer = NULL )
		: m_pointer( pointer )
	{}

	~AutoPtr()
	{
		delete m_pointer;
	}

	T & operator * () const
	{
		return *m_pointer;
	}

	T * operator -> () const
	{
		Assert( m_pointer != NULL );
		return m_pointer;
	}

	operator T* ()
	{
		return m_pointer;
	}

	T* get() const
	{
		Assert( m_pointer != NULL );
		return m_pointer;
	}

	void operator = ( T* pointer )
	{
		if ( m_pointer != pointer )
		{
			delete m_pointer;
		}
		m_pointer = pointer;
	}

private:
	T *		m_pointer;

private:
	// Disallow copies.
	AutoPtr( const AutoPtr<T>& other );
	AutoPtr<T> & operator=( const AutoPtr<T>& other );
};

}//End of namespace abc

#endif // ! __MX_AUTO_PTR_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
