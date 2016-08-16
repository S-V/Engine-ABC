/*
=============================================================================
	File:	Debug.h
	Desc:	Code for debug utils, assertions and other very useful things.
	Note:	Some debug utils depend heavily on the platform.
=============================================================================
*/

#ifndef __MX_DEBUG_UTILS_H__
#define __MX_DEBUG_UTILS_H__

/*
=============================================================================
	
		Run-time checks and assertions.

=============================================================================
*/


/*
=============================================================================
		Assertions.

	Examples of usage:
		Assert( a > b );
		Assert2( x == y, "X must be equal to Y!" );

	NOTE: Asserts are removed in release builds.
=============================================================================
*/

#ifdef MX_DEBUG

	#define Assert( expr )		\
		BREAK_IF( ! (expr) )

	#define Assert2( expr, message )	\
		BREAK_IF( ! (expr) )

	#define AssertPtr( expr )		\
		BREAK_IF( 0 == expr )
#else

	#define Assert( expr )				NOOP
	#define Assert2( expr, message )	NOOP
	#define AssertPtr( expr )			NOOP

#endif // ! MX_DEBUG

//
// FullAsserts are never removed in release builds, they slow down a lot.
//
#define AlwaysAssert( expr )	\
	(void)( (expr) ? 1 : (sys::FatalError( "Assertion failed: '%s' in %s, at '%s', line %d", #expr, __FUNCTION__, __FILE__, __LINE__ ) ))

#define AlwaysAssert2( expr, message )	\
	(void)( (expr) ? 1 : (sys::FatalError( "Assertion failed: '%s': '%s' in %s, at '%s', line %d", message, #expr, __FUNCTION__, __FILE__, __LINE__ ) ))

//
// Causes a breakpoint exception in debug builds, causes fatal error in release builds.
//
#ifdef MX_DEBUG
	#define CHECK( expr )	(void)( (expr) ? 1 : sys::DebugBreak() )
#else
	#define CHECK( expr )	\
		(void)( (expr) ? 1 : (::abc::sys::FatalError( "Check failed: '%s' in %s, at '%s', line %d", #expr, __FUNCTION__, __FILE__, __LINE__ ) ))
#endif // ! MX_DEBUG

/*
=============================================================================
		Debug macros.
=============================================================================
*/

// Everything inside the parentheses will be discarded in release builds.
#ifdef MX_DEBUG
	#define DEBUG_CODE( code )		code
#else
	#define DEBUG_CODE( code )
#endif // ! MX_DEBUG

/*
=============================================================================
	IF_DEBUG is a C/C++ comment-out macro.

	The preprocessor must support the '//' character sequence
	as a single line comment.

	Example:
	IF_DEBUG outputDevice << "Generating a random world " << world.Name();
=============================================================================
*/
#ifdef MX_DEBUG
#	define IF_DEBUG		// Debug version - enable code on line.
#else
#	define _PT_SLASH( c )	/##c
#	define IF_DEBUG			_PT_SLASH(/)	// Release version - comment out line.
#endif // ! MX_DEBUG

/*
=============================================================================

	checked_cast< class TypeTo, class TypeFrom >

	Works as a static_cast, except that it would use dynamic_cast 
	to catch programming errors in debug mode.

=============================================================================
*/
#ifdef MX_DEBUG

	template < class TypeTo, class TypeFrom >
	FORCEINLINE TypeTo checked_cast( TypeFrom ptr )
	{
		if ( ptr )
		{
			TypeTo dtto = dynamic_cast< TypeTo >( ptr );
			TypeTo stto = static_cast< TypeTo >( ptr );
			Assert( dtto != NULL );
			Assert( dtto == stto );
			return stto;
		}
		return NULL;
	}

#else // if !defined( MX_DEBUG )

	template < class TypeTo, class TypeFrom >
	FORCEINLINE TypeTo checked_cast( TypeFrom ptr )
	{
		return static_cast< TypeTo >( ptr );
	}

#endif // ! MX_DEBUG

/*
// Type punning.
// Always cast any type into any other type.
// (This is evil.)
//
template < class TargetType, class SourceType > 
inline TargetType always_cast( SourceType x )
{
	union {
		Source S;
		Target T;
	} value;

	value.S = x;
	return value.T;
}

template< typename A, typename B >
A& TypePunning( B &v )
{
    union
    {
        A *a;
        B *b;
    } pun;
    pun.b = &v;
    return *pun.a;
}

template< typename A, typename B >
const A& TypePunning( const B &v )
{
    union
    {
        const A *a;
        const B *b;
    } pun;
    pun.b = &v;
    return *pun.a;
}
*/

//----------------------------------------------------------------------------
//	Use this to mark unreachable locations 
//	( like an unreachable default case of a switch )
//	Unreachables are removed in release builds.
//----------------------------------------------------------------------------

#ifdef MX_DEBUG

#define  Unreachable	\
	sys::FatalError( "Unreachable code in %s, at '%s', line %d", __FUNCTION__, __FILE__, __LINE__ );

#define  Unreachable2( message )	\
	sys::FatalError( "'%s': unreachable code in %s, at '%s', line %d", message, __FUNCTION__, __FILE__, __LINE__ );

#else

#define  Unreachable	OPT_HINT( 0 )

#define  Unreachable2( message )	OPT_HINT( 0 )

#endif // ! MX_DEBUG



//----------------------------------------------------------------------------
//	This can be used to mark code paths that should not be executed
//	(e.g. overriden member function Write() of some class named ReadOnlyFile).
//----------------------------------------------------------------------------

#ifdef MX_DEBUG
#	define  InvalidCall	DEBUG_BREAK
#else
#	define  InvalidCall		sys::FatalError( "Invalid call in %s, at '%s', line %d", __FUNCTION__, __FILE__, __LINE__ )
#endif // ! MX_DEBUG


//----------------------------------------------------------------------------
//	Use this to mark unimplemented features
//	which are not supported yet and will cause a crash.
//  They are not removed in release builds.
//----------------------------------------------------------------------------
#ifdef MX_DEBUG

	#define  Unimplemented				DEBUG_BREAK
	#define  Unimplemented2( message )	DEBUG_BREAK

#else

	#define  Unimplemented	\
		sys::FatalError( "Unimplemented feature in %s, at '%s', line %d", __FUNCTION__, __FILE__, __LINE__ );

	#define  Unimplemented2( message )	\
		sys::FatalError( "Unimplemented feature '%s' in %s, at '%s', line %d", message, __FUNCTION__, __FILE__, __LINE__ );

#endif // ! MX_DEBUG

/*
=============================================================================
	
		Compile-time checks and assertions.

=============================================================================
*/

/*
============================================================================
	StaticAssert( expression )
	STATIC_ASSERT2( expression, message )

	Fires at compile-time !

	Usage:
		StaticAssert( sizeof(*void) == 4, size_of_void_must_be_4_bytes );
============================================================================
*/
#define StaticAssert( expression )				\
struct CHECK_##__LINE__ {						\
	int m_error[ (expression) ? 1 : -1 ];		\
};

#define STATIC_ASSERT2( expression, message )	\
struct ERROR_##message {						\
	ERROR_##message() {							\
		int _##message[ expression == true ];	\
	}											\
};


/*
============================================================================
	STATIC_CHECK( expression )
	STATIC_CHECK2( expression, message )

	Fires at compile-time !

	Usage:
		STATIC_CHECK2( sizeof(*void) == 4, size_of_void_must_be_4_bytes );
============================================================================
*/

namespace debug
{
	template< bool > struct CompileTimeChecker;
	template<> struct CompileTimeChecker< true > {
		CompileTimeChecker( ... );
	};
}//end of namespace debug


#define STATIC_CHECK( expr )							\
	debug::CompileTimeChecker < (expr) != false > ();


#define STATIC_CHECK2( expr, msg )						\
class ERROR_##msg {										\
	debug::CompileTimeChecker< (expr) != false > error;	\
}


/*
=================================================================
	DELAYED_ASSERT( expression );

	Valid only in function scope !

	Fires at link-time !
	( unresolved external symbol void StaticAssert<0>(void); )

	Usage example:
			void  Foo()
			{
				DELAYED_ASSERT( a == b );

				// ....
			}
=================================================================
* /
namespace debug {

	template< bool > void	StaticAssert();
	template<> inline void	StaticAssert< true >() {}

}//end of namespace debug

#define DELAYED_ASSERT( expression )					\
		::debug::StaticAssert< expression != false >();	\
*/

//--------------------------------------------------------------------------------------------------------------
//	These can be very useful compile-time assersions :
//--------------------------------------------------------------------------------------------------------------

// This one is valid in function scope only!
#define COMPILE_TIME_ASSERT( x )				{ typedef int ZZ_compile_time_assert_failed[ (x) ? 1 : -1 ]; }

// This one is valid in function scope only!
#define COMPILE_TIME_ASSERT2( x )				switch(0) { case 0: case !!(x) : ; }

// This one is valid in file and function scopes.
#define FILE_SCOPED_COMPILE_TIME_ASSERT( x )	extern int ZZ_compile_time_assert_failed[ (x) ? 1 : -1 ]

#define ASSERT_SIZEOF( type, size )				FILE_SCOPED_COMPILE_TIME_ASSERT( sizeof( type ) == size )
#define ASSERT_OFFSETOF( type, field, offset )	FILE_SCOPED_COMPILE_TIME_ASSERT( offsetof( type, field ) == offset )
#define ASSERT_SIZEOF_8_BYTE_MULTIPLE( type )	FILE_SCOPED_COMPILE_TIME_ASSERT( ( sizeof( type ) & 8 ) == 0 )
#define ASSERT_SIZEOF_16_BYTE_MULTIPLE( type )	FILE_SCOPED_COMPILE_TIME_ASSERT( ( sizeof( type ) & 15 ) == 0 )
#define ASSERT_SIZEOF_32_BYTE_MULTIPLE( type )	FILE_SCOPED_COMPILE_TIME_ASSERT( ( sizeof( type ) & 31 ) == 0 )

//--------------------------------------------------------------------------------------------------------------

#endif // !__MX_DEBUG_UTILS_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
