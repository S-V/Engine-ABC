/*
=============================================================================
	File:	PostFx_Bloom.fx
	Desc:	A simple shader for doing the bloom post-processing effect.
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

//
//	This is swiped from http://myheroics.wordpress.com/2008/09/04/glsl-bloom-shader/
//
float4 PS_Main( VS_ScreenOutput input ) : SV_Target
{
	float4 outputColor = (float4) 0;

	int j;
	int i;

	for( i= -4 ;i < 4; i++)
	{
		for (j = -3; j < 3; j++)
		{
			outputColor += SourceTexture.Sample( samplerPoint, input.texCoord + float2( j, i ) * 0.004 ) * 0.25;
		}
	}
	
	if (SourceTexture.Sample( samplerPoint, input.texCoord ).r < 0.3)
	{
		outputColor = outputColor*outputColor*0.012 + SourceTexture.Sample( samplerPoint, input.texCoord );
	}
	else
	{
		if (SourceTexture.Sample( samplerPoint, input.texCoord ).r < 0.5)
		{
			outputColor = outputColor*outputColor*0.009 + SourceTexture.Sample( samplerPoint, input.texCoord );
		}
		else
		{
			outputColor = outputColor*outputColor*0.0075 + SourceTexture.Sample( samplerPoint, input.texCoord );
		}
	}

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
		SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
	}
}
