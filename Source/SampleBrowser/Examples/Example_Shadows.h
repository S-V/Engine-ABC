/*
=============================================================================
	File:	Example_Shadows.h
	Desc:	Shows how to use real-time shadows.
=============================================================================
*/

class Sample_Shadows : public Sample_Basic
{
private:
	enum E_LIGHTS
	{
		LIGHT_1 = 0,
		LIGHT_2,
		LIGHT_3,

		MAX_LIGHTS
	};

	TFixedArray< NodePtr<Light>, MAX_LIGHTS >		lights;

public:
	Sample_Shadows()
	{}

	override(Sample_Basic) const mxChar* GetName() const { return TEXT("Shadows"); }
	
	override(Sample_Basic) bool Create()
	{
		if ( ! Sample_Basic::Create() ) {
			return false;
		}

		rxResources & resources = mxEngine::get().GetRenderer().GetResources();

		TPtr< Model >  model;
		{
			model = this->sceneMgr->AddQuad( 500, Vec2D(100) );
			model->SetMaterial( resources.GetMaterial( "tile" ) );
		}
		if(1){
			ModelDescription  modelDesc;
			modelDesc.initialTransform = Matrix4::CreateScale(Vec3D(0.2f));
			model = this->sceneMgr->AddMesh( TEXT("globe-world-map.3DS"), "globe", modelDesc );
			model->SetMaterial( resources.GetMaterial( "metal_panels" ) );
			model->AddAnimator( new Animator_Orientation( Vec3D::UNIT_SCALE.GetNormalized(), DEG2RAD(1.0f) ) );
			model->SetOrigin(Vec3D(0,4,0));
		}
		if(1){
			ModelDescription  modelDesc;
			modelDesc.initialTransform = Matrix4::CreateScale( Vec3D(0.1f,0.17f,0.1f) ) * Matrix4::CreateTranslation(Vec3D(-30,0,-30));
			modelDesc.texCoordScale = 0.1f;

			model = this->sceneMgr->AddMesh( TEXT("test/test0.3ds"), "mesh0", modelDesc );
			model->SetMaterial( resources.GetMaterial( "stone" ) );
		}
		//{
		//	ModelDescription  modelDesc;
		//	modelDesc.initialTransform = Matrix4::CreateScale( Vec3D(0.1f) ) * Matrix4::CreateTranslation(Vec3D(10,0,10));
		//	model = this->sceneMgr->AddMesh( TEXT("weapons/ak47.3ds"), "AK47", modelDesc );
		//	model->SetMaterial( resources.GetMaterial( "marble" ) );
		//}
		if(1){
			ModelDescription  modelDesc;
			modelDesc.initialTransform = Matrix4::CreateScale( Vec3D(0.2f) ) * Matrix4::CreateTranslation(Vec3D(10,0,-9));
			model = this->sceneMgr->AddMesh( TEXT("angel.3ds"), "angel", modelDesc );
			model->SetMaterial( resources.GetMaterial( "marble" ) );
		}
		if(1){
			ModelDescription  modelDesc;
			modelDesc.initialTransform = Matrix4::CreateScale( Vec3D(0.5f) ) * Matrix4::CreateTranslation(Vec3D(10,2,10));
			model = this->sceneMgr->AddMesh( TEXT("02notebook-model3d.3ds"), "notebook", modelDesc );
			model->SetMaterial( resources.GetMaterial( "glossy_plastic" ) );
		}



		// Init lights.

		mxEngine::get().GetRenderer().GetScene().SetAmbientLightColor(FColor(0.03f,0.03f,0.03f));
		{
			LightCreationInfo  cInfo;
			cInfo.Type			= ELightType::Light_Spot;
			cInfo.Diffuse		= FColor::LIGHT_YELLOW_GREEN;
			cInfo.Intensity		= 1.0f;
			cInfo.Position		= Vec3D(-32,40,-32);
			cInfo.Direction		= ( Vec3D(0,0,0) - cInfo.Position ).GetNormalized();
			cInfo.Range			= 120.0f;
			cInfo.SpotInnerAngle = DEG2RAD( 58.0f );
			cInfo.SpotOuterAngle = DEG2RAD( 60.0f );
			cInfo.bCastShadows	= true;
			cInfo.flare			= resources.LoadTexture("textures/light_corona0.png");
			cInfo.projector		= resources.LoadTexture("textures/swiped/AngleMosaic.dds");

			this->lights[ LIGHT_1 ] = this->sceneMgr->AddLight( cInfo );
			this->lights[ LIGHT_1 ]->AddAnimator( new Animator_Rotation( Vec3D(-40,48,-40), Vec3D::UNIT_Y, DEG2RAD(1.0f) ) );
		}
		{
			LightCreationInfo  cInfo;
			cInfo.Type			= ELightType::Light_Spot;
			cInfo.Diffuse		= FColor::WHITE;
			cInfo.Intensity		= 0.7f;
			cInfo.Position		= Vec3D(5,12,-2);
			cInfo.Direction		= ( Vec3D(0,0,0) - cInfo.Position ).GetNormalized();
			cInfo.Range			= 100.0f;
			cInfo.SpotInnerAngle = DEG2RAD( 58.0f );
			cInfo.SpotOuterAngle = DEG2RAD( 60.0f );
			cInfo.bCastShadows	= true;
			cInfo.flare			= resources.LoadTexture("textures/light_corona0.png");
			cInfo.projector		= resources.LoadTexture("images/misc/leaf.png");

			this->lights[ LIGHT_2 ] = this->sceneMgr->AddLight( cInfo );
		}
		{
			LightCreationInfo  cInfo;
			cInfo.Type			= ELightType::Light_Spot;
			cInfo.Diffuse		= FColor::RED;
			cInfo.Intensity		= 1.0f;
			cInfo.Position		= Vec3D(-20,10,20);
			cInfo.Direction		= ( Vec3D(0,0,0) - cInfo.Position ).GetNormalized();
			cInfo.Range			= 100.0f;
			cInfo.SpotInnerAngle = DEG2RAD( 28.0f );
			cInfo.SpotOuterAngle = DEG2RAD( 40.0f );
			cInfo.bCastShadows	= true;
			cInfo.flare			= resources.LoadTexture("textures/light_corona0.png");
			cInfo.projector		= resources.LoadTexture("textures/swiped/10points.png");

			this->lights[ LIGHT_3 ] = this->sceneMgr->AddLight( cInfo );
		}

		LoadSkybox_Night()->
			AddAnimator( new Animator_Orientation( Vec3D::UNIT_SCALE.GetNormalized(), 0.0003f ) );

		this->spectator->AttachFlashlight(1);
		this->spectator->ToggleFlashLight();

		{
			rxDebugDrawer & debugDrawer = mxEngine::get().GetRenderer().GetDebugDrawer();
			rxDebugTextDesc  desc;

			desc.X = 10, desc.Y = 160;
			desc.Color = FColor::LIGHT_YELLOW_GREEN;
			desc.Text = "Press 1,2,3 to switch lights on/off";
			debugDrawer.CreateText( desc );

			desc.X = 10, desc.Y = 190;
			desc.Color = FColor::GREEN;
			desc.Text = "Press LMB to turn flashlight on/off";
			debugDrawer.CreateText( desc );
		}

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
				case EKeyCode::Key_1 :
					if( Light* pLight = this->lights[ LIGHT_1 ] ) {
						pLight->SetEnabled( ! pLight->IsEnabled() );
					}
					break;

				case EKeyCode::Key_2 :
					if( Light* pLight = this->lights[ LIGHT_2 ] ) {
						pLight->SetEnabled( ! pLight->IsEnabled() );
					}
					break;

				case EKeyCode::Key_3 :
					if( Light* pLight = this->lights[ LIGHT_3 ] ) {
						pLight->SetEnabled( ! pLight->IsEnabled() );
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
				this->spectator->ToggleFlashLight();
			}//if LMB is pressed down
		}//End of Mouse events

		return false;
	}
};
