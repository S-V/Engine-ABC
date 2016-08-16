/*
=============================================================================
	File:	MainEngine.h
	Desc:	Main engine class description.
=============================================================================
*/

#ifndef __MX_MAIN_ENGINE_H__
#define __MX_MAIN_ENGINE_H__

namespace abc {

// Forward declarations.
class rxRenderer;
class rxDebugDrawer;

class mxSoundSystem;
class mxSystemLayer;
class mxGUI;
class mxConsole;
class mxMaterialSystem;
class mxFilePath;
class mxMeshBuilder;
class mxEntitySystem;
class mxFileSystem;
class mxScene;
class mxSceneDescription;

//
//	mxEngine - from here almost all subsystems can be reached.
//
class ENGINE_API mxEngine
{
public:

	//
	// Singleton access - returns a reference to the unique instance of mxEngine.
	//
	static mxEngine & get();

	//--- Common engine interface ----------------------------------------------------

			// Executes the main frame loop.
			// Equivalent to "while(!done){Tick();}".
			// returns 0 o
	int		Run();

	//--- Graphics system -----------------------------------------------------------

	rxRenderer &	GetRenderer();	// render system

	//--- Scene management ----------------------------------------------------------

				// Creates a new scene based on the given description.
	mxScene *	CreateScene( const mxSceneDescription& creationInfo );

				// Create a new scene with default settings. This function is provided for convenience.
	mxScene *	CreateScene();

				// Returns the total number of created scenes.
	mxUInt		GetNumScenes() const;

	mxEntitySystem &	GetEntitySystem();

	//--- File system ---------------------------------------------------------------

	mxFileSystem &		GetFileSystem();

	//--- Materials -----------------------------------------------------------------

	//mxMaterialSystem &	GetMaterialSystem();

	//--- Utilities -----------------------------------------------------------------


	//--- Checks --------------------------------------------------------------------

			// Check if the scene is malformed, objects have improper materials, etc.
			// May print warnings ( and errors and return false ).
	bool	Validate();

private:
	friend class mxSystem;

	//-------------------------------------------------------------------------------
	//	Internal functions.
	//-------------------------------------------------------------------------------

	//
	//	Construction/Initialization/Destruction.
	//
			mxEngine();	// NOTE: Must only be called by the system.

			// NOTE: Must only be called by the system.
	void	Initialize();

			// NOTE: Must only be called by the system.
	void	Shutdown();

	//--- Thinking and game ticking -------------------------------------------------

	// Advances all scenes by the specified amount of time.
	void	Tick( const mxTime deltaTime );	// NOTE: Should only be called by the system.

	//--- Event processing ----------------------------------------------------------

			// Respond to user input events.
	void	HandleInputEvent( const mxInputEvent& rEvent );	// NOTE: Should only be called by the system.

	void	HandleMessage( const TMessagePtr& msg );

private:
	TPtr< rxRenderer >			renderer;
	TPtr< mxFileSystem >		fileSys;
	TPtr< mxEntitySystem >		entitySystem;		// Entity manager.
	TArray< mxScene* >			allScenes;			// All created scenes.
	//TPtr< mxMaterialSystem >	materialSystem;		// Material database.
};

FORCEINLINE rxRenderer & mxEngine::GetRenderer() {
	return *renderer;
}

FORCEINLINE mxEntitySystem & mxEngine::GetEntitySystem() {
	return *entitySystem;
}

FORCEINLINE mxFileSystem & mxEngine::GetFileSystem() {
	return *fileSys;
}

}//End of namespace abc

#endif // ! __MX_MAIN_ENGINE_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
