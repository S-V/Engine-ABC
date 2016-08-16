/*
=============================================================================
	File:	Singleton.h
	Desc:	A very simple singleton template.
	Note:	Don't misuse singletons!
=============================================================================
*/

#ifndef __SINGLETON_H__
#define __SINGLETON_H__

namespace abc {

//
//	Singleton< T >
//
// If you want to make object of some class singleton
// you have to derive this class from Singleton class.
//
template< typename T >
class Singleton {
private:
	// A static pointer to an object of T type.
	// This is the core member of the singleton. 
	// As it is declared as static no more than
	// one object of this type can exist at the time.
	//
	static T * ms_pInstance;

public:
	Singleton()
	{
		Assert2( ms_pInstance == NULL, "Singleton class already instantiated." );

#if defined( _MSC_VER ) && _MSC_VER < 1200
		INT offset = (INT)( T* )1 - (INT)(Singleton<T>* )( T* )1;
		ms_pInstance = ( T* )((INT)this + offset);
#else
		ms_pInstance = static_cast< T* >( this );
#endif
	}

	virtual ~Singleton()
	{
		Assert2( ms_pInstance != NULL, "Singleton class not instantiated." );
		ms_pInstance = NULL;
	}

	static T & GetInstance()
	{
		Assert2( ms_pInstance != NULL, "Singleton class not instantiated." );
		return *ms_pInstance;
	}

	static T * GetInstancePtr()
	{
		Assert2( ms_pInstance != NULL, "Singleton class not instantiated." );
		return ms_pInstance;
	}
};

/*================================
		Singleton< T >
================================*/

template< typename T >
T * Singleton< T >::ms_pInstance = NULL;

/*
==========================================================
	This is used to ensure unique instantiation.
	This code must be placed inside constructor.
==========================================================
*/
#ifdef MX_DEBUG

	#define ENSURE_ONE_CALL								\
	{													\
		static bool Has_Already_Been_Called = false;	\
		if ( Has_Already_Been_Called )					\
		{												\
			Unreachable2( "singleton error" );			\
		}												\
		Has_Already_Been_Called = true;					\
	}

#else

	#define ENSURE_ONE_CALL

#endif //MX_DEBUG

}//End of namespace abc

#endif // ! __SINGLETON_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
