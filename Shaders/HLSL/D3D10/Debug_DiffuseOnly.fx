/*
=============================================================================
	File:	Debug_DiffuseOnly.fx
	Desc:	A shader for rendering with diffuse (texture) color only.
=============================================================================
*/

#include "Common.fxh"

Texture2D	DiffuseTexture;

VS_ScreenOutput VS_Main( VS_ScreenInput input )
{
    VS_ScreenOutput output;

    output.position = float4( input.position, 1 );
	output.texCoord = input.texCoord;

    return output;    
}

float4 PS_Main( VS_ScreenOutput input ) : SV_TARGET
{
	float4 color = DiffuseTexture.Sample( samplerLinear, input.texCoord );
	return color;
}

technique10 DiffuseColorOnly
{
	pass
	{
		SetVertexShader( CompileShader( vs_4_0, VS_Main() ) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader( ps_4_0, PS_Main() ) );
	}
}
