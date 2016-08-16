/*
=============================================================================
	File:	Example_Portals.h
	Desc:	Showcases transformative portals.
=============================================================================
*/

class Sample_Portals : public Sample_Basic 
{
public:
	Sample_Portals()
	{}

	override(Sample_Basic) const mxChar* GetName() const { return TEXT("Portals"); }

	override( mxApplication ) bool Create()
	{
		if ( ! Sample_Basic::Create() ) {
			return false;
		}

		rxResources & resources = mxEngine::get().GetRenderer().GetResources();

		// Create some 3D objects.

		TPtr< Model >  model;
		{
			ModelDescription  modelDesc;
			model = this->sceneMgr->AddQuad( 500, Vec2D(100), "ground", modelDesc );
			model->SetMaterial( resources.GetMaterial( "stone" ) );
		}
		{
			model = this->sceneMgr->AddBox( 1, 1, 1, "testbox0" );
			model->SetOrigin( Vec3D( 2, 1.5f, 0 ) );
			model->SetMaterial( resources.GetMaterial( "gothic" ) );
		}

		// Create a spot light.
		{
			LightCreationInfo  cInfo;
			cInfo.Type			= ELightType::Light_Spot;
			cInfo.Diffuse		= FColor::WHITE;
			cInfo.Intensity		= 0.7f;
			cInfo.Position		= Vec3D(0,10,0);
			cInfo.Direction		= Vec3D(0,-1,0);
			cInfo.Range			= 100.0f;
			cInfo.SpotInnerAngle = DEG2RAD( 86.0f );
			cInfo.SpotOuterAngle = DEG2RAD( 89.0f );
			cInfo.bCastShadows	= true;
			cInfo.flare			= resources.LoadTexture("textures/light_corona0.png");

			Light * light = this->sceneMgr->AddLight( cInfo, "spotLight0" );
		}

		// Create a skybox.
		LoadSkybox_Night();






		TPtr< Portal >  portal0 = this->sceneMgr->AddPortal();
		portal0->SetSize(1,1);
		portal0->SetOrigin(Vec3D(-5,1,0));
		portal0->SetLookAt(Vec3D(0));


		TPtr< Portal >  portal1 = this->sceneMgr->AddPortal();
		portal1->SetOrigin(Vec3D(0,1,-5));
		portal0->SetLookAt(Vec3D(0));




		return true;
	}
};
