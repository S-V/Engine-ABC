/*
=============================================================================
	File:	Win32_Types.h
	Desc:	Win32 platform-specific types and some handy typedefs.
=============================================================================
*/

namespace abc {

//
//	Low-level integer types.
//

typedef signed __int8		byte;
typedef signed __int16		word;
typedef signed __int32		dword;
typedef signed __int64		qword;

typedef unsigned __int8		ubyte;
typedef unsigned __int16	uword;
typedef unsigned __int32	udword;
typedef unsigned __int64	uqword;

//---------------------------------------------------------------

//
//	Character types.
//

// Characters must be signed !
#ifdef _CHAR_UNSIGNED
#	error Wrong project settings: The 'char' type must be signed!
#endif

typedef char		ANSICHAR;		// ANSI character.
typedef wchar_t		UNICODECHAR;	// Unicode character.

#ifdef UNICODE
	typedef UNICODECHAR				mxChar;	// Default character type.
	typedef unsigned __int16		mxUChar;
#else
	typedef char					mxChar;	// Default character type.
	typedef unsigned char			mxUChar;
#endif

// Shortcut for wide character type.
typedef wchar_t		WChar;

//---------------------------------------------------------------

//
//	Base integer types.
//

typedef unsigned __int8		BYTE;
typedef signed __int8		mxInt8;		// Signed 8-bit integer.
typedef unsigned __int8		mxUInt8;	// Unsigned 8-bit integer.

typedef signed __int16		SWORD;
typedef unsigned __int16	UWORD;
typedef signed __int16		mxInt16;	// Signed 16-bit integer.
typedef unsigned __int16	mxUInt16;	// Unsigned 16-bit integer.

typedef unsigned __int32	UDWORD;
typedef signed __int32		mxInt32;	// Signed 32-bit integer.
typedef unsigned __int32	mxUInt32;	// Unsigned 32-bit integer.

// 64-bit wide types.
typedef unsigned __int64	UQWORD;
typedef signed __int64		mxInt64;	// Signed 64-bit integer.
typedef unsigned __int64	mxUInt64;	// Unsigned 64-bit integer.

// An integer type that is guaranteed to be the same size as a pointer.
typedef signed long			mxLong;
typedef unsigned long		mxULong;

// Size_t is used for counts or ranges which need to span the range of a pointer
// ( e.g.: in memory copying routines. )
// Note: these should be used instead of ints for VERY LARGE arrays as loop counters/indexes.
typedef size_t			mxSizeT;
typedef ptrdiff_t		mxPtrDiff_t;

//
// Shortcuts to do less typing.
//
typedef mxSizeT	SizeT;
typedef mxChar	Char;

//
// The most efficient integer types on this platform.
//
typedef signed int		mxInt;	// Must be the most efficient signed integer type on this platform ( the signed integer type used by default ).
typedef unsigned int	mxUInt;	// Must be the most efficient unsigned integer type on this platform ( the unsigned integer type used by default ).

//
// Type ranges.
//
enum EIntegerTypeRanges
{
	MAX_SBYTE	= 0x7F,			// Maximum 8-bit signed integer.
	MAX_UBYTE	= 0xFFU,		// Maximum 8-bit unsigned integer.

	MAX_SWORD	= 0x7FFF,		// Maximum 16-bit signed integer.
	MAX_UWORD	= 0xFFFFU,		// Maximum 16-bit unsigned integer.

	MAX_SDWORD	= 0x7FFFFFFF,	// Maximum 32-bit signed integer.
	MAX_UDWORD	= 0xFFFFFFFFU	// Maximum 32-bit unsigned integer.
};

const mxInt32	MAX_INT16	= MAX_SWORD;
const mxUInt32	MAX_UINT16	= MAX_UWORD;
const mxInt32	MAX_INT32	= MAX_SDWORD;
const mxUInt32	MAX_UINT32	= MAX_UDWORD;

//
//	Base IEEE-754 floating-point types.
//
typedef float		mxFloat32;	// Used when 32-bit floats are explicitly required.
typedef double		mxFloat64;	// Used when 64-bit floats are explicitly required.

// 80-bit extended precision floating point data type.
// Note: this type is not accessible through C/C++. Use asm for that.
//typedef long double	mxFloat80;


typedef mxFloat32	mxReal;	// Default floating-point type.

//
//	FASTBOOL - 'true' is any non-zero value.
//
typedef mxInt32		FASTBOOL;	// For performance-related reasons.

}//End of namespace abc

//===============================================================

//
// The 'null' type.
//
#ifndef NULL
#define NULL	0
#endif

//#define nullptr	0
#define null	0

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
