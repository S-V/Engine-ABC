/*
=============================================================================
	File:	AmbientLight.fx
	Desc:	A simple shader for applying the ambient lighting.
=============================================================================
*/

#include "Common.fxh"
#include "Light_Common.fxh"

float4		LightColor;
Texture2D	DiffuseTexture;

VS_ScreenOutput VS_Main( VS_ScreenInput input )
{
    VS_ScreenOutput output;

    output.position = float4( input.position, 1 );
	output.texCoord = input.texCoord;

    return output;
}

float4 PS_Main( VS_ScreenOutput input ) : SV_Target
{
	float4 outputColor = (float4) 0;

	float4 diffuseColor = DiffuseTexture.Sample( samplerLinear, input.texCoord );
	outputColor = diffuseColor * LightColor;

	return outputColor;
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
