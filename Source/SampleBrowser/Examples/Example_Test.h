/*
=============================================================================
	File:	Example_Test.h
	Desc:	Testing & Debugging.
=============================================================================
*/

class AppTest : public Sample_Basic 
{
public:
	AppTest()
	{}

	override(Sample_Basic) const mxChar* GetName() const { return TEXT("Test"); }

	override( mxApplication ) bool Create()
	{
		if ( ! Sample_Basic::Create() ) {
			return false;
		}

		rxResources & resources = mxEngine::get().GetRenderer().GetResources();

		// Create some 3D objects.

		TPtr< Model >  model;
		{
			model = this->sceneMgr->AddQuad( 500, Vec2D(100), "ground" );
			model->SetMaterial( resources.GetMaterial( "metal_panels" ) );
		}
		if(1){
			model = this->sceneMgr->AddBox( 1, 1, 1, "testbox0" );
			model->SetOrigin( Vec3D( 0, 1.5f, 0 ) );
			model->SetMaterial( resources.GetMaterial( "gothic" ) );
			model->AddAnimator( new Animator_Orientation( Vec3D::UNIT_Y, 0.003f ) );
			model->AddAnimator( new Animator_Orientation( Vec3D::UNIT_X, 0.01f ) );
			model->AddAnimator( new Animator_Rotation( Vec3D::ZERO, Vec3D::UNIT_Y, DEG2RAD(1.1f) ) );
		}

		// Create a spot light.
		{
			LightCreationInfo  cInfo;
			cInfo.Type			= ELightType::Light_Spot;
			cInfo.Diffuse		= FColor::WHITE;
			cInfo.Intensity		= 1.0f;
			cInfo.Position		= Vec3D(0,50,0);
			cInfo.Direction		= Vec3D(0,-1,0);
			cInfo.Range			= 100.0f;
			cInfo.SpotInnerAngle = DEG2RAD( 86.0f );
			cInfo.SpotOuterAngle = DEG2RAD( 89.0f );
			cInfo.bCastShadows	= false;
			cInfo.flare			= resources.LoadTexture("textures/light_corona0.png");

			Light * light = this->sceneMgr->AddLight( cInfo, "spotLight0" );
		//	light->AddAnimator( new Animator_Rotation( Vec3D(0,0,0), Vec3D::UNIT_Y, DEG2RAD(1.0f) ) );
		//	light->AddAnimator( new Animator_Orientation( Vec3D::UNIT_Y, DEG2RAD(1.0f) ) );
		}

		// house
		if(0){
			ModelDescription  modelDesc;
			modelDesc.initialTransform = Matrix4::CreateScale( 0.02f ) * Matrix4::CreateTranslation(Vec3D(-30,0,30));
			modelDesc.texCoordScale = 0.1f;

			model = this->sceneMgr->AddMesh( TEXT("architecture/house_ssao.3ds"), "house", modelDesc );
			model->SetMaterial( resources.GetMaterial( "marble" ) );

			PutOntoGroundPlane( model );
		}

		mxEngine::get().GetRenderer().GetScene().SetAmbientLightColor(FColor(0.2f,0.2f,0.2f));








		this->spectator->AttachFlashlight();








		// Create a 'crosshair'.
		{
			rxRenderer & renderer = mxEngine::get().GetRenderer();
			rxDebugDrawer & debugDrawer = mxEngine::get().GetRenderer().GetDebugDrawer();

			rxDebugTextDesc  desc;
			desc.Color = FColor::LIGHT_YELLOW_GREEN;
			const mxScreenSettings & screen = renderer.GetInfo().DisplayMode.screen;
			desc.X = screen.width/2, desc.Y = screen.height/2;
			debugDrawer.CreateText(desc)->SetText("+");
		}




		LoadSkybox_Night()->
			AddAnimator( new Animator_Orientation( Vec3D::UNIT_SCALE.GetNormalized(), 0.0003f ) );


		return true;
	}
	//----------------------------------------------------------------------------------------------------
	override(mxApplication) bool ProcessInput( const mxInputEvent& rEvent )
	{
		if ( Sample_Basic::ProcessInput( rEvent ) ) {
			return true;
		}
		
		// Keyboard

		if ( rEvent.Type == EInputEvent::InputEvent_Keyboard )
		{
			if ( rEvent.Keyboard.PressedDown )
			{
				switch ( rEvent.Keyboard.Key )
				{
				case EKeyCode::Key_L :
					{
						this->spectator->ToggleFlashLight();
					}
					break;
				}//switch( key )
			}
		}//End of Keyboard events
		

		// Mouse

		if ( rEvent.Type == EInputEvent::InputEvent_Mouse )
		{
			if ( rEvent.Mouse.Type == mxMouseEvent::LeftButton_PressedDown )
			{
				//Shoot();
			}//if LMB is pressed down
			
			if ( rEvent.Mouse.Type == mxMouseEvent::RightButton_PressedDown )
			{
				this->spectator->ToggleFlashLight();
			}//if LMB is pressed down

		}//End of Mouse events

		return false;
	}
};
