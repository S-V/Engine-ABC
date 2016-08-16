/*
=============================================================================
	File:	Object.cpp
	Desc:	Common base class.
=============================================================================
*/

#include <precompiled.h>
#pragma hdrstop
#include <Base.h>

namespace abc {

/*================================
			mxObject
================================*/

mxTypeInfo	mxObject::Type( "mxObject", 'OBJT', NULL, sizeof(mxObject), null );

mxTypeInfo*	mxObject::GetType() const { return & this->Type; }

//---------------------------------------------------------------------------

mxObject::mxObject()
{
}

mxObject::~mxObject()
{
}

}//end of namespace abc

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
