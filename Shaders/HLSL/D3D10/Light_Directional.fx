/*
=============================================================================
	File:	Light_Directional.fx
	Desc:	Simple shaders for rendering a directional light.
=============================================================================
*/

#include "Common.fxh"
#include "Light_Common.fxh"

rxParallelLight	LightData;

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

	float2 texCoord = input.texCoord;
	
	rxSurfaceParams  surface;
	GetSurfaceParams( surface, texCoord );

	rxEyeParams  view;
	GetEyeParams( view );

	outputColor.rgb = ParallelLight( surface, LightData, view );
	outputColor.a = 1;

	return outputColor;
}

technique10 RenderLight
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
