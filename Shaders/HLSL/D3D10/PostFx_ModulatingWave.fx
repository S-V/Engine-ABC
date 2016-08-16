/*
=============================================================================
	File:	PostFx_ModulatingWave.fx
	Desc:	A simple shader for screen distortion
			(similar to underwater effect).
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
    input.texCoord.y = input.texCoord.y + (sin( input.texCoord.x * 50.0f ) * 0.01f);
    input.texCoord.x = input.texCoord.x + (cos(0.5f) * 0.01f);
    
    return SourceTexture.Sample( samplerLinear, input.texCoord );
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

