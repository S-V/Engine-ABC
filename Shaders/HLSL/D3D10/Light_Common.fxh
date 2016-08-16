/*
=============================================================================
	File:	LightCommon.fxh
	Desc:	Common shader code for dynamic lights.
	Note:	The lighting is done in world space.
=============================================================================
*/

//-------------------------------------------------------------------
//	Defines & Constants
//-------------------------------------------------------------------

//#define DEBUG_POINT_LIGHTS
//#define DEBUG_SPOT_LIGHTS

// specular lighting is a bit expensive (because pow() is expensive)
#define ENABLE_SPECULAR

// use Phong lighting model, otherwise, use Blinn-Phong
#define PHONG

// use hardware bilinear 2x2 PCF for shadow mapping to get slightly better results cheaply
#define HARDWARE_PCF

// max. range a shadow-casting light source can have
#define MAX_LINEAR_DEPTH 1.e30f

//-------------------------------------------------------------------
//	Global variables
//-------------------------------------------------------------------

Texture2D	RtPosition;
Texture2D	RtDiffuse;
Texture2D	RtNormal;
Texture2D	RtAux;

//-------------------------------------------------------------------
//	Definitions
//-------------------------------------------------------------------

//
//	rxSurfaceParams - surface material parameters (used for lighting calculations).
//
struct rxSurfaceParams
{
	float3	diffuseColor;
//	float	_pad0;
	float3	specularColor;
//	float	_pad1;
	float3	position;
	float	specularIntensity;
	float3	normal;
	float	specularPower;
};

//	Accepts the screen coordinates of a pixel and returns surface parameters at that pixel.
//
void GetSurfaceParams( out rxSurfaceParams surfParams, in float2 texCoord )
{
	float4 diffuseColor	= GetDiffuse( RtDiffuse, texCoord );
	float4 normalW		= ReadNormal( RtNormal, texCoord );
	float3 positionW	= GetPosition( RtPosition, texCoord ).xyz;
	float4 material		= GetMaterial( RtAux, texCoord );

	surfParams.diffuseColor			= diffuseColor.rgb;
	surfParams.specularColor		= WHITE.rgb;
	surfParams.position				= positionW;
	surfParams.specularIntensity	= material.x;
	surfParams.normal				= normalW.xyz;
	surfParams.specularPower		= material.y;
}

//
//	rxEyeParams - camera parameters needed for lighting calculations.
//
struct rxEyeParams
{
	float3	position;	// eye position
//	float	_pad0;
	float3	lookAt;		// normalized look direction vector
};

void GetEyeParams( out rxEyeParams eyeParams )
{
	eyeParams.position	= GSceneView.EyePosition;
	eyeParams.lookAt	= GSceneView.EyeLookAt;
}

struct rxParallelLight
{
	float3	color;			// diffuse color
//	float	_pad0;
	float3	direction;		// normalized direction
//	float	_pad1;
};

struct rxPointLight
{
	float3	color;			// diffuse color
	float	range;			// range of influence (radius)
	float3	position;
//	float	_pad0;	
	float3	attenuation;
};

struct rxSpotLight
{
	float3	color;			// diffuse color
	float	range;			// range of influence
	float3	position;
	float	cosTheta;		// cosine of half inner cone angle
	float3	direction;		// normalized direction
	float	cosPhi;			// cosine of half outer cone angle
	float3	attenuation;
//	float	_pad1;
};

//
// Vertex for drawing light shapes (for local lights).
//
struct VSInput_LightShape
{
	float3	position : Position;
};

struct VSOutput_LightShape
{
	float4	position : SV_Position;
	float3	screenPos : TexCoord;
};

//-------------------------------------------------------------------
//	States
//-------------------------------------------------------------------


//-------------------------------------------------------------------
//	Functions
//-------------------------------------------------------------------

float3 ParallelLight(
	in rxSurfaceParams surface,
	in rxParallelLight light,
	in rxEyeParams view
	)
{
	float3 outputColor = (float3) 0;

    // diffuse term
	float3 lightVector = -light.direction;
	float  fDot = dot( lightVector, surface.normal );
/**/
	if ( fDot <= 0 ) {
		discard;
		return 0;
	}
/**/
    float3 diffuseColor = light.color * surface.diffuseColor * saturate( fDot );
	outputColor += diffuseColor;

	// specular term
	
#ifdef PHONG
	float3 directionToEye = normalize( view.position - surface.position );
	float3 reflectionVec = reflect( light.direction, surface.normal );
	float specularExponent = surface.specularPower;
    float specularFactor = pow( saturate( dot( reflectionVec, directionToEye ) ), specularExponent );
	float3 specularColor = specularFactor * surface.specularColor * surface.specularIntensity;
	outputColor += specularColor;
	
#else
	// Blinn-Phong
	float3 halfVector = normalize( -view.lookAt + lightVector );
	
	// Compute the angle between the half vector and normal
	float  HdotN = max( 0.0f, dot( halfVector, surface.normal ) );

	// Compute the specular colour
	float specularExponent = surface.specularPower;
	float specularFactor = pow( HdotN, specularExponent );
	float3 specularColor = specularFactor * surface.specularColor * surface.specularIntensity;
	outputColor += specularColor;
#endif

	return outputColor;
}

//---------------------------------------------------------------------------

// returns float4: resulting light color (RGB - float3) and attenuation factor (float).

float4 PointLight(
	in rxSurfaceParams surface,
	in rxPointLight light,
	in rxEyeParams view
	)
{
	float3 outputColor = (float3) 0;

	float3 lightDirection = surface.position - light.position;
	float distance = length( lightDirection );
	lightDirection /= distance;	// normalize lightDirection
	float3 lightVector = -lightDirection;

	float  fDot = dot( lightVector, surface.normal );
/**/
	if ( fDot <= 0 ) {
		discard;
		return 0;
	}
/**/
	fDot = saturate( fDot );
	// diffuse term
    float3 diffuseColor = (light.color * surface.diffuseColor) * fDot;
	outputColor += diffuseColor;

#ifdef ENABLE_SPECULAR
	
	#ifdef PHONG
		float3 directionToEye = normalize( view.position - surface.position );
		float3 reflectionVec = reflect( lightDirection, surface.normal );
		float specularExponent = surface.specularPower;
		float specularFactor = pow( saturate( dot( reflectionVec, directionToEye ) ), specularExponent );
		float3 specularColor = surface.specularIntensity * specularFactor * surface.specularColor;
		outputColor += specularColor;	
	#else
		// Blinn-Phong
		float3 halfVector = normalize( -view.lookAt + lightVector );

		// Compute the angle between the half vector and normal
		float  HdotN = max( 0.0f, dot( halfVector, surface.normal ) );

		// Compute the specular colour
		float specularExponent = surface.specularPower;
		float specularFactor = pow( HdotN, specularExponent );
		float3 specularColor = specularFactor * surface.specularColor * surface.specularIntensity;
		outputColor += specularColor;
	#endif

#endif //ENABLE_SPECULAR


	// calculate attenuation
	float attenuation = saturate( 1.0f - distance / light.range );
	
#if 0
	outputColor /= dot( light.attenuation, float3( 1.0f, distance, distance*distance ) );
#endif

	outputColor *= attenuation;

	return float4( outputColor, attenuation );
}

//---------------------------------------------------------------------------

// returns: float4 - resulting spotlight color (RGB - float3) and spotlight intensity,
// which can be used to modulate color of projective image

float4 SpotLight(
	in rxSurfaceParams surface,
	in rxSpotLight light,
	in rxEyeParams view
	)
{
	float3 outputColor = (float3) 0;

	rxPointLight  pointLight;
	pointLight.color 		= light.color;
	pointLight.position 	= light.position;
	pointLight.range 		= light.range;
	pointLight.attenuation 	= light.attenuation;

	//TODO: optimize, expand this function call and help CSE
	float4 color = PointLight( surface, pointLight, view );
	outputColor += color.rgb;

	float3 lightVector = normalize( light.position - surface.position );

	// cosine of the angle between spotlight direction and light vector
	float cosAngle = saturate( dot( light.direction, -lightVector ) );

	float spotIntensity = smoothstep( light.cosPhi, light.cosTheta, cosAngle );
	spotIntensity *= color.a;
	
	outputColor *= spotIntensity;

	return float4( outputColor, spotIntensity );
}

