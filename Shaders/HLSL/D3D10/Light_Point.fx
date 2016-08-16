/*
=============================================================================
	File:	Light_Point.fx
	Desc:	Simple shaders for rendering a point light.
=============================================================================
*/

#include "Common.fxh"
#include "Light_Common.fxh"

rxPointLight	LightData;
float4x4		WorldViewProjection;

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
	
	outputColor.rgb = PointLight( surface, LightData, view ).rgb;
	outputColor.a = 1;

	return outputColor;
}

technique10 RenderLight
{
	pass
	{
		SetVertexShader( CompileShader( vs_4_0, VS_Main() ) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader( ps_4_0, PS_Main() ) );

		SetBlendState( AdditiveBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
	}
}
