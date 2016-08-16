/*
=============================================================================
	File:	Debug_RenderTarget.fx
	Desc:	A simple shader for debug rendering a render target texture.
	Note:	you can test here whatever code you need.
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

float4 PS_Main( VS_ScreenOutput input ) : SV_TARGET
{
	float4 output = (float4) 0;
	
//	float shadowDepth = SourceTexture.Sample( ShadowMapSampler, input.texCoord ).x;
//	output = float4(shadowDepth,shadowDepth,shadowDepth,1);
//	output = (shadowDepth > 0.5f) ? RED : GREEN;
//	output /= 3;
	
	return output;
}

technique10
{
	pass
	{
		SetVertexShader( CompileShader( vs_4_0, VS_Main() ) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader( ps_4_0, PS_Main() ) );
		
		SetDepthStencilState( DisableDepthTestWrite, 0 );
		SetBlendState( AdditiveBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
	}
}
