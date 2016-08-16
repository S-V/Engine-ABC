
texture  modelTexture
<
	string filename = "masonry_brick.jpg";
>;

texture  detailTexture
<
	string filename = "detailmap.jpg";
>;

sampler modelTextureSampler = sampler_state
{
	Texture = (modelTexture);
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Point;
};

sampler detailTextureSampler = sampler_state
{
	Texture = (detailTexture);
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Point;
};

technique t0
{
  pass p0
  {
    Lighting         = false;

	MaterialDiffuse = { 1.0, 1.0, 1.0, 0.5 };
	
	Sampler[0] = modelTextureSampler;
	TexCoordIndex[0] = 0;
	ColorOp[0]   = Modulate;
	ColorArg1[0] = Texture;
	ColorArg2[0] = Diffuse;
	
	Sampler[1] = detailTextureSampler;
	TexCoordIndex[1] = 1;
	ColorOp[1]   = AddSigned;
	ColorArg1[1] = Texture;
	ColorArg2[1] = Current;
	
	TextureTransformFlags[1] = Count2;
	TextureTransform[1] = { 80.0, 0.0, 0.0, 0.0,
							0.0, 90.0, 0.0, 0.0,
							0.0, 0.0, 80.0, 0.0,
							0.0, 0.0, 0.0, 0.0	};
  }//End Pass

}//End Technique
