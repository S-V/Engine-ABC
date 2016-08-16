/*
=============================================================================
	File:	Common.fxh
	Desc:	Common shader code.
=============================================================================
*/

//-------------------------------------------------------------------
//	Build defines
//-------------------------------------------------------------------

#define USE_NORMAL_MAPPING

//-------------------------------------------------------------------
//	Declarations
//-------------------------------------------------------------------

//
//	rxVertex
//
//	( Application (Geometry stage) -> Vertex program )
//
struct rxVertex
{
	float3	 position : Position;	// (object space)
	float3	 normal   : Normal;		// (object space)
	float3	 tangent  : Tangent;	// (object space)
	float2	 texCoord : TexCoord;	// texture coordinates
};

//
//	These structures are used for rendering full-screen quad.
//
struct VS_ScreenInput
{
    float3 position : Position;
    float2 texCoord : TexCoord;
};
struct VS_ScreenOutput
{
    float4 position : SV_Position;
    float2 texCoord : TexCoord;
};

//
//	G-buffer layout
//
struct PS_Output_GBuffer
{
	float4	position : SV_Target0;
	float4	normal   : SV_Target1;
	float4	diffuse  : SV_Target2;
	float4	aux      : SV_Target3;
};

struct rxMaterialData
{
	float3	emissive;
	float	specularIntensity;
	float	specularPower;
};
struct rxGlobalSceneView
{
	float4x4	ViewMatrix;
	float4x4	ProjectionMatrix;
	float4x4	ViewProjMatrix;
	float4x4	InvViewProjMatrix;

	float3		EyePosition;
	float		pad;
	float3		EyeLookAt;
};

//-------------------------------------------------------------------
//	Common variables
//-------------------------------------------------------------------

shared cbuffer cbPerFrameConstants
{
	rxGlobalSceneView	GSceneView;
	//float2			GInvScrRes;	// inverse of current screen resolution (1/width,1/height)
	//float				GlobalTime;	// in seconds
};

//-------------------------------------------------------------------
//	States
//-------------------------------------------------------------------

shared SamplerState samplerPoint
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Clamp;
    AddressV = Clamp;
};
//BilinearSampler
shared SamplerState samplerLinear
{
    //Filter = MIN_MAG_MIP_LINEAR;
	Filter = MIN_MAG_LINEAR_MIP_POINT;

    AddressU = Wrap;
    AddressV = Wrap;
};
shared SamplerState TrilinearSampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Clamp;
    AddressV = Clamp;
};


shared DepthStencilState DS_NormalZTestWriteNoStencil
{
    DepthEnable 	= true;
    DepthFunc 		= Less_Equal;
    DepthWriteMask 	= All;
    StencilEnable 	= false;
};
shared DepthStencilState EnableDepthTestWrite
{
    DepthEnable    			= true;
    DepthWriteMask 			= All;
};
shared DepthStencilState DisableDepthTestWrite
{
    DepthEnable				= false;
    DepthWriteMask			= Zero;
};
shared DepthStencilState EnableZTestDisableZWrite
{
    DepthEnable    			= true;
    DepthWriteMask 			= Zero;
};

shared BlendState AdditiveBlending
{
    BlendEnable[0] = true;
	BlendOp = Add;
    SrcBlend = One;
    DestBlend = One;
    RenderTargetWriteMask[0] = 0x0F;
};
shared BlendState NoBlending
{
    AlphaToCoverageEnable 	= false;
    BlendEnable[0]        	= false;
	RenderTargetWriteMask[0] = 0x0F;
};

shared RasterizerState RS_CullFront
{
    CullMode = Front;
    FillMode = Solid;
    MultisampleEnable = false;
};
shared RasterizerState RS_CullBack
{
    CullMode = Back;
    FillMode = Solid;
    MultisampleEnable = false;
};
shared RasterizerState RS_NoCull
{
	CullMode = None;
};

//-------------------------------------------------------------------
//	Functions
//-------------------------------------------------------------------

float4 GetPosition( in Texture2D positionMap, in float2 texCoord )
{
	return positionMap.Sample( samplerPoint, texCoord );
}

float4 ReadNormal( in Texture2D normalTexture, in float2 texCoord )
{
	float4 color = normalTexture.Sample( samplerPoint, texCoord );
	return float4(color.rgb * 2 - 1, color.a );
}

float4 GetDiffuse( in Texture2D diffuseTexture, in float2 texCoord )
{
	return diffuseTexture.Sample( samplerPoint, texCoord );
}

float4 GetMaterial( in Texture2D materialTexture, in float2 texCoord )
{
	return materialTexture.Sample( samplerPoint, texCoord );
}

//
// Transforms from NDC to texture space.
//
float2 ScreenToTexCoords( in float2 screenPosition )
{
	//the screen coordinates are in the range [-1,1]*[1,-1]
    //the texture coordinates are in [0,1]*[0,1]
	return 0.5f * ( float2(screenPosition.x, -screenPosition.y) + 1 );
}

//	This function is used to set the contents of the G-buffer to default values.
//
void Reset( out PS_Output_GBuffer buffer )
{
	buffer.position	= float4( 0, 0, 1, 0 );
	buffer.normal	= 0;
	buffer.diffuse	= 0;
	buffer.aux		= 1;
}

// Helper function for converting depth information to a color ( 3x8 bits integer ).
float3 FloatToColor( in float f )
{
	float3 color;
	f *= 256;
	color.x = floor( f );
	f = (f - color.x) * 256;
	color.y = floor( f );
	color.z = f - color.y;
	color.xy *= 0.00390625;	// *= 1.0 / 256
	return color;
}

float ColorToFloat( in float3 color )
{
	const float3 byteToFloat = float3( 1.0, 1.0/256, 1.0/(256 * 256) );
	return dot( color, byteToFloat );
}

//-------------------------------------------------------------------
//	Constants
//-------------------------------------------------------------------

#define MAX_DEPTH		2000.0f
#define INV_MAX_DEPTH	(1/MAX_DEPTH)

//
// Colors.
//
#define	BLACK				float4( 0.0f, 0.0f, 0.0f, 1.0f )
#define	GRAY				float4( 0.5f, 0.5f, 0.5f, 1.0f )
#define	WHITE				float4( 1.0f, 1.0f, 1.0f, 1.0f )

#define	YELLOW				float4( 1.0f, 	1.0f,	0.0f,	1.0f )
#define	BEACH_SAND			float4( 1.0f, 	0.96f,	0.62f,	1.0f )
#define	GREEN				float4( 0.0f, 	1.0f,	0.0f,	1.0f )
#define	LIGHT_YELLOW_GREEN	float4( 0.48f,	0.77f, 	0.46f, 	1.0f )
#define	DARK_YELLOW_GREEN	float4( 0.1f, 	0.48f, 	0.19f, 	1.0f )
#define	BLUE				float4( 0.0f, 	0.0f, 	1.0f, 	1.0f )
#define	RED					float4( 1.0f, 	0.0f, 	0.0f, 	1.0f )
#define	CYAN				float4( 0.0f, 	1.0f, 	1.0f, 	1.0f )
#define	MAGENTA				float4( 1.0f, 	0.0f, 	1.0f, 	1.0f )
#define	DARKBROWN			float4( 0.45f,	0.39f,	0.34f,	1.0f )

//-------------------------------------------------------------------
//	Misc helpers
//-------------------------------------------------------------------

#define	M_PI	3.14159265358979323846f
#define	M_E		2.71828182845904523536f

#define	M_DEG2RAD		(M_PI / 180.0f)
#define	M_RAD2DEG		(180.0f / M_PI)
#define DEG2RAD(x)		((x) * M_DEG2RAD)
#define RAD2DEG(x)		((x) * M_RAD2DEG)

