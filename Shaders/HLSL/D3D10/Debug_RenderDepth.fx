/*
=============================================================================
	File:	Debug_RenderDepth.fx
	Desc:	A simple shader for rendering depth buffer.
=============================================================================
*/

#include "Common.fxh"

Texture2D	DepthTexture;	// DepthTexture == position map

VS_ScreenOutput VS_Main( VS_ScreenInput input )
{
    VS_ScreenOutput output;

    output.position = float4( input.position, 1 );
	output.texCoord = input.texCoord;

    return output;    
}

float4 PS_Main( VS_ScreenOutput input ) : SV_TARGET
{
	// DepthTexture == position map
	float x = DepthTexture.Sample( samplerPoint, input.texCoord ).z;
	//return float4( x,x,x,1 );
	return float4( FloatToColor( x ), 1.0 );
}

technique10
{
	pass
	{
		SetVertexShader( CompileShader( vs_4_0, VS_Main() ) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader( ps_4_0, PS_Main() ) );
		
		SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		SetDepthStencilState( DisableDepthTestWrite, 0 );
	}
}
