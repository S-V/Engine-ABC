/*
=============================================================================
	File:	BuildSettings_Debug.h
	Desc:	Build options, preprocessor settings for compiling debug versions.
=============================================================================
*/

#ifndef __MX_BUILD_SETTINGS_DEBUG_H__
#define __MX_BUILD_SETTINGS_DEBUG_H__

//----------------------------------------------
//	Development.
//----------------------------------------------

#define MX_ENABLE_PROFILING		// Profile and collect data for analysis.
#define MX_DEVELOPER			// Emit code for statistics, testing, in-game profiling and debugging, developer console, etc.

//----------------------------------------------
//	Memory.
//----------------------------------------------

#define MX_DEBUG_MEMORY			// Track each memory allocation/deallocation, detect memory leaks and so on.
#define MX_OVERRIDE_NEWDELETE	// Override the global 'new' and 'delete'.

#endif // !__MX_BUILD_SETTINGS_DEBUG_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
