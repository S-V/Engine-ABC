/*
=============================================================================
	File:	Example_Basic.h
	Desc:	Base class for sample application classes.
=============================================================================
*/

#ifndef __EXAMPLE_BASIC_H__
#define __EXAMPLE_BASIC_H__

//
//	Spectator - represents a human spectator.
//
//	This class is used to conveniently view and interact with the scene graph.
//
class Spectator : public Node {
public:
	Spectator( mxCamera& theCamera, SceneGraph* smgr )
	{
		mSmgr = smgr;
		mCamera = mSmgr->AddCamera( theCamera );
	}
	~Spectator()
	{}

	// accepts a type of flashlight; this value can be 0 or 1
	void AttachFlashlight( int type = 0 )
	{
		LightCreationInfo  cInfo;
		cInfo.Type			= ELightType::Light_Spot;
		cInfo.Diffuse		= FColor::WHITE;
		cInfo.Intensity		= 1.0f;
		cInfo.Position		= Vec3D(1.2f,-0.7f,0.2f);
		cInfo.Direction		= Vec3D(0,0,1);
		cInfo.Range			= 100.0f;
		cInfo.SpotInnerAngle = DEG2RAD( 28.0f );
		cInfo.SpotOuterAngle = DEG2RAD( 30.0f );
		cInfo.bCastShadows	= true;

		rxResources & resources = mxEngine::get().GetRenderer().GetResources();

		//	cInfo.flare			= resources.LoadTexture("textures/light_corona0.png");

		cInfo.projector		= resources.LoadTexture(
			!type ?
			"textures/flashlight/flashlight_hl2.dds"
			:
			"textures/flashlight/flashlight_doom3.bmp"
		);

		mFlashlight = mSmgr->AddLight( cInfo );
		mCamera->AddKid( mFlashlight );
	}

	// switches the light on/off
	void ToggleFlashLight()
	{
		if( null != mFlashlight ) {
			mFlashlight->SetEnabled( !mFlashlight->IsEnabled() );
		}
	}

	override(Node) void _onWorldTransformChanged()
	{
		mCamera->GetAbsoluteTransform() = this->GetAbsoluteTransform();
	}

private:
	NodePtr< Camera >		mCamera;
	NodePtr< Light >		mFlashlight;
	RefPtr< SceneGraph >	mSmgr;
};

//
//	Sample_Basic
//
class Sample_Basic : public mxApplication
{
protected:
	TPtr< mxScene >			scene;		// Our scene.
	RefPtr< SceneGraph >	sceneMgr;	// Pointer to the scene graph manager.
	bool					bPaused;	// true if suspended scene graph animation

	// Camera for viewing the scene (controlled via mouse and keyboard).
	AutoPtr< mxCameraController >	camera;
	
	NodePtr< Spectator >	spectator;

public:
	Sample_Basic()
		: bPaused( false )
	{
		// Note: you cannot allocate any graphics resources here,
		// because the renderer has not been created yet.
	}
	//----------------------------------------------------------------------------------------------------
	virtual const mxChar* GetName() const { return TEXT("Empty"); }
	//----------------------------------------------------------------------------------------------------
	override( mxApplication ) bool Create()
	{
		mxEngine & engine( mxEngine::get() );

		this->scene = engine.CreateScene();
		this->sceneMgr = new SceneGraph( this->scene );

		// Create a camera for viewing the scene.
		{
			mxCamera & theCamera = this->scene->GetActiveCamera();

			// Create a simple camera controller.
			this->camera = new mxCameraController( theCamera );

			// Set initial camera position.
			this->camera->GetCamera().SetPosition( Vec3D( 0, 10, 0 ) );
			this->camera->GetCamera().SetLookAt( Vec3D::UNIT_Z );//Vec3D( 0.5f, 0.15f, 0.8f ) );

			this->spectator = MX_NEW Spectator( this->camera->GetCamera(), this->sceneMgr );
		}
		sys::Print( TEXT("\nScene created successfully!\n") );

		_initPaths();
		_loadMaterials();
		_initDebugUtils();

		PrintSysInfo();

		const mxScreenSettings  screen( engine.GetRenderer().GetInfo().DisplayMode.screen );
		sys::SetMouseCursorVisible( false );

	//	DEBUG_CODE( sys::Beep( 500 ) );	// wake me up when the whole thing compiles
		return true;
	}
	//----------------------------------------------------------------------------------------------------
	override( mxApplication ) void Destroy()
	{
		this->scene = null;
		this->sceneMgr = null;
		this->bPaused = false;

		this->camera = null;

		sys::SetMouseCursorVisible( true );
	}
	//----------------------------------------------------------------------------------------------------
	override( mxApplication ) void PreFrame()
	{
	}
	//----------------------------------------------------------------------------------------------------
	override( mxApplication ) void PostFrame( const mxTime elapsedTime )
	{
		if ( ! bPaused ) {
			this->sceneMgr->Update( elapsedTime );
		}
		this->camera->Update( elapsedTime );

		_updateStatCounters( elapsedTime );
	}
	//----------------------------------------------------------------------------------------------------
	override( mxApplication ) bool ProcessInput( const mxInputEvent& rEvent )
	{
		if ( rEvent.Type == EInputEvent::InputEvent_Keyboard )
		{
			if ( rEvent.Keyboard.PressedDown )
			{
				switch ( rEvent.Keyboard.Key )
				{
					// pause-resume
				case EKeyCode::Key_P :
					{
						this->bPaused ^= 1;
					}
					break;

				}//switch( key )

			}//End if key pressed down

		}//End of Keyboard events

		this->camera->HandleInputEvent( rEvent );

		return false;	// let the event be further processed
	}
	//----------------------------------------------------------------------------------------------------
	void PrintSysInfo()
	{
		rxRenderer & renderer = mxEngine::get().GetRenderer();
		const rxRendererInfo & info = renderer.GetInfo();

		printf_s( "\n/*=== GPU info ====================*/\n" );
		wprintf_s( L"GPU: %s\n", info.DeviceInfo.Description );
		wprintf_s( L"Vendor: %s\n", info.DeviceInfo.VendorName );
		printf_s( "VRAM: %d Mb total\n", info.DeviceInfo.Caps.TotalVideoRAM );
		printf_s( "Display mode: %dx%d\n", info.DisplayMode.screen.width, info.DisplayMode.screen.height );
		printf_s( "\n/*=================================*/\n" );
	}
	//----------------------------------------------------------------------------------------------------

protected:
	//----------------------------------------------------------------------------------------------------
	//	Utilities
	//----------------------------------------------------------------------------------------------------
	
	void PutOntoGroundPlane( Model* model, mxReal height = 0.0f )
	{
		mxBounds localBounds;
		model->GetSpatialProxy()->GetBoundsLocal( localBounds );

		Vec3D newOrigin( model->GetOrigin() );
		newOrigin.y += localBounds.GetHalfSize().y;
		model->SetOrigin( newOrigin );
	}
	Skybox* LoadSkybox_Sunset()
	{
		const char* textureFileNames[6 ] =
		{
			"skybox/sunset/front.bmp",
			"skybox/sunset/back.bmp",
			"skybox/sunset/up.bmp",
			"skybox/sunset/down.bmp",
			"skybox/sunset/left.bmp",
			"skybox/sunset/right.bmp",
		};

		rxResources & resources = mxEngine::get().GetRenderer().GetResources();

		rxTexture * skyboxTextures[6] = {null};
		for ( int i=0; i<6; i++ ) {
			skyboxTextures[i] = resources.LoadTexture( mxFilePath( textureFileNames[i] ) );
		}

		Skybox * skyNode = this->sceneMgr->AddSkybox( skyboxTextures );
		return skyNode;
	}
	Skybox* LoadSkybox_Night()
	{
		const char* textureFileNames[6 ] =
		{
			"skybox/starfield/front.png",
			"skybox/starfield/back.png",
			"skybox/starfield/up.png",
			"skybox/starfield/down.png",
			"skybox/starfield/left.png",
			"skybox/starfield/right.png",
		};

		rxResources & resources = mxEngine::get().GetRenderer().GetResources();

		rxTexture * skyboxTextures[6] = {null};
		for ( int i=0; i<6; i++ ) {
			skyboxTextures[i] = resources.LoadTexture( mxFilePath( textureFileNames[i] ) );
		}

		Skybox * skyNode = this->sceneMgr->AddSkybox( skyboxTextures );
		return skyNode;
	}

private:
	//----------------------------------------------------------------------------------------------------
	//	Initialization
	//----------------------------------------------------------------------------------------------------
	void _initPaths()
	{
		mxEngine & engine( mxEngine::get() );

		engine.GetFileSystem().AddDir(TEXT("../../Data/samples/"));
		engine.GetFileSystem().AddDir(TEXT("../../Data/samples/meshes/"));
		engine.GetFileSystem().AddDir(TEXT("../../Data/samples/images/"));
		engine.GetFileSystem().AddDir(TEXT("../../Data/samples/materials/"));

		//engine.GetMeshBuilder().AddMeshLoader( new MeshLoader_3DS() );
	}
	//----------------------------------------------------------------------------------------------------
	void _loadMaterials()
	{
		LoadMaterialFromFile(TEXT("stone.txt"));
		LoadMaterialFromFile(TEXT("metal.txt"));
		LoadMaterialFromFile(TEXT("misc.txt"));
	}

private:

	//
	// Testing and Debugging.
	//
	TPtr< rxDebugText >	debugText_CameraInfo;
	TPtr< rxDebugText >	debugText_RenderStats;

	//----------------------------------------------------------------------------------------------------
	void _initDebugUtils()
	{
		rxDebugDrawer & debugDrawer = mxEngine::get().GetRenderer().GetDebugDrawer();

		rxDebugTextDesc  desc;

		desc.X = 10, desc.Y = 0;
		desc.Color = FColor::WHITE;
		this->debugText_CameraInfo	= debugDrawer.CreateText( desc );

		desc.X = 10, desc.Y = 20;
		desc.Color = FColor::WHITE;
		this->debugText_RenderStats	= debugDrawer.CreateText( desc );

		rxDebugAxes * pDbgFOR = debugDrawer.CreateDebugAxes();
		pDbgFOR->SetTransform( Vec3D::ZERO, Quat::QUAT_IDENTITY );

		desc.X = 10, desc.Y = 80;
		desc.Color = FColor::YELLOW;
		desc.Text = "WASD - movement, press 'P' to toggle animation on/off";
		debugDrawer.CreateText( desc );
	}
	//----------------------------------------------------------------------------------------------------

	// Updates debug on-screen texts.
	//
	void _updateStatCounters( const mxTime elapsedTime )
	{
		static mxUInt  lastTimeMeasured = 0;

		const mxUInt  currTime = sys::GetClockTicks();

		// Update stats every MIN_UPDATES_INTERVAL milliseconds.
		enum { MIN_UPDATES_INTERVAL = 500 };

		if ( currTime - lastTimeMeasured > MIN_UPDATES_INTERVAL )
		{
			lastTimeMeasured = currTime;

			// Update render stats.
			{
				rxRenderer & renderer = mxEngine::get().GetRenderer();

				const rxStatistics & renderStats = renderer.GetStats();

				this->debugText_RenderStats->SetText(
					"Instant FPS: %u, Avg. FPS: %u, Frame time: %u milliseconds\n"
					"Num. Objects: %u, rendered: %u\n"
				//	"Num. Batches: %u\n"

					, sys::GetInstantFPS(), sys::GetAverageFPS(), sys::GetLastFrameTimeMs()
					, scene->GetEntityCount(), renderStats.numEntities
				//	, renderStats.numBatches
					);
			}
		}

		// Update camera info.
		{
			mxCamera & camera = this->scene->GetActiveCamera();
			const Vec3D & pos = camera.GetView().GetOrigin();
			const Vec3D & lookAt = camera.GetView().GetLookAt();

			this->debugText_CameraInfo->SetText(
				"Camera Pos: (%.3f, %.3f, %.3f), LookAt: (%.3f, %.3f, %.3f)"
				, pos.x, pos.y, pos.z,			lookAt.x, lookAt.y, lookAt.z
			);
		}
	}
};

#endif // ! __EXAMPLE_BASIC_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
