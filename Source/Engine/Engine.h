/*
=============================================================================
	File:	Engine.h
	Desc:	Public engine header file.
=============================================================================
*/

#ifndef __MX_PUBLIC_SHARED_ENGINE_H__
#define __MX_PUBLIC_SHARED_ENGINE_H__

//#include <Base.h>	// Base system.

//--------------------------------------------------

//
// Preprocessor settings
//

//#define ENGINE_BUILD_DLL

#ifdef ENGINE_BUILD_DLL
	#ifdef ENGINE_EXPORTS
	//	#pragma message("Compiling the engine DLL...")
		#define ENGINE_API	__declspec(dllexport)
	#else
	//	#pragma message("Using the engine DLL...")
		#define ENGINE_API	__declspec(dllimport)
	#endif
#else
	#define ENGINE_API
#endif

//--------------------------------------------------

// Application framework.
#include <Application/UserInput.h>
#include <Application/Application.h>

// Render system.
#include <Renderer/Texture.h>
#include <Renderer/Material.h>
#include <Renderer/Geometry.h>
#include <Renderer/Renderer.h>
#include <Renderer/DebugDrawer.h>
#include <Renderer/Messaging.h>

// Main engine.
#include <Engine/MainEngine.h>
#include <Engine/Resource.h>
#include <Engine/SystemLayer.h>
#include <Engine/DLLSupport.h>

// Scene and entity management.
#include <Scene/EntitySystem.h>
#include <Scene/Scene.h>
#include <Scene/SpatialQuery.h>
#include <Scene/SpatialProxy.h>
#include <Scene/SpatialDatabase.h>
#include <Scene/SpatialDatabase_Simple.h>

#endif // !__MX_PUBLIC_SHARED_ENGINE_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
