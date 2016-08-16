/*
=============================================================================
	File:	Debug_ShowNormals.fx
	Desc:	A simple shader for rendering normal map.
=============================================================================
*/

#include "Common.fxh"

Texture2D	NormalTexture;

VS_ScreenOutput VS_Main( VS_ScreenInput input )
{
    VS_ScreenOutput output;

    output.position = float4( input.position, 1 );
	output.texCoord = input.texCoord;

    return output;    
}

float4 PS_Main( VS_ScreenOutput input ) : SV_TARGET
{
	float4 output;
	
	float3 normal = ReadNormal( NormalTexture, input.texCoord ).xyz;
	output.rgb = normal;
	output.a = 1.0;
	
	return output;
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
