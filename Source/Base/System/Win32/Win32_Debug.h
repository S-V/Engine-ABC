/*
=============================================================================
	File:	Win32_Debug.h
	Desc:	These can be very useful for debugging.
=============================================================================
*/

#pragma pack( push, 8 )
	#include <DbgHelp.h>
#pragma pack( pop )

//------------------------------------------------------------------
//		Defines.
//------------------------------------------------------------------

// These are removed in release builds.
#ifdef MX_DEBUG
	#define BREAK_IF( _CONDITION_ )		if (_CONDITION_) { _asm int 3 }
	#define DEBUG_BREAK					{ _asm int 3 }
#else // ifndef MX_DEBUG
	#define BREAK_IF( _CONDITION_ )		NOOP
	#define DEBUG_BREAK
#endif // ! MX_DEBUG

//------------------------------------------------------------------
//		Functions.
//------------------------------------------------------------------

namespace abc {
namespace sys {

// Causes a breakpoint exception to occur.
FORCEINLINE void DebugBreak()
{
	__asm { int 3 }
}

}//End of namespace sys
}//End of namespace abc

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
