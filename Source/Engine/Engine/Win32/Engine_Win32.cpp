/*
=============================================================================
	File:	mxEngine.cpp
	Desc:	Main engine class, implementation.
=============================================================================
*/
#include <precompiled.h>
#pragma hdrstop
#include <Engine.h>

#include <Renderer/D3D/D3DRenderSystem.h>

#include <fcntl.h>

namespace abc {

//
//	mxSystem
//
class mxSystem {
public:
			mxSystem();
			~mxSystem();

	//
	// Overrides ( mxSystem ) :
	//
	bool	Create( const mxSystemCreationInfo& creationParams );
	void	EnterMainLoop();
	void	Shutdown();

	//-------------------------------------------------------
	// Event handling.

	void	HandleInputEvent( const mxInputEvent& rEvent );

	LRESULT WINAPI MsgProc( HWND hWnd, mxUInt message, WPARAM wParam, LPARAM lParam );

	//-------------------------------------------------------

	HWND	GetWindowHandle() { return m_hWnd; }

	HINSTANCE	GetHInstance() { return m_hInstance; }

	bool	IsInitialized() const { return m_bIsInitialized; }

	void	SetActive( bool bActive ) { m_bIsActive = bActive; }

	mxUInt	GetFrameCount() const;		// Returns the total number of frames rendered.
//	mxUInt	GetCurrentTimeMs() const;	// Returns the current (virtual) time elapsed since the application started, in milliseconds.
	mxUInt	GetLastFrameTimeMs() const;	// Returns the amount of time in which the last frame was rendered.

	mxUInt	GetAverageFPS() const;
	mxUInt	GetInstantFPS() const;

	//
	//	Miscellaneous.
	//

	// Returns 'true' if the application is already running in the main loop.
	mxBool	IsRunning() const;

	mxBool	IsPaused() const { return !this->IsRunning(); }


	mxEngine &	GetEngine();

private:

	// Internal functions.

			// Processes a single game frame.
	void	RunFrame();

	void	HandleMessage( const TMessagePtr& msg );

	// returns null on failure
	rxRenderer* InitD3D10Driver( const rxRenderDeviceCreationInfo& cInfo );

private:
	// Explicitly create a console window for simple text I/O.
	void	CreateConsoleWindow();

	void	Error( const char* format, ... ) ATTRIBUTE( (format(printf,1,2)) );

private:
	TPtr< mxApplication >	m_userApp;

	mxEngine			m_engine;

private:
	//
	//	Timing.
	//
	mxUInt	m_TickCountOnLaunch;	// Number of ticks since the last system reset when the application started.

	bool	m_bIsActive;	// False if execution is temporarily suspended (e.g. if the main window is minimized or out of focus, etc).

	

	// Number of frames rendered since the start of the application.
	// This value will be strictly increasing.
	mxUInt		frameCount;

	// Current virtual time (elapsed since the application was started), in milliseconds.
	// This value will be strictly increasing.
	mxUInt		currentTime;

	// Time when the last frame ended, used for calculating delta time values, in milliseconds.
	// This value will be strictly increasing.
	mxUInt		lastTime;

	// Time, elapsed since the last frame, in milliseconds.
	mxUInt		lastFrameTime;

	//
	// Performance counters.
	//
	mxUInt		averageFPS;	// time-smoothed FPS (sampled over all frames)
	mxUInt		instantFPS;

	/*volatile*/ mxBool	isRunning;	// true if the application is already running

	//---------------------------------------

	TPtr< rxRenderer >	renderSystem;

	//---------------------------------------

	HWND		m_hWnd;			// Handle to the main app window.
	HINSTANCE	m_hInstance;	// Handle to the app instance.

private:
	//
	//	User input events handling.
	//

	// For unbuffered input event handling.

	BYTE	m_KeyStates[ EKeyCode::NumKeyCodes ];
	mxUInt	m_nLastMouseX, m_nLastMouseY;

	mxInputEvent	m_lastInputEvent;

private:
	// This will be 'true' if the system has been successfully initialized.
	bool	m_bIsInitialized;
};


FORCEINLINE mxUInt mxSystem::GetFrameCount() const
{ return this->frameCount; }

//FORCEINLINE mxUInt mxSystem::GetCurrentTimeMs() const
//{ return this->currentTime; }

FORCEINLINE mxUInt mxSystem::GetLastFrameTimeMs() const
{ return this->lastFrameTime; }


FORCEINLINE mxUInt mxSystem::GetAverageFPS() const
{	return this->averageFPS; }

FORCEINLINE mxUInt mxSystem::GetInstantFPS() const
{	return this->instantFPS; }


FORCEINLINE mxBool mxSystem::IsRunning() const
{	return this->isRunning; }

//==================================================================
//
//	Global variables
//
//==================================================================

namespace sys {
namespace win32
{
	mxSystem	TheSystem;	// The one and only instance of the system.

}//end of namespace win32
}//end of namespace sys

//
//	Global functions.
//

//
//	G_GetSystem()
//
//mxSystem &	G_GetSystem()
//{
//	return sys::win32::TheSystem;
//}

//
//	mxEngine::get()
//
mxEngine & mxEngine::get()
{
	return sys::win32::TheSystem.GetEngine();
}

//==================================================================

namespace sys {

HWND GetWindowHandle()
{
	return win32::TheSystem.GetWindowHandle();
}

void GetWindowPosition( mxUInt &Left, mxUInt &Top, mxUInt &Right, mxUInt &Bottom )
{
	RECT  rect;
	::GetWindowRect( GetWindowHandle(), &rect );

	Left	= rect.left;
	Top		= rect.top;
	Right	= rect.right;
	Bottom	= rect.bottom;
}

void SetWindowCaption( const mxChar* newCaption )
{
	DWORD dwResult;
	::SendMessageTimeout(
		GetWindowHandle(), WM_SETTEXT, 0,
		reinterpret_cast<LPARAM>( newCaption ),
		SMTO_ABORTIFHUNG, 2000, &dwResult
	);
}

void SetMouseCursorPositionRel( mxUInt x, mxUInt y )
{
	POINT point;
	point.x = x;
	point.y = y;

	::ClientToScreen( GetWindowHandle(), &point );

	::SetCursorPos( point.x, point.y );
}

mxUInt GetFrameCount()
{
	return win32::TheSystem.GetFrameCount();
}
mxUInt GetLastFrameTimeMs()
{
	return win32::TheSystem.GetLastFrameTimeMs();
}
mxUInt GetAverageFPS()
{
	return win32::TheSystem.GetAverageFPS();
}
mxUInt GetInstantFPS()
{
	return win32::TheSystem.GetInstantFPS();
}

}//end of namespace sys

//==================================================================

namespace sys {
namespace win32 {

//
//	G_WindowsMsgProc - main window message handling procedure.
//
FORCEINLINE
LRESULT WINAPI G_WindowsMsgProc( HWND hWnd, mxUInt message, WPARAM wParam, LPARAM lParam )
{
	return TheSystem.MsgProc( hWnd, message, wParam, lParam );
}

}//end of namespace win32
}//end of namespace sys

//
//	mxSystem::MsgProc
//
LRESULT WINAPI mxSystem::MsgProc( HWND hWnd, mxUInt message, WPARAM wParam, LPARAM lParam )
{
	// For mouse input events.
	mxMouseEvent & lastMouseEvent = m_lastInputEvent.Mouse;

	switch ( message )
	{

	//------------------------------------------------------------------------
	//	Keyboard events.
	//------------------------------------------------------------------------

#pragma region Keyboard Events

	case WM_KEYDOWN:
	case WM_KEYUP:
		{
			m_lastInputEvent.Type = EInputEvent::InputEvent_Keyboard;

			mxKeyboardEvent & lastKeyboardEvent = m_lastInputEvent.Keyboard;

			lastKeyboardEvent.Key			= ( EKeyCode ) wParam;
			lastKeyboardEvent.PressedDown = (message == WM_KEYDOWN);

			WORD KeyAsc = 0;
			GetKeyboardState( (::PBYTE) m_KeyStates );
			ToAscii( (::UINT)wParam, (::UINT)lParam, (const ::BYTE* )m_KeyStates, (::LPWORD) &KeyAsc, 0 );

			lastKeyboardEvent.Shift	= ( (m_KeyStates[VK_SHIFT] & 0x80) !=0 );
			lastKeyboardEvent.Control = ( (m_KeyStates[VK_SHIFT] & 0x80) !=0 );

			lastKeyboardEvent.CharCode = ( (m_KeyStates[VK_SHIFT] & 0x80) !=0 );

			//***************
			HandleInputEvent( m_lastInputEvent );
		}
		return 0;

#pragma endregion

	//------------------------------------------------------------------------
	//	Mouse events
	//------------------------------------------------------------------------

#pragma region Mouse Events

	case WM_MOUSEMOVE:
		{
			m_lastInputEvent.Type = EInputEvent::InputEvent_Mouse;

			lastMouseEvent.Type = mxMouseEvent::Cursor_Moved;

			lastMouseEvent.DeltaX = (short) LOWORD( lParam ) - m_nLastMouseX;
			lastMouseEvent.DeltaY = (short) HIWORD( lParam ) - m_nLastMouseY;

			m_nLastMouseX = lastMouseEvent.X	= (short) LOWORD( lParam );
			m_nLastMouseY = lastMouseEvent.Y	= (short) HIWORD( lParam );

			HandleInputEvent( m_lastInputEvent );
		}
		return 0;
/**/
	case WM_MOUSEWHEEL:
		{
			m_lastInputEvent.Type = EInputEvent::InputEvent_Mouse;

			lastMouseEvent.Type = mxMouseEvent::Wheel_Moved;
			lastMouseEvent.WheelDelta = (FLOAT( (short)HIWORD( wParam ) )) / FLOAT( WHEEL_DELTA );

			POINT p = { 0, 0 };
			ClientToScreen( hWnd, &p );

			lastMouseEvent.X	= LOWORD( lParam ) - p.x;
			lastMouseEvent.Y	= HIWORD( lParam ) - p.y;

			HandleInputEvent( m_lastInputEvent );
		}
		return 0;
/**/
	case WM_LBUTTONDOWN:
		{
			m_lastInputEvent.Type = EInputEvent::InputEvent_Mouse;

			lastMouseEvent.Type = mxMouseEvent::LeftButton_PressedDown;

			lastMouseEvent.X	= (short) LOWORD( lParam );
			lastMouseEvent.Y	= (short) HIWORD( lParam );

			HandleInputEvent( m_lastInputEvent );
		}
		return 0;

	case WM_LBUTTONUP:
		{
			m_lastInputEvent.Type = EInputEvent::InputEvent_Mouse;

			lastMouseEvent.Type = mxMouseEvent::LeftButton_LeftUp;
			lastMouseEvent.X	= (short) LOWORD( lParam );
			lastMouseEvent.Y	= (short) HIWORD( lParam );

			HandleInputEvent( m_lastInputEvent );
		}
		return 0;

	case WM_RBUTTONDOWN:
		{
			m_lastInputEvent.Type = EInputEvent::InputEvent_Mouse;

			lastMouseEvent.Type = mxMouseEvent::RightButton_PressedDown;
			lastMouseEvent.X	= (short) LOWORD( lParam );
			lastMouseEvent.Y	= (short) HIWORD( lParam );

			HandleInputEvent( m_lastInputEvent );
		}
		return 0;

	case WM_RBUTTONUP:
		{
			m_lastInputEvent.Type = EInputEvent::InputEvent_Mouse;

			lastMouseEvent.Type = mxMouseEvent::RightButton_LeftUp;
			lastMouseEvent.X	= (short) LOWORD( lParam );
			lastMouseEvent.Y	= (short) HIWORD( lParam );

			HandleInputEvent( m_lastInputEvent );
		}
		return 0;

	case WM_MBUTTONDOWN:
		{
			m_lastInputEvent.Type = EInputEvent::InputEvent_Mouse;

			lastMouseEvent.Type = mxMouseEvent::MiddleButton_PressedDown;
			lastMouseEvent.X	= (short) LOWORD( lParam );
			lastMouseEvent.Y	= (short) HIWORD( lParam );

			HandleInputEvent( m_lastInputEvent );
		}
		return 0;

	case WM_MBUTTONUP:
		{
			m_lastInputEvent.Type = EInputEvent::InputEvent_Mouse;

			lastMouseEvent.Type = mxMouseEvent::MiddleButton_LeftUp;
			lastMouseEvent.X	= (short) LOWORD( lParam );
			lastMouseEvent.Y	= (short) HIWORD( lParam );

			HandleInputEvent( m_lastInputEvent );
		}
		return 0;

#pragma endregion

	//------------------------------------------------------------------------
	//	Miscellaneous events.
	//------------------------------------------------------------------------
	case WM_CREATE:
		// the window has just been created...
		return 0;

	case WM_DESTROY:
		PostQuitMessage( 0 );
		return 0;

	case WM_SIZE:
		{
			/* wParam values:
				SIZE_MAXHIDE
				Message is sent to all pop-up windows when some other window is maximized.
				
				SIZE_MAXIMIZED
				The window has been maximized.
				
				SIZE_MAXSHOW
				Message is sent to all pop-up windows when some other window has been restored to its former size.
				
				SIZE_MINIMIZED
				The window has been minimized.
				
				SIZE_RESTORED
				The window has been resized, but neither the SIZE_MINIMIZED nor SIZE_MAXIMIZED value applies.

				lParam values:
				The low-order word of lParam specifies the new width of the client area. 
				The high-order word of lParam specifies the new height of the client area. 

			*/
			// deactivate if the window was minimized
			if ( wParam == SIZE_MINIMIZED ) {
				SetActive( false );
			} else {
				SetActive( true );
			}

			rxMessage_OnWindowResized  msg(
				(short) LOWORD( lParam ),	// width
				(short) HIWORD( lParam )	// height
			);
			this->HandleMessage( &msg );
		}
		return 0;

	case WM_ENTERMENULOOP:
	case WM_ENTERSIZEMOVE:
		SetActive( false );
		return 0;

	case WM_EXITMENULOOP:
	case WM_EXITSIZEMOVE:
		SetActive( true );
		return 0;

	case WM_ACTIVATEAPP:
		if ( wParam == TRUE ) {
			SetActive( true );
		}
		return 0;

	// WM_ACTIVATE is sent when the window is activated or deactivated.  
	// We pause the game when the window is deactivated and unpause it 
	// when it becomes active.
	case WM_ACTIVATE:
		if( LOWORD( wParam ) == WA_INACTIVE ) {
			SetActive( false );
		} else {
			SetActive( true );
		}
		return 0;

	case WM_SYSCOMMAND:
		// Prevent screensaver or monitor powersave mode from starting.
		if ((wParam & 0xFFF0) == SC_SCREENSAVE ||
			(wParam & 0xFFF0) == SC_MONITORPOWER)
		{
			return 0;
		}
		break;
	}

	return DefWindowProc( hWnd, message, wParam, lParam );
}

/*================================
		mxSystem
================================*/

mxSystem::mxSystem()
	: m_hWnd( null )
	, m_hInstance( null )

	, m_bIsActive( true )

	, m_bIsInitialized( false )
{
	this->frameCount	= 0;
	this->currentTime	= 0;
	this->lastTime		= 0;
	this->lastFrameTime	= 0;

	this->averageFPS = this->instantFPS = 0;

	this->isRunning = false;
}

mxSystem::~mxSystem()
{
	Shutdown();

#ifdef MX_DEBUG
	{
		const mxUInt totalNumRefs = ReferenceCounted::GetTotalReferenceCount();
		sys::Print( "ReferenceCounted : TotalReferenceCount = %u\n", totalNumRefs );
		Assert2( totalNumRefs == 0,
			"All reference counted objects should have been released at this point!" );
		const mxUInt totalNumEntities = mxEntity::GetTotalCount();
		sys::Print( "Entity : GetTotalCount = %u\n", totalNumEntities );
	}
#endif
}

//
//	mxSystem::Create
//
bool mxSystem::Create( const mxSystemCreationInfo& creationInfo )
{
	if ( this->IsInitialized() ) {
		return true;
	}

	Platform_Init();

	//
	// Init timers.
	//
	m_TickCountOnLaunch = (mxUInt)::GetTickCount();

	//
	//	Create a console window and redirect standard output stream to that window.
	//
	CreateConsoleWindow();

	//
	//	Print platform info.
	//
	{
		sys::SystemInfo_s  sysInfo;
		sys::GetSystemInfo( sysInfo );
		sys::Print( TEXT("OS: %s\n"), sysInfo.OSName );

		mxUInt  cpuFreq;
		sys::GetProcessorClockFrequency( cpuFreq );
		sys::Print( TEXT("CPU: %s, Num. cores = %u, CPU frequency = %u MHz, page size = %u bytes\n"),
			sys::GetCpuTypeName( sysInfo.Cpu ), sysInfo.NumProcessors, cpuFreq, sysInfo.ProcessorPageSize );
	}

	//
	//	Initialize the pointer to the user application.
	//
	if ( creationInfo.pUserApp == null )
	{
		::MessageBox( null, TEXT("null ptr to user application"), TEXT("Error"), MB_OK );
		return false;
	}
	m_userApp = creationInfo.pUserApp;
	AssertPtr( m_userApp );

	//
	// Create a window.
	//
	m_hInstance = GetModuleHandle( NULL );

	const mxChar * classname = TEXT("The Engine");

	const WNDCLASSEX wc = {
		sizeof( WNDCLASSEX ),
		CS_CLASSDC,
		sys::win32::G_WindowsMsgProc,
		0L, 0L,
		m_hInstance,
		LoadIcon( m_hInstance, MAKEINTRESOURCE(IDI_APPLICATION) ),
		LoadCursor( NULL, IDC_ARROW ),
		NULL, NULL,
		classname, NULL };

	RegisterClassEx( &wc );	// Register the window class ( adding our own icon to this window ).

	// Create the application's window - ensure the visible viewport starts at a given size.

	RECT initSize = {
		0,
		0,
		creationInfo.screen.width,
		creationInfo.screen.height
	};

	const DWORD windowStyle = WS_OVERLAPPEDWINDOW;
	AdjustWindowRect( &initSize, windowStyle, FALSE );

	m_hWnd = CreateWindow(
		classname,
		TEXT( "3D Engine" ),
		windowStyle,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		initSize.right - initSize.left,
		initSize.bottom - initSize.top,
		NULL,
		NULL,
		wc.hInstance,
		NULL
	);

	if ( NULL == m_hWnd )
	{
		::MessageBox( NULL, TEXT("Failed to create a window"), TEXT("Error!"), MB_OK );
		return false;
	}

	// Make the primary window active.
	ShowWindow( m_hWnd, SW_SHOW );
	UpdateWindow( m_hWnd );

	SetActiveWindow( m_hWnd );
	SetForegroundWindow( m_hWnd );

	//
	// Create the render system.
	//
	rxRenderer * renderSys = null;

	rxRenderDeviceCreationInfo  cInfo;

	cInfo.pWindowHandle = m_hWnd;
	cInfo.screen = creationInfo.screen;

	switch ( creationInfo.driverType )
	{
	case EDriverType::GAPI_None :
		Unimplemented;
		break;

#ifdef MX_D3DX9

	case EDriverType::GAPI_DirectX_9 :
		{
			Unimplemented;
			//using namespace abc::d3d;
			//using namespace abc::d3d9;

			//this->renderSystem = MX_NEW D3D9Renderer();

			//if ( ! this->renderSystem->Create( cInfo ) )
			//{
			//	Error( "Failed to initialize D3D9 device" );
			//	return;
			//}
		}
		break;

#endif // MX_D3DX9


#ifdef MX_D3DX10

	case EDriverType::GAPI_DirectX_10 :
		{
			renderSys = InitD3D10Driver( cInfo );
		}
		break;

#endif // MX_D3DX10

	default:
		Unimplemented;
		return false;

	}//switch

	if( null == renderSys )
	{
		sys::FatalError( TEXT("Failed to initialize %s graphics driver"),
												GetDriverName( creationInfo.driverType ) );
		return false;
	}

	this->renderSystem = renderSys;

	//*********************
//	::SetCursorPos( creationInfo.screen.width/2, creationInfo.screen.height/2 );
	m_nLastMouseX = m_nLastMouseY = 0;

	//
	// Init the engine.
	//

	m_engine.renderer = this->renderSystem;

	m_engine.Initialize();

#if 0
	// Set the initial app data dir.
	{
		m_engine.SetDataDirectory( sys::GetLauchDirectory() );	// Set this by default.

		// If the directory name was specified...
		if ( creationInfo.dataSourceDirectory != null )
		{
			if ( sys::IsValidPathName( creationInfo.dataSourceDirectory ) )
			{
				if ( sys::PathExists( creationInfo.dataSourceDirectory ) )
				{
					m_engine.SetDataDirectory( creationInfo.dataSourceDirectory );
				}
				else {
					sys::Warning( "path %s doesn't exist", creationInfo.dataSourceDirectory );
				}
			}
			else {
				sys::Warning( "%s is not a valid path name", creationInfo.dataSourceDirectory );
			}
		}
	}//End of App Data Dir Init
#endif

	//
	// Create the user application.
	//
	if ( ! m_userApp->Create() )
	{
		::MessageBox( NULL, TEXT("Failed to launch the application."), TEXT("Error!"), MB_OK );
		return false;
	}

	if ( ! m_engine.Validate() )
	{
		::MessageBox( NULL, TEXT("The scene is malformed."), TEXT("Error!"), MB_OK );
		return false;
	}

	m_bIsInitialized = true;

	sys::Print( "System init in %u seconds\n", ((mxUInt)::GetTickCount() - m_TickCountOnLaunch) / 1000 );

	return true;
}

//
//	mxSystem::EnterMainLoop
//
void mxSystem::EnterMainLoop()
{
	if ( IsRunning() ) {
		DEBUG_BREAK
		sys::DbgOut( "The game is already running!" );
		return;
	}
/*
	// Make sure that we have something to simulate and render.
	if ( GetEngine().GetNumScenes() == 0 )
	{
		sys::ShowMessage( "Error", "No scenes are loaded. The application will now exit." );
		Shutdown();
		return;
	}
*/
	// Let the engine know we're about to enter the main game loop.
	{
		isRunning = true;

		//mxEvent  newEvent;
		//newEvent.Type = EEventCode::EVENT_Engine_Initialized;
		//newEvent.Args[0].pAnyArg = &m_engine;

		//GetEngine().HandleEvent( newEvent );
	}

	{
		m_userApp->Loaded();
	}

	// Show the window.
	ShowWindow( m_hWnd, SW_SHOWDEFAULT );
	UpdateWindow( m_hWnd );

	// Enter the message loop.
	MSG msg;
	ZeroMemory( &msg, sizeof(msg) );
	while( msg.message != WM_QUIT )
	{
		if ( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		else
		{
			if ( m_bIsActive ) {
				// Main game loop.
				RunFrame();
			} else {
				::Sleep( 1000 );
			}
		}
	}
}

//
//	mxSystem::RunFrame
//
void mxSystem::RunFrame()
{
	MX_PROFILE( "RunFrame" );

	//************************************
	MX_PROFILE_SCOPE("UserApp : PreFrame()");
		m_userApp->PreFrame();
	MX_END_SCOPE;
	
	this->frameCount++;

	this->currentTime = (mxUInt) ::GetTickCount() - m_TickCountOnLaunch;

	// protect against errors because of time wraparound
	if ( currentTime > 0x70000000 )
	{
		sys::ShowMessage( "Error", "Shutting down due to time wrapping" );
		Shutdown();
		return;
	}

	this->lastFrameTime = this->currentTime - this->lastTime;
	this->lastTime = this->currentTime;

	mxTime  deltaTime;
	deltaTime.iTime = this->lastFrameTime;
	deltaTime.fTime = mxMath::IntToF( this->lastFrameTime );

	// Render and update the scene.
	{
		m_engine.Tick( deltaTime );
	}

	MX_TODO("25.10.09: create a separate thread for rendering")
	//{
	//	// Execute the render commands and issue draw calls.
	//	renderSystem->RenderScene( m_engine.GetActiveView() );
	//}

	//************************************

#ifdef MX_DEBUG
	//::Sleep( 20 );
	//::Sleep( 16 );
	::Sleep( 1 );
	//Yield();
#else
	Yield();
#endif

	//************************************
	MX_PROFILE_SCOPE("UserApp : PostFrame()");
		m_userApp->PostFrame( deltaTime );
	MX_END_SCOPE;

	// Update performance counters.
	{
		const mxUInt numSeconds = this->currentTime / 1024;
		averageFPS = GetFrameCount() / (numSeconds + 1);	// added 1 to avoid div by zero

		instantFPS = 1000 / (GetLastFrameTimeMs() + 1);
	}

#if defined( MX_ENABLE_PROFILING )
	CProfileManager::Increment_Frame_Counter();
#endif
}

//
//	mxSystem::Shutdown
//
void mxSystem::Shutdown()
{
	static bool bHasAlreadyBeenShutdown( false );	// true if this function has already been called

	if ( !IsInitialized() && !IsRunning() && !bHasAlreadyBeenShutdown )
	{
		sys::ShowMessage( "Invalid call", "Cannot shutdown the application during initialization" );
		return;
	}

	if ( !bHasAlreadyBeenShutdown )
	{
		if ( this->IsInitialized() )
		{
			isRunning = false;

			// Destroy the user application.
			if ( m_userApp ) {
				m_userApp->Destroy();
				m_userApp = null;
			}

			// Destroy the engine.
			m_engine.Shutdown();

			// Destroy the render device.
			if ( null != this->renderSystem ) {
				this->renderSystem->Destroy();
				this->renderSystem = null;
			}

			// Shutdown the global math.
			if ( mxMath::IsInitialized() )
			{
				mxMath::Shutdown();
			}

			// Destroy the application main window.
			if ( m_hWnd ) {
				DestroyWindow( m_hWnd );
				m_hWnd = NULL;
			}

			m_bIsInitialized = false;

		}//End if the app was running

		//
		// Dump stats if needed.
		//
		#if defined( MX_ENABLE_PROFILING )
			CProfileManager::dumpAll();
			CProfileManager::CleanupMemory();
		#endif

		// Destroy the type system.
		mxObjectFactory::Destroy();

		Platform_Shutdown();

		#ifdef MX_DEBUG
			MX_TODO("Here's a convenient place to dump memory leaks, resource usage, etc.")
			::MessageBox( GetWindowHandle(), TEXT("Done"),
				TEXT("Success"),
				MB_OK );
		#endif

		bHasAlreadyBeenShutdown = true;
	}

	// Make sure the mouse cursor is visible.
	sys::SetMouseCursorVisible( true );
}

//
//	mxSystem::HandleInputEvent
//
void mxSystem::HandleInputEvent( const mxInputEvent& rEvent )
{
	m_userApp->ProcessInput( rEvent );
	m_engine.HandleInputEvent( rEvent );

	// Handle ESC.
	if ( rEvent.Type == EInputEvent::InputEvent_Keyboard )
	{
		if ( rEvent.Keyboard.Key == EKeyCode::Key_Escape ) {
			this->Shutdown();
			return;
		}
	}
}

//
//	mxSystem::HandleMessage
//
void mxSystem::HandleMessage( const TMessagePtr& msg )
{
	if ( this->renderSystem != null )
	{
		this->renderSystem->HandleMessage( msg );
	}
	m_engine.HandleMessage( msg );
}

//
//	mxSystem::GetEngine
//
mxEngine & mxSystem::GetEngine()
{
	return m_engine;
}

//
//	mxSystem::CreateConsoleWindow
//
//	Creates a console window ( which can be a very useful tool for debugging )
//	and redirects standard C I/O streams to that window.
//
void mxSystem::CreateConsoleWindow()
{
	int  hConHandle;
	long lStdHandle;

	// Allocate a console for this app.
	AllocConsole();

	CONSOLE_SCREEN_BUFFER_INFO  coninfo;
	FILE * fp;

	// Set the screen buffer to be big enough to let us scroll text.
	GetConsoleScreenBufferInfo( GetStdHandle( STD_OUTPUT_HANDLE ), & coninfo );

	// Maximum number of lines the output console should have.
	enum { MAX_CONSOLE_LINES = 1024 };

	coninfo.dwSize.Y = MAX_CONSOLE_LINES;

	SetConsoleScreenBufferSize( GetStdHandle( STD_OUTPUT_HANDLE ), coninfo.dwSize );

	// Redirect unbuffered STDOUT to the console.
	lStdHandle = (long) GetStdHandle( STD_OUTPUT_HANDLE );
	hConHandle = _open_osfhandle( lStdHandle, _O_TEXT );
	fp = _fdopen( hConHandle, "w" );
	*stdout = *fp;
	setvbuf( stdout, NULL, _IONBF, 0 );

	// Redirect unbuffered STDERR to the console.
	lStdHandle = (long) GetStdHandle( STD_ERROR_HANDLE );
	hConHandle = _open_osfhandle( lStdHandle, _O_TEXT );
	fp = _fdopen( hConHandle, "w" );
	*stderr = *fp;
	setvbuf( stderr, NULL, _IONBF, 0 );
}

//
//	CreateEngine
//
mxEngine * CreateEngine( const mxSystemCreationInfo& cInfo )
{
	if( sys::win32::TheSystem.Create( cInfo ) ) {
		return & sys::win32::TheSystem.GetEngine();
	} else {
		return null;
	}
}

//
//	mxEngine::Run
//
int mxEngine::Run()
{
	sys::win32::TheSystem.EnterMainLoop();
	return 0;
}

//
//	mxSystem::InitD3D10Driver
//
rxRenderer* mxSystem::InitD3D10Driver( const rxRenderDeviceCreationInfo& cInfo )
{
	rxRenderer * r = MX_NEW D3D10Renderer();

	if ( r->Create( cInfo ) )
	{
		return r;
	}
	else
	{
		r->Destroy();
		return null;
	}
}

}//End of namespace abc

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
