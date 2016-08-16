/*
=============================================================================
	File:	D3D10Shaders.cpp
	Desc:	D3D10 shader system.
=============================================================================
*/
#include <precompiled.h>
#pragma hdrstop
#include <Engine.h>
#include <Renderer/D3D/D3DRenderSystem.h>

namespace abc {

/*================================
		D3D10GlobalShaderVars
================================*/

D3D10GlobalShaderVars::D3D10GlobalShaderVars()
{}

D3D10GlobalShaderVars::~D3D10GlobalShaderVars()
{}

void D3D10GlobalShaderVars::Initialize( const char* fxHeaderFileName )
{
	AssertPtr( fxHeaderFileName );

	// Compilation flags.

	UINT dwShaderFlags;
/*	D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_PACK_MATRIX_ROW_MAJOR;
	//dwShaderFlags |= D3D10_SHADER_ENABLE_BACKWARDS_COMPATIBILITY;
	DEBUG_CODE
	(
		dwShaderFlags |= ( D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION );
	)
*/
	dwShaderFlags = 0;


	ID3D10Blob * pErrorBuffer = null;

	const HRESULT  hResult = D3DX10CreateEffectPoolFromFile(
		fxHeaderFileName,
		null,							// defines
		null,							// include
		"fx_4_0",						// shader profile
		dwShaderFlags,					// HLSL compile options
		0,								// FX (effect) compile options
		d3d10::device,
		null,							// thread pump
		& this->globalEffectPool.Ptr,
		& pErrorBuffer,
		null
	);

	if ( pErrorBuffer )
	{
		sys::ShowMessage( TEXT("Error"), TEXT("%s"), (const TCHAR*) pErrorBuffer->GetBufferPointer() );
		pErrorBuffer->Release();
	}
	if( FAILED( hResult ) )
	{
		sys::Error( TEXT("Failed to create effect pool from file %s"), fxHeaderFileName );
	}

	AssertPtr( this->globalEffectPool );

	// Get the shared variables

	ID3D10Effect * pPoolEffect = this->globalEffectPool->AsEffect();
	Assert( pPoolEffect->IsValid() );

	this->GSceneView	= pPoolEffect->GetVariableByName(TEXT("GSceneView"));
}

void D3D10GlobalShaderVars::Shutdown()
{
	// NOTE: in Shader::Unload() the shader removes itself from 'this->loadedShaders'.
	//for( UINT iShader = 0; iShader < this->loadedShaders.Num(); iShader++ )
	//{
	//	this->loadedShaders[ iShader ]->Unload();
	//}

	this->globalEffectPool = null;
}

void D3D10GlobalShaderVars::Update( const D3D10ViewConstants* pSceneView )
{
	AssertPtr( pSceneView );
	check( this->GSceneView->SetRawValue( (void*) pSceneView, 0, sizeof(D3D10ViewConstants) ) );
}

void D3D10GlobalShaderVars::RegisterShader( D3D10Shader* newShader )
{
	AssertPtr( newShader );
	this->loadedShaders.Append( newShader );
}

void D3D10GlobalShaderVars::UnregisterShader( D3D10Shader* theShader )
{
	const bool bRemoved = this->loadedShaders.Remove( theShader );
	Assert( bRemoved );
}

void D3D10GlobalShaderVars::OptimizeLoadedShaders()
{
	for( UINT iShader = 0; iShader < this->loadedShaders.Num(); iShader++ )
	{
		this->loadedShaders[ iShader ]->Optimize();
	}
}

/*
	// render targets
	FXPtr< ID3D10EffectShaderResourceVariable >		RtDiffuse;
	FXPtr< ID3D10EffectShaderResourceVariable >		RtAux;
	FXPtr< ID3D10EffectShaderResourceVariable >		RtNormal;
	FXPtr< ID3D10EffectShaderResourceVariable >		RtPosition;

	this->RtDiffuse		= null;
	this->RtAux		= null;
	this->RtNormal		= null;
	this->RtPosition	= null;

void D3D10GlobalShaderVars::BindRenderTargetResources()
{
	Unimplemented;
	check( this->RtDiffuse	->SetResource( d3d10::scene->GetRenderTargetChain().renderTargetSRViews[ D3D10RenderTargetChain::RT_DIFFUSE ] ) );
	check( this->RtAux	->SetResource( d3d10::scene->GetRenderTargetChain().renderTargetSRViews[ D3D10RenderTargetChain::RT_MATERIAL ] ) );
	check( this->RtNormal	->SetResource( d3d10::scene->GetRenderTargetChain().renderTargetSRViews[ D3D10RenderTargetChain::RT_NORMALS ] ) );
	check( this->RtPosition	->SetResource( d3d10::scene->GetRenderTargetChain().renderTargetSRViews[ D3D10RenderTargetChain::RT_POSITION ] ) );
}

void D3D10GlobalShaderVars::UnbindRenderTargetResources()
{
	Unimplemented;
	check( this->RtDiffuse	->SetResource( null ) );
	check( this->RtAux	->SetResource( null ) );
	check( this->RtNormal	->SetResource( null ) );
	check( this->RtPosition	->SetResource( null ) );
}

	//this->RtDiffuse		= pPoolEffect->GetVariableByName(TEXT("RtDiffuse"))->AsShaderResource();
	//this->RtAux		= pPoolEffect->GetVariableByName(TEXT("RtAux"))->AsShaderResource();
	//this->RtNormal		= pPoolEffect->GetVariableByName(TEXT("RtNormal"))->AsShaderResource();
	//this->RtPosition	= pPoolEffect->GetVariableByName(TEXT("RtPosition"))->AsShaderResource();

*/
/*================================
		D3D10Shader
================================*/

D3D10Shader::D3D10Shader()
{}

D3D10Shader::~D3D10Shader()
{}

void D3D10Shader::Load( const char* filename )
{
	this->Effect = d3d10::LoadEffect( filename, d3d10::scene->GetShaderVars().GetGlobalFXPool() );
	
	ID3D10EffectTechnique * pActiveTechnique = this->Effect->GetTechniqueByIndex( 0 );

	D3D10_TECHNIQUE_DESC techDesc;
	check( pActiveTechnique->GetDesc( &techDesc ) );
	if ( techDesc.Passes != 1 ) {
		sys::Error( TEXT("The technique must contain a single pass") );
	}
	this->ActivePass = pActiveTechnique->GetPassByIndex( 0 );
	AssertPtr( this->IsOk() );

	d3d10::scene->GetShaderVars().RegisterShader( this );
}

void D3D10Shader::Unload()
{
	d3d10::scene->GetShaderVars().UnregisterShader( this );

	this->Effect = null;
	this->ActivePass = null;
}

void D3D10Shader::Optimize()
{
	check( this->Effect->Optimize() );
}

bool D3D10Shader::IsOk() const
{
	return this->ActivePass.IsValid()
		&& this->Effect.IsValid();
}

void D3D10Shader::Apply()
{
	check( this->ActivePass->Apply(0) );
}

void D3D10Shader::Unbind()
{
	this->Apply();
}

/*================================
	D3D10Shader_DiffuseOnly
================================*/

D3D10Shader_DiffuseOnly::D3D10Shader_DiffuseOnly()
{}

D3D10Shader_DiffuseOnly::~D3D10Shader_DiffuseOnly()
{}

void D3D10Shader_DiffuseOnly::Load( const char* filename )
{
	D3D10Shader::Load( filename );
	this->DiffuseTexture	= this->GetEffect()->GetVariableByName(TEXT("DiffuseTexture"))->AsShaderResource();
}

void D3D10Shader_DiffuseOnly::Bind()
{
	check( this->DiffuseTexture	->SetResource( d3d10::scene->GetRenderTargetChain().renderTargetSRViews[ D3D10RenderTargetChain::RT_DIFFUSE ] ) );
}

void D3D10Shader_DiffuseOnly::Unbind()
{
	check( this->DiffuseTexture	->SetResource( null ) );
	D3D10Shader::Unbind();
}

/*================================
	D3D10Shader_RenderDepth
================================*/

D3D10Shader_RenderDepth::D3D10Shader_RenderDepth()
{}

D3D10Shader_RenderDepth::~D3D10Shader_RenderDepth()
{}

void D3D10Shader_RenderDepth::Load( const char* filename )
{
	D3D10Shader::Load( filename );
	this->DepthTexture	= this->GetEffect()->GetVariableByName(TEXT("DepthTexture"))->AsShaderResource();
}

void D3D10Shader_RenderDepth::Bind()
{
	check( this->DepthTexture	->SetResource( d3d10::scene->GetRenderTargetChain().renderTargetSRViews[ D3D10RenderTargetChain::RT_POSITION ] ) );
}

void D3D10Shader_RenderDepth::Unbind()
{
	check( this->DepthTexture	->SetResource( null ) );
	D3D10Shader::Unbind();
}

/*================================
	D3D10Shader_ShowNormals
================================*/

D3D10Shader_ShowNormals::D3D10Shader_ShowNormals()
{}

D3D10Shader_ShowNormals::~D3D10Shader_ShowNormals()
{}

void D3D10Shader_ShowNormals::Load( const char* filename )
{
	D3D10Shader::Load( filename );
	this->NormalTexture	= this->GetEffect()->GetVariableByName(TEXT("NormalTexture"))->AsShaderResource();
}

void D3D10Shader_ShowNormals::Bind()
{
	check( this->NormalTexture	->SetResource( d3d10::scene->GetRenderTargetChain().renderTargetSRViews[ D3D10RenderTargetChain::RT_NORMALS ] ) );
}

void D3D10Shader_ShowNormals::Unbind()
{
	check( this->NormalTexture	->SetResource( null ) );
	D3D10Shader::Unbind();
}

/*================================
	D3D10Shader_ShowRenderTarget
================================*/

D3D10Shader_ShowRenderTarget::D3D10Shader_ShowRenderTarget()
{}

D3D10Shader_ShowRenderTarget::~D3D10Shader_ShowRenderTarget()
{}

void D3D10Shader_ShowRenderTarget::Load( const char* filename )
{
	D3D10Shader::Load( filename );
	this->SourceTexture	= this->GetEffect()->GetVariableByName(TEXT("SourceTexture"))->AsShaderResource();
}

void D3D10Shader_ShowRenderTarget::Bind( ID3D10ShaderResourceView* sourceTex )
{
	AssertPtr( sourceTex );
	check( this->SourceTexture	->SetResource( sourceTex ) );
}

void D3D10Shader_ShowRenderTarget::Unbind()
{
	check( this->SourceTexture	->SetResource( null ) );
	D3D10Shader::Unbind();
}

/*================================
	D3D10Shader_FillGBuffer
================================*/

D3D10Shader_FillGBuffer::D3D10Shader_FillGBuffer()
{}

D3D10Shader_FillGBuffer::~D3D10Shader_FillGBuffer()
{}

void D3D10Shader_FillGBuffer::SetMatrices( const Matrix4& worldTransform, const Matrix4& viewProj )
{
	check( this->WorldTransform->SetMatrix( (float*) &worldTransform ) );
	check( this->ViewProjection->SetMatrix( (float*) &viewProj ) );
}

void D3D10Shader_FillGBuffer::SetMaterial( const rxMaterial* theMaterial )
{
	D3D10Material * material = checked_cast< D3D10Material*, rxMaterial* >( const_cast<rxMaterial*>(theMaterial) );
	check( this->DiffuseMap->SetResource( material->BaseMap ) );
	check( this->NormalMap->SetResource( material->NormalMap ) );
	check( this->MaterialData->SetRawValue( & material->Data, 0, sizeof(D3D10MaterialData) ) );
}

void D3D10Shader_FillGBuffer::Load( const char* filename )
{
	D3D10Shader::Load( filename );

	this->WorldTransform	= this->GetEffect()->GetVariableByName(TEXT("WorldTransform"))->AsMatrix();
	this->ViewProjection	= this->GetEffect()->GetVariableByName(TEXT("ViewProjection"))->AsMatrix();
	
	this->DiffuseMap		= this->GetEffect()->GetVariableByName(TEXT("DiffuseMap"))->AsShaderResource();
	this->NormalMap			= this->GetEffect()->GetVariableByName(TEXT("NormalMap"))->AsShaderResource();

	this->MaterialData		= this->GetEffect()->GetVariableByName(TEXT("MaterialData"));
}

void D3D10Shader_FillGBuffer::Unbind()
{
	check( this->DiffuseMap	->SetResource( null ) );
	check( this->NormalMap	->SetResource( null ) );

	D3D10Shader::Unbind();
}

/*================================
	D3D10Shader_AmbientLight
================================*/

D3D10Shader_AmbientLight::D3D10Shader_AmbientLight()
{}

D3D10Shader_AmbientLight::~D3D10Shader_AmbientLight()
{}

void D3D10Shader_AmbientLight::Load( const char* filename )
{
	D3D10Shader::Load( filename );

	this->LightColor		= this->GetEffect()->GetVariableByName(TEXT("LightColor"))->AsVector();
	this->DiffuseTexture	= this->GetEffect()->GetVariableByName(TEXT("DiffuseTexture"))->AsShaderResource();
}

void D3D10Shader_AmbientLight::Bind( const FColor& color )
{
	FLOAT* pData = (FLOAT*) const_cast<FColor&>( color );
	check( this->LightColor->SetFloatVector( pData ) );

	check( this->DiffuseTexture	->SetResource( d3d10::scene->GetRenderTargetChain().renderTargetSRViews[ D3D10RenderTargetChain::RT_DIFFUSE ] ) );
}

void D3D10Shader_AmbientLight::Unbind()
{
	check( this->DiffuseTexture	->SetResource( null ) );
	D3D10Shader::Unbind();
}

/*================================
		D3D10LightShader
================================*/

D3D10LightShader::D3D10LightShader()
{}

D3D10LightShader::~D3D10LightShader()
{}

void D3D10LightShader::Load( const char* filename )
{
	D3D10Shader::Load( filename );

	//this->View		= this->GetEffect()->GetVariableByName(TEXT("View"));
	this->LightData = this->GetEffect()->GetVariableByName(TEXT("LightData"));

	this->RtDiffuse		= this->GetEffect()->GetVariableByName(TEXT("RtDiffuse"))->AsShaderResource();
	this->RtAux			= this->GetEffect()->GetVariableByName(TEXT("RtAux"))->AsShaderResource();
	this->RtNormal		= this->GetEffect()->GetVariableByName(TEXT("RtNormal"))->AsShaderResource();
	this->RtPosition	= this->GetEffect()->GetVariableByName(TEXT("RtPosition"))->AsShaderResource();
}

void D3D10LightShader::Unbind()
{
	check( this->RtDiffuse	->SetResource( null ) );
	check( this->RtAux		->SetResource( null ) );
	check( this->RtNormal	->SetResource( null ) );
	check( this->RtPosition	->SetResource( null ) );

	D3D10Shader::Unbind();
}

void D3D10LightShader::_SetRenderTargets()
{
	check( this->RtDiffuse	->SetResource( d3d10::scene->GetRenderTargetChain().renderTargetSRViews[ D3D10RenderTargetChain::RT_DIFFUSE ] ) );
	check( this->RtAux	->SetResource( d3d10::scene->GetRenderTargetChain().renderTargetSRViews[ D3D10RenderTargetChain::RT_MATERIAL ] ) );
	check( this->RtNormal	->SetResource( d3d10::scene->GetRenderTargetChain().renderTargetSRViews[ D3D10RenderTargetChain::RT_NORMALS ] ) );
	check( this->RtPosition	->SetResource( d3d10::scene->GetRenderTargetChain().renderTargetSRViews[ D3D10RenderTargetChain::RT_POSITION ] ) );
}

/*================================
	D3D10Shader_ParallelLight
================================*/

D3D10Shader_ParallelLight::D3D10Shader_ParallelLight()
{}

D3D10Shader_ParallelLight::~D3D10Shader_ParallelLight()
{}

void D3D10Shader_ParallelLight::Bind( const D3D10ParallelLightData& lightData )
{
	_SetRenderTargets();
	check( this->LightData->SetRawValue( (void*) &lightData, 0, sizeof(D3D10ParallelLightData) ) );
}

/*================================
	D3D10Shader_LocalLight
================================*/

D3D10Shader_LocalLight::D3D10Shader_LocalLight()
{}

D3D10Shader_LocalLight::~D3D10Shader_LocalLight()
{}

void D3D10Shader_LocalLight::Load( const char* filename )
{
	D3D10LightShader::Load( filename );

	this->WorldViewProjection = this->GetEffect()->GetVariableByName(TEXT("WorldViewProjection"))->AsMatrix();
}

/*================================
	D3D10Shader_PointLight
================================*/

D3D10Shader_PointLight::D3D10Shader_PointLight()
{}

D3D10Shader_PointLight::~D3D10Shader_PointLight()
{}

void D3D10Shader_PointLight::Bind( const Matrix4& worldViewProj, const D3D10PointLightData& lightData )
{
	_SetRenderTargets();
	_SetWorldViewProjMatrix( worldViewProj );

	check( this->LightData->SetRawValue( (void*) &lightData, 0, sizeof(D3D10PointLightData) ) );
}

/*================================
	D3D10Shader_SpotLight
================================*/

D3D10Shader_SpotLight::D3D10Shader_SpotLight()
{}

D3D10Shader_SpotLight::~D3D10Shader_SpotLight()
{}

void D3D10Shader_SpotLight::Bind( const Matrix4& worldViewProj, const D3D10SpotLightData& lightData )
{
	_SetRenderTargets();
	_SetWorldViewProjMatrix( worldViewProj );

	check( this->LightData->SetRawValue( (void*) &lightData, 0, sizeof(D3D10SpotLightData) ) );
}

/*================================
	D3D10Shader_SpotLight_Shadowed
================================*/

D3D10Shader_SpotLight_Shadowed::D3D10Shader_SpotLight_Shadowed()
{}

D3D10Shader_SpotLight_Shadowed::~D3D10Shader_SpotLight_Shadowed()
{}

void D3D10Shader_SpotLight_Shadowed::Load( const char* filename )
{
	D3D10Shader_LocalLight::Load( filename );
	this->LightViewProj		= this->GetEffect()->GetVariableByName(TEXT("LightViewProj"))->AsMatrix();
	this->ProjectionTexture	= this->GetEffect()->GetVariableByName(TEXT("ProjectionTexture"))->AsShaderResource();
	this->ShadowTexture		= this->GetEffect()->GetVariableByName(TEXT("ShadowTexture"))->AsShaderResource();
}

void D3D10Shader_SpotLight_Shadowed::Bind( const Matrix4& WVP, const Matrix4& lightViewProj,
			const D3D10SpotLightData& lightData,
			ID3D10ShaderResourceView* projectionTex,
			ID3D10ShaderResourceView* shadowMap )
{
	_SetRenderTargets();
	_SetWorldViewProjMatrix( WVP );
	check( this->LightData->SetRawValue( (void*) &lightData, 0, sizeof(D3D10SpotLightData) ) );

	float * pData = const_cast< float* >( lightViewProj.ToFloatPtr() );
	check( this->LightViewProj->SetMatrix( pData ) );

	AssertPtr( projectionTex );
	check( this->ProjectionTexture->SetResource(projectionTex) );

	AssertPtr( shadowMap );
	check( this->ShadowTexture->SetResource(shadowMap) );
}

void D3D10Shader_SpotLight_Shadowed::Unbind()
{
	check( this->ProjectionTexture->SetResource( null ) );
	check( this->ShadowTexture->SetResource( null ) );
	D3D10Shader_LocalLight::Unbind();
}

/*================================
	D3D10Shader_BuildShadowMap
================================*/

D3D10Shader_BuildShadowMap::D3D10Shader_BuildShadowMap()
{}

D3D10Shader_BuildShadowMap::~D3D10Shader_BuildShadowMap()
{}

void D3D10Shader_BuildShadowMap::Load( const char* filename )
{
	D3D10Shader::Load( filename );
	this->LightWVP		= this->GetEffect()->GetVariableByName(TEXT("LightWVP"))->AsMatrix();
}

void D3D10Shader_BuildShadowMap::Bind( const Matrix4& lightWVP )
{
	float * pData = const_cast< float* >( lightWVP.ToFloatPtr() );
	check( this->LightWVP->SetMatrix( pData ) );
}

//void D3D10Shader_BuildShadowMap::Unbind()
//{
//	check( this->LightWVP->SetMatrix( (float*) & Matrix4::mat4_identity ) );
//	D3D10Shader::Unbind();
//}

/*================================
	D3D10Shader_Billboard
================================*/

D3D10Shader_Billboard::D3D10Shader_Billboard()
{}

D3D10Shader_Billboard::~D3D10Shader_Billboard()
{}

void D3D10Shader_Billboard::Load( const char* filename )
{
	D3D10Shader::Load( filename );
	this->WVP			= this->GetEffect()->GetVariableByName(TEXT("WVP"))->AsMatrix();
	this->Color			= this->GetEffect()->GetVariableByName(TEXT("Color"));
	this->DiffuseTexture= this->GetEffect()->GetVariableByName(TEXT("DiffuseTexture"))->AsShaderResource();
}

void D3D10Shader_Billboard::Bind( const float4x4& wvp, const float3& color, ID3D10ShaderResourceView* texture )
{
	float * pData = const_cast< float* >( wvp.ToFloatPtr() );
	check( this->WVP->SetMatrix( pData ) );

	check( this->Color	->SetRawValue( (void*) &color, 0, sizeof(ColorRGB) ) );

	AssertPtr( texture );
	check( this->DiffuseTexture	->SetResource( texture ) );
}

void D3D10Shader_Billboard::Unbind()
{
	check( this->DiffuseTexture	->SetResource( null ) );
	D3D10Shader::Unbind();
}

/*================================
		D3D10Shader_Skybox
================================*/

D3D10Shader_Skybox::D3D10Shader_Skybox()
{}

D3D10Shader_Skybox::~D3D10Shader_Skybox()
{}

void D3D10Shader_Skybox::Load( const char* filename )
{
	D3D10Shader::Load( filename );
	this->WVP			= this->GetEffect()->GetVariableByName(TEXT("WVP"))->AsMatrix();
	this->SideTexture	= this->GetEffect()->GetVariableByName(TEXT("SideTexture"))->AsShaderResource();
}

void D3D10Shader_Skybox::SetWorldViewProj( const Matrix4& wvp )
{
	float * pData = const_cast< float* >( wvp.ToFloatPtr() );
	check( this->WVP->SetMatrix( pData ) );
}

void D3D10Shader_Skybox::SetTexture( ID3D10ShaderResourceView* sideTexture )
{
	AssertPtr( sideTexture );
	check( this->SideTexture	->SetResource( sideTexture ) );
}

void D3D10Shader_Skybox::Unbind()
{
	check( this->SideTexture	->SetResource( null ) );
	D3D10Shader::Unbind();
}

/*================================
		D3D10Shader_Bloom
================================*/

D3D10Shader_Bloom::D3D10Shader_Bloom()
{}

D3D10Shader_Bloom::~D3D10Shader_Bloom()
{}

void D3D10Shader_Bloom::Load( const char* filename )
{
	D3D10Shader::Load( filename );
	this->SourceTexture	= this->GetEffect()->GetVariableByName(TEXT("SourceTexture"))->AsShaderResource();
}

void D3D10Shader_Bloom::Bind( ID3D10ShaderResourceView* pSourceTexture )
{
	AssertPtr( pSourceTexture );
	check( this->SourceTexture	->SetResource( pSourceTexture ) );
}

void D3D10Shader_Bloom::Unbind()
{
	check( this->SourceTexture	->SetResource( null ) );
	D3D10Shader::Unbind();
}

/*================================
		D3D10Shader_Grayscale
================================*/

D3D10Shader_Grayscale::D3D10Shader_Grayscale()
{}

D3D10Shader_Grayscale::~D3D10Shader_Grayscale()
{}

void D3D10Shader_Grayscale::Load( const char* filename )
{
	D3D10Shader::Load( filename );
	this->SourceTexture	= this->GetEffect()->GetVariableByName(TEXT("SourceTexture"))->AsShaderResource();
}

void D3D10Shader_Grayscale::Bind( ID3D10ShaderResourceView* pSourceTexture )
{
	AssertPtr( pSourceTexture );
	check( this->SourceTexture	->SetResource( pSourceTexture ) );
}

void D3D10Shader_Grayscale::Unbind()
{
	check( this->SourceTexture	->SetResource( null ) );
	D3D10Shader::Unbind();
}

/*================================
	D3D10Shader_BlackAndWhite
================================*/

D3D10Shader_BlackAndWhite::D3D10Shader_BlackAndWhite()
{}

D3D10Shader_BlackAndWhite::~D3D10Shader_BlackAndWhite()
{}

void D3D10Shader_BlackAndWhite::Load( const char* filename )
{
	D3D10Shader::Load( filename );
	this->SourceTexture	= this->GetEffect()->GetVariableByName(TEXT("SourceTexture"))->AsShaderResource();
}

void D3D10Shader_BlackAndWhite::Bind( ID3D10ShaderResourceView* pSourceTexture )
{
	AssertPtr( pSourceTexture );
	check( this->SourceTexture	->SetResource( pSourceTexture ) );
}

void D3D10Shader_BlackAndWhite::Unbind()
{
	check( this->SourceTexture	->SetResource( null ) );
	D3D10Shader::Unbind();
}

/*================================
	D3D10Shader_InvertColor
================================*/

D3D10Shader_InvertColor::D3D10Shader_InvertColor()
{}

D3D10Shader_InvertColor::~D3D10Shader_InvertColor()
{}

void D3D10Shader_InvertColor::Load( const char* filename )
{
	D3D10Shader::Load( filename );
	this->SourceTexture	= this->GetEffect()->GetVariableByName(TEXT("SourceTexture"))->AsShaderResource();
}

void D3D10Shader_InvertColor::Bind( ID3D10ShaderResourceView* pSourceTexture )
{
	AssertPtr( pSourceTexture );
	check( this->SourceTexture	->SetResource( pSourceTexture ) );
}

void D3D10Shader_InvertColor::Unbind()
{
	check( this->SourceTexture	->SetResource( null ) );
	D3D10Shader::Unbind();
}

/*================================
	D3D10Shader_Emboss
================================*/

D3D10Shader_Emboss::D3D10Shader_Emboss()
{}

D3D10Shader_Emboss::~D3D10Shader_Emboss()
{}

void D3D10Shader_Emboss::Load( const char* filename )
{
	D3D10Shader::Load( filename );
	this->SourceTexture	= this->GetEffect()->GetVariableByName(TEXT("SourceTexture"))->AsShaderResource();
}

void D3D10Shader_Emboss::Bind( ID3D10ShaderResourceView* pSourceTexture )
{
	AssertPtr( pSourceTexture );
	check( this->SourceTexture	->SetResource( pSourceTexture ) );
}

void D3D10Shader_Emboss::Unbind()
{
	check( this->SourceTexture	->SetResource( null ) );
	D3D10Shader::Unbind();
}

/*================================
	D3D10Shader_Sharpen
================================*/

D3D10Shader_Sharpen::D3D10Shader_Sharpen()
{}

D3D10Shader_Sharpen::~D3D10Shader_Sharpen()
{}

void D3D10Shader_Sharpen::Load( const char* filename )
{
	D3D10Shader::Load( filename );
	this->SourceTexture	= this->GetEffect()->GetVariableByName(TEXT("SourceTexture"))->AsShaderResource();
}

void D3D10Shader_Sharpen::Bind( ID3D10ShaderResourceView* pSourceTexture )
{
	AssertPtr( pSourceTexture );
	check( this->SourceTexture	->SetResource( pSourceTexture ) );
}

void D3D10Shader_Sharpen::Unbind()
{
	check( this->SourceTexture	->SetResource( null ) );
	D3D10Shader::Unbind();
}

/*================================
	D3D10Shader_Sepia
================================*/

D3D10Shader_Sepia::D3D10Shader_Sepia()
{}

D3D10Shader_Sepia::~D3D10Shader_Sepia()
{}

void D3D10Shader_Sepia::Load( const char* filename )
{
	D3D10Shader::Load( filename );
	this->SourceTexture	= this->GetEffect()->GetVariableByName(TEXT("SourceTexture"))->AsShaderResource();
}

void D3D10Shader_Sepia::Bind( ID3D10ShaderResourceView* pSourceTexture )
{
	AssertPtr( pSourceTexture );
	check( this->SourceTexture	->SetResource( pSourceTexture ) );
}

void D3D10Shader_Sepia::Unbind()
{
	check( this->SourceTexture	->SetResource( null ) );
	D3D10Shader::Unbind();
}

/*================================
	D3D10Shader_ModulatingWave
================================*/

D3D10Shader_ModulatingWave::D3D10Shader_ModulatingWave()
{}

D3D10Shader_ModulatingWave::~D3D10Shader_ModulatingWave()
{}

void D3D10Shader_ModulatingWave::Load( const char* filename )
{
	D3D10Shader::Load( filename );
	this->SourceTexture	= this->GetEffect()->GetVariableByName(TEXT("SourceTexture"))->AsShaderResource();
}

void D3D10Shader_ModulatingWave::Bind( ID3D10ShaderResourceView* pSourceTexture )
{
	AssertPtr( pSourceTexture );
	check( this->SourceTexture	->SetResource( pSourceTexture ) );
}

void D3D10Shader_ModulatingWave::Unbind()
{
	check( this->SourceTexture	->SetResource( null ) );
	D3D10Shader::Unbind();
}

/*================================
	D3D10Shader_Blur
================================*/

D3D10Shader_Blur::D3D10Shader_Blur()
{}

D3D10Shader_Blur::~D3D10Shader_Blur()
{}

void D3D10Shader_Blur::Load( const char* filename )
{
	D3D10Shader::Load( filename );
	this->SourceTexture	= this->GetEffect()->GetVariableByName(TEXT("SourceTexture"))->AsShaderResource();
}

void D3D10Shader_Blur::Bind( ID3D10ShaderResourceView* pSourceTexture )
{
	AssertPtr( pSourceTexture );
	check( this->SourceTexture	->SetResource( pSourceTexture ) );
}

void D3D10Shader_Blur::Unbind()
{
	check( this->SourceTexture	->SetResource( null ) );
	D3D10Shader::Unbind();
}

/*================================
	D3D10Shader_GBlur
================================*/

D3D10Shader_GBlur::D3D10Shader_GBlur()
{}

D3D10Shader_GBlur::~D3D10Shader_GBlur()
{}

void D3D10Shader_GBlur::Load( const char* filename )
{
	D3D10Shader::Load( filename );
	this->SourceTexture	= this->GetEffect()->GetVariableByName(TEXT("SourceTexture"))->AsShaderResource();
}

void D3D10Shader_GBlur::Bind( ID3D10ShaderResourceView* pSourceTexture )
{
	AssertPtr( pSourceTexture );
	check( this->SourceTexture	->SetResource( pSourceTexture ) );
}

void D3D10Shader_GBlur::Unbind()
{
	check( this->SourceTexture	->SetResource( null ) );
	D3D10Shader::Unbind();
}


/*================================
		D3D10Shader_Combine
================================*/

D3D10Shader_Combine::D3D10Shader_Combine()
{}

D3D10Shader_Combine::~D3D10Shader_Combine()
{}

void D3D10Shader_Combine::Load( const char* filename )
{
	D3D10Shader::Load( filename );
	this->SourceTexture	= this->GetEffect()->GetVariableByName(TEXT("SourceTexture"))->AsShaderResource();
}

void D3D10Shader_Combine::Bind( ID3D10ShaderResourceView* pSourceTexture )
{
	AssertPtr( pSourceTexture );
	check( this->SourceTexture	->SetResource( pSourceTexture ) );
}

void D3D10Shader_Combine::Unbind()
{
	check( this->SourceTexture	->SetResource( null ) );
	D3D10Shader::Unbind();
}

}//End of namespace abc

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
