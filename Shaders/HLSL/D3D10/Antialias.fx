/*
=============================================================================
	File:	Antialias.fx
	Desc:	A simple shader for anti-aliasing.
=============================================================================
*/

#include "Common.fxh"

float2	pixel_size = {1.0f / 1024.0f, 1.0f / 768.0f};
float	weight = 1.5f;

VS_ScreenOutput VS_Main( VS_ScreenInput input )
{
    VS_ScreenOutput output;

    output.position = float4( input.position, 1 );
	output.texCoord = input.texCoord;

    return output;    
}

const float2 kernel[8] =
{
	float2(-1,1),
	float2(1,-1),
	float2(-1,1),
	float2(1,1),
	float2(-1,0),
	float2(1,0),
	float2(0,-1),
	float2(0,1)
};

float4 PS_Main( VS_ScreenOutput IN ): SV_TARGET
{
	float4 tex = tex2D(normalTex, IN.texCoord);
	float factor = 0.0f;

	for(int i = 0; i < 4; i++)
	{
		float4 t = tex2D(normalTex, IN.texCoord + kernel[i] * pixel_size);
		t -= tex;
		factor += dot(t, t);
	}
	factor = min(1.0, factor) * weight;
 
	float4 color = 0.0f; 
	for(int j = 0; j < 8; j++)
	{
	    color += tex2D(diffuseTex, IN.texCoord + kernel[j] * pixel_size * factor);
  	}

  	color += 2.0 * tex2D(diffuseTex, IN.texCoord);
	color = color * (1.0 / 10.0);
  
	return color;
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
