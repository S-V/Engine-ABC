/*
=============================================================================
	File:	Texture.cpp
	Desc:	Textures used for rendering.
=============================================================================
*/
#include <precompiled.h>
#pragma hdrstop
#include <Engine.h>

namespace abc {

/*================================
		rxTextureDescription
================================*/

rxTextureDescription::rxTextureDescription()
{
	SetDefaults();
}

void rxTextureDescription::SetDefaults()
{
	Format	=  EPixelFormat::PF_A8R8G8B8;
	Type	=  ETextureType::TT_2D;
	Usage	=  ETextureUsage::TU_NORMAL;

	Width	=  0;
	Height	=  0;
	Depth	=  0;
}

bool rxTextureDescription::IsValid() const
{
	return true;
}

}//End of namespace abc

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
