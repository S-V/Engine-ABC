/*
=============================================================================
	File:	BuildShadowMap.fx
	Desc:	HLSL code to write a hardware depth map (depth-stencil texture).
=============================================================================
*/

#include "Common.fxh"

float4x4   LightWVP;

struct VertexShaderOutput
{
	float4	positionH : SV_Position;
};

VertexShaderOutput VS_Main( rxVertex input )
{
	VertexShaderOutput  output;

//	input.position += input.normal * 0.003; // reduce self-shadowing
	
	output.positionH = mul( float4( input.position, 1 ), LightWVP );
	return output;
}

void PS_Main( VertexShaderOutput input )
{}

technique10
{
	pass
	{
		SetVertexShader( CompileShader( vs_4_0, VS_Main() ) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader( ps_4_0, PS_Main() ) );
		
		SetDepthStencilState( DS_NormalZTestWriteNoStencil, 0 );
		SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetRasterizerState( RS_CullBack );
	}
}
