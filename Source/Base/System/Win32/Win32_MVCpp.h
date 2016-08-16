/*
=============================================================================
	File:	Win32_MVCpp.h
	Desc:	Win32-specific code for MVC++ compilers.
=============================================================================
*/

#ifndef __MX_PLATFORM_WINDOWS_32_COMPILER_MVCpp_H__
#define __MX_PLATFORM_WINDOWS_32_COMPILER_MVCpp_H__

#ifndef MX_PLATFORM_WIN_32
#	define MX_PLATFORM_WIN_32	1
#endif

//---------------------------------------------------------------
//	Build options available on this platform.
//---------------------------------------------------------------

#define MX_D3DX9		// Compile with DirectX 9 SDK.
#define MX_D3DX10		// Compile with DirectX 10 SDK.

//---------------------------------------------------------------

// Make sure we're compiling the correct (debug/release) version.
#ifdef MX_DEBUG

#	ifndef DEBUG
#		define DEBUG
#	endif

#	ifndef _DEBUG
#		define _DEBUG
#	endif

#	undef NDEBUG

#else // if in release mode

#	ifdef DEBUG
#		undef DEBUG
#	endif

#	ifdef _DEBUG
#		undef _DEBUG
#	endif

#	ifndef NDEBUG
#		define NDEBUG
#	endif

#endif // !MX_DEBUG

//---------------------------------------------------------------

// Check the compiler and windows version.

#ifndef MX_COMPILER_MSVC
#	error Microsoft Visual C++ compiler required!
#endif

#if (_MSC_VER < 1400 )	// MVC++ 2005
#	error A version of Microsoft Visual C++ 8.0 ( 2005 ) or higher is required!
#endif

/*
	Microsoft Visual C++ 10.0 ( 2010 )	-	_MSC_VER == 1600
	Microsoft Visual C++ 9.0 ( 2008 )	-	_MSC_VER == 1500
	Microsoft Visual C++ 8.0 ( 2005 )	-	_MSC_VER == 1400
	Microsoft Visual C++ 7.1 ( 2003 )	-	_MSC_VER == 1310
	Microsoft Visual C++ 7.0 ( 2002 )	-	_MSC_VER == 1300
	Microsoft Visual C++ 6.0 ( 1998 )	-	_MSC_VER == 1200
*/

#	if ( _MSC_VER == 1200 )
#		define MX_COMPILER_NAME		"Microsoft Visual C++ 6.0"
#	elif ( _MSC_VER == 1300 )
#		define MX_COMPILER_NAME		"Microsoft Visual C++ 7.0"
#	elif ( _MSC_VER == 1400 )
#		define MX_COMPILER_NAME		"Microsoft Visual C++ 8.0"
#	elif ( _MSC_VER == 1500 )
#		define MX_COMPILER_NAME		"Microsoft Visual C++ 9.0"
#	elif ( _MSC_VER == 1600 )
#		define MX_COMPILER_NAME		"Microsoft Visual C++ 10.0"
#	else
#		error Unknown compiler !
#	endif

/*
//
// Ensure proper handling of for-scope.
//
#if (_MSC_VER <= 1200)
#	define for if(0); else for
#else
#	pragma conform( forScope, on )
#	pragma warning( disable: 4258 )	// 'variable' : definition from the for loop is ignored; the definition from the enclosing scope is used
#endif
*/

//
//	Prevent the compiler from complaining.
//
#include "DisableWarnings.h"

//
//	Include windows-specific headers.
//
#ifndef WINVER				// Allow use of features specific to Windows XP or later.
#define WINVER 0x0501		// Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif						

#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE			// Allow use of features specific to IE 6.0 or later.
#define _WIN32_IE 0x0600	// Change this to the appropriate value to target other versions of IE.
#endif

#define NOMINMAX	// Suppress Window's global min/max macros.

#define NOGDICAPMASKS
#define OEMRESOURCE
#define NOATOM
#define NOCTLMGR
#define NOMEMMGR
#define NOMETAFILE
#define NOOPENFILE
#define NOSERVICE
#define NOSOUND
#define NOCOMM
#define NOKANJI
#define NOHELP
#define NOPROFILER
#define NODEFERWINDOWPOS
#define NOMCX

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#define VC_EXTRALEAN
#define STRICT	1
#include <windows.h>
#include <winbase.h>
#include <process.h>
#include <mmsystem.h>
#include <mmreg.h>

// CRT's memory leak detection.
#ifdef MX_DEBUG
	#include <crtdbg.h>
#endif

#include <dbghelp.h>

//---------------------------------------------------------------
//	C Run-Time Library Header Files
//---------------------------------------------------------------

#include <memory>
#include <malloc.h>
#include <assert.h>
#include <stdarg.h>
#include <stdlib.h>
#include <tchar.h>
#include <string.h>
//#include <strsafe.h>

#include <math.h>

#include <io.h>
#include <stdio.h>
//#include <fstream>


//---------------------------------------------------------------
//	DirectX SDK
//---------------------------------------------------------------

// Enable DirectX extra debugging.

#ifdef MX_DEBUG
	#ifndef D3D_DEBUG_INFO
	#	define D3D_DEBUG_INFO
	#endif // !D3D_DEBUG_INFO
#endif //MX_DEBUG

// Include main DirectX headers.

#if defined (MX_D3DX9) || defined(MX_D3DX10)

	#define MX_DIRECT3D

	// DirectX error codes.
	#include <dxerr.h>
	#pragma comment( lib, "DxErr.lib" )
#endif

//
//	D3D9
//

#ifdef MX_D3DX9
	#include <d3d9.h>
	#include <d3dx9.h>

	#pragma comment( lib, "d3d9.lib" )

#ifdef MX_DEBUG
	#pragma comment( lib, "d3dx9d.lib" )	// Debug
#else
	#pragma comment( lib, "d3dx9.lib" )		// Release
#endif // !MX_DEBUG

#endif // MX_D3DX9

//
//	D3D10
//

#ifdef MX_D3DX10
	#include <d3d10.h>
	#include <d3dx10.h>

	#pragma comment( lib, "d3d10.lib" )

#ifdef MX_DEBUG
	#pragma comment( lib, "d3dx10d.lib" )	// Debug
#else
	#pragma comment( lib, "d3dx10.lib" )	// Release
#endif

#endif // MX_D3DX10


// Should we use Direct3D library functions in math calculations?
#ifdef MX_DIRECT3D
//#	define USE_D3D_MATH_FUNC
#endif // MX_DIRECT3D

//---------------------------------------------------------------
//		Undefs.
//---------------------------------------------------------------

#include "Win32_Undefs.h"

//---------------------------------------------------------------
//		Defines.
//---------------------------------------------------------------

#ifdef MX_BUILD_ENGINE_DLL
	// We're building our own DLL.
	#define MX_API	__declspec(dllexport)
#else
	#define MX_API	__declspec(dllimport)
#endif // !MX_ENGINE_DLL

#ifndef MX_DEBUG
	#undef _SECURE_SCL
	#define _SECURE_SCL 0

	#undef _HAS_ITERATOR_DEBUGGING
	#define _HAS_ITERATOR_DEBUGGING 0

	#undef _HAS_EXCEPTIONS
	#define _HAS_EXCEPTIONS 0
#endif // !MX_DEBUG

//---------------------------------------------------------------
//		Types.
//---------------------------------------------------------------

#include "Win32_Types.h"

//---------------------------------------------------------------
//	Compiler-specific stuff.
//---------------------------------------------------------------

// Check current compiler settings.

//
// RTTI must be disabled in release builds !
//
#ifdef _CPPRTTI
#	ifndef MX_DEBUG
#		error Project settings: mxTypeInfo must be disabled in release version!
#	else
//		We need it for dynamic_cast ( checked_cast ).
#	endif // ! MX_DEBUG
#endif

#ifndef MX_DEBUG
#	define dynamic_cast	"use of dynamic_cast is prohibited"
#endif //!MX_DEBUG

//
// Exception handling must be disabled in release builds !
//
#ifdef _CPPUNWIND
#	ifndef MX_DEBUG
#		error Project settings: Disable exception handling!
#	else
//		We use exceptions in debug mode.
#	endif // ! MX_DEBUG
#endif

//---------------------------------------------------------------

#define INLINE			inline
#define FORCEINLINE		__forceinline

#define NOOP			__noop	// '__noop' discards everything inside parentheses.

#define MX_DEPRECATED	__declspec( deprecated )

// MVC++ compiler allows non-strings in pragmas.
#define MX_PRAGMA( x )	__pragma x

//
//	Function attributes extension
//	(to allow the compiler perform more error checking, inhibit unwanted warnings, perform more optimizations, etc)
//	(it will be __attribute__(x) under GCC).
//
#define ATTRIBUTE( x )

//
//	Calling conventions.
//
#define VARARGS	__cdecl
#define CDECL	__cdecl
#define PASCAL	__stdcall	// Standard calling convention.
#define VPCALL	__fastcall

#define MX_CALL	__cdecl

//---------------------------------------------------------------------------

#pragma inline_depth( 255 )
#pragma inline_recursion( on )

// Don't initilaize the pointer to the vtable in a constructor of an abstract class.
#define NO_VTABLE	__declspec( novtable )

// Tell the compiler what size of a pointer to a member of a forward-declared class it should use.
// e.g.
//	class VIRTUAL_INHERITANCE Foo;
//	int Foo::*ptr;
// NOTE: You should try to use single inheritance only (for best perfomance).
//
#define SINGLE_INHERITANCE		__single_inheritance
#define MULTIPLE_INHERITANCE	__multiple_inheritance
#define VIRTUAL_INHERITANCE		__virtual_inheritance

// Assume that the given pointer is not aliased.
#define RESTRICT( pointer )		__restrict pointer

// Tell the compiler that the specified condition is assumed to be true.
// 0 can be used to mark unreachable locations after that point in the program.
#define OPT_HINT( expr )		__assume(( expr ))

/* Right from the book:

noalias means that a function call does not modify or reference visible global state 
and only modifies the memory pointed to directly by pointer parameters (first-level indirections). 
If a function is annotated as noalias, the optimizer can assume that, in addition to the parameters themselves,
only first-level indirections of pointer parameters are referenced or modified inside the function.
The visible global state is the set of all data that is not defined or referenced outside of the compilation scope,
and their address is not taken. The compilation scope is all source files (/LTCG (Link-time Code Generation) builds) or a single source file (non-/LTCG build).
*/
#define NO_ALIAS __declspec( noalias )

// Visual C++ compiler doesn't have unroll pragma (?)
#define MX_UNROLL

//---------------------------------------------------------------
//		CPU
//---------------------------------------------------------------

// Assumptions about supported versions of streaming SIMD extensions.
// Valid numbers are 0, 1, 2, 3, 4, 5, ..., ?
#define MX_SSE_VERSION	1

namespace abc {
namespace sys {

// Size of the processor's cache line, in bytes.
enum { CACHE_LINE_SIZE = 64 };

}//End of namespace sys
}//End of namespace abc

#include <intrin.h>
#include <xmmintrin.h>
#include <emmintrin.h>

#pragma intrinsic( memset, memcpy, memcmp )
#pragma intrinsic( strcpy, strlen, strcat, strcmp )

#pragma intrinsic( abs, fabs, labs )
#pragma intrinsic( sin, cos, tan, atan, atan2 )
#pragma intrinsic( exp, log, log10, sqrt )

//---------------------------------------------------------------
//	Asm.
//---------------------------------------------------------------

#include "Win32_Asm.h"

//---------------------------------------------------------------
//	Utilities for Debugging.
//---------------------------------------------------------------

#include "Win32_Debug.h"

//---------------------------------------------------------------
//	Data alignment and various memory macros.
//---------------------------------------------------------------

namespace abc {

// Minimum memory alignment that is possible, in bytes.
enum { MINIMUM_ALIGNMENT = 4 };

// Memory allocations are aligned on this boundary by default.
enum { DEFAULT_MEMORY_ALIGNMENT = 8 };

enum { EFFICIENT_ALIGNMENT = 16 }; // The most efficient memory alignment.

__forceinline bool G_IsValidAlignment( size_t alignmentInBytes )
{
	return (alignmentInBytes >= MINIMUM_ALIGNMENT)
		&& (alignmentInBytes & (alignmentInBytes - 1)) == 0;	// alignment must be a power of two
}

__forceinline void Prefetch( void const* ptr )
{
	(void)* ( char const volatile* ) ptr;
}

}//End of namespace abc

MX_WARNING( "These memory alignment macros may not work with GNU compilers" )

#ifdef __GNUC__
#pragma warning The memory alignment macros may not work!
#endif

#ifndef UNALIGNED
#define UNALIGNED( x )	__unaligned( x )
#endif

#define ALIGN( bytes )	__declspec(align( bytes ))
#define ALIGN_4			__declspec(align( 4 ))
#define ALIGN_16		__declspec(align( 16 ))
#define ALIGN_128		__declspec(align( 128 ))

// Align by the size of cache line.
#define ALIGN_BY_CACHE_LINE		__declspec(align( CACHE_LINE_SIZE ))

#define IS_8_BYTE_ALIGNED( pointer )		(( (UINT_PTR)(pointer) & 7) == 0)
#define IS_16_BYTE_ALIGNED( pointer )		(( (UINT_PTR)(pointer) & 15) == 0)
#define IS_32_BYTE_ALIGNED( pointer )		(( (UINT_PTR)(pointer) & 31) == 0)

// Get the alignment of the passed type/variable, in bytes.
//	BUG: there's a bug in MSVC++: http://old.nabble.com/-Format--Asserts-with-non-default-structure-packing-in-1.37-(MSVC)-td21215959.html
//	_alignof does correctly report the alignment of 2-byte aligned objects,
//	but that only the alignment of structures can be specified by the user.
#define ALIGNMENT( x )		__alignof( x )

// These macros change alignment of the enclosed class/struct.
#define MX_SET_PACKING( byteAlignment )		MX_PRAGMA( pack( push, byteAlignment ) )
#define MX_END_PACKING						MX_PRAGMA( pack( pop ) )

//---------------------------------------------------------------

namespace abc {

FORCEINLINE mxInt MemCmp( const void* pMem1, const void* pMem2, mxSizeT numBytes )
{
	return ::memcmp( pMem1, pMem2, numBytes );
}

//
//	NOTE: 'pDestAddress' and 'pSrcAddress' must not point at the same memory location!
//
FORCEINLINE void MemCopy( void* __restrict pDestAddress, const void* __restrict pSrcAddress, mxSizeT numBytes )
{
	assert( pDestAddress && pSrcAddress );
	assert( pDestAddress != pSrcAddress );
	::memcpy( pDestAddress, pSrcAddress, numBytes );
}

FORCEINLINE void MemMove( void* __restrict pDestAddress, const void* __restrict pSrcAddress, mxSizeT numBytes )
{
	::memmove( pDestAddress, pSrcAddress, numBytes );
}

FORCEINLINE void MemSet( void* pAddress, UDWORD value, mxSizeT numBytes )
{
	::memset( pAddress, value, numBytes );
}

FORCEINLINE void MemZero( void* pAddress, mxSizeT numBytes )
{
	::memset( pAddress, 0, numBytes );
}

//
// Stack memory allocation
//
FORCEINLINE void* StackAlloc( mxSizeT numBytes )
{
	assert( numBytes > 0 );
	return _alloca( (numBytes + 7) & ~7 );
}

FORCEINLINE void StackFree( void* pMem )
{
	// nothing
}

}//end of namespace abc

//---------------------------------------------------------------
//	Threading.
//---------------------------------------------------------------

#include "Win32_Threading.h"

//---------------------------------------------------------------
//	Miscelaneous.
//---------------------------------------------------------------

namespace abc {

namespace sys {

//
//	File system.
//

// Maximum length a path name can have.
enum { MAX_PATH_CHARS = (MAX_PATH) };

// Maximum length a file name can have.
enum { MAX_FILENAME_CHARS = 256 };	// Just in case (255 is enough on Windows).

//---------------------------------------------------------------

// [Windows only] Returns a handle to the main application window.
HWND	GetWindowHandle();

// [Windows only] Returns the handle to the application's instance.
//HINSTANCE GetApplicationHInstance();

}//End of namespace sys

}//End of namespace abc


//---------------------------------------------------------------
//	Utilities.
//---------------------------------------------------------------

#include "Win32_Timer.h"
#include "Profiler.h"

#endif // ! __MX_PLATFORM_WINDOWS_32_COMPILER_MVCpp_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
