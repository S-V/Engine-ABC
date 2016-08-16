/*
=============================================================================
	File:	SpatialProxy.cpp
	Desc:	Spatial proxy can be thought of
			as a spatial database of a single object.
=============================================================================
*/

#include <precompiled.h>
#pragma hdrstop
#include <Engine.h>

namespace abc {

/*================================
	mxSpatialProxy
================================*/

DEFINE_ABSTRACT_CLASS( mxSpatialProxy, 'SPRX', mxEntityComponent );

/*================================
	mxSpatialProxy_Infinite
================================*/

DEFINE_CLASS( mxSpatialProxy_Infinite, 'SPIE', mxSpatialProxy );

/*================================
	mxSpatialProxy_Sphere
================================*/

DEFINE_CLASS( mxSpatialProxy_Sphere, 'SPRS', mxSpatialProxy );

/*================================
	mxSpatialProxy_Box
================================*/

DEFINE_CLASS( mxSpatialProxy_Box, 'SPRB', mxSpatialProxy );

}//End of namespace abc

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
