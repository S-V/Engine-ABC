/*
=============================================================================
	File:	PostFx_Sepia.fx
	Desc:	A simple shader for photographic print toning.
=============================================================================
*/

#include "Common.fxh"
#include "Light_Common.fxh"

Texture2D	SourceTexture;

VS_ScreenOutput VS_Main( VS_ScreenInput input )
{
	VS_ScreenOutput output;

	output.position = float4( input.position, 1 );
	output.texCoord = input.texCoord;

	return output;
}

float4 PS_Main( VS_ScreenOutput input ) : SV_Target
{
	float3 IConv = {0.299f, 0.587f, 0.114f};
	float4 SConv = {0.191f, -0.054f, -0.221f, 0.0f};
	float3 currS = SourceTexture.Sample( samplerLinear, input.texCoord ).rgb;
	float y = dot( IConv, currS );
	return y + SConv;	
}

technique10
{
	pass
	{
		SetVertexShader( CompileShader( vs_4_0, VS_Main() ) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader( ps_4_0, PS_Main() ) );

		SetDepthStencilState( DisableDepthTestWrite, 0 );
		SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
	}
}

