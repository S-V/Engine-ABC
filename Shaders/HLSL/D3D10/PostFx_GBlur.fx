/*
=============================================================================
	File:	PostFx_GBlur.fx
	Desc:	A simple shader for blurring an image
			using weights that follow a Gaussian distribution.
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
//	This code was lifted and modified from Microsoft DirectX SDK.
//
static const int KERNEL_SIZE = 13;

float2 PixelKernelH[ KERNEL_SIZE ] =
{
    { -6, 0 },
    { -5, 0 },
    { -4, 0 },
    { -3, 0 },
    { -2, 0 },
    { -1, 0 },
    {  0, 0 },
    {  1, 0 },
    {  2, 0 },
    {  3, 0 },
    {  4, 0 },
    {  5, 0 },
    {  6, 0 },
};
float2 PixelKernelV[ KERNEL_SIZE ] =
{
    { 0, -6 },
    { 0, -5 },
    { 0, -4 },
    { 0, -3 },
    { 0, -2 },
    { 0, -1 },
    { 0,  0 },
    { 0,  1 },
    { 0,  2 },
    { 0,  3 },
    { 0,  4 },
    { 0,  5 },
    { 0,  6 },
};

// TexelKernel[i] = PixelKernel[i] / vec2(screenWidth,screenHeight)
float2 TexelKernelH[ KERNEL_SIZE ];
float2 TexelKernelV[ KERNEL_SIZE ];

static const float BlurWeights[ KERNEL_SIZE ] = 
{
    0.002216,
    0.008764,
    0.026995,
    0.064759,
    0.120985,
    0.176033,
    0.199471,
    0.176033,
    0.120985,
    0.064759,
    0.026995,
    0.008764,
    0.002216,
};

float ColorScale = 1.0f;

float4 PS_Main( VS_ScreenOutput input ) : SV_Target
{
	float4 outputColor = (float4) 0;

	int j;
	int i;
/** /
	const float width = 800;
	for (i = 0; i < KERNEL_SIZE; i++)
    {
		TexelKernelH[i] = PixelKernelH[i]/width;
	}
/**/	
	// Blur the image horizontally.
    for (i = 0; i < KERNEL_SIZE; i++)
    {    
        outputColor += SourceTexture.Sample( samplerPoint, input.texCoord + TexelKernelH[i].xy ) * BlurWeights[i];
    }

/** /	
	const float height = 600;
	for (i = 0; i < KERNEL_SIZE; i++)
    {
		texelKernelV[i] = texelKernelV[i]/height;
	}
/**/
	// Blur the image vertically.
	for (i = 0; i < KERNEL_SIZE; i++)
    {    
        outputColor += SourceTexture.Sample( samplerPoint, input.texCoord + TexelKernelV[i].xy ) * BlurWeights[i];
    }

	return outputColor * ColorScale;
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
