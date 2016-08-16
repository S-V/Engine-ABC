/*
=============================================================================
	File:	PostFx_Emboss.fx
	Desc:	A simple shader that implements the Emboss effect.
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
	float4 color = 0.5f;
	color -= SourceTexture.Sample( samplerPoint, input.texCoord - 0.001f ) * 2.0f; 
    color += SourceTexture.Sample( samplerPoint, input.texCoord + 0.001f ) * 2.0f;
    color = (color.x + color.y + color.z) * 0.3333333f;
	return color * 1.5f;
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
