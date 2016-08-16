/*
=============================================================================
	File:	Example_CSG.h
	Desc:	Shows how to perform constructive solid geometry operations
			on simple meshes.
=============================================================================
*/

class Sample_CSG : public Sample_Basic {
private:
	RefPtr< CSGModel >		csgModelInv;	// subtractive model for CSG
	RefPtr< CSGModel >		csgModel;		// additive model for CSG
	ESetOp					boolOp;			// current set operation

public:
	Sample_CSG()
		: boolOp( ESetOp::CSG_Difference )
	{}

	override(Sample_Basic) const mxChar* GetName() const { return TEXT("CSG"); }

	override(Sample_Basic) bool Create()
	{
		if ( ! Sample_Basic::Create() ) {
			return false;
		}

		sys::Print(TEXT("\nSelect the type of CSG operation ( 0 - Difference, 1 - Union ):\n"));
		{
			int n;
			if( 1 != scanf( "%d", &n ) || (0 != n && 1 != n) )
			{
				sys::Print(TEXT("\nInvalid number! Defaulting to Union...\n"));
				this->boolOp = CSG_Union;
			}
			else {
				this->boolOp = (n == 0) ? CSG_Difference : CSG_Union;
			}
		}

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

		rxResources & resources = mxEngine::get().GetRenderer().GetResources();

		TPtr< Model >  model;
		
		// ground
		if(1){
			ModelDescription  modelDesc;
			modelDesc.bCsgModel = true;

			model = this->sceneMgr->AddQuad( 100, Vec2D(70), "ground", modelDesc );
			model->SetMaterial( resources.GetMaterial( "metal" ) );
		}
		// stone slab
		if(1){
			ModelDescription  modelDesc;
			modelDesc.bCsgModel = true;

			model = this->sceneMgr->AddBox( 10, 2, 10, "testbox0", modelDesc );
			model->SetPosition(Vec3D(0,2,0));
			model->SetMaterial( resources.GetMaterial( "marble" ) );
			model->AddAnimator( new Animator_Orientation( Vec3D::UNIT_Y, -0.01f ) );
		}
		// tower
		if(1){
			mxMeshPtr  mesh;

			MeshDescription  meshDesc;
			{
				meshDesc.initialTransform = Matrix4::CreateScale( Vec3D(0.1f,0.2f,0.1f) );
				meshDesc.texCoordScale = 0.1f;
			}
			mesh = LoadMeshFromFile( TEXT("test/test0.3ds"), meshDesc );

			ModelDescription  modelDesc;
			modelDesc.bCsgModel = true;

			model = this->sceneMgr->AddMesh( mesh, "tower", modelDesc );
			model->SetMaterial( resources.GetMaterial( "stone" ) );
			model->SetOrigin(Vec3D(-30,0,-30));

			// tower with inverted normals

			mesh->FlipNormals();
			model = this->sceneMgr->AddMesh( mesh, "tower_inverted", modelDesc );
			model->SetMaterial( resources.GetMaterial( "metal_panels" ) );
			model->SetOrigin(Vec3D(25,10,-30));
		}
		// house
		if(0){
			ModelDescription  modelDesc;
			modelDesc.initialTransform = Matrix4::CreateScale( 0.02f ) * Matrix4::CreateTranslation(Vec3D(-30,0,30));
			modelDesc.texCoordScale = 0.1f;
			//modelDesc.bCsgModel = true;

			model = this->sceneMgr->AddMesh( TEXT("architecture/house_ssao.3ds"), "house", modelDesc );
			model->SetMaterial( resources.GetMaterial( "marble" ) );

			PutOntoGroundPlane( model );
		}

		// rotating solid model
		if(1){
			ModelDescription  modelDesc;
			modelDesc.bCsgModel = true;

			model = this->sceneMgr->AddBox( 10, 8, 3, "moving_csg_model", modelDesc );
			model->SetPosition(Vec3D(10,4.5f,10));
			model->SetMaterial( resources.GetMaterial( "tile_colored" ) );
			model->AddAnimator( new Animator_Rotation( Vec3D(0), Vec3D::UNIT_Y, DEG2RAD(1.0f) ) );
			model->AddAnimator( new Animator_Orientation( Vec3D::UNIT_Y, 0.003f ) );
		}

		// screw
		if(0){
			ModelDescription  modelDesc;
		//	modelDesc.initialTransform = Matrix4::CreateScale( 0.2f );// * Matrix4::CreateTranslation(Vec3D(-30,10,30));
			modelDesc.texCoordScale = 0.1f;
			modelDesc.bCsgModel = true;

			model = this->sceneMgr->AddMesh( TEXT("screw-model.3DS"), "screw", modelDesc );
			model->SetMaterial( resources.GetMaterial( "metal_panels" ) );
			model->SetOrigin(Vec3D(-30,-20,30));
		}

		if(1){
			ModelDescription  modelDesc;
			modelDesc.bCsgModel = true;

			model = this->sceneMgr->AddBox( 10, 5, 10, "moving_csg_model1", modelDesc );
			model->SetPosition(Vec3D( 10, 15.0f, 10 ));
			model->SetMaterial( resources.GetMaterial( "concrete" ) );
			model->AddAnimator( new Animator_Rotation( Vec3D(0), Vec3D::UNIT_Y, DEG2RAD(-1.0f) ) );
			model->AddAnimator( new Animator_Orientation( Vec3D::UNIT_Y, -0.003f ) );
			model->AddAnimator( new Animator_Orientation( Vec3D::UNIT_Z, 0.003f ) );

			// attach a point light to this model
			if(1){
				LightCreationInfo  cInfo;
				cInfo.Type		= ELightType::Light_Point;
				cInfo.flare		= resources.LoadTexture("textures/light_corona0.png");

				NodePtr<Light> light;

				cInfo.Diffuse	= FColor::RED;
				cInfo.Position	= Vec3D(7,5,7);
				cInfo.Range		= 18.0f;
				light = this->sceneMgr->AddLight( cInfo );
				light->AddAnimator( new Animator_Rotation( Vec3D::ZERO, Vec3D::UNIT_Y, DEG2RAD(0.07f) ) );
				model->AddKid( light );

				cInfo.Diffuse	= FColor::LIGHT_YELLOW_GREEN;
				cInfo.Position	= Vec3D(5,-6,7);
				cInfo.Range		= 22.0f;
				light = this->sceneMgr->AddLight( cInfo );
				light->AddAnimator( new Animator_Rotation( Vec3D::ZERO, Vec3D::UNIT_X, DEG2RAD(-0.09f) ) );
				model->AddKid( light );

				cInfo.Diffuse	= FColor::BLUE;
				cInfo.Position	= Vec3D(-7,-4,-7);
				cInfo.Range		= 20.0f;
				light = this->sceneMgr->AddLight( cInfo );
				light->AddAnimator( new Animator_Rotation( Vec3D::ZERO, Vec3D::UNIT_X, DEG2RAD(0.03f) ) );
				model->AddKid( light );
			}
		}

		// create models for csg
		if(1){
			const mxChar* modelName =
				TEXT("test/cube.3ds")
			//	TEXT("screw-model.3DS")
				;

			mxMeshPtr  mesh;

			MeshDescription  meshDesc;
			{
				meshDesc.initialTransform = Matrix4::CreateScale( 0.05f);//3f );
				meshDesc.texCoordScale = 0.2f;
			}
			mesh = LoadMeshFromFile( modelName, meshDesc );

			// Transform the mesh if needed.
			{
				Matrix4  mat( _InitIdentity );

				const mxReal OFFSET_EPS = 0.1f;
				if( mesh->bounds.GetCenter().Length() > OFFSET_EPS )
				{
					const Vec3D translation( mesh->bounds.GetCenter() * -1.0f );
					mat.SetTranslation( translation );
				}
				if(1){
					// dimensions of the CSG model (limits placed on diagonal length)
					const mxReal MIN_MODEL_SIZE = 2.0f;
					const mxReal MAX_MODEL_SIZE = 10.0f;

					// Scale the model and translate it to the local center.
					const mxReal diagSize = mesh->bounds.Size().Length();

					if( !IsInRangeInc( diagSize, MIN_MODEL_SIZE, MAX_MODEL_SIZE ) )
					{
						const mxReal scaling = MAX_MODEL_SIZE / diagSize;

						mat *= Matrix4::CreateScale(Vec3D( scaling ));
					}
				}

				// rotate the model a bit
				if(1)	mat *= mxAngles(50,90,70).ToMat4();

				if( mat != Matrix4::mat4_identity )
				{
					mesh->Transform( mat );
				}
			}

			// Create an additive model.
			{
				CSGInfo  cInfo;
				cInfo.mesh = mesh;
				this->csgModel = NewCSGModel( cInfo );
			}
			// Create a subtractive model.
			{
				mesh->FlipNormals();

				CSGInfo  cInfo;
				cInfo.mesh = mesh;
				this->csgModelInv = NewCSGModel( cInfo );
			}
		}

		this->spectator->AttachFlashlight();

		{
			rxDebugDrawer & debugDrawer = mxEngine::get().GetRenderer().GetDebugDrawer();

			rxDebugTextDesc  desc;
			desc.X = 10, desc.Y = 300;
			desc.Color = FColor::LIGHT_YELLOW_GREEN;
			desc.Text = "Press L or Right Mouse Button to turn flashlight on/off";
			debugDrawer.CreateText( desc );
		}


		// Setup lights.

		mxEngine::get().GetRenderer().GetScene().SetAmbientLightColor(FColor(0.1f,0.1f,0.1f));

		// Create a spot light.
		if(1){
			LightCreationInfo  cInfo;
			cInfo.Type			= ELightType::Light_Spot;
			cInfo.Diffuse		= FColor::LIGHT_YELLOW_GREEN;
			cInfo.Intensity		= 0.7f;
			cInfo.Position		= Vec3D(0,20,0);
			cInfo.Direction		= Vec3D(0,-1,0);
			cInfo.Range			= 100.0f;
			cInfo.SpotInnerAngle = DEG2RAD( 86.0f );
			cInfo.SpotOuterAngle = DEG2RAD( 89.0f );
			cInfo.bCastShadows	= true;
			cInfo.flare			= resources.LoadTexture("textures/light_corona0.png");

			Light * light = this->sceneMgr->AddLight( cInfo );
		}

		LoadSkybox_Night()->
			AddAnimator( new Animator_Orientation( Vec3D::UNIT_SCALE.GetNormalized(), 0.0003f ) );

		return true;
	}
	//----------------------------------------------------------------------------------------------------
	void DoBoolOp( ESetOp op, const Matrix4* operandTransform, Model* subject, CSGModel* operand2 )
	{
		Assert( operandTransform && subject && operand2 );

		CSGInput csgInfo;
		csgInfo.type = op;
		
		operand2->SetTransform( operandTransform );
		csgInfo.operand = operand2;

		subject->ApplyCSG( csgInfo );
	}
	//----------------------------------------------------------------------------------------------------
	void Shoot()
	{
		const mxSceneView & view = this->camera->GetView();

		Vec3D hitPosition;
		if( mxEntity * pHitObject = this->scene->GetSpatialDatabase().CastRay( view.GetOrigin(), view.GetLookAt(), hitPosition ) )
		{
			if( Model * pModel = DynamicCast<Model>(pHitObject) )
			{
				// transform the subtractive model to the hit point
				static Matrix4 localToWorld;
	
				localToWorld.SetIdentity();
				localToWorld.SetTranslation( hitPosition );

				//sys::Print("\nMatrix: %s\n", localToWorld.ToString() );

				DoBoolOp(
					this->boolOp, &localToWorld,
					pModel,
					(this->boolOp == ESetOp::CSG_Union) ? this->csgModel : this->csgModelInv );
			}

			if(1){
				rxResources & resources = mxEngine::get().GetRenderer().GetResources();
				Billboard * bill = this->sceneMgr->AddBillboard( resources.LoadTexture("textures/light_corona1.png"), Vec2D(0.1f) );
				bill->SetOrigin( hitPosition - view.GetLookAt()*0.01f );
				static int count = 0;
				printf("\n%d hit: %s\n", count++, hitPosition.ToString() );
			}
		}
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
				Shoot();
			}//if LMB is pressed down
			
			if ( rEvent.Mouse.Type == mxMouseEvent::RightButton_PressedDown )
			{
				this->spectator->ToggleFlashLight();
			}//if LMB is pressed down

		}//End of Mouse events

		return false;
	}
};
