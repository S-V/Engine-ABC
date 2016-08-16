/*
=============================================================================
	File:	Macros.h
	Desc:	Helper macros used throughout the engine.
=============================================================================
*/

#ifndef __MX_HANDY_MACROS_H__
#define __MX_HANDY_MACROS_H__

//-------------------------------------------------------------

//
// Debug or Release mode ?
//
#ifdef MX_DEBUG
	#define	g_bDebugMode	true
	#define	g_bReleaseMode	false
#else
	#define	g_bDebugMode	false
	#define	g_bReleaseMode	true
#endif

//-------------------------------------------------------------

// The standard defines that 'inline' doesnt enforce the function to necessarily be inline.
// It only 'hints' the compiler that it is a good candidate for inline function.
// So it should be decided whether functions defined with MX_INLINE
// should be force-inlined or not after thorough performance measurements are taken.
#define MX_INLINE	inline

//-------------------------------------------------------------

//
//	ARRAY_SIZE
//
#define ARRAY_SIZE( pArray )		( sizeof( pArray ) / sizeof( pArray[0] ) )

template< typename T, size_t N >
size_t CountOf( const T (&) [N] )	{ return N; }

//
//	OFFSET_OF( _struct, member )
//
#define OFFSET_OF( _struct, member )		size_t( ((_struct*) 0)->member )

//-------------------------------------------------------------

#ifndef BIT
#define BIT(n)		(1 << (n))
#endif

#define BITS_TO_BYTES(x) (((x)+7)>>3)
#define BYTES_TO_BITS(x) ((x)<<3)

//-------------------------------------------------------------

#define Interface( ClassName )		struct NO_VTABLE ClassName

#define override( BaseClassName )	virtual

//-------------------------------------------------------------
//	Usage:
//		NO_COPY_CONSTRUCTOR( ClassName );
//		NO_ASSIGNMENT( ClassName );
//
#define NO_COPY_CONSTRUCTOR( ClassName );	\
	ClassName( const ClassName & );	// Do Not implement!

#define NO_ASSIGNMENT( ClassName );			\
	ClassName & operator = ( const ClassName & );	// Do Not implement!

//-------------------------------------------------------------

// Creates a string from the given expression.
//
#define TO_STR(X)	#X

// Conversions from ANSI char to wide char type.
//
#define WIDEN2(x)		L ## x
#define WIDEN(x)		WIDEN2(x)

#define __WFILE__		WIDEN(__FILE__)
#define __WFUNCTION__	WIDEN(__FUNCTION__)

//-------------------------------------------------------------

#define B_RET( x )	{if( !(x) ) return false;}

//-------------------------------------------------------------
//	Markers (hints to the developer).
//-------------------------------------------------------------
//
//	Pros: bookmarks are not needed;
//		  easy to "Find All References" and no comments needed.
//	Cons: changes to the source file require recompiling;
//		  time & data have to be inserted manually.
//
#define MX_NOTE( message_string )
#define MX_TODO( message_string )
#define MX_REFACTOR( message_string )
#define MX_BUG( message_string )
#define MX_FIXME( message_string )
#define MX_HACK( message_string )
#define MX_OPTIMIZE( message_string )
#define MX_WARNING( message_string )
#define MX_REMOVE_THIS
#define MX_UNDONE

// this is used to mark notes which i address to myself
#define WHY( message_string )

//-------------------------------------------------------------
//	Evil hacks.
//-------------------------------------------------------------

#if 0

// This is used to break data hiding principles (to 'open' private and protected variables).

#define private		public
#define protected	public

#define const_cast		"const_cast<> is prohibited"

#endif

//-------------------------------------------------------------

#endif // !__MX_HANDY_MACROS_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
