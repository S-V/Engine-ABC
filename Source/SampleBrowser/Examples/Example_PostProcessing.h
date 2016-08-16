/*
=============================================================================
	File:	Example_PostProcessing.h
	Desc:	Shows how to use post-processing.
=============================================================================
*/

class Sample_PostProcess : public Sample_Basic {
private:
	TArray< rxPostEffect* >		postEffects;	// all created post-processing effects
	int						currentPostFxIndex;	// index of currently selected post-processing effect
	TPtr< rxDebugText >		currentPostFxText;

private:
	rxPostEffect* GetCurrentPostFx()
	{
		Assert(postEffects.Num()>0);
		return postEffects[ currentPostFxIndex ];
	}
	void UpdateCurrentPostFxText()
	{
		const bool bIsEnabled = GetCurrentPostFx()->IsEnabled();

		currentPostFxText->SetText(
			TEXT("Current PostFX: %s (%s)"),
			GetPostFxName(GetCurrentPostFx()->GetType()),
			bIsEnabled ? TEXT("Enabled") : TEXT("Disabled")
		);

		currentPostFxText->SetColor( bIsEnabled ? FColor::RED : FColor::GRAY );
	}
	void SelectNextPostFx()
	{
		++currentPostFxIndex;
		if( currentPostFxIndex >= (int)this->postEffects.Num() )
		{
			currentPostFxIndex = 0;
		}
		UpdateCurrentPostFxText();
	}
	void SelectPrevPostFx()
	{
		--currentPostFxIndex;
		if( currentPostFxIndex < 0 )
		{
			if(this->postEffects.Num() > 0) {
				currentPostFxIndex = (int)(this->postEffects.Num() - 1);
			} else {
				currentPostFxIndex = 0;
			}
		}
		UpdateCurrentPostFxText();
	}

public:
	Sample_PostProcess()
		: currentPostFxIndex( -1 )
	{}

	override(Sample_Basic) const mxChar* GetName() const { return TEXT("Post-Processing"); }

	override(Sample_Basic) bool Create()
	{
		if ( ! Sample_Basic::Create() ) {
			return false;
		}

		rxResources & resources = mxEngine::get().GetRenderer().GetResources();

		TPtr< Model >  model;

		// ground
		if(1){
			model = this->sceneMgr->AddQuad( 400, Vec2D(70) );
			model->SetMaterial( resources.GetMaterial( "stone" ) );
		}
		// house
		if(1){
			ModelDescription  modelDesc;
			modelDesc.initialTransform = Matrix4::CreateScale( 0.02f );// * Matrix4::CreateTranslation(Vec3D(-30,0,30));
			modelDesc.texCoordScale = 0.1f;

			model = this->sceneMgr->AddMesh( TEXT("architecture/house_ssao.3ds"), "house", modelDesc );
			model->SetMaterial( resources.GetMaterial( "marble" ) );

			PutOntoGroundPlane( model );
		}
		if(1){
			model = this->sceneMgr->AddBox( 5, 10, 5 );
			model->SetPosition(Vec3D(12,6,12));
			model->SetMaterial( resources.GetMaterial( "metal_tile" ) );
			model->AddAnimator( new Animator_Rotation( Vec3D(0), Vec3D::UNIT_Y, DEG2RAD(1.0f) ) );
			model->AddAnimator( new Animator_Orientation( Vec3D::UNIT_Y, 0.007f ) );
			model->AddAnimator( new Animator_Orientation( Vec3D::UNIT_X, 0.01f ) );
		}

		this->spectator->AttachFlashlight(1);

		rxScene & gfxScene = mxEngine::get().GetRenderer().GetScene();

		// Setup lights.

		gfxScene.SetAmbientLightColor(FColor(0.1f,0.1f,0.1f));

		{
			LightCreationInfo  cInfo;
			cInfo.Type			= ELightType::Light_Directional;
			cInfo.Diffuse		= FColor(0.9f, 0.4f, 0.2f, 1.0f);
			cInfo.Intensity		= 0.7f;
			cInfo.Direction		= Vec3D(-0.707f,-0.072f,-0.703f).GetNormalized();
			cInfo.bCastShadows	= true;
			
			Light * light = this->sceneMgr->AddLight( cInfo );
		}

		LoadSkybox_Sunset();

		// Create post-processing effects.

		rxPostEffectManager * postFxMgr = gfxScene.GetPostEffectManager();
		this->postEffects.Append( postFxMgr->AddGrayScale() );
		this->postEffects.Append( postFxMgr->AddBlackAndWhite() );
		this->postEffects.Append( postFxMgr->AddInvertColor() );
		this->postEffects.Append( postFxMgr->AddEmboss() );
		this->postEffects.Append( postFxMgr->AddSharpen() );
		this->postEffects.Append( postFxMgr->AddSepia() );
		this->postEffects.Append( postFxMgr->AddModulatingWave() );
		this->postEffects.Append( postFxMgr->AddBlur() );
		this->postEffects.Append( postFxMgr->AddGBlur() );
		this->postEffects.Append( postFxMgr->AddBloom() );

		this->currentPostFxIndex = 0;

		{
			rxDebugDrawer & debugDrawer = mxEngine::get().GetRenderer().GetDebugDrawer();

			rxDebugTextDesc  desc;
			desc.X = 10, desc.Y = 100;
			desc.Color = FColor::LIGHT_YELLOW_GREEN;
			desc.Text = "Press T to toggle post-processing on/off";
			debugDrawer.CreateText( desc );

			desc.X = 10, desc.Y = 130;
			desc.Color = FColor::LIGHT_YELLOW_GREEN;
			desc.Text = "Press F/G (or rotate mouse wheel) to cycle through post-processing effects";
			debugDrawer.CreateText( desc );

			desc.X = 10, desc.Y = 160;
			desc.Color = FColor::LIGHT_YELLOW_GREEN;
			desc.Text = "Press LMB to toggle current post-processing effect on/off";
			debugDrawer.CreateText( desc );

			desc.X = 10, desc.Y = 190;
			desc.Color = FColor::GREEN;
			desc.Text = "Press RMB to turn flashlight on/off";
			debugDrawer.CreateText( desc );

			for(int iPostFx = 0; iPostFx < (int)this->postEffects.Num(); iPostFx++)
			{
				desc.X = 10, desc.Y = 230 + iPostFx * 20;
				desc.Color = FColor::YELLOW;
				desc.Text = GetPostFxName( this->postEffects[iPostFx]->GetType() );
				debugDrawer.CreateText( desc );

				// disable current effect
				this->postEffects[iPostFx]->SetEnabled(false);
			}

			{
				rxDebugTextDesc  desc;
				desc.X = 500, desc.Y = 100;
				desc.Color = FColor::RED;
				this->currentPostFxText = debugDrawer.CreateText( desc );
				UpdateCurrentPostFxText();
			}
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
				case EKeyCode::Key_T :
					{
						static bool b = true;
						b ^= 1;
						mxEngine::get().GetRenderer().GetScene().GetPostEffectManager()->ToggleAllEffects( b );
					}
					break;

				case EKeyCode::Key_G :
					SelectNextPostFx();
					break;

				case EKeyCode::Key_F :
					SelectPrevPostFx();
					break;

				}//switch( key )
			}
		}//End of Keyboard events
		
		// Mouse

		if ( rEvent.Type == EInputEvent::InputEvent_Mouse )
		{
			switch( rEvent.Mouse.Type )
			{
			case mxMouseEvent::LeftButton_PressedDown :
				GetCurrentPostFx()->SetEnabled( ! GetCurrentPostFx()->IsEnabled() );
				UpdateCurrentPostFxText();
				break;

				case mxMouseEvent::RightButton_PressedDown :
					this->spectator->ToggleFlashLight();
					break;

				case mxMouseEvent::Wheel_Moved :
					if( rEvent.Mouse.WheelDelta > 0 )
						SelectPrevPostFx();
					else
						SelectNextPostFx();
					break;
			}

		}//End of Mouse events

		return false;
	}
};
