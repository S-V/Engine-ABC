/*
=============================================================================
	File:	Skybox.fx
	Desc:	A simple shader for rendering a skybox.
=============================================================================
*/

#include "Common.fxh"

float4x4	WVP;
Texture2D	SideTexture;

struct VS_Input
{
	float3 Pos : POSITION;
	float2 Tex : TEXCOORD0;
};

struct VS_Output
{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD0;
};

VS_Output VS_Main( VS_Input input )
{
	VS_Output output;

	output.Pos = mul( float4( input.Pos, 1.0 ), WVP );
	output.Pos.w = output.Pos.z;
	output.Tex = input.Tex;

	return output;
}

SamplerState samplerSkybox
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Clamp;
    AddressV = Clamp;
};
DepthStencilState DepthStencil_SkyBoxLast
{
	DepthEnable    	= TRUE;
	DepthWriteMask 	= ZERO;
	DepthFunc		= Less_Equal;
};

float4 PS_Main( VS_Output input ) : SV_Target
{
	float4 outputColor = SideTexture.Sample( samplerSkybox, input.Tex );
	return outputColor;
}

technique10
{
	pass
	{
		SetVertexShader( CompileShader( vs_4_0, VS_Main() ) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader( ps_4_0, PS_Main() ) );

		SetDepthStencilState( DepthStencil_SkyBoxLast, 0 );
		SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetRasterizerState( RS_NoCull );
	}
}
