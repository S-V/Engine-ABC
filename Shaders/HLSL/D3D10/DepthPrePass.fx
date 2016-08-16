/*
=============================================================================
	File:	DepthPrePass.fx
	Desc:	A simple shader for rendering geometry in a depth pre-pass.
=============================================================================
*/

#include "Common.fxh"

float4x4	WorldViewProjectionMatrix;

struct VS_Input
{
	float3 pos : POSITION;
};

struct VS_Output
{
	float4 pos : SV_POSITION;
};

VS_Output VS_Main( VS_Input input )
{
	VS_Output output;
	output.pos = mul( float4( input.pos, 1.0 ), WorldViewProjectionMatrix );
	return output;
}

float4 PS_Main( VS_Output input ) : SV_Target
{
	return 0;
}

technique10
{
	pass
	{
		SetVertexShader( CompileShader( vs_4_0, VS_Main() ) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader( ps_4_0, PS_Main() ) );

		SetDepthStencilState( EnableDepthTestWrite, 0 );
		SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
	}
}
