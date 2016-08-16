/*
=============================================================================
	File:	Material.cpp
	Desc:
=============================================================================
*/
#include <precompiled.h>
#pragma hdrstop
#include <Engine.h>

namespace abc {

/*================================
		SColor
================================*/

// 32-bit ARGB color

const SColor	SColor::BLACK	( 255,   0,   0,   0 );
const SColor	SColor::GRAY	( 255, 127, 127, 127 );
const SColor	SColor::WHITE	( 255, 255, 255, 255 );

const SColor	SColor::YELLOW	( 255, 255, 255,   0 );
const SColor	SColor::GREEN	( 255,   0, 255,   0 );
const SColor	SColor::BLUE	( 255,   0,   0, 255 );
const SColor	SColor::RED		( 255, 255,   0,   0 );
const SColor	SColor::CYAN	( 255,   0, 255, 255 );
const SColor	SColor::MAGENTA	( 255, 255,   0, 255 );

/*================================
		FColor
================================*/

// 128-bit RGBA color

const FColor	FColor::BLACK		( 0.0f, 0.0f, 0.0f, 1.0f );
const FColor	FColor::GRAY		( 0.5f, 0.5f, 0.5f, 1.0f );
const FColor	FColor::WHITE		( 1.0f, 1.0f, 1.0f, 1.0f );

const FColor	FColor::YELLOW				( 1.0f, 	1.0f,	0.0f,	1.0f );
const FColor	FColor::BEACH_SAND			( 1.0f, 	0.96f,	0.62f,	1.0f );
const FColor	FColor::GREEN				( 0.0f, 	1.0f,	0.0f,	1.0f );
const FColor	FColor::LIGHT_YELLOW_GREEN	( 0.48f,	0.77f, 	0.46f, 	1.0f );
const FColor	FColor::DARK_YELLOW_GREEN	( 0.1f, 	0.48f, 	0.19f, 	1.0f );
const FColor	FColor::BLUE				( 0.0f, 	0.0f, 	1.0f, 	1.0f );
const FColor	FColor::RED					( 1.0f, 	0.0f, 	0.0f, 	1.0f );
const FColor	FColor::CYAN				( 0.0f, 	1.0f, 	1.0f, 	1.0f );
const FColor	FColor::MAGENTA				( 1.0f, 	0.0f, 	1.0f, 	1.0f );
const FColor	FColor::DARKBROWN			( 0.45f,	0.39f,	0.34f,	1.0f );

//-----------------------------------------------------

}//End of namespace abc

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
