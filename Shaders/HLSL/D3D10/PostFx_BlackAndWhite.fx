/*
=============================================================================
	File:	PostFx_BlackAndWhite.fx
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
#if 1
	float4 color = SourceTexture.Sample( samplerLinear, input.texCoord );
	// over-saturated Black and White
	color.xyz = ( color.x + color.y + color.z ) * 0.333333f;
	
	if( color.x<0.25f || color.x>0.75f ) {
		color.x = 0.0f;
	} else {
		color.x = 1.0f;
	}
	
	if( color.y<0.25f || color.y>0.75f ) {
		color.y = 0.0f;
	} else {
		color.y = 1.0f;
	}
	
	if( color.z<0.25f || color.z>0.75f ) {
		color.z = 0.0f;
	} else {
		color.z = 1.0f;
	}

    return color;
	
#elif 1
	// supposed to be Black and White
	float4 color = SourceTexture.Sample( samplerLinear, input.texCoord );
	const float2 PixelKernel[4] =
	{
		{ 0,  1 },
		{ 1,  0 },
		{ 0, -1 },
		{-1,  0 }
	};

	float4 sum = 0;
	for( int i = 0; i < 4; i++ )
	{
		sum += saturate( 1 - dot( color.xyz, SourceTexture.Sample( samplerLinear, input.texCoord + PixelKernel[i] ).xyz ) );
	}
	return sum;

#else
	// 'Green shift'
	float4 color = SourceTexture.Sample( samplerLinear, input.texCoord );
	return float4( color.r * 0.3f, color.g * 0.59f, color.b * 0.11f, 1.0f );
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
