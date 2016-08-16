/*
=============================================================================
	File:	Example_Meshes.h
	Desc:	Shows how to load meshes from files.
=============================================================================
*/

class Sample_Meshes : public Sample_Basic
{
public:
	Sample_Meshes()
	{}
	
	override(Sample_Basic) const mxChar* GetName() const { return TEXT("Meshes"); }

	override(Sample_Basic) bool Create()
	{
		if ( ! Sample_Basic::Create() ) {
			return false;
		}

		rxResources & resources = mxEngine::get().GetRenderer().GetResources();

		// Create some 3D objects.

		TPtr< Model >  model;
		{
			ModelDescription  modelDesc;
			//modelDesc.bCsgModel = true;

			model = this->sceneMgr->AddQuad( 500, Vec2D(100), "ground", modelDesc );
			model->SetMaterial( resources.GetMaterial( "metal" ) );
		}

		if(1){
			ModelDescription  modelDesc;
			modelDesc.initialTransform = Matrix4::CreateScale( Vec3D(0.1f,0.2f,0.1f) ) * Matrix4::CreateTranslation(Vec3D(-44,0.1f,3));
			//modelDesc.bCsgModel = true;
			modelDesc.texCoordScale = 0.1f;

			model = this->sceneMgr->AddMesh( TEXT("test/test0.3ds"), "mesh0", modelDesc );
			model->SetMaterial( resources.GetMaterial( "stone" ) );
		}

		if(1){
			ModelDescription  modelDesc;
			modelDesc.initialTransform = Matrix4::CreateScale( 1e-2f ) * Matrix4::CreateTranslation(Vec3D(30,0,30));
			//modelDesc.bCsgModel = true;
			modelDesc.texCoordScale = 0.1f;

			model = this->sceneMgr->AddMesh( TEXT("architecture/door03.3ds"), "door", modelDesc );
			model->SetMaterial( resources.GetMaterial( "stone" ) );
		}

		if(1){
			ModelDescription  modelDesc;
			modelDesc.initialTransform = Matrix4::CreateScale( 0.05f ) * Matrix4::CreateTranslation(Vec3D(-35,0.1f,40));
			//modelDesc.bCsgModel = true;
			//modelDesc.texCoordScale = 100.0f;

			model = this->sceneMgr->AddMesh( TEXT("architecture/84mosque-ciro.3DS"), "mesh1", modelDesc );
			model->SetMaterial( resources.GetMaterial( "marble" ) );
		}

		if(1){
			ModelDescription  modelDesc;
			modelDesc.initialTransform = Matrix4::CreateScale( 0.01f ) * Matrix4::CreateTranslation(Vec3D(0,0.1f,0));
			//modelDesc.bCsgModel = true;
			//modelDesc.texCoordScale = 100.0f;

			model = this->sceneMgr->AddMesh( TEXT("architecture/Modern Structure.3ds"), "mesh2", modelDesc );
			model->SetMaterial( resources.GetMaterial( "marble" ) );
		}


		// Create some lights.

		// Create a spot light.
		{
			LightCreationInfo  cInfo;
			cInfo.Type			= ELightType::Light_Spot;
			cInfo.Diffuse		= FColor::WHITE;
			cInfo.Intensity		= 1.0f;
			cInfo.Position		= Vec3D(0,40,0);
			cInfo.Direction		= Vec3D(-0.3f,-1,0).GetNormalized();
			cInfo.Range			= 200.0f;
			cInfo.SpotInnerAngle = DEG2RAD( 86.0f );
			cInfo.SpotOuterAngle = DEG2RAD( 89.0f );
			cInfo.bCastShadows	= true;
			cInfo.flare			= resources.LoadTexture("textures/light_corona0.png");

			Light * light = this->sceneMgr->AddLight( cInfo, "spotLight0" );
			light->AddAnimator( new Animator_Rotation( Vec3D(-5,0,-5), Vec3D::UNIT_Y, DEG2RAD(1.0f) ) );
			//light->AddAnimator( new Animator_Orientation( Vec3D::UNIT_Y, DEG2RAD(1.0f) ) );
		}

		// Create many small lights.
		if(0){
			enum { NUM_POINT_LIGHTS = 400 };
			const float low = -120, high = 120;
			const float minRange = 2, maxRange = 5;
			for( int i=0; i<NUM_POINT_LIGHTS; i++ )
			{
				int seed1, seed2;
				__asm {
					rdtsc
					mov	[seed1], eax
					mov	[seed2], edx
				}
				mxRandom	random(seed1);
				float x = random.RandomFloat( low, high );

				random.SetSeed( (int)mxMath::Cos(x) * 2 - x );
				float z = x * random.RandomFloat( low, high );

				random.SetSeed(seed1 * z + x*x);
				const float minHeight = 1, maxHeight = 33;
				float y = random.RandomFloat( minHeight, maxHeight );


				const Vec3D position( x, y, z );

				LightCreationInfo  cInfo;
				cInfo.Type		= ELightType::Light_Point;
				cInfo.Diffuse	= GetRandomColor(x-z+i);
				cInfo.Position	= position;
				cInfo.Range		= random.RandomFloat( minRange, maxRange );
				cInfo.flare		= resources.LoadTexture("textures/light_corona1.png");

				Light * light = this->sceneMgr->AddLight( cInfo );
				light->AddAnimator( new Animator_Rotation( Vec3D::ZERO, Vec3D::UNIT_Y, DEG2RAD(0.5f) ) );
			}
		}
		else {
			for( int i=0; i<10; i++ ) {
				for( int j=0; j<10; j++ ) {
					for( int k=0; k<2; k++ ) {

						const Vec3D position( (i-5)*10, (k + 2)*7, (j-5)*10 );

						const float minRange = 2, maxRange = 6;

						int seed;
						__asm {
							rdtsc
							mov	[seed], eax
						}
						mxRandom	random(seed);

						LightCreationInfo  cInfo;
						cInfo.Type		= ELightType::Light_Point;
						cInfo.Diffuse	= GetRandomColor(i-j+k);
						cInfo.Position	= position;
						cInfo.Range		= random.RandomFloat( minRange, maxRange );
						cInfo.flare		= resources.LoadTexture("textures/light_corona1.png");

						Light * light = this->sceneMgr->AddLight( cInfo );
						light->AddAnimator( new Animator_Rotation( Vec3D::ZERO, Vec3D::UNIT_Y, DEG2RAD(0.5f) ) );
					}
				}

			}
		}

		// Create a skybox.
		//LoadSkybox_Sunset();
		LoadSkybox_Night()->
			AddAnimator( new Animator_Orientation( Vec3D::UNIT_SCALE.GetNormalized(), 0.0003f ) );

		this->spectator->AttachFlashlight(1);

		return true;
	}
	//----------------------------------------------------------------------------------------------------
	override(mxApplication) bool ProcessInput( const mxInputEvent& rEvent )
	{
		if ( Sample_Basic::ProcessInput( rEvent ) ) {
			return true;
		}
		
		// Mouse

		if ( rEvent.Type == EInputEvent::InputEvent_Mouse )
		{
			switch( rEvent.Mouse.Type )
			{
				case mxMouseEvent::RightButton_PressedDown :
					this->spectator->ToggleFlashLight();
					break;
			}

		}//End of Mouse events

		return false;
	}
};
