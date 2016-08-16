/*
=============================================================================
	File:	PostFx_Grayscale.fx
	Desc:	A simple shader for doing the 'Black and White' effect
			that converts a colored image to black and white.
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
#if 1
	// Gray scale shader
	return (color.x + color.y + color.z) * 0.3333333f;
#elif 1
	// Black and White shader, swiped from DX SDK
	const float3 LuminanceConv = { 0.2125f, 0.7154f, 0.0721f };
	return dot( color.xyz, LuminanceConv );
#endif
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
