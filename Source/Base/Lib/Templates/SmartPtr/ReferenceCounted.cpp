/*
=============================================================================
	File:	ReferenceCounted.cpp
	Desc:	A very simple basic class for reference counted objects.
=============================================================================
*/

#include <precompiled.h>
#pragma hdrstop
#include <Base.h>

namespace abc {

#ifdef MX_DEBUG

// debug utils
INT	ReferenceCounted::_totalNumReferences = 0;

#endif

}//End of namespace abc

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
