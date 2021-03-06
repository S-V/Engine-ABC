/*
=============================================================================
	File:	Billboard.fx
	Desc:	A simple shader for rendering a transparent billboard.
=============================================================================
*/

#include "Common.fxh"

float4x4	WVP;
float3		Color;
Texture2D	DiffuseTexture;

struct VS_Input
{
	float3 pos : POSITION;
	float2 uv : TEXCOORD0;
};

struct VS_Output
{
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD0;
};

VS_Output VS_Main( VS_Input input )
{
	VS_Output output;

	output.pos = mul( float4( input.pos, 1.0 ), WVP );
	output.uv = input.uv;

	return output;
}

float4 PS_Main( VS_Output input ) : SV_Target
{
	float4 outputColor = DiffuseTexture.Sample( samplerLinear, input.uv );
	outputColor.rgb *= Color;
	return outputColor;
}

technique10
{
	pass
	{
		SetVertexShader( CompileShader( vs_4_0, VS_Main() ) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader( ps_4_0, PS_Main() ) );

		SetDepthStencilState( EnableZTestDisableZWrite, 0 );
		SetBlendState( AdditiveBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetRasterizerState( RS_NoCull );
	}
}
