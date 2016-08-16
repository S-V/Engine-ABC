/*
=============================================================================
	File:	Base.h
	Desc:	Base subsystem public header file.
=============================================================================
*/

#ifndef __MX_SHARED_BASE_H__
#define __MX_SHARED_BASE_H__

#include <Build/BuildDefines.h>		// Build configuration options, compile settings.
#include <Common/Macros.h>			// Common stuff.
#include <System/Platform.h>		// Platform-specific stuff.
#include <Debug/Debug.h>			// Helpers for debugging.
#include <Types/Types.h>			// Basic types.
#include <Types/Checks.h>			// Sanity types.

#include <Memory/Memory.h>			// Memory management.

#include <Lib/Lib.h>				// Foundation Library.

#include <Object/ObjectFactory.h>	// Class factory. NOTE: must be included before 'RTTI.h' !
#include <Object/RTTI.h>			// Run-Time Type Information.
#include <Object/Object.h>			// Base class. NOTE: must be included after 'RTTI.h' !
#include <Object/Message.h>			// Messaging.

#include <IO/DataStream.h>		// Input/Output system.
#include <IO/Files.h>			// File system.

#endif // !__MX_SHARED_BASE_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
