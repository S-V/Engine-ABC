/*
=============================================================================
	File:	Light_Spot.fx
	Desc:	Simple shaders for rendering a spot light.
=============================================================================
*/

#include "Common.fxh"
#include "Light_Common.fxh"

rxSpotLight	LightData;
float4x4	WorldViewProjection;

struct VS_Input
{
	float3	position : Position;
};
struct VS_Output
{
	float4	position : SV_Position;
	float3	screenPos : TexCoord;
};

VS_Output VS_Main( VS_Input input )
{
    VS_Output output;

	output.position	= mul( float4( input.position, 1 ), WorldViewProjection );
	output.screenPos = float3( output.position.xy, output.position.w );

    return output;
}

float4 PS_Main( VS_Output input ) : SV_Target
{
	float4 outputColor = (float4) 0;

	float2 screenXY = input.screenPos.xy / input.screenPos.z;
	float2 texCoord = ScreenToTexCoords( screenXY );

	rxSurfaceParams  surface;
	GetSurfaceParams( surface, texCoord );

	rxEyeParams  view;
	GetEyeParams( view );
	
	outputColor = SpotLight( surface, LightData, view );

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
