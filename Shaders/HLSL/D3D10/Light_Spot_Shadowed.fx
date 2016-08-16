/*
=============================================================================
	File:	Light_Spot_Shadowed.fx
	Desc:	Simple shaders for rendering a shadow-casting spot light
			with a projective texture.
=============================================================================
*/

#include "Common.fxh"
#include "Light_Common.fxh"

rxSpotLight	LightData;
float4x4	WorldViewProjection;
float4x4	LightViewProj;
Texture2D	ProjectionTexture;
Texture2D	ShadowTexture;

#ifdef HARDWARE_PCF

SamplerComparisonState PCF_Sampler
{
    ComparisonFunc = LESS;
    Filter = COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
    AddressU = Border;
    AddressV = Border;
    BorderColor = float4( MAX_LINEAR_DEPTH, 0, 0, 0 );
};

#else

SamplerState ShadowMapSampler
{
	Filter = MIN_MAG_LINEAR_MIP_POINT;
	
	// clamped so the shadow shows up only once
	AddressU = Clamp;
	AddressV = Clamp;
};

#endif // HARDWARE_PCF

struct VSOutput
{
	float4	position : SV_Position;
	float3	screenPos : TexCoord0;
};

VSOutput_LightShape VS_Main( VSInput_LightShape input )
{
    VSOutput_LightShape output;

	output.position	= mul( float4( input.position, 1 ), WorldViewProjection );
	output.screenPos = float3( output.position.xy, output.position.w );
	
    return output;
}

float4 PS_Main( VSOutput input ) : SV_Target
{
	float4 outputColor = (float4) 0;

	float2 screenXY = input.screenPos.xy / input.screenPos.z;
	float2 texCoord = ScreenToTexCoords( screenXY );

	rxSurfaceParams  surface;
	GetSurfaceParams( surface, texCoord );

	rxEyeParams  view;
	GetEyeParams( view );
	
	outputColor = SpotLight( surface, LightData, view );
	float spotIntensity = outputColor.a;

	//------------------------------------------------------------------------------
	
	float4 lightSpacePosH = mul( float4( surface.position, 1 ), LightViewProj );
	lightSpacePosH.xyz /= lightSpacePosH.w;
	
	float2 projTexCoords = ScreenToTexCoords( lightSpacePosH.xy );
	
	// Get shadow color.
	float depth = lightSpacePosH.z;

	// calculate shadow occlusion factor
#ifdef HARDWARE_PCF
	const float bias = 
					//0.001f;
					0.0001f;
					//0.00007f;
					//0.00005f;
	float shadowFactor = ShadowTexture.SampleCmpLevelZero( PCF_Sampler, projTexCoords, depth-bias );
#else
	float shadowDepth = ShadowTexture.Sample( ShadowMapSampler, projTexCoords ).x;
	float shadowFactor = (depth <= shadowDepth+0.001f) ? 1.0f : 0.0f;	// shadow occlusion factor
#endif // !HARDWARE_PCF

	
#if 1
	// Add projected texture color.
	float4 projTexColor = ProjectionTexture.Sample( samplerLinear, projTexCoords );
	projTexColor *= spotIntensity * 0.3f;
	outputColor += projTexColor;
#else
	// Modulate with projected texture color.
	float4 projTexColor = ProjectionTexture.Sample( samplerLinear, projTexCoords );
	outputColor *= spotIntensity * projTexColor;
#endif

	outputColor *= shadowFactor;
	
	return outputColor;
}

technique10
{
	pass
	{
		SetVertexShader( CompileShader( vs_4_0, VS_Main() ) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader( ps_4_0, PS_Main() ) );

		SetBlendState( AdditiveBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
	}
}
