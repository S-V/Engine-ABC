/*
=============================================================================
	File:	Material.h
	Desc:	Definition of material classes used for rendering.
=============================================================================
*/

#ifndef __RX_RENDER_MATERIAL_H__
#define __RX_RENDER_MATERIAL_H__

namespace abc {

/*
====================================================================

		Color representation.

====================================================================
*/

//
//	SColor - a 32 bit ARGB color.
//
class SColor {
public:
	UINT8	A, R, G, B;	// The color is stored in A8R8G8B8 format.

public:
	SColor()
	{}

	explicit SColor( UINT32 color )
		: A( color & 0xff << 24 )
		, R( color & 0xff << 16 )
		, G( color & 0xff << 8 )
		, B( color & 0xff )
	{}

	SColor( UINT8 alpha, UINT8 red, UINT8 green, UINT8 blue )
		: A( alpha ), R( red ), G( green ), B( blue )
	{}

	UINT32	ToInt() const
	{
		return *(UINT32* ) this;
	}

	void Set( UINT8 alpha, UINT8 red, UINT8 green, UINT8 blue )
	{
		A = alpha;
		R = red;
		G = green;
		B = blue;
	}

public:
	static const SColor		BLACK;
	static const SColor		GRAY;
	static const SColor		WHITE;
	
	static const SColor		YELLOW;
	static const SColor		GREEN;
	static const SColor		BLUE;
	static const SColor		RED;
	static const SColor		CYAN;
	static const SColor		MAGENTA;

private:
	// This function is never called, it just contains compile-time assertions.
	void StaticChecks()
	{
		STATIC_CHECK( sizeof( SColor ) == sizeof( mxInt32 ) );
	}
};

//
//	ColorRGB - represents a color in RGB space.
//
class ColorRGB {
public:
	FLOAT	R, G, B;	// The red, the green and the blue components.

public:
	ColorRGB()
	{}

	ColorRGB( FLOAT red, FLOAT green, FLOAT blue )
		: R( red ), G( green ), B( blue )
	{}

	ColorRGB( const ColorRGB& other )
		: R( other.R ), G( other.G ), B( other.B )
	{}

	ColorRGB( const float3& v )
	{
		*this = *(ColorRGB*) &v;
	}

	// Sets all three components of a color.

	void Set( FLOAT red, FLOAT green, FLOAT blue )
	{
#if 0
		R = Clamp( red, 0.0f, 1.0f );
		G = Clamp( green, 0.0f, 1.0f );
		B = Clamp( blue, 0.0f, 1.0f );
#else
		R = red;
		G = green;
		B = blue;
#endif
	}

	operator		FLOAT * ()			{ return &R; }
	operator const	FLOAT * () const	{ return &R; }

	operator		float3 & ()			{ return *(float3*)this; }
	operator const	float3 & () const	{ return *(float3*)this; }

	operator		float4 () const	{ return float4( R, G, B, 1 ); }

	// Returns a reference to the i-th component of a color. The value of i must be 0, 1, or 2. 
	FLOAT &			operator [] ( IndexT i );

	//	Returns a constant reference to the i-th component of a color. The value of i must be 0, 1, or 2. 
	const FLOAT &	operator [] ( IndexT i ) const;

	//	Sets all three components to the value f. 
	ColorRGB &		operator = ( FLOAT f );

	//	Adds the color c. 
	ColorRGB &		operator += ( const ColorRGB& c );

	//	Subtracts the color c. 
	ColorRGB &		operator -= ( const ColorRGB& c );

	//	Multiplies by the color c. 
	ColorRGB &		operator *= ( const ColorRGB& c );

	//	Multiplies all three components by t. 
	ColorRGB &		operator *= ( FLOAT t );

	//	Divides all three components by t.
	ColorRGB &		operator /= ( FLOAT t );

	ColorRGB & operator = ( const ColorRGB& other )
	{
		this->R = other.R;
		this->G = other.G;
		this->B = other.B;
		return *this;
	}

	ColorRGB & operator = ( const class FColor& other );

	// Returns the negation of the color c.
	friend ColorRGB operator - ( const ColorRGB& c );	

	// Returns the sum of the colors color1 and color2.
	friend ColorRGB operator + ( const ColorRGB& color1, const ColorRGB& color2 );

	//	Returns the difference of the colors color1 and color2. 
	friend ColorRGB operator - ( const ColorRGB& color1, const ColorRGB& color2 );

	//	Returns the product of the colors color1 and color2. 
	friend ColorRGB operator * ( const ColorRGB& color1, const ColorRGB& color2 );

	//	Returns the product of the color c and the scalar t. 
	friend ColorRGB operator * ( const ColorRGB& c, FLOAT t );

	//	Returns the product of the color c and the scalar t. 
	friend ColorRGB operator * ( FLOAT t, const ColorRGB& c );

	//	Returns the product of the color c and the inverse of the scalar t. 
	friend ColorRGB operator / ( const ColorRGB& c, FLOAT t );

	//	Returns a boolean value indicating the equality of the colors color1 and color2. 
	friend bool operator == ( const ColorRGB& color1, const ColorRGB& color2 );

	//	Returns a boolean value indicating the inequality of the colors color1 and color2.
	friend bool operator != ( const ColorRGB& color1, const ColorRGB& c2 );
};

FORCEINLINE
ColorRGB & ColorRGB::operator *= ( FLOAT f )
{
	this->R *= f;
	this->G *= f;
	this->B *= f;
	return *this;
}

FORCEINLINE
ColorRGB operator * ( const ColorRGB& c, FLOAT t )
{
	return ColorRGB(
		c.R * t,
		c.G * t,
		c.B * t
	);
}

FORCEINLINE
bool operator == ( const ColorRGB& color1, const ColorRGB& color2 )
{
	return ( color1.R == color2.R )
		&& ( color1.G == color2.G )
		&& ( color1.B == color2.B );
}

//
//	FColor - a color in RGBA space; consists of 4 floating-point values.
//
class FColor {
public:
	FLOAT	R, G, B, A;

public:
	FColor()
	{}

	FColor( const float4& v )
	{
		*this = *(FColor*) &v;
	}

	FColor( FLOAT red, FLOAT green, FLOAT blue, FLOAT alpha = 1.0f )
		: R( red ), G( green ), B( blue ), A( alpha )
	{}

	FColor( const FColor& other )
		: R( other.R ), G( other.G ), B( other.B ), A( other.A )
	{}

	FColor( UINT32 rgba )
	{
		const FLOAT f = 1.0f / 255.0f;
		R = f * (FLOAT) UINT8( rgba >> 16 );
		G = f * (FLOAT) UINT8( rgba >>  8 );
		B = f * (FLOAT) UINT8( rgba >>  0 );
		A = f * (FLOAT) UINT8( rgba >> 24 );
	}

	FColor( const ColorRGB& rgb )
	{
		this->R = rgb.R;
		this->G = rgb.G;
		this->B = rgb.B;
		this->A = 1.0f;
	}

	// Sets all four components of a color.

	void Set( FLOAT red, FLOAT green, FLOAT blue, FLOAT alpha = 1.0f )
	{
#if 0
		R = Clamp( red, 0.0f, 1.0f );
		G = Clamp( green, 0.0f, 1.0f );
		B = Clamp( blue, 0.0f, 1.0f );
		A = Clamp( alpha, 0.0f, 1.0f );
#else
		R = red;
		G = green;
		B = blue;
		A = alpha;
#endif
	}

	SColor ToARGB() const
	{
		return SColor(
			UINT8( A * 255.0f ),
			UINT8( R * 255.0f ),
			UINT8( G * 255.0f ),
			UINT8( B * 255.0f ) );
	}
	
	ColorRGB ToRGB() const
	{
		return ColorRGB( R, G, B );
	}

	operator		ColorRGB ()			{ return ToRGB(); }
	operator const	ColorRGB () const	{ return ToRGB(); }

	operator		FLOAT * ()			{ return &R; }
	operator const	FLOAT * () const	{ return &R; }

	operator		float3 & ()			{ return *(float3*)this; }
	operator const	float3 & () const	{ return *(float3*)this; }

	operator		float4 & ()			{ return *(float4*)this; }
	operator const	float4 & () const	{ return *(float4*)this; }

	FColor	operator * ( FLOAT f ) const
	{
		return FColor( R * f, G * f, B * f, A * f );
	}
	FColor	operator *= ( FLOAT f ) const
	{
		return FColor( R * f, G * f, B * f, A * f );
	}
	FColor	operator * ( const FColor& other ) const
	{
		return FColor( R * other.R, G * other.G, B * other.B, A * other.A );
	}
	FColor	operator / ( FLOAT f ) const
	{
		FLOAT inv = mxMath::Reciprocal( f );
		return FColor( R * inv, G * inv, B * inv, A * inv );
	}
	FColor	operator /= ( FLOAT f ) const
	{
		FLOAT inv = mxMath::Reciprocal( f );
		return FColor( R * inv, G * inv, B * inv, A * inv );
	}

	FColor & operator = ( const FColor& other )
	{
		this->R = other.R;
		this->G = other.G;
		this->B = other.B;
		this->A = other.A;
		return *this;
	}

	friend bool operator == ( const FColor& color1, const FColor& color2 )
	{
		return ( color1.R == color2.R )
			&& ( color1.G == color2.G )
			&& ( color1.B == color2.B )
			&& ( color1.A == color2.A );
	}

	bool IsValid() const
	{
		return true;
		//return IsInRange( R, 0.0f, 1.0f )
		//	&& IsInRange( G, 0.0f, 1.0f )
		//	&& IsInRange( B, 0.0f, 1.0f )
		//	&& IsInRange( A, 0.0f, 1.0f );
	}

public:
	static const FColor		BLACK;
	static const FColor		GRAY;
	static const FColor		WHITE;
	
	static const FColor		YELLOW;
	static const FColor		BEACH_SAND;
	static const FColor		GREEN;
	static const FColor		LIGHT_YELLOW_GREEN;
	static const FColor		DARK_YELLOW_GREEN;
	static const FColor		BLUE;
	static const FColor		RED;
	static const FColor		CYAN;
	static const FColor		MAGENTA;
	static const FColor		DARKBROWN;
};

FORCEINLINE
ColorRGB & ColorRGB::operator = ( const FColor& other )
{
	this->R = other.R;
	this->G = other.G;
	this->B = other.B;
	return *this;
}

/*
====================================================================

	GPU programs (shaders)

====================================================================
*/

//
//	EShaderType
//
enum EShaderType
{
	ST_Unknown_Shader = 0,
	ST_Vertex_Program,
	ST_Fragment_Program,
	ST_Geometry_Program,
	ST_Compute_Shader,

	NumShaderTypes
};

//
//	ShaderConstantType
//
struct ShaderConstantType
{
	// This enum contains types of constants used in GPU programs.
	// Note: the order is important (optimized for range checks).
	enum EGPUConstant
	{
		Float1,
		Float2,
		Float3,
		Float4,

		Matrix2x2,
		Matrix2x3,
		Matrix2x4,
		Matrix3x2,
		Matrix3x3,
		Matrix3x4,
		Matrix4x2,
		Matrix4x3,
		Matrix4x4,

		Int1,
		Int2,
		Int3,
		Int4,
		
		Sampler1D,
		Sampler2D,
		Sampler3D,
		SamplerCube,
	};

	FORCEINLINE
	bool IsInteger( EGPUConstant type )
	{
		return (type >= Int1) && (type <= Int4);
	}

	FORCEINLINE
	bool IsFloat( EGPUConstant type )
	{
		return (type >= Float1) && (type <= Matrix4x4);
	}

	FORCEINLINE
	bool IsSampler( EGPUConstant type )
	{
		return (type >= Sampler1D) && (type <= SamplerCube);
	}
};

/*
====================================================================

	Render queue priority

====================================================================
*/

//
// EDrawOrder - enumerates render queue priorities assigned to materials.
//
enum EDrawOrder
{
	DO_BACKGROUND = 0,	// skyboxes, backgrounds representing distant scenery, environments, etc

	DO_GUI,			// GUI surfaces

	DO_PRE,			// for post-processing effects that need to be rendered before normal scene objects

	DO_OPAQUE,		// fully opaque geometry

	DO_NORMAL,		// normal scene objects

	DO_DECALS,		// scorch marks, bullet hits, explosion marks, etc
	DO_TRANSPARENT,	// alpha-blended surfaces

	DO_PORTAL,		// portal surfaces

	DO_NEAREST,		// weapons, smoke puffs, etc
	DO_POST_PROCESS,

	DO_LAST,
};

//
//	ETextureLayer - enumerates texture layers a material can have.
//
enum ETextureLayer
{
	TL_Diffuse	= 0,	// diffuse color (albedo) (RGBA)
	TL_Detail,			// detail texture (RGBA)
	TL_Normals,			// normals (XYZ)
	TL_Specular,		// specular or gloss map (L8 - specular intensity)
//	TL_Reflection,		// for cubic environment reflection mapping
//	TL_Displacement,	// for hardware tesselation

	TL_Num_Layers
};

typedef TFixedArray< rxTexture*, ETextureLayer::TL_Num_Layers >	rxTextureLayers;

/*
=======================================================================

		Render material

=======================================================================
*/

//
//	rxMaterialDescription - used for creating new graphics materials.
//
class rxMaterialDescription {
public:
	String		name;	// must NOT be empty

	rxTextureLayers	layers;

	ColorRGB	emissive;	// for modulation with diffuse color
	FLOAT		specularIntensity;	// (default = 0.0f)
	FLOAT		shininess;	// specular exponent, must be > 0  (default = 1.0f)

public:
	rxMaterialDescription()
	{
		SetDefaults();
	}

	void SetDefaults()
	{
		name.Empty();

		layers.Zero();

		emissive.Set( 1,1,1 );
		specularIntensity = 0.0f;
		shininess = 1.0f;
	}

	bool IsOk() const
	{
		return ( ! name.IsEmpty() )
			&& (shininess > 0)
			&& 1;
	}
};

//
//	rxMaterial - is a base class for all render materials.
//
//	Materials represent visual properties of drawable objects.
//
class rxMaterial {
public:
	// ...

protected:
	virtual ~rxMaterial() {}
};

}//End of namespace abc

#endif // !__RX_RENDER_MATERIAL_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
