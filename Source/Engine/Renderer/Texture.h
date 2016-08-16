/*
=============================================================================
	File:	Texture.h
	Desc:	Textures used for rendering.
=============================================================================
*/

#ifndef __RX_TEXTURE_H__
#define __RX_TEXTURE_H__

namespace abc {

//
//	EPixelFormat - enumerates allowed pixel formats.
//
enum EPixelFormat
{
	PF_DEFAULT = 0,		// Default format (taken from file or unknown).
	PF_A8R8G8B8,		// Default - 32 bit. 8 bits per channel.
	PF_A2R10G10B10,		// 32 bit, 2 bit alpha and 10 bit RGB channels.
	PF_R8G8B8,			// 32 bit without alpha - alpha channel inaccessible.
	PF_A1R5G5B5,		// 16 bit - 1 per alpha and 5 per other channels.
	PF_R5G5B5,			// 16 bit - 5 per colour channels. No alpha.
	PF_R5G6B5,			// 16 bit - 5 bits per red and blue and 6 per green.
	PF_G16R16F,			// 32 bit floating point. Only red and green channels.
	PF_A16B16G16R16F,	// 64 bit, 4 channel floating point texture.
	PF_R16F,			// 16 bit, 1 channel floating point texture.
	PF_G32R32F,			// 64 bit, 2 channel floating point texture.
	PF_R32F,			// 32 bit, 1 channel floating point texture.
	PF_A32B32G32R32F,	// 128 bit, 4 channel floating point texture.
	PF_A8,				// 8 bit, 1-channel texture.

	//- Compressed formats. ------------------------------------------------------

	PF_DXT1,
	PF_DXT3,
	PF_DXT5,
};

//
//	ETextureType
//
enum ETextureType
{
	TT_UNKNOWN = 0,
	TT_1D,			// 1D Texture - array-like.
	TT_2D,			// 2D Texture - you will use it in most cases.
	TT_3D,			// 3D Texture - memory usage expensive.
	TT_CUBE			// Cube texture.
};

//
//	ETextureUsage
//
enum ETextureUsage
{
	TU_NORMAL = 0,		// Regular texture (you cannot write to it).
	TU_DYNAMIC,			// Texture is dynamic, you can write to it.
	TU_RENDERTARGET,	// Texture will be used as a render target.
};

//
//	rxTextureDescription
//
class rxTextureDescription {
public:
	EPixelFormat	Format;
	ETextureType	Type;
	ETextureUsage	Usage;

	// Set these values to 0 to let them be determined automatically.
	mxUInt	Width;
	mxUInt	Height;
	mxUInt	Depth;	// in pixels, only valid for 3D volume textures

public:
	rxTextureDescription();

	void	SetDefaults();
	bool	IsValid() const;
};

//
//	rxTexture - is an actual texture resource.
//
class rxTexture {
public:

	// Converts a height map into a normal map.
	// Param: amplitude - constant value by which the height values in the normal map are multiplied.
	// Returns: the pointer to this texture.
	virtual rxTexture* MakeNormalMap( FLOAT amplitude = 1.0f ) = 0;

	// More stuff to be added soon...

protected:
	virtual ~rxTexture() {}
};

}//End of namespace abc

#endif // !__RX_TEXTURE_H__

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
