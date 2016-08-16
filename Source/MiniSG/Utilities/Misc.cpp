/*
=============================================================================
	File:	Misc.cpp
	Desc:	Miscellaneous helpers.
=============================================================================
*/

#include <precompiled.h>
#pragma hdrstop
#include <MiniSG.h>

namespace abc {


//---------------------------------------------------------------

FColor GetRandomColor( int seed )
{
	static const FColor the_colors[] =
	{
		FColor::GRAY,
		FColor::WHITE,
		FColor::YELLOW,
		FColor::BEACH_SAND,
		FColor::GREEN,
		FColor::LIGHT_YELLOW_GREEN,
		FColor::DARK_YELLOW_GREEN,
		FColor::BLUE,
		FColor::RED,
		FColor::CYAN,
		FColor::MAGENTA,
		FColor::DARKBROWN
	};

	mxRandom	random( seed );
	return the_colors[ random.RandomInt( ARRAY_SIZE(the_colors) ) ];
}

//---------------------------------------------------------------

}//End of namespace abc

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
