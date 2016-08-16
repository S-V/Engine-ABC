/*
=============================================================================
	File:	OcclusionQuery.fx
	Desc:	A simple shader for for performing an occlusion query.
=============================================================================
*/

#include "Common.fxh"

float4x4	WVP;

float4 VS_Main( float3 positionW : Position ) : SV_Position
{
	return mul( float4( input.Pos, 1.0 ), WVP );
}

float4 PS_Main( VS_Output input ) : SV_Target
{
	return float4( 0 );
}

technique10
{
	pass
	{
		SetVertexShader( CompileShader( vs_4_0, VS_Main() ) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader( ps_4_0, PS_Main() ) );
	}
}
