/*
=============================================================================
	File:	D3D10Shaders.h
	Desc:	This file contains C++ classes which are mirrored
			in the corresponding fixed shaders.
=============================================================================
*/

#ifndef __MX_D3D10_SHADERS_H__
#define __MX_D3D10_SHADERS_H__

namespace abc {

//
//	FXPtr - used for automatic validation of effect variables.
//
template< class T >
class FXPtr : public TPtr< T > {
public:
	FXPtr()
		: mPtr( null )
	{}
	FXPtr( T* theValue )
	{
		AssertPtr( theValue );
		Validate( theValue );

		mPtr = pointer;
	}
	explicit FXPtr( const TPtr<T>& other )
	{
		*this = other;
	}
	~FXPtr()
	{}

	T * operator -> () const
	{
		AssertPtr( mPtr );
		return mPtr;
	}
	
	T & operator * () const
	{
		AssertPtr( mPtr );
		return *mPtr;
	}

	operator T* ()
	{
		return mPtr;
	}

	void operator = ( T* theValue )
	{
		if ( null == theValue )
		{
			// shutting down
			Assert( null != mPtr );
		}
		else
		{
			// init for the first time
			Assert( null == mPtr );
			AssertPtr( theValue );
			Validate( theValue );
		}
		mPtr = theValue;
	}
	void operator = ( const TPtr<T>& other )
	{
		*this = other.mPtr;
	}

	bool operator == ( T* pObject ) const
	{
		return ( mPtr == pObject );
	}
	bool operator != ( T* pObject ) const
	{
		return ( mPtr != pObject );
	}
	bool operator == ( const TPtr<T>& other ) const
	{
		return ( mPtr == other.mPtr );
	}
	bool operator != ( const TPtr<T>& other ) const
	{
		return ( mPtr != other.mPtr );
	}

	bool IsNull() const
	{
		return ( mPtr == null );
	}
	bool IsValid() const
	{
		return ( mPtr != null );
	}

	void SetNull()
	{
		mPtr = null;
	}

private:
	void Validate( T* theValue )
	{
		if( ! theValue->IsValid() )
		{
			ID3D10EffectVariable * pFXVar = checked_cast< ID3D10EffectVariable*, T* >( theValue );
			D3D10_EFFECT_VARIABLE_DESC desc;
			ensure( pFXVar->GetDesc( &desc ) );
			sys::Error( TEXT("Failed to find effect variable %s"), desc.Name );
		}
	}

private:
	T *		mPtr;
};

/*
=====================================================================

			Global shader variables.

=====================================================================
*/

//
//	D3D10ViewConstants
//
//	Note: this structure is mirrored in shader code so if this is changed,
//	the corresponding shaders must be changed too!
//
ALIGN_16 struct D3D10ViewConstants
{
	float4x4	ViewMatrix;
	float4x4	ProjectionMatrix;
	float4x4	ViewProjMatrix;
	float4x4	InvViewProjMatrix;

	float3		EyePosition;
	float		pad;
	float3		EyeLookAt;

public:

	// Refresh - must be called after changing the render view.

	void Refresh( const mxSceneView& view )
	{
		ViewMatrix			= view.GetViewMatrix();
		ProjectionMatrix	= view.GetProjectionMatrix();
		ViewProjMatrix		= ViewMatrix * ProjectionMatrix;
		InvViewProjMatrix	= ViewProjMatrix.Inverse();

		EyePosition		= view.GetOrigin();
		EyeLookAt		= view.GetLookAt();
	}

public:
	D3D10ViewConstants()
	{
		ViewMatrix = ProjectionMatrix = ViewProjMatrix = InvViewProjMatrix = Matrix4::mat4_identity;
		EyePosition	= Vec3D::UNIT_Z;
		EyeLookAt	= Vec3D::UNIT_Z;
	}
	~D3D10ViewConstants()
	{}
};


// Forward declarations.
class D3D10Shader;

//
//	D3D10GlobalShaderVars - used for sharing global shader variables (via the effect pool).
//
//	This structure must be initialized before loading any shaders!
//
struct D3D10GlobalShaderVars {
public:
	D3D10GlobalShaderVars();
	~D3D10GlobalShaderVars();

	void Initialize( const char* fxHeaderFileName );
	void Shutdown();

	// send the given data to the GPU
	void Update( const D3D10ViewConstants* pSceneView );

public:
	// Internal functions.
	ID3D10EffectPool *	GetGlobalFXPool();

	void RegisterShader( D3D10Shader* newShader );
	void UnregisterShader( D3D10Shader* theShader );

	void OptimizeLoadedShaders();	// removes metadata from memory

private:
	// Shared variables.

	// per-frame constants
	FXPtr< ID3D10EffectVariable >	GSceneView;

	DXPtr< ID3D10EffectPool >		globalEffectPool;

	//TResourceSet< D3D10Shader >	loadedShaders;
	TArray< D3D10Shader* >			loadedShaders;
};

FORCEINLINE
ID3D10EffectPool * D3D10GlobalShaderVars::GetGlobalFXPool() {
	return globalEffectPool;
}

//
//	D3D10Shader - is a set of render states for processing geometry.
//
//	NOTE: basically, it's a wrapper around DirectX effects,
//	and it must only contain a single technique with a single pass.
//
class D3D10Shader {
public:
					D3D10Shader();
					~D3D10Shader();

	// Initialization / Destruction

	virtual void	Load( const char* filename );
	virtual void	Unload();

	// Derived classes will usually contain Bind() function with the corresponding parameter list.

	// Unbinds the shader from the pipeline.
	virtual void	Unbind();

	// Sets the state contained in the active render pass to the graphics device.
	// This function must be called before Draw()'s to correctly render geometry with this shader.
			void	Apply();

public:
	// Utility functions.

	ID3D10Effect *		GetEffect();
	ID3D10EffectPass *	GetActivePass();

	// Removes the reflection metadata from memory.
	void			Optimize();

	virtual bool	IsOk() const;

private:
	TPtr< ID3D10EffectPass >	ActivePass;
	DXPtr< ID3D10Effect >		Effect;
};

FORCEINLINE ID3D10Effect * D3D10Shader::GetEffect() {
	return this->Effect;
}

FORCEINLINE ID3D10EffectPass * D3D10Shader::GetActivePass() {
	return this->ActivePass;
}

/*
=====================================================================

			Debug shaders

=====================================================================
*/

class D3D10Shader_DiffuseOnly : public D3D10Shader {
public:
			D3D10Shader_DiffuseOnly();
			~D3D10Shader_DiffuseOnly();

	void	Load( const char* filename );

	void	Bind();
	void	Unbind();

private:
	FXPtr< ID3D10EffectShaderResourceVariable >		DiffuseTexture;
};

class D3D10Shader_RenderDepth : public D3D10Shader {
public:
			D3D10Shader_RenderDepth();
			~D3D10Shader_RenderDepth();

	void	Load( const char* filename );

	void	Bind();
	void	Unbind();

private:
	FXPtr< ID3D10EffectShaderResourceVariable >		DepthTexture;
};

class D3D10Shader_ShowNormals : public D3D10Shader {
public:
			D3D10Shader_ShowNormals();
			~D3D10Shader_ShowNormals();

	void	Load( const char* filename );

	void	Bind();
	void	Unbind();

private:
	FXPtr< ID3D10EffectShaderResourceVariable >		NormalTexture;
};

class D3D10Shader_ShowRenderTarget : public D3D10Shader {
public:
			D3D10Shader_ShowRenderTarget();
			~D3D10Shader_ShowRenderTarget();

	void	Load( const char* filename );

	void	Bind( ID3D10ShaderResourceView* sourceTex );
	void	Unbind();

private:
	FXPtr< ID3D10EffectShaderResourceVariable >		SourceTexture;
};

/*
=====================================================================

			Geometry buffer

=====================================================================
*/

//
//	D3D10MaterialData - used for rendering into G-buffer.
//
ALIGN_16 struct D3D10MaterialData
{
	float3	emissive;			// emissive color is multiplied by diffuse
	float	specularIntensity;
	float	specularPower;

public:
	D3D10MaterialData()
		: emissive(1,1,1)
		, specularIntensity( 0.0f )
		, specularPower( 1.0f )
	{}

	bool IsOk() const
	{
		return (specularPower > 0.0f);
	}
};

//
//	D3D10Shader_FillGBuffer
//
class D3D10Shader_FillGBuffer : public D3D10Shader {
public:
			D3D10Shader_FillGBuffer();
			~D3D10Shader_FillGBuffer();

	void SetMatrices( const Matrix4& worldTransform, const Matrix4& viewProj );

	// Set material parameters.
	void SetMaterial( const rxMaterial* material );

	//
	//	Override ( D3D10Shader ) :
	//
	void	Load( const char* filename );
	void	Unbind();

private:
	FXPtr< ID3D10EffectMatrixVariable >			WorldTransform;
	FXPtr< ID3D10EffectMatrixVariable >			ViewProjection;

	FXPtr< ID3D10EffectShaderResourceVariable >	DiffuseMap;
	//FXPtr< ID3D10EffectShaderResourceVariable >	DetailMap;
	FXPtr< ID3D10EffectShaderResourceVariable >	NormalMap;
	//FXPtr< ID3D10EffectShaderResourceVariable >	SpecularMap;

	FXPtr< ID3D10EffectVariable >				MaterialData;
};

/*
=====================================================================

					Light shaders

=====================================================================
*/

//
//	D3D10Shader_AmbientLight
//
class D3D10Shader_AmbientLight : public D3D10Shader {
public:
	D3D10Shader_AmbientLight();
	~D3D10Shader_AmbientLight();

	void Load( const char* filename );

	void Bind( const FColor& color );
	void Unbind();

private:
	FXPtr< ID3D10EffectVectorVariable >				LightColor;
	FXPtr< ID3D10EffectShaderResourceVariable >		DiffuseTexture;
};

//
//	D3D10LightShader
//
class D3D10LightShader : public D3D10Shader {
public:
	D3D10LightShader();
	~D3D10LightShader();

	void Load( const char* filename );

	void Unbind();

protected:
	void _SetRenderTargets();

protected:
	FXPtr< ID3D10EffectVariable >		LightData;

	FXPtr< ID3D10EffectShaderResourceVariable >		RtDiffuse;
	FXPtr< ID3D10EffectShaderResourceVariable >		RtAux;
	FXPtr< ID3D10EffectShaderResourceVariable >		RtNormal;
	FXPtr< ID3D10EffectShaderResourceVariable >		RtPosition;
};

// These structures are mirrored in shaders so if they're changed,
// the corresponding shaders must be changed too!
//
//	D3D10ParallelLightData
//
ALIGN_16 struct D3D10ParallelLightData
{
	float3	color;
	float	_pad0;
	float3	direction;
	float	_pad1;

public:
	bool IsOk() const
	{
		return (direction.IsNormalized())
			;
	}
};

//
//	D3D10Shader_ParallelLight
//
class D3D10Shader_ParallelLight : public D3D10LightShader {
public:
	D3D10Shader_ParallelLight();
	~D3D10Shader_ParallelLight();

	void Bind( const D3D10ParallelLightData& lightData );
};

//
//	D3D10Shader_LocalLight - for (possibly, shadow-casting) lights with finite extent.
//
class D3D10Shader_LocalLight : public D3D10LightShader {
protected:
	D3D10Shader_LocalLight();
	~D3D10Shader_LocalLight();

public:
	void Load( const char* filename );

protected:
	void _SetWorldViewProjMatrix( const Matrix4& mat )
	{
		float * pData = const_cast< float* >( mat.ToFloatPtr() );
		check( this->WorldViewProjection->SetMatrix( pData ) );
	}

private:
	FXPtr< ID3D10EffectMatrixVariable >		WorldViewProjection;	// for drawing light shape
};

// These structures are mirrored in shaders so if they're changed,
// the corresponding shaders must be changed too!
//
//	D3D10PointLightData
//
ALIGN_16 struct D3D10PointLightData
{
	float3	color;
	float	range;
	float3	position;
	float	_pad0;
	float3	attenuation;

public:
	bool IsOk() const
	{
		return (range >= 0)
			;
	}
};

//
//	D3D10Shader_PointLight
//
class D3D10Shader_PointLight : public D3D10Shader_LocalLight {
public:
	D3D10Shader_PointLight();
	~D3D10Shader_PointLight();

	void Bind( const Matrix4& worldViewProj, const D3D10PointLightData& lightData );
};

// These structures are mirrored in shaders so if they're changed,
// the corresponding shaders must be changed too!
//
//	D3D10SpotLightData
//
ALIGN_16 struct D3D10SpotLightData
{
	float3	color;			// diffuse color
	float	range;			// range of influence
	float3	position;
	float	cosTheta;		// cosine of half inner cone angle
	float3	direction;		// normalized direction
	float	cosPhi;			// cosine of half outer cone angle
	float3	attenuation;
	float	_pad0;

public:
	bool IsOk() const
	{
		return (range >= 0.0f)
			&& (direction.IsNormalized())
			&& (IsInRange(cosTheta, 0.0f, 1.0f) > 0)
			&& (IsInRange(cosPhi, 0.0f, 1.0f) > 0)
			&& (mxMath::ACos(cosTheta) < mxMath::ONEFOURTH_PI)
			&& (mxMath::ACos(cosPhi) < mxMath::ONEFOURTH_PI)
			;
	}
};

//
//	D3D10Shader_SpotLight
//
class D3D10Shader_SpotLight : public D3D10Shader_LocalLight {
public:
	D3D10Shader_SpotLight();
	~D3D10Shader_SpotLight();

	void Bind( const Matrix4& worldViewProj, const D3D10SpotLightData& lightData );
};

//
//	D3D10Shader_SpotLight
//
class D3D10Shader_SpotLight_Shadowed : public D3D10Shader_LocalLight {
public:
	D3D10Shader_SpotLight_Shadowed();
	~D3D10Shader_SpotLight_Shadowed();

	void	Load( const char* filename );

	void Bind( const Matrix4& WVP, const Matrix4& lightViewProj,
			const D3D10SpotLightData& lightData,
			ID3D10ShaderResourceView* projectionTex,
			ID3D10ShaderResourceView* shadowMap );

	void	Unbind();

private:
	FXPtr< ID3D10EffectMatrixVariable >			LightViewProj;
	FXPtr< ID3D10EffectShaderResourceVariable >	ProjectionTexture;
	FXPtr< ID3D10EffectShaderResourceVariable >	ShadowTexture;
};

/*
=====================================================================

					Shaders for shadow mapping.

=====================================================================
*/

//
//	D3D10Shader_BuildShadowMap
//
class D3D10Shader_BuildShadowMap : public D3D10Shader {
public:
	D3D10Shader_BuildShadowMap();
	~D3D10Shader_BuildShadowMap();

	//
	//	Override ( D3D10Shader ) :
	//
	void	Load( const char* filename );

	void	Bind( const Matrix4& lightWVP );

private:
	FXPtr< ID3D10EffectMatrixVariable >		LightWVP;
};

/*
=====================================================================

	Shaders for drawing translucent surfaces.

=====================================================================
*/

//
//	D3D10Shader_Billboard
//
class D3D10Shader_Billboard : public D3D10Shader {
public:
	D3D10Shader_Billboard();
	~D3D10Shader_Billboard();

	//
	//	Override ( D3D10Shader ) :
	//
	void	Load( const char* filename );

	void	Bind( const float4x4& wvp, const float3& color, ID3D10ShaderResourceView* texture );
	void	Unbind();

private:
	FXPtr< ID3D10EffectMatrixVariable >			WVP;
	FXPtr< ID3D10EffectVariable >				Color;
	FXPtr< ID3D10EffectShaderResourceVariable >	DiffuseTexture;
};

/*
=====================================================================

			Shaders for sky rendering.

=====================================================================
*/

//
//	D3D10Shader_Skybox
//
class D3D10Shader_Skybox : public D3D10Shader {
public:
	D3D10Shader_Skybox();
	~D3D10Shader_Skybox();

	void	Load( const char* filename );

	void	SetWorldViewProj( const Matrix4& wvp );
	void	SetTexture( ID3D10ShaderResourceView* sideTexture );

	void	Unbind();

private:
	FXPtr< ID3D10EffectMatrixVariable >			WVP;
	FXPtr< ID3D10EffectShaderResourceVariable >	SideTexture;
};

/*
=====================================================================

			Shaders for Post-processing & Image effects.

=====================================================================
*/

//
//	D3D10Shader_Bloom
//
class D3D10Shader_Bloom : public D3D10Shader {
public:
	D3D10Shader_Bloom();
	~D3D10Shader_Bloom();

	void	Load( const char* filename );

	void	Bind( ID3D10ShaderResourceView* pSourceTexture );
	void	Unbind();

private:
	FXPtr< ID3D10EffectShaderResourceVariable >	SourceTexture;
};

//
//	D3D10Shader_Grayscale
//
class D3D10Shader_Grayscale : public D3D10Shader {
public:
	D3D10Shader_Grayscale();
	~D3D10Shader_Grayscale();

	void	Load( const char* filename );

	void	Bind( ID3D10ShaderResourceView* pSourceTexture );
	void	Unbind();

private:
	FXPtr< ID3D10EffectShaderResourceVariable >	SourceTexture;
};

//
//	D3D10Shader_BlackAndWhite
//
class D3D10Shader_BlackAndWhite : public D3D10Shader {
public:
	D3D10Shader_BlackAndWhite();
	~D3D10Shader_BlackAndWhite();

	void	Load( const char* filename );

	void	Bind( ID3D10ShaderResourceView* pSourceTexture );
	void	Unbind();

private:
	FXPtr< ID3D10EffectShaderResourceVariable >	SourceTexture;
};

//
//	D3D10Shader_InvertColor
//
class D3D10Shader_InvertColor : public D3D10Shader {
public:
	D3D10Shader_InvertColor();
	~D3D10Shader_InvertColor();

	void	Load( const char* filename );

	void	Bind( ID3D10ShaderResourceView* pSourceTexture );
	void	Unbind();

private:
	FXPtr< ID3D10EffectShaderResourceVariable >	SourceTexture;
};

//
//	D3D10Shader_Emboss
//
class D3D10Shader_Emboss : public D3D10Shader {
public:
	D3D10Shader_Emboss();
	~D3D10Shader_Emboss();

	void	Load( const char* filename );

	void	Bind( ID3D10ShaderResourceView* pSourceTexture );
	void	Unbind();

private:
	FXPtr< ID3D10EffectShaderResourceVariable >	SourceTexture;
};

//
//	D3D10Shader_Sharpen
//
class D3D10Shader_Sharpen : public D3D10Shader {
public:
	D3D10Shader_Sharpen();
	~D3D10Shader_Sharpen();

	void	Load( const char* filename );

	void	Bind( ID3D10ShaderResourceView* pSourceTexture );
	void	Unbind();

private:
	FXPtr< ID3D10EffectShaderResourceVariable >	SourceTexture;
};

//
//	D3D10Shader_Sepia
//
class D3D10Shader_Sepia : public D3D10Shader {
public:
	D3D10Shader_Sepia();
	~D3D10Shader_Sepia();

	void	Load( const char* filename );

	void	Bind( ID3D10ShaderResourceView* pSourceTexture );
	void	Unbind();

private:
	FXPtr< ID3D10EffectShaderResourceVariable >	SourceTexture;
};

//
//	D3D10Shader_ModulatingWave
//
class D3D10Shader_ModulatingWave : public D3D10Shader {
public:
	D3D10Shader_ModulatingWave();
	~D3D10Shader_ModulatingWave();

	void	Load( const char* filename );

	void	Bind( ID3D10ShaderResourceView* pSourceTexture );
	void	Unbind();

private:
	FXPtr< ID3D10EffectShaderResourceVariable >	SourceTexture;
};

//
//	D3D10Shader_Blur
//
class D3D10Shader_Blur : public D3D10Shader {
public:
	D3D10Shader_Blur();
	~D3D10Shader_Blur();

	void	Load( const char* filename );

	void	Bind( ID3D10ShaderResourceView* pSourceTexture );
	void	Unbind();

private:
	FXPtr< ID3D10EffectShaderResourceVariable >	SourceTexture;
};

//
//	D3D10Shader_GBlur
//
class D3D10Shader_GBlur : public D3D10Shader {
public:
	D3D10Shader_GBlur();
	~D3D10Shader_GBlur();

	void	Load( const char* filename );

	void	Bind( ID3D10ShaderResourceView* pSourceTexture );
	void	Unbind();

private:
	FXPtr< ID3D10EffectShaderResourceVariable >	SourceTexture;
};


MX_REFACTOR("rename this into D3D10Shader_CombineFinal?")
//
//	D3D10Shader_Combine
//
class D3D10Shader_Combine : public D3D10Shader {
public:
	D3D10Shader_Combine();
	~D3D10Shader_Combine();

	void	Load( const char* filename );

	void	Bind( ID3D10ShaderResourceView* pSourceTexture );
	void	Unbind();

private:
	FXPtr< ID3D10EffectShaderResourceVariable >	SourceTexture;
};

}//End of namespace abc

#endif // !__MX_D3D10_SHADERS_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
