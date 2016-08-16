/*
=============================================================================
	File:	MainEngine.cpp
	Desc:	Main engine class, implementation.
=============================================================================
*/
#include <precompiled.h>
#pragma hdrstop
#include <Engine.h>

namespace abc {

/*================================
		mxEngine
================================*/

mxEngine::mxEngine()
{
	// singleton
	ENSURE_ONE_CALL;
}

//
//	mxEngine::Initialize
//
void mxEngine::Initialize()
{
	// Initialize the global math.
	if ( ! mxMath::IsInitialized() )
	{
		mxMath::Init();
	}

	if( null == fileSys ) {
		fileSys = MX_NEW mxFileSystem();
	}

	if ( entitySystem.IsNull() )
	{
		entitySystem = MX_NEW mxEntitySystem();
		entitySystem->Initialize();
	}

	// TODO: analyze current hardware, maybe, run performance measurements, perform test runs
	// and determine optimal num triangles per batch, best CPU/GPU strategy, best perf settings, etc.
}

//
//	mxEngine::Shutdown
//
void mxEngine::Shutdown()
{
	// Delete all scenes.
	for ( mxUInt iScene = 0; iScene < allScenes.Num(); iScene++ )
	{
		mxScene * pScene = allScenes[ iScene ];
		pScene->Shutdown();
		MX_FREE( pScene );
	}
	allScenes.Clear();

	if ( entitySystem.IsValid() )
	{
		entitySystem->Shutdown();
		MX_FREE( entitySystem );
		entitySystem = null;
	}

	if( null != fileSys ) {
		MX_FREE( fileSys );
		fileSys = null;
	}
}

//
//	mxEngine::CreateScene
//
mxScene* mxEngine::CreateScene( const mxSceneDescription& creationInfo )
{
	mxScene * pNewScene = MX_NEW mxScene();
	pNewScene->Initialize( creationInfo );

	allScenes.Append( pNewScene );
	return pNewScene;
}

mxScene * mxEngine::CreateScene()
{
	return CreateScene( mxSceneDescription() );
}

//
//	mxEngine::GetNumScenes
//
mxUInt mxEngine::GetNumScenes() const
{
	return allScenes.Num();
}

//
//	mxEngine::Tick
//
void mxEngine::Tick( const mxTime elapsedTime )
{
	MX_PROFILE( "mxEngine::Tick" );

	GetEntitySystem().Tick( elapsedTime );

	// Update all scenes.
	for ( IndexT iScene = 0; iScene < allScenes.Num(); iScene++ )
	{
		mxScene * pScene = allScenes[ iScene ];

		// Update.
		pScene->Tick( elapsedTime );

		// Render.
		pScene->Present();
	}
}

//
//	mxEngine::HandleInputEvent
//
void mxEngine::HandleInputEvent( const mxInputEvent& rEvent )
{
	(void) rEvent;
}

//
//	mxEngine::HandleMessage
//
void mxEngine::HandleMessage( const TMessagePtr& msg )
{
	(void) msg;
}

//
//	mxEngine::Validate
//
bool mxEngine::Validate()
{
	// Validate all created scenes.
/*	for ( IndexT iScene = 0; iScene < allScenes.Num(); iScene++ )
	{
		mxScene * pScene = allScenes[ iScene ];
		pScene->Validate();
	}
*/
	return true;
}

}//End of namespace abc

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
