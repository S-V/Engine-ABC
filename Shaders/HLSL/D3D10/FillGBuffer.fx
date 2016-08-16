/*
=============================================================================
	File:	FillGBuffer.fx
	Desc:	HLSL code to fill geometry buffer.
=============================================================================
*/

#include "Common.fxh"

//
//	Per-object settings.
//
row_major float4x4   WorldTransform;	// world transformation matrix for object
row_major float4x4   ViewProjection;

Texture2D  DiffuseMap;
Texture2D  DetailMap;
Texture2D  NormalMap;
Texture2D  SpecularMap;

rxMaterialData	MaterialData;

//-------------------------------------------------------------------
//	Vertex program.
//-------------------------------------------------------------------

struct VertexShaderOutput
{
	float4	positionH : SV_Position;	// (clip space)
	float4	positionW : TexCoord0;		// (world space)
	float3	normalW   : Normal;			// (world space)
	float3  tangentW  : Tangent;		// (world space)
	float2	texCoord  : TexCoord1;		// texture coordinates
};

VertexShaderOutput VS_Main( rxVertex input )
{
	VertexShaderOutput  output;

	output.positionW 	= mul( float4( input.position, 1 ), WorldTransform );
	output.positionH	= mul( float4( output.positionW ), ViewProjection );
	output.normalW		= mul( input.normal, (float3x3)WorldTransform );
	output.tangentW		= mul( input.tangent, (float3x3)WorldTransform );
	output.texCoord		= input.texCoord;

	return output;
}

//-------------------------------------------------------------------
//	Fragment program.
//-------------------------------------------------------------------

PS_Output_GBuffer PS_Main( VertexShaderOutput input )
{
	PS_Output_GBuffer  output;
	Reset( output );

	//	Write world-space position.
	//
	output.position.rgb = input.positionW.xyz;


	//	Write world-space normal.
	//
#ifdef USE_NORMAL_MAPPING
	float3 normalT = NormalMap.Sample( samplerLinear, input.texCoord ).rgb * 2 - 1;

	// build orthonormal basis
	float3 N = normalize( input.normalW );
	float3 T = normalize( input.tangentW - dot( input.tangentW, N ) * N );
	float3 B = cross( N, T );

	float3 normal = input.tangentW * normalT.x + B * normalT.y + input.normalW * normalT.z;
	normal = normalize(normal);
	output.normal.rgb = normal * 0.5 + 0.5;
#else
	output.normal.rgb = input.normalW * 0.5 + 0.5;
#endif // !USE_NORMAL_MAPPING
	
	

	// Write diffuse color.
	//
	float4 diffuse = DiffuseMap.Sample( samplerLinear, input.texCoord );
	output.diffuse.rgb = diffuse.xyz * MaterialData.emissive;
	
	
	// Write auxiliary material parameters.
	//
	output.aux.x = MaterialData.specularIntensity;
	output.aux.y = MaterialData.specularPower;
	
	
	return output;
}

technique10
{
	pass
	{
		SetVertexShader( CompileShader( vs_4_0, VS_Main() ) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader( ps_4_0, PS_Main() ) );
		
		SetDepthStencilState( DS_NormalZTestWriteNoStencil, 0 );
		SetRasterizerState( RS_CullBack );
		SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
	}
}
