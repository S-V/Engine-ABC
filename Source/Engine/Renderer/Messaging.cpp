/*
=============================================================================
	File:	Messaging.cpp
	Desc:	Definitions of messages targeted at render system.
=============================================================================
*/
#include <precompiled.h>
#pragma hdrstop
#include <Engine.h>

namespace abc {

/*================================
	rxMessage_OnWindowResized
================================*/

DEFINE_CLASS( rxMessage_OnWindowResized, 'RMWS', mxMessage );

rxMessage_OnWindowResized::rxMessage_OnWindowResized( mxUInt newWidth, mxUInt newHeight )
	: width( newWidth ), height( newHeight )
{}

}//End of namespace abc

//--------------------------------------------------------------//
//				End Of File.									//
//--------------------------------------------------------------//
