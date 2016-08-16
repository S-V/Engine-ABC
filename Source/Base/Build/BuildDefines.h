/*
=============================================================================
	File:	BuildDefines.h
	Desc:	Build options, preprocessor settings for compiling different versions.
	Note:	This header must be included in the first place !
=============================================================================
*/

#ifndef __MX_BUILD_DEFINES_H__
#define __MX_BUILD_DEFINES_H__

//----------------------------------------------
//	Debug/Release.
//----------------------------------------------

#if defined(_DEBUG)
	#define MX_DEBUG		// Debug build.
	#include "BuildSettings_Debug.h"
#else
	#include "BuildSettings_Release.h"
#endif// MX_DEBUG

//----------------------------------------------
//	Development.
//----------------------------------------------

#ifdef BASE_EXPORTS
	#define MX_BUILD_ENGINE_DLL	// Are we building a DLL ?
#endif

#define MX_COMPILE_WITH_DIRECTX9
#define MX_COMPILE_WITH_DIRECTX10
#define MX_COMPILE_WITH_OPENGL

// Also see MX_TARGET_DRIVER in Framework.h

#define MX_USE_SSE_ASM	// Use SIMD extensions.

//----------------------------------------------
//	Misc settings.
//----------------------------------------------

#define MX_ENGINE_VERSION	"0.0.0"	// For preventing file version problems, DLL hell, etc.

#define MX_DEMO_BUILD	// If this is defined, the executable will work only with demo data.

#endif // !__MX_BUILD_DEFINES_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
