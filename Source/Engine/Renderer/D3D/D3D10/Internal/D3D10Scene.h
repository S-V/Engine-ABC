/*
=============================================================================
	File:	D3D10Scene.h
	Desc:	Actual D3D10 deferred shading scene renderer.
=============================================================================
*/

#ifndef __MX_D3D10_SCENE_H__
#define __MX_D3D10_SCENE_H__

namespace abc {

/*
======================================================================
	
		Settings.

======================================================================
*/

const UINT DEFAULT_SHADOWMAP_RESOLUTION = 512;//1024;

// make light volumes a bit bigger to hide light shape geometry artifacts
const bool RX_D3D10_SCALE_BIAS = true;

/*
======================================================================
	
		Render view.

======================================================================
*/

//
//	D3D10RenderTargetChain - collection of render targets for deferred shading.
//
class D3D10RenderTargetChain {
public:
/*
	//
	//	G-buffer layout
	//
	struct PS_Output_GBuffer
	{
		float4	position : SV_Target3;	// R16 G16 B16	- world-space position, A16 - unused.
		float4	normal   : SV_Target2;	// R10 G10 B10	- world-space normal, A2 - unused.
		float4	diffuse  : SV_Target0;	// R16 G16 B16	- diffuse color, A8 - unused.
		float4	aux      : SV_Target1;	// R16 G16		- auxiliary material parameters ( specularIntensity, specularPower ).
	};
*/
	// 64 + 32 + 64 + 32 = 192 bits per pixel.

	enum
	{
		RT_POSITION,
		RT_NORMALS,
		RT_DIFFUSE,
		RT_MATERIAL,

		NUM_RENDER_TARGETS,	// is the number of simultaneous render targets which are used for filling G-buffer
	};

public:
			D3D10RenderTargetChain();
			~D3D10RenderTargetChain();

	void	Initialize( UINT screenWidth, UINT screenHeight );
	void	Shutdown();

	void	Bind();		// binds the render targets to the pipeline
	void	Unbind();	// unbinds the render targets and restores the main render target

	// this is needed for rendering those objects that update only the diffuse portion of the G-buffer (e.g. skyboxes, backgrounds)
	void	SetDiffuseRT();

			// Clears the render targets to default values.
	void	Clear();

public:
	TFixedArray< DXPtr< ID3D10ShaderResourceView >,	NUM_RENDER_TARGETS >	renderTargetSRViews;
	TFixedArray< DXPtr< ID3D10RenderTargetView >,	NUM_RENDER_TARGETS >	renderTargetViews;
	TFixedArray< DXPtr< ID3D10Texture2D >,			NUM_RENDER_TARGETS >	renderTargetTextures;
};

//
//	ScreenQuadVertex
//
class ScreenQuadVertex {
public:
	Vec3D	XYZ;
	Vec2D	UV;

public:
	ScreenQuadVertex( const Vec3D& position, const Vec2D& texCoords )
		: XYZ( position ), UV( texCoords )
	{}
};

//
//	D3D10FullScreenQuad - full screen quad used for deferred shading & post-processing effects.
//
class D3D10FullScreenQuad {
public:
			D3D10FullScreenQuad();
			~D3D10FullScreenQuad();

	void	Initialize( D3D10Shader& screenShader );
	void	Shutdown();

public:
	void	Render( D3D10Shader& shader );

	ID3D10InputLayout *	GetScreenVertexLayout();

private:
	DXPtr< ID3D10Buffer >		vertexBuffer;
	DXPtr< ID3D10InputLayout >	vertexLayout;	// screen vertex layout
};

/*
======================================================================
	
		Render view.

======================================================================
*/

//
//	D3D10View
//
class D3D10View : public rxView {
public:
		D3D10View()
		{}

		D3D10View( const mxSceneView& view )
		{
			this->Refresh( view );
		}

		~D3D10View()
		{}

	void Refresh( const mxSceneView& view )
	{
		this->constants.Refresh( view );
	}

public:
	D3D10ViewConstants	constants;

public:
	operator	const D3D10ViewConstants &	() const	{ return this->constants; }
	operator		D3D10ViewConstants &	()			{ return this->constants; }
};

FORCEINLINE
const D3D10ViewConstants & ToD3D10View( const rxView& theView )
{
	return ( checked_cast< const D3D10View*, const rxView* >( &theView ) )->constants;
}

/*
======================================================================
	
		Render queue.

======================================================================
*/

class D3D10Model;
class D3D10GlobalLight;
class D3D10LocalLight;
class D3D10MiniLight;
class D3D10Portal;
class D3D10Billboard;
class D3D10Sky;

//
//	D3D10RenderQueue - collection of objects that can be rendered.
//
struct D3D10RenderQueue : public rxQueue {
public:
	enum ELimits
	{
	//	INITIAL_LIST_SIZE = 256,	// initial
	//	LIST_GRANULARITY = 256,		// the objects list grows by this amount

		MAX_MODELS			= 962,
		MAX_TRANSLUCENT		= 450,
		MAX_GLOBAL_LIGHTS	= 16,
		MAX_LOCAL_LIGHTS	= 450,
		MAX_MINI_LIGHTS		= 150,
		MAX_PORTALS			= 16,
		MAX_SKIES			= 4,

		MAX_QUEUE_SIZE		= 2048	// max. total number of objects that can be rendered,
	};
	void _checks()
	{
		StaticAssert(
				MAX_MODELS + MAX_TRANSLUCENT
			+ MAX_GLOBAL_LIGHTS + MAX_LOCAL_LIGHTS + MAX_MINI_LIGHTS
			+ MAX_SKIES + MAX_PORTALS
				== MAX_QUEUE_SIZE );
	}

public:
	TFixedList< D3D10Model*, MAX_MODELS >				models;			// estimated count: [0..1200]
	TFixedList< D3D10Billboard*, MAX_TRANSLUCENT >		translucent;	// estimated count: [0..300]
	TFixedList< D3D10GlobalLight*, MAX_GLOBAL_LIGHTS >	globalLights;	// estimated count: [0..10]
	TFixedList< D3D10LocalLight*, MAX_LOCAL_LIGHTS >	localLights;	// estimated count: [0..150]
	TFixedList< D3D10MiniLight*, MAX_MINI_LIGHTS >		miniLights;		// estimated count: [0..450]
	TFixedList< D3D10Portal*, MAX_PORTALS >				portals;		// estimated count: [0..16]
	TFixedList< D3D10Sky*, MAX_SKIES >					skies;			// estimated count: [0..1]

public:
				D3D10RenderQueue();
				~D3D10RenderQueue();

	// Returns the total number of objects in this render queue.
	SizeT Num() const
	{
		return models.Num() 
			+ translucent.Num()
			+ globalLights.Num() + localLights.Num() + miniLights.Num() 
			+ portals.Num()
			+ skies.Num();
	}

	// Empties the internal list of objects.
	void Clear()
	{
		models		.Reset();
		translucent	.Reset();
		globalLights.Reset();
		localLights	.Reset();
		miniLights	.Reset();
		portals		.Reset();
		skies		.Reset();
	}
};

FORCEINLINE
D3D10RenderQueue & ToD3D10Queue( rxQueue& queue ) {
	return *( checked_cast< D3D10RenderQueue*, rxQueue* >( &queue ) );
}

/*
======================================================================
	
		Mesh drawing.

======================================================================
*/

//
//	D3D10Model
//
class D3D10Model : public rxModel {
public:
			D3D10Model();
			~D3D10Model();

	void	Setup( const rxModelDescription& desc );
	void	Destroy();

	//
	//	Override ( rxDrawEntity ) :
	//

	// normal rendering procedure
	void	Render( const rxView& view, rxQueue& queue );
	void	Remove();

	//
	//	Override ( rxModel ) :
	//
	void	SetTransform( const Matrix4& newWorldTransform );
	void	SetMaterial( rxMaterial* newMaterial );
	void	SetGeometry( const rxDynMeshData* newMesh );
	bool	IsOk() const;

public:
	// render geometry for filling G-buffer
	void	RenderGeometry();

	// render shadow for shadow mapping
	void	RenderShadowMap();

//	void	RenderShadowVolume( const rxView& view );

	const Vec3D & GetOrigin() const;

public:
	Matrix4					worldTransform;
	
	DXPtr< ID3D10Buffer >	pVB;
	DXPtr< ID3D10Buffer >	pIB;	// can be null if non-indexed drawing is used
	
	rxBatch			batch;

	TPtr< D3D10Material >	material;

	FLOAT		depth;	// for sorting primitives by depth
};

/*
======================================================================
	
		Scene lighting.

======================================================================
*/

class D3D10LightStage;

//
//	D3D10Light - base class for all D3D10 lights.
//
struct D3D10Light {
public:
	D3D10Light()
	{}

	virtual ~D3D10Light();

private:


};

//
//	D3D10GlobalLight - light with infinite extent, rendered with a full-screen quad.
//
struct D3D10GlobalLight : D3D10Light
{
	D3D10GlobalLight()
	{}

	virtual void RenderGlobalLight() = 0;
};

//
//	D3D10LocalLight - light with limited extent, rendered as a shape.
//
struct D3D10LocalLight : D3D10Light
{
	D3D10LocalLight()
	{}

	virtual void RenderLocalLight( const D3D10ViewConstants& view ) = 0;
};

//
//	D3D10DirectionalLight
//
struct D3D10DirectionalLight : D3D10GlobalLight, public rxParallelLight
{
	D3D10DirectionalLight( const rxLightDescription& desc, const Vec3D& direction );
	~D3D10DirectionalLight();

	//
	//	Override ( rxParallelLight ) :
	//
	const Vec3D &	GetDirection() const;
	void			SetDirection( const Vec3D& newDir );

	//
	//	Override ( rxLight ) :
	//
	void			SetColor( const ColorRGB& newDiffuseColor );

	//
	//	Override ( D3D10GlobalLight ) :
	//
	void			RenderGlobalLight();

	//
	//	Override ( rxDrawEntity ) :
	//
	void			Render( const rxView& view, rxQueue& queue );
	void			Remove();

public:
	D3D10ParallelLightData		data;	// light data sent to the shader
};

//
//	D3D10PointLight
//
struct D3D10PointLight : D3D10LocalLight, public rxPointLight
{
	D3D10PointLight( const rxLightDescription& desc, const Vec3D& origin, FLOAT range );
	~D3D10PointLight();

	Matrix4		GetWorldTransform() const;
	bool		ContainsPoint( const Vec3D& point ) const;

	//
	//	Override ( rxPointLight ) :
	//
	const Vec3D &	GetOrigin() const;
	void			SetOrigin( const Vec3D& newOrigin );

	FLOAT			GetRange() const;
	void			SetRange( FLOAT newRange );

	//
	//	Override ( rxLight ) :
	//
	void			SetColor( const ColorRGB& newDiffuseColor );

	//
	//	Override ( rxDrawEntity ) :
	//
	void			Render( const rxView& view, rxQueue& queue );
	void			Remove();

	//
	//	Override ( D3D10LocalLight ) :
	//
	void			RenderLocalLight( const D3D10ViewConstants& view );

public:
	D3D10PointLightData		data;	// light data sent to the shader
};

//
//	D3D10SpotLight
//
struct D3D10SpotLight : D3D10LocalLight, public rxSpotLight
{
	D3D10SpotLight( const rxLightDescription& desc,
					const Vec3D& origin, const Vec3D& lightDirection,
					FLOAT range, FLOAT innerAngle, FLOAT outerAngle );

	~D3D10SpotLight();

	const Matrix4 &	GetWorldTransform() const;
				
				// this test assumes that the cone bottom is a circle
	bool		ContainsPoint( const Vec3D& point ) const;

	const mxSceneView &	GetView() const;	// returns the view from this light

	//
	//	Override ( rxSpotLight ) :
	//
	void	SetInnerConeAngle( FLOAT theta );
	void	SetOuterConeAngle( FLOAT phi );

	const Vec3D &	GetDirection() const;
	void			SetDirection( const Vec3D& newDir );

	void	SetProjector( rxTexture* texture );

	//
	//	Override ( rxPointLight ) :
	//
	const Vec3D &	GetOrigin() const;
	void			SetOrigin( const Vec3D& newOrigin );

	FLOAT			GetRange() const;
	void			SetRange( FLOAT newRange );

	//
	//	Override ( rxLight ) :
	//
	void			SetColor( const ColorRGB& newDiffuseColor );

	//
	//	Override ( rxDrawEntity ) :
	//
	void			Render( const rxView& view, rxQueue& queue );
	void			Remove();

	//
	//	Override ( D3D10LocalLight ) :
	//
	void			RenderLocalLight( const D3D10ViewConstants& view );

private:
	void RecalcViewAndTransform() const;

public:
	D3D10SpotLightData		data;	// light data sent to the shader
	mutable mxSceneView		view;	// view from this light
	mutable Matrix4			worldTransform;	// for drawing light shape
	FLOAT					coneAngle;	// full cone angle (not half angle)

	// projective image (additively blended with backbuffer)
	DXPtr< ID3D10ShaderResourceView >	projector;
};

FORCEINLINE
const Matrix4& D3D10SpotLight::GetWorldTransform() const
{
	return this->worldTransform;
}

FORCEINLINE
const mxSceneView& D3D10SpotLight::GetView() const
{
	return this->view;
}

/*
==============================================================
	
			Deferred rendering stages.

==============================================================
*/

//
//	ERenderStage - enumerates types of deferred rendering stages.
//
enum ERenderStage
{
	RS_Unknown	= 0, // unknown or custom
	RS_GBuffer,
	RS_Lights,
	RS_Alpha,
	RS_Post
};

//
//	D3D10RenderStage - base class for all D3D10 render stages.
//
struct D3D10RenderStage
{
	D3D10RenderStage()
	{}
	~D3D10RenderStage()
	{}

protected:

	// These function must be called in derived classes
	// just before main rendering procedure.

	void	PrepareRender();
};

//========================================================================

//
//	D3D10GeometryStage
//
struct D3D10GeometryStage : D3D10RenderStage
{
	D3D10GeometryStage();
	~D3D10GeometryStage();

	void	Initialize();
	void	Shutdown();

	void	Render( const D3D10ViewConstants& view, D3D10RenderQueue& renderQueue );

public://private:
	D3D10Shader_FillGBuffer		fxFillGBuffer;
	DXPtr< ID3D10InputLayout >	geometryVertexLayout;
};

//========================================================================

//
//	D3D10DebugStage - used for Testing & Debugging.
//
struct D3D10DebugStage : D3D10RenderStage
{
	enum EDebugRenderMode
	{
		Render_Normal	= 0,

		Render_DiffuseOnly,	// show diffuse color
		Render_DepthOnly,
		Render_SurfaceNormals,
	};

public:
	D3D10DebugStage();
	~D3D10DebugStage();

	void	Initialize();
	void	Shutdown();

	// returns 'true' if this stage was rendered and other stages should be skipped.
	bool	Render( D3D10RenderTargetChain& renderTargets, D3D10FullScreenQuad& screenQuad );

	void	SetMode( EDebugRenderMode newMode );

private:
	TEnum< EDebugRenderMode >	mode;	// current mode

public://public so that shaders are freely accessable
	D3D10Shader_DiffuseOnly		fxDiffuseOnly;	// diffuse color (albedo)
	D3D10Shader_RenderDepth		fxRenderDepth;	// show Z (depth)
	D3D10Shader_ShowNormals		fxShowNormals;	// show world-space normals
};

FORCEINLINE void D3D10DebugStage::SetMode( EDebugRenderMode newMode ) {
	this->mode = newMode;
}

//========================================================================

//
//	D3D10LightStageData
//
struct D3D10LightStageData
{
	// Render targets for shadow mapping.
	D3D10DepthMap						shadowMap;

	//-- Shaders ------------------------------------------------

	D3D10Shader_AmbientLight			fxAmbientLight;
	D3D10Shader_ParallelLight			fxDirectionalLight;
	D3D10Shader_PointLight				fxPointLight;
	D3D10Shader_SpotLight				fxSpotLight;

	D3D10Shader_BuildShadowMap			fxBuildShadowMap;

	D3D10Shader_SpotLight_Shadowed		fxSpotLight_Shadowed;

	// Light shapes for drawing local lights.

	DXPtr< ID3D10InputLayout >			lightShapeVertexLayout;	// vertex format for drawing light shapes
	D3D10LightShape_Sphere				pointLightShape;
	D3D10LightShape_Cylinder			spotLightShape;	// apex - in Y-direction (top radius close to 0), bottom radius = 1, height = 1

	// Render states for drawing local lights.

	// if camera is inside light volume
	DXPtr< ID3D10RasterizerState >		rsState_CullFrontFaces;
	DXPtr< ID3D10DepthStencilState >	dsState_CullFrontFaces;

	// if camera is outside light volume
	DXPtr< ID3D10RasterizerState >		rsState_CullBackFaces;
	DXPtr< ID3D10DepthStencilState >	dsState_CullBackFaces;

	//-- 'Scratchpad' -------------------------------------------

	// these are used for generating render queues for collecting shadow casters, etc.
	mxVisibleSet		tempVisibleSet;
	D3D10RenderQueue 	tempRenderQueue;

public:
	D3D10LightStageData();
	~D3D10LightStageData();

	void Initialize();
	void Shutdown();
};

//
//	D3D10LightStage
//
struct D3D10LightStage : D3D10RenderStage
{
	D3D10LightStage();
	~D3D10LightStage();

	void	Initialize();
	void	Shutdown();

	void	Render( const D3D10ViewConstants& view, D3D10RenderQueue& renderQueue );

public:

	D3D10LightStageData &	GetData();

	D3D10DepthMap &	BuildShadowMap( const mxSceneView& lightView, UINT resolution );

	// returns the total number of all created lights
	UINT	NumCreatedLights() const;

public:
	rxParallelLight *	CreateParallelLight( const Vec3D& direction,
								const rxLightDescription& desc = rxLightDescription() );

	rxPointLight *		CreatePointLight( const Vec3D& center, FLOAT radius,
								const rxLightDescription& desc = rxLightDescription() );

	rxSpotLight *		CreateSpotLight( const Vec3D& center, const Vec3D& direction, FLOAT range,
								FLOAT innerAngle, FLOAT outerAngle,
								const rxLightDescription& desc = rxLightDescription() );

	void SetAmbientLightColor( const FColor& ambientColor )
	{
		this->ambientColor = ambientColor;
	}
	const FColor & GetAmbientLightColor() const
	{
		return this->ambientColor;
	}

private:
	// render shadow-casting scene geometry into shadow map
	//void	RenderSceneToShadowMap( const mxSceneView& lightView );

private:
	D3D10LightStageData		data;

	FColor					ambientColor;

	//-- Persistent data. ---------------------------------------

	TArray< D3D10Light* >	spawnedLights;	// all created lights
};

FORCEINLINE
D3D10LightStageData & D3D10LightStage::GetData() {
	return this->data;
}

//========================================================================

//
//	D3D10AlphaStage - used for drawing transparent objects.
//
struct D3D10AlphaStage : D3D10RenderStage
{
	D3D10AlphaStage();
	~D3D10AlphaStage();

	void	Initialize();
	void	Shutdown();

	void Render( const D3D10ViewConstants& view, D3D10RenderQueue& renderQueue );

private:
};

//
//	D3D10Billboard
//
struct D3D10Billboard : public rxBillboard
{
	D3D10Billboard( rxTexture* texture, const Vec3D& origin,
					const Vec2D& size, const ColorRGB& color );
	~D3D10Billboard();

	//
	//	Override ( rxBillboard ) :
	//
	void	SetColor( const ColorRGB& newColor );
	void	SetTexture( rxTexture* newTexture );
	void	SetOrigin( const Vec3D& newOrigin );
	void	SetSize( const Vec2D& newSize );

	//
	//	Override ( rxDrawEntity ) :
	//
	void	Render( const rxView& view, rxQueue& queue );
	void	Remove();

public://private:
	Vec3D					origin;
	ColorRGB				color;
	TPtr< D3D10Texture >	texture;
	Vec2D					size;
};

//========================================================================

//
//	D3D10Sky
//
struct D3D10Sky : public rxSky
{
	virtual void RenderSky( const D3D10ViewConstants& view ) = 0;

	//
	//	Override ( rxDrawEntity ) :
	//
	void	Render( const rxView& view, rxQueue& queue );
	void	Remove();
};

//
//	D3D10Skybox
//
struct D3D10Skybox : public D3D10Sky
{
	D3D10Skybox( rxTexture* sides[AABB::NUM_SIDES] );
	~D3D10Skybox();

	//
	//	Override ( D3D10Sky ) :
	//
	void RenderSky( const D3D10ViewConstants& view );

	//
	//	Override ( rxSky ) :
	//
	void	SetOrientation( const Quat& newOrientation );
	void	SetTextures( rxTexture* sides[AABB::NUM_SIDES] );

private:
	TPtr< D3D10Texture >		textures[ AABB::NUM_SIDES ];

	Quat	orientation;

	DXPtr< ID3D10InputLayout >	pIA;
	DXPtr< ID3D10Buffer >		pVB;
	DXPtr< ID3D10Buffer >		pIB;
};

//
//	D3D10Portal
//
struct D3D10Portal : public rxPortal
{
	D3D10Portal( FLOAT width, FLOAT height );
	~D3D10Portal();

	//
	//	Override ( rxPortal ) :
	//
	void		SetLookAt( const Vec3D& lookAt );
	void		SetWorldTransform( const Matrix4& localToWorld );

	void		SetSize( FLOAT width, FLOAT height );

	void		SetBuddy( rxPortal* other );
	rxPortal *	GetBuddy();

	//
	//	Override ( rxDrawEntity ) :
	//
	void	Render( const rxView& view, rxQueue& queue );
	void	Remove();

public:
	void	DrawPortalQuadOnly();

public:
	mxSceneView				view;	// view from this portal
	Matrix4					worldTransform;
	TPtr< D3D10Portal >		twin;	// exit portal
	Vec2D					size;
};

//
//	D3D10PortalStuff
//
struct D3D10PortalStuff
{
//	D3D10Shader_Portal		fxPortal;

	//
	//	PortalQuadVertex
	//
	class PortalQuadVertex {
	public:
		Vec3D	XYZ;
		Vec2D	UV;

	public:
		PortalQuadVertex( const Vec3D& position, const Vec2D& texCoords )
			: XYZ( position ), UV( texCoords )
		{}
	};
	
	DXPtr< ID3D10Buffer >		pVB;	// quad, tristrip

public:
	D3D10PortalStuff();
	~D3D10PortalStuff();

	void Initialize();
	void Shutdown();
};

//========================================================================

class D3D10PostEffect;

//
//	D3D10PostStage - used for post-processing effects.
//
struct D3D10PostStage : D3D10RenderStage, public rxPostEffectManager
{
	D3D10PostStage();
	~D3D10PostStage();

	void	Initialize( UINT screenWidth, UINT screenHeight );
	void	Shutdown();

	void Render();

	UINT	NumPostEffects() const;

	//
	//	Override ( rxPostEffectManager ) :
	//
	void			ToggleAllEffects( bool bEnable );
	rxPostEffect *	AddColoring();
	rxPostEffect *	AddGrayScale();
	rxPostEffect *	AddBlackAndWhite();
	rxPostEffect *	AddInvertColor();
	rxPostEffect *	AddEmboss();
	rxPostEffect *	AddSharpen();
	rxPostEffect *	AddSepia();
	rxPostEffect *	AddCellShading();
	rxPostEffect *	AddModulatingWave();

	rxPostEffect *	AddBlur();
	rxPostEffect *	AddGBlur();

	rxPostEffect *	AddBloom();
	rxPostEffect *	AddDepthOfField();

	rxPostEffect *	AddMotionBlur();
	rxPostEffect *	AddRadialBlur();

	rxPostEffect *	AddHDR();
	rxPostEffect *	AddSSAO();

public://private:
	FASTBOOL						bEnabled;

	D3D10RenderTarget				accumRT;	// accumulation render target

	// Shaders.
	D3D10Shader_Combine				fxCombine;

	//D3D10Shader_Coloring			fxColoring;
	D3D10Shader_Grayscale			fxGrayscale;
	D3D10Shader_BlackAndWhite		fxBlackAndWhite;
	D3D10Shader_InvertColor			fxInvertColor;
	D3D10Shader_Emboss				fxEmboss;
	D3D10Shader_Sharpen				fxSharpen;
	D3D10Shader_Sepia				fxSepia;
//	D3D10Shader_CellShading			fxCellShading;
	D3D10Shader_ModulatingWave		fxModulatingWave;
	D3D10Shader_Blur				fxBlur;
	D3D10Shader_GBlur				fxGBlur;
	D3D10Shader_Bloom				fxBloom;
//	D3D10Shader_DoF					fxDoF;
//	D3D10Shader_MotionBlur			fxMotionBlur;
//	D3D10Shader_RadialBlur			fxRadialBlur;
//	D3D10Shader_HDR					fxHDR;
//	D3D10Shader_SSAO				fxSSAO;

	TArray< D3D10PostEffect* >		postEffects;
};

//
//	D3D10PostEffect - is the base class for all classes representing a D3D10 post-processing effect.
//
struct D3D10PostEffect : public rxPostEffect
{
public:
	D3D10PostEffect( EPostFx theType = EPostFx::PostFx_Unknown )
		: rxPostEffect( theType )
	{}
	virtual ~D3D10PostEffect()
	{}

	//
	// D3D10PostEffect interface.
	//
	virtual void Render( D3D10PostStage* postStage, ID3D10ShaderResourceView* pSourceTexture ) = 0;

	//
	//	Override ( rxPostEffectManager ) :
	//
	void	Remove();
};
MX_REFACTOR("reduce code duplication")
//
//	D3D10PostEffect_Grayscale
//
struct D3D10PostEffect_Grayscale : public D3D10PostEffect
{
	D3D10PostEffect_Grayscale();
	~D3D10PostEffect_Grayscale();

	//
	//	Override ( D3D10PostEffect ) :
	//
	void Render( D3D10PostStage* postStage, ID3D10ShaderResourceView* pSourceTexture );
};

//
//	D3D10PostEffect_BlackAndWhite
//
struct D3D10PostEffect_BlackAndWhite : public D3D10PostEffect
{
	D3D10PostEffect_BlackAndWhite();
	~D3D10PostEffect_BlackAndWhite();

	//
	//	Override ( D3D10PostEffect ) :
	//
	void Render( D3D10PostStage* postStage, ID3D10ShaderResourceView* pSourceTexture );
};

//
//	D3D10PostEffect_InvertColor
//
struct D3D10PostEffect_InvertColor : public D3D10PostEffect
{
	D3D10PostEffect_InvertColor();
	~D3D10PostEffect_InvertColor();

	//
	//	Override ( D3D10PostEffect ) :
	//
	void Render( D3D10PostStage* postStage, ID3D10ShaderResourceView* pSourceTexture );
};

//
//	D3D10PostEffect_Emboss
//
struct D3D10PostEffect_Emboss : public D3D10PostEffect
{
	D3D10PostEffect_Emboss();
	~D3D10PostEffect_Emboss();

	//
	//	Override ( D3D10PostEffect ) :
	//
	void Render( D3D10PostStage* postStage, ID3D10ShaderResourceView* pSourceTexture );
};

//
//	D3D10PostEffect_Sharpen
//
struct D3D10PostEffect_Sharpen : public D3D10PostEffect
{
	D3D10PostEffect_Sharpen();
	~D3D10PostEffect_Sharpen();

	//
	//	Override ( D3D10PostEffect ) :
	//
	void Render( D3D10PostStage* postStage, ID3D10ShaderResourceView* pSourceTexture );
};

//
//	D3D10PostEffect_Sepia
//
struct D3D10PostEffect_Sepia : public D3D10PostEffect
{
	D3D10PostEffect_Sepia();
	~D3D10PostEffect_Sepia();

	//
	//	Override ( D3D10PostEffect ) :
	//
	void Render( D3D10PostStage* postStage, ID3D10ShaderResourceView* pSourceTexture );
};

//
//	D3D10PostEffect_ModulatingWave
//
struct D3D10PostEffect_ModulatingWave : public D3D10PostEffect
{
	D3D10PostEffect_ModulatingWave();
	~D3D10PostEffect_ModulatingWave();

	//
	//	Override ( D3D10PostEffect ) :
	//
	void Render( D3D10PostStage* postStage, ID3D10ShaderResourceView* pSourceTexture );
};

//
//	D3D10PostEffect_Blur
//
struct D3D10PostEffect_Blur : public D3D10PostEffect
{
	D3D10PostEffect_Blur();
	~D3D10PostEffect_Blur();

	//
	//	Override ( D3D10PostEffect ) :
	//
	void Render( D3D10PostStage* postStage, ID3D10ShaderResourceView* pSourceTexture );
};

//
//	D3D10PostEffect_GBlur
//
struct D3D10PostEffect_GBlur : public D3D10PostEffect
{
	D3D10PostEffect_GBlur();
	~D3D10PostEffect_GBlur();

	//
	//	Override ( D3D10PostEffect ) :
	//
	void Render( D3D10PostStage* postStage, ID3D10ShaderResourceView* pSourceTexture );
};

//
//	D3D10PostEffect_Bloom
//
struct D3D10PostEffect_Bloom : public D3D10PostEffect
{
	D3D10PostEffect_Bloom();
	~D3D10PostEffect_Bloom();

	//
	//	Override ( D3D10PostEffect ) :
	//
	void Render( D3D10PostStage* postStage, ID3D10ShaderResourceView* pSourceTexture );
};

/*
======================================================================
	
				Miscellaneous.

======================================================================
*/

//
//	D3D10SkyData
//
struct D3D10SkyData
{
	//-- Shaders ------------------------------------
	D3D10Shader_Skybox			fxSkybox;

	//-- Skybox geometry. ---------------------------

	struct SkyBoxVertex
	{
		Vec3D	XYZ;
		Vec2D	UV;

		SkyBoxVertex()
		{}
		SkyBoxVertex( const Vec3D& position, const Vec2D& texCoords )
			: XYZ( position )
			, UV( texCoords )
		{}
	};
	typedef UINT16	SkyBoxIndexType;

	// Unit cube ( -0.5..+0.5 ).

	DXPtr< ID3D10InputLayout >	pIL;
	DXPtr< ID3D10Buffer >		pVB;
	DXPtr< ID3D10Buffer >		pIB;
	UINT 						numVertices;
	UINT 						numIndices;

public:
	D3D10SkyData();
	~D3D10SkyData();

	void Initialize();
	void Shutdown();
};

//
//	D3D10ParticleSystemData
//
struct D3D10ParticleSystemData
{
	//--- Billboards ----------------------------------

	D3D10Shader_Billboard	fxBillboard;

	struct BillboardVertex
	{
		Vec3D	XYZ;
		Vec2D	UV;

		BillboardVertex()
		{}
		BillboardVertex( const Vec3D& position, const Vec2D& uv )
			: XYZ( position )
			, UV( uv )
		{}
	};
	typedef UINT16	BillboardIndexType;

	DXPtr< ID3D10InputLayout >	pIL;
	DXPtr< ID3D10Buffer >		pVB;	// quad, tristrip

	//----------------------------------------

	struct ParticleVertex
	{
		Vec3D	XYZ;
		Vec3D	Direction;
		Vec2D	Size;
		FLOAT	LifeTime;

		ParticleVertex()
		{}
	};

public:
	D3D10ParticleSystemData();
	~D3D10ParticleSystemData();

	void Initialize();
	void Shutdown();
};

//
//	D3D10MiscData
//
struct D3D10MiscData
{
	//--- Misc shaders. --------------------------
	D3D10Shader_ShowRenderTarget	fxShowRT;

	//--- Skies ----------------------------------
	D3D10SkyData				skyData;

	//--- Particles ------------------------------
	D3D10ParticleSystemData		particleSysData;

	//--------------------------------------------

	// white texture (resolution: 2x2)
	DXPtr< ID3D10ShaderResourceView >	srvWhiteTexture;

	// black texture (resolution: 2x2)
	DXPtr< ID3D10ShaderResourceView >	srvBlackTexture;

public:
	D3D10MiscData();
	~D3D10MiscData();

	void Initialize();
	void Shutdown();
};

/*
======================================================================
	
				Main scene renderer.

======================================================================
*/

//
//	D3D10Scene
//
class D3D10Scene : public rxScene {
public:
	D3D10GlobalShaderVars &		GetShaderVars();

	D3D10RenderStage *			GetCurrentRenderStage();
	void						SetCurrentRenderStage( D3D10RenderStage* pStage );

	D3D10RenderTargetChain &	GetRenderTargetChain();
	D3D10FullScreenQuad &		GetFullscreenQuad();

	const D3D10ViewConstants &	GetViewContants() const;

	D3D10GeometryStage &		GetGeometryStage();
	D3D10LightStage &			GetLightStage();
	D3D10PostStage &			GetPostStage();

	D3D10MiscData &				GetMiscData();

	//
	//	Override ( rxScene ) :
	//

	rxModel *			CreateModel( const rxModelDescription& desc );

	//-----------------------------------------------------------------------
	//	Dynamic lights.
	//-----------------------------------------------------------------------

	rxParallelLight *	CreateParallelLight( const Vec3D& direction,
								const rxLightDescription& desc = rxLightDescription() );

	rxPointLight *		CreatePointLight( const Vec3D& center, FLOAT radius,
								const rxLightDescription& desc = rxLightDescription() );

	rxSpotLight *		CreateSpotLight(const Vec3D& center, const Vec3D& direction, FLOAT range,
										FLOAT innerAngle, FLOAT outerAngle,
										const rxLightDescription& desc = rxLightDescription() );

	void				SetAmbientLightColor( const FColor& ambientColor );
	const FColor &		GetAmbientLightColor() const;

	//-----------------------------------------------------------------------
	//	Billboards.
	//-----------------------------------------------------------------------

	rxBillboard *		CreateBillboard( rxTexture* texture, const Vec3D& origin,
								const Vec2D& size, const ColorRGB& color = FColor::WHITE );

	//-----------------------------------------------------------------------
	//	Sky model.
	//-----------------------------------------------------------------------

	rxSky *				CreateSkybox( rxTexture* sides[AABB::NUM_SIDES] );

	//-----------------------------------------------------------------------
	//	Transformative portals.
	//-----------------------------------------------------------------------

	rxPortal *	CreatePortal( FLOAT width, FLOAT height );

	//-----------------------------------------------------------------------
	//	Post-processing.
	//-----------------------------------------------------------------------

	rxPostEffectManager* GetPostEffectManager();

public:
	void	RemoveModel( D3D10Model* theModel );
	void	RemoveSky( D3D10Sky* theSky );

public:
	// Internal functions.
	
	friend class D3D10Renderer;

			D3D10Scene();
			~D3D10Scene();

	void	Initialize( UINT screenWidth, UINT screenHeight );
	void	Shutdown();

	void	DrawScene( const mxSceneView& view, mxScene* scene );

			// Generates the visible set and a render queue given a scene view.
	void	BuildRenderQueue( const mxSceneView& view,
				D3D10View &OutFullView, mxVisibleSet &OutVisibleSet, D3D10RenderQueue &OutQueue );

private:
			// Render primitives not affected by lights.
	void	RenderUnlitPrimitives( const D3D10ViewConstants& view, D3D10RenderQueue& renderQueue );

private:
	D3D10RenderTargetChain		renderTargetChain;
	D3D10FullScreenQuad			fullScreenQuad;

	TPtr< mxScene >				scene;	// the scene we're drawing
	TPtr< mxSpatialDatabase >	spatialHash;

	D3D10View					cachedView;		// precomputed matrices, etc.

	D3D10GlobalShaderVars		shaderVars;		// global shader variables

	//--- Render stages ------------------------------------

	D3D10GeometryStage			geometryStage;
	D3D10DebugStage				debugStage;
	D3D10LightStage				lightingStage;
	D3D10AlphaStage				alphaStage;
	D3D10PostStage				postStage;

	TPtr< D3D10RenderStage >	currentStage;

	// these were calculated in the last frame
	D3D10RenderQueue 	renderQueue;
	mxVisibleSet		visibleSet;

	//----------------------------------

	D3D10MiscData		miscData;

	TArray< D3D10Model* >		allModels;	// all created render models
	TArray< D3D10Billboard* >	allBillboards;	MX_TODO("<- move this into alpha stage")
	TArray< D3D10Sky* >			allSkies;
	TArray< D3D10Portal* >		allPortals;
};

FORCEINLINE
D3D10GlobalShaderVars & D3D10Scene::GetShaderVars() {
	return this->shaderVars;
}

FORCEINLINE
D3D10RenderStage * D3D10Scene::GetCurrentRenderStage()
{
	return this->currentStage;
}

FORCEINLINE
void D3D10Scene::SetCurrentRenderStage( D3D10RenderStage* pStage )
{
	AssertPtr( pStage );
	this->currentStage = pStage;
}

FORCEINLINE
D3D10RenderTargetChain & D3D10Scene::GetRenderTargetChain() {
	return this->renderTargetChain;
}

FORCEINLINE
D3D10FullScreenQuad & D3D10Scene::GetFullscreenQuad() {
	return this->fullScreenQuad;
}

FORCEINLINE
const D3D10ViewConstants & D3D10Scene::GetViewContants() const {
	return this->cachedView.constants;
}

FORCEINLINE
D3D10GeometryStage & D3D10Scene::GetGeometryStage() {
	return this->geometryStage;
}

FORCEINLINE
D3D10LightStage & D3D10Scene::GetLightStage() {
	return this->lightingStage;
}

FORCEINLINE
D3D10PostStage & D3D10Scene::GetPostStage() {
	return this->postStage;
}

FORCEINLINE
D3D10MiscData & D3D10Scene::GetMiscData() {
	return this->miscData;
}

}//End of namespace abc

#endif // !__MX_D3D10_SCENE_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
