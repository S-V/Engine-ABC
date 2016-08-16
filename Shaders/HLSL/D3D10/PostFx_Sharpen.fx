/*
=============================================================================
	File:	PostFx_Sharpen.fx
	Desc:	A simple shader for sharpening an image.
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
	float4 color = SourceTexture.Sample( samplerPoint, input.texCoord );

    color -= SourceTexture.Sample( samplerPoint, input.texCoord + 0.0001f ) * 10.0f;
    color += SourceTexture.Sample( samplerPoint, input.texCoord - 0.0001f ) * 10.0f;

   	return color;
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
