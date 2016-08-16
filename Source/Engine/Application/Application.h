/*
=============================================================================
	File:	Application.h
	Desc:	User application skeleton.
=============================================================================
*/

#ifndef __MX_ENGINE_FRAMEWORK_APPLICATION_H__
#define __MX_ENGINE_FRAMEWORK_APPLICATION_H__

namespace abc {

// Forward declarations.
class mxInputEvent;
class mxEvent;
class mxEngine;
class mxApplication;

//
//	EDriverType - enumerates all supported graphics driver types.
//
enum EDriverType
{
	GAPI_None = 0,	// Doesn't use any graphics drivers, useful for applications when visualization is not needed.

	GAPI_DirectX_9,
	GAPI_DirectX_10,
//	GAPI_DirectX_11,

//	GAPI_OpenGL_3_2,
//	GAPI_OpenGL_4,

	GAPI_AutomaticSelection,		// Select the best GAPI automatically.

	NUM_GRAPHICS_APIs
};

const mxChar* GetDriverName( EDriverType driver );

//
// mxScreenSettings - Screen settings.
//
struct ENGINE_API mxScreenSettings
{
	mxUInt		width;
	mxUInt		height;
	bool		isFullscreen;

public:
		mxScreenSettings();

	void	SetDefaultValues();
	bool	IsValid() const;
};

//
//	mxGraphicsSettings - used for initializing the graphics device.
//
struct ENGINE_API mxGraphicsSettings
{
	mxScreenSettings	screen;
	EDriverType			driverType;

public:
			mxGraphicsSettings();

	void	SetDefaultValues();
	bool	IsValid() const;
};

//
//	mxSystemSettings - for initializing the base system.
//
struct ENGINE_API mxSystemSettings
{
	// if true then an CRT text console will be created.
	bool	bCreateConsole;

	// Work in progress...
	// Memory requirements, target frame rate, expected CPU load, etc.

	//bool	bCreateLog;
	// ...

public:
			mxSystemSettings();

	void	SetDefaultValues();
	bool	IsValid() const;
};

//
//	mxSystemCreationInfo - used for initializing the engine.
//
struct ENGINE_API mxSystemCreationInfo
	: public mxGraphicsSettings
	, public mxSystemSettings
{
	mxApplication *		pUserApp;	// pointer to the user application

public:
			mxSystemCreationInfo();

	void	SetDefaultValues();
	bool	IsValid() const;
};

//
// Initializes the engine and launches a new application.
//
extern "C" ENGINE_API
	mxEngine *
		MX_CALL CreateEngine( const mxSystemCreationInfo& );

//
//	mxApplication
//
//	User application classes should derive from this class.
//
class mxApplication : public IUserInputHandler
{
public:
	virtual		~mxApplication() {}

	//
	//	Initialization / Destruction.
	//

					// This function is typically used for loading the application.
					// Returns 'false' in case of failure.
	virtual bool	Create() { return true; }

	// This function gets called after Create() (usually when all resources have been loaded and scenes have been successfully created).
	virtual void	Loaded() {}

					// This function should be used to release all application resources and for shutting down.
	virtual void	Destroy() {}

	// These functions are called before and after each frame correspondingly.
	virtual void	PreFrame() {}
	virtual void	PostFrame( const mxTime elapsedTime ) {}

	//
	// Event handling.
	//

	override(IUserInputHandler)
	bool	ProcessInput( const mxInputEvent& rEvent ) { return false; };
};

}//End of namespace abc

#endif // ! __MX_ENGINE_FRAMEWORK_APPLICATION_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
